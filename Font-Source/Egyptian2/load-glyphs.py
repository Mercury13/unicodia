import fontforge
import psMat
import os
import shutil

TEMPFILENAME = '~UnicodiaSesh.ttf'
OUTFILENAME = 'UnicodiaSesh.ttf'
HINTER = 'd:/Soft/FontEditing/ttfautohint.exe'
INKSCAPE = 'c:/Program Files/Inkscape/bin/inkscape.com'

# These pairs are known to be bad
BAD_JSESH_HIEROS = {
    0x13AD2: "E240",   # lion → lioness
    0x13AD3: "E241",   # lion → lioness
    0x13BD4: "F128",   # cauldron? → udder
    0x13BDA: "F132B",  # IDK
    0x13EB2: "M39B",   # big basket of fruit
    0x13F29: "N31h",   # flowers in circular road
    0x13F44: "N104",   # a well with a stroke
}
BAD_JSESH_KEYS = BAD_JSESH_HIEROS.keys()

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
SMALLSIMPVALUE = 0.6
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

def improveGlyph(glyph, logBad):
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
    elif logBad:
        nSelfIntersecting += 1
        log.write("{} self-intersects, {} so far\n".format(
                glyph.glyphname, nSelfIntersecting))
    #glyph.removeOverlap()
    # Correct direction
    return isOk

# General dimensions
CELLHEIGHT = 1000
CELLWIDTH = 1100
BOTTOMHANG = 125
BEARING = 40
V_THRESHOLD = CELLHEIGHT - BOTTOMHANG * 2
# General height-limited (=positive)
MICROSCOPIC = 320
TEENY = 400
TINY = 500
SMALL = 650
MEDIUM = 800
# Special height-limited
BUCKLER = 800
BOWSTRING = 800
# General width-limited (=negative)
SQUARE = -1000
MEDWIDE = -1250
WIDE = -1400
EVERYBIT = -1500  # squeeze out every bit of width
#Special width-limited
ARM = -1000
SMALLDONKEY = -1220
MEDDONKEY = -1300  # donkeys with folded/outstretched legs are somehow equal
BUBALIS = -1150

