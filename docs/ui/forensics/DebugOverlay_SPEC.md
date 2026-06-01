# DebugOverlay — forensics spec (TPlayScreen TimerTick F-key debug paths)

The "debug overlays" in `TPlayScreen::TimerTick @ 0x47c630` are NOT a
dedicated UI panel. They are three different existing-subsystem
invocations triggered by per-key cases of the TimerTick switch:

1. **TTextBar message push** (`"Updating script files..."`,
   `"Reloading rules files..."`) — text is appended to the existing
   message-overlay (the `TTextBar` global singleton `0x65c5d0`,
   spec'd in [TTextBar_SPEC.md](TTextBar_SPEC.md)). NOT a new panel.
2. **Existing `PopupDef` yes/no modal** (`"exitgameyn"`) — invoked via
   the shared `FUN_0053c060` wrapper documented in
   [PopupDef_SPEC.md](PopupDef_SPEC.md) §6.3. The popup-engine handles
   layout, the message-key lookup, and the modal pump; TimerTick only
   pushes the panel key + flags. NOT a new panel.
3. **Screenshot-to-`ss.bmp`** (raster dump of the live display) before
   showing a save/options/in-game-menu modal. Filesystem side-effect,
   no on-screen UI of its own; the modal-pane that follows is the
   visible artifact.

So the "debug overlay" task is really a forensics writeup of TimerTick's
**dispatch-by-VK-code switch** + a list of which existing systems each
case calls. The reconstruction agent does NOT build a new panel — it
wires the dispatcher and invokes already-spec'd panels/primitives.

---

## §0 — Sources & status

- **Class**: `TPlayScreen` (`cls_0x5a5320`), vtable
  `cls_0x5a5320__vftable_5a5320.cpp:19`.
- **Method**: `TPlayScreen::TimerTick` — vtable slot 12 (`+0x30`) override
  of `TScreen::TimerTick @ 0x490660`.
- **Address**: `0x0047c630`, size `0x910 = 2320` bytes (ends at the
  `add esp, 0xb8 / ret 8` at `0x0047cce0`); jumptable + index table
  immediately follow at `0x0047cce4` and `0x0047cd0c`.
- **Body extraction**: NO dedicated `recon/discovered/cls_*TimerTick*.cpp`
  exists for this address — body decoded directly from
  `data/Revenant.exe` via
  `objdump -d -M intel --start-address=0x47c630 --stop-address=0x47cf40`.
  All instruction citations below quote the resulting disassembly.
- **Recon files read** (paths relative to repo root
  `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`):
  - `recon/discovered/renames/agent_screens.txt:186-191` — TimerTick
    address + comment listing the three string anchors and "F12 / script
    / rules / ESC exit confirm" semantic flag.
  - `recon/discovered/renames/agent_ui_wave4_sidebar_cascade.txt:96-110`
    — `FUN_0047cf40_TPlayScreen_DispatchCommand` (the gameplay
    Command switch — distinct from TimerTick).
  - `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:480-531`
    — the analogous screenshot-before-savegame/loadgame pattern in
    Command cases `0x54`/`0x55`; reuses the same `FUN_00438d80` +
    `(*+0x5c)` + `FUN_004a2960("ss.bmp", 3)` sequence and confirms the
    TimerTick screenshot decoding.
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:87`
    — `*(uint*)(param_1+0x5d8) = *puVar6>>6 & 1` (the init of the
    `+0x5d8` "ESC-shows-confirm" flag).
  - `recon/classes_original/_data.txt:86606-86632` — the
    string-pool offsets for `s_ss.bmp_005d7608`,
    `s_Updating_script_files..._005d7610`,
    `s_Updating_script_files..._005d762c`,
    `s_Reloading_rules_files..._005d7648`,
    `s_Reloading_rules_files..._005d7664`, `s_exitgameyn_005d7680`,
    `s_ss.bmp_005d768c` — every string's exact address + the TimerTick
    XREF.
  - `data/resources_unzipped/english.def:~1090` —
    `EXITGAMEYN "\nAre you sure you want to exit the game and return to Windows?"`
    and `QUITGAMEYN "\nAre you sure you want to quit the current module?"`
    (the message-key → body lookup target).
  - `docs/ui/forensics/PopupDef_SPEC.md` — full spec for the popup
    engine. TimerTick's ESC case is a *consumer* of `FUN_0053c060`.
  - `docs/ui/forensics/TTextBar_SPEC.md` — full spec for the message
    overlay. TimerTick's script/rules cases are *producers* into the
    TTextBar log (via `FUN_0054d170(0x65c5d0, fmt, ...)`).
  - `docs/ui/briefs/B_r7_def_widget_engine.md` — DefScreen / DefWidget
    architecture for the popup + InGameMenu paths.
  - `recon/discovered/renames/agent_ui_wave2_defengine.txt:281-282` —
    `FUN_00537110_LoadDef_ingamemenu` identification (the ESC alt path
    target).
  - `docs/ui/forensics/UI_METHOD_MAP.md` §1, §8 — `SDrawParam`,
    `FUN_00438d80` shadow/blit-descriptor init, `(*+0x5c)` blit (used
    by the screenshot raster grab).
  - `docs/ui/forensics/InGameMenuDef_SPEC.md` — the modal that ESC
    opens when not already inside it.
- **Status**: `forensics-complete` for the dispatch + per-case routing.
  Three secondary helpers (`FUN_004c5880/4c58b0/4c58d0` — F9 stat-restore
  on the player) are signature-level identified but their inner subsystem
  semantics (heal? max-out? give items?) need extraction to confirm. See
  §14 UNCONFIRMED-A.
- **Errors to close in existing ports**: NONE — `src/playscreen.cpp:993`
  currently binds VK_F12 to the snapshot's `StartEditor()/ShutDownEditor()`
  toggle, which is the **1998/99 dev-mode** F12, not retail. Retail F12
  is the screenshot-to-`ss.bmp`. The port needs to either drop the
  editor-toggle hotkey or move it to a non-retail key so VK_F12 can
  reach the retail debug path. The retail behaviors below have no
  current port (greenfield).
- **Snapshot note (supplementary — superseded by retail)**: the
  pre-release `src/playscreen.cpp` does not implement the retail
  F-key debug ladder. Its only F-key handler is the `StartEditor`
  toggle at `:988-1005`. F1..F4 are bound to spell-invoke slots
  (`GAMECMD_INVOKE1..4` at `:76-79`) via the snapshot's
  `g_defaultGameControls[]` table. None of `"Updating script files..."`,
  `"Reloading rules files..."`, `"exitgameyn"`, or the screenshot-to-
  `ss.bmp` path appears in the snapshot — confirmed by
  `grep -rn 'Updating script\|Reloading rules\|exitgameyn\|ss.bmp' src/`
  → no matches. The snapshot binding table is useful as an API
  reference (the `SControlEntry` shape) but provides no behavior for
  this spec.

## §1 — Overview

`TPlayScreen::TimerTick(this, key, ch)` is the per-frame screen-level
key dispatcher. The signature is the standard `TScreen::TimerTick`
override (slot 12), called by the screen-pump for every key event
queued by the input layer.

It does TWO things:

1. **Dev/system-key shortcut switch** (`0x47c6a3..0x47cc07`): if `key`
   falls in `0x1b..0x7b` (ESC and the letter/F-key range) it runs a
   small switch on `key` directly that handles a fixed set of cases
   without consulting the gameplay binding table. These are the
   "F-key shortcuts" — script reload, rules reload, screenshot, exit
   confirm, debug toggles.
2. **Gameplay-binding fallback** (`0x47cc15..0x47cccc`): for any key
   not handled above (or `key=0x50` and `+0x5d4 != 0` — a special
   re-dispatch), it consults the keymap singleton at `DAT_0065a9a8`
   via `FUN_00439150_KeyDispatch(key, ch, modeFlags)`. The keymap
   returns a `GAMECOMMAND` integer which is then re-dispatched via
   `(*this+0x4c)(cmd)` (vtable slot 0x4c is the same Command
   dispatcher implemented at `0x0047cf40`).

The dev/system switch coexists with the gameplay binding table — it
**preempts** the table for keys it handles, and falls through for
everything else.

### When the cases run
TimerTick runs every screen-pump iteration; the input layer feeds it
`(key, ch)` from the queued event(s). When a modal child is active
(`this+0x5d4 != 0`), every case except `key==0xd` (VK_RETURN) is
short-circuited at the top of the function (`0x47c674..0x47c679`): so
F6/F7/F9/F11/F12/ESC are ALL silently dropped while a modal is up,
preserving the popup's keyboard focus. The same gate is applied to
the keymap-fallback dispatch at `0x47ccb1..0x47ccbe`.

**Per-case visibility predicates** (each case's local gates are in §5):
- `0x47c6be` (F9, key=0x78): requires player char `0x667fcc != 0` AND
  multiplayer-or-saving sentinel `0x668130 != 0`.
- `0x47c6f9` (F12, key=0x7b): requires save-sentinel `0x66829c == 0`
  (i.e. NOT currently saving) AND no busy pane state AND no MP-host /
  not-in-script-pump.
- `0x47c876` ('R', key=0x52): requires dev-mode `0x65c9e0 != 0` AND
  game-loaded `0x668130 != 0` — MapPane reload (NOT a player-flag
  toggle; see §6.3b).
- `0x47c82f` (VK_F11, key=0x7a): requires dev-mode `0x65c9e0 != 0`
  AND player `0x667fcc != 0` — player flag toggle (§6.3).
- `0x47c8ed` (F6, key=0x75) / `0x47c94d` (F7, key=0x76): both require
  dev-mode `0x65c9e0 != 0` AND `0x668130 != 0` (the rules subsystem
  globals being loaded).
- `0x47c9b8` ('X', key=0x58): dev-mode + `0x668130 != 0`.
- `0x47cb7e` ('P', key=0x50): if dev-mode, do rules-reload variant;
  else if NOT in a modal-block, do the cursor save/restore +
  `+0x5d4` toggle (debug-pause), with a final fall-through to keymap
  dispatch if neither matches.
- `0x47ca1d` (ESC, key=0x1b): branches on the ingame-menu-is-up flag
  `this+0x5d8` — see §5 case 0x1b.

## §2 — Asset roster

The debug paths consume the following PRE-EXISTING UI assets and string
resources. None are new (this spec creates no new asset binding).

| asset | source | role | cite |
|---|---|---|---|
| `"Updating script files...\n"` | string-pool `005d7610` | TTextBar local-print body | recon `_data.txt:86611-86612` |
| `"Updating script files..."` | string-pool `005d762c` | TTextBar MP/network broadcast body | recon `_data.txt:86616-86617` |
| `"Reloading rules files...\n"` | string-pool `005d7648` | TTextBar local-print body | `_data.txt:86621-86622` |
| `"Reloading rules files..."` | string-pool `005d7664` | TTextBar MP broadcast body | `_data.txt:86626-86627` |
| `"exitgameyn"` | string-pool `005d7680` | message-key passed to `FUN_0053c060` → looked up in `DAT_0065d4d0` english-strings table | `_data.txt:86631-86632`; disasm `0x47ca66` |
| `EXITGAMEYN "\nAre you sure you want to exit the game and return to Windows?"` | `data/resources_unzipped/english.def` | the body string that "exitgameyn" resolves to | (grep result, line near 1080) |
| `"ss.bmp"` | string-pool `005d7608` (used at `0x47c7ff`) | screenshot filename — F12 case | `_data.txt:86606-86607` |
| `"ss.bmp"` | string-pool `005d768c` (used at `0x47cb51`) | identical "ss.bmp" — ESC-alt screenshot case | `_data.txt:86636-86637` |
| popup engine + `popup.def` + `Background` 398×212 | popuptex/popupnotex/popupalpha .dat | the chrome the exitgameyn popup paints into | `PopupDef_SPEC.md` §2 (already spec'd) |
| TTextBar overlay surface + Arial-12 "Small" font | shared HUD font | the surface the script/rules messages appear on | `TTextBar_SPEC.md` (already spec'd) |
| `InGameMenu` DEF chrome | `ingamemenu.def` | the alternate ESC target (when in-game menu NOT yet open) | `InGameMenuDef_SPEC.md` (already spec'd) |

> Note: there is NO new bitmap or new font for these debug paths. They
> compose entirely out of three existing UI subsystems — TTextBar log,
> PopupDef, InGameMenu — plus a generic raster-grab to a `.bmp` file.

## §3 — Coordinate frames & surfaces

TimerTick does no direct drawing. The "frames" relevant to this spec
are those of the systems it INVOKES; this section enumerates them for
the reconstruction agent's reference and points to the owning spec for
the literal numbers.

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer TL (root) | n/a | `(0, 0)` | display 640×480 stock; `0x280 = 640` × `0x1e0 = 480` (pushed at `0x47c761` / `0x47c766`) | disasm `0x47c761,0x47c766` |
| **popup-pane** (exitgameyn) | screen | TL | screen `(0x81, 0x7a) = (129, 122)` | the 398×212 `Background` popup chrome | `PopupDef_SPEC.md` §3 (frame table) |
| **popup-panel-local** (`yesno`) | popup-pane | TL | `(0, 0)` in pane | "message" TEXT at `(32,60)` w=306 h=72; "yes" button `(84,139)` 93×27; "no" button `(201,139)` 93×27 | `PopupDef_SPEC.md` §4 `yesno` table |
| **TTextBar fg-local** | TTextBar screen rect (1280-wide rect at the foreground origin set by `Initialize`) | TL | `(4, 9)` (text body origin inside the fg surface) | one printable line of script/rules message | `TTextBar_SPEC.md` §3 ("fg-local (4,9) stacking by lineH") |
| **InGameMenu-pane** | screen | (per `ingamemenu.def`) | (see `InGameMenuDef_SPEC.md` §3) | the in-game-menu chrome opened by ESC when `+0x5d8 == 0` | `InGameMenuDef_SPEC.md` §3 |
| **screenshot-buffer** | n/a (offscreen) | n/a | `0x280 × 0x1e0` (640 × 480) | the raster-grabbed display copy that `FUN_004a2960("ss.bmp", 3)` writes to disk | disasm `0x47c761`,`0x47c766`,`0x47c804` |

**Composition formula.** TimerTick itself does not paint; the chip-model
composition for each consumer is in that consumer's spec:
- exitgameyn message body: `screen = (129,122) + (32,60) + in-cell` — see
  `PopupDef_SPEC.md` §3.
- script/rules TTextBar entries: `screen = fg.origin + (4, 9 + line*lineH)`
  — see `TTextBar_SPEC.md` §3.

**Surfaces.** Two are created locally by TimerTick:
- The **screenshot raster-grab surface** at case F12 and case ESC-alt:
  allocated as `FUN_004a1ec0(0x280, 0x1e0, mode)` (`0x47c76b` / `0x47cab9`)
  where `mode = (display_fmt!=0xf) ? 4 : 2` (the 2/4 ternary at
  `0x47c753..0x47c75d` — picks RGB565 vs RGB555 to match the display).
  Wrapped in a `&PTR_FUN_005a3ee4` (vtable for `TBitmap` family;
  `0x47c79f` / `0x47caf1`), and freed via `FUN_004830f0` (`0x47c818` /
  `0x47cb6a`). The grab itself is the standard
  `FUN_00438d80` `SDrawParam` init + `(*surface+0x5c)` ParamBlit
  (UI_METHOD_MAP §1, §8.1, §15a).
- The **popup pane** when the exitgameyn flow runs: allocated and pumped
  by `FUN_0053c060`/`FUN_0053bf00`/`FUN_0048f040` — see
  `PopupDef_SPEC.md` §6.3.

### Layout diagram — exitgameyn (when ESC + `+0x5d8 != 0`)

The exitgameyn popup is the standard `popup.def` `yesno` panel (398×212
chrome at screen `(129, 122)`):

```
screen 0,0                                                                   screen 640,0
├────────────────────────────────────────────────────────────────────────────┤
│  (frozen screenshot of last-frame display, blitted as backdrop -- BUT     │
│  only on the ESC-alt path; the exitgameyn-direct path does NOT screenshot)│
│                                                                            │
│           ┌────────────────────────────────────────────────────────┐ ◀── popup-pane at (129,122)
│           │ ┌──────────────────────────────────────────────────┐    │   398×212 Background chrome
│           │ │ TEXT "message" cell (32,60) 306×72                │   │
│           │ │   body = lookup("EXITGAMEYN")                     │   │
│           │ │   = "\nAre you sure you want to exit the game     │   │
│           │ │      and return to Windows?"                       │   │
│           │ │   font = "Med" TEXT_CENTER TEXT_SHADOW             │   │
│           │ └──────────────────────────────────────────────────┘    │
│           │                                                          │
│           │   ┌─────────────────┐               ┌─────────────────┐ │
│           │   │  "Yes"  93×27   │               │  "No"   93×27   │ │ Row y=139
│           │   │  panel-local    │               │  panel-local    │ │
│           │   │    (84,139)     │               │    (201,139)    │ │
│           │   └─────────────────┘               └─────────────────┘ │
│           │                                                          │
│           └──────────────────────────────────────────────────────────┘
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

All literal values in this diagram are CITED to `PopupDef_SPEC.md` §4 and
§6.1 (the `0x81,0x7a,0x18e,0xd4` pushed by `LoadDef_popup`).

### Layout diagram — TTextBar messages (script/rules reload)

The `"Updating script files..."` and `"Reloading rules files..."` strings
are *not* a new panel — they are inserted at the head of TTextBar's
scrollback ring (the `0x5c`-byte line records, see `TTextBar_SPEC.md` §9).
Visually they appear as one line in the message overlay, top-of-screen,
with the standard per-line 120-tick alpha fade and stacking:

```
TTextBar fg-local (origin = TTextBar.fg.tl in screen):

  (4, 9 + 0*lineH)  ▸ "[older message N-3]"               [fading]
  (4, 9 + 1*lineH)  ▸ "[older message N-2]"               [fading]
  (4, 9 + 2*lineH)  ▸ "Reloading rules files..."          ◀── newest
  (4, 9 + 3*lineH)  ▸                                         (fresh, alpha=255)
                       ⋮
```

(See `TTextBar_SPEC.md` §3, §8 — the print API `FUN_0054d170(0x65c5d0,
fmt, ...)` pushes a record to the head of the scrollback ring at the
shared font and color; TimerTick is just one of many producers.)

### Layout diagram — F12 screenshot (no on-screen UI)

F12 takes a 640×480 raster grab of the live display and writes it to
`ss.bmp` in the working directory. There is **no overlay** drawn — the
visible "before/after" is the disk file. The grab is debounced by a
sentinel at `this+0x6bc` (set to 1 after a successful grab, cleared at
the next F12 tick — see §5 case 0x7b).

## §4 — Static element layout

**Not applicable** — TimerTick paints no static elements of its own.
All visible artifacts are owned by another spec:

- exitgameyn popup chrome + buttons + message → `PopupDef_SPEC.md` §4
  (`yesno` table).
- script/rules messages → `TTextBar_SPEC.md` §4 / §8 (the per-line text
  cells).
- ESC-alt InGameMenu → `InGameMenuDef_SPEC.md` §4.
- F12 / ESC-alt screenshot → file write only, no element layout.

The reconstruction agent must read the consumer spec for layout; this
spec only specifies WHICH consumer each VK case invokes.

## §5 — Draw order / composition (per case)

TimerTick is a dispatcher, not a paint method. "Draw order" here means
"what TimerTick CALLS, in source order, per case." The literal disasm
flow for each case is below; cross-reference §6 for the per-case
pseudocode.

### Top-of-function (every call)

```
0x47c630..0x47c682  prologue + SEH frame (push ExceptionList +
                    pvStack_c at -0x1, push 0x59d442 etc.)
0x47c656            mov ebx, ecx               ; this = ecx
0x47c664            mov esi, [esp+0xd0]        ; ch  = arg2
0x47c66b            mov edi, [esp+0xcc]        ; key = arg1
0x47c672..0x47c679  if (this->+0x5d4 != 0 && key != 0x50) → jmp default
                    ; (a modal-active block; key==0x50 has a special
                    ;  re-dispatch path further down — see case 0x50)
0x47c67f..0x47c688  push ch; push key; mov ecx, this
                    call FUN_00490660           ; TScreen::TimerTick
                                                ; (slot-12 base)
0x47c68a            if (ch == 0) → jmp default  ; skip the dev switch,
                                                ; fall through to keymap.
                                                ; In practice keymap
                                                ; entries require a char,
                                                ; so ch=0 → no dispatch.
0x47c690..0x47c69d  if (this->+0x1c > 0 &&
                       this->[+0x2c + 4*+0x1c] != 0) → jmp default
                    ; a modal child-pane is active — bail.
0x47c6a3..0x47c6b7  switch index:
                      ecx = key - 0x1b
                      if (ecx > 0x60) → jmp default
                      dl  = byte[ecx + 0x47cd0c]    ; idx table
                      jmp [4*edx + 0x47cce4]        ; jumptable
```

### The jumptable @ `0x47cce4`
Decoded from binary (the data immediately following the `ret 8`):

| idx | handler addr | case role |
|---|---|---|
| 0 | `0x0047ca1d` | key `0x1b` (ESC) — exit confirm OR in-game menu |
| 1 | `0x0047cb7e` | key `0x50` ('P') — pause toggle / dev rules-reload variant |
| 2 | `0x0047c876` | key `0x52` ('R') — MapPane reload (see §6.3b) |
| 3 | `0x0047c9b8` | key `0x58` ('X') — debug render flag toggle |
| 4 | `0x0047c8ed` | key `0x75` (VK_F6) — reload SCRIPT files |
| 5 | `0x0047c94d` | key `0x76` (VK_F7) — reload RULES files |
| 6 | `0x0047c6be` | key `0x78` (VK_F9) — debug player stat-restore (3 vtable calls) |
| 7 | `0x0047c82f` | key `0x7a` (VK_F11) — debug player flag toggle (see §6.3) |
| 8 | `0x0047c6f9` | key `0x7b` (VK_F12) — screenshot to `ss.bmp` |
| 9 | `0x0047cc08` | DEFAULT (fall through to keymap-resolve) |

### The index table @ `0x47cd0c`
Indexed by `(key - 0x1b)`. Length 0x61 (covers VK 0x1b..0x7b). Every
entry not listed in the §5 table above maps to index 9 (default). The
relevant non-default entries (decoded from raw binary at file offset
`0x7cd0c` + (key-0x1b)):

| key (hex) | key (dec) | VK name | idx | handler |
|---|---|---|---|---|
| 0x1b | 27  | VK_ESCAPE | 0 | 0x0047ca1d |
| 0x50 | 80  | 'P'       | 1 | 0x0047cb7e |
| 0x52 | 82  | 'R'       | 2 | 0x0047c876 |
| 0x58 | 88  | 'X'       | 3 | 0x0047c9b8 |
| 0x75 | 117 | VK_F6     | 4 | 0x0047c8ed |
| 0x76 | 118 | VK_F7     | 5 | 0x0047c94d |
| 0x78 | 120 | VK_F9     | 6 | 0x0047c6be |
| 0x7a | 122 | VK_F11    | 7 | 0x0047c82f |
| 0x7b | 123 | VK_F12    | 8 | 0x0047c6f9 |

Every other value in 0x1b..0x7b → idx 9 = default branch → falls
through to the keymap dispatcher at `0x47cc08..0x47ccc7`.

### Default branch @ `0x47cc08` — keymap dispatch

```
0x47cc08..0x47cc14  if (DAT_00668154 != 0) → jmp return-from-func
                    ; (multiplayer hosts skip the local keymap)
0x47cc15..0x47cc24  read eax = this->[+0x2c + 4*(this->+0x1c)]
                                  (or 0 if +0x1c <= 0);
                    if (al & 0x02) → jmp ret
                    ; the al&0x02 test is unusual on a pointer; the
                    ; child-pane slot at +0x2c appears to be a tagged
                    ; word (UNCONFIRMED, see §14 item P).
                    ; Net effect: "if the topmost child pane has tag
                    ; bit 0x02 set, consume input here without keymap."
0x47cc2c..0x47cc34  esi = DAT_00667fcc (player)
                    if (esi == 0) → arg3 = 1 (no player → "normal mode")
0x47cc36..0x47cc8b  derive `modeFlags` from player state:
                      if (player.actionblock[0] == 3) modeFlags = 2
                      elif (player.actionblock[0] == 0x19) modeFlags = 4
                      elif (TActionBlock_Is("sneak")) modeFlags = 8
                      else modeFlags = 1
0x47cc92..0x47cca8  reload edx=ch (esp+0xd0), esi=key (esp+0xcc),
                    push modeFlags, push edx, push esi
                    mov ecx, 0x65a9a8                 ; keymap singleton
                    call FUN_00439150_KeyDispatch     ; → eax = cmd
0x47ccad..0x47ccaf  if (eax == 0) → jmp ret           ; no binding
0x47ccb1..0x47ccbe  if (this->+0x5d4 != 0 && key != 0x50) → jmp ret
                    ; modal-active guard re-checked AFTER keymap
0x47ccc0..0x47ccc5  push cmd; mov ecx, this; call [this+0x4c]
                    ; vtable slot 0x4c = Command dispatcher
                    ; (= FUN_0047cf40_TPlayScreen_DispatchCommand at runtime)
0x47ccc8..0x47cce0  pop ExceptionList, restore esp, ret 8
```

> So the dev/system switch and the gameplay binding table cooperate:
> dev keys preempt; everything else flows through the keymap → command
> dispatcher pair. The `0x50` exception means **'P' can both be handled
> here (modal-pause) AND, on second pass through this dispatch, route
> to the keymap-resolved command** — see case 0x50 in §6.

## §6 — Algorithms (pseudocode per case)

Each case in source order. Cite the disasm range, the gates, the
side effects, and the existing-system spec the case invokes.

### 6.1 Case 0x1b (VK_ESCAPE) — `0x47ca1d..0x47ca8f` + alt `0x47ca94..0x47cb6f`

Two sub-paths depending on the **InGameMenu-is-open** flag `this+0x5d8`.

```c
case 0x1b:
  // 0x47ca1d-0x47ca25
  if (DAT_00668154 != 0)                  // MP host? — skip
    goto skip_to_dispatch;                // (jumps mid-default-branch
                                          //  to do nothing local; the
                                          //  MP host doesn't get
                                          //  exitgameyn — server quits)

  // 0x47ca50-0x47ca58
  if (this->+0x5d8 != 0) {
    // PATH A — InGameMenu is currently up; ESC offers exit confirm.
    FUN_0049c830(0x667548);               // SaveCursorPos
                                          // (0x47ca5a-5f)
    int result = FUN_0053c060(
        /*flags=*/ 3,                     // bit0=chrome_flags=0,
                                          // bit1=yesno panel,
                                          // bit4(=0)=lookup msg by key
        /*msg_arg=*/ "exitgameyn");       // → english.def EXITGAMEYN
                                          // = "Are you sure you want
                                          //    to exit the game and
                                          //    return to Windows?"
                                          // (0x47ca64-6b; PopupDef_SPEC
                                          //  §6.3)
    FUN_0049c890(0x667548);               // RestoreCursorPos
                                          // (0x47ca73-7a)
    if (result == 0) break;               // No/Cancel — stay in game
                                          // (0x47ca81)
    (*DAT_005a3298)(0);                   // Yes — call PostQuitMessage()
                                          // or equivalent shutdown
                                          // (0x47ca87-89). The literal
                                          // is an indirect call through
                                          // 0x5a3298 (UNCONFIRMED-B —
                                          // likely WinMain quit hook)
  } else {
    // PATH B — InGameMenu is NOT up; ESC opens it (with a
    // pre-screenshot so the menu renders over a frozen frame).
    // 0x47ca94-0x47cb6f
    screenshot_to_ss_bmp();               // see §6.6
    FUN_0047e500(this);                   // = ShowInGameMenu, opens
                                          // ingamemenu.def at the
                                          // global DAT_0066f748
                                          // (0x47cb72-74; see
                                          //  InGameMenuDef_SPEC.md and
                                          //  recon agent_ui_wave2_
                                          //  defengine.txt:281
                                          //  FUN_00537110_LoadDef_
                                          //  ingamemenu)
  }
  break;
```

### 6.2 Case 0x50 ('P') — `0x47cb7e..0x47cbfe`

```c
case 0x50:
  // Tier-1: dev mode hot-reload of rules
  // 0x47cb7e-0x47cba8
  if (DAT_0065c9e0 != 0) {              // dev-mode flag (see §13)
    FUN_0048b990(0x65d7a8);             // rules.subsystem.Stop()
    FUN_0048cab0(0x65d7a8);             // rules.subsystem.ReloadFromDisk()
                                        // (this is the SAME rules-reload
                                        // pair called by F7; difference
                                        // is the absence of any
                                        // TTextBar print — silent
                                        // reload, see UNCONFIRMED-D)
    FUN_0045a680(0x10, 0);              // MapPane.refresh(flags=0x10)
                                        // — same finish as F7
                                        // (0x6668d8 = MapPane global)
    break;
  }

  // Tier-2: pause/resume toggle (gated by lots of state)
  // 0x47cbac..0x47cbf7
  if (DAT_00668154 != 0) return;        // MP host — no-op
  if (DAT_0066829c != 0) goto dispatch; // saving-in-progress — fall
                                        // through to keymap (so 'P'
                                        // can still be a gameplay
                                        // binding during a save)
  if (this->+0x1c > 0 &&
      (this->[+0x2c+4*+0x1c] & 0x02))   // child-pane consumes
    goto dispatch;
  if (DAT_0065c670 != 0)                // some global "key blocked"
    goto dispatch;                      // flag — skip

  if (this->+0x5d4 != 0) {              // currently paused?
    FUN_0049c890(0x667548);             // RestoreCursorPos
    this->+0x5d4 = 0;                   // clear pause
  } else {
    FUN_0049c830(0x667548);             // SaveCursorPos
    this->+0x5d4 = 1;                   // engage pause
  }
  break;
```

> Note the **two distinct semantics** for 'P':
> - In dev builds, 'P' is a silent rules-reload (no TTextBar print).
> - In retail/non-dev, 'P' toggles the screen's pause flag (`+0x5d4`)
>   and save/restores the cursor position.
> The `0x65c9e0` gate likely keys off a build/launch arg (see §14
> UNCONFIRMED-C).

### 6.3 Case 0x7a (VK_F11) — player flag `0x100000` toggle — `0x47c82f..0x47c875`

```c
case 0x7a:
  // 0x47c82f-0x47c844
  if (DAT_0065c9e0 == 0) break;           // dev-mode gate
  if (DAT_00667fcc == 0) break;           // no player → no-op
  uint32_t* flags = (uint32_t*)(player + 0x110);
  if (*flags & 0x100000) {                // 0x47c855
    *flags &= ~0x100000;                  // 0x47c85b-60: clear
  } else {                                // 0x47c86b
    *flags |=  0x100000;                  //              set
  }
  break;
```

> The bit `0x100000` in `character+0x110` is a single-bit player-state
> flag in the character class. Candidate meanings: god-mode /
> no-clip / unkillable / debug-render-bounding-box. The per-bit
> semantics of `+0x110` are NOT extracted here — this is the char
> class's "flags" word. See UNCONFIRMED-F.

### 6.3b Case 0x52 ('R') — MapPane reload + dev-host-list refresh — `0x47c876..0x47c8ec`

```c
case 0x52:
  // 0x47c876-0x47c889
  if (DAT_0065c9e0 == 0) break;           // dev-mode gate
  if (DAT_00668130 == 0) break;           // game-loaded gate

  // 0x47c890-0x47c899: an unconditional MapPane call
  FUN_004546a0(/*this=*/(TMapPane*)0x6668d8);
                                          // = MapPane method @ 0x4546a0
                                          //   (UNCONFIRMED — likely a
                                          //   "rebuild walkmap cache"
                                          //   or "force visibility
                                          //   refresh"; semantically
                                          //   sibling to FUN_0045a680
                                          //   used by F7)

  // 0x47c89a-0x47c8a1: MP gate
  if (DAT_00668154 != 0) {                // MP-host?
    // 0x47c8a7-0x47c8d1: walk script-script-list (0x656f00/0x656f10),
    // check a per-entry flag bit (>>16 & 1) on the top entry's +0x14;
    // if set → bail. Otherwise fall through to the MapPane "reset" pair.
    if (script_list_entry_top_bit()) break;
  }

  // 0x47c8d4-0x47c8e7: MapPane reset pair
  FUN_00453390((TMapPane*)0x6668d8);      // ResetState (UNCONFIRMED)
  FUN_00453320((TMapPane*)0x6668d8);      // RecomputeMesh? (UNCONFIRMED)
  break;
```

> 'R' is **NOT** the player-flag toggle — that's F11 only. 'R' is a
> MapPane "reload/refresh" hotkey that re-runs the MapPane's state
> setup (likely the same setup `0x45a680` is the public face of, but
> called more directly). The MP-side path is more conservative —
> only proceeds when the top script entry's bit `0x10000` is clear.
>
> UNCONFIRMED-E1 — confirm the FUN_0045a680 vs FUN_00453390/00453320
> difference (probably "fast vs full" reload).
>
> UNCONFIRMED-E2 — the script-list walk uses `0x656f00` (count) and
> `0x656f10` (table base, entries at `+0x28`) — these need
> identification. The `eax+0x14 >> 0x10 & 1` test is "top bit-0 of the
> high-word of the entry's field 0x14".

### 6.4 Case 0x58 ('X') — `0x47c9b8..0x47ca18`

```c
case 0x58:
  // 0x47c9b8-0x47c9d1
  if (DAT_0065c9e0 == 0) break;           // dev-mode
  if (DAT_00668130 == 0) break;           // rules loaded
  uint32_t state = DAT_006671f0;

  // 0x47c9d7-0x47ca0e: 2-state toggle on DAT_006671f0
  if (state != 0) {                       // currently "on" → reset
    DAT_006671f0 = 0;
    ((TMapPane*)0x6668d8)->vtable[+0x2c](1); // (0x47c9ff-0c)
  } else if (state == 2) {                // already 2 → noop
    DAT_006671f0 = 2;                     // (jump @ 0x47c9f0; the
                                          //  cmp eax,2 path is a
                                          //  fall-through guard)
  } else {                                // 0x → engage
    ((TMapPane*)0x6668d8)->vtable[+0x2c](1); // (0x47c9e0-ed)
    DAT_006671f0 = 2;
  }
  break;
```

`0x6668d8` is the **MapPane** global (cross-confirmed in
`recon/discovered/renames/agent_walkmap.txt:11`). Vtable slot `+0x2c`
on the map-pane is the toggle target — likely a "debug overlay" or
"walkmap render" toggle (UNCONFIRMED-G).

### 6.5 Case 0x75 (VK_F6) — script reload — `0x47c8ed..0x47c948`

```c
case 0x75:
  if (DAT_0065c9e0 == 0) break;           // dev-mode  (0x47c8ed-f4)
  if (DAT_00668130 == 0) break;           // game loaded
                                          // (0x47c8fa-01)
  if (DAT_00668154 != 0) {                // is MP-host  (0x47c907-0e)
    // MP path: printf-style to local stdout/log buffer
    FUN_0041ee50("Updating script files...\n");
                                          // (0x47c910-1a; addr
                                          //  005d7610)
  } else {
    // local-print path → TTextBar message log
    FUN_0054d170(&DAT_0065c5d0,           // = TTextBar singleton
                 "Updating script files..."); // (005d762c)
                                          // (0x47c92c-3b)
  }
  // Subsystem reload, regardless of print path:
  ((TScriptManager*)0x65def0)->Reload();  // FUN_004970b0(0x65def0)
                                          // (0x47c91d-22 / 47c93e-43;
                                          //  cls_TScriptManager_ParseScripts_
                                          //  496860.cpp confirms 0x65def0
                                          //  is the TScriptManager
                                          //  singleton)
  break;
```

> `FUN_0041ee50` and `FUN_0054d170` are **both message-log producers**.
> Cross-reference: TScriptManager itself uses the *same pair* via the
> same `DAT_00668154 ? ee50 : 54d170` switch — see
> `cls_TScriptManager_ParseScripts_496860.cpp:249-256` printing
> `"Updated '%s' script"`. So this case 0x75 is "fire the script
> reload, push a heads-up into the same message channel TScriptManager
> already uses on successful reload."
>
> The TWO different string variants (`005d7610` with `\n`, `005d762c`
> without) are an artifact of the two output paths: the printf-style
> log gets the `\n`-terminated string, the TTextBar wrapper consumes
> a no-`\n` string and adds its own line break.

### 6.6 Case 0x76 (VK_F7) — rules reload — `0x47c94d..0x47c9b3`

```c
case 0x76:
  if (DAT_0065c9e0 == 0) break;           // dev-mode  (0x47c94d-54)
  if (DAT_00668130 == 0) break;           // game loaded
                                          // (0x47c95a-61)
  if (DAT_00668154 != 0) {
    FUN_0041ee50("Reloading rules files...\n");
                                          // (0x47c970-7a; 005d7648)
  } else {
    FUN_0054d170(&DAT_0065c5d0,           // TTextBar singleton
                 "Reloading rules files..."); // (005d7664)
                                          // (0x47c97f-8e)
  }
  // Rules reload pair:
  ((TRulesSubsystem*)0x65d7a8)->Stop();   // FUN_0048b990
                                          // (0x47c991-96)
  ((TRulesSubsystem*)0x65d7a8)->ReloadFromDisk(); // FUN_0048cab0
                                          // (0x47c99b-a0)
  // Refresh the map so loaded rules take visual effect:
  ((TMapPane*)0x6668d8)->refresh(0x10, 0);// FUN_0045a680(0x10, 0, this=0x6668d8)
                                          // (0x47c9a5-ae)
  break;
```

> `0x65d7a8` is the rules subsystem singleton (UNCONFIRMED-H — the
> only evidence is the call pairing 0x48b990/0x48cab0; the class is
> not named in current renames).

### 6.7 Case 0x78 (VK_F9) — player stat-restore — `0x47c6be..0x47c6f8`

```c
case 0x78:
  if (DAT_00667fcc == 0) break;           // no player
                                          // (0x47c6be-c6)
  if (DAT_00668130 == 0) break;           // game loaded
                                          // (0x47c6cc-d3)
  // Three sequential vtable calls on the player character:
  TCharacter* p = (TCharacter*)DAT_00667fcc;
  p->vtable[+0x1b4](p, 0);                // FUN_004c5880(0)
                                          // (0x47c6d9-de)
  p->vtable[+0x1c4](p,                    // FUN_004c58b0
        p->vtable[+0x1d8](p));            //   (gets bar source-rect getter
                                          //    first, passes its result)
                                          // (0x47c6de-eb)
  p->vtable[+0x1d0](p);                   // FUN_004c58d0
                                          // (0x47c6e9-f3)
  break;
```

The vtable offsets `+0x1b4`, `+0x1c4`, `+0x1d0`, `+0x1d8` are on the
**character object** (compare with UI_METHOD_MAP §15b: `+0x1c0`=Health,
`+0x1c8`=Fatigue, `+0x1d0`=Mana, `+0x1d8`=HP-bar-getter). So these
three calls are **stat-modifier methods adjacent to the Health/Fatigue/
Mana getters** — almost certainly the **"set max" / "restore to max"**
setters for the player's three pools. Net effect: F9 fully restores
HP/MP/FT to max. (UNCONFIRMED-A — confirm by extracting the three
vtable slot bodies; the structural placement is highly indicative but
not literally confirmed.)

### 6.8 Case 0x7b (VK_F12) — screenshot — `0x47c6f9..0x47c82e`

Visual output is **disk-only** (`ss.bmp` in the working directory). The
"overlay" wording in the task is a misnomer for this case.

```c
case 0x7b:
  // 0x47c6f9-0x47c732 — gate checks
  if (DAT_0066829c != 0) break;          // saving-in-progress
  if (this->+0x6b0 != 0) break;          // pane-state field
  if (this->+0x6b4 != 0) break;          // pane-state field
  if (DAT_0065b488 != 0 &&
      DAT_0066812c == 0) break;          // some MP / script-pump
                                         // combined gate

  // Debounce sentinel: if non-zero, clear and bail. Means F12 can
  // only fire once per "release" — the next tick clears the
  // flag so a fresh press takes a fresh screenshot.
  if (this->+0x6bc != 0) {               // (0x47c734-3a)
    this->+0x6bc = 0;
    break;
  }

  // 0x47c747-0x47c81c — the raster grab.
  TSurface* display = (TSurface*)DAT_005d79e0;
  int fmt = display->vtable[+0x18](0);   // GetPixelFormat()
                                         // (0x47c74e-50)
  int alloc_mode = (fmt != 0xf) ? 4 : 2; // pick RGB565 vs RGB555
                                         // (0x47c753-5d)
  void* backing = FUN_004a1ec0(0x280, 0x1e0, alloc_mode);
                                         // alloc 640×480 raster
                                         // (0x47c761-6b)

  TBitmap* shot = (TBitmap*)FUN_00482fb0(0x6c); // alloc TBitmap obj
                                         // (0x47c770-79; size 0x6c)
  if (shot != 0) {
    FUN_004bcb00(shot);                  // TBitmap ctor
    shot->vtable = &PTR_FUN_005a3ee4;    // (0x47c79f; PTR_FUN_005a3ee4
                                         //  = the TBitmap vtable)
    FUN_004a3a40(backing);               // attach backing surface
                                         // (0x47c7a5)
  }

  display->vtable[+0x24]();              // Lock() / begin-frame
                                         // (0x47c7c1)

  // The raster-grab: blit display → backing surface via the standard
  // shadow-descriptor init + (*+0x5c) ParamBlit (UI_METHOD_MAP §1+§8.1).
  SDrawParam dp;
  FUN_00438d80(&dp,
               0, 0,                     // shadow (dx,dy) = (0,0)
                                         //   i.e. no displacement
               0, 0,                     // src x,y = (0,0)
               display->width,           // = [edi+4]
               display->height,          // = [edi+8]
               0x80000000);              // drawmode DM_USEDEFAULT
                                         //   (= opaque copy)
                                         // (0x47c7c4-e4)
  shot->surface->vtable[+0x5c](&dp,      // ParamBlit: src=display,
       display, 0, 0);                   //   dst=shot's backing
                                         // (0x47c7e9-fa)

  // Write to disk:
  FUN_004a2960("ss.bmp", 3);             // shot->SaveToFile("ss.bmp",
                                         //   format=3 /*BMP*/)
                                         // (0x47c7fd-06; literal at
                                         //  005d7608)

  if (shot != 0)
    shot->vtable[+0x0](shot, 1);         // destructor (deleting=1)
                                         // (0x47c80f-15)
  FUN_004830f0(backing);                 // free raster buffer
                                         // (0x47c817-18)

  this->+0x6bc = 1;                      // mark "shot taken"
                                         // (0x47c820-29)
  break;
```

**This case is NOT a UI panel.** It writes one PNG-style file (actually
BMP at format=3) to disk and sets a sentinel. The "before/after" is the
file, not the screen. The reconstruction agent should wire it as
`Renderer->CaptureDisplayToFile("ss.bmp")` — see §13.

### 6.9 The TTextBar print primitive `FUN_0054d170`

Pseudo-signature (from §6.5/§6.6 call shapes and TTextBar_SPEC.md
references):

```c
void FUN_0054d170(TTextBar* bar /*= &DAT_0065c5d0*/,
                  const char* fmt, ...);
```

Behavior (inferred — UNCONFIRMED-I; the body has not been extracted):
formats `fmt + args` into a stack buffer via the sprintf-family helper
(UI_METHOD_MAP §10 `FUN_0058b100` is a candidate), then pushes the
resulting C-string into TTextBar's scrollback ring (the `0x5c`-byte line
records described in `TTextBar_SPEC.md` §9) with the default channel
color, default lifetime (120 ticks), and the standard 3-pass shadow.

