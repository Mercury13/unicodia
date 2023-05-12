# Build Sutton ligatures!

import fontforge

def findExactByName(lookup, name):
  if name in lookup:
    return lookup[name]
  return None
  
def sgnwGlyphName(code, fill=1, rot=1, alt=''):
  name = "u{}{:X}".format(alt, code)
  if fill > 1:
    name = name + "_F{}".format(fill)
  if rot > 1:
    name = name + "_R{}".format(rot)
  return name

def findExactByCode(lookup, code, fill=1, rot=1):
  gname = sgnwGlyphName(code, fill, rot)
  r1 = findExactByName(lookup, gname)
  if not (r1 is None):
    return r1
  gname = sgnwGlyphName(code, fill, rot, 'x')
  r1 = findExactByName(lookup, gname)
  return r1

# Start scripting
fontforge.runInitScripts()
font = fontforge.activeFont()

# build lookup dictionary
lookup = {}
for x in font.glyphs():
  lookup[x.glyphname] = x

logFile = open('x.log', 'w')

for code in range(0x1D800, 0x1DA8B):
  wasSubtableAdded = False
  subtableName = "{:X}".format(code)
  mainName = sgnwGlyphName(code)
  print(mainName, file=logFile, flush=True)
  mainGlyph = findExactByName(lookup, mainName)
  if not (mainGlyph is None):
    for fill in range (1, 7):
      fillSuff = ""
      if fill > 1:
        fillSuff = " F{}".format(fill)
      for rot in range (1, 17):
        if fill != 1 or rot != 1:
          rotSuff = ""
          if rot > 1:
            rotSuff = " R{}".format(rot)
          thatGlyph = findExactByCode(lookup, code, fill, rot)
          if not (thatGlyph is None):
            if not wasSubtableAdded:
              subtable = font.addLookupSubtable("Sgnw liga", subtableName)
              wasSubtableAdded = True
            thatGlyph.addPosSub(subtableName, mainName + fillSuff + rotSuff)
