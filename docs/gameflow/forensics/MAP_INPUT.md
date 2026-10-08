# Map input — forensics

How retail Revenant turns the mouse over the map into a cursor and an
action: which object is under the pointer, the cursor shown for it, and
what a click on it (or on the floor) does. Written for the doors and lift
gates, which the port couldn't use with the mouse (the resurrection-room
door after the opening). Companion to
[../../gameplay/forensics/PLAYER_INPUT.md](../../gameplay/forensics/PLAYER_INPUT.md)
(keys, the control map, `TPlayScreen::Command`, attacks, equipping from the
HUD — not repeated here), [EXITS.md](EXITS.md) (what `Use` on a door does)
and [SCREEN_SYSTEM.md](SCREEN_SYSTEM.md) (how a screen routes input to its
panes).

Sources: Ghidra on `data/Revenant.exe`. The decompiles are in
[`recon/discovered/mapinput/`](../../../recon/discovered/mapinput/), named
`<Class>_<Method>_<addr>.cpp`; the 1998 source (`mappane.cpp`,
`3dscene.cpp`, `animimage.cpp`, `imagery.cpp`, `cursor.cpp`) for names.
Confidence: **[C]** read in the decompile or disassembly, **[I]** inferred,
**[U]** unknown.

## 0. Summary

- The map pane (`TMapPane`, the global `MapPane` `0x006668d8`, vtable
  `0x5a5370`) is an ordinary pane of the play screen (added at
  `0x0047ad65`): the screen hands it clicks and moves under the pointer,
  and draws it every frame. [C]
- **The pick** (`TMapPane::OnObject(x, y, with)` `0x00452520`) asks the
  3D scene first: the mesh drawn over the pointer (a probe of 9 pixels
  around it, found while the frame drew). Only if no mesh is there does it
  walk the map's 2D objects: the screen rectangle must hold the point,
  then the first such object wins unless a later one is always-on-top or
  has its own pixel visible at the point. Objects with no cursor (and no
  pickup) are skipped; tiles and 3D meshes never come from the walk. [C]
- **The cursor** is re-picked every 8th screen frame in
  `TMapPane::Animate` (`0x00454450`): an inventory item gives the hand,
  anything else its `CursorType(dragobj)`. The chosen bitmap ("hand",
  "eye", "mouth", "door", "stairs", "hourglass", "swords") is drawn *in
  place of* the arrow, not in its corner, and only for that frame; the
  pane sets it again each frame. [C]
- **Left button** (no combat, no bow): button down remembers the object
  under the pointer; button up on the same object, if it is within 96
  units of Locke or is a character, `Use`s it (or picks it up if it is an
  inventory item). A farther object makes Locke walk toward it (64 units
  short), if the straight line to it is walkable, else "too far"; an item
  walked to is picked up on arrival. A click on the floor does nothing:
  walking is the right button. [C]
- **Right button** held: walk toward the pointer (the wedge cursor, the
  numpad direction keys synthesized from the angle); release stops. [C]
- In combat mode a left press attacks (a random swing), or, over another
  character Locke can fight, makes it his target. With the bow: draw and
  aim, release shoots. [C]

## 1. The map pane in the play screen

`TMapPane` vtable `0x5a5370` (35 slots; `TPane` base `0x5a4494`). The
slots it overrides, by their 1998 roles [C, from the bodies]:

| Slot | Address | Role |
|---|---|---|
| 0, 1 | `0x0044d5c0`, `0x0044d9c0` | `Initialize`, `Close` |
| 3, 4 | `0x0044dca0`, `0x0044ddc0` | background buffers |
| 10 | `0x00487c10` | `Update` |
| 18 | `0x00450dc0` | `PaneResized` (stores the pane rect, re-clips) |
| 19 | `0x00454390` | `Pulse` (objects, `UpdateMapPos` `0x004539d0`, scroll origin) |
| 20 | `0x004541c0` | `DrawBackground` |
| 22 | `0x00454450` | `Animate(draw)`: objects, the 3D scene, **the cursor** (§3) |
| 24 | `0x004545f0` → `0x00457ef0` | per-object overlay pass |
| 25 | `0x0044f140` | `MouseClick(button, x, y)` (§4) |
| 26 | `0x004504d0` | `MouseMove(button, x, y)` (§5) |
| 27 | `0x0044e6e0` | `KeyPress` |
| 30 | `0x00454600` | `OnEvent` (`0x101`/`0x102`: blit-effect flag) |

`x`, `y` are pane coordinates (the pane sits at 0,0 on the 640×480
screen). The pane's scroll origin (`+0x78`, `+0x7c`, the 1998 `posx`,
`posy`) turns them into map-screen coordinates, the space
`WorldToScreen` maps world positions into: `p = (x + posx, y + posy)`.

