# How to build UnicodiaSesh font

1. Need software: Inkscape, TtfAutoHint
2. Put JSesh SVGs to Svg directory. WARNING: those SVGs are non-versioned
3. Open UnicodiaSesh.sfd
4. Run load-glyphs.py
5. Wait REALLY long, approx 30m
6. DO NOT SAVE
7. Check sesh.log, it might show problems
8. The longest operation, SVG uniting, will be cached, and next runs will be shorter

Meaning of directories:
- InkTest: just a test for Inkscape
- svg: raw JSesh SVGs
- svg-ex:
  - SVGs from JSesh exceptions
  - SVGs that appeared after UniKemet
  - possibly with my slight remakes (see svg-remade)
- svg-my: rather big remakes, both new and existing. These changes automatically make object My:
  - deleting an object
  - adding an object
  - filling hollow object, hollowing filled object
  - major simplifications that surely change content
  - making anything (pose, object, thickness) more pronounced
- svg-remade: small remakes of existing JSesh hieros w/o changing content:
  - changing canvas and/or image size
  - subtracting white
  - manual uniting
  - removal of red
  - nudging vertices to eliminate situation “program 1 sees intersection, program 2 doesn’t” 
  - nudging vertices to align them to some guide
  - simplification
  - removal of SVG artifacts
  - removal of tiny objects from uniting
  - turning bad ellipses to right ellipses when dimensions are kept
  - turning 1-point curve to something that does not look like glitch
  - enforcing symmetry via half+clone
  - flipping, rotation by right angles
- cache: cache for auto-Inkscape’d JSesh SVGs (running Inkscape takes ≈1s/file)
- manual: fixed directly in FontForge, everything is done manually except bearings (gradually phasing out this directory)

# Error messages

- XXX self-intersects / Critical / Built-in systems are unable to fix SVG’s self-intersections. Use other ways, like ``manual`` (complete pre-scaled glyphs) and ``svg-remade`` (technical changes).
- XXX exists / Warning / Way A is overridden by way B. Just delete that file.

# How to update data

1. After loading Unikemet.txt run file filter_unikemet.bat
2. Check what changed in Unisesh.txt. That’s some random ID and JSesh ID. Changed → something really big happened.
3. In rationale.txt there are a few chars I found for myself. Have they landed to the same or different code point?
4. Then check sample images (using delta code charts).
5. Then descriptions (using Unikemet changes)

Check = what changed? Is the character original or redrawn? Can we find something in JSesh base? Maybe revert to JSesh’s image?

Why Unisesh.txt? Just to make changes more obvious.

# Colour code

- All shades of violet: convert to path, mainly Stroke to path
- Crimson, grey, dark-blue etc: just to remind that you need to unite the objects while exporting
- Yellow: background here. Either subtract, or leave as and let the loader do its job
  - Yellow fill + violet outline are permitted
  - Yellow fill + black/grey/… outline are permitted too, though not encouraged
- Scarlet: technical rigs e.g. to put a guideline in place; delete when exporting
- Orange: reference drawing; delete when exporting
