# TStatPane (retail) — forensics from disassembly + emulator run

Subject: the side panel's Stats pane, `TStatPane` (`d:\revenant\StatPane.cpp`,
literals `0x5e5604/5620/563c/5658`), class `cls_0x5a5ba0`, vtable `0x005a5ba0`,
global instance `0x0065b140`.

## 0. Method and how sure each part is

- **Static:** capstone disassembly of `recon/retail_asm/baseline/Revenant.rebuilt.exe`
  (SHA-256 `28bec273…72b5`, the same file as the GOG `Revenant.exe` and the emulator's
  `retail-cd/REVENANT/Revenant.exe`). Every address below is an instruction address in
  that image. I did not use Ghidra output except to cross-check.
- **Dynamic:** the HUD emulator (`tools/retail_runtime/slots/hud`); the committed
  fixture is `statpane.py` (`hud_ab.py statpane`). It loads `StatsPane.dat`→`0x65c6f4`
  and `SpellIcons.dat`→`0x65bc3c`, builds a hudscene fixture player with a stat sheet
  (§10), runs Initialize `0x546b50`, SetClipRect (vt+0x24) and slot 20 `0x549010`, and
  records every GDI call (TA_UPDATECP modelled as Windows does it).
- Items marked **[EMU]** were seen in the emulator. Unmarked items are static only.
  Pixel widths in the emulator come from macOS Arial's hdmx table, not Win98 GDI. So x
  positions set by POS/TAB, and the right edges of right-aligned text, are exact. The x
  of text that follows other text on the same line depends on font metrics (see
  UNCONFIRMED U1).

---

## 1. Object layout (pane fields used by this class)

| off | meaning | written at | read at |
|---|---|---|---|
| +0x04,+0x08,+0x0c,+0x10 | pane **x=452 (0x1c4), y=0, w=188 (0xbc), h=306 (0x132)**. +0x14..+0x20 hold the same values again | ctor `0x488910` | SetClipRect `0x491a80`, MouseClick bounds `0x5493f6/0x5493fb`, flush `0x549389-0x549396` |
| +0x2c,+0x30 | scroll offset subtracted when the pane sets the surface origin | — | `0x491a95/0x491aa0` |
| +0x3c | owning screen. Slot 16 `0x491bb0` returns `+0x3c == [0x667fd0]` | — | stat notify `0x4735e4` |
| +0x40 | must be non-zero for a stat change to dirty the pane (meaning UNCONFIRMED) | — | `0x4735c8` |
| +0x48 | hidden flag (Hide `0x491bd0` sets it to 1; showing the pane clears it at `0x47d49c/0x47f02b`) | | `0x4735d1` |
| +0x50 | **dirty** (slot 11 `0x444fc0` = SetDirty(v): `mov [ecx+0x50],v`) | | slot 20 `0x549013` |
| +0x88/+0x98 | TButtonPane button count / array | AddButton `0x436790`, ClearButtons `0x436890` | |
| +0x17c | HDC of the draw surface (GetDC) | paint `0x54922d` | WriteText/WriteField, MoveToEx everywhere |
| +0x180 | HDC of the mosaic scratch surface | paint `0x549294` | multipage `0x5486f1/0x5487cd` |
| +0x184 | the draw surface passed to the paint | paint `0x54921a` | ICON, flush |
| +0x188 | the "RingT" surface (`cls 0x5a3ee4`, 0x6c bytes, made from SpellIcons.dat "RingT") | Init `0x546c68` | ICON `0x547bdd` |
| +0x18c | mosaic scratch surface (`cls 0x5a3980`, 0x78 bytes, 188×306, flags 0x200, origin set to pane x,y) | Init `0x546bee` | paint |
| +0x190/+0x194 | text work buffer / its size (malloc 0x800 per paint) | paint `0x5492e9/0x5492fd` | TEXT/FIELD |
| +0x198 | **bound object** (only SetObject writes it, at `0x5496c4`) | | everywhere |
| +0x19c | **format name** (heap string). SetObject sets "page1"; SetFormat sets the button name | `0x549716`, `0x488aa9` | FORMAT match `0x5477ea` |
| +0x1a0/+0x1a4 | statpane.def text (NUL-terminated) / its length | Init `0x546cbf/0x546ca5` | interpreter `0x547666` |
| +0x1a8 | stream position of the last block walked (decides whether buttons are rebuilt) | `0x547907` | `0x5478f7` |
| +0x1ac | multipage **current page** (0..2) | Next/Prev `0x5494ce/0x549508`, clamp `0x54881a` | grayable/multipage |
| +0x1b0 | multipage **last page index** (0..2) | `0x54873b/0x5487f2/0x548802` | Next rule `0x5485c5` |

---

## 2. Initialize (slot 0, `0x00546b50`) and Close (slot 1, `0x00546d70`)

```
Initialize(this):                                   // called from TPlayScreen init 0x47aced
  TPane::Initialize()                               // 0x546b71 call 0x434e40
  m = new(0x78) cls_0x5a3980                        // 0x546b76..0x546bcc
      m->Init(w=this[+0xc]=188, h=this[+0x10]=306, 0x200, m[+0x10], 0)   // 0x4a5740 @0x546bc7
      m[+0x68]=0; m[+0x70]=1
  this[+0x18c] = m
  m->vt[0x40](this[+4]=452, this[+8]=0)             // 0x546bf4: mosaic origin = pane pos
  r = new(0x6c) cls_0x5a3ee4(bitmap = [0x65bc3c]->Find("RingT"))   // 0x546c1c..0x546c40
  this[+0x188] = r
  sprintf(path, "%s%s", [0x65dde8] /*ResourcePath*/, "statpane.def")   // 0x546c49..0x546c6e
  f = OpenResource(path, "rt", 0)                   // 0x546c7e call 0x4a13f0; 3rd arg 0 = archive first
  if !f: FatalError("Unable to load STATPANE.DEF")  // 0x546c8d
  len = FileLength(f)                               // 0x4a17b0
  this[+0x1a4] = len
  if len < 1: this[+0x1a0]=this[+0x1a4]=0
  else: buf = malloc(len+1); fread(buf,len,1,f)     // 0x4a15a0
        buf[len]=0
        if buf[0] & 0x80: XOR every byte with 0xCC  // 0x4834e0 / 0x483540 (obfuscated DEFs)
        this[+0x1a0]=buf
  // the file is never closed (no close call follows; minor retail leak)
  this[+0x1a8]=this[+0x17c]=this[+0x184]=this[+0x190]=this[+0x194]=0
  this[+0x1ac]=this[+0x1b0]=this[+0x19c]=0         // 0x546d15..0x546d3f
  this->SetObject([0x667fcc] /*Player*/)           // vt+0xb8 @0x546d45
  return 1
```
- **No buttons are made here.** All buttons come from the DEF's `BUTTON` tokens during
  the paint (§4.3). No key bindings, no art.
- **Which file loads [EMU]:** `0x4a1240(path, mode, preferLoose=0)` looks in the
  mounted archives first (`0x4a12b5..0x4a1309`) and falls back to `fopen` only after.
  - In the emulator the loaded length was **8679**: the `resources.rvr` member,
    byte-identical to `data/resources_unzipped/statpane.def`.
  - The 8763-byte loose `data/Resources/statpane.def` (shipped with the 1.2 patch, with
    the Atck/Def/Attack/Defense rows commented out) is **not** used.

```
Close(this):  free(this[+0x1a0]); this[+0x1a0]=this[+0x1a4]=0       // 0x546d7b
              if mosaic: mosaic->vt[0](1) /*delete*/; if ring: ring->vt[0](1)
              free(this[+0x19c]); this[+0x19c]=0; TButtonPane::Close  // 0x546dd8 call 0x434f30
```

