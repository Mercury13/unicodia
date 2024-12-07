# How to develop
If you did everything that requires rebuilding blue ligatures:
1. Delete them.
2. Switch encoding to Unicode/full, then again to Compact
3. Run Python script.
4. Probably you’ll need to modify some tables: Subj ligs, Oo Au

# Colours in FontForge
* RED: probably unused
* YELLOW: prebuilt syllable CV, CLoopvir: KU
* PALE GREEN (123, 0.2, 1): prebuilt syllable C_C (underscore = virama): K_K
* ACID GREEN: prebuilt syllable C_CV: K_KU
* ORANGE: subjoined consonant _C: subjoined K
* BROWN: everything else subjoined: _CV, _C_C…
* PALE BLUE: workaround consonant VC, VC_C

# Colours in HTML
* RED: really need redoing
* ORANGE: synthesized rakar (hook-shaped final R) instead of true, bad but infeasible syllable

## Some chosen bad things
* KTt: need true ligature
* KTTh: need true ligature
* KTRY: need true ligature, or at least nicer subjoined Y
* KTV: beautiful!, but there’s a special ligature
* KSsNnY: need special subjoined NnY
  * KSsNnYvR: unfeasible but overlaps
