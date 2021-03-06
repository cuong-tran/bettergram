# This file is part of Bettergram.
#
# For license and copyright information please follow this link:
# https://github.com/bettergram/bettergram/blob/master/LEGAL

{
  'conditions': [[ 'build_mac', {
    'xcode_settings': {
      'INFOPLIST_FILE': '../Telegram.plist',
      'CURRENT_PROJECT_VERSION': '<!(./print_version.sh)',
      'ASSETCATALOG_COMPILER_APPICON_NAME': 'AppIcon',
      'OTHER_LDFLAGS': [
        '-lbsm',
        '-lm',
        '/usr/local/lib/liblzma.a',
      ],
    },
    'include_dirs': [
      '/usr/local/include',
    ],
    'library_dirs': [
      '/usr/local/lib',
    ],
    'configurations': {
      'Debug': {
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
        },
      },
      'Release': {
        'xcode_settings': {
          'DEBUG_INFORMATION_FORMAT': 'dwarf-with-dsym',
          'LLVM_LTO': 'YES',
          'GCC_OPTIMIZATION_LEVEL': 'fast',
        },
      },
    },
  }], [ 'build_macold', {
    'defines': [
      'OS_MAC_OLD',
    ],
    'xcode_settings': {
      'OTHER_CPLUSPLUSFLAGS': [ '-nostdinc++' ],
      'OTHER_LDFLAGS': [
        '-isysroot', '/',
        '-lbase',
        '-lcrashpad_client',
        '-lcrashpad_util',
        '/usr/local/macold/lib/libz.a',
        '/usr/local/macold/lib/libopus.a',
        '/usr/local/macold/lib/libopenal.a',
        '/usr/local/macold/lib/libiconv.a',
        '/usr/local/macold/lib/libavcodec.a',
        '/usr/local/macold/lib/libavformat.a',
        '/usr/local/macold/lib/libavutil.a',
        '/usr/local/macold/lib/libswscale.a',
        '/usr/local/macold/lib/libswresample.a',
        '/usr/local/macold/lib/libexif.a',
        '/usr/local/macold/lib/libc++.a',
        '/usr/local/macold/lib/libc++abi.a',
      ],
    },
    'include_dirs': [
      '/usr/local/macold',
      '/usr/local/macold/include/c++/v1',
      '<(libs_loc)/macold/libexif-0.6.20',
      '<(libs_loc)/macold/crashpad',
      '<(libs_loc)/macold/crashpad/third_party/mini_chromium/mini_chromium',
    ],
    'configurations': {
      'Debug': {
        'xcode_settings': {
          'PRODUCT_BUNDLE_IDENTIFIER': 'io.bettergram.BettergramDebugOld',
        },
        'library_dirs': [
          '<(libs_loc)/macold/crashpad/out/Debug',
        ],
      },
      'Release': {
        'xcode_settings': {
          'PRODUCT_BUNDLE_IDENTIFIER': 'io.bettergram.Bettergram',
        },
        'library_dirs': [
          '<(libs_loc)/macold/crashpad/out/Release',
        ],
      },
    },
    'postbuilds': [{
      'postbuild_name': 'Force Frameworks path',
      'action': [
        'mkdir', '-p', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks/'
      ],
    }, {
      'postbuild_name': 'Copy Updater to Frameworks',
      'action': [
        'cp',
        '${BUILT_PRODUCTS_DIR}/Updater',
        '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks/',
      ],
    }, {
      'postbuild_name': 'Force Helpers path',
      'action': [
        'mkdir', '-p', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Helpers/'
      ],
    }, {
      'postbuild_name': 'Copy crashpad_handler to Helpers',
      'action': [
        'cp',
        '<(libs_loc)/macold/crashpad/out/${CONFIGURATION}/crashpad_handler',
        '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Helpers/',
      ],
    }],
  }, {
    'xcode_settings': {
      'OTHER_LDFLAGS': [
        '/usr/local/lib/libz.a',
        '/usr/local/lib/libopus.a',
        '/usr/local/lib/libopenal.a',
        '/usr/local/lib/libiconv.a',
        '/usr/local/lib/libavcodec.a',
        '/usr/local/lib/libavformat.a',
        '/usr/local/lib/libavutil.a',
        '/usr/local/lib/libswscale.a',
        '/usr/local/lib/libswresample.a',
      ],
    },
    'include_dirs': [
      '<(libs_loc)/crashpad',
      '<(libs_loc)/crashpad/third_party/mini_chromium/mini_chromium',
    ],
    'configurations': {
      'Debug': {
        'library_dirs': [
          '<(libs_loc)/crashpad/out/Debug',
        ],
      },
      'Release': {
        'library_dirs': [
          '<(libs_loc)/crashpad/out/Release',
        ],
      },
    },
  }], [ '"<(build_macold)" != "1" and "<(build_macstore)" != "1"', {
    'xcode_settings': {
      'OTHER_LDFLAGS': [
        '-lbase',
        '-lcrashpad_client',
        '-lcrashpad_util',
      ],
     },
    'configurations': {
      'Debug': {
        'xcode_settings': {
          'PRODUCT_BUNDLE_IDENTIFIER': 'io.bettergram.BettergramDebug',
        },
      },
      'Release': {
        'xcode_settings': {
          'PRODUCT_BUNDLE_IDENTIFIER': 'io.bettergram.Bettergram',
        },
      },
    },
    'postbuilds': [{
      'postbuild_name': 'Force Frameworks path',
      'action': [
        'mkdir', '-p', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks/'
      ],
    }, {
      'postbuild_name': 'Copy Updater to Frameworks',
      'action': [
        'cp',
        '${BUILT_PRODUCTS_DIR}/Updater',
        '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks/',
      ],
    }, {
      'postbuild_name': 'Force Helpers path',
      'action': [
        'mkdir', '-p', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Helpers/'
      ],
    }, {
      'postbuild_name': 'Copy crashpad_client to Helpers',
      'action': [
        'cp',
        '<(libs_loc)/crashpad/out/${CONFIGURATION}/crashpad_handler',
        '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Helpers/',
      ],
    }]
  }], [ 'build_macdmg', {
    'postbuilds': [{
      'postbuild_name': 'Sign .app',
      'action': [
        'codesign',
        '-v',
        '--force',
        '--deep',
        '-s',
        '<!(cat ../../../TelegramPrivate/mac_certificate_identity)',
        '${BUILT_PRODUCTS_DIR}/Bettergram.app'
      ],
    }, {
      'postbuild_name': 'Copy BettergramDmg.json',
      'action': [
        'cp',
        '../../../TelegramPrivate/BettergramDmg.json',
        '${BUILT_PRODUCTS_DIR}/BettergramDmg.json',
      ],
    }, {
      'postbuild_name': 'Copy Bettergram.icns',
      'action': [
        'cp',
        '../../../TelegramPrivate/Bettergram.icns',
        '${BUILT_PRODUCTS_DIR}/Bettergram.icns',
      ],
    }, {
      'postbuild_name': 'Remove .dmg',
      'action': [
        'rm',
        '-rf',
        '${BUILT_PRODUCTS_DIR}/Bettergram.dmg'
      ],
    }, {
      'postbuild_name': 'Create .dmg',
      'action': [
        './create_dmg.sh',
        '${BUILT_PRODUCTS_DIR}/BettergramDmg.json',
        '${BUILT_PRODUCTS_DIR}/Bettergram.dmg'
      ],
    }, {
      'postbuild_name': 'Sign .dmg',
      'action': [
        'codesign',
        '-v',
        '--force',
        '--deep',
        '-s',
        '<!(cat ../../../TelegramPrivate/mac_certificate_identity)',
        '${BUILT_PRODUCTS_DIR}/Bettergram.dmg'
      ],
    }],
  }], [ 'build_macstore', {
    'xcode_settings': {
      'PRODUCT_BUNDLE_IDENTIFIER': 'io.bettergram.Bettergram',
      'OTHER_LDFLAGS': [
      ],
      'FRAMEWORK_SEARCH_PATHS': [
      ],
    },
    'mac_sandbox': 1,
    'mac_sandbox_development_team': '<!(cat ../../../TelegramPrivate/mac_development_team)',
    'product_name': 'Bettergram',
    'sources': [
      '../Telegram/Telegram Desktop.entitlements',
    ],
    'defines': [
      'TDESKTOP_DISABLE_AUTOUPDATE',
      'OS_MAC_STORE',
    ],
    'postbuilds': [{
      'postbuild_name': 'Clear Frameworks path',
      'action': [
        'rm', '-rf', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks'
      ],
    }, {
      'postbuild_name': 'Force Frameworks path',
      'action': [
        'mkdir', '-p', '${BUILT_PRODUCTS_DIR}/Bettergram.app/Contents/Frameworks/'
      ],
    }]
  }]],
}
