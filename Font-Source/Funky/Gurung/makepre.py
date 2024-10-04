# Make pre-forms ligatures

CH_NORM = ""
CH_LOW = ".Low"
CH_SLANT = ".Slant"
NORMAL_CHAR = { "Type": CH_NORM }
LOW_CHAR = { "Type": CH_LOW }
SLANT_CHAR = { "Type": CH_SLANT }

SCRIPT = "Gukh"

BASE_PREFIX = SCRIPT + "."
BASE_CHARS = {
    "K" : NORMAL_CHAR,
    "G" : LOW_CHAR,
    "Ng" : LOW_CHAR,
    "C" : LOW_CHAR,
    "J" : NORMAL_CHAR,
    "H" : SLANT_CHAR,
    "Tt" : LOW_CHAR,
    "Dd" : LOW_CHAR,
    "V" : SLANT_CHAR,
    "T" : NORMAL_CHAR,
    "D" : NORMAL_CHAR,
    "N" : NORMAL_CHAR,
    "P" : NORMAL_CHAR,
    "B" : SLANT_CHAR,
    "M" : NORMAL_CHAR,
    "Y" : SLANT_CHAR,
    "R" : SLANT_CHAR,
    "L" : NORMAL_CHAR,
    "S" : NORMAL_CHAR,
};

MEDIAL_PREFIX = SCRIPT + ".Med."
MEDIAL_CHARS = {
    "Y" : { },
    "V" : { },
}

LIGATURE_PREFIX = SCRIPT + ".Ligm."


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
def addLig(font, basePrefix, baseCode, baseProp, medCode, medProp, shouldExist):
    baseName = basePrefix + baseCode
    medName = MEDIAL_PREFIX + medCode + baseProp["Type"];
    ligName = LIGATURE_PREFIX + baseCode + "." + medCode
    baseGlyph = findGlyph(font, baseName, shouldExist)
    if baseGlyph == None:
        return
    medGlyph = findGlyph(font, medName, True)
    ligGlyph = ensureGlyph(font, ligName)
    if ligGlyph.glyphname == NEWGLYPH_NAME:
        ligGlyph.glyphname = ligName
        ligGlyph.addPosSub("Gukh LIGM-1", baseName + " " + medName)
    # Ligate medial, then base
    ligGlyph.clear()
    ligGlyph.addReference(medName)
    baseOffset = medGlyph.width
    matrix = (1, 0, 0, 1, baseOffset, 0)    
    ligGlyph.addReference(baseName, matrix)
    ligGlyph.width = baseOffset + baseGlyph.width
    # Add anchors
    addAnchor(baseGlyph, ligGlyph, "Matra 625", baseOffset, 0)
    addAnchor(medGlyph, ligGlyph, "Virama", 0, 0)


fontforge.runInitScripts()
font = fontforge.activeFont()
for baseCode, baseProp in BASE_CHARS.items():
    for medCode, medProp in MEDIAL_CHARS.items():
        addLig(font, BASE_PREFIX, baseCode, baseProp, medCode, medProp, True)
        # K and Kh have the same properties, but Kh doesn’t need to exist
        addLig(font, BASE_PREFIX, baseCode+"h", baseProp, medCode, medProp, False)

# Add dotted circle
for medCode, medProp in MEDIAL_CHARS.items():
    addLig(font, "Shape.", "Dotc", NORMAL_CHAR, medCode, medProp, True)
