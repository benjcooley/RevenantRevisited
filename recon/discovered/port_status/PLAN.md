# Port Plan — bite-sized systems

Each system below is a closed tree rooted at one or two **top-level methods**. The system is **complete** when:

1. The top-level method has been ported line-for-line and marked ✅ vetted.
2. Every method it transitively calls is also ✅ vetted (or marked ⚪ out-of-scope with a stub that satisfies callers, or ✅ vetted-via-parent for trivial helpers).

Status values: ✅ vetted · ✅ vetted-via-parent · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

---

## Systems

### Gameplay core (combat loop)

| # | System | Top-level method(s) |
|---|---|---|
| 1 | **Monster AI + wander** | `TCharacter::AI` (called from `Pulse`) |
| 2 | **Combat resolution** (hit→damage→death; includes combat-move data parsing + match-by-distance) | `TCharacter::ResolveAttack`, `TCharacter::Damage` |
| 3 | **Player input → action** | `TPlayer::Pulse`, `TCharacter::ButtonAttack` / `ButtonAction` |
| 4 | **Per-monster behaviors** | `TCharacter::AI_PerMonster` (`meth_0x4c9b70`) |
| 6 | **Stats / XP / level-up / training** | `TPlayer::SetExperience` + level-up handler + OBJSTAT pairs |

### Movement

| # | System | Top-level method(s) |
|---|---|---|
| 7 | **Character/player movement + nav + stuck detection** | `TCharacter::Move` (uses `Blocked`, `shovedir` shove-around) |

### Items / equipment

| # | System | Top-level method(s) |
|---|---|---|
| 8 | **Inventory** | `TInventory::Add/Remove/Use`, item-class `Use` virtuals |
| 9 | **Spells** | `TCharacter::Cast` family, talisman lookup |
| 10 | **Avatar / paperdoll mesh swap** | equip handler that switches body-part meshes |

### World / scripting / narrative

| # | System | Top-level method(s) |
|---|---|---|
| 11 | **Scripting engine** (gameplay + narrative, dialog driven by scripts) | `TScript::Pulse`/`Continue`, command-table dispatch, `TGameState` |
| 12 | **Save / load** | `TSaveGame::Read/Write` |

### UI / HUD

| # | System | Top-level method(s) |
|---|---|---|
| 13 | **UI shell (out-of-game)** | `TLogoScreen`, main menu, options, save/load dialogs |
| 14a | **Statbars** (HUD health/fatigue/mana bars) | `TPlyrStatusBar::Pulse` / equivalent |
| 14b | **Automap** | `TAutoMap::Pulse` |
| 14c | **Combat / game log** | text bar + ring buffer |
| 14d | **Overhead stat bars + damage text** | per-character HUD overlay |
| 14e | **Dialog overlay UI** | the choice/say UI driven by System 11 |
| 15 | **Scrolls / books** | book/scroll TPane (uses `book.dat`) |

### Glue (small but real)

| # | System | Top-level method(s) |
|---|---|---|
| 16 | **Cursor / mouse-over targeting** | mouse-cursor type lookup + reticle |
| 17 | **Trigger zones / area transitions** | `TArea::Enter/Exit/Pulse`, music/ambient swap |
| 18 | **Effects spawn-side** (blood, sparks, magic FX call sites — *not* their rendering) | `TCharacter::EffectBurst`, etc. |
| 19 | **Doors / exits / entrances** (open/close, level transition, sector entry/exit) | `TExit::Activate`, `TExit::Pulse`, door-`Use` on `OBJCLASS_EXIT` / `OBJCLASS_CONTAINER` |

---

## Demo 1 — combat + exploration demo 🎯

**Scope:** player can navigate the world (walk/run, control keys), execute combo / combat moves, attack and die. Monsters wander, are present in the world (spawn), pursue when they spot the player, attack, and die. Move system works (no stuck-at-rocks). **Doors open and close. Entrances and exits transition correctly between sectors / maps.**

**In-scope systems:** **1, 2, 3, 4 (Araknid only), 7, 19**.

**Out of scope for Demo 1:**
- UI/HUD beyond what currently works (no new statbars, automap, combat log, dialog UI)
- Scripting (no NPC behaviour, no triggers, no dialog) — but exits/doors *without* script behaviour still work
- Effects (rendering — call sites are kept but visual fx may not appear)
- Sound (silent swings OK for Demo 1)
- Level-up
- Save/load
- Spells / inventory / paperdoll
- Per-creature variants beyond Araknid

**"Done" smoke tests:**

