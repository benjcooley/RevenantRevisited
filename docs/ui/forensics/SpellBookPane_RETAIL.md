# Spell book pane (cls_0x5a5ae8): retail forensics

Subject: the side panel's spell book, class `cls_0x5a5ae8`, vtable `0x005a5ae8` (46 slots, 0..45; the entries at `0x5a5ba0`/`0x5a5ba4` = `0x546b50`/`0x546d70` belong to the NEXT class, a stat pane that loads `statpane.def` and `RingT`). Global instance `0x0065a9d8`. TButtonPane, placed by its parent at screen (452, 0), 188 x 306.

## 0. Method and evidence

- **Static.** Linear disassembly of `recon/retail_asm/baseline/Revenant.rebuilt.exe` (capstone over the PE sections) for `0x5449c0..0x546e00`, plus the helpers `0x4be2b0`, `0x4acb80`, `0x42c2d0`, `0x41c840`, `0x41cb80`, `0x429950`, `0x419dd0`, `0x444e20`, `0x438d80`, `0x43a100..0x43a420`, `0x4361b0`, `0x435cb0`, `0x435d70`, `0x445210`, and the font setup `0x485da2..0x485e8d`. Every constant below is cited by instruction address.
- **Dynamic.** An emulator probe ran retail's own Initialize, rebuild, slot 19, slot 20/paint, `0x544fb0` and the arrow callbacks in the HUD world (`tools/retail_runtime/slots/hud`, booted image; `TSpellList::Initialize 0x53ea00` called first, since boot stops before it). Fixture player: known spells `LI, L, BI, CBJ, E`; mana 40; a Spell Pouch. The committed fixture `tools/retail_runtime/slots/hud/spellbook.py` reproduces the paint (`hud_ab.py spellbook`). Tagged **[emu]** below. The emulator records GDI text calls but draws no glyphs, so text geometry, flags and colours are verified; text pixels are not.
- Helper identities come from port REVSYNC comments (`src/spell.h`, `src/dialog.h`, `src/player.cpp:1170-1260`, `src/character.h:237-246`) where noted.

### Helper glossary (as used here)

| addr | meaning | evidence |
|---|---|---|
| `0x4be2b0` | Text(this=surface; x, y, w, h, text, startLine, fontIdx, colour(by-value dword), flags, blitMode); `ret 0x28` | 0x4befde; font = arg7 `[esp+0x330]` 0x4be2e7; colour = arg8 `[esp+0x334]` 0x4be6a0; flags = arg9 `[esp+0x338]` 0x4be5c6; startLine = arg6 → `0x4aceb0` 0x4be668 |
| colour arg | dword `0x00RRGGBB`; GDI COLORREF built at 0x4be7b8..0x4be7d2 (R = byte2, G = byte1, B = byte0). `-1` = font default: `font+0x40` (or `+0x44` with flag 0x800, `+0x48` with flag 0x1000) 0x4be6a7..0x4be6e1 | [emu] arg 0xF2E7FF → SetTextColor 0xFFE7F2 |
| flags | `&1` left (DT 0x2810), else `&4` right (0x2812), else `&2` centre (0x2811) 0x4be5cd..0x4be5f1; `&0x10/0x20/0x40` single-line/vcentre/bottom 0x4be5f5..0x4be611; `&0x80` single line, y = baseline (y − font+0x5c) 0x4be617..0x4be652; `&0x100` adds DT_CALCRECT (measure only) 0x4be62c; `&0x400` 3-pass black shadow 0x4be7ac | [emu] 0x401 → DT 0x2810, 0x402 → 0x2811 |
| shadow passes | at (dx,dy), (dx+1,dy), (dx,dy+1), then colour at (0,0); (dx,dy) = (1,1) if font+0x50 ≤ 14, (2,1) if ≤ 25, else one pass at (2,2) 0x4be716..0x4be747, 0x4be83b..0x4be92c | [emu] Small (h 12): (1,1),(2,1),(1,2); SpellTitle (h 17): (2,1),(3,1),(2,2) |
| return | line count from `0x4acb80`(font, text, w, flags, startLine, 0x2710, 0) 0x4bed7c (GetTextExtentPoint32A word wrap); 1 for empty text 0x4befc1 | |
| `0x429950` | PackRGB(this=dst; a1, a2, a3): dst[2]=a1, dst[1]=a2, dst[0]=a3 → dword 0x00(a1)(a2)(a3) = R,G,B | 0x429950..0x42996a |
| `0x419dd0` / `0x444e20` | colour copy-ctor / colour-from-int ctor (by-value arg construction) | 0x419dd0, 0x444e20 |
| `0x4bd680` | TSurface::Put(x, y, bitmap, drawmode, colour) | primitives.py; [emu] |
| `0x438d80` | fills an SDrawParam: (dp, dx, dy, sx, sy, w, h, drawmode); intensity 0x1f | 0x438d80..0x438ded |
| surface vtbl | `+0x40` SetOrigin(x,y) (fields +0x14/+0x18); `+0x44` SetClipRect(x,y,w,h) (+0x20..+0x2c); `+0x48` SetClipMode (+0x1c); `+0x5c` ParamBlit(dp, src, 0, 0); `+0x64` Box; `+0x1c` SetKeyColor; key at +0x30 | save/restore at 0x545c08..0x545d6e; [emu] cs fields |
| `0x53ed70` | TSpellList::GetSpellDataByTalismans → **SSpellData** (ICONNAME at +0x5c) | spell.h:157; 0x544ff7/0x545004 |
| `0x53ef90` | TSpellList::GetVariantDataByTalismans → **SSpellVariant** (name +0x04, mana +0x4c, skilllevel +0x5c) | spell.h:166; [emu] LI → "Fire Flash", 31, 1 |
| `0x51b7c0` | TPlayer::HasTalismans(talismans): the Spell Pouch covers the code | player.cpp:1217 |
| `0x46e7f0` | DialogList.DisplayName(dst, size, name) | dialog.h:51 |
| `0x49d6d0 / 0x49d780 / 0x49d800` | DialogList (`0x0065d4d0`) FindLine / GetLine(id) / GetLine(tag) | dialog.h:44-49 |
| `0x4d5c20 / 0x4d5b90` | CastByTalismans(t,0,0,0) / CastByName("Fizzle",0,0,0) | character.h:241-244 |
| `0x54d170` on `0x0065c5d0` | TextBar.Print(fmt, ...) | player.cpp:1190-1212 (same messages) |
| `0x43a410` | → drag payload buffer `0x656398` | 0x43a410 |
| `0x43a170(s)` | copy s into the payload; if non-empty play a random "talis1..3"-style pickup sound | 0x43a170..0x43a229 |
| `0x43a240(bmp,hx,hy)` | build the drag cursor from bmp (opaque) plus overlay `[0x6563a4]` (DM_TRANSPARENT), hotspot (hx,hy); (0,0,0) clears | 0x43a240..0x43a3cd |
| `0x43a140(p)` / `0x43a100(...)` | dragged-object pointer `0x6563cc` / cursor reset | 0x43a140, 0x43a100 |
| `0x41c840` / `0x41cb80` | TPointerArray Add (append when no holes, else first hole) / RemoveAt (shift down) | 0x41c8b0..0x41c8cd, 0x41cb80 |
| fonts | `DAT_0065abc4` = FontTable.Find("Small") 0x485dcf; **`DAT_00667c2c` = FontTable.Find("SpellTitle")** 0x485e79..0x485e8d. FONT.DEF: Small = WINFONT Arial 12 COLOR 255 255 255; SpellTitle = WINFONT Times New Roman 18 COLOR 255 255 255 | [emu] font+0x50: Small 12, SpellTitle 17; +0x40 = 0xFFFFFF |

