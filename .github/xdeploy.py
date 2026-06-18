def simpleFile(v : string, dest : string, isRelease : bool) : bool
    v = v.replace('//', '/')
    v = v.replace('/|', '|')
    v = v.replace('\\/', '/')
    v = v.replace('\\|', '|')
    isReleaseOnly = False
    # Some files are release-only
    if v.endswith('.dll'):
        isReleaseOnly = True
    # Get destination path`
    whereBar = v.find('|')
    if whereBar < 0:
        print(f'Cannot find a bar in "{v}"')
        sys.exit(1)
    subdirFile = v[(whereBar + 1):]
    # Extract destination subdirs
    additionalSubpath = ''
    destDir = DIR_DEPLOY
    pSlash = subdirFile.rfind('/')
    if pSlash >= 0:
        additionalSubpath = subdirFile[:pSlash]
        destDir = destDir + '/' + additionalSubpath
    # Get src name
    srcName = v.replace('|', '/')
    # Copy or register errors
    if os.path.isfile(srcName):
        # Could reverse and elif, but this branch is good,
        # and (actual) else is bad
        if isRelease or (not isReleaseOnly):
            if additionalSubpath != '':
                os.makedirs(destDir, exist_ok=True)
            shutil.copy(srcName, destDir)
            print(f'Copied {srcName} --> {destDir}.')
        else:
            print(f'Skipped {srcName} --> {destDir}, not a release.')
        return True
    else:
        hasBadFiles = True
        print(f'File {srcName} is missing, skipping.')
        return False
