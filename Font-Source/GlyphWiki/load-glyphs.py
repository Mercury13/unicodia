import fontforge
import psMat

# get transformation matrix
mat1 = psMat.translate(0, -600)
mat2 = psMat.scale(5.3, 5.4)
mat3 = psMat.translate(-30, -150)
mat = psMat.compose(mat1, mat2)
mat = psMat.compose(mat, mat3)

font = fontforge.activeFont()

# Convert layers to cubic
font.is_quadratic = False

# import hieroglyphs
file = open('hani-tofu.txt', 'r')
for line0 in file:
    line = line0.strip()
    code = int(line, base=16)
    svgName = "AutoRemade/{}.svg".format(line)
    glyph = font.createChar(code)
    glyph.importOutlines(svgName, scale=False)
    glyph.transform(mat, ("round"))
    glyph.width = 1000

# Convert layers to quadratic
font.is_quadratic = True

# Hint glyphs
font.selection.all()
font.autoHint()
font.autoInstr()

font.generate('UnicodiaHan.ttf')
