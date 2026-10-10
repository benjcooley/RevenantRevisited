# Rift1: authentic animtex tag rejected by retail

Rift1 `0xadbcef18`, shipped `Imagery/Misc/rift1.i3d`, is deferred. Do not replace
its `d` parameter with `v` and call the resulting animation retail fidelity.
No production repair, animated-atlas support or static-render acceptance was
granted by this preflight.

The exact asset SHA-256 is
`58fbb47a8d293744458363f73996e971223118a53e3c4abc76ddc9de17b1263f`.
Its sole raw tag at state 0/frame 0 is `animtex`, text `obj=rift,u=4,d=4`. There is
one object named `rift`, four vertices, two triangles and one stored ARGB4444
texture frame. Authored UVs already occupy the first quarter-cell. Ordinary
SetTextureFrame cannot supply a 16-frame animation from that single texture.

Retail registration `0x401a60` installs builder `0x5e82d8`, factory `0x405b40`
and controller vtable `0x5a34c4`. Actual ParseItem `0x401a80` compares native
strings `u`, `v`, `g`, at `0x5c59ac`, `0x5c59b4`, `0x5c59bc`; it does not accept
`d`. Running the complete native tag parser and Initialize `0x401b70` against
the unchanged tag returns 0 after U=4, leaving V=0. Exact-name object selection
has already selected one object. Authored vertex bytes remain unchanged and the
original UV array at controller+`0x40` has not been allocated.

Actual RefreshControllers `0x40df90` then executes the real failure-admission
branch at `0x40e0c8`, calls deleting destructor `0x405bc0`, and admits zero
controllers. One run stops immediately before that destructor, without replacing
or intercepting it. A separate unmodified cold-heap execution enters the
destructor and faults at `0x405c14`, reading the null UV-array pointer: its cleanup
uses the already selected object count even though initialization never allocated
the saved UV array. Both observations replay exactly in two fresh isolated runs.

This is a bounded native failure in a declared minimal imagery/owner fixture with
original CRT heap and actual registration/admission. It does not establish that
every real retail heap state or natural Rift placement has the same fault. It also
does not establish a clean failed-controller removal, a successfully rendered
static retail path, or any native animtex Render call. The probe deliberately never
calls Render on the failed controller; V=0 would make its division invalid.

For a legally initialized u/v tag, recovered Render `0x401c90` selects a global
atlas cell and adds its offset to the preserved authored UVs; it does not scale
those UVs. Those legal rules do not authorize repairing this different shipped
tag. Future work may investigate an independently evidenced runtime alias/asset
revision or instrument the failure safely, but should preserve the original
failure and avoid guessing a caller/default value.

Run the reproducible diagnostic from the repository root:

```sh
/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/rift_failed_animtex_probe.py /absolute/path/to/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/rift-failed-animtex-preflight
```

The verified executable SHA-256 is
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
The default archive is `data/imagery.rvi`; `--archive` accepts another exact copy.
[Probe](../../../tools/retail_runtime/rift_failed_animtex_probe.py) and
[machine-readable report](../../../recon/retail_asm/runtime/effects/rift-failed-animtex-preflight/manifest.json)
retain actual initialization return, parser strings, admitted count, deletion
trace, fault address and exact fresh-repeat evidence. No DOSBox or OS boot is used.
