# Retail Options pane: controller list, key cells, names, rebinding, list input

Forensics for the port's Options controller list (`options.def` LISTBOX
"controller", the `name0..name7` TEXT widgets, and the key rebind). Ground truth
is the retail memory image run in the emulator
(`recon/retail_asm/baseline/Revenant.rebuilt.exe` through
`tools/retail_runtime/slots/hud/options.py`), plus capstone disassembly of the
same image. Every claim cites an instruction address. The Ghidra decomp
`recon/discovered/cls_0x5b9744_TOptionsScreen_OnControl_53aa90.cpp` and
`recon/ghidra/cls_0x5b9744.cpp` were used only to find structure, and each point
taken from them was re-checked against the disassembly. Anything not confirmed is
marked **UNCONFIRMED**. The draw rules in [DEF_ENGINE.md](../../ui/forensics/DEF_ENGINE.md) are taken
as given and not repeated here.

The scenarios were run as scratch scripts over `OptionsFixture` (MouseClick
slot 25, MouseMove 26, KeyPress 27, CharPress 28, Joystick 29, the field
getter slot 41); they aren't kept. The A/B cases in
`tools/retail_ab/hud_ab.py options` (`list-*`, `cell-*`, `rebind-*`) replay
the ones the port reproduces through the same fixture, and compare the
rebind buffer and capture state as well as the screen.

---

## 0. The pieces

The Options pane vtable is at `0x005b9744`. The instance at `0x0066fcc0` takes
these slots:

| slot | offset | function | role |
|---|---|---|---|
| 11 | +0x2c | 0x444fc0 | SetDirty(bool). Observed: the pane's +0x50 becomes 1 after a key |
| 25 | +0x64 | **0x53ba50** | MouseClick: `+0x1b0 = 0`, then the base click 0x436530 (0x53ba5f-0x53ba69) |
| 26 | +0x68 | 0x436660 | MouseMove (base) |
| 27 | +0x6c | **0x53b9e0** | KeyPress |
| 28 | +0x70 | 0x436460 | CharPress (base; the list's CharPress is `ret 8`, 0x438890) |
| 29 | +0x74 | **0x53ba30** | Joystick: always `0x53ba80(key, down)` (0x53ba3a) |
| 37 | +0x94 | **0x53aa90** | OnControl (widget, code) |
| 41 | +0xa4 | **0x53b5d0** | field getter |

TScreen sends a key to the top modal pane's +0x6c (0x004906bd), a char to +0x70
(0x004907bd) and a joystick event to +0x74 (0x004908bd). The play screen's
KeyPress 0x47c630 calls TScreen::KeyPress first (0x47c683). Nothing filters keys
before the pane: ESC, ENTER and the rest all arrive.

Pane members:

| off | meaning | set by |
|---|---|---|
| +0x1ac | rebind buffer: `NumControls × 0x24` = 3 codes × 3 key dwords per control | Open 0x53a8b0 |
| +0x1b0 | **capturing** flag | 0 at Open; 0 on every MouseClick (0x53ba5f); 1 on list event 0x138a (0x53b5b0); 1 or 0 in 0x53ba80 |
| +0x1b4 / +0x1b8 / +0x1bc | **chord** cap[0..2], −1 = empty | −1 at Open; 0x53ba80 |
| +0x1c0 | **rebind item** (list item index), −1 = none | −1 at Open; event 0x138a (0x53b5b6); −1 after commit (0x53bd01) |
| +0x1c4 | list widget pointer (event 1, 0x53aad4). The paths below don't use it: they look the list up by name with 0x436980 | |

At Open the emulator shows `capturing 0, cap [−1,−1,−1], rebind −1, list top 0,
sel −1` (t4).

Fields of the list widget (vtable `0x5a3d44`) used here: count +0x98, top item
+0x9c, selected +0xa0, visible +0xa4, rows +0xa8, cols +0xac, itemw/h +0xb0/+0xb4,
content x/y/w/h +0xb8..+0xc4, child scrollbar +0xd4. The list's vtable slots: 11
Activate `ret 4` (0x438870), **12 KeyPress `ret 8` (0x438880)**, 13 CharPress
`ret 8`, 14 MouseClick 0x431500, 15 MouseMove 0x4316b0 (forwards only to the child
scrollbar), 21 hit-rect 0x42a7a0, 22 hotkey 0x42a7f0, 23 child event 0x4316e0.

**List events.** They are sent through `0x42a820`. Because the list has no
parent (+0x78), they go to the pane's OnControl (`pane->vt[+0x94]`, 0x42a8a5-0x42a8ac):

| code | sent by | meaning | Options |
|---|---|---|---|
| 5000 `0x1388` | SetSelection 0x430b80, at 0x430c32, when the selection changed | selection changed | ignored (falls through to `return 1`, 0x53b5a2-0x53b5bc) |
| 5001 `0x1389` | SetTop 0x430ab0, at 0x430b67, when the top changed | scrolled | refreshes `name0..7` (§2) |
| 5002 `0x138a` | list MouseClick, at 0x431683 (mouse-up over an item) | item clicked | **starts the capture**: `+0x1b0 = 1`, `+0x1c0 = list.selected` (0x53b5aa-0x53b5b6) |
| 5003 `0x138b` | 0x431615, double-click; only with list flag 0x100000 | – | never sent: the options list's control word is 0x91000 (live) |

OPTIONS.md §4 calls 0x1389/0x138a "the rebind path". More exactly, 0x1389 is the
name refresh and 0x138a arms the capture. The keys themselves are captured by
KeyPress or Joystick (§3).

The child scrollbar's 4000 does **not** reach the pane. Its parent (+0x78) is the
list, so 0x42a820 calls `list->vt[+0x5c]` = 0x4316e0 (0x42a885-0x42a890). That
handler sets the top item to `value * cols` (VLIST; 0x4316f6-0x431707) and calls
SetTop when it differs (0x43171f).

---

## 1. Q1: the "key" field getter `0x0053b5d0`

The signature is `getter(char* field, char* buf, int size, COLOR* colour, int index)`,
`ret 0x14`. The list's FormatDraw calls it once per visible cell, with
`f.string = "key"` (DEF_ENGINE.md §5).

```
if (_stricmp("Key", field) != 0)              // 0x53b5e2-0x53b5f1; 0x59a530 is _stricmp
    return base getter 0x436ea0(...)          // 0x53b97f-0x53b9a2
ctrl = index / 2 ; code = index % 2           // 0x53b5ff-0x53b61a (signed div/mod)
if (index == this[+0x1c0] && this[+0x1b0])    // 0x53b614-0x53b62c: the cell being rebound, while capturing
    keys = &this[+0x1b4]                       // the chord so far
    if (keys[0] == 0 || keys[0] == -1) { sprintf(buf, "-"); return 1 }   // 0x53b632-0x53b649, "-" @0x5e4620 (0x53b7b0)
else
    keys = buffer + ctrl*0x24 + code*12        // 0x53b7ba-0x53b7cb
    if (keys[0] == 0 || keys[0] == -1) { sprintf(buf, "-"); return 1 }   // 0x53b7ce-0x53b7d9, "-" @0x5e4634 (0x53b958)
buf[0] = 0
for k in 0..2:                                 // 0x53b66b / 0x53b7fd, 3 iterations
    if (keys[k] == 0 || keys[k] == -1) continue
    name = GetKeyName(keys[k])                 // 0x4396b0, this 0x65a9a8
    look = "KEY" + name                        // 0x53b69f-0x53b6dd
    if (!name) { tmp[0] = (char)keys[k]; if (!isalnum(tmp[0])) look = "-" }   // 0x53b6ee-0x53b71a
    DisplayName(tmp, 100, look)                // 0x46e7f0 (0x53b72b)
    if (tmp starts with "KEY") tmp = name      // 0x53b730-0x53b759
    if (!buf[0]) sprintf(buf, "%s", tmp)       // 0x53b768
    else         sprintf(buf, "%s-%s", buf, tmp)   // 0x53b77e
return 1
```

- **Item → control and code.** Item i is control `i/2` and code `i%2`: column 0
  ("Key 1") is code 0 and column 1 ("Key 2") is code 1. The third code (code 2)
  is never shown, but the rebind conflict pass does clear it (§3.4).
- **GetKeyName 0x4396b0.**
  - A–Z and 0–9 return a one-character static string (0x4396b4-0x4396da).
  - Any other code is looked up in the `{code, name}` table at
    0x5cdc98..0x5ce188 (158 entries; dump in `keytab.py`). An unknown code gives
    NULL (0x4396f3).
  - Joystick codes are 0x400..0x43f (JOYUPLF…DJOY24).
- **DisplayName 0x46e7f0(dst, size, key).**
  - It keeps only the key's alphanumeric characters (0x46e824-0x46e84d) and looks
    the result up in the string table (`0x49d800`, `this` 0x65d4d0).
  - On a hit it copies the string. A miss returns `"[key]"`, so the raw key is
    copied instead (0x46e863-0x46e876).
  - The table is the shipped `english.def`. Its "Control keys" section
    (lines 2085–2240) maps `KEY<name>` to display strings. All of them are
    identity except **`KEYCTRL "Cont"`** (line 2097).
  - As a result, the CTRL chords read **`Cont-A`**, `Cont-LEFT` and so on. This
    was observed in the emulator: `t2` shows every cell, and `t3` the lookups.
- **The "starts with KEY" check.** "KEYA" has no entry, so DisplayName returns
  "KEYA", and the check falls back to the raw name "A". It also covers 0–9.
- **Joining.** The keys of a code are joined in **stored order** with `-`.
  Observed results: `SHIFT-Cont-ALT` (3 keys), `JOY7-JOY4` (2 keys), `Cont-Z`.
- **Empty values.** An unbound code shows `-`. The cell being rebound shows `-`
  until the first key arrives (the same string, 0x5e4620).
- **Unnamed codes.** A code with no name and a non-alphanumeric low byte (any
  unnamed VK below 0x100, e.g. 0xa2 VK_LCONTROL or 0x15) gets the lookup key
  "-" → DisplayName "" → miss → the raw "-". It shows **`-`**, the same as
  unbound, even though the code is stored (t7 F: `0xa2 → '-'`, buffer `[162,0,0]`).
- **Colour.** The getter **never writes the colour**: nothing in
  0x53b5d0-0x53b9af touches the 4th argument. Every getter call in the emulator
  returned the colour untouched (`0x123456` in → `0x123456` out), including the
  capture cell. Every key cell therefore draws in the FIELD colour 0xFFFF64, and the
  cell being rebound is marked only by the selection Box (§3.5).

Observed default cells with the emulator's INI: `F1 | JOY7-JOY4`,
`Cont-A | JOY8-JOY1`, `- | -`, `SPACE | -`, `Cont-LEFT | DJOYLF`, `Cont-BS | -`
(Quick Save), `LEFT | JOYLF`.

## 2. Q2: `name0..name7`

They are set in two places, with the same code:
- **event 1** (0x53ae1a-0x53aee2), after SetCount (0x53aae8), so top = 0;
- **event 0x1389** (0x53b4af-0x53b58c), that is, after every change of the top
  item. 0x1389 is sent only by SetTop 0x430ab0 (0x430b67), which is called from the
  scrollbar's 4000 (0x431720) and from SetSelection's auto-scroll (0x430bfa).

The 0x1389 handler acts only when the widget's name `_stricmp`s to "Controller"
(0x53b4bf-0x53b4ce). It does for the DEF's "controller". Then:

```
row = list.top / 2                         // 0x53b4f7-0x53b506 (a hard /2, not /cols)
for k in 0..7:                             // 0x53b50c / 0x53b589
    GetControl(row + k, &entry)            // 0x439060, this 0x65a9a8 (0x53b51c)
    w = widget named "name" + ('0'+k)      // "name0" with the digit patched (0x53b521-0x53b538)
    if (w && entry.name) {                 // 0x53b53f-0x53b549
        DisplayName(text, 100, entry.name) // 0x53b56f: "Invoke 1" → key "Invoke1" → english.def "Invoke 1"
        w->SetText(text)                   // vt+0x18 (0x53b581)
    }
```

- **Name k** is the control `top/2 + k`, i.e. the control on visible row k.
- The names are localized through english.def's "Control Names" section, keyed by
  the name with spaces removed. A control with no entry (e.g. "Show Spells",
  "Quick Save") falls back to the raw table name.
- **Fewer than 8 rows.** This can't happen with the shipped table. The scroll max
  is `(138−16+1)/2 = 61` rows (SetCount), and `61+7 = 68` is the last control, so
  every one of the 8 names always has a control. In general:
  - GetControl with an out-of-range index returns 0 **without writing** `entry`
    (0x43906a-0x439077);
  - so `entry.name` would keep the previous row's pointer (or stack garbage for
    k=0), and a name would repeat. **UNCONFIRMED** (unreachable).
