# PlayScreenOverlay (TPlayScreen::DrawBackground, slot 4) — Forensics SPEC

> **Status:** `forensics-complete`, with a *task-reframing* finding the reader
> must read first (§0). The slot-4 body does **NOT** paint a full-screen text
> overlay; the strings the task brief named ("system is autosaving", "Please
> wait...", "Autosave [", "newgame", "Loading Game \"%s\"... Please Wait") are
> **TTextBar message-log lines** emitted by this controller, not a separately
> drawn HUD/overlay. There is also no separate `TLoadingScreen`. This spec
> documents (a) what slot-4 actually does, (b) why no overlay panel exists, and
> (c) the message-log lines + dispatch paths so a reconstructor can wire the
> behavior correctly against the already-specced `TTextBar` panel.

## §0 — Sources & status

- Class: `TPlayScreen` (vtable `cls_0x5a5320`, ctor-rename
  `cls_0x5a5320_TCharacter.cpp` is a Ghidra merge artifact — see [[feedback-recon-agent-protocol]] §Ghidra class-merging caveat).
- Method: vtable slot 4 = `TPlayScreen::DrawBackground` = `FUN_0047bd20` =
  `virt_meth_0x47bd20` (range `0x0047bd20..0x0047c2b7`, body 0x597 bytes).
  Inherited base: `TScreen::DrawBackground` at `0x48fda0` (called as super at the
  bottom of slot-4 path, `0x47c21f`).
- **Body NOT extracted by the bulk Ghidra dump.** The closest the recon got is
  the rename entry in
  `recon/discovered/renames/agent_screens.txt:158-163` and the per-string XREFs
  in `recon/ghidra/_data.txt:86504-86575`. The body in this spec is decoded from
  `objdump -d data/Revenant.exe` over `0x47bd20..0x47c2b7` (raw x86), cited
  inline as `47bd20:…`. The slot label is confirmed by the vtable file
  `recon/ghidra/cls_0x5a5320__vftable_5a5320.cpp:11`
  (`virt_meth_0x47bd20_TPlayScreen_DrawBackground` at table offset 16 = slot 4).
- Recon files consulted:
  - `recon/ghidra/cls_0x5a5320__vftable_5a5320.cpp` (slot identity)
  - `recon/discovered/renames/agent_screens.txt:140-192` (slot map + string anchors)
  - `recon/discovered/renames/agent_ui_wave2_textbar_bottom.txt:30-50, 164-180`
    (TTextBar `cls_0x5a5560`, instance global `0x65c5d0`, `PrintLine` = `FUN_0054d170`)
  - `recon/discovered/renames/agent_ui_textbar.txt` (TTextBar reassessment)
  - `recon/classes_original/cls_0x5701f0.cpp` (the chat/network-message
    add helper that `0x5701f0` is — NOT a TTextBar PrintLine; see §1)
  - `recon/discovered/renames/agent_world.txt:293-298` (`FUN_0049d800` =
    `TextResource::Lookup`, `FUN_0054d170` = `PrintLine`)
  - `docs/ui/forensics/TTextBar_SPEC.md` (the panel that actually renders the
    log lines — load-bearing for §5 / §13)
  - `recon/ghidra/_data.txt:86504-86600` (strings + XREF anchors)
  - `recon/ghidra/_data.txt:138019-138033` (global `0x676668` cross-refs — the
    chat/network message sink)
- Status: `forensics-complete` (controller). No overlay panel exists, so §2 (asset
  roster), §3 (frame table), §4 (element layout), §7 (effects/shadows), §8 (text
  rendering table) are intentionally short ("not applicable, why" — see each
  section). The substantive content is §5 (dispatch + log emission), §6
  (controller-helper pseudocode), §9 (autosave-timer animation/timing), §11
  (retail bugs), §12 (controller pseudocode), §14 (UNCONFIRMED).
- **Errors to close if re-specifying:** none (no prior port).
- **Snapshot note (supplementary — superseded by retail):** no 1998/99
  `src/playscreen.cpp` overlay code matches these strings; the snapshot tree's
  autosave path is non-existent (autosave was added late). The shipped retail is
  authoritative for the dispatch layout described here.

---

## §1 — Overview

**TPlayScreen::DrawBackground** (slot 4 — inherits the conceptual *paint
background* role from `TScreen`) is named misleadingly: most of its body is
**non-paint controller logic** that runs every screen tick before the call to
`TScreen::DrawBackground` (the actual background paint pass) at the end
(`47c21f: call 0x48fda0`).

The body is a top-level dispatcher driving:

1. **Editor wake**: drives an editor-only "wake the host" path when a 3-state
   transition lands and `0x659800==0`; calls `meth_0x4cee70` on the global object
   pointed to by `[0x667fcc]`. (47bd41-47bdeb) — NOT in scope of an overlay.
2. **Cinematic / movie hook**: when global `0x65a62c != 0` and `[ebp+0x5dc] != 0`
   (a screen-mode toggle), runs the cine/movie advance via `meth_0x48eb00`
   (TScreen-side) and `meth_0x48e610` on the cine controller `0x65a618`, then
   pokes `[0x65cb30] -> vtable+0x28`. (47bdf6-47be27) — NOT in scope.
3. **Autosave fire** (47be28-47bf01): countdown timer; on expiry, emit 3
   **TTextBar log lines** announcing the autosave — `"The system is autosaving!
   You may experience temporary problems!"`, `"Please wait..."`, and the
   formatted `"Autosave [<slot>]"` — and set the deferred autosave flag
   `[ebp+0x5e8] = 1` so the actual save happens later in the tick chain.
4. **Deferred newgame/loadgame announcement** (47bf01-47bfdf): if
   `[ebp+0x5e4] != 0` (a pending "new/load game" flag), look up the
   character/save display name, optionally lookup the `"loadgamefmt"`
   localization key as `"Loading Game \"%s\"... Please Wait"`, **PrintLine**
   that to the TTextBar message log, then queue the actual load via
   `FUN_0047c580` (`meth_0x47c580`), and call the editor-wake path again.
5. **Deferred savegame announcement** (47bfe5-47c0be): the mirror of (4) for
   `"curgame"` / `"savegamefmt"` / `"Saving Game \"%s\"... Please Wait"`.
6. **Per-frame mode flags** (47c0be-47c220): clear pending bits at
   `[ebp+0x670, 0x67c, 0x674, 0x678, 0x5e0, 0x6d4]` (each a small "mode change"
   queue) and dispatch their actions (cursor/area/sector/animator/script).
7. **End-area cleanup** at 47c176-47c21d: when `[0x65a298] != 0`, tear down the
   bottom-bar global `0x65a890`, reset the message-bar host `0x6668d8`, then
   reset the main player container `0x65b8b0` and the inventory state.
8. **Super-call**: `TScreen::DrawBackground` at `48fda0` — *this* iterates the
   pane list and calls each pane's paint. The HUD panes (StatusBar, TextBar,
   sidebars, BottomBar, etc.) draw themselves there.
