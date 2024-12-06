# Make pre-forms ligatures

CH_NORM = ''
NORMAL_CHAR = { 'Prefix': 'Tutg.', 'AutoComp': False }
SYLLABLE = { 'Prefix': 'Syl.', 'AutoComp': True }

BASE_CHARS = {
    'K'   : NORMAL_CHAR,
    'Kh'  : NORMAL_CHAR,
    'G'   : NORMAL_CHAR,
    'Gh'  : NORMAL_CHAR,
    'Ng'  : NORMAL_CHAR,
    'C'   : NORMAL_CHAR,
    'Ch'  : NORMAL_CHAR,
    'J'   : NORMAL_CHAR,
    'Jh'  : NORMAL_CHAR,
    'Ny'  : NORMAL_CHAR,
    'Tt'  : NORMAL_CHAR,
    'Tth' : NORMAL_CHAR,
    'Dd'  : NORMAL_CHAR,
    'Ddh' : NORMAL_CHAR,
    'Nn'  : NORMAL_CHAR,
    'T'   : NORMAL_CHAR,
    'Th'  : NORMAL_CHAR,
    'D'   : NORMAL_CHAR,
    'Dh'  : NORMAL_CHAR,
    'N'   : NORMAL_CHAR,
    'P'   : NORMAL_CHAR,
    'Ph'  : NORMAL_CHAR,
    'B'   : NORMAL_CHAR,
    'Bh'  : NORMAL_CHAR,
    'M'   : NORMAL_CHAR,
    'Y'   : NORMAL_CHAR,
    'R'   : NORMAL_CHAR,
    'L'   : NORMAL_CHAR,
    'V'   : NORMAL_CHAR,
    'Sh'  : NORMAL_CHAR,
    'Ss'  : NORMAL_CHAR,
    'S'   : NORMAL_CHAR,
    'H'   : NORMAL_CHAR,
    'Ll'  : NORMAL_CHAR,
    'Rr'  : NORMAL_CHAR,
    'Lll' : NORMAL_CHAR,
    'K_K' : SYLLABLE,
};

ST_EE = 'Link cons+Ee-1'      # Ee subtable
ST_AI = 'Link cons+Ai-1'      # Ai subtable

VOWELS = {
    'Ee'  : { 'Subt' : ST_EE },
    'Ai'  : { 'Subt' : ST_AI },
}


def findGlyph(font, name, shouldExist):
    for glyph in font.glyphs():
        if glyph.glyphname == name:
            return glyph
    if shouldExist:
        raise Exception("Char " + name + " should exist")
    return None


NEWGLYPH_NAME = ".New.Glyph"

#
#  Ensures that glyph is in the font, returns it
#  The sign of newly-created glyph is glyphName == NEWGLYPH_NAME
#     Change it ASAP
#
def ensureGlyph(font, name):
    glyph = findGlyph(font, name, False)
    if glyph == None:
        glyph = font.createChar(-1, NEWGLYPH_NAME)
    return glyph


def findAnchor(glyph, name):
    anchors = glyph.anchorPoints
    for p in anchors:
        if p[0] == name:
            return p
    return None


def addAnchor(baseGlyph, newGlyph, name, offset, minX):
    anchor = findAnchor(baseGlyph, name)
    if anchor != None:
        type = anchor[1]
        normalX = anchor[2] + offset
        x = max(normalX, minX)
        y = anchor[3]
        newGlyph.addAnchorPoint(name, type, x, y)


ST_WORKAR = 'Tutg workar-1'   # workaround subtable

#
#  Adds ligature:
#  basePrefix: smth like "Gukh." or "Shape."
#  baseCode : smth like "K" or "Kh"
#  baseProp : properties of base, e.g. which version of medial to insert
#  medCode : smth like "Y"
#  medProp : properties of medial glyph (to find proper place for virama)
#  shouldExist: [+] base character should exist, die with error
#               [-] skip it silently
#               (medial should exist, ligature is created if missing)
#
def addLig(font, baseCode, baseProp, vowCode, vowProp):
    baseName = baseProp['Prefix'] + baseCode
    vowName = 'Uml.' + vowCode
    ligName = 'Wa.' + baseCode + vowCode
    baseGlyph = findGlyph(font, baseName, True)
    vowGlyph = findGlyph(font, vowName, True)
    ligGlyph = ensureGlyph(font, ligName)
    if ligGlyph.glyphname == NEWGLYPH_NAME:
        ligGlyph.glyphname = ligName
        if baseProp['AutoComp']:
            # Prebuilt ligature
            # 1. Add ligature itself
            # Ligature: ligature ← components
            ligGlyph.addPosSub(ST_WORKAR, baseName + " " + vowName)
            # 2. Add by parts
            things = baseCode.split('_')
            baseComp = ''
            for x in things:
                if baseComp != '':
                    baseComp = baseComp + ' Uml.Conj '
                baseComp = baseComp + 'Tutg.' + x
            # Ligature: ligature ← components
            ligGlyph.addPosSub(ST_WORKAR, baseComp + " " + vowName)            
        else:
            # Simple ligature, just add
            # Ligature: ligature ← components
            ligGlyph.addPosSub(ST_WORKAR, baseName + " " + vowName)
        # Add 
        subt = vowProp['Subt']
        if subt != '':
            # Single sub: source → dest
            baseGlyph.addPosSub(subt, ligName)
    # Ligate vowel, then base
    ligGlyph.clear()
    ligGlyph.addReference(vowName)
    baseOffset = vowGlyph.width
    matrix = (1, 0, 0, 1, baseOffset, 0)    
    ligGlyph.addReference(baseName, matrix)
    ligGlyph.width = baseOffset + baseGlyph.width
    ligGlyph.color = 0x4FD5FF

# Run, run!
fontforge.runInitScripts()
font = fontforge.activeFont()
for baseCode, baseProp in BASE_CHARS.items():
    for vowCode, vowProp in VOWELS.items():
        addLig(font, baseCode, baseProp, vowCode, vowProp)
