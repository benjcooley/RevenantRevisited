# MouseCursor — TPlayScreen-owned cursor layer (engine-level)

## §0 — Sources & status

- **Target:** the engine-level mouse cursor layer that TPlayScreen wires up
  during `Initialize` (`cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:250-258`).
  Composed of:
  - the default `cursor` sprite (stored at `DAT_0065a28c`),
  - the `handcursor` sprite (stored at `_DAT_0065a284`),
  - a `cursorshadow` sprite used by the down-right drop shadow,
  - a 5-of-7 set of corner-overlay sprites (hand/eye/mouth/door/swords —
    stairs/hourglass are referenced by name but the bitmaps are NOT shipped in
    playscrn.dat; see §11).
- **NOT a TPane subclass.** No class id — this is a set of free-functions
  (`FUN_0043a020` family at `.text` `0x43a020..0x43a930`) acting on module-level
  globals (`0x6563a0..0x6563d4`). Confirmed: `cls_0x5a5320_TPlayScreen_Initialize`
  calls only `FUN_0043a020(cursor)` to install the initial cursor; no pane is
  created or owned.
- **Class identification:** the 1998 pre-release `src/cursor.{h,cpp}`
  (`RevenantRevisited/legacy/cursor.cpp:1-195`) is the **near-1:1 source** of
  the retail layer. Every retail function maps to a snapshot function by
  signature + side-effect (verified function-by-function in §6 by reading the
  disassembly at `0x43a020..0x43a930` against the snapshot src). This panel is
  therefore in the **retail-confirmed** state via snapshot fingerprint
  (per [[reference-revsync-convention]]) — the snapshot version of cursor.cpp
  IS the shipped retail behavior, modulo Win32-only RestrictCursor/ReleaseCursor
  which we leave stubbed (see §13).
- **Recon files read:**
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:250-258`
    (Initialize)
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Close_47b290.cpp:32`
    (Close calls `FUN_0043a020(0)`)
  - `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:15-16`
    (DeathScreen installs its own cursor)
  - `recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp:26-27`
    (LogoScreen installs its own cursor)
  - `recon/ghidra/_data.txt:70921-70946,126725-126810,131253-131256,132028-132691`
    (string literals + global addresses + MAPPANE globals + Windowed flag)
  - `recon/ghidra/cls_0x5a4358_TConsolePane.cpp:420-438` (Ghidra-merged region
    helpers that DO/UNDO the default cursor — see §10)
  - `recon/classes_original/cls_0x5a5320_TCharacter.cpp:7807` (per-frame
    `DrawMouseShadow()` call from PlayScreen frame logic)
  - `recon/classes_original/cls_0x5a5ed4_likely_ObjectCollection.cpp:1112`
    (TScreen base draw method calls `DrawMouseCursor()` at end of frame)
  - `recon/classes_original/cls_0x5a5320_TCharacter.cpp:910-924` (a
    `SetMouseShadow(shadow, 0, 0x2b)` site = drag shadow setup, see §6.4)
  - `recon/classes_original/cls_0x5a58c0.cpp:602-668` (Inventory drag use
    of `SetDragBitmap`/`SetDragObj`)
  - `recon/classes_original/cls_0x5a5750.cpp:405-406`,
    `cls_0x5a5ba0.cpp:276-277`, `cls_0x5a4494.cpp:595` (ClearDrag/Drop sites)
  - `RevenantRevisited/legacy/cursor.cpp:1-195` (snapshot src — the
    paragraph below permits using it as a structural mirror; coordinate values
    nonetheless cited to retail disassembly).
  - `RevenantRevisited/legacy/RevDefs.h:360-370` (the CURSOR_HAND..CURSOR_SWORDS
    type enum, `NUMCURSORTYPES=7`).
- **Disassembly:** `objdump -d -M intel --start-address=0x43a020
  --stop-address=0x43a930 data/Revenant.exe` (every function body in §6 cited
  to specific bytes in this dump).
- **Asset dump:** `python3 tools/ui/dump_dat.py /tmp/playscrn.dat
  --out-dir /tmp/playscrn` after `unzip resources.rvr playscrn.dat`.
- **Status:** `forensics-complete` for all behavior the engine layer owns
  (init, set, drag, draw, shadow). **forensics-partial** on the cursor-type
  matrix at the edges — stairs/hourglass bitmaps are not in playscrn.dat (the
  lookup returns NULL at runtime; see §11), and the per-object `CursorType()`
  return-value matrix is owned by each object class (out of scope here — this
  spec only documents how the cursor layer consumes the int).
- **Errors to close in the existing port** (`src/cursor.{h,cpp}`):
  1. `RestrictCursor`/`ReleaseCursor` are Win32-stubbed (`#if 0`). Retail
     uses `ClipCursor`/`GetClientRect`/`SetCursorPos`/`ClientToScreen`
     (`0x5a3274/80/84/88`). Port-side path TBD ([[project-target-platform]]),
     but the spec MUST state the retail behavior (§6.10) so the implementor can
     decide.
  2. The existing port's `SetMouseBitmap` clears MouseShadow only via the
     commented-out `//MouseShadow = nullptr` (retail keeps shadow if cursor !=
     "cursor"). Snapshot src matches retail (preserves shadow). Verified at
     `0x43a020..0x43a06a` — retail does NOT clear MouseShadow when
     cursor != GameData->Bitmap("cursor"); it only clears `MouseCursorAdd` and
     sets `priority=1`. The port's behavior is correct; mention is a no-op.
  3. The existing port's TCursorHud composites via `Renderer->DrawBitmap` in
     the swapchain pass — correct per §5 draw order. No fix needed.
- **Snapshot note (supplementary — superseded by retail):**
  `RevenantRevisited/legacy/cursor.{h,cpp}` is a near-1:1 match for retail.
  Both the FUNCTION SHAPE and the SIDE-EFFECTS (which global gets written)
  match the disassembly verbatim. The only divergence is that retail's
  defaults for `SetMouseShadow` come from the inline call inside SetMouseBitmap
  (`offsetx=1, offsety=3`) — the same defaults are encoded in
  `legacy/cursor.h:19`. So snapshot serves as a **readable C++ mirror of
  retail behavior** for this layer (algorithm intent + field/global names),
  but every coordinate / drawmode / global address in this spec is cited to
  the disassembly, not the snapshot.

---

## §1 — Overview

The cursor layer is a **screen-wide, render-last sprite layer** owned by
TPlayScreen (and re-owned by every other TScreen: TLogoScreen, TDeathScreen,
TConsolePane). It composites three sprite slots on top of the HUD on every
frame:

1. **DragBitmap** — an item the user is dragging out of an inventory slot
   (e.g. equipped sword being moved to the bag). Anchored to
   `(cursorx - grabx, cursory - graby)`. Set by inventory/equip/spell panes
   via `SetDragBitmap(bm, grabx, graby)` while the user holds an item; cleared
   one frame later via `cleardragbitmap` after the draw.
2. **MouseCursor** — the active cursor sprite (default = `cursor` 14×20;
   per-screen can install `handcursor` 16×20 or any other). Anchored to
   `(cursorx, cursory)`.
3. **MouseCursorAdd** — a small corner-overlay sprite (hand/eye/mouth/door/
   swords) added when the cursor hovers over an interactable object; suppresses
   MouseCursor rendering and is drawn in its place. Reset to NULL at the end of
   every frame. Set per-frame by `CursorOverObject(oi)`, which calls the
   object's `oi->vtable+0xc0` (`CursorType()`) — the int return is indexed into
   the static `CursorTypes[7]` name table (`0x5ce648`) to fetch the sprite by
   name.

In parallel, a **soft drop shadow** (`cursorshadow` 14×20) is drawn ONCE
PER FRAME BEFORE the panes, clipped to the screen regions OUTSIDE the 3D
map pane (snapshot intent: shadow visible over HUD chrome only, not over the
3D scene). When the active shadow is something OTHER than `cursorshadow` (the
drag-icon shadow set at `0,+0x2b`), the shadow is clipped to the MAPPANE only.