9. **Tail (47c224-47c2b1)**: on `[ebp+0x1c]` ≤ 0 or specific mode bits, runs
   the modal cleanup `meth_0x52b9f0` (UI cursor reset), and on
   `[0x6682bc] != 0 && [ebp+0x54] == 0 && mode_bits_7==0` runs a multiplayer
   disconnect popup: looks up `mpdisconnectedserver` / `mpdisconnectedtimeout`
   / `mpdisconnected` keys by disconnect-code (3/2/1), calls `FUN_0053c060`
   (ShowPopupScreen, kind=9), and sets `[0x65cb08] = 0x65d358` (probably
   redirect-next-screen) before invoking `meth_0x48ea40` (TScreen-side cleanup).

The strings the brief enumerated are all consumed by paths (3), (4), (5), or
the multiplayer-disconnect tail of (9). **None of them is rendered as a
standalone full-screen overlay.** Paths (3)/(4)/(5) feed the message log
(`TTextBar`), and the disconnect strings are *script-key arguments* passed to
the popup-screen launcher (which routes through the existing menu system — see
`MainMenu_SPEC.md` / `PopupDef_SPEC.md`), not text drawn by `TPlayScreen`.

The actual *backdrop fill* (the literal "draw background" that the name implies)
is done by the inherited `TScreen::DrawBackground` at `0x48fda0`, which is the
last thing slot 4 calls (47c21f). The decompile in the field-name tables for
`TScreen::DrawBackground` shows it fills the display with a backdrop tile / does
no text. Per the brief's "may overlap with LoadingScreen — distinguish" — there
is **no separate `TLoadingScreen` class**. The loading message goes into the
existing `TTextBar` (already specced).

### One-paragraph plain-language summary

When the player is mid-game, every screen tick the engine ticks down a 10-min
autosave timer and, when it hits zero, dumps a 3-line announcement into the
in-game message log. If the player just bound the F-key shortcut "load game"
or "save game" (or a script triggered one), the same method also dumps a
"Loading/Saving Game \"<name>\"..." announcement into the message log, then
kicks the actual load/save into the next tick. If the player just got booted
from a multiplayer session, it pops the "you got disconnected" popup. The
"DrawBackground" name on this method is a misnomer — most of the body is
deferred-event dispatch; the actual screen backdrop is rendered by the inherited
`TScreen::DrawBackground` at the very end of the body.

---

## §2 — Asset roster

**Not applicable.** This method draws no bitmaps and loads no assets. It only:
- emits text into `TTextBar` (which owns its own assets — see
  [`TTextBar_SPEC.md`](TTextBar_SPEC.md) §2),
- triggers `FUN_0053c060` (popup-screen launcher, which loads `"cursor"` via
  `FUN_0046d710` — `recon/ghidra/_data.txt:115103`), and
- super-calls `TScreen::DrawBackground` (which uses *that* class's assets, out
  of scope).

If a future reconstructor wants the "autosaving!" message to draw as a real
modal banner (a deviation from retail), it must own that decision; retail does
not do this.

---

## §3 — Coordinate frames & surfaces

**Not applicable.** Slot 4 takes no coordinates, defines no surfaces, draws no
geometry. The only screen-touching outputs are:
- `TTextBar::PrintLine(0x65c5d0, fmtStr, gameNameStr)` calls — those compose into
  the TTextBar's own scratch surface (see
  [`TTextBar_SPEC.md`](TTextBar_SPEC.md) §3 — fg-local `(4, 9)` stacking).
- `FUN_0053c060(scriptKey, 9)` — this triggers a separate screen (popup), which
  has its own frame model.
- The super-call `TScreen::DrawBackground` does the backdrop fill (out of scope
  of this slot — handled by the parent class).

Frame table (the only frames involved in this slot's behavior are *re-used*
from other panels; this slot owns none):

| frame | parent | anchor | origin in parent | what is expressed in it | cite |
|---|---|---|---|---|---|
| TTextBar fg-local | TTextBar pane | — | (see TTextBar §3) | the 3 autosave / load / save log lines | `TTextBar_SPEC.md` |
| popup-screen-local | screen | — | (popup screen owns it) | the `mpdisconnected*` popup | `MainMenu_SPEC.md` / `PopupDef_SPEC.md` |

---

## §4 — Static element layout

**Not applicable.** No elements drawn by this slot.

---

## §5 — Draw order / composition

The "draw order" of slot 4 is actually a **dispatch order**, not a paint order.
The single paint-equivalent step is the super-call at the end. The ordered
sequence:

| seq | step | code range | what happens |
|---|---|---|---|
| 1 | acquire frame timer | `47bd20-47bd2c` | calls `meth_0x49a380` on global `0x65abc8` (frame-timer / FPS counter — out of scope) |
| 2 | early-out if `[ebp+0x5d4] != 0` | `47bd31-47bd3b` | a "screen has been closed / superseded" flag — skips entire body and jumps to `47c2b1` (epilogue) |
| 3 | editor-wake path | `47bd41-47bdeb` | if `[0x66829c]!=0 && [ebp+0x48]==3 && [0x659800]==0` and the host returns the magic 0xb mode — wakes the editor host (`meth_0x47c580(this, 0)`, dispatch through `[0x667fcc]`, command 5/7 via `0x6668d8`); skip if any predicate fails |
| 4 | cinematic advance | `47bdf6-47be27` | only when `[0x65a62c]!=0 && [ebp+0x5dc]!=0 && !meth_0x48eb00(this)` — drives the movie/cine controller at `0x65a618` |
| 5 | **autosave fire** | `47be28-47bf01` | if game active (`[0x6682bc]!=0`) and autosave-enabled (`[0x5d7aa4]!=0`), decrement timer `[0x5d7aac]`; on 0, emit 3 TTextBar lines and set `[ebp+0x5e8]=1`. See §6.1. |
| 6 | **deferred newgame/loadgame** | `47bf01-47bfdf` | if `[ebp+0x5e4]!=0`, build the announcement, PrintLine to TTextBar, kick `meth_0x47c580(this, 0)`, clear flag. See §6.2. |
| 7 | **deferred savegame** | `47bfe5-47c0be` | if `[ebp+0x5e8]!=0`, build the announcement, PrintLine to TTextBar, clear flag. See §6.3. Note this also picks up the autosave-set flag from step 5. |
| 8 | cursor / area dispatch | `47c0be-47c107` | bit flags at `[ebp+0x670, 0x67c]` — UI cursor + scratch reset on `0x6668d8` |
| 9 | sector load (deferred) | `47c107-47c138` | flag `[ebp+0x674]` — calls `FUN_0044c320(globals)` with 4 args from `0x65a270..0x65a27c` (likely sector-load coords) |
| 10 | object spawn (deferred) | `47c138-47c14b` | flag `[ebp+0x678]` — calls `FUN_0044cd20()` (no args, probably "process spawn queue") |
| 11 | reset world after fade | `47c14b-47c161` | flag `[ebp+0x5e0]` — `FUN_0047de30(this)` + `FUN_0047e0f0(this)` (probably FadeOutComplete + ApplyResetWorld) |
| 12 | script reload | `47c161-47c176` | flag `[ebp+0x6d4]` — `FUN_0047f3a0(this)` (probably script reload after F-key) |
| 13 | **end-area cleanup** | `47c176-47c21d` | if `[0x65a298]!=0`, full teardown: stop `0x65a890` (bottom-bar/log host), reset message host `0x6668d8`, reset player container `0x65b8b0`, push 0x40-byte block at `[esp+0x10]` (size `0x2710 = 10000`) into `FUN_00453940` (probably scroll/log reset), and clear inventory display `[0x666974]/[0x666970]` |
| 14 | **super-call** | `47c21f` | `TScreen::DrawBackground(this)` = `FUN_0048fda0` — *this is where the actual background paint + per-pane Animate calls happen* |
| 15 | modal cursor reset | `47c224-47c241` | reads `[ebp+0x1c]` (pane stack depth), tests bit 3 of `[ebp+4*depth+0x2c]` (pane flag) — calls `meth_0x52b9f0` on `0x65b4f0` (UI cursor reset) if bit clear |
| 16 | **MP disconnect popup** | `47c241-47c2ac` | if connected (`[0x6682bc]!=0`) and on-screen status `[ebp+0x54]==0` and no pane mask bits 7 — query disconnect reason via `meth_0x577be0(0x676738)`, select script key (`"mpdisconnectedserver"` / `"mpdisconnectedtimeout"` / `"mpdisconnected"`), call `FUN_0053c060(key, 9)` (ShowPopupScreen kind 9), set `[0x65cb08]=0x65d358` (probably "go to main menu"), call `meth_0x48ea40(this)` |
| 17 | epilogue | `47c2b1-47c2b6` | `pop ebp; pop ebx; add esp,0x10; ret` |

