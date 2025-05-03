import fontforge
import psMat
import os

TEMPFILENAME = 'UnicodiaHan1.ttf'
OUTFILENAME = 'UnicodiaHan.ttf'
HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'

# get transformation matrix
mat1 = psMat.translate(0, -600)
mat2 = psMat.scale(5.3, 5.4)
mat3 = psMat.translate(-30, -150)
mat = psMat.compose(mat1, mat2)
mat = psMat.compose(mat, mat3)

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())

# import hieroglyphs
file = open('hani-tofu.txt', 'r')
for line0 in file:
    line = line0.strip()
    if (line != '') and (not line.startswith('#')):
        code = int(line, base=16)
        svgName = "AutoRemade/{}.svg".format(line)
        glyph = font.createChar(code)
        glyph.glyphname = "u" + line.upper()
        glyph.importOutlines(svgName, scale=False)
        glyph.transform(mat)
        glyph.width = 1000

# Work glyph-by glyph
# (Somehow it’s quicker and works better)
index = 0
for glyph in font.glyphs():
    if (index >= nHandGlyphs):    
        # Round and add extrema
        fg = glyph.layers[1]
        fg.round()
        fg.addExtrema("all")
        fg.round()
        # Simplify to get rid of poor extrema
        fg.simplify(0.1)
        fg.round()
        # Hint
        glyph.foreground = fg
        # Correct direction
        if not glyph.selfIntersects():
            glyph.correctDirection()
    ++index;

font.generate(TEMPFILENAME)

# Run external hinter
CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))