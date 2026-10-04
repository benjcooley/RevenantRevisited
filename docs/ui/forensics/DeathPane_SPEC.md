# TDeathPane (+ TDeathScreen wrapper) — Reconstruction SPEC (Game-Over modal)

> Pixel-faithful reconstruction spec for the player-death modal: a full-screen
> 640×480 "Game Over" background bitmap (`background` from `death.dat`) with
> three TButton-style word-buttons stamped on top — **Restart**, **Load**,
> **Exit** — each with its own bitmap-shaped (non-square) hit rect and a
> dedicated hotkey (VK_TAB / VK_RETURN / VK_ESCAPE per the pre-release shape).
>
> Two retail classes cooperate:
> - **`TDeathPane` (`cls_0x5b93c4`)** — the pane: holds the 3 buttons, paints
>   `background` once on dirty, then defers button paint to TButtonPane.
> - **`TDeathScreen` (`cls_0x5b9374`)** — a tiny retail-era TScreen wrapper
>   whose Initialize loads `death.dat` into a global multi, then activates the
>   singleton TDeathPane instance.
>
> Follow FORENSICS_PROTOCOL §0–§14. Every coordinate names its frame; every
> literal is cited `file:line` with hex(=dec).

---

## §0 — Sources & status

**Classes:**
- **`TDeathPane` = `cls_0x5b93c4`** (vtable @ `0x5b93c4`, 47 slots, 188 bytes
  long; bytewise layout in `recon/classes_original/cls_0x5b93c4__vftable_5b93c4.cpp:5-55`).
  Singleton instance `DAT_0066f500` (`recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:17`).
  Class identity is **confirmed B.r17** via the `"Trouble initializing Death pane"`
  string anchor (XREF from `0x5338a0` body) + the 3-button create sequence with
  string labels `"Restart"`/`"Load"`/`"Exit"` matching `src/death.cpp:47-49` 1:1
  — full evidence in `docs/ui/briefs/B_r12_oog_closeout.md:34-38` and
  `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt:40-101`.
- **`TDeathScreen` = `cls_0x5b9374`** (TScreen subclass; single member
  `cls_0x5a5ed4_TScreen` at offset 0, length 112). Identification per
  `B_r12_oog_closeout.md:20` + `recon/discovered/port_status/TDeathScreen.md:3-8`.

**Key method addresses (TDeathPane vtable, byte offsets, dump in
`recon/classes_original/cls_0x5b93c4__vftable_5b93c4.cpp`):**

| byte ofs | role | retail addr | recon | port shell |
|---|---|---|---|---|
| (ctor, not in vtable) | `TDeathPane::TDeathPane()` | `0x533cc0` | `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1409-1428` | `src/death.h:22` |
| 0  | `Initialize` (override) | `0x5339b0` | `recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp` (full body) **+** `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1311-1374` (cross-check) | `src/death.cpp:36` |
| 4  | (TButtonPane intermediate slot) | `0x533d90` | body NOT extracted; bodyless — likely thin stub | — |
| 80 | `DrawBackground` (override) | `0x533c10` | `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1391-1402` | `src/death.cpp:73` |
| 124 | scalar-dtor (override) | `0x533da0` | `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1463-1473` | implicit |
| 184 | `Close` (override) | `0x533c00` | `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1380-1385` | `src/death.cpp:59` |
| (dtor, not in vtable) | `~TDeathPane()` | `0x533d20` | `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1434-1457` | implicit |

> **Vtable-slot caveat.** "Slot N" in port_status / brief docs is the **byte
> offset** of the entry, not the index. Slot 80 = entry at byte 80 = **table
> index 20**; slot 184 = entry at byte 184 = **index 46** (the trailing slot in
> the 47-slot vtable). The byte form is used throughout this spec to match
> the port-status doc.

**Key method addresses (TDeathScreen, vtable fully enumerated 2026-05-29):**

The cls_0x5b9374 vtable has **20 entries (80 bytes)** per
`recon/classes_original/cls_0x5b9374__vftable_5b9374.cpp:6-28`. It is the
TScreen base layout (`cls_0x5a5ed4` — 19 entries, 76 bytes, per
`recon/classes_original/cls_0x5a5ed4__vftable_5a5ed4.cpp:6-26`) plus a **20th
trailing slot at byte 76** (whose type is `pointer`, not a method ptr — see
§17.2 and §14 item 11). TDeathScreen overrides **exactly 3 of the 19 inherited
TScreen slots** (dtor / Initialize / Close); the remaining 16 (Pulse,
DrawBackground, Animate, mouse/key/joystick input, Redraw, TimerTick, etc.)
are inherited from TScreen as `FUN_0048fda0_*`, `FUN_0048ff00_*`,
`FUN_00490030_*`, etc.

| byte ofs | role | retail addr | recon | TScreen base addr (slot inherited?) |
|---|---|---|---|---|
| 0  | **scalar-dtor** (override) | `0x533c90` | `cls_0x5b9374.cpp:50-60` (body: `cls_0x5a5ed4_TScreen::~cls_0x5a5ed4_TScreen()` then `if (deletingDtor) FUN_004830f0(this)` — standard MSVC scalar-deleting dtor) | base `0x492060` overridden |
| 4  | **Initialize** (override) | `0x5338a0` | `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp` (full body) **+** `recon/classes_original/cls_0x5b9374.cpp:15-31` (cross-check) | base `0x4920a0` (10-byte stub: clears `mbr_0x54=0`, returns 0 — `recon/discovered/cls_0x5a5ed4_TScreen_Initialize_4920a0.cpp:1-9`) overridden |
| 8  | **Close** (override) | `0x533910` | body NOT YET EXTRACTED — see §17.3 + §14 item 6 (resolve: `DecompileAddr.java 0x00533910`); inferred from data XREFs (§17.3) | base `0x4920b0` overridden |
| 12 | Pulse (inherited) | `0x46cf20` | base body not extracted; `cls_0x5b9374__vftable_5b9374.cpp:10` shows base ptr | base **inherited** |
| 16 | DrawBackground (inherited) | `0x48fda0` | (TScreen base) | base **inherited** |
| 20 | Animate (inherited) | `0x48ff00` | (TScreen base) | base **inherited** |
| 24 | MouseClick (inherited) | `0x490030` | (TScreen base) | base **inherited** |
| 28 | MouseMove (inherited) | `0x490110` | (TScreen base) | base **inherited** |
| 32 | KeyPress (inherited) | `0x4901e0` | (TScreen base) | base **inherited** |
| 36 | CharPress (inherited) | `0x4902c0` | (TScreen base) | base **inherited** |
| 40 | Joystick (inherited) | `0x4903b0` | (TScreen base) | base **inherited** |
| 44 | Redraw (inherited) | `0x490530` | (TScreen base) | base **inherited** |
| 48 | TimerTick (inherited) | `0x490660` | (TScreen base) | base **inherited** |
| 52..72 | (other TScreen slots — names unmapped) | `0x490760` / `0x490860` / `0x46cf30` / `0x490960` / `0x490bd0` / `0x491870` | (TScreen base) | base **inherited** |
| **76** | (trailing slot — `pointer` per OOAnalyzer, not a method) | — | `cls_0x5b9374__vftable_5b9374.cpp:26` shows raw `pointer` type, not a function ptr; likely a `type_info*` / RTTI ref or a Cinematix-convention trailing nil — see §14 item 11 | **new slot beyond TScreen** |
| (ctor) | `TDeathScreen::TDeathScreen` | `0x533c60` | `recon/classes_original/cls_0x5b9374.cpp:37-44` (trivial — chains to `cls_0x5a5ed4_TScreen::cls_0x5a5ed4_TScreen` then sets vftptr to `&cls_0x5b9374__vftable_5b9374_005b9374`) | — |

**Recon files read:**
- `recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp` (Initialize body — `0x5339b0`)
- `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp` (Screen wrapper Initialize — `0x5338a0`)
- `recon/classes_original/cls_0x5b9374.cpp:15-60` (TDeathScreen Initialize cross-check + ctor `0x533c60` + scalar-dtor `0x533c90` bodies)
- `recon/classes_original/cls_0x5b9374__vftable_5b9374.cpp:6-28` (full 20-slot TDeathScreen vtable layout — see §0 method-address table for Initialize/Close/dtor slot identification)
- `recon/classes_original/cls_0x5a5ed4__vftable_5a5ed4.cpp:6-26` (TScreen base 19-slot vtable — confirms TDeathScreen inherits 16 of 19 slots and overrides 3)
- `recon/discovered/cls_0x5a5ed4_TScreen_Initialize_4920a0.cpp:1-9` (TScreen::Initialize base — 10-byte stub: `mbr_0x54=0; return 0;` — proves the `mbr_0x54=0` line in TDeathScreen::Initialize is the "call base then add behaviour" pattern)
- `recon/discovered/cls_0x5a5320_TPlayScreen_Animate_47b4a0.cpp:1-13` (TPlayScreen::Animate — drains the deferred next-pane queue at `mbr_0x69c` by calling its slot-0 method then nulling it; this is the snapshot's `SetNextPane(&DeathPane)` dispatcher)
- `recon/discovered/renames/agent_screens.txt:163-169` (TPlayScreen::Animate documented as processing the `SetNextPane (param+0x69c)` queue — confirms the dispatch hook for death activation)
- `src/player.cpp:202-218` (snapshot — TPlayer::Pulse activation trigger: `if (Health() <= 0 && deathframe++ > 100) PlayScreen.SetNextPane(&DeathPane);`)
- `src/playscreen.cpp:539-547` (snapshot — TPlayScreen::Update drains `nextpane` via `AddPane(nextpane)` then nulls; matches retail `0x47b4a0` shape but with explicit AddPane instead of slot-0 call)
- `src/playscreen.h:150` (snapshot — `void SetNextPane(PTPane pane) { nextpane = pane; }` — the field-write helper)
- `recon/discovered/port_status/TDeathPane.md` (port-status, vtable summary, open questions)
- `recon/discovered/port_status/TDeathScreen.md` (port-status for the wrapper)
- `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt:40-125` (B.r17 evidence chain — Initialize, Close, DrawBackground, ctor renames + string anchors)
- `recon/discovered/cls_0x5a45c8_TButtonPane_vtable.txt` (TButtonPane base vtable — `0x434e40` Init / `0x434f30` Close / `0x435d70` DrawBackground / `0x435de0` Animate)
- `recon/discovered/cls_0x5a45c8_intermediate_init_434e40.cpp` (TButtonPane::Initialize body — zero out the TPointerArray<TButton, MAXBUTTONS=0x10>)
- `recon/classes_original/cls_0x5b93c4_TPlayer.cpp` (the **MISLABELED** OOAnalyzer-flattened class file — Initialize cross-check `:1311-1374`, Close `:1380-1385`, DrawBackground `:1391-1402`, ctor `:1409-1428`, dtor `:1434-1457`, scalar-dtor `:1463-1473`). **TPlayer is `cls_0x5b4f30`; the `_TPlayer` filename suffix is wrong** — see warning in `port_status/TDeathPane.md:7`.
- `recon/classes_original/cls_0x5b93c4__vftable_5b93c4.cpp` (47-slot vtable layout)
- `recon/classes_original/cls_0x5b9374.cpp` (TDeathScreen Initialize body cross-check)
- `recon/classes_original/_data.txt:113917-113977` (string anchors: `"death.dat"` at `0x5e3ebc`, `"Trouble initializing Death pane"` at `0x5e3ec8`, `"cursor"` at `0x5e3ee8`, `"goluc01"`..`"gosar00"` at `0x5e3ef0..0x5e3f10`, `"Restart"` at `0x5e3f18`, `"Load"` at `0x5e3f20`, `"Exit"` at `0x5e3f28`, `"background"` at `0x5e3f30`)
- `recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp` (sibling pattern — TLogoScreen builds 5 menu buttons with the SAME `FUN_0042c400(multi, name, 0,0,0,0, 0xffffffff, 0x10, 0xffffffff, 0)` ctor + `FUN_00436790` add + `FUN_00436900(i)` then-patch idiom — proves the helper signatures)
- `recon/classes_original/cls_0x5a3c68__vftable_5a3c68.cpp` (the retail TButton-family class; 27 vtable slots, 112-byte vtable)
- `src/death.h` (snapshot — TDeathPane class shape: `TButtonPane` subclass, 3 virtuals, `deathdata` member)
- `src/death.cpp` (snapshot — Initialize/Close/DrawBackground bodies, button handlers `DeathBtnRestart`/`DeathBtnLoad`/`DeathBtnExit`)
- `src/button.h` (snapshot — TButton/TButtonPane shape: NewButton, MAXBUTTONS, Button(idx) accessor)
- `src/button.cpp:88-115` (snapshot — `TButton::Draw` using `DM_USEREG | DM_BACKGROUND` + `DM_TRANSPARENT` for non-square buttons → confirms the **registration-point** mechanic for these word-buttons)
- `recon/discovered/cls_TArea_FUN_0041ba00.cpp:67-80` (sibling pattern — the same `FUN_0049c430` → `FUN_0049b990(snd, 0x7f, 1, 0, 0x50, 700)` audio-play idiom: volume `0x7f=127`, flags `1`, priority `0`, pan `0x50=80`, distance `700`)
- Asset measure: `tools/ui/dump_dat.py /Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/death.dat --out-dir /tmp/death_dump` → `/tmp/death_dump/death.def` + 10 PNGs (verified via Pillow: 1 background 640×480 + 9 button bitmaps in 3 sets {U,D,S}×{Restart 170×62, Load 172×62, Exit 172×62})
- `docs/ui/forensics/FORENSICS_PROTOCOL.md` + `NOMENCLATURE.md` + `UI_METHOD_MAP.md` (mandatory pre-reads)

**Method-map references:**
- `UI_METHOD_MAP.md §4` (`FUN_004bd680` sprite stamp = the bitmap stamps used for background + buttons — drawmode `0x10`=`DM_BACKGROUND` and bitmap-`REG` ptr behaviour).
- `UI_METHOD_MAP.md §9` (`FUN_0046d710` resource-by-name on a multi — the by-name form is what `DrawBackground` uses to fetch the `"background"` bitmap; the screen wrapper also calls it for the `"cursor"` resource at `0x5338e2`).
- `UI_METHOD_MAP.md §1` (drawmode literals: `0x10 = DM_BACKGROUND`, `0x100 = DM_TRANSPARENT`).
- `UI_METHOD_MAP.md §15a` (surface vtable — DrawBackground touches slot byte-offset `+144` via the pane's own vtable, not the surface vtable; the surface vtable does not factor in TDeathPane's leaf code).
- `UI_METHOD_MAP.md §16` (chroma key — TDeathPane bitmaps declare `kc=0x0` per `/tmp/death_dump/death.def` and the U/D/S button bitmaps carry **real alpha** (`flags=0x402` → BM_15BIT + the high-bit alpha-flag the dump tool reads); the background is `flags=0x2 = BM_15BIT` plain opaque. **Do NOT** apply the global magenta key to the button alpha bitmaps — see §7.)

**Status:** **forensics-complete** for the visual/composition contract (the spec
fully pins what the panel looks like and how to repaint it) **and for the
screen-level lifecycle contract** (added 2026-05-29 — full TDeathScreen
vtable mapped, override slot map enumerated, activation/composition/exit
sequence pinned via snapshot `TPlayer::Pulse` + retail `TPlayScreen::Animate`
dispatcher; see §17). **forensics-partial** for three non-visual aspects:
(a) the exact 5th sound `"gosar00"`'s playback path (stored in `local_10`
but unreferenced by the RandomRange(0,4) picker — see §14 item 4),
(b) TDeathScreen::Close (`0x533910`) and the 3 button-handler bodies
(`0x533950/970/990`) — XREF-inferred only, bodies pending extraction
(§14 items 6 + 8), and (c) the exact retail TPlayer Pulse-equivalent that
fires TDeathScreen::Initialize — snapshot path is pinned, retail call site
TBD (§14 item 3 + §17.1).

