# Character Portrait — Forensics SPEC

How retail produces the **character portrait** (the head image inside the gold
`Ring` of the player/target status-bar chip) — and, where it overlaps, the
**paperdoll** (TEquipPane) character figure.

> **HEADLINE (this overturns the task's working hypothesis).** The HUD portrait
> is **NOT a live 3D head render**. It is the character's **pre-authored
> inventory icon** — a flat RGB555 bitmap baked into the character's `.i3d`
> file, one per object *state*, retrieved by `imagery->GetInvImage(state)`. The
> `+0x130` call on the Player object resolves to the **unmodified base
> `TObjectInstance::InventoryImage`** (`FUN_0046f190`); **TPlayer does not
> override it**, and the port **already implements the entire path**
> (`TObjectInstance::InventoryImage` → `T3DImagery::GetInvImage` →
> `icons[state].invitem`). See §0 evidence + §14 for what the implementor must
> still confirm (does the *player* character's `.i3d` actually ship an icon, and
> its dimensions).

---

## §0 — Sources & status

**Status:** `forensics-complete` for the render path + class/vtable resolution.
`forensics-partial` only on the *data* (exact baked-icon pixel dimensions and
whether the shipped player `.i3d` carries an icon at all) — these live in a
packed binary `.rvi` and need an extractor (§14-1).

**Class / vtable resolution (the spine of this spec):**

| symbol | addr | role | evidence |
|---|---|---|---|
| portrait call site (player) | — | `iVar1 = (**(code**)(*DAT_00667fcc + 0x130))()` then `FUN_004bd680(0,0,iVar1,0x80000000,0)` | `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp:16,18` |
| portrait call site (target) | — | same on the target char ptr | `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_TargetSide_54a310.cpp:19,21` |
| `DAT_00667fcc` | global | **the Player** (`cls_0x5b4f30_TPlayer *`) | typed at `recon/ghidra/cls_0x5b5560.cpp:646`, `cls_0x57d9d0.cpp:1293/1324/1338`, `cls_0x5756d0.cpp:1416` |
| **TPlayer vtable** | `0x5b4f30` | slot **[76] = byte 0x130 → `FUN_0046f190`** (the BASE method; **no override**) | `DumpVtable.java 0x5b4f30` → `[76] @ 005b5060 -> 0046f190` |
| **TCharacter vtable** | `0x5a5320` | slot [76] = byte 0x130 → `0x487c30` = **empty stub `RET 0xc`** (returns garbage/0) | `DumpVtable.java 0x5a5320` → `[76]->00487c30`; `Disasm.java 0x487c20` → `00487c30  RET 0xc` |
| base TObjectInstance vtable | `0x5a7b98` | slot [76] = byte 0x130 → `FUN_0046f190` | `DumpVtable.java 0x5a7b98` → `[76] @ 005a7cc8 -> 0046f190` |
| **`FUN_0046f190`** = `TObjectInstance::InventoryImage` | `0x46f190` | reads imagery `this+0x54`, calls imagery `vtable[0xd4](state,0)` then `[0xd8](state)`; returns a bitmap surface | decompiled `/tmp/base_46f190.c` (this session) |
| imagery `vtable[0xd4]` = `GetInvImage` | `0x40ce60` | returns `icons[state].invitem` (`param_1[0x27][state*2]`) | decompiled `/tmp/imagery_d4_getinvimage.c` |
| imagery `vtable[0xd8]` = `GetInvAnimation` | `0x40cef0` | returns `icons[state].invanim` (`param_1[0x27]+4+state*8`) | decompiled `/tmp/imagery_d8_getinvanim.c` |

> **Mislabel caveat (AGENT_PROTOCOL):** the recon's *bulk* dumps mis-merge class
> labels — `recon/ghidra/cls_0x5a7b98_likely_TPlayScreen.cpp` is actually the
> **base TObjectInstance/TCharacter-ancestor vtable**, and there is a stray
> `cls_0x5b93c4_TPlayer` whose `+0x130` (`0x436340`) is a **menu/group command
> handler** (`void(this,int,int)` switching command ids 0x401/0x403/0x404/
> 0x406/0x408) — that vtable is NOT the Player's. The authoritative TPlayer
> vtable is `0x5b4f30`, fixed by the *typed* `DAT_00667fcc` references above. I
> resolved every slot by **dumping the live vtable from the binary**, not by
> trusting the merged labels.

**Recon files read:** `cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp`,
`..._helper_TargetSide_54a310.cpp`, `cls_0x5a5320__vftable_5a5320.cpp`,
`cls_0x5a7b98__vftable_5a7b98.cpp`, `cls_0x5a35ac__vftable_5a35ac.cpp`,
`cls_0x5b8e94_TObjectInstance_Final.cpp`,
`recon/classes_original/cls_0x5a7b98_TCharacter.cpp` (struct only),
`recon/discovered/port_status/EquipSidebarPane.md`,
`docs/ui/briefs/B_r2_character_panel_hunt.md`.
**Ghidra extractions (this session):** `/tmp/base_46f190.c`,
`/tmp/portrait_487c30.c` (stub), `/tmp/tplayer_portrait_436340.c` (wrong vtable,
documented), `/tmp/imagery_d4_getinvimage.c`, `/tmp/imagery_d8_getinvanim.c`.
**Prior briefs:** `TPlyrStatusBar_SPEC.md` (§2 portrait note, §3, §15b).
**Port source consulted:** `src/object.h:1036,1066`, `src/imagery.h:380,200`,
`src/imagery.cpp:858`, `src/3dimage.cpp:339-373,2005-2024`,
`src/3dimagebody.h:403,414-420`, `src/bitmapdata.h:34-56`,
`src/equip.{h,cpp}`, `src/testmodes.cpp:462-560`,
`src/uiplyrstatusbartest.cpp`.

**Errors this spec closes (vs the task brief + the placeholder harness):**
1. **"TPlayer/TCharacter override +0x130 to render the head live."** FALSE.
   TPlayer inherits the base; TCharacter's override is an empty stub. There is
   no live-render path behind the HUD portrait. (Evidence above.)
2. The harness (`uiplyrstatusbartest.cpp:199`) uses a 30×30 `LockeFace`
   placeholder. The correct source is the character's own baked `.i3d` icon via
   `oi->InventoryImage()` — already implemented in the port.

**Snapshot note (supplementary — superseded by retail):** the 1998/99 snapshot
`src/equip.cpp` *is* the retail `TEquipPane` (same class), and its
`DrawBackground`/`DrawInvItem` logic is the live retail behavior, not a
superseded design — see §5/§6. No separate snapshot portrait widget exists; the
HUD portrait has always been the inventory-icon blit.

---

## §1 — Overview

The portrait is the head-and-shoulders image of a character shown inside the
gold `Ring` of a status-bar chip — player chip (always, when a Player exists)
and target chip (when a target is acquired). Retail obtains it by calling the
object's `InventoryImage()` virtual (`+0x130`) and blitting the returned bitmap
**opaque** at the chrome surface's top-left `(0,0)`; the `Ring` and the
three stat icons are stamped *over* it afterward, so the Ring frames the head.

Plain-language: every Revenant object that can appear in inventory carries a
small pre-rendered 2D icon baked into its `.i3d` art file (weapons show a
sword icon, potions a bottle, etc.). A character's icon is its **face/portrait**.
The HUD just blits that same baked icon. Nothing is rendered in 3D at HUD time.

**Visibility predicate:** the whole player side runs only if
`DAT_00667fcc != 0` (a Player exists) — `helper_PlayerSide_54a0a0.cpp:13`. The
portrait blit additionally runs only if `InventoryImage()` returned non-null —
`:17` (`if (iVar1 != 0)`). Same on the target side
(`helper_TargetSide_54a310.cpp:19-21`).

---

## §2 — Asset roster

The portrait is **not a named bitmap asset.** It is binary icon data embedded in
the character's `.i3d` imagery body (inside `data/imagery.rvi`), keyed by object
state.

| asset | archive / source | entry | WxH | format | role | cite |
|---|---|---|---|---|---|---|
| character portrait | `data/imagery.rvi` → the char's `.i3d` body | `S3DStateImagery[state].invitem` (inline `OTBitmap`) | **UNCONFIRMED px** (≤ Ring 44×44 visible; see §14-1) | **RGB555** (`BM_15BIT`), converted to RGB565 on load | the head image blitted into the Ring | `src/3dimagebody.h:414-420`; `src/3dimage.cpp:349-360`; format §4 |
| `Ring` | `statusbarnotex.dat` | `Ring` | 44×44 | RGB555 + alpha | gold frame stamped OVER the portrait | `TPlyrStatusBar_SPEC.md:181`; PlayerSide `:21,30` |
| `LockeFace` (HARNESS PLACEHOLDER ONLY) | `portraits.dat` | `LockeFace` | 30×30 | — | stand-in used by `uiplyrstatusbartest` until the real `.i3d` icon is wired; **do NOT treat as the asset** | `TPlyrStatusBar_SPEC.md:183`; `uiplyrstatusbartest.cpp:199` |

There is **no atlas** — `invitem` is a single self-contained bitmap (its own
`TBitmapData` header: width@0, height@4, flags@0x10, keycolor@0x18, …
`src/bitmapdata.h:38-45`).

---

## §3 — Coordinate frames & surfaces

The portrait occupies the **chrome/surface frame** of the chip (the `+0x6c`
mosaic the side helper composes into), at its top-left. The chip/chrome frame
graph is the same one already established in `TPlyrStatusBar_SPEC.md §3`; the
portrait adds no new frame, it just sits at chrome-local `(0,0)`.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | — (root) | — | framebuffer TL `(0,0)`; `pane_w = this+0xc` (live) | final pixels | `TPlyrStatusBar_SPEC.md:267` |
| **player-chip** | screen | TL | screen `0` | the player card | NOMENCLATURE §1 |
| **target-chip** | screen | TR | screen `pane_w − 0xc1` (193) | the target card | NOMENCLATURE §1 |
| **player-chrome surface** (`+0x6c`) | player-chip | TL | chip-local `0` → screen `0` | portrait, BackPanel, Ring, icons | PlayerSide `:14,18,25,30-33` |
| **target-chrome surface** (`+0x6c`) | target-chip | TL | **chip-local `0x41` (= `0xc1 − 0x80`)** | portrait, BackPanel, Ring, icons | TargetSide `:18,21`; `TPlyrStatusBar_SPEC.md:272` |
| **portrait bitmap (invitem)** | chrome surface | TL | chrome-local `(0,0)` | the head image pixels | PlayerSide `:18`; TargetSide `:21` |

**Composition to screen:**
- Player portrait TL: `screen = 0 (chip) + 0 (chrome) + (0,0) = (0,0)`.
- Target portrait TL: `screen = (pane_w − 0xc1) + 0x41 + (0,0) = (pane_w − 0x80, 0)`.
  (`0xc1 − 0x41 = 0x80`; matches the BackPanel screen origin in
  `TPlyrStatusBar_SPEC.md:363`.)

> The portrait is drawn into the **chrome scratch surface first** (stage-a),
> then that surface is blitted to the live display surface (`+0x70` player /
> `+0x74` target) via `(*+0x5c)` (stage-b). The portrait's *final* on-screen
> position is therefore whatever the chrome surface's stage-b blit lands at — but
> because the portrait sits at chrome-local `(0,0)` and the chrome surface is
> blitted to its chip with the offsets above, the composed result is the
> `(0,0)` / `(pane_w−0x80,0)` values above. (Two-stage blit, NOMENCLATURE §3;
> the chrome→display blit is PlayerSide `:34`, TargetSide `:37` per the parent
> spec.)

### Layout diagram (player chip; portrait + ring)

```
chrome surface (+0x6c), chrome-local origin = chip-local 0 (player)
0                                    BackPanel 128 wide
┌─────────────────────────────────────────────┐
│ (0,0) portrait (invitem) blitted OPAQUE      │   ← stage 1 (PlayerSide :18)
│   ┌───────────┐                              │
│   │  head     │   Ring 44x44 centered on     │   ← Ring stamped OVER it,
│   │  pixels   │   chrome anchor (0x1a,0x1f)  │      ALPHA (PlayerSide :30)
│   └───────────┘   (TL ≈ 0x1a-w/2, 0x1f-h/2) │
│                       icons at x=0x2b (HP/MP/FT, alpha)
└─────────────────────────────────────────────┘
```

The portrait fills behind the Ring; only the disc the Ring encloses is seen as
"the face." The Ring (44×44) is the effective visible aperture.

### Surfaces

| surface | size | role | cite |
|---|---|---|---|
| `this+0x6c` chrome mosaic | BackPanel-sized (128×64) | the scratch the portrait+chrome+ring+icons compose into | PlayerSide `:14`; `TPlyrStatusBar_SPEC.md:298-304` |
| `this+0x60` | `0x28×0x28` (40×40) | small scratch; the portrait corner is committed here when `iVar1!=0` via `(*+0x5c)` (PlayerSide `:27-28`) | `TPlyrStatusBar_SPEC.md:304` |
| `invitem` bitmap | the icon's own WxH | the source the portrait blit reads | §2 |

> The `this+0x60` commit at PlayerSide `:26-28` copies the freshly-stamped
> portrait region out of `+0x6c` into the `+0x60` cache, so a later partial
> redraw can restore the portrait without re-fetching the icon. The port can
> ignore this micro-cache and just re-blit the icon (it is cheap).

---

## §4 — Static element layout

| element | space | player (x,y) | size | target (x,y) | source rect | cite |
|---|---|---|---|---|---|---|
| portrait (invitem) | chrome-local → composed screen | `(0,0)` → screen `(0,0)` | invitem WxH | chrome-local `(0,0)` → screen `(pane_w−0x80, 0)` | full bitmap `(0,0,w,h)` | PlayerSide `:18`; TargetSide `:21`; §3 composition |
| Ring (frames it) | chrome-local | center `(0x1a,0x1f)` → TL `(0x1a−22, 0x1f−22)` | 44×44 | center `(0x62,0x1f)` | full | PlayerSide `:30`; `uiplyrstatusbartest.cpp:80-82` |

**Drawmode:** portrait blit uses `drawmode = 0x80000000 = DM_USEDEFAULT`
(opaque copy) — `FUN_004bd680(0,0,iVar1,0x80000000,0)` (PlayerSide `:18`,
TargetSide `:21`). The portrait is drawn **first/opaque**; the Ring on top is
`0x2000 = DM_ALPHA`. (Drawmode decode: UI_METHOD_MAP §1.)

> Why opaque is fine: the portrait is the first thing stamped into the freshly
> `Box`-cleared chrome surface (PlayerSide `:15` clears `+0x6c` with
> `(*+0x64)(0,0,w,h,0,0xffff,0x7f7f,0x80000000)`), so an opaque copy simply lays
> down the head; everything else composes over it.

---

## §5 — Draw order / composition

From `helper_PlayerSide_54a0a0.cpp` (target side mirrors at `54a310`):

1. `:15` **clear** chrome surface `+0x6c` to color 0 (`Box`/`(*+0x64)` opaque).
2. `:16` `iVar1 = Player->InventoryImage()` (`(*+0x130)()`, = `FUN_0046f190`).
3. `:17-19` **if `iVar1 != 0`: blit portrait OPAQUE** at chrome-local `(0,0)`
   (`FUN_004bd680(0,0,iVar1,0x80000000,0)`).
4. `:20-24` resolve named sprites: `BackPanel`, `Ring`, `HealthIcon`,
   `ManaIcon`, `FatigueIcon` (`FUN_0046d710`).
5. `:25` blit `BackPanel` OPAQUE at `(0,0)` (the chrome panel art).
6. `:26-29` if portrait present, commit it into the `+0x60` cache via `(*+0x5c)`.
7. `:30` blit `Ring` ALPHA centered.
8. `:31-33` blit the three stat icons ALPHA.
9. `:34-35` blit chrome surface `+0x6c` → display surface `+0x70` via `(*+0x5c)`.

> Note the portrait is laid down **before** `BackPanel` (step 3 before step 5).
> `BackPanel` is the chrome ring/panel art with a transparent center aperture;
> stamping it opaque after the portrait would erase the head — so retail relies
> on `BackPanel` having an alpha/keyed center, OR the portrait region being
> outside the BackPanel's opaque area. **The visible portrait is the union of
> "portrait pixels not covered by opaque BackPanel pixels," then framed by the
> Ring.** (The exact BackPanel center transparency is a §11/§14 detail — for the
> port, draw portrait → BackPanel(keyed) → Ring → icons in that order and the
> compositing handles it.)

---

## §6 — Algorithms (the portrait getter chain)

### 6a. `TObjectInstance::InventoryImage` = `FUN_0046f190` (the `+0x130` body)

```
// param_1 = the object (Player). this+0x54 = imagery; this+0xc = state (u16)
InventoryImage(obj):
    img = obj[0x54]                          // imagery ptr
    if img == null: return 0
    state = obj[0xc]                          // u16 object state
    bm = img->vtable[0xd4](state, 0)          // GetInvImage(state, 0)
    if bm != 0: return bm
    anim = img->vtable[0xd8](state)           // GetInvAnimation(state) fallback
    if anim != 0 and anim[0x38] != 0:         // anim has frame data
        return anim[0x38] + 0x38 + anim       // resolve first frame's inline bitmap
    return 0
cite: /tmp/base_46f190.c (this session); offsets src/3dimagebody.h, bitmapdata.h
```

- The `anim[0x38] + 0x38 + anim` arithmetic is the standard **offset-relative
  resolve** of an inline bitmap inside an animation frame (an `OFFSET` field +
  base). For a static portrait the `GetInvImage` branch returns first and the
  anim branch is unused.
- Port equivalent (already exists, identical semantics):
  `TObjectInstance::InventoryImage()` = `imagery ? imagery->GetInvImage(GetState()) : nullptr`
  (`src/object.h:1066`). The port folds the anim fallback into `GetInvImage`'s
  callers; if a portrait is animated, `GetInvAnimation` is the path
  (`src/imagery.h:382` family).

### 6b. imagery `GetInvImage` = `FUN_0040ce60` (vtable +0xd4)

```
GetInvImage(img, state):
    if not loaded: lazy-load the .i3d body (InitializeMesh)   // img[3]==0 branch
    icons = img[0x27]                          // == byte offset 0x9c: S3DImageryIcons*
    if icons == 0 or state < 0 or state > NumStates(): return 0
    bm = icons[state*2]                         // == icons[state].invitem
    if bm == 0: bm = icons[0]                   // fallback to state-0 icon
    if bm == 0: return 0
    // patch palette/key once (flags &6, field+0x18) then:
    return bm
cite: /tmp/imagery_d4_getinvimage.c; struct src/3dimage.h:90-95
```

Port equivalent (identical): `T3DImagery::GetInvImage(state)` →
`icons[state].invitem` (`src/3dimage.cpp:2005-2014`). `S3DImageryIcons =
{ TBitmap* invitem; TAnimation* invanim; }` (8 bytes → the `*2`/`*8` indexing).

### 6c. how `invitem` exists at all (load-time bake-in, NOT runtime render)

```
T3DImagery::InitializeMesh():                     // src/3dimage.cpp:339-373
    if (body.flags & I3D_HASICONS):               // I3D_HASICONS = 2
        for state in 0..NumStates():
            im = body.imagery[state]              // S3DStateImagery
            if im.invsize > 0:
                icon = alloc(im.invsize)
                if im.invitem:                     // an OTBitmap blob in the .i3d
                    memcpy(icon, im.invitem, im.invsize)
                    icons[state].invitem = icon
                    Convert15to16(icon)            // RGB555 -> RGB565 in place
                elif im.invanim:                   // animated portrait variant
                    memcpy(icon, im.invanim, im.invsize)
                    icons[state].invanim = icon
                    for each frame: Convert15to16(frame.bitmap)
cite: src/3dimage.cpp:339-373; flags src/3dimagebody.h:403; struct :414-420
```

**This is the whole story:** the portrait pixels ship inside the `.i3d` file as a
baked `OTBitmap` (offset-relative TBitmap = header + pixels), one per state,
gated by the `I3D_HASICONS` flag. No camera, no 3D draw, no render-to-surface.

---

## §7 — Effects & shadows

None on the portrait itself. The portrait blit is a plain opaque copy
(`DM_USEDEFAULT`). No drop shadow, no glow, no tint. The Ring on top carries its
own alpha; the icons carry alpha. (Shadows in this panel are on the **bars** and
**text**, not the portrait — see `TPlyrStatusBar_SPEC.md §7`.)

---

## §8 — Text rendering

N/A to the portrait. The character **name/level** label is a separate text
element under the ring, fully specified in `TPlyrStatusBar_SPEC.md §8` (it is not
part of the portrait image).

---

## §9 — Animation & dynamic behavior

- **Static by default.** A portrait is a fixed baked bitmap for the object's
  current *state*. It changes only when the object's **state** changes
  (`obj[0xc]`), because `GetInvImage(state)` indexes by state. Most characters
  have a single relevant state → the HUD portrait is effectively static.
- **Animated portrait (rare, supported):** if the `.i3d` provides `invanim`
  instead of `invitem` for a state, the portrait is an animation
  (`S3DImageryIcons.invanim`, resolved by `GetInvAnimation` /
  `FUN_0040cef0`). The base `+0x130` only returns the **first frame**
  (`anim[0x38]+0x38+anim`, §6a); to actually animate you'd call
  `GetInvAnimation` and step frames — but the HUD `+0x130` path uses the static
  first frame. No evidence the player portrait is animated.
- **Redraw model:** the side helper re-runs (re-fetch + re-blit the portrait)
  when the chip is rebuilt — i.e. when the bound character changes (player set,
  target acquired/lost). The `+0x60` cache (§3) lets partial redraws restore the
  portrait without re-fetching. The port should re-fetch `InventoryImage()` when
  the bound `oi` (or its state) changes; cache otherwise (use a version counter
  on `{oi, state}` per [[feedback-versions-over-flags]]).

No tween blocks apply.

---

## §10 — Input & dispatch

The portrait is non-interactive (it is inside the status-bar chip, which has no
hit rects of its own beyond the panel). N/A.

---

## §11 — Retail bugs NOT to reproduce

- **Magenta key vs alpha.** `invitem` is RGB555 with magenta (`0x7C1F`) as the
  implicit transparency key (UI_METHOD_MAP §16); the port converts to RGB565 on
  load. The portrait is blitted **opaque** (`DM_USEDEFAULT`) into a cleared
  scratch, so any magenta border pixels become solid magenta in the scratch and
  are only hidden because the BackPanel/Ring cover the chip edges. **Intended
  visual:** the head disc inside the Ring; do not let stray magenta show. If a
  magenta fringe appears around the head in the port, blit the portrait with the
  chroma key honored (or rely on the Ring aperture) — same fix family as the
  pink-halo guidance ([[project-retail-pink-halo-bug]]), but the portrait is art
  not text so there's no 3-pass shadow involved.
- **Opaque-portrait-then-opaque-BackPanel ordering** (§5): correct only because
  BackPanel's center is transparent/keyed. Reproduce the *order* (portrait →
  BackPanel → Ring → icons) and ensure BackPanel's center alpha is respected;
  don't "fix" it by reordering.

