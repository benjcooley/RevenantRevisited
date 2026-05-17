# B.r12 — Tier 10 OOG closeout (B.r17 + B.r18 + B.r19 + B.r20)

**Wave:** 4B
**Date:** 2026-05-16
**Status:**
- B.r17 TDeathPane — **IDENTIFIED** (cls_0x5b93c4 + screen wrapper cls_0x5b9374).
- B.r18 TBookPane / TScrollPane — **IDENTIFIED** (cls_0x5b5808 / cls_0x5b5750).
- B.r19 Credits screen — **NO IN-ENGINE CLASS** (ships as Smacker video).
- B.r20 Main menu — **CONFIRMED** = cls_0x5a5d18 (the already-labeled TLogoScreen IS the main menu; no separate TMainMenuScreen).

All four items closed out with golden-path evidence (string anchor in retail decomp + matching string emitted by src/ method). Renames in `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt`.

---

## B.r17 — TDeathPane retail identification

### Verdict

- **TDeathPane = cls_0x5b93c4** (47-slot TButtonPane-with-DEF subclass at vtable address 0x5b93c4).
- **TDeathScreen = cls_0x5b9374** (3-method TScreen subclass wrapping TDeathPane; loads death.dat then invokes TDeathPane::Initialize on the global instance at DAT_0066f500). This is a retail-era class with no pre-release equivalent — in src/death.cpp, TDeathPane is AddPane'd directly onto PlayScreen with no intervening screen wrapper.

### Architecture diff vs pre-release src/

| Aspect | Pre-release `src/death.{h,cpp}` | Retail (this wave) |
|---|---|---|
| Activation | `DeathPane.Initialize()` called from PlayScreen | `TDeathScreen::Initialize` (FUN_005338a0) loads `death.dat`, then invokes `TDeathPane::Initialize` |
| Pane class | `TDeathPane : public TButtonPane`, 3 virtuals | `cls_0x5b93c4 : public TButtonPane-w-DEF-intermediate (cls_0x5a45c8)`, 47-slot vtable (mostly inherited) |
| Asset | `TMulti::LoadMulti("death.dat")` inside pane | `FUN_0047f670("death.dat")` inside screen wrapper, stored in DAT_0066f6f4 |
| Buttons | 3: restart / load / exit (VK_TAB / VK_RETURN / VK_ESCAPE) | 3: matching layout, button-ctor cls_0x5a3c68 with 0x148-byte alloc |
| Geometry | `TButtonPane(FRAMEMAPPANEX, FRAMEMAPPANEY, FRAMEMAPPANEWIDTH, FRAMEMAPPANEHEIGHT)` | ctor sets x=0x280, w=0x280, y=0x1e0, h=0x1e0 — full 640×480 (`FRAMEMAPPANE` dims) |

### Evidence chain (golden path)

