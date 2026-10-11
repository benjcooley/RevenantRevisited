# Ogrok watcher format deferral, 2026-10-10

All three watcher types use the generic EFFECT/default animator with no tags,
one four-vertex `hips` billboard, four active STILL frames and exact ARGB1555
textures (`7c00/03e0/001f/8000`). Watcher3 stores six frames; only0..3 are active.
The shared alpha-loader correction preserves authored coverage for watcher2/3.

Fresh `watcher_format_boundary.py` diagnostics reproduce the original Blue
software neon/rainbow fingerprint for each exact asset's raw frame0, while the
portable loader correctly decodes authored1555 colors. Original
GetClosestTextureFormat4164c0 leaves the supplied descriptor unchanged in
software mode, and original ConvertTexture416790 copies identical-format raw
bytes exactly. Complete default/owner/key/RenderObject/software drawing executes
with exact resident raw1555 texels and explicit CULL_NONE/source lighting.

The decoded resource and resident COM surface are declared fixture boundaries.
Full LoadTexture/CreateSurface, original texture-frame transfer and natural
device behavior have not been executed; this is format forensics, not an
accurate graphics reference or primary visual closure. The historical bounded
frontend's "any alpha implies4444" format label likewise does not establish
retail texture transfer. No graphics patch or retagging was made to imitate
the original software1555 interpretation. Root explicitly deferred all three
primary closures and retained the correct portable decoding.

Private evidence:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-watchers-20261010/frozen-diagnostic2/manifest.json`.
Three fresh whole-frame0 diagnostics and exact image/depth warm replays are
pinned there. `accepted`, accurate-reference and primary-visual flags remain
false; no new rendered row or acceptance credit is claimed.
