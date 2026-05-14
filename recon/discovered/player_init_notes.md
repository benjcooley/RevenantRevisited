# Player initialization / new-game flow (retail)

## TL;DR

Retail does **all** new-character setup in one function: `TPlayer::ClearPlayer`
at `0x518750`. This function is called from:

1. The TPlayer constructor (at `0x518450`) — runs every time a TPlayer object
   is constructed.
2. `TPlayerManager::Clear()` (at `0x51eda0`) — re-runs Clear on every existing
   player in the manager (called from TLogoScreen::Initialize, LoadGame, and
   multiplayer disconnect).

Stats come from `chardata->classdata->statreqs[6]` (the SClassData per-class
stat allocation table loaded from `rules.def`/`char.def`):

* `statreqs[i] != 0` → that stat is hardcoded to `|statreqs[i]|`.
* `statreqs[i] == 0` → that stat gets one share of the remaining "stat budget"
  (target total = `0x54 = 84`). All-zero classdata → 6 stats of ~14 each.

Skills are blanket-initialized: all 11 skills (`SK_ATTACK..SK_LOCKPICK`) start
at value **30**. Skill mods come from `classdata->skillmods[]` and are added
on read (`SkillPcnt`/`SkillMod`), not stored in the OBJSTAT array.

`Level` and `Exp` are **NOT** set by ClearPlayer. The retail "new game" path
relies on `data/Modules/Demo/newgame.sav` having a pre-baked Locke with
Level=1, full Health/Fat/Mana, sword equipped, etc. The TPlayer object that
the constructor produces is essentially a blank slate that the savegame then
overwrites.

This means our snapshot has TWO viable porting routes:

