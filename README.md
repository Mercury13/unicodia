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

# Compatibility
x64 only. My tofu policy is: **W10 should support everything possible, W7 base plane only**.

* WXP, WVista and W8 are completely abandoned by all imaginable software.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* W11 will probably work, but there will be problems for two reasons.
  * Qt is a large wad of software, and something may go wrong (like Embarcadero VCL + Aero Flip)
  * No one checked fonts. Even in W7 and W10 they are different.

# Release policy
* Version 0.1: when tofu policy of base plane is fulfilled.
* Version 1.0: when **both** happen…
  * All chars of Unicode 13 are drawn, even badly.
  * Basic search.
* Unicode 14: when one of three happens…
  * Scheherazade (a well-known Arabic font) is updated to U14.
  * Any of U14 scripts is ready.
  * Someone releases emoji for U14, and I manage to pull them somehow.
* Future tools (**not** milestones of 1.0):
  * Emoji tools: ligatures, more comprehensive reference.
  * Better CJK reference.
  * Sutton SignWriting tool.
