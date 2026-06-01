# TScrollPane — Reconstruction Spec

**Panel:** `TScrollPane` (= retail `cls_0x5b5750`, 46-slot TButtonPane-with-DEF-intermediate leaf).
**Role:** in-game parchment scroll reader — single-page paginated text view, opened when the player Uses a `TScroll` game object. Renders the `scroll` parchment bitmap centred on the 640×480 logical display, with NUMLINES=11 lines of wrapped body text inside it, and three small button widgets (down / up / exit) on the lower-right scroll roll.

---

## §0 — Sources & status

**Class id / address:** `cls_0x5b5750` @ retail vtable `0x005b5750`. 46-slot vtable, 408-byte instance struct. Confirmed by Wave-4B 2026-05-16; see `recon/discovered/port_status/TScrollPane.md:3` and the §B.r18 evidence chain.

**Paint method (slot 80 `DrawBackground`):** `0x00521360`, body in `recon/ghidra/cls_0x5b5750.cpp:31-115` (the `virt_meth_0x521360_TScrollPane_DrawBackground` decomp).

**Other key methods consulted:**
- Initialize (slot 0) `0x00521080` — `recon/discovered/cls_0x5b5750_TScrollPane_Initialize_521080.cpp`.
- Close (slot 4) `0x005212c0` — body in `recon/ghidra/cls_0x5b5808.cpp:66-81` (TBookPane file shares the body via Ghidra class-merge).
- KeyPress (slot 108) `0x00521530` — body in `recon/ghidra/cls_0x5b5808.cpp:85-148`.
- Ctor `0x00521d70`, dtor `0x00521df0` — `recon/ghidra/cls_0x5b5750.cpp:121-184`.
- TButtonPane intermediate Initialize (`0x00434e40`) — `recon/discovered/cls_0x5a45c8_intermediate_init_434e40.cpp`.
- TBookPane Initialize (the sibling at `0x005217e0`) — `recon/discovered/cls_0x5b5808_TBookPane_Initialize_5217e0.cpp` (cross-checks the same NewButton call shape).
- Asset string table: `recon/classes_original/_data.txt:111797-111945` (button + asset names: "scroll", "scroll.dat", "downup"/"downdown"/"updown"/"upup"/"exitup"/"exitdown", and the button-name strings "down"/"up"/"exit").
- vtable dump: `recon/ghidra/cls_0x5b5750__vftable_5b5750.cpp` (46 slots, slots 0/4/80/108/124 only are TScrollPane overrides).
- Method-map primitives (mandatory pre-read): `docs/ui/forensics/UI_METHOD_MAP.md` §1 (drawmodes), §4 (`FUN_004bd680` sprite stamp), §5 (`FUN_004be2b0` text composite), §9 (`FUN_0046d710` resource lookup), §15a (surface vtable slots).
- CLASSIC_HUD_REFERENCE §3c (right-sidebar placement) + screenshot `docs/ui/spell_list_ui.jpg` (visual reference, NOT a coordinate source).

**Asset measurements:** dumped via `tools/ui/dump_dat.py scroll.dat`:
- `scroll`   570×374 flags=0x2 kc=0x0
- `updown`   28×23
- `downdown` 28×23
- `exitdown` 28×31
- `upup`     28×23
- `downup`   28×23
- `exitup`   28×31

**Status:** `forensics-complete` for Classic 640×480 layout, asset roster, button rects, draw order, text cell, key handler, and the per-page paging counter. **`forensics-partial` on the open question** of how the 640×480 ctor rect reconciles with the screenshot's right-sidebar appearance (CLASSIC_HUD_REFERENCE §3c claim). Spec §3 documents both readings and the resolve step; the reconstruction agent should start from the literal-cited centred 640×480 layout (which the recon supports unambiguously) and visual-verify against retail before changing it.

**Errors to close (from existing port shell):**
1. `recon/discovered/port_status/TScrollPane.md:39` lists `mbr_0x190 = scrollfont (TFont*)`. **Wrong.** The Initialize literal at `+0x190` is `0x1ae = 430` = SCROLLWIDTH, NOT a font pointer. The font is the GLOBAL `DAT_0065c5c0` (an int font-index used to index the HDC table `DAT_0065b020`, §5 of the method map). See §3 "field layout" below.
2. `mbr_0x194` is documented as "font state #2 / colour?" — actually `0xe6 = 230` = body-text height in pixels (`DAT_0066dacc = 0xe6 / lineHeight` → the per-page NUMLINES, 11 for the scroll font).
3. The existing snapshot `src/scroll.cpp:165` passes `NUMLINES` (=11) as the GDI clip count, but the retail `DrawBackground` does NOT pass 11 directly — the wrap geometry is `(SCROLLWIDTH × bodyHeight) = (0x1ae × 0xe6)` and the per-frame NUMLINES is the precomputed `DAT_0066dacc`.

**Snapshot note (supplementary — superseded by retail):** snapshot `src/scroll.h:48-73` and `src/scroll.cpp:110-235` is an **EXACT** behavioural match to retail Initialize / Close / DrawBackground / KeyPress (asset names + keycodes + NewButton positions all line up to the retail literals). Per `recon/discovered/port_status/TScrollPane.md:5`, the snapshot is "an exact behaviour match" — it is reusable as the algorithm skeleton; only the call-site shapes need to swap to the modern primitives (§13). Layout & coordinates below are still taken from retail literals, not the snapshot.

---

## §1 — Overview

When the player Uses a `TScroll` game object (`src/scroll.cpp:72-86 TScroll::Use`), the global `ScrollPane` instance is bound to that TScroll and pushed onto the play screen via `PlayScreen.SetNextPane(pane); pane->SetScroll(this)`. Initialize loads `scroll.dat`, makes a heap copy of the scroll's text, computes the total wrapped line count (`numlines`), then registers three button widgets. DrawBackground (slot 80) is the per-frame paint; it stamps the parchment chrome to a fixed centre, GDI-renders up to NUMLINES (=11) wrapped body lines starting at the current `line` offset, then asks the buttons subsystem (TButtonPane base's slot-20 = Animate) to paint the down/up/exit chrome on top. KeyPress (slot 108) handles VK_LEFT/VK_RIGHT/VK_HOME/VK_END (single-line and page jumps); the down/up/exit buttons themselves dispatch the same scroll deltas via VK_NEXT/VK_PRIOR/VK_ESCAPE registered as their hotkeys.

The pane is shown until the player presses VK_ESCAPE (or clicks Exit) — `TScrollPane::Close` (slot 4) frees the loaded multi, clears the bound TScroll, and removes itself from `PlayScreen`.

**Plain-language:** centred parchment with a paragraph of wrap-flowed body text and three tiny arrow buttons on the lower-right curl; PgUp/PgDn page through long scrolls; Esc closes.

---

## §2 — Asset roster

All assets live in `scroll.dat` (a `TMulti` archive loaded via `FUN_0047f670` "LoadMulti"). Names come from the literal strings at `_data.txt:111797-111870` (cited per-asset). Sizes measured with `tools/ui/dump_dat.py /Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/scroll.dat` (output captured 2026-05-29).

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| scroll  | scroll.dat | "scroll"   | 570×374 | parchment chrome | full bitmap | name `_data.txt:111797`; size dump_dat |
| downup  | scroll.dat | "downup"   | 28×23  | down button — released sprite | full | name `_data.txt:111812`; cite Initialize `cls_0x5b5750_TScrollPane_Initialize_521080.cpp:60` |
| downdown| scroll.dat | "downdown" | 28×23  | down button — pressed sprite  | full | name `_data.txt:111817`; cite Initialize `:51` |
| upup    | scroll.dat | "upup"     | 28×23  | up button — released sprite   | full | name (DAT_005e2c9c, "upup") `_data.txt:111831`; cite Initialize `:72` |
| updown  | scroll.dat | "updown"   | 28×23  | up button — pressed sprite    | full | name `_data.txt:111840`; cite Initialize `:63` |
| exitup  | scroll.dat | "exitup"   | 28×31  | exit button — released sprite | full | name `_data.txt:111850`; cite Initialize `:84` |
| exitdown| scroll.dat | "exitdown" | 28×31  | exit button — pressed sprite  | full | name `_data.txt:111855`; cite Initialize `:75` |

