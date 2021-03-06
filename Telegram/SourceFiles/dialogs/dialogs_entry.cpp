/*
This file is part of Bettergram.

For license and copyright information please follow this link:
https://github.com/bettergram/bettergram/blob/master/LEGAL
*/
#include "dialogs/dialogs_entry.h"

#include "dialogs/dialogs_key.h"
#include "dialogs/dialogs_indexed_list.h"
#include "mainwidget.h"
#include "auth_session.h"
#include "styles/style_dialogs.h"
#include "history/history_item.h"
#include "history/history.h"
#include "base/flags.h"
#include "auth_session.h"
#include "data/data_session.h"
#include "bettergram/bettergramservice.h"

#include <QSettings>

namespace Dialogs {
namespace {

auto DialogsPosToTopShift = 0;

uint64 DialogPosFromDate(TimeId date) {
	if (!date) {
		return 0;
	}
	return (uint64(date) << 32) | (++DialogsPosToTopShift);
}

uint64 ProxyPromotedDialogPos() {
	return 0xFFFFFFFFFFFF0001ULL;
}

uint64 PinnedDialogPos(int pinnedIndex) {
	return 0xFFFFFFFF00000000ULL + pinnedIndex;
}

} // namespace

Entry::Entry(const Key &key, uint64 id)
: lastItemTextCache(st::dialogsTextWidthMin)
, _key(key) {
	loadIsFavorite(id);
	loadPinnedIndex(id);
}

void Entry::cachePinnedIndex(int index) {
	if (_pinnedIndex != index) {
		const auto wasPinned = isPinnedDialog();
		_pinnedIndex = index;
		updateChatListSortPosition();
		updateChatListEntry();
		if (wasPinned != isPinnedDialog()) {
			changedChatListPinHook();
		}

		QSettings settings(Bettergram::BettergramService::instance()->bettergramSettingsPath(), QSettings::IniFormat);
		settings.beginGroup(Auth().user()->phone());
		settings.beginGroup("pinned");
		settings.setValue(QString::number(_key.id()), _pinnedIndex);
		settings.endGroup();
		settings.endGroup();
	}
}

void Entry::cacheProxyPromoted(bool promoted) {
	if (_isProxyPromoted != promoted) {
		_isProxyPromoted = promoted;
		updateChatListSortPosition();
		updateChatListEntry();
		if (!_isProxyPromoted) {
			updateChatListExistence();
		}
	}
}

bool Entry::needUpdateInChatList() const {
	return inChatList(Dialogs::Mode::All) || shouldBeInChatList();
}

void Entry::updateChatListSortPosition() {
	if (Auth().supportMode()
		&& _sortKeyInChatList != 0
		&& Auth().settings().supportFixChatsOrder()) {
		return;
	}
	_sortKeyInChatList = useProxyPromotion()
		? ProxyPromotedDialogPos()
		: isPinnedDialog()
		? PinnedDialogPos(_pinnedIndex)
		: DialogPosFromDate(adjustChatListTimeId());
	if (needUpdateInChatList()) {
		setChatListExistence(true);
	}
}

void Entry::updateChatListExistence() {
	setChatListExistence(shouldBeInChatList());
}

void Entry::setChatListExistence(bool exists) {
	if (const auto main = App::main()) {
		if (exists && _sortKeyInChatList) {
			main->createDialog(_key);
			updateChatListEntry();
		} else {
			main->removeDialog(_key);
		}
	}
}

TimeId Entry::adjustChatListTimeId() const {
	return chatsListTimeId();
}

void Entry::changedInChatListHook(Dialogs::Mode list, bool added) {
}

void Entry::changedChatListPinHook() {
}

RowsByLetter &Entry::chatListLinks(Mode list) {
	return _chatListLinks[static_cast<int>(list)];
}

const RowsByLetter &Entry::chatListLinks(Mode list) const {
	return _chatListLinks[static_cast<int>(list)];
}

Row *Entry::mainChatListLink(Mode list) const {
	auto it = chatListLinks(list).find(0);
	Assert(it != chatListLinks(list).cend());
	return it->second;
}

Row *Entry::rowInCurrentTab() const {
	return _rowInCurrentTab;
}

void Entry::setRowInCurrentTab(Row *row) {
	_rowInCurrentTab = row;
}

PositionChange Entry::adjustByPosInChatList(
		Mode list,
		not_null<IndexedList*> indexed) {
	const auto movedFrom = posInChatList(list);
	indexed->adjustByPos(chatListLinks(list));
	const auto movedTo = posInChatList(list);
	return { movedFrom, movedTo };
}

void Entry::setChatsListTimeId(TimeId date) {
	if (_lastMessageTimeId && _lastMessageTimeId >= date) {
		if (!inChatList(Dialogs::Mode::All)) {
			return;
		}
	}
	_lastMessageTimeId = date;
	updateChatListSortPosition();
}

int Entry::posInChatList(Dialogs::Mode list) const {
	if (list == Mode::All) {
		return _rowInCurrentTab ? _rowInCurrentTab->pos() : mainChatListLink(list)->pos();
	}
	return mainChatListLink(list)->pos();
}

not_null<Row*> Entry::addToChatList(
		Mode list,
		not_null<IndexedList*> indexed) {
	if (!inChatList(list)) {
		chatListLinks(list) = indexed->addToEnd(_key);
		changedInChatListHook(list, true);
	}
	return mainChatListLink(list);
}

void Entry::removeFromChatList(
		Dialogs::Mode list,
		not_null<Dialogs::IndexedList*> indexed) {
	if (inChatList(list)) {
		indexed->del(_key);
		chatListLinks(list).clear();
		changedInChatListHook(list, false);
	}
}

void Entry::removeChatListEntryByLetter(Mode list, QChar letter) {
	Expects(letter != 0);

	if (inChatList(list)) {
		chatListLinks(list).remove(letter);
	}
}

void Entry::addChatListEntryByLetter(
		Mode list,
		QChar letter,
		not_null<Row*> row) {
	Expects(letter != 0);

	if (inChatList(list)) {
		chatListLinks(list).emplace(letter, row);
	}
}

void Entry::updateChatListEntry() const {
	if (const auto main = App::main()) {
		if (inChatList(Mode::All)) {
			main->repaintDialogRow(
				Mode::All,
				_rowInCurrentTab ? _rowInCurrentTab : mainChatListLink(Mode::All));
			if (inChatList(Mode::Important)) {
				main->repaintDialogRow(
					Mode::Important,
					mainChatListLink(Mode::Important));
			}
		}
	}
}

void Entry::updateChatListEntry(Row *row) const
{
	if (const auto main = App::main()) {
		if (inChatList(Mode::All)) {
			main->repaintDialogRow(
				Mode::All,
				row);
			if (inChatList(Mode::Important)) {
				main->repaintDialogRow(
					Mode::Important,
					row);
			}
		}
	}
}

void Entry::loadIsFavorite(uint64 id) {
	Bettergram::BettergramService::instance()->portSettingsFiles();

	QString keyString = QString::number(id);

	QSettings settings(Bettergram::BettergramService::instance()->bettergramSettingsPath(), QSettings::IniFormat);
	settings.beginGroup(Auth().user()->phone());
	settings.beginGroup("favorites");

	if (settings.contains(keyString)) {
		_isFavorite = settings.value(keyString).toBool();
	} else {
		_isFavorite = false;
	}

	settings.endGroup();
	settings.endGroup();
}

void Entry::loadPinnedIndex(uint64 id)
{
	Bettergram::BettergramService::instance()->portSettingsFiles();
	QSettings settings(Bettergram::BettergramService::instance()->bettergramSettingsPath(), QSettings::IniFormat);
	settings.beginGroup(Auth().user()->phone());
	settings.beginGroup("pinned");
	_pinnedIndex = settings.value(QString::number(id)).toInt();
	settings.endGroup();
	settings.endGroup();

	if (_pinnedIndex > 0) {
		Auth().data().insertPinnedDialog(_key, _pinnedIndex);
	}
}

void Entry::setIsFavoriteDialog(bool isFavorite) {
	if (_isFavorite != isFavorite) {
		_isFavorite = isFavorite;
		QString keyString = QString::number(_key.id());

		QSettings settings(Bettergram::BettergramService::instance()->bettergramSettingsPath(), QSettings::IniFormat);
		settings.beginGroup(Auth().user()->phone());
		settings.beginGroup("favorites");

		if (_isFavorite) {
			settings.setValue(keyString, _isFavorite);
		} else {
			settings.remove(keyString);
		}

		settings.endGroup();
		settings.endGroup();
	}
}

void Entry::toggleIsFavoriteDialog() {
	setIsFavoriteDialog(!_isFavorite);
}

} // namespace Dialogs
