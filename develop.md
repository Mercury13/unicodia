# How to develop?
* Download/find all tools for building.
* Ensure that your Qt has MinGW 11+. Or make a custom kit of some MinGW Qt and MSYS compiler; tested Qt 6.1.3, 6.7.0. I actually use 6.1.3 as the last supporting W7 and fast enough on W10.
* Compile and run AutoBuilder.
* Move UcAuto.cpp to Unicodia/Uc.
* Compile Unicodia.
* Then you need to recreate working directory, symlinking all files/folders existing in original Unicodia.
* Congratulations! You can now run Unicodia directly from Qt Creator!

# Hidden commands
* Ctrl+T — tofu stats
* F12 — reload translation from disk. Locale does NOT reload
* Ctrl+F12 — dump Library tile info to opt.xml, for access optimization. After placing it into NotoEmoji and running tape.bat the first chunk of emoji.zip will contains all emoji needed for tiles
* Ctrl+Shift+Q — test emoji repainting engine

# How to update emoji?
* Simple emoji (no skintones)
  * Ensure that it has no &lt;clipPath&gt;, edit if needed.
    * Originals is directory for original SVGs, Remakes if for remade but unoptimized
  * It’s better to run [SvgCleaner](https://github.com/RazrFalcon/svgcleaner) on it
  * If emoji-related library data changed → copy opt.xml from AutoBuilder to NotoEmoji
  * Make sure **Unicodia is closed**, run tape.bat
* Two or more skintones (e.g. interracial kiss/handshake): same. They are neither repainted nor checked.
* One skintone (one person, same-race handshake…), repainted versions are stored
  * After that press Ctrl+Shift+Q in Unicodia.
  * Copy resulting BMP files to MiscFiles\SkinToneQa. You can do it manually, or using “Good” button in QaCompareBmp tool.
* One skintone, repainted versions are created on-the-fly (e.g. because of &lt;clipPath&gt;)
  * So-called “preparation”: we need to remember what was originally
    * First copy ORIGINAL files with all skintones to NotoEmoji
	* It’s better to run SvgCleaner
    * Make sure Unicodia is closed, run tape.bat
    * Press Ctrl+Shift+Q in Unicodia.
    * Copy resulting BMP files to MiscFiles\SkinToneQa. You can do it manually, or using “Good” button in QaCompareBmp tool.
  * Then normal work commences.
    * It’s absolutely needed to change **nose colour** E59600 → E49600, to prevent confusion with ears
    * Remove original files, copy your remake.
    * It’s absolutely needed to run SvgCleaner, repainting engine relies on it (e.g. lowercase, XML attributes rather than CSS)
    * Make sure Unicodia is closed, run tape.bat
    * Press Ctrl+Shift+Q in Unicodia.
    * Check what’s different using QaCompareBmp. If some colour is repainted erroneously (teacher/blackboard, fireman/insignia) → change colour a bit and repeat
    * If you are satisfied with difference → press “Good” button in QaCompareBmp.

# How to update to next Unicode
* Go to ``MiscFiles/RawData``, modify ``load.bat`` and load bases of the next Unicode
* Make ``AutoBuilder`` run (who knows what will be added to the next bases)
  * Warning, while Unicode is alpha/beta, AutoBuilder may contain manual patches: at the time of writing it’s #349
* Run it
* Copy all ``UcAuto*`` to ``Unicodia/Uc``
* Make Unicodia compile, first of all add new Unicode version (``EcVersion``), write manually new script/block data (``UcData``/``UcBlocks.cpp``)
  * While the block is full of tofu, add ``Ifg::MISSING`` flag to synthIcon field
  * Now Unicodia runs somehow!!
* Non-compulsory data:
  * Texts
    * Run UTranslator, open ``en.uorig``
    * Write additional data Unicodia may run w/o: first of all script/block textual info
      * Better place the text in order using commands “Move up/Move down”
    * If you know languages, translate to Russian/Ukrainian using the same UTranslator
      * File → Update data, Go → Find warnings → All, and close those changes
    * If some blocks should be logically near, but specific language makes a mess (Greek → Ancient Greek numbers), add an ellipsis to the beginning using ``locale.xml``.
      * This ellipse will automatically go if you sort blocks by code
  * Block extension history:
    * Add the new version to BlockExtensionHistory
    * Run it, check if the block was extended/shrunk
    * Write that to ``UcBlocks.cpp``
      * Sample: during U2…16 three blocks were extended (Tibt, Ahom, Egyp format) and one shrunk (Tang supp)
  * Decapitalization rules: “LATIN CAPITAL LETTER A” → “Latin capital letter A”
    * They are in ``AutoBuilder/data.cpp``
	* See Decapitalization rules below
    * Of course, run AutoBuilder
    * Always compare before and after with some diff tool! Heal one thing and hurt another — that’s frequent!
    * Copy CPP files again
  * Block icons
    * There are two ways to make a block icon, SVG and programmatic. I’ll talk about the former, default
    * 16×16 px, scale 1. Put them to ``Resources/Scripts``, optimize somehow, write a new filename to ``scripts.qrc``
    * Go to UcBlocks.cpp. Write in ``synthIcon`` field what characters (1 or 2) you drew and how to align that picture to pixels on HiDPI
    * Open your new Unicode, clock “Find” icon
    * It’ll show SYNTHESIZED icon, not small 16×16. Make that icon in harmony with 16×16 using ``synthIcon`` field
  * Placeholders for emoji: see above
  * Fonts
    * Find somehow, write in ``UcData.h``/``UcFonts.cpp``
	* There are two ways to write a font, tie to script or tie to block. Block is stronger, and I tie to script if possible
    * Modify block icon to permanent, see above
  * GlyphWiki’s placeholder of CJK fonts
    * (To be written)
  * Latin in Library
    * AutoBuilder has some sort of forget checker, and when the tofu is filled, you may write it to ``MiscFiles/library.xml`` and reduce number of forgotten characters
* When Unicode transitions from beta to release, go to ``MiscFiles/RawData`` and change addresses to final ones

# Decapitalization rules
* Example one: “Garay capital letter Ca”
  * You should turn script mode on (it’ll automatically capitalize CA using keyword LETTER), and need one rule “GARAY”, flag TRIG_SCRIPT
* Example two: “Myanmar Eastern Pwo Karen digit Zero”
  * Myanmar/Karen/Zero already work, but need to do something with “Eastern Pwo”, and need two-word script EASTERN+PWO
* Example three: “Ol Onal letter O”
  * Already got two-word script “Ol Chiki”, but it’s possible to add “Ol Onal” to the same dictionary entry (AFAIR, up to 3 or 4 options)
* Example four: “Tulu-Tigalari letter A”
  * If you just write as in example 1, it’ll automatically capitalize to “Tulu-tigalari”, and need a custom capitalization “Tulu-Tigalari”
* Example five: “Tulu-Tigalari Vedic tone Svarita”
  * You may do something with common rules, but I think it’s easier to coin an exception (keyword TONE is overloaded)
* Example six: “Garay vowel length mark”
  * VOWEL is ofter a keyword (compare “Klingon vowel Foo”), but here it is NOT → just write an exception
* Example seven: “logic gate Or”
  * That’s a really special case, no point to write rules and hurt something now or in the future → exception!

# If you want to rebuild GlyphWiki font
(This section is being written)

# What do utilities do?
* AutoBuilder — build UcAuto.cpp from Unicode base.
  * **Warning**: transition to older/newer Unicode requires a bit of handwork.
* BlockExtensionHistory: 
* GwLoader — loads glyphs from GlyphWiki
* GwRemake — does an auto-remake of GlyphWiki
* PanoseTool — early tool that used to remove fonts’ declared script support. Left for history. Current Unicodia uses custom font loading code based on PanoseTool + font matching flags.
* QaCompareBmp — to check what changed in single-skintone emoji (e.g. changed repainting engine, stored→repainted)
* SmartCopy — copies the file if it actually differs.
* TapeMaker — creates a tape file for graphic emoji.
* [UTranslator](https://github.com/Mercury13/utranslator) — translation tool.

# Unicodia emoji style guide
If you decide to make a new character, there are two decisions available: 1) Mimic existing style; 2) Use new style. If you decide the latter (right now just three emoji have new style, placard and pushing hands)…

I discovered that most of professional emoji… do not work! Do not work because they have too much detail and too little contrast. The last professional emoji that really worked is… 2018 Microsoft!

**High priority:** emoji that are really badly grasped.

**Canvas size:** 64×64. For oblong/round object: leave 2px from edge. For square object: leave 6px from edge. For faces: leave 7px from edge.

**Colour scheme:** GNOME Human Interface Guidelines unless REALLY needed (e.g. military helmet). Use plain colours. Use black #000 and white #FFF sparingly, mainly for outlines and small details. Hands/people use Noto’s colours for skin.

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