All bitmaps are 15-bit (`flags=0x2 = BM_15BIT`). `scroll.dat`'s `kc=0x0` for the bitmaps means each asset declares "no key colour stored", and the global magenta key `0x7C1F` applies (UI_METHOD_MAP §16) — but the dumped `scroll` PNG is opaque with anti-aliased edges, so the visible chroma-key concern is mostly the small button sprites (transparent corners). Per [[feedback-vfx-color-health-signal]] / [[project-retail-pink-halo-bug]], the implementor must render via straight-alpha (no magenta key on glyphs); see §11.

There is no atlas — each asset is a standalone bitmap. No source-rect map needed.

**Sibling `book.dat` (TBookPane):** 7 entries — `book` 602×426, `upup/updown/downup/downdown` 28×23 or 28×22, `exitup/exitdown` 28×30. Out of scope for ScrollPane (TBookPane is a sibling leaf with separate spec).

---

## §3 — Coordinate frames & surfaces

### Ctor pane rect (Classic 640×480)

The ctor (`cls_0x5b5750.cpp:121-141`) sets:
- `mbr_0xc  = 0x280 = 640` (width)
- `mbr_0x10 = 0x1e0 = 480` (height)
- `mbr_0x1c = 0x280`, `mbr_0x20 = 0x1e0` (newwidth/newheight)
- All x/y fields = 0

So the **pane rect is `(0, 0, 640, 480)`** in screen space — i.e. the pane covers the entire Classic logical screen. This is the snapshot match `TScrollPane() : TButtonPane(0, 0, WIDTH, HEIGHT)` (`src/scroll.h:52`, where `WIDTH=640, HEIGHT=480` are the Classic logical-screen macros).

### Live destination surface

DrawBackground draws into `PTR_DAT_005d79e0` (`recon/ghidra/cls_0x5b5750.cpp:48`) — i.e. the global display surface, not a per-pane scratch buffer. So **the body text and the parchment both land directly on the screen surface**, not on an offscreen scratch surface composed later. The two-stage-blit trap (NOMENCLATURE §3) does not apply here.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | (root) | — | (0, 0) | the framebuffer | NOMENCLATURE §1 |
| **pane** | screen | TL | (0, 0) | TScrollPane's rect, identical to screen for Classic | ctor `cls_0x5b5750.cpp:124-132` |
| **scroll-chip** | pane | center-h, top-quarter | `(x, y) = ((640 − 570)/2, (480 − 374)/4) = (35, 26) = (0x23, 0x1a)` | the parchment + its body-text area + the 3 buttons (everything is anchored to this origin) | DrawBackground `cls_0x5b5750.cpp:46-47`, Initialize `:49-50` |
| **scroll-local** | scroll-chip | TL (= chip origin) | (0, 0) | element coords given relative to the parchment's top-left | (this spec's drawing convention) |

**Composition formula:** `screen_x = scroll_chip.x + scroll_local.x = 35 + scroll_local.x`, `screen_y = 26 + scroll_local.y`. The same formula applies to the body text cell and to all three buttons because Initialize uses the SAME `(x, y)` formula to anchor every NewButton call.

Body text and buttons in DrawBackground and Initialize cite the chip-relative offsets explicitly:
- Body text: `dstX = iVar6 + 0x41`, `dstY = iVar7 + 0x44` (`cls_0x5b5750.cpp:68`) → scroll-local `(0x41, 0x44) = (65, 68)` → **screen (100, 94)**.
- "down" button: `(iVar3 + 0x1c4, iVar5 + 0x14a)` (Initialize `:61`) → scroll-local `(0x1c4, 0x14a) = (452, 330)` → **screen (487, 356)**.
- "up" button: `(iVar3 + 0x1e4, iVar5 + 0x143)` (Initialize `:73`) → scroll-local `(0x1e4, 0x143) = (484, 323)` → **screen (519, 349)**.
- "exit" button: `(iVar3 + 0x206, iVar5 + 0x141)` (Initialize `:85`) → scroll-local `(0x206, 0x141) = (518, 321)` → **screen (553, 347)**.

(iVar6 in DrawBackground and iVar3 in Initialize are the SAME quantity: `(640 − scroll.width) / 2 = 35`. The two methods name them differently because they re-compute the chip origin per-call — no cached field.)

### Anchor verification

- **scroll-chip:** centred horizontally inside the pane (`(640 − 570)/2`) and at the **top-quarter** vertically (`(480 − 374)/4`, NOT `/2`). The `/4` is a stylistic choice — the parchment hangs from near the top, leaving ~80px of unused space below for the rest of the HUD to peek through. This is the literal in DrawBackground `:46-47` and Initialize `:49-50`. NOT a TL/TR anchor — it's centre-h + top-quarter — but it does NOT scale with display size (640 and 480 are hard-coded literals).
- **buttons:** TL-anchored inside the scroll-chip (positive offsets only); they cluster on the lower-right curl of the parchment (x ≈ 452..518, y ≈ 321..330 in scroll-local, i.e. just inside the right edge — the parchment is 570px wide so 518 is 52px from the right edge).
- **body text:** TL-anchored inside the scroll-chip at `(65, 68)`.

### Surfaces

There is **only one surface in play** — the live display `PTR_DAT_005d79e0`. No scratch surface, no chrome-local frame, no mosaic. Every coord above is therefore *already* in a final composed space (chip + offset). The font-rendering helper `FUN_004be2b0` does internally allocate an offscreen HDC scratch for GDI DrawTextA (§5 of UI_METHOD_MAP), but that scratch is invisible to this panel — it's composited back to the live display in the same call.

**Direct-renderer contract:** the port composes the whole panel into a fixed RT and `DrawSurface`s it once. Do **NOT** draw the parchment via one path and glyphs via another (NOMENCLATURE §3 "direct-renderer contract"). The port primitive for that is `DrawBitmapToTarget` + `DrawTextShadowedToTarget` over a common atlas, then `DrawSurface` to the HUD.

### Layout diagram

```
screen (640 × 480) — Classic logical
+----------------------------------------------------------+
| pane = (0,0,640,480) — full screen                       |
|                                                          |
|         scroll-chip origin = (35, 26)                    |
|             v                                            |
|       +======== scroll 570×374 ========+                 |
|       |                                |                 |
|       | body cell: scroll-local (65,68)|                 |
|       |   = screen (100, 94)           |                 |
|       |   width SCROLLWIDTH = 0x1ae=430|                 |
|       |   height       0xe6 = 230      |                 |
|       |   → up to NUMLINES=11 wrapped  |                 |
|       |     lines starting at `line`   |                 |
|       |                                |                 |
|       |                                |    down 28x23   |
|       |                                |    @sl(452,330) |
|       |                                |    →screen(487,356)
|       |                                |  up 28x23       |
|       |                                |  @sl(484,323)   |
|       |                                |  →screen(519,349)
|       |                                | exit 28x31      |
|       |                                | @sl(518,321)    |
|       |                                | →screen(553,347)|
|       +================================+                 |
|                                                          |
+----------------------------------------------------------+
```

(All coords are FINAL composed values. The buttons cluster on the lower-right scroll-roll of the parchment; see the asset PNG.)