GLYPH_SIZES = {
    0x1347B: MEDWIDE, 0x1347C: MEDWIDE,
    0x13481: MEDWIDE,
    0x1348E: MEDWIDE,
    0x134AA: MEDIUM, 0x134AB: MEDIUM,
    0x134AF: MEDIUM,
    0x134BE: SQUARE, 0x134C0: MEDWIDE, 0x134C1: MEDWIDE,
    0x134C2: WIDE,  0x134C3: WIDE,  0x134C5: WIDE,
    0x134F4: MEDWIDE,
    0x134FA: WIDE,
    0x1351D: WIDE, 0x1351E: WIDE,
    0x135BB: MEDWIDE,
    0x135BD: WIDE,
    0x135D3: MEDWIDE,
    0x135C4: MEDWIDE, 0x135C5: MEDWIDE, 0x135C6: MEDWIDE, 0x135C7: MEDWIDE,
    0x135DC: EVERYBIT, 0x135DD: EVERYBIT,
    0x135FB: WIDE,
    0x13605: WIDE,
    0x13606: EVERYBIT,
    0x13609: WIDE, 0x1360A: WIDE, 0x1360B: WIDE, 0x1360C: EVERYBIT,
    0x13615: EVERYBIT,
    0x13617: WIDE, 0x13618: WIDE, 0x1361A: WIDE,
    0x1361C: WIDE,
    0x13625: WIDE,
    0x136A4: WIDE,
    0x136CE: EVERYBIT,
    0x136D1: WIDE, 0x136D2: EVERYBIT,
    0x1371B: MEDIUM,
    0x13746: MEDIUM, 0x13747: MEDIUM,
    0x1378C: WIDE,
    0x137E3: WIDE, 0x137E4: WIDE, 0x137E5: WIDE, 0x137E6: WIDE,
    0x137F1: WIDE, 0x137F2: WIDE,
    0x137F9: WIDE,
    0x13856: WIDE,
    0x13872: WIDE,
    0x138BE: WIDE,
    0x13923: WIDE, 0x13924: WIDE, 
    0x1392A: EVERYBIT,
    0x1392F: SMALL,
    0x1393C: MEDIUM,
    0x13948: MEDWIDE, 0x13949: WIDE, 0x1394A: MEDWIDE,
    0x1395F: MEDIUM,
    0x1396A: MEDIUM, 0x1396B: MEDIUM, 0x1396C: MEDIUM, 0x1396D: MEDIUM, 0x1396E: MEDWIDE,
    0x13973: WIDE,
    0x1397B: WIDE,
    0x13981: MEDWIDE, 0x13982: MEDWIDE, 0x13983: MEDWIDE, 0x13984: MEDWIDE,
    0x1398F: MICROSCOPIC,
    0x1399F: SMALL,
    0x139C2: WIDE, 0x139C3: ARM,  # arms
    0x139C9: ARM, # arms
    0x139F6: EVERYBIT,
    0x139F8: WIDE,
    0x139FC: SMALL,
    0x13A05: SMALL,
    0x13A0A: SMALL, 0x13A0B: SMALL, 0x13A0C: MEDIUM, 0x13A0F: MEDIUM,
    0x13A17: MEDIUM, 0x13A18: MEDIUM,
    0x13A31: SMALL,
    0x13A34: SMALL,
    0x13A39: MEDDONKEY,  # donkeys
    0x13A42: SMALLDONKEY, 0x13A43: SMALLDONKEY,  # more donkeys
    0x13A53: CELLHEIGHT,
    0x13A7B: BUBALIS, # newborn bubalis
    0x13A85: WIDE,
    0x13A54: WIDE, 0x13A55: WIDE, 0x13A56: WIDE, 0x13A58: WIDE,
    0x13A85: EVERYBIT,
    0x13B2D: EVERYBIT,
    0x13B59: WIDE,
    0x13B5B: WIDE,
    0x13B60: MEDIUM,
    0x13B69: WIDE,
    0x13B93: WIDE, 0x13B94: WIDE,
    0x13BAA: WIDE, 0x13BAB: WIDE,
    0x13BB1: MEDIUM,
    0x13BC3: TINY,
    0x13BCB: SMALL, 0x13BCC: WIDE,
    0x13BD4: SMALL,
    0x13BF8: SMALL,
    0x13BFA: MEDIUM, 0x13BFB: MEDIUM,
    0x13C01: SMALL, 0x13C02: SMALL,
    0x13C05: TINY, 0x13C06: TINY,
    0x13C08: SMALL,
    0x13C27: CELLHEIGHT, 0x13C28: CELLHEIGHT,
    0x13C2E: MEDIUM, 0x13C2F: MEDIUM,
    0x13C3A: EVERYBIT,
    0x13CE1: MEDIUM, 0x13CE2: MEDWIDE,
    0x13D2B: MEDIUM, 0x13D2C: MEDIUM, 0x13D2D: MEDIUM,
    0x13D2F: MEDWIDE, 0x13D30: WIDE,    
    0x13D4D: SMALL, 0x13D4E: SMALL, 0x13D4F: TINY,
    0x13D61: MEDWIDE,
    0x13D65: MEDWIDE,
    0x13D7F: EVERYBIT,
    0x13D97: WIDE,
    0x13DA9: MEDWIDE,
    0x13DC5: EVERYBIT, 0x13DC6: EVERYBIT,
    0x13DC9: MEDIUM,
    0x13DE4: EVERYBIT,
    0x13E0C: MEDIUM,
    0x13E2B: MEDIUM, 0x13E2C: MEDIUM, 0x13E2D: MEDIUM,
    0x13E2F: MEDIUM, 0x13E30: MEDIUM, 0x13E31: MEDIUM, 0x13E32: MEDIUM,
    0x13E35: WIDE,
    0x13E37: WIDE,
    0x13E46: SMALL,
    0x13E64: TINY,
    0x13E85: MEDIUM,
    0x13EA3: SMALL, 0x13EA4: SMALL, 0x13EA5: TEENY,
    0x13EA8: MEDWIDE,
    0x13EAB: SMALL,
    0x13EBE: MEDIUM,
    0x13EE7: MEDWIDE, 0x13EE8: MEDWIDE,
    0x13EF5: TINY,
    0x13F44: TINY, 0x13F47: MEDIUM,
    0x13F4B: MEDIUM,
    0x13F50: MEDWIDE, 0x13F51: MEDWIDE, 0x13F52: MEDWIDE, # building plans
    0x13F74: WIDE, # overthrow wall, not really enough width
    0x13FD7: MEDWIDE,
    0x13FEF: EVERYBIT,  # gate of lions
    0x14004: MEDWIDE,
    0x14015: SMALL,
    0x140CA: MEDIUM,
    0x140F5: WIDE, 0x140F6:WIDE, 0x140F7: WIDE, 0x140F8: WIDE,
    0x140FA: MEDIUM,
    0x140FE: EVERYBIT,
    0x14178: WIDE, 0x14179: WIDE,  # feathers on horns
    0x1417F: WIDE,  # more feathers on horns
    0x14180: MEDWIDE, 0x14181: MEDWIDE,
    0x14183: MEDWIDE, 0x14184: MEDWIDE,
    0x141B5: WIDE, 0x141B6: EVERYBIT,
    0x141F1: MEDIUM,
    0x14202: BUCKLER,
    0x14224: BOWSTRING, 0x14225: BOWSTRING, 0x14226: BOWSTRING, 0x14227: BOWSTRING, 0x14228: BOWSTRING,  # bowstrings
    0x1428E: MEDIUM,
    0x14295: SMALL,
    0x142D8: MEDIUM,
    0x1431B: MEDIUM,
    0x1431E: SMALL,
    0x14321: MEDIUM,
    0x14326: MEDIUM, 0x14327: MEDIUM, 0x14328: SMALL, 0x14329: MEDWIDE,
    0x14356: SMALL,
    0x14369: MEDIUM, 0x1436A: MEDIUM,
    0x1436D: MEDIUM,
    0x1437C: MEDIUM, 0x1437D: MEDIUM, 0x1437E: MEDIUM,
    0x1438D: MEDIUM, 0x1438E: SMALL, 0x1438F: MEDIUM,
    0x14392: MEDIUM,
    0x14394: MEDIUM,
    0x14399: MEDIUM, 0x1439C: MEDIUM, 
    0x1439F: MEDIUM, 0x143A0: MEDIUM, 0x143A1: MEDIUM,
    0x143A7: MEDIUM,
    0x143AE: MEDIUM,
    0x143B4: MEDIUM,
    0x143BA: MEDIUM,
    0x143BC: MEDIUM, 0x143BE: MEDIUM,
    0x143C0: MEDIUM,
    0x143CA: SMALL, 0x143CB: SMALL, 0x143CC: SMALL,
    0x143D4: MEDWIDE, 0x143D5: MEDIUM,
    0x143E0: SMALL,
    0x143EE: MEDIUM,
    0x143F5: WIDE,
}

