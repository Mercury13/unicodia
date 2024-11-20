import fontforge
import psMat
import os

OUTFILENAME = 'UnicodiaSesh.otf'
#HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())

log = open('sesh.log', 'w')

# checks whether codepoint is good
def isCpGood(code):
    return (code >= 0x13460) and (code <= 0x143FF);

log.write("Loading SVG\n");

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

CELLSIZE = 1000

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
                    # Run Inkscape
                    cmdline = '"c:/Program Files/Inkscape/bin/inkscape.com" --actions=select-all;path-union --export-filename=~ex.svg {}'
                    os.system(cmdline.format(svgName))
                    # Load SVG
                    glyph = font.createChar(code)
                          # both Unicode and fname, for troubleshooting
                    glyph.glyphname = "u{}_{}".format(sHex.upper(), sValue)
                    glyph.importOutlines('~ex.svg', scale=False, correctdir=True)
                    # Get transformation matrix
                    mat1 = psMat.translate(0, svgHeight - 800)  # move over baseline
                    mat2 = psMat.scale(CELLSIZE / svgHeight) # And now to CELLSIZE
                    mat3 = psMat.translate(0, -125)
                    mat = psMat.compose(mat1, mat2)
                    mat = psMat.compose(mat, mat3)
                    glyph.transform(mat)
                    # Check width by ACTUAL (not requested) width
                    # @todo [urgent] what width?
                    glyph.width = CELLSIZE

log.write("Improving quality\n");

def removeOpenPaths(layer):
    nPaths = len(layer)
    for i in reversed(range(nPaths)):
        contour = layer[i]
        if not contour.closed:
            del layer[i]

SIMPVALUE = 0.6

def removeMicroIntersections(layer):
    nContours = len(layer)
    for i in reversed(range(nContours)):
        contour = layer[i]
        selfInter = contour.selfIntersects()
        if selfInter:
            # just do whatever possible
            tempLayer = fontforge.layer()
            tempLayer += contour.dup()
            if len(tempLayer) != 1:
                raise Exception('Temp layer has !=1 contours')
            tempLayer.removeOverlap()
            # Managed to get empty layer?
            if len(tempLayer) == 0:
                del layer[i]
                continue
            # Got exactly one, replace
            if (len(tempLayer) == 1) and not tempLayer.selfIntersects():
                contour = tempLayer[0]

# Work glyph-by glyph
# (Somehow it’s quicker and works better)
index = 0
nSelfIntersecting = 0
for glyph in font.glyphs():
    if (index >= nHandGlyphs):  # No hand-drawn glyphs here
        fg = glyph.layers[1]
        # Remove open paths
        removeOpenPaths(fg)
        # Round and add extrema
        #fg.round()
        fg.addExtrema("all")
        #fg.round()
        # Simplify to get rid of poor extrema
        removeMicroIntersections(fg)
        fg.simplify(SIMPVALUE, ['mergelines'])
        #fg.round()
        # Hint
        selfInter = fg.selfIntersects()
        if not selfInter:
            glyph.correctDirection()
        else:
            nSelfIntersecting += 1
            log.write("{} self-intersects, {} so far\n".format(
                    glyph.glyphname, nSelfIntersecting))
        glyph.foreground = fg
        #glyph.removeOverlap()
        # Correct direction
    index += 1

log.write("Generating font\n")
font.generate(OUTFILENAME)

# Run external hinter
#CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
#os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))

log.write("OK\n")
log.close()