- **Timing.** Observed in the emulator (t8) with the screen's own pass order:
  - a scroll redraws the list items **immediately**, inside the input call (SetTop →
    DrawItems(surface, 0), 0x430b5c);
  - the 8 name widgets redraw on the **next pane draw** (a ParamBlit restore plus a
    Text each);
  - both happen before the next present. Visible result after one down-arrow click:
    `Invoke 2 … Combat Combo 5`.
- The DEF text ("Up", "Down", …) is never drawn: event 1 replaces it inside Open,
  before the first draw.

## 3. Q3: selection and rebinding

### 3.1 The click (pane → list)

1. **Mouse-down on a cell.**
   - The Options MouseClick clears `+0x1b0` (0x53ba5f).
   - The base click 0x436530 hit-tests the widgets in DEF order (0x4365ea-0x436614).
     The list rect (60,220,512,170) comes before `name0..7`.
   - For button 1 it calls 0x4369f0 (focus): the "frontend_move" sound (0x436a2c),
     then `SetFlags(|8)`. The list drops bit 8 because its control word lacks 0x10
     (0x4306c5-0x4306d5), so it never becomes the focus widget, and the sound plays
     on every press.
   - Then the list's MouseClick 0x431500 runs: the child scrollbar takes the click
     first if it contains the point (0x43150e-0x431532). Otherwise, for button 1 and
     not VIEWONLY, it hit-tests (0x431556) and calls `SetSelection(i)` when i ≥ 0
     (0x431566).
   - SetSelection 0x430b80 sets `sel`, redraws the old and new cells **immediately**
     with `DrawItem(surf, i, 0)` (0x430c0b, 0x430c22), and sends 5000 if the
     selection changed (Options ignores it).
