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
- svg-my: rather big remakes, both new and existing
- svg-remade: small remakes of extsting JSesh hieros w/o changing content: subtracting white, manual addition
- cache: cache for auto-Inkscape’d JSesh SVGs (running Inkscape takes ≈1s/file)
- manual: fixed directly in FontForge, everything is done manually except bearings (gradually phasing out this directory)
- manual-wide: same, but accent that the image is wide

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