**Important:** the only step in the above list that resembles "drawing" is step
14 (the super-call). All other steps are **state changes / message emission /
deferred dispatch**, despite living inside a method named `DrawBackground`.

---

## §6 — Algorithms (pseudocode per helper)

The three message paths are the substantive deliverable here. They are all
*controller code*, not paint code, but they are what produces the strings the
task brief enumerated.

### §6.1 Autosave fire (47be28-47bf01)

```
struct {
  // file offset                         decompile name           initial / units
  DAT_005d7aa4 : int = 1;                // AutosaveEnabled (bool flag)
  DAT_005d7aa8 : int = 0x3840 (=14400);  // AutosaveIntervalTicks
                                         //   (10 min @ 24Hz sim tick;
                                         //    `_data.txt:` initial value `40 38 00 00`)
  DAT_005d7aac : int = 0x3840;           // AutosaveTimer (countdown, ticks)
  DAT_006682bc : int;                    // GameActive (non-zero in-game)
  DAT_005d74a0 : char[] = "The system is autosaving!  You may experience temporary problems!";
  DAT_005d74e4 : char[] = "Please wait...";
  DAT_005d74f4 : char[] = "Autosave [";
  DAT_005d7500 : char[] = "]";
  DAT_00676774 : char[260];              // SlotIdBuffer (set elsewhere — see UNCONFIRMED §14 item 1)
  DAT_00676668 : ChatMessageSink*;       // chat / network text channel (see §6.4)
  EBP+0x5e8    : int;                    // *DeferredSaveAnnounce* — slot 4 sets to 1, step (7) consumes
};

void TPlayScreen_AutosaveFire(TPlayScreen* this /*= ebp*/) {
  // 47be28
  if (DAT_006682bc == 0) return;                // game not active → skip
  if (DAT_005d7aa4 == 0) return;                // autosave disabled → skip
  // 47be43
  int t = --DAT_005d7aac;                       // tick the countdown
  if (t != 0) return;                           // not yet time
  // 47be54-47be5f
  DAT_005d7aac = DAT_005d7aa8;                  // reset timer = interval

  // 47be5a-47be7d — emit 3 lines into the chat/network text channel
  ChatChannel_Add(DAT_00676668, /*channel*/ -1, DAT_005d74a0);  // "The system is autosaving!..."
  ChatChannel_Add(DAT_00676668, /*channel*/ -1, DAT_005d74e4);  // "Please wait..."

  // 47be82-47bef0 — build "Autosave [<slot>]" into stack buf (ebp+0x5f0, 260 bytes)
  char buf[260]; buf[0x103]='\0';
  size_t n; size_t rem;
  memcpy(buf, "Autosave [", 0x103);             // copies + null-cap at 0x103
  n   = strlen(buf);                            // 10
  rem = 0x104 - n;                              // remaining
  memcpy(buf+n, DAT_00676774, rem - 1);         // append slot text (set elsewhere)
  buf[n + rem - 1] = '\0';
  n   = strlen(buf);
  rem = 0x104 - n;
  memcpy(buf+n, "]", rem - 1);                  // append "]"
  buf[n + rem - 1] = '\0';

  ChatChannel_Add(DAT_00676668, /*channel*/ -1, buf);            // "Autosave [<slot>]"

  // 47bef5 — set the "do save" flag for step (7) to pick up THIS tick
  this->mbr_0x5e8 = 1;                          // DeferredSaveAnnounce/DeferredSave
}
```

**Source citations**
- Whole block: `objdump 47be28..47bef5`.
- `DAT_005d7aa4 = 1` and `DAT_005d7aa8 = 0x3840 = 14400` (=10 min @ 24Hz):
  `_data.txt: DAT_005d7aa4 01 00 00 00`, `DAT_005d7aa8 40 38 00 00`.
- `DAT_005d74a0..005d7500` text + XREFs: `_data.txt:86504-86523` (anchors
  `s_The_system_is_autosaving!_..._005d74a0`, `s_Please_wait..._005d74e4`,
  `s_Autosave_[_005d74f4`, `DAT_005d7500` byte `]`).
- `0x676668` = chat/network text channel (sink ChatMessageSink): pushed by
  `47be67/47be7d` (`mov ecx, 0x676668; call 0x5701f0`). The call target
  `0x5701f0` is **NOT** `TTextBar::PrintLine` — it is a network/chat
  enqueue helper (`recon/classes_original/cls_0x5701f0.cpp:19-67`) that
  pushes a typed line (type byte `0x03`) onto an internal byte buffer and,
  when `DAT_0067682c != 0` (network connection), forwards it via
  `vtable[+0x04]` to `DAT_00676884` (the network sender). When not
  connected, it falls through to `meth_0x578ec0(0x676738, 1)` which is the
  local TTextBar / on-screen-log emitter (`agent_ui_textbar` —
  `0x676738` is the editor/game TTextBar host, see UNCONFIRMED §14 item 2).
- AutosaveTimer cross-refs: `0x5d7aac` write at `47be49/47be5f`, read at
  `47be43` (`_data.txt` XREF[3]).
