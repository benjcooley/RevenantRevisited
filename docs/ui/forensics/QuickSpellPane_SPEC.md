# TQuickSpellPane — Reconstruction SPEC (Bottom-bar quick-spell ring strip)

> Pixel-faithful reconstruction spec for the 4-slot **quick-spell ring strip**
> on the left end of the bottom utility bar (the row of gold rings under the
> player view that show the four currently-bound quick-cast spells; clicking a
> ring casts the bound spell).
>
> The pane is HOSTED by `TBottomBarPane` (see `BottomBarPane_SPEC.md`; the bar
> draws the chrome plate, this pane stamps the 4 ring buttons on top — see
> `BottomBarPane_SPEC.md §5` and `cls_0x5a5808.cpp:163,187`).
>
> Follow FORENSICS_PROTOCOL §0–§14. Every coordinate names its frame; every
> literal is cited `file:line` with hex(=dec).

---

## §0 — Sources & status

**Class:** `TQuickSpellPane` = `cls_0x5a5a30` (vtable @ `0x5a5a30`); singleton instance @ `DAT_0065c6f8` (size 400 bytes / `0x190`).
**Class identity:** **confirmed 98%** — the leaf-TButtonPane vtable shape (slots 1,2,7,19,22,23,27,28,29 byte-identical to `cls_0x5a45c8 = TButtonPane`) matches the **src forward-decl** `class TQuickSpellPane : public TButtonPane` in `src/spellpane.h` (cf. `recon/discovered/cls_0x5a5a30_TQuickSpellPane_vtable.txt:43`; `renames/agent_ui_wave2_textbar_bottom.txt`). The role match — "the 4-slot spell-ring strip with Ring{U,D,G} sprites" — is independently confirmed in `recon/discovered/port_status/TBottomPane.md`.

**Key method addresses (vtable at `0x5a5a30`, dump in `recon/discovered/cls_0x5a5a30_TQuickSpellPane_vtable.txt`):**

| slot | role | addr | recon |
|---|---|---|---|
| 0  | `Initialize`              | `0x544160` | `recon/discovered/FUN_00544160_TQuickSpellPane_init.cpp` |
| ctor (not in vtable) | ctor | `0x488580` | `recon/discovered/cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp` |
| 19 | `DrawBackground` (base TButtonPane) | `0x435d70` | inherited — iterates children, calls TButton paint |
| **20** | **Animate / per-tick update** (override) | `0x5444a0` | **body NOT extracted** (§14-A); writes `DAT_0065b688 = ?` per disasm metadata (`recon/ghidra/_data.txt:130275` shows `virt_meth_0x5444a0:005444a7 (W)` of `DAT_0065b688`) |
| **21** | **Draw / Overlay** (the requested entry, override) | `0x5444c0` | `recon/ghidra/cls_0x5a5a30.cpp:205-269` |
| 25 | MouseClick / hit-test (override) | `0x544890` | body NOT extracted; reads `DAT_0065d0d0` (the content-presented gate, same one used by SpellbookPane slot 25) |
| 26 | (override) | `0x5449a0` | body NOT extracted |
| 31 | per-class trailing dtor / cleanup | `0x4885e0` | `recon/ghidra/cls_0x5a5a30.cpp:54-69` |