2. **Mouse-up.**
   - `+0x1b0 = 0` again.
   - The list (button 4, 0x431572) plays "click1" for any release inside the list
     rect, even one that misses every item (0x431632-0x431669).
   - It hit-tests again. On a hit, `SetSelection(i)` (no event if unchanged) and
     **event 5002** (0x431683), which sets `+0x1b0 = 1` and `+0x1c0 = list.sel`.
3. A release **outside the list rect** never reaches the list. The list takes no
   mouse capture, and the base click routes a release by hit test. The selection
   stays, but no capture is armed (t7 D).
4. **Down on one cell, up on another** arms the capture on the release cell
   (t7 E: down on cell 0, up on cell 3 → sel 3, rebind item 3).

Observed (t4):
- after the down: `sel 0, capturing 0`, cell "F1", Box (245,225,150,20) 0x323;
- after the up: `capturing 1, rebind 0`. The getter now returns "-".

### 3.2 Routing the keys

```
KeyPress 0x53b9e0(key, down):
    list = FindWidget("controller")            // 0x436980 (FATAL if missing)
    if (list.selected >= 0) Rebind(key, down)   // 0x53b9ed-0x53ba03
    else base KeyPress 0x4361f0(key, down)      // 0x53ba18
Joystick 0x53ba30(key, down): Rebind(key, down) // unconditional
CharPress: base 0x436460 → the list's CharPress `ret 8`. No effect (t7 I).
```

While the list has **any** selection, every key goes to Rebind, and the base
handler (hotkeys, 0x4362e4-0x436333) is skipped. When `+0x1c0` is −1, Rebind
discards the key (§3.3), so the key does nothing at all. The list itself has no
key handling: its KeyPress slot is `ret 8`.

### 3.3 Rebind `0x0053ba80(key, down)`

