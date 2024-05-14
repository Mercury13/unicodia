# Adding big umlaut, mid part
# Limitation: works with Unicode characters only, not ligatures
import fontforge
import os

# Two numbers = range of CPs, one number = a single CP
charsToAdd = [ [ 0x3E2, 0x3EF ], [ 0x2C80, 0x2CEE ], [ 0x2CF2, 0x2CF3 ] ]
anchorName = 'Biguml mid'
umlY = 770

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
def deleteAnchor(glyph):
    x = glyph.width // 2
    glyph.addAnchorPoint(anchorName, 'base', x, umlY)
    

fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
for glyph in font.glyphs():
    code = glyph.unicode
    if code in cps:
        deleteAnchor(glyph)