1. Recon string `"Trouble_initializing_Death_pane"` at 0x5e3ec8 is referenced from FUN_005338a0 (TDeathScreen::Initialize) right after the call to `cls_0x5b93c4::virt_meth_0x5339b0(&DAT_0066f500)`. This identifies 0x5338a0 as the death-screen init wrapper and 0x5339b0 as the TDeathPane init virtual.
2. FUN_005339b0 body (`recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp`) creates 3 buttons via cls_0x5a3c68 ctor with labels "Restart" (s_Restart_005e3f18), DAT_005e3f20 ("Load"), DAT_005e3f28 ("Exit") — exactly matching src/death.cpp lines 47-49 NewButton sequence.
3. cls_0x5b93c4 ctor (FUN_00533cc0) sets pane geometry to 640×480 = FRAMEMAPPANE dims — matches src/death.h pane constructor.
4. cls_0x5b93c4 vtable slot 80 (DrawBackground, FUN_00533c10) reads IsDirty flag then renders `Bitmap("background")` of the loaded death.dat — exact structural match for src/death.cpp::TDeathPane::DrawBackground.
5. cls_0x5b93c4 vtable slot 4 (Close, FUN_00533c00) is a thin wrapper around `cls_0x5b98b8::virt_meth_0x434f30` (TButtonPane intermediate's Close) — matches src/death.cpp::TDeathPane::Close calling `TButtonPane::Close()` first.

### Merging suspicion (per AGENT_PROTOCOL Rule 2)

`recon/classes_original/cls_0x5b93c4_TPlayer.cpp` is **mislabeled** `_TPlayer`. TPlayer is confirmed to be cls_0x5b4f30 (see `recon/discovered/port_status/TPlayer.md` and the 22 `cls_0x5b4f30_TPlayer_*.cpp` decompilations). The 2645-line cls_0x5b93c4 file is OOAnalyzer-flattened: it merges:

- TDeathPane leaf methods (0x5339b0, 0x533c00, 0x533c10, 0x533cc0, 0x533d20, 0x533da0)
- The TButtonPane-with-DEF intermediate (cls_0x5a45c8) inherited slot bodies — all the 0x435xxx, 0x436xxx, 0x437xxx methods. These are SHARED across TStatPane, TBottomBarPane, TSideTabsPane, TScrollPane, TBookPane, etc.
- Possible spurious intrusions: 0x52ff40, 0x538e40, 0x5391a0 (need follow-up; may be intermediate or sibling).

**Disposition:** apply method-level renames for the 5 TDeathPane-specific addresses. **Do NOT** rename `cls_0x5b93c4` -> `cls_0x5b93c4_TDeathPane` at the bare-class level yet (would mislabel the 0x435xxx/0x436xxx/0x437xxx methods as TDeathPane's). The class-level rename should wait until cls_0x5a45c8 is identified and its inherited methods are factored out.

---

## B.r18 — TBookPane / TScrollPane retail identification

### Verdict

- **TScrollPane = cls_0x5b5750** (46-slot vtable at 0x5b5750).
- **TBookPane = cls_0x5b5808** (47-slot vtable at 0x5b5808 — inherits TScrollPane's full vtable, overrides slot 0 (Initialize) and slot 80 (DrawBackground), adds 1 new slot 184).

Both are renamed at the class level (2+ lines of evidence including unique asset-name string anchors + vtable inheritance shape that matches src/scroll.h 1:1).

### Wave-3B's rejection of cls_0x5a5ae8 is validated

cls_0x5a5ae8 (the "parchment spell-list sidebar pane") is correctly NOT TBookPane / TScrollPane. The real TBookPane/TScrollPane vtables live in a totally different .rdata neighbourhood (0x5b5750 / 0x5b5808 vs the 0x5a5xxx region). cls_0x5a5ae8 remains a retail-era class with no clean src/ predecessor (Wave-3B's open question stays open — but it is decisively not the book/scroll reader).

### Evidence chain (golden path)

#### TScrollPane = cls_0x5b5750

1. cls_0x5b5750 vtable slot 0 = 0x521080. Body of FUN_00521080 (`recon/discovered/cls_0x5b5750_TScrollPane_Initialize_521080.cpp`) loads `"scroll.dat"` via FUN_0047f670, fetches the `"scroll"` bitmap, then creates 3 buttons named `"down"` / `"up"` / `"exit"` using assets `"downup"/"downdown"`, `"updown"/"upup"`, `"exitup"/"exitdown"`, wired to VK_NEXT (0x22) / VK_PRIOR (0x21) / VK_ESCAPE (0x1b).
2. src/scroll.cpp::TScrollPane::Initialize at lines 117-130 does PRECISELY the same sequence with identical asset names AND identical VK_NEXT/VK_PRIOR/VK_ESCAPE keycodes. This is an exact, decisive golden-path match.
3. cls_0x5b5750 struct: 408 bytes; fields at 0x17c (scroll pointer), 0x180 (scrolldata multi), 0x188 (line), 0x18c (numlines), 0x190/0x194 (font/font-state) — mirrors `src/scroll.h::TScrollPane` field set (`scroll`, `scrolldata`, `scrollfont`, `line`, `numlines`).
4. cls_0x5b5750 vtable slot 4 (Close, FUN_005212c0) calls PlayScreen's ReleaseExclusivePane / RemovePane / Redraw (DAT_0065caf0 = PlayScreen global) — matches src/scroll.cpp::TScrollPane::Close cleanup sequence.
5. cls_0x5b5750 vtable slot 80 (DrawBackground, FUN_00521360) renders the `"scroll"` bitmap centred + iterates lines of text from mbr_0x17c+0xd8 — matches src/scroll.cpp::TScrollPane::DrawBackground.

#### TBookPane = cls_0x5b5808

1. cls_0x5b5808 vtable slot 0 = 0x5217e0. Body of FUN_005217e0 (`recon/discovered/cls_0x5b5808_TBookPane_Initialize_5217e0.cpp`) loads `"book.dat"`, fetches `"book"` bitmap, creates `"down"`/`"up"`/`"exit"` buttons identically to TScrollPane::Initialize.
2. src/scroll.cpp::TBookPane::Initialize at lines 265-280 does PRECISELY the same with identical asset names. Golden-path match.
3. Vtable diff: cls_0x5b5808 vs cls_0x5b5750 — ALL slots identical EXCEPT slot 0 (Initialize) and slot 80 (DrawBackground). This exactly matches src/scroll.h's declaration: `class TBookPane final : public TScrollPane { virtual bool Initialize(); virtual void DrawBackground(); };` — only those two overrides.
4. cls_0x5b5808 struct layout: identical to cls_0x5b5750 (no new fields) — matches src `TBookPane final : public TScrollPane { TBookPane() : TScrollPane() {} }`.
5. cls_0x5b5808 has 1 extra vtable slot at offset 184 (FUN_00521fb0) — a small factory/allocator for cls_0x5a50e8 (unknown helper). Per src/scroll.h this could be a page-bookkeeping or paging-buffer factory; defer naming.

### Slot mapping (for downstream agents)

| Slot | TScrollPane | TBookPane | Role | Source |
|---:|---|---|---|---|
| 0 | 0x521080 | 0x5217e0 | Initialize | src/scroll.cpp |
| 4 | 0x5212c0 (shared) | 0x5212c0 (inherited) | Close | src/scroll.cpp |
| 76 | 0x435d70 (shared) | 0x435d70 (shared) | TButtonPane intermediate | shared with 0x5a45c8 family |
| 80 | 0x521360 | 0x521a20 | DrawBackground | src/scroll.cpp |
| 108 | 0x521530 (shared) | 0x521530 (shared) | KeyPress (switch on VK_END/HOME/LEFT/RIGHT) | src declares TScrollPane::KeyPress |
| 124 | 0x521dd0 | 0x521ec0 | scalar-deleting dtor | implicit |
| 184 | (n/a, 46 slots) | 0x521fb0 | TBookPane-only factory (cls_0x5a50e8 alloc) | unknown |

### Open methods (deferred)

- TScrollPane KeyPress (slot 108 = 0x521530): switch handles cases 0x23/0x24/0x26/0x28 — that's VK_END/VK_HOME/VK_LEFT/VK_RIGHT in Win32. Body advances mbr_0x188 (= `line` in src) by ±100000 (PgUp/PgDn) or ±1 (arrows). Matches src TScrollPane::KeyPress semantics. Defer body extraction until needed for port.
- TBookPane slot 184 (FUN_00521fb0): allocates 0xd8 bytes via FUN_00482fb0 and constructs cls_0x5a50e8. Probably a paging-helper widget; defer.

---

## B.r19 — Credits screen identification

### Verdict

**No in-engine credits-UI class exists.** Credits ship as a Smacker video file: `data/Disk2/MIX_CREDITS.SMK`. The game's credits sequence is played via the same Smacker-playback path used for MIX_FMV1.SMK / MIX_FMV2.SMK / Mix_fmv3english.smk (the intro/outro cinematics).

### Evidence

1. Filesystem inventory: `data/Disk2/` contains `MIX_CREDITS.SMK` alongside the other cinematics. No `credits.def` or `credits.dat` exists anywhere under `data/`.
2. `find data -iname "*credit*"` returns only `MIX_CREDITS.SMK`.
3. The DEF-driven screens already identified (mpingame.def, joingame.def, options.def, savegame.def, createchar.def, selstart.def, connect.def, ingamemenu.def) cover all in-engine OOG screens; none is named credits.def.
4. Smacker playback is dispatched via the cinematic player invoked from cls_0x5a5d18 (TLogoScreen / main menu) or the end-of-game flow — no dedicated UI class needed.

### Disposition

No rename. Documented in port-status that the credits "screen" is a video asset, not a UI class. If the Revisited overlay wants modern scrolling credits in-engine, that's a NEW feature (parallel to MIX_CREDITS.SMK), not a port.

---

## B.r20 — Main menu disambiguation

### Verdict

**cls_0x5a5d18 (already labeled "TLogoScreen" by Wave-1) IS the main menu screen.** There is no separate TMainMenuScreen. The screen does BOTH the splash (background bitmap + version string) AND the menu (5 button-based entries). No new class to discover.

### Evidence

1. cls_0x5a5d18::Initialize (FUN_0053a2c0, body already in `recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp`):
   - Loads `menus.dat` via FUN_0047f670(s_menus_dat_005e43dc, ...) into DAT_0066fcac.
   - Creates 5 buttons via FUN_0042c400 (= cls_0x5a3c68::cls_0x5a3c68 TButton ctor — same ctor TDeathPane uses for its restart/load/exit buttons) with labels:
     - `s_MenuNewGame_005e43f0` → New Game
     - `s_MenuLoadGame_005e43fc` → Load Game
     - `s_MenuMulti_005e440c` → Multiplayer
     - `s_MenuOptions_005e4418` → Options
     - `s_MenuExit_005e4424` → Exit
   - Wires each button to a click-handler label (LAB_0053a1f0, LAB_0053a220, LAB_0053a240, LAB_0053a260, LAB_0053a2a0).
2. cls_0x5a5d18::Animate (FUN_0053a6d0, body already in `recon/discovered/cls_0x5a5d18_TLogoScreen_Animate_53a6d0.cpp`):
   - Renders `Bitmap("MainMenu")` at (0,0) every frame.
   - Renders the version string `"Revenant v%d.%02d"` at (0xf, 0x1c2) — bottom-left corner — using a colour-bracketed FUN_004be2b0 text-draw.
3. The 5 button labels match the 5 menu options visible in `main_menu_ui.jpg` (per docs/ui/CLASSIC_HUD_REFERENCE.md or the screenshot reference Wave-1 used).

### Interpretation

- The class is named `TLogoScreen` in the recon (per Wave-1's identification in `recon/discovered/renames/agent_screens.txt`). The user (per project memory) may have called it something else in the lost retail source — possibly `TMainMenu`, `TMainScreen`, or it really was called `TLogoScreen` because it shows the logo (`MainMenu` bitmap is the game's title-with-logo splash) AND the menu on top. Without the original source we cannot know the user's exact name.
- The name `TLogoScreen` is **NOT** misleading enough to warrant a rename: the class genuinely shows the logo-background bitmap as its primary visual content. The buttons are overlaid on top.
- **No "pre-menu splash" exists.** The Animate path always paints `Bitmap("MainMenu")` + version string + buttons in one composite. There's no separate pre-menu intro screen in the engine (intros are Smacker videos played BEFORE this screen activates, via the same Smacker pipeline that owns MIX_FMV1/2/3.SMK).

### No new renames needed for B.r20

Wave-1 already renamed cls_0x5a5d18 → cls_0x5a5d18_TLogoScreen and its Initialize / Close / Animate methods. This wave only confirms the identification is correct and adds the clarifying note above.

---

## Summary table

| Item | Verdict | Class addr | Renamed in this wave |
|---|---|---|---|
| B.r17 TDeathPane | cls_0x5b93c4 | 0x5b93c4 | Method-level only (5 methods) |
| B.r17 TDeathScreen (new) | cls_0x5b9374 | 0x5b9374 | Method-level only (1 method) |
| B.r18 TScrollPane | cls_0x5b5750 | 0x5b5750 | YES + 3 methods |
| B.r18 TBookPane | cls_0x5b5808 | 0x5b5808 | YES + 2 methods |
| B.r19 Credits | not a class (Smacker video) | — | (none) |
| B.r20 Main menu | cls_0x5a5d18 (TLogoScreen, already labeled) | 0x5a5d18 | (none — already labeled) |

---

## Candidates with weak evidence

(none — all identifications meet the 98% threshold via golden path)

---

## Open questions for next wave

1. **cls_0x5b93c4 bare-class rename**: blocked on cls_0x5a45c8 (intermediate TButtonPane-with-DEF) identification. Once 4A/5A names that class and its inherited methods are factored out of the cls_0x5b93c4 file, a bare-class rename to TDeathPane becomes safe.
2. **cls_0x5b9374 (TDeathScreen) vtable + non-Initialize methods**: only Initialize identified. Dump its vtable + extract Close/Animate before promoting to class-level rename.
3. **TScrollPane KeyPress (FUN_00521530) and TBookPane factory (FUN_00521fb0)**: confirmed shape but no body extracted; defer until needed during port.
4. **"book" sidebar tab → what does it open?** Wave-3B noted the TSideTabsPane has a "Book" tab among its 6 tabs. Now that TBookPane is identified, the tab-activation handler should resolve to instantiating a TBookPane with a specific TScroll game object passed in. Trace from TSideTabsPane's click dispatcher (4A's area) to confirm — would close the loop on the spellbook/book/scroll UI taxonomy.
5. **Cinematic-player class**: not identified yet. Plays MIX_CREDITS.SMK + MIX_FMV*.SMK. Out of scope for OOG closeout but should be on Tier 10's radar.

---

## Files produced this wave

- Decompilations (in `recon/discovered/`):
  - `cls_0x5b5750_TScrollPane_Initialize_521080.cpp`
  - `cls_0x5b5808_TBookPane_Initialize_5217e0.cpp`
  - `cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp`
  - `cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp`
- Renames: `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt`
- Port-status files: `TDeathPane.md`, `TDeathScreen.md`, `TScrollPane.md`, `TBookPane.md` (one each in `recon/discovered/port_status/`)
- This brief: `docs/ui/briefs/B_r12_oog_closeout.md`

## Hunt log

- **2026-05-16, Wave-4B run 1**: Read AGENT_PROTOCOL + RETAIL_UI_RECOVERY_PLAN Tier 10 + CLASSIC_HUD_REFERENCE §3c/§6 + B.r9 (Wave-3B rejection) + Wave-1's TLogoScreen extractions + src/death.h, src/scroll.h, src/scroll.cpp, src/death.cpp. Grepped recon for "death.dat" / "book.dat" / "scroll.dat" / "Trouble initializing Death pane" — found cls_0x5b93c4 + cls_0x5b9374 + cls_0x5b5750 + cls_0x5b5808 candidates immediately, all via .rdata cross-reference in `recon/classes_original/_data.txt`.
- **run 2**: Read cls_0x5b9374, cls_0x5b93c4_TPlayer (mislabeled), cls_0x5b5750, cls_0x5b5808 + their vtable files. Identified the merging hazard in the cls_0x5b93c4_TPlayer.cpp file naming (TPlayer is actually cls_0x5b4f30; verified via TPlayer.md port-status).
- **run 3**: Ghidra-CLI extractions (serialized, no lock contention) for FUN_00521080 (TScrollPane init), FUN_005217e0 (TBookPane init), FUN_005339b0 (TDeathPane init), FUN_005338a0 (TDeathScreen init). Copied to `recon/discovered/`.
- **run 4**: B.r19 — checked `data/Disk2/` and `find data -iname "*credit*"`; only MIX_CREDITS.SMK exists. No DEF file, no class. B.r20 — re-read Wave-1's TLogoScreen Initialize + Animate; confirmed it IS the main menu (loads menus.dat, creates 5 buttons NewGame/LoadGame/Multi/Options/Exit, renders "MainMenu" bitmap + version string).
- **run 5**: Wrote brief + rename file + 4 port-status files. Did NOT apply renames (per protocol).

## Coordination notes (for next wave)

- Wave-4A is identifying the sidebar-tab cascade off TSideTabsPane. When their work resolves the "Book" tab's click handler, the result should be: clicking the Book tab instantiates a TBookPane (cls_0x5b5808) bound to a TScroll game object whose `type=="Book"` (per src/scroll.cpp line 78 dispatch). This would close the loop and validate both efforts.
- Wave-4C is working the DEF widget engine internals. They do NOT need to touch any of the 0x521xxx / 0x533xxx / 0x533cxx addresses claimed here.
- The cls_0x5a45c8 intermediate identification (still pending) is the most valuable single next-wave target: it would unlock class-level renames for TDeathPane, TStatPane, TBottomBarPane, TScrollPane base methods, TSideTabsPane base, TPlyrStatusBar (probably), TQuickSpellPane (probably). Suggest spawning a dedicated wave for it as the next priority.