**Errors to close (existing port shell):** `src/death.cpp` / `src/death.h` are
the pre-release snapshot and are **superseded** by retail on these points:

1. **Activation site changed.** Pre-release `TDeathPane` is `AddPane`'d
   directly onto PlayScreen inside its own `Initialize` (`src/death.cpp:51-52`).
   Retail factored that out into `TDeathScreen` (`0x5338a0`) which loads
   `death.dat` THEN calls `TDeathPane::Initialize` on the singleton at
   `DAT_0066f500`; the AddPane / SetExclusivePane wiring lives in the screen
   wrapper / dispatcher path (`port_status/TDeathScreen.md:8-12`). The port
   must instantiate a `TDeathScreen`-equivalent (or fold the load+init+cursor
   sequence into `TDeathPane::Initialize` if the screen layer isn't being
   ported), not just `DeathPane.Initialize()` standalone.
2. **`deathdata` is now global, not a member.** Retail's
   `DAT_0066f6f4 = FUN_0047f670("death.dat")` (`0x5338a0:10`) is the **screen
   wrapper's** global, NOT `TDeathPane::deathdata`. The pane's
   `DrawBackground` reads `DAT_0066f6f4` directly (`cls_0x5b93c4_TPlayer.cpp:1397`).
3. **Random death sound.** Retail plays one of 5 character-specific death-gasp
   SFX (`goluc01` .. `gosar00`) at Initialize — pre-release `src/death.cpp` has
   **no audio** in Initialize. Port must add it (§9 / §6.4).
4. **Button hotkeys differ from snapshot src.** `src/death.cpp:47-49` lists
   `VK_TAB / VK_RETURN / VK_ESCAPE`. The retail `FUN_0042c400` ctor args
   contain `0xffffffff, 0x10, 0xffffffff` as the trailing 3 ints (not VK
   keycodes) — see §10. The 3 dispatch labels are `LAB_00533950` (Restart),
   `LAB_00533970` (Load), `LAB_00533990` (Exit) wired post-creation via
   `FUN_00436900(i).+0x80 = LAB_xxxxxx`. The pre-release VK mapping is the
   correct default (VK_TAB/RETURN/ESC) and is what the LAB stubs surface back
   to handlers; we keep that hotkey mapping per snapshot (and the dispatch
   wiring per retail). See §10 / §14 item 2.
5. **Button positions are NOT literal (x,y,w,h)** like the snapshot. The retail
   `cls_0x5a3c68` ctor is called with `(multi, name, 0,0,0,0, ...)` — all four
   geometry args are **zero**. The buttons get their on-screen position from
   the **bitmap's registration point** (`REG = (regx, regy)` field in the
   bitmap header, set at asset-bake time), so paint uses `DM_USEREG` which
   plots the bitmap at `dst = (x - regx, y - regy)` with `(x,y)=(0,0)` and the
   measured REGs `(-22,-404)`/`(-232,-404)`/`(-438,-404)` from `death.def`. The
   pre-release `NewButton("restart", 76, 144, 148, 80, ...)` literal coords are
   **NOT** the retail layout — the retail buttons sit at the bottom of the
   640×480 image (y=404). Verified: the `Background` bitmap visibly has the
   three button frames baked in at the bottom row, matching `(22,404)`
   `(232,404)` `(438,404)` exactly.

**Snapshot note (supplementary — superseded by retail).** Pre-release
`src/death.h:18-30` declares `class TDeathPane : public TButtonPane` with 3
virtuals (Initialize, Close, DrawBackground) and the `TMulti* deathdata`
member; `src/death.cpp:36-84` implements them and `:18-34` defines the 3 free
`DeathBtn*` handlers (currently all stubs that `SaveGame.ReadGame()` +
`DeathPane.Close()`). **Reusable:** the class shape (`TButtonPane` subclass +
3 virtuals + 3 handler functions); the `DM_USEREG | DM_BACKGROUND` paint mode
for buttons (`button.cpp:97-99`). **Superseded:** literal button coords
(retail uses REG), pane geometry is FRAMEMAPPANE (640×480 — confirmed same in
retail ctor `0x533cc0`), and the activation flow (retail TDeathScreen wrapper).
Do NOT pull snapshot button coordinates into §4.

---

## §1 — Overview

When the player dies in-game, the engine activates `TDeathScreen` (the screen
wrapper) which loads `death.dat` and shows the pane. The pane covers the
entire 640×480 game frame, drawing a single full-screen image (a stylized
"GAME OVER" graveyard scene with the words "RESTART / LOAD / EXIT" baked into
the bottom edge of the background as un-pressed silhouettes), then stamps the
three real button bitmaps on top in their unpressed `*U` state. Hovering /
clicking / pressing a hotkey swaps the appropriate button's state to its
pressed `*D` bitmap (TButton paints whichever of up/down is current). Clicking
Restart / Load / Exit dispatches one of three labelled handlers
(`LAB_00533950`/`970`/`990`), all of which (per the pre-release shell) call
`SaveGame.ReadGame()` to reload the last save and `DeathPane.Close()`.

- **Instances:** ONE — `DAT_0066f500` (singleton TDeathPane).
- **Hosted by:** itself — TDeathPane is its own top-level full-screen pane
  registered on PlayScreen as exclusive (the pre-release uses
  `PlayScreen.AddPane(this); PlayScreen.SetExclusivePane(this, true);` at
  `src/death.cpp:51-52`; the retail wiring is the same shape, lifted into the
  TDeathScreen wrapper).
- **Visibility predicate:** activated by external trigger (player-death
  dispatcher in TPlayer / TPlayScreen — exact site pending, see §14 item 3);
  closes itself when any of the 3 buttons fires its handler.
- **Repaint trigger:** `mbr_0x50` (TPane IsDirty equivalent — the same field
  read by `TScrollPane::DrawBackground 0x521360` and other TButtonPane
  subclasses). On first Initialize the pane is dirty; once painted, dirty is
  cleared (the snapshot does `SetDirty(false)` at `src/death.cpp:80`; retail
  does the equivalent inside the TButtonPane base chain — see §9).

**Plain language.** A 640×480 stone-carved tombstone scene fills the whole
screen, with three big orange-text buttons spelling RESTART / LOAD / EXIT
sitting in a row across the bottom. Each button has an up state (orange glow)
and a down state (darker, pressed-looking). Tab / Enter / Esc each fire one of
them; clicking the bitmap shape itself works too (the hit-test is by alpha,
not the full rectangle). Selecting any button reloads your last save.

---

## §2 — Asset roster

All assets live in `death.dat` (10 entries, 808,476 bytes total). Measured
with `tools/ui/dump_dat.py --out-dir /tmp/death_dump` (PNG dims verified via
Pillow).

| asset | archive | entry name | index | WxH (measured) | flags | REG (regx, regy) | role | cite |
|---|---|---|---|---|---|---|---|---|
| background | death.dat | `background` | 0 | 640×480 | `0x2` = BM_15BIT (opaque) | `(0, 0)` | full-screen "Game Over" backdrop; chrome | `/tmp/death_dump/death.def:5-14` |
| Restart up | death.dat | `RestartU` | 1 | 170×62 | `0x402` (BM_15BIT + alpha) | `(-22, -404)` | Restart button — unpressed sprite | `/tmp/death_dump/death.def:16-26` |
| Load up | death.dat | `LoadU` | 2 | 172×62 | `0x402` | `(-232, -404)` | Load button — unpressed sprite | `/tmp/death_dump/death.def:28-37` |
| Exit up | death.dat | `ExitU` | 3 | 172×62 | `0x402` | `(-438, -404)` | Exit button — unpressed sprite | `/tmp/death_dump/death.def:39-48` |
| Restart down | death.dat | `RestartD` | 4 | 170×62 | `0x402` | `(-22, -404)` | Restart pressed sprite | `/tmp/death_dump/death.def:50-59` |
| Load down | death.dat | `LoadD` | 5 | 172×62 | `0x402` | `(-232, -404)` | Load pressed sprite | `/tmp/death_dump/death.def:61-70` |
| Exit down | death.dat | `ExitD` | 6 | 172×62 | `0x402` | `(-438, -404)` | Exit pressed sprite | `/tmp/death_dump/death.def:72-81` |
| Restart selected | death.dat | `RestartS` | 7 | 170×62 | `0x402` | `(-22, -404)` | Restart "selected"/focused sprite (radio-group focus highlight; may be unused for a 3-button no-radio set — see §14 item 2) | `/tmp/death_dump/death.def:83-92` |
| Load selected | death.dat | `LoadS` | 8 | 172×62 | `0x402` | `(-232, -404)` | Load selected sprite | `/tmp/death_dump/death.def:94-103` |
| Exit selected | death.dat | `ExitS` | 9 | 172×62 | `0x402` | `(-438, -404)` | Exit selected sprite | `/tmp/death_dump/death.def:105-114` |

Plus one **out-of-archive** asset:
| asset | archive | entry name | role | cite |
|---|---|---|---|---|
| cursor | other (PlayScreen-shared asset multi) | `cursor` | mouse-cursor sprite re-registered by TDeathScreen.Initialize via `FUN_0046d710("cursor"); FUN_0043a020(uVar2)` at `0x5338e2`/`0x5338e8` | `cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:15-16` |

**REG-field semantics (load-bearing).** A bitmap header carries a
**registration point** `(regx, regy)`. When painted with `DM_USEREG`, the
bitmap is plotted at `dst = (drawX − regx, drawY − regy)` (`src/button.cpp:97`
`Display.Put(x, y, ub, DM_USEREG | DM_BACKGROUND)`). Retail's
`cls_0x5a3c68::cls_0x5a3c68(multi, name, 0,0,0,0, ...)` passes
`drawX=drawY=0`, so each button bitmap appears at `dst = (0 − regx, 0 − regy)
= (−regx, −regy)`. For the death buttons:

| name | (−regx, −regy) → screen dst (TL) | computed bottom-right |
|---|---|---|
| Restart{U,D,S} | `(22, 404)` | `(22+170, 404+62)` = `(192, 466)` |
| Load{U,D,S} | `(232, 404)` | `(232+172, 404+62)` = `(404, 466)` |
| Exit{U,D,S} | `(438, 404)` | `(438+172, 404+62)` = `(610, 466)` |

