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
Right now W7 and W10, x64 only.
* WXP, WVista and W8 are completely abandoned by all imaginable software.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* W11 will probably work, but there will be problems for two reasons.
  * Qt is a large wad of software, and something may go wrong (like Embarcadero VCL + Aero Flip)
  * No one checked fonts. Even in W7 and W10 they are different.