Pane fields used: `+0x17c` scroll, `+0x180` content surface, `+0x184` ScrlTop surf, `+0x188` ScrlBottom surf, `+0x18c` ScrollPaper surf, `+0x190` Scroll bitmap, `+0x194` last mouse y, `+0x198`/`+0x19c` scroll-step enables, `+0x1a0` total content height, `+0x1a4` (int16) row count, `+0x1a6` (int16) capacity, `+0x1a8` row-height table, `+0x1ac/+0x1b0` last mouse x/y, `+0x1c8` pressed row, `+0x1cc` release row, `+0x1d0` pending scroll delta. `+0x50` = dirty (global `0x65aa28` = `0x65a9d8+0x50`).

Player: known spells `TPointerArray` at `player+0x2ec` (count +0, items +4, capacity +8, growby +0xc, data +0x10 = `player+0x2fc`), entries `char*` talisman codes. Mana = `player->vtbl[+0x1d0]()`.

---

## 1. Initialize (slot 0, `0x5449e0`)

```
TButtonPane::Initialize()                                   // 0x5449fe  (0x434e40)
key = (display->BitsPerPixel() == 16) ? 0xF81F : 0x7C1F     // 0x544c40..0x544c5a

+0x184 = new TSurface(SpellScroll["ScrlTop"],    0x400)     // 0x544a28, 0x544a4c (0x4a5ca0), store 0x544a6c   32x12
+0x188 = new TSurface(SpellScroll["ScrlBottom"], 0x400)     // 0x544a97, 0x544ac1, store 0x544ad7          40x16
+0x180 = new TSurface(0x94, 0xE5, 0x400)                    // 0x544b08/0x544b0d, 0x544b22, store 0x544b38  148x229 content
+0x18c = new TSurface(SpellScroll["ScrollPaper"], 0x400)    // 0x544b63, 0x544b8d, store 0x544ba3          148x80
content+0x5c = new TSurface(0x84, 0xC8, 0x400)              // 0x544bd4/0x544bd9, 0x544bee: 132x200 text scratch
content+0x60 = 1   (content owns it)                        // 0x544c16
+0x190 = SpellScroll["Scroll"]; SetBackground(Scroll)       // 0x544c06..0x544c27 (0x435660)   188x306
content->SetClipMode(1)            // wrap mode             // 0x544c32..0x544c35 (vtbl +0x48)
content/ScrlTop/ScrlBottom/ScrollPaper ->SetKeyColor(key)   // 0x544c61, 0x544c6d, 0x544c79, 0x544c85 (vtbl +0x1c)
+0x17c = 0; +0x1cc = -1; +0x1c8 = -1; +0x1d0 = 0;           // 0x544c96..0x544ca8
+0x1a0 = 0; +0x19c = 1; +0x198 = 1                          // 0x544cae..0x544cba

// TButton ctor 0x42c2d0(name, x, y, w, h, key, func, downBmp, upBmp, a10, a11, a12, group, repeat, a15, a16, greyBmp)
up   = TButton("up",   0xA9, 0x96, 0x18, 0x18, 0, 0x5449C0, ScrlArwUD, ScrlArwUU, 0, 0, 1, -1, 0, 0, 0, ScrlArwUG)  // 0x544d08..0x544d33
AddButton(up); up.drawmode(+0xd4) |= 0x2000                // 0x544d47; 0x544d6b  -> 0x80002000
down = TButton("down", 0xA9, 0xAE, 0x18, 0x18, 0, 0x5449D0, ScrlArwDD, ScrlArwDU, 0, 0, 1, -1, 0, 0, 0, ScrlArwDG)  // 0x544dc2..0x544ded
AddButton(down); down.drawmode |= 0x2000                    // 0x544e01; 0x544e25
```