**Recon files read:**
- `recon/ghidra/cls_0x5a5a30.cpp` (full ghidra dump for the class: ctor, Init `0x544160`, Draw `0x5444c0`, dtor)
- `recon/ghidra/cls_0x5a5a30__vftable_5a5a30.cpp` (vtable layout)
- `recon/discovered/cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp`
- `recon/discovered/FUN_00544160_TQuickSpellPane_init.cpp`
- `recon/discovered/cls_0x5a5a30_TQuickSpellPane_vtable.txt`
- `recon/ghidra/cls_0x5a3c68.cpp` (TButton-like widget — paint `meth_0x42cc30:499-664`, ctors `42c2d0/42c500/42c600` — the button class for 2 of the 4 quick-spell slots)
- `recon/ghidra/cls_0x5b9bf0.cpp` (a `cls_0x5a3c68` subclass + 2 extra ints `mbr_0x148/mbr_0x14c` — ctor `00546a70:33-51` — the button class for the first 2 quick-spell slots; forwards to `42c2d0`)
- `recon/ghidra/cls_0x5a3ab8.cpp` (widget base ctor at `0x42a210`: writes `mbr_0x60=x, mbr_0x64=y, mbr_0x68=w, mbr_0x6c=h, mbr_0x5c=text-label, mbr_0x80=callback`)
- `recon/ghidra/cls_0x5a7b98_likely_TPlayScreen.cpp` (**Ghidra mislabel**: actually `TObjectInstance/TCharacter/TPlayer` family per `CharacterPortrait_SPEC.md §0`) — `meth_0x51b560:15019` (get-quick-spell-slot-name, returns `&field_0x2cc + slot*6`), `meth_0x51b580:15034` (set quick-spell, writes `field_0x2cc + slot*6` + sets `UNK_0065c6fc._76_4_ = 1` = TQuickSpellPane dirty), `meth_0x51b5d0:15053` (Cast-from-slot, calls `meth_0x4d5c20_Cast`), `meth_0x51b7c0:15121` (validate spell-name-known-to-player)
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` (TPlayScreen Initialize — `DAT_0065bc3c = SpellIcons.dat:108`, `DAT_0065a570 = BottomBar.dat:129`, `FUN_00544160:186` calls TQuickSpellPane Init)
- `recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp` (Pulse sets `DAT_0065b688 = 1` at `:252` on mode→2 entry — force-redraw of the bottom panes)
- `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:321-326` — **command IDs 0x14..0x17 (20..23) → `FUN_00544630(cmd-0x13)`** (the quick-spell-cast hook with slot index 1..4)
- `recon/classes_original/cls_0x544630.cpp` — `FUN_00544630(slot)` body (`:21-25`): validates `0 < slot < 5`, calls `meth_0x51b5d0(DAT_00667fcc)` (cast spell from slot)
- `recon/classes/_data.txt`: string anchors `s_RingU_005e51f8` (`:116240`), `s_RingD_005e5200` (`:116244`), `s_RingG_005e51f0` (`:116236`) — and the per-button repeats at `5e520c/5214/521c/5228/5230/5238/5244/524c/5254`; button text labels `"1"/"2"/"3"/"4"` at `0x5e5208` (`:116248`) / `0x5e5224` (`:116265`) / `0x5e5240` (`:116282`) / `0x5e525c` (`:116299`); per-slot x coords `0x5e4fe8` (`0x0a=10`, `:115901`), `0x5e4fec` (`0x3c=60`, `:115904`), `0x5e4ff0` (`0x6e=110`, `:115907`), `0x5e4ff4` (`0xa0=160`, `:115910`); per-slot y coords `0x5e4ff8/ffc/5000/5004` all `0x0a=10` (`:115913` and following 3)
- `recon/ghidra/_data.txt:130270-130276` (XREF map for `DAT_0065b688` — proves slots 20/21 read/write it; slot 21 readsteveread at `0x5444d8`)
- `recon/ghidra/_data.txt:131315-131326` (XREF map for `DAT_0065d0d0` — proves slot 25 reads it)
- `docs/ui/forensics/BottomBarPane_SPEC.md` (host pane — confirms `TBottomBarPane::Draw 0x52c800` invokes `TQuickSpellPane::virt_meth_0x5444c0(&DAT_0065c6f8)` at `cls_0x5a5808.cpp:163,187`)
- `docs/ui/forensics/SpellbookPane_SPEC.md` (sibling — `SpellIcons.dat` carries 40×40 spell-name keyed circle icons; talisman glyph table)
- `docs/ui/forensics/CharacterPortrait_SPEC.md §0` (proves `DAT_00667fcc = TPlayer*`; the `cls_0x5a7b98_likely_TPlayScreen.cpp` filename is a Ghidra merge artifact — those methods are on `TCharacter`/`TPlayer`)
- `tools/ui/dump_dat.py` runs against `SpellIcons.dat` (Ring* entries) + `bottombar.dat` — JSONs at `/tmp/dat_dump/SpellIcons.json` + `/tmp/bottombar_dump/bottombar.json`
- `docs/ui/sample_screen_1.jpg` (regression-only reference image; shows 4 rings at bottom-left with spell icons inside + 2 lines of spell-name text below each)

**Prior briefs:** `docs/ui/briefs/B_r3_playscreen_panes.md`; `docs/ui/briefs/B_r5_bottom_panes.md`; `recon/discovered/port_status/TBottomPane.md`; `recon/discovered/port_status/TSideTabsPane.md` (referencing the **sidebar** counterpart `cls_0x5a5978 QuickSpellSidebarPane` — a DIFFERENT class, do not confuse).

**Method-map references:** UI_METHOD_MAP §4 (`FUN_004bd680` sprite stamp = the Ring blit), §5 (`FUN_004be2b0` text — for the spell-name 2-line label, IF that text comes from this path — see §14-D), §15a (surface vtable), §16 (chroma key — Ring* are alpha-flagged `0x104`, not magenta, so do NOT apply the global key on them).

**Status:** `forensics-partial`. Frame, asset roster, button construction, base-draw selector logic, command-dispatch path, instance/host wiring are all firmly pinned from extracted recon. The **per-tick relayout (slot 20 `0x5444a0`)**, the **MouseClick hit-test (slot 25 `0x544890`)** body, and the **mechanism that paints the spell icon + spell-name text inside/under each ring** (see §14-A/D) are NOT extracted in this snapshot of recon. The 4 UNCONFIRMED items each carry a precise resolve step (§14).

**Errors to close (no existing port):** none — NEW retail class with no `src/` shell. `grep src/ -iE 'TQuickSpellPane|quick spell|QuickSpellPane'` returns only forward-decls referenced by other panes.

**Snapshot note (supplementary — superseded by retail).** `src/spellpane.h` forward-declares `class TQuickSpellPane : public TButtonPane` (no body shipped). The pre-release `src/spellpane.{cpp,h}` we DO have is the **talisman COMPOSER** (`TSpellPane`, button rows `spell/book/add/back/down/up/min`) — entirely different role. Reusable: only the class shape (`TQuickSpellPane : TButtonPane`); layout/coords/assets are superseded by retail (§2–§9). Do NOT pull snapshot layout into this spec.

---

## §1 — Overview

The **quick-spell pane** is the left ~210 px segment of the bottom utility bar
that shows **4 gold ring buttons in a horizontal row**, each ring bound to one
of the player's 4 quick-cast slots. When a slot is bound to a spell the player
knows + can cast, the ring is active (clickable, casts on click). When the
slot is empty or the spell is unknown/uncastable, the ring is shown in the
**disabled state** (the `RingG`/glow variant per the TButton state map — §6).

- **Instances:** ONE (`DAT_0065c6f8`). Not mirrored.
- **Hosted by:** `TBottomBarPane` (the bar chrome plate is drawn first by the
  host, then `TBottomBarPane::Draw 0x52c800` invokes
  `cls_0x5a5a30_TQuickSpellPane::virt_meth_0x5444c0(&DAT_0065c6f8)` at
  `cls_0x5a5808.cpp:163` — see `BottomBarPane_SPEC.md §5`). In an off-display
  scratch composite, the chrome method also re-invokes the pane at
  `cls_0x5a5808.cpp:187`.
- **Visibility:** the pane has its own visible flag at `mbr_0x50` (Draw gate
  `:217`). The bottom-bar Pulse path force-shows it on entering mode 2 (idle
  play), pushing `_DAT_0065c740 = _DAT_0065c744 = 0` and `_DAT_0065c748 = 1`,
  then `DAT_0065b688 = 1` ("bottom-bar force-redraw" flag — Pulse `:240-254`).
  Draw `:217` runs the work pass when ANY of `(target != display) OR
  (visible flag) OR (DAT_0065b688 != 0)` is true.
- **State source:** the 4 quick-spell slot **names** live on the **player**
  (`DAT_00667fcc + 0x2cc + slot*6` — a 5-char spell code + null per slot, 5
  slots total at indices 0..4 of which 0 is reserved for the active-cursor
  spell and 1..4 are the four ring bindings; `meth_0x51b560:15019`,
  `meth_0x51b580:15034`).

**Plain language.** Imagine four golden donuts laid flat in a row on the left
end of the bottom bar. Each donut frames the icon for the spell the player has
bound to that quick-cast slot. Clicking a donut casts that spell (or starts
the targeted cast). The cycling/scroll cited in older notes is **not** part of
this pane in the recon I read — there are no scroll arrows here, just the 4
ring buttons. (See §14-A: the per-tick slot-20 method is not extracted; if it
implements any cycle/scroll, this spec misses it.)

---

## §2 — Asset roster

The **ring chrome sprites** live in **`SpellIcons.dat`** (the same archive the
SpellbookPane reads its icons from — opened by TPlayScreen Initialize as
`DAT_0065bc3c = FUN_0047f670("SpellIcons.dat", ...)` at
`cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:108`). The TQuickSpellPane
Init resolves the ring sprites by name from that container
(`FUN_0046d710(DAT_0065bc3c, s_RingU/RingD/RingG)` — `init.cpp:28-30` per
slot). Dumped with `python3 tools/ui/dump_dat.py <…>/SpellIcons.dat` →
`/tmp/dat_dump/SpellIcons.json`.

| asset | archive | entry name | WxH (measured) | flags | kc | role | source rect | cite |
|---|---|---|---|---|---|---|---|---|
| **RingU** | SpellIcons.dat | `RingU` (idx 52) | **48×48** | `0x104` BM_16BIT + BM_ALPHA | `0x0` | base/"up" ring (always-drawn base sprite — TButton `mbr_0x9c`) | whole 48×48 | dump idx 52; init.cpp:28 `FUN_0046d710(s_RingU_005e51f8)`; ctor maps `param_9 → mbr_0x9c` (cls_0x5a3c68.cpp:113) |
| **RingD** | SpellIcons.dat | `RingD` (idx 53) | **48×48** | `0x104` | `0x0` | "down"/pressed ring (used when `flag & 0x10000` — TButton `mbr_0xa0`) | whole 48×48 | dump idx 53; init.cpp:29; cls_0x5a3c68.cpp:120 `mbr_0xa0 = param_8` |
| **RingG** | SpellIcons.dat | `RingG` (idx 54) | **48×48** | `0x104` | `0x0` | "disabled"/no-spell ring (used when `flag & 4` — TButton `mbr_0xa8`) | whole 48×48 | dump idx 54; init.cpp:30; cls_0x5a3c68.cpp:121 `mbr_0xa8 = in_stack_00000044` |
| (spell circle icon, drawn INSIDE the ring) | SpellIcons.dat | `<Player.quickSpell[slot].name>` | **40×40** | `0x2` (15-bit) | `0x0` | the actual spell circle icon resolved by the slot's bound spell name (see SpellbookPane_SPEC §2) | whole 40×40 | dump idxs 0..51 (one per spell); **UNCONFIRMED-D** — Draw path that stamps this is not extracted in this snapshot |

**Not owned by this pane (sibling — for context):**
| asset | WxH | owner | note |
|---|---|---|---|
| `UtilityBar` | 640×60 | TBottomBarPane | bar chrome plate (drawn BEFORE the pane) |
| `Arrow*` (UU/UD/UG/DU/DD/DG) | 24×24 | (UNUSED by this pane — see "Errata" below) | |

**Errata to `BottomBarPane_SPEC.md §2`:** that spec's "Not owned by this pane"
table attributes `Arrow*` (24×24, 6 entries from bottombar.dat) to
TQuickSpellPane. The TQuickSpellPane Init body (`FUN_00544160`) does NOT load
ANY asset from `bottombar.dat` (`DAT_0065a570`); it loads its sprites by name
from **`SpellIcons.dat`** (`DAT_0065bc3c`). The `Arrow*` assets belong to a
different consumer (likely a sidebar arrow / not the bottom ring strip).
Flagged as cross-spec correction.

**Source-rect map.** Each Ring sprite is consumed WHOLE (no atlas slicing).
No regx/regy offsets (both 0). The 16-bit + alpha encoding means the per-pixel
alpha channel carries transparency; **do NOT apply the global magenta key on
these** ([[project-retail-shadow-semantics]] / UI_METHOD_MAP §16).

```
RingU/RingD/RingG 48x48 (whole bitmap, alpha-keyed)
 ┌──────────────┐
 │   ╭──────╮   │   gold ring frame
 │   │ 40x40 │   │   ← spell-icon would be centered IN here (≈ +4,+4)
 │   ╰──────╯   │
 └──────────────┘
