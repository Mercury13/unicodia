# Character transferring from font to font, SENDING part
# Limitation: works with Unicode characters only, not ligatures
import fontforge
import os

# Two numbers = range of CPs, one number = a single CP
charsToSend = [ [ 0x21, 0x40 ], 0x5C, [ 0x5E, 0x60 ], [ 0x7B, 0x7E ], 0xA0, 0x374, 0x37E, 0x384, 0x387 ]

# Expands charsToSend to set of Unicode CPs
def getCps():
    r = set()
    for v in charsToSend:
        if type(v) is list:
            # add range
            for code in range(v[0], v[1] + 1):
                r.add(code)
        else:
            # add one
            r.add(v)
    return r


# Sends one character, writes its info to file
def sendChar(file, glyph, code):
    wid = max(glyph.width, 0)
    fname = 'u' + format(code, 'X') + '.svg'
    # TSV: fname → code → width
    print(fname + '\t' + str(code) + '\t' + str(wid), file=file, flush=True)
    glyph.export(fname)


fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
# Finally open file
file = open('send.txt', 'w')
for glyph in font.glyphs():
    code = glyph.unicode
    if code in cps:
        sendChar(file, glyph, code)
# close file
file.close()