- `DAT_00676774`: only-XREF is the `47beb9` push (`_data.txt:138186`); the
  *writer* is in the actual autosave code outside this slot — see §14 item 1.

### §6.2 Deferred newgame / loadgame announcement (47bf01-47bfdf)

```
struct {
  EBP+0x5e4    : int;            // DeferredLoadAnnounce (set by F-key/load-game cmd)
  EBP+0x5ec    : int;            // SaveGameIndex (>=0 = list entry, <0 = use script key)
  EBP+0x5f0    : char[260];      // BuiltSaveName scratch (in-place name buffer)
  DAT_0065a628 : SaveEntry**;    // SaveGameList (indexed by EBP+0x5ec)
                                 // (each entry: *entry = pointer to display name)
  DAT_0065d4d0 : TextResource*;  // localization dictionary (passes ecx)
  DAT_0065c5d0 : TTextBar*;      // message-log instance (PrintLine target)
  DAT_00668154 : int;            // GameType flag (0 = singleplayer-mode, !=0 = ...)
  DAT_0065c610 : int;            // PrintGameHeader gate (announce in TTextBar?)
  // strings:
  s_newgame_005d7504 : char[] = "newgame";        // TextResource key (header)
  s_newgame_005d750c : char[] = "newgame";        // TextResource key (display name fallback)
  s_New_Game_005d7514 : char[] = "New Game";      // literal fallback if dict miss
  s_loadgamefmt_005d7520 : char[] = "loadgamefmt";              // header format key
  s_loadgamefmt_005d752c : char[] = "loadgamefmt";              // (duplicated; lookup arg)
  s_Loading_Game_005d7538 : char[] = "Loading Game \"%s\"... Please Wait"; // literal fallback
};

void TPlayScreen_AnnounceLoadGame(TPlayScreen* this) {
  // 47bf01 — guard
  if (this->mbr_0x5e4 == 0) return;

  // 47bf0d-47bf61 — pick the display name
  const char* nameStr;
  if (this->mbr_0x5f0[0] != 0) {
    // there's a name already pre-built in the in-place buffer — use it
    nameStr = &this->mbr_0x5f0[0];
  } else {
    int idx = this->mbr_0x5ec;
    if (idx >= 0) {
      // pick from the save-game list
      // 47bf27-47bf30: ecx = DAT_0065a628; edx = ecx[idx]; nameStr = *edx
      nameStr = *((const char**)DAT_0065a628[idx]);
    } else {
      // dictionary lookup: TextResource::FindKey("newgame")
      // 47bf34: push "newgame"; ecx=DAT_0065d4d0; call FUN_0049d6d0 (FindKey)
      if (TextResource_FindKey(DAT_0065d4d0, "newgame") < 0) {
        // not present → use literal "New Game"
        nameStr = "New Game";
      } else {
        // present → look up the value
        // 47bf47: push "newgame"; ecx=DAT_0065d4d0; call FUN_0049d800 (Lookup)
        nameStr = TextResource_Lookup(DAT_0065d4d0, "newgame");
      }
    }
  }

  // 47bf63-47bfa8 — optionally emit the header "Loading Game \"<name>\"... Please Wait"
  if (DAT_00668154 == 0 || DAT_0065c610 != 0) {     // skip if [SP-mode]==0 AND [headerGate]==0 ; else continue
    const char* fmt;
    if (TextResource_FindKey(DAT_0065d4d0, "loadgamefmt") < 0) {
      fmt = "Loading Game \"%s\"... Please Wait";   // literal fallback (005d7538)
    } else {
      fmt = TextResource_Lookup(DAT_0065d4d0, "loadgamefmt");
    }
    TTextBar_PrintLine(DAT_0065c5d0, fmt, nameStr);  // FUN_0054d170
  }

  // 47bfab-47bfd0 — kick the actual load
  // 47bfae: al = mbr_0x5f0[0]; if 0 → use mbr_0x5ec (load by index), else use buf (load by name)
  if (this->mbr_0x5f0[0] == 0) {
    // 47bfb2-47bfbe: push mbr_0x5ec; ecx=0x65a618 (load controller); call FUN_0048e5b0
    LoadController_LoadByIndex(DAT_0065a618, this->mbr_0x5ec);
  } else {
    // 47bfc5-47bfcb: push edi (=&mbr_0x5f0); ecx=0x65a618; call FUN_0048df70
    LoadController_LoadByName(DAT_0065a618, &this->mbr_0x5f0[0]);
  }

  // 47bfd0-47bfd9
  meth_0x47c580(this, 1);          // [TPlayScreen-private cleanup before load]
  this->mbr_0x5e4 = 0;             // clear DeferredLoadAnnounce
}
```

**Source citations**
- `47bf01-47bfd9` all from `objdump`.
- `0x5d7504/0x5d750c/0x5d7514/0x5d7520/0x5d752c/0x5d7538` strings: `_data.txt:86524-86549`.
- `0x65a628` = SaveGameList: see `meth_0x48e5b0/48df70` references — load-by-index entry points.
- `0x65d4d0` = TextResource: `_data.txt` XREF block has it as the ecx target
  for both `FUN_0049d6d0` and `FUN_0049d800`; `agent_world.txt:295` confirms
  `FUN_0049d800 = TextResource::Lookup`.
- `0x65c5d0` = TTextBar global instance:
  `agent_ui_wave2_textbar_bottom.txt:30-34, 165`
  ("`DAT_0065c5d0 = &PTR_FUN_005a5560` directly").
- `0x54d170` = `TTextBar::PrintLine`: `agent_world.txt:296`.

### §6.3 Deferred savegame announcement (47bfe5-47c0be)

Structurally identical to §6.2 with these substitutions:

