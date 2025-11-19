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
    0x1333B: "U7",     # Anyway the hoe is mine, but meanwhile…
    0x13BD4: "F128",   # cauldron? → udder
    0x13BDA: "F132B",  # IDK
    0x13F29: "N31h",   # flowers in circular road
}
BAD_JSESH_KEYS = BAD_JSESH_HIEROS.keys()

fontforge.runInitScripts()
font = fontforge.activeFont()
nHandGlyphs = sum(1 for _ in font.glyphs())
nSelfIntersecting = 0

log = open('sesh.log', 'w')

# checks whether codepoint is good
def isCpGood(code):
    return ((code >= 0x13000) and (code <= 0x13385)) \
        or ((code >= 0x13460) and (code <= 0x143FF));

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

SIMPVALUE = 0.8
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

# General dimensions
CELLHEIGHT = 1000
CELLWIDTH = 1100
BRKHEIGHT = 1200  # brackets’ height
BOTTOMHANG = 100
BEARING = 50
SMALL_BEARING = 35
V_THRESHOLD = CELLHEIGHT - BOTTOMHANG * 2
# General height-limited (=positive)
MICROSCOPIC = 320
TEENY = 400
TINY = 500
SMALL = 650
MEDIUM = 800
# Special height-limited
DRINKING = 750
LOAF = 700
SMALL_TIT = 700
BUCKLER = 800
BOWSTRING = 800
COILED_ROPE = 800
BOWING = 950
# General width-limited (=negative)
NORMAL = -CELLWIDTH
SQUARE = -1000
HARE_NO_WHISKERS = -1131
MEDWIDE = -1250
WIDE = -1400
EVERYBIT = -1500  # squeeze out every bit of width
# Special width-limited
ARM = -1000
GOAT_CALF = -1200
SUN_HILL = -1200
SMALL_DONKEY = -1220
TAILLESS_DONKEY = -1240
MED_DONKEY = -1300  # donkeys with folded/outstretched legs are somehow equal
BUBALIS = -1150
COILED_WIDE = -1450