(decimal: `22 = 0x16`, `232 = 0xe8`, `438 = 0x1b6`, `404 = 0x194`, `62 = 0x3e`,
`170 = 0xaa`, `172 = 0xac`, `466 = 0x1d2`, `610 = 0x262`)

These positions match the visible button "wells" already painted into the
640×480 background bitmap (the background has the un-pressed silhouettes
baked at exactly those positions, so the live U-state sprite blits cover them
identically; verify by overlaying `01_RestartU.png` at `(22, 404)` onto
`00_background.png` — the alpha-feathered edges land exactly on the baked
silhouette outline).

**Atlas note:** none of these are atlases — each bitmap is a single
contiguous WxH image with its own REG. No source-rect map needed.

---

## §3 — Coordinate frames & surfaces

### Pane geometry (cite the ctor)

The pane fills the entire game frame:

| field | value | meaning | cite |
|---|---|---|---|
| `this+0xc` (`mbr_0xc`) | `0x280 = 640` | pane width | `cls_0x5b93c4_TPlayer.cpp:1412` |
| `this+0x10` (`mbr_0x10`) | `0x1e0 = 480` | pane height | `cls_0x5b93c4_TPlayer.cpp:1414` |
| `this+0x1c` (`mbr_0x1c`) | `0x280 = 640` | (duplicate width — TPane stores two width fields, `+0xc` is the live display width, `+0x1c` is the original) | `cls_0x5b93c4_TPlayer.cpp:1413` |
| `this+0x20` (`mbr_0x20`) | `0x1e0 = 480` | (duplicate height) | `cls_0x5b93c4_TPlayer.cpp:1415` |
| `this+0x4` / `+0x8` / `+0x14` / `+0x18` | `0` | pane origin (px, py) = `(0,0)` | `cls_0x5b93c4_TPlayer.cpp:1417-1420` |

Matches `src/death.h:22` `TButtonPane(FRAMEMAPPANEX=0, FRAMEMAPPANEY=0,
FRAMEMAPPANEWIDTH=640, FRAMEMAPPANEHEIGHT=480)`.

### Frame table (mandatory — every coord names its frame)

The pane is full-screen, so the frame graph is shallow:

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | (root) | TL | `(0, 0)` framebuffer | the final display target | — |
| **pane** | screen | TL (fills) | `(0, 0)` — pane fills the screen | the TDeathPane rect | `cls_0x5b93c4_TPlayer.cpp:1417-1420` (px/py = 0) |
| **bitmap-reg** | pane (via DM_USEREG) | derived | for each button: `dst = (−regx, −regy)` in pane | each button bitmap's top-left | `/tmp/death_dump/death.def:21-23,32-34,43-45` etc. + `src/button.cpp:97` (`DM_USEREG`) |

**Composition formula** (final screen coord for any button bitmap):
```
screen_xy = pane_origin + bitmap-reg = (0,0) + (−regx, −regy) = (−regx, −regy)
```
There are no scratch/mosaic surfaces. The pane paints **directly** into the
display (`PTR_DAT_005d79e0` = the global display surface; `cls_0x5b93c4_TPlayer.cpp:1398`).

**Verify chip/panel placement against parent corners.** The pane fills the
640×480 screen and is **TL-anchored** at `(0, 0)`. The three button bitmaps
are each independently positioned by their bitmap REG and (after the
DM_USEREG resolve) are effectively **TL-anchored** at screen
`(22, 404)`/`(232, 404)`/`(438, 404)` — see §2 table. There is no mirror axis
(no player/target split); this is a single, unmirrored screen.

### Surfaces

- **No scratch surface.** The DrawBackground path stamps the `background`
  bitmap directly onto the display via `FUN_004bd680(PTR_DAT_005d79e0, 0, 0,
  background_handle, DM_BACKGROUND=0x10)` (`cls_0x5b93c4_TPlayer.cpp:1398`).
  Per UI_METHOD_MAP §4, `FUN_004bd680(x, y, resource, drawmode, 0)` stamps
  into the "implicit current surface" — and the first arg here is the global
  display ptr, so the implicit surface IS the display.
- **No chrome surface, no mosaic surface, no live-render surface.**
- **Direct-renderer contract.** The port should still respect the project's
  compose-to-target contract: paint the background + 3 button sprites into a
  640×480 HUD render target and `DrawSurface` it during the HUD pass (§13).
  Even though retail draws straight to the display, the port's two-path rule
  (no straight-to-swapchain mixing) applies.

### Layout diagram (ASCII)

```
screen / pane (640 × 480, TL-anchored at (0,0))
0                                                                640
├──────────────────────────────────────────────────────────────────┤  0
│                                                                  │
│                                                                  │
│             [background bitmap fills the entire pane]            │
│                "Game Over" graveyard scene                       │
│         (button silhouettes baked-in at the bottom row)          │
│                                                                  │
│                                                                  │
│  22         192    232          404 438              610         │
│  ┌──────────┐      ┌─────────────┐  ┌─────────────────┐          │  404
│  │ Restart  │      │    Load     │  │      Exit       │          │
│  │  (U/D/S) │      │  (U/D/S)    │  │   (U/D/S)       │          │
│  └──────────┘      └─────────────┘  └─────────────────┘          │  466
│                                                                  │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘  480
```

(The three buttons sit on a single horizontal row at `y ∈ [404, 466)`,
widths {170, 172, 172}, with **210 px** gap between Restart-right (192) and
Load-left (232) and a **34 px** gap between Load-right (404) and Exit-left
(438). All values pinned to bitmap REG fields — §2 table.)

---

## §4 — Static element layout

One row per element, every coordinate **final, composed, in screen frame**.

| element | space | (x, y) | (w, h) | source rect | drawmode | cite |
|---|---|---|---|---|---|---|
| background | screen | `(0x00, 0x00)` = `(0, 0)` | `(0x280, 0x1e0)` = `(640, 480)` | full bitmap | `DM_BACKGROUND` = `0x10` | `cls_0x5b93c4_TPlayer.cpp:1398` `FUN_004bd680(disp, 0, 0, bm, 0x10)` |
| Restart {U/D/S} | screen | `(0x16, 0x194)` = `(22, 404)` | `(0xaa, 0x3e)` = `(170, 62)` | full bitmap | `DM_USEREG | DM_BACKGROUND` (snapshot `button.cpp:97`); chroma= **alpha**, not magenta (flags 0x402) | REG `(−22, −404)` @ `death.def:21-23` → dst `(−regx, −regy)` per `src/button.cpp:97-99` |
| Load {U/D/S} | screen | `(0xe8, 0x194)` = `(232, 404)` | `(0xac, 0x3e)` = `(172, 62)` | full bitmap | as above | REG `(−232, −404)` @ `death.def:33-35` |
| Exit {U/D/S} | screen | `(0x1b6, 0x194)` = `(438, 404)` | `(0xac, 0x3e)` = `(172, 62)` | full bitmap | as above | REG `(−438, −404)` @ `death.def:45-47` |

**Which {U,D,S} bitmap is shown per button per frame:** governed by TButton
state (the `down` field set by mouse-down / hotkey-down):
- **Up** state (default, hover-out, not-pressed) → `*U` bitmap.
- **Down** state (mouse-down inside the bitmap hit area, or hotkey held)
  → `*D` bitmap.
- **Selected** state (`*S`) → unused for this 3-button non-radio set —
  the `cls_0x5a3c68` ctor third radio-group arg is `0xffffffff = -1` per the
  ctor call (`Initialize_5339b0.cpp:43/53/63` — args `..., 0xffffffff, 0x10,
  0xffffffff, 0`), and per `src/button.h:118` `radiogroup=-1` ⇒ not a radio
  button ⇒ `*S` (selected/focused) state is never entered. **The 3 `*S`
  bitmaps are loaded but should never appear in normal play** — flag as
  asset-roster-vestigial (or as a focused-keyboard-nav fallback per the
  unconfirmed item §14 item 2). For the port: load them for asset parity but
  do not show them unless a keyboard-focus extension is added.

---

## §5 — Draw order / composition

The paint method is `TDeathPane::DrawBackground` at `0x533c10`
(`cls_0x5b93c4_TPlayer.cpp:1389-1402`). It runs once per dirty frame:

```
TDeathPane::DrawBackground(this):
  if (this->mbr_0x50 != 0):                                     # IsDirty
    bg = FUN_0046d710("background")                              # by-name lookup in death.dat multi (DAT_0066f6f4)
                                                                 # (the recon shows FUN_0046d710(DAT_0066f6f4); the string XREF at 0x533c20 proves a 2-arg by-name form — see §14 item 1)
    FUN_004bd680(PTR_DAT_005d79e0, 0, 0, bg, 0x10)               # Display.Put(0,0,bg, DM_BACKGROUND)
    (*this->vftptr_0x0->virt_meth_0x4361b0_144)(this)            # likely PlayScreen.DrawOverhangs() / SetClipRect bookkeeping (the snapshot equivalent of src/death.cpp:78-79)
    # mbr_0x50 (dirty) cleared by TButtonPane base call below
  cls_0x5b98b8::virt_meth_0x435de0(this)                         # chain to TButtonPane base — paints the 3 buttons (see §6.3 caveat)
  return
```

**Pass structure:**

| pass | action | recon line |
|---|---|---|
| 1 (gated on `mbr_0x50 != 0`) | resolve and stamp `background` (640×480) at screen `(0,0)` with `DM_BACKGROUND`. | `cls_0x5b93c4_TPlayer.cpp:1397-1398` |
| 2 (same gate) | call vtable byte-offset `+144` on this pane — a no-arg method, likely the `DrawOverhangs` + `SetClipRect` equivalent. The snapshot `src/death.cpp:78-79` does both; the leaf's responsibility ends with "trigger PlayScreen's existing overhang/clip housekeeping". | `cls_0x5b93c4_TPlayer.cpp:1399` |
| 3 (every paint) | chain to TButtonPane intermediate's button-redraw path (the OOAnalyzer-decompiled call is to `cls_0x5b98b8::virt_meth_0x435de0` = `FUN_00435de0` = **slot 20 = TButtonPane::Animate(bool draw)** per the cls_0x5a45c8 vtable dump at `cls_0x5a45c8_TButtonPane_vtable.txt:22`). This walks the pane's `Buttons[]` array (16 slots, only [0..2] populated) and invokes `TButton::Draw` on each non-null entry — which stamps the appropriate `*U` or `*D` sprite at its REG-derived screen pos with `DM_USEREG | DM_BACKGROUND`. | `cls_0x5b93c4_TPlayer.cpp:1401` |

> **NOTE on pass 3.** The pre-release snapshot `src/death.cpp:83` calls
> `TButtonPane::DrawBackground()` (slot 19 = `0x435d70`), not `Animate(false)`
> (slot 20 = `0x435de0`). The OOAnalyzer recon shows slot 20. Both end up
> calling `RedrawButtons()` / equivalent loop on the buttons array — the
> visible effect is the same (the 3 buttons get painted), but the
> structural path differs. The port should call whichever TButtonPane
> base routine actually iterates and paints buttons; the snapshot's
> `DrawBackground → RedrawButtons` is the safer match for the snapshot
> shell. See §14 item 5 for the exact-chain ambiguity.

**Per-frame composition order on screen:**
1. background bitmap (pass 1) → covers the whole 640×480 frame.
2. Restart{U/D} bitmap → blitted at `(22, 404)` with alpha, edges feather into
   the baked silhouette.
3. Load{U/D} bitmap → blitted at `(232, 404)`.
4. Exit{U/D} bitmap → blitted at `(438, 404)`.

(Pre-release ordering is Restart→Load→Exit by NewButton call order; retail is
the same — Initialize creates them in the same order at
`Initialize_5339b0.cpp:43/53/63`. The Buttons[] array is index-major and
`RedrawButtons`/Animate walks it `0..N-1`.)

---

## §6 — Algorithms (pseudocode per helper)

### §6.1 `TDeathScreen::Initialize` (the wrapper, `0x5338a0`)

Cite: `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:1-19`.

```
TDeathScreen::Initialize(this):
  this->mbr_0x54 = 0                                             # TScreen base bookkeeping
  DAT_0066f6f4 = FUN_0047f670("death.dat", 0xffffffff, 0)        # load multi → global slot
                                                                 # (FUN_0047f670 = TMulti::LoadMulti; cf. agent_ui_wave4_oog_closeout.txt:115-117)
  ok = FUN_005339b0_TDeathPane_Initialize(&DAT_0066f500)         # call pane Initialize on the singleton
  if (ok == 0):
    FUN_00481c10("Trouble initializing Death pane", 0)           # Status() log; FUN_00481c10 == Status/Fatal helper
  cur = FUN_0046d710("cursor")                                   # fetch the cursor sprite (from a shared multi, NOT death.dat)
  FUN_0043a020(cur)                                              # register cursor sprite as the active cursor
  FUN_0048ed90(&DAT_0066f500, 0xffffffff)                        # PlayScreen.AddPane(deathpane, exclusive) — matches src/death.cpp:51-52
  return 1
```

