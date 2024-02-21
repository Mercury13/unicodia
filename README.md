# What is Unicodia?

It is a simple Unicode encyclopedia and replacement for Character Map. Right now Windows only.

**Lifecycle phase: 5** (production/stable). Minor troubles with sustainability, but generally survived two releases of Unicode.

**I’m in Ukraine torn with war, so I’ll release often.** See “war release” tag in Issues.

# How to build?
* Slight C++20 and std::filesystem here → so need either MSYS or recent Qt with MinGW 11.
* Also need cURL (present in W10 18H2+), [7-zip](https://7-zip.org), [UTransCon](https://github.com/Mercury13/utranslator).
* Configure and run tape.bat file.
* Configure and run rel.bat file.

# How to develop?
* Download/find all tools mentioned above.
* Ensure that your Qt has MinGW 11+. Or make a custom kit of some MinGW Qt and MSYS compiler; tested Qt 6.1.2, 6.1.3, 6.2.0, 6.2.4. I actually use 6.1.3 as the last supporting W7 and fast enough on W10.
* Compile and run AutoBuilder.
* Move UcAuto.cpp to Unicodia/Uc.
* Compile Unicodia.

## If you want to rebuild GlyphWiki font
(This section is being written)

# Compatibility and policies

## Platforms
**Win7/10/11 x64 only.** Rationale:
* WXP, WVista and W8 are completely abandoned by all imaginable software. Though I did some improvements specially for W8.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* Recently checked Windows 11, and it works.

## Tofu/misrenderings
* **W10/11 should support everything possible, W7 base plane only**.
* Some base plane scripts (e.g. Georgian Nuskhuri) will not be shown the best way in W7, but will be shown.
* Of course, same for plane 1 scripts: if I find that some font supports e.g. Lycian, I’ll include it to font chain.
* Three scripts of plane 1 are considered “extremely important” and **will** be supported in W7: Phoenician, Aramaic, Brahmi. I’ll also support Gothic, just because its serif look is really “gothic”.
* Small misrenderings in descriptions are tolerable, I’ll fix them only if samples are bad, or if the font has other problems.

## Update Unicode
Wartime: as soon as beta version arrives.

Peacetime (probably): as soon as some big font covering major non-Han set arrives.

Emergency releases of one or two characters (e.g. currency, Japanese era): instantly, then think about their images.

## Fonts
Update to beta: only if fixes major misrender and/or professionally implements new character.

Naming: Noto if tables and existing glyphs are surely untouched; Uto otherwise.

## Language

**English.** International: truck > lorry, petrol > gas. Prefer British form if both are good.

**Russian.** Ё is mandatory. No war jargon. No grammatical concessions to Ukrainian. Describe 2022 war as neutral as possible.

**Ukrainian.** Avoid 2019 reform, earlier changes (e.g. frequent Ґ in loanwords) are embraced. No war jargon. Describe topics sensitive to nationalists as delicately as possible.

## Future functionality
* Finish GlyphWiki loader.
* Better CJK reference.

# What do utilities do?
* AutoBuilder — build UcAuto.cpp from Unicode base.
  * **Warning**: transition to older/newer Unicode requires a bit of handwork.
* GwLoader — loads glyphs from GlyphWiki
* GwRemake — does an auto-remake of GlyphWiki
* PanoseTool — early tool that used to remove fonts’ declared script support. Left for history. Current Unicodia uses custom font loading code based on PanoseTool + font matching flags.
* SmartCopy — copies the file if it actually differs.
* TapeMaker — creates a tape file for graphic emoji.
* [UTranslator](https://github.com/Mercury13/utranslator) — translation tool.

# Unicodia emoji style guide
If you decide to make a new character, there are two decisions available: 1) Mimic existing style; 2) Use new style. If you decide the latter (right now just three emoji have new style, placard and pushing hands)…

I discovered that most of professional emoji… do not work! Do not work because they have too much detail and too little contrast. The last professional emoji that really worked is… 2018 Microsoft!

**High priority:** emoji that are really badly grasped.

**Canvas size:** 64×64. For oblong/round object: leave 2px from edge. For square object: leave 6px from edge. For faces: leave 7px from edge.

**Colour scheme:** GNOME Human Interface Guidelines unless REALLY needed (e.g. military helmet). Use plain colours. Use black #000 and white #FFF sparingly, mainly for outlines and small details.

**Projection:** frontal/sideways unless the object is better identified in other projection. Bilateral animals, vehicles and people face to the left.

**Style:** avoid toyish look. Machinery should be based on real-life models.

**Outline style:** outer 2px, the darkest colour of palette (unless outlining grey/white/luminous thing). Inner may be thinner if present. Butts are round for natural objects and square/slanted for boxy ones. Same for corners.

**Outline presence:** outer may be partly omitted to gain more dynamic look; the object should continue from the OUTER edge of outline (like in OpenMoji). Inner lines are mostly absent. White things are outlined more aggressively. Truncation lines are never outlined. You may omit outline of small outer details (e.g. hooves); but if they really define object’s shape, it’s better to outline them at least partly, at least with 1px. 

**Grid:** 1px; extremum nodes are snapped.

## Flags
These rules are intended to work around renderer deficiencies. Emoji are intended to be drawn in small size, and rendering SVG leaves half-pixel lines. Also white and black are main colours everywhere.

* Joining fields:
  * Fields always overlap to at least 1/16 width. Example (England): you just draw a white rectangle, and a cross over it.
  * When fields are large, that overlap should shun the edge: Example (Ukraine, Poland and other bicolours): one half is a rectangle, the other is a hexagon, with two vertices making that overlap.
* Colour scheme:
  * White is pure white FFF, black is Twitter black 141414.
* White fields adjacent to background:
  * Small (Norway, US): just white
  * Medium, both ends are of the same colour (Argentina, Oman): outline of that colour
  * Medium, ends have different colour (France, Luxembourg): grey outline
  * Large (England, Slovakia): also grey outline
  * Difference: small = white field makes a negative space rather than a hole; large = coloured outline makes too much visual noise

# Hidden commands
* Ctrl+T — tofu stats
* F12 — reload translation from disk. Locale does NOT reload
* Ctrl+F12 — dump Library tile info to opt.xml, for access optimization (1st chunk contains all tiles)