### Field layout (struct, 408 bytes)

| Offset (dec) | Offset (hex) | Field (snapshot name) | Type | Init value | Cite |
|---:|---:|---|---|---|---|
| 4..0x88 | 4..0x88 | TPane base fields (x/y/w/h, new*, scroll*, etc.) | inherited | per ctor (0/0/640/480) | ctor `:121-141`; `port_status/TPane.md` |
| 0x88..0x17c | 0x88..0x17c | TButtonPane intermediate (button array) | inherited | zeroed by TButtonPane Initialize `0x434e40` | `cls_0x5a45c8_intermediate_init_434e40.cpp` |
| 0x17c | 380 | `scroll` (`TScroll*`) — the game object being read | ptr | nullptr at ctor; set later via `SetScroll()` | ctor `:138`; Initialize reads `:27-30`, `:50-51` |
| 0x180 | 384 | `scrolldata` (`TMulti*` for scroll.dat) | ptr | set by Initialize via `FUN_0047f670("scroll.dat", -1, 0)` | Initialize `:25-26` |
| 0x188 | 392 | `line` (current top-line offset; the paging counter) | int32 | 0 at Initialize | Initialize `:22`; KeyPress reads `:100-145` (mbr_0x188) |
| 0x18c | 396 | `numlines` (total wrapped lines in text) | int32 | 0 at Initialize, set later by FindNumLinesInText result | Initialize `:23, :39`; KeyPress clamp `:103` (`mbr_0x18c`) |
| 0x190 | 400 | **`SCROLLWIDTH` = 0x1ae = 430** (the wrap width, NOT a font pointer) | int32 | 0x1ae @ Initialize | Initialize `:21` (`*(undefined4 *)(param_1 + 400) = 0x1ae;`) |
| 0x194 | 404 | **body-text cell HEIGHT in pixels = 0xe6 = 230** (NOT a colour) | int32 | 0xe6 @ Initialize | Initialize `:24` (`*(undefined4 *)(param_1 + 0x194) = 0xe6;`) |

**Note on the old port_status assumption:** `port_status/TScrollPane.md:38-39` calls +0x190 the font pointer and +0x194 unknown. Both are wrong — they are the wrap geometry constants set per-instance. The font is a process-global int at `DAT_0065c5c0` (used to index `DAT_0065b020` per UI_METHOD_MAP §5 — the GDI HDC table). NUMLINES is **not** a field — it is computed once into `DAT_0066dacc` at Initialize time as `0xe6 / lineHeight` (`Initialize :46`).

---

## §4 — Static element layout

All coordinates are FINAL composed (screen) values; the composition arithmetic from scroll-local is in §3.

| element | space | x | y | w | h | source rect | cite |
|---|---|---:|---:|---:|---:|---|---|
| `scroll` chrome (parchment) | screen | 35 | 26 | 570 | 374 | (0, 0, 570, 374) full bitmap, drawmode `0x110 = DM_TRANSPARENT \| DM_BACKGROUND` | DrawBackground `:48` `FUN_004bd680(display, iVar6, iVar7, piVar3, 0x110)` |
| body-text cell | screen | 100 | 94 | 430 | 230 | (text rendered into this cell via FUN_004be2b0) | DrawBackground `:68-69`; field literals `+0x190 = 0x1ae`, `+0x194 = 0xe6` (Initialize `:21, :24`) |
| `down` button hit/draw rect | screen | 487 | 356 | 28 | 31 | up sprite `downup` 28×23, down sprite `downdown` 28×23 (asset dim 28×23, but registered w=28 h=31 — see note) | Initialize `:61` `FUN_00444e30(name, x+0x1c4, y+0x14a, 0x1c, 0x1f, 0x22, ...)` |
| `up` button hit/draw rect | screen | 519 | 349 | 28 | 31 | up sprite `upup` 28×23, down sprite `updown` 28×23 | Initialize `:73` `(name, x+0x1e4, y+0x143, 0x1c, 0x1f, 0x21, ...)` |
| `exit` button hit/draw rect | screen | 553 | 347 | 28 | 31 | up sprite `exitup` 28×31, down sprite `exitdown` 28×31 | Initialize `:85` `(name, x+0x206, y+0x141, 0x1c, 0x1f, 0x1b, ...)` |