**Visibility:** the cursor layer is "always on" while a TScreen is active and
its `DAT_006682bc == 0` (no completed-action gate) (`cls_0x5a5ed4:1106`).
Both PlayScreen and the post-game screens own one. There is no mouse-hide
predicate in the retail engine — the cursor is always visible as long as the
sprite handle is non-NULL. The port's `g_os_cursor_owns_pixels` predicate and
`ImGui::WantCaptureMouse` are macOS-port additions, not retail behavior.

---

## §2 — Asset roster

All sprite assets live in **`playscrn.dat`** (resources.rvr). Magenta key
`0x7C1F` (RGB555) is the chroma key per [[project-retail-pink-halo-bug]] and
UI_METHOD_MAP §16. Bitmaps are 15-bit (`flags=0x2`).

| asset            | archive       | entry name      | WxH   | role                                           | source rect | cite                                                       |
|------------------|---------------|-----------------|-------|------------------------------------------------|-------------|------------------------------------------------------------|
| cursor           | playscrn.dat  | `cursor`        | 14×20 | default mouse cursor                           | full bitmap | `_data.txt:86468` (literal), `tools/ui/dump_dat.py` measured |
| cursorshadow     | playscrn.dat  | `cursorshadow`  | 14×20 | soft drop shadow under default cursor          | full bitmap | `_data.txt:70927, 70940`                                   |
| handcursor       | playscrn.dat  | `handcursor`    | 16×20 | "grab/use" cursor pose (inv drag, hover)        | full bitmap | `_data.txt:86473`                                          |
| hand             | playscrn.dat  | `hand`          | 20×21 | CURSOR_HAND (0) corner overlay                 | full bitmap | `_data.txt:70887` `null_00657965 = "eye"`/`hand` table; dump |
| eye              | playscrn.dat  | `eye`           | 20×21 | CURSOR_EYE (1)                                 | full bitmap | dump                                                       |
| mouth            | playscrn.dat  | `mouth`         | 20×21 | CURSOR_MOUTH (2)                               | full bitmap | `_data.txt:70890`                                          |
| door             | playscrn.dat  | `door`          | 20×21 | CURSOR_DOOR (3)                                | full bitmap | `_data.txt:70894-70898` `"door"`                           |
| stairs           | **MISSING**   | `stairs` (ref)  | n/a   | CURSOR_STAIRS (4) — bitmap not shipped         | n/a         | name at `_data.txt:70903`; dump shows no `stairs` entry    |
| hourglass        | **MISSING**   | `hourglass`(ref)| n/a   | CURSOR_HOURGLASS (5) — bitmap not shipped      | n/a         | name at `_data.txt:70908`; dump shows no `hourglass` entry |
| swords           | playscrn.dat  | `swords`        | 20×21 | CURSOR_SWORDS (6)                              | full bitmap | `_data.txt:70913`                                          |
| (per-screen)     | per-screen .dat (death.dat / menus.dat / …) | `cursor` | varies | each screen calls `FUN_0046d710("cursor")` against its own GameData; the global stack resolves the name from the active mounted archive | full | `cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:15`; `cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp:26-27` |

Important: only PlayScreen mounts `handcursor`. DeathScreen and LogoScreen
only install the default `cursor`. The `handcursor` sprite is **stored** by
TPlayScreen::Initialize but never installed by `SetMouseBitmap` in any
extracted recon (UNCONFIRMED-A: which subsystem calls
`SetMouseBitmap(handcursor)` — drag-pickup vs hover-on-grabbable — is not in
the recon set we read. See §14.).

The `CursorTypes` name table at `PTR_DAT_005ce648` is an array of 7 string
pointers (`_data.txt:70861-70875`):

```
PTR_DAT_005ce648[0] = "hand"       (-> 005ce664)
PTR_DAT_005ce648[1] = "eye"        (-> 00657965 / "null_00657965"; the 'eye'
                                    string lives in .rdata as 'eye\0' merged
                                    into a larger null pad)
PTR_DAT_005ce648[2] = "mouth"      (-> 005ce670)
PTR_DAT_005ce648[3] = "door"       (-> 005ce678)
PTR_DAT_005ce648[4] = "stairs"     (-> 005ce680)
PTR_DAT_005ce648[5] = "hourglass"  (-> 005ce688)
PTR_DAT_005ce648[6] = "swords"     (-> 005ce694)
```

This matches `legacy/cursor.cpp:36` `CursorTypes[NUMCURSORTYPES] = {"hand",
"eye", "mouth", "door", "stairs", "hourglass", "swords"}` exactly.

---

## §3 — Coordinate frames & surfaces

The cursor layer is **screen-anchored** (no chip model, no scratch surface,
no offscreen RT). Every coordinate it consumes is already in **screen** space:
`cursorx`/`cursory` are the global mouse position the input layer publishes
into `DAT_00668510 (cursorx)` and `DAT_00668514 (cursory)`
(`_data.txt:133673,133696`). The cursor draws DIRECTLY to the display
(`PTR_DAT_005d79e0` — the global TDisplay-like) via `FUN_004bd680` (the sprite
stamp helper, UI_METHOD_MAP §4).

### Frame table

| frame              | parent  | anchor | origin in parent (formula + literal)                                    | what is expressed in it             | cite                                       |
|--------------------|---------|--------|--------------------------------------------------------------------------|--------------------------------------|--------------------------------------------|
| screen             | (root)  | TL     | framebuffer top-left = `(0,0)`; WIDTH=`0x280=640`, HEIGHT=`0x1e0=480`   | every cursor draw position           | disasm `0x43a4b0 mov ecx, 0x280`, `0x43a4c8 mov eax, 0x1e0`; `0x43a64a,4f`; `RevDefs.h` |
| cursor-pos         | screen  | (point)| `(cursorx, cursory)` = `(DAT_00668510, DAT_00668514)`, clamped to `[1,WIDTH]`×`[1,HEIGHT]` (clamp at draw time, `0x43a48b-4cd`) | MouseCursor and MouseCursorAdd dst  | disasm `0x43a51b, 43a523`; cf. `_data.txt:133673,133696` |
| drag-pos           | screen  | (point)| `(cursorx - grabx, cursory - graby)` where `grabx=DAT_006563a8`, `graby=DAT_006563ac` | DragBitmap dst                       | disasm `0x43a4e6-4f6, 43a4ec sub`; globals at `_data.txt:126725,126732` |
| shadow-pos         | screen  | (point)| `(cursorx + shadowoffsetx, cursory + shadowoffsety)` where `shadowoffsetx=DAT_006563bc` (default 1), `shadowoffsety=DAT_006563c0` (default 3) | MouseShadow dst                      | disasm `0x43a45f-46b add ecx,eax; add edx,ecx`; globals at `_data.txt:126766,126777` |
| MAPPANE-rect       | screen  | TL     | `(MAPPANEX, MAPPANEY)` = `(DAT_006663d8, DAT_006663d4)`, size `(MAPPANEWIDTH=DAT_00667c30, MAPPANEHEIGHT=DAT_0065c5c4)` | the 3D map viewport (the cursor shadow is clipped to OUTSIDE this) | disasm `0x43a5e5, 5ea, 5ed, 5f2` (push order); `_data.txt:132028-132691` |
| OS-window-rect     | desktop | TL     | `GetClientRect(MainWnd, &r); ClientToScreen(MainWnd, &r); right = r.left+WIDTH; bottom = r.top+HEIGHT` | the desktop rect ClipCursor restricts to (when !Windowed||Borderless) | disasm `0x43a898 GetClientRect call, 43a8a9 ClientToScreen, 43a8db add 0x280; 43a8e1 add 0x1e0; 43a8ef ClipCursor` |

**Composition (final draw positions):**

```
DragBitmap_dst   = (cursorx - grabx,            cursory - graby)
MouseCursor_dst  = (clamp(cursorx,1,640),       clamp(cursory,1,480))
MouseCursorAdd_dst = (clamp(cursorx,1,640),     clamp(cursory,1,480))
MouseShadow_dst  = (cursorx + shadowoffsetx,    cursory + shadowoffsety)
```