GLYPH_SIZES = {
    # EGYP MAIN -- A: Man
    0x13009: MEDWIDE,  # tired
    0x13010: SQUARE,  # corpse
    0x13012: SQUARE, 0x13013: BOWING,  # horiz man / bowing man
    0x1303F: SQUARE,  0x13040: SQUARE, # lying mummy
    # D: parts of human
    0x13076: MEDIUM,  # head in profile
    0x13081: TEENY, 0x13082: TEENY, 0x13084: TEENY, # parts of eyes
    0x13086: MEDIUM,  # parts of eyes
    0x1307D: MEDWIDE, 0x1307E: MEDWIDE, # eyes in rectangle
    0x13088: MEDIUM, 0x13089: MEDIUM, 0x1308A: MEDIUM, # ears and faces
    0x13091: SMALL_TIT,  # tit
    0x1309C: WIDE,  # 2 arms
    0x130AC: SQUARE,  # fist
    0x130B0: WIDE,  #finger…
    0x130B6: EVERYBIT, 0x130B7: SQUARE,  # fingers
    0x130C3: MEDIUM, 0x130C4: MEDIUM,  # IDK, limit them by height
    0x130CD: MEDWIDE, 0x130CE: MEDWIDE, 0x130CF: EVERYBIT, 0x130D0: EVERYBIT, 0x130D1: MEDWIDE, # rings
    # E: animals
    0x130D2: MEDWIDE, 0x130D3: WIDE, 0x130D4: MEDWIDE, 0x130D5: MEDWIDE,  # cows
    0x130D6: MEDWIDE, 0x130D7: MEDWIDE, 0x130D8: MEDWIDE,
    0x130D9: GOAT_CALF, 0x130DA: GOAT_CALF,  # goats w/o beard
    0x130DB: MEDWIDE, 0x130DC: MEDWIDE, 0x130DD: MEDWIDE, 0x130DE: MEDWIDE,  # bubalis, cow, sheep
    0x130DF: MEDWIDE,  # pig…
    0x130E1: MEDWIDE, 0x130E2: MEDWIDE, 0x130E3: MEDWIDE, 0x130E4: MEDWIDE,  # jackal
    0x130E5: MEDWIDE,  # jackal
    0x130EB: WIDE, 0x130EC: MEDWIDE, 0x130ED: MEDWIDE, 0x130EE: WIDE, 0x130EF: MEDWIDE, # misc. animals
    0x130F0: MEDWIDE,  # elephant
    0x130F6: MEDWIDE, 0x130F7: MEDWIDE,  # goat, monkey
    0x130F9: MEDWIDE, 0x130FA: SQUARE,  # hare
    # F: parts of animals
    0x13105: MEDWIDE,  # ram’s head
    0x13114: MEDIUM, 0x13115: MEDIUM,  # animal’s ears
    0x1312B: WIDE,  # some kind of spine
    0x13132: SQUARE, 0x13133: SQUARE, 0x13134: SQUARE, 0x13135: SQUARE,  # intestine
    0x13136: SQUARE, 0x13137: SQUARE, 0x13138: SQUARE,  # intestine
    # G: birds
    0x13140: MEDWIDE, 0x13141: MEDWIDE, # vultures
    0x1314B: MEDWIDE,  # bark of Sokar
    0x1314C: MEDWIDE, 0x1314D: MEDWIDE, 0x1314E: MEDWIDE, 0x1314F: MEDWIDE,  # monuments of falcon
    0x13152: EVERYBIT, 0x13154: WIDE, 0x13155: WIDE, 0x13156: WIDE,  0x13157: WIDE, # more birds
    0x1315A: MEDWIDE, 0x1315B: MEDWIDE,  # and more birds
    0x1315F: MEDWIDE,  # flamingo
    0x13162: EVERYBIT,  # storks
    0x13167: MEDWIDE,  # cormorant
    0x13170: MEDWIDE,  # some seagull
    0x13172: WIDE,  # chick+bread, legacy
    0x13174: WIDE, 0x13175: WIDE, 0x13176: WIDE,  # chicks
    0x13178: WIDE, 0x13179: WIDE,  # ducklings
    0x1317A: WIDE,  # ducks
    0x1317C: MEDWIDE,  # goose
    0x1317E: MEDWIDE,  # plucked duck
    # H: parts of birds
    0x1317F: MEDIUM, 0x13180: MEDIUM, 0x13181: MEDIUM, 0x13182: MEDIUM, 0x13183: MEDWIDE,  # misc
    0x13187: MEDIUM,  #egg
    # I: reptiles
    0x13188: MEDWIDE, 0x13189: MEDWIDE, 0x1318A: WIDE, 0x1318B: MEDWIDE, 0x1318C: MEDWIDE, 0x1318D: WIDE,  # lizard, turtle, croc
    0x1318E: SMALL,  # croc scale
    0x13191: MEDWIDE,  # viper
    0x13199: MEDWIDE, 0x1319A: MEDWIDE,  # snakes
    # K: fish
    0x1319B: MEDWIDE, 0x1319C: MEDWIDE, 0x1319D: MEDWIDE, 0x1319E: MEDWIDE, 0x1319F: MEDWIDE,  # misc fishes
    0x131A0: SMALL,  # fish scale
    0x131A2: MEDWIDE,  # catfish
    # L: lesser animals
    0x131A9: MEDIUM, 0x131AA: MEDIUM,  # bivalves
    # M: plants
    0x131BE: WIDE, 0x131BF: EVERYBIT, 0x131C0: EVERYBIT,  # flowers counting
    0x131D1: MEDWIDE,  # two stalks
    0x131E0: MEDWIDE,  # grains
    # N: geography
    0x131F2: SQUARE,  # sky
    0x131F3: MEDIUM,   # sun
    0x131F4: MEDWIDE,  # uraeus
    0x131F7: MEDIUM, 0x131F8: MEDIUM,  # moon
    0x13205: TEENY,
    0x1320D: SUN_HILL,  # sun behind hill
    0x1320E: SMALL,  # small slope
    0x13211: MEDIUM,  # clay
    0x13214: MEDWIDE, 0x13215: MEDIUM,  # some metal
    0x1321E: MEDIUM, 0x1321F: MEDIUM,  # wells
    # Nu/Ul: coats of arms
    0x13230: WIDE,  # Nl16: hanging fishtail
    0x13248: WIDE,  # Nu17: hanging jackal’s tail
    # O: buildings
    0x1326A: MEDWIDE,  # portico with 7 uraei
    0x13275: MEDIUM,  # pedestal of sun
    0x1327E: WIDE,  # four Y-poles
    0x1328B: MEDIUM,  # corner
    0x1328E: WIDE,  # double stairs
    0x13292: MEDIUM, 0x13293: MEDIUM,  # dome
    0x13295: SMALL,  # enclosed mound
    0x13296: SMALL,  # village
    0x13297: MEDIUM,  # threshing floor
    0x13298: MEDIUM, 0x13299: MEDIUM,  # same, simplified
    # P: ships
    0x1329B: MEDWIDE, 0x1329C: MEDWIDE,  # basic boat
    0x1329D: MEDWIDE,  # ship under sail
    0x1329E: MEDWIDE, 0x1329F: MEDWIDE,  # sacred ship
    0x132A0: MEDWIDE,  # fisher’s boat
    0x132A6: MEDIUM,  # rudder oar
    # Q: furniture
    0x132AA: SMALL,  # stool
    # R: religious
    0x132B8: SMALL,  # cup w/smoke
    # S: clothing and decorations
    0x132DD: MEDIUM,  # broad collar
    0x132E9: MEDIUM,  # seal
    0x132EC: MEDWIDE,  # cartouche + 2 flails
    0x13306: MEDIUM,  # Arafat hat
    # T: weapons
    0x13312: MEDWIDE, 0x13313: MEDWIDE, 0x13314: MEDWIDE,  # bows
    0x1331D: WIDE,  # chariot
    0x13328: MEDIUM,  # butcher’s block
    # U: tools
    0x1334A: SMALL,
    # V: ropes
    0x13364: MEDWIDE, 0x13365: WIDE, 0x13366: MEDWIDE, 0x13367: MEDWIDE,  # rope counting
    0x13368: WIDE, 0x13369: WIDE, 0x1336A: EVERYBIT, 0x1336B: EVERYBIT,  # rope counting
    0x13371: SMALL, 0x13372: SMALL, 0x13373: MEDIUM,  # rope coils
    0x13375: MEDIUM,  # coil + bread
    0x13376: MEDIUM,  # round cartouche
    0x13378: SMALL,  # part of cartouche
    0x13382: EVERYBIT,  # looped cord
    # EGYP A
    # A: man
    0x1347B: MEDWIDE, 0x1347C: MEDWIDE,
    0x13481: MEDWIDE,
    0x1348E: MEDWIDE,
    0x134AA: MEDIUM, 0x134AB: MEDIUM,
    0x134AF: MEDIUM,
    0x134BE: SQUARE, 0x134C0: MEDWIDE, 0x134C1: MEDWIDE,
    0x134C2: WIDE,  0x134C3: WIDE,  0x134C5: WIDE,
    0x134F4: MEDWIDE,
    0x134FA: WIDE,
    0x13518: MEDWIDE,  # pouring
    0x1351B: WIDE, 0x1351C: WIDE, 0x1351D: WIDE, 0x1351E: WIDE,  # pouring
    0x1352E: WIDE,  # man w/grindstone, no need to squeeze
    0x13548: EVERYBIT,
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
    # B: woman
    0x1371B: MEDIUM,
    0x13746: MEDIUM, 0x13747: MEDIUM,
    # C: god
    0x1378C: WIDE,
    0x137E3: WIDE, 0x137E4: WIDE, 0x137E5: WIDE, 0x137E6: WIDE,
    0x137F1: WIDE, 0x137F2: WIDE,
    0x137F9: WIDE,
    0x13856: WIDE,
    0x13872: WIDE,
    0x138BE: WIDE,
    0x13923: WIDE, 0x13924: WIDE, 
    0x1392A: EVERYBIT,  # woman holding sky
    # D: parts of human
    0x1392D: MEDIUM, 0x1392E: SMALL, 0x1392F: SMALL,  # heads
    0x1393C: MEDIUM,
    0x13944: MEDWIDE, 0x13945: SQUARE, 0x13946: SQUARE,  # head + arm
    0x13948: MEDWIDE, 0x13949: WIDE, 0x1394A: MEDWIDE,
    0x1395F: DRINKING,  # drinking head+arms
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
    0x13A0A: SMALL, 0x13A0B: SMALL, 0x13A0C: MEDIUM,  # testicles
    0x13A0E: SMALL, 0x13A0F: MEDIUM,  # vulva
    0x13A17: MEDIUM, 0x13A18: MEDIUM, # smth on feet: wanted consistent size?
    0x13A31: SMALL,  # toe
    0x13A34: SMALL,  # windpipe
    # E: animals
    0x13A39: MED_DONKEY,  # donkeys
    0x13A41: TAILLESS_DONKEY, 0x13A42: SMALL_DONKEY, 0x13A43: SMALL_DONKEY,  # more donkeys
    0x13A53: CELLHEIGHT,
    0x13A7B: BUBALIS,  # newborn bubalis
    0x13A85: WIDE,
    0x13AAB: GOAT_CALF,  #goat
    0x13A54: WIDE, 0x13A55: WIDE, 0x13A56: WIDE, 0x13A58: WIDE,
    0x13A85: EVERYBIT,
    0x13ABB: MEDWIDE,  # hyena
    0x13ABD: HARE_NO_WHISKERS, 0x13ABE: HARE_NO_WHISKERS, 0x13ABF: MEDWIDE,  # hares
    0x13B2D: EVERYBIT,
    0x13B3C: WIDE,  # attacking bull
    0x13B59: WIDE,
    0x13B5B: WIDE,
    0x13B60: MEDIUM,
    0x13B69: WIDE,
    0x13B74: MEDWIDE,  # push-pull cow
    0x13B93: WIDE, 0x13B94: WIDE,
    0x13BAA: WIDE, 0x13BAB: WIDE,
    0x13BB1: MEDIUM,
    0x13BC3: TINY,
    0x13BCB: SMALL, 0x13BCC: WIDE,
    0x13BD4: SMALL,
    0x13BD9: MEDWIDE,  # some sort of heart
    0x13BF8: SMALL,
    0x13BFA: MEDIUM, 0x13BFB: MEDIUM,
    0x13C01: SMALL, 0x13C02: SMALL,
    0x13C05: TINY, 0x13C06: TINY,
    0x13C08: SMALL,
    0x13C27: WIDE, 0x13C28: CELLHEIGHT,
    0x13C2E: MEDIUM, 0x13C2F: MEDIUM,  # low-detail owls
    0x13C31: MEDWIDE,
    0x13C3A: EVERYBIT,
    0x13CBF: MEDWIDE, 0x13CC0: MEDWIDE,  # sitting falcons
    0x13CCF: NORMAL,  # short-tailed bird
    0x13CE1: MEDIUM, 0x13CE2: MEDWIDE,  # ducks
    0x13CF7: MEDWIDE, 0x13CF8: EVERYBIT,  # lapwings
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
    0x13E64: SMALL,
    0x13E85: MEDIUM,
    0x13EA3: SMALL, 0x13EA4: SMALL,
    0x13EA8: MEDWIDE,
    0x13EAB: SMALL,
    0x13EB8: MEDWIDE,  # vine on trellis
    0x13EBE: MEDIUM,
    0x13EE7: MEDWIDE, 0x13EE8: MEDWIDE,
    0x13EF5: TINY,
    0x13F44: MEDIUM, 0x13F45: MEDIUM,  # wells
    0x13F47: MEDIUM,
    0x13F4B: MEDIUM,
    0x13F50: MEDWIDE, 0x13F51: MEDWIDE, 0x13F52: MEDWIDE, # building plans
    0x13F74: WIDE, # overthrow wall, not really enough width
    0x13F97: MEDWIDE,  # portico with 7 uraei
    0x13FD7: MEDWIDE,
    0x13FEF: EVERYBIT,  # gate of lions
    0x14004: MEDWIDE,
    0x14015: MEDIUM,  # threshing floor + village
    0x14016: MEDIUM,  # another version of threshing floor
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
    0x14324: WIDE,
    0x14326: MEDIUM, 0x14327: MEDIUM, 0x14328: SMALL, 0x14329: MEDWIDE,
    0x1432A: COILED_ROPE, 0x1432B: COILED_WIDE, 0x1432C: COILED_ROPE,
    0x14356: SMALL,
    0x14369: MEDIUM, 0x1436A: MEDIUM,
    0x1436D: MEDIUM,
    0x1437C: MEDWIDE, 0x1437D: WIDE, 0x1437E: WIDE,  # water racks
    0x1438B: MEDWIDE,
    0x1438D: MEDIUM, 0x1438E: SMALL, 0x1438F: MEDIUM,
    0x14392: MEDIUM,
    0x14394: MEDIUM,
    0x14399: MEDIUM, 0x1439C: MEDIUM, 
    0x1439F: MEDIUM, 0x143A0: MEDIUM, 0x143A1: MEDIUM,
    0x143A7: MEDIUM,
    0x143AE: MEDIUM,
    0x143B4: MEDIUM,
    0x143BA: MEDIUM,
    0x143BC: MEDIUM,
    0x143C0: MEDIUM,
    0x143CA: MEDIUM, 0x143CB: LOAF, 0x143CC: LOAF,  # loafs
    0x143CE: SMALL,
    0x143D4: MEDWIDE, 0x143D5: MEDIUM,
    0x143DF: MEDIUM, 0x143E0: SMALL, # empty circles
    0x143EE: MEDIUM,
    0x143F5: WIDE,
}