```

---

## §3 — Coordinate frames & surfaces

### Pane rect (ctor + TPane base)

The ctor (`cls_0x5a5a30.cpp:29-48` / `cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp:5-22`)
sets the TPane fields (live + template):

| field | ctor value | meaning | cite |
|---|---|---|---|
| `mbr_0x4`  (x, live)     | `0`                 | pane x | `cls_0x5a5a30.cpp:39` (`mbr_0x4 = 0`) |
| `mbr_0x8`  (y, live)     | `0x1a4 = 420`       | pane y | `cls_0x5a5a30.cpp:32` |
| `mbr_0xc`  (w, live)     | `0x280 = 640`       | pane width (greedy — but the pane only USES the left ≈210 px) | `cls_0x5a5a30.cpp:34` |
| `mbr_0x10` (h, live)     | `0x3c = 60`         | pane height | `cls_0x5a5a30.cpp:36` |
| `mbr_0x14/0x18/0x1c/0x20` (template x/y/w/h) | `0/0x1a4/0x280/0x3c` | template mirror | `cls_0x5a5a30.cpp:33,35,37,40` |

So the pane rect @ Classic 640×480 is **(x=0, y=420, w=640, h=60)** — the
**same rect as `TBottomBarPane`** (bottom 60 px strip, bottom-anchored). At
runtime the bar's SetRect (`0x52c930`, `BottomBarPane_SPEC §9`) propagates the
new w/h to `UNK_0065c6fc._24_/._28_` (= QSP `mbr_0x1c` / `mbr_0x20`, template
w/h) so the QSP resizes with the bar.

### Bottom-anchor formula

`pane_y = display_h − 0x3c(=60)`. At Classic `480 − 60 = 420 = 0x1a4` ✓ —
matches the ctor literal exactly (two-way confirmed; same anchor model as the
host bar).

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | TL (root) | `(0,0)` | final pixels; `display_w/h` runtime | — |
| **bar-pane** (= host) | screen | **BL (bottom-anchored)** | `x = 0` (left-anchored); `y = display_h − 0x3c(=60)` (= `0x1a4` @ Classic) | the bottom-bar 60-px strip | `cls_0x5a5808.cpp:32,34,36,39` (BottomBarPane) |
| **qsp-pane** | screen (sibling of bar-pane, same coords) | **BL** | `x = 0`; `y = display_h − 0x3c(=60)` | the quick-spell strip (overlays the left part of the bar) | `cls_0x5a5a30.cpp:32-39`; `cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp:5-12` |
| **button[i]** (i = 0..3) | qsp-pane | TL (left/top-anchored, fixed offset) | qsp-pane-local `(x_i, 10)` where **x_0=0x0a(10), x_1=0x3c(60), x_2=0x6e(110), x_3=0xa0(160)** and y = `0x0a(10)` for all four | one ring button (32×32 hit-rect, 48×48 visible sprite) | init.cpp `:31-32, :46-47, :61-62, :78-79`; per-slot DATs `005e4fe8/fec/ff0/ff4` (x, _data.txt :115901/115904/115907/115910) + `005e4ff8/ffc/5000/5004` (y, _data.txt :115913+) |
| **source-rect** | each Ring bitmap | TL | `(0,0)` | the (sx=0,sy=0,sw=48,sh=48) read from RingU/D/G | dump idx 52/53/54 |

**Composition to screen** for button i:
```
screen_x = qsp_pane_x(=0) + x_i
screen_y = (display_h − 60)  + 10
```
i.e. **screen** `(x_i, display_h − 50)` for the top-left of the 48×48 ring
sprite. At Classic 640×480 the 4 ring TLs are at:
- button 0 (slot 1, label "1"): **screen (10, 430)**
- button 1 (slot 2, label "2"): **screen (60, 430)**
- button 2 (slot 3, label "3"): **screen (110, 430)**
- button 3 (slot 4, label "4"): **screen (160, 430)**

### Corner verification

- pane x: bare constant `0` ⇒ **left-anchored**, offset 0 (ctor `:39`).
- pane y: literal `0x1a4 = 420` decoded as `display_h − 0x3c(=60)` ⇒
  **bottom-anchored**, offset 60 (matches host bar exactly).
- button x_i: bare constants from `DAT_005e4fe8/fec/ff0/ff4` (`0x0a, 0x3c,
  0x6e, 0xa0` — `_data.txt:115901,115904,115907,115910`). Step = `0x32 = 50`.
  ⇒ **TL-anchored in the pane**, constant offsets — the strip does NOT scale
  with bar width; at 1920 the rings stay at the same pane-local x_i (and the
  bar grows around them to the right).
- button y: bare constant `0x0a = 10` for all four ⇒ TL-anchored, top of the
  pane. Bottom of a 48-tall sprite drawn at y=10 lands at y=58 (within the
  60-tall pane). Top inset = 10, bottom inset = 2 — the rings sit just a touch
  toward the top of the bar.

### Spacing arithmetic

Slot pitch = `0x32 = 50` px between TL-x; sprite width 48 → adjacent rings
overlap by `48 − 50 = −2` (i.e. 2-px gap). Total strip span
= `x_3 + 48 − x_0 = 0xa0 + 0x30 − 0x0a = 0xc6 = 198` px (rings occupy pane-x
10..208). Plenty of room within the 640 (or wider) bar.

### Surfaces (direct-renderer contract)

Draw is invoked by the host with the target surface in `in_stack_00000004`
(normally the global display `PTR_DAT_005d79e0`, but the host also re-invokes
it with a scratch target in the off-display compose path —
`BottomBarPane_SPEC §5`). The Draw branches at `:261-268` on whether the
target is the display:
- target == display ⇒ `cls_0x5b98b8::virt_meth_0x435de0(this)` then
  `(*vftbl->virt_meth_0x444fc0_44)(this)` — normal post-draw / mark-clean.
- target != display ⇒ `cls_0x5b93c4::virt_meth_0x435cb0(this)` then same
  post-draw — the scratch-target finalize.

The port should compose the chip onto one offscreen RT (the host bar's RT),
then `DrawSurface` it during the HUD swapchain pass — same contract as the
host bar.

### Layout diagram (pane-local; origin = screen `(0, display_h−60)`)

```
qsp-pane-local x:  0   10                 60                110               160     208           640+
                   ├───┬──────────────────┬──────────────────┬──────────────────┬──────┬─────────────┤  y=0
                   │   │ Ring  (48x48)    │ Ring             │ Ring             │ Ring │             │
                   │   │ button 0 (slot1) │ button 1 (slot2) │ button 2 (slot3) │ btn3 │             │  y=10  ring TL
                   │   │ click-rect 32x32 │ click-rect 32x32 │ click-rect 32x32 │ 32x32│             │
                   │   │ at (10,10)-(42,42)│ at (60,10)-(92,42)│ etc.            │      │             │  y=42  click-rect BR
                   │   │ sprite (10..58, 10..58)               │                  │     │             │  y=58  sprite BR
                   │   │                                                                              │  y=60
                   └───┴──────────────────┴──────────────────┴──────────────────┴──────┴─────────────┘
                                                                              (rest of bar — owned by host)