The DM_USEREG flag (low byte `0x100` in the `0x21100 / 0x20100` drawmode)
biases the dst position by the source bitmap's per-pixel registration point
`(bm->regx, bm->regy)` inside the surface's `Put` (legacy `bitmap.h`,
verified in port shell). So the **true tip position** of the cursor sprite is
`(cursorx - bm->regx, cursory - bm->regy)` — the spec's `cursorx,cursory` is
the **hot-spot anchor**, not the bitmap top-left. The port's `TCursorHud::Draw`
already uses this contract via `Renderer->DrawBitmap` which respects regx/regy.

### Layout diagram

```
screen 640 × 480
┌─────────────────────────────────────────────────────────────────────────┐
│                                                                         │
│                                                                         │
│   ┌─────────────── MAPPANE (DAT_006663d8, DAT_006663d4) ────────────┐    │
│   │                          MAPPANE WxH = (DAT_00667c30,           │    │
│   │                                          DAT_0065c5c4)          │    │
│   │                                                                 │    │
│   │            ●(cursorx,cursory)                                   │    │
│   │           ◢(+1,+3) cursorshadow ← but CLIPPED OUT of MAPPANE    │    │
│   │                                                                 │    │
│   │                                                                 │    │
│   └─────────────────────────────────────────────────────────────────┘    │
│                                                                         │
│   ↑ cursorshadow only renders in this strip (outside MAPPANE)           │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

For drag-bitmap shadow (the `(0,+0x2b)` set, §6.4) the rule INVERTS: shadow
clips to MAPPANE ONLY, NOT to HUD chrome (`0x43a5d9-5fc`).

---

## §4 — Static element layout (table)

| element        | space  | (x,y)                                  | (w,h) | source rect | cite                                  |
|----------------|--------|----------------------------------------|-------|-------------|---------------------------------------|
| MouseCursor    | screen | (clamp(cursorx,1,640), clamp(cursory,1,480)) | 14×20 | full        | disasm `0x43a48b..4cd, 43a51b-32`     |
| MouseShadow    | screen | (cursorx+shadowoffsetx, cursory+shadowoffsety); default `(cursorx+1, cursory+3)` for `cursorshadow`; `(cursorx+0, cursory+0x2b=43)` for drag-shadow | 14×20 / drag-shadow size | full | disasm `0x43a45f-46b`; default-set at `0x43a055/05f` (`SetMouseBitmap` → `SetMouseShadow(shadow,1,3)`); drag-set `TCharacter:923` `SetMouseShadow(uVar3,0,0x2b)` |
| MouseCursorAdd | screen | (clamp(cursorx,1,640), clamp(cursory,1,480)) | 20×21 (hand/eye/mouth/door/swords) | full | disasm `0x43a542-55`                    |
| DragBitmap     | screen | (cursorx − grabx, cursory − graby)     | varies | full        | disasm `0x43a4dc-4fe`                 |

There is no mirrored side. Every cursor draw is at the global `(cursorx,
cursory)` — there is one cursor.

---

## §5 — Draw order / composition

Two distinct calls, called from DIFFERENT places in the frame:

1. **`DrawMouseShadow()` (no-arg)** — `FUN_0043a5a0`. Called from the TPlayScreen
   per-frame update path (`cls_0x5a5320_TCharacter.cpp:7807` — Ghidra-merged
   into the TCharacter file but the call site is the PlayScreen
   `virt_meth_0x491a80` flow). Called BEFORE pane draws so the shadow ends up
   UNDER the panes (the snapshot in `legacy/cursor.cpp:125-153` confirms this
   is the intended order).
2. **`DrawMouseCursor()` (no-arg)** — `FUN_0043a480`. Called from
   `TScreen::virt_meth_0x491870` (the screen base Draw method) at line `1112`
   of `cls_0x5a5ed4_likely_ObjectCollection.cpp` — AFTER all the panes' draw
   methods (`meth_0x48f340`, `0x4aa930` blit-effects flush, `0x48f450`,
   `0x48f560`, `0x48f680`). So the cursor sits ON TOP of everything in the HUD
   composition.

Within `DrawMouseCursor()` (decoded from disasm at `0x43a480..0x43a59a`), the
order is fixed:

| step | element        | drawmode literal | drawmode decoded                              | cite      |
|------|----------------|------------------|------------------------------------------------|-----------|
| 1    | ResetClipRect  | `(*+0x24)()`     | drop any clip set by the shadow pass           | `0x43a488`|
| 2    | clamp `cursorx` to `[1,640]` and `cursory` to `[1,480]`                                                      | `0x43a48b-4cd` |
| 3    | DragBitmap (if non-null) | `0x20100`        | `DM_TRANSPARENT | DM_USEREG`                  | `0x43a4e0`|
| 4    | MouseCursor (if non-null AND MouseCursorAdd is null) | `0x21100` | `DM_TRANSPARENT | DM_USEREG | DM_ALIAS` | `0x43a524`|
| 5    | MouseCursorAdd (if non-null) | `0x21100`        | `DM_TRANSPARENT | DM_USEREG | DM_ALIAS`        | `0x43a54d`|
| 6    | if `cleardragbitmap` (`DAT_006563d0`) → SetDragBitmap(0,0,0); zero DragBitmap/grabx/graby/cleardragbitmap | `0x43a560-587` |
| 7    | zero `MouseCursorAdd` and `priority` (drop hover state at end of frame)                                       | `0x43a58d-593` |

**Net composition:** drag bitmap underneath; either the main cursor OR the
corner overlay (XOR — when an interactable is hovered, the overlay REPLACES
the cursor) on top. The retail `DrawMouseCursor` does NOT draw both the cursor
and the corner overlay simultaneously — this is a one-or-the-other decision
made at `0x43a521`'s `jne 0x43a54c` (skip MouseCursor draw if Add is set).

**Drawmode constants** (from UI_METHOD_MAP §1 + `revdefs.h`):
- `0x00100 = DM_TRANSPARENT` — color-key magenta as alpha
- `0x20000 = DM_USEREG` — bias dst by bitmap's `(regx, regy)` (hot-spot anchor)
- `0x01000 = DM_ALIAS` — antialias edges via the bitmap's alias data
- `0x20100 = DM_TRANSPARENT | DM_USEREG` (drag bitmap; no AA)
- `0x21100 = DM_TRANSPARENT | DM_USEREG | DM_ALIAS` (cursor + add + shadow; AA on)

`DM_USEREG=0x20000` is NOT in UI_METHOD_MAP §1's drawmode table — needs to be
added (Gaps in §13). It's read from `legacy/RevDefs.h`/the snapshot calls; the
retail uses it consistently as `0x20000` per the literal hex sums above.

---

## §6 — Algorithms (pseudocode per helper)

The retail cursor module is 12 free functions at `.text 0x43a020..0x43a930`.
Each is small (5–110 bytes) and maps 1:1 to a snapshot function. Citations
are the disassembly addresses + the snapshot-src line for the C++ shape.

### Globals (cite `_data.txt:126725..0` and disasm)

| Global         | Snapshot name              | Role                                                                |
|----------------|-----------------------------|---------------------------------------------------------------------|
| `DAT_006563a0` | (internal sprite cache)    | held drag-bitmap render surface; reset by `0x43a3de` and `0x43a301` |
| `DAT_006563a4` | (internal drag dst surface)| dst surface ptr for drag bitmap composite; `0x43a263/0x43a35d`      |
| `DAT_006563a8` | `grabx`                    | DragBitmap registration x; `_data.txt:126725`                       |
| `DAT_006563ac` | `graby`                    | DragBitmap registration y; `_data.txt:126732`                       |
| `DAT_006563b0` | `priority`                 | bool: corner overlay set by toppriority caller; `_data.txt:126739`  |
| `DAT_006563b4` | `MouseCursor`              | active cursor TBitmap*; `_data.txt:126747`                          |
| `DAT_006563b8` | `MouseShadow`              | active shadow TBitmap*; `_data.txt:126752`                          |
| `DAT_006563bc` | `shadowoffsetx`            | per-shadow x-offset (default 1 for cursorshadow); `_data.txt:126766`|
| `DAT_006563c0` | `shadowoffsety`            | per-shadow y-offset (default 3 for cursorshadow); `_data.txt:126777`|
| `DAT_006563c4` | `MouseCursorAdd`           | corner-overlay TBitmap*; `_data.txt:126788`                         |
| `DAT_006563c8` | `DragBitmap`               | DragBitmap TBitmap*; `_data.txt:126797`                             |
| `DAT_006563cc` | `DragObj`                  | dragged TObjectInstance*; `_data.txt:126805`                        |
| `DAT_006563d0` | `cleardragbitmap`          | bool latch — clear DragBitmap after one more draw; `_data.txt:126810`|
| `DAT_0065a28c` | (PlayScreen "cursor" handle)| TPlayScreen's cached "cursor" lookup result; `_data.txt:129172`     |
| `DAT_0065a284` | (PlayScreen "handcursor" handle) | TPlayScreen's cached "handcursor"; `_data.txt:129166` (stored only) |
| `DAT_005ce640` | `oldcursorx` save          | cursor pos saved on ReleaseCursor; `_data.txt:70853-857`            |
| `DAT_005ce644` | `oldcursory` save          | "                                                                  |
| `DAT_00668510` | `cursorx`                  | live mouse x (input writes, draw reads); `_data.txt:133673`         |
| `DAT_00668514` | `cursory`                  | live mouse y                                                        |
| `DAT_00668190` | `Windowed`                 | bool; `_data.txt:133438`                                            |
| `DAT_006663d8` | `MAPPANEX`                 | MAPPANE x; `_data.txt:132040`                                       |
| `DAT_006663d4` | `MAPPANEY`                 | MAPPANE y; `_data.txt:132028`                                       |
| `DAT_00667c30` | `MAPPANEWIDTH`             | MAPPANE w; `_data.txt:132670`                                       |
| `DAT_0065c5c4` | `MAPPANEHEIGHT`            | MAPPANE h; `_data.txt:130952`                                       |
| `DAT_0065cb0c` | (active area index)        | InCompleteExclusion check input; `_data.txt:131253`                 |
| `DAT_0065cb1c` | (area-flags array base)    | flag table; `&0xf0` bit set → "complete exclusion"; `_data.txt:131256`|

### 6.1 `SetMouseBitmap(cursor)` — `FUN_0043a020` @ `0x43a020`

```
void SetMouseBitmap(PTBitmap cursor):                   // disasm 0x43a020-7e
    MouseCursor = cursor                                 // 0x43a030: [0x6563b4] = esi (param1)
    handle = FUN_0046d710("cursor")                      // 0x43a02b push s_cursor_005ce6a4; 0x43a036 call 0x46d710
    if cursor == handle:                                 // 0x43a03b cmp esi,eax; jne 0x43a06a
        // Match: install the default cursorshadow and reset its offsets
        MouseShadow = FUN_0046d710("cursorshadow")       // 0x43a046 push s_cursorshadow_005ce6ac; 0x43a04b call; 0x43a050 [0x6563b8] = eax
        shadowoffsetx = 1                                // 0x43a055 [0x6563bc] = 1
        shadowoffsety = 3                                // 0x43a05f [0x6563c0] = 3
        return
    else:                                                // 0x43a06a..7e
        MouseCursorAdd = NULL                            // 0x43a06a [0x6563c4] = 0
        priority = 1                                     // 0x43a074 [0x6563b0] = 1
        return
    // Note: MouseShadow is NOT cleared in the else branch — the previously
    // installed shadow (drag-shadow, dialog-shadow, etc.) persists. This is
    // intentional: the caller selects shadow via a separate SetMouseShadow
    // call. Matches snapshot legacy/cursor.cpp:39-51 (which has the same
    // `//MouseShadow = NULL` commented out).
