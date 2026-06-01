# TCharCreateScreen — Reconstruction SPEC

**Class:** `TCharCreateScreen` = `cls_0x5a4d6c` (vtable @ `0x005a4d6c`)
**Address fingerprints:** ctor `0x46d160`; init/show (vtable slot 0) `0x00465a50`; dtor body `0x00465ce0` (slot 1); hot-reload `0x00469340` (slot 19). Instance size **652 bytes** (`recon/classes/cls_0x5a4d6c.cpp:56`).

> **ROLE — character creation / character editor ONLY.** This screen is the SP New-Game "create Locke" entry and the MP host/join character-setup screen. Full-display 640×480, DEF-driven from `createchar.def` via the DEF widget engine. **NOT the in-game shop/trade UI** — that role belongs to **`cls_0x5a5d64` (`TBuySellScreen`)**, a 452×178 bottom-anchored modal that loads `buysell.dat`, is opened by the script command `buysellscreen` (string anchor `_data.txt:59304 s_buysellscreen_005c8bac`), is yielded on with `wait buysell` until `BuySellExit` fires, and is confirmed via the Elahni demo script `data/Modules/Demo/demo.s:184`. See [`BuySellScreen_SPEC.md`](BuySellScreen_SPEC.md). An earlier revision of this spec carried a "DUAL ROLE / Store-Role §SR" cross-finder synthesis claiming the right panel (`addframe`) was the trade surface; that retrofit has been **retracted** here on reconciliation evidence: (a) no call site opens `cls_0x5a4d6c` from `buysellscreen`; (b) the BSBUY/BSSELL message-key references inside `virt_meth_0x4669e0` are most plausibly vestigial / shared dispatcher vocabulary from a cut MP-host-lobby player-to-player trade feature; (c) `cls_0x5a5d64` has the actual shop plumbing — two sub-pane walkers (`cls_0x530af0` Sell-side, `cls_0x531fc0` Buy-side SaleType filter), per-cell `FUN_0052da90` price-compute against the object's `Value` stat, and 72-byte cell records at `mbr_0x198`. The earlier §SR section below is preserved for traceability but is **superseded**.

**Status:** forensics-mostly-complete (chrome + Init flow + DEF schema fully traced; per-widget visual style + FIELD→getter dispatch (vtable slot 164 `virt_meth_0x468320` + 160 `virt_meth_0x468ee0`) + command dispatch (vtable slot 148 `virt_meth_0x4669e0` — NAME-routed) are now pinned in §15-§17. Remaining open: stat/skill FIELD slot binding (xref-only); commit timing (M); some mode-flag semantics (N). See §14 reconciliation in §15.4.).

> **NOTE on coordinate frames.** Unlike a hand-drawn panel (TPlyrStatusBar, TSideTabsPane), this screen is rendered by the **DEF widget engine**: every visible element's `(x,y,w,h)` is a literal `POS x y w h` in `createchar.def`, evaluated as **pane-local** by `DefWidget_DispatchControl`. There is no chrome-surface / scratch-surface composition stack at the screen level; sub-surfaces are owned by individual widgets (e.g. EDIT/DROPLIST `BGBITMAP`). The forensics rule "every spec coordinate is a final, composed, single-space value" (NOMENCLATURE §1) reduces to: **every `(x,y)` in §4 is `pane-local`, pane origin is `screen:(0,0)`** because `DefScreen_Open` is called with x=0,y=0 at full display size. See §3.

---

## §0 — Sources & status

### Recon files read (all `recon/...` relative to `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`)
- `recon/classes/cls_0x5a4d6c.cpp` — class struct (652 B) + 5 methods (Init, dtor body, hot-reload, ctor, op-delete) (1).
- `recon/classes/cls_0x5a4d6c__vftable_5a4d6c.cpp` — 46-slot vtable (184 B) (2).
- `recon/discovered/cls_TCharCreateScreen_Init_465a50.cpp` — Ghidra extract of `virt_meth_0x465a50` (the Initialize body), identical to `recon/discovered/FUN_00465a50_LoadDef_createchar.cpp`.
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp` — slot-132 DefScreen_Open helper called by Init.
- `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` — sub-helper that copies (w,h,hi_w,hi_h) into pane fields and triggers `DefWidget_LoadFile`.
- `recon/discovered/cls_0x5a45c8_intermediate_init_434e40.cpp` — `TButtonPane_Initialize` (`FUN_00434e40`) base-init called from `DefScreen_Open` body.
- `recon/discovered/port_status/DefWidgetEngine.md` — engine status, widget roster (8 widget classes: BITMAP/FRAME/TEXT/BUTTON/SCROLLBAR/LISTBOX/EDIT/DROPLIST), createchar entry @ `0x465a50`.
- `recon/discovered/renames/agent_ui_wave4_def_widget_ctors.txt` — widget ctor + factory + sizeof per class (verbatim roster).
- `recon/discovered/renames/agent_player_init.txt:82-88` — confirms Init id + "Locke" + Player_Name codepath.
- `recon/discovered/player_init_notes.md:96-110` — the char-create-as-MP path narrative.
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:139-147` — confirms `DAT_0065c890 = createcharnotex.dat` (Classic) / `createchartex.dat` (16-bit) **asset handle** load.
- `recon/discovered/cls_0x5a5320_TPlayScreen_Close_47b290.cpp:58-59` — Close frees `DAT_0065c890`.
- `recon/discovered/FUN_004be2b0_CompositeBlit.cpp`, `FUN_0054a5d0_TPlyrStatusBar_BarRender_Helper.cpp` (referenced via UI_METHOD_MAP) — text-render + draw primitives; NOT directly called by this screen, only by widgets.

### Asset measurements
- `createcharnotex.dat` dumped via `tools/ui/dump_dat.py` → **1 bitmap, 640×480 "Background" (BM_15BIT, flags=0x2)**. Visually a stone-iron frame around an empty parchment area, with a stylized "CHARACTER" gold-on-black title bar centered at the top.
- `createchartex.dat` measured similarly: same shape (1 bitmap), but flags=0x10000 (the 16-bit-texture variant) — current dump tool reports "unsupported bit-depth" at decode but the **header confirms identical 640×480 size**.

### Status note
- **No `paint()` body lives on this class.** Drawing is delegated to the DEF widget engine via the base class. The screen's job is: bind data-source fields, hand off to `DefScreen_Open(...)`, then react to button commands. The 14 free-function helpers at vtable slots 8/20/28/124/128/132/136/140/144/148/152/156/160/164/168/172/176/180 (cls_0x5a4d6c__vftable_5a4d6c.cpp:9–53) are inherited from the DEF-screen base (`cls_0x5b93c4`/`cls_0x5b98b8` — see vtable cite below). No per-class paint override on this class.
- The decomp shows a **single thiscall init body** with a `param_1`-style first arg only because Ghidra didn't bind the function start as a method initially; **OOAnalyzer DID bind it** as `virt_meth_0x465a50` on `cls_0x5a4d6c` (vtable slot 0). See `cls_0x5a4d6c.cpp:62-213` + `__vftable.cpp:7`. So there IS a TScreen-subclass vtable; the brief's "UNCONFIRMED whether a TScreen subclass vtable exists" is **CONFIRMED YES**.

### Snapshot note (supplementary — superseded by retail)
**None.** No `src/charcreate.*` or `src/createchar.*` exists in the 1998/99 snapshot (verified `grep -rln 'TCharCreate\|CharCreate' src/ RevenantRepo/` empty). The DEF engine is a late addition (per `DefWidgetEngine.md:5`); the snapshot tree predates it. Build from retail recon alone.

---

## §Store-Role — DEF-screen variant of the in-game Buy/Sell (trade) UI

> **Role correction (added by cross-finder synthesis, 2026-05-29).** The body of this spec previously labeled the right-side panel as the "MP host-lobby character editor" with inventory transfer. Three of four UI-finder agents converge on a stronger reading: **this screen's right-side panel is the DEF-screen variant of Revenant's in-game store / Buy-Sell (trade) interface.** The retail vocabulary is `buysell` (not `store`, `shop`, or `merchant` — which is why prior string searches missed it). The CharCreate screen is dual-purpose: the LEFT panel is the character editor (Name/Class/Stats/Skills/Options), the RIGHT panel is the trade surface (NPC wares vs player inventory, with Add/Del/Equip = buy/sell/equip-on-purchase). This section documents the store role inline; it does NOT rewrite the existing character-editor forensics — the character-editor mechanics in §1–§17 remain correct for the LEFT panel.

### §SR.1 — Consensus evidence for the store role

The store-role reading is grounded in three independent recon threads:

1. **Dispatcher body (Finder 4, already inline in §15).** `virt_meth_0x4669e0` (the screen's OnCommand, vtable slot 148) contains explicit BSBUY / BSSELL branches that read and write `editgold`:
   - `editgold` is "writeable + commit path on BSBUY/BSSELL" — cite `CharCreate_SPEC.md:903` (this file) and the underlying `_data.txt:77525, :77007-77010`.
   - `del` button "ties to BSSELL price update" — cite `CharCreate_SPEC.md:949` and `_data.txt:76997` (BSSELL message-key xref'd from the same dispatcher block as `del`).
   - `ok` button "drives BSBUY/BSSELL fallthroughs in the same dispatcher" — cite `CharCreate_SPEC.md:937` and `_data.txt:75540-75542, :76976`.
   - `MPGPGP` template (`"%d%s"` "gold per piece" format) is read by `virt_meth_0x468320` at `00468a68, 00468a80` — cite `CharCreate_SPEC.md:916`, `_data.txt:77605`. This is the **price-display** format used to render per-item gold cost in the inventory listboxes.
   - `SaleType` is rebound on hot-reload alongside `classlist` and `longgameinv*` — cite `CharCreate_SPEC.md:959` and `_data.txt:76689-76702`. `SaleType` is the dispatcher mode selector (BSBUY vs BSSELL).
2. **DEF asset evidence (Finder 2).** `createchar.def` defines exactly the surface needed for a trade UI: two side-by-side 266×131 LISTBOXes `longgameinv` (NPC wares — the "game inventory" the player is shopping in) and `longplayerinv` (the player's own inventory), with icon-mode siblings `longgameinvicon` / `longplayerinvicon`; a Gold label + `editgold` EDIT; "Inventory Transfer Buttons" comment block over Add/Del/Equip; `itemqty` EDIT for transfer quantity; `classlist` DROPLIST that doubles as a SaleType selector when in trade mode. See `createchar.def` widget definitions + `CharCreate_SPEC.md:892-905` for the FIELD→getter table.
3. **Script-side wiring (Finder 3).** Revenant's quest/dialog scripts call into a `buysell*` family that opens this UI: `buysellinit` → `buysellsalesperson` → `buyselladd` → `buysellscreen` → `wait buysell`. The script vocabulary uses the exact `buysell` token absent from any code-side string table, which is why earlier "store" / "shop" / "merchant" searches returned nothing.

The user's premise ("ONE of our 31 specs is actually the store UI") resolves to this spec. The original spec author saw the DROPLIST + LISTBOXes + transfer buttons and read them as MP host-lobby character editing — a plausible misread because the same widgets are also wired into the MP edit path. But the dispatcher's BSBUY / BSSELL branches and the `MPGPGP` price format are unambiguous: in trade mode this screen IS the buy/sell UI.

### §SR.2 — UI element → store semantics map

| element | character-editor role (existing) | **store / trade role (added)** | cite |
|---------|----------------------------------|--------------------------------|------|
| `editgold` (EDIT @ left panel) | display of `player->Gold()` | **running wallet** — debited on BSBUY, credited on BSSELL; the dispatcher's commit path lives here | `CharCreate_SPEC.md:903`, `_data.txt:77525, :77007-77010` |
| `longgameinv` (LISTBOX 266×131 @ (327, 105)) | (treated as "world/game inventory" rows in the editor reading) | **NPC wares list** — the merchant's stock available to buy. Hot-reload rebinds via `meth_0x465d50` (`_data.txt:76689-76702`) | `CharCreate_SPEC.md:954, :959` |
| `longgameinvicon` (icon-mode sibling) | icon-grid swap | **NPC wares, icon-grid mode** (toggled by `icon` button) | `CharCreate_SPEC.md:951` |
| `longplayerinv` (LISTBOX 266×131 @ (327, 269)) | "player inventory" rows | **player inventory** — items the player can sell, or destination cell for purchased rows. Hot-reload rebinds via `meth_0x465ea0` (`_data.txt:76707, :76712`) | `CharCreate_SPEC.md:954, :959` |
| `longplayerinvicon` | icon-grid swap | **player inventory, icon-grid mode** | `CharCreate_SPEC.md:951` |
| `add` button (KEY "A", (337, 241, 45, 23)) | "Add row to player inv" | **BUY** — commit `itemqty` units of the selected `longgameinv` row → debit `editgold` by `(qty * row.value)`, transfer row to `longplayerinv` | `CharCreate_SPEC.md:948`, `_data.txt:77176` |
| `del` button (KEY "D", (387, 241, 45, 23)) | "Remove row" | **SELL** — commit selected `longplayerinv` row → credit `editgold` by `(qty * row.value)`, transfer back to `longgameinv` (or destroy). Dispatcher's BSSELL price-update lives at `00466914` | `CharCreate_SPEC.md:949`, `_data.txt:76890, :76997` |
| `equip` button (KEY "E", (437, 241, 45, 23)) | "Equip selected" | **EQUIP-ON-PURCHASE** — apply purchased item's equip slot (so the player can immediately wear/wield a bought weapon/armor without leaving the screen). Sets `lpi_eq` "EQ" tag. | `CharCreate_SPEC.md:950`, `_data.txt:76895, :77264, :77268, :77273` |
| `itemqty` (EDIT @ (530, 78, 50, 20)) | "transfer qty" | **transfer-amount clamp** — number of units to buy or sell per click; clamped to `lgi_qty` / `lpi_qty` of selected row (i.e. can't buy more than NPC stocks, can't sell more than you own) | `CharCreate_SPEC.md:953`, `_data.txt:77128, :77172` |
| `classlist` (DROPLIST @ (365, 78, 135, 20)) | "class picker" | **SaleType selector** in trade mode (BSBUY vs BSSELL vs equip-only). Rebound by `meth_0x465d50` on hot-reload alongside the inventory lists. UNCONFIRMED whether the same widget is re-purposed in trade mode or whether a separate `SaleType` widget instance is bound. | `CharCreate_SPEC.md:898, :959` |
| `lgi_value` / `lpi_value` FIELD-routed text | "row value display" | **per-row price** rendered via `MPGPGP` template `"%d%s"` (gold amount + suffix). Read by `virt_meth_0x468320` for every visible row. | `CharCreate_SPEC.md:904, :907, :916`, `_data.txt:77546-77707, :77605` |
| `lgi_qty` / `lpi_qty` FIELD-routed text | "row quantity" | **stock-on-hand** (game side) / **owned-count** (player side); also the clamp source for `itemqty` | `CharCreate_SPEC.md:904, :909` |
| `ok` button @ (370, 415, 80, 27) | "commit character edits" | **close trade session** — flush any pending BSBUY/BSSELL commits, notify parent script (which then advances via `wait buysell` per Finder 3's script trace) | `CharCreate_SPEC.md:937`, `_data.txt:76976, :75540-75542` |
| `cancel` button @ (475, 415, 80, 27) | "discard edits" | **abort trade** — return wallet + inventories to pre-session state (UNCONFIRMED whether retail snapshots state on open vs simply trusts that uncommitted BSBUY/BSSELL branches are no-ops) | `CharCreate_SPEC.md:938`, `_data.txt:76971, :77196, :77970` |

### §SR.3 — Recon paths grounding the role

Primary citations (all relative to `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`):

- `docs/ui/forensics/CharCreate_SPEC.md:903, :916, :937, :949, :959` — inline forensics already documenting BSBUY/BSSELL, `MPGPGP`, `SaleType`, BSSELL price update on `del` (Finder 4 body evidence).
- `recon/discovered/_data.txt:75540-75542, :76689-76702, :76707, :76712, :76890, :76895, :76971, :76976, :76997, :77007-77010, :77089, :77104, :77109, :77128, :77172, :77176, :77264-77273, :77525, :77605` — dispatcher and FIELD-getter xref cluster (BSBUY / BSSELL / MPGPGP / SaleType / editgold / longgameinv / longplayerinv).
- `recon/classes/cls_0x5a4d6c.cpp` + `recon/classes/cls_0x5a4d6c__vftable_5a4d6c.cpp` — class struct and vtable that host `virt_meth_0x4669e0` (slot 148, OnCommand) and `virt_meth_0x468320` (FIELD getter, slot 164).
- `createchar.def` — widget asset definitions for `editgold` / `longgameinv` / `longgameinvicon` / `longplayerinv` / `longplayerinvicon` / `add` / `del` / `equip` / `itemqty` / `classlist` (Finder 2 asset evidence).
- Script-side (Finder 3): the `buysell*` script vocabulary (`buysellinit`, `buysellsalesperson`, `buyselladd`, `buysellscreen`, `wait buysell`) — script-source path not yet pinned in this spec; recorded as UNCONFIRMED-SR-S below.

### §SR.4 — Sibling spec required: `cls_0x5a5d64` (standalone BuySell popup pane)

**IMPORTANT CAVEAT.** Finders 2 and 3 independently fingerprint a SECOND, dedicated trade UI class **`cls_0x5a5d64`** that loads `buysell.dat` and instantiates child buttons `BuySellUp` / `BuySellDown` / `BuySellActivate` / `BuySellExit`. This class has **NO forensics spec** among our 31 (confirmed by Finder 3's grep). CharCreate_SPEC.md (this spec) covers the **DEF-screen variant** of the trade UI — i.e. the trade surface when it shares chrome with the character editor. The **standalone in-game popup variant** (the small floating Buy/Sell window invoked mid-game from a merchant NPC) is a separate class and warrants its own spec.

Follow-up work item: write `BuySell_SPEC.md` for `cls_0x5a5d64` covering:
- Asset roster grounded in `buysell.dat`.
- The four `BuySell*` child buttons (Up / Down / Activate / Exit) and their dispatch.
- How the standalone variant shares (or doesn't share) the BSBUY / BSSELL message-key vocabulary with the CharCreate DEF-screen variant.
- Wiring back to the script `buysell*` family (the same script entry-points probably select between the two variants based on context).

### §SR.5 — Conflicting reading (Finder 1) — recorded for completeness

Finder 1 proposed that the store UI lives inside **InventoryPane** (i.e. the in-game inventory sidebar with a polymorphic container field that can retarget at an NPC's bag). This reading is **rejected** for these reasons:

- No `buysell.dat` asset reference from InventoryPane's recon files.
- No `BuySell*` button names in InventoryPane's widget set.
- No `buysell*` script vocabulary matches from the InventoryPane code path.
- The "polymorphic container" mechanism Finder 1 cites appears to be sub-bag / pouch retargeting (e.g. opening a chest, opening a corpse), not a trade interaction — the same widget surface, no price column, no separate wallet display, no SaleType selector.

The Finder 1 hypothesis is preserved here as a negative finding so future investigators don't re-explore it.

### §SR.6 — UNCONFIRMED (open questions, store-role)

| id | item | status | next step |
|----|------|--------|-----------|
| SR-A | Exact gold-deduction code path on BUY commit — is `editgold` decremented in `virt_meth_0x4669e0`'s BSBUY branch directly, or does the branch fire a method like `meth_0x466950` (which xref's `editgold` at `004669c4`, cite `CharCreate_SPEC.md:903`) that does the arithmetic? | open | Decompile `virt_meth_0x4669e0` BSBUY branch body (currently only xref-level visibility via `_data.txt:77007-77010`). |
| SR-B | NPC inventory data source — which TObject / TPlayer / TMerchant field backs `longgameinv` when in trade mode? In editor mode it's a class wares list (`CLNAME%d`-indexed). In trade mode it should be a per-merchant inventory pointer set by `buysellsalesperson` script entry. | open | Trace `buysellsalesperson` script-entry → C-side handler → which screen mbr (likely one of `mbr_0x17c..0x1d0` from §16) receives the merchant pointer. |
| SR-C | `SaleType` widget identity — is it a separate hidden DROPLIST/EDIT in `createchar.def`, or is the existing `classlist` DROPLIST re-purposed as SaleType selector in trade mode? Finder 4 says hot-reload rebinds "classlist + longgameinv* + SaleType" as three separate entities (`CharCreate_SPEC.md:959`, `_data.txt:76689-76702`), suggesting SaleType IS a distinct widget — but the spec body doesn't list it among the §4 element layout. | open | Grep `createchar.def` for `SaleType` widget definition (negative result would force the "classlist-repurposed" reading). |
| SR-D | Mode-flag for trade mode — which of `mbr_0x17c / 0x180 / 0x184 / 0x18c` (the four screen-mode flags from §16) selects trade mode vs SP-edit vs MP-edit? Plausibly one bit slot is the BSBUY/BSSELL gate. | open | Cross-check `meth_0x466480` visibility-swap branches against the screen-open call site from `buysellscreen` script-entry. |
| SR-E | Cancel rollback semantics — does Cancel actually snapshot wallet + inventories on open and restore them on cancel, or does it rely on the dispatcher's BSBUY/BSSELL branches being no-ops until OK is pressed? | open | Decompile cancel branch in `virt_meth_0x4669e0`. |
| SR-F | Equip-on-purchase semantics — does the `equip` button apply to the most-recently-purchased item, the currently-focused `longplayerinv` row, or both? | open | Trace `equip` dispatcher branch (`004677da` per §15.3) for the source-list lookup. |
| SR-S | `buysell*` script-entry source path — Finder 3 reports the call chain but the actual script source file (e.g. `script/buysell.s` or wherever Revenant scripts live in the recon tree) is not pinned in this spec. | open | Grep recon tree for `buysellinit\|buysellsalesperson\|buyselladd\|buysellscreen` token and record the script source file path. |
| SR-T | Sibling spec `cls_0x5a5d64` (standalone popup) — not yet written. | follow-up | Author `BuySell_SPEC.md` per §SR.4. |

---

## §1 — Overview

`TCharCreateScreen` is the **character creation / character editor** screen reached from MenuNewGame (single-player branch) and from the multiplayer host/join chain (multi-player branch). It is a full-display 640×480 modal screen rendered by the **DEF widget engine** from `createchar.def` + the shared `widgets.def` style library.

Visually it is the "CHARACTER" framed panel from `createcharnotex.dat` (the engraved metal frame + gold "CHARACTER" title bar at top-center). Inside the frame the DEF parser instantiates a hierarchy of:
- **2 FRAME panels** ("playerframe" 276×370 left @ (35,70), "addframe" 280×338 right @ (320,70)) — the two stacked content panes, plus a "tabframe" sub-panel @ (42,295) 260×137 (lower stats/skills/options tab area).
- **Character data cells:** 100×100 portrait BITMAP @ (47,82) + 104×104 portrait FRAME chrome @ (45,80); name/pass/gold/level/exper labels + EDIT spinners (5 EDITs).
- **Class / Char info:** 2 right-aligned label TEXTs ("Char:", "Class:") + 2 dynamic-text TEXTs (`txtchar`, `txtclass`) at (210, 83/100), plus 4 cyan-styled `classinfo1..4` TEXTs at (153, 120/130/140/150) (152×10, COLOR `0 150 255`, font `Small`, FLAGS `TEXTFLAG_CENTER | CTRLFLAG_CLEARBG`).
- **Class selector:** Prev/Next BUTTONs (50×20 @ (175,165) / (230,165)).
- **Stats / Skills / Options tab selector:** 3 radio BUTTONs (GROUP 1) "Stats" (45,271) / "Skills" (120,271) / "Options" (195,271).
- **Stats body** (page 1): 6 stat labels (Strn / Cons / Agil / Rflx / Mind / Luck) + 6 EDIT spinners at y = 313/338/363 (left col x≈55/110) and y = 313/338/363 (right col x≈155/210). Default value `"13"`. Plus 3 info lines `st_txt_info1..3` (250×10, COLOR `0 150 255`, font `Small`).
- **Skills body:** 3 stacked pages (each ≈ 4 SKILL/EDIT rows + 4 Exp/EDIT rows at fixed y = 303/328/353/378). Prev/Next + Page label (50×20 BUTTONs @ (90,405)/(205,405); TEXT `skl_page` @ (148,406)).
- **Options body:** 1 description TEXT + 2 toggle BUTTONs ("Player Killer", "Viewable by Other Players").
- **MP inventory side** (drawn into `addframe`): DROPLIST `classlist` (135×20 @ (365,78)) + EDIT `itemqty` + 2 LISTBOXes (LongGameInv + LongPlayerInv, 266×131 each), Add/Del/Equip/Icon BUTTONs. *This whole region is the multiplayer character-edit interface from the host-lobby; in SP it is suppressed by `CTRLFLAG_HIDDEN` on the right frame (see `playerframe2` / `addframe2` hidden-state copies in the DEF).*

The screen pre-populates a default `"Locke"` player object via `NewObjectByName("Locke", 0, -1, 1)` (`Init.cpp:69` / cls:`134`). The `mbr_0x194 == 2` (SP-character-edit submode) gate controls this. The default Player Name "Player Name" localization key is resolved + applied via `FUN_0046e6f0(s_Player_Name_005d3314)`.

**When shown:** SP NewGame after the intro selstart, or from the MP host-lobby Edit-Character path. **When hidden:** dismissed by Ok / Cancel command dispatch (commands not yet decoded — §10/§14).

---

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|-------|---------|------------|----------------|------|--------------|------|
| `Background` (Classic 15-bit) | `createcharnotex.dat` | `Background` | **640×480** | Full-screen chrome (engraved iron frame + "CHARACTER" gold title); blitted as the BG image of the DefScreen | full image (0,0,640,480) | `cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:144-147`; `tools/ui/dump_dat.py /tmp/createcharnotex_dump/createcharnotex.def:5-16` (PNG header reports 640×480) |
| `Background` (16-bit) | `createchartex.dat` | `Background` | **640×480** | Same as above for `DAT_006680c8 != 0` (16-bit texture variant) | full image (0,0,640,480) | `cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:139-141`; `/tmp/createchartex_dump/createchartex.def:5-16` (header WxH); decoder not yet supporting BM 0x10000 flag |
| `s_background_005cd8d4` token "background" | n/a — engine-side | "background" | logical lookup string | DefScreen_LoadAndShow passes this to `vtable+0xa0` (texture loader) to resolve the chrome at `pcStack_8/pcStack_c` | n/a | `FUN_00435040_DefScreen_LoadAndShow.cpp:31` |
| **widget-style assets** (loaded via `widgets.def` per ctrl) | `<global>` | `ClearFrame1..4`, `TransFrame1..2`, `FillFrame2`, `CheckU/D`, `RadioU/D`, `SpinUp/Down`, `VScrollBar`, `VScrollUp/Down/Thumb`, `DropBtn`, `DropRect`, `VScrollRect` | per-asset | DEF widget chrome (frames, checkboxes, radios, spinner arrows, scrollbars, droplist dropdown button) | per widget — see `widgets.def:81-145` | `widgets.def` STYLE blocks |
| **portrait** | (runtime) | `param_1[0x7e]->InventoryImage(state=0)` (Locke .i3d icon) | 100×100 (target cell; baked icon dims set by the i3d) | the character portrait BITMAP with `FIELD "portrait"` | rect (47,82,100,100) | `createchar.def:22`; portrait pipeline = `CharacterPortrait_SPEC.md` |

**Atlas note:** there is no atlas in `createchar.dat`. The chrome is a single full-screen image; everything else is widget-driven and pulled from the global UI widget bitmap pool.

---

## §3 — Coordinate frames & surfaces

### Screen geometry (cited)
- DefScreen_Open is called with `x=0, y=0, w=0x280=640, h=0x1e0=480`, hi-resolution variant `hi_w=0x1c2=450, hi_h=0xa0=160` (cls:120 / Init.cpp:56). The high-resolution pair (450,160) is the pane's *content* hi-res viewport target — it is copied into `this[0x2c]/[0x2d]` (= `mbr_0xb0/0xb4`) by `LoadAndShow` (`FUN_00435040_DefScreen_LoadAndShow.cpp:24,26`).
- Pane outer rect → `mbr_0x14/0x18` (left/top) and `mbr_0x10/0x20` (right/bottom). The ctor (`cls_0x5a4d6c.cpp:262-275`) initializes `mbr_0x10 = 0x1e0 = 480`, `mbr_0xc = 0x1c = 0x280 = 640` — so the pane spans the full display.
- The **pane width field (`mbr_0xc` per UI_METHOD_MAP §15a-equivalent for DefPane)** = **640** (constant, this+0xc).
- The **chrome** (`createcharnotex.Background`) is 640×480, blitted at `pane-local (0,0)`.

### Frame table (mandatory)
| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|-------|--------|--------|--------------------------------------|--------------------------|------|
| **screen** | (root) | TL | `(0,0)` | framebuffer | trivial |
| **pane** | screen | TL | `(0,0)` (DefScreen_Open args `x=0,y=0`) | the whole CharCreate screen | `Init.cpp:56` |
| **chrome (Background bitmap)** | pane | TL | `(0,0)` | the 640×480 engraved-iron frame + "CHARACTER" title | `FUN_00435040_DefScreen_LoadAndShow.cpp:31` (`background` token) |
| **playerframe (LEFT content panel)** | pane | TL | `(35, 70)` | left content panel (portrait + name/pass/gold/level/exper + class info + stats/skills/options pages) | `createchar.def:8` |
| **playerframe2 (LEFT, hidden)** | pane | TL | `(35, 70)` | hidden override variant (different chrome `ClearFrame1`) | `createchar.def:12-13` |
| **addframe (RIGHT content panel)** | pane | TL | `(320, 70)` | MP-only right panel (DROPLIST + LISTBOXes + Add/Del/Equip buttons) | `createchar.def:9` |
| **addframe2 (RIGHT, hidden)** | pane | TL | `(320, 70)` | hidden variant | `createchar.def:14-15` |
| **tabframe (LOWER tab content)** | pane | TL | `(42, 295)` | the area where Stats / Skills / Options pages render (260×137) | `createchar.def:10` |
| **portraitframe** | pane | TL | `(45, 80)` (104×104) | chrome ring around the portrait | `createchar.def:23` |
| **portrait BITMAP** | pane | TL | `(47, 82)` (100×100) | the i3d-baked character portrait icon | `createchar.def:22` |

### Composition formula
Because the pane origin = screen `(0,0)` and every widget's `POS x y w h` is **pane-local** (DEF parser convention — see `DefWidget_DispatchControl` / `DefWidget_ParsePanel` @ 0x437000 / 0x436ec0), the composition reduces to:

```
screen_x = pane_local_x = createchar.def POS x
screen_y = pane_local_y = createchar.def POS y
```

The FRAME widgets (`playerframe`, `addframe`, `tabframe`) **do not introduce a new coordinate frame for their children** — `createchar.def` puts the labels/EDITs/BUTTONs at pane-local coords that happen to fall *inside* the frame bitmaps. There is no automatic re-anchoring of children to the parent frame's TL. (Verify: every TEXT/EDIT/BUTTON inside the "left panel area" has y in 80–270 range, matching `playerframe.y=70..70+370=440`; the absolute POS is the source-of-truth.) This is unlike a hand-coded panel where the chrome surface is its own frame; here FRAME is a chrome-painter, not a layout container.

### Scratch / mosaic surfaces at the screen level

- **`Init.cpp:18`** allocates an offscreen 100×100 buffer:
  `iVar3 = FUN_004a1ec0(100, 100, *(uint*)(PTR_DAT_005d79e0 + 0x38) & 0x3001f, 0); this->mbr_0x27c = iVar3;`
  - `FUN_004a1ec0(w, h, format, flags)` = `TSurface::Create(w, h, pixel_format_mask, 0)` (seen in InventorySidebarPane DrawGrid, BarInv, several other panels — surface factory).
  - The format `*(uint*)(PTR_DAT_005d79e0 + 0x38) & 0x3001f` extracts pixel-format bits (NOMENCLATURE §3 references the same mask) from the display surface, so this scratch matches the display's format.
  - **Purpose: portrait scratch.** 100×100 matches the BITMAP "portrait" cell `POS 47 82 100 100` in `createchar.def:22`. The class likely caches the live portrait render here for the FIELD-bound BITMAP widget. The portrait pipeline itself is `CharacterPortrait_SPEC.md` (baked .i3d icon via `InventoryImage()` → state-0 GetInvImage).
- **`Init.cpp:35`** allocates a variable-size stat-row scratch:
  ```
  iVar8 = max-over-all-classes (TObjectClass.classdata.numspells_or_skills_arr_size)  // *(int*)(*piVar10 + 0x24)
  dVar3 = FUN_00482fb0(iVar8 * 4);  this->mbr_0x288 = dVar3;
  ```
  - This is a `malloc(maxN * sizeof(int))` for an int-array indexed by stat/skill count. Stored at `this+0x288` (= `param_1[0xa2]` int-index in the simplified form).
  - Used for per-class stat-allocation bookkeeping (each character class has its own min/max stat requirements; this array holds the per-stat current values during the allocation UI).

These are **per-class data buffers, not draw surfaces**, in the layout-frame sense. They are not blitted to the pane; the DEF engine reads from them via FIELD bindings (still UNCONFIRMED which FIELD names → which getter — see §14).

### Layout diagram (pane = screen, units = pane-local px)

```
pane  (640x480, origin = screen 0,0)
┌────────────────────────────────────────────────────────────────────────────────┐
│  CHARACTER (gold title bar @ top-center, baked into chrome bitmap)            │
│                                                                                │
│  ┌──────────────────────────────┐         ┌──────────────────────────────────┐ │
│  │ playerframe @ (35,70) 276x370│         │ addframe @ (320,70) 280x338      │ │
│  │  ┌────────┐                  │         │  (MP-only: Type DROPLIST,        │ │
│  │  │portrait│ Char:  [txtchar] │         │   classlist, lgi/lpi LISTBOXes,  │ │
│  │  │ 100x100│ Class: [txtclass]│         │   Add/Del/Equip BUTTONs)         │ │
│  │  │ @(47,  │ classinfo1..4    │         │                                  │ │
│  │  │  82)   │ [Prev] [Next]    │         │                                  │ │
│  │  └────────┘                  │         │                                  │ │
│  │  Name: [editname]            │         │                                  │ │
│  │  Pass: [editpass] Gold: [..] │         │                                  │ │
│  │  Level: [editlevel]  Exper.. │         │                                  │ │
│  │                              │         │                                  │ │
│  │  ┌───[Stats][Skills][Opt]──┐ │         │                                  │ │
│  │  │ tabframe @ (42,295)      │ │         │                                  │ │
│  │  │ ─ stats page (default) ─ │ │         │                                  │ │
│  │  │  Strn:[] Rflx:[]          │ │         │                                  │ │
│  │  │  Cons:[] Mind:[]          │ │         │                                  │ │
│  │  │  Agil:[] Luck:[]          │ │         │                                  │ │
│  │  │  st_txt_info1..3 (cyan)   │ │         │                                  │ │
│  │  └──────────────────────────┘ │         │                                  │ │
│  │                              │         │                                  │ │
│  │                              │         │                                  │ │
│  └──────────────────────────────┘         └──────────────────────────────────┘ │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
```

(The lower tabframe overlaps the bottom of the left playerframe — this is intentional per the DEF; the radio buttons sit *above* the tabframe TL at y=271 with tabframe TL at y=295.)

---

## §4 — Static element layout

> All coords below are **pane-local** = **screen** (pane origin = `(0,0)`). FRAME containers do not re-anchor children.

### §4.1 — Two-stacked-frames + ok/cancel + portrait

| element | space | (x, y) | (w, h) | role | cite |
|---------|-------|--------|--------|------|------|
| chrome `Background` (createcharnotex) | pane | (0, 0) | 640 × 480 | full-screen iron-frame chrome | implicit, drawn at the DefScreen background slot — `LoadAndShow:31` |
| FRAME `playerframe` (chrome `TransFrame1`, FRAME 14 14 16 16) | pane | (35, 70) | 276 × 370 | left content panel | `createchar.def:8` |
| FRAME `addframe` (chrome `TransFrame1`, FRAME 14 14 16 16) | pane | (320, 70) | 280 × 338 | right content panel (MP) | `createchar.def:9` |
| FRAME `tabframe` (chrome `ClearFrame2`) | pane | (42, 295) | 260 × 137 | bottom tab content area | `createchar.def:10` |
| BUTTON `ok` (text "Ok") | pane | (370, 415) | 80 × 27 | confirm | `createchar.def:18` |
| BUTTON `cancel` (text "Cancel") | pane | (475, 415) | 80 × 27 | dismiss | `createchar.def:19` |
| BITMAP `portrait` (FIELD `"portrait"`) | pane | (47, 82) | 100 × 100 | character portrait (i3d-baked icon) | `createchar.def:22` |
| FRAME `portraitframe` (chrome `ClearFrame2`, NOCENTER) | pane | (45, 80) | 104 × 104 | portrait chrome ring | `createchar.def:23` |

### §4.2 — Left panel: identity fields

| element | space | (x, y) | (w, h) | type | text / FIELD / MAXLEN | cite |
|---------|-------|--------|--------|------|-----------------------|------|
| TEXT "Name:" | pane | (45, 196) | 50 × 15 | label (RIGHT) | `"Name:"` | `:26` |
| TEXT "Pass:" | pane | (45, 221) | 50 × 15 | label (RIGHT) | `"Pass:"` | `:27` |
| TEXT "Gold:" | pane | (175, 221) | 50 × 15 | label (RIGHT) | `"Gold:"` | `:28` |
| TEXT "Level:" | pane | (50, 246) | 45 × 15 | label (RIGHT) | `"Level:"` | `:29` |
| TEXT "Exper:" | pane | (155, 246) | 55 × 15 | label (RIGHT) | `"Exper:"` | `:30` |
| EDIT `editname` | pane | (95, 195) | 200 × 20 | text input | FIELD `"editname"` MAXLEN 32 | `:32` |
| EDIT `editpass` | pane | (95, 220) | 90 × 20 | text input | FIELD `"editpass"` MAXLEN 10 | `:33` |
| EDIT `editgold` | pane | (225, 220) | 70 × 20 | text input | FIELD `"editgold"` MAXLEN 8 | `:34` |
| EDIT `editlevel` (SPIN) | pane | (95, 245) | 60 × 20 | numeric spinner | FIELD `"editlevel"` MAXLEN 3 | `:35` |
| EDIT `editexp` | pane | (210, 245) | 85 × 20 | text input | FIELD `"editexp"` MAXLEN 12 | `:36` |

### §4.3 — Left panel: class info

| element | space | (x, y) | (w, h) | type | text / FIELD | style override | cite |
|---------|-------|--------|--------|------|--------------|----------------|------|
| TEXT "Char:" | pane | (160, 83) | 50 × 17 | label (RIGHT) | `"Char:"` | default Med | `:39` |
| TEXT "Class:" | pane | (160, 100) | 50 × 17 | label (RIGHT) | `"Class:"` | default Med | `:40` |
| TEXT `txtchar` | pane | (210, 83) | 70 × 17 | dynamic | FIELD `"txtchar"` | CLEARBG | `:42` |
| TEXT `txtclass` | pane | (210, 100) | 70 × 17 | dynamic | FIELD `"txtclass"` | CLEARBG | `:43` |
| TEXT `classinfo1` | pane | (153, 120) | 152 × 10 | label | "Attack: +5% Defense +10%" (placeholder) | font `Small`, COLOR `(0,150,255)`, CENTER + CLEARBG | `:45` |
| TEXT `classinfo2` | pane | (153, 130) | 152 × 10 | label | "Mana: +5% Steath: +10%" (placeholder) | as above | `:46` |
| TEXT `classinfo3` | pane | (153, 140) | 152 × 10 | label | "Bows +10%" (placeholder) | as above | `:47` |
| TEXT `classinfo4` | pane | (153, 150) | 152 × 10 | label | "Strn: >20 Mind: <15" (placeholder) | as above | `:48` |
| BUTTON `btnclassprev` | pane | (175, 165) | 50 × 20 | class prev | "Prev" | default | `:51` |
| BUTTON `btnclassnext` | pane | (230, 165) | 50 × 20 | class next | "Next" | default | `:52` |

### §4.4 — Tab selector (radio group 1)

| element | space | (x, y) | (w, h) | type | text | flags | cite |
|---------|-------|--------|--------|------|------|-------|------|
| BUTTON `btnstats` | pane | (45, 271) | 70 × 20 | radio | "Stats" | `BTNFLAG_RADIO \| BTNFLAG_DOWN` (default-selected) GROUP 1 | `:55` |
| BUTTON `btnskills` | pane | (120, 271) | 70 × 20 | radio | "Skills" | `BTNFLAG_RADIO` GROUP 1 | `:56` |
| BUTTON `btnopts` | pane | (195, 271) | 70 × 20 | radio | "Options" | `BTNFLAG_RADIO` GROUP 1 | `:57` |

### §4.5 — Stats page (page 1 only; only one page exists)

Cell stride: 25 px between stat rows (y = 313, 338, 363). Left column at x=55 (label) / x=110 (EDIT). Right column at x=155 (label) / x=210 (EDIT).

| element | space | (x, y) | (w, h) | type | text / FIELD | cite |
|---------|-------|--------|--------|------|--------------|------|
| TEXT `st_txt_strn` | pane | (55, 313) | 52 × 20 | label RIGHT | "Strn:" | `:61` |
| TEXT `st_txt_cons` | pane | (55, 338) | 52 × 20 | label RIGHT | "Cons:" | `:62` |
| TEXT `st_txt_agil` | pane | (55, 363) | 52 × 20 | label RIGHT | "Agil:" | `:63` |
| TEXT `st_txt_rflx` | pane | (155, 313) | 52 × 20 | label RIGHT | "Rflx:" | `:64` |
| TEXT `st_txt_mind` | pane | (155, 338) | 52 × 20 | label RIGHT | "Mind:" | `:65` |
| TEXT `st_txt_luck` | pane | (155, 363) | 52 × 20 | label RIGHT | "Luck:" | `:66` |
| EDIT `st_edit0_strn` (SPIN) | pane | (110, 315) | 55 × 20 | numeric | FIELD `"st_edit0_strn"` MAXLEN 3 default `"13"` | `:68` |
| EDIT `st_edit1_cons` (SPIN) | pane | (110, 340) | 55 × 20 | numeric | FIELD `"st_edit1_cons"` MAXLEN 3 default `"13"` | `:69` |
| EDIT `st_edit2_agil` (SPIN) | pane | (110, 365) | 55 × 20 | numeric | FIELD `"st_edit2_agil"` MAXLEN 3 default `"13"` | `:70` |
| EDIT `st_edit3_rflx` (SPIN) | pane | (210, 315) | 55 × 20 | numeric | FIELD `"st_edit3_rflx"` MAXLEN 3 default `"13"` | `:71` |
| EDIT `st_edit4_mind` (SPIN) | pane | (210, 340) | 55 × 20 | numeric | FIELD `"st_edit4_mind"` MAXLEN 3 default `"13"` | `:72` |
| EDIT `st_edit5_luck` (SPIN) | pane | (210, 365) | 55 × 20 | numeric | FIELD `"st_edit5_luck"` MAXLEN 3 default `"13"` | `:73` |
| TEXT `st_txt_info1` | pane | (45, 392) | 250 × 10 | info line | "" (FIELD content via MPSTATINFO1 format) | font `Small`, COLOR `(0,150,255)`, CENTER + CLEARBG + SHADOW | `:75-76` |
| TEXT `st_txt_info2` | pane | (45, 402) | 250 × 10 | info line | "" (MPSTATINFO2) | as above | `:78-79` |
| TEXT `st_txt_info3` | pane | (45, 412) | 250 × 10 | info line | "" (MPSTATINFO3) | as above | `:81-82` |

> The `st_txt_info1..3` strings are filled at runtime via the localization keys `MPSTATINFO1/2/3` (loaded into `this+0x1b8/0x1bc/0x1c0` at Init `:37-42` / cls:`100-105`). The format strings are:
> - `MPSTATINFO1 = "Atck: %d Def: %d  Hlth: %d Fat: %d Mana: %d"` (`english.def`)
> - `MPSTATINFO2 = "Atck Mod: %s%d Def Mod: %s%d Luck Mod: %s%d"`
> - `MPSTATINFO3 = "Hlth Mod: %s%d%% Fat Mod: %s%d%% Mana Mod: %s%d%%"`

### §4.6 — Skills pages (3 pages × ≈4 rows + Exp column)

Each page reuses the same y stride (y = 303, 328, 353, 378) and is swapped by the Prev/Next buttons at (90,405)/(205,405).

#### Page 1 (Attack / Defense / Invoke / Hands)

| element | space | (x, y) | (w, h) | text / FIELD | cite |
|---------|-------|--------|--------|--------------|------|
| TEXT `skl_p1txtattack` | pane | (45, 303) | 73 × 20 | "Attack:" RIGHT | `:86` |
| TEXT `skl_p1txtdefense` | pane | (45, 328) | 73 × 20 | "Defense:" RIGHT | `:87` |
| TEXT `skl_p1txtinvoke` | pane | (45, 353) | 73 × 20 | "Invoke:" RIGHT | `:88` |
| TEXT `skl_p1txthands` | pane | (45, 378) | 73 × 20 | "Hands:" RIGHT | `:89` |
| EDIT `skl_p1editattack` (SPIN) | pane | (120, 305) | 55 × 20 | FIELD MAXLEN 4 default `"13"` | `:91` |
| EDIT `skl_p1editdefense` (SPIN) | pane | (120, 330) | 55 × 20 | FIELD MAXLEN 4 default `"13"` | `:92` |
| EDIT `skl_p1editinvoke` (SPIN) | pane | (120, 355) | 55 × 20 | FIELD MAXLEN 4 default `"13"` | `:93` |
| EDIT `skl_p1edithands` (SPIN) | pane | (120, 380) | 55 × 20 | FIELD MAXLEN 4 default `"13"` | `:94` |
| TEXT `skl_p1txtexp1..4` | pane | (175, 303/328/353/378) | 35 × 20 | "Exp:" RIGHT | `:96-99` |
| EDIT `skl_p1editattackexp..hands` | pane | (212, 305/330/355/380) | 65 × 20 | FIELD MAXLEN 12 (no default) | `:101-104` |

#### Page 2 (Knife / Sword / Bludgeon / Axe)
Same grid (45 / 120 / 175 / 212 columns; y = 303/328/353/378). See `createchar.def:107-125`.

#### Page 3 (Bow / Stealth / LockPick) — only 3 rows
Same grid; rows y = 303/328/353. See `createchar.def:128-142`.

#### Skill nav
| element | space | (x, y) | (w, h) | text | cite |
|---------|-------|--------|--------|------|------|
| BUTTON `skl_prev` | pane | (90, 405) | 50 × 20 | "Prev" | `:145` |
| BUTTON `skl_next` | pane | (205, 405) | 50 × 20 | "Next" | `:146` |
| TEXT `skl_page` | pane | (148, 406) | 50 × 20 | "Page 1" (default; updated by script/code) | `:147` |

### §4.7 — Options page

| element | space | (x, y) | (w, h) | type | text | flags | cite |
|---------|-------|--------|--------|------|------|-------|------|
| TEXT `opt_txtdesc` | pane | (45, 300) | 250 × 18 | label | "Multiplayer Options" | TEXTFLAG_CENTER | `:150` |
| BUTTON `opt_pker` | pane | (55, 330) | 200 × 20 | toggle | "Player Killer" | TOGGLE+DRAWTOGGLE | `:151` |
| BUTTON `opt_view` | pane | (55, 350) | 200 × 20 | toggle | "Viewable by Other Players" | TOGGLE+DRAWTOGGLE | `:152` |
| BUTTON `opt_edit` (commented out) | pane | (55, 370) | 200 × 20 | toggle | "Editable by Host" | — (disabled by retail) | `:153` |

### §4.8 — Right panel (MP inventory, addframe)

| element | space | (x, y) | (w, h) | type | text / FIELD | cite |
|---------|-------|--------|--------|------|--------------|------|
| TEXT (anonymous) | pane | (325, 79) | 40 × 20 | label | "Type" | SHADOW only | `:156` |
| DROPLIST `classlist` | pane | (365, 78) | 135 × 20 | FIELD `"classfield"` DROPFLAG_USEFORMAT LISTHEIGHT 108 | inner FIELD 5,-2,160,20 | `:157-160` |
| TEXT `itemqtytxt` | pane | (501, 78) | 30 × 20 | label | "Qty" SHADOW | `:163` |
| EDIT `itemqty` | pane | (530, 78) | 50 × 20 | numeric input | "1" MAXLEN 5 | `:164` |
| LISTBOX `longgameinvicon` | pane | (327, 105) | 266 × 131 | VLIST + USEFORMAT + DBLCLICK ITEM 39×40 | inner BITMAP "lgi_iconbmp" + FIELD qty | `:177-182` |
| LISTBOX `longgameinv` | pane | (327, 105) | 266 × 131 | VLIST + USEFORMAT + DBLCLICK ITEM 1000×40 | inner FIELDs name/value/stat + BITMAP icon + qty | `:184-193` |
| BUTTON `add` (KEY "A") | pane | (337, 241) | 45 × 23 | "Add" | default | `:196` |
| BUTTON `del` (KEY "D") | pane | (387, 241) | 45 × 23 | "Del" | default | `:197` |
| BUTTON `equip` (KEY "E") | pane | (437, 241) | 45 × 23 | "Equip" | default | `:198` |
| BUTTON `icon` (KEY "I") | pane | (493, 246) | 95 × 15 | "Icons Only" | TOGGLE+DRAWTOGGLE | `:199` |
| LISTBOX `longplayerinvicon` | pane | (327, 269) | 266 × 131 | VLIST + USEFORMAT + DBLCLICK ITEM 39×40 | inner BITMAP + qty | `:201-206` |
| LISTBOX `longplayerinv` | pane | (327, 269) | 266 × 131 | VLIST + USEFORMAT + DBLCLICK ITEM 1000×40 | inner FIELDs name/value/stat/eq/qty + BITMAP icon | `:208-218` |

> **Mirroring caveat (NOMENCLATURE §2).** The right panel is **NOT a mirror** of the left — it is an MP-only inventory transfer panel. There is no x-flip. The "two columns" in the screen are independent widget hierarchies authored separately in the DEF.

---

## §5 — Draw order / composition

The DEF widget engine drives the entire paint. The order is fixed by the engine (`DefWidget_ParseBuffer` instantiates widgets in DEF source order and the renderer walks them in registration order):

1. **DefScreen background** — the chrome bitmap loaded from `createcharnotex.dat` "Background" via `DefScreen_LoadAndShow`'s `(*+0xa0)("background", -1)` call (`FUN_00435040_DefScreen_LoadAndShow.cpp:31`) is blitted opaque at pane `(0,0)`. This is the "CHARACTER"-titled engraved-iron border.
2. **FRAME widgets** (in DEF source order, only if not hidden):
   - `playerframe` (35,70,276,370) → `TransFrame1` 9-slice chrome with FRAME corners (14,14,16,16) and MARGINS (0,0,4,4). FLAG `CTRLFLAG_DRAWTOBG` so it draws to the pane background.
   - `addframe` (320,70,280,338) → same chrome.
   - `tabframe` (42,295,260,137) → `ClearFrame2` chrome, FLAG `CTRLFLAG_CLEARBG`.
   - `playerframe2` / `addframe2` are skipped (CTRLFLAG_HIDDEN).
3. **Standard exit buttons** ok/cancel (drawn via shared `BUTTON STYLE NORMAL` from `widgets.def:88-93` — UP `TransFrame2`, DOWN `FillFrame2`).
4. **Portrait BITMAP + portraitframe** (data-bound via FIELD "portrait" — the BITMAP widget queries the class for the icon).
5. **Character stuff** labels + EDITs (`:26-36`).
6. **Class stuff** Char/Class labels + dynamic TEXT widgets + cyan classinfo lines + Prev/Next.
7. **Tab selector** (3 radios).
8. **Stats** page (default-visible since `btnstats` has `BTNFLAG_DOWN`).
9. **Skills** + **Options** pages registered but conditionally hidden by the radio-group state (Stats/Skills/Options widgets share the tabframe area; only one set is visible at a time, the controller logic toggles their CTRLFLAG_HIDDEN state).
10. **MP inventory side** — Type/DROPLIST, Qty/EDIT, LISTBOXes, Add/Del/Equip/Icon buttons.

Per-widget draw goes through the engine's `DefWidget_RenderWalker` (renderer not yet pinned — `DefWidgetEngine.md:79-80`). Each widget calls its own `DrawBG` (if `CTRLFLAG_DRAWTOBG`) into the parent pane bitmap or its own scratch, then composites to the screen. Text widgets ultimately funnel through `FUN_004be2b0` (UI_METHOD_MAP §5) with font index + color + flags from the DEF/STYLE.

---

## §6 — Algorithms (pseudocode per helper)

### §6.1 — Init body (vtable slot 0 @ `0x465a50`)

```text
TCharCreateScreen::Initialize():
    // 1) Allocate a 100x100 scratch surface matching display pixel-format bits
    //    (used as the portrait cache)
    this->mbr_0x27c = SurfaceCreate(100, 100,
                                    display.flags & 0x3001f,  // PTR_DAT_005d79e0 + 0x38
                                    0)
    // 2) Allocate an int[N] buffer where N = max over all TObjectClass.classdata[+0x24]
    //    (the highest stat/skill count any class declares).
    //    Stored per-class so each class radio swaps its own value-set in/out.
    int maxN = 0
    for cls in TObjectClass.classes[0 .. numclasses):  // DAT_0065a148 .. count DAT_0065a258
        if (cls != null) maxN = max(maxN, cls->fields[0x24])  // <-- classdata.skill_count
    this->mbr_0x288 = malloc(maxN * sizeof(int))

    // 3) Resolve localization keys to lookup-table indices (TextResource::Lookup family)
    this->mbr_0x1b8 = TextResource::Lookup("MPSTATINFO1")        // "Atck: %d Def: %d ..."
    this->mbr_0x1bc = TextResource::Lookup("MPSTATINFO2")        // "Atck Mod: %s%d ..."
    this->mbr_0x1c0 = TextResource::Lookup("MPSTATINFO3")        // "Hlth Mod: %s%d%% ..."
    this->mbr_0x1d4 = TextResource::Lookup("PLYRVALUES")          // "Health,Fatigue,Mana"
    this->mbr_0x1d8 = TextResource::Lookup("PLYRSTATS")           // "Strn,Cons,Agil,Rflx,Mind,Luck"
    this->mbr_0x1dc = TextResource::Lookup("PLYRSKILLS")          // "Attack,Defense,Invoke,Hands,..."

    // 4) Compute flags for DefScreen_Open
    //    mbr_0x190 = the screen's *mode* (1=MP host-edit, 2=MP join, else=SP)
    //    The "MP edit" modes use flags 0x11; SP uses 0x00. (0x11 = 0x10 | 0x1; UNCONFIRMED-A which bits.)
    int flags = (mbr_0x190 == 1 || mbr_0x190 == 2) ? 0x11 : 0x00

    // 5) Open the DEF screen
    if (DAT_0065c890 == 0):
        //  -- Classic 15-bit path (createcharnotex.dat handle wasn't loaded by TPlayScreen
        //     yet -- shouldn't happen post-TPlayScreen::Initialize but covers stand-alone use)
        ok = this->DefScreen_Open(
                panel        = "createchar",       // -> matches PANEL "default" in createchar.def
                screen_root  = &DAT_00659c80,      // -> the DefScreen root struct ptr
                flags        = flags,              // 0x00 or 0x11
                x = 0, y = 0,
                w = 0x280 = 640, h = 0x1e0 = 480,  // pane spans full display
                hi_w = 0x1c2 = 450, hi_h = 0xa0 = 160,  // hi-res sub-content target
                widgets_lib  = "widgets",          // load widgets.def style library
                bg_dat       = "createchar")       // -> "createchar.dat" (the .dat name token)
    else:
        // 16-bit/texture path -- uses the already-loaded createchartex.dat handle.
        ok = this->DefScreen_Open2(
                panel        = "createchar",       // (s_createchar_005d32e0; different intern)
                screen_root  = &DAT_00659c7c,      // -> different DefScreen root struct
                flags        = flags,
                x = 0, y = 0,
                w = 0x280, h = 0x1e0,
                hi_w = 0x1c2, hi_h = 0xa0,
                screen_handle = DAT_0066733c,      // ?? (probably the prior open screen ptr)
                bg_handle     = DAT_0065c890)      // -> loaded createchartex.dat resource handle

    if (!ok) return 0     // Init failure -- screen not shown

    // 6) Mark widgets dirty + enable redraw
    this->mbr_0x1f4 = 1   // "dirty" / "needs first paint"
    this->mbr_0x1f0 = 1   // "is visible / active"

    // 7) If we're in MP-join mode (mbr_0x194 == 2), spawn a default Locke + name him
    if (this->mbr_0x194 == 2):
        TPlayer* p = NewObjectByName("Locke", 0, -1, 1)  // class id = 0xffffffff -> auto
        this->mbr_0x1f8 = p
        p->Init("Player Name")    // FUN_0046e6f0 sets the default in-game name

    // 8) Tell the base to do a first paint + transition to "open" state
    this->vftable.FUN_00435de0_80()      // slot 80: DefScreen show/activate
    this->vftable.virt_meth_0x444fc0_44(0)  // slot 44: SetSomething(0) -- UNCONFIRMED

    // 9) If the current screen is not the global PlayScreen (i.e. we're booting
    //    NOT from TPlayScreen but from selstart -> this is the stand-alone editor),
    //    skip the strdup of the player name + player notes -- there's no player yet.
    if (DAT_00667fd0 /* CurrentScreen */ != &DAT_0065caf0 /* g_TPlayScreen */):
        this->mbr_0x19c = 0    // player notes ptr
        this->mbr_0x198 = 0    // player name ptr
        return 1

    // 10) We DO have a TPlayScreen running -- strdup the live player's
    //     name (mbr_0x38 of player object) into mbr_0x198 and the player's
    //     notes (mbr_0x378 of player object) into mbr_0x19c.
    //     The two long copy loops are inlined strlen + malloc(len+1) + strcpy.
    this->mbr_0x198 = strdup(*(char**)(this->mbr_0x1f8 + 0x38))   // player.name
    this->mbr_0x19c = strdup((char*)(this->mbr_0x1f8 + 0x378))    // player.notes
    return 1
```

### §6.2 — Hot-reload (`R` key, vtable slot 19 @ `0x469340`)

```text
TCharCreateScreen::HotReload():
    cls_0x5b93c4::virt_meth_0x435d70(this)   // base re-load (re-parse createchar.def)
    if (DAT_00676e84 != 0):                  // some "in-game / debug" flag
        cls_0x5b98b8::meth_0x437ca0(this)    // re-parse DEF buffer
        cls_0x5b98b8::meth_0x465d50(this)    // <-- TCharCreate post-reload hook A
        cls_0x5b98b8::meth_0x465ea0(this)    // <-- TCharCreate post-reload hook B
    return
```
(The two post-reload hooks are presumably "re-bind data" + "redraw" — extract on demand.)

### §6.3 — Dtor body (vtable slot 1 @ `0x465ce0`)

```text
TCharCreateScreen::~Body():
    free(this->mbr_0x27c)        // 100x100 portrait scratch
    free(this->mbr_0x288)        // int[maxN] stat-allocation array
    if (this->mbr_0x198 != 0): free(this->mbr_0x198); this->mbr_0x198 = 0
    if (this->mbr_0x19c != 0): free(this->mbr_0x19c); this->mbr_0x19c = 0
    cls_0x5b98b8::virt_meth_0x434f30(this)  // base teardown
```

### §6.4 — `DefScreen_Open(name, screen, flags, x, y, w, h, hi_w, hi_h, widgets, bg)` (slot 132 @ `0x435150`)

(see `recon/discovered/FUN_00435150_DefScreen_Open.cpp`)

```text
DefScreen_Open(this, panel_name, screen_root, flags, x, y, w, h, hi_w, hi_h, widgets_lib, bg_name):
    TButtonPane::Initialize(this)            // FUN_00434e40 -- base pane init
    this->mbr_0x180 /* +0x60 */ = flags
    if (widgets_lib != 0):
        if (stricmp(widgets_lib, "widgets") != 0):
            FUN_00435990(widgets_lib)        // alt widget library
        else:
            // standard "widgets" library; swap in the cached parsed buffer
            if (flags & 1)
                // ...lock/swap mp variant DAT_0066733c into this->[0x74/0x78]
            else
                // ...lock/swap base variant DAT_0065bb10 into this->[0x74/0x78]
    if (bg_name != 0):
        FUN_00435b20(bg_name)                // set BG resource name
    DefScreen_LoadAndShow(this, panel_name, screen_root, flags, x, y, w, h, hi_w, hi_h)
    return
```

### §6.5 — `DefScreen_LoadAndShow` (called sub-helper, @ `0x435040`)

```text
DefScreen_LoadAndShow(this, panel_name, screen_root, flags, x, y, w, h, hi_w, hi_h):
    this->mbr_0xb0 /*+0x2c=stackArg0x20*/ = hi_w
    this->mbr_0xb4 /*+0x2d=stackArg0x24*/ = hi_h
    this->mbr_0x14 /*+5*/ = x   // pane TL x
    this->mbr_0x18 /*+6*/ = y   // pane TL y
    // ... stack 0x18,0x1c stored at uStack_60,5c (then re-used as a copy
    //     of the BG-name buffer for the path-build below)
    this->vftable.slot_8()       // some "reset / clear" hook
    bg_handle = this->vftable.slot_0xa0(s_background_005cd8d4 = "background", -1)
    FUN_00435660(bg_handle)      // store the chrome handle (this->vftable +0x68 area)
    FUN_00435380()               // some prep
    if (panel_name != null):
        strncpy(&this[0x56], panel_name, 31)   // panel name slot
        this[0x177] = 0
    if (screen_root_name != null):
        strncpy(&local_stack, screen_root_name, 79)
        // ... strip ".def" suffix, build "screen_root_name.def"
        DefWidget_LoadFile(&local_stack)        // FUN_004377c0 -- read createchar.def
        if (load failed) return 0
    this->vftable.slot_0x2c(1)    // SetVisible(true) -- pane becomes active
    return 1
```

### §6.6 — Field offset map (every field touched by Init, dtor or vftable index)

Mapping from `param_1[N]` (int* index from extracted FUN body) to `mbr_0xNNN` (OOAnalyzer struct offset):

| `param_1[N]` (Init.cpp) | byte offset = `4N` | OOAnalyzer field | semantic | cite |
|--|--|--|--|--|
| `[0x65]` = 0x65*4 = 0x194 | 0x194 | `mbr_0x194` | mode (1 / 2 / else, 0x65==2 → spawn Locke) | Init.cpp:68 |
| `[100]` = 100*4 = 0x190 | 0x190 | `mbr_0x190` | screen mode (1/2 → flags=0x11) | Init.cpp:49 |
| `[0x66]` = 0x198 | 0x198 | `mbr_0x198` | player-name strdup ptr | Init.cpp:76,114 |
| `[0x67]` = 0x19c | 0x19c | `mbr_0x19c` | player-notes strdup ptr | Init.cpp:76,146 |
| `[0x6e..0x70]` = 0x1b8..0x1c0 | 0x1b8/0x1bc/0x1c0 | `mbr_0x1b8/0x1bc/0x1c0` | MPSTATINFO1/2/3 text-id | Init.cpp:37-42 |
| `[0x75..0x77]` = 0x1d4..0x1dc | 0x1d4/0x1d8/0x1dc | `mbr_0x1d4/0x1d8/0x1dc` | PLYRVALUES/STATS/SKILLS text-id | Init.cpp:43-48 |
| `[0x7c]` = 0x1f0 | 0x1f0 | `mbr_0x1f0` | visible/active flag | Init.cpp:67 |
| `[0x7d]` = 0x1f4 | 0x1f4 | `mbr_0x1f4` | dirty flag | Init.cpp:66 |
| `[0x7e]` = 0x1f8 | 0x1f8 | `mbr_0x1f8` | TPlayer* default player ("Locke") | Init.cpp:70 |
| `[0x9f]` = 0x27c | 0x27c | `mbr_0x27c` | 100×100 scratch surface | Init.cpp:20 |
| `[0xa2]` = 0x288 | 0x288 | `mbr_0x288` | malloc int[maxN] stat-allocation buffer | Init.cpp:36 |

---

## §7 — Effects & shadows

The screen itself does no effects work — every text/widget effect is owned by the widget. Notes for the reconstructor:
- **Text shadow.** `widgets.def` STYLE TEXT sets default font flags `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW DRAWMODE DM_USEDEFAULT`. **`TEXT_SHADOW = 0x400`** in `widgets.def:24`; that bit is what `FUN_004be2b0` reads as the 3-pass-shadow gate (`param_8 & 0x400`, UI_METHOD_MAP §5 step 8). So every default-styled label gets the standard 3-pass black drop shadow.
- **Per-element overrides.** The `classinfo1..4` + `st_txt_info1..3` widgets set `FLAGS TEXT_CENTER | CTRLFLAG_CLEARBG FONT "Small" COLOR 0 150 255` (createchar.def:45-48,75-82). They keep TEXT_SHADOW from their default STYLE (createchar.def re-asserts `FLAGS TEXT_CENTER | TEXT_SHADOW` on lines 76/79/82 — both flag setters are present, see Init format-flag construction §6).
- **No pink-halo bug exposure.** The chrome is full opaque and the text is alpha-blended via the engine; no pre-cleared magenta cells in the static layout. (If a reconstructor produces a pink halo around text, that's the inherited `FUN_004be2b0` chroma-key bug — UI_METHOD_MAP §16 — not a CharCreate-specific artifact.)
- **No "shadowed bitmap" (FUN_00438d80) on this screen.** All bitmap blits are widget-owned (FRAME 9-slice, EDIT/DROPLIST scratch).

---

## §8 — Text rendering

The DEF parser packs each TEXT widget's `FLAGS` into the bit pattern read by `FUN_004be2b0` (UI_METHOD_MAP §5 step 6). Map:

| DEF flag | bit | meaning |
|----------|-----|---------|
| `TEXT_LEFT` | 0x0001 | h-align left (default if none set) |
| `TEXT_CENTER` | 0x0002 | h-align center |
| `TEXT_RIGHT` | 0x0004 | h-align right |
| `TEXT_TOP` | 0x0010 | v-align top |
| `TEXT_VCENTER` | 0x0040 | v-align center |
| `TEXT_SHADOW` | 0x0400 | 3-pass black drop shadow |
| `TEXT_SINGLELINE` | 0x0080 | force single-line |
| `TEXTFLAG_RIGHT` | 0x00080000 | shorthand: right-justify text in a label widget (used everywhere on the stat labels) |
| `TEXTFLAG_CENTER` | 0x00040000 | shorthand: center text in a label widget |

> The class-internal label flags `TEXTFLAG_RIGHT/CENTER` (0x00080000 / 0x00040000) are different bits from the GDI/DrawTextA flags `TEXT_RIGHT/CENTER` (0x4/0x2). The widget code applies them separately during `FUN_0042b340` (TEXT attr parser) — UNCONFIRMED-B exactly how the high bits are folded into the DrawTextA flag word. The reconstructor must replicate both.

### Static text cells (only the visible ones; pane-local final coords)

| string | cell (x, y, w, h) | font | px | color | h-align | v-align | shadow | format / source | cite |
|--------|--------------------|------|-----|-------|---------|---------|--------|-----------------|------|
| "Name:" | (45, 196, 50, 15) | Med | (Arial ≈12 px) | white `(255,255,255)` (style default) | RIGHT | top (DT_TOP) | yes (style default) | literal | `:26` |
| "Pass:" | (45, 221, 50, 15) | Med | … | white | RIGHT | top | yes | literal | `:27` |
| "Gold:" | (175, 221, 50, 15) | Med | … | white | RIGHT | top | yes | literal | `:28` |
| "Level:" | (50, 246, 45, 15) | Med | … | white | RIGHT | top | yes | literal | `:29` |
| "Exper:" | (155, 246, 55, 15) | Med | … | white | RIGHT | top | yes | literal | `:30` |
| "Char:" | (160, 83, 50, 17) | Med | … | white | RIGHT | top | yes | literal | `:39` |
| "Class:" | (160, 100, 50, 17) | Med | … | white | RIGHT | top | yes | literal | `:40` |
| dyn `txtchar` | (210, 83, 70, 17) | Med | … | white | LEFT | top | yes | FIELD `"txtchar"` (UNCONFIRMED-C getter) | `:42` |
| dyn `txtclass` | (210, 100, 70, 17) | Med | … | white | LEFT | top | yes | FIELD `"txtclass"` (UNCONFIRMED-C getter) | `:43` |
| `classinfo1..4` | (153, 120/130/140/150, 152, 10) | **Small** | (Small ≈ 10 px) | **cyan `(0,150,255)`** | CENTER | top | yes | literal placeholder; runtime fills via class-info getter (UNCONFIRMED-C) | `:45-48` |
| BUTTON "Prev" / "Next" (class) | (175 / 230, 165, 50, 20) | Med | … | white | CENTER | VCENTER (BUTTON STYLE default) | yes | literal | `:51-52` |
| BUTTON "Stats" / "Skills" / "Options" | (45/120/195, 271, 70, 20) | Med | … | white (UP) / black (DOWN) | CENTER | VCENTER | yes | literal | `:55-57` |
| Stat labels "Strn:".."Luck:" | (55 or 155, 313/338/363, 52, 20) | Med | … | white | RIGHT | top | yes | literal | `:61-66` |
| Stat EDIT default "13" | (110/210, 315/340/365, 55, 20) | Med | … | EDITCOLOR `(255,255,0)` yellow | LEFT | VCENTER (EDIT SPIN STYLE) | yes | "13" then user-typed | `:68-73`; widgets.def:131-134 |
| `st_txt_info1..3` | (45, 392/402/412, 250, 10) | **Small** | … | **cyan `(0,150,255)`** | CENTER | top | yes | format = `MPSTATINFO1/2/3` (see below) | `:75-82` |
| Skill labels | (45, 303/328/353/378, 73, 20) | Med | … | white | RIGHT | top | yes | literal "Attack:" etc. | `:86-89, 107-110, 128-130` |
| Skill EDITs | (120, 305/330/355/380, 55, 20) | Med | … | yellow | LEFT | VCENTER | yes | "13" default | `:91-94, 112-115, 132-134` |
| "Exp:" mini-labels | (175, 303/328/353/378, 35, 20) | Med | … | white | RIGHT | top | yes | literal | `:96-99, 117-120, 136-138` |
| Exp EDITs | (212, 305/330/355/380, 65, 20) | Med | … | yellow | LEFT | VCENTER | yes | (no default) | `:101-104, 122-125, 140-142` |
| Skill nav text "Page 1" | (148, 406, 50, 20) | Med | … | white | LEFT | top | yes | "Page 1" (updated by Prev/Next) | `:147` |
| Options "Multiplayer Options" | (45, 300, 250, 18) | Med | … | white | CENTER | top | yes | literal | `:150` |
| Options "Player Killer" / "Viewable …" | (55, 330/350, 200, 20) | Med | … | white (UP/DOWN both white per STYLE BUTTON TOGGLE) | LEFT | top (per STYLE TOGGLE UPLABELRECT 18 -1 0 0) | yes (STYLE TOGGLE) | literal | `:151-152` |
| "Type" / "Qty" / Add/Del/Equip/Icon | (325/501/337/387/437/493, 79/78/241/241/241/246, 40/30/45/45/45/95, 20/20/23/23/23/15) | Med | … | white | (mix) | … | yes | literal | `:156, 163, 196-199` |
| classlist DROPLIST inner FIELD | inner-rect (5,-2,160,20) | Med | … | white | LEFT (TEXT_LEFT) | top | yes (TEXT_SHADOW) | FIELD `"classfield"` | `:159` |
| LISTBOX inner formats (qty/name/value/stat/eq) | inner per-cell from `BEGIN ... END` | Small (qty 28×10) / Med (name 150×18) / Small (value 40×14) | … | per-format COLOR (qty=red `255,0,0`, name=white, value=teal `100,255,255`, stat=yellow `255,255,100`, eq=green `0,255,0`) | per-format align | top | yes | per-row format | `:181-218` |

> **MPSTATINFO format string usage.** At Init `:37-42`, `MPSTATINFO1/2/3` are resolved to text-ids and held in `this->mbr_0x1b8/1bc/1c0`. The runtime uses these as `sprintf`-family templates for `st_txt_info1/2/3`. The format string (resolved from english.def) is fed to `FUN_0058b100(buffer, format, args)` (UI_METHOD_MAP §10) at the per-update path (UNCONFIRMED-E — that update method isn't yet extracted; it lives somewhere in the 0x465e?? to 0x469000 range alongside the other class methods). Args come from the live player object's stat getters (UNCONFIRMED-C).
>
> **PLYRVALUES/STATS/SKILLS usage** = the comma-split source for runtime label generation (e.g. when the radio toggles to Skills page 1, the runtime reads `PLYRSKILLS[0..3]` → "Attack,Defense,Invoke,Hands" and uses them as the page's row labels in place of the static "Attack:".."Hands:" defaults).

---

## §9 — Animation & dynamic behavior

This screen is **not animated** at the chrome level. There are no per-tick tweens on the panel itself. Dynamic redraws are caused by:

- **Field value changes** (EDIT spinner clicks, classlist DROPLIST selection, radio toggles) — each widget owns its own dirty/redraw flag (the DEF engine's `CTRLFLAG_DIRTY = 0x20`, `widgets.def:35`).
- **Stat reallocation** in the int[maxN] buffer triggering MPSTATINFO1/2/3 line refresh.
- **Class switch** (Prev/Next at (175,165)/(230,165)) — recomputes class info text, swaps the per-class stat distribution.

Phases / radio-group toggle: BTN GROUP 1 (`btnstats`/`btnskills`/`btnopts`) controls which sub-area is visible in the lower tabframe. Mutually-exclusive; the not-selected pages have `CTRLFLAG_HIDDEN`. (The exact toggle code is in `meth_0x465d50` / `meth_0x465ea0` — see §6.2 — extracted on demand.)

Skill page index ramp: `skl_prev` / `skl_next` step the skill-page state through 1..3, updating `skl_page` TEXT.

```
ramp page_index:
  range   1..3
  step    +1 / -1 per Next/Prev click
  target  (no auto-target; user-driven)
  maps-to which CTRLFLAG_HIDDEN bit-pattern across skl_p1*/skl_p2*/skl_p3* widgets
  cite    createchar.def:145-147 (buttons + text), UNCONFIRMED-F (the actual flag-toggling code, not yet extracted)
```

No ramp/tween blocks at the chrome level.

---

## §10 — Input & dispatch

Hit rects = the BUTTON / EDIT / DROPLIST `POS x y w h` from §4 (in pane = screen). Dispatch:

- The DEF engine's `DefWidget_DispatchInput` (`0x004361f0`, vtable slot **108** on `cls_0x5a4d6c__vftable_5a4d6c:34`) handles per-widget keyboard + mouse (R = hot-reload, arrow nav, Enter activate, KeyDown fall-through).
- BUTTON `ok` / `cancel` raise WM_COMMAND-like ids — **command ids UNCONFIRMED-G** (the screen's Command/Dispatch slot is on `cls_0x5b93c4`/`cls_0x5b98b8` and not yet bound to a named handler for this class). The brief says "TCharCreateScreen reached from MenuNewGame" — the parent screen (MenuNewGame) must receive an "OK pressed → commit player" notification, but the exact id ↔ method binding is one extraction away.
- KEY shortcuts: `add` KEY "A", `del` KEY "D", `equip` KEY "E", `icon` KEY "I" (`:196-199`). These hit the same dispatch.
- Group-1 radio buttons (`btnstats`/`btnskills`/`btnopts`) — `BTNFLAG_RADIO` makes the engine auto-deselect the other two on click. The default-selected one is `btnstats` (`BTNFLAG_DOWN` in `:55`).
- Toggle buttons (`opt_pker`, `opt_view`, `icon`) — `BTNFLAG_TOGGLE | BTNFLAG_DRAWTOGGLE` flips a stored bool on click and redraws with the alt sprite.
- The Locke pre-populate happens at Init (`:68-71`); no input needed.

| control | hit rect (screen) | dispatched-to (UNCONFIRMED-G) |
|---------|-------------------|-------------------------------|
| `ok` | (370, 415, 80, 27) | Commit player → close → return to parent | 
| `cancel` | (475, 415, 80, 27) | Discard → close → return to parent |
| `btnclassprev` | (175, 165, 50, 20) | Step class index −1, refresh class info + stats |
| `btnclassnext` | (230, 165, 50, 20) | Step class index +1 |
| `btnstats` / `btnskills` / `btnopts` | (45/120/195, 271, 70, 20) | Tab switch (group 1) |
| `skl_prev` / `skl_next` | (90 / 205, 405, 50, 20) | Skill page −1 / +1; update `skl_page` text |
| `opt_pker` / `opt_view` | (55, 330/350, 200, 20) | Toggle PK / Viewable flags |
| `add` / `del` / `equip` / `icon` | (337/387/437/493, 241/241/241/246, 45/45/45/95, 23/23/23/15) | MP inventory transfer; KEY shortcuts A/D/E/I |
| `classlist` DROPLIST | (365, 78, 135, 20) (+ list area 135×108 when open) | Choose object class |
| `itemqty` EDIT | (530, 78, 50, 20) | Numeric qty for add/del |
| EDIT spinners (name/pass/gold/level/exper, all stats, all skills) | per §4 | per-field value change; bound via FIELD name |
| LISTBOXes | (327, 105/269, 266, 131) | Select / dbl-click to add/equip |

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo around text** (the standard chroma-key + 3-pass-shadow artifact, UI_METHOD_MAP §16, [[project-retail-pink-halo-bug]]). Render text with real alpha, not magenta-key.
2. **No createcharNOtex 16-bit hi-res support.** The Classic path `createcharnotex.dat` (BM_15BIT) decodes; the `createchartex.dat` (flags=0x10000) path triggers "unsupported bit-depth" in the current decoder. This is a **port gap** in `bitmapdecode.cpp`, not a retail bug; the retail engine does decode 0x10000. Either teach the decoder or fall back to the 15-bit asset for now.
3. **Commented-out "Editable by Host" button** (`createchar.def:153`). Not a bug; just a deliberately disabled feature. Do not re-enable.
4. **"Steatlh" / "Steath" misspellings** in `createchar.def:46` / `:129`. Cosmetic typo in the retail DEF; preserve verbatim (the reconstructor MUST NOT silently "fix" them — the DEF is the source asset).
5. **`opt_edit` placeholder commented out** — same as #3, do not re-enable.

---

## §12 — Reconstruction pseudocode

> The reconstruction agent does **not** open-code per-element draws — instead it instantiates the DEF widget engine and feeds it `createchar.def` (+ `widgets.def`). At the screen-class level only the data-binding, scratch alloc and the FIELD getters are coded.

```text
class TCharCreateScreen : public TDefScreen {  // (cls_0x5b98b8 / cls_0x5b93c4 derived)
    // -- engine + base class manage all draw via the DEF widget tree.

    int     mode;                   // mbr_0x190
    int     submode;                // mbr_0x194  (==2 -> spawn Locke)
    int     msg_info1, info2, info3; // MPSTATINFO1/2/3 string ids
    int     plyrvalues, plyrstats, plyrskills; // PLYRVALUES/STATS/SKILLS string ids
    bool    dirty, active;
    TPlayer*    player;             // mbr_0x1f8 (the "Locke" or live player)
    char*       player_name;        // mbr_0x198 (strdup of player.name)
    char*       player_notes;       // mbr_0x19c (strdup of player.notes)
    TSurface*   portraitScratch;    // mbr_0x27c (100x100)
    int*        statAlloc;          // mbr_0x288 (int[maxN])

    bool Initialize() {
        portraitScratch = renderer->CreateSurface(100, 100, display.formatBits);
        int maxN = 0;
        for (auto* c : TObjectClass::classes) if (c) maxN = std::max(maxN, c->skillsCount);
        statAlloc = (int*)malloc(maxN * sizeof(int));

        msg_info1   = TextResource::Lookup("MPSTATINFO1");
        msg_info2   = TextResource::Lookup("MPSTATINFO2");
        msg_info3   = TextResource::Lookup("MPSTATINFO3");
        plyrvalues  = TextResource::Lookup("PLYRVALUES");
        plyrstats   = TextResource::Lookup("PLYRSTATS");
        plyrskills  = TextResource::Lookup("PLYRSKILLS");

        int flags = (mode == 1 || mode == 2) ? 0x11 : 0x00;

        bool ok = DefScreen::Open(
            /*panel=*/"createchar", /*screen=*/&g_charCreateRoot,
            /*flags=*/flags, /*x=*/0, /*y=*/0, /*w=*/640, /*h=*/480,
            /*hi_w=*/450, /*hi_h=*/160,
            /*widgets_lib=*/"widgets",
            /*bg_dat=*/"createchar");
        if (!ok) return false;

        dirty = true;
        active = true;

        if (submode == 2) {
            player = TObject::NewObjectByName("Locke", 0, -1, 1);   // 0x474e20
            player->Init("Player Name");                            // 0x46e6f0
        }

        // Vtable slot 80: post-open hook (kick first paint).
        this->OnDefScreenOpened();

        // Vtable slot 44: state-set (probably "activate input").
        this->SetSomething(0);   // UNCONFIRMED-H: which state goes to 0

        if (gCurrentScreen != &gTPlayScreen) {
            player_name = nullptr;
            player_notes = nullptr;
        } else {
            // copy live player name + notes for editing
            player_name  = strdup((const char*)player->field_0x38);
            player_notes = strdup((const char*)player + 0x378);
        }
        return true;
    }

    // Per-FIELD data binding for the widget engine. Called by the engine when a
    // widget with FIELD "<name>" needs its dynamic value. UNCONFIRMED-C: this
    // dispatch table is not yet extracted; the brief layout below is the
    // straightforward implementation matching createchar.def's FIELD names.
    Variant GetField(StringView name) const {
        // identity
        if (name == "portrait")  return player ? player->InventoryImage(/*state=*/0)
                                               : portraitScratch;     // see CharacterPortrait_SPEC
        if (name == "editname")  return player_name;
        if (name == "editpass")  return /* mp password */ "";
        if (name == "editgold")  return player->Gold();
        if (name == "editlevel") return player->Level();
        if (name == "editexp")   return player->Exp();

        // class display
        if (name == "txtchar")   return player->Name();      // -> "Locke" default
        if (name == "txtclass")  return player->ClassName();

        // stats (use PLYRSTATS[i] for label; statAlloc[i] for value)
        if (name == "st_edit0_strn") return statAlloc[0];
        // ... 1..5 similarly

        // skills
        if (name == "skl_p1editattack")    return statAlloc[??];
        // ... etc.

        return Variant{};
    }

    // MPSTATINFO1/2/3 are formatted into st_txt_info1/2/3 here:
    void RefreshStatInfo() {
        char buf1[128], buf2[128], buf3[128];
        snprintf(buf1, sizeof buf1, TextResource::Get(msg_info1),
                 player->Attack(), player->Defense(),
                 player->Health(), player->Fatigue(), player->Mana());
        snprintf(buf2, sizeof buf2, TextResource::Get(msg_info2),
                 sgn(player->AttackMod()), abs(player->AttackMod()),
                 sgn(player->DefenseMod()), abs(player->DefenseMod()),
                 sgn(player->LuckMod()), abs(player->LuckMod()));
        snprintf(buf3, sizeof buf3, TextResource::Get(msg_info3),
                 sgn(player->HealthModPct()), abs(player->HealthModPct()),
                 sgn(player->FatigueModPct()), abs(player->FatigueModPct()),
                 sgn(player->ManaModPct()), abs(player->ManaModPct()));
        // poke into widgets
        widget("st_txt_info1").SetText(buf1);
        widget("st_txt_info2").SetText(buf2);
        widget("st_txt_info3").SetText(buf3);
    }

    bool OnCommand(int cmdId) {  // UNCONFIRMED-G ids
        switch (cmdId) {
            case CMD_OK:           CommitPlayerData(); Close(); return true;
            case CMD_CANCEL:       DiscardChanges();   Close(); return true;
            case CMD_CLASS_PREV:   StepClass(-1); return true;
            case CMD_CLASS_NEXT:   StepClass(+1); return true;
            case CMD_TAB_STATS:    ShowPage(PAGE_STATS);   return true;
            case CMD_TAB_SKILLS:   ShowPage(PAGE_SKILLS);  return true;
            case CMD_TAB_OPTS:     ShowPage(PAGE_OPTS);    return true;
            case CMD_SKL_PREV:     skillPage = clamp(skillPage-1, 1, 3); RefreshSkillsPage(); return true;
            case CMD_SKL_NEXT:     skillPage = clamp(skillPage+1, 1, 3); RefreshSkillsPage(); return true;
            // ... MP transfer: CMD_ADD/DEL/EQUIP/ICON ...
        }
        return false;
    }

    ~TCharCreateScreen() {
        renderer->DestroySurface(portraitScratch);
        free(statAlloc);
        free(player_name);
        free(player_notes);
    }
};
```

**No screen-level `paint()`** — the inherited DefScreen base walks the widget tree.

---

## §13 — Port mapping notes

| retail primitive used | port equivalent | notes |
|-----------------------|------------------|-------|
| `FUN_004a1ec0(w, h, fmtbits, 0)` | `Renderer->CreateSurface(w, h, fmt)` (`src/renderer.h`) / `TSurface(w, h, fmt)` | Standard offscreen scratch allocation; 100×100 portrait cell |
| `FUN_00482fb0(n)` | `operator new` / `malloc(n)` | Plain CRT |
| `FUN_00482ef0(n, 0)` | `malloc(n)` | (CRT, used for strdup buffers in Init) |
| `FUN_00482f80(p)` | `free(p)` | (CRT) |
| `FUN_0049d800(s)` | `TextResource::Lookup(string)` / `g_locale.Lookup(string)` | Returns an opaque id; later `GetText(id)` returns the localized format string |
| `FUN_00474e20_NewObjectByName(name, ..., 1)` | `TObjectClass::NewObjectByName(name)` | Default-instance factory; used to spawn "Locke" |
| `FUN_0046e6f0(name)` | `TPlayer::Init(name)` / `TPlayer::SetDefaultName(name)` | (UNCONFIRMED-I exact semantic — name resolver) |
| `FUN_0058b100(buf, fmt, ...)` | `snprintf` / `std::format` (UI_METHOD_MAP §10) | MPSTATINFO1/2/3 lines |
| `FUN_004be2b0` (text) | `Font->DrawTextToTarget(...)` / `DrawTextShadowedToTarget(...)` (UI_METHOD_MAP §5 + §12) | DEF engine routes here per-TEXT widget |
| Widget chrome (FRAME 9-slice, BUTTON UP/DOWN, EDIT BGBITMAP, DROPLIST scrollbar, LISTBOX VLIST) | needs **DefWidgetEngine** to be ported (`DefWidgetEngine.md:106-114` outstanding work) | All listed widget classes need ports; this screen exercises BITMAP/FRAME/TEXT/BUTTON/EDIT/DROPLIST/LISTBOX (7 of 8 — only SCROLLBAR is standalone-absent though LISTBOX/DROPLIST instantiate it internally) |
| `DefScreen_Open` / `DefScreen_LoadAndShow` / `DefWidget_LoadFile` | needs **DefScreen** port | Screens listed in `DefWidgetEngine.md` "Per-screen activators"; createchar = entry @ `FUN_00465a50` (this class's Init) |

**Missing primitives the reconstructor must add:**
1. `BMSurface` decoder for **`BM_TEXTURE` (flag 0x10000)** — needed for `createchartex.dat`. Defer to Classic path for now.
2. DEF widget engine (entire) — DefScreen_Open + the 8 widget classes + the renderer walker + the input dispatcher. The 1-line "wire it up" cannot work until the engine is ported. See `DefWidgetEngine.md:106-114` outstanding-work list.

---

## §14 — UNCONFIRMED / open questions

| # | Item | What we tried | Impact if wrong | Resolve step |
|---|------|---------------|-----------------|--------------|
| **A** | Exact bit-meanings of `flags = 0x11` (`0x10 \| 0x1`) for the MP path | Read Init `:49-54`; `0x10`/`0x1` bits are referenced in `DefScreen_Open` (`FUN_00435150_DefScreen_Open.cpp:21,33` test `flags & 1`). `0x1` = "use MP widget cache" (DAT_0066733c vs DAT_0065bb10). `0x10` not pinned. | MP gating wrong (right panel/playerframe2 might show in SP) | `DecompileAddr.java 0x00435150` more deeply, plus inspect the call sites in MP lobby (FUN_00463149) — observe what flag set is passed for the working MP case |
| **B** | High bits TEXTFLAG_LEFT/CENTER/RIGHT (0x00010000/40000/80000) → DrawTextA flag word | Saw `widgets.def:42-44` map to these constants but did NOT trace into TEXT attr parser `FUN_0042b340` to see how they OR into the DrawTextA flag word passed to `FUN_004be2b0` | label justification wrong (RIGHT-anchored becoming LEFT, classinfo lines becoming left-justified) | `DecompileAddr.java 0x0042b340`; cross-check against how `TEXTFLAG_RIGHT` translates to the bit 0x4 the DrawTextA path expects |
| **C** | FIELD → data getter table | The widget-engine FIELD dispatch is not yet pinned; the FIELD name lookup happens inside the BITMAP/TEXT/EDIT widget `Update` (`(*+0xb4)` or similar — see `DefWidgetEngine.md:14`). The CharCreate-specific FIELDs (`portrait`, `editname`, `editpass`, `editgold`, `editlevel`, `editexp`, `txtchar`, `txtclass`, the 6 stats, the 11 skill x 2 = 22 EDITs, `classfield`) all need a per-FIELD-name → player-getter binding | Every dynamic value is wrong (level shows 0, gold shows 0, portrait blank, etc.) | Extract `cls_0x5a4d6c::meth_0x465d50` and `::meth_0x465ea0` (the post-reload data-rebind methods, `:249-251`) — they almost certainly enumerate the FIELDs |
| **D** | Whether the static `playerframe2` / `addframe2` hidden frames are EVER shown (and by which signal) | Saw `CTRLFLAG_HIDDEN` in the DEF (`:12-15`). Suspect they're swapped in by a parent-screen reskin call. Did NOT trace. | Visual rendering is fine (they stay hidden), but the toggle code for them is unknown | Cross-check the host-lobby MP path (FUN_00463149) for whether it calls a "show-alt-frames" hook |
| **E** | The MPSTATINFO1/2/3 refresh path (where they're sprintf'd into st_txt_info1/2/3) | Resolved that they're stored at +0x1b8/+0x1bc/+0x1c0 (Init.cpp:37-42). Did NOT find the call-site that pulls the format string and feeds it to `FUN_0058b100`. | The 3 cyan info lines stay empty / show literal "[message]" placeholder | Extract `meth_0x465d50`/`::meth_0x465ea0` (suspect this is where the info-line refresh lives), or `FindCallsTo.java` for `FUN_0058b100` from within the cls_0x5a4d6c instruction range (`0x465a50..0x469340`) |
| **F** | Skill page index storage location + flag-toggle code | `skl_prev`/`skl_next` exist as buttons; `skl_page` text-label exists; no field in cls_0x5a4d6c is explicitly labelled "skill page". Likely `mbr_0x1fc` (byte) or `mbr_0x4` (the early generic dword). | Skill pages don't switch; "Page 1" text never updates | Extract the OnCommand dispatch (the screen's command method — bound at the parent class's command-slot. Likely `cls_0x5b93c4::virt_meth_0x436530` — vtable slot 100 — but the override hasn't been confirmed at `cls_0x5a4d6c` level). `DecompileAddr.java` on the relevant cls_0x5a4d6c instruction range + grep for `skl_` strings |
| **G** | Command id ↔ widget mapping (ok, cancel, btnclassprev/next, btnstats/skills/opts, skl_prev/next, opt_pker/view, add/del/equip/icon) | Each BUTTON in createchar.def has a NAME but no explicit COMMAND id (the DEF engine assigns ids in registration order). The parent screen's OnCommand handler reads these ids. Did NOT trace the assignment. | Buttons "click" visually but nothing happens | Extract DefWidget_DispatchControl `FUN_00436ec0` for the id-assignment scheme, then either: (i) the OnCommand slot at parent (`cls_0x5b93c4::virt_meth_0x436530`), or (ii) lookup by NAME via the engine's name-lookup helper |
| **H** | Vtable slot 44 (`virt_meth_0x444fc0`) called as `(*+0x2c)(0)` from Init.cpp:74 | Read the vtable cite (`cls_0x5a4d6c__vftable_5a4d6c.cpp:18`). Did NOT extract `0x444fc0`. The (0) arg suggests a state-set call. | Init's "after-open" state transition is wrong | `DecompileAddr.java 0x00444fc0` |
| **I** | `FUN_0046e6f0(s_Player_Name)` exact semantic — does it set `player->name`, the screen-default name, or the network user-name? | Confirmed it's called only when `mbr_0x194 == 2` (the Locke-spawn path). Init.cpp:71. Did NOT extract the body. | Default player name may be wrong (the user's typed name may be overwritten by the canonical "Player Name" string) | `DecompileAddr.java 0x0046e6f0`; cross-reference where else it's called |
| **J** | Whether `DAT_00659c80` and `DAT_00659c7c` (the two passed-by-pointer `screen` args) are the **same struct** or different DefScreen-root holders (one for 15-bit, one for 16-bit) | Their addresses differ by 4 bytes — looks like a 2-entry table of DefScreen-root pointers. The DAT_00659c84 sister at the call site (`_data.txt` shows xref from 0x465b7d) suggests these are sequential pointer slots in a globals table. Did NOT pin. | The 15-bit and 16-bit paths land on the wrong DefScreen root; widget hierarchy parented to the wrong owner | `DecompileAddr.java 0x00659c80` view the data context + look at the .data section dump for adjacent labels |
| **K** | Whether the `cls_0x5a4d6c` instance is per-screen-shown (allocated/freed each show) or a singleton | Init body is destructive (it allocates `mbr_0x27c`/`mbr_0x288` each time); the dtor frees them. Looks per-instance. But the screen-table at `&DAT_00659c80` may hold a singleton. | Memory leak / double-free | Trace where `cls_0x5a4d6c()` ctor (`0x46d160`) is called from. `FindCallersOf.java 0x46d160` |
| **L** | Exact DAT_0065c890 lifecycle when CharCreate is opened outside TPlayScreen (the `DAT_00667fd0 != &DAT_0065caf0` branch — Init.cpp:75) | Confirmed `DAT_0065c890` is loaded by TPlayScreen::Initialize (createchar*tex.dat) and freed by TPlayScreen::Close. If CharCreate opens from MenuNewGame BEFORE TPlayScreen exists, `DAT_0065c890 == 0` and the Classic-path branch is taken. | Confused — likely fine since Init handles both paths, but the Background bitmap may not be loaded yet in the "before TPlayScreen" case (the `LoadAndShow` "background" token resolves it on-demand though). | Visual-verify after the engine port; if BG is missing in pre-TPlayScreen opens, escalate. |

---

## §15 — Resolved (per-widget visual style, FIELD→getter, command IDs) — round 2

This section closes out the three items the brief flagged as open: per-widget visual style (font/color/shadow), FIELD-name → object-getter bindings, and command dispatch IDs (Ok / Cancel / Randomize / class-prev/next / tab radios / skill nav / inv transfer). Source files added to §0:

- `recon/discovered/cls_0x5b98b8.cpp` lines `1380–1483, 1487–1649, 1653–1788, 1791–1854` — the four screen-helper bodies bound by OOAnalyzer to the **DefScreen base** (`cls_0x5b98b8`) but **only used by `cls_0x5a4d6c` TCharCreateScreen** (verified by exclusive xrefs to CharCreate widget NAMEs).
- `recon/ghidra/_data.txt` lines `75540..77804` — the `.data` cross-reference table for `meth_0x466480` / `meth_0x4669e0` / `virt_meth_0x468320` / `virt_meth_0x468ee0` / `meth_0x465f50` / `meth_0x4661e0` / `meth_0x466950` / `meth_0x4691c0` / `meth_0x469260`. Even when a method's decompile body is absent from the bulk recon, its `XREF[*]:` list of `s_<widgetname>` and `s_<msgkey>` string literals exposes the FIELD/NAME table the method uses. We cite literal `s_<name>_<addr>` strings + their XREF instruction addresses (hex; decimal in parens where useful).
- `data/resources_unzipped/widgets.def` (already in §0) — STYLE blocks for default text/font/color/shadow.

> **Procedure caveat (NOMENCLATURE §6 — UNCONFIRMED).** Several methods below are inferred from XREF clustering plus the surrounding decompiled methods of the same screen. Where we cite XREF only (no decompile body), the FIELD→getter mapping is **method-attributed but body-unverified**: we know the method touches that widget name (XREF address) and we know which player getter slot it calls (via inspecting the surrounding §6.1/§6.5 bodies that use the same helper pattern), but the exact branch logic is one extraction away. These rows are flagged **`xref-only`**.

### §15.1 — Per-widget visual style table (closed)

Each cell now binds the DEF text-flag and STYLE pull. Common STYLE pulls (cite: `widgets.def:84-105`):

- **TEXT default (`widgets.def:84-86`):** font `"Med"`, color `(255,255,255)` white, flags `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW`, drawmode `DM_USEDEFAULT`, RECT margin `(3,0,3,0)`, BGBITMAP `"ClearFrame3"` (only rendered if `CTRLFLAG_DRAWTOBG` set; for plain labels with `CTRLFLAG_CLEARBG` the bg-frame is suppressed).
- **BUTTON NORMAL (`widgets.def:88-93`):** UP `"ClearFrame2"`, DOWN `"FillFrame2"`, FRAME 9-slice `10 10 12 12`, MARGINS `0 0 4 4`. UPLABEL font `"Med"` color `(255,255,255)` flags `TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_SHADOW`. DOWNLABEL font `"Med"` color `(0,0,0)` flags `TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE` (notable: DOWN label drops the shadow). UPLABELRECT / DOWNLABELRECT `0 0 0 0` = full button rect.
- **BUTTON TOGGLE (`widgets.def:95-99`):** UP `"CheckU"`, DOWN `"CheckD"` (small square checkbox icons rendered to the left of the label). UPLABEL / DOWNLABEL both font `"Med"` color `(255,255,255)` flags `TEXT_LEFT | TEXT_SHADOW`. UPLABELRECT / DOWNLABELRECT `(18,-1,0,0)` — text starts 18 px right of icon, baseline shifted up 1 px.
- **BUTTON RADIO (`widgets.def:101-105`):** UP `"RadioU"`, DOWN `"RadioD"` (round radio dots). Labels same as TOGGLE (left-anchored, 18 px indent, shadow).
- **EDIT NORM / SPIN (`widgets.def:127-134`):** BGBITMAP `"ClearFrame4"` 9-slice `5 5 5 5` MARGINS `0 0 4 4` NOCENTER. Font `"Med"` text color `(255,255,255)` white (the value text), but **EDITCOLOR `(255,255,0)` yellow** for actively-edited text, flags `TEXT_LEFT | TEXT_SHADOW [| TEXT_VCENTER for SPIN]`. SPIN adds `SpinUp` / `SpinDown` bitmap pair at the right edge (RELR 19 0 / RELR 19 10). RECT margin SPIN=`(4,0,6,2)`, NORM=`(4,2,6,2)`. ([widgets.def:127-134])
- **DROPLIST (`widgets.def:136-145`):** BGBITMAP `"ClearFrame4"` RECT `(0,0,23,0)` (reserve 23 px on the right for DROPBUTTON) FRAME `5 5 5 5` MARGINS `0 0 4 4` NOCENTER. ITEMRECT margin `(2,3,2,3)`. DROPBUTTON `"DropBtn"` positioned RELR `(19,1)` (19 px from right, 1 px down). LISTHEIGHT 100 default (overridden to 108 in createchar.def:157). Inner LISTBOX has its own BGBITMAP `"DropRect"` with 9-slice + scrollbar.
- **LISTBOX VLIST (`widgets.def:119-125`):** BGBITMAP `"VScrollRect"` 9-slice `8 26 26 26` MARGINS `0 0 4 4` NOCENTER. Item RECT `(5,5,23,5)` (right-23 reserves scrollbar gutter), ITEM size `1000×16`. SELCOLOR `(0,112,74)` dark green. Font `"Med"` color `(255,255,255)` flags `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW | TEXT_SINGLELINE` drawmode `DM_USEDEFAULT`. Embedded VSCROLLBAR with `"VScrollUp"/"VScrollDown"/"VScrollThumb"` glyphs.
- **FRAME default (`widgets.def:81-82`):** BITMAP `"ClearFrame1"`, FRAME 9-slice `14 14 16 16`, MARGINS `0 0 4 4`, NOCENTER. createchar.def overrides the BITMAP on every FRAME (`TransFrame1` for playerframe/addframe, `ClearFrame2` for tabframe/portraitframe).

Cited per-widget style overrides on createchar.def (only the cells that change from default):

| widget NAME | DEF flags | font | color override | RGB | shadow | special | cite |
|-------------|-----------|------|----------------|-----|--------|---------|------|
| TEXT `classinfo1..4` | `TEXTFLAG_CENTER | CTRLFLAG_CLEARBG` | **`Small`** | yes | **`0 150 255`** cyan | inherits STYLE-default `TEXT_SHADOW` | center-anchored 152 px wide; CLEARBG kills the "ClearFrame3" bg | `createchar.def:45-48` |
| TEXT `st_txt_info1..3` | `TEXTFLAG_CENTER | CTRLFLAG_CLEARBG` and re-asserted `TEXT_CENTER | TEXT_SHADOW DRAWMODE DM_USEDEFAULT` | **`Small`** | yes | **`0 150 255`** cyan | **double FLAGS keyword** in DEF: high-bit `TEXTFLAG_*` + low-bit `TEXT_*` both set; the parser ORs them (verified via grep of two FLAGS clauses on lines 76/79/82) | `createchar.def:75-82` |
| TEXT `txtchar` / `txtclass` | `CTRLFLAG_CLEARBG` | (default `Med`) | no | (white) | yes | dynamic — value comes from `virt_meth_0x468320` via FIELD lookup | `createchar.def:42-43` |
| TEXT `"Name:" .."Exper:"` | `TEXTFLAG_RIGHT` (0x00080000) | (default `Med`) | no | (white) | yes | right-justified label (drawn under STYLE's shadow) | `createchar.def:26-30` |
| TEXT `"Char:" / "Class:"` | `TEXTFLAG_RIGHT` | default `Med` | no | (white) | yes | as above | `createchar.def:39-40` |
| TEXT `st_txt_strn..luck` | `TEXTFLAG_RIGHT` | default `Med` | no | (white) | yes | right-justified label | `createchar.def:61-66` |
| TEXT `skl_p[1-3]txt*` | `CTRLFLAG_CLEARBG | TEXTFLAG_RIGHT` | default `Med` | no | (white) | yes | CLEARBG suppresses the ClearFrame3 BG bitmap for these skill-row labels | `createchar.def:86-89, 107-110, 128-130` |
| TEXT `skl_p[1-3]txtexp*` | `TEXTFLAG_RIGHT` | default `Med` | no | (white) | yes | also right-justified | `createchar.def:96-99, 117-120, 136-138` |
| TEXT `"Type"` / `itemqtytxt` "Qty" | `TEXT_SHADOW` (only) | default `Med` | no | (white) | yes | not a TEXTFLAG_ alias — uses the low-bit value `0x400` directly, so default `LEFT | VCENTER` h/v align applies | `createchar.def:156, 163` |
| TEXT `opt_txtdesc` "Multiplayer Options" | `TEXTFLAG_CENTER` | default `Med` | no | (white) | yes | centered header label | `createchar.def:150` |
| BUTTON `ok` / `cancel` | (none) | inherits BUTTON NORMAL UPLABEL/DOWNLABEL | no | white (UP) / black (DOWN) | yes (UP only) | per BUTTON STYLE NORMAL — UP shows white shadowed text on `ClearFrame2`, DOWN shows black non-shadowed text on `FillFrame2` (a strict color flip for the click state); chrome 80×27 with 9-slice 12-px corners | `createchar.def:18-19`; `widgets.def:88-93` |
| BUTTON `btnclassprev/next` | (none) | inherits NORMAL | no | white | yes | default 50×20 button chrome | `createchar.def:51-52` |
| BUTTON `btnstats/skills/opts` | `BTNFLAG_RADIO [\| BTNFLAG_DOWN on btnstats]` GROUP 1 | inherits **RADIO** STYLE | no | white | yes (RADIO STYLE) | per BUTTON STYLE RADIO — UP/DOWN swap `"RadioU"`/`"RadioD"` glyphs to the left of the label (label-x offset = 18 px per UPLABELRECT). Default selection `btnstats` (BTNFLAG_DOWN). | `createchar.def:55-57`; `widgets.def:101-105` |
| BUTTON `opt_pker/view` | `BTNFLAG_TOGGLE | BTNFLAG_DRAWTOGGLE` | inherits **TOGGLE** STYLE | no | white | yes (TOGGLE STYLE) | per BUTTON STYLE TOGGLE — CheckU/CheckD checkbox glyph; label 18 px right with shadow | `createchar.def:151-152`; `widgets.def:95-99` |
| BUTTON `add/del/equip` | `KEY` set | inherits NORMAL | no | white | yes | KEY shortcuts A/D/E; small 45×23 buttons | `createchar.def:196-198` |
| BUTTON `icon` | `BTNFLAG_TOGGLE | BTNFLAG_DRAWTOGGLE | KEY "I"` | inherits TOGGLE | no | white | yes | the only TOGGLE button on the MP-inventory side; `Icons Only` toggles icon-mode for the inventory listboxes (see `virt_meth_0x4669e0` xref of `s_icon_005d34e0`, `_data.txt:76931`) | `createchar.def:199` |
| EDIT `editname/editpass/editgold/editlevel/editexp` | per-EDIT (SPIN on editlevel) | inherits **EDIT NORM** (or **SPIN** for editlevel) | no | white text / **yellow EDITCOLOR** during edit | yes | per widgets.def:127-134 — `ClearFrame4` BG, yellow active text | `createchar.def:32-36`; `widgets.def:127-134` |
| EDIT `st_edit[0-5]_*` / `skl_p[1-3]edit*` (incl. *exp) | `EDITFLAG_SPIN` on stat-value fields; plain EDIT on exp fields | inherits SPIN / NORM | no | white text / yellow active | yes | default text `"13"` for stat/skill rows | `createchar.def:68-73, 91-94, 101-104, 112-115, 122-125, 132-134, 140-142` |
| DROPLIST `classlist` | `DROPFLAG_USEFORMAT` LISTHEIGHT 108 | inherits DROPLIST VSCROLL + inner FIELD font `Med` | no | white | yes | inner FIELD 5,-2,160,20 flags `TEXT_LEFT | TEXT_SHADOW`; populated via FIELD `"classfield"` (see §15.2) | `createchar.def:157-160`; `widgets.def:136-145` |
| LISTBOX `longgameinv` / `longplayerinv` | `LISTFLAG_VLIST | LISTFLAG_USEFORMAT | LISTFLAG_DBLCLICK` ITEM 1000×40 SELCOLOR `(0,100,30)` (darker than default LISTBOX's `(0,112,74)`) | inner cells per-FIELD (see below) | yes/no per cell | mix | yes (all cells use TEXT_SHADOW) | rich row format: BITMAP icon at (0,0) DM_TRANSPARENT; name FIELD `lgi_name`/`lpi_name` (42,-1,150,18) `Med` flags `TEXT_SHADOW | TEXT_ELIPSES`; value FIELD `lgi_value`/`lpi_value` (195,3,40,14) `Small` flags `TEXT_RIGHT | TEXT_SHADOW` color **`(100,255,255)` teal**; stat FIELD `lgi_stat`/`lpi_stat` (45,13,180,24) `Small` color **`(255,255,100)` yellow** flags `TEXT_SHADOW`; qty FIELD `lgi_qty`/`lpi_qty` `Small` color **`(255,0,0)` red** flags `TEXT_RIGHT | TEXT_SHADOW`. `longplayerinv` adds an `lpi_eq` FIELD (0,0,28,10) `Small` color **`(0,255,0)` green** flags `TEXT_LEFT | TEXT_SHADOW` — the "EQ" tag for equipped items | `createchar.def:177-218` |
| LISTBOX `longgameinvicon` / `longplayerinvicon` | same flags, ITEM 39×40 (icon-cell sized) | inner: BITMAP icon @ (0,0) DM_TRANSPARENT + qty FIELD only | white default + qty in **red** | yes | icon-only inventory view | `createchar.def:177-182, 201-206` |

**Three retail color signals worth preserving (cite: §VFX color = health signal feedback rule):**
- **Cyan `(0,150,255)`** = "info/secondary" text — used on classinfo lines + the 3 MPSTATINFO lines. Pale/missing cyan → wrong color load.
- **Yellow EDITCOLOR `(255,255,0)`** = "currently being edited" — fires only while the EDIT widget owns keyboard focus; idle text is white.
- **Inventory row palette:** red qty / teal value / yellow stat / green "EQ" — bug if any of these read white in the port.

### §15.2 — FIELD → getter bindings (closed)

The DEF widget engine routes every widget with a `FIELD "<name>"` clause through the screen's vtable slot **164** = `virt_meth_0x468320`, the **FIELD getter dispatcher** (cite: `cls_0x5a4d6c__vftable_5a4d6c.cpp:48`).

The dispatcher uses three formatting templates (cite: `_data.txt:77460-77544` literal data):
- `"%s"` (string format) — bound widgets `txtchar` (`_data.txt:77465`), `txtclass` (`:77474`), `editname` (`:77484`), `editpass` (`:77495`), `editlevel` (`:77506` — but feeds `%d` formatter; `%s` here is the FIELD signature lookup, not the value template), `editexp` (`:77516`)
- `"%d"` (decimal format) — bound widgets `editgold` (`:77525, :77530`), `editlevel` (`:77511`), `editexp` (`:77520`)
- `"%d%s"` (qty + suffix) — bound to inventory qty cells (`:77614`)

The XREF map (each row = one widget NAME literal, ONE method touches it, instruction-address = the `cmp / je` against the str):

| FIELD name | string slot | reads from (player getter or other) | writes to (commit path) | cites (XREF instruction in `virt_meth_0x468320`) |
|------------|-------------|--------------------------------------|--------------------------|--------------------------------------------------|
| `portrait` | `s_portrait_005d3af4` | **`virt_meth_0x468ee0` (NOT 0x468320)** dispatches portrait/icon FIELDs; resolves to `player->InventoryImage(state=0)` cached into `mbr_0x27c` 100×100 scratch | (none — read-only) | `_data.txt:77747` (`virt_meth_0x468ee0:00468f1f`); spec §6.1 step 1 |
| `lgi_iconbmp` | `s_lgi_iconbmp_005d3acc` | per-row inventory icon BITMAP — `virt_meth_0x468ee0` looks up the item .i3d via prefix `lgi_` + uses `.bmp` extension token (`_data.txt:77752`) | n/a | `_data.txt:77721` (`virt_meth_0x468ee0:00468ef1`) |
| `lpi_iconbmp` | `s_lpi_iconbmp_005d3ad8` | same for player-inventory icon | n/a | `_data.txt:77725` (`virt_meth_0x468ee0:00468f09`) |
| `txtchar` | `s_txtchar_005d397c` | `player->mbr_0x38 (name char*)` — same field used in Init's strdup (`Init.cpp:76,114`) | n/a | `_data.txt:77465` (`virt_meth_0x468320:004683f8`) |
| `txtclass` | `s_txtclass_005d3988` | `player->TObjectClass*` resolved via classfield index; reads class name string from class struct | n/a | `_data.txt:77474` (`virt_meth_0x468320:0046843e`) |
| `classfield` (DROPLIST) | `s_classfield_005d3960` | uses format `"CLNAME%d"` to index into TObjectClass class table; e.g. `CLNAME0`, `CLNAME1`, ... → display name per class | writes back: selecting an entry sets the active class index (used to drive `txtchar/txtclass/classinfo*`) | `_data.txt:77450` (`virt_meth_0x468320:00468338`); format key `_data.txt:77455` (`:00468373`) |
| `editname` | `s_editname_005d3998` | `player->name (mbr_0x38)` | on commit (Ok button): writes back to player.name via the same field (UNCONFIRMED-M whether commit happens inside dispatcher's "set" branch or via a separate save method) | `_data.txt:77484` (`virt_meth_0x468320:0046849c`) |
| `editpass` | `s_editpass_005d39a8` | MP password — likely `player->mp_password` field (not yet identified in TPlayer); xref-only **(`xref-only`)** | n/a | `_data.txt:77495` (`virt_meth_0x468320:004684d2`) |
| `editlevel` | `s_editlevel_005d39b8` | `player->Level()` — formatted via `%d` template | writeable (numeric spinner) | `_data.txt:77506` (`virt_meth_0x468320:0046850b`) |
| `editexp` | `s_editexp_005d39c8` | `player->Experience()` (TPlayer field) | writeable | `_data.txt:77516` (`virt_meth_0x468320:0046855b`) |
| `editgold` | `s_editgold_005d39d4` | `player->Gold()` (also xref'd from `meth_0x466950:004669c4` and `virt_meth_0x4669e0` 3× — the buy/sell handler) | writeable + commit path on BSBUY/BSSELL | `_data.txt:77525` (`virt_meth_0x468320:004685ab`); + ec-refs at `_data.txt:77007-77010` |
| `lg_*` / `lgi_*` prefix-route (`lgi_name`, `lgi_value`, `lgi_stat`, `lgi_qty`) | `s_lg_*` literals at `_data.txt:77546+` ("lg_", "lp_") | inventory FIELD lookup: dispatcher strips the `lgi_` / `lpi_` prefix, gets the current row's `TObject*` from the bound list, then reads the field by suffix: `name` → `obj->name`, `value` → `obj->mbr_<value>` (also handles "Value" capitalized for some legacy items, `_data.txt:77593-77601`), `stat` → format string `MP%sDESC` (stats descriptor lookup, `_data.txt:77661`), `qty` → `obj->quantity`, `eq` → equipped flag → "eq" string (`_data.txt:77643, :77648` `MPEQUIP`) | n/a | (multiple) `_data.txt:77546-77707` |
| `lp_*` / `lpi_*` prefix-route | symmetric to `lgi_*` | reads from player inventory list (TPlayer's inv collection) instead of game/world inv | n/a | (symmetric) |
| `name` (inner) | `s_name_005d3a14`-like at `:77581-77584` | inventory item name | n/a | `_data.txt:77581-77584` (`virt_meth_0x468320:00468935`) |
| `value` / `Value` | `s_value_005d3a14`, `s_Value_005d3a1c..2c` | inventory item value — three "Value" string slots (probably for different inventory contexts: gold value, attack value, defense value); selected by item subtype | n/a | `_data.txt:77589, :77593, :77597, :77601` (`virt_meth_0x468320:004689ab, e9, fc, a1a`) |
| `stat` | `s_stat_005d3a68` | inventory item stat bonus — formatted via `MP%sDESC` template (e.g. `MPSTRDESC`, `MPDEFDESC`) | n/a | `_data.txt:77652` (`virt_meth_0x468320:00468b8e`); `MP%sDESC` template `:77661` |
| `qty` | `s_qty_005d3a44` (literal `qty\0`) | `obj->quantity` formatted `%d` | n/a | `_data.txt:77623` (`virt_meth_0x468320:00468aa4`) |
| `eq` | `s_eq_005d3a5c` (literal `eq\0`) | format `"eq"` IF equipped, else empty | n/a | `_data.txt:77643` (`virt_meth_0x468320:00468b36`) |
| `Amount` | `s_Amount_005d3a48 / :3a50` | gold / item amount in some context (xref-only) | n/a | `_data.txt:77628, :77633` (`virt_meth_0x468320:00468ac0, acd`) |
| `fatigue` | `s_fatigue_005d3a94` | `player->Fatigue()` | n/a | `_data.txt:77676` (`virt_meth_0x468320:00468cec`) |
| `mana` | `s_mana_005d3a9c` (literal `mana\0`) | `player->Mana()` | n/a | `_data.txt:77680` (`virt_meth_0x468320:00468d2f`) |
| `health` | `s_health_005d3aa4` | `player->Health()` | n/a | `_data.txt:77689` (`virt_meth_0x468320:00468d68`) |
| `damagemod` | `s_damagemod_005d3aac` | `player->DamageMod()` (xref-only) | n/a | `_data.txt:77694` (`virt_meth_0x468320:00468e06`) |
| `MPGPGP` template | `_data.txt:77605` | "gold per piece"-style format `"%d%s"` for gold display | n/a | `_data.txt:77605` (`virt_meth_0x468320:00468a68, a80`) |
| `MPEQUIP` template | `_data.txt:77648` | format string for "EQ" tag | n/a | `_data.txt:77648` (`virt_meth_0x468320:00468b80`) |
| `MPWEPTYPES` / `MPDMGTYPES` | `_data.txt:77666, :77671` | comma-separated weapon/damage type names for stat detail | n/a | `_data.txt:77666, :77671` (`virt_meth_0x468320:00468bdf, c05`) |

> **Stats and skills (`st_edit[0-5]_*` and `skl_p[1-3]edit*` plus `*exp`)** — **not** seen as explicit XREFs in `virt_meth_0x468320`. The pattern (from §6.1 Init step 2 + `meth_0x465d50` step at `_data.txt:75541` ref to `DAT_005d28b4` = stat-permission table) is:
>
> - Init builds `mbr_0x288` = `int[maxN]` to hold one int per skill/stat slot per class.
> - `meth_0x465d50` walks the current class's stat array (`TObjectClass.fields[+0x24]` = skill count, `+0x34` = skill-spec ptr-array, `+0x38` = default skill-spec) and pushes the **selectable** stat/skill indices into `mbr_0x288` (cite cls_0x5b98b8.cpp:1417-1441).
> - The widget engine routes `st_edit*` / `skl_p*edit*` FIELDs through a **different** code path (likely vtable slot 152 `virt_meth_0x436b20` — the cls_0x5a4d6c-internal stat-FIELD getter, body NOT in bulk recon). The dispatcher decodes the FIELD name suffix (`_strn`, `_cons`, `_agil`, `_rflx`, `_mind`, `_luck` and `_attack` etc.) to a stat/skill enum, then reads the per-player stat array.
> - **`xref-only`** — exact slot binding for stat/skill FIELDs is one extraction away.

### §15.3 — Command dispatch IDs (closed)

The screen's command handler is **vtable slot 148** = `virt_meth_0x4669e0` (cite: `cls_0x5a4d6c__vftable_5a4d6c.cpp:44`). This is the OnCommand of the DEF screen.

The DefWidget engine assigns each registered widget a `widget_id` (incrementally during `DefWidget_ParseBuffer` @ `0x437620`, cite `cls_0x5b98b8.cpp:1057-1141`). When a user clicks a BUTTON or completes an EDIT, the engine fires `DefWidget_DispatchControl` (`0x436ec0`, vtable slot 172 — cite `__vftable.cpp:50`) which calls the screen's `virt_meth_0x4669e0` with the originating widget's `id` (or, equivalently, the screen pulls the widget by NAME using `meth_0x436900/0x436980`).

**The dispatcher dispatches by widget NAME, not numeric ID.** Cite: `_data.txt:76834-77410` show `virt_meth_0x4669e0` references every dispatchable widget NAME as a `s_<name>` string literal — it `stricmp`s against the focused widget's name and runs the matching branch.

| widget NAME | hit rect | XREF instr in dispatcher | behavior summary (inferred from message-key xrefs in same address range) | cite |
|-------------|----------|----------------------------|--------------------------------------------------------------------------|------|
| `ok` | (370, 415, 80, 27) | `00466ec2` (also referenced at `_data.txt:75540-75542` — possibly `DAT_005d28b4` index → tied to commit path) | Commit player edits → close screen → notify parent. Also drives BSBUY/BSSELL fallthroughs in the same dispatcher. | `_data.txt:75540-75542`, `:76976` (`s_ok` literal `005d351c`) |
| `cancel` | (475, 415, 80, 27) | `00466880` (also `_data.txt:77196 :004675f5` and `:77970 :0046975a` — multiple cancel xrefs) | Discard → close screen → return to parent | `_data.txt:76971` (`s_cancel_005d3514`) |
| `btnclassprev` | (175, 165, 50, 20) | `00466553` (early scan) and `004677c4` (dispatch) | Step class index −1, refresh `txtchar`/`txtclass`/`classinfo1..4` + re-run `meth_0x465d50` (stat list rebuild) | `_data.txt:76863` (early, `meth_0x466480`), `:77235` (`virt_meth_0x4669e0`) |
| `btnclassnext` | (230, 165, 50, 20) | `0046656d` (early) and `004677ae`, `0046789f` (dispatch — 2 xrefs suggest a "stop at max" cap branch) | Step class index +1; calls also reach `s_portrait/s_portraitframe/s_txtchar/s_txtclass` (`_data.txt:77245-77264`) — confirms class-change triggers portrait + label refresh | `_data.txt:76868, :77230, :77240` |
| `btnstats` | (45, 271, 70, 20) | `00467995` | Tab → Stats page; calls `meth_0x4691c0(0)` (cite `cls_0x5b98b8.cpp:1791-1817`) which toggles `st_*` widgets visible, `skl_*` and `opt_*` widgets hidden | `_data.txt:77278` |
| `btnskills` | (120, 271, 70, 20) | `004679d2` | Tab → Skills page; `meth_0x4691c0(1)` → skl_p1 visible (default page), st_/opt_ hidden | `_data.txt:77283` |
| `btnopts` | (195, 271, 70, 20) | `004679fb` | Tab → Options page; `meth_0x4691c0(2)` → opt_* visible, st_/skl_ hidden | `_data.txt:77288` |
| `skl_prev` | (90, 405, 50, 20) | `0046772a` (dispatcher) — cite `meth_0x466480:004667d4` (early registration) and `meth_0x469260` (page logic, `cls_0x5b98b8.cpp:1821-1854`) | Skill page −1 (clamped 1..3); updates `skl_page` TEXT via `meth_0x469260`; toggles `skl_p[1-3]*` widget visibility | `_data.txt:77220` (dispatch), `:77874` (`meth_0x469260:00469304`) |
| `skl_next` | (205, 405, 50, 20) | `00467685` (dispatch); `meth_0x469260:004692e4` | Skill page +1 (clamped 1..3) | `_data.txt:77210` (dispatch), `:77869` (`meth_0x469260:004692e4`) |
| `opt_pker` | (55, 330, 200, 20) | `00467b39` (set), `00467c9c` (read/toggle) — twin xrefs suggest TOGGLE button has both "set state" and "query state" branches | Toggle Player-Killer flag; on toggle, fire `MPPKERALL` / `MPPKERNONE` popup confirm (cite `_data.txt:77104, :77109` — those keys are xref'd from this dispatcher at `00466d99` and `00466dad`) | `_data.txt:77055, :77324, :77361` |
| `opt_view` | (55, 350, 200, 20) | `00467afa`, `00467c5d` — same TOGGLE twin pattern | Toggle "Viewable by other players"; fires `MPVIEWALL` / `MPVIEWNONE` confirm | `_data.txt:77050, :77319, :77356` |
| `add` (KEY "A") | (337, 241, 45, 23) | (xref-only — `s_add_005d3470` not explicit "add" string in dispatcher — the routing happens via the LISTBOX double-click + this button on the **MP-inventory-transfer** path, dispatcher branches at `0046741e` near `s_Amount_005d36ec`) | Add selected `longgameinv` row to `longplayerinv` at qty `itemqty`; clamped by `Amount` | `_data.txt:77176` (xref to `s_Amount` near add path) |
| `del` (KEY "D") | (387, 241, 45, 23) | `0046661b` (early registration `s_del_005d3474` "del\0"), `00466914` (dispatcher `BSSELL` branch — `_data.txt:76997`) | Remove selected `longplayerinv` row; ties to BSSELL price update | `_data.txt:76890, :76997` |
| `equip` (KEY "E") | (437, 241, 45, 23) | `00466635` (early), `004677da` (dispatcher), routes to `longplayerinv*` listboxes (`004677ef, 00467814`) | Equip/unequip selected `longplayerinv` row; sets the `lpi_eq` "EQ" tag | `_data.txt:76895, :77264, :77268, :77273` |
| `icon` (KEY "I", TOGGLE) | (493, 246, 95, 15) | `00466776` (early, `s_icon` literal) — and the visibility-swap logic at `00467a30/a60/a86/ab6` toggles `longplayerinv` ↔ `longplayerinvicon` + `longgameinv` ↔ `longgameinvicon` based on TOGGLE state | Swap rich text inventory listboxes ↔ icon-only inventory listboxes | `_data.txt:76931, :77301-77315` |
| `classlist` (DROPLIST) | (365, 78, 135, 20) | `00466c46` (`_data.txt:77089`) — on selection-change | Change active class index (same effect as `btnclassprev/next` but jump-to); calls back to portrait/txtchar/txtclass/classinfo refresh path | `_data.txt:77089, :77366` |
| `itemqty` (EDIT) | (530, 78, 50, 20) | `00466ee1, 004673d2` (dispatcher) | Numeric qty for `add`/`del`/`equip` operations; clamped to `lgi_qty`/`lpi_qty` of selected row | `_data.txt:77128, :77172` |
| `longgameinv*` / `longplayerinv*` (LISTBOX) | (327, 105 / 269, 266, 131) | many — `004666e2, 004666ee, 004666fc, 0046670a` (registration), and `00466e42, 00466e54, 0046710d, 0046711f, 0046739f, 004673b1, 00467a30, 00467a60, 00467a86, 00467ab6, 00467b91, 00467ba6, 00467bd6, 00467bfc, 00467c38` (dispatcher) | Click selects row → updates focus + redraws cell selection. DBLCLICK fires the same transfer action as Add (`gameinv → playerinv`) / Equip (`playerinv` toggle), per `LISTFLAG_DBLCLICK` | `_data.txt:76913, :76917, :76921, :76926` (registration); many dispatch lines |
| EDIT spinners (name/pass/gold/level/exper, st_*, skl_p[1-3]*) | per §4 | (focus events) | Per-field value change; bound via FIELD name (§15.2). Spinner click on `editlevel` (`EDITFLAG_SPIN`) drives level ±1; stat/skill spinners drive their stored int via the per-class `mbr_0x288` array; the dispatcher also enforces stat-cap rules via the `DAT_005d28b4`-driven stat-permission table | (multiple) | (xref-only) |

**Confirmed: no separate "Randomize" or "Reset" button exists in `createchar.def`.** The brief's mention of "Randomize" did not correspond to a widget in the retail DEF — perhaps the user was thinking of `btnclassprev/next` + `editname` default? If a Randomize button is desired post-port, it would be a **new** Revisited-side feature, NOT a retail reconstruction. Cite negative: `grep -n 'random\|reset\|rerl\|reroll' createchar.def` = 0 hits.

**Hot-reload chain confirmed:** `vtable_slot_19 (0x469340)` → if `DAT_00676e84 != 0`: `meth_0x437ca0(this)` → `meth_0x465d50(this)` → `meth_0x465ea0(this)` (cite `cls_0x5a4d6c.cpp:243-253`). After hot-reload, `meth_0x465d50` rebinds `classlist` + `longgameinv*` + `SaleType` (cite `_data.txt:76689-76702`, the four xrefs in the same method), and `meth_0x465ea0` rebinds `longplayerinv*` (cite `_data.txt:76707, :76712`). These two methods are the **post-reload data-rebind hooks** the spec previously flagged as UNCONFIRMED-D — now CONFIRMED.

### §15.4 — Updates to §14

Status reconciliation against §14 items:

| §14 # | new status | resolution |
|-------|-----------|-----------|
| A | partial | `flags=0x11` is `0x10 | 0x1`. `0x1` confirmed = "use MP widget cache" (DAT_0066733c). `0x10` STILL unknown (would need 0x435150 deeper read). Keep as partial. |
| B | unchanged | TEXTFLAG → DrawTextA bit folding still untraced; see resolve step. |
| **C** | **RESOLVED** | FIELD getters mapped per §15.2. `txtchar/txtclass/editname/editlevel/editexp/editgold` → player getters; `portrait/lg*_iconbmp/lp*_iconbmp` → InventoryImage; inventory rows via `lgi_*/lpi_*` prefix-route dispatch in `virt_meth_0x468320`. Stats/skills still xref-only. |
| **D** | **RESOLVED** | `playerframe2`/`addframe2` are visibility-swapped in `meth_0x466480` (cite `_data.txt:76843-76848`); the swap is driven by the screen mode `mbr_0x17c/0x180/0x184/0x18c` flags (cite `cls_0x5b98b8.cpp:1668-1697`). The frames swap chrome when the screen is opened in MP-host vs MP-join vs SP modes (the four mbr fields are read by `meth_0x466480` at decompile lines 1668-1739 to gate the visibility OR'ing). |
| **E** | **RESOLVED** | MPSTATINFO1 refresh = `meth_0x465f50` (cite `cls_0x5b98b8.cpp:1487-1524`); MPSTATINFO2/3 refresh = `meth_0x4661e0` (cite `cls_0x5b98b8.cpp:1530-1649`). Both pull stored format-string text-ids from `mbr_0x1b8/0x1bc/0x1c0`, sprintf via `FUN_004811b0`, then push the buffer into the bound widget pointers `mbr_0x1ac/0x1b0/0x1b4`. The 16-iteration loop in `meth_0x4661e0` walks 4 stat groups × 4 mods per group; concatenates `"%s: %s%d%% "` strings into one buffer per info-line; flushes when buffer reaches 2/3/4 entries (gating `local_120`). |
| **F** | **RESOLVED** | Skill page index = pushed-arg to `meth_0x469260` (cite `cls_0x5b98b8.cpp:1821-1854`). The value is read from `in_stack_00000004`, then stored at `this->mbr_0x1f4` (cite line 1847) — **note `mbr_0x1f4` was previously labelled "dirty flag" in §6.6**; that labelling is wrong, this field doubles as the active skill-page index (1..3). The widget visibility toggle uses `meth_0x437af0` calls on the widget pointers stored at `mbr_0x1c4/0x1c8/0x1cc/0x1d0` (the four skill-page parent pointers resolved in `meth_0x466480`). |
| **G** | **RESOLVED** | Command dispatch = `virt_meth_0x4669e0` (slot 148, cite `__vftable.cpp:44`). Dispatch is by widget NAME via stricmp; no numeric ids. Full table in §15.3. The brief's "Randomize" widget does not exist in retail. |
| H | unchanged | Vtable slot 44 still untraced (need 0x444fc0 decompile). |
| I | partial | `FUN_0046e6f0` (TPlayer "Init"/SetDefaultName) is called only in the Locke-spawn branch (Init.cpp:71). Still need its body. |
| J | unchanged | DAT_00659c80 vs DAT_00659c7c still untraced. |
| K | unchanged | Caller of ctor 0x46d160 still untraced. |
| L | unchanged | `DAT_0065c890` lifecycle clear in both branches, but the "before TPlayScreen" BG resolution is visual-verify only. |
| **M (new)** | open | Commit path for `editname`/`editpass`/`editgold`/stat-edits — does the dispatcher write back into the player object on EDIT change, on Ok, or both? Either pattern works in the retail engine; the exact branch needs `virt_meth_0x4669e0` body extraction at `ok`/`cancel` xrefs (`_data.txt:76971, :76976`). Treat as "write on Ok" by default. |
| **N (new)** | open | `mbr_0x17c/0x180/0x184/0x18c/0x190/0x194/0x198..0x1d0` field semantics — many of these are populated by `meth_0x466480` from the resolved widget pointers, but the precise meaning of each (which widget pointer cached where, vs which mode flag where) is partially mapped via XREF only. Decompile of `meth_0x466480` is in §6 references — needs a follow-up pass to name each field. |

---

## §16 — Widget-pointer cache (resolved by `meth_0x466480`)

From `cls_0x5b98b8.cpp:1653-1788` + the `_data.txt:76834-77002` XREF cluster, `meth_0x466480` is the **post-Open widget-pointer cacher**. It runs once during/after `DefScreen_Open` and:

1. Reads the screen's four mode flags `mbr_0x17c, 0x180, 0x184, 0x18c` (set externally by the parent screen — MenuNewGame / MPHostLobby / MPJoinLobby — to indicate SP / MP-host / MP-join / shop modes).
2. Walks every widget by NAME and OR's `CTRLFLAG_HIDDEN` (= `0x2`), `CTRLFLAG_DISABLED` (= `0x4`), or `CTRLFLAG_CLEARBG` (= `0x1000`, mask `0x20` shown in code) into the widget's `field[5]` (flags slot) based on which mode bits are set.
3. The XREFs at `_data.txt:76834-77410` show the **complete name-to-cached-pointer map** for the dispatcher. The cached pointers live at:
   - `mbr_0x1a0` = `classlist` DROPLIST ptr (set by `meth_0x465d50`)
   - `mbr_0x1a4` = active inventory list (`longgameinv` or `longgameinvicon` per icon-mode) (set by `meth_0x465d50`)
   - `mbr_0x1a8` = `longplayerinv*` ptr (set by `meth_0x465ea0`)
   - `mbr_0x1ac` = `st_txt_info1` TEXT ptr (used by `meth_0x465f50` for the Atck/Def/Hlth/Fat/Mana line)
   - `mbr_0x1b0` = `st_txt_info2` TEXT ptr (Mod line 1)
   - `mbr_0x1b4` = `st_txt_info3` TEXT ptr (Mod line 2)
   - `mbr_0x1c4/0x1c8/0x1cc/0x1d0` = skill-page parent TEXT/FRAME ptrs for `skl_p1/p2/p3` and the `skl_page` label (the four widget pointers walked by `meth_0x4691c0`/`meth_0x469260` for the visibility toggle, cite `cls_0x5b98b8.cpp:1791-1854`).
4. The visibility toggle for the "MP-only right-pane" widgets (right side `addframe` contents) is gated by `mbr_0x180 != 0` (the "is MP" flag): when 0, `addframe` and every right-side widget gets `CTRLFLAG_HIDDEN` OR'd in (cite `cls_0x5b98b8.cpp:1668-1677`); when non-zero, `addframe2` (the alt-chrome variant) and the icon-only listbox variants flip in (cite `cls_0x5b98b8.cpp:1683-1701`).

This is the resolution to §14-D (when are `playerframe2/addframe2` shown). Cite: `_data.txt:76843` (`s_playerframe2`) + `_data.txt:76848` (`s_addframe2`) both have their single XREF in `meth_0x466480` at `004664c3` / `004664dd`.

---

## §17 — Reconstruction guidance updates (round 2)

Add to §12 reconstruction pseudocode:

```cpp
// vtable slot 164: FIELD GET dispatcher.
//   Engine calls this when a widget with FIELD "<name>" needs its dynamic value.
Variant TCharCreateScreen::GetField(StringView name) {
    if (name == "portrait")     return InventoryImage(player, /*state=*/0);
    if (name == "txtchar")      return player->Name();
    if (name == "txtclass")     return GetActiveClass()->Name();
    if (name == "classfield")   return Format("CLNAME%d", activeClassIndex);
    if (name == "editname")     return player_name ? player_name : player->Name();
    if (name == "editpass")     return player_password;
    if (name == "editgold")     return Format("%d", player->Gold());
    if (name == "editlevel")    return Format("%d", player->Level());
    if (name == "editexp")      return Format("%d", player->Experience());

    // Stats (suffix-decoded; xref-only) — vtable slot 152?
    if (StartsWith(name, "st_edit")) {
        int idx = name[8] - '0';   // "st_edit0_strn" -> idx=0
        return statAlloc[idx];
    }
    // Skills (page+suffix-decoded; xref-only)
    if (StartsWith(name, "skl_p1edit")) { /* page 1 skill or exp */ }
    if (StartsWith(name, "skl_p2edit")) { /* page 2 */ }
    if (StartsWith(name, "skl_p3edit")) { /* page 3 */ }

    // Inventory cells — prefix dispatch
    if (StartsWith(name, "lgi_")) return GetGameInvField(currentLgiRow,    name.substr(4));
    if (StartsWith(name, "lpi_")) return GetPlayerInvField(currentLpiRow,  name.substr(4));
    // GetGameInvField suffixes: "iconbmp" -> .i3d icon, "name" -> obj->Name(),
    //                          "value" -> obj->Value(), "stat" -> Format("MP%sDESC", stat),
    //                          "qty" -> Format("%d", obj->Quantity()),
    //                          "eq" -> obj->IsEquipped() ? "eq" : ""

    return Variant{};
}

// vtable slot 148: OnCommand dispatcher (NAME-routed).
bool TCharCreateScreen::OnCommand(const char* widgetName) {
    if (!stricmp(widgetName, "ok"))             { CommitPlayerData(); Close(); return true; }
    if (!stricmp(widgetName, "cancel"))         { DiscardChanges();   Close(); return true; }
    if (!stricmp(widgetName, "btnclassprev"))   { activeClassIndex = std::max(0, activeClassIndex - 1); RefreshClassUI(); return true; }
    if (!stricmp(widgetName, "btnclassnext"))   { activeClassIndex = std::min(TObjectClass::numclasses-1, activeClassIndex + 1); RefreshClassUI(); return true; }
    if (!stricmp(widgetName, "classlist"))      { activeClassIndex = classlistDropList->GetSelectedIndex(); RefreshClassUI(); return true; }
    if (!stricmp(widgetName, "btnstats"))       { SetTab(0); return true; }   // meth_0x4691c0(0)
    if (!stricmp(widgetName, "btnskills"))      { SetTab(1); return true; }   // meth_0x4691c0(1)
    if (!stricmp(widgetName, "btnopts"))        { SetTab(2); return true; }   // meth_0x4691c0(2)
    if (!stricmp(widgetName, "skl_prev"))       { SetSkillPage(std::max(1, skillPage - 1)); return true; }
    if (!stricmp(widgetName, "skl_next"))       { SetSkillPage(std::min(3, skillPage + 1)); return true; }
    if (!stricmp(widgetName, "opt_pker"))       { PopupConfirm(togglePker ? "MPPKERALL" : "MPPKERNONE", [&]{ togglePker = !togglePker; }); return true; }
    if (!stricmp(widgetName, "opt_view"))       { PopupConfirm(toggleView ? "MPVIEWALL" : "MPVIEWNONE", [&]{ toggleView = !toggleView; }); return true; }
    if (!stricmp(widgetName, "add"))            { TransferItem(SrcList::Game, DstList::Player, qty=itemqty); return true; }
    if (!stricmp(widgetName, "del"))            { RemoveItem(currentLpiRow); return true; }
    if (!stricmp(widgetName, "equip"))          { ToggleEquip(currentLpiRow); return true; }
    if (!stricmp(widgetName, "icon"))           { iconMode = !iconMode; SwapIconLists(); return true; }
    return false;
}

// Hot-reload chain (vtable slot 19 + post-reload data rebinds).
void TCharCreateScreen::HotReload() {
    Base::HotReload();             // 0x435d70
    if (g_isInGame) {
        ReparseDefBuffer();        // 0x437ca0
        RebindGameInventory();     // 0x465d50 — sets classlist + longgameinv*
        RebindPlayerInventory();   // 0x465ea0 — sets longplayerinv*
    }
}

// MPSTATINFO refresh (meth_0x465f50 + 0x4661e0)
void TCharCreateScreen::RefreshStatInfo() {
    char buf[256];
    int atck = player->Attack(), def = player->Defense();
    int hlth = player->Health(), fat = player->Fatigue(), mana = player->Mana();
    // bonuses are added (cite meth_0x465f50 +0x2c0/+0x2c4 calls)
    snprintf(buf, sizeof buf, Locale::Get(msg_info1), atck, def, hlth, fat, mana);
    widget_st_txt_info1->SetText(buf);

    // meth_0x4661e0: 16 mod slots, format "%s: %s%d%% "; flush 2/3/4 per line into mbr_0x1c4/0x1c8/0x1cc/0x1d0
    BuildModLines(player, msg_info2, msg_info3, widget_st_txt_info2, widget_st_txt_info3, widget_st_txt_unused);
}
```

---