For these cases there are no `%` substitutions — the format strings are
literal — so the buffer write is effectively `bar.PrintLine(literal)`.

### 6.10 The screenshot primitive (case F12 + case ESC-alt)

Both call sites share the same body. The same idiom recurs in
`FUN_0047cf40_TPlayScreen_DispatchCommand` case 0x54 (savegame) and
case 0x55 (autosave) at `0047cf40:480-531` — verified identical
(alloc 0x280×0x1e0 raster, FUN_00438d80 init, (*+0x5c) blit,
`FUN_004a2960("ss.bmp", 3)`, free). So the screenshot is reused
**every time a save-or-modal screen is about to take over the display**
— the modal renders over the frozen frame instead of black/empty.

Pseudo:

```c
void TPlayScreen::ScreenshotToSsBmp() {
  TSurface* display = (TSurface*)DAT_005d79e0;
  int fmt = display->vtable[+0x18](0);
  void* backing = FUN_004a1ec0(640, 480, (fmt!=0xf)?4:2);
  TBitmap* shot = ConstructBitmap();
  AttachBacking(shot, backing);
  display->vtable[+0x24]();                  // Lock
  SDrawParam dp;
  FUN_00438d80(&dp, 0,0, 0,0, display->w, display->h, DM_USEDEFAULT);
  shot->surface->vtable[+0x5c](&dp, display, 0, 0);
  shot->SaveToFile("ss.bmp", BMP);           // FUN_004a2960
  delete shot;
  FUN_004830f0(backing);
}
```