```

### 6.2 `GetMouseBitmap()` — `FUN_0043a080` @ `0x43a080`

```
PTBitmap GetMouseBitmap():                               // disasm 0x43a080-85
    return MouseCursor                                   // 0x43a080 mov eax,[0x6563b4]; ret
```

### 6.3 `SetMouseShadow(shadow, offsetx, offsety)` — `FUN_0043a090` @ `0x43a090`

```
void SetMouseShadow(PTBitmap shadow, int offsetx=1, int offsety=3):  // disasm 0x43a090-ad
    MouseShadow = shadow                                 // [0x6563b8] = eax (param1)
    shadowoffsetx = offsetx                              // [0x6563bc] = ecx (param2)
    shadowoffsety = offsety                              // [0x6563c0] = edx (param3)
```

Default offsets `(1,3)` come from the inline `SetMouseShadow(shadow)` call
inside SetMouseBitmap (§6.1), which loads literals `1` and `3` into the
globals before this function is called. Caller-supplied defaults match
`legacy/cursor.h:19` `int offsetx=1, int offsety=3`.

### 6.4 `SetMouseCornerBitmap(PTBitmap corner, BOOL toppriority)` — `FUN_0043a0b0` @ `0x43a0b0`

```
void SetMouseCornerBitmap(PTBitmap corner, BOOL toppriority=FALSE):  // disasm 0x43a0b0-cb
    if corner != NULL OR toppriority != 0:               // 0x43a0b8-be: test;jne; test;je
        MouseCursorAdd = corner                          // 0x43a0c0 [0x6563c4] = eax
        priority = toppriority                           // 0x43a0c5 [0x6563b0] = ecx
    return
```

### 6.5 `SetMouseCornerBitmap(int type, BOOL toppriority)` — `FUN_0043a0d0` @ `0x43a0d0`

```
void SetMouseCornerBitmap(int type, BOOL toppriority=FALSE):  // disasm 0x43a0d0-ff
    if type == CURSOR_NONE (-1): return                  // 0x43a0d4 cmp eax,-1; je 0x43a0ff
    name = CursorTypes[type]                             // 0x43a0d9 mov eax,[4*eax + 0x5ce648]
    bm = GameData->Bitmap(name)                          // 0x43a0e0/e7: GameData ptr 0x65abc0; call 0x46d710
    if bm == NULL: return                                // 0x43a0ee test eax,eax; je
    MouseCursorAdd = bm                                  // 0x43a0f0 [0x6563c4] = eax
    priority = 0                                         // 0x43a0f5 [0x6563b0] = 0
    return
```

**The CursorTypes table** (§2) gives bitmap names; missing entries (stairs,
hourglass) silently fall through to "no corner overlay shown this frame".

### 6.6 `SetDragBitmap(drag, x, y)` — `FUN_0043a100` @ `0x43a100`

```
void SetDragBitmap(PTBitmap drag, int x, int y):         // disasm 0x43a100-2b
    // First reset previous drag scratch surface state
    SetDragBitmap_internal_reset()  // FUN_0043a240 with (0,0,0) // 0x43a100-06 push 0,0,0; call 0x43a240
    DragBitmap = drag                                     // 0x43a11a [0x6563c8] = eax
    grabx = x                                             // 0x43a11f [0x6563a8] = ecx
    graby = y                                             // 0x43a125 [0x6563ac] = edx
    return
```

`FUN_0043a240` (the inner "reset and prep drag composite surface" helper) is
a larger function (`0x43a240..0x43a40d`, 461 bytes) that handles the dragged
bitmap's per-frame composite surface — allocating/clearing it (`(*+0x44)`
with `(0,0,0x2710,0x2710)` = clear sub-rect; `(*+0x40)` Origin reset; uses
the global TDisplay `PTR_DAT_005d79e0`). For port purposes this is internal
to the drag layer and the modern renderer handles it implicitly — the spec
does not need every byte. **Net effect:** when a non-null DragBitmap is set,
a chip-color cleared scratch surface is established and the drag bitmap is
stamped into it once via `FUN_004bd680(0,0, drag, 0x80000000, 0)`
(`0x43a358 push 0x80000000; 0x43a35d call 0x4bd680`); each frame's
DrawMouseCursor re-emits the cached composite.

### 6.7 `ClearDragBitmap()` — `FUN_0043a130` @ `0x43a130`

```
void ClearDragBitmap():                                  // disasm 0x43a130-3a
    cleardragbitmap = 1                                  // 0x43a130 [0x6563d0] = 1; ret
