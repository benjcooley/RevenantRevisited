# CharUtility and SewerWater: bounded default-animator audit

Audited 2026-10-07 before any production change. The exact retail EFFECT rows
are `CharUtility:0xadbcef13` and `SewerWater:0xadbcef19`. CharUtility supplies
five root meshes/textures (shadow, comflash, vision, flamef, comring), 50
vertices and 40 triangles. SewerWater supplies one root mesh/texture, four
vertices and two triangles. Both have one looping `STILL` state, constant
nine-word scalar position/rotation/scale keys, one texture frame per material,
and no authored controller tags. CharUtility's state has 100 frames;
SewerWater's has one. These are separate from StillWater and the watchers.

The native registry contract is checked directly, not inferred from an asset
name. The probe recovers the retail `PUSH name; MOV ECX,builder; CALL 40db90`
registration sites from executable bytes, executes those original constructor
calls and original lookup `40dca0`, and checks both exact names and `EFFECT`
return default builder `5e8508`. A named FLAME lookup is a positive control.
This initializes only the builder registry, not full game startup.

The new probe reads pinned shipped assets, remaps each independent root part
to object0 for original `GetAniKey/CalcObjectMatrix` calls, and transforms its
authored vertices through original matrix/point functions. It compares those
packets with compiled current production key decoder, matrix, mesh extraction
and generic static submission bodies. Original software projection/raster is
shared for each pair. Texture slots, local indices and part order stay intact.

Scope is constant authored geometry/material and bounded helper submission;
identity or the explicit `(16,-8,4)` owner matrix, white vertex color, cullNONE
and depth test/write are fixture inputs. Character utility selection, real
map binding/normal illumination, natural lifecycle and modern GPU acceptance
remain separate. A static helper sample does not establish those behaviors.
No geometry scale, camera fit or per-effect lighting adjustment is permitted.

Source tools: `tools/retail_runtime/default_static_probe.py` and
`default_static_profiles.json`. Generated native/port images, packets,
source hashes and replay results stay under ignored
`recon/retail_asm/runtime/effects/default-static-frontend-ab/`.

Run from the repository root using the environment from the runtime setup guide:

```sh
.venv/bin/python tools/retail_runtime/default_static_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/default-static-frontend-ab
.venv/bin/python -m unittest discover -s tools/retail_runtime -p 'test_default_static.py'
```

The first full comparison passes eight cases, two warm replays each, with zero
differing RGB565 pixels and identical depth. CharUtility has six cases across
frames0/50/99 and two owner poses; SewerWater has two frame0 cases. Every part
and all54 vertices/42 triangles are retained. Maximum corner errors are
`7.68e-6` and `4.70e-8` world units. Native frames remain constant and the owner
translation changes each visible image. Initial white-input references contain
7,864–7,910 and 9,341–9,362 visible pixels respectively. No production fix was
needed, and none was applied. Original and port geometry/material are matched
within this explicit fixture; natural character/map and Metal remain open.

The completed eight-case evidence is now copied to the read-only directory
`recon/retail_asm/runtime/effects/default-static-frontend-ab-sealed-1fa5468d7d7c047a/`.
Its manifest SHA is
`1fa5468d7d7c047a004b6a730a15e11fc3dfb21022a4840d50c5c68a68ffed27`.
Use a new output directory for subsequent runs; do not overwrite this evidence.

The next easy-candidate scan found `LabGateBarrierS:0xad92bc37` and
`LabGateBarrierE:0xad92bc38`: both have one constant root quad, two triangles,
no tags and no texture. Actual native lookup confirms the default builder.
They are deferred at the untextured material/raster boundary; the existing
textured fixture must not substitute an invented white texture. No additional
pixel pass is claimed. The remaining zero-triangle `StrikeEffect` dummy is a
spell-controlled placeholder and is not a standalone visual replacement for
its actual spell-specific imagery.