def glyphSize(cp):
    if cp in GLYPH_SIZES:
        return GLYPH_SIZES[cp]
    if (cp >= 0x13A4C) and (cp <= 0x13A59):
        return MEDWIDE  # animal of Seth
    if (cp >= 0x13A36) and (cp <= 0x13B28):
        return WIDE     # donkeys, goats, lions, mice, sphinxes, crocodiles
    if (cp >= 0x13B29) and (cp <= 0x13B58):
        return MEDWIDE     # bulls
    if (cp >= 0x13B97) and (cp <= 0x13BA0):
        return MEDWIDE  # architectural (?) donkeys
    if (cp >= 0x13B91) and (cp <= 0x13BA1):
        return WIDE     # horses, lions, elephants
    if (cp >= 0x13C0B) and (cp <= 0x13D0E):
        return WIDE     # birds
    if (cp >= 0x13D17) and (cp <= 0x13D1F):
        return MEDIUM   # birds’ heads
    if (cp >= 0x13D32) and (cp <= 0x13D48):
        return WIDE     # crocodiles
    if (cp >= 0x13DB0) and (cp <= 0x13E01):
        return WIDE     # snakes, fish, beetles
    if (cp >= 0x1401A) and (cp <= 0x1407A):
        return MEDWIDE     # boats
    return -CELLWIDTH  # default CELLWIDTH × CELLHEIGHT

