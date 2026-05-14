# TCharacter port status

Retail vtable: **`cls_0x5a7b98`** (110+ slots). Recon dump: `recon/classes_original/cls_0x5a7b98_TCharacter.cpp`. Our source: [src/character.h](../../../../src/character.h), [src/character.cpp](../../../../src/character.cpp).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Combat / AI core

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Pulse` (virtual) | TBD | character.cpp:181 | 🟡 partial | We have a working Pulse but haven't verified line-by-line vs retail. Unique-string anchor blocked finding it in retail. |
| `AI` | 0x4c8b60 | character.cpp:2284 | 🟡 partial | Full retail body ported (2026-05-09): target validation, periodic sweep, in-range attack-or-sidestep tree, out-of-range Go(angle), wander fallback, sight-loss watchdog tail. TODO: oldab `& 0x01000000` flag mask (TActionBlock layout differs); chardata+0x440 sight-loss tunable; chardata+0x1d8/+0x1dc waitticks bounds (currently reusing attackfreq). |
| `AI_PerMonster` | 0x4c9b70 | — | ⚪ Demo 1: stubbed | Demo 1 only ships Araknid which has NO per-monster overlay (per `araknid_ai_notes.md`). Body in `cls_0x5a7b98_TCharacter_AI_PerMonster_4c9b70.cpp` covers only Baez/Solifuge/Jhaga/Yhagoro. Marked TODO retail in AI() body. |
| `WanderToWaypoint` | 0x4c8b60 inline | character.cpp:2589 | ✅ vetted | Retail-faithful single-search-with-commit; uses `wander_target` (mbr_0x8d) + `wander_commit` (mbr_0x92). Replaces the `cached_waypoint`/`waypoint_retry` band-aid. |
| `Blocked` | 0x4c39d0 | character.cpp:613 | ✅ vetted | Retail-equivalent: walkmap delta + char-blocking pass. Two known retail-only filters not yet folded in (movetopos skip char-block; OF_PARALIZE re-mask) — neither hits Demo 1. |
| `CharBlocking` | 0x4d4db0 | character.cpp:4499 | ✅ vetted | Retail-equivalent. Tighter range (128 vs retail 224); doesn't yet filter on doing->attack INTERACTIVE flag, paralised, sleeping-bit-2, fallen — Demo 1 doesn't hit those branches. |
| `BeginCombat` | TBD | character.cpp | ⚫ not-started | |
| `BeginFighting` | 0x4d3b90 | character.cpp:3873 | ⚫ not-started | |
| `EndFighting` | 0x4d3fd0 | character.cpp:3940 | ⚫ not-started | |
| `SetFighting` | 0x4d4790 | character.cpp:3968 | ⚫ not-started | |
| `IsEnemy` | 0x4c89c0 | character.cpp:2250 | ⚫ not-started | Likely simple — string-list lookup against chardata->enemies. |
| `FindCharacters` | 0x4cd690 | character.cpp:2439 | ⚫ not-started | |
| `FindClosestEnemy` | 0x4c95e0 (FindWaypoint conflict?) | character.cpp:2542 | ⚫ not-started | Recon agents named 0x4c95e0 as `FindWaypoint`; need to disambiguate. |
| `CanSeeCharacter` | 0x4cd540 | character.cpp:2407 | ⚫ not-started | |
| `CanHearCharacter` | TBD | character.cpp:2389 | ⚫ not-started | |

## Resolve* (action dispatch)

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `ResolveAction` | TBD | character.cpp:513 | ⚫ not-started | actionsmap agent flagged: TCharacter does NOT override ResolveAction in retail (slot still points at TComplexObject's trivial base). Refactored away. |
| `ResolveMove` | TBD | character.cpp:1389 | ⚫ not-started | |
| `ResolveAttack` | 0x4c1bb0 | character.cpp:1614 | ⚫ not-started | 5798-byte body decompiled. |
| `ResolveImpact` | TBD | character.cpp:1742 | ⚫ not-started | |
| `ResolveBlock` | TBD | character.cpp:1830 | ⚫ not-started | |
| `ResolveDead` | TBD | character.cpp:1842 | ⚫ not-started | |
| `ResolveCombat` | 0x4c3490 (TPlayer) | character.cpp:1867 | ⚫ not-started | TPlayer override at 0x4c3490; TCharacter base addr TBD. |
| `ResolveCombatMove` | TBD | character.cpp:1998 | ⚫ not-started | |
| `ResolveBowAim` | TBD | character.cpp:2012 | ⚫ not-started | |
| `ResolveBowShoot` | TBD | character.cpp:2025 | ⚫ not-started | |
| `ResolveLeap` | TBD | character.cpp:2060 | ⚫ not-started | |
| `ResolvePull` | TBD | character.cpp:2065 | ⚫ not-started | |
| `ResolveSay` | TBD | character.cpp:2111 | ⚫ not-started | |
| `ResolvePivot` | TBD | character.cpp:2123 | ⚫ not-started | |

## Attack search

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `IsValidAttack` | 0x4d1120 | character.cpp:3290 | ⚫ not-started | |
| `FindButtonAttack` | 0x4d1ff0 | character.cpp:3432 | ⚫ not-started | |
| `FindPcntAttack` | 0x4d1eb0 | character.cpp:3478 | ⚫ not-started | |
| `RandomAttack` | 0x4d2900 | character.cpp:3578 | ⚫ not-started | |
| `SpecificAttack` | 0x4d2a60 | character.cpp:3592 | ⚫ not-started | |
| `DoAttack` | 0x4d2120 | character.cpp:3501 | ⚫ not-started | |

## Movement / actions

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Go(angle)` | TBD | character.cpp:2660 | ⚫ not-started | |
| `Go(vect)` | TBD | character.cpp:2789 | ⚫ not-started | |
| `Goto(x,y)` | TBD | character.cpp:2797 | ⚫ not-started | |
| `Stop` | 0x4cee70 | character.cpp:2811 | ⚫ not-started | |
| `Pivot` | TBD | character.cpp:2941 | ⚫ not-started | |
| `Block` | TBD | character.cpp:3651 | ⚫ not-started | |
| `Dodge` | 0x4d6220 (sidestep) | character.cpp:3711 | 🔴 incomplete | Retail has sidestep dodge with l/r anims; ours is generic. |
| `Leap` | TBD | character.cpp:3609 | ⚫ not-started | |
| `Pull` | TBD | character.cpp:3022 | ⚫ not-started | |
| `Pickup` | TBD | character.cpp:3003 | ⚫ not-started | |
| `Pulp` | TBD | character.cpp:3723 | ⚫ not-started | |
| `Burn` | 0x4d3590 | character.cpp:3757 | ⚫ not-started | |
| `Flail` | TBD | character.cpp:4460 | ⚫ not-started | |
| `KnockBack` | TBD | character.cpp:3795 | ⚫ not-started | |