```

> NOTE on hit-rect vs sprite. The TButton stores rect as `(mbr_0x60=x,
> mbr_0x64=y, mbr_0x68=w, mbr_0x6c=h)` (cls_0x5a3ab8 ctor `:110-113`); w=h=32.
> The Ring sprite is 48×48 stamped at `(mbr_0x60, mbr_0x64)` via
> `meth_0x4bd680(surface, mbr_0x60, mbr_0x64, mbr_0x9c, drawmode)` (TButton
> paint `:629`), so the sprite extends 16 px right + 16 px down past the
> click-rect. **The click-rect is the inner 32×32 corner; the visible ring is
> the 48×48 box at the same TL.** UNCONFIRMED-B: is the player meant to click
> anywhere on the visible ring (48×48), or only the 32×32 inner box? The
> recon stores a 32×32 hit-rect (the (w,h) at the ctor) — but the visible art
> is larger; this is either intentional (the inner click area is what
> matters) or there's a separate hit-test in slot 25 (`0x544890`, not
> extracted). Resolve with `DecompileAddr.java 0x00544890`.

---

## §4 — Static element layout

All (x,y) **screen** coordinates (composed). One row per ring button; the
mirrored-side column is absent (single instance, no mirror). `display_h` is
runtime; at Classic = 480 → button TL.y = 430.

| element | space | (x, y) | (w, h) hit | (w, h) sprite | source rect | drawmode | cite |
|---|---|---|---|---|---|---|---|
| button 0 ring | screen | (`0 + 0x0a`, `display_h − 60 + 0x0a`) = **(10, 430) @ Classic** | (32, 32) | (48, 48) | RingU/RingD/RingG @ (0,0,48,48) | `0x80000000` opaque OR `0x2000` alpha (per drawmode field `mbr_0xd4` defaulting `0x80000000` — `cls_0x5a3c68.cpp:122,580-583`) | init.cpp:31; ctor `:9-12` for pane origin; cls_0x5a3c68.cpp:629 stamps at (mbr_0x60, mbr_0x64) |
| button 1 ring | screen | (`0 + 0x3c`, `display_h − 60 + 0x0a`) = **(60, 430) @ Classic** | (32, 32) | (48, 48) | same | same | init.cpp:46 |
| button 2 ring | screen | (`0 + 0x6e`, `display_h − 60 + 0x0a`) = **(110, 430) @ Classic** | (32, 32) | (48, 48) | same | same | init.cpp:61 |
| button 3 ring | screen | (`0 + 0xa0`, `display_h − 60 + 0x0a`) = **(160, 430) @ Classic** | (32, 32) | (48, 48) | same | same | init.cpp:78 |
| spell-icon overlay (per ring, slot 1..4) | screen | **UNCONFIRMED-D** — likely centered inside ring sprite at ring-TL `(x_i + 4, y_pane + 4)` to seat a 40×40 icon inside the 48×48 ring | (40, 40) | (40, 40) | SpellIcons[`<player.quickSpell[slot].name>`] sx=0,sy=0,sw=40,sh=40 | alpha (`0x2000`) — same as SpellbookPane icon (SpellbookPane_SPEC §7) | NOT extracted (§14-D) |
| spell-name text label (per ring, 2 lines) | screen | **UNCONFIRMED-D** — visually under each ring; per `sample_screen_1.jpg` reference image: each ring shows a 2-line spell-name wrapped underneath. Cell origin likely `(x_i, y_pane + 50)` extending to bottom of bar. | (≈50, ≈14×2) | (font-rendered) | — | font-id with bit 0x400 shadow | NOT extracted (§14-D) |

**Stretch arithmetic** — none on this pane. Both Ring sprite and (likely)
spell icon are stamped 1:1; no dst-size override.

**Mirror.** None — single instance, no mirror column.

---

## §5 — Draw order / composition

The pane's draw is split across two vtable methods (per the inheritance shape
`leaf TButtonPane`):

**Slot 19 `0x435d70` (inherited `TButtonPane::DrawBackground`):** iterates the
4 child buttons in their list order and calls each button's paint
(`cls_0x5a3c68::virt_meth_0x42cc30` — `cls_0x5a3c68.cpp:499-664`). For each
button it composes: optional pressed/glow/disabled state sprite (per `flag`
bits), the base ring sprite (`mbr_0x9c = RingU`), and (if `mbr_0xd8/dc/e0/e4
≠ 0`) the text label via `meth_0x4be2b0`.

**Slot 21 `0x5444c0` (this class's `Overlay` — fully extracted, `cls_0x5a5a30.cpp:205-269`):**
the per-frame slot-validation pass. In order:
1. **Visibility gate** (`:217`): if `target != display` OR `mbr_0x50 != 0` OR
   `DAT_0065b688 != 0` → enter the work loop. (Else skip directly to the
   post-draw at `:261`.) The `DAT_0065b688` flag is set by TPlayScreen Pulse
   on mode→2 re-entry and indicates "redraw the bottom panes regardless of
   visible bit" (Pulse `:252`).
2. **Per-button slot-validation loop** (`:218-258`): for `i = 0 .. (this+0x88 - 1)` (the 4 buttons in `cls_0x41c7f0`'s child list):
   - `piVar2 = cls_0x5b98b8::meth_0x436900(this, i)` — get the i-th child widget.
   - If `piVar2 != null && piVar2[4] == 4` (i.e. widget type id = 4 = TButton — confirmed by `cls_0x5a3c68` having `mbr_0x10 = 4` per the cls_0x5a3ab8 ctor at `cls_0x5a3c68.cpp:96` passing `param_2 = 4`):
     - Read the bound spell name for this button's slot (via the player's quick-spell array; the slot index is the button's `mbr_0x148` — set to 1/2/3/4 by Init): `pcVar4 = (char*)TPlayScreen::meth_0x51b560(DAT_00667fcc, slot)`. (The `slot` arg is `in_stack_00000008` to `meth_0x51b560` — Ghidra elides it.)
     - Walk the string to find length; if it's not empty (`iVar3 != -2` — i.e. length ≥ 1):
       - `meth_0x51b560(player, slot)` again (to get pointer fresh) — `iVar3 = cls_0x53ed70::meth_0x53ed70(UNK_00667c34+4)` — the **spell registry lookup** by name returns nonzero if the spell exists in the SPELL.DEF roster (same lookup the SpellbookPane uses — `SpellbookPane_SPEC.md §0`/§6).
       - If found AND `TPlayScreen::meth_0x51b7c0(player, name)` returns nonzero (the **player-knows-spell** validator — `cls_0x5a7b98_likely_TPlayScreen.cpp:15121-15287`) — the button is **enabled**: `(**(code**)(*piVar2 + 0x1c))(piVar2[5] & 0xfffffffb)` — clear bit 2 (`0x4` = disabled) of the widget's flag word `mbr_0x14` (`:247`).
     - Else (no spell bound, or spell unknown, or player can't cast it):
       `(**(code**)(*piVar2 + 0x1c))(piVar2[5] | 4)` — set bit 2 (disabled) of the widget's flag word (`:253`).
3. **Base draw children pass** (`:259`): `(*vftbl->FUN_004361b0_144)()` — TButtonPane base "draw all children" / commit (slot 144 of the vtable = `FUN_004361b0`). This is where each button's paint actually fires, with the bit-2 disabled flag now set/cleared. Per the TButton paint state map at `cls_0x5a3c68.cpp:561-583`: bit-2 disabled ⇒ `mbr_0xa8 = RingG` is the active sprite; bit clear ⇒ `mbr_0x9c = RingU` (default) is the active sprite.
4. **Post-draw / finalize** (`:261-268`):
   - target == display ⇒ `virt_meth_0x435de0` then `virt_meth_0x444fc0_44`.
   - target != display ⇒ `virt_meth_0x435cb0` then `virt_meth_0x444fc0_44`.

**Net visible order (bottom→top):** host bar UtilityBar plate + BarEndCap →
per-button (TButton paint draws background ring sprite — RingU by default,
RingG if disabled, RingD if pressed) → (UNCONFIRMED-D) per-button spell icon
inside the ring + spell-name text label below → blit-effect flush by host.

---

## §6 — Algorithms

### 6a. Init (slot 0 `0x544160`)

```
TQuickSpellPane.Initialize():                       # cls_0x5a5a30.cpp:115-201 ; init.cpp:3-88
    TButtonPane.Initialize(this)                    # FUN_00434e40 — base init
    bpp = display->vtable[+0x18]()                  # bits-per-pixel
    keycolor = (bpp == 16) ? 0xf81f : 0x7c1f        # init.cpp:21 — RGB565 vs RGB555 magenta
                                                    # (this is passed as param_15 to each
                                                    #  button ctor; param_15 is unused in the
                                                    #  cls_0x5a3c68 body, so the keycolor is
                                                    #  effectively dropped here — it's a
                                                    #  defensive arg)
    for i in 0..3:
        widget = new (cls_0x5b9bf0 if i<2 else cls_0x5a3c68)(
            name        = ["1","2","3","4"][i],              # &DAT_005e5208/5224/5240/525c
            x           = [0x0a, 0x3c, 0x6e, 0xa0][i],       # DAT_005e4fe8/fec/ff0/ff4
            y           = [0x0a, 0x0a, 0x0a, 0x0a][i],       # DAT_005e4ff8/ffc/5000/5004
            w           = 0x20,                              # 32
            h           = 0x20,                              # 32
            secondaryText = 0,                               # no text-label string set
            callback    = [LAB_005440a0,LAB_005440d0,
                           LAB_00544100,LAB_00544130][i],   # thunks → dispatcher cmd 0x14+i
            spriteDown  = FUN_0046d710(SpellIcons, "RingD"), # mbr_0xa0
            spriteUp    = FUN_0046d710(SpellIcons, "RingU"), # mbr_0x9c (default base sprite)
            ?           = 0, ?=0, ?=1,
            slotIndex   = [1, 2, 0xffffffff, 0xffffffff][i], # → mbr_0x148 (for slots 0,1 only)
            ?           = [5, 5, 0, 0][i],                   # param_14 — group/category id
            keyColor    = keycolor,                          # passed but unused
            ?           = 0,
            spriteGlow  = FUN_0046d710(SpellIcons, "RingG")  # mbr_0xa8 (disabled state)
        )
        if i >= 2:                                            # slots 2,3 use cls_0x5a3c68 directly
            widget.mbr_0x148 = (i+1)                          # 3 or 4 — slot id stamp
            widget.mbr_0x14c = 5                              # category/group id
        widget.vftbl = cls_0x5b9c54.vftbl                     # widget-type override
        this.AddChild(widget)                                 # FUN_00436ae0
