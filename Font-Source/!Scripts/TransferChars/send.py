# Character transferring from file to file, SENDING part
import fontforge
import os

charsToSend = [ [ 0x2CA8, 0x2CF3 ], [ 0xFE24, 0xFE26], 0x0323 ]

# Converts charsToSend to set of Unicode CPs
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
    fg = glyph.layers[1]   # foreground layer
    fname = 'u' + format(code, 'X') + '.svg'
    print(fname + ' ' + str(code) + ' ' + str(wid), file=file, flush=True)
    fg.export(fname)


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