---

## §12 — Reconstruction pseudocode

```
// Compose the portrait into the chip render target (the §3 chrome frame).
// Called when (re)building a side chip; oi = the bound character (Player or target).

void DrawPortrait(TSurface* chipRT, TObjectInstance* oi, int chromeX, int chromeY)
{
    if (!oi) return;
    PTBitmap face = oi->InventoryImage();        // == retail (*oi+0x130)(); src/object.h:1066
    if (!face) return;                            // character has no baked icon -> no portrait
    // OPAQUE copy at the chrome top-left (final composed (chromeX,chromeY) per §3)
    Renderer->DrawBitmapToTarget(chipRT, face, chromeX + 0, chromeY + 0);
    // (then the caller stamps BackPanel(keyed), Ring(alpha, centered), icons(alpha))
}
```

- `chromeX/chromeY` are the chrome-frame origin in the RT: player `(0,0)`;
  target the mirrored origin (see `uiplyrstatusbartest.cpp` DrawSide / §3).
- `InventoryImage()` already does the full retail `+0x130` chain (§6). Do NOT
  add a 3D render — there is none.
- Replace the harness `LockeFace` placeholder
  (`uiplyrstatusbartest.cpp:199`) with `oi->InventoryImage()`.

---

## §13 — Port mapping notes

