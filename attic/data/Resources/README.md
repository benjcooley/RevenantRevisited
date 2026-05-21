# attic/data/Resources/

Pre-release `.def` files retired from `data/Resources/` on 2026-05-20.

Each of these had a **retail** counterpart inside the shipped archives
(`data/resources.rvr`), so the loose copies here were shadowing the retail
data in the VFS resolution order (RunPath before archives). The engine now
resolves the retail versions through the virtual file system instead.

- `rules.def` — pre-release; 86 KB with stale CHARACTER blocks (e.g. "Blue
  Druhg") absent from the retail `class.def`. Retail `rules.def` (in
  `resources.rvr`, ~8.6 KB) carries only global rules tags; characters come
  from retail `char.def`. This shadow caused a fatal "Invalid character type"
  at boot on Windows (macOS skipped it via a backslash-path quirk).
- `joingame.def`, `mpingame.def`, `options.def`, `spell.def`, `statpane.def` —
  pre-release copies of files that also exist (retail) in `resources.rvr`.

Kept live in `data/Resources/` because they have **no** retail archive copy:
`effects.def`, `render_metadata.def` (port additions).

Not modified — see ../../README.md (attic rules).