The reconstruction agent should expose this as
`Renderer->CaptureDisplayToFile("ss.bmp")` — see §13.

## §7 — Effects & shadows

**Not applicable to TimerTick itself.** The visible artifacts are
either:
- TTextBar lines (3-pass black shadow per font flag `0x400` — see
  `TTextBar_SPEC.md` and UI_METHOD_MAP §5 step 8), OR
- the popup chrome (no shadow on the popup body — the DEF widget
  engine handles per-widget effects, see `PopupDef_SPEC.md` §7), OR
- the InGameMenu chrome (see `InGameMenuDef_SPEC.md` §7), OR
- nothing at all (the screenshot writes to disk).

No new shadow / chroma / glow is added by TimerTick.

## §8 — Text rendering

**Not applicable** — TimerTick draws no text. The texts seen on screen
come from the TTextBar (script/rules messages) and the popup (exitgameyn
body) which are owned by their respective specs:

| string | owned by | cite |
|---|---|---|
| `"Updating script files..."` | TTextBar message log | `TTextBar_SPEC.md` §8 + §3 |
| `"Reloading rules files..."` | TTextBar message log | `TTextBar_SPEC.md` §8 + §3 |
| `"\nAre you sure you want to exit the game and return to Windows?"` | PopupDef "message" TEXT widget | `PopupDef_SPEC.md` §4 (yesno table row "TEXT message") + the body string is the lookup of `"exitgameyn"` in `DAT_0065d4d0` (english.def `EXITGAMEYN`) |

