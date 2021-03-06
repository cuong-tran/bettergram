/*
This file is part of Bettergram.

For license and copyright information please follow this link:
https://github.com/bettergram/bettergram/blob/master/LEGAL
*/
#include "media/media_clip_qtgif.h"

namespace Media {
namespace Clip {
namespace internal {

QtGifReaderImplementation::QtGifReaderImplementation(FileLocation *location, QByteArray *data) : ReaderImplementation(location, data) {
}

ReaderImplementation::ReadResult QtGifReaderImplementation::readFramesTill(TimeMs frameMs, TimeMs systemMs) {
	if (!_frame.isNull() && _frameTime > frameMs) {
		return ReadResult::Success;
	}
	auto readResult = readNextFrame();
	if (readResult != ReadResult::Success || _frameTime > frameMs) {
		return readResult;
	}
	readResult = readNextFrame();
	if (_frameTime <= frameMs) {
		_frameTime = frameMs + 5; // keep up
	}
	return readResult;
}

TimeMs QtGifReaderImplementation::frameRealTime() const {
	return _frameRealTime;
}

TimeMs QtGifReaderImplementation::framePresentationTime() const {
	return qMax(_frameTime, 0LL);
}

ReaderImplementation::ReadResult QtGifReaderImplementation::readNextFrame() {
	if (_reader) _frameDelay = _reader->nextImageDelay();
	if (_framesLeft < 1) {
		if (_mode == Mode::Normal) {
			return ReadResult::EndOfFile;
		} else if (!jumpToStart()) {
			return ReadResult::Error;
		}
	}

	_frame = QImage(); // QGifHandler always reads first to internal QImage and returns it
	if (!_reader->read(&_frame) || _frame.isNull()) {
		return ReadResult::Error;
	}
	--_framesLeft;
	_frameTime += _frameDelay;
	_frameRealTime += _frameDelay;
	return ReadResult::Success;
}

bool QtGifReaderImplementation::renderFrame(QImage &to, bool &hasAlpha, const QSize &size) {
	Assert(!_frame.isNull());
	if (size.isEmpty() || size == _frame.size()) {
		int32 w = _frame.width(), h = _frame.height();
		if (to.width() == w && to.height() == h && to.format() == _frame.format()) {
			if (to.byteCount() != _frame.byteCount()) {
				int bpl = qMin(to.bytesPerLine(), _frame.bytesPerLine());
				for (int i = 0; i < h; ++i) {
					memcpy(to.scanLine(i), _frame.constScanLine(i), bpl);
				}
			} else {
				memcpy(to.bits(), _frame.constBits(), _frame.byteCount());
			}
		} else {
			to = _frame.copy();
		}
	} else {
		to = _frame.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}
	hasAlpha = _frame.hasAlphaChannel();
	_frame = QImage();
	return true;
}

TimeMs QtGifReaderImplementation::durationMs() const {
	return 0; // not supported
}

bool QtGifReaderImplementation::start(Mode mode, TimeMs &positionMs) {
	if (mode == Mode::Inspecting) {
		return false;
	}
	_mode = mode;
	return jumpToStart();
}

QtGifReaderImplementation::~QtGifReaderImplementation() = default;

bool QtGifReaderImplementation::jumpToStart() {
	if (_reader && _reader->jumpToImage(0)) {
		_framesLeft = _reader->imageCount();
		return true;
	}

	_reader = nullptr;
	initDevice();
	_reader = std::make_unique<QImageReader>(_device);
#ifndef OS_MAC_OLD
	_reader->setAutoTransform(true);
#endif // OS_MAC_OLD
	if (!_reader->canRead() || !_reader->supportsAnimation()) {
		return false;
	}
	_framesLeft = _reader->imageCount();
	if (_framesLeft < 1) {
		return false;
	}
	return true;
}

} // namespace internal
} // namespace Clip
} // namespace Media