# Brackets: they are taller than normal chars and have different bearings
BRK_OPEN = 1
BRK_CLOSE = -1

BRK_INFO = {
    0x13258: BRK_OPEN  * 280, 0x13259: BRK_OPEN  * 200, 0x1325A: BRK_OPEN  * 200,  # walls 1
    0x1325B: BRK_CLOSE * 200, 0x1325C: BRK_CLOSE * 200, 0x1325D: BRK_CLOSE * 280,  # walls 2
    0x13286: BRK_OPEN * 180,  0x13287: BRK_CLOSE * 180,  # round fortified walls
    0x13288: BRK_OPEN * 280,  0x13289: BRK_CLOSE * 280,  # square fortified walls
    0x13379: BRK_OPEN,  0x1337A: BRK_CLOSE, 0x1337B: BRK_CLOSE,  # cartouches
    0x1342F: BRK_OPEN,  # leftover cartouche
}

def glyphSize(cp):
    if cp in GLYPH_SIZES:
        return GLYPH_SIZES[cp]
    if cp in BRK_INFO:
        return BRKHEIGHT
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

def fixBearings(glyph, cp):
    leftBearing = BEARING
    rightBearing = BEARING
    # Squeezing every bit → smaller bearings
    if cp in GLYPH_SIZES:
        if GLYPH_SIZES[cp] < WIDE:
            leftBearing = SMALL_BEARING
            rightBearing = SMALL_BEARING
    # Have brackets → take from table
    if cp in BRK_INFO:
        what = BRK_INFO[cp]
        if what > 0:
            rightBearing = -what
        elif what < 0:
            leftBearing = what
    glyph.left_side_bearing = leftBearing
    glyph.right_side_bearing = rightBearing
    