The leading `\n` in EXITGAMEYN is intentional — it produces an empty
first line so the visible body sits in the lower half of the (306×72)
TEXT cell, leaving room at the top for visual breathing space (the DEF
"message" widget is set top-aligned, not v-centered, see
NOMENCLATURE §2 "v-align").

## §9 — Animation & dynamic behavior

The only dynamic state TimerTick owns is the **F12 debounce sentinel**:

```
ramp this->+0x6bc:
  range   0..1
  step    set to 1 immediately on successful screenshot
          (no per-tick advance)
  target  1 after capture; 0 after next F12 tick if still pressed
  drive   F12 case in TimerTick
  on-end  one-shot: cleared on next tick that re-enters case 0x7b
  cite    disasm 0x47c734-3a (clear), 0x47c820-29 (set)
```

This is a simple edge-trigger filter, not a tweened value. It ensures
holding F12 doesn't spam screenshot files — one file per press-release
cycle. **No on-screen visual.**

The exitgameyn popup itself has the standard DEF-screen
modal-pump animation (button hover/press states) which is the popup
engine's responsibility, not TimerTick's. See `PopupDef_SPEC.md` §9.

The screenshot-then-modal flow is essentially: "grab → present the
frozen frame as backdrop (handled by the modal's render path which
draws over whatever is currently composited) → modal pump runs". There
is no fade between live and frozen — it is instantaneous, achieved by
the simple act of NOT clearing the back buffer before drawing the
modal chrome.