| retail call | what it does | canonical port primitive | home |
|---|---|---|---|
| `(*Player + 0x130)()` (`FUN_0046f190`) | get portrait bitmap for current state | `oi->InventoryImage()` (already = `imagery->GetInvImage(GetState())`) | `src/object.h:1066` |
| imagery `(*+0xd4)(state,0)` (`FUN_0040ce60`) | `icons[state].invitem` | `T3DImagery::GetInvImage(state)` | `src/3dimage.cpp:2005` |
| imagery `(*+0xd8)(state)` (`FUN_0040cef0`) | `icons[state].invanim` (animated portrait) | `T3DImagery::GetInvAnimation(state)` | `src/3dimage.cpp:2016` |
| baked-icon load (`I3D_HASICONS`) | parse `.i3d` `invitem`/`invanim`, RGB555→565 | `T3DImagery::InitializeMesh()` icons block | `src/3dimage.cpp:339-373` |
| `FUN_004bd680(0,0,face,0x80000000,0)` | opaque blit portrait at (0,0) | `Renderer->DrawBitmapToTarget(rt, face, x, y)` | renderer (UI_METHOD_MAP §12) |

**No new primitive needed.** The entire portrait path already exists in the port.
The reconstruction work is purely **wiring**: in the status-bar side composite,
fetch `oi->InventoryImage()` and `DrawBitmapToTarget` it at the chrome origin,
ordered before BackPanel — replacing the `LockeFace` placeholder.