```

Notes:
- The button class differs for slots 0/1 vs 2/3. `cls_0x5b9bf0` is a thin
  subclass of `cls_0x5a3c68` that stores `mbr_0x148/mbr_0x14c` (slot id +
  category) IN its own struct fields (declared at offset 328/332 of the
  cls_0x5b9bf0 struct — `cls_0x5b9bf0.cpp:7-10`). For slots 2/3 the Init
  manually pokes the same offsets after construction (`init.cpp:64-65,80-81`)
  — same effective layout, just constructed differently. This is a Ghidra
  artifact of how two ctor families happen to be reused. **The 4 buttons all
  end up with `mbr_0x148 = (1|2|3|4)` and `mbr_0x14c = 5`.**
- `mbr_0x148` is the **quick-spell slot index** the button is bound to (1..4
  in the player's 5-slot `field_0x2cc` array; slot 0 is the cursor/hand and
  is NOT shown in this pane). This is the slot id passed to
  `meth_0x51b560(player, slot)` by the per-frame Draw.
- `mbr_0x14c = 5` is a fixed category id; possibly the "TButtonPane group" /
  "widget-type tag" used for input filtering. **UNCONFIRMED-C** — not
  cross-verified.

### 6b. Per-frame slot-validation (slot 21 `0x5444c0`, full pseudocode)

```
TQuickSpellPane.Overlay(target):                 # cls_0x5a5a30.cpp:205-269
    if target != display OR mbr_0x50 != 0 OR DAT_0065b688 != 0:
        for i in 0..(mbr_0x88 - 1):              # mbr_0x88 = child count (4)
            btn = cls_0x41c7f0::GetChild(this, i) # meth_0x436900
            if btn == NULL: continue
            if btn.mbr_0x10 != 4: continue        # widget type != TButton

            slot   = btn.mbr_0x148                # 1..4
            player = DAT_00667fcc                 # TPlayer*
            if player == NULL:
                btn.SetFlag(btn.mbr_0x14 | 4)     # disable
                continue

            namePtr = player.GetQuickSpellName(slot)   # meth_0x51b560
            if namePtr == NULL or *namePtr == '\0':
                btn.SetFlag(btn.mbr_0x14 | 4)     # empty slot — disabled
                continue

            registered = cls_0x53ed70::Lookup(&UNK_00667c34+4, namePtr)
            castable   = player.CanCastQuickSpell(slot)     # meth_0x51b7c0
            if registered != 0 and castable != 0:
                btn.SetFlag(btn.mbr_0x14 & ~4)    # enabled (clear disabled bit)
            else:
                btn.SetFlag(btn.mbr_0x14 | 4)     # disabled

        # base "draw all children" — each button's paint runs here with
        # the current disabled bit deciding RingU vs RingG.
        this.vftbl[144]()                          # FUN_004361b0 (DrawChildren)

    if target == display:
        cls_0x5b98b8::virt_meth_0x435de0(this)
        this.vftbl[+0x44]()                         # mark-drawn / clear dirty
    else:
        cls_0x5b93c4::virt_meth_0x435cb0(this)
        this.vftbl[+0x44]()
```

### 6c. TButton state-machine paint (cls_0x5a3c68 paint — `:499-664`)

The per-button paint picks the sprite by `mbr_0x14` (the widget-flag word):

| flag bit | name (inferred) | sprite picked | source |
|---|---|---|---|
| `0x10000` (& a0 ≠ 0) | pressed/down | `mbr_0xa0` = **RingD** | cls_0x5a3c68.cpp:561 |
| `0x8`     (& a4 ≠ 0) | hover/glow    | `mbr_0xa4` = **null** (our init passes 0 here) — falls through | cls_0x5a3c68.cpp:562 |
| `0x4`     (& a8 ≠ 0) | **disabled**  | `mbr_0xa8` = **RingG** | cls_0x5a3c68.cpp:563 |
| (else)               | normal/up     | `mbr_0xd8` (text color → `mbr_0x9c = RingU` is the always-drawn base) | cls_0x5a3c68.cpp:564 |

The base sprite (`mbr_0x9c = RingU`) is stamped unconditionally at
(`mbr_0x60`, `mbr_0x64`) via `meth_0x4bd680(surface, x, y, RingU, drawmode)`
(`cls_0x5a3c68.cpp:629`). The selected state sprite is composited per the
state branch.

Drawmode (`mbr_0xd4`): defaults to `0x80000000` (= `DM_USEDEFAULT`) per the
ctor (`cls_0x5a3c68.cpp:122`); the paint at `:580-583` falls back to
`(srcBitmap[0x14] & 0xfffdffef) | 0x20` if still default — i.e. the bitmap's
own flags with bit `0x20` (= `DT_TOP`? — irrelevant for sprites) ORed in. For
the alpha-flagged Rings (flags `0x104` = BM_16BIT + BM_ALPHA), this yields
the alpha-blend drawmode. (UI_METHOD_MAP §4 / §16 — `0x2000 = DM_ALPHA`
preferred in modern port.)

### 6d. Command dispatch (the cast)

```
# On button click — TButton emits its command id via the central dispatcher.
# Per init the 4 buttons have callback thunks LAB_005440a0..00544130 (the
# command-emit shims); the TPlayScreen dispatcher case range is:
case 0x14: case 0x15: case 0x16: case 0x17:
    FUN_00544630(cmd - 0x13)        # cls_0544630.cpp:21-25 ; slot = 1..4
        if (0 < slot < 5) and player != NULL:
            player.CastFromQuickSpell(slot)   # meth_0x51b5d0 (cls_0x5a7b98:15053)
