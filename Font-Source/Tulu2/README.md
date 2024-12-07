# How to develop
If you did everything that requires rebuilding blue ligatures:
1. Delete them.
2. Switch encoding to Unicode/full, then again to Compact
3. Run Python script.
4. Probably you’ll need to modify some tables: Subj ligs, Oo Au

# Colours in FontForge
* RED: probably unused
* YELLOW: prebuilt syllable CV, CLoopvir: KU
* PALE GREEN: prebuilt syllable C_C (underscore = virama): K_K
* ACID GREEN: prebuilt syllable C_CV: K_KU
* ORANGE: subjoined consonant _C: subjoined K
* PALE BLUE: workaround consonant VC, VC_C

# Colours in HTML
* RED: really need redoing
* ORANGE: synthesized rakar (hook-shaped final R) instead of true, bad but infeasible syllable
* ↓ bad composition of main ligature

Some chosen bad things:
* KTTh: need true ligature
* KTR: need true rakar instead of synthesized, both as a base for bigger ligatures and for beauty
* KTRY: same, but not just rakar
* Most VocRR: need small version of umlaut
* KTV: beautiful!, but there’s a special ligature