## §10 — Input & dispatch (TimerTick is the dispatcher)

TimerTick IS an input handler. This section enumerates the hit/key
table:

| VK code | name | mode/role | gates required | handler | invokes |
|---|---|---|---|---|---|
| 0x1b (27) | VK_ESCAPE | exit-confirm OR show-in-game-menu | `0x668154==0` (not MP host); `+0x5d8` selects sub-path | `0x47ca1d` | `FUN_0053c060("exitgameyn", flags=3)` (PopupDef yesno) OR screenshot + `FUN_0047e500` (InGameMenu) |
| 0x50 ('P') | letter P | hot-reload rules (dev) OR pause toggle | dev-mode for tier-1; lots of "not modal" guards for tier-2 | `0x47cb7e` | `FUN_0048b990` + `FUN_0048cab0` + `FUN_0045a680` (rules subsystem) OR cursor save/restore + `+0x5d4` toggle |
| 0x52 ('R') | letter R | MapPane reload (`0x4546a0` + `0x453390` + `0x453320`) | dev-mode + game-loaded; MP-host has extra script-list gate | `0x47c876` | MapPane fast-refresh + (non-MP) MapPane state reset pair |
| 0x58 ('X') | letter X | toggle MapPane debug-render | dev-mode + rules-loaded | `0x47c9b8` | `MapPane.vtable[+0x2c](1)` + `DAT_006671f0 ^= 2` |
| 0x75 (117) | VK_F6 | reload SCRIPT files | dev-mode + game-loaded | `0x47c8ed` | TTextBar print + `TScriptManager.Reload()` |
| 0x76 (118) | VK_F7 | reload RULES files | dev-mode + game-loaded | `0x47c94d` | TTextBar print + rules reload + MapPane refresh |
| 0x78 (120) | VK_F9 | restore player HP/MP/FT to max (DEBUG cheat) | player exists + game-loaded | `0x47c6be` | 3× character vtable calls (`+0x1b4`, `+0x1c4`, `+0x1d0` w/ result of `+0x1d8`) |
| 0x7a (122) | VK_F11 | toggle player flag `0x100000` | dev-mode + player exists | `0x47c82f` | `*(player+0x110) ^= 0x100000` |
| 0x7b (123) | VK_F12 | screenshot to `ss.bmp` | save-not-in-progress + no busy pane | `0x47c6f9` | grab display → write `ss.bmp` (file output, no UI) |
| every other VK 0x1b..0x7b | — | falls to keymap | (default) | `0x47cc08` | `FUN_00439150` keymap → vtable+0x4c Command dispatcher |
| any VK outside 0x1b..0x7b | — | falls to keymap directly | (default) | `0x47cc08` | same |