```
list = FindWidget("controller") ; changed = 0
if (!down) {                                   // key up, 0x53bc95
    if (cap[0] != key) goto tail               // only the release of the chord's FIRST key matters
    capturing = 0
    list->SetSelection(-1)                     // 0x53bcb4: Box gone, old cell redrawn now (pre-commit text)
    goto conflicts                             // 0x53bcb9 → 0x53bb22
}
if (rebind == -1) goto tail                    // 0x53baa7-0x53bab2: no target, so the key is swallowed
if (!capturing) {                              // 0x53bc64: target set but not capturing (e.g. after a click elsewhere)
    cap = [key, -1, -1] ; capturing = 1 ; SetDirty(1) ; goto tail
}
// capturing: add the key to the chord
for (i = 0; cap[i] != -1 && i < 3; i++)        // 0x53bac4-0x53baf5
    if (cap[i] == key) return                  // 0x53bae4: auto-repeat/duplicate, no redraw
if (i < 3) cap[i] = key                        // 0x53bb0a
else { capturing = 0 ; cap[2] = key ; changed = 1 }   // 0x53baf7-0x53bb03: a 4th key REPLACES the 3rd and ends the capture
SetDirty(1)                                    // 0x53bb11-0x53bb17
if (!changed) goto tail
conflicts:                                     // 0x53bb22-0x53bc00, see §3.4
tail:                                          // 0x53bc06
if (capturing) {                               // 0x53bd0d
    if (rebind != -1 && (w = pane.widget[rebind])) w->SetFlags(w.flags | 0x20)   // 0x53bd18-0x53bd3d
    SetDirty(1) ; return                       // 0x53bd40
}
if (rebind == -1) return                       // 0x53bc14-0x53bc1f
for k in 0..2: buffer[rebind/2][rebind%2][k] = (cap[k] != -1) ? cap[k] : 0   // COMMIT 0x53bc25-0x53bce6
cap = [-1,-1,-1] ; rebind = -1                 // 0x53bcec-0x53bd01
SetDirty(1)                                    // 0x53bd40-0x53bd46
```

- **What a chord is.** A chord is **up to 3 keys in press order**, held together.
  It is committed when **the first key of the chord is released**, or at once on
  a 4th distinct key down. Releasing the other keys does nothing. Modifiers are
  not special: CTRL (0x11), SHIFT (0x10) and ALT (0x12) are ordinary codes. The
  generic VK codes come through (the map holds 17/18, not 0xa2/0xa4). Order
  matters: CTRL then A stores `[17,65]`; A then CTRL would store `[65,17]`.
- **Refused or special keys.** There are none: no ESC cancel, no ENTER confirm,
  no DEL clear. Observed (t7 F): ESC → `ESCAPE` `[27,0,0]`, ENTER → `RETURN`,
  DEL → `DEL`, BS → `BS`, LWIN → `LWIN`, PAUSE → `PAUSE`. A code can't be cleared
  directly; it is only cleared by the conflict pass when the same chord is bound
  elsewhere. Mouse buttons can't be bound: clicks never reach KeyPress, and a click
  resets the capture.
- **Joystick.** Joystick buttons are captured the same way, through slot 29
  (t7 G: JOY3 → `[1034,0,0]` on item 1). With no target, a joystick event does
  nothing.
- **Auto-repeat.** A repeated key-down of a key already in the chord returns
  without a redraw (0x53bae4).
- **Bug: `pane.widget[rebind]`.** The tail marks *pane widget number `rebind`*
  dirty: 0x436900 indexes the pane's widget array with a **list item index**. The
  SetDirty(1) that follows redraws everything anyway, so the bug isn't visible.

### 3.4 The conflict pass (binding a key that's already used)

This runs before the commit (0x53bb22-0x53bc00), over **every control and every
code 0..2**, the rebinding control's own other codes included. It reads the live
control map's modes (GetControl 0x439060) and the **buffer** keys:

```
for ctrl in 0..NumControls-1:
  for code in 0..2:
    if (GetControl(ctrl).mode & GetControl(rebind/2).mode)        // 0x53bb68-0x53bb87
      p = buffer[ctrl][code]
      if (all k: p[k] == cap[k] || cap[k] == -1)  p[0..2] = 0     // 0x53bba8-0x53bbcc
```

A captured `-1` slot acts as a **wildcard**. Binding a single key K therefore
clears every overlapping-mode code whose **first** key is K, whatever follows it.

Observed (t7 A): binding CTRL alone to Invoke 1 (mode 7) cleared:
- Combat Combo 1–3 (`CTRL-A/S/D`, mode 2);
- all eight Combat Dodges (`CTRL-arrows`, mode 2);
- Load Game, Save Game and Quick Save (`CTRL-…`, mode −1).

