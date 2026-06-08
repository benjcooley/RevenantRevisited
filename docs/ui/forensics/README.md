# UI Forensics & Reconstruction

A two-phase, spec-as-contract pipeline for rebuilding Revenant's retail UI
panels pixel-accurate in the modern port. Mirrors the VFX worktree's
forensics/reconstruct split.

```
   retail decomp + assets
            │
            ▼
   ┌──────────────────┐   produces    ┌────────────────────────┐
   │  ui-forensics     │ ───────────▶ │ <ClassName>_SPEC.md     │
   │  (trace → spec)   │              │ self-contained contract │
   └──────────────────┘              └───────────┬────────────┘
                                                  │ sole input
                                                  ▼
                                      ┌────────────────────────┐
                                      │  ui-reconstruct         │
                                      │  (spec → engine code)   │
                                      │  + visual verify        │
                                      └───────────┬────────────┘
                                                  │ gaps feed back
                                                  └────────► improve spec + protocol + method map
```

The forensics agent writes NO engine code; the reconstruction agent reads ONLY
the spec. If the reconstruction needs to open the recon, that's a spec gap that
gets reported and fixed — each pass tightens the pipeline.

## Documents

| file | role |
|------|------|
| [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md) | Phase 1 rules + the spec template. Source of truth for the `ui-forensics` skill. |
| [RECONSTRUCTION_PROTOCOL.md](RECONSTRUCTION_PROTOCOL.md) | Phase 2 rules + port toolbox + verification. Source of truth for the `ui-reconstruct` skill. |
| [NOMENCLATURE.md](NOMENCLATURE.md) | Shared vocabulary: coordinate spaces, the chip model, anchoring/alignment, the **tween/animation notation**, ASCII diagram conventions. Mandatory pre-read for both phases. |
| [UI_METHOD_MAP.md](UI_METHOD_MAP.md) | Behavior of every common draw/blit/text/effect retail function, each with a "Port mapping" to a `src/renderer.h`/`src/surface.h` primitive. Mandatory pre-read for tracers so they convert decomp→spec mechanically. |
| [WORKED_EXAMPLE_text_coords.md](WORKED_EXAMPLE_text_coords.md) | A fully worked trace (TPlyrStatusBar text cells) teaching the two-coordinate-space trap, the chip model, SDrawParam decode from disassembly, and font-flag decoding. |
| `<ClassName>_SPEC.md` | One per panel — the reconstruction contract produced by Phase 1. |

## Skills

- `.claude/skills/ui-forensics` — invoke to trace a panel → spec.
- `.claude/skills/ui-reconstruct` — invoke to rebuild a panel from its spec.

## Shared item-cell (`TInvSlot`)

Item cells across BarInv, Inventory, and Equip are NOT each pane's private code — they're instances of one shared class `TInvSlot : TButton` (`src/invslot.{h,cpp}`, landed 2026-05-30). The class evolves from the snapshot `TTalismanButton : TButton` precedent (`src/spellpane.h:23`), generalizing it for any item. Per-instance config:

