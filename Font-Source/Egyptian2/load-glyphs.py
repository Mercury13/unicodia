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

def isCpGood(code):
    return (code >= 0x13460) and (code <= 0x143FF);    

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
                    glyph = font.createChar(code)
                    glyph.glyphname = "u" + line.upper()
                    glyph.importOutlines(svgName, scale=False)
                    #glyph.transform(mat)
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