```

**One-frame deferred clear** — DragBitmap is not actually cleared here.
`DrawMouseCursor` (§5 step 6) reads `cleardragbitmap`, calls
`SetDragBitmap(NULL,0,0)`, and resets `cleardragbitmap=0`. This guarantees
the dragged bitmap is visible for one final frame after the user drops, so
the on-release visual is consistent.

### 6.8 `SetDragObj(PTObjectInstance inst)` — `FUN_0043a140` @ `0x43a140`

```
void SetDragObj(PTObjectInstance inst):                  // disasm 0x43a140-53
    DragObj = inst                                       // [0x6563cc] = ecx (param1)
    if inst != NULL: tail-call 0x473a10                  // 0x43a14e jmp 0x473a10
    return
```

`FUN_00473a10` is a refcount/lifetime helper on the object (UNCONFIRMED-B —
not in our extracted set; likely the object-ref AddRef). The dragged object
needs lifetime pinning so it survives map streaming/sector unloads while held.
Inferred from `legacy/cursor.cpp` which holds a `PTObjectInstance` ptr
without explicit AddRef in C++ (the snapshot relied on a different ownership
scheme). Resolve: `DecompileAddr.java 0x00473a10`.

### 6.9 `GetDragObj()` — `FUN_0043a160` @ `0x43a160`

```
PTObjectInstance GetDragObj():                           // disasm 0x43a160-65
    return DragObj                                       // mov eax,[0x6563cc]; ret
```

### 6.10 `DrawMouseShadow(x, y, w, h)` — `FUN_0043a420` @ `0x43a420`

The 4-arg, clip-rect variant. Internal helper called only by the no-arg
`DrawMouseShadow()` (§6.11).

```
void DrawMouseShadow(int x, int y, int w, int h):       // disasm 0x43a420-474
    Display->(*+0x44)(x, y, w, h)                        // 0x43a43c call [eax+0x44] — SetClipRect
    sx = cursorx + shadowoffsetx                         // 0x43a45f-466
    sy = cursory + shadowoffsety                         // 0x43a44a-45d
    FUN_004bd680(sx, sy, MouseShadow, 0x21100, 0)        // 0x43a46f call sprite-stamp
        // drawmode = DM_TRANSPARENT | DM_USEREG | DM_ALIAS = 0x21100
    return
```

`(*+0x44)` on the display = `SetClipRect` per UI_METHOD_MAP §15a (slot+0x44 =
`Unlock`/commit-region; HERE it is the display surface's clip-region setter
which shares the slot in the retail surface vtable — UNCONFIRMED-C in
UI_METHOD_MAP §17 #9 covers this slot ambiguity for non-pane displays).
**Resolve:** the snapshot src `legacy/cursor.cpp:99` does `Display->SetClipRect`,
so the slot dispatched at `+0x44` here IS `SetClipRect` on TDisplay. The
UI_METHOD_MAP method-map says `+0x44` on TSurface is `Unlock`; on TDisplay
the corresponding slot was reused for SetClipRect (TDisplay has a different
vtable layout from a TSurface). Treat as TDisplay-specific.

### 6.11 `DrawMouseShadow()` (no args) — `FUN_0043a5a0` @ `0x43a5a0`

```
void DrawMouseShadow():                                  // disasm 0x43a5a0-814
    if MouseShadow == NULL: return                       // 0x43a5a0-5a7

    Display->(*+0x40)(0, 0)                              // 0x43a5b8-bc call [eax+0x40] — SetOrigin(0,0)

    cursorshadow_handle = FUN_0046d710("cursorshadow")   // 0x43a5c5 lookup; 0x43a5ca call 0x46d710
    if MouseShadow != cursorshadow_handle:               // 0x43a5d5 cmp; je
        // Drag-shadow or other custom shadow: clip to MAPPANE ONLY, draw once
        Display->SetClipRect(MAPPANEX, MAPPANEY, MAPPANEW, MAPPANEH)
            // 0x43a5e5-5f9 push MAPPANEH(=DAT_0065c5c4), MAPPANEW(=DAT_00667c30),
            //    MAPPANEY(=DAT_006663d4), MAPPANEX(=DAT_006663d8); call [edx+0x44]
        FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                     MouseShadow, 0x21100, 0)
        goto cleanup
    else:
        // Default cursorshadow: clip per MAPPANE-vs-cursor geometry
        area_flags = (DAT_0065cb0c > 0) ? *(DAT_0065cb1c + 4*DAT_0065cb0c) : 0
                                                          // 0x43a62c-640
        if area_flags & 0xf0 != 0:                       // 0x43a640 test al,0xf0
            // "InCompleteExclusion" = no 3D scene visible (cinematic/dialog
            //  takeover) — clip to whole screen, single draw.
            Display->SetClipRect(0, 0, 0x280, 0x1e0)     // 0x43a64a-65a
            FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                         MouseShadow, 0x21100, 0)
            goto cleanup
        else:
            // Normal: shadow falls OUTSIDE MAPPANE, broken into up to 4
            // strips drawn ONE OR TWO of which apply depending on cursor pos.
            //
            // Left strip:
            if cursorx <= MAPPANEX:                      // 0x43a68d-9a
                Display->SetClipRect(0, 0, MAPPANEX, 0x1e0)
                FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                             MouseShadow, 0x21100, 0)
            // Right strip:
            else if (cursorx + MouseShadow->width) >= (MAPPANEX + MAPPANEWIDTH):
                                                          // 0x43a6dd-6ec
                Display->SetClipRect(MAPPANEX+MAPPANEWIDTH, 0,
                                     0x280-MAPPANEX-MAPPANEWIDTH, 0x1e0)
                                                          // 0x43a6f4-708
                FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                             MouseShadow, 0x21100, 0)
            // Top strip:
            if cursory <= MAPPANEY:                      // 0x43a740-4d
                Display->SetClipRect(0, 0, 0x280, MAPPANEY)
                                                          // 0x43a755-761
                FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                             MouseShadow, 0x21100, 0)
            // Bottom strip:
            else if (cursory + MouseShadow->height) >= (MAPPANEY + MAPPANEHEIGHT):
                                                          // 0x43a790-7a6
                Display->SetClipRect(0, MAPPANEY+MAPPANEHEIGHT, 0x280,
                                     0x1e0-MAPPANEY-MAPPANEHEIGHT)
                                                          // 0x43a7ae-7c2
                FUN_004bd680(cursorx+shadowoffsetx, cursory+shadowoffsety,
                             MouseShadow, 0x21100, 0)

cleanup:
    Display->ResetClipRect()                              // 0x43a800-80e push (clip4,clip8) from
                                                          //    display state; call [edx+0x44]
    return
```

**Spatial intent:** the default cursor's drop shadow is only visible over HUD
chrome (border panes, statusbars, sidebars), never over the 3D map view —
because over the 3D scene a soft drop shadow would interact poorly with the
per-frame Z-buffer of the rendered geometry. The cursor itself (no shadow)
still draws over the 3D view via `DrawMouseCursor`.

### 6.12 `DrawMouseCursor()` — `FUN_0043a480` @ `0x43a480`

Already documented in §5 (composition table). Pseudocode:

```
void DrawMouseCursor():                                  // disasm 0x43a480-59a
    Display->(*+0x24)()                                  // 0x43a488 call [eax+0x24] — ResetClipRect
    cx = clamp(cursorx, 1, 0x280)                        // 0x43a48b-4ad
    cy = clamp(cursory, 1, 0x1e0)                        // 0x43a4b5-4cd

    if DragBitmap != NULL:                               // 0x43a4d2-dd
        FUN_004bd680(cx - grabx, cy - graby,
                     DragBitmap, 0x20100, 0)             // 0x43a4dc-4fe
            // drawmode = DM_TRANSPARENT | DM_USEREG (no AA on drag bitmap)

    if MouseCursor != NULL AND MouseCursorAdd == NULL:   // 0x43a50f-21
        FUN_004bd680(cx, cy, MouseCursor, 0x21100, 0)    // 0x43a523-32
            // drawmode = DM_TRANSPARENT | DM_USEREG | DM_ALIAS

    if MouseCursorAdd != NULL:                           // 0x43a542-4a
        FUN_004bd680(cx, cy, MouseCursorAdd, 0x21100, 0) // 0x43a54c-5b

    if cleardragbitmap != 0:                             // 0x43a560-7
        SetDragBitmap(NULL, 0, 0)                        // 0x43a56a-77 (FUN_0043a240
                                                          //    via 0x43a100's prologue)
        DragBitmap = 0; grabx = 0; graby = 0;
        cleardragbitmap = 0                              // 0x43a575-87
    MouseCursorAdd = NULL                                // 0x43a58d
    priority = 0                                         // 0x43a593
    return