```
(`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:321-326`,
`cls_0x544630.cpp:14-26`, `cls_0x5a7b98_likely_TPlayScreen.cpp:15053-15117`)

The cast path inside `meth_0x51b5d0` (`:15053+`) re-validates the slot
(`meth_0x51b7c0`), then either `meth_0x4d5c20_Cast` (begin cast) or shows an
"unknown spell" prompt via `FUN_0054d170` (the dialog/log dispatch).

---

## §7 — Effects & shadows

- **No drop shadow** on the ring sprites. The 4 button ctors do NOT call
  `FUN_00438d80_BlitEffect_StructInit` for the ring blits — the ring is
  drawn via `meth_0x4bd680` (sprite stamp) at the button origin with no
  `(dx,dy)` shadow setup. (Cf. UI_METHOD_MAP §4; the bar chrome itself is
  unshadowed per `BottomBarPane_SPEC §7`.)
- **Chroma key:** RingU/RingD/RingG are flags `0x104` (BM_16BIT + BM_ALPHA)
  with `kc=0`. They are **alpha-keyed, NOT magenta-keyed** — same convention
  as TSideTabsPane strips (`TSideTabsPane_SPEC §10`). **Do NOT apply the
  global magenta key** ([[project-retail-shadow-semantics]] / UI_METHOD_MAP
  §16) or you punch holes in the black pixels of the ring frame.
- **Text 3-pass shadow:** if the per-ring spell-name label is drawn via
  `FUN_004be2b0` with a font-id bit `0x400` (as the SpellbookPane does), the
  text shadow is the standard 1-px right+bottom black 3-pass per
  UI_METHOD_MAP §5. **UNCONFIRMED-D** — the label-draw path is not extracted
  in this snapshot.
- **Blit-effect flush:** the host bar's Draw calls
  `FUN_004aacb0_BlitEffect_Iterate` over the bar rect after all sibling
  panes draw (`BottomBarPane_SPEC §7`). The QuickSpell ring sprites do NOT
  register any effects (no `FUN_004aa850_BlitEffect_Setup` call observed in
  this pane's code path), so this pane contributes nothing to that flush.
- **Pink-halo bug** ([[project-retail-pink-halo-bug]]) is NOT a concern for
  the ring sprites (they have real alpha, no magenta key). It WOULD apply
  to any text rendered via `FUN_004be2b0` — the implementor should render
  text with real-alpha coverage atlases (UI_METHOD_MAP §5 port mapping), not
  reproduce the retail magenta-key fringe.

---

## §8 — Text rendering

**Confirmed text on this pane: NONE** in the extracted Draw `0x5444c0`. The
draw body contains no `FUN_004be2b0` / `meth_0x4be2b0_CompositeBuffer` calls
itself; all text rendering would have to happen inside the per-button paint
(`cls_0x5a3c68::virt_meth_0x42cc30 :656`) or in the **unextracted** slot 20
(`0x5444a0`).

- The TButton paint's text path (`cls_0x5a3c68.cpp:639-657`) is **inactive
  for the quick-spell buttons** because the Init passes `param_6 = 0` (no
  secondary-text label string) into the ctor, leaving `mbr_0x5c = 0`. And
  `mbr_0xd8..0xe4` (the state-specific text colors) are zeroed in the ctor
  (`cls_0x5a3c68.cpp:116-119`). With both null, the `if (local_6c != 0)`
  guard at `:639` skips the text draw entirely. So the base TButton paint
  emits **no** text for our 4 ring buttons.

- The 2-line spell-name text visible in `sample_screen_1.jpg` under each
  ring ("Advanced/healing", "IronSkin", "Fire/Flash", "Ice/Bolt") therefore
  has to come from a DIFFERENT code path:
  1. **Slot 20 `0x5444a0`** (per-tick Animate) — could pull the bound spell
     name from the player + look up the localized display name + compose
     the 2-line text into a cached scratch surface that the base draw
     blits — flagged §14-D. OR
  2. **The TButtonPane base TextLabel slots** at vtable offsets `+0x130/+0x148`
     (`FUN_00436de0`, `FUN_00436ea0`) — possible — but those are still
     inherited (not overridden), suggesting they're not used here.
  3. **A sibling pane** (e.g. a hypothetical `TQuickSpellLabelPane` that
     paints the labels under the rings, sharing the host bar's rect) — none
     observed in the recon. The 4 children of the QuickSpellPane are
     exactly the 4 ring buttons (`mbr_0x88` count = 4 — no label children).

**Until slot 20 (`0x5444a0`) is extracted (§14-A) the spell-name text path
remains UNCONFIRMED.** The most likely candidate is (1) above.

| string | cell | font | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|
| (button label "1"/"2"/"3"/"4") | — | — | — | — | — | — | `mbr_0x18` ctor name field | NOT drawn (mbr_0x5c=0 in Init; TButton paint text-cell gate fails — §8 above) |
| spell-name (2-line label under each ring) | **UNCONFIRMED-D** | likely `0x401` (per SpellbookPane pattern) | likely center | top (DT_TOP) | likely yes (bit 0x400) | wrapped spell display name | `<player.quickSpell[slot].displayName>` (localized via spell-class registry, same path SpellbookPane uses) | NOT extracted (§14-D) |

---

## §9 — Animation & dynamic behavior

### Dirty / redraw model

```
flag DAT_0065b688 (global "bottom-bar needs redraw"):
  set    1   by TPlayScreen::Pulse on mode→2 transition (Pulse :252)
             AND by TQuickSpellPane.slot20 0x5444a0 (per disasm meta —
             body not extracted, UNCONFIRMED-A)
             AND by BarInv slot 20 (virt_meth_0x52ca50)
  read       by TQuickSpellPane.Draw 0x5444c0 :217 (forces work pass
             regardless of visible-bit) — see §5.
             AND by TBarInvPane.Draw 0x52ca70 (sibling).
  cleared    presumably by the post-draw / vtable[+0x44] (444fc0_44) —
             UNCONFIRMED, body of that base method not consulted here.
  cite       recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:252;
             recon/ghidra/_data.txt:130270-130276 (XREF map).

flag UNK_0065c6fc._76_4_ (= QSP this+0x4c, the pane's own dirty flag):
  set    1   by TPlayer.SetQuickSpell(slot, name) — meth_0x51b580:15046
             (whenever the player binds a new spell to slot 1..4)
  read       by the pane's redraw scheduler — UNCONFIRMED (slot 20 body
             not extracted)
  cite       cls_0x5a7b98_likely_TPlayScreen.cpp:15046

flag mbr_0x50 (pane visible bit, byte 80):
  set/cleared by the host bar's on-show propagation
             (TBottomBarPane :138-150 — BottomBarPane_SPEC §9; the
             OnShow virt_meth_0x52c7e0 forces `._72 = ._68 = 1` on the
             BarInv and QuickSpell globals).
```

There are **no animated tweens** identified in the extracted code path. No
fade-in/out, no cycle scroll, no pulse on cast. The pane is a "redraw when
state changes" panel — the per-frame Overlay is **not** an animator, just a
state-validator that toggles the disabled bit per button. If a cycle/pulse
animation exists, it would live in slot 20 (UNCONFIRMED-A).

> **Sanity check vs the brief.** The task summary mentions "cycle/scroll
> animation tween" as a thing to trace. The recon I read shows **no scroll
> arrows, no cycle button** on this pane — the 4 rings are fixed-slot
> bindings the player edits elsewhere (likely via the SpellbookPane:
> SpellbookPane_SPEC §10 says "clicking a spell **picks it up for
> casting/quick-assign**" — so the bind path is in the SpellbookPane, not
> here). Until slot 20 is extracted, I cannot rule out a per-tick cosmetic
> animation; but a cycling-icon animation seems unlikely on a 4-fixed-slot
> ring pane.

### State machine

Single state: "visible & dirty-driven". The per-button paint state machine
is the TButton state machine (§6c) — normal / pressed / disabled. There is
no per-pane phase.

---

## §10 — Input & dispatch (interactive)

### Hit rects

| control | hit rect (pane-local) | hit rect (screen @ Classic) | command id | callback | cite |
|---|---|---|---|---|---|
| ring button 0 | (10, 10, 32, 32) | (10, 430, 32, 32) | **0x14 = 20** | `LAB_005440a0` | init.cpp:31 (x,y,w,h); dispatcher `:321-326` |
| ring button 1 | (60, 10, 32, 32) | (60, 430, 32, 32) | **0x15 = 21** | `LAB_005440d0` | init.cpp:46 |
| ring button 2 | (110, 10, 32, 32) | (110, 430, 32, 32) | **0x16 = 22** | `LAB_00544100` | init.cpp:61 |
| ring button 3 | (160, 10, 32, 32) | (160, 430, 32, 32) | **0x17 = 23** | `LAB_00544130` | init.cpp:78 |

### Dispatch path

```
TButton click → emit cmd 0x14+i (LAB_005440a0..00544130 thunk)
              → TPlayScreen::DispatchCommand (FUN_0047cf40, case 0x14..0x17)
              → FUN_00544630(cmd - 0x13)        # cls_0x544630.cpp:14-26
                    if 0 < slot < 5 and player:
                        TPlayer::CastFromQuickSpell(slot)  # meth_0x51b5d0
                            → meth_0x51b7c0   validate name
                            → meth_0x4d5c20_Cast   (begin cast)
                            (or → FUN_0054d170 show "unknown spell" log entry)
