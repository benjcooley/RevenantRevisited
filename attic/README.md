# attic/

Old code pulled out of the live tree during the port, **preserved for
reference**. Nothing in `attic/` is built or included by anything in
`src/`.

Use it when:
- A modernization made a file redundant but we may still want to read
  the original (DirectDraw/DirectInput wrappers, CD-audio MCI code,
  etc.).
- A subsystem was rewritten and the old implementation is the only
  record of a 1990s-era algorithm (e.g. the MMX DLS lighting code
  being ported to a pixel shader — the original MMX lives here while
  the shader lands in `src/`).
- We removed a big `#if 0` block from a live file during cleanup but
  suspect pieces of it might still be useful.

## Layout

Mirror the `src/` tree: something removed from `src/foo.cpp` goes to
`attic/src/foo.cpp` (or `attic/src/foo_mmx.cpp` for partial extracts).
Mirror `recon/` if anything reconstruction-side gets retired too.

For partial extracts, start the attic file with a header comment
identifying where it came from and why it was retired:

```cpp
// Retired from src/dls.cpp (MMX per-pixel lighting, circa 1998 pre-
// release). Replaced by the Metal pixel shader in src/shaders/dls.*.
// Kept here as algorithmic reference for the shader.
```

## Rules

- **Don't modify files in attic.** If an attic file needs updating,
  the thing to update is what replaced it in `src/`.
- **Don't `#include` attic files from `src/`.** Attic is outside the
  build. If you find yourself wanting to, that's a signal the code
  should move back into `src/`.
- **Don't re-delete from attic.** We keep everything.
- **Use `git mv` for whole-file moves** so history follows.