---

## 3. The paint

### 3.1 Slot 20 `0x00549010` (called every frame by the screen)
```
if this[+0x50]:                                     // 0x549013
    this->vt[0x54](display=[0x5d79e0], drawmode=0x10)   // 0x549022..0x549027: slot 21 = paint
    this->vt[0x2c](0)                               // 0x54902c..0x549030: SetDirty(0)
```
Before slot 20 the screen calls slot 9 `0x491a80(surface=0)` (SetClipRect):
`display->vt[0x40](x-[+0x2c], y-[+0x30])` sets the origin (452,0),
`display->vt[0x44](452,0,188,306)` sets the clip, `display->vt[0x48](0)` [EMU: display
origin read back as 452,0].

### 3.2 Slot 21 `0x005491c0` paint(surface, drawmode)
```
surface->vt[0x64](0,0,188,306, color 0, 0xffff, 0, drawmode)  // 0x5491ce..0x5491ea Box (TSurface 0x4bde60)
bm = [0x65c6f4 /*StatsPane.dat*/]->Find("Stats")               // 0x5491ed..0x5491fb (0x46d710)
surface->Put(0, 0, bm, drawmode, 0)                            // 0x549200..0x549207 (0x4bd680)
this->vt[0x90]()                    // 0x549210 slot 36 0x4361b0: every button flags |= 0x20 (dirty)
this[+0x184] = surface              // 0x54921a
surface->vt[0xc]()->GetDC(&this[+0x17c])   // IDirectDrawSurface vt+0x44; failure -> 0x4a90d0(err,"StatPane.cpp",0x354)
SelectObject(this[+0x17c], FontTable(0x65b010).Get([0x65abc4])+0x38)    // 0x549247..0x54925f
SetViewportOrgEx(this[+0x17c], surface[+0x14], surface[+0x18], 0)      // 0x549265..0x549279 (= 452,0)
mosaic=this[+0x18c]; mosaic->vt[0xc]()->GetDC(&this[+0x180])           // err line 0x366
SelectObject(this[+0x180], same font); SetViewportOrgEx(this[+0x180], mosaic[+0x14], mosaic[+0x18], 0)
this[+0x194]=0x800; this[+0x190]=malloc(0x800)                         // 0x5492e4..0x5492fd
RenderFormat()                                                          // 0x549303 call 0x5475e0
free(this[+0x190])
mosaic ReleaseDC(this[+0x180]) (err 0x371); surface ReleaseDC(this[+0x17c]) (err 0x360)   // vt+0x68
this->vt[0x2c](0)                   // 0x549374 SetDirty(0)
this->0x435cb0(surface)             // 0x54937a slot 35: TButtonPane draws its buttons (all invisible here, §4.3)
if surface == [0x5d79e0]: 0x4aacb0(452, 0, 188, 306, 6)   // 0x549389..0x54939b: display dirty rect
```
[EMU] Primitives in one paint, in order:
1. `Box(0,0,188,306,color 0,mode 0x10)`, done as two `ParamBlit` with mode `0x20000010`
   (DM_FILL|DM_BACKGROUND).
2. `Put(0,0,"StatsPane:Stats" 188×306, mode 0x10)`.

Nothing else. All other output is GDI text. `drawmode` is always `0x10`
(DM_BACKGROUND), passed in by slot 20.

**Font:** `[0x65abc4]` is the result of `FontTable.Find("Small")`, stored at `0x485dcf`
(`push "Small"` at `0x485db1`, then `call 0x4acb30`). FONT.DEF defines
`WINFONT "Small" FONT "Arial" 12`, i.e. `CreateFontA(+12, Arial)`, a 12 px cell.
[EMU] Every text call was "Arial 12 w400". There is one font for the whole pane; the
DEF has no font token.

**Coordinates:** GDI viewport origin = the surface origin = pane (452,0). So every DEF
coordinate is **pane-local**, and screen = (452 + x, 0 + y) in Classic 640×480.

---

## 4. The format interpreter `0x005475e0` (statpane.def)

### 4.1 Locals and initial state (`0x547601..0x547647`)
| local (frame offset) | meaning | initial |
|---|---|---|
| `[esp+0x18]` | cursor **x** | 0x14 = 20 |
| `[esp+0x14]` | cursor **y** | 20 |
| `[esp+0x1c]` | **lineStartX** | 20 |
| `[esp+0x94]` | **lineHeight** | 0xc = 12 |
| `[esp+0x78]` | **lineWrap** (absolute x limit; −1 = none) | −1 |
| `[esp+0x60..0x62]` | **current colour**, stored as bytes {B,G,R} | ff,ff,ff (white) |
| `[esp+0x68]` | named-colour count (max 8) | 0 |
| `[esp+0x188]` + 10·i | colour names (char[10], 9 chars + NUL) | |
| `[esp+0x134]` + 4·i | colour values {B,G,R,·} | |
| `[esp+0x98]` | **firstPass** (rebuild the buttons) | 1 |

The tokenizer is the shared TToken at `[esp+0x20]`:
- type at `[esp+0x30]`: 2 TEXT, 4 IDENT, 8 NUMBER, 10 EOF;
- text pointer at `[esp+0x48]`;
- methods: `0x478a10` Get, `0x479580` WhiteGet, `0x4795a0` LineGet, `0x4795c0` SkipLine,
  `0x4795f0` SkipBlock, `0x479450` DefineGet, `0x479700` Is(text) (= `_stricmp`,
  case-insensitive), `0x47a410` Parse(fmt,…).

So keywords are case-insensitive, `//` comments and `#define` lines work, and quoted
strings are type 2.

### 4.2 Binding and block selection
```
if this[+0x198]==0:                                 // 0x54760e
    if [0x667fcc]==0: return 0                      // 0x547657 (chrome only, no text)
    this->SetObject([0x667fcc])                     // 0x547660 vt+0xb8
if this[+0x1a0]==0: WriteText(20,20,"Invalid Stat Layout",white); return 0   // 0x54766e..0x547694
stream = memory stream over [buf, buf+len)          // 0x5476a0..0x5476d7 (vtable 0x5a36f8, name "String")
t.DefineGet()                                       // 0x54773e
loop while t.type != EOF:
    pos = stream->GetPos()                          // 0x547754 -> ebp
    if !t.Is("CLASS"): error                        // 0x547763 ("Invalid Stat Layout", return 0)
    t.WhiteGet(); if t.type != TEXT: error          // 0x547779
    if t.Is("default") or ListMatch(t.text, obj[+0x48]->name):  // 0x547785 / 0x5477a9
        // ListMatch 0x483460: t.text is a COMMA-SEPARATED list, compared case-insensitively
        t.WhiteGet(); match=1
        if t.Is("FORMAT"): t.WhiteGet(); match = t.Is(this[+0x19c]); t.WhiteGet()   // 0x5477c9..0x547805
        t.LineGet()
        if match: goto FOUND                        // 0x547815
    else: t.SkipLine()                              // 0x5477b9
    t.SkipBlock(); t.LineGet()                      // 0x54781f, 0x547828
// EOF without a match falls through to FOUND's ClearButtons and then fails the BEGIN test -> error
FOUND:                                              // 0x5478f7
    if pos == this[+0x1a8]: firstPass = 0
    this[+0x1a8] = pos
    if firstPass: this->ClearButtons()              // 0x547919 call 0x436890 (deletes all, SetDirty(1))
    if !t.Is("BEGIN"): error                        // 0x547928
    t.LineGet()
    while !t.Is("END"): dispatch(token)             // loop head 0x5479a4 / tail 0x5488ba
    t.LineGet(); free token buffer; return 1        // 0x5488d4..0x548fe3
```
- Blocks are tried in file order. `"default"` matches any object, so put it last.
- The object's class name is `[obj+0x48]+4`: "PLAYER", "CHARACTER", "WEAPON"… [EMU
  "PLAYER"].
