# What is Unicodia?

It is a simple Unicode encyclopedia and the most comprehensive character map ever. Right now Windows only.

**Lifecycle phase: 5** (production/stable). Minor troubles with sustainability, but generally survived two releases of Unicode. And will survive the third if I survive.

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
See [develop.md](develop.md).

# Compatibility and policies

## Platforms
**Win7/10/11 x64 only.** Rationale:
* WXP, WVista and W8 are completely abandoned by all imaginable software. Though I did some improvements specially for W8.
* No obstacles for x86, just untested because no one compiled Qt for x86.
* Recently checked Windows 11, and it works.

## Tofu/misrenderings
* **W10/11 should support everything possible, W7 just runs somehow.** At the time of testing still no BMP tofu, per old policy.
* Previously W7 supported the entire base plane and three important plane 1 scripts. I dropped that guarantee, though I did nothing against it, just did not test
* Small misrenderings in descriptions are tolerable, I’ll fix them only if samples are bad, or if the font has other problems.

## Update Unicode
Wartime: as soon as base arrives, and release date is frozen, even on alpha review stage

Peacetime (probably): stable release + some big font covering a major set arrives. Han too if the coverage is really high

Emergency releases of a few characters (e.g. currency, Japanese era): instantly, even if they are tofu

## Fonts
Fonts are always updated to release versions. Font is updated to alpha/beta if fixes a major misrender, and/or professionally implements a new character.

Naming: Noto if tables and existing glyphs are surely untouched; Uto otherwise.

These fonts are taken to Unicodia without author’s consent:
* Craggy font with missing/trivial tables.
* Font without license belonging to author of Unicode request and released by him/her.
* Author is surely SIL, even if found elsewhere.
* ASCII mapping is NOT a reason.

## Language
**Common.** No war jargon. Describe 2022 war as neutral as possible. Every _lingua franca_ (English, Russian) in its international form. Write examples in appropriate language where feasible: e.g. English L10n → examples in English.

_(Currently Russian only but may apply to all languages with noun-adjective agreement.)_ Adjectives like _Georgian_ may agree to _script_, or to _language_. The rules are…
* BLOCKS: strongly connected to language → to language _(грузинский=Georgian [language])_. Otherwise to script _(батакская=Batak [script])_.
* SCRIPTS: of course agree to script _(грузинская=Georgian [script])_.

**English.** International: truck > lorry, petrol > gas. Prefer British form if both are good. Punctuation around quotes is British/international: it’s inside quotes if it’s a part of “phrase being quoted”.

**Russian.** Ё is mandatory. No grammatical concessions to Ukrainian.

**Ukrainian.** Avoid 2019 reform, earlier changes (e.g. frequent Ґ in loanwords) are embraced. Describe topics sensitive to nationalists as delicately as possible.

## Future functionality
* Finish GlyphWiki loader.
* Better CJK reference.
* Plane map.