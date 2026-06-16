import os
import sys
import shutil

print('Deploy Unicodia on Windows')

params = sys.argv[1:]
if len(params) < 1:
    print('Usage: deploy_win.py qtdir')
    exit(1)

QT_DIR = sys.argv[1]
print(f'The Qt dir is {QT_DIR}.')

FILES = [
    'build-UC|Unicodia.exe',
    QT_DIR + '/bin|Qt6Core.dll',
]
DIR_DEPLOY = 'deploy-UC'
os.mkdir(DIR_DEPLOY)

for v in FILES:
    v = v.replace('//', '/')
    v = v.replace('/|', '|')
    whereBar = v.find('|')
    if whereBar < 0:
        print(f'Cannot find a bar in "{1}"')
        sys.exit(1)
    subdirFile = v[(whereBar + 1):]
    # @todo [urgent] no paths
    paths = subdirFile.split('/')
    if len(paths > 1):
        print(f'Additional subpaths unsupported!')
        sys.exit(1)
    destDir = DIR_DEPLOY
    srcName = v.replace('|', '/')
    shutil.copy(srcName, DIR_DEPLOY)
    print(f'Copied {srcName} --> {DIR_DEPLOY}.')
