# Build Sutton ligatures!

import fontforge
from enum import Enum

class CharType(Enum):
  NORMAL = 0,
  LIGATURE = 1

class FindResult:
  def __init__(self, glyph, type):
    self.glyph = glyph
    self.isFound = not(glyph is None)
    self.type = type

def findExactByName(lookup, name, isEx):
  if name in lookup:
    return FindResult(lookup[name], isEx)
  return FindResult(None, False)
  
def sgnwGlyphName(code, fill=1, rot=1, alt=''):
  name = "u{}{:X}".format(alt, code)
  if fill > 1:
    name = name + "_F{}".format(fill)
  if rot > 1:
    name = name + "_R{}".format(rot)
  return name

def findExactByCode(lookup, code, fill=1, rot=1):
  gname = sgnwGlyphName(code, fill, rot)
  r1 = findExactByName(lookup, gname, CharType.NORMAL)
  if r1.isFound:
    return r1
  gname = sgnwGlyphName(code, fill, rot, 'x')
  r1 = findExactByName(lookup, gname, CharType.LIGATURE)
  return r1

# Table: upper umlauts
upperUmlauts = set([0x1DA01, 0x1DA02, 0x1DA03, 0x1DA04, 0x1DA05, 0x1DA08, 0x1DA09, 0x1DA75])
# These umlauts are PARTLY upper
upperUmlautsAsText = 'u1DA07_F2 u1DA07_F2_R2'

# Start scripting
fontforge.runInitScripts()
font = fontforge.activeFont()

# build lookup dictionary
lookup = {}
for x in font.glyphs():
  lookup[x.glyphname] = x

logFile = open('x.log', 'w')

# go through Sutton table
for code in range(0x1D800, 0x1DA8B):
  wasSubtableAdded = False
  subtableName = "{:X}".format(code)
  mainName = sgnwGlyphName(code)
  print(mainName, file=logFile, flush=True)
  mainRes = findExactByName(lookup, mainName, CharType.NORMAL)
  if mainRes.isFound:
    for fill in range (1, 7):
      fillSuff = ""
      if fill > 1:
        fillSuff = " F{}".format(fill)
      for rot in range (1, 17):
        thatRes = findExactByCode(lookup, code, fill, rot)
        if fill != 1 or rot != 1:
          rotSuff = ""
          if rot > 1:
            rotSuff = " R{}".format(rot)
          if thatRes.isFound:
            if not wasSubtableAdded:
              font.addLookupSubtable("Sgnw liga", subtableName)
              wasSubtableAdded = True
            glyphComposition = mainName + fillSuff + rotSuff
            if thatRes.type == CharType.LIGATURE:
              thatRes.glyph.addPosSub(subtableName, 'uni25CC ' + glyphComposition)
              thatRes.glyph.addPosSub(subtableName, 'u1D9FF ' + glyphComposition)
              thatRes.glyph.addPosSub(subtableName, 'u1D9FF_F5 ' + glyphComposition)
            thatRes.glyph.addPosSub(subtableName, glyphComposition)
        if thatRes.isFound and thatRes.type == CharType.NORMAL:
          if code in upperUmlauts:
            upperUmlautsAsText = upperUmlautsAsText + ' ' + thatRes.glyph.glyphname

# IDK how to modify upper umlaut lookup → do it manually
print('>> UPPER UMLAUTS HERE', file=logFile)
print(upperUmlautsAsText, file=logFile, flush=True)