- The whole file is re-tokenised on **every** paint. The named colours are locals, so
  they are re-declared every paint.
- Any parse error writes "Invalid Stat Layout" in white at (20,20) and **aborts** the
  rest of the walk (return 0).
- An unknown keyword is an error (`0x548d4d`).

### 4.3 Tokens

Each branch consumes its own tokens. The loop does not advance the token (`0x5488ba`
only re-tests "END").

| token | syntax | effect (addresses) |
|---|---|---|
| `BUTTON` | `BUTTON "name" x y w h` | `Parse("%20s %d %d %d %d\n")` (`0x5479f8`). On a parse failure it writes the error text but continues. **Only when firstPass** (`0x547a3d`): `b = new(0x148) TButton` (`0x42c600`) with args (name, x, y, w, h, key 0, bitmaps none {0,0,0}, −1, **flags 0x100000**, group −1, 0) (`0x547a6a..0x547ab6`), then `AddButton(b)` (`0x547aca`, `0x436790`: index = count, flags \|= 0x20). Flag 0x100000 makes TButton's draw return at once (`0x42cc4f`), so these are **invisible hot-spots**. [EMU] Page1 gives 8 buttons `page1 (63,108,25,14)`, `page2 (89,108,29,14)`, `trainstrn..trainluck (24,120+12k,62,12)`, all with flags 0x100020, key 0, group −1. Page2 gives only page1/page2. |
| `ICON` | `ICON` | WhiteGet, LineGet (`0x547aeb..0x547af8`). Releases the DC (`0x547b12`, err 0x360). `bm = obj->vt[0x130]()` (inventory image, `0x547b34`). If `bm[+0x18]==0` (no key) and class≠8 (money): temporarily key = 0xf81f (16-bpp display) or 0x7c1f (`0x547b5a..0x547b7e`). If class≠0xc (not a character): `surface->0x4bda20(x+3, y+3, bm, 0x10100, 0x1f)` (offset shadow copy, `0x547bb0`). Always `surface->Put(x, y, bm, 0x100 /*DM_TRANSPARENT*/, 0)` (`0x547bcc`). If class==0xc: `desc=0x438d80(&d, x, y, 0, 0, ring.w, ring.h, 0x100)`, then `surface->vt[0x5c](&d, ring, 0, 0)` (RingT over the portrait, `0x547c0c..0x547c25`). Restores the key, re-gets the DC (err 0x354), re-selects the font and viewport (`0x547c33..0x547c98`). **The cursor is not moved.** |
| `LINEHEIGHT` | `LINEHEIGHT n` | `Parse("%d\n", &lineHeight)` (`0x547cd1`) |
| `LINEWRAP` | `LINEWRAP none` / `LINEWRAP n` | "none": lineWrap = −1 (`0x547d27`). Otherwise `Parse("%d\n",&lineWrap)` (`0x547d43`). |
| `COLOR` | `COLOR r g b` / `COLOR name r g b` / `COLOR name` | WhiteGet. If the token is an IDENT: look the name up (`_stricmp`). If it is missing and count<8, add it (name truncated to 9 chars). The target becomes that entry (`0x547d88..0x547df5`); then WhiteGet. If the next token is a NUMBER: `Parse("%b %b %b\n", &t[2], &t[1], &t[0])`, i.e. R→byte2, G→byte1, B→byte0 (`0x547e0f..0x547e22`). The target is the current colour when no name was given. Otherwise, with a valid named entry: current = that entry (`0x547e47`), then LineGet. A **new** name with no numbers copies an uninitialised entry (retail quirk). |
| `POS` | `POS x y` | `Parse("%d %d\n",&x,&y)`, then **lineStartX = x**, `MoveToEx(x,y)` (`0x547e7b..0x547eb1`) |
| `MOVE` | `MOVE dx dy` | x += dx; y += dy; **lineStartX = x**; MoveToEx (`0x547f02..0x547f32`) |
| `TAB` | `TAB n` | **x = lineStartX + n** (relative to the line start); MoveToEx(x,y) (`0x547f7b..0x547f9a`) |
| `NEXTLINE` | `NEXTLINE` | WhiteGet, LineGet; **y += lineHeight; x = lineStartX**; MoveToEx (`0x547fca..0x547fed`) |
| `TEXT` / `FIELD` | see §4.4 | |
| `END` | | ends the block |

### 4.4 TEXT / FIELD grammar (`0x547ff8..0x5488b5`)

```
TEXT  [modifier] [colorname] <token>
FIELD [modifier] [colorname] ["prefix"] fieldname ["suffix"]
```

**modifier** is **at most one** of the keywords below. There is no loop: each match
jumps straight on to `0x5481fa`. Flags are reset at `0x548031..0x548064`.

| modifier | args | effect |
|---|---|---|
| `block` | `w h` | `Parse("%d %d",&w,&h)`; block=1 (`0x5480a0`) |
| `multipage` | `w h` | same parse; multipage=1, block=1 (`0x5480ec`) |
| `grayable` | | grayable=1 |
| `prevbutton` | | prev=1, grayable=1 (`0x54814a` → `0x548122`) |
| `nextbutton` | | next=1, grayable=1 |
| `center` | | `SetTextAlign(hdc, 7)` = TA_CENTER\|TA_UPDATECP (`0x548188`); noWrapGate (`[esp+0x8c]`) = 0 |
| `right` | | `SetTextAlign(hdc, 3)` = TA_RIGHT\|TA_UPDATECP (`0x5481bc`); noWrapGate = 0 |
| `left` | | `SetTextAlign(hdc, 1)` = TA_LEFT\|TA_UPDATECP (`0x5481dc`) |
| *(none)* | | `SetTextAlign(hdc, 1)` without consuming a token (`0x5481ed..0x5481f4`) |

The block-type and grayable-type modifiers do **not** call SetTextAlign. They inherit
the alignment left by the previous TEXT/FIELD (or align 0 after a block FIELD).

**colorname:** `colorPtr = &current` (`0x5481fa`). If the token is an IDENT that names
a declared colour, `colorPtr = &colors[i]` and WhiteGet (`0x54820b..0x548247`).
Otherwise the token is left alone; for a FIELD it is the field name.

**Building the buffer** (`buf = this[+0x190]`, size `this[+0x194]` = 0x800):
- **FIELD** (`0x54826c..0x5483a4`):
  1. `buf[0]=0`.
  2. If the token is TEXT, copy it as the prefix and WhiteGet.
  3. `Parse("%30t", fieldname)` (`0x5482ca`).
  4. `ok = Resolve(fieldname, buf+len, size-len, &colorName)` (`0x548309`).
  5. If ok and Resolve returned a colour name, look that name up in the palette and set
     colorPtr (`0x548319..0x54835b`).
  6. If the next token is TEXT, append it as a suffix and WhiteGet.
- **TEXT** (`0x5483a6`): copy the token verbatim, whatever its type.
- Then `t.LineGet()` (`0x5483cd`).

**Line wrap** (`0x5483d2..0x548456`): if lineWrap > 0 and noWrapGate is set (no
center/right modifier):
```
GetTextExtentExPointA(hdc, buf, strlen, 10000, 0, 0, &sz)
if x + sz.cx > lineWrap: y += lineHeight; x = lineStartX; MoveToEx(x,y)
```
This happens **before** the failed-field check, so a field that failed measures "".