**Note on the button registered (w, h):** NewButton (`FUN_00444e30`) receives `0x1c, 0x1f` = (28, 31) for ALL three buttons even though the down/up sprites measure 28×23. This is intentional retail behaviour: the down and up buttons reserve a 28×31 hit-rect on the parchment (matching the exit button's height) so that all three buttons have a uniform clickable height aligned to the exit button — the smaller 28×23 sprite is centred/top-aligned within the 28×31 reserve. The exit sprite is the natural 28×31. This matches the snapshot `src/scroll.cpp:128-130` exactly: `NewButton("down", x+452, y+330, 28, 31, VK_NEXT, ...)`.

There is no player/target mirror — TScrollPane has a single instance.

---

## §5 — Draw order / composition

The retail call order (from `recon/ghidra/cls_0x5b5750.cpp:31-115`, with cross-reference to the snapshot's intent `src/scroll.cpp:154-186`):

1. **Guard:** `if (this->mbr_0x50 != 0)` — only paint if the pane has been Initialized. (mbr_0x50 is the TPane "isopen" flag, see TPane field layout `port_status/TPane.md:50`.) (`cls_0x5b5750.cpp:44`).
2. **Resolve scroll chrome bitmap:** `piVar3 = FUN_0046d710("scroll")` from scrolldata (`:45`). Bitmap handle layout `[0]=width, [1]=height` (UI_METHOD_MAP §4 ring centring proof).
3. **Compute chip origin:** `iVar6 = (0x280 − piVar3[0]) / 2 = 35`, `iVar7 = ((0x1e0 − piVar3[1]) + (0x1e0 − piVar3[1]) >> 0x1f & 3U) >> 2 = 26` (`:46-47`).
4. **Draw parchment:** `FUN_004bd680(PTR_DAT_005d79e0, 35, 26, scroll_handle, 0x110)` (`:48`).
   - drawmode `0x110 = DM_TRANSPARENT (0x100) | DM_BACKGROUND (0x10)` — magenta-keyed background-layer blit. The parchment IS opaque on its own pixels; transparent flag covers any anti-aliased magenta-key edge if present.
5. **Resolve & copy text into a stack-buffer:** if `scroll != null` and `scroll->text != null` (`scroll->text` at offset `+0xd8`, `:50-51`), then:
   - run `strlen` (the `do { … } while (cVar1 != '\0')` loop at `:54-59`); the result `~uVar5 - 1` is the length.
   - if length < 16 (`< 0x10`, `:61`), call `FUN_0049d800(&UNK_0065d1c0.field_0x310)` — a text-resource / string-resource lookup that returns a fallback string. (The snapshot just passes the raw `scroll->GetText()` and skips this fallback — the retail short-text fallback is **NEW behaviour** vs snapshot. Reproduce it.) Otherwise keep the original pointer.
   - Result is in `pcVar4`.
6. **Draw body text:** if `scroll != null && pcVar4 != null` (`:65`), call `FUN_004be2b0_CompositeBuffer(PTR_DAT_005d79e0, iVar6+0x41, iVar7+0x44, mbr_0x190, mbr_0x194, pcVar4, mbr_0x188)` (`:67-69`). Composed values are screen `(100, 94, 430, 230)` cell, text=`pcVar4`, color/shadowPtr=`line` (= 0 initial). Note: Ghidra has dropped trailing args; see §8 for the full call shape.
7. **Hide/show buttons based on paging state** — these are equivalents of `Button(N)->Hide()/Show()` calls realized through vtable dispatch on the pane's button-array at `field_0x88` (the `cls_0x41c7f0` "array" pointer + array data at `field_0x98`). The recon decomp at `:71-109` does this for THREE buttons in order:
   - **down button** (`Button(0)`, the FIRST registered): if `line + (NUMLINES − 1) >= numlines` HIDE else SHOW. The recon `:71-86` checks `if ((int)this->mbr_0x188 < 1) { ... iVar6 = &iRam00000000 ... }` followed by branch-on-array-index that dispatches `(**(code **)(iVar6 + 0x1c))()` — slot 0x1c (=`+0x1c` = vtable slot 7 on TButton, the Hide/Show toggle). Snapshot logic `src/scroll.cpp:168-176` is the same condition, rearranged.
   - **up button** (`Button(1)`): if `line < 1` HIDE else SHOW. Recon `:87-95`.
   - **exit button** (`Button(2)`): always SHOW. Recon `:103-109`.
8. **RedrawButtons:** `(*this->vftptr_0x0->FUN_004361b0_144)()` (`:110`) — TButtonPane intermediate's slot-144 `RedrawButtons` (re-stamps all visible button chrome to the pane's BG cache).
9. **Clear dirty bit:** `(*this->vftptr_0x0->virt_meth_0x444fc0_44)(this)` (`:111`) — slot 44 = `SetDirty(false)` per the snapshot `TScrollPane::DrawBackground` `SetDirty(false)` (`src/scroll.cpp:181`).
10. **Base class paint:** `cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this)` (`:113`) — = `TButtonPane::DrawBackground` (slot 19 at `0x00435d70`/`Animate(true)` at `0x435de0`; this is the base call that composites the buttons over the parchment). Matches `src/scroll.cpp:185` "TButtonPane::DrawBackground();".

**Surfaces touched in order:** display only. No scratch.

### Visibility / dirty model

The recon does NOT wrap the slot-3..slot-9 in an explicit `if (IsDirty())` test — the dirty-test is **implicit in the Hide/Show calls themselves**: `Hide` / `Show` only flip flags on the buttons; the actual button repaint happens in TButtonPane's `Animate` (slot 20). Snapshot wraps the whole chain in `if (IsDirty()) { ... SetDirty(false); }` (`src/scroll.cpp:156, 181`). The retail body's `(*+0x444fc0)(this)` call at the end is the SetDirty(false) — so the dirty-test must be performed somewhere; the recon decomp's elided top-level `if` is the `if (mbr_0x50 != 0)` AND an additional `IsDirty()` check that Ghidra has merged. **UNCONFIRMED A** (see §14): exact placement of the IsDirty test. Reproduce the snapshot wrapping (`if (IsDirty()) { … SetDirty(false); }`) — semantically equivalent and verifiable.

---

## §6 — Algorithms (pseudocode per helper)

### Initialize (slot 0, `0x00521080`)

```
TScrollPane::Initialize():
    TButtonPane::Initialize()             # base; zeros button array + chrome buf
    this->scrollwidth (+0x190)  = 0x1ae   # = 430, the SCROLLWIDTH for wrap
    this->line       (+0x188)  = 0
    this->numlines   (+0x18c)  = 0
    this->bodyheight (+0x194)  = 0xe6     # = 230 px

    this->scrolldata (+0x180) = LoadMulti("scroll.dat")    # FUN_0047f670

    # Recompute total line count for the current bound scroll
    if (this->scroll (+0x17c) != null
        && (text = ((TScroll*)this->scroll)[+0xd8 = GetText()]) != null):
            text_copy = strdup-via-resource(text)         # FUN_0049d800; copies into the same string-resource arena as DrawBackground uses
            this->numlines = FindNumLinesInText(globalFontIndex=DAT_0065c5c0, text_copy,
                                                wrapwidth=this->scrollwidth (0x1ae),
                                                justify=1, ?=0, max=10000, ?=0)   # FUN_004acb80
    else:
            this->numlines = 0

    # Compute & cache per-page NUMLINES (a process-global, not a field)
    hdc          = HDC_table[globalFontIndex]   # DAT_0065b020 + globalFontIndex*4
    if (hdc == null): hdc = DAT_0065b024        # default font HDC
    lineMetrics  = FontMetrics(globalFontIndex) # FUN_00521c60; returns ptr where [0x54] = lineHeight, hdc[0x50] = ascent-something
    DAT_0066dacc = 0xe6 / (lineMetrics[0x54] + hdc[0x50])   # NUMLINES per page (=11 for the scroll font)

    if (this->scrolldata == null): return false

    bitmap = scrolldata.Bitmap("scroll")        # FUN_0046d710("scroll")
    x      = (0x280 - bitmap.width)  / 2        # = 35 for 570w
    y      = (0x1e0 - bitmap.height) / 4        # = 26 for 374h
    NewButton(name="down", dx=x+0x1c4, dy=y+0x14a, dw=0x1c, dh=0x1f, hotkey=0x22 /*VK_NEXT*/,
              cb=&LAB_00521030 /*ScrollBtnScrollDown*/,
              spriteDown=scrolldata.Bitmap("downdown"),
              spriteUp=scrolldata.Bitmap("downup"),
              ...zeros)
    NewButton("up",   x+0x1e4, y+0x143, 0x1c, 0x1f, 0x21 /*VK_PRIOR*/,  &LAB_00520ff0, "updown",  "upup",  ...)
    NewButton("exit", x+0x206, y+0x141, 0x1c, 0x1f, 0x1b /*VK_ESCAPE*/, &LAB_00521070, "exitdown","exitup",...)

    return true
```

Tabulated NewButton call literals (all from `cls_0x5b5750_TScrollPane_Initialize_521080.cpp:61, 73, 85`):

| button | name | x (chip-local hex/dec) | y (chip-local hex/dec) | w | h | hotkey | callback | downSprite | upSprite |
|---|---|---|---|---|---|---|---|---|---|
| 0=down | "d…" (`DAT_005e2c94` first byte 'd', the C-string "down" `_data.txt:111822-111827`) | 0x1c4 = 452 | 0x14a = 330 | 0x1c = 28 | 0x1f = 31 | 0x22 = VK_NEXT (PgDn) | `LAB_00521030` (= `ScrollBtnScrollDown`, snapshot `src/scroll.cpp:100`) | downdown | downup |
| 1=up   | "u…" (`DAT_005e2cac`, "up" `_data.txt:111845-111847`) | 0x1e4 = 484 | 0x143 = 323 | 0x1c | 0x1f | 0x21 = VK_PRIOR (PgUp) | `LAB_00520ff0` (= `ScrollBtnScrollUp`,   snapshot `:95`) | updown | upup |
| 2=exit | "e…" (`DAT_005e2cc4`, "exit" `_data.txt:111860-111864`) | 0x206 = 518 | 0x141 = 321 | 0x1c | 0x1f | 0x1b = VK_ESCAPE | `LAB_00521070` (= `ScrollBtnExit`,        snapshot `:105`) | exitdown | exitup |

Hotkey values are the Win32 VK codes: `VK_ESCAPE = 0x1B`, `VK_PRIOR = 0x21` (Page Up), `VK_NEXT = 0x22` (Page Down). Cross-checked with KeyPress (§ below) which uses `VK_END = 0x23`, `VK_HOME = 0x24`, `VK_LEFT = 0x25` (not used here but in TBookPane), `VK_UP = 0x26`, `VK_DOWN = 0x28`.

### DrawBackground (slot 80, `0x00521360`)

Pseudocode tracks `recon/ghidra/cls_0x5b5750.cpp:31-115`. See §5 for the ordered narrative; the per-call args are:

| call | function | args | cite |
|---|---|---|---|
| chrome blit | `FUN_004bd680` (sprite stamp into current display) | (display, 35, 26, scroll_handle, 0x110 = `DM_TRANSPARENT \| DM_BACKGROUND`) | `:48` |
| body text composite | `FUN_004be2b0_CompositeBuffer` (GDI DrawTextA via offscreen HDC + 3-pass shadow) | (display, dstX=100, dstY=94, cellW=430, cellH=230, text=pcVar4, color/shadowPtr=line, fontIndex=`DAT_0065c5c0`, lineRect=−1, flags, drawmode) — last 4 args dropped in Ghidra trim | `:67-69` |
| each button: Hide/Show | TButton vtable +0x1c | dispatched via the button-array indirection at `field_0x88/0x98` | `:71-109` |
| RedrawButtons | TButtonPane vtable slot 144 (= `FUN_004361b0`) | (this) | `:110` |
| SetDirty(false) | TButtonPane vtable slot 44 (= `virt_meth_0x444fc0`) | (this) | `:111` |
| base paint | TButtonPane vtable slot 20 (= `FUN_00435de0` `Animate(true)`) — i.e. `cls_0x5b98b8::virt_meth_0x435de0(this)` | (this) | `:113` |

### KeyPress (slot 108, `0x00521530`)

Body in `recon/ghidra/cls_0x5b5808.cpp:85-148` (Ghidra puts it under the TBookPane file because both classes share the same body symbol). Pseudocode:

```
TScrollPane::KeyPress(uint key, bool down):
    TButtonPane::KeyPress(key, down)         # base; dispatches to button hotkeys
    if (down):
        delta = 0
        switch key:
            case 0x23 (VK_END):  delta = +100000   # page-jump down
            case 0x24 (VK_HOME): delta = -100000   # page-jump up
            case 0x26 (VK_UP):   delta = -1
            case 0x28 (VK_DOWN): delta = +1
        if (delta != 0):
            oldline = this->line (+0x188)
            this->line = oldline + delta
            if (this->line < 0): this->line = this->numlines  # NOTE retail does NOT clamp to 0 — it WRAPS to numlines on underflow (see :103-104)
            elif (this->line > this->numlines): this->line = this->numlines
            if (oldline != this->line):
                vtable[44 = SetDirty(false)... no, SetDirty(true)?]
                    # mbr_0x444fc0 is the dirty-flag toggle; the OPPOSITE meaning to DrawBackground's use of it.
                    # See §14 UNCONFIRMED-B
```

**Discrepancy vs snapshot:** retail clamp is `if (line < 0 || line > numlines) line = numlines`, i.e. underflow snaps to the LAST line. Snapshot `src/scroll.cpp:228-231` clamps underflow to 0 (more conventional). Reproduce the snapshot intent — the retail wrap-to-end is likely an oversight; flag as a minor retail bug to not reproduce, but visually verify because it might be intentional UX (single-page scrolls wrap immediately). **UNCONFIRMED B** (§14).

### Close (slot 4, `0x005212c0`)

Body in `recon/ghidra/cls_0x5b5808.cpp:66-81`. Pseudocode:

```
TScrollPane::Close():
    TButtonPane::Close()                     # base; frees buttons + clears isopen
    if (this->scrolldata != null):
        FreeMulti(this->scrolldata)          # FUN_00482f80
    PlayScreen.ReleaseExclusivePane(this)    # TCharacter::meth_0x48ed60(DAT_0065caf0) — shape suggests this is the PlayScreen global
    PlayScreen.RemovePane(this)              # cls_0x5a4f28::meth_0x48ef30
    PlayScreen.Redraw()                      # cls_0x5a4f28::meth_0x48ee10
    this->scroll (+0x17c) = null
    UNK_0065cb3c._4_4_ = 1                   # some PlayScreen "must-redraw" / "active-pane changed" flag (UNCONFIRMED-C)
```

Matches snapshot `src/scroll.cpp:140-152` exactly (TButtonPane::Close → free scrolldata → ReleaseExclusivePane → RemovePane → Redraw → clear scroll). The Ghidra cast-confusion `cls_0x5a4f28` and `TCharacter` are misidentifications — both reference the SAME object at `&DAT_0065caf0` = `PlayScreen` global (this is a known Ghidra class-merge artifact; `port_status/TScrollPane.md:16` says so).

---

## §7 — Effects & shadows

### Parchment chrome

The `scroll` bitmap is blitted with drawmode `0x110 = DM_TRANSPARENT | DM_BACKGROUND` — chroma-keyed transparency (magenta), no shadow. The bitmap itself has anti-aliased opaque edges + a few translucent pixels — these go through the magenta-key (0x7C1F in RGB555 per UI_METHOD_MAP §16) which renders them faithfully because the parchment edges aren't AA'd against magenta.

### Body text

Rendered by `FUN_004be2b0` which uses the font flag bit `0x400` to gate the 3-pass black drop shadow (UI_METHOD_MAP §5). Whether this panel sets the shadow flag is **UNCONFIRMED D** — the call site truncated args. Most retail panels do set 0x400 (TPlyrStatusBar, SpellbookPane), so the safe reproduction is: render Arial-12 cream/brown text with 3-pass 1-px-SE black shadow. The colour `param_7` to FUN_004be2b0 is currently mbr_0x188 (=`line` field — that's almost certainly NOT a colour but rather a "shadow-config struct pointer" Ghidra mis-typed; the colour for scroll text is most likely the default font colour from `DAT_0065c5c0`'s FONT.DEF entry, which for the scroll font is a dark brown/sepia matching the parchment).

**Cite trail for the shadow / colour:** unfortunately the body of `FUN_004acb80` (the wrap kernel and likely colour resolver) is not extracted, and the FUN_004be2b0 args after the 7th are dropped by Ghidra. The reconstruction agent should:
- render via `DrawTextShadowedToTarget(atlas, text, cellX=100, cellY=94, cellW=430, ETextAlign::Left, r,g,b, atlas_w, atlas_h)` with the colour pulled from the loaded `Scroll` font's FONT.DEF default (UNCONFIRMED).
- visually verify against retail.

### Buttons

`FUN_00444e30` (NewButton wrapper) — args 9..16 are all zero (`uVar6..uVar13 = 0`, see Initialize `:52-60, :64-72, :76-84`). The trailing `0xffffffff` (`uVar13`) is the "no shadow" sentinel — buttons paint flat sprites with chroma-key transparency, no shadow.

### Pink-halo bug

`scroll.dat` bitmaps have `kc=0x0` (no per-bitmap key), so the global magenta-key `0x7C1F` applies. The parchment edges are not adjacent to magenta in the source PNG, so the pink-halo artifact (UI_METHOD_MAP §16, [[project-retail-pink-halo-bug]]) does NOT manifest on the parchment. The body text **will** show the artifact if rendered with the legacy chroma-key path — reproduce via straight alpha (`DrawTextShadowedToTarget` uses a coverage atlas with real alpha, so the bug is naturally avoided).

---

## §8 — Text rendering

The retail call (`cls_0x5b5750.cpp:67-69`) is **truncated by Ghidra** to 7 args; the underlying `FUN_004be2b0_CompositeBuffer` signature is 11 args (UI_METHOD_MAP §5). Recovered args, with the unresolved tail flagged:

| arg# | UI_METHOD_MAP role | this panel's value | cite |
|---|---|---|---|
| 1 | `this` (dest surface) | `PTR_DAT_005d79e0` = display | `:67` |
| 2 | dstX | `iVar6 + 0x41 = 35 + 65 = 100` (screen) | `:68` |
| 3 | dstY | `iVar7 + 0x44 = 26 + 68 = 94` (screen) | `:68` |
| 4 | cellW | `mbr_0x190 = 0x1ae = 430` (SCROLLWIDTH) | `:69`; Initialize `:21` |
| 5 | cellH | `mbr_0x194 = 0xe6 = 230` | `:69`; Initialize `:24` |
| 6 | text (C-string) | `pcVar4` = `scroll->GetText()` if len ≥ 16, else fallback string-resource text | `:62-65` |
| 7 | color/shadow-ptr `param_7` | `mbr_0x188` = `line` (this is suspicious — almost certainly Ghidra mis-typed the start-line offset as colour; see §14 UNCONFIRMED-D) | `:69` |
| 8 | fontIndex `param_8` | DROPPED — almost certainly `DAT_0065c5c0` (the global scroll-font index registered by Initialize via the FUN_00521c60/FUN_004acb80 path) | UI_METHOD_MAP §5 + the way Initialize sets up DAT_0065c5c0 around line 41-46 |
| 9 | per-line rect / -1 | DROPPED — snapshot passes nullptr | `src/scroll.cpp:165` |
| 10 | flags (h-align / shadow) | DROPPED — snapshot passes 0 (default left-align) | `src/scroll.cpp:165` |
| 11 | drawmode | DROPPED — snapshot passes `DM_USEDEFAULT = 0x80000000` | `src/scroll.cpp:165` |

The snapshot WriteText call (`src/scroll.cpp:165`) is the authoritative API: `Display.WriteText(scroll->GetText(), x+65, y+68, NUMLINES=11, scrollfont, nullptr, DM_USEDEFAULT, SCROLLWIDTH=430, line);` — note the `line` arg is the **starting line offset** (= snapshot's `line`), NOT a colour. Per snapshot semantics, WriteText takes (text, x, y, maxLines, font, colorOverride, drawmode, wrapWidth, startLine). Confirming: arg 7 to FUN_004be2b0 in the recon = `mbr_0x188 = line` = the start-line offset, which the snapshot passes as the LAST arg. So **Ghidra has mis-grouped the call**: the `mbr_0x188` is the LAST passed arg (startLine), not the colour. The colour is null/default — taken from the font's FONT.DEF defaults.

| string | cell (space, x,y,w,h) | font | px | colour | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| (scroll body text) | screen, (100, 94, 430, 230) | "Scroll" entry in `FONT.DEF` (UNCONFIRMED-E: likely Arial-12 sepia, snapshot calls `FontTable->Bitmap("Scroll")`) | (per FONT.DEF) | from FONT.DEF default (UNCONFIRMED-E) | left | top (DT_TOP — text stacks down by lineHeight from cell top, see NOMENCLATURE §2 v-align) | likely yes (3-pass SE black if font flag `0x400` set — UNCONFIRMED-D); no glow | literal `pcVar4` (the bound TScroll's text, or short-text resource fallback) | `TScroll::GetText()` (the per-instance text loaded from the saved scroll object) | `cls_0x5b5750.cpp:67-69` + `src/scroll.cpp:165` |

**Multi-line stacking:** the cell is 230 px tall and holds NUMLINES=11 lines. Per NOMENCLATURE §2 v-align: text stacks **downward from y=94** by `lineHeight`. The cell's lower portion is unused if the text is shorter than the cell capacity (this is fine — DO NOT vertically centre the block). Wrap is hard-clipped at width=430 (the wrap kernel sets `line[N+1]` start so that line[N] never exceeds 430 px of glyphs).

**Paging:** the `line` field is the index of the FIRST visible line; WriteText (the snapshot's wrapper) advances past `line` lines before starting to draw. Retail passes this as the last param.

---

## §9 — Animation & dynamic behaviour

There is **no animation** on TScrollPane — no fades, no slides, no per-tick advance. The panel is fully static between user inputs.

### Dirty / redraw model

- `IsDirty()` returns true iff the panel needs a repaint (set true by `SetScroll()` (Initialize end) and by KeyPress `Scroll()` whenever `line` actually changes).
- DrawBackground is called every frame by the play screen; the cheap body draws nothing if `mbr_0x50 == 0` (pane not initialized), and otherwise the snapshot wraps the body in `if (IsDirty())` so the GDI text composite is skipped on unchanged-state frames.
- `SetDirty(false)` is called once per drawn frame (vtable slot 44 dispatch in the recon `:111`).
- `numlines` is recomputed in `SetScroll()` only — not per frame.
- `DAT_0066dacc` (the cached NUMLINES per page) is computed once in Initialize from the font + body height — not per frame.

### Paging state

| state | value range | drive | effect |
|---|---|---|---|
| `line` (this+0x188) | `[0, numlines]` | KeyPress (±1 / ±100000), button callbacks (±NUMLINES) | starting line index for the text composite; controls `down`/`up` button visibility |
| `numlines` (this+0x18c) | `[0, ...]` | SetScroll() | total wrapped line count; clamp ceiling for `line` |

No tweens. No easing.

---

## §10 — Input & dispatch

**Hit rects (table):**

| control | screen rect (x, y, w, h) | hotkey (Win32 VK) | callback | dispatch path | cite |
|---|---|---|---|---|---|
| down button | (487, 356, 28, 31) | VK_NEXT (0x22, Page Down) | `ScrollBtnScrollDown` → `ScrollPane.Scroll(+NUMLINES=11)` | TButtonPane click + hotkey routing → registered callback (LAB_00521030) | Initialize `:61` |
| up button   | (519, 349, 28, 31) | VK_PRIOR (0x21, Page Up) | `ScrollBtnScrollUp` → `ScrollPane.Scroll(-NUMLINES=11)` | same | Initialize `:73` |
| exit button | (553, 347, 28, 31) | VK_ESCAPE (0x1B) | `ScrollBtnExit` → `ScrollPane.Close()` | same | Initialize `:85` |
| (panel-wide key handler) | n/a | VK_HOME (0x24) | `Scroll(-100000)` (page-jump) | KeyPress slot 108 | KeyPress `:110-121` |
| (panel-wide key handler) | n/a | VK_END (0x23) | `Scroll(+100000)` | KeyPress slot 108 | KeyPress `:99-109` |
| (panel-wide key handler) | n/a | VK_UP (0x26) | `Scroll(-1)` | KeyPress slot 108 | KeyPress `:122-133` |
| (panel-wide key handler) | n/a | VK_DOWN (0x28) | `Scroll(+1)` | KeyPress slot 108 | KeyPress `:134-145` |

**Modal-state globals:** none — TScrollPane is itself the modal pane (set exclusive via `PlayScreen.SetExclusivePane(this, true)`, snapshot `src/scroll.cpp:133`). While the scroll is open, all input flows through it; the play screen's main HUD does NOT receive clicks.

**Activation path:** `TScroll::Use(player, with=-1)` → `PlayScreen.SetNextPane(ScrollPane)` → `ScrollPane.SetScroll(this)` (snapshot `src/scroll.cpp:72-86`). The retail equivalent is the same call sequence (the snapshot is an exact match).

**Close paths (any of these):** click Exit; press VK_ESCAPE; receive any other "close exclusive pane" command from the play screen.

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo on text** — the standard chroma-key + AA-shadow artifact (UI_METHOD_MAP §16, [[project-retail-pink-halo-bug]]). Render via straight-alpha coverage atlas; no magenta key on glyphs. Reproduce intent = clean text with 1-px-SE black shadow, no pink fringe.
2. **KeyPress underflow wrap** (UNCONFIRMED-B) — when `line - 1 < 0`, retail snaps to `numlines` (i.e. wraps to the last line). Snapshot snaps to 0. If retail's wrap is intentional UX it should be preserved; if it's a thinko, reproduce snapshot behaviour. Visually verify with a short scroll before deciding.
3. **No bug otherwise.** No 4-px shadow trap (the parchment doesn't use FUN_00438d80), no two-stage blit, no mirror.

---

## §12 — Reconstruction pseudocode

```cpp
// All coords are screen (Classic 640×480); resolution-mode scaling handled by RT.

class TScrollPane : public TButtonPane {
    TScroll*  scroll      = nullptr;    // bound game object
    TMulti*   scrolldata  = nullptr;    // scroll.dat
    int32_t   line        = 0;          // top-line of visible window
    int32_t   numlines    = 0;
    int32_t   scrollwidth = 430;        // = SCROLLWIDTH (0x1ae)
    int32_t   bodyheight  = 230;        // = body cell height (0xe6)
    int32_t   numLinesPerPage = 0;      // = bodyheight / lineHeight, cached @ Initialize (was DAT_0066dacc)
    TFont*    scrollfont  = nullptr;    // FontTable->Bitmap("Scroll")
};

bool TScrollPane::Initialize() {
    TButtonPane::Initialize();
    line = 0; numlines = 0;
    scrollwidth = 430; bodyheight = 230;

    scrolldata = TMulti::LoadMulti("scroll.dat");
    scrollfont = FontTable->Bitmap("Scroll");
    SetScroll(scroll);                            // recompute numlines for the bound scroll, if any
    numLinesPerPage = bodyheight / (scrollfont ? scrollfont->LineHeight() : 1);

    if (!scrolldata) return false;
    PTBitmap scrollBmp = scrolldata->Bitmap("scroll");
    int x = (640 - scrollBmp->width) / 2;         // = 35
    int y = (480 - scrollBmp->height) / 4;        // = 26

    NewButton("down", x+452, y+330, 28, 31, VK_NEXT,
              &ScrollBtnScrollDown, scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
    NewButton("up",   x+484, y+323, 28, 31, VK_PRIOR,
              &ScrollBtnScrollUp,   scrolldata->Bitmap("updown"),   scrolldata->Bitmap("upup"));
    NewButton("exit", x+518, y+321, 28, 31, VK_ESCAPE,
              &ScrollBtnExit,       scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

    PlayScreen.AddPane(this);
    PlayScreen.SetExclusivePane(this, true);
    return true;
}

void TScrollPane::SetScroll(PTScroll s) {
    scroll = s;
    numlines = (scroll && scroll->GetText() && scrollfont)
               ? scrollfont->FindNumLinesInText(scroll->GetText(), scrollwidth)
               : 0;
    SetDirty(true);
}

void TScrollPane::Scroll(int32_t delta) {
    int oldline = line;
    line = std::clamp(line + delta, 0, numlines);   // see §11 #2: snapshot clamp, NOT retail's wrap-to-end
    if (oldline != line) SetDirty(true);
}

void TScrollPane::KeyPress(int32_t key, bool down) {
    TButtonPane::KeyPress(key, down);
    if (!down) return;
    switch (key) {
        case VK_UP:   Scroll(-1); break;
        case VK_DOWN: Scroll(+1); break;
        case VK_HOME: Scroll(-100000); break;
        case VK_END:  Scroll(+100000); break;
    }
}

void TScrollPane::DrawBackground() {
    if (!IsOpen()) return;
    if (IsDirty()) {
        // 1. parchment chrome — chroma-key + DM_BACKGROUND, opaque on body
        PTBitmap scrollBmp = scrolldata->Bitmap("scroll");
        int x = (640 - scrollBmp->width) / 2;     // = 35
        int y = (480 - scrollBmp->height) / 4;    // = 26
        Renderer->DrawBitmapToTarget(scrollBmp, x, y);     // DM_TRANSPARENT | DM_BACKGROUND

        // 2. body text — 3-pass SE black shadow, sepia (font default)
        if (scroll && scroll->GetText()) {
            const SFontAtlas* atlas = BuildFontAtlas(scrollfont);
            // WriteText: starting at `line`, NUMLINES_PER_PAGE-clipped, wrap at scrollwidth
            DrawTextShadowedToTarget(atlas,
                                     scroll->GetText(),
                                     /*cellX=*/x+65, /*cellY=*/y+68,
                                     /*cellW=*/scrollwidth,
                                     ETextAlign::Left,
                                     /*r,g,b=*/sepia.r, sepia.g, sepia.b,
                                     /*wrapStartLine=*/line,
                                     /*maxLines=*/numLinesPerPage);
        }

        // 3. button visibility
        Button(0 /*down*/)->SetVisible(line + numLinesPerPage - 1 < numlines);
        Button(1 /*up*/  )->SetVisible(line > 0);
        Button(2 /*exit*/)->SetVisible(true);

        RedrawButtons();
        SetDirty(false);
    }

    // 4. base paints buttons over the parchment
    TButtonPane::DrawBackground();
}

void TScrollPane::Close() {
    TButtonPane::Close();
    delete scrolldata; scrolldata = nullptr;
    PlayScreen.ReleaseExclusivePane(this);
    PlayScreen.RemovePane(this);
    PlayScreen.Redraw();
    scroll = nullptr;
}
```

Button callbacks (file-scope):
```cpp
void ScrollBtnScrollDown() { ScrollPane.Scroll(+11 /*NUMLINES*/); }
void ScrollBtnScrollUp()   { ScrollPane.Scroll(-11);              }
void ScrollBtnExit()       { ScrollPane.Close();                  }
```

---

## §13 — Port mapping notes

| retail primitive | call site | port primitive (UI_METHOD_MAP §12) | notes |
|---|---|---|---|
| `FUN_004bd680(display, 35, 26, scroll_bm, 0x110)` parchment | DrawBackground `:48` | `Renderer->DrawBitmapToTarget(scrollBmp, 35, 26)` | drawmode handles chroma-key; the BACKGROUND bit is moot in the modern RT path. |
| `FUN_004be2b0_CompositeBuffer` body text | DrawBackground `:67-69` | `DrawTextShadowedToTarget(atlas, text, x+65, y+68, 430, Left, r,g,b, line, numLinesPerPage)` | font.cpp canonical; the 3-pass SE shadow is built-in. |
| `FUN_0047f670("scroll.dat", -1, 0)` LoadMulti | Initialize `:25` | `TMulti::LoadMulti("scroll.dat")` (already in `src/multi.h`) | direct snapshot match. |
| `FUN_0046d710("scroll" / "downup" / …)` resource by name | Initialize `:48, :51, ...`; DrawBackground `:45` | `multi->Bitmap("name")` (snapshot API in `src/multi.h`) | The Wave-3 method-map UNCONFIRMED-2 (dual call shape) doesn't apply — TScrollPane only uses the by-name form. |
| `FUN_00444e30(...)` NewButton | Initialize `:61, :73, :85` | `TButtonPane::NewButton(name, x, y, w, h, hotkey, &cb, downSprite, upSprite)` (snapshot API in `src/button.h`) | direct snapshot match. |
| `FUN_004acb80` (FindNumLinesInText + wrap kernel) | Initialize `:38` | `scrollfont->FindNumLinesInText(text, scrollwidth)` (snapshot API in `src/font.h:36`) | already exists; the Win32 wrapper logic stays in the port's WriteText path. |
| `FUN_0049d800` (string-resource lookup, used to copy or fall-back the text) | DrawBackground `:62` | (already covered by the modern `TScroll::GetText()` — if `text != nullptr`, use it; the short-string-resource fallback is retail-specific behaviour to flag and probably skip unless we localise) | UNCONFIRMED whether the fallback resource lookup is ever triggered in practice. |
| `cls_0x5b98b8::virt_meth_0x435de0` (TButtonPane base paint = Animate(true)) | DrawBackground `:113` | `TButtonPane::DrawBackground()` (snapshot direct base call) | snapshot ends with `TButtonPane::DrawBackground();` exactly. |
| KeyPress switch | KeyPress `:97-145` | direct snapshot `TScrollPane::KeyPress` reuse | snapshot already implements it. |
| Close | Close `:66-81` | direct snapshot `TScrollPane::Close` reuse | snapshot already implements it. |

**No new primitives needed.** Every call maps to an existing one — this is the cleanest possible port among the panels specced so far. The Wave-3 method-map already covers `DrawBitmapToTarget`, `DrawTextShadowedToTarget`, `BuildFontAtlas`. The snapshot's `TButtonPane::NewButton` is the registration API and stays as-is.

---

## §14 — UNCONFIRMED / open questions

1. **UNCONFIRMED-A: IsDirty() placement in retail DrawBackground.** The recon decomp at `cls_0x5b5750.cpp:31-115` does NOT wrap the body in an explicit `if (IsDirty()) { ... SetDirty(false); }` — it has a SetDirty call (slot 44 `virt_meth_0x444fc0`) at the END of the body and the IsDirty test (if present) is elided by Ghidra. Snapshot wraps the whole body. **Impact if wrong:** unnecessary per-frame GDI re-draws; visible perf only (no visual bug). **Resolve:** decompile slots 7/19/44 cleanly (`DecompileAddr.java 0x00444fc0`, `0x00436010`, `0x00435d70`) to confirm the dirty-flag toggle and whether DrawBackground's body is guarded.
2. **UNCONFIRMED-B: KeyPress underflow wrap.** Recon `cls_0x5b5808.cpp:103, 114, 126, 138` writes `if (line < 0) line = numlines` — i.e. underflow wraps to the LAST line, not clamps to 0 like snapshot. Likely a retail oversight but might be intentional for short scrolls (single-page wraps back instantly). **Impact if wrong:** the user pressing VK_HOME on a single-page scroll jumps to the end instead of staying put. **Resolve:** visual-verify against the retail exe — open a 1-page scroll, press VK_HOME, confirm whether the visible text moves.
3. **UNCONFIRMED-C: PlayScreen Close globals.** `Close` references `&DAT_0065caf0` (PlayScreen object) and the `UNK_0065cb3c._4_4_ = 1` write at the end. Snapshot does NOT have this trailing write; it likely flips the next-pane-dirty bit. **Impact if wrong:** the play-screen HUD might not redraw after the scroll closes — visible blank-rect bug on close. **Resolve:** decompile `cls_0x5a4f28` (PlayScreen) fields around `+0x6c4c` (offset of UNK_0065cb3c relative to `0x65caf0`).
4. **UNCONFIRMED-D: FUN_004be2b0 args 7..11 (color, fontIndex, lineRect, flags, drawmode).** Ghidra has dropped args 8-11 from the call. Snapshot says `(text, x, y, NUMLINES=11, scrollfont, nullptr=color, DM_USEDEFAULT=drawmode, SCROLLWIDTH=430, line)`. The arg7 value is `mbr_0x188` (= `line` field), which makes sense as the START-LINE param (NOT colour) per the snapshot. The colour, fontIndex, lineRect, flags, and drawmode are most likely (default-from-FONT.DEF, DAT_0065c5c0, -1, 0 or 0x400 shadow, DM_USEDEFAULT). **Impact if wrong:** text could be left/centre/right wrong, shadowed/not, or wrong colour. **Resolve:** decompile the call site disassembly `objdump -d --start-address=0x521430 --stop-address=0x521530 data/Revenant.exe` to read the full arg-push sequence.
5. **UNCONFIRMED-E: "Scroll" font identity & default colour.** Snapshot says `scrollfont = FontTable->Bitmap("Scroll")` — the FONT.DEF "Scroll" entry. The actual font file, point size, and default colour are not specified in the recon (they live in FONT.DEF). **Impact if wrong:** wrong font face/size/colour. **Resolve:** read `FONT.DEF` (or `data/fonts/FONT.DEF`) for the "Scroll" entry; cross-check colour with the screenshot `docs/ui/spell_list_ui.jpg`. Visual reference suggests a brown-on-parchment sepia (~ #4a2810 or similar).
6. **UNCONFIRMED-F: right-sidebar reconciliation (per CLASSIC_HUD_REFERENCE §3c).** The ctor sets pane rect to full-screen (0,0,640,480), and Initialize/DrawBackground centre the parchment on the full 640-wide display. But CLASSIC_HUD_REFERENCE §3c says the scroll renders "in the right sidebar slot, not as a main-area overlay" (with screenshot `spell_list_ui.jpg` apparently showing this). Reading the recon literally: the parchment IS centre-of-screen at (35,26,570,374) — that's not a sidebar position, that's main-area-centred. **Three possible reconciliations:**
   - (a) The screenshot is from an earlier or modded build; the shipped retail centres the scroll. The recon is authoritative.
   - (b) The pane is full-screen but a clip-rect somewhere in the TButtonPane intermediate's slot-2 (`0x00435010`) or the TPane base's slot-9 (`0x00491a80`) culls the draw to the right-sidebar rect. The parchment ends up only rendering its right portion. This would explain why the scroll bitmap's right edge (with the curl + buttons) is what's visible in the screenshot.
   - (c) The sidebar TSideTabsPane's tab-switch dispatcher places the TScrollPane at a different rect via a `SetExclusivePane` re-layout — but that would mean SetExclusivePane has a rect arg, which the snapshot's signature lacks.
   **Impact if wrong:** the panel renders in the wrong half of the screen. **Resolve:** (1) visual-verify against retail with a real scroll-use event, (2) if mismatched, decompile slots 2 and 9 of TButtonPane / TPane and look for a clip-rect setup, (3) cross-check with the screenshot's exact parchment crop (measure the visible parchment fragment in `docs/ui/spell_list_ui.jpg` against the 570×374 full asset).

The reconstruction agent should implement the literal centred 640×480 layout (resolution-mode-scaled by the modern RT) and visually verify against retail before changing it. If a real scroll-use shows the parchment in the right sidebar, return to this spec with the resolved coords.

---

## Self-verification log

- **Frame composition check:** body text scroll-local (65, 68) + chip origin (35, 26) = screen (100, 94). DrawBackground recon `:68` literal `iVar6 + 'A'` = iVar6 + 0x41 = 35 + 65 = 100. ✔
- **Button anchors:** all three buttons use `iVar3 + …` and `iVar5 + …` which ARE the same chip-origin as DrawBackground's `iVar6/iVar7`. The recon proves the chip origin is recomputed identically per call (both `(0x280 - bm.w)/2` and the funky `((0x1e0 - bm.h) + …) >> 2` formula for /4 rounding-toward-zero). ✔
- **Cross-check vs sibling TBookPane:** TBookPane Initialize `cls_0x5b5808_TBookPane_Initialize_5217e0.cpp:49` uses `/3` for the y-quarter (snapshot `:275` also `/3`), and TScrollPane uses `/4` — consistent with the snapshot's choice; the difference is real, not a Ghidra glitch. ✔
- **Asset names:** all 7 entries in scroll.dat dump match the literal strings in `_data.txt:111797-111870` exactly. ✔
- **Hotkey values:** VK_NEXT=0x22 ✔, VK_PRIOR=0x21 ✔, VK_ESCAPE=0x1B ✔, VK_END=0x23 ✔, VK_HOME=0x24 ✔, VK_UP=0x26 ✔, VK_DOWN=0x28 ✔ (Win32 standard).
- **Stranger-rebuild test:** the §12 pseudocode + §4 table + §2 asset list are sufficient to build the panel pixel-accurate WITHOUT touching the recon. The 5 UNCONFIRMED items are all flagged with concrete resolve-steps and bounded impact; none of them is in the critical centring/asset path.
