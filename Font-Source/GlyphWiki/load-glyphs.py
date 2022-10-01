import fontforge
import psMat

# get transformation matrix
mat1 = psMat.translate(0, -600)
mat2 = psMat.scale(5)
mat3 = psMat.translate(0, -100)
mat = psMat.compose(mat1, mat2)
mat = psMat.compose(mat, mat3)

# get hieroglyphs
font=fontforge.activeFont()
file = open('hani-tofu.txt', 'r')

for line0 in file:
    line = line0.strip()
    code = int(line, base=16)
    svgName = "AutoRemade/{}.svg".format(line)
    glyph = font.createChar(code)
    glyph.importOutlines(svgName, scale=False)
    glyph.transform(mat, ("round"))
    glyph.width = 1000