**Draw selection:**
```
if FIELD and !ok: draw nothing (prefix included)    // 0x54845a..0x54846e
elif multipage: §4.6
elif block:     WriteField(x, y, w, h, buf, &current /*NOT colorPtr*/, &x, &y)   // 0x54848e..0x5484b4
elif grayable:                                                                   // 0x5484b9
    if _stricmp(buf,"Next")==0 or next:   col = (this[+0x1ac] < this[+0x1b0]) ? green : grey   // 0x5485bf
    elif _stricmp(buf,"Prev")==0 or prev: col = (this[+0x1ac] > 0) ? green : grey             // 0x54851a
    else: draw nothing
    WriteText(x, y, buf, &col, &x, &y)       // green = RGB(6,221,0) 0x548546..; grey = RGB(60,60,60) 0x548595..
else: WriteText(x, y, buf, colorPtr, &x, &y)                                       // 0x54865e..0x54867c
```

### 4.5 Text helpers

**WriteText `0x00546de0`** (thiscall, args x, y, text, colorPtr, outX*, outY*; `ret 0x18`):
```
n = strlen(text)
SetTextColor(hdc, 0); SetBkMode(hdc, TRANSPARENT=1)            // 0x546e03, 0x546e12
MoveToEx(x+1,y+1); TextOutA(x+1, y+1, text, n)                  // 0x546e2f / 0x546e47
MoveToEx(x+2,y+1); TextOutA(x+2, y+1, text, n)                  // 0x546e5f / 0x546e77
MoveToEx(x+1,y+2); TextOutA(x+1, y+2, text, n)                  // 0x546e9a / 0x546eb2
SetTextColor(hdc, c[0]<<16 | c[1]<<8 | c[2])  // colorPtr bytes {B,G,R}, giving COLORREF 0x00BBGGRR = the DEF's r,g,b   0x546ed5
MoveToEx(x,y); TextOutA(x, y, text, n)                           // 0x546eea / 0x546eff
GetCurrentPositionEx(hdc,&pt); *outX = pt.x; *outY = pt.y        // 0x546f11..0x546f35
```
- All four passes use the DC's alignment, normally TA_UPDATECP: the reference point is
  the current position (set by the MoveToEx just before) and the cell **top** sits at y.
- The **cursor advance comes from the CP after the coloured pass**:
  - left (1): CP = x + width;
  - right (3): CP = x − width, the text's left edge;
  - center (7): CP stays at x.
- y never changes.
- Shadow: black copies at (+1,+1), (+2,+1), (+1,+2) behind the colour pass. For right
  and center text the shadows use the same alignment, so the offsets hold relative to
  the glyphs. [EMU] e.g. "Name: " at (29,63), (30,63), (29,64) in black, then (28,62)
  in cream; right-aligned "16" draws over 74..84 with its shadow over 75..85 / 76..86.

**WriteField `0x00546f40`** (args x, y, w, h, text, colorPtr, unused, outY*; `ret 0x20`):
```
SetTextAlign(hdc, 0); SetTextColor(hdc, 0); SetBkMode(hdc, 1)    // 0x546f63..0x546f81
DrawTextA(hdc, text, n, {x+1,y+1,x+1+w,y+1+h}, 0x2810)          // 0x546fca
DrawTextA(hdc, text, n, {x+2,y+1,x+2+w,y+1+h}, 0x2810)          // 0x547006
DrawTextA(hdc, text, n, {x+1,y+2,x+1+w,y+2+h}, 0x2810)          // 0x547046
SetTextColor(colour); hgt = DrawTextA(hdc, text, n, {x,y,x+w,y+h}, 0x2810)   // 0x547069, 0x5470a2
if outY: *outY += hgt                                            // 0x5470b0 (y advances by text height)
MoveToEx(hdc, x+1, y+1)                                          // 0x5470c5 (quirk: +1,+1)
```
`0x2810` = DT_EDITCONTROL\|DT_NOPREFIX\|DT_WORDBREAK: left, top, word-wrapped, clipped to
the rect. The 6th arg (&x) is never read.

**Unreferenced helpers** (out-of-line copies; no call or pointer anywhere in the image):
- `0x547170` TextWidth via GetTextExtentExPointA;
- `0x5470e0` DrawTextExA on the mosaic;
- `0x5471b0` the error writer;
- `0x5471e0` the train marker;
- `0x549040/0x5490c0/0x549100/0x549180` the DC get/release pairs.

### 4.6 multipage (`0x548686..0x5488b5`): the pane's only "scroll"
```
p0 = buf
DrawTextExA(hdcMosaic, p0, len, {x,y,x+w,y+h}, 0x2810, &dtp{cbSize=0x14})   // 0x548712, measuring pass on the mosaic
if dtp.uiLengthDrawn >= len: p1=0; this[+0x1b0]=0                          // 0x548800
else: p1 = p0+drawn; this[+0x1b0]=1
      DrawTextExA(hdcMosaic, p1, …same rect…)                               // 0x5487d5
      if drawn2 < strlen(p1): this[+0x1b0]=2; p2=p1+drawn2 else p2=0
if this[+0x1ac] > this[+0x1b0]: this[+0x1ac] = this[+0x1b0]; p2 = 0        // 0x548816 (quirk: a clamp to 2 then draws nothing)
switch this[+0x1ac]: 0→WriteField(x,y,w,h,p0,&current,&x,&y); 1→p1 (if non-null); 2→p2 (if non-null)
```
- At most 3 pages.
- The mosaic's viewport origin is (452,0) on a 188×306 surface, so the measuring draws
  land off the surface. The mosaic is never composited.
- **Page navigation:** the Next/Prev buttons (OnControl, §6) change `+0x1ac`.
  `TEXT grayable "Next"/"Prev"` draws them green or grey (§4.4).

---

## 5. Field resolution

### 5.1 Pane resolver `0x00547240` (args name, buf, size, colorNameOut*; returns 1 if found)

| field | rule (addresses) | output |
|---|---|---|
| `train<stat>` (`_strnicmp(name,"train",5)`, `0x547257`) | If the bound object ≠ `[0x667fcc]`: `buf=""`, return 1 (`0x547277`). `idx = StatIndex(name+5)` (`0x51a280`: strn 0, cons 1, agil 2, rflx 3, mind 4, luck 5, otherwise −1, which returns 0). If size≥3: '+' if `player[+0x360]==idx`; another '+' if `player->vt[0xd4]("Level") < 15 && player[+0x364]==idx` (`0x5472b3..0x5472de`). | "", "+" or "++" |
| `objdesc` (`0x5472fb`) | `key=sprintf("obj%08x", obj[+0x4c][+0x1c])`. Look it up in the string table `0x65d4d0` (`0x49d6d0`/`0x49d780`). If missing, use `sprintf("obj%s", obj[+0x48]->name)` via `0x49d800`. strncpy (`0x547396`). | description text |
| `type` (`0x5473b9`) | `t = obj->vt[0xd4]("type")`. For 0..7, string-table keys STATCFGHANDS, STATCFGKNIFE, STATCFGSWORD, STATCFGBLUDGEON, STATCFGAXES, STATCFGBLUDGEON, STATCFGBOWS, STATCFGBOWS (jump table `0x54758c`). | weapon class |
| `stmod<4-char stat><1\|2>` (`_strnicmp(name,"stmod",5)`, `0x547441`; requires name[9]∈'1'..'2' and name[10]==0) | Colour name by `obj->vt[0xdc](idx+0x22)`: <12 "red", >16 "green", else "blue" (`0x5474b4..0x5474ce`). Value: `m = obj->0x51a320(idx,0)` (attribute modifier); n=1 gives `10*m`, n=2 gives `m` (`0x5474f5/0x547506`). Format: '+' prefix when ≥0, then `_itoa(v,10)`. | e.g. "+20" |
| anything else | `obj->vt[0xc8](name, buf, 0x50)` (`0x547576`), the object's GetFieldText | |

