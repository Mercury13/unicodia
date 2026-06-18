import os
import sys

import xdeploy

print('Deploy Unicodia on Windows')

params = sys.argv[1:]
if len(params) < 3:
    print('Usage: deploy_win.py srcdir msysdir qtdir [--rel]')
    exit(1)

SRC_DIR = sys.argv[1]
MSYS_DIR = sys.argv[2] + '/mingw64/bin'
QT_DIR = sys.argv[3]
print(f'The source dir is {SRC_DIR}.')
print(f'The MSYS dir is {MSYS_DIR}.')
print(f'The Qt dir is {QT_DIR}.')

isRelease = False
for v in params:
    if v == '--rel':
        isRelease = True
print(f'Release: {isRelease}')

print('Copying files START')
FILES = [
    'build-UC|Unicodia.exe',
    SRC_DIR + '/MiscFiles|Unicodia.xml',
    SRC_DIR + '|LICENSE',
    SRC_DIR + '/MiscFiles/WinManifest|Resources.pri',
    SRC_DIR + '/MiscFiles/WinManifest|Unicodia.VisualElementsManifest.xml',
    SRC_DIR + '/MiscFiles/WinManifest/Root|Start/en-US/Start_70.scale-80.png',
    SRC_DIR + '/MiscFiles/WinManifest/Root|Start/en-US/Start_70.scale-100.png',
    SRC_DIR + '/MiscFiles/WinManifest/Root|Start/en-US/Start_70.scale-140.png',
    SRC_DIR + '/MiscFiles/WinManifest/Root|Start/en-US/Start_70.scale-180.png',
    SRC_DIR + '/MiscFiles/WinManifest/Root|Start/en-US/Start_150.scale-180.png',
    QT_DIR + '/bin|Qt6Core.dll',
    QT_DIR + '/bin|Qt6Gui.dll',
    QT_DIR + '/bin|Qt6Widgets.dll',
    QT_DIR + '/bin|Qt6Svg.dll',
    QT_DIR + '/bin|Qt6SvgWidgets.dll',
    QT_DIR + '/plugins|iconengines/qsvgicon.dll',
    QT_DIR + '/plugins|platforms/qwindows.dll',
    QT_DIR + '/plugins|styles/qwindowsvistastyle.dll',
    # MinGW
    MSYS_DIR + '|libgcc_s_seh-1.dll',
    MSYS_DIR + '|libstdc++-6.dll',
    MSYS_DIR + '|libwinpthread-1.dll',
    # MbedTLS
    MSYS_DIR + '|libmbedcrypto-16.dll',
    MSYS_DIR + '|libmbedtls-21.dll',
    MSYS_DIR + '|libmbedx509-7.dll',
]
DIR_DEPLOY = 'deploy-UC'
os.mkdir(DIR_DEPLOY)

hasBadFiles = False
for v in FILES:
    result = xdeploy.simpleFile(v, DIR_DEPLOY, isRelease)
    hasBadFiles = hasBadFiles or result

if hasBadFiles:
    print('Have bad files, exiting')
    sys.exit(1)

print('Copying files OK')