# @return [+] OK
def improveGlyph(glyph, cp, logBad):
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
    # Hint
    glyph.foreground = fg
    fixBearings(glyph, cp)
    selfInter = glyph.selfIntersects()
    isOk = False
    if not selfInter:
        glyph.correctDirection()
        isOk = True
    elif logBad:
        nSelfIntersecting += 1
        log.write("CRIT: {} self-intersects, {} so far\n".format(
                glyph.glyphname, nSelfIntersecting))
    #glyph.removeOverlap()
    # Correct direction
    return isOk

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
    if (actualWidth < 220) and (actualHeight < 220):
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
    return improveGlyph(glyph, cp, logBad)
    
def newGlyph(font, code, glyphName):
    glyph = font.createChar(code)
    glyph.glyphname = glyphName
    return glyph

# Load glyph manually optimized by user
def loadManualGlyph(font, code, glyphName, fname, logName):
    glyph = newGlyph(font, code, glyphName)
    glyph.importOutlines(fname, scale=False, correctdir=True)
    glyph.simplify(SMALLSIMPVALUE, ['mergelines'])
    fixBearings(glyph, code)
    if glyph.selfIntersects():
        log.write("CRIT: {} is {}, and still self-intersects!\n".format(glyph.glyphname, logName))

def getManualName(dirName, glyphName):
    return "{}/{}_UnicodiaSesh.svg".format(dirName, glyphName)
    