1. Load test map. Within 10 seconds, the Araknid in the map walks toward at least one waypoint object (verified via overlay).
2. When Locke comes within sight range, the Araknid changes target to Locke and walks toward him.
3. The Araknid does not freeze at rocks — when blocked, it shoves around obstacles and reaches the player.
4. With Locke wielding a sword (default loadout), pressing the attack key swings; the swing connects when in range; the Araknid takes damage.
5. After enough hits, the Araknid dies and stops moving.
6. Locke can be killed by the Araknid (death anim, controls disable).
7. Combos and combat-moves trigger as the player chains inputs.
8. Player can walk through a door — door plays open animation, becomes passable, closes after.
9. Player can step on an exit object — map transitions to the next sector / level; player position carries over correctly.
10. Going back through the entrance returns to the prior map.

If all 10 pass, Demo 1 is shippable.

**Recon prerequisites (Wave 3):**

- Field-offset cross-reference (`recon/discovered/field_map.md`) — *blocking*. Every port needs this.
- Full AI() body lines we haven't read (lines 100-329 of `cls_0x5a7b98_TCharacter_AI_4c8b60.cpp`)
- AI_PerMonster Araknid branch (which case in `meth_0x4c9b70`'s switch)
- TPlayer Pulse helpers (`FUN_004cf000`, `FUN_004cf490`)
- Walkmap-shove logic (`Move()` lines ~815-859 already in our source — vet vs retail)
- TPlayer ResolveAttack + Damage + CalcDamage chains (already decompiled in `cls_0x5b4f30_TPlayer_*.cpp`)
- TExit / door behaviour: `Activate`, `Pulse`, level/sector transition mechanics (recon for `OBJCLASS_EXIT` instances + interaction)

**Order of attack:**

1. Build the **field-offset map** (1 focused recon agent, ~30 min).
2. Spawn Wave 3 agents in parallel (per-monster Araknid, TPlayer combat input, walkmap nav, AI body completion).
3. Vet System 7 (movement) — leaf-most. Anchors all the "stuck" debugging.
4. Vet System 1 (AI) — port full body now that field-map exists.
5. Vet System 2 (combat resolution) — ports ResolveAttack tree.
6. Vet System 3 (player input) — ports Pulse + ButtonAttack tree.
7. Vet System 4 partial (Araknid only).
8. Run smoke tests 1-7. Iterate.

---

## Staging plan — beyond Demo 1

### Phase B — Stat/equipment-aware combat (Tier 2)

**Adds:** Systems **6, 8, 14a, 14d, 18**.

**What works:** Locke has a real health bar (statbar). Damage numbers float over enemies. Healing potions in inventory restore HP. Killing monsters grants XP toward leveling. Effects (blood spurts) render. Combat feels readable.

**Recon prerequisites:**
- TInventory full hierarchy (item subclasses: weapons, potions, talismans)
- TPlayer XP / level-up trigger logic
- TPlyrStatusBar / overhead damage-text panes
- Effect classes (TBloodEffect, TSparksEffect, etc.)

**Smoke test:** kill 5 Araknids. XP increases. HP bar updates as you take damage. Damage numbers float over hit creatures. Drink potion → HP refills. Press level-up → stat-pick UI (rendered later — for now just stat increment).

### Phase C — Story playability (Tier 3)

**Adds:** Systems **11, 9, 15, 14e, 17, 16**.

**What works:** NPCs talk to Locke. Dialog choices work. Spells cast. Books open and show lore. Areas have music transitions. Mouse-over shows what you're targeting. Locke can use the spell system. Howards1 patrols his waypoints (per [demo.s](data/Modules/Demo/demo.s) script).

**Recon prerequisites:**
- TScript::Continue + full command table (175 entries)
- TGameState full read/write
- Dialog UI screens
- TBookScreen / book.dat layout
- TArea trigger semantics

**Smoke test:** load Demo module map, walk past HOWARD1, he says his greeting, dialog choices appear and select correctly. Cast a fireball. Open a book.

### Phase D — Shippable (Tier 4)

**Adds:** Systems **12, 13, 10, 4 (full)**.

**What works:** Main menu lets player launch new game / load game / options. Save game persists state and loads back. Equipping a sword shows the sword on Locke (paperdoll). Every monster type in the demo has its retail-faithful behavior.

**Recon prerequisites:**
- TSaveGame format identified (recon agent will chase)
- TLogoScreen/menu vtable + full button wiring
- Paperdoll mesh-swap pipeline
- Each remaining monster's AI_PerMonster branch (Druhg, Ogrok, Issathi, Solifuge, Spider Queen, Lurker, Rahul, Monk, Golems...)

**Smoke test:** launch → main menu → new game → play 30 min → save → quit → relaunch → load → resume from same state. Equip changes visible.

---

## PR / community staging

We have a small audience already watching the project (~20 followers). The staging plan ties releases to community moments so each tier produces something **shareable** — both internally to keep the existing watchers engaged, and externally when Tier 1 lands as the launch moment.

### Tier 0 → "still here" updates (now)

Audience: existing 20 followers.

- Brief update notes when a system or port milestone lands. Markdown-friendly, low-effort, technical.
- Post in whatever channel they're already watching (existing dev blog / discord / mailing list).
- No public launch yet.

### Tier 1 → public launch 🚀 ("27 year old game back to life")

Audience: Reddit (r/gamedev, r/programming, r/Revenant if it exists, r/macgaming, r/retrogaming) + organic spread.

Trigger: **Demo 1 smoke tests 1-10 pass**. Combat is fightable. Doors and exits work. There's a video that conveys "this is alive again."

**Materials we need before posting:**

- 30-90 second gameplay video showing:
  - Locke walking, running through a map
  - Combat encounter with Araknid (engagement, swings, kills)
  - Going through a door / level transition
- Screenshots, including one with clearly recognizable retail visuals
- A blog/Reddit post draft covering:
  - What Revenant was (1999 ARPG, Cinematix, ahead-of-its-time isometric+3D)
  - Why this project exists (personal — original dev, modern macOS, port from pre-release source dump)
  - How it's built (Ghidra-assisted reverse engineering of retail; multi-agent recon pipeline; line-by-line vetting against retail; the rendering replaced with modern shaders, gameplay code ported from the original 1998 source + retail diffs)
  - What works now (combat, exploration, doors)
  - What's coming next (Tier 2/3/4 staging — gives readers reason to follow)
- Repo state: README updated, build instructions current, screenshots pinned
- A landing target — either a project page or a pinned post that aggregates updates

**Risk to manage at launch:**
- Stability: nothing kills a launch like a viewer downloading the build and crashing in 30 seconds. Smoke tests should include a 10-minute stability run before the post goes up.
- Legal positioning: be clear about the source-data origin (pre-release leak vs retail), what's reverse-engineered, and what's user-supplied (assets — players need their own retail copy).

### Tier 2 → "now it feels like a game" (devblog cadence)

Trigger: **statbars + damage text + inventory + XP** (Tier 2 systems land).

Posts:
- "Adding the HUD" — short post showing health bars / damage numbers
- "Inventory works" — picking up items, drinking potions
- "First level-up" — XP curve + stat-pick

Cadence: every system that lands gets a post. Aim every 1-2 weeks at this point so the audience grows organically.

### Tier 3 → "story playable" (bigger marketing moment)

Trigger: **scripting + dialog + spells** (Tier 3 systems land). NPCs talk. The player can complete a real quest segment.

This is the second Reddit-worthy moment. By now we're showing the *game* not just the engine.

### Tier 4 → beta release

Trigger: **save/load + main menu + paperdoll + all monsters** (Tier 4). The full demo module (maybe Ahkuilon) playable end-to-end.

Public beta — distribute to the early-access watchers + open broader sign-up. Collect feedback systematically.

---

## Beta-test readiness criteria

- All systems above marked ✅ vetted (or ⚪ out-of-scope with documented stubs).
- `recon/discovered/port_status/*.md` shows zero 🔴 incomplete or ⚫ not-started rows in the gameplay-core systems.
- Full demo path playable start to end on all maps in the active module.
- 30-minute play session with no crashes, no soft-locks, no missing animations, no infinite loops.
- Save/load round-trips cleanly.
- All combat moves / combos / spells / scripted NPCs verified per smoke tests.

---

## How "vetted" works in practice

For each method in scope:

1. Open the retail decomp body in `recon/discovered/cls_*<method>*.cpp` (or in `recon/classes_original/cls_0x...`).
2. Open our source method.
3. Compare line-by-line. The decomp uses `mbr_0xXX` field offsets; check the field map (`recon/discovered/field_map.md`) to translate.
4. If they match (or only deviate in named, intentional ways) → set status `✅ vetted` with date + initials.
5. If we're missing logic → port it. Re-vet.
6. If a callee isn't yet vetted, the parent can still be marked `✅ vetted` only when the callee itself is — system completion is bottom-up.

For trivial accessors (one-line getters/setters, obvious math like Distance/Radius): mark `✅ vetted-via-parent` when the calling method is vetted; one note covers the helper.

For batch-able sets (script command table, OBJSTAT getter/setter pairs): vet as a batch — read all retail bodies once, port together, mark batch ✅ vetted.

Use `git grep -E "🔴 incomplete|⚫ not-started|🟡 partial" recon/discovered/port_status/` to find work remaining.
