import fontforge
import psMat
import os
import shutil

OUTFILENAME = 'UnicodiaSesh.otf'
#HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())
nSelfIntersecting = 0

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
    
# @return  [+] smth was deleted
def removeSmallPaths(layer, maxSize):
    nPaths = len(layer)
    wasDel = False
    for i in reversed(range(nPaths)):
        contour = layer[i]
        # Small, probably traces of intersections?
        [x1,y1,x2,y2] = contour.boundingBox()
        if (x2 - x1 <= maxSize) and (y2 - y1 <= maxSize):
            del layer[i]
            wasDel = True
    return wasDel

def removeObviousPaths(layer):
    nPaths = len(layer)
    for i in reversed(range(nPaths)):
        contour = layer[i]
        # Closed?
        if not contour.closed:
            del layer[i]
    removeSmallPaths(layer, 2)

SIMPVALUE = 0.6
BIGSMALLVALUE = 5

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
            removeSmallPaths(tempLayer, BIGSMALLVALUE)
            # Managed to get empty layer? Probably just reverse contour?
            if (len(tempLayer) != 1) or tempLayer.selfIntersects():
                tempLayer = fontforge.layer()
                tempLayer += contour.dup()
                tempLayer[0].reverseDirection()
                tempLayer.removeOverlap()
                removeSmallPaths(tempLayer, BIGSMALLVALUE)
            # Got exactly one, replace
            if (len(tempLayer) == 1) and not tempLayer.selfIntersects():
                layer[i] = tempLayer[0]

def improveGlyph(glyph):
    global nSelfIntersecting
    fg = glyph.layers[1]
    # Remove open paths
    removeObviousPaths(fg)
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
    glyph.foreground = fg
    isOk = False
    if not selfInter:
        glyph.correctDirection()
        isOk = True
    else:
        nSelfIntersecting += 1
        log.write("{} self-intersects, {} so far\n".format(
                glyph.glyphname, nSelfIntersecting))
    #glyph.removeOverlap()
    # Correct direction
    return isOk

def loadGlyph(glyph, fname, svgHeight):
    glyph.importOutlines(fname, scale=False, correctdir=True)
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
    return improveGlyph(glyph)

CELLSIZE = 1000

# import hieroglyphs
def loadUnikemet():
    file = open('Unikemet.txt', 'r')
    nCps = 0
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
                        cacheName = "cache/{}.svg".format(sValue)
                        manualName = "manual/{}.svg".format(sValue)
                        svgHeight = getSvgHeight(svgName)  # requested rather than actual size
                        # Load SVG
                        glyph = font.createChar(code)
                              # both Unicode and fname, for troubleshooting
                        glyph.glyphname = "u{}_{}".format(sHex.upper(), sValue)
                        # Load?
                        if os.path.exists(manualName):
                            # Manual glyph
                            loadGlyph(glyph, manualName, svgHeight)
                        elif os.path.exists(cacheName):
                            # Cached glyph: already ran software
                            loadGlyph(glyph, cacheName, svgHeight)
                        else:
                            # Unknown glyph
                            isGood = loadGlyph(glyph, svgName, svgHeight)
                            if not isGood:
                                # Run Inkscape
                                log.write("Forced to run Inkscape!\n")
                                cmdline = '"c:/Program Files/Inkscape/bin/inkscape.com" --actions=select-all;path-union --export-filename={} {}'
                                os.system(cmdline.format(cacheName, svgName))
                                glyph.clear()
                                loadGlyph(glyph, cacheName, svgHeight)
                        nCps += 1
                        if nCps >= 5000:
                            return

loadUnikemet()

log.write("Generating font\n")
font.generate(OUTFILENAME)

# Run external hinter
#CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
#os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))

log.write("OK\n")
log.close()
