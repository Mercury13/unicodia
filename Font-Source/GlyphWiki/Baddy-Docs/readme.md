# Technical problems of GlyphWiki SVGs

## Small (up to 3/200) diameter

* Sample: 3162C
* Frequency: 40/1937 = 2.1%
* Cause: mechanical uniting strokes

## Repeating points

Never seen real repeating points in the field. Probably all repeating points fall into the next category.

## Points so close that rounding makes them repeat

* Sample: 31353
* Frequency: extremely common, >75%
* Cause 1: mechanical uniting strokes
* Cause 2: designer erases a part of glyph and draws it from scratch.

I’ve seen points 0.001 units apart.

## Back-forth strokes

* Sample: 3138A
* Frequency: 129/1937 = 5.6%
* Cause: sharp corner + point close to corner’s apex + rounding

## Extremely sharp corner (<5°)

* Sample: 31418
* Frequency: 22/1937 = 1.1%
* Cause 1: tiny step on an almost vertical line when X coordinates are same for some reason, and Y are not!
* Cause 2: poor grafting of glyph parts

## Angle exactly 180°

* Sample: 31353
* Frequency: almost everywhere, >94%
* Cause: rounding led to an extraneous point
