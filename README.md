# What is Unicodia?

It is a simple Unicode encyclopedia and replacement for Character Map.
* Right now Russian only.
* Right now Windows only.

# How to compile?
* Download MSYS, slight C++20 here.
* Make custom kit of some MinGW Qt and MSYS compiler; tested Qt 6.1.2 to 6.2.2, though 6.2 branch does not work under W7.
* Compile AutoBuilder.
* Download and unzip big 220 M https://www.unicode.org/Public/14.0.0/ucdxml/ucd.all.flat.zip
* Run AutoBuilder.
* Move UcAuto.cpp to Unicodia/Uc.
* Compile Unicodia.

# Compatibility and policies

## Platforms
**Win7/10 x64 only.** Rationale:
* WXP, WVista and W8 are completely abandoned by all imaginable software.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* W11 will probably work, but there will be problems for two reasons.
  * Qt is a large wad of software, and something may go wrong (like Embarcadero VCL + Aero Flip)
  * No one checked fonts. Even in W7 and W10 they are different.

## Tofu/misrenderings
* **W10 should support everything possible, W7 base plane only**.
* Some base plane scripts (e.g. Georgian Nuskhuri) will not be shown the best way in W7, but will be shown.
* Of course, same for plane 1 scripts: if I find that some font supports e.g. Gothic, I’ll include it to font chain.
* Three scripts of plane 1 are considered “extremely important” and **will** be supported in W7: Phoenician, Aramaic, Brahmi.
* Small misrenderings in descriptions are tolerable, I’ll fix them only if samples are bad, or if the font has other problems.

## Future functionality
* Translate into English.
* Emoji tools: ligatures, more comprehensive reference.
* Better CJK reference.
* Sutton SignWriting tool.

# What do utilities do?
* AutoBuilder — build UcAuto.cpp from Unicode base.
  * '''Warning''': transition to older/newer Unicode requires a bit of handwork.
* PanoseTool — early tool that used to remove fonts’ declared script support. Left for history. Current Unicodia uses custom font loading code based on PanoseTool + font matching flags.
* [UcTranslator](https://github.com/Mercury13/utranslator) — incomplete translation tool. Data is still hardcoded into Unicodia.exe, but someday I’ll globalize it.