| §6.2 (load) | §6.3 (save) | cite |
|---|---|---|
| guard `mbr_0x5e4 != 0` | guard `mbr_0x5e8 != 0` | `47bf01` vs `47bfdf` |
| key `"newgame"` | key `"curgame"` | `005d7504` vs `005d755c` |
| literal `"New Game"` | literal `"Current Game"` | `005d7514` vs `005d756c` |
| key `"loadgamefmt"` | key `"savegamefmt"` | `005d7520` vs `005d757c` |
| literal `"Loading Game \"%s\"... Please Wait"` | literal `"Saving Game \"%s\"... Please Wait"` | `005d7538` vs `005d7594` |
| `LoadController_LoadByIndex(0x65a618, idx)` (`FUN_0048e5b0`) | `SaveController_SaveByIndex(0x65a618, idx)` (`FUN_0048df40`) | `47bfbe` vs `47c0a6` |
| `LoadController_LoadByName(0x65a618, name)` (`FUN_0048df70`) | `SaveController_SaveByName(0x65a618, name)` (`FUN_0048d720`) | `47bfcb` vs `47c0b3` |
| cleanup before load `meth_0x47c580(this, 1)` | — (none — save doesn't need pre-cleanup) | `47bfd4` vs none |
| clear `mbr_0x5e4 = 0` | clear `mbr_0x5e8 = 0` | `47bfd9` vs `47c0b8` |

Cited strings: `_data.txt:86550-86574`. Note: **the autosave path at §6.1 sets
`mbr_0x5e8 = 1`**, so the autosave block is what triggers a *real* save through
the same code path that user-initiated saves use — the "Autosave [N]" text is
just the chat-line announcement; the actual save flows through §6.3.

### §6.4 Chat / network text channel (the autosave message sink)

The three autosave lines route through `meth_0x5701f0` (the `cls_0x5701f0`
method), **not** through `TTextBar::PrintLine`. Verbatim from
`recon/classes_original/cls_0x5701f0.cpp:19-67`:

```
undefined4 __thiscall meth_0x5701f0(cls_0x5701f0 *this, int channel /*=-1*/) {
  // called as: mov ecx, 0x676668; push channel; push msg; call 0x5701f0
  // the 'msg' string is what arrives in 'in_stack_00000008'
  if (msg != null && msg[0] != '\0') {
    // 1) append 0x03 (type byte = chat line)
    // 2) append channel low 16-bits little-endian
    // 3) append the msg string
    // 4) if DAT_0067682c != 0 (network connected),
    //      forward the buffer to DAT_00676884 via vtable[+0x4]
    //    else
    //      meth_0x578ec0(DAT_00676738, 1)
    //    (the local TTextBar host injection)
  }
  return ...;
}
```

This is why the autosave / chat lines appear in the in-game TTextBar log: when
not multiplayer, `meth_0x578ec0` is the local emitter (UNCONFIRMED §14 item 2)
that ultimately pushes to the same TTextBar.

### §6.5 MP-disconnect popup (47c241-47c2ac)

Selects a script key from the disconnect-reason int, launches the popup screen
via `FUN_0053c060(key, kind=9)`, then sets a "go-to-main-menu" trampoline.

```
struct {
  DAT_006682bc : int;                    // ConnectedToServer (non-zero in MP session)
  EBP+0x54     : int;                    // PaneStackDepth / "on a screen?"
  EBP+0x1c     : int;                    // CurrentPaneIndex
  EBP+...+0x2c : PaneFlags[];            // per-pane bits — bit 3 = "modal exclusive"
  DAT_00676738 : NetworkSession*;        // (also the chat fallback target — see §6.4)
  DAT_0065cb08 : ScreenTrampoline*;
  DAT_0065d358 : MainMenuTrampoline;     // the destination after disconnect
  s_mpdisconnectedserver_005d75b4 : "mpdisconnectedserver";  // disconnect code 3
  s_mpdisconnectedtimeout_005d75cc : "mpdisconnectedtimeout"; // code 2
  s_mpdisconnected_005d75e4       : "mpdisconnected";        // code 1 (also fallback)
};

void TPlayScreen_MPDisconnectPopup(TPlayScreen* this) {
  if (DAT_006682bc == 0) return;          // not connected → no popup
  if (this->mbr_0x54 != 0) return;        // we're on a transitional/utility screen → defer

  // tail also reads per-pane flag bits 7 — only run popup if those bits clear
  int paneFlags = (this->mbr_0x1c > 0) ? this->mbr[mbr_0x2c + 4*mbr_0x1c] : 0;
  if (paneFlags & 7) return;

  int reason = meth_0x577be0(0x676738);   // NetSession::DisconnectReason()
  if (reason == 0) return;                // 0 = not disconnected
  const char* key;
  switch (reason) {
    case 3: key = "mpdisconnectedserver";  break;
    case 2: key = "mpdisconnectedtimeout"; break;
    case 1: key = "mpdisconnected";        break;
    default: key = lookupPriorString();    // [esp+0x8] — see UNCONFIRMED §14 item 3
  }

  FUN_0053c060(key, /*kind*/ 9);          // ShowPopupScreen — pops a modal popup
                                          // with the localized "you got disconnected" message

  DAT_0065cb08 = DAT_0065d358;            // queue main-menu trampoline
  meth_0x48ea40(this);                    // TScreen-side: kick the trampoline next tick
}
```

**Source citations**
- All addrs from `objdump 47c241-47c2ac`.
- String XREFs: `_data.txt:86575-86589`.
- `FUN_0053c060` references `"cursor"` and `"popup"` script keys
  (`_data.txt:115103, 115108`) — confirms popup screen role.

---

## §7 — Effects & shadows

**Not applicable.** This method draws no glyphs and no sprites. The downstream
TTextBar log lines inherit TTextBar's text rendering (`0x400` 3-pass black
shadow, see [`TTextBar_SPEC.md`](TTextBar_SPEC.md) §7); a reconstructor must NOT
re-implement shadow logic here — emit the line and let TTextBar own it.

---

## §8 — Text rendering

**Not applicable for this slot.** Text *content* is produced here; text
*rendering* is owned by `TTextBar` and the popup-screen subsystem. The slot 4
contract is:

| string source | format string | what feeds `%s` | emitted via | rendered by | cite |
|---|---|---|---|---|---|
| `0x5d74a0` | (none, literal) `"The system is autosaving!  You may experience temporary problems!"` | — | `ChatChannel_Add(0x676668, -1, str)` | TTextBar (local fallback path) | `47be5a, _data.txt:86504` |
| `0x5d74e4` | (none, literal) `"Please wait..."` | — | `ChatChannel_Add(0x676668, -1, str)` | TTextBar | `47be71, _data.txt:86509` |
| concat of `0x5d74f4 + DAT_00676774 + 0x5d7500` | `"Autosave [" + <slot> + "]"` (built in stack buf) | — | `ChatChannel_Add(0x676668, -1, str)` | TTextBar | `47be8d/47beb9/47bee2, _data.txt:86514-86523` |
| TextResource("loadgamefmt") OR literal `0x5d7538` | `"Loading Game \"%s\"... Please Wait"` | save name (built/looked-up earlier) | `TTextBar::PrintLine(0x65c5d0, fmt, name)` | TTextBar | `47bf73-47bfa3, _data.txt:86537-86549` |
| TextResource("savegamefmt") OR literal `0x5d7594` | `"Saving Game \"%s\"... Please Wait"` | save name | `TTextBar::PrintLine(0x65c5d0, fmt, name)` | TTextBar | `47c05b-47c08b, _data.txt:86563-86574` |
| TextResource("newgame") OR literal `0x5d7514` | `"New Game"` (display name fallback) | — | (passed as `%s` arg to the line above, not printed itself) | (none) | `47bf34-47bf5a, _data.txt:86524-86535` |
| TextResource("curgame") OR literal `0x5d756c` | `"Current Game"` (display name fallback) | — | (passed as `%s` arg) | (none) | `47c01c-47c042, _data.txt:86550-86562` |
| `mpdisconnectedserver / -timeout / mpdisconnected` (script-key STRING ids) | — | — | `FUN_0053c060(key, 9)` (popup launcher) | popup screen | `47c274/47c280/47c28a, _data.txt:86575-86589` |

