# Discovered Functions / Renames

This directory accumulates retail reverse-engineering findings that didn't make it into OOAnalyzer's first pass:

- **`*.cpp`** — decompiled bodies of functions Ghidra missed during the original recon dump (force-created via `DecompileAddr.java`). File naming: `<class>_<RealName>_<addr>.cpp` (e.g. `cls_0x5a7b98_TCharacter_AI_4c8b60.cpp`).
- **`renames/*.txt`** — rename pairs applied to all `recon/**/*.cpp` in place by `recon/scripts/apply_renames.sh`. One pair per line in `BARE|RENAMED` format, `#` comments ignored. Multiple agents can write their own .txt file without merge conflicts.

## Naming convention

Keep the original address as a prefix and append the identified name: `meth_0x4c9b70_AI_PerMonster`, `cls_0x5a7b98_TCharacter`, `FUN_004c8b60_AI`. Address-as-prefix preserves traceability — you can still grep for `0x4c9b70` and find every reference, while the suffix tells you what it is.

## Workflow for adding identifications

1. Decompile the function via Ghidra CLI and save the body to a file in this directory:
   ```
   /opt/homebrew/Cellar/ghidra/12.0.4/libexec/support/analyzeHeadless \
     /Users/benjamincooley/projects/RevenantRevisited/data RevenantDev \
     -process Revenant.exe -noanalysis -readOnly \
     -scriptPath /Users/benjamincooley/projects/RevenantRevisited/recon/ghidra_scripts \
     -postScript DecompileAddr.java 0x004cXXXX \
     /Users/benjamincooley/projects/RevenantRevisited/recon/discovered/cls_0xCCCC_ClassName_FuncName_4cXXXX.cpp
   ```
2. Append your rename pairs to `renames/<your_label>.txt` (one pair per line):
   ```
   meth_0x4cXXXX|meth_0x4cXXXX_FuncName
   FUN_004cXXXX|FUN_004cXXXX_FuncName
   ```
3. Run `bash recon/scripts/apply_renames.sh` — every `.cpp` under `recon/` gets the renames applied.

## Useful Ghidra scripts

In `recon/ghidra_scripts/` (callable via `-postScript NAME.java arg1 arg2 ...`):

- `DecompileAddr.java <hex_addr> <out_path>` — decompile a function (force-creates if not yet recognized).
- `CallersOf.java <hex_addr>` — list call sites of a function.
- `FindStringRefs.java <ascii_string>` — find which functions reference a given C-string literal.
- `FindImmRefs.java <hex_addr>` — find which instructions take this address as an immediate operand.
- `FindBytes.java <hex_value>` — find a 4-byte LE pattern (catches vtable entries pointing at a function).
- `DumpVtable.java <hex_addr> [n_slots]` — dump a vtable as a list of slot→function pointers.
- `FnsInRange.java <lo> <hi>` — list functions whose entry point falls in [lo, hi].
- `Disasm.java <hex_addr> [n_ins]` — raw disassembly of a window starting at addr.
- `FindFn.java <hex_addr>` — find what function contains a given instruction address.

The Ghidra project is at `data/RevenantDev` (full retail analysis, 6012 functions). The other projects (`Revenant`, `RevenantBC`, `RevenantBC2`) have only partial analysis.

## High-confidence anchors (already identified)

- **TCharacter** = `cls_0x5a7b98` (vtable address 0x5a7b98).
- **TPlayer** = `cls_0x5b4f30` (vtable inherits TCharacter's virtuals; recon yaml mismapped as TPlayScreen).
- **TCharacter::AI** = `FUN_004c8b60` — wander/patrol via "waypoint"-named objects, calls AI_PerMonster conditionally.
- **TCharacter::AI_PerMonster** = `meth_0x4c9b70` — switch on character name (Solifuge/Jhaga/Yhagoro/...), allocates per-type state buffers (this->mbr_0x280 = type id, this->mbr_0x284 = state ptr).
- **TCharacter::ResolveAttack** = `virt_meth_0x4c1bb0` — uses "atkimpale" / "blood" / "sparks" string anchors.
- **TCharacter::EffectBurst** = `meth_0x4c85d0` — string-compares against "blood"/"sparks"/"Blood".
- **TCharacter::IsEnemy** = `meth_0x4c89c0` (likely) — iterates `chardata->enemies` list at field 0xfc+0x70.