```

(`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:321-326`; `cls_0x544630.cpp:21-25`;
`cls_0x5a7b98_likely_TPlayScreen.cpp:15053-15117`)

### Modal-state global

`DAT_0065d0d0` — the "content presented" gate (read by TQuickSpellPane slot
25 `0x544890` per disasm meta; not extracted body). When zero, no mouse
input dispatched here. Same gate the SpellbookPane uses to suppress clicks
when its scroll is closed (`SpellbookPane_SPEC §10`).

### Disabled button click semantics

When a button has bit `4` set (disabled state — RingG rendered), the TButton
base will likely swallow the click without dispatching the command — the
standard widget pattern. Confirmed-in-pattern but **not extracted from the
TButton dispatch path** (TButton's dispatch is `FUN_004361f0_DefWidget_DispatchInput`
at vtable slot 108, not read here).

### Quick-spell BIND path (not implemented here, for context)

The pane reads the binding (`meth_0x51b560`). The binding is WRITTEN by
`meth_0x51b580(player, slot, "<5-char-spell-code>")` (`cls_0x5a7b98:15034`)
— called from elsewhere (likely SpellbookPane's "quick-assign" hook or a
hotkey path). When the binding changes, `meth_0x51b580` sets
`UNK_0065c6fc._76_4_ = 1` (the QSP's own dirty flag) — the next redraw
revalidates that slot.

---

## §11 — Retail bugs NOT to reproduce

- **None identified** for the Ring blits themselves — alpha-flagged sprites,
  no chroma-key bleed, no shadow setup, no glow color drift.
- **Pink-halo on text** ([[project-retail-pink-halo-bug]]) WOULD apply to any
  spell-name label rendered through `FUN_004be2b0` (UI_METHOD_MAP §5 + §16) —
  if the implementor adds the labels, render through the `BuildTTFAtlas` +
  `CompositeSwapchainTinted` pipeline with real alpha, not magenta-keyed
  scratch. (Same caveat as SpellbookPane_SPEC §11.)
- Once slot 20 `0x5444a0` is extracted (§14-A), re-audit for any
  magenta-cleared scratch reuse that could expose a pink fringe under a
  multi-pass blit.

---

## §12 — Reconstruction pseudocode

```
class QuickSpellPane:
    RingU, RingD, RingG : Bitmap   # SpellIcons.dat, 48x48, alpha
    buttons[4] : RingButton

    def init(self):
        self.rect = Rect(x=0, y=display_h - 60, w=display_w, h=60)   # §3 BL-anchored, greedy w
        self.RingU = LoadBitmap("SpellIcons.dat", "RingU")
        self.RingD = LoadBitmap("SpellIcons.dat", "RingD")
        self.RingG = LoadBitmap("SpellIcons.dat", "RingG")
        positions = [(10,10),(60,10),(110,10),(160,10)]               # §4 ; pane-local
        for i, (x,y) in enumerate(positions):
            self.buttons[i] = RingButton(
                slot_index   = i + 1,                                  # 1..4 (slot 0 is the cursor)
                hit_rect     = Rect(x, y, 32, 32),                     # §10
                sprite_up    = self.RingU,                             # default base sprite
                sprite_down  = self.RingD,                             # pressed
                sprite_disabled = self.RingG,                          # no-spell or unknown
                sprite_size  = (48, 48),                               # visible art (extends past hit-rect)
                command      = 0x14 + i,                               # §10
            )

    def overlay(self, target):                                          # slot 21 0x5444c0  — §5/§6b
        if target != display or self.visible or DAT_0065b688:
            for btn in self.buttons:
                name = player.GetQuickSpellName(btn.slot_index)         # meth_0x51b560
                if not name or not SpellRegistry.has(name) or not player.CanCastQuickSpell(btn.slot_index):
                    btn.set_flag_disabled(True)                         # RingG will be used
                else:
                    btn.set_flag_disabled(False)                        # RingU
            self.draw_children(target)                                  # vtable slot 144
        # post-draw / mark-clean
        self.finalize(target)                                           # branch on target == display

    def draw_button(self, btn, target):                                 # cls_0x5a3c68 paint
        # base + state sprite stamped at hit_rect TL ; 48x48 extends past click rect
        stamp(target, btn.sprite_up, btn.hit_rect.x, btn.hit_rect.y, alpha=True)
        if btn.flag & DISABLED:
            stamp(target, btn.sprite_disabled, btn.hit_rect.x, btn.hit_rect.y, alpha=True)
        elif btn.flag & PRESSED:
            stamp(target, btn.sprite_down,     btn.hit_rect.x, btn.hit_rect.y, alpha=True)
        # base TButton text path skipped (mbr_0x5c == 0 ; mbr_0xd8 == 0)

        # UNCONFIRMED-D — spell icon + 2-line spell name label.
        # If those exist (per the reference image), they likely go here in
        # slot 20 0x5444a0 or a sibling pass. Implementor: confirm by
        # extracting 0x5444a0 (§14-A/D) before adding.
        # Tentative draw if confirmed:
        # spell = player.GetQuickSpell(btn.slot_index)
        # if spell:
        #     icon = SpellIcons[spell.name]   # 40x40
        #     stamp(target, icon, btn.hit_rect.x + 4, btn.hit_rect.y + 4, alpha=True)
        #     drawTextShadowed(target, spell.displayName, cell=Rect(btn.hit_rect.x-9, 60, 50, 28),
        #                      font=0x401, color=RGB(...), wrap=2lines)

    def click(self, x, y):                                              # slot 25 0x544890 — UNCONFIRMED-B
        if not DAT_0065d0d0:        # content-presented gate
            return
        # forward to button pump — each ring's command 0x14+i fires the
        # central TPlayScreen DispatchCommand path