The literal `"Loading Game \"%s\"... Please Wait"` and `"Saving Game \"%s\"...
Please Wait"` are **fallback strings** used only when the localization
dictionary key (`loadgamefmt` / `savegamefmt`) is missing. They are emitted by
slot 4 only via `TTextBar::PrintLine`; there is no separate full-screen "wait"
overlay panel.

---

## §9 — Animation & dynamic behavior

The only animated value in slot 4 is the autosave countdown timer.

```
ramp AutosaveTimer (DAT_005d7aac):
  range   0..AutosaveIntervalTicks (= DAT_005d7aa8 = 0x3840 = 14400 ticks
                                   = 10 min @ 24Hz)
  step    -1 per slot-4 tick (= 1 per screen tick, since slot 4 runs once per tick)
  target  0
  resets-to AutosaveIntervalTicks on reaching 0 (47be5a-47be5f)
  drive   DrawBackground call frequency (per screen tick)
  on-end  Emit 3 TTextBar lines + set DeferredSaveAnnounce flag (mbr_0x5e8=1)
  gates   only ticks when DAT_006682bc != 0 && DAT_005d7aa4 != 0
  cite    47be28-47bf01; initial 0x3840 in `_data.txt: DAT_005d7aa8 40 38 00 00`
```

```
AutosaveTimer  (10 min @ 24Hz)
14400 ─●────╮
       │     \_____
       │           \________________
       │                            \______
       │                                   \_______
       │                                            \____  fires:
    0  ┤                                                  ●  - PrintLine x3
       │                                                       - set mbr_0x5e8
       │                                                       - resets to 14400
       └───────────────────────────────────────────────────  ticks
       0                              ...                  14400
```

There are no per-frame painted animations driven by this slot.

**Dirty/redraw model:** slot 4 has no cache; it runs every tick regardless of
whether the screen content changed. Each deferred-action flag (`mbr_0x5e4,
0x5e8, 0x670, 0x674, 0x678, 0x67c, 0x6d4, 0x5e0`) follows a "set elsewhere,
consume here, clear" pattern.

---

## §10 — Input & dispatch

No direct input handling. The deferred-action flags this slot consumes are set
by:

| flag | set by | cite |
|---|---|---|
| `mbr_0x5e4` (DeferredLoad) | `FUN_0048e5b0/0048df70` callers + F-key in TimerTick `0x47c630` (key handler — `recon/discovered/renames/agent_screens.txt:186-191`) | (call-site) |
| `mbr_0x5e8` (DeferredSave) | this slot (autosave path) + user save F-key | `47bef5` + TimerTick |
| `mbr_0x670/0x67c/0x674/0x678/0x6d4/0x5e0` | TPlayScreen state-change accessors (out of scope) | — |
| `DAT_006682bc` (GameActive) / `DAT_005d7aa4` (AutosaveEnabled) | game lifecycle / settings | — |
| disconnect-reason for popup | `meth_0x577be0(0x676738)` on the NetSession `0x676738` | `47c266` |

---

## §11 — Retail bugs NOT to reproduce

1. **The 0x103-byte `memcpy` over an 11-byte literal source for "Autosave ["**
   (47be88-47be93). The compiler emitted `memcpy(dst, "Autosave [", 0x103)`
   which reads 0x103 bytes from the source and may walk into adjacent read-only
   `.rdata` (it does, in fact, here — the very next bytes are `\0\0]\0\0\0`
   etc.). It's safe because:
   - `.rdata` is mapped, so the read can't fault, and
   - the code immediately re-terminates the buffer at offset `0x103` via
     `mov byte ptr [esi+0x103], 0` (47be9f) and **then** runs `strlen` to find
     the real end of the copied content for the next concat.

   This is *technically defined behavior* (reads stay within the section) but
   it's a code-smell that a port must NOT reproduce. The port should use
   `snprintf(buf, sizeof(buf), "Autosave [%s]", DAT_00676774)` instead. There is
   no visible runtime symptom of the over-read in retail.

2. **The 10-min default autosave interval is hardcoded at compile time
   (`0x3840` initializer at `0x5d7aa8`) and only overridden by the
   `[Autosave]/AutosaveTime` INI key** (via the save-config path
   `FUN_00484500:00484a52` / `FUN_00484ed0:00485270` — `_data.txt:88697,
   88975`). A port should keep this in the existing INI/Revenant.ini config
   path; do not bake the interval into the engine.

3. **No on-screen wait dialog during autosave.** The retail behavior is that
   only message-log text is emitted — the player is not visually blocked from
   doing anything during the save. A modern port may choose to overlay a
   "Saving..." badge but that is a *deviation*, not a retail bug.

4. **No "Loading..." splash either.** Same as (3) — the load-game path also
   emits only a TTextBar line. The player sees the world fade away and the
   pane teardown in step (13); the message log says "Loading Game...". This is
   a deliberate retail design choice (continuous play, no loading screens), not
   a missing feature. A port that adds a loading splash is making a
   product-design deviation.

---

## §12 — Reconstruction pseudocode

