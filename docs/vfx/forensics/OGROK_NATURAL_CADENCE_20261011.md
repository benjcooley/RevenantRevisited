# Original Ogrok arena animation progression

The natural arena check supports the user's objection: the original audience is
made of complete CHARACTER actors with weighted watch-state choices, not the
four-frame EFFECT helpers named OgrokWatcher1/2/3. Those helpers cannot establish
the arena's actual animation cadence.

## Captured original scene

Starting from the original resident arena checkpoint at level0, camera/player
`12482,17440,32`, the emulator completed85 consecutive full original screen ticks
with drawing enabled. The screen counter advanced from3 to88. Each tick received
the registered original WinMM callback `49e2f0`, period41ms. The added controlled
virtual time was3485ms; these are not host-throughput or Windows98 hardware
wall-clock measurements.

Eleven saved spectators near Baez were tracked by their original map indices.
Every captured state length agrees with the packed CHARACTER imagery header and
the original `GetAniLength` result. Their watch clips span15–77frames once their
native action controllers choose subsequent watch variants. Observed examples:

- PaleOgrokclub, index613707169, `100:watcha`:31frames. Native retriggers at screen
  frames4,35,66, giving two31-tick intervals,1271ms each in controlled virtual time.
- Ogrokhammer, index613351543, switches from66-frame `15:watchc` to15-frame
  `100:watchc`. Subsequent retriggers at58,73,88 give two15-tick intervals,615ms.
- Ogrokhammer, index620562662, switches from24-frame `30:watchb` to30-frame
  `100:watchb`. Retriggers at36,66 are30ticks apart,1230ms.
- DarkOgrokhammer, index614279890, starts77-frame `20:watchb`, then selects
 30-frame `100:watchb` at screen39.

These are non-looping root states (`AF_ROOT=0x400`), not `AF_LOOPING`. The original
frame updater reaches the final authored frame and holds it before the actor's
action controller retriggers or chooses another weighted variant. A modulo-only
frame verifier incorrectly calls those terminal holds failures. The report uses
each current state's actual length and flags: zero inconsistent same-state
advances, terminal holds or retriggers were found. Some states changed before a
second complete repeated cycle, so the report does not claim complete cycle
coverage for every spectator or every authored watch variant.

The older scene checkpoint uses the diagnostic startup whose map colors were
corrupt. No appearance approval comes from these images. Subsequent appearance
captures should use the proper original `SOFTWARE3D` WinMain startup recovered by
the main agent. Neither actors nor animation rates were modified during this run.
Optional bitmap cache warming was omitted by the documented input fixture;
original on-demand decompression, actor/action updates and raster drawing ran.

## Four-frame helper usage

`arena_watcher_usage_audit.py` audited all seven shipped `.rvm` modules:5307 map
sectors. None contains the little-endian IDs `aeaeeb23`, `aeaeeb24`, or `aeaeeb25`
anywhere in its bytes, including nested serialized data. Therefore no candidate
sector needed selective object parsing. The85 distinct packed/loose module and
resource script/configuration texts contain none of their names, hexadecimal IDs,
unsigned decimal IDs, or signed decimal IDs. Case aliases of `resources.rvr` are
deduplicated on macOS.

The2048095-byte original rebuilt executable, SHA-256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`, also has zero
literal helper-ID DWORDs and zero ASCII helper names. This is a literal audit,
not control-flow proof: indirect CLASS lookup, computed IDs, runtime creation,
save overrides and other executable paths remain possible. **Do not classify the
helpers as abandoned, remove their176-row verification obligations, or credit
natural arena fidelity from their isolated appearance tests.** The user-approved
reviewer rate0.25 remains a presentation setting, not measured retail cadence.

## Reproduce and inspect

From the VFX validation checkout, set
`PYTHONPATH=tools/retail_runtime:tools/retail_asm` and use the retail-asm Python
environment described in [the natural-scene guide](NATURAL_ARENA_AND_FLAME_20261011.md).
Keep licensed images, executable bytes and capture artifacts outside source Git.

```sh
python tools/retail_runtime/mapped_scene_frame.py \
  --checkpoint /path/to/arena/after-frame.image \
  --ticks 85 --draw --output /tmp/arena-native-85

python tools/retail_runtime/arena_cadence_report.py \
  --initial-manifest /path/to/arena/manifest.json \
  --frames-manifest /tmp/arena-native-85/manifest.json \
  --imagery data/imagery.rvi --output /tmp/arena-cadence.json

python tools/retail_runtime/arena_watcher_usage_audit.py \
  --executable recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output /tmp/watcher-shipped-usage.json

python -m unittest tools/retail_runtime/test_arena_cadence_report.py
```

The seven report regression checks cover real non-loop terminal holds/retriggers,
short-run coverage, missing screen ticks, wrong timer periods, vanished actors,
wrong frame advancement and packed-header disagreement. They test the reporting
boundary, not emulate or replace the original game logic.

Private evidence is under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-ogrok-arena-cadence-followup-20261011/`,
including `manifest.json`, `after-frame.image` and `cadence-final.json`. The
complete usage/literal audit is the adjacent
`vfx-ogrok-arena-cadence-followup-usage-binary-20261011.json`. Input manifests,
checkpoint, imagery and tools are pinned in the reports. Earlier short-control
reports and an initial modulo-only diagnostic are retained and grant no pass
credit.