```

Reference §2/§4/§10 for the literal numbers. The implementor MUST extract
slot 20 (`0x5444a0`) before adding the spell-icon/text overlay — do not
mock those visuals (per [[feedback-no-standins]] / [[feedback-ui-no-mock-use-retail]]).

---

## §13 — Port mapping notes

| retail call | port primitive (UI_METHOD_MAP §12) | home |
|---|---|---|
| `meth_0x4bd680(target, x, y, RingU/D/G, drawmode)` ring stamp | `DrawBitmapToTarget(bm, x, y)` (alpha-keyed sprites — UI_METHOD_MAP §4) | renderer |
| TButton state-machine paint (`virt_meth_0x42cc30`) | a small retained-mode `TButton` widget on the port's TButtonPane base — selects sprite per flag word + stamps via `DrawBitmapToTarget` | ui (button widget) |
| compose chip RT → HUD | `DrawSurface` of the host bar's RT (the QSP draws into the host's RT — §3 surfaces) | renderer |
| `meth_0x51b560(player, slot)` quick-spell name read | a `Player::QuickSpellName(int slot)` accessor on the modernized player object | gameplay |
| `meth_0x51b580(player, slot, name)` quick-spell bind | a `Player::SetQuickSpell(int slot, const char* name)` mutator | gameplay |
| `meth_0x51b5d0(player, slot)` cast | a `Player::CastQuickSpell(int slot)` action | gameplay |
| `meth_0x51b7c0(player, name)` known-spell check | `Player::CanCastSpell(const char* name)` predicate | gameplay |
| dispatcher case 0x14..0x17 → `FUN_00544630` | `TPlayScreen::DispatchCommand` table → `CastQuickSpell(slot)` | screen |
| spell registry lookup (`cls_0x53ed70`) | `SpellRegistry::FindByName` (SpellbookPane_SPEC §13 — same primitive) | gameplay |

**No missing primitives.** The Rings are alpha-blitted bitmaps; `DrawBitmap*ToTarget`
covers the four-sprite stamps. The only ports missing are the modernized
player-side hooks (quick-spell name array, cast call) which the gameplay
burndown (`docs/gameplay/BURNDOWN.md`) already enumerates.

---

## §14 — UNCONFIRMED / open questions

1. **UNCONFIRMED-A — slot 20 `virt_meth_0x5444a0` body (per-tick relayout /
   animator).** The vtable dump (`cls_0x5a5a30_TQuickSpellPane_vtable.txt:22`)
   marks this slot as an OVERRIDE. The disasm metadata at
   `recon/ghidra/_data.txt:130275` shows it WRITES `DAT_0065b688` (the
   "force redraw bottom" flag). I did NOT extract the body, so I cannot
   confirm whether it:
   - simply pings `DAT_0065b688 = 1` on a state change (most likely),
   - composites the spell icons into a cached surface (likely candidate for
     where the in-ring icons get drawn — §14-D),
   - implements any cycle/scroll/pulse animation (the brief mentioned this,
     but the rest of the pane shows no scroll arrows / no cycle button —
     UNLIKELY but not ruled out).
   **Impact:** without this body, §9 (animation), §8 (text), and the
   spell-icon-in-ring composite in §4/§6/§12 are educated guesses. **Resolve:**
   `DecompileAddr.java 0x005444a0` — should be a small body (the slot likely
   does compose-into-cache + flag-set). Cross-check what fields of the QSP
   struct (`mbr_0x4c/0x50/0x54/0x17c/0x18c`) it touches and whether it issues
   a `FUN_004be2b0`/`meth_0x4bd680` to a scratch surface (which would
   pinpoint the icon+text composite path).

2. **UNCONFIRMED-B — slot 25 `virt_meth_0x544890` (MouseClick hit-test).** Per
   `_data.txt:131323` it READS `DAT_0065d0d0` (the content-presented gate).
   Body NOT extracted. I assumed the hit-rect is the button's 32×32 inner
   box (stored at `mbr_0x60..0x68` etc.); if 0x544890 implements a custom
   per-pixel hit-test on the 48×48 ring sprite (e.g. using the sprite's
   alpha channel) the visible click area could be larger. **Impact:** how
   close to the ring's outer edge is clickable. **Resolve:**
   `DecompileAddr.java 0x00544890`.

3. **UNCONFIRMED-C — `mbr_0x14c = 5` semantic.** All 4 buttons end up with
   `mbr_0x14c = 5` after Init. The value is shared across the 4 buttons and
   matches what looks like a "category id" for grouping. I did NOT trace any
   consumer of this field. **Impact:** likely cosmetic — possibly used by
   the TButtonPane base for input-routing / group-toggle (mutually-exclusive
   selection?), but our 4 buttons are clearly NOT mutually exclusive (the
   player can have all 4 active at once). **Resolve:** grep `mbr_0x14c |
   ` byte offset `0x14c` across `cls_0x5b9bf0`/`cls_0x5a3c68`/TButtonPane
   reads; or `DecompileAddr.java` on FUN_00436090 (TButtonPane slot 23) to
   see if it filters by this id.

4. **UNCONFIRMED-D — Spell-icon + spell-name text inside / under each ring.**
   The reference image (`docs/ui/sample_screen_1.jpg`) shows BOTH a colored
   spell icon centered inside each Ring frame AND a 2-line spell-name label
   under each ring. Neither is drawn by:
   - the extracted slot 21 `0x5444c0` (no `meth_0x4bd680` icon stamp, no
     `meth_0x4be2b0` text call in the body),
   - the per-button paint (`cls_0x5a3c68.cpp:499-664` — text path skipped
     because `mbr_0x5c == 0` per the Init).
   The most likely host is **slot 20 `0x5444a0`** (UNCONFIRMED-A); a less
   likely alternative is a hidden child or a TPlayScreen overlay pass.
   **Impact:** without this, the reconstruction draws BARE rings with no
   icons inside and no names underneath — a visible regression vs retail.
   The implementor **MUST extract `0x5444a0` first** before declaring this
   pane done. **Resolve:** `DecompileAddr.java 0x005444a0` AND visual-verify
   the resulting reconstruction against `docs/ui/sample_screen_1.jpg`.
   Tentative model if confirmed:
   - per slot, look up `SpellIcons[<player.quickSpell[i].name>]` (40×40),
     stamp centered in the 48×48 ring (`+4,+4` offset);
   - per slot, resolve the spell's localized display name from the spell
     registry (cls_0x53ed70 → spell-class object → name string), then
     `DrawTextShadowed` it in a cell `(x_i − ~9, 50, ~50, ~28)` with font
     `0x401` + 3-pass shadow (same convention as the SpellbookPane).

5. **UNCONFIRMED-E — slot 26 `virt_meth_0x5449a0` (override) role.**
   Inherited slot 26 in TButtonPane is `FUN_00436460` per the base vtable;
   this class overrides it. No data references in `_data.txt` for the addr.
   **Impact:** possibly a context-menu/right-click handler or a drag-drop
   target (the SpellbookPane's slot 26 is the "pick up spell for casting"
   handler — `SpellbookPane_SPEC §10`). **Resolve:**
   `DecompileAddr.java 0x005449a0`.

6. **UNCONFIRMED-F — Bottom-bar pane Z-order vs the QSP.** Both `TBottomBarPane`
   and `TQuickSpellPane` declare the same rect `(0, display_h−60, w, 60)`.
   The host's Draw invokes BarInv then QuickSpell then base
   (`cls_0x5a5808.cpp:162-164`), which is correct draw order (chrome →
   BarInv → QuickSpell). But the QSP also has its OWN `mbr_0x4` etc. — is
   it a CHILD of the bar or a SIBLING under TBottomPane? The recon I read
   does not show TQuickSpellPane being added to the bar's child list — the
   host bar dispatches to it via a *direct* `cls_0x5a5a30::virt_meth_0x5444c0`
   call on the GLOBAL `&DAT_0065c6f8`, not through `cls_0x41c7f0::GetChild`.
   So the QSP is a **standalone pane managed by the host's draw** — not in
   the bar's child list. **Impact:** input routing — does the QSP receive
   clicks via the bar's button pump, or via TPlayScreen's input
   distribution? **Resolve:** trace the call to `FUN_004361f0_DispatchInput`
   on the bar vs the QSP, and check `TBottomPane` (`cls_0x5a5468`) for
   whether the QSP is enumerated there as a sibling pane.

7. **Errata in `BottomBarPane_SPEC.md §2`:** the "Not owned by this pane"
   table attributes `Arrow*` (24×24 alpha sprites in `bottombar.dat`) to
   TQuickSpellPane. The Init body proves this is wrong — TQuickSpellPane
   loads `RingU/RingD/RingG` from **`SpellIcons.dat`** (`DAT_0065bc3c`), not
   the bottombar archive. The Arrow* assets must belong to a different
   consumer (none observed in this pane's code). The bottom-bar spec should
   be amended with: "Owner of `bottombar.dat` Arrow* is UNCONFIRMED — NOT
   TQuickSpellPane." Resolve by grepping `s_ArrowUU` / `s_ArrowDU` etc. in
   `recon/classes/_data.txt` for the XREF method addresses.

8. **Width-greedy assumption.** The ctor sets template width `0x280 = 640`
   and the bar's SetRect (`0x52c930`) re-propagates the live width to the
   QSP template (`UNK_0065c6fc._24_/._28_`). I assumed the pane's mbr_0xc
   updates to match the bar's at runtime. But the 4 button positions are
   FIXED constants (10/60/110/160), so the pane's effective USED region is
   just `x ∈ [10, 208]` regardless. **Impact:** none for layout (the rings
   are TL-anchored); only matters if a hit-test gate uses the pane's
   `mbr_0xc` for window-test. **Resolve:** none required if the rings stay
   TL-anchored.