**Modal-state globals.** The two relevant `this+...` fields:
- `this+0x5d4` (player-pause flag): set/cleared by case 0x50. While
  non-zero, TimerTick aborts at the very top (except for `key=0xd`
  VK_RETURN, which can still resume).
- `this+0x5d8` (InGameMenu-is-open flag): toggled by `FUN_0047cf40`
  case (the InGameMenu setup paths) — see `cls_0x5a5320_TPlayScreen_
  Initialize_47a660.cpp:87` for the init read. While non-zero, ESC's
  case-0x1b path takes Tier-A (exitgameyn popup); while zero, Tier-B
  (screenshot + open InGameMenu).
- `this+0x6bc` (F12 screenshot debounce): per §9 — one-shot edge filter
  for the screenshot.

**Multiplayer-host gate.** `DAT_00668154` is checked in: case 0x1b
(skip the keystroke entirely as MP-host), case 0x75/0x76 (use printf
fallback `FUN_0041ee50` instead of the TTextBar wrapper because the
TTextBar isn't bound on the dedicated server). This flag is the
"is server" sentinel; the dedicated MP-server has no UI overlay.

**The `(key, ch)` pair.** TimerTick takes BOTH a virtual key code (`key`)
and a character (`ch`). The dev-switch keys are matched against `key`
only. The keymap fallback uses BOTH (to disambiguate, e.g.,
`'Shift+W'` from `'w'`). The `if (ch == 0) → jmp default` short-circuit
at the top means key-DOWN events with no character (modifiers held by
themselves) skip both the switch and the fallback.

## §11 — Retail bugs NOT to reproduce

1. **F12 captures the FROZEN frame, not the live one** — by the time
   `ParamBlit` runs, the display has already drawn this frame's
   overlays (status bars, UI panes, etc.) and is in its "presentation"
   state. The `ss.bmp` shows the gameplay backdrop **with the UI**
   composited on top, which is usually intended. UNCONFIRMED-J — the
   port could optionally offer a "clean-frame" screenshot toggle, but
   that is a deviation, not a retail bug, so keep the default behavior.

2. **The `if (ch == 0) → default` short-circuit drops key-down events
   that arrive without a translated character.** On modern sokol_app /
   Win32 paths this is unusual but possible (e.g., dead-key sequences
   in IME). Document as expected behavior; do NOT add a defensive "if
   ch is zero, treat as the VK literal" path — that would change
   timing semantics and could cause the dev-switch keys to fire on
   modifier-only key-downs.

3. **F12 screenshot has no on-screen feedback.** Retail just silently
   writes the file; there is no "screenshot taken" toast in the
   TTextBar. The port should match — adding a toast would be a UX
   deviation and could mask the debounce behavior. The
   `this+0x6bc` sentinel acts as the only feedback (the next press
   does NOT take a second shot until released).

4. **"Pink halo" — N/A** to TimerTick's direct outputs (it draws no
   text); the TTextBar/popup outputs inherit the well-documented pink-
   halo retail bug if reproduced verbatim. See
   `TPlyrStatusBar_SPEC.md:295` and the project memory entry
   `project_retail_pink_halo_bug`. **Do NOT reproduce — render text via
   `Renderer->DrawTextShadowedToTarget` with real alpha.**

## §12 — Reconstruction pseudocode

