# Adding big umlaut, mid part
# Limitation: works with Unicode characters only, not ligatures
import fontforge
import os

# Two numbers = range of CPs, one number = a single CP
charsToAdd = [ [ 0x3E2, 0x3EF ], [ 0x2C80, 0x2CEE ], [ 0x2CF2, 0x2CF3 ] ]
anchorLeft = 'Biguml left'
anchorMid = 'Biguml mid'
anchorRight = 'Biguml right'
umlY = 790

# Expands charsToSend to set of Unicode CPs
def getCps():
    r = set()
    for v in charsToAdd:
        if type(v) is list:
            # add range
            for code in range(v[0], v[1] + 1):
                r.add(code)
        else:
            # add one
            r.add(v)
    return r


# Adds umlaut to glyph
def addUml(glyph):
    w = glyph.width
    glyph.addAnchorPoint(anchorLeft,  'base', 0,      umlY)
    glyph.addAnchorPoint(anchorMid,   'base', w // 2, umlY)
    glyph.addAnchorPoint(anchorRight, 'base', w,      umlY)
    

fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
for glyph in font.glyphs():
    code = glyph.unicode
    if code in cps:
        addUml(glyph)
