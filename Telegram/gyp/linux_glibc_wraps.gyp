# This file is part of Bettergram.
#
# For license and copyright information please follow this link:
# https://github.com/bettergram/bettergram/blob/master/LEGAL

{
  'includes': [
    'common.gypi',
  ],
  'targets': [{
    'target_name': 'linux_glibc_wraps',
    'type': 'static_library',
    'sources': [
      '../SourceFiles/platform/linux/linux_glibc_wraps.c',
    ],
    'conditions': [[ '"<!(uname -m)" == "x86_64" or "<!(uname -m)" == "aarch64"', {
      'sources': [
        '../SourceFiles/platform/linux/linux_glibc_wraps_64.c',
      ],
    }, {
      'sources': [
        '../SourceFiles/platform/linux/linux_glibc_wraps_32.c',
      ],
    }]],
  }],
}