## Sense / perception

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Hearing(dist)` | TBD | character.cpp:2552 | ⚫ not-started | |
| `Sight(dist)` | TBD | character.cpp:2575 | ⚫ not-started | |
| `Visibility` | TBD | character.cpp:1172 | ⚫ not-started | |
| `Transparency` | TBD | character.cpp:1156 | ⚫ not-started | |
| `ResetStealthValues` | 0x4cdbb0 | character.cpp:2590 | ⚫ not-started | |
| `SetHasSeen` | 0x4c5940 | character.cpp:1110 | ⚫ not-started | |
| `HasSeenMe` | TBD | character.cpp:1097 | ⚫ not-started | |

## Damage / health

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Damage` | TBD | character.cpp:941 | ⚫ not-started | |
| `CalculateDamage` | TBD | character.cpp:927 | ⚫ not-started | |
| `GetDamageType` | TBD | character.cpp:885 | ⚫ not-started | |
| `RestoreHealth` | TBD | character.cpp:1090 | ⚫ not-started | |

## Cast / signal

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Cast` | 0x4d5c20 | character.cpp:4440 | ⚫ not-started | "SPLCASTOK"/"SPLCASTFAIL" anchors. |
| `CastByName` | TBD | character.cpp:4448 | ⚫ not-started | |
| `CastByTalismans` | TBD | character.cpp:4454 | ⚫ not-started | |
| `SetCast` | TBD | character.cpp:4280 | ⚫ not-started | |
| `SignalAttack` | TBD | character.cpp:2622 | 🟡 partial | We added auto-engage; haven't verified vs retail. |
| `SignalHostility` | TBD | character.cpp:2616 | 🟡 partial | 2-line stub. |
| `SignalMovement` | TBD | character.cpp:2612 | 🔴 incomplete | empty body. |

## Container / use

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `UseContainer` | 0x4dd480 | — | 🔴 incomplete | Retail-only: lockpicking dialog flow with CONTWRONGKEY/CONTPICKFAIL/CONTPICKTOUGH/CONTUNLOCK. No source equivalent. |
| `Use` | TBD | character.cpp:4036 | ⚫ not-started | Stub in source — just triggers DIALOG. Retail Use() routes through UseContainer. |
| `TryUse` | TBD | character.cpp:3050 | ⚫ not-started | |
| `TryGet` | TBD | character.cpp:3095 | ⚫ not-started | |

## Stat / state

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `StatLookup` | TBD (virtual) | — | 🔴 incomplete | Retail-only: script-driven stat query by name ("armor"/"maxhealth"/"attackpct"/...). |
| `IsFinalState` | 0x4c8950 | character.cpp:2242 | ⚫ not-started | |
| `IsEnemy` (above) | | | | |
| `Disable` | TBD | character.cpp:2829 | ⚫ not-started | |
| `Jump` | TBD | character.cpp:2838 | ⚫ not-started | |
| `MakeVisible/Invisible` | TBD | character.cpp:4197/4219 | ⚫ not-started | |
| `SetFade/GetFade/UpdateFade` | 0x4d57a0 (UpdateFade) | character.cpp:4239..4280 | ⚫ not-started | UpdateFade verified. |

## Out of scope

| Method | Retail addr | Notes |
|---|---|---|
| `Animate` | TBD | ⚪ Animation engine — label only, do not port. |
| `Move` | 0x4c46d0 | ⚪ Hooks rendering/physics — label only. |

## Mode / root

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `DefaultRootState` | 0x4d6db0 (returns "combat" or default) | character.h:101 | 🟡 partial | |
| `SetWalkMode/SneakMode/RunMode` | TBD | character.cpp:2844..2905 | ⚫ not-started | |
| `IsCombat / IsSneakMode / etc.` | inline | character.h | ⚫ not-started | |
| `GetMoveAction` | TBD | character.cpp:3842 | ⚫ not-started | |
| `GetLeapAction` | TBD | character.cpp:3859 | ⚫ not-started | |

## Bow

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `DrawBow` | TBD | character.cpp:3212 | ⚫ not-started | |
| `AimBow / Left / Right` | TBD | character.cpp:3229..3249 | ⚫ not-started | |
| `ShootBow` | TBD | character.cpp:3255 | ⚫ not-started | |
| `IsBowDrawn` | TBD | character.cpp:3281 | ⚫ not-started | |

## Save / Load

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `Save` | 0x4d50d0 | character.cpp:4177 | ⚫ not-started | 255-line retail body — writes mbr_0x138/13c/140 (health/fatigue/mana). |
| `Load` | 0x4d4eb0 | character.cpp:4121 | ⚫ not-started | Version-branched read. |
| `SaveState` | 0x4dbb80 | — | ⚫ not-started | TComplexObject layer — string-write helper. |

## Diagnostics added during this port

These exist in our source as part of the port-debugging overlay. Out of scope for vetting against retail.

| Symbol | Source | Notes |
|---|---|---|
| `ai_pulse_count`, `ai_ai_count` | character.h:580 | Public counters incremented at Pulse/AI entry. Not in retail. |
| `DoingAction()`, `RootAction()`, `NextAttack()`, etc. | character.h:582+ | Diagnostic accessors. Not in retail. |
