# Combat slot: retail A/B fixtures

The combat dojo's retail sides (plan, katas and results:
`docs/gameplay/COMBAT_DOJO.md`). Versioned with the emulator; the A/B
driver runs a fixture from the repository's own `tools/retail_runtime`
when it has it. Each fixture runs
original retail TCharacter code on the unchanged baseline (`28bec273…`) on
a small fixture world built per case: setup once, checkpoint, restore per
case. The port side and the compare live on the combat branch
(`tools/retail_ab/retail_ab.py combat-*`, `tools/retail_ab/combat_targets.py`,
`src/retailab_combat.cpp`).

| Fixture | Original code | Seams (answered from the case, recorded) |
|---|---|---|
| `combat_call.py` `call: go` (kata M3) | Go(angle) `0x4ce350` and what it calls: HasActionAni, GetAngleMoveAnim, IsValidTarget, Distance, SetDesired, ForceCommand | the shared ones below; FindClearPath `0x4c39d0`, FindCharacters `0x4cd690` (empty world), TPlayer SetPlayerState `0x51d680`, CanSeeCharacter `0x4cd540` |
| `combat_call.py` `call: resolve-combat` / `resolve-combat-move` (kata M5) | ResolveCombat `0x4c7980` / ResolveCombatMove `0x4c7f80` on the doing block, with SetFighting, AdvanceAngles | as above |
| `data_parse.py` (kata D1, `combat-data`) | TRules ctor `0x488160` (static init isn't run), Initialize `0x48b690` with Load `0x48b990` and every block loader, the tokenizer and Parse, BindTypes `0x48cab0` | the file layer (zfopen_rel `0x4a13f0`, length / read / close, FileExists `0x4a1c00`) onto the case's folder, FatalError `0x481c10` / `0x481d10` (stops the case), FindObjType `0x475210` and the class registry from class.def's type names, the item halves of BindTypeData `0x48af30` / `0x48b4a0` |

## Reusable pieces (`guest.py`)

- `start(exe)`: CRT init (`tools/retail_runtime/fixturekit.py`), then the original angle/distance
  tables (`0x41e2de`..`0x41e535`), fixed ids for the stat globals static
  init would fill, and fault reports that name the access and the code
  addresses on the stack.
- `CombatWorld`: TCharacter (0x2a0, vtable `0x5a7848`) and TPlayer (0x674,
  vtable `0x5b4f30`) objects with the real vtables, chardata, root / doing
  / desired action blocks built by the original ctor (`0x4da9f0`), and the
  globals combat reads (CombatFace, frame, AI off, player control).
- Shared seams: FindState / FindTransitionState (state table), SetState
  and a stand-in imagery at `+0x54` (NumStates, GetAniFlags, frame counts;
  any other imagery slot fails the case), GetObjStat (Health / Fatigue /
  Mana) and GetStat (Radius).
- `block_dump` / `character_dump`: the compared record, action block flags
  by meaning (retail's bits differ from the port's 1998 bitfield).
- `call(vm, ...)`: `vm.call` with the fault named.

Run one case directly:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/slots/combat/combat_call.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --case case.json
```

Measured (2026-10-07): ~0.7 ms per case, setup ~10 ms.