```

### 6.13 `CursorOverObject(inst, toppriority)` — `FUN_0043a820` @ `0x43a820`

```
void CursorOverObject(PTObjectInstance inst, BOOL toppriority):  // disasm 0x43a820-75
    if toppriority == 0 AND priority != 0: return        // 0x43a825-30 (skip when
                                                          //    already top-priority)
    if inst == NULL: return                              // 0x43a836-8 (no inst, no overlay)
    type = inst->vtable+0xc0(DragObj)                    // 0x43a840-3 call [eax+0xc0]
        // CursorType(PTObjectInstance with = DragObj)
    if type == CURSOR_NONE (-1): return                  // 0x43a849-4c
    name = CursorTypes[type]                             // 0x43a84e mov [4*eax + 0x5ce648]
    bm = GameData->Bitmap(name)                          // 0x43a855-5c
    if bm == NULL: return                                // 0x43a861-3
    MouseCursorAdd = bm                                  // 0x43a865 [0x6563c4] = eax
    priority = 0                                         // 0x43a86a [0x6563b0] = 0
    return
```

**The object's vtable+0xc0 slot is `CursorType(PTObjectInstance with)`.**
This is the load-bearing identification — every interactable object overrides
this method to return the right enum (see `legacy/cursor.h:22-24` + the
`back` decomp-text grep showing per-object overrides: container → HAND, NPC →
MOUTH, door → DOOR, weapon under attackable → SWORDS, etc.).

### 6.14 `RestrictCursor()` — `FUN_0043a880` @ `0x43a880`

```
void RestrictCursor():                                   // disasm 0x43a880-8f8
    if Windowed != 0: return                             // 0x43a880-8a (only restrict when NOT
                                                          //    windowed OR borderless)
    GetClientRect(MainWindow.hwnd, &r)                   // 0x43a898 call [PTR_GetClientRect_005a3280]
    ClientToScreen(MainWindow.hwnd, (POINT*)&r)          // 0x43a8a9 call [PTR_ClientToScreen_005a3288]
    SetCursorPos(r.left + oldcursorx, r.top + oldcursory)
                                                          // 0x43a8c5-c9 call [PTR_SetCursorPos_005a3284]
    r.right  = r.left + 0x280                            // 0x43a8db
    r.bottom = r.top + 0x1e0                             // 0x43a8e1
    ClipCursor(&r)                                       // 0x43a8ef call [PTR_ClipCursor_005a3274]
    return
```

`MainWindow.hwnd` is fetched from `DAT_0065b8ec`. `oldcursorx`/`oldcursory`
are saved at `DAT_005ce640`/`DAT_005ce644` (see §6.15).

### 6.15 `ReleaseCursor()` — `FUN_0043a900` @ `0x43a900`

```
void ReleaseCursor():                                    // disasm 0x43a900-27
    if Windowed != 0: return                             // 0x43a900-7
    oldcursorx = cursorx                                 // 0x43a909-16 [0x5ce640] = [0x668510]
    oldcursory = cursory                                 // 0x43a90e/1b [0x5ce644] = [0x668514]
    ClipCursor(NULL)                                     // 0x43a914 push 0; 0x43a921 call [0x5a3274]
    return
```

**Both early-return when Windowed.** Snapshot `legacy/cursor.cpp:168-182`
does the restrict unconditionally and the clip only when `!Windowed ||
Borderless`. Retail collapsed these into a single Windowed-only gate; the
visible-cursor saving step ALSO only runs in fullscreen because there is
nothing to release in windowed mode. (Snapshot saved cursor pos always; retail
optimized.)

---

## §7 — Effects & shadows

- **Sprite chroma key:** `0x7C1F` (magenta in RGB555). Per UI_METHOD_MAP §16
  and [[project-retail-pink-halo-bug]] — the cursor sprites carry alpha in
  their bitmap (DM_ALIAS), so the chroma key is layered on top by DM_TRANSPARENT
  but the soft edges come from the per-bitmap alias channel. **No pink-halo
  bug applies here** — the cursor draws straight to the display (not into a
  magenta-cleared scratch), so there's no chroma-keyed fringe.
- **Drop shadow:** `cursorshadow` is a pre-baked dark version of the cursor
  sprite (14×20) drawn at `cursor+(1,3)` BEFORE the cursor itself. The
  separation of "shadow drawn under panes, cursor drawn over panes" gives the
  cursor a parallax-friendly soft drop. NOT computed at runtime — the shadow
  is a separate asset, NOT a darkened copy of the cursor pixels.
- **No 3-pass text shadow** (no text in this layer).
- **DM_ALIAS** is the per-sprite antialias channel. Combined with
  DM_TRANSPARENT it gives a clean soft cursor outline that anti-aliases against
  the underlying HUD chrome.

---

## §8 — Text rendering

**Not applicable.** The cursor layer renders only sprites. There is no text.

---

## §9 — Animation & dynamic behavior

- **No tweens.** The cursor sprite does not animate; it is a static sprite
  follow-the-mouse layer. The only dynamic value is `(cursorx, cursory)` which
  is published by the input/event layer (mouse motion → write to
  `DAT_00668510/14`), and the per-frame `MouseCursorAdd` which is set by
  hover callbacks (§6.13) and unconditionally cleared at end-of-frame
  (`DrawMouseCursor` step 7).
- **Dirty/redraw model:** none. The cursor layer is RE-DRAWN every frame.
  No caching — `cleardragbitmap` is a 1-frame delayed-clear latch (§6.7) but
  is not a dirty/version flag.
- **State machine:** trivial 3-state visual:
  - **Default cursor** — `MouseCursor=cursor, MouseCursorAdd=NULL, DragBitmap=NULL`
    → cursor sprite + shadow.
  - **Hover-over-interactable** — `MouseCursorAdd != NULL` → corner overlay
    REPLACES the cursor for that frame (cursor not drawn — see §5 step 4 cond).
  - **Drag-active** — `DragBitmap != NULL` → drag bitmap drawn UNDER cursor; if
    a corner overlay is also active it stacks on top of the cursor as usual.
  After every frame: `MouseCursorAdd → NULL`, `priority → 0` (§5 step 7).
  The hover state must be re-asserted EVERY frame by a `CursorOverObject` call
  from the per-frame pane hit-test logic. If no call → cursor reverts to
  default visual.

---

## §10 — Input & dispatch

- **Cursor position publication.** Per-frame, the input layer (Win32 mouse
  events) writes `(cursorx, cursory)` to `DAT_00668510/14`. The 3D scene reads
  them too (`cls_0x411eb0_T3DScene.cpp:472-473`) — picking the world point
  under the cursor. (Beyond this spec's scope; mentioned for completeness.)
- **mousebutton.** `extern int mousebutton` in `legacy/cursor.h:16` — port
  shell has it as `int32_t mousebutton`; UNCONFIRMED-D: which DAT_* address
  the retail uses. Not used by the cursor draw layer; consumed by panes for
  hit-testing.
- **No hit rects.** The cursor layer has no clickable areas of its own.
- **Region-helper undocumented gotcha.** `cls_0x5a4358_TConsolePane.cpp:420-438`
  shows a small helper (function at `0x43e510`) which calls `FUN_0043a020`
  with two different arguments inside a region-rect-build loop:
  - one branch installs the "wait" cursor (via a `FUN_0046d710(DAT_00656ce4)`
    lookup — the global at `0x656ce4` is written by `FUN_0043d410` and is the
    busy/wait cursor handle),
  - the other restores `DAT_0065a28c` (the default "cursor" handle).

  This is an editor/region-define modal cursor swap and is NOT a TConsolePane
  function — it's Ghidra-merged. The "wait" cursor handle is what the snapshot
  calls `CURSOR_HOURGLASS` semantically, but instead of a corner overlay it
  swaps the MAIN cursor bitmap. UNCONFIRMED-E: which subsystem this region
  helper actually belongs to. The cursor layer just consumes the
  `SetMouseBitmap` calls; it doesn't care who calls them.

---

## §11 — Retail bugs NOT to reproduce

1. **stairs / hourglass bitmaps absent from playscrn.dat.** Per §2, the
   `CursorTypes[]` string table at `0x5ce648` references all 7 names, but the
   actual sprites for `stairs` (CURSOR_STAIRS=4) and `hourglass`
   (CURSOR_HOURGLASS=5) are NOT in playscrn.dat (verified with dump_dat).
   `FUN_0046d710("stairs")` and `FUN_0046d710("hourglass")` return NULL at
   runtime, and `SetMouseCornerBitmap(int type)` early-outs at `0x43a0ee`
   when the lookup is NULL — so requesting either type is a silent no-op
   (the previous corner overlay stays).

   **Treatment in the port:** do not silently swallow — log a `log_warn` once
   per name when the lookup misses, so the port flags any place the game asks
   for these. If the snapshot's intent (e.g. CURSOR_HOURGLASS for
   savegame loading) is needed, source a stand-in from another archive (e.g.
   menus.dat) OR mark as a Revisited extension. NOT a bug to reproduce.

2. **`MouseShadow` is NOT cleared by `SetMouseBitmap` when cursor !=
   "cursor".** Snapshot src has the line `//MouseShadow = NULL;` commented
   out; retail matches. This means if subsystem A installs a custom shadow
   then subsystem B sets a non-default cursor, B will inherit A's shadow until
   B explicitly calls `SetMouseShadow`. Defensible behavior (caller knows
   what it wants), but easy footgun. **Not a bug, but document it.**

