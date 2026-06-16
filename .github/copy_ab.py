# Python version of SmartCopy, with a small kink:
# it doen not replace versioned file, instead just
# checks equality.

import sys
import shutil
import os

print('Python SmartCopy!')

params = sys.argv[1:]
if len(params) < 1:
    print('Usage: copy_ab.py dest_dir')
    sys.exit(1)
    
filesToCopy = [ 'UcAuto.cpp' ]

filesToCheck = [ 'UcAutoLib.cpp', 'UcAutoCount.h',
    'UcAutoScripts.h', 'UcAutoOldComp.cpp', 'UcAutoSutton.cpp' ]

destDir = sys.argv[1]
for x in filesToCopy:
    shutil.copy(x, destDir)
    print('Copied {x} to dir {destDir}')

for x in filesToCheck:
    with open(x, 'rb') as file:
        srcData = file.read()    
    destPath = os.path.join(sys.argv[1], x)
    with open(destPath, 'rb') as file:
        destData = file.read()
    if srcData != destData:
        print('Files {x} and {destPath} are inequal!')
        sys.exit(1)
    print('Files {x} and {destPath} are equal')