**The `testmodes.cpp` live-3D char-preview (`SCharPreviewState`,
`RebuildCharPreviewForRosterIndex`, `RenderCharPreviewMode`,
`SampleI3DAnimPose`) is NOT the portrait path** — it renders the full 3D mesh
and is the right tool only IF a future "Revisited" mode wants live-rendered
portraits (a deviation from retail). For Classic-faithful HUD, use the baked
icon (`InventoryImage`). Keep the two clearly separate.

---

## §14 — Paperdoll overlap (TEquipPane)

The character pane's "paperdoll" uses the **same baked-icon mechanism**, not a
live render either:

- **Body figure = static background bitmap.** `TEquipPane::DrawBackground`
  blits a full-pane background (`top` for SECT_TOP, `bottom` for SECT_BOTTOM) via
  `Display.Put(0,0, section==SECT_TOP?top:bottom, DM_BACKGROUND)`
  (`src/equip.cpp:140`). The drawn character body is **painted into that
  background art** — there is no per-frame 3D figure render.
- **Equipped items = per-item baked icons** overlaid on the body. For each of
  `NUM_EQ_SLOTS`, `Player->GetEquip(i)->DrawInvItem(EquipLoc[section][i].x, .y)`
  (`src/equip.cpp:144-147`). `DrawInvItem` → `imagery->DrawInvItem` →
  `Display.Put(x, y, oi->InventoryImage(), DM_TRANSPARENT|DM_BACKGROUND)`
  (`src/imagery.cpp:858-869`) — the identical `InventoryImage()`/`GetInvImage`
  path as the HUD portrait, but blitted **transparent** (keyed) so the item icon
  composites over the body art.