**The shipped DEF never reaches the `stmod` path.** It asks for `strnmod1`,
`strnmod2`, `consmod1`, `consmod2`, `agilmod1`, `rflxmod1`, `mindmod1`, `mindmod2`,
`luckmod1`, `atckmod1-6` and `defmod1-6`. None of them is a field anywhere in the
GetFieldText chain or the PLAYER stat lists, and no such string exists in the exe. They
return 0, so **neither the value nor the inline label ("Dmg", " Hit", "Hlth", …) is
drawn**. [EMU] confirmed: those FIELDs make no TextOutA.

### 5.2 Object chain (vt+0xc8)

**TPlayer `0x51dfb0`:**
- `class`: `strncpy(buf, [[obj+0xfc]+0x190], size-1)`, the class display string. The
  port reads this as `chardata->classdata->name` (src/player.cpp:827).

**TCharacter `0x4d5260`** (all `_itoa` base 10 unless marked):
| field | value |
|---|---|
| `armor` | `vt2bc()` [ArmorValue]; if obj is the Player: `+ vt260()` + Σ entry[+0x134] over the list at obj+0x170 (`0x5407d0`) |
| `maxhealth` / `maxfatigue` / `maxmana` | `vt1d8()` / `vt1e0()` / `vt1e8()` |
| `attackpct` | obj≠Player: `vt2c4() − P.vt2c0() − P.vt2ec() − P.vt260() − list.0x5407d0() + vt2ec()`. obj==Player: `list.0x540820() /*Σ+0x138*/ + vt2ec() + vt2c4()`. Printed with `"%d"`. |
| `defensepct` | obj≠Player: `vt2c0() − P.vt2c4() − P.vt2ec() − list.0x540820() + vt2ec()`. obj==Player: `list.0x5407d0() + vt2ec() + vt2c0() + vt260()`. Printed with `"%d"`. |
| `damage` | `d = (vt2b0()+100)*vt2d0()/100`. If `t=vt2cc()` is 1, 2 or 4: `d = (vt3d4()+100)*d/100`. Signed /100 via 0x51eb851f, see `0x4d54f6..0x4d5560`. |
| `stealth` | `vt2e8()` |

**TObjectInstance `0x472f80`** (`buf[0]=0` first):
| field | value |
|---|---|
| `name` / `objtype` | The type name `[[obj+0x4c]+0]`, stripped to alnum, looked up in the string table `0x65d4d0`. A result starting with '[' means missing, and the raw name is used. |
| `objclass` | "CLASS"+alnum(class name) looked up the same way, then `0x59bd3e(buf+1)` (case transform of the tail) |
| `statmod` | the player's stat-mod listing via `P->vt[0xec]`, formatted with `"%s%s %d "`, `"%s%s (%d) "`, `"%s%s %d%% "` |
| `experience` | for class 0xc with a Player: `Player->0x51a5b0(obj->vt[0xd4]("Value"))`; else 0 |
| otherwise | The OBJSTATS list (`[obj+0x48]+0x1c` count, `+0x20` names, stride 0x50). Class 0x0b (player): `vt[0xdc](i)`; else `obj[+0xac][i]`. If not found there, the STATS list (`+0x14/+0x18`) with per-type values. If not found anywhere, return 0. |

[EMU] PLAYER OBJSTATS indices:
- 3 Health, 5 Mana, 17 Level, 18 Exp, 19 NextExp;
- 34–39 Strn, Cons, Agil, Rflx, Mind, Luck;
- 40 Attack, 41 Defense, 42 Invoke, 43 Hands, 44 Knife, 45 Sword, 46 Bludgeon, 47 Axes,
  48 Bows, 49 Stealth, 50 LockPick;
- 51–61 the same order + "Exp"; 62–72 the same order + "NxtExp".

So the DEF fields `level`, `exp`, `nextexp`, `health`, `mana`, the stat names, the skill
names, `<skill>exp` and `<skill>nxtexp` are all `GetObjStat(i)` as `%d`.

---

## 6. Input, page model, train buttons, dirty model

**MouseClick (slot 25, `0x5493b0`, args button, x, y):**
- `button==2` (MB_RIGHTDOWN): `SetObject([0x667fcc])`, which rebinds the player and
  resets to "page1"; then `TButtonPane::MouseClick` (`0x5493c7..0x5493dd`).
- `button==4` (MB_LEFTUP) inside (0..w, 0..h):
  - `SetDragBitmap(0,0,0)` (`0x43a100`), `SetDragObj(0)` (`0x43a140`);
  - `PlaySound(&DAT_00670648)` (`0x43a170`). This is a .bss string referenced only here,
    so it is probably empty and silent (U5);
  - `0x43a240(0,0,0)`;
  - then `this->vt[0x28]()` (slot 10 `0x445210`: dirty=1, all buttons dirty)
    (`0x549400..0x54942e`).
- Always: `TButtonPane::MouseClick(button,x,y)` (`0x436530`). Slot 26 `0x549450` only
  forwards to `0x436660`.

**OnControl (slot 37, `0x549470`, args button, msg)**, acts only when `msg==0xbb8`
(CONTROL_CLICKED = 3000). Name = `button+0x18`:
```
if _strnicmp(name,"page",4)==0: this->vt[0xbc](name)    // 0x5494a3 SetFormat: format = button name ("page1"/"page2")
elif name=="Next": this[+0x1ac] = min(this[+0x1ac]+1, 2); SetDirty(1)    // 0x5494bd..0x5494da
elif name=="Prev": this[+0x1ac] = max(this[+0x1ac]-1, 0); SetDirty(1)    // 0x5494f1..0x54950e
if bound == [0x667fcc]:                                                   // 0x549511
  trainstrn/traincons/trainagil/trainrflx/trainmind/trainluck -> stat id 0x22/0x23/0x24/0x25/0x26/0x27, idx 0/1/2/3/4/5
     if Player->vt[0xdc](id) > 30: TextBar(0x65c5d0).Print(StringTable.Get("STATMAX")); return 1   // 0x54962b..0x54963b
     else Player->TrainStat(idx) (0x51fe50); SetDirty(1)                                              // 0x549681..0x549692
return 1
```

**TrainStat `0x51fe50`:**
```
if GetObjStat(idx+0x22) >= 30: return
s = this[+0x368]
if idx == this[+0x360+4*s] and GetObjStat(idx+0x22) >= 29: return
s = (this[+0x368]==0); this[+0x368]=s; this[+0x360+4*s]=idx
this->vt[0xd0]("trainstat")
```
So the six train hot-spots choose the two attributes raised at the next level-up. The
port calls them `levelupstats`; per player.cpp AwardKillExp, both are raised up to
level 15 and only the first after that. The "+"/"++" next to each stat shows the
current choice.

**Page model:**
- Two **format pages**: `+0x19c` = "page1" or "page2". They are switched only by the
  invisible `page1`/`page2` hot-spots, which sit over the header text "Stats" and
  "/Skills".
- SetObject always resets to "page1".
- The current page shows in the header colour: the active word is green.
- Separately there are up to three **description pages** (`+0x1ac/+0x1b0`) for
  `multipage` fields in the inspector formats, moved by Next/Prev.
- There is no scroll bar or scroll offset.

**SetObject (slot 46, `0x5496a0`):**
```
if old: old->vt[0xa0]()
this[+0x198]=obj
free(this[+0x19c]); this[+0x19c]=strdup("page1")
this->vt[0x2c](1)
if obj: obj->vt[0x9c]()
```
For TPlayer both notifies are empty (`0x4779f0`/`0x477a00` are a bare `ret`). Callers:
- Initialize `0x546d45`;
- the interpreter `0x547660`;
- right click `0x5493d2`;
- external: `0x44f8e3`, `0x44fb74` (examined objects), `0x46e4ac`, `0x5185bf` (passes 0),
  `0x538329` (Player).