- Up button (169,150) 24x24, Down button (169,174) 24x24, both pane-local. They overhang the 188-wide pane by 5 px (169+24 = 193); [emu] they are clipped at x = 188.
- Ctor arg mapping (0x42c2d0 disasm): arg7 func non-null → flag 0x40 (0x42c2f0); arg10 → 0x80000, arg11 → 0x40000, arg12 → 0x20000 (0x42c303..0x42c338); arg14 → `+0x90`, and only `> 0` sets the repeat flag 0x80 (0x42c370..0x42c37d). **repeat = 0: the arrows do not auto-repeat.** arg8 → `+0xa0` (down), arg9 → `+0x9c` (up), arg17 → `+0xa8` (grey), `+0xd4` = 0x80000000 (0x42c39e..0x42c3e4). Arg12 = 1 matches the snapshot's `notsquare` (pixel hit test) slot; UNCONFIRMED for retail.
- Key 0: no hotkey. Callbacks: `0x5449C0` = `0x544f00(this, -1)`, `0x5449D0` = `0x544f00(this, +1)` (0x5449c0..0x5449dc).
- [emu] Arrow faces: `Put(169,150, ScrlArwUU, 0x80002000)` and `Put(169,174, ScrlArwDU, 0x80002000)`, each after restoring the pane background under the button (ParamBlit mode 0x20 from the pane's background surface).

Close (slot 1, `0x544e40`): base close (0x434f30), then deletes `+0x18c`, `+0x180` (and with it the scratch), `+0x184`, `+0x188`.

Slot 20 (`0x544eb0`): `if (+0x50) { this->paint(display 0x5d79e0, 0) /*+0x54*/; this->vtbl[+0x2c](0); } DrawButtons()` (0x435de0).

---

## 2. Paint (slot 21, `0x5452d0`): `paint(dest, mode)`

`dest` = arg1 (`[esp+0x2d0]`), `mode` = arg2 (`[esp+0x2d4]`, 0 from slot 20). All dest coordinates are pane-local. Content coordinates are window-relative: `k = scroll − rowTop`, and a row element at row-local y lands at content y `= local − k`.

```
+0x198 = 1; +0x19c = 1                                       // 0x5452f1, 0x5452f7
dest.Put(0, 0, Scroll(+0x190), mode, 0)                      // 0x5452ea..0x54530a   chrome 188x306
cs = +0x180
cs.Box(0, 0, 0x94, 0xE5, cs.key(+0x30), 0xFFFF, 0x7F7F, 0x80000000)   // 0x54530f..0x545336  clear to 0xF81F

// parchment, scroll-aligned
y0 = (scroll >= 0) ? -(scroll % 80) : (-80 - (scroll % 80))  // 0x54533f..0x545358 (0x50 at 0x54533f)
for (y = y0; y < 0x140; y += 0x50)                           // 0x54535a, 0x5453a9, 0x5453ac
    dp = MakeDP(dx 0, dy y, sx 0, sy 0, w paper.w, h paper.h, mode 0x102)   // 0x545372..0x54538c
    cs.ParamBlit(&dp, ScrollPaper(+0x18c))                   // 0x5453a6
this->vtbl[+0x90]()   // 0x4361b0: every button flags |= 0x20 (redraw)   // 0x5453b8

if (!player || player.known.count == 0) goto composite        // 0x5453be..0x5453d8
cur = known[0] (copied into a 12-byte local)                  // 0x5453f8..0x54543e
nTal = strlen(cur)                                            // 0x545442..0x545456
spell   = nTal ? GetSpellDataByTalismans(cur)   : 0           // 0x54546a  -> [esp+0x44]
variant = nTal ? GetVariantDataByTalismans(cur) : 0           // 0x54547d  -> [esp+0x3c]
top = 0; bottom = heights[0]; descH = heights[0] - 0x5B       // 0x545486..0x54549b

for i in 0 .. count-1:
  if (bottom > scroll && top < scroll + 0xE5):                // 0x5454b5, 0x5454c7
    k    = scroll - top                                       // 0x5454cf..0x5454d3
    over = bottom - scroll - 0xE5                             // 0x5454d5..0x5454de

    // (a) DESCRIPTION: dialog line "SPELL<talismans>"
    if (k < descH + 0x2C && k + 0xE5 > 0x2C):                 // 0x5454db..0x5454fd
        txt = DialogList.GetLine(sprintf("SPELL%s", cur))     // 0x54550f (fmt 0x5e52e4), 0x54555f
        cs.Text(0x0A, 0x2C - k, 0x84, descH, txt, 0, Small, RGB(0xF2,0xE7,0xFF), 0x401, 0x80000000)
                                                              // 0x545524..0x54557b
    // (b) NAME + ICON + RING
    if (k < 0x32):                                            // 0x545584
        name = DisplayName(variant->name (+0x04), 100)        // 0x54558d..0x54559f
        sp = strchr(name, ' ')                                // 0x5455ab..0x5455ae
        if (sp):
            part2 = sp + 1                                    // 0x5455c0..0x5455dc
            part1 = name[0 .. strlen(name) - strlen(part2) - 1)   // 0x5455e1..0x545617
            if strlen(part1) > 10: part1 = part1[0..8] + ".."     // 0x545614..0x54562f
            if strlen(part2) > 10: part2 = part2[0..8] + ".."     // 0x545637..0x54564d
            cs.Text(0x36, 4 - k,    0x55, 0x56, part1, 0, SpellTitle, -1, 0x402, 0x80000000)  // 0x545655..0x54568c
            cs.Text(0x36, 0x10 - k, 0x55, 0x56, part2, 0, SpellTitle, -1, 0x402, 0x80000000)  // 0x545691..0x5456bc -> 0x545753
        else:
            if strlen(name) > 10: name = name[0..8] + ".."    // 0x5456c1..0x545714
            cs.Text(0x36, 0x0A - k, 0x55, 0x56, name, 0, SpellTitle, -1, 0x402, 0x80000000)   // 0x54571c..0x545753
        if (spell):                                           // 0x545758
            icon = SpellIcons.Find(spell->iconname (+0x5C))   // 0x545764..0x54576e (0x46d6b0, no error)
                   ?: SpellIcons["Aura"]                      // 0x545779..0x545784 (0x5e52ec)
            if (!icon) skip
            d = (HasTalismans(known[i]) && +0x1C8 == i) ? -2 : 0   // 0x5457a7..0x5457d1
            icon.key(+0x18) = key (0xF81F / 0x7C1F)           // 0x5457d5..0x5457fe
            cs.Put(0x0C + d, d - k, icon, 0x100, 0)           // 0x5457eb..0x545818  DM_TRANSPARENT
            ring = "RingG"                                    // 0x5458a6 (0x5e537c)
            if (known[i] && *known[i] && (v = GetVariantDataByTalismans(known[i]))   // 0x54584c
                && HasTalismans(known[i])                     // 0x545862
                && v->mana (+0x4C) <= player->Mana())         // 0x545873..0x54587c (jg -> RingG)
                ring = (+0x1C8 == i) ? "RingD" : "RingU"      // 0x545882..0x545899 (0x5e5384 / 0x5e538c)
            cs.Put(0x0C + d, d - k, SpellIcons[ring], 0x2000, 0)   // 0x5458ab..0x5458c7  DM_ALPHA, 48x48

    // (c) TALISMAN GLYPH ROW
    if (over < 0x1E):                                         // 0x5458d4
        x = 0x0C                                              // 0x5458ed
        for j in 0 .. nTal-1:                                 // 0x5459d4..0x5459ee
            for t in 0 .. TalismanClass.NumTypes(DAT_0066deec)-1:
                code = TalismanClass(0x66dedc).GetStat(t, "Code")      // 0x545904..0x54594b
                if toupper(code) == toupper(cur[j]):          // 0x54594f..0x54596a (0x58b266 = toupper)
                    sprintf(tmp, "%scandy", TalName[t] @0x5e4f80)      // 0x54597c..0x54598e  DEAD: result unused
                    g = SpellScroll[GlyphName[t] @0x5e4fb4]   // 0x54599a..0x5459b0
                    cs.Put(x, descH + 0x3D - k, g, mode | 0x2000, 0)   // 0x5459aa..0x5459cf
                    break
            x += 0x10                                         // 0x5459e1

    // (d) SKILL / MANA
    if (over < 0x2F && (v = GetVariantDataByTalismans(cur))): // 0x5459fc, 0x545a0f
        y = descH + 0x2C - k                                  // 0x545a49..0x545a6a
        cs.Text(0x0A, y, 0x84, descH, sprintf("%s: %d", GetLine("SPANESKILLS"), v->skilllevel(+0x5C)),
                0, Small, RGB(0x05,0xF4,0xF4), 0x401, 0x80000000)        // 0x545a20..0x545a99
        cs.Text(0x5A, y, 0x84, descH, sprintf("%s: %d", GetLine("SPANEMANA"), v->mana(+0x4C)),
                0, Small, RGB(0xE9,0xFF,0x25), 0x401, 0x80000000)        // 0x545a9e..0x545b15

  // next row
  top = bottom                                                // 0x545b21..0x545b29
  i++; if (known.data && i < count && known[i]) cur = known[i]   // 0x545b33..0x545b96  (else cur keeps the previous code)
  nTal = strlen(cur); spell/variant re-looked-up if nTal > 0  // 0x545b9a..0x545bda
  descH = heights[i] - 0x5B; bottom += heights[i]             // 0x545bde..0x545bfe

composite:                                                    // 0x545c08
  save cs origin/clipmode/cliprect (+0x14..+0x2c)             // 0x545c18..0x545c5e
  cs.SetClipRect(cs.originx, cs.originy, 0x94, 0xE5)          // 0x545c0e..0x545c68
  dest.ParamBlit(dp{dx 0x12, dy 0x2F, w 148, h 229, src 0,0, mode = mode|0x10000104}, cs)  // 0x545c96, 0x545cbd, 0x545cc8, 0x545d1b
  cs.SetClipMode(saved); cs.SetOrigin(saved); cs.SetClipRect(saved)   // 0x545d30, 0x545d47, 0x545d6e
  dest.ParamBlit(dp{dx 0x4F, dy 0x2D, w/h ScrlTop (32x12),    mode = mode|0x10000100}, ScrlTop)     // 0x545d9c, 0x545dc3, 0x545dce, 0x545e21
  dest.ParamBlit(dp{dx 0x4B, dy 0x105, w/h ScrlBottom (40x16), same mode}, ScrlBottom)              // 0x545e69, 0x545e74, 0x545ec7
  if (dest != display) 0x435cb0(dest)  else 0x4aacb0(x, y, w, h, 6)   // 0x545ed0..0x545efb
```

Draw order: Scroll chrome; content cleared to key; paper tiles; per visible row: description, name (1 or 2 parts), icon, ring, glyphs, Skill, Mana; then content → pane at (18, 47) (key pixels transparent, so the paper's ragged key-coloured edges show the Scroll stone); then ScrlTop at (79, 45) and ScrlBottom at (75, 261), both keyed, over the content edges; then the arrow buttons (slot 20 → 0x435de0).

**[emu] confirmation** (scroll 0, rows LI/L): Put Scroll (0,0) mode 0; Box content (0,0,148,229) colour 0xF81F; paper at y 0/80/160/240 mode 0x102; `Text(10,44,132,60, "Calling forth…", font 6 Small, 0xF2E7FF, 0x401)`; `Text(54,4,85,86,"Fire", font 11 SpellTitle, -1, 0x402)`; `Text(54,16,…,"Flash")`; `Put(12,0, Fire Flash, 0x100)`; `Put(12,0, RingG, 0x2000)`; glyphs `Put(12,121, SMoon)`, `Put(28,121, SSky)` mode 0x2000; `Text(10,104,…,"Skill: 1", 0x05F4F4)`; `Text(90,104,…,"Mana: 31", 0xE9FF25)`; row 1 `Text(10,195,132,84,…)`, `Text(54,161,…,"Poison")`, `Put(12,151, Poison)`; content blit dst (18,47,148,229) mode 0x10000104; ScrlTop (79,45,32,12); ScrlBottom (75,261,40,16) mode 0x10000100. GDI: description/stats DT 0x2810 Arial 12, names DT 0x2811 Times New Roman 18 white. Pressed castable row (Might at scroll 598, `+0x1c8 = 4`): `Put(10,52, Might, 0x100)`, `Put(10,52, RingD, 0x2000)`, name at y 64.

Text calls summary (content-local; `k = scroll − rowTop`, `descH = rowHeight − 91`):

| text | x | y | w | h | font | colour | flags |
|---|---|---|---|---|---|---|---|
| description `GetLine("SPELL"+code)` | 10 | 44 − k | 132 | descH | Small | 0xF2E7FF = RGB(242,231,255) | 0x401 shadow, left, word-wrap |
| name part 1 (two-part name) | 54 | 4 − k | 85 | 86 | SpellTitle | −1 → font COLOR (white) | 0x402 shadow, centre |
| name part 2 | 54 | 16 − k | 85 | 86 | SpellTitle | −1 | 0x402 |
| name (one word) | 54 | 10 − k | 85 | 86 | SpellTitle | −1 | 0x402 |
| `"Skill: %d"` (variant skilllevel) | 10 | descH + 44 − k | 132 | descH | Small | 0x05F4F4 = RGB(5,244,244) | 0x401 |
| `"Mana: %d"` (variant mana cost) | 90 | descH + 44 − k | 132 | descH | Small | 0xE9FF25 = RGB(233,255,37) | 0x401 |

Labels: english.def `SPANESKILLS "Skill"`, `SPANEMANA "Mana"` (english.def:2080-2081). Format `"%s: %d"` at 0x5e5310 / 0x5e5324.

---

## 3. Row layout, rebuild, scroll model

### Row height (rebuild `0x546830`)

```
free(+0x1a8); +0x1a8 = 0; +0x1a4 = 0; +0x1a6 = 0              // 0x54683b..0x546871
font = Fonts[DAT_0065abc4] ?: DAT_0065b024                    // 0x546856..0x546888
if (!player) return                                           // 0x54688c
total = 0; lastSpell = 0
for i = 0; i < player.known.count; i++:                       // 0x5468a0, 0x546a42..0x546a4d
    t = known[i]
    if (t && *t) lastSpell = GetSpellDataByTalismans(t)       // 0x5468bb..0x5468de  (empty code keeps the previous result)
    if (lastSpell):
        lines = cs.Text(0, 0, 0x84, 1, DialogList.GetLine(sprintf("SPELL%s", t)),
                        0, Small, -1, 0x501, 0x80000000)      // 0x5468ee..0x54693f   measure only (DT_CALCRECT)
        h = lines * font->height(+0x50) + 0x5B                // 0x546944..0x546955
        append h to +0x1a8 (grow by 0x40 entries)             // 0x546958..0x546a08 (0x40 at 0x546968)
        total += h                                            // 0x546a0f
    else:
        free(known[i]); known.RemoveAt(i); i--                // 0x546a17..0x546a3b
+0x1a0 = total                                                // 0x546a53..0x546a57
```

- Row height = `lines × lineHeight(Small) + 91`, where `lines` = the word-wrap count (`0x4acb80`) of the description at 132 px in "Small". [emu] lineHeight (Small font+0x50) = 12; heights LI 151 (5 lines), L 175 (7), BI 175, CBJ 151, E 175; total 827.
- Row anatomy: 0..44 icon (40) + name beside it; description at 44, height descH; Skill/Mana at descH + 44; glyph row at descH + 61 (20 tall); 10 px to the next row (descH + 91).
- Runs from: `TPlayerManager::SetMainPlayerNum` (call at 0x51f048, after the player switch; covers game load) and `0x544fb0` after it adds a new spell (call 0x545147). It does not reset the scroll and does not invalidate.
- Rows dropped: entries whose code resolves to no SPELL are deleted from the player's list by the rebuild. In the paint, rows are skipped when `bottom <= scroll` or `top >= scroll + 229` (0x5454b5/0x5454c7). Inside a visible row, each block has its own gate: description (0x5454e4/0x5454fa), name+icon+ring only while `k < 50` (0x545584), glyphs while `bottom − 30 < scroll + 229` (0x5458d4), Skill/Mana while `bottom − 47 < scroll + 229` (0x5459fc).
- The description `h` is not clipped. It is that row's own `lines × 12`. Row 1's y is `rowHeight[0] + 44`.

### Scroll model

- `+0x17c` scroll = content y of the window top, in pixels, 0 = first row at the top. Window = 229 px (`0xE5`).
- Clamp, every step in slot 19: first `scroll = min(scroll, total − 229)` (0x5466c4..0x5466d4, `add edx,0xffffff1b`), then `scroll = max(scroll, 0)` (0x5466e0..0x5466e4). With fewer than 229 px of content it stays 0.
- `+0x1d0` = pending delta in pixels. Slot 19 (`0x546620`), only when `+0x198 && +0x19c` (0x546628..0x54663e):
  ```
  d = +0x1d0
  if |d| >= 0x28: step = sign(d)*0x28; +0x1d0 = d - step     // 0x546652..0x546674
  else: step = d; +0x1d0 = 0; if d == 0 -> done              // 0x54667b..0x546687 (remainder applied whole)
  old = scroll; scroll = clamp(scroll + step)
  if scroll != old:
      cs.SetOrigin(0, scroll)                                 // 0x5466fc..0x546701
      cs.SetClipRect(newly exposed strip)  (0x4ad610 / 0x4ad6a0 rect difference) // 0x546704..0x5467a2
      dirty(0x65aa28) = 1; RedrawButtons(+0x90); +0x198 = 0   // 0x5467a5..0x5467bf
  TButtonPane::Animate()  (0x435d70)                          // 0x5467cb
  ```
  So the scroll moves at most 40 px per paint cycle: paint sets `+0x198 = 1`, a step that moves clears it. [emu] delta 130 → 0→40→80→120→130. A delta that overshoots the clamp is still consumed 40 per tick without movement ([emu] delta 2000: scroll stops at 598, `+0x1d0` keeps counting down).
- Arrows (`0x544f00`, dir −1/+1): find the first row `i` whose top or top+40 lies in `[scroll, scroll+229]` (inclusive; 0x544f3b..0x544f58). Down: `+0x1d0 = top[i+1] − scroll` (0x544f80..0x544f89). Up: `+0x1d0 = top[i−1] − scroll` (top[−1] = 0) (0x544f96..0x544f9c). This SETS the delta. One row per click, no auto-repeat. [emu] Down at scroll 0 → 151; at 160 → 166; Up at 160 → −160.
- Drag-scroll: see §5. It ADDS to `+0x1d0`.
- `+0x19c` is cleared by `0x544fb0` after a visible add (0x545273), which defers the next step until a paint.

### Retail repaint region (ring buffer). Do not reproduce.

The content surface is in wrap mode (`SetClipMode(1)`, 0x544c35). Its origin tracks the scroll (`SetOrigin(0, scroll)`, 0x546701), and its clip rect (content coordinates) decides what a paint actually updates:
- slot 19 sets the clip to the strip just exposed by a step ([emu] 0→20: clip (0,229,148,20); 120→130: (0,349,148,10));
- a full window `SetClipRect(origin, 148, 229)` is set only by a LEFTDOWN icon hit (0x545ff9), the LEFTUP cleanup (0x546333), a drag start (0x546501), and `0x544fb0`'s visible add (0x545255); the paint restores the saved clip after its blit (0x545d6e);
- showing the book (TPlayScreen 0x47d387 / 0x47f12f → slot 10 `0x445210` = dirty + RedrawButtons) and every other repaint do not reset the clip.

So retail repaints only the last clip region and keeps stale pixels elsewhere. [emu] `i_stale`: after a 20 px scroll, row 1 was changed and repainted. The draw call for the new icon ran at (12,131), but the screen kept the old icon until a full invalidate (`j_full`). The visible output otherwise equals a full redraw in window coordinates, and wrapped writes are clipped away. **Port: redraw the full 148 x 229 window every paint.** The ring-buffer and strip-clip need no port.

---

## 4. Order of the known spells

The paint does **not** reorder. It walks `player+0x2fc[0 .. count−1]` in array order (0x5453f8, 0x545b33..0x545b96). There is no compare or Add in `0x5452d0`. The Ghidra Draw lines ~74-128 are only the strncpy of `known[0]` into the local code buffer.
- Order = learn order. `0x544fb0` appends with TPointerArray::Add (`0x41c840`, 0x545140), which appends when the array has no holes and fills the first null slot otherwise.
- The rebuild deletes unresolvable entries and shifts the rest down (`0x41cb80`, 0x546a36).
- No sort anywhere in the pane.

---

## 5. Per-tick, input, find/learn

### Slot 19 `0x546620`: see §3. It ends with TButtonPane::Animate (button repeat and animation).

### Slot 25 MouseClick `0x545f10(type, x, y)` (pane-local x, y; MB codes per revdefs.h: 1 = LEFTDOWN, 4 = LEFTUP)

```
if (!DAT_0065d0d0) return                                     // 0x545f11..0x545f1e
+0x1ac = x; +0x1b0 = y                                        // 0x545f33, 0x545f39
TButtonPane::MouseClick(type, x, y)  (0x436530)               // 0x545f3f   (arrows)
inside = 0<=x<w(+0xc) && 0<=y<h(+0x10)

IconHit(i):  top_i < scroll + y - 0x2F < bottom_i             // 0x545fa2..0x545fac
          && 0x0C < x - 0x12 < 0x34                           // 0x545fae..0x545fbd  (pane x 31..69)
          && top_i - scroll < y - 0x2F < top_i - scroll + 0x28 // 0x545fbf..0x545fcd (icon 40 tall)

type == 1 && inside:                                          // 0x545f44..0x545f69
    for each i with IconHit(i):
        +0x1c8 = i; cs.SetClipRect(origin,148,229); dirty = 1; RedrawButtons   // 0x545fd5..0x546010
type == 4 && inside:                                          // 0x546041..0x546066
    p = payload (0x43a410)
    if (*p):                                                  // something dropped on the book
        if GetSpellDataByTalismans(p) && p == known[j] (strcmp): +0x1d0 = top_j - scroll   // 0x546091..0x546124
    else if (+0x1c8 >= 0):
        +0x1cc = last i with IconHit(i)                       // 0x546147..0x5461a6
        if (+0x1cc == +0x1c8):                                // released on the pressed icon -> CAST
            t = known[+0x1c8]
            G = (DAT_0066829c == 0 || DAT_0067682c != 0)      // not a network client (port: script.cpp:830, player.cpp:1170)
            if (!t || !*t):            if G: CastByName("Fizzle")                       // 0x5462d4..0x5462f7
            elif !HasTalismans(t):     if G: Print(GetLine("SPLMISTAL")); CastByName("Fizzle")  // 0x546293..0x5462f7
            elif !CastByTalismans(t,0,0,0): if G: CastByName("Fizzle"); Print("SPLCASTFAIL")      // 0x546202..0x546256
            else:                      if G: Print(GetLine("SPLCASTOK"))                // 0x54625e..0x546289
    // cleanup, every LEFTUP inside the pane                  // 0x5462fc
    if (+0x1c8 != -1): +0x1c8 = -1; cs.SetClipRect(origin,148,229); dirty = 1; RedrawButtons
    +0x1cc = -1; cursor(0,0,0) (0x43a240); 0x43a100(0,0,0); payload = "" (0x43a170(0x670644)); draggedObj = 0 (0x43a140(0))
```

- Selection is only visible while the button is held (`+0x1c8` = pressed row): icon shifted (−2,−2) if HasTalismans, RingD if also mana ≥ cost. A plain click casts the spell, with no target.
- Right button (2/5), middle, double-click: nothing spellbook-specific. They are only forwarded to the button pump.

### Slot 26 MouseMove `0x546390(buttons, x, y)`

```
+0x1ac = x; +0x1b0 = y; TButtonPane::MouseMove(...)  (0x436660)     // 0x5463a5..0x5463b1
innerArea = inside && 0x12 < x < 0xA6 && 0x12 < y < 0x114            // 0x5463b6..0x5463e0
if innerArea && buttons == 1 && +0x1c8 == -1:                        // DRAG-SCROLL  0x5463e2..0x5463f2
    +0x1d0 += (+0x194 - y); +0x194 = y; return                       // 0x5463f4..0x546414 (content follows the pointer)
if buttons == 1 && +0x1c8 >= 0 && *payload == 0:                     // DRAG START  0x546417..0x54643f
    t = known[+0x1c8]; if t && *t && (s = GetSpellDataByTalismans(t)):   // 0x546445..0x546483
        draggedObj = 0                                               // 0x54648b
        cursor(SpellIcons.Find(s->iconname) ?: Find("Aura"), 0x14, 0x14) // 0x546490..0x5464bb  hotspot (20,20)
        payload = t   (+ pickup sound)                               // 0x5464c1
        +0x1c8 = -1; cs.SetClipRect(origin,148,229); dirty = 1; RedrawButtons   // 0x5464cf..0x546518
+0x194 = y                                                           // 0x54651e
```

- The drag starts on the first move with the left button held after an icon press. There is no distance threshold, and neither HasTalismans nor mana is needed.
- What is carried: the talisman code string in the global payload `0x656398`, and a cursor made of the spell's 40x40 icon composited with the overlay `[0x6563a4]`, hotspot (20,20). The drop onto the quick-spell rings is handled by the bottom bar, which reads the payload. A LEFTUP back over the book scrolls to that spell (above).
- `buttons == 1` is an exact compare (0x5463e2, 0x546417). Per the port's `mousebutton` convention, left held alone.

### `0x544fb0(talismans)`: learn-or-reveal (called after the spell pane casts: 0x542da3/0x542dbb/0x542df9/0x542e11; and by an all-spells loop at 0x54dac4)

```
if (!t || !*t) return; s = GetSpellDataByTalismans(t); if (!s || !s->iconname[0]) return     // 0x544fd3..0x545009
id = DialogList.FindLine("SPELL"+t); if id < 0 return; txt = GetLine(id); if !txt return      // 0x54500f..0x545048
rowH = cs.Text(0,0,0x84,1, txt, 0, Small, -1, 0x501, 0x80000000) * Small.height + 0x5B       // 0x54504e..0x545086
top = 0
for i: if strcmp(known[i], t) == 0:                           // 0x5450b2..0x5450e8 (case-sensitive)
          +0x1d0 = top - scroll; dirty = 1; RedrawButtons; return     // 0x545287..0x5452bb
       top += heights[i]
// not known: learn it
known.Add(strdup6(t))   (malloc 6)                            // 0x545106..0x545140
rebuild()  (0x546830)                                         // 0x545147
+0x1d0 = top - scroll; +0x1a0 = top + rowH                    // 0x54514c..0x545171
append rowH to the height table AGAIN                         // 0x545177..0x54521d  (duplicate entry; harmless)
if (top < scroll + 0xE5): cs.SetClipRect(origin,148,229); dirty = 1; RedrawButtons; +0x19c = 0   // 0x545217..0x545273
else: this->vtbl[+0x28]()   (dirty + RedrawButtons)           // 0x5452be..0x5452c2
```

[emu] find "E" → `+0x1d0` = 652; add "JI" → list gains JI, count/table 7 entries for 6 spells, total 1002, `+0x1d0` 827; unknown "QQ" → no change. Spells without an ICONNAME or without a `SPELL<code>` dialog line are never added.

Unreferenced helpers in the class range (no callers, no address refs): `0x546530` (ring bitmap for row: the same rule as the paint), `0x5465e0` (row state: 1 = no talismans, 2 = pressed, 3 = normal), `0x5467e0` (full-window invalidate), `0x544ee0` (`+0x1d0 += arg`).

---

## 6. Player-state dependencies

- **Ring** (0x545846..0x5458a6): RingU = HasTalismans(code) && variant.mana ≤ player.Mana() (vtbl +0x1d0); RingD = the same && `+0x1c8 == i`; RingG = otherwise (talismans missing, mana short, or the code has no variant). RingT is not used by this pane. Rings are 48x48 (SpellIcons.dat), drawn at the icon's top-left with DM_ALPHA.
- **Icon offset**: (−2,−2) when HasTalismans && `+0x1c8 == i`. Mana is not checked here (0x5457a7..0x5457c7).
- **Icon**: SpellIcons.dat (`0x65bc3c`) entry named by the SPELL's ICONNAME (`SSpellData+0x5c`), else "Aura". 40x40, DM_TRANSPARENT, key forced to magenta.
- **Text**: no player dependency. "Skill: N" is the variant's `skilllevel` (+0x5c, the invoke skill needed), not the player's skill. "Mana: N" is the variant's cost (+0x4c). Both colours are constant.
- **Casting** on click: HasTalismans, CastByTalismans, messages (§5).
- Retail only refreshes rings in the current clip region (§3). Port: full redraw, so ring state follows mana and talismans every paint.

---

## 7. Answers index (coordinator's 9 questions)

1. Row pitch = `lines(Small, 132 px) × 12 + 91`. Scroll `+0x17c` in px, clamp `[0, max(0,total−229)]`, animated 40 px per paint cycle from delta `+0x1d0`. Arrows step one row (to the next or previous row top) and do not repeat. §3.
2. Description h = that row's own `lines × lineHeight`, not clipped. Skipped rows and per-block gates are in §3.
3. Colours are constant: Skill RGB(5,244,244), Mana RGB(233,255,37), description RGB(242,231,255). "Skill:" = variant skilllevel. Insufficiency shows only in the ring. §2, §6.
4. RingG/U/D rule in §6; RingT unused; icon by ICONNAME, fallback "Aura".
5. Name = DisplayName(variant name), split at the FIRST space: part 1 = first word, part 2 = the rest. Any part over 10 chars becomes 8 chars + "..". Drawn in SpellTitle, centred, at y 4/16; a one-word name at y 10. §2.
6. Glyphs `S<Talisman>` 20x20 from SpellScroll.dat, x = 12 + 16·j, y = descH + 61, mode|DM_ALPHA. Glyph index = TalismanClass type whose "Code" matches (case-blind); name table `0x5e4fb4`. [emu] L→SMoon, I→SSky, C→SOcean, B→SLife, J→SEarth, E→SSoul.
7. §5: press on icon → `+0x1c8`; release on the same icon → cast; move with left held → drag (payload = code, cursor = icon, hotspot 20,20); drag on paper → scroll; drop on the book → scroll to that spell; right-click nothing.
8. Paper, ScrlTop and ScrlBottom are SpellScroll.dat (`0x666640`) bitmaps converted to TSurfaces at Initialize (`0x4a5ca0(bmp,0x400)`), keyed 0xF81F/0x7C1F. Scroll is drawn as a bitmap Put.
9. Rebuild builds the row-height table and total, and prunes unresolvable codes. It runs from SetMainPlayerNum (0x51f048) and after a learn in `0x544fb0` (0x545147). Learning itself = `0x544fb0`.

---

## 8. Corrections to docs/ui/forensics/SpellbookPane_SPEC.md

1. Content origin is pane (18, 47) (0x545cbd/0x545cc8), not (20, 38). It is a code immediate in the paint, not a surface field.
2. ScrlTop and ScrlBottom are blitted to the pane at (79, 45) and (75, 261) (0x545dc3/0x545dce, 0x545e69/0x545e74), not into the content surface.
3. The Scroll chrome is drawn by the paint itself (first Put, 0x54530a, mode = paint arg). The opening "stamp ScrollPaper once + Box" is really "Put Scroll to the pane + clear content to its key".
4. Text roles were swapped. `"SPELL%s"` is the DESCRIPTION (Small, (10, 44, 132, descH)). The spell NAME is the variant name split in two, drawn in SpellTitle at (54, 4/16 or 10, 85, 86), centred.
5. `DAT_00667c2c` is a FONT (FontTable "SpellTitle", Times New Roman 18), passed as the font argument. It is not a "theme default colour". The name's colour is −1 = the font's COLOR (white).
6. Argument order of `0x4be2b0` is (x, y, w, h, text, startLine, font, colour, flags, mode). The shadow bit 0x400 and the alignment live in the flags argument, not the font id. 0x402 = centred (not left). 0x100 = measure (DT_CALCRECT). The shadow is 3 passes at (dx,dy),(dx+1,dy),(dx,dy+1) with (dx,dy) = (1,1) for Small and (2,1) for SpellTitle.
7. Colours were byte-reversed and the Skill/Mana hues swapped. PackRGB(a1,a2,a3) = R,G,B. Description RGB(242,231,255), Skill RGB(5,244,244) cyan, Mana RGB(233,255,37) yellow-green.
8. Skill/Mana y = rowTop + descH + 44 with h = descH; glyph y = rowTop + descH + 61. They are not at fixed +44 / +61. The name sits at x 54, not 10.
9. Icon at (12,0), DM_TRANSPARENT 0x100, key forced. The ring (RingU/D/G, 48x48, DM_ALPHA) at the same point, with a (−2,−2) pressed shift. The old spec had no ring logic.
10. `0x53ed70` → SSpellData (ICONNAME), `0x53ef90` → SSpellVariant (name, mana +0x4c, skill +0x5c). The task brief had these reversed.
11. Slot 19: the sub-40 remainder is applied in one step, not snapped to 0. Steps are gated per paint (`+0x198` cleared on a move, set by the paint). Clamp is max first, then 0.
12. Slot 26 is drag-scroll and drag-start, not "selection visual". Click is cast-on-release (slot 25 LEFTUP), not "pick up". The drag cursor is the 40x40 icon with hotspot (20,20); `0x14,0x14` is the hotspot, not a 20x20 cursor.
13. `0x544fb0` is learn-or-reveal (adds an unknown spell, then scrolls to it), not just find.
14. Arrows: no auto-repeat (repeat 0), draw mode 0x80002000, one row per click via `0x544f00`. "40 px/tick" is the delta animation. The rect overhangs the pane by 5 px.
15. Paper tiles use drawmode 0x102. Tiling start `−(scroll mod⁺ 80)`, loop while `y < 320`.
16. No sorting; order = learn order.
17. Hit-test icon box = content x (12,52), y (0,40) exclusive, i.e. pane x 31..69, `0x2F` y offset.
18. UNCONFIRMED-C resolved: icons are SpellIcons.dat 40x40 (global `0x65bc3c`). UNCONFIRMED-D resolved: labels "Skill"/"Mana" (english.def:2080-2081). UNCONFIRMED-E resolved: `"%scandy"` is a dead sprintf.
19. The 46-slot vtable ends at slot 45. `0x546b50`/`0x546d70` (statpane.def, RingT) belong to the next class.

---

## 9. Retail bugs and quirks (do not reproduce unless noted)

- Repaint limited to the content clip region: stale rings and rows after any repaint that did not reset the clip (§3). Port: full redraw.
- `0x544fb0` appends a duplicate height entry after its rebuild. Harmless: the loops stop at the player count.
- An overshooting scroll delta is consumed silently. A later drag adds to the leftover. Arrows overwrite it.
- An empty or NULL code in the known list reuses the previous row's code and lookups in both the paint and the rebuild.
- `strdup` with a fixed 6-byte buffer (0x545106): codes over 5 chars would overflow. Spell codes are at most 5.
- Two-part name lines are 12 px apart in a 17 px font (lines overlap). This is faithful layout, not a bug.

## 10. UNCONFIRMED

1. Real Win98 metrics: Small line height 12, SpellTitle 17 and the wrap line counts are emulator values (TrueType-table metrics, `gdi.py`). Row heights depend on them.
2. Win98 DrawTextA with DT_WORDBREAK|DT_EDITCONTROL on a single word wider than the 85 px name cell (break or clip?).
3. The cursor overlay bitmap `[0x6563a4]` composited by `0x43a240` (identity unknown).
4. `DAT_0066829c` / `DAT_0067682c` = network-game / host flags (taken from port comments, not traced).
5. The source of MouseMove's `buttons` value (assumed: left held = 1, as the port's `mousebutton`).
6. TButton ctor args 10-12 semantics (radial/toggle/notsquare by snapshot analogy); arg12 = 1 sets flag 0x20000.
7. The pickup sound name built in `0x43a170` ("talis"+digit; exact string not decoded).
8. Pane screen position (452, 0) is the caller's placement (task statement / fixture), not traced in TSidePane here.