# Load glyph created from scratch by user
# The glyph is surely flat, do not run InkScape
def loadMyGlyph(font, sHex, cp, svgName):
    newGlyphName = "u{}".format(sHex.upper())
    glyph = newGlyph(font, cp, newGlyphName)
    newSvgHeight = getSvgHeight(svgName)
    loadGlyph(glyph, cp, svgName, newSvgHeight, True)

def checkLowPriority(fname):
    if os.path.exists(fname):
        log.write("WARN: {} exists!\n".format(fname))

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
                                extensionName = "svg-ex/{}.svg".format(sHex)
                                svgName = "svg-my/{}.svg".format(sHex)
                                if os.path.exists(svgName):
                                    checkLowPriority(extensionName)
                                    loadMyGlyph(font, sHex, code, svgName)
                                elif os.path.exists(extensionName):
                                    loadMyGlyph(font, sHex, code, extensionName)
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
                            extensionName = "svg-ex/{}.svg".format(sHex)
                            reallyMyName = "svg-my/{}.svg".format(sHex)
                            # Load?
                            isLoaded = True
                            if os.path.exists(reallyMyName):
                                checkLowPriority(manualName)
                                checkLowPriority(extensionName)
                                checkLowPriority(svgRemadeName)
                                checkLowPriority(cacheName)
                                loadMyGlyph(font, sHex, code, reallyMyName)
                            elif os.path.exists(manualName):
                                # Manual glyph
                                checkLowPriority(extensionName)
                                checkLowPriority(svgRemadeName)
                                checkLowPriority(cacheName)
                                loadManualGlyph(font, code, glyphName, manualName, 'manual')
                            elif os.path.exists(extensionName):
                                checkLowPriority(svgRemadeName)
                                checkLowPriority(cacheName)
                                loadMyGlyph(font, sHex, code, extensionName)
                            elif os.path.exists(svgRemadeName):
                                glyph = newGlyph(font, code, glyphName)
                                svgHeight = getSvgHeight(svgRemadeName)
                                checkLowPriority(cacheName)
                                loadGlyph(glyph, code, svgRemadeName, svgHeight, True)
                            elif os.path.exists(cacheName) and not isKnownBadGlyph:
                                # Cached glyph: already ran software
                                glyph = newGlyph(font, code, glyphName)
                                svgHeight = getSvgHeight(cacheName)
                                loadGlyph(glyph, code, cacheName, svgHeight, True)
                            elif not isKnownBadGlyph:
                                # Unknown glyph
                                glyph = newGlyph(font, code, glyphName)
                                svgHeight = getSvgHeight(svgName)
                                isGood = loadGlyph(glyph, code, svgName, svgHeight, False)
                                if not isGood:
                                    # Run Inkscape
                                    log.write("NOTE: Forced to run Inkscape on {}.\n".format(glyphName))
                                    if svgHeight > 200:
                                        # leave
                                        cmdline = '"{}" --actions=select-all;path-union;fit-canvas-to-selection --export-filename={} {}'
                                        os.system(cmdline.format(INKSCAPE, cacheName, svgName))
                                    else:
                                        # enlarge
                                        cmdline = '"{}" --actions=select-all;transform-scale:40;path-union;fit-canvas-to-selection --export-filename={} {}'
                                        os.system(cmdline.format(INKSCAPE, cacheName, svgName))
                                    glyph.clear()
                                    svgHeight = getSvgHeight(cacheName)
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