* **Route A (compatibility)** — emulate retail by populating the player from
  classdata in TPlayer::ClearPlayer (stats + skills) **and** also setting
  `Level=1`/`Exp=0` (not done by retail's ClearPlayer because newgame.sav
  carries those values, but we don't always load that save).
* **Route B (parity)** — load `data/Modules/Demo/newgame.sav` in our boot
  path. Requires porting TPlayer::Load (already partially done — see below)
  and the savegame container format. More work, but matches retail exactly.

We recommend **Route A** for the immediate fix, with the caveat that retail's
starting equipment ("Locke" with Stoss sword + leather armor + bow + ammo)
must come from somewhere else entirely (newgame.sav or a hardcoded list in
the demo intro script). Route A leaves Locke unarmed but at full stats.

---

## Full call chain — main menu "New Game" → in-game Locke

```
TLogoScreen::Initialize (0x53a2c0)
  ├─ TPlayerManager::Clear (0x51eda0)              [empty player list]
  │   └─ for each existing player:
  │       ├─ TCharacter::ClearChar (0x4c18a0)
  │       └─ TPlayer::ClearPlayer (0x518750)
  ├─ creates menus.dat buttons:
  │   ├─ "MenuNewGame"   → callback: FUN_0048e610 (LoadNewGame wrapper)
  │   ├─ "MenuLoadGame"  → callback: FUN_0048df70 (LoadGame)
  │   └─ ...
  └─ ... cursor / version-text setup

[user clicks "MenuNewGame"]

LoadNewGame (0x48e610)
  └─ LoadGame("newgame", 1) (0x48df70)
       ├─ open <data root>/Modules/Demo/newgame.sav
       ├─ read 0x80-byte header (game state) into DAT_00668300
       ├─ optional 0x200-byte multiplayer block
       ├─ read body buffer
       ├─ for each serialized object:
       │    ├─ CreateObjectFromStream (0x471ce0)
       │    │     ├─ read class+type+offsets
       │    │     ├─ TObjectClass::NewObject (0x474bb0) — fires TPlayer ctor
       │    │     │     ├─ TCharacter::TCharacter (0x4c16b0) → ClearChar
       │    │     │     │   loads chardata via FUN_0048c780(objtype, objclass)
       │    │     │     ├─ allocate equip[1] + quickspells[4]
       │    │     │     ├─ set vtable to 0x5b4f30
       │    │     │     └─ TPlayer::ClearPlayer (0x518750)  ← stats applied
       │    │     │           ├─ stat budget split via classdata->statreqs
       │    │     │           ├─ all 11 skills = 30
       │    │     │           ├─ talisman list rebuilt
       │    │     │           └─ TPlayer::RefreshStats (0x51c660)
       │    │     │                 sets MaxHealth/Fatigue/Mana on chardata,
       │    │     │                 applies equipment-slot modifiers
       │    │     └─ vt[Load](version, objversion) = TPlayer::Load (0x51b960)
       │    │         OVERWRITES stats from save: equipment ptrs, quickspells,
       │    │         body angles (+0x304..0x310), state bits (+0x360..0x368),
       │    │         "active spell" string (+0x494..0x4f0), Exp (+0x370/0x374),
       │    │         talismans, position (+0x668..0x670 = +0x10..0x18 copies)
       │    ├─ TPlayerManager::AddPlayer(p, -1) (0x51f0a0)
       │    ├─ TPlayer::SetPlayerState (0x51d680) — clears combat bit, sets idle
       │    └─ if (mainplayer flag) TPlayerManager::SetMainPlayer(p) (0x51f060)
       └─ FUN_004609f0 — load saved current sector
```

The character-creation flow (multi-player, character-edit screen
`createcharnotex.dat` interface) takes a different path:

```
TCharCreateScreen::Initialize (0x465a50)
  ├─ NewObjectByName("Locke", ...) (0x474e20)        [player slot 2 path]
  │   └─ TObjectClass::NewObject (0x474bb0) → TPlayer ctor as above
  └─ stash player in scope at param_1[0x7e]
```

This path skips the savegame (because the user is creating a *new* character
about to be saved), so the player has only the ClearPlayer-derived stats —
no equipment, no level set. The character-creation UI then walks the user
through stat allocation and writes back to the player object.

---

## What ClearPlayer (0x518750) sets — annotated decompile

```c
// Retail TPlayer::ClearPlayer — pseudocode based on Ghidra body.
// Field offsets are byte offsets into the TPlayer object.
void TPlayer::ClearPlayer()
{
    field_0xde = 0;             // some flag (likely OnTheHog or similar)
    field_0xe6 = field_0xe7 = 0;
    field_0x199 = -1;           // sentinel index

    memset(&field_0x124, 0, 24*4);   // 24 ints — animation state cache?
    memset(&field_0x15c, 0, 56*4);   // 56 ints — combat state buffer
    field_0x194 = field_0x195 = field_0x196 = field_0x197 = 0;

    memset(equipment[0..10], 0, 11*sizeof(void*));   // equipment slots

    flags |= 0x80000;           // mark "non-map" / "always live"
    memset(&field_0xb3, 0, 7*4);  // ?
    field_0xc0 = 0;             // talisman count?

    // Drop existing temporary spell list
    for (i in talismans) free(talismans[i]); talismans.clear();

    // Reset render flags via virtual call (vt[0x40] = SetFlags)
    self->SetFlags(flags & ~0x4000000);

    field_0xd9 = field_0xd8 = field_0xda = 0;

    // ---- STAT BUDGET DISTRIBUTION ----
    int *statreqs = chardata->classdata->statreqs;   // SClassData.statreqs[6]
    int target = 0x54;   // 84
    for (i = 0; i < 6; i++) target -= abs(statreqs[i]);
    // remaining_per_zero_stat in 16.16 fixed-point:
    fixed budget_per = (target << 16) / 6 + 1;
    fixed acc = budget_per;
    for (i = 0; i < 6; i++) {
        int v;
        if (statreqs[i] == 0) {
            v = acc >> 16;
            acc += budget_per - (acc & 0xffff0000);  // distribute remainder
        } else {
            v = abs(statreqs[i]);
        }
        SetObjStat(0x22 + i, v);   // PLRSTAT_FIRST + i
    }

    // ---- SKILL INIT ----
    for (i = 0; i < 11; i++)
        SetObjStat(0x49 + i, 30);   // SK_FIRST + i = baseline 30

    // ---- TALISMAN / QUICKSPELL REBUILD ----
    free(talismans_array);
    talismans_count = 0;
    talismans_capacity = 0;
    int total_spells = chardata->classdata->numspells;  // chardata[0x12] +0x1c
    for (i = 0; i < total_spells; i++) {
        if (FUN_0051ff30(i)) {           // matches some criteria
            if (capacity == count) FUN_0051ffa0(-1);  // grow capacity
            talismans_array[count++] = { id=i, modifier=0 };
        }
    }

    RefreshStats();   // 0x51c660 — recompute MaxHealth/Fatigue/Mana etc.

    field_0x13c = 0;    // ai/state byte
    field_0xdb = 1;     // ?
    SomethingWithObject(&DAT_005e2714, 0);  // FUN_00472db0 — clears region?

    field_0xdd = global_frame_counter;
    field_0xdc = global_other_counter;
    field_0x198 = 0;

    // Clear attack-impact log: 10 entries × 6 ints each
    memset(&field_0xe8, 0, 10*6*4);

    // Free quickspell entries (re-init to 0 capacity)
    for (i = 0; i < quickspells_count; i++) free(quickspells[i].name);
    quickspells_count = 0;
    quickspells_capacity = 0;
}
```

### Critical observations

* **Level (PLRVAL_LEVEL = 6) and Exp (PLRVAL_EXP = 7) are NOT set.** Retail
  expects newgame.sav to carry them.
* **Health / Fatigue / Mana current values are not explicitly set here**;
  `TPlayer::RefreshStats` computes the *Max* values, but current Health
  is whatever ClearChar (TCharacter version) already seeded via
  `vt[0x314]()` which reads MaxHealth. Net effect for a fresh ctor: current
  values *equal* max values, computed from default classdata.
* **No starting equipment.** The 11 equipment slots are `memset` to 0.
  Locke's starting sword/armor/bow comes from newgame.sav.
* **No talismans / pre-defined quickspells.** Quickspell array re-init to
  empty.
* **No stat-pcnt cache or derived modifiers** — those are recomputed on
  every read in our source via `PlyrStatPcnt` etc.

---

## TPlayer::Load (0x51b960) — savegame deserialization

The Load body is a **versioned tape player**:

| version | reads / sets |
|---|---|
| `>= 4` | super::Load (TCharacter @ 0x4d4eb0); reset version mask |
| `>= 4` | clear strings at +0x2cc, +0x2d2, +0x2d8, +0x2de, +0x2e4 (5×6-byte buffers — 4 quickspell-buttons + construct? matches MAXTALISMANLEN=6, QSPELL_NUM=5) |
| `>= 5` | int N = readInt; for (N) { allocStr(6); readBytes(6); FUN_0041c840(str); } — populates the pre-game spell list |
| `5..8` (legacy) | skip N×4 bytes (old format) |
| `>= 7` | read 4 ints into +0x304..0x310 (S3DPoint + extra = body angles?) |
| `>= 8` | read 3 ints into +0x360..0x368 (player state bits / sneak/stealth flags) |
| `>= a..c` (`>=10 && <11`) | read 0x32 bytes into +0x494 (active-spell name) |
| `>= b` (`>=11 && <13`) | read 4-byte int into +0x4d8 (active spell variant?) |
| `>= d` (`>=13`) | read 3 ints into +0x4d8/+0x490/+0x4dc + read 0x32-byte string at +0x4f0 (full spell-cast state record) |
| `>= d` | read 2 ints into +0x36c (state) and +0x370 (Exp); compute +0x374 = Exp*24/100; read N×8-byte talismans+modifiers; read 2 ints +0x650/+0x654; (`>=15`) read 2 more ints +0x658/+0x65c |
| `>= e` (`>=14`) | call FUN_00529770 — slot for an extra subsystem (CharacterCreate? class data?) |
| **always** | copy position(+0x10..+0x18) → cached_pos(+0x668..+0x670); call RefreshStats (0x51c660) |

**Important:** Load does NOT call ClearPlayer first. Load assumes ClearPlayer
has already run in the constructor — so it incrementally overwrites whatever
the ctor seeded. The Load result for a fresh `newgame.sav` is therefore:

* Stats from classdata (unchanged from ClearPlayer)
* Skills all 30 (unchanged from ClearPlayer)
* Health/Fat/Mana max from RefreshStats
* Plus whatever the savegame writes: Exp, equipment ptrs (resolved by parent
  TCharacter::Load via item id-list), quickspell name bytes, body-orient,
  cast state.

---

## Side-by-side vs our TPlayer::ClearPlayer

| Behavior | Retail (`0x518750`) | Ours (`src/player.cpp:108`) |
|---|---|---|
| Clear equipment slots | `memset(11 slots, 0, 8*11)` | `memset(NUM_EQ_SLOTS=11, 0, ...)` ✅ |
| Clear quickspells | `quickspells.count=0; cap=0; free(array)` | `memset(QSPELL_NUM*MAXTALISMANLEN)` 🟡 (different repr but same effect) |
| Set OF_NONMAP | `flags |= 0x80000` | `flags |= OF_NONMAP` ✅ |
| OnTheHog = false | (implicit via field_0xde = 0) | `OnTheHog = false` ✅ |
| **Stat budget from classdata->statreqs** | Yes — distributes 84 points across 6 stats | **MISSING** ❌ |
| **Skills baseline 30** | Yes — all 11 skills set to 30 | **MISSING** ❌ |
| **Talisman/spell list rebuild from chardata** | Yes — walks chardata->numspells | **MISSING** ❌ |
| **RefreshStats / Max-computation** | Yes (0x51c660) | Partial — we just call `SetHealth(MaxHealth())` etc.; no equipment-modifier pass ❌ |
| Level=1 | NO (newgame.sav carries it) | YES (band-aid) 🟡 |
| Animation/combat state buffers | Cleared (0x124/0x15c regions) | Inherited from TCharacter::ClearChar 🟡 |
| Attack-impact log | `memset(10×6 ints)` | inherited 🟡 |

The two big gaps (`❌`) explain the symptom: with `chardata->classdata->
statreqs[]` not propagated into the OBJSTAT array, `PlyrStat(PLRSTAT_CONS)` =
0, so `MaxFatigue() = fatigueperlevel * Level * (100 + 0 + 0) / 100`. With
our band-aid `SetLevel(1)`, that's at least non-zero, but combat skills
(`SK_*` = 0 instead of 30) make every action's calculated fatigue cost
massive — Locke can swing once then runs dry, *if* the attack passes the
gate at all. Once we add the stat-budget + skill-30 init, MaxFatigue and
Skill-derived percentages all reach sensible values.

---

## Recommended port plan

### Phase 1 — Minimum viable Locke (Route A)

In `src/player.cpp`, expand `TPlayer::ClearPlayer` to mirror retail. Add as
sibling helpers in `TPlayer` (private):

```cpp
private:
    void DistributeStartingStats();   // statreqs → PLRSTAT_*
    void InitStartingSkills();        // 11 skills × 30
    void RefreshStats();              // recompute MaxHealth/Fatigue/Mana,
                                      //   apply equipment modifiers
```

`ClearPlayer` body becomes:

```cpp
void TPlayer::ClearPlayer()
{
    memset(equipment, 0, NUM_EQ_SLOTS * sizeof(TObjectInstance*));
    flags |= OF_NONMAP;
    memset(quickspells, 0, QSPELL_NUM * MAXTALISMANLEN);
    OnTheHog = false;

    SetLevel(1);                  // not in retail's ClearPlayer; needed
                                  //   because we don't load newgame.sav
    SetExp(0);

    if (chardata && chardata->classdata) {
        DistributeStartingStats();
        InitStartingSkills();
    }

    RefreshStats();

    // Health/Fat/Mana to full (after Refresh has set the Maxes)
    SetHealth(MaxHealth());
    SetFatigue(MaxFatigue());
    SetMana(MaxMana());
}
```

`DistributeStartingStats`:

```cpp
void TPlayer::DistributeStartingStats()
{
    const int target = 84;            // retail constant
    const int *req = chardata->classdata->statreqs;
    int sum = 0;
    for (int i = 0; i < NUM_PLRSTATS; i++) sum += abs(req[i]);
    // Fixed-point distribution to mirror retail's rounding behavior.
    int64_t budget_per = ((int64_t)(target - sum) << 16) / NUM_PLRSTATS + 1;
    int64_t acc = budget_per;
    for (int i = 0; i < NUM_PLRSTATS; i++) {
        int v;
        if (req[i] == 0) {
            v = (int)(acc >> 16);
            acc += budget_per - (acc & 0xffff0000);
        } else {
            v = abs(req[i]);
        }
        SetObjStat(PLRSTAT_FIRST + i, v);
    }
}
```

`InitStartingSkills`:

```cpp
void TPlayer::InitStartingSkills()
{
    for (int i = 0; i < NUM_SKILLS; i++)
        SetObjStat(SK_FIRST + i, 30);
}
```

`RefreshStats` (initial cut — no equipment loop yet):

```cpp
void TPlayer::RefreshStats()
{
    // No-op in our snapshot: MaxHealth/Fatigue/Mana are virtuals computed
    // on read from chardata + PlyrStatPcnt. No cached fields to refresh.
    // Stub kept for future equipment-modifier pass to mirror retail
    // 0x51c660. Hook EquipPane.SetDirty(true) here so the HUD refreshes.
}
```

### Phase 2 — Equipment / starting kit

If we want Locke to spawn with a sword instead of fists, two options:

* **Hardcode in ClearPlayer** — for each slot in classdata->starting_equip
  (we'd add this field to SClassData), call `NewItemByName(name)` and
  `Equip(item, slot)`. Quick but adds source-side data we don't have today.
* **Load newgame.sav** — port enough of TPlayer::Load + TArea::Load + the
  game.sav container to read the demo's starting state. This is the retail
  behavior and gives us complete fidelity, but requires a streaming-format
  port that's currently outside scope (TArea::Load is partially mapped at
  `recon/classes/cls_TArea_Load_41a4e0.cpp`).

### Phase 3 — Match retail Load semantics

When we port savegame I/O, mirror retail's pattern: ClearPlayer first (in
ctor), then Load incrementally overwrites. Our Load implementation should
NOT re-clear the player.

