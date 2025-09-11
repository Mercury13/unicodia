# How to build GlyphWiki font

1. Load http://en.glyphwiki.org/dump.tar.gz
2. Put to KageArchiver’s home directory: BOTH files from dump, ``hani-tofu.txt`` (better symlink rather than copy), ``Unihan_IRGSources.txt`` (again, better symlink)
3. Get ``hani-tasks.txt``, put here, it’ll be versioned.
4. For the next step need Node.JS (``node.exe``), TtfAutoHint
5. Open SFD, run script ``load-glyphs.py``.