```c
// TPlayScreen::TimerTick(VK key, char ch)  — slot 12 override
void TPlayScreen::TimerTick(int32_t key, char ch)
{
    // Modal-state guard: when paused/captured, only RETURN gets through.
    if (this->modalPause != 0 && key != VK_RETURN) {
        TScreen::TimerTick(key, ch);              // chain to base
        return;
    }

    // Base first (TScreen::TimerTick at 0x490660 — slot 12 default).
    TScreen::TimerTick(key, ch);

    // Drop if a child pane already owns input.
    if (this->modalChildOwnsInput()) return;

    // The retail short-circuit at 0x47c688: if ch==0, skip the dev
    // switch and fall straight through to keymap. Modifier-only key-
    // downs would have no char and won't match any keymap entry, so
    // they effectively no-op. (Port equivalent: just don't enter the
    // switch.)
    if (ch != 0) {
      // -- Dev/system shortcut switch --
      switch (key) {
    case VK_ESCAPE:                          // 0x1b
        OnEscapeKey();                       // §6.1
        return;
    case 'P':                                // 0x50
        OnLetterP();                         // §6.2
        return;
    case 'R':                                // 0x52  (§6.3b)
        if (Globals::devMode && Globals::gameLoaded) {
            MapPane()->FastRefresh();        // FUN_004546a0
            if (Globals::isMpHost &&
                ScriptList::TopEntryBlockBit())
                return;                      // MP guard
            MapPane()->StateReset();         // FUN_00453390
            MapPane()->RecomputeMesh();      // FUN_00453320
        }
        return;
    case 'X':                                // 0x58
        if (Globals::devMode && Globals::gameLoaded) {
            MapPane()->vtable_2c(1);
            Globals::xDebugFlag = (Globals::xDebugFlag != 0) ? 0 : 2;
        }
        return;
    case VK_F6:                              // 0x75 — script reload
        if (Globals::devMode && Globals::gameLoaded) {
            TTextBar::Print("Updating script files...");
            TScriptManager::Singleton().Reload();
        }
        return;
    case VK_F7:                              // 0x76 — rules reload
        if (Globals::devMode && Globals::gameLoaded) {
            TTextBar::Print("Reloading rules files...");
            TRules::Singleton().Stop();
            TRules::Singleton().ReloadFromDisk();
            MapPane()->Refresh(/*flags=*/ 0x10);
        }
        return;
    case VK_F9:                              // 0x78 — restore stats
        if (Player() && Globals::gameLoaded) {
            Player()->RestoreHealthToMax();   // vtable +0x1b4
            Player()->RestoreFatigueToMax();  // vtable +0x1c4
            Player()->RestoreManaToMax();     // vtable +0x1d0
            // (verify identities — see UNCONFIRMED-A)
        }
        return;
    case VK_F11:                             // 0x7a — debug flag toggle
        if (Globals::devMode && Player())
            Player()->flags ^= 0x100000;
        return;
    case VK_F12:                             // 0x7b — screenshot
        OnF12Screenshot();                   // §6.8
        return;
      }
    } // end "if (ch != 0)"

    // -- Fallback: keymap → command dispatcher --
    if (Globals::isMpHost) return;           // MP server, no local keymap
    int modeFlags = DeriveModeFlagsFromPlayer();
    GAMECOMMAND cmd = Keymap::Singleton().Resolve(key, ch, modeFlags);
    if (cmd == 0) return;
    if (this->modalPause != 0 && key != 'P') return; // re-check; 'P' can
                                                     // also fire pause
    Command(cmd);                            // = FUN_0047cf40 dispatcher
}

void TPlayScreen::OnEscapeKey()
{
    if (Globals::isMpHost) return;
    if (this->inGameMenuOpen) {              // +0x5d8 != 0
        Cursor::SavePos();
        int result = PopupDef::ShowSync(
            PopupDef::YesNo,                 // = flags 3 (bit1=yesno)
            /*messageKey=*/ "exitgameyn");
        Cursor::RestorePos();
        if (result == /*positive*/ 1)
            Application::Quit();             // call DAT_005a3298
    } else {
        Renderer::CaptureDisplayToFile("ss.bmp");
        ShowInGameMenu();                    // FUN_0047e500
    }
}

void TPlayScreen::OnLetterP()
{
    if (Globals::devMode) {
        TRules::Singleton().Stop();
        TRules::Singleton().ReloadFromDisk();
        MapPane()->Refresh(0x10);
        return;
    }
    if (Globals::isMpHost) return;
    if (Globals::saving) { dispatchToKeymap(); return; }
    if (this->modalChildOwnsInput()) { dispatchToKeymap(); return; }
    if (Globals::keysBlocked != 0) { dispatchToKeymap(); return; }
    if (this->paused) {                      // +0x5d4
        Cursor::RestorePos();
        this->paused = 0;
    } else {
        Cursor::SavePos();
        this->paused = 1;
    }
}

void TPlayScreen::OnF12Screenshot()
{
    if (Globals::saving) return;
    if (this->paneBusy()) return;            // +0x6b0, +0x6b4 checks
    // (other state checks — see §6.8)
    if (this->screenshotPending) {           // +0x6bc != 0 — debounce
        this->screenshotPending = 0;
        return;
    }
    Renderer::CaptureDisplayToFile("ss.bmp");
    this->screenshotPending = 1;
}
```

`Renderer::CaptureDisplayToFile(path)` and `PopupDef::ShowSync(kind,
messageKey)` are the two new shared primitives this spec asks for (see
§13).

## §13 — Port mapping notes

| retail call | port equivalent | home |
|---|---|---|
| `FUN_00490660(this, key, ch)` (TScreen base TimerTick) | `TScreen::TimerTick(key, ch)` | `src/screen.h` |
| `FUN_00439150(0x65a9a8, key, ch, modeFlags)` (keymap resolve) | `Keymap::Resolve(key, ch, modeFlags)` | `src/controlmap.{h,cpp}` (already has `SControlEntry`, `g_defaultGameControls`) |
| `(*this+0x4c)(cmd)` (Command dispatcher) | `TPlayScreen::Command(cmd)` | `src/playscreen.cpp:1037` (currently a stub — task brief: top blocker for gameplay phase A) |
| `FUN_0053c060("exitgameyn", flags=3)` | `PopupDef::ShowSync(PopupKind::YesNo, "exitgameyn")` | `src/popupdef.{h,cpp}` (greenfield — `PopupDef_SPEC.md` §13 lists the missing primitives; treat this as one usage site for the same engine) |
| `FUN_0047e500` (Show InGameMenu) | `InGameMenu::Show()` | `src/ingamemenu.{h,cpp}` (greenfield — `InGameMenuDef_SPEC.md` §13) |
| `FUN_0054d170(0x65c5d0, fmt, ...)` (TTextBar message push) | `TTextBar::Singleton().Print(fmt, ...)` | `src/textbar.{h,cpp}` (greenfield — `TTextBar_SPEC.md` §13) |
| `FUN_0041ee50(fmt, ...)` (printf log) | `log_info(fmt, ...)` (the rxi log facade — `src/logging.h`) | per `project_logging` memory; routes through stdout/log on MP host |
| F12 screenshot ladder (alloc / FUN_00438d80 / `(*+0x5c)` / FUN_004a2960) | `Renderer->CaptureDisplayToFile("ss.bmp")` (NEW primitive) | `src/renderer.{h,cpp}` — needs a new method that wraps "snapshot current back buffer to a `.bmp` file". The retail format=3 corresponds to a 16-bit BMP. |
| `FUN_0049c830(0x667548)` / `FUN_0049c890(0x667548)` (cursor save / restore) | `Cursor::SavePos()` / `Cursor::RestorePos()` | likely `src/window.{h,cpp}` or `src/input.h` (needs identification — see UNCONFIRMED-K) |
| `FUN_004c5880/4c58b0/4c58d0` (F9 stat-restore) | `TCharacter::RestoreHealthToMax()` / `RestoreFatigueToMax()` / `RestoreManaToMax()` (or their actual identities once extracted) | `src/character.{h,cpp}` |
| `FUN_0048b990(0x65d7a8)` / `FUN_0048cab0(0x65d7a8)` (rules reload pair) | `TRules::Stop()` / `TRules::ReloadFromDisk()` | `src/rules.{h,cpp}` (needs identification — see UNCONFIRMED-H) |
| `FUN_004970b0(0x65def0)` (script reload) | `TScriptManager::Reload()` | `src/script.{h,cpp}` |
| `FUN_0045a680(0x10, 0)` on MapPane | `MapPane::Refresh(flags=0x10)` | `src/mappane.{h,cpp}` |

**Two new primitives this spec asks for:**

1. `Renderer->CaptureDisplayToFile(const char* path)` — snapshot the
   current display backing surface to a 16-bit BMP. Used by F12, ESC-alt,
   and the savegame/loadgame paths. The implementation can mirror the
   retail ladder (alloc surface, ParamBlit display → surface, write
   BMP) — modernized to a single direct GPU readback + libpng/stb_image_write.
2. `PopupDef::ShowSync(PopupKind kind, const char* messageKey)` — the
   sync-modal-popup wrapper. ESC's exit-confirm is one call site;
   PopupDef_SPEC has the full primitive list.

**Modernization notes**, per project memory `feedback_modernization_stance`:
- The dev-mode gate (`DAT_0065c9e0`) is a single global bool — in the
  port, prefer `Globals::devMode` keyed off the `--revisited` or
  `--dev` command-line arg (see `project_revisited_settings`).
- The 9-entry switch is fine as a `switch (key)` — no need for a table
  lookup. The retail jumptable is a compiler optimization, not an
  intent.
- F12 should ALSO write to a timestamped file (`ss_<unixtime>.bmp` or
  similar) so repeated shots aren't overwritten — this is a sanctioned
  small modernization that doesn't change retail semantics.
  UNCONFIRMED-L — confirm with user before changing the filename
  convention.

## §14 — UNCONFIRMED / open questions

**A. F9 stat-restore identity (`FUN_004c5880/4c58b0/4c58d0`).** The 3
vtable calls on the player character are structurally aligned with the
Health/Fatigue/Mana getter set (UI_METHOD_MAP §15b: `+0x1c0`=HP getter,
`+0x1c8`=FT, `+0x1d0`=MP). The F9 path calls `+0x1b4`, `+0x1c4`, `+0x1d0`
— so `+0x1b4` and `+0x1c4` are NEW. The naming "restore to max" is
inferred from F9's common debug semantics, but could equally be
"give weapon" / "level up" / "boost xp". **Impact**: a wrong port
binds the F9 cheat to the wrong stat (cosmetically wrong, not
unsafe). **Resolve**: `DecompileAddr.java 0x004c5880`, `0x004c58b0`,
`0x004c58d0` and read what they do to which char field
(`+0x1c0`/`+0x1c8`/`+0x1d0`).