The port equivalent (folded into a `TDeathScreen` class or — if no screen
layer ports — into `TDeathPane::Initialize`):

1. Load the `death.dat` archive into the pane's data slot (member or global).
2. Call `TDeathPane::Initialize` (which is the retail-style 3-button create +
   sound play + flag-setting routine in §6.2).
3. Swap the active cursor to the `"cursor"` resource for the lifetime of the
   pane.
4. Add the pane to PlayScreen as exclusive (matches `src/death.cpp:51-52`).

### §6.2 `TDeathPane::Initialize` (`0x5339b0`)

Cite: `recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp` (full
body 1-82) + `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1311-1374`
(OOAnalyzer cross-check).

```
TDeathPane::Initialize(this):
  TButtonPane::Initialize(this)                                  # FUN_00434e40 — zeros the Buttons[] array etc.

  # ----- death-gasp SFX -----
  goluc = "goluc01"                                              # 4 picker slots + 1 unused 5th
  goand = "goand01"
  gojha = "gojha01"
  gooli = "gooli01"
  gosar = "gosar00"                                              # local_10; assigned but unused — see §14 item 4
  pick = FUN_00483300_RandomRange(0, 4)                          # 0..3 (exclusive upper per the picker semantics)
  snd_id = FUN_0049c430(audio_ctx = &DAT_00667548, picks[pick])  # acquire/lookup sound by basename
  if (snd_id >= 0):
    if (FUN_0049b650(audio_ctx, snd_id) != 0):                   # sound loaded check
      FUN_0049b990(audio_ctx, snd_id, 0x7f, 1, 0, 0x50, 700)     # play: vol=127, flags=1, priority=0, pan=0x50=80, distance=700
                                                                 # (param shape per sibling cls_TArea_FUN_0041ba00.cpp:74-80)

  # ----- pane state flags -----
  this->mbr_0x60 |= 0xe                                          # set 3 bits — these are likely "dirty | needs-redraw | exclusive-eligible"
                                                                 # (the exact bit map is per TPane; the OR-in of 0xe is what the snapshot's SetFullScreen(false)/AddPane sequence amounts to)

  # ----- early-out if death.dat failed to load -----
  if (DAT_0066f6f4 == 0):                                        # the screen wrapper's multi handle
    return 0                                                     # (the wrapper will Status() "Trouble initializing Death pane")

  # ----- create the 3 buttons -----
  for (i, name) in [(0, "Restart"), (1, "Load"), (2, "Exit")]:
    b = malloc(0x148)                                            # FUN_00482fb0(0x148) — TButton size
    if (b):
      cls_0x5a3c68::cls_0x5a3c68(b,                              # retail TButton ctor at FUN_0042c400
                                 DAT_0066f6f4,                   # the multi to fetch <name>U/<name>D/<name>S from
                                 name,
                                 0, 0, 0, 0,                     # x, y, w, h — all zero (positions come from bitmap REG; hit-rect comes from bitmap alpha — see §10)
                                 0xffffffff,                     # radio group = -1 (not a radio button)
                                 0x10,                           # flags = 0x10 — see §10 / §14 item 2
                                 0xffffffff,                     # repeat = -1 (no repeat)
                                 0)                              # extra/key? = 0 — see §14 item 2
    TButtonPane::AddButton(this, b)                              # FUN_00436790 — push into Buttons[]
                                                                 # (cls_0x5b98b8::meth_0x436790 in the OOAnalyzer view)

  # ----- post-create per-button patching -----
  for i in [0, 1, 2]:
    b = TButtonPane::Button(i)                                   # FUN_00436900(i) — Buttons[i]
    b->mbr_0x14 |= 0x40                                          # set the "pixelcheck / non-square hit-test from bitmap alpha" flag
                                                                 # (snapshot src/button.h:64 `pixelcheck = notsquare && ubm` — mbr_0x14 bit 0x40 is the runtime equivalent)
    b->mbr_0x80 = &LAB_005339[5/7/9][0]                          # callback fn ptr:
                                                                 #   button[0] (Restart) → LAB_00533950
                                                                 #   button[1] (Load)    → LAB_00533970
                                                                 #   button[2] (Exit)    → LAB_00533990

  return 1
```

**Per-call literal-arg map for `cls_0x5a3c68` ctor:**

| call | name arg | x | y | w | h | radio | flags | repeat | tail | callback patched later |
|---|---|---|---|---|---|---|---|---|---|---|
| `0x533a8a` | `"Restart"` (`0x5e3f18`) | 0 | 0 | 0 | 0 | `-1` | `0x10` | `-1` | 0 | `LAB_00533950` (Restart handler) |
| `0x533ae9` | `"Load"` (`0x5e3f20`) | 0 | 0 | 0 | 0 | `-1` | `0x10` | `-1` | 0 | `LAB_00533970` (Load handler) |
| `0x533b48` | `"Exit"` (`0x5e3f28`) | 0 | 0 | 0 | 0 | `-1` | `0x10` | `-1` | 0 | `LAB_00533990` (Exit handler) |

Cite: `Initialize_5339b0.cpp:43,53,63` and `cls_0x5b93c4_TPlayer.cpp:1343,1350,1358` for the ctor calls; `:67-75` and `:1363-1371` for the post-patches.

### §6.3 `TDeathPane::DrawBackground` (`0x533c10`)

Already given in §5 as the paint sequence. Pseudocode:

```
TDeathPane::DrawBackground(this):
  if (this->mbr_0x50 != 0):                                      # IsDirty
    bg = FUN_0046d710(DAT_0066f6f4, "background")                # by-name in death.dat multi
                                                                 # (the OOAnalyzer cleanup elides the name arg; the string XREF at 0x533c20 in _data.txt:113973 proves the literal is loaded into the call — §14 item 1)
    FUN_004bd680(display, 0, 0, bg, 0x10)                        # Display.Put(0,0,bg, DM_BACKGROUND)
    this->vftptr->slot_at_byte_144(this)                         # likely PlayScreen.DrawOverhangs() + SetClipRect bookkeeping
  TButtonPane::DrawBackground_or_Animate(this)                   # chain to base; paints the 3 buttons (snapshot calls slot 19 DrawBackground; OOAnalyzer recon calls slot 20 Animate — see §14 item 5)
  return
```

### §6.4 `TDeathPane::Close` (`0x533c00`)

Cite: `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1378-1385`.

```
TDeathPane::Close(this):
  TButtonPane::Close(this)                                       # cls_0x5b98b8::virt_meth_0x434f30 — slot 1 of TButtonPane base
                                                                 # (frees buttons + closes pane base; per the snapshot, also: PlayScreen.ReleaseExclusivePane, PlayScreen.RemovePane, PlayScreen.Redraw — but those live in TButtonPane::Close in the retail layering, not the TDeathPane leaf)
  return
```

> The retail leaf is **smaller** than the snapshot's: snapshot `src/death.cpp:59-71`
> also frees `deathdata` and re-calls `PlayScreen.SetFullScreen(saveisfullscreen)`.
> Retail moved `deathdata` to a global (the screen wrapper's
> `DAT_0066f6f4`) so the pane no longer owns it — the pane just chains to
> TButtonPane::Close. The wrapper's symmetric Close (not yet decompiled, see
> §14 item 6) presumably frees the multi.

### §6.5 `TDeathPane::TDeathPane` (ctor, `0x533cc0`)

Cite: `recon/classes_original/cls_0x5b93c4_TPlayer.cpp:1407-1428`.

```
TDeathPane::TDeathPane(this):
  this->mbr_0xc  = 0x280   # 640 (live width)
  this->mbr_0x1c = 0x280   # 640 (orig width)
  this->mbr_0x10 = 0x1e0   # 480 (live height)
  this->mbr_0x20 = 0x1e0   # 480 (orig height)
  this->vftptr_0x0 = &TPane_vftable_5a4494                       # interim TPane vtable (3-stage ctor pattern)
  this->mbr_0x4 = this->mbr_0x14 = this->mbr_0x8 = this->mbr_0x18 = 0      # px=py=0; clear flags
  this->mbr_0x48 = this->mbr_0x4c = this->mbr_0x50 = this->mbr_0x54 = 0    # zero state
  cls_0x41c7f0::cls_0x41c7f0(&this->field_0x88, 0x10)             # TPointerArray<TButton, 16=MAXBUTTONS>(buttons array storage)
  this->vftptr_0x0 = &TDeathPane_vftable_5b93c4                   # final leaf vtable
```

Matches `src/death.h:22` `TDeathPane() : TButtonPane(0, 0, 640, 480) {}` plus
the standard MSVC 3-stage vtable transition.

---

## §7 — Effects & shadows

**No drop shadow.** Per UI_METHOD_MAP §4 the drawmode `0x10 = DM_BACKGROUND`
for the background blit + `DM_USEREG | DM_BACKGROUND` for the button blits are
**not** the shadow-pipeline path; there is no `FUN_00438d80` shadow-setup
call, no `(*+0x5c)` blit-with-effects, no `FUN_004be2b0` text. The buttons are
**bitmap-stamped sprites**, not GDI text. The death pane has zero shadow work.

**Chroma key.** Per `/tmp/death_dump/death.def:23/34/45/...` every button
bitmap has `KEYCOLOR 0x0` (no explicit magenta key) and `FLAGS 0x402` (`BM_15BIT
| 0x400`). The `0x400` flag is the **alpha-present** bit (NOT the text-shadow
flag — that bit lives on `param_8` to `FUN_004be2b0`, and this code never
calls FUN_004be2b0). The button bitmaps carry their own per-pixel alpha;
**do NOT** apply the global magenta key to them on blit. The `background`
bitmap is `flags=0x2` (BM_15BIT plain) and is opaque; no key needed for it
either. UI_METHOD_MAP §16 covers this convention.

**Pink-halo bug.** Not applicable — no GDI text path runs in this panel, so
the magenta-cleared-scratch + AA-edge artifact cannot occur here. The retail
death screen has no documented rendering bug.

---

## §8 — Text rendering

**Not applicable.** TDeathPane renders **zero glyphs**: there is no
`FUN_004be2b0` call in any of the 4 leaf methods (Init / Close / DrawBackground
/ ctor), nor in the TDeathScreen wrapper's Initialize. All visible text is
baked into the bitmap assets:
- The "GAME OVER" word art is part of the `background` bitmap.
- The button labels "Restart" / "Load" / "Exit" are part of the per-button
  `RestartU/D/S`, `LoadU/D/S`, `ExitU/D/S` bitmaps (each label is etched into
  the alpha-fringed sprite).

No font flag decoding, no shadow pass, no h-align / v-align, no cell
geometry, no color source. (If a future port needs to add a font-rendered
overlay — e.g. a "press a key to retry" hint — it must do so as a new
feature, not as a reconstruction of retail.)

---

## §9 — Animation & dynamic behavior

**No tweens.** No per-tick value animations: no fade, no scroll, no pulse,
no countdown.

**Per-frame "animation" is purely TButton state-swap.** The only dynamic
element is each TButton's `down` field (set by hover-down/key-down/released).
When `down` flips, `dirty` is set, and the next DrawBackground pass paints
the `*D` bitmap instead of the `*U` bitmap at the same `(x, y)` (per
`src/button.cpp:97`). This is **discrete**, not interpolated.

**Dirty/redraw model:**

| field | role | cite |
|---|---|---|
| `TDeathPane::mbr_0x50` | pane-level IsDirty — gates the background re-paint | `cls_0x5b93c4_TPlayer.cpp:1396` |
| `TButton::dirty` (`mbr_0x??` — exact offset on cls_0x5a3c68 unknown, see §14 item 7) | per-button IsDirty — gates each `TButton::Draw` | `src/button.h:84` (snapshot) |

The pane's dirty flag is set on Initialize (by `mbr_0x60 |= 0xe`, which
includes the dirty bit) and cleared once `DrawBackground` runs the pass-1
work (the snapshot does `SetDirty(false)` in `src/death.cpp:80`; the retail
chain to TButtonPane base inherently clears it). After the first paint, the
background is **not redrawn** unless something external sets dirty again.
Buttons each maintain their own dirty (set by `SetState`/`SetDirty`/`Hide`/
`Show` per `src/button.h:44-48`) — when a button presses, it dirty-flags
itself but does NOT dirty-flag the pane; only the button re-blits.

**State machines / phases:** none. The pane is binary on/off — instantiated
at death, destroyed (or hidden) after one button click.

**SFX as a one-shot event (NOT a tween).** The death-gasp SFX (§6.2) plays
**once** at Initialize. Implementation note: pick `RandomRange(0, 4)` →
play one of `goluc01/goand01/gojha01/gooli01` once at vol=127, pan=80,
distance=700, flags=1. The 5th string `gosar00` is loaded into `local_10`
but never picked from (see §14 item 4).

---

## §10 — Input & dispatch

### Hit rects