```cpp
// TPlayScreen::DrawBackground — vtable slot 4 — controller, not paint.
// Order of operations is load-bearing: many of the dispatch flags read here
// are SET by other methods on this class (TimerTick / MouseClick / etc.)
// during the previous frame.

void TPlayScreen::DrawBackground() {
  GlobalFrameTimer_Tick(g_FrameTimer);   // 47bd20-47bd2c

  // (47bd31) "screen already closed" early-out
  if (this->mbr_screenClosed != 0) return;

  // ───────────────── EDITOR WAKE PATH ─────────────────  (47bd41-47bdeb)
  if (g_EditorActive != 0 && this->mbr_pendingState == 3 && g_RunningInEditor == 0) {
    int mode = WindowHost_Mode(g_WindowHost);
    if (mode != 0 && mode == 0xb && g_EditorActive != 0 && g_HostIsAwake == 0) {
      this->EditorWake(0);
      if (g_EditorHost != null) {
        EditorHost_DispatchCmd(g_EditorHostUI, /*cmd*/ 5);
        EditorHost_Notify(g_EditorHost, 0);
      }
      EditorContext_Init(g_EditorCtx);
      this->EditorRoute(&g_EditorCtx, -1);
      this->EditorPostCmd(7, &g_EditorCtx);
    }
  }

  // ───────────────── CINEMATIC ADVANCE ────────────────  (47bdf6-47be27)
  if (g_CineActive != 0) {
    if (this->mbr_cineEnable != 0 && !this->mbr_cineDone()) {
      CineController_Step(g_CineCtrl);
      this->mbr_cineEnable = 0;
      if (g_CineTarget != null)
        ((TObject*)g_CineTarget)->vtable[0x28/4]();   // notify-end-of-cine
    }
  }

  // ───────────────── AUTOSAVE FIRE ─────────────────  (47be28-47bf01) — §6.1
  if (g_GameActive != 0 && g_AutosaveEnabled != 0) {
    if (--g_AutosaveTimer == 0) {
      g_AutosaveTimer = g_AutosaveIntervalTicks;
      ChatChannel_Add(g_ChatSink, -1, "The system is autosaving!  You may experience temporary problems!");
      ChatChannel_Add(g_ChatSink, -1, "Please wait...");
      char buf[260];
      // PORT: replace the 3-stage memcpy with a single snprintf (see §11 bug 1)
      snprintf(buf, sizeof(buf), "Autosave [%s]", g_AutosaveSlotIdBuf);
      ChatChannel_Add(g_ChatSink, -1, buf);
      this->mbr_deferredSave = 1;                     // -> §6.3 picks this up THIS tick
    }
  }

  // ───────────────── DEFERRED LOAD/NEWGAME ANNOUNCE ─────────────────  (47bf01-47bfdf) — §6.2
  if (this->mbr_deferredLoad != 0) {
    const char* name = PickDisplayName(this, /*key=*/"newgame", /*fallback=*/"New Game");
    if (g_SinglePlayerMode == 0 || g_AnnounceLoadHeader != 0) {
      const char* fmt = LocalizedFormat("loadgamefmt", "Loading Game \"%s\"... Please Wait");
      TTextBar_PrintLine(g_TTextBar, fmt, name);
    }
    if (this->mbr_saveNameBuf[0] == 0)
      LoadController_LoadByIndex(g_LoadCtrl, this->mbr_saveIdx);
    else
      LoadController_LoadByName(g_LoadCtrl, this->mbr_saveNameBuf);
    this->PreLoadCleanup(1);
    this->mbr_deferredLoad = 0;
  }

  // ───────────────── DEFERRED SAVE ANNOUNCE ─────────────────  (47bfe5-47c0be) — §6.3
  if (this->mbr_deferredSave != 0) {
    ScratchSurface_Reset(g_LogHost);  // 47bff0
    const char* name = PickDisplayName(this, /*key=*/"curgame", /*fallback=*/"Current Game");
    if (g_SinglePlayerMode == 0 || g_AnnounceLoadHeader != 0) {
      const char* fmt = LocalizedFormat("savegamefmt", "Saving Game \"%s\"... Please Wait");
      TTextBar_PrintLine(g_TTextBar, fmt, name);
    }
    if (this->mbr_saveNameBuf[0] == 0)
      SaveController_SaveByIndex(g_LoadCtrl, this->mbr_saveIdx);
    else
      SaveController_SaveByName(g_LoadCtrl, this->mbr_saveNameBuf);
    this->mbr_deferredSave = 0;
  }

  // ───────────────── PER-FRAME MODE CHANGE BITS ─────────────────  (47c0be-47c161)
  if (this->mbr_cursorReset != 0) {
    ScratchSurface_Reset(g_LogHost);
    LogHost_Pump(g_LogHost, 0);
    LogHost_Flush(g_LogHost);
    this->mbr_cursorReset = 0;
  }
  if (this->mbr_logRebuild != 0) {
    LogHost_Rebuild(g_LogHost);
    this->mbr_logRebuild = 0;
  }
  if (this->mbr_sectorLoad != 0) {
    SectorLoad(g_SectorLoadXMin, g_SectorLoadYMin, g_SectorLoadXMax, g_SectorLoadYMax);
    this->mbr_sectorLoad = 0;
  }
  if (this->mbr_spawnQueue != 0) {
    SpawnQueue_Process();
    this->mbr_spawnQueue = 0;
  }
  if (this->mbr_fadeOutComplete != 0) {
    this->FadeOutComplete();
    this->ApplyResetWorld();
  }
  if (this->mbr_scriptReload != 0) {
    this->ScriptReload();
    this->mbr_scriptReload = 0;
  }

  // ───────────────── END-AREA CLEANUP ─────────────────  (47c176-47c21d)
  if (g_EndAreaRequested != 0) {
    g_EndAreaRequested = 0;
    BottomBar_Stop(g_BottomBar);
    BottomBar_Free(g_BottomBar);
    LogHost_Pump(g_LogHost, 0);
    LogHost_Flush(g_LogHost);
    LogHost_Free(g_LogHost);
    Player_Stop(g_PlayerCtrl);
    Player_Free(g_PlayerCtrl);

    int blk = 0x40;       // [esp+0x20]
    int sz  = 0x2710;     // 10000   [esp+0x18, +0x1c]
    LogHost_SetParams(g_LogHost, /*sz*/sz, /*sz*/sz, /*flags*/blk, 0, 8);

    g_InventoryDisplayDirty = 0;
    if (g_InventoryDisplay != 0) InventoryDisplay_Free(g_LogHost);
    LogHost_Resize(g_LogHost, /*saved-state ptr*/ &savedState);
  }

  // ───────────────── SUPER-CALL (the real "paint background") ──── (47c21f)
  TScreen::DrawBackground(this);

  // ───────────────── MODAL CURSOR RESET ─────────────────  (47c224-47c241)
  int flags = (this->mbr_paneStackDepth > 0)
              ? this->mbr_paneFlags[this->mbr_paneStackDepth]
              : 0;
  if ((flags & 8) == 0) UICursor_Reset(g_UICursor);

  // ───────────────── MP DISCONNECT POPUP ─────────────────  (47c241-47c2ac) — §6.5
  if (g_NetConnected != 0 && this->mbr_onTransientScreen == 0) {
    int paneBits = (this->mbr_paneStackDepth > 0)
                  ? this->mbr_paneFlags[this->mbr_paneStackDepth]
                  : 0;
    if ((paneBits & 7) == 0) {
      int reason = NetSession_DisconnectReason(g_NetSession);
      if (reason != 0) {
        const char* key;
        switch (reason) {
          case 3: key = "mpdisconnectedserver";  break;
          case 2: key = "mpdisconnectedtimeout"; break;
          case 1: key = "mpdisconnected";        break;
          default: key = priorStr;               // UNCONFIRMED §14 item 3
        }
        ShowPopupScreen(key, /*kind*/ 9);
        g_NextScreenTrampoline = g_MainMenuTrampoline;
        this->KickTrampoline();
      }
    }
  }
}
```

For the reconstruction agent: **most of the above is OUT OF SCOPE for a UI
panel reconstruction.** The UI-relevant lines are the three log emissions
(autosave, load, save) and the disconnect popup. The other branches are
gameplay/screen-state plumbing that other tracks own (gameflow / save-load).
Wire the UI part by:

1. Adding three new TTextBar emit calls.
2. Routing `g_AutosaveSlotIdBuf` from the save-controller into the chat sink.
3. Calling the existing `ShowPopupScreen("mpdisconnected*", 9)` path.

Do NOT add a full-screen autosave/loading panel.

---

## §13 — Port mapping notes

