# Character transferring from font to font, SENDING part
# Limitation: works with Unicode characters only, not ligatures
import fontforge
import os

# Two numbers = range of CPs, one number = a single CP
charsToSend = [ [ 0x2CA8, 0x2CF3 ], [ 0xFE24, 0xFE26], 0x0323 ]

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
    iSlot = font.findEncodingSlot(code)
    if iSlot < 0:
        raise Exception('Character not found')
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