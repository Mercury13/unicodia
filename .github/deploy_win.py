import os
import sys
import shutil

print('Deploy Unicodia on Windows')

params = sys.argv[1:]
if len(params) < 1:
    print('Usage: deploy_win.py qtdir [--rel]')
    exit(1)

QT_DIR = sys.argv[1]
print(f'The Qt dir is {QT_DIR}.')

isRelease = False
for v in params:
    if v == '--rel':
        isRelease = True
print(f'Release: {isRelease}')

FILES = [
    'build-UC|Unicodia.exe',
    QT_DIR + '/bin|Qt6Core.dll',
    QT_DIR + '/bin|Qt6Widgets.dll',
    QT_DIR + '/bin|Qt6Widgets1.dll',
    QT_DIR + '/bin|Qt6Svg.dll',
    QT_DIR + '/bin|Qt6SvgWidgets.dll',
]
DIR_DEPLOY = 'deploy-UC'
os.mkdir(DIR_DEPLOY)

hasBadFiles = false
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
    paths = subdirFile.split('/')
    if len(paths) > 1:
        print(f'Additional subpaths unsupported!')
        sys.exit(1)
    destDir = DIR_DEPLOY
    srcName = v.replace('|', '/')
    if os.path.isfile(srcName):
        # Could reverse and elif, but this branch is good,
        # and (actual) else is bad
        if isRelease or (not isReleaseOnly):
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