def fixBearings(glyph):
    glyph.left_side_bearing = BEARING
    glyph.right_side_bearing = BEARING
    
def fixSize(cp, glyph, svgHeight):
    mySize = glyphSize(cp)
    if (mySize < 0):
        [myWidth, myHeight] = [-mySize, CELLHEIGHT]
    else:
        [myWidth, myHeight] = [-MEDWIDE, mySize] # shrinking → give a bit leeway
    # Get transformation matrix
    mat1 = psMat.translate(0, svgHeight - 800)  # move over baseline
    mat2 = psMat.scale(myHeight / svgHeight) # And now to myHeight
    mat = psMat.compose(mat1, mat2)
    glyph.transform(mat)
    # Check width by ACTUAL (not requested) width
    [x1,y1,x2,y2] = glyph.boundingBox()
    actualWidth = x2 - x1
    actualHeight = y2 - y1
    if (actualWidth < 300) and (actualHeight < 300):
        log.write("{} is really small\n".format(
                glyph.glyphname))
    if actualWidth > myWidth:
        newHeight = myHeight * myWidth / actualWidth
        mat = psMat.scale(myWidth / actualWidth)
        glyph.transform(mat)
    # Hang properly
    [x1,y1,x2,y2] = glyph.boundingBox()
    if y2 > V_THRESHOLD:
        mat = psMat.translate(0, (V_THRESHOLD - y2)  / 2)
        glyph.transform(mat)

def loadGlyph(glyph, cp, fname, svgHeight, logBad):
    glyph.importOutlines(fname, scale=False, correctdir=True)
    fixSize(cp, glyph, svgHeight)
    fixBearings(glyph)
    return improveGlyph(glyph, logBad)
    
def newGlyph(font, code, glyphName):
    glyph = font.createChar(code)
    glyph.glyphname = glyphName
    return glyph

# Load glyph manually optimized by user
def loadManualGlyph(font, code, glyphName, fname, logName):
    glyph = newGlyph(font, code, glyphName)
    glyph.importOutlines(fname, scale=False, correctdir=True)
    glyph.simplify(SMALLSIMPVALUE, ['mergelines'])
    fixBearings(glyph)
    if glyph.selfIntersects():
        log.write("{} is {}, and still self-intersects!\n".format(glyph.glyphname, logName))

def getManualName(dirName, glyphName):
    return "{}/{}_UnicodiaSesh.svg".format(dirName, glyphName)
    
# Load glyph created from scratch by user
# The glyph is surely flat, do not run InkScape
def loadMyGlyph(font, sHex, cp, svgName):
    newGlyphName = "u{}".format(sHex.upper())
    glyph = newGlyph(font, cp, newGlyphName)
    newSvgHeight = getSvgHeight(svgName)
    loadGlyph(glyph, cp, svgName, newSvgHeight, True)

