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

        cachedSvg = CACHE_PATH + sUcode.upper() + '_' + sHash + '.svg'
        if not os.path.exists(cachedSvg):
            cmdline = NODELINE.format(NODE, RUN, entryPoint, baseDefs, cachedSvg)
            debugFile = open(CACHE_PATH + '~hani.log', 'w')
            debugFile.write(cmdline)
            debugFile.write('\n')
            debugFile.close()
            os.system(cmdline)

            fileSize = os.path.getsize(cachedSvg)
            if (fileSize < 200):
                raise Exception('Probably made an empty file');

        glyph = font.createChar(ucode)
        glyph.glyphname = "u" + sUcode.upper()
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
        fg.simplify(0.1)
        fg.round()
        fg.removeOverlap()
        fg.round()
        glyph.foreground = fg
        glyph.width = 1000

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

#font.generate(TEMPFILENAME)

# Run external hinter
#CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
#os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))