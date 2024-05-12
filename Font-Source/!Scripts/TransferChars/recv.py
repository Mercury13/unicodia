# Character transferring from font to font, RECEIVING part
# Limitation: works with Unicode characters only, not ligatures
import fontforge
import os

fontforge.runInitScripts()
font = fontforge.activeFont()
# Finally open file
file = open('send.txt', 'r')
while True:
    # Read line, check for content
    line = file.readline()
    if not line:
        break
    # check what read
    # File format: fname → code → width
    cells = line.split('\t')
    if len(cells) < 3:
        continue      
    fname = cells[0]
    code = int(cells[1])
    width = int(cells[2])
    # Find or create codepoint
    glyph = font.createChar(code)
    glyph.glyphname = "u" + format(code, 'X')
    glyph.importOutlines(fname, scale=False)
    glyph.width = width    
# Close file
file.close()