**SetFormat (slot 47, `0x488a30`):** free the old `+0x19c`, strdup(name), SetDirty(1).
It does not reset `+0x1ac`.

**Close:** see §2.

**Dirty model:** slot 20 paints only when `+0x50` is set. Setters:
- SetObject;
- SetFormat;
- Next/Prev/train clicks;
- a left-up in the pane (slot 10);
- showing the pane (`0x47d492..0x47d4a8`: `+0x48=+0x4c=0`, then slot 10);
- ClearButtons;
- **any stat change on the bound object**: TObjectInstance vt+0xd0 `0x4735c0` sets
  `[0x65b190]=1` (pane+0x50) when `[0x65b2d8]` (pane+0x198) == that object,
  `pane+0x40 != 0`, `pane+0x48 == 0` and slot 16 returns true.

There is no periodic redraw. Each repaint redraws the whole pane: Box, chrome, full DEF
walk.

---

## 7. statpane.def as loaded by retail (resources.rvr, 8679 bytes, CRLF)

The full text is the file `data/resources_unzipped/statpane.def`. It is byte-identical
to the rvr member, and line numbers are the same in both versions. Lines 1–156 are
quoted verbatim in the appendix. Lines 158–568 match the loose file printed in
CharacterStatsPane_SPEC's source, **except that lines 206–224 (Attack/Defense rows) are
active** in the rvr version.

The loose 1.2-patch `data/Resources/statpane.def` comments out lines 130–154 and
206–224 and is otherwise identical. **Retail does not load it** (§2).

### 7.1 Page 1: CLASS "Player" FORMAT "Page1" [EMU positions]

Palette (def 20–27; COLORREF in the log):
| name | RGB |
|---|---|
| default (current) | 255,240,215 |
| aqua | 113,187,255 |
| teal | 95,228,193 |
| red | 255,150,150 |
| blue | 110,160,255 |
| green | 6,221,0 |
| yellow | 255,247,73 |

Layout: lineStartX = 28 (`POS 28 62`), lineHeight = 12. All text is Arial +12 with the
3-pass shadow.

| y | def lines | items (x / align / colour / source) |
|---|---|---|
| 62 | 29–31 | "Name: " 28 L cream · `name` follows L aqua (type name via string table) |
| 74 | 33–35 | "Class: " 28 L cream · `class` L aqua (`[[P+0xfc]+0x190]`) |
| 86 | 37–43 | "Lvl: " 28 cream · `level` teal (stat 17) · " Exp: " cream · `exp` teal (18) · " Nxt: " cream · `nextexp` teal (19). All L, flowing. |
| 98 | 45–53 | "Hlth: " 28 cream · `health` red (3) · "/" red · `maxhealth` red (vt1d8) · " Mana: " cream · `mana` blue (5) · "/" blue · `maxmana` blue (vt1e8). All L, flowing. |
| 110 | 55–58 | TAB 40 → x 68: "Stats" **green** L, then "/Skills" cream L |
| 122 | 60–69 | TAB12 → x40: `trainstrn` **R** cream ("", "+", "++"; right edge 40) · TAB14 → x42: "Strn" L cream · TAB56 → x84: `strn` **R** cream (stat 34, right edge 84) · TAB70 → x98: `strnmod1` / `strnmod2` **draw nothing** |
| 134 | 71–80 | same columns: traincons · "Cons" · cons (35) · consmod1/2 nothing |
| 146 | 82–90 | trainagil · "Agil" · agil (36) · agilmod1 nothing |
| 158 | 92–100 | trainrflx · "Rflx" · rflx (37) · rflxmod1 nothing |
| 170 | 102–111 | trainmind · "Mind" · mind (38) · mindmod1/2 nothing |
| 182 | 113–121 | trainluck · "Luck" · luck (39) · luckmod1 nothing |
| 194 | 123–127 | "Armor: " 28 cream · `armor` yellow · " Dmg: " cream · `damage` yellow. All L. |
| 206 | 130–141 | LINEWRAP 150: "Atck: " 28 cream (measured 23 < 150, no wrap) · atckmod1–6 nothing · LINEWRAP none · TAB145 → x173: `attackpct` **R** cream (right edge 173) |
| 218 | 143–154 | "Def: " 28 cream · defmod1–6 nothing · TAB145 → x173: `defensepct` R cream |

Hot-spots (def 11–18, invisible, made on first walk):
| name | rect (x,y,w,h) |
|---|---|
| page1 | 63,108,25,14 |
| page2 | 89,108,29,14 |
| trainstrn … trainluck | 24, 120/132/144/156/168/180, 62, 12 |

[EMU] Sample flowing x positions with emulator metrics (Win98 may differ by ~1 px):
- "Locke" at 57;
- the Exp value at 74, " Nxt: " at 94;
- maxhealth at 63, " Mana: " at 78;
- "/Skills" at 89 (header "Stats" spans 68..89).

### 7.2 Page 2: CLASS "Player" FORMAT "Page2" (def 158–295) [EMU]

- Rows 62–98: same as Page 1.
- Header row 110: TAB40 → x68 "Stats/" cream, then "Skills" **green**.
- Hot-spots: page1/page2 only.

Skill rows, one per NEXTLINE:
- TAB10 → x38: label, L, cream;
- TAB68 → x96: `<skill>` **R** cream (right edge 96);
- TAB76 → x104: `<skill>exp` L cream, then "/" cream, then `<skill>nxtexp` L cream,
  flowing.

| y | row (stat ids) |
|---|---|
| 122 | Attack (40/51/62) |
| 134 | Defense (41/52/63) |
| 146 | Invoke (42/53/64) |
| 158 | Hands (43/54/65) |
| 170 | Sword (45/56/67) |
| 182 | Bludgeon (46/57/68) |
| 194 | Axes (47/58/69) |
| 206 | Bows (48/59/70) |
| 218 | Lockpick (50/61/72) |

### 7.3 Inspector formats (def 297–568, static only)

CLASS "Character", "Weapon", "Armor" and "Default" are picked when an examined object is
bound; "Default" matches anything.

Common pieces:
- BUTTON "Prev" (27,250,25,14) and "Next" (143,250,29,14).
- A label/value pattern: `TAB n` / `TEXT right "Label: "` / `TAB n` / `FIELD aqua value`.
  The label ends at 28+n and the value starts there.
- `ICON` at TAB 82 → x110 on line 2: the portrait, with RingT for characters or the
  (+3,+3) shadow copy otherwise.
- "Description:", then `FIELD multipage 135 {100|80|80|120} objdesc` at x38, in the
  current colour.
- `POS 27 250` / `TEXT grayable "Prev"` and `POS 143 250` / `TEXT grayable "Next"`:
  green (6,221,0) when that direction can page, grey (60,60,60) when not.

The red/blue palette here is the old one (255,43,43 / 97,110,255).

---

## 8. Retail quirks worth knowing (not bugs to reproduce unless noted)

1. The **stat-mod columns never draw** because the field names don't match the resolver
   (§5.1). This is the retail look; reproduce it in Classic.
2. Right-aligned text leaves the cursor at its left edge, and centred text leaves it
   unchanged (TA_UPDATECP). The DEF always TABs after them.
3. block/multipage FIELDs ignore the named colour and use the current colour.
4. WriteField ends with `MoveToEx(x+1,y+1)`.
5. Clamping the multipage page zeroes the page-2 pointer, so it draws nothing.
6. COLOR with a new name and no numbers copies an uninitialised colour.
7. statpane.def is never closed after Initialize.
8. Empty `train*` results still make four TextOutA calls with "". Harmless.