Binding `CTRL-A` to Invoke 1 cleared Combat Combo 1 (`[17,65]`) but **not** Combat
Swing (`[65]`) (t4). Codes in non-overlapping modes are kept (e.g. Use, mode 1,
against a mode-2 control). Only the buffer changes; OK writes the buffer back to
the map (OPTIONS.md §5).

### 3.5 What the screen shows, and when

Observed with the screen's own draw sequence (`t8`: SetClipRect, slot 20, then
SetDirty(0), as `TScreen 0x48ff00` does at 0x48ff5e-0x48ff74, with the screen not
dirty):

| moment | drawn | cell 0 shows |
|---|---|---|
| mouse-down | at once, inside the click: `Box(245,225,150,20, SELCOLOR→0x323)`, then `Text(…,'F1')`. The two ParamBlits in the trace are inside the Box call, its fill (t9 chains 0x4a6a87/0x4aa2c2 under 0x430ee5) | old binding in the green box |
| mouse-up | **nothing**: no redraw is requested (pane +0x50 stays 0, the next frame draws nothing) | **still the old binding** "F1" in the box. The capture's "-" isn't drawn yet |
| 1st key down (CTRL) | SetDirty(1) → **full pane redraw** next frame (background plus every widget) | `Cont` in the box |
| A down | full redraw | `Cont-A` in the box |
| A up | full redraw (tail, still capturing) | `Cont-A` |
| CTRL up (commit) | at once: SetSelection(−1) → DrawItem(0,0) = the cell's background restore plus `Text 'F1'` (the pre-commit buffer); then SetDirty → full redraw next frame | `Cont-A`, no box. The 'F1' is overdrawn before the next present |

- The selection Box is **the being-rebound indicator**. There is no "Press a key"
  text and no colour change. The empty capture text is "-".
- Because the mouse-up doesn't repaint, retail keeps showing the old binding until
  the first key. **UNCONFIRMED** whether something outside the pane repaints it
  sooner in the real game (§6).

### 3.6 When the selection and capture end, including the quirks

- **Normal end: the first chord key is released.** The commit, then
  `SetSelection(−1)`, so the Box disappears.
- **4th key.** The commit happens, but **SetSelection(−1) is not called**. The Box
  stays on the cell (t7 B: after `SHIFT CTRL ALT Z` the buffer holds `[16,17,90]`
  and the Box is still drawn). `rebind` is −1, but `list.selected` ≥ 0, so every
  later key goes to Rebind and is swallowed (t7 B: Q did nothing). This lasts until
  another cell is clicked.
- **Down on a cell, up outside the list.** The same stale state: Box, no target,
  keys swallowed (t7 D: Z did nothing).
- **A click elsewhere while capturing.** This covers a toggle, OK held, or a
  scrollbar arrow, since every pane MouseClick clears `+0x1b0`. The capture display
  ends: the cell reverts to the stored binding at the next redraw, the Box stays,
  and the target stays. The next key-down starts a **fresh** chord into the same
  item (0x53bc64), even if it has scrolled out of view (t7 C, H). The click itself
  acts normally; the toggle in C flipped.
- **Clicking another cell mid-chord.** The partial chord **carries over** to the
  new cell: 5002 does not reset `cap`. In t7 J, CTRL was held on cell 0, then cell 3
  was clicked, and cell 3 showed `Cont`. Pressing A gave `Cont-A` on cell 3, and
  releasing CTRL committed it to cell 3; cell 0 was untouched.
- **OK during a capture.** OK writes back the buffer only; an uncommitted chord is
  dropped. **Cancel** drops everything (OPTIONS.md §5–6).

## 4. Q4: list input in the DEF engine

### 4.1 The hit test `0x004310b0(x, y)` (pane-local)

```
if (x < cx || x >= cx + cw || y < cy || y >= cy + ch) return -1     // cw = cols*itemw, ch = rows*itemh (0x4310b6-0x4310ee)
VLIST: i = top + ((y-cy)/itemh)*cols + (x-cx)/itemw                 // 0x4310f9-0x431141
HLIST: i = top + ((x-cx)/itemw)*rows + (y-cy)/itemh                 // 0x43111c-0x431139
return i < count ? i : -1                                           // 0x431143-0x43114b
```

Options: cx 245, cy 225, cw 300, ch 160, itemw 150, itemh 20, cols 2. So (300,235)
→ item `top + 0` and (450,255) → item `top + 3`. The name column (x < 245) lies
inside the list rect but misses every item. A release there plays "click1" and
does nothing else.