There is **no rectangle hit rect** for the buttons. The post-Initialize
patch `b->mbr_0x14 |= 0x40` (`Initialize_5339b0.cpp:68/72/75`) sets the
TButton "pixel-check / not-square" flag (snapshot src/button.h:64
`pixelcheck = notsquare && (ubm != nullptr)`). With pixelcheck on, the hit
test reads the bitmap's alpha at the cursor offset and rejects clicks on
fully-transparent pixels. Effective hit area = the visible (non-alpha-0)
pixels inside the bitmap's natural rect:

| button | bitmap | effective hit rect (screen) | hit-test method |
|---|---|---|---|
| Restart | `RestartU` (170×62) | inside `(22, 404)..(192, 466)`, alpha > 0 only | bitmap-alpha pixelcheck |
| Load | `LoadU` (172×62) | inside `(232, 404)..(404, 466)`, alpha > 0 only | bitmap-alpha pixelcheck |
| Exit | `ExitU` (172×62) | inside `(438, 404)..(610, 466)`, alpha > 0 only | bitmap-alpha pixelcheck |

The TButton hit-test routine is on `cls_0x5a3c68` (slot ?? of its 27-slot
vtable; not extracted in this snapshot — see §14 item 7).

### Command dispatch

Each button has a direct C-function callback patched in at Initialize end:

| button | callback fn | retail addr | snapshot equivalent | effect |
|---|---|---|---|---|
| 0 (Restart) | `LAB_00533950` | `0x533950` | `DeathBtnRestart` (`src/death.cpp:18-22`) | `SaveGame.ReadGame(); DeathPane.Close();` |
| 1 (Load) | `LAB_00533970` | `0x533970` | `DeathBtnLoad` (`src/death.cpp:24-28`) | `SaveGame.ReadGame(); DeathPane.Close();` |
| 2 (Exit) | `LAB_00533990` | `0x533990` | `DeathBtnExit` (`src/death.cpp:30-34`) | `SaveGame.ReadGame(); DeathPane.Close();` |

(The pre-release snapshot has all 3 handlers doing the same `SaveGame.ReadGame
+ DeathPane.Close` — a placeholder that was never differentiated. Retail
LAB_00533950 / 970 / 990 bodies are not extracted in this snapshot but the
labels exist at those exact addresses, so the per-handler split is preserved
in retail. The Exit handler in particular should logically Quit-to-main-menu
rather than reload, but per the snapshot, the placeholder behaviour holds.
See §14 item 8.)

### Hotkeys

The pre-release `NewButton(..., VK_TAB, DeathBtnRestart, ...)` /
`VK_RETURN` / `VK_ESCAPE` (`src/death.cpp:47-49`) is the **intended** key
binding. The retail `cls_0x5a3c68` ctor args `(0xffffffff, 0x10, 0xffffffff,
0)` (4 trailing ints) **do not** carry VK_TAB/RETURN/ESCAPE in those slots
— see §14 item 2 for the open question on where retail wires the hotkeys.
Conservative port: bind VK_TAB → button 0 (Restart), VK_RETURN → 1 (Load),
VK_ESCAPE → 2 (Exit) per the snapshot. (The retail binding is most likely
the same — it just doesn't surface in the ctor literals, so it's either a
default per-button-index policy, lives in the `*S` pulse handler, or is
encoded in the `flags=0x10` value.)

### Modal-state globals

The death pane is **exclusive** — once shown, it suppresses all other
PlayScreen input (per `src/death.cpp:52`
`PlayScreen.SetExclusivePane(this, true)`; retail same shape lifted into
TDeathScreen.Initialize at `0x5338a0:17` `FUN_0048ed90(&DAT_0066f500, 0xffffffff)`,
where `0xffffffff = -1` is the `exclusive=true` sentinel per the PlayScreen
AddPane signature observed in `cls_0x5a5320` recon). No radio-group state,
no toggle, no per-pane mode.

---

## §11 — Retail bugs NOT to reproduce

**None observed in TDeathPane / TDeathScreen** — the panel is straightforward
bitmap stamping with no text and no shadow pipeline. The visual is clean.

Minor pre-release shell oddities (not retail bugs, but call them out so the
reconstruction agent doesn't replicate them blindly):

1. **All 3 handlers identical in `src/death.cpp:18-34`** — they all just
   reload the last save. A polished implementation would differentiate
   Restart (reload current map / start of area), Load (open the load-game
   panel), Exit (quit to main menu). Mark this for design-side review; do
   NOT reproduce the all-three-do-`ReadGame` placeholder.
2. **Unused 5th SFX `gosar00`** assigned to `local_10` in
   `Initialize_5339b0.cpp:25` but never read by the `RandomRange(0, 4)` →
   `local_20[i]` picker (which only sees `goluc01/goand01/gojha01/gooli01`).
   Either a dropped 5th character or an off-by-one bug in retail. Don't
   "fix" it on import — preserve `RandomRange(0,4)` picking from the 4 — but
   note for future Revisited deviation that adding the 5th case is one-line.

---

## §12 — Reconstruction pseudocode

```
class TDeathPane : public TButtonPane:
  // ctor — matches src/death.h:22; full-screen pane, no member data (death.dat is global)
  TDeathPane() : TButtonPane(0, 0, 640, 480) {}

  bool Initialize():
    TButtonPane::Initialize()                              // clears Buttons[16]
    int picks[4] = { "goluc01", "goand01", "gojha01", "gooli01" }
    int n = RandomRange(0, 4)                              // [0..3]
    int snd = AudioCtx.LookupSound(picks[n])
    if (snd >= 0 && AudioCtx.IsLoaded(snd)):
      AudioCtx.Play(snd, /*vol*/127, /*flags*/1, /*pri*/0, /*pan*/80, /*dist*/700)
    SetFlag(FLAG_DIRTY | FLAG_NEEDS_REDRAW | FLAG_EXCLUSIVE_OK)   // mbr_0x60 |= 0xe
    if (!DeathDataMulti)                                   // global death.dat handle
      return false
    NewButton("Restart", LAB_DeathBtnRestart)              // §6.2 + §10
    NewButton("Load",    LAB_DeathBtnLoad)
    NewButton("Exit",    LAB_DeathBtnExit)
    for (i in 0..2):
      Button(i)->SetPixelCheck(true)                       // hit-test via bitmap alpha (mbr_0x14 |= 0x40)
    return true

  // NewButton helper (port-side): retail TButton ctor takes (multi, basename,
  // 0,0,0,0, -1, 0x10, -1, 0) — the basename gets "U"/"D"/"S" suffixed inside
  // the ctor to fetch the 3 bitmaps; position comes from the bitmap REG;
  // hit-rect from bitmap alpha (after SetPixelCheck).
  void NewButton(const char* name, void(*handler)()):
    TButton* b = new TButton()
    b->Init(DeathDataMulti, name, /*x*/0, /*y*/0, /*w*/0, /*h*/0,
            /*radio*/-1, /*flags*/0x10, /*repeat*/-1, /*extra*/0)
    b->callback = handler
    AddButton(b)

  void Close():
    TButtonPane::Close()                                   // chain to base (frees buttons + base cleanup)
    PlayScreen.ReleaseExclusivePane(this)                  // per src/death.cpp:66 — actually inside TButtonPane::Close in retail layering, but include for completeness
    PlayScreen.RemovePane(this)
    PlayScreen.Redraw()

  void DrawBackground():
    if (IsDirty()):
      PTBitmap bg = DeathDataMulti->Bitmap("background")
      Display.Put(0, 0, bg, DM_BACKGROUND)                 // §5 pass 1, FUN_004bd680 equivalent
      PlayScreen.DrawOverhangs()                           // §5 pass 2; the vtable[+144] call
      SetClipRect()
      SetDirty(false)
    TButtonPane::DrawBackground()                          // §5 pass 3 — paints the 3 buttons (snapshot semantics; matches src/death.cpp:83)
                                                           // (retail OOAnalyzer shows Animate(false) instead — same effective behaviour, §14 item 5)

// the 3 handlers (per src/death.cpp:18-34; LAB_00533950/970/990 in retail)
static void DeathBtnRestart(): SaveGame.ReadGame(); DeathPane.Close();
static void DeathBtnLoad():    SaveGame.ReadGame(); DeathPane.Close();
static void DeathBtnExit():    SaveGame.ReadGame(); DeathPane.Close();   // see §11 item 1

// TDeathScreen (port if the screen layer is being ported; otherwise fold into TDeathPane::Initialize)
class TDeathScreen : public TScreen:
  bool Initialize():
    DeathDataMulti = TMulti::LoadMulti("death.dat")        // global; null on failure
    bool ok = DeathPane.Initialize()
    if (!ok):
      Status("Trouble initializing Death pane")
    Cursor.Set(ResolveSprite("cursor"))                    // active cursor swap
    PlayScreen.AddPane(&DeathPane, /*exclusive*/true)
    return true
```

Reference §3 / §4 for coordinates and §10 for hit-testing + dispatch.

---

## §13 — Port mapping notes

The port composes the panel into one 640×480 HUD render target and
`DrawSurface`'s it during the HUD pass (project's compose-to-target
contract — `feedback_ui_no_mock_use_retail`, `feedback_centralized_rendering`).

| retail primitive | port primitive (canonical, `src/renderer.h`) | UI_METHOD_MAP cite |
|---|---|---|
| `FUN_004bd680(disp, 0, 0, bg, 0x10)` (background blit, opaque, DM_BACKGROUND) | `Renderer->DrawBitmapToTarget(bm, 0, 0, hudRT)` | §4 + §12 |
| `FUN_004bd680(disp, 0, 0, sprite, DM_USEREG|DM_BACKGROUND)` (button blits with REG) | `Renderer->DrawBitmapToTarget(bm, /*x=*/-bm->regx, /*y=*/-bm->regy, hudRT)` — i.e. **resolve DM_USEREG ourselves** by applying the bitmap's REG to the (0,0) draw position. The port's `DrawBitmap*` family does NOT have a USE_REG mode; the caller applies the REG. | §4 + §12 (this is a gap — see Gaps below) |
| `FUN_0046d710("background")` (by-name resource lookup on a multi) | `multi->Bitmap("background")` (mirrors snapshot `src/death.cpp:77`) — `TMulti` already exposes `Bitmap(const char*)` by name | UI_METHOD_MAP §9 |
| `FUN_0049b990(...)` (audio play) | the project's audio play primitive (TBD; the audio layer is on `feature/gameflow`, not here) | — |
| **No** text primitive | — | §8 N/A |
| **No** shadow primitive | — | §7 N/A |
| **No** scratch/mosaic surface | — | §3 |

**Port gaps (needs new primitive, or already-supported via caller-side REG apply):**
- DM_USEREG: not a built-in renderer mode. The caller subtracts the bitmap's
  `regx/regy` from the draw position (mirrors what `Display.Put(...,
  DM_USEREG, ...)` does internally in pre-release `surface.cpp`). No new
  primitive needed if `PTBitmap` exposes `regx/regy` fields (snapshot
  `src/bitmap.h` does: `bitmap->regx`, `bitmap->regy`). Verify the
  port's `PTBitmap` carries them; if not, that's a port-side gap.
- Pixel-check hit-test (TButton's "not-square" mode): the port's input layer
  must be able to query a bitmap's alpha at `(x, y)` for hit testing. Snapshot
  `src/button.cpp:96-97` uses `Display.PutSV(..., DM_USEREG | DM_BACKGROUND |
  DM_TRANSPARENT, ...)` for the actual paint, but the hit-test code (not
  shown in `src/button.cpp` snapshot) presumably reads the bitmap's alpha
  channel. If the port's TBitmap exposes per-pixel alpha (BM_15BIT + alpha
  flag), this is "just read the bit". If not, that's a port primitive gap.

---

## §14 — UNCONFIRMED / open questions

Each item: what you tried, impact if wrong, exact step to resolve.

1. **`FUN_0046d710` call shape in DrawBackground** (per UI_METHOD_MAP §9 +
   §17 item 2 unconfirmed).
   - The OOAnalyzer decompile of `DrawBackground 0x533c10`
     (`cls_0x5b93c4_TPlayer.cpp:1397`) shows
     `cls_0x46d6b0::meth_0x46d710(DAT_0066f6f4)` — a single-arg call with
     just the multi. But `_data.txt:113973` shows `s_background_005e3f30`
     XREF'd from `virt_meth_0x533c10:00533c20` — the string IS loaded inside
     the body. So either the call is actually 2-arg `(multi, "background")`
     and OOAnalyzer dropped the second arg (most likely — sibling
     TBookPane/TScrollPane `DrawBackground` recons all show the 2-arg
     by-name form), OR the multi has a built-in "background" name slot.
   - **Impact if wrong:** if the name slot is implicit (i.e. the multi has a
     "primary" bitmap by default), the port could simply call
     `multi->FirstBitmap()` or `multi->Bitmap(0)` — but the snapshot
     `src/death.cpp:77` says `deathdata->Bitmap("background")` explicitly, so
     this is almost certainly the by-name form. Conservative port: use
     `multi->Bitmap("background")` to match snapshot. Low risk.
   - **Resolve:** `DecompileAddr.java 0x00533c10` for the cleaner decomp, and
     `DecompileAddr.java 0x0046d710` for the FUN_0046d710 body to confirm
     1-arg vs 2-arg shape.