---

## 9. Corrections to `docs/ui/forensics/CharacterStatsPane_SPEC.md`

1. **Font (§2, U-2):** it is WINFONT **"Small" = Arial, CreateFontA(+12)**. `[0x65abc4]`
   is written at `0x485dcf` with `FontTable.Find("Small")`. It is not a gold bitmap
   font.
2. **Pane geometry (§3):** `mbr_0x4 = 452` is the pane **x**, not an unused width.
   +0x8 = y = 0. The rect is (452,0,188,306) from the ctor. The GDI viewport origin is
   the surface origin set by SetClipRect, which is the pane position.
3. **Which DEF (§0/§4/§8):** retail loads the **resources.rvr** statpane.def (8679 B),
   not `data/Resources/statpane.def`. Page 1 therefore has **Atck/Def rows at
   y=206/218**, and Page 2 has **Attack/Defense rows**.
4. **Row y-values (§3 diagram, §8):** the stat rows are at **122/134/146/158/170/182**
   and Armor at **194**. The spec's 120…180/192 are 2 px too high. 110+12 = 122.
5. **Stat-mod fields (§8):** `strnmod1` etc. **never draw**, and neither do their
   labels "Dmg"/" Hit"/….
6. **Alignment codes (§6a, §8):** `center` → `SetTextAlign(7)` (TA_CENTER|TA_UPDATECP),
   `right` → `3` (TA_RIGHT|TA_UPDATECP), none/`left` → `1`.
   - There is no "reset to TA_LEFT after each field". The default is set **before** a
     TEXT/FIELD that has no modifier (`0x5481f4`).
   - block/multipage/grayable/prev/next leave the alignment untouched.
7. **Modifiers (§6a):** at most **one** modifier keyword, followed by an optional colour
   name. It is not a list.
8. **WriteField (§6b):** `0x2810` = DT_EDITCONTROL|DT_NOPREFIX|**DT_WORDBREAK**, not
   DT_SINGLELINE.
   - The return value (height) is added to **y**, not to a width.
   - It is used only for `block`/`multipage` fields, with the current colour.
9. **Cursor advance (§6a):** it comes from the GDI current position after the coloured
   TextOutA (TA_UPDATECP): left x+w, right x−w, center x.
10. **Paint steps (§5):**
    - vt+0x90 is slot 36 "mark buttons dirty", not a pre-pass.
    - vt+0x2c is slot 11 **SetDirty(0)**, not "button compose".
    - Button drawing is `0x435cb0` (slot 35).
    - The paint's drawmode is always 0x10 (DM_BACKGROUND), from slot 20.
    - The text surface is the display (slot 20 passes `[0x5d79e0]`).
    - The mosaic is a measuring scratch surface for `multipage` only.
11. **Buttons (§4, U-4):** BUTTON hot-spots are TButtons with **flags 0x100000**. They
    are never drawn and have no art. The button ctor call is
    (name,x,y,w,h, key 0, no bitmaps, −1, 0x100000, group −1, 0).
    - They are rebuilt (after ClearButtons `0x436890`) only when the selected block
      changes.
    - Page 2 has only page1/page2.
12. **Page switching (§9, U-5):** OnControl slot 37 `0x549470`. A button named "page…"
    calls slot 47 **SetFormat(name)** (`0x488a30`); the format string is the button
    name.
13. **Train buttons (§10):** the green/grey (6,221,0)/(60,60,60) logic belongs to
    **grayable Prev/Next** (multipage paging), not to the train fields.
    - Train FIELDs print "", "+" or "++" in cream: the chosen level-up attributes,
      `player+0x360/+0x364`, the second only while Level<15.
    - A click calls `TPlayer::TrainStat` (`0x51fe50`), or prints STATMAX when the stat
      is >30.
14. **MouseClick codes (§10):** 2 = MB_RIGHTDOWN (rebind the player, page1). 4 =
    **MB_LEFTUP** (cancel drag + sound + invalidate), not left-down.
15. **SetObject (§6c):** it marks dirty through **vt+0x2c(1)** (SetDirty), not vt+0x44.
    `obj->vt[0x9c]()` takes no argument and is an empty `ret` for TPlayer.
16. **ICON (§4/§7):** RingT is drawn only for **class 0xc (CHARACTER)**. Weapons, armor
    and other non-characters get a (+3,+3) copy via `0x4bda20` (mode 0x10100, 0x1f)
    instead.
17. **CLASS matching (§6a):** the CLASS string is a **comma-separated, case-insensitive
    list** (`0x483460`). FORMAT is compared case-insensitively.
18. **LINEWRAP (§6a):** the limit is an **absolute** x (`x + width > n`). It is skipped
    for center/right text and measured with GetTextExtentExPointA.
19. **Field resolver (U-1):** resolved; see §5. The `stmod…` path exists but the DEF
    doesn't use it.
20. **Dirty sources (§9):** add the stat-changed notification (TObjectInstance vt+0xd0
    `0x4735c0`) and the pane-show path.
21. **Chrome (§2):** loaded through retail's loader, it showed as a 16-bit bitmap
    (flags 4) on the 16-bpp display [EMU]. The spec's flags 0x2 is the on-disk format.
    Mode 0x10, not "pane drawmode param_3 opaque".

---

## 10. Fixture table: what the paint reads from the bound object

(Already sent to main.) The bound object is `pane+0x198`, set only by SetObject (§6).
G_PLAYER `0x667fcc` must be set **before** Initialize and must equal the bound object
for train marks, the armor bonus and the attack/defense percentages.

| read | where | meaning | fake |
|---|---|---|---|
| `+0x48` → class | `0x54779d`, `0x47338e`, `0x473152` | +4 name ("PLAYER"); +0x1c/+0x20 OBJSTATS; +0x14/+0x18 STATS; +0x24 TYPES | `u32(0x65a148 + 4*0x0b)` |
| `+4` word | `0x4733c0`, ICON | class id (0x0b player) | 0x0b |
| `+6` word | `0x473412` | type index (STATS path only) | 0 |
| `+0x4c` → type rec | `0x472fbd`, `0x547319` | +0 name, +0x1c objdesc key | record with +0 = "Locke" |
| `+0xfc` → chardata | `0x51dfce` | [+0x190] class display string | block with +0x190 string |
| `+0x170`/`+0x180` | `0x5407d0`, `0x540820` | effect list count / data | 0 |
| `+0x360`, `+0x364`, `+0x368` | `0x5472b3`, `0x5472d6`, TrainStat | level-up choices / last slot | ints |
| vt+0xdc(i) | `0x4733ca` | GetObjStat (ids in §5.2) | stub |
| vt+0xd4(name) | `0x5472c6` ("Level"), `0x5473d6` ("type") | GetStat by name | stub |
| vt+0x1d8 / 0x1e0 / 0x1e8 | TCharacter | max health / fatigue / mana | hudscene stubs |
| vt+0x2bc, 0x260 | armor | ArmorValue, DmgResMisc | stub |
| vt+0x2b0, 0x2d0, 0x2cc, 0x3d4 | damage | (unnamed getters) | stub |
| vt+0x2c0, 0x2c4, 0x2ec | attack/defense pct | (unnamed getters) | stub |
| vt+0x9c / 0xa0 | SetObject | notify (empty for TPlayer) | real |
| vt+0xc8 | `0x547576` | GetFieldText chain | real |
| vt+0x130 | ICON only | inventory image | not needed for Player |

Globals:
| global | contents |
|---|---|
| `0x65c6f4` | StatsPane.dat |
| `0x65bc3c` | SpellIcons.dat (Init faults on null) |
| `0x65d4d0` | string table |
| `0x65b010` / `0x65abc4` | font table / "Small" font index |
| `0x65c5d0` | text bar (STATMAX) |