3. **One-frame ghost on drop.** `ClearDragBitmap()` does NOT actually drop
   the bitmap; it sets `cleardragbitmap=1` which `DrawMouseCursor` reads at
   end-of-frame. So the dragged bitmap remains visible for ONE more frame
   after the user releases. **Intentional** (smoother visual on release), not
   a bug — but document it so the port doesn't "fix" it.

4. **Clamp to 640×480.** `DrawMouseCursor` clamps `cursorx/y` to `[1,640]
   × [1,480]` even though the port may run at a higher virtual resolution.
   This is a 640×480-baked retail constant. For the Classic Mode port, retain
   the clamp. For Revisited Mode (higher native res) the clamp must move to
   `[1, display_w] × [1, display_h]` ([[project-resolution-modes]]).

---

## §12 — Reconstruction pseudocode

```cpp
// Globals (port-side; one each, no chip mirror)
PTBitmap MouseCursor       = nullptr;
PTBitmap MouseShadow       = nullptr;
PTBitmap MouseCursorAdd    = nullptr;
PTBitmap DragBitmap        = nullptr;
TObjectInstance* DragObj   = nullptr;
int      shadowoffsetx     = 0;     // default 1 for cursorshadow
int      shadowoffsety     = 0;     // default 3 for cursorshadow
int      grabx, graby      = 0;
bool     priority          = false;
bool     cleardragbitmap   = false;
int      oldcursorx, oldcursory;    // for ReleaseCursor save

static const char* kCursorTypes[NUMCURSORTYPES] = {
    "hand", "eye", "mouth", "door", "stairs", "hourglass", "swords"
};

// 6.1 SetMouseBitmap
void SetMouseBitmap(PTBitmap cursor) {
    MouseCursor = cursor;
    if (cursor == GameData->Bitmap("cursor")) {
        MouseShadow    = GameData->Bitmap("cursorshadow");
        shadowoffsetx  = 1;
        shadowoffsety  = 3;
    } else {
        // NOTE: MouseShadow intentionally NOT cleared (retail behavior).
        MouseCursorAdd = nullptr;
        priority       = true;
    }
}

// 6.3 SetMouseShadow(bm, offx=1, offy=3) — direct field set.

// 6.4 / 6.5 SetMouseCornerBitmap (two overloads exactly as snapshot).

// 6.13 CursorOverObject
void CursorOverObject(TObjectInstance* inst, bool toppriority = false) {
    if (toppriority || !priority) {
        int type = CURSOR_NONE;
        if (inst) type = inst->CursorType(DragObj);
        SetMouseCornerBitmap(type, toppriority);
    }
}

// 6.11 DrawMouseShadow() — called BEFORE pane draws each frame
void DrawMouseShadow() {
    if (!MouseShadow) return;
    Display.SetOrigin(0, 0);
    if (MouseShadow != GameData->Bitmap("cursorshadow")) {
        // Drag-shadow path: clip to MAPPANE only
        DrawMouseShadowImpl(MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT);
    } else if (PlayScreen.InCompleteExclusion()) {
        DrawMouseShadowImpl(0, 0, WIDTH, HEIGHT);
    } else {
        // Normal: shadow OUTSIDE the MAPPANE strips
        if (cursorx <= MAPPANEX)
            DrawMouseShadowImpl(0, 0, MAPPANEX, HEIGHT);
        else if (cursorx + MouseShadow->width >= MAPPANEX + MAPPANEWIDTH)
            DrawMouseShadowImpl(MAPPANEX + MAPPANEWIDTH, 0,
                                 WIDTH - (MAPPANEX + MAPPANEWIDTH), HEIGHT);

        if (cursory <= MAPPANEY)
            DrawMouseShadowImpl(0, 0, WIDTH, MAPPANEY);
        else if (cursory + MouseShadow->height >= MAPPANEY + MAPPANEHEIGHT)
            DrawMouseShadowImpl(0, MAPPANEY + MAPPANEHEIGHT, WIDTH,
                                 HEIGHT - (MAPPANEY + MAPPANEHEIGHT));
    }
    Display.ResetClipRect();
}

static void DrawMouseShadowImpl(int x, int y, int w, int h) {
    Display.SetClipRect(x, y, w, h);
    Display.Put(cursorx + shadowoffsetx, cursory + shadowoffsety,
                MouseShadow,
                DM_TRANSPARENT | DM_USEREG | DM_ALIAS);   // 0x21100
}

// 6.12 DrawMouseCursor() — called AFTER pane draws each frame
void DrawMouseCursor() {
    Display.ResetClipRect();
    int cx = std::clamp(cursorx, 1, WIDTH);
    int cy = std::clamp(cursory, 1, HEIGHT);

    if (DragBitmap) {
        Display.Put(cx - grabx, cy - graby, DragBitmap,
                    DM_TRANSPARENT | DM_USEREG);          // 0x20100 (no AA)
    }
    if (MouseCursor && !MouseCursorAdd) {
        Display.Put(cx, cy, MouseCursor,
                    DM_TRANSPARENT | DM_USEREG | DM_ALIAS); // 0x21100
    }
    if (MouseCursorAdd) {
        Display.Put(cx, cy, MouseCursorAdd,
                    DM_TRANSPARENT | DM_USEREG | DM_ALIAS); // 0x21100
    }

    if (cleardragbitmap) {
        SetDragBitmap(nullptr, 0, 0);
        cleardragbitmap = false;
    }
    MouseCursorAdd = nullptr;
    priority       = false;
}
```

**Lifecycle wiring on the port:**

1. `TPlayScreen::Initialize`: `cursor=FUN_0046d710("cursor");
   handcursor=FUN_0046d710("handcursor"); SetMouseBitmap(cursor)`. Fail-out if
   either is NULL.
2. `TLogoScreen::Initialize`, `TDeathScreen::Initialize`: per-screen
   `bm=FUN_0046d710("cursor"); SetMouseBitmap(bm)`.