2. **Retail hotkey binding source.** Snapshot `src/death.cpp:47-49` binds
   VK_TAB / VK_RETURN / VK_ESCAPE for the 3 buttons. The retail
   `cls_0x5a3c68` ctor takes `(0xffffffff, 0x10, 0xffffffff, 0)` as the
   trailing 4 ints (radio=-1, flags=0x10, repeat=-1, extra=0). The flags
   `0x10` value is consistent across TLogoScreen's 5 menu buttons too — so
   it's not per-button-specific. Either retail wires hotkeys later (post-Init
   patch, possibly in the missing `LAB_00533950/970/990` body), or by
   button-index policy (`Button(0)` always gets the dispatcher's "primary"
   key from a global VK table), or via the `*S` "Selected" state path
   (which would be keyboard-focus-driven). The `cls_0x5a3c68` ctor body at
   `FUN_0042c400` (not extracted in this recon snapshot) likely also takes
   the key as an arg that the test rig should map.
   - **Impact if wrong:** the keyboard shortcuts could fire the wrong button
     or none at all. Snapshot mapping (TAB/RET/ESC → 0/1/2) is the most
     plausible and that's the conservative port. Low UX risk.
   - **Resolve:** `DecompileAddr.java 0x0042c400` to read the
     `cls_0x5a3c68::cls_0x5a3c68` body and see if it stores any of the 4
     trailing ints into a `key` field, or if the `0x10` flag enables an
     implicit per-index VK lookup. Also extract `LAB_00533950 / 970 / 990`
     bodies (they MIGHT call `b->SetKey(VK_TAB)` etc.).

3. **TDeathScreen activation site (partial — snapshot pinned, retail-side unconfirmed).**
   - **Snapshot path (confirmed):** `src/player.cpp:202-218` `TPlayer::Pulse`
     polls `Health() <= 0` every tick; when true for >100 ticks
     (`deathframe++ > 100`), calls `PlayScreen.SetNextPane(&DeathPane)`
     (`src/player.cpp:213`). `SetNextPane` writes the static `nextpane`
     field (`src/playscreen.h:150`); `TPlayScreen::Update`
     (`src/playscreen.cpp:539-547`) consumes it via `AddPane(nextpane)` then
     nulls. Net effect: ~100-tick post-death grace period (death animation
     plays before the modal pops). See §17.1 for the full lifecycle sketch.
   - **Retail dispatch hook (confirmed):** `TPlayScreen::Animate` at
     `0x47b4a0` drains the queue at `param_1 + 0x69c`
     (`cls_0x5a5320_TPlayScreen_Animate_47b4a0.cpp:6-9`) by calling its
     **slot-0 method** then nulling (NOT AddPane like snapshot). For
     TDeathPane (cls_0x5b93c4) slot 0 = TDeathPane::Initialize at `0x5339b0`
     — but that path **requires DAT_0066f6f4 (death.dat multi) to already be
     loaded**, which only TDeathScreen::Initialize does. So:
     - Either retail's TPlayer::Pulse-equivalent calls
       `TDeathScreen::Initialize(&TDeathScreen_instance)` directly (which
       does the full load+init+cursor+AddPane sequence inline at
       `0x5338a0:9-17`) — bypassing SetNextPane entirely.
     - Or retail's `nextpane` is the TDeathScreen instance and its slot 0 is
       reinterpreted (but slot 0 of cls_0x5b9374 is the dtor at `0x533c90`,
       not Initialize — so this is structurally wrong).
     - Most plausible: retail TPlayer::Pulse calls TDeathScreen::Initialize
       directly, NOT via the SetNextPane queue.
   - **Death-detection condition (UNCONFIRMED retail):** snapshot uses
     `Health() <= 0` + 100-tick gate. Retail equivalent likely lives in
     `cls_0x5b4f30 TPlayer`'s Animate (`0x518aa0`) or a Damage hook.
     `FUN_004c5810_Damage` (`cls_0x5b4f30_TPlayer_Damage_4c5810.cpp:1-21`)
     reads/writes HP via `vftptr+0x1c8/+0x1c4` but contains NO call to
     TDeathScreen::Initialize — so death detection lives elsewhere (likely
     in a per-tick Pulse override or post-Damage UpdateState hook).
   - **TDeathScreen static instance address (UNCONFIRMED).** The static
     allocator wrappers `LAB_00533840` / `LAB_00533860`
     (`_data.txt:56606-56608` — entries in a CRT-time init-table at
     `0x5c5860/64`) reference `DAT_0066f500` — but DAT_0066f500 is the
     TDeathPane instance, not TDeathScreen. So the TDeathScreen instance is
     a separate static at an unidentified address (likely adjacent in
     `.data` per Cinematix one-global-per-screen convention used for
     TLogoScreen, TPlayScreen, etc.). The TDeathScreen ctor `0x533c60` has
     no XREFs in the dumps we've read — its instantiator is in a CRT init
     trampoline (likely the LAB at `0x533840` body) not yet extracted.
   - **Impact:** only affects when/how the panel shows. Doesn't affect the
     spec's painting contract. The conservative port (matching snapshot
     verbatim: `TPlayer::Pulse` polls HP+counter, calls
     `TDeathScreen::Initialize()` once) will work.
   - **Resolve:**
     1. `DecompileAddr.java 0x518aa0` (TPlayer::Animate) — search for
        HP-zero gate.
     2. `DecompileAddr.java 0x00533840 0x00533860` — they likely ctor/dtor
        the TDeathScreen static instance and reveal its address.
     3. If (1) finds nothing, dump the full cls_0x5b4f30 vtable + look for
        a Pulse-like slot or "Die" method, or grep recon for `FUN_005338a0`
        cross-refs in unextracted bodies.

4. **`gosar00` 5th sound is dead code.** Assigned to `local_10` at
   `Initialize_5339b0.cpp:25` but never read by `RandomRange(0, 4)` which
   only picks `local_20[0..3] = {goluc, goand, gojha, gooli}`.
   - **Impact:** none in retail (verified by reading the decomp). Could be
     a half-implemented 5th case (intent: pick from 5 not 4) or a leftover
     from a dropped Sarah-character. Don't "fix" it.
   - **Resolve:** none required — flag in spec and move on.

5. **DrawBackground tail-chain target: slot 19 (DrawBackground) vs slot 20
   (Animate) of TButtonPane base.**
   - The OOAnalyzer recon (`cls_0x5b93c4_TPlayer.cpp:1401`) shows
     `cls_0x5b98b8::virt_meth_0x435de0(this)` = `FUN_00435de0` =
     **TButtonPane vtable slot 20 = Animate(bool draw)** per
     `cls_0x5a45c8_TButtonPane_vtable.txt:22`. The snapshot
     `src/death.cpp:83` calls `TButtonPane::DrawBackground()` (slot 19 =
     `FUN_00435d70`).
   - **Impact:** functionally both end up iterating Buttons[] and painting
     each (`Animate` typically calls `RedrawButtons` internally; `DrawBackground`
     calls it directly per `src/button.h:106`). Visible output is the same.
     Performance: `Animate(false)` is the every-tick path so calling it from
     `DrawBackground` would be redundant (Animate is called by the pane's
     own Animate slot too). The more likely truth: OOAnalyzer mislabeled the
     slot — the bytes at the call site are
     `mov eax, [this]; call dword ptr [eax + 0x4c]` (or similar), and slot
     index 19 vs 20 differ only by 4 bytes (`0x4c = 76 = byte offset of
     slot 19`, `0x50 = 80 = byte offset of slot 20`). One-byte misread on
     the encoded offset would flip them.
   - **Resolve:** `DecompileAddr.java 0x00533c10` with hex bytes to see the
     literal call offset, OR disassemble `0x533c10` and look for `call
     dword ptr [eax + 0x4c]` vs `[eax + 0x50]`. Low spec risk: port uses
     `TButtonPane::DrawBackground` (snapshot match) and validates by visual
     parity.

6. **TDeathScreen vtable now fully mapped; Close body still pending.**
   Resolved 2026-05-29 from `cls_0x5b9374__vftable_5b9374.cpp:6-28`:
   the vtable is **20 entries / 80 bytes** with TDeathScreen overriding
   exactly **3 of 19 TScreen slots** — slot 0 (byte 0) = scalar-dtor
   `0x533c90` (body at `cls_0x5b9374.cpp:50-60`); slot 1 (byte 4) =
   Initialize `0x5338a0` (already mapped); **slot 2 (byte 8) = Close
   `0x533910`** — **NEW identification**. The body of `0x533910` is NOT yet
   extracted in this recon snapshot, but the XREFs at `_data.txt:137070-137072`
   show it touches `DAT_0066f500` (TDeathPane pane instance, at `:533910`
   and `:533921`) and `DAT_0066f6f4` (death.dat multi handle, R at `:53392c`,
   W at `:53393b`) — so its body almost certainly: (1) calls
   TDeathPane::Close on `DAT_0066f500`, (2) frees `DAT_0066f6f4`
   (the death.dat multi), (3) writes `DAT_0066f6f4 = 0` (the W XREF at
   `:53393b`). The cursor swap-back and PlayScreen.RemovePane likely
   happen inside TDeathPane::Close's TButtonPane chain.
   - **Impact:** not for paint. For lifecycle / leak prevention only.
   - **Resolve:** `DecompileAddr.java 0x00533910` to extract the body and
     confirm the multi-free + pane-Close sequence inferred from XREFs.

7. **`cls_0x5a3c68` TButton subclass details (member layout for `mbr_0x14`,
   `mbr_0x80`, vtable Draw + hit-test slots).**
   - The post-Initialize patches `b->mbr_0x14 |= 0x40` (the pixelcheck bit)
     and `b->mbr_0x80 = LAB_xxx` (the callback fn ptr) work without the
     spec needing exact field semantics, but a clean port wants typed fields:
     `flags`, `buttonfunc` per snapshot `src/button.h:84,87`. The 27-slot
     vtable of cls_0x5a3c68 has Draw, hit-test, state-set, etc. — none
     extracted in this recon snapshot.
   - **Impact:** the spec doesn't depend on TButton internals beyond "set
     pixel-check flag" and "set callback". The port uses the snapshot
     `TButton` class with snapshot `Draw()` and a portable `OnButton()`
     hit-test that respects pixelcheck. Low risk.
   - **Resolve:** if the project decides to port TButton from retail rather
     than snapshot: `DumpVtable.java 0x5a3c68`, then extract the 27 slots
     individually.

8. **`LAB_00533950 / 970 / 990` (the 3 button callbacks) body content.**
   Snapshot `src/death.cpp:18-34` says they all do `SaveGame.ReadGame +
   DeathPane.Close`. Retail might genuinely have differentiated bodies
   (e.g. LAB_00533990 Exit → TLogoScreen.Initialize() to return to main
   menu, LAB_00533970 Load → open a load-game pane, LAB_00533950 Restart
   → ReadGame).
   - **Impact:** Restart/Load/Exit semantics. Spec-wise this is the dispatch
     contract, not the paint, so the visual reconstruction is unaffected.
   - **Resolve:** `DecompileAddr.java 0x00533950 0x00533970 0x00533990`.

9. **`virt_meth_0x4361b0_144` (the slot called between bg paint and
   button-paint) identity.** This is byte-offset 144 = slot index 36 on the
   cls_0x5b93c4 vtable, pointing to `0x4361b0` (in the TButtonPane
   intermediate range). Most likely the equivalent of
   `PlayScreen.DrawOverhangs()` + `SetClipRect()` from the snapshot
   (`src/death.cpp:78-79`).
   - **Impact:** if this is something else (e.g. focus-highlight redraw), the
     port might miss an overhang paint pass. Verification cost is low; the
     snapshot's `DrawOverhangs + SetClipRect` is a safe match.
   - **Resolve:** `DecompileAddr.java 0x004361b0`. Identify which TPane / 
     TButtonPane base method this is in the cls_0x5a45c8 inheritance chain.

10. **Class-level rename of `cls_0x5b93c4 → TDeathPane`.** Blocked per
    `port_status/TDeathPane.md:7` on cls_0x5a45c8 (the TButtonPane
    intermediate) identification finishing — once its inherited methods are
    factored out of the cls_0x5b93c4 file, the bare-class rename is safe.
    This is a **recon-side bookkeeping item**, not a spec gap. Listed only
    so future tracers don't get spooked by the mislabeled
    `cls_0x5b93c4_TPlayer.cpp` filename.