---

## Files written this pass

* `recon/discovered/cls_0x5b4f30_TPlayer_Load_51b960.cpp` — the main savegame
  load body (versioned).
* `recon/discovered/cls_0x5b4f30_TPlayer_Ctor_518450.cpp` — TPlayer ctor.
* `recon/discovered/cls_0x5b4f30_TPlayer_CtorVariant_518570.cpp` — actually
  TPlayer dtor (Ghidra picked it up alongside the ctor). Kept for reference.
* `recon/discovered/cls_0x5b4f30_TPlayer_ClearPlayer_518750.cpp` — THE init.
* `recon/discovered/cls_0x5b4f30_TPlayer_ResetPlayer_51eda0.cpp` — actually
  TPlayerManager::Clear. Filename is misleading — kept for traceability.
* `recon/discovered/cls_0x5a7b98_TCharacter_ClearChar_4c18a0.cpp` — base
  character clear; called from PlayerManager::Clear and every ctor.
* `recon/discovered/cls_TCharCreateScreen_Init_465a50.cpp` — char-create
  screen ctor that creates a default "Locke" via NewObjectByName.
* `recon/discovered/cls_misc_LoadGame_48df70.cpp` — savegame loader.
* `recon/discovered/cls_misc_NewObjectByName_474e20.cpp` — bsearch
  name-based factory.
* `recon/discovered/cls_misc_CreateObjectFromStream_471ce0.cpp` — used by
  LoadGame and TArea::Load to deserialize tagged objects.
* `recon/discovered/renames/agent_player_init.txt` — rename pairs.
