# What is Unicodia?

It is a simple Unicode encyclopedia and replacement for Character Map. Right now Windows only.

**Lifecycle phase: 5** (production/stable). Minor troubles with sustainability, but generally survived two releases of Unicode.

**I’m in Ukraine torn with war, so I’ll release often.** See “war release” tag in Issues.

# How to translate?
* Ask programmer to add localized buttons if needed. One button is international for now, A-Z, and it already has Cyrillic version. The rest are unchangeable for now… until needed.
* Download Lang-src/en.uorig from this repo.
  * If you are able to use Git, better use it. We’ll be able to work together on one translation.
* Put Unicodia to writeable location.
* Create language directory, edit locale.xml for that language.
* Download [UTranslator](https://github.com/Mercury13/utranslator). New → Translation of *.uorig.
  * If you don’t know English, use other \*.utran file as reference translation.
* Press F12 in Unicodia to reload translation without reopening the entire program.
  * Warning, it reloads strings only; all locales are loaded on startup.
* When new original arrived: File → Update data, Go → Find warnings → All.

# How to build?
* Slight C++20 and std::filesystem here → so need either MSYS or recent Qt with MinGW 11.
* Also need cURL (present in W10 18H2+), [7-zip](https://7-zip.org), [UTransCon](https://github.com/Mercury13/utranslator).
* Configure and run tape.bat file.
* Configure and run rel.bat file.

# How to develop?
See develop.md.

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
Fonts are always updated to release versions. Font is updated to alpha/beta if fixes a major misrender, and/or professionally implements a new character.

Naming: Noto if tables and existing glyphs are surely untouched; Uto otherwise.

## Language

**English.** International: truck > lorry, petrol > gas. Prefer British form if both are good.

**Russian.** Ё is mandatory. No war jargon. No grammatical concessions to Ukrainian. Describe 2022 war as neutral as possible.

**Ukrainian.** Avoid 2019 reform, earlier changes (e.g. frequent Ґ in loanwords) are embraced. No war jargon. Describe topics sensitive to nationalists as delicately as possible.

## Future functionality
* Finish GlyphWiki loader.
* Better CJK reference.
