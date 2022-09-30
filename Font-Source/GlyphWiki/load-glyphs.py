import fontforge

font=fontforge.activeFont()

file = open('hani-tofu.txt', 'r')

for line0 in file:
    line = line0.strip()
    code = int(line, base=16)
    svgName = "Raw/{}.svg".format(line)
    glyph = font.createChar(code)
    glyph.importOutlines(svgName, scale=False)