11. **TDeathScreen 20th vtable slot (byte 76) is `pointer`-typed.** Per
    `cls_0x5b9374__vftable_5b9374.cpp:26` the cls_0x5b9374 vtable is 80
    bytes (TScreen is 76 bytes), and the trailing slot is annotated as
    `pointer` (raw) rather than a `virt_meth_*` function-ptr type. This is
    NOT a fourth override — it's either (a) a Cinematix-convention trailing
    nil/sentinel that OOAnalyzer reads as `pointer` because it's `0x00000000`
    in the binary, (b) a non-method data slot the Cinematix base intends as
    a `type_info*` / RTTI ref / class-name pointer (some Cinematix screens
    parallel-track class identity via a per-vtable string), or (c) an
    artifact of OOAnalyzer mis-decoding the table length. Other TScreen
    subclasses (TLogoScreen, TPlayScreen) should be sanity-checked: if
    they also have a trailing pointer slot in their vtables, this is a base
    convention; if not, it's TDeathScreen-specific.
    - **Impact:** zero on paint. Probably zero on lifecycle (the slot is
      almost certainly never dispatched-through at runtime).
    - **Resolve:** `cat recon/classes_original/cls_0x5a5d18__vftable_5a5d18.cpp`
      (TLogoScreen) and compare vtable length; or `DecompileAddr.java
      0x005b9374+0x4c` (`0x005b93c0`) to read the raw bytes at the 20th
      slot offset.

---

## §15 — Self-verify (mandatory)

- **Cross-check derived coords.** Button (Restart) screen-x = `−regx` = `22`.
  Background width = 640, button-row right edge = `438 + 172 = 610` < 640 →
  fits inside the pane. Button (Restart) right edge = `22 + 170 = 192` <
  Load left edge = `232` → no overlap. Button (Load) right = `232 + 172 =
  404` < Exit left = `438` → no overlap. All 3 buttons sit in the bottom
  62-px row `y ∈ [404, 466)`, leaving `480 − 466 = 14` px of background-only
  below the buttons — matches the visible reference image (the buttons are
  near the bottom but not flush against it).
- **Frame composition.** Background: `screen = pane = (0,0)`,
  `dst = pane_origin + (0,0) = (0,0)` ✓. Buttons: `screen = pane + bitmap-reg
  = (0,0) + (−regx, −regy)` ✓ for all 3.
- **Mirroring.** None applicable — no player/target split.
- **Coord-frame tagging.** Every (x,y) in §2/§3/§4 is in screen frame and
  states so explicitly. No sub-surface-local value printed under a screen
  label.
- **Stranger-test.** A UI programmer with this spec and the 10 dumped PNGs
  could reproduce the panel as: (a) `DrawBitmap(background, 0, 0)`, (b)
  three sequential `DrawBitmap(restart{U|D}, 22, 404)` / `(load{U|D}, 232,
  404)` / `(exit{U|D}, 438, 404)` blits driven by per-button down-state,
  (c) wire VK_TAB/RETURN/ESC + alpha-pixelcheck mouse hit-test to fire 3
  handler stubs. Asset names, exact dims, exact dst, exact drawmode all in
  §2 + §4. Nothing in this spec requires re-opening the recon.

---

## §16 — Reference image / regression check (optional)

`/tmp/death_dump/00_background.png` (640×480) — the full reference frame in
its "no buttons drawn yet" state. The 3 button "silhouettes" baked into the
bottom row at `(22,404)`/`(232,404)`/`(438,404)` are visible as un-pressed
chrome outlines; the live `*U` bitmaps will be painted over them and produce
the final visible state. Cross-overlay any of `01_RestartU.png` / `02_LoadU.png`
/ `03_ExitU.png` onto the background at those positions and the alpha edges
align exactly with the silhouette outlines — this is the implicit visual
regression test for "did REG decode correctly". Run after the port lands.

---

## §17 — Screen-level lifecycle (TDeathScreen wrapper)

This section pins the **lifecycle contract** for the `TDeathScreen`
(`cls_0x5b9374`) TScreen subclass that wraps `TDeathPane`. The pane spec
(§1–§16) covers WHAT the modal looks like and HOW it repaints; this section
covers WHO instantiates it, WHEN it activates, WHAT composition order
TScreen-vs-TPane sit in, and WHAT exits the modal. Forensics scope is the
non-paint orchestration around the panel.

### §17.1 — Activation trigger (player HP ≤ 0)

**Snapshot (confirmed; retail same shape per §14 item 3):**

```
TPlayer::Pulse():                                              # src/player.cpp:204-218
  TCharacter::Pulse()                                          # base tick
  if (!Editor):
    if (Health() <= 0):                                        # HP poll
      if (++deathframe > 100):                                 # ~100-tick grace; lets death anim play
        PlayScreen.SetNextPane(&DeathPane)                     # queue the modal
    else:
      deathframe = 0                                           # reset counter on revive (defensive)
```

Cite: `src/player.cpp:202-218`. `deathframe` is a static global (file-scope
int at `src/player.cpp:202`), so it persists across Pulse calls. The 100-tick
gate is **load-bearing** — it provides a frame budget for the death
animation, knockback, ragdoll, etc. to play out before the modal interrupts
gameplay. The port must preserve this gate (or replace it with a "death
animation finished" predicate driven by the animation state machine — but
the simple frame-counter is the conservative match).

**Trigger condition summary:**

| field | predicate | cite | meaning |
|---|---|---|---|
| `Editor` | `!Editor` | `src/player.cpp:208` | don't trigger in the editor; HP-zero in edit mode is meaningless |
| `Health() <= 0` | true | `src/player.cpp:210` | the player is dead this tick |
| `deathframe > 100` | true after incrementing | `src/player.cpp:212` | sustained-death gate; ~100 ticks @ 24Hz ≈ 4.2 s real-time (per `feedback_framerate_independent_anim`, the port should re-express as `~4.2s` real-time, not literal frame count) |

**Reset behaviour:** `deathframe = 0` whenever `Health() > 0`
(`src/player.cpp:216`). So if the player is revived (e.g. via scripted heal)
the modal is **not** pre-empted — but once SetNextPane fires, the modal is
queued and TPlayScreen.Update will surface it next tick regardless of
subsequent HP changes.

**Retail equivalent (UNCONFIRMED).** The retail TPlayer Pulse-equivalent
hasn't been extracted — see §14 item 3 for resolve steps. Most plausible:
retail TPlayer's Animate (`0x518aa0`) or a derived Pulse slot does the same
HP+counter gate, then calls `TDeathScreen::Initialize` **directly** on the
TDeathScreen static instance (NOT via SetNextPane), because the retail
SetNextPane dispatch at `0x47b4a0` calls the queued pane's **slot 0** —
which for cls_0x5b9374 is the dtor at `0x533c90`, the wrong slot for
activation. The port should call `TDeathScreen::Initialize` directly from
the death-trigger site and skip the SetNextPane queue for the screen
wrapper.

### §17.2 — Composition order: TScreen vs TPane

The retail death modal is **NOT** a pure pane stacked on PlayScreen — it is
a TScreen-wrapper that contains the pane and orchestrates its lifecycle.
The composition is:

```
TScreen (game-mode container, ~= "the whole input/draw context")
└── TPlayScreen (the in-game screen — owns the 3D world view + HUD)
    └── PlayScreen.panes[] (linked list of TPane overlays)
        └── TDeathPane (the modal full-screen pane — added when death triggers)

(separate) TDeathScreen (a sibling TScreen — only instantiated to drive the
                        death-modal lifecycle; does NOT replace TPlayScreen
                        on the screen-stack)
```

**Key invariant.** `TDeathScreen` does **not** displace `TPlayScreen` as
the active TScreen. Per `cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:17`,
the wrapper's Initialize ends by `FUN_0048ed90(&DAT_0066f500, 0xffffffff)`
which is **PlayScreen.AddPane(TDeathPane, exclusive=true)** — i.e. it adds
the TDeathPane to the *existing TPlayScreen's* pane stack as exclusive
(suppressing other PlayScreen input). The TDeathScreen object itself is a
lifecycle-only sidecar — its body is the load+init+cursor+add sequence.

**Why TDeathScreen exists at all (vs a fatter TDeathPane::Initialize).** In
the snapshot the same load+init sequence lives inside `TDeathPane::Initialize`
(`src/death.cpp:36-57` loads death.dat into a member, swaps fullscreen,
NewButton's, AddPane's, SetExclusivePane's). Retail factored those
non-paint responsibilities OUT of TDeathPane (so TDeathPane is a pure pane
that paints whatever death.dat already-loaded contains) and INTO
TDeathScreen (a TScreen subclass that owns the load + cursor + add
sequence). Benefits: (1) TDeathPane::Initialize can be re-run without
re-loading death.dat; (2) the death.dat handle is a screen-lifetime
resource, naturally tied to a TScreen object's lifetime; (3) cursor swap
sits with screen-mode-change code where the rest of the cursor-stack-push
code lives.

**Draw-order semantics — TScreen does NOT paint.** The TDeathScreen
wrapper has no visible drawn elements of its own. Its 16 inherited TScreen
vtable slots (DrawBackground at `0x48fda0`, Animate at `0x48ff00`, Pulse
at `0x46cf20`, ...) are the base no-ops — TScreen's DrawBackground is a
container method, not a paint method, and TDeathScreen never overrides it.
Visual output is 100% produced by TDeathPane's DrawBackground (§5) running
on top of TPlayScreen's normal pane-paint pass.

**Pane composition on the PlayScreen stack while death is active:**

```
PlayScreen.panes[] before death:
  [TPlyrStatusBar, TQuickSpellPane, TPortraitPane, ..., ...]
                                   ↓ player dies, TDeathScreen::Initialize runs
PlayScreen.panes[] after death (TDeathPane appended + flagged exclusive):
  [TPlyrStatusBar, TQuickSpellPane, TPortraitPane, ..., ..., TDeathPane (EXCLUSIVE)]
                                                              ↑
                                              suppresses input + paint on all earlier panes
```

The `exclusive` flag (`0xffffffff = -1` sentinel per `FUN_0048ed90(&DAT_0066f500, 0xffffffff)`)
tells PlayScreen to:
- (input) route ALL mouse/key events to TDeathPane only — the HUD beneath
  is dead until the modal closes;
- (paint) repaint TDeathPane every frame (since it's full-screen at
  640×480, it covers everything underneath anyway — the exclusive flag is
  primarily an input-routing semantic, not a paint-order semantic).

The HUD panes (status bars, portrait, etc.) remain in the panes[] array
during the modal — they just don't receive input or matter for paint.
When TDeathPane closes (post-button-click), it ReleaseExclusivePane's
itself and RemovePane's from the list, and the HUD resumes (or, in the
snapshot's placeholder flow, `SaveGame.ReadGame()` reloads the world
entirely so the panes-array rebuild happens via the load path).

### §17.3 — TDeathScreen::Close (`0x533910`)

**Body NOT YET EXTRACTED** — only XREF-inferred. See §14 item 6.

Inferred from XREFs (`_data.txt:137070-137072` for `DAT_0066f500` and
`:137083-137090` for `DAT_0066f6f4`):

```
TDeathScreen::Close(this):   # @ 0x533910 — inferred, body pending extraction
  TDeathPane::Close(&DAT_0066f500)                              # 0x533910:0x53391a — uses DAT_0066f500 (R/*)
                                                                # (presumably (*DAT_0066f500.vftptr[184/4=46])(...)
                                                                #  invoking TDeathPane::Close at 0x533c00)
  if (DAT_0066f6f4 != 0):                                       # 0x53392c — reads multi handle
    free(DAT_0066f6f4)                                          # frees death.dat multi
    DAT_0066f6f4 = 0                                            # 0x53393b — null-out
  # cursor swap-back: presumably restores the prior cursor sprite (the one that was
  # active before TDeathScreen::Initialize did FUN_0043a020(cursor)). Where the
  # prior cursor is stashed is TBD — likely a static at a known address, or the
  # cursor system has its own stack.
  return
```