**B. `*(DAT_005a3298)(0)` — the exit hook.** The exitgameyn-positive
path makes an indirect call through `0x5a3298`. The most likely
identities are `PostQuitMessage(0)` (Win32) or
`atexit`-registered shutdown. **Impact**: a wrong port wires
exitgameyn-Yes to the wrong handler (process won't quit, or quits
without saving config). **Resolve**: look up the import-table entry at
`0x5a3298` — `DumpVtable.java` on `0x5a3298` or
`objdump -d --start-address=0x5a3290 --stop-address=0x5a32a0
data/Revenant.exe` will read the import thunk.

**C. Dev-mode flag `DAT_0065c9e0`.** Used as the dev/debug gate by
cases 0x50, 0x52, 0x58, 0x75, 0x76. Whether it is set by a launch
arg (`-debug`, `-dev`), by a registry key, or by an INI flag is not
yet identified. **Impact**: without finding the setter, the port will
either always-enable (security: shipped F12 in retail acts as
screenshot only because of the *separate* `0x66829c` gate; the other
F-keys could leak debug behavior to players) or always-disable (debug
keys won't work for devs). **Resolve**: `FindImmRefs 0x65c9e0` to
locate writers — then trace to the launch-time setter.

**D. 'P' silent-reload vs F7 with-message.** Case 0x50 dev-tier-1
performs the SAME rules-reload pair as F7 but skips the TTextBar
message print. Likely intent: 'P' is a quick-iterate hotkey used
during dev that doesn't spam the message log; F7 is the "official"
reload that confirms in-log. UNCONFIRMED whether there is a second
side-effect 'P' should have over F7 (the calls are identical). **Resolve**:
diff the two case bodies at `0x47cb88..0x47cbaa` vs `0x47c97f..0x47c9b3`
— if structurally identical except for the print, this is just a
silent variant.

**E1. FUN_004546a0 vs FUN_0045a680 (the MapPane refresh siblings).**
'R' (case 0x52, §6.3b) calls `FUN_004546a0(MapPane)` unconditionally.
F7 (case 0x76, §6.6) calls `FUN_0045a680(MapPane, 0x10, 0)` after
the rules reload. Both touch the MapPane; not yet verified whether
`0x4546a0` is "fast invalidate" and `0x45a680` is "full rebuild" or
vice-versa. **Resolve**: extract both bodies (`DecompileAddr.java
0x004546a0` and `0x0045a680`).

**E2. 'R' MP-host script-list gate.** The walk over `0x656f00` /
`0x656f10[+0x28]` checking `>>16 & 1` on `+0x14` is a script-list
top-entry check. The full struct + index is opaque. **Resolve**:
`FindImmRefs 0x656f00` to find the writer that initializes the table.

**E3. 'R' MapPane reset pair (`FUN_00453390` + `FUN_00453320`).**
The non-MP / cleared-script-list branch calls these two MapPane
methods. Likely "ClearSectorCache + ReloadVisibleSectors" or similar
content-refresh. **Resolve**: extract bodies.

**E4. 'R' is NOT the player flag toggle.** Earlier draft of this spec
conflated 'R' with F11 because both share the dev-mode gate. The
disasm at `0x47c876` is MapPane work (`0x6668d8`-targeted calls),
NOT a `character+0x110` flag toggle. Only F11 (case 0x7a, §6.3) is
the flag-toggle. Confirmed via fresh `objdump`. **Resolved**.

**F. Player flag `0x100000` semantics.** The bit toggled in
`character+0x110`. Candidate: god mode / no-clip / debug overlay
on character / immortal. **Impact**: cosmetic only — wrong port binds
the cheat key to the wrong toggle. **Resolve**: search the character
class for reads of `flags & 0x100000` (`FindImmRefs 0x100000` or grep
the recon).

**G. `MapPane.vtable[+0x2c](1)` — the 'X' MapPane toggle.** Vtable
slot 0x2c on MapPane. **Resolve**: dump the MapPane vtable
(`cls_0x5a5658__vftable*` or extract via `DumpVtable.java`). Slot
ordering: probably a "debug overlay" toggle (walkmap render?
sector grid? blip toggle?).

**H. Rules subsystem identity (`DAT_0065d7a8`).** The `0x48b990/0x48cab0`
pair is "Stop + ReloadFromDisk". Class identity not yet in renames.
**Resolve**: `FindImmRefs 0x65d7a8` to find the ctor.

**I. `FUN_0054d170` body.** TTextBar Print wrapper. Behavior is
inferred from caller pattern matching (TScriptManager + TArea both
use it with literal format strings). Whether it's the actual TTextBar
`Print` (`cls_0x5a5560` vtable slot ?) or a thin printf-style wrapper
that does its own format then calls TTextBar internally — UNCONFIRMED.
**Resolve**: `DecompileAddr.java 0x0054d170`.

**J. F12 captures with UI overlay.** Verified for retail — the
screenshot IS the live (already-composited) display. UNCONFIRMED
whether the port should preserve this or offer a clean-frame option.
**Resolve**: ask user (likely "preserve, can add clean-frame later as
Revisited deviation").

**K. Cursor save/restore (`FUN_0049c830` / `FUN_0049c890` on
`0x667548`).** These look like "save/restore mouse pos around modal."
Where does the port already implement this? **Resolve**: grep
`src/window.cpp` and `src/input.cpp` for cursor-pos manipulators.

**L. F12 filename modernization.** `ss.bmp` overwrites every press.
Modernizing to `ss_<n>.bmp` or timestamped is sanctioned but should
be confirmed with user (`feedback_modernization_stance` — small
modernization, doesn't change semantics, but changes user-visible
artifact).

**M. The `key == 0x50` exception at the top guard (`0x47c676..0x47c679`).**
The function returns early when `+0x5d4 != 0` UNLESS `key == 0x50`. So
'P' is treated specially even when "paused" — likely because 'P' is
how the pause is cleared (case 0x50's else branch toggles
`+0x5d4`). Confirmed at `0x47c676` (`cmp edi, 0x50` / `jne 0x47cc08`).
**Resolve**: none needed — this is decode-confirmed and consistent.

**P. TPane child-slot bit `0x02`.** The default-branch guard at
`0x47cc15..0x47cc24` does `test al, 0x02` on the dword read from
`[ebx + 4*ebx_0x1c + 0x2c]`. On a real pointer the low 2 bits are
always zero (4-byte alignment), so the test would never trigger —
meaning the slot stores a TAGGED word (low bits = flag, high bits =
pointer), OR `+0x2c` is a separate flag array indexed by `+0x1c`.
The top-of-function guard at `0x47c690..0x47c69d` reads the same
slot but tests non-zero (treating it as a pointer). So the slot
likely holds a pointer with low-bit flag stowage. **Impact**: the
port's TPane child-list implementation must preserve this tag bit,
or the keyboard-block semantic for some popups will be inverted.
**Resolve**: read TPane's child-stack push/pop code (probably
`FUN_004353b0` or nearby) to see who sets the low bits.

**N. The KeyDispatch struct (`cls_0x439150`).** The keymap class has
4 fields per binding (`mbr_0x14` table, `mbr_0x18` default, `mbr_0x1c`
held-set, `mbr_0x20` released-set). Spec'd behaviorally enough for
this case but a full forensics on the keymap is OUT OF SCOPE here —
this spec only covers TimerTick's CONSUMER role. **Resolve**: separate
forensics pass on `cls_0x439150` if a panel needs to introspect the
bindings (probably the in-game options screen).

**O. The popup result polarity.** `PopupDef_SPEC.md` notes (§14 C)
that the popup returns 1 = positive button, 0 = negative button. The
exit-confirm path here matches: `if (result != 0) call exit-thunk`.
UNCONFIRMED in PopupDef_SPEC; the F-key spec's reading agrees with
the popup-engine reading and provides one more witness. **Resolve**:
covered in `PopupDef_SPEC.md` §14 C — extract `FUN_0048f040`'s exit
condition.

---

## Summary for the reconstruction agent

**There is NO new panel to build.** TimerTick is a key-dispatcher that:

1. Wires a 9-entry hard-coded VK switch ahead of the gameplay keymap.
2. Pushes plain literal strings into the TTextBar message log for
   script/rules reload (consume `TTextBar_SPEC.md`).
3. Opens the existing PopupDef `yesno` panel with the string-table
   key `"exitgameyn"` when ESC + the in-game-menu is up
   (consume `PopupDef_SPEC.md`).
4. Opens the existing InGameMenu when ESC + the in-game-menu is NOT
   up (consume `InGameMenuDef_SPEC.md`).
5. Captures the display to `ss.bmp` for F12 + before save/options
   modals (needs new `Renderer->CaptureDisplayToFile` primitive — §13).
6. Toggles a handful of debug state on F11 (player flag bit 0x100000)
   and 'X' (MapPane debug overlay + `DAT_006671f0`).
7. Reloads the MapPane on 'R' (FUN_004546a0 + FUN_00453390 + FUN_00453320).
8. Restores player stats on F9 (debug cheat — 3 vtable calls).
8. Falls back to the keymap → Command-dispatcher pair for every other
   key.

The reconstruction agent's deliverable is a clean `TPlayScreen::TimerTick`
implementation in `src/playscreen.cpp` (replacing the snapshot's
F12-editor-toggle hotkey at `:988-1005`) plus the two missing shared
primitives (`Renderer::CaptureDisplayToFile`, `PopupDef::ShowSync` — but
the latter is already on PopupDef_SPEC's wish list, this spec just
confirms one more call site). The TTextBar/PopupDef/InGameMenu specs
are the panels actually drawn; this spec is the dispatcher upstream.