## 2. The pick: `OnObject(x, y, with)` `0x00452520`

```
on = (x, y) == last 3D pick point ? last 3D pick : Pick3D(x, y)     // §2.1
if (!on)
    p = (x + posx, y + posy)
    updatemulti->SetClipRect(p.x, p.y, 1, 1)                          // vtable +0x44
    priority = false
    for each object in the map window (TMapIterator flags 0x20,       // 0x0044cf80
                                        CHECK_NOINVENT)
        if !obj->OnObject(p)                              continue    // §2.2
        if !Editor && obj->ObjClass() == TILE (9)         continue
        if obj->imagery's imageryid == OBJIMAGE_MESH3D (1) continue   // meshes: §2.1 only
        if on && !obj->AlwaysOnTop() && (priority || !obj->GetZ(updatemulti))
                                                          continue    // §2.3
        if !Editor && !(GetDragObj() == null && obj->IsInventoryItem())
                   && obj->CursorType(with) == CURSOR_NONE
                                                          continue
        on = obj;  priority = obj->AlwaysOnTop()
    updatemulti->SetClipRect(0, 0, w, h)
return on
```

[C] The mesh test reads `inst+0x54` (imagery) `+4` (its entry) `+0x54`
(the header) `+0` (`imageryid`): retail's `SImageryEntry` is the 1998
layout (80-byte file name, status, header). The 1998 `OnObject` tested
`IsInInventory` instead and had no 3D stage.

What the rules mean:

- The **first** object (in the map iterator's order) whose rectangle
  holds the point and that has something to offer becomes the answer. A
  later one replaces it only if it is **always on top**, or if no
  always-on-top object has been taken yet and **its own pixel is visible
  at the point** (`GetZ`). So a visible pixel beats a mere rectangle, and
  always-on-top beats both.
- A 3D object found by the probe is returned as is: no `CursorType`
  filter. Locke himself is a mesh, so the pointer over Locke picks Locke
  (whose cursor is none). [C]

### 2.1 The 3D pick (`T3DScene::DrawScene` `0x00412db0`, `PickAt` `0x004142d0`)

Outside the editor, every `DrawScene` (`0x00412db0`, called from
`TMapPane::Animate`) picks at the pointer (`cursorx`, `cursory`,
`0x00668510`/`14`) unless `0x006680e4` is set [C]:

- Before the meshes draw, 9 probe pixels of the 16-bit back buffer are
  saved and painted magenta `0xf81f`: the pointer's pixel, its four
  diagonal neighbours (±1, ±1), and (0, ±2), (±2, 0).
- Each mesh object of the first pass is drawn (`+0x40`, the D3D draw);
  its screen extent (`0x005e8748..54`, software path) must overlap the
  5×5 square around the pointer; then the probe pixels are read and set
  back to magenta. If any is no longer magenta, the object covered it:
  `0x005e91d4` (the pick) = that object. Later objects overwrite earlier
  ones, so **the last-drawn mesh with a visible probe pixel wins**; the
  z-buffer (the map's, then the meshes drawn before) hides covered ones.
- After the scene the probe pixels get their saved values back.
- First-pass objects: imagery `OBJIMAGE_MESH3D` and not class 25
  (EFFECT), drawn on the current level, not `OF_INVISIBLE`/`OF_EDITOR`
  outside the editor.
  The player is one of them.

`PickAt(x, y)` `0x004142d0` sets the pick flag and the point and runs a
whole `DrawScene` to answer for a point other than the pointer's; the
result is cached with its point (`0x005e91cc`/`d0`/`d4`), which is how
`OnObject` reuses the frame's pick. [C]

### 2.2 The rectangle (`TObjectInstance::OnObject` `0x00477b30`)

`OnObject(p)` = `GetScreenRect(r)` (slot `0xf4`, `0x00471020`) and
`r.In(p)` (`0x0041c720`, edges included). A light (`OF_LIGHT`, 4) uses
its imagery's rectangle (the light icon). `GetScreenRect` is the
imagery's (`TObjectImagery::GetScreenRect` `0x00447210`, the 1998 body):
the object's map-screen position less the state's registration point
(`regx`, `regy`), the state's `width` × `height` (the largest frame); a
state with no size gets 32×32 at (16, 32) outside the editor's rect
update. [C]

### 2.3 Visible at the point and always on top

`TObjectInstance::GetZ(surface)` (slot `0x104`, `0x00477bc0`) and
`AlwaysOnTop()` (slot `0x10c`, `0x00477be0`) ask the imagery (`+0x18`,
`+0x28`). [C]

| Imagery | `GetZ(oi, surface)` | `AlwaysOnTop(oi)` |
|---|---|---|
| base `0x5a486c` | `0x004484f0`: false | `0x00448500`: false |
| `T3DImagery` `0x5a35ac` | `0x004109f0`: true | `0x00410a00`: true |
| `TAnimImagery` `0x5a384c` | `0x00418b70`, below | `0x00418830`: the still image isn't z-buffered and the state is `ANIIM_LIT` |

`TAnimImagery::GetZ` (the 1998 body): no still image → false. A
`ANIIM_LIT` state whose bitmap has no z-buffer → true. A state with
neither `ANIIM_LIT` nor `ANIIM_UNLIT` → editor only. An alpha bitmap
without a z-buffer → editor only. Otherwise `surface->ZFind` (`0x004bddc0`)
of the still at the object's screen position, inside the 1×1 clip at the
point: true when one of its non-transparent pixels there passes the
screen's z-buffer, i.e. **the object's own pixel is what the frame shows
at the point**. [C] (Retail drops the 1998 `DM_REVERSEHORZ` for flipped
objects. [C])

`TPlayer::GetZ` is false outside the editor (1998). `IsInventoryItem`
(slot `0x134`, `0x00477c00`) is `flags & 0x40000000`.

## 3. The cursor (`TMapPane::Animate` `0x00454450`)

```
AnimateObjects(draw)                       0x00458750
if draw && !0x006682bc: DrawScene()        0x00412db0 (the 3D pick, §2.1)
x = cursorx - pane.x, y = cursory - pane.y; inside the pane:
if (CurrentScreen->frame (+0x48) & 7) == 1:  cursortype (+0x12c) = CURSOR_NONE
    if !Editor:
        on = OnObject(x, y, null)
        if on && GetDragObj() == null && on->IsInventoryItem()
              && !(on->flags & OF_INVISIBLE):
            cursortype = CURSOR_HAND
            if on has an animator: animator->+0x35 = 1        0x004461c0 [U: a hover mark]
        else if on:
            cursortype = on->CursorType(GetDragObj())          slot 0xc0
SetMouseCornerType(cursortype)                                 0x0043a0d0
```

[C] The type is worked out every 8th screen frame (the screen's frame
counter, one per drawn tick: about 3 times a second at 24 Hz) and kept in
the pane (`+0x12c`) between; it is handed to the cursor every frame.

**The bitmap** (`0x0043a0d0`): `CURSOR_NONE` (−1) does nothing; any other
type looks up `CursorTypes[type]` (`0x005ce648`: "hand", "eye", "mouth",
"door", "stairs", "hourglass", "swords") and sets it as the corner bitmap
(`0x006563c4`). The cursor draw (`0x0043a480`, every frame) draws the drag
bitmap, then **the corner bitmap if there is one, else the arrow** — the
"corner" bitmap replaces the arrow at the pointer, registration point and
all — and clears the corner bitmap and its priority for the next frame.
`SetMouseBitmap` (`0x0043a020`) with anything but "cursor" also clears it.
[C]

`TExit::CursorType(with)` (`0x0050d370`): an openable, visible door shows
"door" (the hand when something is held over it); the port has it
(EXITS.md §7). In the port's (1998) classes a character shows "mouth"
unless it is aggressive (none) or a corpse with items (the hand); scrolls
the eye, food the mouth; inventory items get the hand from `Animate`
itself. Retail's overrides of those classes weren't checked [U].

## 4. Clicks (`TMapPane::MouseClick` `0x0044f140`)

Before anything: a point over one of the side tabs' controls
(`0x004364d0` on the side tabs pane `0x0065be50`) is left to them. Outside
the editor every click sends `Notify(4, player)` (`0x00499ff0`) first.
[C]

### 4.1 Right button

- **Down** (2): outside the stats pane's info mode (`0x0065c9e0`),
  `UpdateMouseMovement` (`0x0044ee00`, §5). In info mode: the object under
  the pointer (not a tile, effect, exit, helper or shadow) goes to the
  stats pane (`0x005496a0` on `0x0065b140`). [C]
- **Up** (5): if a right-button walk is on (`+0x11c`): the arrow back,
  the synthesized direction key released, the walk flag cleared. [C]

### 4.2 Left button down (1)

Only with a player and inside the pane. With `doing` the player's
current action block (`+0xe0`) and the hover type (`+0x12c`) none or
"swords" [C]:

- **Combat** (`doing` is `ACTION_COMBAT` 3): `on = OnObject(x, y, null)`;
  a character (class 12) that isn't Locke's target and that he may fight
  (`0x004c89c0`) becomes his target (`0x004d4790`); otherwise
  `ButtonAttack(random(1, 3))` (`0x004d2480`). Done.
- **Bow** (`doing` is 0x19): if the bow isn't drawn, `DrawBow`
  (`0x004d0aa0`) and `AimBow` at the pointer (`0x004d0c70`); `clicked` set.
- **Otherwise**: `on = OnObject(x, y, null)`; `onobject` (`+0xf8`) = its
  map index or −1; `clicked` (`+0x120`) set; an inventory item also goes
  to the stats pane's info. Nothing else happens on the press.

### 4.3 Left button up (4)

With a live player (slot `0x1c0` ≥ 1) and inside the pane [C]:

- Combat mode with the press consumed above: nothing. Bow drawn and
  pressed: `ShootBow` toward the pointer (`0x004d0fd0`).
- `with` = the item held from the inventory, the belt or the equipment
  pane; `on = OnObject(x, y, with)`.
- **A click** (`clicked`):
  - `on` is null, or within 96 units of Locke (`Distance`, slot 4,
    `0x0046ea20`: < 0x61), or a character: if `on` is the object pressed
    (`onobject`), then
    - not an inventory item: `on->Use(player, -1)` (slot `0xbc`) — a
      door's `Use` runs its USE script (EXITS.md §1.7, §1.9);
    - an inventory item: `player->Pickup(item, null)` (`0x004cfef0`) and
      `TakenObject` = it (players and characters are never picked up).
  - farther, not a character: walk to it. `FindClickPos` (`0x0044e930`)
    gives the floor point under the pointer; the goal is that point less
    64 units toward Locke (2 steps of 32 along the line). The line from
    Locke is checked every 32 units for `(dist − 32) / 32` steps with
    `GetWalkHeightRadius(p, level, 32)` (`0x004530a0`): a rise over 32 or
    a zero (blocked) walk cell stops it. All clear: `Goto(goal, on)`
    (`0x004cedb0`); else the text bar shows `ITEMTOFAR`. The object given
    to `Goto` (`+0x288`) is picked up when the walk arrives within 8
    units (`0x004c6155`, `0x004c7fc3`: `Pickup(obj)`, only for a block
    `Goto` marked, `+0x60` bit `0x1000`), which does nothing for a
    non-item: a far door takes a second click.
  - A press and release on the floor (no object) does nothing.
- **No click** (a drag from the inventory/belt/equipment let go over the
  map): `with` used on the object under it (`on->Use(player, with)`), else
  dropped on the floor at the pointer (`FULLSINGDROP` / `FULLMULTDROP`
  messages, a walk check, armour falls flat). [C, not detailed further
  here]
- Always: `clicked` cleared; inside the pane the drag bitmap, the drag
  talisman name and the drag object are cleared (`0x0043a100`,
  `0x0043a170`, `0x0043a140`).

## 5. Moves and the right-button walk

`MouseMove` (`0x004504d0`), outside the editor [C]:

- Right button held and walking: `UpdateMouseMovement` again.
- Bow drawn: `AimBow` at the pointer.

`UpdateMouseMovement` (`0x0044ee00`): the point under the pointer at
Locke's height + 50; within 16 units of Locke (x and y) the arrow and the
walk flag, nothing else (the direction key stays down); otherwise the
`wedge-<dir>` cursor with its `…shadow` at (0, 43), and the numpad
direction for the 45° sector of the angle (`(angle + 0x10) & 0xe0`:
PgUp, Right, PgDn, Down, End, Left, Home, Up for NE … N) pressed through
the screen (slot `0x30`), the previous one released. [C]

## 6. Port notes

What the port had before this work (2026-10-07):

- `TMapPane` isn't one of `TPlayScreen`'s panes; the game mode
  (`src/runtimemode.cpp`) takes the play field's mouse events. Its left
  button only attacked in combat; its right button walked with its own
  copy of `UpdateMouseMovement`. `TMapPane::MouseClick`, `MouseMove` and
  `Animate` (the 1998 bodies) were never called, so no hover cursor and
  no click on an object.
- `TMapPane::OnObject` was the 1998 body: no 3D stage, `GetZ` and
  `GetScreenRect` through the 1998 software surfaces (`updatemulti`,
  `posx`/`posy`), which the GPU renderer doesn't keep. It had no caller.
- The GPU renderer already writes an object id per pixel (the G-buffer's
  id target, drawable index + 1; opaque tiles and meshes, not transparent
  tiles), used by the editor's click-select through a synchronous
  one-pixel readback.

The port's design and state: §7.
