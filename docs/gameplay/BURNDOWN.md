# Gameplay / Combat / Controls — Burndown

Live status. Update as work progresses. Goal: bring Demo 1 (Locke wakes up at Misthaven fountain, walks to the forest, fights an Araknid, takes a quicksave, exits to the next sector) to end-to-end playable. Adjacent burndowns: [../ui/BURNDOWN.md](../ui/BURNDOWN.md) for the HUD / OOG screens (parallel UI worktree), VFX burndown lives on the VFX worktree.

**Legend:** `[ ]` pending · `[~]` in progress · `[x]` done · `[!]` blocked

Last updated: 2026-05-17 (B.1 landed)

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

## Phase B — Player setup at boot

Without a properly-equipped Player, attacks miss for "no weapon," fatigue is wrong, and damage isn't computed.

- `[x]` **B.1 Auto-equip starter loadout on `SpawnDefaultPlayer`** — done 2026-05-17. Short Sword + Brown Cloth shirt/pants/boots, via the new `EquipStarterLoadout` helper in [playscreen.cpp](../../src/playscreen.cpp). Item names come from the runtime class.def packed in `imagery.rvi` (not the legacy/Class.Def at repo root). Will be superseded when newgame.sav loading lands (Phase E).
- `[ ]` **B.2 Level from save instead of hardcoded 10** — [player.cpp:189](../../src/player.cpp#L189) placeholder makes skill checks pass that shouldn't. Tie to save/load (Phase E) — until then leave the placeholder but log a warning at boot so it doesn't get forgotten.
- `[ ]` **B.3 Inventory pre-population** — Demo 1 expects some starter potions / quest item per [../OBJECT_TYPES.md](../OBJECT_TYPES.md). Concrete list TBD; depends on what the demo script triggers reference.
- `[ ]` **B.4 New-game entry path** — currently boot always goes through `SpawnDefaultPlayer` with hardcoded position ([playscreen.cpp:304](../../src/playscreen.cpp#L304)). Real new-game = create Player + place at the module's start position from `module.def`. The plumbing exists (`TModule.startpos[]` per [[project-module-system]] memory, not yet parsed). *Crosses with main-menu work in UI worktree C.1.*

## Phase C — Combat smoke

Once A + B land, walk the full attack→hit→damage→death cycle against a real enemy.

- `[ ]` **C.1 First-blood test** — spawn Locke + one Araknid via `--test=combat`; player swings; verify `[character] hit / damage / impact` log lines. Catches all the gaps that don't fail loudly.
- `[ ]` **C.2 Hit-reaction + stagger** — [character.cpp:1033-1080](../../src/character.cpp#L1033) plays the impact anim; verify the looptime in [../COMBAT.md](../COMBAT.md) maps correctly to frames and stuns the attacker as intended.
- `[ ]` **C.3 Death + corpse persistence** — Araknid dies, corpse stays in sector. Verify it doesn't despawn on next sector load (currently `RemoveObject` only fires explicitly, so this should work — confirm with a sector cross).
- `[ ]` **C.4 Player death** — Locke HP → 0. What happens? Probably nothing right now. Define: respawn at last save, or "you died" overlay → main menu? Depends on main menu (UI C.1).
- `[ ]` **C.5 XP on kill** — no caller awards XP ([player.h](../../src/player.h) `Exp()` accessor exists, no writer in combat death path). Add `Player->AddExp(amount)` call in the attacker's death handler. Amount from rules.def per [../COMBAT_RULES.md](../COMBAT_RULES.md).
- `[ ]` **C.6 Level up** — when `Exp >= NextLevelExp`, bump `Level`, restore HP/MP, log to text bar. Skill / stat advancement model per [../OBJECT_STATS.md](../OBJECT_STATS.md).
- `[ ]` **C.7 Multi-enemy** — three Araknids in a clearing. Verify AI target acquisition ([character.cpp:2444-2472](../../src/character.cpp#L2444-L2472)) picks them up and combat continues without deadlocks.

## Phase D — Sector transitions / System 19

`TExit` is largely complete ([exit.cpp:342](../../src/exit.cpp#L342), [exit.cpp:403](../../src/exit.cpp#L403)). Risk areas are paging and state preservation.

- `[ ]` **D.1 Sector-paging verify after `TExit::Activate`** — `Player.SetPos` jumps Locke to a new sector. Confirm `TMapRenderer` actually loads the new 3x3 sector window and unloads the old (memory `project-mappane-renderer-split` says renderer owns loaded sectors; verify the SetPos hook reaches it).
- `[ ]` **D.2 NPC AI cleanup on sector unload** — when a sector unloads, do enemy AI ticks halt? Run a smoke where you cross sectors mid-combat; check no AI errors on the old sector. Likely needs an `AI()` early-out for unresident objects.
- `[ ]` **D.3 Sector-cross state snapshot** — `TExit::Activate` doesn't checkpoint Player state. Should it? Decide whether sector cross is implicitly a quicksave point (matches retail behavior?) or just position teleport. Probably just position; document the decision.
- `[ ]` **D.4 Door interaction (`TExit::Use`)** — for doors that need a key/lever, `Openable()` ([exit.cpp:372](../../src/exit.cpp#L372)) gates Activate. Verify a locked door + key item works end-to-end on a demo door.
- `[ ]` **D.5 `TPressPlate` + `TSpikeWall`** ([exit.h:510](../../src/exit.h#L510)) — already implemented, no demo trigger exists. Confirm with `--test=exit-pressplate`.
- `[ ]` **D.6 `TLever` interaction** — door-opening lever ([exit.h:124](../../src/exit.h#L124)). Same smoke pattern as D.5.

## Phase E — Save / load + quicksave

Save I/O works ([savegame.cpp:27](../../src/savegame.cpp#L27)); needs keybindings and the menu screens. Coordinate with UI worktree C.5/C.6.

- `[ ]` **E.1 Quicksave / quickload keybindings** — F5/F9 (or whatever you prefer) → `SaveGame(0)` / `LoadGame(0)`. Single line each in the dispatcher.
- `[ ]` **E.2 Save indicator** — TextBar toast on save success (already prints something? verify).
- `[ ]` **E.3 NPC / world persistence in saves** — currently only Player is serialized ([savegame.cpp:72](../../src/savegame.cpp#L72)). Decide model: respawn all NPCs from class.def on load (cheap, retail-like) vs. persist dead-flags / inventory states (more authentic, more code). Probably the former for Demo 1.
- `[ ]` **E.4 Save-file version handling** — `MAP_VERSION` mismatch fails load with no upgrade path ([savegame.cpp:40](../../src/savegame.cpp#L40)). Document the format; add a one-line "incompatible save" error path that returns to main menu instead of crashing.

## Phase F — Script runtime completeness

`TScriptManager` boots and parses; opcode coverage is unverified. Demo scripts (`demo.s`, `forest.s`, etc.) are loaded per-area at [area.cpp:407](../../src/area.cpp#L407) but may silently skip unimplemented opcodes.

- `[ ]` **F.1 Opcode inventory** — enumerate every opcode the demo scripts actually use (grep the .s files in `data/Modules/Ahkuilon_unzipped/`). Cross-reference with `TScript::Continue` dispatch in [src/script.cpp](../../src/script.cpp). List missing opcodes.
- `[ ]` **F.2 Port missing opcodes per priority** — port the ones the demo scripts use first; others can wait. Each missing opcode logged with `[script] unhandled opcode <X>` so silent failures don't hide.
- `[ ]` **F.3 Trigger fire-through** — confirm `TRIGGER_PROXIMITY`, `TRIGGER_ACTIVATE`, `TRIGGER_DIALOG` actually fire for the player's actions. `TExit::Activate` already calls `Trigger(TRIGGER_ACTIVATE)` ([exit.cpp:344](../../src/exit.cpp#L344)) — find one demo-script trigger and verify it fires.
- `[ ]` **F.4 `Cast` / `Say` / `Wait` opcodes** — the common script vocabulary. Likely partially present; smoke-test by running a known scripted scene from Demo 1.
- `[ ]` **F.5 GameState variable read/write** — `TGameState` ([script.h:259](../../src/script.h#L259)) holds named flags. Verify scripts read + write them and they persist across save/load (E.3 dependency).

## Phase G — Inventory / equipment runtime (input side)

UI side is in [../ui/BURNDOWN.md](../ui/BURNDOWN.md) B.6/B.7. Game-state side lives here.

- `[ ]` **G.1 Pick up item from world** — Locke walks over a dropped item, presses Use. Adds to inventory. Use `TInventory::AddObject` (search needed).
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
- **Sector paging coupling.** D.1 may turn into a deeper refactor if `Player.SetPos` doesn't already trigger sector load (memory says renderer owns sectors but the trigger path isn't confirmed).
- **Script opcode coverage.** F.1 might surface a long tail of missing opcodes; if so, prioritize ruthlessly by what Demo 1 actually invokes.
- **Inventory UI dependency.** A.6 / G.* require something to draw the inventory; until UI worktree B.6 lands, the game-side wiring can only smoke-test via logs.
- **Save persistence model.** E.3 is a design call with downstream impact on F.5 (GameState persistence).

## Cross-references

- Design intent: [../COMBAT.md](../COMBAT.md), [../COMBAT_ATTACKS.md](../COMBAT_ATTACKS.md), [../COMBAT_RULES.md](../COMBAT_RULES.md), [../MOVEMENT.md](../MOVEMENT.md), [../SCRIPTING.md](../SCRIPTING.md), [../OBJECT_SYSTEM.md](../OBJECT_SYSTEM.md), [../OBJECT_STATS.md](../OBJECT_STATS.md), [../OBJECT_TYPES.md](../OBJECT_TYPES.md), [../SAVE_GAME.md](../SAVE_GAME.md).
- Master plan: [../PORT_PLAN.md](../PORT_PLAN.md) Phase 4 (Feature recovery).
- UI burndown (parallel): [../ui/BURNDOWN.md](../ui/BURNDOWN.md) — especially B.11 (`GAMECOMMAND` dispatch un-stub) which overlaps Phase A here.
