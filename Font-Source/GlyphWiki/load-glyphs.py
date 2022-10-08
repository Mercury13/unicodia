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

# Convert layers to cubic
font.is_quadratic = False

# import hieroglyphs
file = open('hani-tofu.txt', 'r')
for line0 in file:
    line = line0.strip()
    code = int(line, base=16)
    svgName = "AutoRemade/{}.svg".format(line)
    glyph = font.createChar(code)
    glyph.glyphname = "u" + line.upper()
    glyph.importOutlines(svgName, scale=False)
    glyph.transform(mat)
    glyph.width = 1000

# Convert layers to quadratic
font.is_quadratic = True

# Work glyph-by glyph
# (Somehow it’s quicker and works better)
for glyph in font.glyphs():
    # Simplify a few times
    fg = glyph.layers[1]
    fg.round()
    fg.addExtrema()
    fg.round()
    for i in range(3):
        fg.simplify(4)
    fg.round()
    fg.addExtrema()
    fg.round()
    # Hint
    glyph.foreground = fg
    # Correct direction
    if not glyph.selfIntersects():
        glyph.correctDirection()

font.generate(TEMPFILENAME)

# Run external hinter
CMDLINE = '{} --windows-compatibility --stem-width-mode=sss --symbol {} {}'
os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))