import fontforge

font=fontforge.activeFont()

file = open('hani-tofu.txt', 'r')
lines = file.readlines()

for line0 in lines:
    line = line0.strip()
    code = int(line, base=16)
    svgName = "Raw/{}.svg".format(line)
    glyph = font.createChar(code)
    glyph.importOutlines(svgName, scale=False)
