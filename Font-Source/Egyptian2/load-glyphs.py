import fontforge
import psMat
import os

OUTFILENAME = 'UnicodiaSesh.otf'
#HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'

# get transformation matrix
mat1 = psMat.translate(0, -600)
mat2 = psMat.scale(5.3, 5.4)
mat3 = psMat.translate(-30, -150)
mat = psMat.compose(mat1, mat2)
mat = psMat.compose(mat, mat3)

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())

# checks whether codepoint is good
def isCpGood(code):
    return (code >= 0x13460) and (code <= 0x143FF);

# Gets SVG’s height in units
# No complete XML parsing
# Very basic error checking
def getSvgHeight(fname):
    # Read data
    f = open(fname, 'r')
    data = f.read()
    f.close()
    # Try to get data
    trigger1a = 'height="'
    trigger2 =  '"'
    trigger1b = "height='"   # same length!
    pos1 = data.find(trigger1a)
    if (pos1 < 0):
        pos1 = data.find(trigger1b)
        trigger2 = "'"
        if (pos1 < 0):
            raise Exception('Cannot find trigger1: {}'.format(fname))
    pos1 += len(trigger1a)
    # debug: raise Exception("{}:{}".format(fname, pos1))
    pos2 = data.find(trigger2, pos1)
    sNumber = data[pos1:pos2]
    # Some SVGs have measurement unit under 'height'
    if (sNumber.endswith('px') or sNumber.endswith('pt')):
        sNumber = sNumber[:-2]
    return float(sNumber)

# import hieroglyphs
file = open('Unikemet.txt', 'r')
for line0 in file:
    line = line0.strip()
    if (line != '') and (not line.startswith('#')):
        cols = line.split('\t')
        if (len(cols) >= 3):
            sCp = cols[0]
            sCommand = cols[1]
            sValue = cols[2]
            if (sCp.startswith('U+') and (sCommand == 'kEH_JSesh')):
                sHex = sCp[2:]
                code = int(sHex, base=16)
                if (isCpGood(code)):
                    svgName = "svg/{}.svg".format(sValue)
                    svgHeight = getSvgHeight(svgName)  # requested rather than actual size
                    # Load SVG
                    glyph = font.createChar(code)
                    glyph.glyphname = "u{}_{}".format(sHex.upper(), sValue)
                    glyph.importOutlines(svgName, scale=False)
                    # Get transformation matrix
                    mat1 = psMat.translate(0, -800)  # move under baseline
                    mat1a = psMat.translate(0, svgHeight)  # move under baseline
                    mat2 = psMat.scale(1000.0 / svgHeight)
                    mat3 = psMat.translate(0, -100)
                    mat = psMat.compose(mat1, mat1a)
                    mat = psMat.compose(mat, mat2)
                    mat = psMat.compose(mat, mat3)
                    glyph.transform(mat)
                    # @todo [urgent] what width?
                    glyph.width = 1000

# Work glyph-by glyph
# (Somehow it’s quicker and works better)
index = 0
for glyph in font.glyphs():
    if (index >= nHandGlyphs):    
        # Round and add extrema
        fg = glyph.layers[1]
        #fg.round()
        fg.addExtrema("all")
        #fg.round()
        # Simplify to get rid of poor extrema
        fg.simplify(0.1)
        #fg.round()
        # Hint
        glyph.foreground = fg
        # Correct direction
        if not glyph.selfIntersects():
            glyph.correctDirection()
    ++index;

font.generate(OUTFILENAME)

# Run external hinter
#CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
#os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))
