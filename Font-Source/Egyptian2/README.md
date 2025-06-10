# How to build UnicodiaSesh font

1. Need software: Inkscape, TtfAutoHint
2. Put JSesh SVGs to Svg directory. WARNING: those SVGs are non-versioned
3. Open UnicodiaSesh.sfd
4. Run load-glyphs.py
5. Wait REALLY long, approx 30m

Meaning of directories:
- InkTest: just a test for Inkscape
- svg: raw JSesh SVGs
- svg-remade: JSesh SVGs remade somehow, currently subtracted white things
- cache: cache for auto-Inkscape’d JSesh SVGs (running Inkscape takes ≈1s/file)
- manual: everything is done manually except bearings
- manual-wide: same, but accent that the image is wide