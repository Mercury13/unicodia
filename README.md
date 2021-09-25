# What is Unicodia?

It is a simple Unicode encyclopedia and replacement for Character Map.
* Right now Russian only.
* Right now deep WIP
* Right now Windows only.

# How to compile?
* Download MSYS, slight C++20 here.
* Make custom kit of some MinGW Qt and MSYS compiler; tested Qt 6.1.2.
* Compile AutoBuilder.
* Download and unzip big 220 M https://www.unicode.org/Public/13.0.0/ucdxml/ucd.all.flat.zip
* Run AutoBuilder.
* Move UcAuto.cpp to Unicodia/Uc.
* Compile Unicodia.

# Compatibility and policies

## Platforms
'''Win7/10 x64 only.''' Rationale:
* WXP, WVista and W8 are completely abandoned by all imaginable software.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* W11 will probably work, but there will be problems for two reasons.
  * Qt is a large wad of software, and something may go wrong (like Embarcadero VCL + Aero Flip)
  * No one checked fonts. Even in W7 and W10 they are different.

## Tofu/misrenderings
* **W10 should support everything possible, W7 base plane only**.
* Some base place scripts (e.g. Georgian Nuskhuri) will not be shown the best way in W7, but will be shown.
* Three scripts of plane 1 are considered “extremely important” and **will** be supported in W7: Phoenician, Aramaic, Brahmi.
* Small misrenderings in descriptions are tolerable, I’ll fix them only if samples are bad, or if the font has other problems.

## Future functionality
* Version 0.1: when tofu policy of base plane is fulfilled, and some sort of About is made.
* Version 1.0: when **all** happen…
  * All chars of Unicode 13 are drawn, even badly.
  * Basic search.
  * Icon/logo, legal info.
* Unicode 14: when any of U14 scripts is ready, incl. Arabic and emoji but excl. other amendments (even greatly revamped Egyptian hieroglyphs).
* Future tools (**not** milestones of 1.0):
  * Emoji tools: ligatures, more comprehensive reference.
  * Better CJK reference.
  * Sutton SignWriting tool.
