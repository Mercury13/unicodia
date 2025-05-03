import fontforge
import psMat
import os
import hashlib
import base64

TEMPFILENAME = '~UnicodiaHan1.ttf'
OUTFILENAME = 'UnicodiaHan.ttf'
HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'
NODE = 'd:/Soft/NodeJs/node.exe'
RUN = '../KageForNodeJs/run.js'

# get transformation matrix
mat1 = psMat.translate(0, -600)
mat2 = psMat.scale(5.3, 5.4)
mat3 = psMat.translate(-30, -150)
mat = psMat.compose(mat1, mat2)
mat = psMat.compose(mat, mat3)

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())

# load tested versions

# import ideographs
file = open('hani-tasks.txt', 'r')
baseDefs = ''
CACHE_DIR = '~cache'
CACHE_PATH = CACHE_DIR + '/'
NODELINE = '{} {} !{}{} >{}'

# Make cache directory
isDir = os.path.isdir(CACHE_DIR)
if not isDir:
   os.mkdir(CACHE_DIR)

errFile = open(CACHE_PATH + '~err.log', 'w')
errFile.write('Errors START\n')

nShownErrors = 0
nSuppressedErrors = 0
glyphToDataId = {}

for line0 in file:
    line = line0.strip()
    if (line == ''):
        continue
    cmd = line[0]
    if (cmd == '#'):
        # COMMAND # comment
        continue
    if (cmd == '='):
        # COMMAND = assign
        thing = line[1:].strip()
        if (thing == ''):
            raise Exception('Command = (assign) must have some data')
        baseDefs = baseDefs + ' ' + thing
    elif (cmd == 'G'):
        # COMMAND G go
        thing = line[1:].strip()
        params = thing.split()
        if len(params) != 2:
            raise Exception('Command G (go) must have 2 params')
        sUcode = params[0]
        ucode = int(sUcode, base=16)
        entryPoint = params[1]

        baHash = hashlib.sha224(baseDefs.encode()).digest()
        sHash = base64.b64encode(baHash, b'+-').decode()
        sHash = sHash[:12]   # 12 enough!

        dataId = sUcode.upper() + '_' + sHash
        cachedSvg = CACHE_PATH + dataId + '.svg'
        if not os.path.exists(cachedSvg):
            cmdline = f'{NODE} {RUN} !{entryPoint}{baseDefs} >{cachedSvg}'
            debugFile = open(CACHE_PATH + '~hani.log', 'w')
            debugFile.write(cmdline)
            debugFile.write('\n')
            debugFile.close()
            os.system(cmdline)

            fileSize = os.path.getsize(cachedSvg)
            if (fileSize < 200):
                raise Exception('Probably made an empty file');

        glyph = font.createChar(ucode)
        gname = 'u' + sUcode.upper()
        glyph.glyphname = gname
        glyph.importOutlines(cachedSvg, scale=False, simplify=False)

        # Work
        fg = glyph.layers[1]
        for contour in fg:
            if (not contour.closed):
                raise Exception('Found open contour');
            isClock = contour.isClockwise()
            if (isClock == -1):
                contour.round(10)
            elif not isClock:
                raise Exception('Found non-clockwise contour');
        fg.removeOverlap()
        fg.transform(mat)
        # Round to help cubic simplification that’ll destroy extraneous lines’ midpoints
        fg.round(10)
        fg.simplify(0.2)
        fg.removeOverlap()
        glyph.foreground = fg
        glyph.width = 1000
        glyphToDataId[gname] = dataId
        
        # Stop those base defs
        baseDefs = ''
    else:
        raise Exception('Unknown command ' + cmd)

# Work glyph-by glyph
# (Somehow it’s quicker and works better)
# index = 0
# for glyph in font.glyphs():
   # if (index >= nHandGlyphs):
       # # Round and add extrema
       # fg = glyph.layers[1]
       # fg.round()
       # fg.addExtrema("all")
       # fg.round()
       # # Simplify to get rid of poor extrema
       # fg.simplify(0.1)
       # fg.round()
       # # Hint
       # glyph.foreground = fg
       # # Correct direction
       # if not glyph.selfIntersects():
           # glyph.correctDirection()
   # ++index

# Now change curves to quad and simplify for a while
font.is_quadratic = True
for glyph in font.glyphs():
    fg = glyph.layers[1]
    fg.simplify(0.2)
    glyph.foreground = fg
    if glyph.selfIntersects():
        dataId = glyphToDataId[glyph.glyphname]
        msg = f'{dataId}: glyph self-intersects\n'
        errFile.write(msg)
        nShownErrors += 1

errFile.write(f'Errors END, {nShownErrors} shown, {nSuppressedErrors} suppressed\n')
errFile.close()

# Saving in TTF will auto-round, that’s OK
font.generate(TEMPFILENAME)

# Run external hinter
CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))