---

## 11. UNCONFIRMED

- **U1** Exact Win98 GDI advance widths for Arial +12. Emulator x positions for text that
  follows other text use macOS Arial hdmx. Anchors (POS/TAB/right edges) are exact.
- **U2** Which DirectDraw buffer `display->vt[0xc]()` returns, i.e. where the GDI text
  lands, compared with the DM_BACKGROUND chrome/Box. The emulator recorded one surface
  id (279092352) for all text but I did not map it to back/background.
- **U3** The archive-first load was shown in the emulator (retail boot and retail file
  code). It has not been checked in the DOSBox lab or real retail.
- **U4** Identity of `0x4bda20` (non-character ICON copy at +3,+3, mode 0x10100, arg
  0x1f) and the exact parameter meanings of `0x438d80` for the RingT blit. The ICON
  path did not run in the emulator.
- **U5** `&DAT_00670648` click sound: .bss and referenced only at `0x549412`. Presumably
  empty, so no sound.
- **U6** Names of TCharacter getters vt+0x2b0, 0x2c0, 0x2c4, 0x2cc, 0x2d0, 0x2e8, 0x2ec,
  0x3d4. The formulas are exact; the meanings are not.
- **U7** Pane `+0x40` meaning, and slot 16 (`+0x3c == [0x667fd0]`) as "on current
  screen".
- **U8** Win98 `DrawTextExA` uiLengthDrawn with DT_EDITCONTROL decides the multipage
  splits. multipage was not exercised in the emulator.
- **U9** GDI text is not clipped to the pane (the DC covers the whole surface).
  Over-long strings would spill past x=188. Inferred, not observed.
- **U10** The actual strings shown for `class` (chardata class names) and the localized
  `name` (string table) for each hero.
- **U11** Placement at resolutions other than Classic 640×480. Only the ctor's (452,0)
  is known.

---

## Appendix: statpane.def lines 1–156 (rvr version, the one retail loads)

```
     1	// ********* Revenant Stat Pane Format File ********
     2	// -------------------------------------------------
     3
     4	// Revenant - Copyright 1999 Cinematix Studios, Inc.
     5
     6	CLASS "Player" FORMAT "Page1"
     7	BEGIN
     8	  LINEHEIGHT 12
     9	  LINEWRAP none
    10
    11	  BUTTON "page1" 63 108 25 14
    12	  BUTTON "page2" 89 108 29 14
    13	  BUTTON "trainstrn" 24 120 62 12
    14	  BUTTON "traincons" 24 132 62 12
    15	  BUTTON "trainagil" 24 144 62 12
    16	  BUTTON "trainrflx" 24 156 62 12
    17	  BUTTON "trainmind" 24 168 62 12
    18	  BUTTON "trainluck" 24 180 62 12
    19
    20	  COLOR 255 240 215
    21	  COLOR aqua 113 187 255
    22	  COLOR teal 95 228 193
    23	//COLOR red 255 43 43    // old red was hard to read... bumped it up a bit
    24	  COLOR red 255 150 150
    25	  COLOR blue 110 160 255
    26	  COLOR green 6 221 0
    27	  COLOR yellow 255 247 73
    28
    29	  POS 28 62
    30	  TEXT "Name: "
    31	  FIELD aqua name
    32
    33	  NEXTLINE
    34	  TEXT "Class: "
    35	  FIELD aqua class
    36
    37	  NEXTLINE
    38	  TEXT "Lvl: "
    39	  FIELD teal level
    40	  TEXT " Exp: "
    41	  FIELD teal exp
    42	  TEXT " Nxt: "
    43	  FIELD teal nextexp
    44
    45	  NEXTLINE
    46	  TEXT "Hlth: "
    47	  FIELD red health
    48	  TEXT red "/"
    49	  FIELD red maxhealth
    50	  TEXT " Mana: "
    51	  FIELD blue mana
    52	  TEXT blue "/"
    53	  FIELD blue maxmana
    54
    55	  NEXTLINE
    56	  TAB 40
    57	  TEXT green "Stats"
    58	  TEXT "/Skills"
    59
    60	  NEXTLINE
    61	  TAB 12
    62	  FIELD right trainstrn
    63	  TAB 14
    64	  TEXT "Strn"
    65	  TAB 56
    66	  FIELD right strn
    67	  TAB 70
    68	  FIELD "Dmg" strnmod1
    69	  FIELD " Hit" strnmod2
    70
    71	  NEXTLINE
    72	  TAB 12
    73	  FIELD right traincons
    74	  TAB 14
    75	  TEXT "Cons"
    76	  TAB 56
    77	  FIELD right cons
    78	  TAB 70
    79	  FIELD "Hlth" consmod1
    80	  FIELD " Ftg" consmod2
    81
    82	  NEXTLINE
    83	  TAB 12
    84	  FIELD right trainagil
    85	  TAB 14
    86	  TEXT "Agil"
    87	  TAB 56
    88	  FIELD right agil
    89	  TAB 70
    90	  FIELD "Atk" agilmod1
    91
    92	  NEXTLINE
    93	  TAB 12
    94	  FIELD right trainrflx
    95	  TAB 14
    96	  TEXT "Rflx"
    97	  TAB 56
    98	  FIELD right rflx
    99	  TAB 70
   100	  FIELD "Def" rflxmod1
   101
   102	  NEXTLINE
   103	  TAB 12
   104	  FIELD right trainmind
   105	  TAB 14
   106	  TEXT "Mind"
   107	  TAB 56
   108	  FIELD right mind
   109	  TAB 70
   110	  FIELD "Mana " mindmod1
   111	  FIELD " Spl " mindmod2
   112
   113	  NEXTLINE
   114	  TAB 12
   115	  FIELD right trainluck
   116	  TAB 14
   117	  TEXT "Luck"
   118	  TAB 56
   119	  FIELD right luck
   120	  TAB 70
   121	  FIELD "Rolls " luckmod1
   122
   123	  NEXTLINE
   124	  TEXT "Armor: "
   125	  FIELD yellow armor
   126	  TEXT " Dmg: "
   127	  FIELD yellow damage
   128
   129
   130	  NEXTLINE
   131	  LINEWRAP 150
   132	  TEXT "Atck: "
   133	  FIELD atckmod1
   134	  FIELD atckmod2
   135	  FIELD atckmod3
   136	  FIELD atckmod4
   137	  FIELD atckmod5
   138	  FIELD atckmod6
   139	  LINEWRAP none
   140	  TAB 145
   141	  FIELD right attackpct
   142
   143	  NEXTLINE
   144	  LINEWRAP 150
   145	  TEXT "Def: "
   146	  FIELD defmod1
   147	  FIELD defmod2
   148	  FIELD defmod3
   149	  FIELD defmod4
   150	  FIELD defmod5
   151	  FIELD defmod6
   152	  LINEWRAP none
   153	  TAB 145
   154	  FIELD right defensepct
   155
   156	END
```
Lines 157–568 are identical to `data/Resources/statpane.def` lines 157–568, except that
lines 206–224 are not commented out:
```
   206	  NEXTLINE
   207	  TAB 10
   208	  TEXT "Attack"
   209	  TAB 68
   210	  FIELD right attack
   211	  TAB 76
   212	  FIELD attackexp
   213	  TEXT "/"
   214	  FIELD attacknxtexp
   215
   216	  NEXTLINE
   217	  TAB 10
   218	  TEXT "Defense"
   219	  TAB 68
   220	  FIELD right defense
   221	  TAB 76
   222	  FIELD defenseexp
   223	  TEXT "/"
   224	  FIELD defensenxtexp
```