### 4.2 The child scrollbar

Rect (551,224,16,162); arrows at (552,225) and (552,374); track 243..356;
thumb x 553. All of this is live (t5). Range: min 0, max 61 (rows), **page 16**.
The page is set to the list's *visible item count* (`child.page = list.visible`,
0x430a83-0x430a8a), while the value is in rows, so one page is 16 rows, two
screens.

MouseClick 0x42eed0:
- **Arrow down.** "click1" (0x42ef2e / 0x42effe). Flags `|0x40000` (up) or
  `|0x200000` (down) for "armed", plus the pressed bit 0x80000 / 0x100000, so the D
  bitmap is drawn. It redraws the arrow (0x42efb3 / 0x42f083) and **captures the
  mouse**: `+0x90 = 1`, `pane+0xa4 = sb` (0x42f10e-0x42f11c).
- **Arrow release.** The capture is released. Only **if the pointer is still on
  the arrow** (0x42e8a0 / 0x42ea50) does it call `SetValue(value ∓ 1)`
  (0x42f249 / 0x42f29c). Either way the arrow is unpressed (0x42e8f0 / 0x42eaa0).
- **While an arrow is held.** MouseMove 0x42f2e0 sets or clears the pressed image
  as the pointer leaves or re-enters the arrow (0x42f2f4-0x42f393, 0x42f39a-0x42f436).
- **Thumb.** Grabbed only when `max − min ≥ 1` (0x42f09b-0x42f0ac). It stores the
  grab offset (0x42f0d3-0x42f0ee), sets the D image (0x400000) and captures. The
  drag is
  `v = min + (clamp(y − grabY, trackStart, trackEnd) − trackStart)·(max − min)/(trackEnd − trackStart)`
  (0x42f448-0x42f4c8), then SetValue. The thumb **snaps** to
  `trackStart + (trackEnd−trackStart)·v/(max−min)` (SetValue 0x42e478-0x42e4a0).
  It makes no sound.
- **Track.** "click1" (0x42f128). VSCROLL: `y < thumb.y` → `SetValue(max(v − page, min))`,
  else `SetValue(min(v + page, max))` (0x42f16d-0x42f1c4). No capture.
- **SetValue 0x42e440.** It clamps the value, redraws the thumb (0x42e525) and
  sends 4000 when the value changed (0x42e534). The 4000 goes to the list, which
  sets the top to `value * 2` (§0). SetTop then draws the content immediately
  (0x430b5c) and sends 5001, which updates the names (§2).

Observed (t6):
- a down-arrow click: value 0→1, top 2, thumb 243→244;
- an arrow pressed, moved off and released off: no change;
- an up-arrow click: back to 0;
- three track clicks below the thumb: value 16/32/48 (top 32/64/96, thumb
  272/302/331, names "Belt Use 1…" after the first);
- the thumb grabbed at +5 and dragged up 50 px: value 17 (thumb 274); 200 px: 0.

### 4.3 Scrolling and the selection

A scroll does **not** change the list selection (t7 H: `sel 0` kept with top 10).
The Box is drawn only while the item is visible; DrawItems walks only the
visible range. A scroll does cancel the capture display, because the arrow click
goes through the pane MouseClick (`+0x1b0 = 0`). The target stays, and the next
key rebinds the off-screen item (t7 H: Z went to Invoke 1 key 1 at top 10).

SetSelection auto-scrolls a selection that isn't visible to
`SetTop(sel − visible/2)` (0x430bd4-0x430bfa). A mouse selection is always
visible, so this never triggers here.

### 4.4 Keyboard

- The list has **no keyboard navigation**: its KeyPress slot is `ret 8`
  (0x438880).
- The base pane's arrow/ENTER widget navigation (0x436246-0x4362e1) runs only
  with pane flag `+0x60 & 8`. Options opens with 0 (title) or 0x11 (in game), so
  it never runs.
- Base hotkeys (0x4362e4) run only while the list has no selection (§3.2).
  options.def defines no HOTKEY.
- There is no mouse wheel in this path: only buttons 1 and 4 are handled.
  **UNCONFIRMED** whether retail had wheel input at all.

## 5. Q5: what's visible beyond DEF_ENGINE.md

