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
    # B
    'B_B' : SYLLABLE,
    'B_Y' : SYLLABLE,
    # Bh
    'Bh_Y' : SYLLABLE,
    # C
    'C_C' : SYLLABLE,
    'C_Ch' : SYLLABLE,
    # D
    'D_D' : SYLLABLE,
    'D_Dh' : SYLLABLE,
    'D_M' : SYLLABLE,
    'D_R' : SYLLABLE,
    'D_Y' : SYLLABLE,
    # Dd
    'Dd_Dd' : SYLLABLE,
    'Dd_Y' : SYLLABLE,
    # G
    'G_G' : SYLLABLE,
    'G_R' : SYLLABLE,
    'G_Y' : SYLLABLE,
    # H
    'H_M' : SYLLABLE,
    'H_Y' : SYLLABLE,
    # J
    'J_J' : SYLLABLE,
    'J_Ny' : SYLLABLE,
    # K
    'K_K' : SYLLABLE, 'K_K_R' : SYLLABLE,
    'K_T' : SYLLABLE, 'K_T_Y' : SYLLABLE, 'K_T_R' : SYLLABLE,        
    'K_Ss' : SYLLABLE, 'K_Ss_M' : SYLLABLE, 'K_Ss_M_Y' : SYLLABLE, 'K_Ss_R' : SYLLABLE,
    'K_R' : SYLLABLE,
    'K_V' : SYLLABLE,
    'K_Y' : SYLLABLE,
    # L
    'L_L' : SYLLABLE,
    'L_P' : SYLLABLE,
    'L_Y' : SYLLABLE,
    # M
    'M_M' : SYLLABLE,
    'M_P' : SYLLABLE,
    'M_Y' : SYLLABLE,
    # N
    'N_D' : SYLLABLE,
    'N_Dh' : SYLLABLE,
    'N_M' : SYLLABLE,
    'N_N' : SYLLABLE,
    'N_T' : SYLLABLE, 'N_T_N' : SYLLABLE, 'N_T_R' : SYLLABLE,
    'N_Y' : SYLLABLE,
    # Ng
    'Ng_G' : SYLLABLE,
    'Ng_K' : SYLLABLE,
    # Nn
    'Nn_Dd' : SYLLABLE,
    'Nn_Ddh' : SYLLABLE,
    'Nn_M' : SYLLABLE,
    'Nn_T' : SYLLABLE,
    'Nn_Tth' : SYLLABLE,
    'Nn_Y' : SYLLABLE,
    # Ny
    'Ny_C' : SYLLABLE,
    'Ny_Ch' : SYLLABLE,
    'Ny_J' : SYLLABLE,
    'Ny_Ny' : SYLLABLE,
    # P
    'P_P' : SYLLABLE,
    'P_R' : SYLLABLE,
    'P_Y' : SYLLABLE,
    # T
    'T_M' : SYLLABLE,
    'T_N' : SYLLABLE,
    'T_R' : SYLLABLE,
    'T_Th' : SYLLABLE,
    'T_V' : SYLLABLE,
    'T_Y' : SYLLABLE,
    # Tt
    'Tt_Y' : SYLLABLE,
    # V
    'V_Y' : SYLLABLE,
    # S
    'S_R' : SYLLABLE,
    'S_Th' : SYLLABLE,
    'S_V' : SYLLABLE,
    'S_Y' : SYLLABLE,
    # Sh
    'Sh_C' : SYLLABLE,
    'Sh_R' : SYLLABLE,
    'Sh_V' : SYLLABLE,
    'Sh_Y' : SYLLABLE,
    # Ss
    'Ss_Nn' : SYLLABLE,
    'Ss_Tt' : SYLLABLE, 'Ss_Tt_R' : SYLLABLE,
    'Ss_Tth' : SYLLABLE,
}

ST_EE = 'Link cons+Ee-1'      # Ee subtable
ST_AI = 'Link cons+Ai-1'      # Ai subtable

VOWELS = {
    'Ee'  : { 'Subt' : ST_EE },
    'Ai'  : { 'Subt' : ST_AI },
}

# Subjoined kern pairs, right glyphs
# (left are our consonants and ligatures)
KR_MAIN = { 'Main' : True }     # KR = kern right
KR_VOWEL = { 'Main' : False }
SUBJ_KERN_RIGHTS = {
    'Subj.Bh' : KR_MAIN, 'Subj.BhU' : KR_VOWEL, 'Subj.BhUu' : KR_VOWEL,
    'Subj.J' : KR_MAIN, 'Subj.JU' : KR_VOWEL, 'Subj.JUu' : KR_VOWEL,
    'Subj.Ny' : KR_MAIN, 'Subj.NyU' : KR_VOWEL, 'Subj.NyUu' : KR_VOWEL,
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

KERN_SUBTABLE = 'Autokern'

def addKern(font, myGlyph, addAll):
    myGlyph.removePosSub(KERN_SUBTABLE)
    for xcode, xprop in SUBJ_KERN_RIGHTS.items():
        if addAll or (xprop['Main']):
            rightGlyph = findGlyph(font, xcode, True)
            mywidth = min(myGlyph.width, rightGlyph.width)
            myGlyph.addPosSub(KERN_SUBTABLE, rightGlyph.glyphname, -mywidth)

BAD_ANCHORS = [ 'U', 'U mark', 'VocR', 'VocR mark' ]

def copyAnchors(destGlyph, srcGlyph, dx):
    for anc in srcGlyph.anchorPoints:
        clazz = anc[0]
        type = anc[1]
        x = anc[2]
        y = anc[3]
        if not (clazz in BAD_ANCHORS):
            destGlyph.addAnchorPoint(clazz, type, x + dx, y)

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
    addKern(font, baseGlyph, True)
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
    copyAnchors(ligGlyph, baseGlyph, baseOffset)
    addKern(font, ligGlyph, False)
    ligGlyph.width = baseOffset + baseGlyph.width
    ligGlyph.color = 0x4FD5FF

# Run, run!
fontforge.runInitScripts()
font = fontforge.activeFont()
for baseCode, baseProp in BASE_CHARS.items():
    for vowCode, vowProp in VOWELS.items():
        addLig(font, baseCode, baseProp, vowCode, vowProp)
