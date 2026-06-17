import os
import sys
import shutil

print('Deploy Unicodia on Windows')

params = sys.argv[1:]
if len(params) < 2:
    print('Usage: deploy_win.py srcdir qtdir [--rel]')
    exit(1)

SRC_DIR = sys.argv[1]
QT_DIR = sys.argv[2]
print(f'The Qt dir is {QT_DIR}.')
print(f'The source dir is {SRC_DIR}.')

isRelease = False
for v in params:
    if v == '--rel':
        isRelease = True
print(f'Release: {isRelease}')

print('Copying files')
FILES = [
    'build-UC|Unicodia.exe',
    SRC_DIR + '/MiscFiles|Unicodia.xml',
    SRC_DIR + '|LICENSE',
    SRC_DIR + '/MiscFiles/WinManifest|Resources.pri',
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
    QT_DIR + '/plugins|imageformats/qsvg.dll',
    QT_DIR + '/plugins|platforms/qwindows.dll',
    QT_DIR + '/plugins|styles/qwindowsvistastyle.dll',
    # @todo [urgent] MinGW files
]
DIR_DEPLOY = 'deploy-UC'
os.mkdir(DIR_DEPLOY)

hasBadFiles = False
for v in FILES:
    v = v.replace('//', '/')
    v = v.replace('/|', '|')
    v = v.replace('\\/', '/')
    v = v.replace('\\|', '|')
    isReleaseOnly = False
    # Some files are release-only
    if v.endswith('.dll'):
        isReleaseOnly = True
    # Split into subdirs
    whereBar = v.find('|')
    if whereBar < 0:
        print(f'Cannot find a bar in "{1}"')
        sys.exit(1)
    subdirFile = v[(whereBar + 1):]
    # @todo [urgent] no paths
    additionalSubpath = ''
    pSlash = subdirFile.rfind('/')
    if pShash >= 0:
        additionalSubpath = subdirFile[:pShash]
        continue
    destDir = DIR_DEPLOY
    if additionalSubpath != '':
        destDir = os.path.join(destDir, additionalSubpath)
    srcName = v.replace('|', '/')
    if os.path.isfile(srcName):
        # Could reverse and elif, but this branch is good,
        # and (actual) else is bad
        if isRelease or (not isReleaseOnly):
            if additionalSubpath != '':
                os.mkdir(destDir)
            shutil.copy(srcName, destDir)
            print(f'Copied {srcName} --> {destDir}.')
        else:
            print(f'Skipped {srcName} --> {destDir}, not a release.')
    else:
        hasBadFiles = True
        print(f'File {srcName} is missing, skipping.')

if hasBadFiles:
    print('Have bad files, exiting')
    sys.exit(1)