1. **Partial redraws.** Some draws happen inside the input calls; the full
   redraw comes only on SetDirty:
   - mouse-down draws the selected cell at once (Box plus text);
   - SetSelection redraws the old cell: its background restored from the list
     background, `DrawItem(…,0)` → the list's DrawBackground over the cell
     (0x430efd-0x430f06), then the text;
   - SetTop redraws the content area (restore plus all visible cells);
   - the scrollbar redraws its arrow and thumb in place;
   - each key event calls SetDirty(1), and the pane redraws in full on the next
     draw (t8: 50 ParamBlit / 22 Put / 40 Text / 176 PutSubrect / 1 Box).
2. **The capture shows only the Box.** The text changes only from the first key
   on. "-" appears only if a redraw happens before the first key.
3. **Pressed images.** The scrollbar arrows show their D bitmap while held (and
   lose it while the pointer is off the arrow). The thumb shows its D bitmap while
   dragged. The draw rules already pick D from the 0x80000 / 0x100000 / 0x400000
   bits (defengine §4); this section says when those bits are set.
4. **"Cont".** CTRL displays as `Cont` (english.def). The port must take key
   display names from the string table, not from the key-name table.
5. **Sounds.**
   - "click1": list mouse-up anywhere in the list rect (0x431632), scrollbar arrow
     press (0x42ef2e/0x42effe) and track click (0x42f128).
   - "frontend_move": every mouse-down on the list, through 0x4369f0 (0x436a2c).
     This is derived from the disassembly and not run; **UNCONFIRMED** that the
     sample exists.
6. **Stale states.** The Box stays after a 4-key commit or after down-on-cell /
   up-outside, and keys are swallowed until another cell is clicked (§3.6).
7. **The names update one draw after the items** (§2). This isn't visible at
   frame granularity.

## 6. UNCONFIRMED

- **Mouse-up repaint in the shipped game.** Whether any screen-level repaint
  (screen +0x50 dirty, fade pulses in the 0x10 in-game route, the composed-surface
  pass 0x436090) redraws the pane between the mouse-up and the first key. If one
  does, the "-" would show earlier. In the emulator, with the screen not dirty,
  nothing redraws (t8, title route; the in-game variant t10 behaves the same for
  state and text).
- **"frontend_move".** That the sample exists, so the mouse-down sound is audible.
- **The out-of-range name path.** GetControl out of range leaves `entry` stale
  (§2). Unreachable with 69 controls.
- **A latent crash.** A code with no name whose low byte is alphanumeric gives
  the lookup "KEY" → DisplayName "KEY" → the "starts with KEY" path →
  `strncpy(tmp, NULL, 0x63)` (0x53b749-0x53b751 with ebp = 0). It can't be reached
  from the keyboard: every unnamed VK below 0x100 has a non-alphanumeric low byte.
  Joystick codes above 0x43f would reach it. **UNCONFIRMED** whether any input
  produces them.
- **Mouse wheel.** Whether retail had wheel input for lists at all.

## 7. Port checklist (retail behaviour to reproduce)

1. Key cell text = the stored keys in order, each through
   `StringTable("KEY"+name)` with the raw-name fallback, joined by `-`; `-` when
   empty. The cell being rebound shows the chord so far (`-` when empty). The colour
   is always the FIELD colour.
2. `name0..7` are set from `control(top/2 + k)` through the string table, on open
   and on every top change.
3. **Click.** Down selects (Box). The up event (5002) arms the capture on the
   up's cell. Any pane click clears "capturing" but keeps the target and the
   partial chord.
4. **Keys while the list has a selection** all go to the rebind:
   - the first down starts the chord, and later downs append (up to 3);
   - a duplicate is ignored;
   - a 4th key replaces the 3rd and commits without clearing the selection;
   - the release of the first chord key commits and clears the selection.
5. **Conflict pass** before the commit: clear every code in any control whose
   mode overlaps the target's mode and whose keys match the chord, with unfilled
   chord slots as wildcards. This includes code 2 and the target control's own codes.
6. **Scrollbar.** An arrow steps one row on release over the arrow. The track
   pages by 16 rows (`page = visible items`). The thumb drag maps linearly and
   snaps. The scroll keeps the selection.
7. There is no keyboard navigation in the list. ESC, ENTER and DEL are bindable
   keys, not commands, while a cell is selected.
