# Gameplay / Combat / Controls — Burndown

Live status. Update as work progresses. Goal: bring Demo 1 (Locke wakes up at Misthaven fountain, walks to the forest, fights an Araknid, takes a quicksave, exits to the next sector) to end-to-end playable.

**Track ownership.** Several systems originally listed here are owned by the **gameflow** track ([../gameflow/README.md](../gameflow/README.md) / [../gameflow/BURNDOWN.md](../gameflow/BURNDOWN.md)). Save / load, sector transitions (exits / doors), the script runtime, the main menu, the new-game path, dialogs, death+restart, and the whole audio system live there. This burndown covers what stays in **core gameplay/combat on main**: input dispatch, attacks, AI, equipment, XP/leveling. Adjacent: [../ui/BURNDOWN.md](../ui/BURNDOWN.md) for HUD/OOG screens, the VFX worktree's INVENTORY for spell + impact effects.

**Forensics first.** Before porting any combat / movement / AI / animation behavior, follow [AGENT_GUIDE.md](AGENT_GUIDE.md) — the forensics-first protocol (constants, state-machine shape, animation events, trigger sites, data deps) + the shared Ghidra recon rigor. Same discipline the VFX + UI tracks use.

**Legend:** `[ ]` pending · `[~]` in progress · `[x]` done · `[!]` blocked · `[-]` cancelled / handed off

Last updated: 2026-05-19 (added AGENT_GUIDE forensics protocol)

---

## Phase A — Player input dispatch (the blocker)