**Symmetric to Initialize:** Initialize loads death.dat → calls
TDeathPane::Init → swaps cursor → AddPane. Close calls TDeathPane::Close
→ frees death.dat → swaps cursor back → (RemovePane lives inside
TDeathPane::Close per §6.4's TButtonPane base chain).

**Activation site for Close (UNCONFIRMED for retail).** The 3 button
handlers (`LAB_00533950/970/990`) currently extract as
`SaveGame.ReadGame(); DeathPane.Close();` in the snapshot — so
`DeathPane.Close()` exits the pane-side of the modal, and the screen-side
(TDeathScreen::Close) is presumably called either: (a) as a follow-up from
inside DeathPane.Close itself, (b) by the save-load-path's screen-rebuild
that scrubs all sidecar screens, or (c) by an explicit
TDeathScreen.Close() call in one of the button handlers (most likely Exit).
See §14 items 6 + 8.

### §17.4 — Music / sound on entry

The death-modal's only audio is the **death-gasp SFX** played inside
TDeathPane::Initialize (NOT TDeathScreen) — see §6.2 and §9. One of 4 (5
loaded, see §11 item 2) character-specific gasps is picked via
`RandomRange(0,4)` and played at `vol=127, flags=1, priority=0, pan=80,
distance=700`. Cited:
`recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp:20-35` for
the pick + play sequence.

**No music change on entry — UNCONFIRMED but likely.** Neither
TDeathScreen::Initialize nor TDeathPane::Initialize call any music-control
primitive (no XREF to known music helpers like `FUN_004cc930`-family
play-music functions). The implication: the existing area BGM keeps playing
behind the death modal — which is consistent with retail Revenant's
no-fade-out-on-death behaviour (the player hears the area BGM continue
even after death).

If the port wants to *add* a music change on death (e.g. fade to a
"game over" stinger), that's a Revisited deviation, not a retail-fidelity
port. See `feedback_modernization_stance` — modernization is structural;
audio-design deviations go in a separate Revisited PR with an explicit
[Revisited] toggle.

**No music change on exit — UNCONFIRMED.** Same reasoning: no audio call
in the inferred TDeathScreen::Close body (§17.3) or TDeathPane::Close
(§6.4). The post-click `SaveGame.ReadGame()` will likely trigger area
re-load which resets the BGM via the area-music system — but that's the
save-load path's behaviour, not the death-modal's.

### §17.5 — Cursor management on entry / exit

**On entry (confirmed):** `TDeathScreen::Initialize` at `0x5338a0` calls
`FUN_0046d710("cursor")` to resolve the cursor sprite from a shared multi
(NOT death.dat — the cursor is a PlayScreen-level shared asset; the
ghidra `cls_0x5b9374.cpp:27` recon shows `meth_0x46d710(DAT_0065abc0)` —
i.e. `FUN_0046d710` is called on the `DAT_0065abc0` multi, NOT
`DAT_0066f6f4` death.dat). Then `FUN_0043a020(cursor_sprite)` installs the
sprite as the active cursor. Cite:
`cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp:15-16`.

`DAT_0065abc0` is a shared global multi (`_data.txt:113926` shows
`s_cursor_005e3ee8` XREF'd here, and `_data.txt:129843` shows DAT_0065abc0
itself with 40+ XREFs across the UI codebase). The `"cursor"` string at
`0x5e3ee8` is one entry in DAT_0065abc0 (likely the system cursor multi
that ships with the engine).

**On exit (UNCONFIRMED):** TDeathScreen::Close presumably swaps back the
prior cursor. The mechanism is one of:
- a cursor-system stack (push on FUN_0043a020, pop on a sibling
  `FUN_0043a0xx` function — needs Ghidra extraction);
- a static "prior cursor" pointer stashed by TDeathScreen::Initialize
  (no such write is visible at `0x5338a0` in the extracted body, so this is
  unlikely);
- the cursor stays as the death-modal cursor until the next pane (the
  reloaded game world via SaveGame.ReadGame) explicitly sets its own
  cursor — which is the path TPlayScreen.Initialize takes
  (`cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` — needs grep
  verification).

**Resolve:** grep recon for `FUN_0043a020` callers + a paired
`FUN_0043a0??` cursor-pop function; or `DecompileAddr.java 0x00533910` to
read TDeathScreen::Close's cursor handling directly.

### §17.6 — Exit transitions

The death modal exits when ANY of the 3 buttons fires its handler
(`LAB_00533950 / 970 / 990`). Per §10 + the snapshot
`src/death.cpp:18-34`, all 3 placeholder handlers do:

```
DeathBtnXxx():
  SaveGame.ReadGame()                  # reload last save → triggers full area+player rebuild
  DeathPane.Close()                    # tear down the pane (and the screen wrapper, per §17.3)
```

Three intended distinct exits (per snapshot comment shape, see §11 item 1):

| button | intended behaviour | actual snapshot behaviour | retail (UNCONFIRMED) |
|---|---|---|---|
| Restart | reload current save (or restart-at-area-entry) | `SaveGame.ReadGame()` + `DeathPane.Close()` (placeholder) | likely `SaveGame.ReadGame()` + close — verify via `DecompileAddr.java 0x00533950` |
| Load | open the load-game pane (TLoadGameDef per `LoadGameDef_SPEC.md`) | `SaveGame.ReadGame()` + `DeathPane.Close()` (placeholder) | likely activates `cls_0x?_TLoadScreen` — verify via `DecompileAddr.java 0x00533970` |
| Exit | quit to main menu (TLogoScreen per `MainMenu_SPEC.md`) | `SaveGame.ReadGame()` + `DeathPane.Close()` (placeholder) | likely activates `cls_0x5a5d18_TLogoScreen` (back to title) — verify via `DecompileAddr.java 0x00533990` |

**No fade / transition animation.** The handler bodies (per snapshot)
synchronously close the pane and start a save-load sequence — there is NO
crossfade, NO music sting, NO pause-while-loading screen between the
button click and the reloaded world appearing. The save-load itself takes
~1-2 seconds during which the player sees the standard
"Loading Game …" overlay rendered by TPlayScreen::DrawBackground
(`virt_meth_0x47bd20_TPlayScreen_DrawBackground` per
`agent_screens.txt:160-163`). So the visible transition is:
`[death modal] → click → [Loading Game overlay] → [reloaded world]`.

**No exit on Esc/right-click outside button.** The modal is exclusive
(§10) and has no "cancel/dismiss" affordance — the player MUST pick one
of the 3 buttons. The 3 hotkeys VK_TAB/VK_RETURN/VK_ESCAPE per snapshot
each fire a distinct button (not a generic dismiss); ESC=Exit (button 2),
NOT a cancel. Esc-elsewhere does nothing because the exclusive pane
suppresses other PlayScreen input.

### §17.7 — Port mapping (TDeathScreen → engine equivalent)

The project's screen model is the in-flight `TScreen` + `TPlayScreen`
hierarchy in `src/` (snapshot — TScreen at `src/screen.h`, TPlayScreen at
`src/playscreen.h`). The death-modal can be ported in one of two shapes:

| approach | description | when to use |
|---|---|---|
| **A: fold into TDeathPane::Initialize** | follow the snapshot pattern: TDeathPane owns death.dat loading + cursor swap + AddPane; no separate TDeathScreen class. Trigger site (`TPlayer::Pulse`) calls `DeathPane.Initialize()` directly. | preferred if the project's TScreen layer is minimal and won't port a full TDeathScreen sibling. Matches `src/death.cpp:36-57` 1:1. |
| **B: port TDeathScreen as a sibling TScreen** | follow the retail layering: TDeathScreen is a real class that owns death.dat + drives the pane. Trigger site calls `TDeathScreen.Initialize()`. | preferred if the project has multiple `TScreen` subclasses already and wants to keep that pattern consistent (e.g. for save/load, options, credits sidecar screens). |

**Both approaches produce identical visible behaviour** — the difference
is purely code organization. Snapshot's `src/death.cpp` already implements
approach A; retail implements B. Recommended: **approach A** for the
initial port (matches snapshot, simpler, no new class file), then refactor
to B later if/when other sidecar TScreen subclasses are being ported.

| retail primitive | port primitive | cite |
|---|---|---|
| `TPlayer::Pulse` HP+counter gate → `SetNextPane(&DeathPane)` | `TPlayer::Pulse` (already exists at `src/player.cpp:204-218`) — keep the snapshot body verbatim; replace literal `100` with `time-based ~4.2s` per `feedback_framerate_independent_anim` | `src/player.cpp:202-218` |
| `FUN_0047f670("death.dat", ...)` (TMulti::LoadMulti) | `TMulti::LoadMulti("death.dat")` | `src/death.cpp:40` (snapshot) |
| `FUN_0046d710("cursor")` (cursor sprite resolve from shared multi DAT_0065abc0) | `Cursor.LoadCursor("cursor")` or the project's cursor system equivalent | UI_METHOD_MAP §9 |
| `FUN_0043a020(cursor)` (install active cursor) | `Cursor.SetActive(sprite)` | — (port-side cursor system) |
| `FUN_0048ed90(&DAT_0066f500, 0xffffffff)` (AddPane + exclusive) | `PlayScreen.AddPane(&DeathPane); PlayScreen.SetExclusivePane(&DeathPane, true);` | `src/death.cpp:51-52` + `src/playscreen.h` |
| `TPlayScreen::Animate (0x47b4a0)` drain of `nextpane` slot-0 dispatch | snapshot uses explicit `AddPane(nextpane)` at `src/playscreen.cpp:539-547` — keep this; simpler than slot-0 indirect call | `src/playscreen.cpp:539-547` |
| 3 button callbacks `LAB_00533950/970/990` | snapshot stubs `DeathBtnRestart/Load/Exit` at `src/death.cpp:18-34` — see §17.6 for intended behaviour per button | `src/death.cpp:18-34` |

### §17.8 — Lifecycle state diagram (ASCII)

```
   [PlayScreen running, player alive]
              │
              │  TPlayer::Pulse polls Health() <= 0 each tick
              │
              ▼
   [player dies — Health() <= 0 first frame]
              │
              │  deathframe counts up each Pulse while HP<=0
              │  (~100 ticks ≈ 4.2s @ 24Hz)
              │
              ▼
   [deathframe > 100 — fire SetNextPane(&DeathPane)]
              │
              │  next TPlayScreen.Update / Animate frame
              │
              ▼
   [TDeathScreen::Initialize (retail) OR TDeathPane::Initialize (snapshot path)]
        │
        ├── load death.dat (DAT_0066f6f4 ← TMulti::LoadMulti)
        ├── TDeathPane::Initialize:
        │     ├── TButtonPane::Initialize (zero Buttons[])
        │     ├── play random death-gasp SFX (one of 4 goluc/goand/gojha/gooli)
        │     ├── create 3 buttons (Restart, Load, Exit) at REG-derived positions
        │     ├── post-patch each button: pixelcheck flag + callback fn ptr
        │     └── set dirty flags (mbr_0x60 |= 0xe)
        ├── Cursor.SetActive("cursor" sprite from shared multi DAT_0065abc0)
        └── PlayScreen.AddPane(&DeathPane, exclusive=true)
              │
              ▼
   [TDeathPane visible — paint loop running]
        │
        │  Frame N: DrawBackground stamps background bitmap + 3 buttons
        │  Frame N+1..: paint only repeats buttons (background dirty = false)
        │  Input: mouse hover/click + TAB/RET/ESC routed to TDeathPane only
        │         (exclusive flag suppresses underlying HUD)
        │  Audio: area BGM continues; no new music event
        │
        ▼
   [player clicks Restart / Load / Exit, or hits TAB / RET / ESC]
              │
              ▼
   [LAB_00533950 / 970 / 990 handler fires]
        │
        ├── SaveGame.ReadGame()      # snapshot placeholder; retail TBD per §17.6
        └── DeathPane.Close()
              │
              ├── TButtonPane::Close (frees Buttons[])
              ├── PlayScreen.ReleaseExclusivePane(this)
              ├── PlayScreen.RemovePane(this)
              └── PlayScreen.Redraw()
              │
              │  (retail also: TDeathScreen::Close at 0x533910 — frees DAT_0066f6f4,
              │   swaps cursor back — see §17.3)
              ▼
   [TDeathPane gone, PlayScreen.panes[] reverts to pre-death state]
              │
              │  SaveGame.ReadGame() rebuilds the world from the save file
              │
              ▼
   [reloaded world running, deathframe = 0, player alive again]
```

### §17.9 — UNCONFIRMED summary for the screen-level lifecycle

(Re-stating §14 items relevant to TDeathScreen specifically, plus 2 new
items 12/13 introduced by this section.)

| # | item | resolve step |
|---|---|---|
| 3 | retail TPlayer death-trigger site + retail SetNextPane-vs-direct-call decision | `DecompileAddr.java 0x518aa0` (TPlayer::Animate) + grep for `FUN_005338a0` external callers |
| 6 | TDeathScreen::Close body at `0x533910` (XREF-inferred only) | `DecompileAddr.java 0x00533910` |
| 8 | LAB_00533950 / 970 / 990 button handler bodies (3 dispatch endpoints) | `DecompileAddr.java 0x00533950 0x00533970 0x00533990` |
| 11 | TDeathScreen vtable 20th slot (`pointer` type — sentinel? RTTI? sibling-screen base convention?) | compare with `cls_0x5a5d18__vftable_5a5d18.cpp` (TLogoScreen) length |
| 12 (NEW) | TDeathScreen static instance address (separate from DAT_0066f500 which is TDeathPane) | `DecompileAddr.java 0x00533840 0x00533860` (the static-allocator wrappers); also grep recon for `cls_0x5b9374` ctor (`FUN_00533c60`) XREFs in unextracted CRT-init bodies |
| 13 (NEW) | cursor swap-back mechanism on TDeathScreen::Close (push/pop stack? prior-cursor static? next-screen-resets?) | extract `0x533910` body (item 6); also extract `FUN_0043a020`'s sibling `FUN_0043a0??` functions for a paired "pop cursor" call |

The lifecycle contract for the port is **stable** even with these open
items unresolved:
1. Activation = `TPlayer::Pulse` HP-zero + grace counter → call
   TDeathScreen::Initialize (or TDeathPane::Initialize if folding A) ONCE.
2. Composition = TDeathPane appended to PlayScreen.panes[] as exclusive;
   TScreen wrapper is lifecycle-sidecar, not screen-stack-displacer.
3. Music/SFX = one-shot death-gasp on entry; no music change on entry or
   exit (the area BGM keeps playing).
4. Cursor = swap to "cursor" sprite on entry; restore on exit (mechanism
   TBD per item 13).
5. Exit = button click fires handler → close pane + (retail) close screen
   wrapper → save-load path rebuilds world.