3. `TPlayScreen::Close`: `SetMouseBitmap(NULL)`.
4. Per-frame (in the TScreen base Draw):
   a. `DrawMouseShadow()` after the 3D scene but BEFORE the panes,
   b. (panes draw),
   c. `DrawMouseCursor()` at the very end of the swapchain pass — already
      done by the port's `TCursorHud::Draw` registered as a HUD drawable at
      conventional z=0.
5. Per-frame hover update (called from pane hit-tests):
   `CursorOverObject(hoverObject)`.

---

## §13 — Port mapping notes

Maps to the port primitives (UI_METHOD_MAP §12) and the existing port shell
(`src/cursor.{h,cpp}` + `src/renderer.h`):

| retail call                              | port mapping                                                                |
|------------------------------------------|------------------------------------------------------------------------------|
| `FUN_004bd680(x,y,bm,0x21100,0)`        | `Renderer->DrawBitmap(bm, x, y)` (DM_TRANSPARENT|DM_USEREG|DM_ALIAS) — bm carries alpha; regx/regy handled by the renderer |
| `FUN_004bd680(x,y,bm,0x20100,0)`        | `Renderer->DrawBitmap(bm, x, y)` (DM_TRANSPARENT|DM_USEREG, no AA) — drag path |
| `FUN_004bd680(x,y,bm,0x80000000,0)`     | `Renderer->DrawBitmap(bm, x, y)` opaque copy — used by `FUN_0043a240` drag-prep |
| `FUN_0046d710("<name>")`                | `GameData->Bitmap("<name>")` — by-name asset cache lookup                  |
| `Display->(*+0x44)(x,y,w,h)` (TDisplay) | `Display.SetClipRect(x, y, w, h)`                                          |
| `Display->(*+0x40)(0,0)` (TDisplay)     | `Display.SetOrigin(0, 0)`                                                  |
| `Display->(*+0x24)()` (TDisplay)        | `Display.ResetClipRect()`                                                  |
| `ClipCursor / GetClientRect / ClientToScreen / SetCursorPos` | macOS: no direct sokol_app equivalent (the port's `RestrictCursor` is `#if 0`'d). Options: (a) leave stubbed in fullscreen, (b) implement via `sapp_mouse_capture` + manual centering. **DECISION DEFERRED.** |

**The existing port shell is structurally complete.** No new primitive is
needed — every retail draw call reduces to `Renderer->DrawBitmap` (the port
splits `DrawMouseShadow` clip-rect calls through `Display.SetClipRect` which
already exists).

**Drawmode `DM_USEREG=0x20000` is NOT in UI_METHOD_MAP §1.** It's defined in
`legacy/RevDefs.h` and consumed by `Display.Put`/`Renderer->DrawBitmap`. The
method-map's drawmode table should be extended to include it (Gap noted §14).

---

## §14 — UNCONFIRMED / open questions

**A. handcursor install caller (the `handcursor` SetMouseBitmap site).**
TPlayScreen::Initialize stores the `handcursor` sprite handle at `DAT_0065a284`
but the recon set we read shows no caller that does
`SetMouseBitmap(DAT_0065a284)`. The likely callers are: (1) the
inventory-drag pickup state (when user clicks an item to start drag), (2) a
hover-on-portable-object hint. Snapshot `legacy/cursor.{h,cpp}` does NOT
auto-swap to `handcursor`; the snapshot's drag visual is purely DragBitmap +
SetMouseShadow(`shadow`, 0, 0x2b). So retail may have ADDED handcursor as a
1.0+ post-snapshot extension — but the actual call site is missing from our
discovered/ set.
- **Impact if wrong:** the hand-cursor never appears (drag uses the regular
  cursor + drag bitmap). Visually present in retail screenshots though, so a
  caller exists somewhere.
- **Resolve:** `grep -rn "DAT_0065a284\|_DAT_0065a284" recon/ghidra/` returned
  zero readers. Try `DecompileAddr.java` on the inventory/equip click handlers
  (e.g. `cls_0x5a58c0` inventory) for `0x65a284` references in the raw
  disassembly: `objdump -d --start-address=0x537000 --stop-address=0x538000
  data/Revenant.exe | grep -i '0x65a284'`. Or run
  `DumpVtable.java` on TInventory's `MouseDown` slot.

**B. `FUN_00473a10` — the SetDragObj tail-call target.** Likely an
object refcount / saferef pin so the dragged object survives sector unloads.
Not extracted.
- **Impact if wrong:** dragged object pointer could dangle during streaming;
  the engine layer here just stores the ptr.
- **Resolve:** `DecompileAddr.java 0x00473a10`.

**C. TDisplay vtable slot at `+0x44` is `SetClipRect` (NOT `Unlock` as in the
TSurface vtable map UI_METHOD_MAP §15a).** The snapshot `legacy/cursor.cpp:99`
`Display->SetClipRect(...)` confirms by behavior, but the slot identity in the
UI method map names it `Unlock`/commit-region for TSurfaces. TDisplay is a
sibling vtable that reuses the slot for a different purpose.
- **Impact if wrong:** if the port's `Display.SetClipRect` doesn't actually
  reach the slot, the shadow draws unclipped (cosmetic — small).
- **Resolve:** `DumpVtable.java` on `PTR_DAT_005d79e0`'s vtable (= the global
  display object) — confirm slot +0x44 is `SetClipRect`. Then add a TDisplay
  vtable row to UI_METHOD_MAP §15 (currently §15a is TSurface only).

**D. `mousebutton` global address.** `legacy/cursor.h:16` declares
`extern int mousebutton`. Not addressed in the disassembly I extracted; the
cursor draw code doesn't read it. Snapshot says it's the input layer's button
bitfield.
- **Impact if wrong:** none for cursor draw — for hit-testing only.
- **Resolve:** grep retail for the pattern of reading a button bit (e.g.
  `mov al, [DAT_*]; test al, 0x1`) near input dispatch (`FUN_0047cf40`
  TPlayScreen::DispatchCommand).

**E. The Ghidra-merged "region helper" (`0x43e510`) and its `DAT_00656ce4`
"wait cursor".** `cls_0x5a4358_TConsolePane.cpp:420-438` is mis-merged into
the TConsolePane file; the helper is part of an editor or region-define
modal flow that uses `FUN_0043a020` to swap to a hourglass-equivalent cursor
during multi-click rect drag. The cursor-state matrix this implies is:
- default `cursor` ↔ swap to "wait" `DAT_00656ce4`. Likely a 2-state hourglass
  selection cursor for the editor.
- **Impact if wrong:** game-play is unaffected; this is editor-only. The
  cursor layer doesn't care who calls it.
- **Resolve:** `DecompileAddr.java 0x0043d410` (which writes `DAT_00656ce4`)
  + `DecompileAddr.java 0x0043e510` (the helper).

**F. UI_METHOD_MAP gap: `DM_USEREG=0x20000` not in the drawmode table.** The
cursor layer uses it on every draw. Should be added so other panels (TButton
also uses it per the snapshot) can cite it.
- **Resolve:** extend UI_METHOD_MAP §1's drawmode table with `DM_USEREG=0x20000
  = 131072` — bias dst by `(bm->regx, bm->regy)`.

**G. The exact behavior of `FUN_0046d710` when handed a name string the
asset cache does not contain.** For stairs/hourglass it must return NULL (the
caller checks). Treat as confirmed by the caller-side null-check pattern at
`0x43a0ee`/`0x43a863`, but the body itself is not extracted (UI_METHOD_MAP §9
already lists this as UNCONFIRMED).
- **Resolve:** `DecompileAddr.java 0x0046d710`.

**H. RestrictCursor / ReleaseCursor port strategy.** Win32-only retail; sokol
has no native ClipCursor. The decision is a port-side architecture choice
(see [[project-target-platform]]). The forensics is complete; the port can
either: (1) leave stubbed (current behavior), (2) implement via
sapp_lock_mouse and re-emit, (3) use AppKit `[NSCursor unhide]` +
`CGAssociateMouseAndMouseCursorPosition(false)`.
- **Impact:** in fullscreen on a multi-monitor setup, the cursor can wander
  off the game viewport. Visible artifact.
- **Resolve:** product decision, not a forensics gap.