| retail call | port primitive | home | notes |
|---|---|---|---|
| `ChatChannel_Add(0x676668, -1, str)` | (none yet) | needs new primitive | The chat/network text channel doesn't have a clean port equivalent. The natural place is `src/console.h` or wherever `TTextBar` already enqueues; a single function `MessageLog::AddLine(channel, str)` covers both autosave and chat. The fallback path (`meth_0x578ec0`) already routes to `TTextBar` — see §14 item 2 to confirm the exact entry point. |
| `TTextBar::PrintLine(0x65c5d0, fmt, …)` (FUN_0054d170) | `TTextBar::PrintLine(...)` | `src/textbar.h` (per TTextBar_SPEC §13) | already specced |
| `TextResource_FindKey / Lookup` (FUN_0049d6d0 / FUN_0049d800) | `Dictionary::Find / Lookup` | needs a localization service entry | `agent_world.txt:295` flags this as "TextResource::Lookup" — the canonical home is the localization layer (see [[project-localization]]) |
| `ShowPopupScreen(key, kind=9)` (FUN_0053c060) | (existing PopupScreen path) | `src/screen.h` / popup system (`PopupDef_SPEC.md`) | wire to the existing popup-screen launcher |
| `TScreen::DrawBackground` (FUN_0048fda0) | inherited TScreen behavior | base class | nothing for slot 4 to add |

---

## §14 — UNCONFIRMED / open questions

1. **`DAT_00676774` — the autosave slot-name buffer.** Slot 4 only READS this
   global (in the "Autosave [<slot>]" concat). The single XREF in `_data.txt`
   shows only this read. The *writer* is presumably in the autosave config /
   save-controller (`FUN_00484500/FUN_00484ed0`, which are the
   `Autosave`/`AutosaveTime` INI handlers per `_data.txt:88692, 88970`) or in
   the save-controller `FUN_0048df40/df70`. **Impact if wrong:** the autosave
   message will print "Autosave []" with empty brackets, or garbage. **Resolve:**
   `DecompileAddr.java 0x00484500` and `0x00484ed0` to find what writes the
   slot name into `0x676774`, OR grep the binary for writes via `objdump |
   grep "676774"` after a full disassembly.

2. **`meth_0x578ec0(0x676738, 1)` — the local-fallback "show this chat line in
   the on-screen log" entry point.** The chat-add helper `meth_0x5701f0`
   falls through to this when not multiplayer (`cls_0x5701f0.cpp:66`). I
   inferred this routes to `TTextBar`, but the exact wiring (does it push to
   the same `TTextBar` instance at `0x65c5d0`, or to a sibling log host on
   `0x676738`?) is unread. The XREF for `0x676738` (the NetSession instance
   per the disconnect path in §6.5) is mixed-purpose. **Impact if wrong:** the
   3 autosave lines might not actually surface in the existing TTextBar — they
   might go to a different log surface that the port hasn't built. **Resolve:**
   `DecompileAddr.java 0x00578ec0` to read the body; cross-check that
   `0x676738` exposes a `TTextBar`-shaped vtable slot at the destination it
   reaches.

3. **`switch(reason) default: key = [esp+0x8]`** at 47c291. The default branch
   loads `eax = [esp+0x8]` — that's an earlier stack slot, holding *some*
   string set up in the path above (`47c266: ecx = 0x676738; call
   0x577be0`). My read says the default falls back to a recent string the
   prior code sequence left at `[esp+0x8]`, but I haven't pinned which prior
   line writes it (the `push 0x9` and `push eax` at 47c295 push the kind and
   the key for `FUN_0053c060`; the `[esp+0x8]` slot is below those pushes so
   it's a pre-existing variable). **Impact if wrong:** a 4th unknown
   disconnect reason emits a wrong popup key (likely empty or stale). **Resolve:**
   re-trace the prologue with `objdump --start-address=0x47bd20` and watch which
   `[ebp+…]` value the compiler aliased to `[esp+0x8]` after the spill, or extract
   the function with `DecompileAddr.java 0x0047bd20`.

4. **`0x65c610` ("PrintGameHeader" gate) and `0x668154` ("GameType")
   semantics.** I conservatively named them based on usage (`!= 0` skips the
   PrintLine header). The exact meaning (multiplayer vs SP toggle? "in-game
   load" vs "first-load" gate?) is inferred. **Impact:** the load/save header
   line may print when it shouldn't (annoying chat noise) or vice versa.
   **Resolve:** grep all writers of `0x668154` — it has 98 XREFs, so the easiest
   path is `DecompileAddr.java` on a couple of writers in the save-controller.

5. **`FUN_0053c060` exact kind-9 semantics.** I asserted "kind 9 = modal
   popup". This is inferred from the `"cursor"` lookup at the start of the
   function (a popup loads a custom cursor) and from the structural similarity
   to `FUN_0053c100 / FUN_0053c1d0` which reference `"popup"`
   (`_data.txt:115108-115110`). Body fully unread. **Impact:** the disconnect
   path might route to a different screen kind. **Resolve:**
   `DecompileAddr.java 0x0053c060` and identify the kind enum from
   `PopupDef_SPEC.md` if covered, else extract the screen-kind enum.

6. **Slot-4 method body itself is not extracted by Ghidra.** Everything in
   this spec was decoded from raw x86 via `objdump`. A `DecompileAddr.java
   0x0047bd20` run would replace inference with cited C-like syntax and pin
   field offsets like `mbr_0x5e4/0x5e8/0x670/...` to a real struct layout.
   **Impact:** the per-tick flag-field offsets above are correct but
   field-name aliases are inferred. **Resolve:** dispatch a Ghidra extraction.

---

## Summary (≈150 words)

`TPlayScreen::DrawBackground` (slot 4 @ `0x47bd20`) is misnomed — its body is
a per-tick controller, not a paint method. The five "overlay" strings the brief
flagged (`"system is autosaving"`, `"Please wait..."`, `"Autosave ["`,
`"newgame"`, `"Loading Game \"%s\"... Please Wait"`) are NOT a full-screen text
overlay. They are TTextBar message-log lines: autosave fires three of them when
the 10-min (14400-tick) timer at `DAT_005d7aac` expires (block at
`47be28-47bf01`); deferred newgame/loadgame announcement
(`47bf01-47bfdf`) prints `loadgamefmt`+name via `TTextBar::PrintLine`
(`FUN_0054d170` on `0x65c5d0`); savegame mirror at `47bfe5-47c0be`. The
disconnect tail (`47c241-47c2ac`) launches a popup screen for
`mpdisconnected*` keys. No separate `TLoadingScreen` exists. The actual
backdrop paint is the super-call to `TScreen::DrawBackground` at the end
(`47c21f`). Reconstruction wires three TTextBar emits + the popup launch;
**there is no overlay panel to build.**

### UNCONFIRMED list (resolve before reconstruct)

1. `DAT_00676774` writer (autosave slot-name buffer) — see §14 item 1.
2. `meth_0x578ec0` exact TTextBar wiring (local-fallback path) — see §14 item 2.
3. Default-case popup key at 47c291 (`[esp+0x8]` source) — see §14 item 3.
4. `0x65c610` / `0x668154` gate semantics — see §14 item 4.
5. `FUN_0053c060` popup kind=9 semantics — see §14 item 5.
6. Ghidra extraction of slot-4 body itself — see §14 item 6.
