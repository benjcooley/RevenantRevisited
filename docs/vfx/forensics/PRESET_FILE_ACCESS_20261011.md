# Filename preset access in the original emulator

The bounded software fixture ran the original CRT heap/thread prefix, but not
the separate C initializer that allocates `FILE` streams. Calling `_ioinit`
`59299f` creates descriptors, while initializer `59151e` creates 512 streams.
Without the latter, even the existing `revenant.ini` returns null from original
CRT fopen `58b5db` before any `CreateFileA` request. This explains why the first
CFire/sgeyser initializer probes could not establish file absence.

`tools/retail_runtime/preset_file_access.py` provides an explicit opt-in for
filename-dependent fixtures. It reuses the HUD slot's Windows file service,
mounts the actual install read-only, executes both original I/O initializers
when needed, and creates the file-manager mutex through the original WinMain
fragment. It then exercises original custom fopen `4a1240`, length `4a17b0`,
fread `4a15a0` and close `4a1540`. No tokenizer, preset parser, controller
parameters or file contents are supplied by a replacement implementation.

The positive control reads the complete actual `revenant.ini` byte-for-byte.
The descriptor-only negative control returns null before `CreateFileA`.
Actual requests for `cfire.txt`, `csparks.txt` and `vapor.txt` now reach the
Windows file service and fail because those loose files are absent. A separate
audit checks every top-level ZIP-compatible installed pack for the three
exact basenames; none contains them. This is an audit of the recorded install,
not a claim about every caller, historical patch or alternate data set.

The complete original multiple-tag initializer confirms CFire has zero
filename controllers in this install. Sgeyser retains its real inline
104-slot controller and has no filename-based vapor controller. Do not
invent missing preset definitions or call a geometry-only preview complete.
The fixture's `particle_required=None` option bypasses only its former
one-controller prototype precondition; original Initialize remains in charge
of controller creation. Existing True/False contracts are preserved.

Reproduce with the lab Unicorn virtual environment:

```sh
python tools/retail_runtime/preset_file_access.py --data-root <retail-install> --output <new-private-directory>
python tools/retail_runtime/multiple_partsys_native_preflight.py --startup --data-root <retail-install> --output <another-new-private-directory>
```

Private evidence: `RevenantRetailLab/research/vfx-preset-files-20261011/`.
`archive-qualified` pins the positive/negative file controls, API trace and
pack hashes; `multiple-qualified` records both original initializer results.
No visual pass is granted by these I/O and initialization checks.