# import hieroglyphs
def loadUnikemet():
    file = open('Unisesh.txt', 'r')
    nCps = 0
    sOldCp = ''
    hasSeshGlyph = True
    for line0 in file:
        line = line0.strip()
        if (line != '') and (not line.startswith('#')):
            cols = line.split('\t')
            if (len(cols) >= 3):
                sCp = cols[0]
                sCommand = cols[1]
                sValue = cols[2]
                if sCp.startswith('U+'):
                    if sCp != sOldCp:
                        if (sOldCp != '') and not hasSeshGlyph:
                            sHex = sOldCp[2:]
                            code = int(sHex, base=16)
                            if (isCpGood(code)):
                                svgName = "svg-my/{}.svg".format(sHex)
                                if os.path.exists(svgName):
                                    loadMyGlyph(font, sHex, code, svgName)
                        hasSeshGlyph = False
                    sOldCp = sCp
                    if sCommand == 'kEH_JSesh':
                        sHex = sCp[2:]
                        code = int(sHex, base=16)
                        if (isCpGood(code)):
                            hasSeshGlyph = True
                            isKnownBadGlyph = (code in BAD_JSESH_KEYS) and (BAD_JSESH_HIEROS[code] == sValue)
                                  # both Unicode and fname, for troubleshooting
                            glyphName = "u{}_{}".format(sHex.upper(), sValue)
                            svgName = "svg/{}.svg".format(sValue)
                            svgRemadeName = "svg-remade/{}.svg".format(sValue)
                            cacheName = "cache/{}.svg".format(sValue)                        
                            manualName = getManualName('manual', glyphName)
                            manualWideName = getManualName('manual-wide', glyphName)
                            reallyMyName = "svg-my/{}.svg".format(sHex)
                            svgHeight = getSvgHeight(svgName)  # requested rather than actual size
                            # Load?
                            isLoaded = True
                            if os.path.exists(reallyMyName):
                                if os.path.exists(manualName):
                                    log.write("{} exists!\n".format(manualName))
                                if os.path.exists(manualWideName):
                                    log.write("{} exists!\n".format(manualWideName))
                                if os.path.exists(cacheName):
                                    log.write("{} exists!\n".format(cacheName))
                                loadMyGlyph(font, sHex, code, reallyMyName)
                            elif os.path.exists(manualWideName):
                                # Manual glyph
                                loadManualGlyph(font, code, glyphName, manualWideName, 'manual-wide')
                            elif os.path.exists(manualName):
                                # Manual glyph
                                loadManualGlyph(font, code, glyphName, manualName, 'manual')
                            elif os.path.exists(svgRemadeName):
                                glyph = newGlyph(font, code, glyphName)
                                svgHeight = getSvgHeight(svgRemadeName)
                                loadGlyph(glyph, code, svgRemadeName, svgHeight, True)
                            elif os.path.exists(cacheName) and not isKnownBadGlyph:
                                # Cached glyph: already ran software
                                glyph = newGlyph(font, code, glyphName)
                                loadGlyph(glyph, code, cacheName, svgHeight, True)
                            elif not isKnownBadGlyph:
                                # Unknown glyph
                                glyph = newGlyph(font, code, glyphName)
                                isGood = loadGlyph(glyph, code, svgName, svgHeight, False)
                                if not isGood:
                                    # Run Inkscape
                                    log.write("Forced to run Inkscape!\n")
                                    cmdline = '"{}" --actions=select-all;path-union --export-filename={} {}'
                                    os.system(cmdline.format(INKSCAPE, cacheName, svgName))
                                    glyph.clear()
                                    loadGlyph(glyph, code, cacheName, svgHeight, True)
                            else:
                                isLoaded = False
                            if isLoaded:
                                nCps += 1
                                if nCps >= 5000:
                                    return

loadUnikemet()

log.write("Generating font\n")
font.generate(TEMPFILENAME)

# Run external hinter
CMDLINE = '{} --stem-width-mode=sss --symbol {} {}'
os.system(CMDLINE.format(HINTER, TEMPFILENAME, OUTFILENAME))

log.write("OK\n")
log.close()