- **Slot rect** — pane-local (x, y, w, h)
- **`allowedType`** — `-1` = accept any item; otherwise an `EqSlot`-style filter (Equip's per-slot policy mirrors `TPlayer::CanEquip`). This avoids colliding with `EQ_HEAD == 0`.
- **`emptyPlaceholder`** — optional sprite painted when the slot is empty (Equip's named pictograms: "Head", "Hand2", etc.)
- **Paint style** — toggles for the per-text-cell matrix (gold-as-bitmap-Gold / qty-red-top-right / bag-count-white-bottom-center)

Consumers today:
- `src/uibarinvtest.cpp` — 9 shelf slots
- `src/uiinventorytest.cpp` — 4×3 grid slots
- `src/uiequiptest.cpp` — 11 EQ_* equipment slots

Architecture note: slots initiate drags and expose local accept policy; they do
not own cross-system transfers. The top-level drag/drop manager
(`UIDragState`, mapping back to retail `TPlayScreen` globals) owns pickup/drop
transactions across playfield, inventory, bottom bar, and equipment, including
replacement/swap/return behavior and the pickup/drop sound effect. The current
harness sound cue is a placeholder until the exact retail sound name is pinned
from a capture or sound-registry pass.

**Retail evidence** for the drag system that `TInvSlot` will hook into (cited in `invslot.h` header banner, not invented):
- Dispatch owner: `cls_0x5a5320_TPlayScreen::virt_meth_0x44f140` (12 R/W XREFs)
- Cross-pane globals: `DAT_0065b878` (dragged EQ-slot), `DAT_00668518` (mouse-down), `DAT_00668510/14` (mouse x/y), `DAT_0066829c` (swap-allowed), `DAT_00667fcc` (Player)
- Per-pane: `mbr_0x180..0x194` (grabslot/startslot/page/dragflag/clickX/Y)
- Dispatch shape: `(eventType, paramA, paramB)`, eventType ∈ {1=down, 4=up/drop, 5=use/equip}
- Drop-commit helpers: `meth_0x4701f0` (GetInventory), `[container+0x58]` (AddToInventory), `meth_0x519300` (Equip), `meth_0x5199b0` (SetInventorySlot), `FUN_00538e40` (re-sync)

**Open this pass:** `HandleEvent` + `CanAcceptDrop` are stubs; the cross-pane `UIDragState` singleton + TPlayScreen MouseClick dispatch port land in a next pass. There is **no snapshot precedent** for a generic item-cell class (verified by direct grep of `src/`); `TInvSlot` is a clean evolution of `TTalismanButton`, NOT a port of an existing retail class.

## Right-sidebar multiplex architecture

`TSideTabsPane` is the 52×232 button strip that drives **6 multiplexed sidebar panes** (per user, 2026-05-30). The 6 buttons split into two 3-way region selectors stacked in the strip:

- **TOP slot (upper 3 buttons)** — picks one of `EquipPane` / `SpellbookPane` / `StatPane`. These are the tall (188×306) parchment/character views that fill the upper sidebar region.
- **BOTTOM slot (lower 3 buttons)** — picks one of `InventoryPane` / `MapPane` (automap) / `TSpellPane` (talisman composer). These are the short (188×174) tools that occupy the lower-right corner rect `(452, 306, 188, 174)`. Selected via `DAT_0065d1bc` (0=Inventory, 1=AutoMap, 2=Spell).

Both slots can be visible simultaneously, each independently multiplexed. Hover ramp `0..8` on every button. See [TSideTabsPane_SPEC.md](TSideTabsPane_SPEC.md).

## Panel roster & status

| panel | class | spec | impl | notes |
|-------|-------|------|------|-------|
| Player/target stat bars | `TPlyrStatusBar` (`cls_0x5a54e4`) | [TPlyrStatusBar_SPEC.md](TPlyrStatusBar_SPEC.md) (forensics-complete) | partial (test mode) | Classic path fully specced; closes 4 port errors (text Y, name centering, 4-slice bar, per-side shadow). Open: char-object bar getters, I3D portrait (deferred) |
| Six-button sidebar tabs | `TSideTabsPane` (`cls_0x5a5750`) | [TSideTabsPane_SPEC.md](TSideTabsPane_SPEC.md) (forensics-complete) | **DONE** — clean-room build from spec, verified perfect first pass (`--test=ui-sidetabs`) | Orchestrates the 6 multiplexed sidebar panes — see [architecture section above](#right-sidebar-multiplex-architecture). Two 3-way region selectors (upper = top-slot tab cluster, lower = bottom-slot tab cluster); 52×232 Up/Down/Select strips; hover ramp 0..8. Burndown: [TSideTabsPane_BURNDOWN.md](TSideTabsPane_BURNDOWN.md) |
| Map / automap sidebar | `cls_0x5a5658` (MapSidebarPane; cand. `TAutoMap`) | [MapPane_SPEC.md](MapPane_SPEC.md) (forensics-partial) | `--test=ui-map` (chrome PASS, body OOS) | **BOTTOM-slot multiplex with `InventoryPane` + `TSpellPane`** in rect (452,306) 188×174. Chrome=`Amap`, **Marker is a 4×4 ATLAS of 4×4-px color cells** (row 0=greens, 1=oranges, 2=reds, 3=blues) — NOT a single 16×16 blip; PlusSel/MinusSel zoom buttons are TOP-anchored within chrome (spec §4 wording "bottom" is wrong). Body is a live scrolled 5×5 64px-tile mesh; mesh internals OUT-OF-SCOPE (3D path). No text. Beware Ghidra-merged TBarInvPane methods in the decomp file. **BM_UNKNOWN_0x10000 confirmed = ARGB4444 little-endian** (decoder now branched in bitmapdecode.cpp). |
| Bottom command/status bar | `TBottomBarPane` (`cls_0x5a5808`) | [BottomBarPane_SPEC.md](BottomBarPane_SPEC.md) (forensics-complete) | **DONE** — `--test=ui-bottombar` PASS | Bottom-anchored full-width strip, y=display_h−60, h=60. Chrome only: UtilityBar 640×60 X-stretched + BarEndCap 10×60 at (w−10,0), both opaque. Hosts (does not own) quick-spell ring + BarInv slots. |
| Spell Book sidebar ("Book" tab) | `SpellbookSidebarPane` (`cls_0x5a5ae8`) | [SpellbookPane_SPEC.md](SpellbookPane_SPEC.md) (forensics-complete) | — | **TOP-slot multiplex with `EquipPane` + `TStatPane`** (tall 188×306 parchment scroll, `Scroll` chrome + tiled `ScrollPaper`). Scrollable known-spells list. Draw = vtable slot 21 `FUN_005452d0` (extracted this wave). Per-spell ≈91px row: 40×40 icon + cream name + talisman glyph row (16px) + desc/skill block + cream/green/cyan "Skill:/Mana:" lines (font 0x401/0x402, 3-pass shadow). Scroll ±40px/tick; up/down arrows @(169,150)/(169,174). Open: side-region origin, content-surface inset (20,38), 40×40-vs-32×32 icon set, SPANE* label text. |
| Spell creation / talisman composer ("Spell" tab) | `TSpellPane` = `cls_0x5a5978` | [SpellCreatePane_SPEC.md](SpellCreatePane_SPEC.md) (forensics-skeleton + 5 GAPs) | **DONE (partial)** — `--test=ui-spellcreate` PASS for chrome + 12 talisman icons + 5 buttons | **Found 2026-05-30** — was missed by all 5 prior critic passes. **BOTTOM-slot multiplex with `InventoryPane` + `MapPane`** in rect `(452, 306, 188, 174)`; selected when `DAT_0065d1bc == 2`. Chrome `spellconstr` + 12 talisman icons (SStars/SLaw/SLife/SSky/SChaos/SDeath/SSoul/SSun/SOcean/SMoon/SEarth/SWard) + 5 buttons at confirmed (x,y). **Talisman grid currently placeholder** (4×3 @ (10,8) pitch 38×33) — real positions live in slot-84 `0x5435f0`, not yet extracted (GAP-1, highest priority). Methods: AddTal/RemoveTal/Invoke/Scroll/ToggleTalismanNames. Init `0x5432a0` ("Trouble initializing spell pane"), Draw `0x5435c0`, DrawBackground `0x5435f0` (TBD), click `0x543c40` (partial). Inline source GAPs 1-5 flag spell-name region, pouch composition cells, button down-state cycle, snapshot double-stamp shadow. |
| Message log + opponent bar | `TTextBar` (`cls_0x5a5560`) | [TTextBar_SPEC.md](TTextBar_SPEC.md) (forensics-complete) | — | Transparent multi-line message overlay (scrollback ring of 0x5c-byte line records, per-line alpha fade ttl·255/24 over a 120-tick life) + opponent name/`texthealthbar` (200×11, PutHue health-driven hue rotation, slide x=min(0,level−186)). Text Arial-12 "Small", left/top, 0x400 3-pass shadow, at fg-local (4,9) stacking by lineH. Open: PutHue hue-rotate primitive, health-line slot, line-type colors 2/0x10. |
| Inventory sidebar | `cls_0x5a58c0` (cand. `TInventory`) | [InventoryPane_SPEC.md](InventoryPane_SPEC.md) (forensics-complete + binary-disasm proofs) | **DONE** — `--test=ui-inventory` PASS; cells composed via shared `TInvSlot` | **BOTTOM-slot multiplex with `MapPane` + `TSpellPane`** (188×174 short panel). 4×3 item grid, origin pane-local (8,42), pitch 45×44, interior 40, **column-major** slot col*3+row+page. Backpack 20×20, arrows 24×24, **GoldPile 32×32 at `mbr_0x19c`** (Ghidra OOAnalyzer mislabeled this as Backpack — disproved by binary disasm `:53767c/:53769e/:5376d2` on shipped Revenant.exe). Gold format literal `"%d$"` (binary-proved at `0x5e41a8`), retail font `GoldMed` (`0x6663dc`) with port override to small `Gold` for legibility. Per-text-cell matrix: gold (bitmap Gold), item qty (red top-right), bag-count (white bottom-center). **Paint composed via [TInvSlot](#shared-item-cell-tinvslot) — see new subsystem section.** |
| Character / stats sidebar | `TStatPane` (`cls_0x5a5ba0`) | [CharacterStatsPane_SPEC.md](CharacterStatsPane_SPEC.md) (forensics-complete) | `--test=ui-stats` (user-verified) | **TOP-slot multiplex with `EquipPane` + `SpellbookPane`** (tall 188×306). CONFIRMED via `d:\revenant\StatPane.cpp` fingerprint. Not the snapshot skill-tree — a **DEF-driven object-inspector**: chrome `Stats`, then `statpane.def` Player/Page1 walked by `FUN_005475e0` emitting cream/colored GDI text w/ 3-pass SE shadow onto the parent surface. Cursor: POS 28 62, NEXTLINE +12, TAB n. Open: field value-getter map, sidebar font identity, button sprites, page1↔page2 toggle. |
| Character portrait | (rendered into PlyrStatusBar Ring) | [CharacterPortrait_SPEC.md](CharacterPortrait_SPEC.md) (forensics-complete) | **DONE** — wired in `--test=ui-plyrstatusbar` | Portrait = baked `.i3d` icon via `InventoryImage()` → `GetInvImage(state)` on **state 0**, blitted under the Ring's BM_ALPHA glass disc. Root-cause fixes: const-mismatch on imagery image-getters (was breaking icons game-wide), icon-loader memset-in-loop, BM_ALPHA buffer ignored by decoder, async body-load timing. Live-3D head = future Revisited deviation. |
| Game console (editor only) | `TConsolePane` = `cls_0x5a4358` (TPane → TTextPane → TConsolePane chain) | [ConsolePane_SPEC.md](ConsolePane_SPEC.md) (forensics-complete) | — | **CORRECTION:** the in-game message overlay is NOT TConsolePane — it's [TTextBar](TTextBar_SPEC.md). TConsolePane is the **editor's** interactive command console (CreateEventA + _beginthreadex), only invoked from editor-diagnostic paths. Pane rect (0, 356, 360, 125) on editor display; 10000-byte buffer, command history, color escape `{...}`. `cls_0x4a22f0` REJECTED (generic blit helper). |
| NPC dialog panel | `TDialogPane` = `cls_0x5a5c60` (NOT `cls_0x570900`) (TPane → TButtonPane → TDialogPane) | [DialogPane_SPEC.md](DialogPane_SPEC.md) (forensics-partial) | — | **CORRECTION:** task brief's `cls_0x570900` is a TPlayerSlot network helper. Real TDialogPane is `cls_0x5a5c60` (496 B); Init `0x534fd0` matches TPlayScreen's "Trouble initializing dialog pane" error path. Conditional top/bottom anchor, NPC text + 1–6 choice keys (retail MAXCHOICES=6, snapshot=4), hover-fade ramp `(mbr_0x58*0xff)/0xc` on child entry class `cls_0x534d40`. |
| Equipment paperdoll | `EquipSidebarPane` = `cls_0x5a55dc` (NOT `cls_0x54dd40`) (direct TPane leaf, 32 slots) | [EquipPane_SPEC.md](EquipPane_SPEC.md) (forensics-complete for slot geometry) | **DONE** — `--test=ui-equip` PASS (icon-overlay paperdoll); live-3D body OOS | **TOP-slot multiplex with `SpellbookPane` + `TStatPane`** (tall 188×306). **CORRECTION:** task brief's `cls_0x54dd40` is wrong. Real class is `cls_0x5a55dc`, 32-slot direct-TPane leaf. **All 11 EQ_* slot (x,y) anchors CONFIRMED** via literal hex from `DAT_005e3f60` 11×{int,int} table (HEAD/NECK/BODY/OFFHAND/PRIMEHAND/R_ACCESSORY/L_ACCESSORY/RANGEDWEAPON/AMMO/LEGS/FEET). Slot-appropriate items via EqSlot filter (TPlayer::CanEquip parity). Item icons via `oi->InventoryImage()` + invanim for animated potion. Live-3D Locke body is OOS: 3 blockers (char3d regression, missing `Renderer->SubmitMesh*ToTarget`, no `invanim` body in Locke's .i3d). |
| Quick-spell ring (bottom-bar strip) | `TQuickSpellPane` = `cls_0x5a5a30` (TPane → TButtonPane leaf, instance @ `0x65c6f8`) | [QuickSpellPane_SPEC.md](QuickSpellPane_SPEC.md) (forensics-complete) | **DONE** — `--test=ui-quickspell` PASS | 4-slot ring strip; Ring{U,D,G} sprites cycle for state; 40×40 spell icons + 2-line cream labels w/ 3-pass shadow; disabled-state dim tint. Hosted by bottom bar (`cls_0x5a5808.cpp:163,187`). Draw slot 21 = `0x5444c0`. |
| Bottom-bar inventory slots | `TBarInvPane` = `cls_0x5a56d4` (instance @ host bottom bar) | [BarInvPane_SPEC.md](BarInvPane_SPEC.md) (forensics-complete) | **DONE** — `--test=ui-barinv` PASS; cells composed via shared `TInvSlot` | 9-slot pouch/quick-item row at pitch 45 from origin (220,10). Per-slot baked .i3d icon via `InventoryImage()`; value-text + Pouch overlay (inner icon + qty). Methods Ghidra-merged into `cls_0x5a5658` (same file as automap — kept separate). Init `0x52c970`, Draw `0x52ca70`, drag `0x52d6e0`. **Composed via [TInvSlot](#shared-item-cell-tinvslot).** |
| **Out-of-game / menus (wave 3)** | | | | |
| Main menu / title screen | `TLogoScreen` = `cls_0x5a5d18` | [MainMenu_SPEC.md](MainMenu_SPEC.md) (forensics-complete for paint + 5 buttons) | — | `menus.dat` chrome + MenuNewGame/LoadGame/Multi/Options/Exit items. Animate (slot 5, `0x53a6d0`) draws `MainMenu` sprite at (0,0,16) + version text `"Revenant v%d.%02d"`. Initialize at `0x53a2c0`. |
| Death screen | `TDeathPane` = `cls_0x5b93c4` + `TDeathScreen` wrapper = `cls_0x5b9374` | [DeathPane_SPEC.md](DeathPane_SPEC.md) (forensics-complete) | — | Game-over modal: `death.dat` chrome + 3 buttons (Restart / Load / Exit) bound to VK_TAB / VK_RETURN / VK_ESCAPE. Init `0x5339b0` (matches "Trouble initializing Death pane"); Close slot 184 = `0x533c00`; DrawBackground slot 80 = `0x533c10`. TDeathScreen owns lifecycle. |
| Character creation / editor | `TCharCreateScreen` = `cls_0x5a4d6c` | [CharCreate_SPEC.md](CharCreate_SPEC.md) (forensics-mostly-complete) | — | DEF-driven (`createchar.def`). SP New-Game "create Locke" + MP host/join character setup. Full-display 640×480. Init `0x465a50`. **NOT the shop** — that is `cls_0x5a5d64` ([BuySellScreen_SPEC.md](BuySellScreen_SPEC.md)). BSBUY/BSSELL message keys in the dispatcher are vestigial cut-MP-trade vocabulary. |
| **In-game store / shop / trade** | `TBuySellScreen` = `cls_0x5a5d64` | [BuySellScreen_SPEC.md](BuySellScreen_SPEC.md) (forensics-complete) | — | 452×178 bottom-anchored modal, chrome `BuySellMain` from `buysell.dat`. Opened by script command `buysellscreen` after `buysellinit` / `buysellsalesperson` / `buyselladd` / `buysellshoptype` wiring (15 `buysell*` commands at `_data.txt:59040-59326`). Two sub-pane walkers: `cls_0x530af0` Sell-side (player inv via 8 `DAT_0065a14c..` `TObjectClass` tables), `cls_0x531fc0` Buy-side (NPC wares list filtered by `SaleType`). Per-cell price = object's `Value` stat via `FUN_0052da90`. 72-byte cell records (18 dwords) at `mbr_0x198`. Action buttons: BuySellUp / BuySellDown / BuySellActivate / BuySellExit (VK_UP / VK_DOWN / 'A' / 'E'). `BSGOLD`/`BSBUY`/`BSSELL`/`BSEXIT` are **localization tag IDs** (not bitmap names), resolved via `english.def`. Demo wiring: `data/Modules/Demo/demo.s:184` (Elahni spell-trainer). |
| Floating text (damage + over-head dialog) | `TPlayScreen::AddPostCharAnim` overlay system | [FloatingText_SPEC.md](FloatingText_SPEC.md) (forensics-partial) | — | Rising damage numbers + over-head floating dialog text. Shared transient-overlay mechanism anchored to world position; rises + fades over TTL. Working in retail per `PLAN.md:128`; currently stubbed in our port at `src/playscreen.cpp:1217`. Retail per-frame drain-loop call site + `flashred` asset/font binding still UNCONFIRMED. |
| Load Game screen | `cls_0x5b9584` (loadgame.def host) | [LoadGameDef_SPEC.md](LoadGameDef_SPEC.md) (forensics-partial) | — | DEF-driven savegame selector. Activator `FUN_00539380 = LoadDef_loadgame`. Save-slot list, thumbnail/preview, action buttons. |
| Save Game screen | `cls_0x5b963c` (savegame.def host) | [SaveGameDef_SPEC.md](SaveGameDef_SPEC.md) (forensics-complete envelope) | — | DEF chrome + open/close traced. Brief's "no class" claim refuted — it's a dedicated 184-byte class. |
| In-game pause menu | `cls_0x5b9480` (hosts ingamemenu.def SP+MP variants) | [InGameMenuDef_SPEC.md](InGameMenuDef_SPEC.md) (forensics-complete) | — | ESC-key pause menu; subclasses through `cls_0x5b93c4 → cls_0x5a4494_TPane`. SP vs MP DEF variants. |
| Options / Settings screen | `cls_0x5b9744` (candidate `TOptionsScreen`) | [OptionsDef_SPEC.md](OptionsDef_SPEC.md) (forensics-partial) | — | 184 B, 46-slot DEF-screen subclass. Controller-rebind ring buffer decoded. Settings-apply body `0x53aa90` not extracted; FIELD→getter table + command-id mapping not pinned. |
| Parchment scroll reader | `TScrollPane` = `cls_0x5b5750` (46-slot TButtonPane+DEF leaf) | [ScrollPane_SPEC.md](ScrollPane_SPEC.md) (forensics-complete) | — | Single-page paginated reader opened on `Use TScroll`. `scroll` parchment centred on 640×480; NUMLINES=11; 3 buttons (down/up/exit) on lower-right roll. DrawBackground slot 80 = `0x521360`. |
| Parchment book reader | `TBookPane` = `cls_0x5b5808` (inherits TScrollPane) | [BookPane_SPEC.md](BookPane_SPEC.md) (forensics-complete static layout) | — | Two-page book variant of ScrollPane: shared chrome + 3 buttons; per-page text region. |
| Mouse cursor | (engine-level cursor layer, not a TPane) | [MouseCursor_SPEC.md](MouseCursor_SPEC.md) (forensics-complete) | — | TPlayScreen Initialize @ `0x47a660:250-258` wires `cursor` + `handcursor` into globals `0x6563a0..0x6563d4`. Position/hotspot/state per HUD context. |
| Modal popups (popup.def) | shared popup chrome (4 variants) | [PopupDef_SPEC.md](PopupDef_SPEC.md) (forensics-complete) | — | All four popup.def panels (yes/no, OK, etc.) on the shared `cls_0x5b93c4` base. Reusable for `exitgameyn`, autosave warnings, etc. |
| **HUD overlays + boot (wave 4)** | | | | |
| Loading screen | `TLoadScreen` = `cls_0x4485a0` | [LoadingScreen_SPEC.md](LoadingScreen_SPEC.md) (forensics-complete) | — | Boot/level-load progress screen. `loadbar.dat` = 2-entry TMulti: Background (640×480) at screen (0,0) + Bar (584×48 BM_15BIT\|BM_ALPHA) at screen (24,414). `Step(delta, redraw_bg)` advances accumulator 0..1000 + blits left-slice; `Set(float, redraw_bg)` for absolute. Per-module `<module>\loadscreen.bmp` override path exists but dormant. Distinct from `loadgame.def` (saved-game picker). |
| PlayScreen text overlays | TPlayScreen DrawBackground (slot 4 @ `0x47bd20`) | [PlayScreenOverlay_SPEC.md](PlayScreenOverlay_SPEC.md) (forensics-complete) | — | Full-screen text overlays: "system is autosaving", "Please wait...", "Autosave [...]", "Loading Game \"%s\"... Please Wait", "newgame". |
| Pause indicator | (not a sprite — TPlayScreen status path) | [PauseSprite_SPEC.md](PauseSprite_SPEC.md) (forensics-complete; **negative finding**) | — | The "PAUSE" string is a status-flag name lookup, NOT a sprite overlay. No dedicated pause-sprite UI exists in retail. |
| F-key debug overlays | TPlayScreen TimerTick (`0x47c630`) | [DebugOverlay_SPEC.md](DebugOverlay_SPEC.md) (forensics-complete) | — | "Updating script files...", "Reloading rules files...", "exitgameyn" confirm. Modal-confirm path overlaps with `PopupDef_SPEC.md`. |
| Select-Start (MP) | `cls_0x5a4e24` (selstart.def host) | [SelStartDef_SPEC.md](SelStartDef_SPEC.md) (forensics-complete envelope) | — | **MP-only** despite the brief calling it SP character-pick. DEF title "Revenant MP Select Start"; gated by `DAT_0066829c != 0`; "MULTIPLAYER" baked into chrome. 394×316 popup at pane-origin (126,65); 4 widgets: startmsg TEXT, startlist LISTBOX (VLIST row h=18), ok + cancel buttons. Loader `FUN_00469370`. Out of scope for the user's "skip network" cut — spec retained for reference. |
| _(remaining panels)_ | see [../RECON_UI_COVERAGE.md](../RECON_UI_COVERAGE.md) | — | — | additional UI classes inventoried |

## Conventions baked in

- **Cite everything** `file:line` + literal hex/decimal. No screenshot-derived
  coordinates.
- **State the coordinate space** for every number (NOMENCLATURE §1).
- **Tables + ASCII diagrams + tween blocks** are required, not optional.
- **Flag retail bugs** (pink halo, +4,+4 shadow trap) so the implementor
  reproduces intent, not artifacts.
- **Conservative > confident-wrong** — UNCONFIRMED with a resolve-step beats a
  guess (the AGENT_PROTOCOL cost-asymmetry rule).