`TPlayScreen::Command` at [src/playscreen.cpp:969](../../src/playscreen.cpp#L969) is a full stub. All non-movement input (attack, dodge, inventory, spells, mode switch, pause) routes to nothing. Until this dispatcher is real, no combat happens.

- `[ ]` **A.1 `Command()` dispatcher skeleton** — switch on `GAMECOMMAND`, log unhandled cases. Drop the `TODO(port)` marker. *Tiny, but unblocks everything below.*
- `[ ]` **A.2 `GAMECMD_SWING / THRUST / CHOP` → `Player->ButtonAttack(buttonid)`** — call the working attack pipeline at [character.cpp:4239](../../src/character.cpp#L4239). Map button ids per the design in [../COMBAT.md](../COMBAT.md). Hold-to-charge gating from [../COMBAT_RULES.md](../COMBAT_RULES.md) tracked separately.
- `[ ]` **A.3 `GAMECMD_BLOCK`** — already wired in [playscreen.cpp:1025](../../src/playscreen.cpp#L1025) via `UpdateMove`. Verify it still fires after the dispatcher refactor; move the call inside `Command()` if cleaner.
- `[ ]` **A.4 `GAMECMD_DODGE`** — define semantics (animation + i-frames? brief speed boost?) per [../COMBAT.md](../COMBAT.md), then call into `Player->Dodge(...)` (new). Currently no implementation on either side.
- `[ ]` **A.5 `GAMECMD_COMBAT` / `GAMECMD_BOW`** — combat-mode toggle + bow-mode toggle. Drives `CTRL_COMBATMODE` / `CTRL_BOWMODE` bits in ControlMap so the per-mode bindings (Swing/Block/Dodge live in `CTRL_COMBATMODE`) actually take effect. Verify mode transitions don't strand input state.
- `[ ]` **A.6 `GAMECMD_INVENTORY`** — toggles the inventory pane. Stub-fires a log for now; full UI lives in [../ui/BURNDOWN.md](../ui/BURNDOWN.md) B.6 — coordinate so the dispatch side lands before the pane code so wiring is just-in-time.
- `[ ]` **A.7 `GAMECMD_INVOKE1..4`** — spell quick-cast. Calls `Player->Cast(spellname, target)`; spell list comes from the QuickSpellPane state (UI worktree B.4).
- `[ ]` **A.8 `--test=gameplay-input`** — synthesizes a `Player`, fires each `GAMECOMMAND` once, asserts the right method was called. Cheap regression net once the dispatcher is real.

> Overlaps with UI [../ui/BURNDOWN.md](../ui/BURNDOWN.md) B.11 ("GAMECOMMAND dispatch un-stub"). Core combat dispatch (A.1-A.5) lives here; inventory/spell dispatch (A.6/A.7) is the seam — landing the skeleton + combat first means the UI side can plug into a real dispatcher when its panes are ready.

## Phase B — Player setup at boot

Without a properly-equipped Player, attacks miss for "no weapon," fatigue is wrong, and damage isn't computed.

- `[x]` **B.1 Auto-equip starter loadout on `SpawnDefaultPlayer`** — done 2026-05-17. Short Sword + Brown Cloth shirt/pants/boots, via the new `EquipStarterLoadout` helper in [playscreen.cpp](../../src/playscreen.cpp). Item names come from the runtime class.def packed in `imagery.rvi` (not the legacy/Class.Def at repo root). Will be superseded when gameflow's newgame.sav loading lands.
- `[ ]` **B.2 Level from save instead of hardcoded 10** — [player.cpp:189](../../src/player.cpp#L189) placeholder makes skill checks pass that shouldn't. Tied to gameflow's save/load (T5) — until then leave the placeholder but log a warning at boot so it doesn't get forgotten.
- `[-]` **B.3 Inventory pre-population** — *Handed to gameflow T4 (starting scripts / location). The retail starter inventory is set by the opening script, not by C++; gameflow owns the script runtime.*
- `[-]` **B.4 New-game entry path** — *Handed to gameflow T3 (New Game) / T4 (starting scripts / location). The hardcoded `SpawnDefaultPlayer` call in [playscreen.cpp:304](../../src/playscreen.cpp#L304) is the stand-in until that lands.*

## Phase C — Combat smoke

Once A + B land, walk the full attack→hit→damage→death cycle against a real enemy.

- `[ ]` **C.1 First-blood test** — spawn Locke + one Araknid via `--test=combat`; player swings; verify `[character] hit / damage / impact` log lines. Catches all the gaps that don't fail loudly.
- `[ ]` **C.2 Hit-reaction + stagger** — [character.cpp:1033-1080](../../src/character.cpp#L1033) plays the impact anim; verify the looptime in [../COMBAT.md](../COMBAT.md) maps correctly to frames and stuns the attacker as intended.
- `[ ]` **C.3 Death + corpse persistence** — Araknid dies, corpse stays in sector. Verify it doesn't despawn on next sector load (currently `RemoveObject` only fires explicitly, so this should work — confirm with a sector cross).
- `[ ]` **C.4 Player death** — Locke HP → 0. *Death+restart flow is gameflow T7; this entry tracks only the combat-side death trigger (impact → HP=0 → `IsDead()` flag set → death anim plays).*
- `[ ]` **C.5 XP on kill** — no caller awards XP ([player.h](../../src/player.h) `Exp()` accessor exists, no writer in combat death path). Add `Player->AddExp(amount)` call in the attacker's death handler. Amount from rules.def per [../COMBAT_RULES.md](../COMBAT_RULES.md).
- `[ ]` **C.6 Level up** — when `Exp >= NextLevelExp`, bump `Level`, restore HP/MP, log to text bar. Skill / stat advancement model per [../OBJECT_STATS.md](../OBJECT_STATS.md).
- `[ ]` **C.7 Multi-enemy** — three Araknids in a clearing. Verify AI target acquisition ([character.cpp:2444-2472](../../src/character.cpp#L2444-L2472)) picks them up and combat continues without deadlocks.
- `[ ]` **C.8 Combat SFX cues fire** — now that gameflow has the SFX registry populated from `resources.rvr` ([../gameflow/BURNDOWN.md](../gameflow/BURNDOWN.md) T0), confirm sword swings / hit impacts / Araknid death actually play their sounds during C.1. The SFX names are bound in `class.def` / `rules.def`; this is just "does the play call reach the audio backend." If broken, file under gameflow.

## Phase D — Sector transitions / System 19 *(handed to gameflow T10)*

The whole TExit / door auto-activate / "Misthaven return" cycle is gameflow's T10. The combat side cares about two things, tracked here as cross-cuts:

- `[ ]` **D-cross.1 NPC AI cleanup on sector unload** — when a sector unloads mid-combat, enemy AI ticks need to stop firing on the unresident objects. Likely a one-line early-out in `TCharacter::AI()`; the actual sector-paging plumbing belongs to gameflow.
- `[ ]` **D-cross.2 Combat-mode preservation across sector cross** — if Locke is in combat when he steps on an exit, does combat mode survive the teleport? Probably should not (no enemy on the other side), but worth a smoke test once gameflow's exit plumbing lands.

Original D.1-D.6 items (sector paging, door + lever interaction, press plates) are owned by gameflow.

## Phase E — Save / load *(handed to gameflow T5/T6/T7)*

Gameflow owns the save-game data model (T5), the load/save UI + slot management (T6), and the death/restart loop (T7). Combat-side cross-cuts:

- `[ ]` **E-cross.1 Per-character serialization parity** — when gameflow lands NPC/world persistence, verify combat state (current target, current attack, fatigue tick, impact stagger remaining) round-trips. If it doesn't, the combat code may need new save/load hooks. *Track in this file once gameflow's persistence model is decided.*

Original E.1-E.4 items (quicksave bindings, save indicator toast, persistence model decision, version compat) are owned by gameflow.

## Phase F — Script runtime *(handed to gameflow T8)*

Gameflow owns the scripting engine bring-up. Combat-side cross-cuts:

- `[ ]` **F-cross.1 Script-callable combat actions** — `Player->BeginFighting / Cast / Go / Stop / Say` are C++ entry points the script VM dispatches into. Verify the function signatures the gameflow VM needs match what we expose. *Audit only — no work unless gameflow flags a mismatch.*

Original F.1-F.5 items (opcode inventory, missing-opcode port, trigger fire-through, Cast/Say/Wait, GameState read/write) are owned by gameflow.

## Phase G — Inventory / equipment runtime (input side)

UI side is in [../ui/BURNDOWN.md](../ui/BURNDOWN.md) B.6/B.7. Game-state side lives here.

- `[ ]` **G.1 Pick up item from world** — Locke walks over a dropped item, presses Use. Adds to inventory via `TObjectInstance::AddToInventory`.
- `[ ]` **G.2 Drop item to world** — `GAMECMD_INVDROP` ([playscreen.cpp:104](../../src/playscreen.cpp#L104)). Spawns the object in the sector at Player's feet, removes from inventory.
- `[ ]` **G.3 Use consumable** — `GAMECMD_INVUSE` ([playscreen.cpp:102](../../src/playscreen.cpp#L102)). Potion → restore HP, scroll → cast spell, etc. Per-object `Use()` virtual in [src/object.h](../../src/object.h).
- `[ ]` **G.4 Move equipped item between slots** — `GAMECMD_INVMOVE` ([playscreen.cpp:103](../../src/playscreen.cpp#L103)). Swap-on-conflict semantics from retail.
- `[ ]` **G.5 Container interaction** — chest → contents listed in inventory pane. `TContainer` exists. Verify pickup loop.

## Phase H — Boss AI (post-Demo 1)

Demo 1 only has Araknids. Boss work is for the full game.

- `[ ]` **H.1 Solifuge AI** — TODO stub at [character.cpp:2435](../../src/character.cpp#L2435).
- `[ ]` **H.2 Jhaga AI** — same stub.
- `[ ]` **H.3 Baez AI** — same stub.
- `[ ]` **H.4 Yhagoro AI** — same stub.

Each needs reading the retail behavior out of recon/. Defer until after Demo 1 is playable end-to-end.

## Risks / unknowns

- **Mode-bit lifetime in ControlMap.** The per-mode keys (CTRL_COMBATMODE) only fire when the bit is set; need to verify mode transitions in A.5 don't strand keys mid-press.
- **Cross-track seams.** Several phases now have D-cross / E-cross / F-cross items — these are intentional thin slices that activate when gameflow lands the matching feature. Watch the gameflow burndown.
- **Inventory UI dependency.** A.6 / G.* require something to draw the inventory; until UI worktree B.6 lands, the game-side wiring can only smoke-test via logs.

## Cross-references

- Design intent: [../COMBAT.md](../COMBAT.md), [../COMBAT_ATTACKS.md](../COMBAT_ATTACKS.md), [../COMBAT_RULES.md](../COMBAT_RULES.md), [../MOVEMENT.md](../MOVEMENT.md), [../SCRIPTING.md](../SCRIPTING.md), [../OBJECT_SYSTEM.md](../OBJECT_SYSTEM.md), [../OBJECT_STATS.md](../OBJECT_STATS.md), [../OBJECT_TYPES.md](../OBJECT_TYPES.md), [../SAVE_GAME.md](../SAVE_GAME.md).
- Master plan: [../PORT_PLAN.md](../PORT_PLAN.md) Phase 4 (Feature recovery).
- **Gameflow burndown:** [../gameflow/BURNDOWN.md](../gameflow/BURNDOWN.md) — owns audio, save/load, scripts, exits/doors, menus, new game, death+restart, dialog. Cross-watched from Phases C.4 / C.8 / D / E / F here.
- **UI burndown:** [../ui/BURNDOWN.md](../ui/BURNDOWN.md) — especially B.11 (`GAMECOMMAND` dispatch un-stub) which overlaps Phase A.6/A.7 here.