- **Drag bitmap** while dragging an item is also `GetEquip(slot)->InventoryImage()`
  (`src/equip.cpp:65-68`).

So the paperdoll is "static body backdrop + transparent item-icon overlays."
The only render-to-surface difference from the HUD portrait is the **drawmode**
(`DM_TRANSPARENT|DM_BACKGROUND` for paperdoll items vs `DM_USEDEFAULT` opaque for
the HUD portrait), because paperdoll items must show the body through their
transparent borders, whereas the HUD portrait is the bottom layer.

---

## §14b — UNCONFIRMED / open questions

1. **Does the shipped *player* character `.i3d` actually carry an `invitem`, and
   at what dimensions?** — The render path is fully confirmed, but the *data*
   (the baked head pixels) lives in `data/imagery.rvi` (binary, packed). I could
   not measure it (no `.i3d` icon extractor in `tools/`). **Impact if wrong:**
   if the player's `.i3d` has no icon, `InventoryImage()` returns null and the
   HUD shows no portrait (the `if (iVar1 != 0)` guard, PlayerSide `:17`) — the
   Classic screenshot (`docs/ui/CLASSIC_HUD_REFERENCE.md:56`, "Locke's face
   shown") proves an icon DOES exist for the player in the shipped game, so
   expect one. **Resolve:** extract the player char's `.i3d` body from
   `imagery.rvi`, check `flags & I3D_HASICONS`, read `S3DStateImagery.invitem`
   header (`width`@0, `height`@4); or runtime-log `oi->InventoryImage()` +
   `bm->width/height` in a `--test` that spawns the player. (The Ring is 44×44,
   so the visible aperture is ≤ 44px; the baked icon is likely ~44×44 or smaller,
   but **do not assume** — measure.)

2. **TPlayer's *real* class id / hierarchy depth.** Confirmed TPlayer's vtable is
   `0x5b4f30` (typed `DAT_00667fcc` refs) and its `+0x130` is the base method.
   Not confirmed: whether TPlayer derives from TCharacter (whose `+0x130` is the
   stub `0x487c30`) or directly from TObjectInstance — TPlayer's `+0x130` points
   to the *base* (`0x46f190`), which means **whatever the chain, TPlayer's slot
   resolves to the base getter**, so this does not affect the portrait. **Impact
   if wrong:** none for the portrait. **Resolve:** dump the ctor that sets the
   TPlayer vftptr if hierarchy clarity is wanted elsewhere.

3. **BackPanel center transparency.** §5/§11 assume `BackPanel` has a
   transparent/keyed center so the opaque portrait (drawn first) shows through.
   Not measured here. **Impact if wrong:** wrong draw order could erase the head.
   **Resolve:** dump `statusbarnotex.dat:BackPanel` with `tools/ui/dump_dat.py`
   and inspect its center alpha/key region (or just verify visually after
   wiring; the order portrait→BackPanel→Ring is what retail does regardless).

4. **Animated portrait in retail.** The `invanim` branch (§6a/§9) exists but I
   found no evidence the player portrait uses it. **Impact if wrong:** a
   one-frame static portrait where retail showed a subtle idle animation.
   **Resolve:** after #1, check whether the player state's icon entry is
   `invitem` (static) or `invanim` (animated).
