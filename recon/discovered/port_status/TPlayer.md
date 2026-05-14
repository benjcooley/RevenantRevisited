# TPlayer port status

Retail vtable: **`cls_0x5b4f30`** (265 slots = TCharacter's 137 + 128 new). Recon: `recon/classes_original/cls_0x5b4f30_TPlayScreen.cpp` (mislabeled — it's TPlayer). Body decompilations live in `recon/discovered/cls_0x5b4f30_TPlayer_*.cpp`. Our source: [src/player.h](../../../../src/player.h), [src/player.cpp](../../../../src/player.cpp).

51 of TCharacter's slots are overridden + 128 brand-new TPlayer slots (mostly OBJSTAT pairs for player stats).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started.

## TCharacter overrides

| Method | Retail addr | Slot | Our source | Status | Notes |
|---|---|---|---|---|---|
| dtor | 0x51ff00 | 0 | player.cpp | ⚫ not-started | |
| `DistanceTo` | 0x4d61b0 | 4 | player.cpp | ⚫ not-started | Retail adds class-0xb/0xc adjust. |
| `SetMoveTarget` | 0x4d6e00 | 12 | player.cpp | ⚫ not-started | |
| `RemoveItem` | 0x51ec20 | 16 | player.cpp | ⚫ not-started | |
| `Animate` | 0x518aa0 | 272 | player.cpp | ⚪ out-of-scope | Animation hook. |
| `Move` | 0x4c3260 | 528 | player.cpp | ⚪ out-of-scope | Rendering/physics hook. |
| `ResolveCombat` | 0x4c3490 | 540 | player.cpp | ⚫ not-started | |
| `DefaultRootState` | 0x520150 | 544 | player.h | ⚫ not-started | Returns "walk". |
| `CalcDamage` | 0x4c4860 | 548 | player.cpp | ⚫ not-started | resist+protection helper. |
| `ResolveAttack` | 0x4c4950 | 552 | player.cpp | ⚫ not-started | 3776-byte attack-flow. |
| `Damage` | 0x4c5810 | 556 | player.cpp | ⚫ not-started | calls CalcDamage → SetHealth. |
| `Pulse` | 0x4c9790 | 564 | player.cpp | 🔴 incomplete | Combat-engagement frame; helpers FUN_004cf000 / FUN_004cf490 not yet identified. |
| `AI` | 0x4cdcc0 | 568 | player.h:`virtual void AI() {}` | ✅ vetted | 9-line empty stub matches our `virtual void AI() {}`. |
| `MaxHealth` | 0x520630 | 472 | player.h | ⚫ not-started | |
| `MaxFatigue` | 0x5206d0 | 480 | player.h | 🟡 partial | Retail formula divides by 200; our source by 100. Discrepancy. |
| `MaxMana` | 0x520770 | 488 | player.h | ⚫ not-started | |
| `Load` | 0x51b960 | 352 | player.cpp | ⚫ not-started | |
| `Save` | 0x51bdc0 | 356 | player.cpp | ⚫ not-started | Writes ObjVersion=4. |
| `SetPos` | 0x51bd20 | 360 | player.cpp | 🔴 incomplete | Retail propagates position to attached items via stored ptr arrays. Ours doesn't override. |

## TPlayer-only virtuals (no TCharacter equivalent)

| Method | Retail addr | Slot | Our source | Status | Notes |
|---|---|---|---|---|---|
| `StealthMod` | 0x520860 | 744 | player.cpp | ⚫ not-started | |
| `GetCombatRoot` | 0x51b340 | 772 | player.h | ⚫ not-started | "hand"/"combat"/"cstaff"/"cknife". |
| `GetBowRoot` | 0x51b3b0 | 776 | player.h | ⚫ not-started | "cbow"/"bow". |
| `GetTorchRoot` | 0x51b3f0 | 780 | — | 🔴 incomplete | "torch"/"walk". No source equivalent. |
| `GetExperience` | 0x51d800 | 788 | player.h | ⚫ not-started | |
| `SetExperience` | 0x51d810 | 792 | player.h | ⚫ not-started | |

## Non-virtual instance methods

| Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|
| `GetOnYerHog` (cheat) | 0x51b420 | — | ⚪ out-of-scope | Toggles Locke ↔ Hog. Cheat code. |
| `SavePlayerFile` | 0x51e0a0 | — | 🔴 incomplete | Standalone .chr export with "PLYR" magic. Retail-only. |
| `SetPlayerState` | 0x51d680 | — | 🔴 incomplete | Bitfield + HUD/cursor update. Retail-only. |

## OBJSTATFUNC pairs (slots 600-696)

~25 pairs (50 slots) of getter/setter for player-private stats. Retail-only — not all match our `OBJSTAT*` macros yet. Treat as: each pair is a small accessor; we'll port them en bloc when we add the corresponding source-side stats.

Status: 🔴 incomplete (entire range)

## Out of scope

`Animate`, `Move`, plus animation- and rendering-related slots. Label only.
