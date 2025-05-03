# Technical problems of GlyphWiki SVGs

## Small (up to 3/200) diameter

* Sample: 3162C
* Frequency: 40/1937 = 2.1%
* Cause: mechanical uniting strokes [00.png](00.png)

## Repeating points

Never seen real repeating points in the field. Probably all repeating points fall into the next category.

## Points so close that rounding makes them repeat

* Sample: 31353
* Frequency: extremely common, >75%
* Cause 1: mechanical uniting strokes
* Cause 2: designer erases a part of glyph and draws it from scratch.
* Danger: breaks algorithms
* Workaround: delete them
* Regardless of cause, illustration is [01.png](01.png)

I’ve seen points 0.001 units apart.

## ADJACENT back-forth lines

* Sample: 3138A
* Frequency: 129/1937 = 5.6%
* Cause: sharp corner + point close to corner’s apex + rounding
* Danger: may break algorithms, false self-intersection
* Workaround: delete them

## Extremely sharp corner (<5°) short (one side up to 2 smallest units)

* Sample: 31418
* Frequency: 20/1937 = 1.1%
* Known cause: tiny step on an almost vertical line when X coordinates are same for some reason, and Y are not! [03.png](03.png)
* Danger: false corners, sometimes false self-intersection
* Workaround: delete them

## Extremely sharp corner (<5°) long (both sides > 2 smallest units)

* Sample: 31BC9
* Frequency: 2/1937 = 0.1%
* Cause: poor grafting of glyph parts [02.png](02.png)
* Danger: rounding may lead to self-intersection
* Workaround: delete them

## Angle exactly 180°

* Sample: 31353
* Frequency: almost everywhere, >94%
* Cause: rounding led to an extraneous point
* Danger: mostly harmless
* Workaround: delete them

## NON-ADJACENT back-forth lines
* Sample: 2FC5
* Frequency: single SVG
* Cause: poor grafting of glyph parts. They always occur on horizontal/vertical lines: in diagonal they are really unlikely because need coincidence of several factors
* Danger: may break algorithms, false self-intersection
* Workaround: inkscape manually, Path→Union
