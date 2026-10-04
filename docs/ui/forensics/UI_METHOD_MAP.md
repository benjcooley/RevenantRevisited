# UI Method Map — Common Retail Draw / Blit / Text / Effect Primitives

**Status:** forensic reference. This is **mandatory pre-reading** for every per-panel tracer agent. It catalogs the shared primitive functions that all UI paint methods call so that panel tracers convert decomp → spec mechanically instead of re-deriving primitive behavior each time.

**This document describes behavior only — it modifies no implementation code.**

**Accuracy protocol (read `docs/ui/AGENT_PROTOCOL.md` first).** Per the cost-asymmetry rule, a wrong behavior description here propagates into every panel spec. When evidence is weak I write **UNCONFIRMED** and state what would resolve it. Every claim cites `recon-file:line` plus the literal hex/decimal it came from. Numbers are shown as `0xNN = DD` (hex = decimal).

All recon paths are relative to the repo root `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`.

---

## 0. Index table

| Symbol (addr) | Purpose | Port primitive | Confidence |
|---|---|---|---|
| `FUN_00414d70` (0x414d70) | TBitmap/surface → surface blit (handles tiled/mosaic + plain) | `Renderer->DrawBitmapSubrect[Tinted]` / `TSurface::Blit` | HIGH |
| `FUN_004bd490_Blit` (0x4bd490) | low-level blit dispatch (size check + vtable +0x50/+0x60 routing) | internal to `TSurface::ParamBlit`/`BlitHandler` | HIGH |
| `FUN_004bd680` (0x4bd680) | **no dedicated drop** — sprite stamp `(x,y,resource,drawmode,0)` into "current surface" | `Renderer->DrawBitmap[Subrect]` (opaque) / `…Tinted` (alpha) | MEDIUM (signature confirmed at call sites; body UNCONFIRMED) |
| `FUN_004be2b0_CompositeBuffer` (0x4be2b0) | GDI `DrawTextA` renderer (offscreen HDC → surface), w/ 3-pass drop shadow | `Renderer->CompositeSwapchainTinted` over a TTF coverage atlas | HIGH (behavior) / MEDIUM (some HDC field offsets) |
| `FUN_00429950_BlitEffect_PackRGB` (0x429950) | 3-byte RGB→struct pack (`[2]=B,[1]=G,[0]=R,[3]=0`) | inline color set on draw call | HIGH |
| `FUN_00419dd0_BlitEffect_SetField` (0x419dd0) | 1-int field copy `*dst=*src` | inline field set | HIGH |
| `FUN_00438d80_BlitEffect_StructInit` (0x438d80) | init the 22-int blit/draw descriptor struct (shadow dx,dy + dst rect) | `SDrawParam` fill (`MakeDP`) + `DrawBitmap…Shadowed` | HIGH (offsets) / MEDIUM (semantics of all 22 fields) |
| `FUN_004aa850_BlitEffect_Setup` (0x4aa850) | register an effect into the 10-slot global table (stride `0x13`) | `Renderer->DrawBitmap…Shadowed` (modern replaces the registry) | HIGH |
| `FUN_004aa7c0_BlitEffect_AddOrUpdate` (0x4aa7c0) | patch (srcX,srcY,offX,offY) of an existing slot | — (subsumed by per-call shadow params) | HIGH |
| `FUN_004aa280_BlitWithEffects` (0x4aa280) | base blit then iterate active effects | `DrawBitmap…Shadowed` does both passes | HIGH |
| `FUN_004aacb0_BlitEffect_Iterate` (0x4aacb0) | for each active slot intersecting the rect → compute dst, call Apply | internal to the shadow helper | HIGH |
| `FUN_004aaeb0_BlitEffect_Apply` (0x4aaeb0) | dirty-rect clip + actual shadow/glow effect blit | internal to the shadow helper | MEDIUM (large body; outer logic clear, inner switch partial) |
| `FUN_0046d710` (0x46d710) | resource-by-name lookup → bitmap/sprite handle (iterator on a handle in the by-index form) | asset-cache lookup by name | MEDIUM (signature confirmed; body UNCONFIRMED) |
| `FUN_0058b100` (0x58b100) | `sprintf`-family format-into-buffer | `snprintf` / `std::format` | MEDIUM (signature confirmed; body UNCONFIRMED) |
| `FUN_0054a5d0` (0x54a5d0) | the 4-section bar-fill kernel (cap/bright/dim/cap) | per-panel bar draw via `DrawBitmapSubrect` | MEDIUM (see TPlyrStatusBar §6) |

Surface/pane vtable-offset map is in §15. Chroma-key convention is §16. UNCONFIRMED list is §17.

---

## 1. Background: the surface vtable + drawmodes (read this first)

Panel paint code calls surface methods **by vtable offset**, e.g. `(**(code **)(*surface + 0x5c))(&dp, src, 0, 0)`. The retail surface class is the ancestor of the ported `src/surface.h` `TSurface` (which deliberately preserves the legacy `Put/Blit/Box/WriteText/GetBlit/ZPut` API — see `src/surface.h:286-307`). The numeric offsets are the **compiled vtable slot offsets** of that class; §15 maps each one. The port does not call by offset — it calls the named method — so panel specs should translate `(*+0x5c)(…)` → `surface->ParamBlit(…)` / `Renderer->DrawSurface(…)` and `(*+0x64)(…)` → `surface->Box(…)`.

**Drawmode constants** (from `src/revdefs.h:307-338`) decode the magic literals that appear all over the paint code:

| Literal | Constant | Meaning |
|---|---|---|
| `0x80000000 = 2147483648` | `DM_USEDEFAULT` | "supply a default" — for blits this is the plain opaque copy |
| `0x00002000 = 8192` | `DM_ALPHA` | alpha-channel blend |
| `0x00000100 = 256` | `DM_TRANSPARENT` | keycolor transparency |
| `0x20000000 = 536870912` | `DM_FILL` | fill dest with current color |
| `0x00008000 = 32768` | `DM_TRANSLUCENT` | translucent blend |
| `0x00001000 = 4096` | `DM_ALIAS` | antialias edges using alias data |
| `0x00000010 = 16` | `DM_BACKGROUND` | draw to background |
| `0x00000001 = 1` | `DM_NOCLIP` | disable clipping |

So `FUN_004bd680(0,0,sprite,0x80000000,0)` = "opaque copy sprite at (0,0)"; `FUN_004bd680(0x2b,3,icon,0x2000,0)` = "alpha-blend icon at (43,3)". (Citations: `cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp:25,33`.)

**`SDrawParam`** (the struct passed by pointer to `(*+0x5c)` / `(*+0x64)`) is defined at `src/graphics.h:58-94`. Field order: `drawmode, func, callback, data, originx, originy, clipx, clipy, clipwidth, clipheight, dx, dy, dwidth, dheight, sx, sy, swidth, sheight, color, intensity, zpos, normal`. This is the modern shape; the retail compiled struct that `FUN_00438d80` fills is the same family (§7). The first int of the struct is always `drawmode` — that is why blit-effect structs start with the drawmode word.

---

## 2. `FUN_00414d70` — TBitmap / surface → surface blit

- **Symbol + addr + recon:** `FUN_00414d70` @ `0x00414d70`, `recon/discovered/FUN_00414d70_TBitmap_BlitToSurface.cpp` (size 1186).
- **Identified signature (observed):**
  `undefined4 FUN_00414d70(dstX, dstY, dstSurf?, srcBitmap*, dstBitmap*, srcW, srcH, drawmode, sx, sy, dwidth, dheight, color/zinfo, flags)`
  — i.e. `(int dx, int dy, undefined4 p3, int* src, int* dst, int sw, int sh, undefined4 mode, int sx, int sy, int dw, int dh, undefined4 p13, uint flags)`. (params at recon `:4-6`).
- **One-line purpose:** general bitmap/surface → surface rectangle blit; the workhorse non-mosaic path used by the bar fill (`FUN_0054a5d0` Classic branch).
- **Behavior (step by step):**
  1. **Format compatibility:** if both `param_4` (src) and `param_5` (dst) are non-null, compares `(*vtable+8)()` (= `SurfaceType`/`BitsPerPixel`-class getter) of each; returns 0 if mismatched (`:37-44`).
  2. **Degenerate-rect rejection:** returns 0 if `srcW<1 || srcH<1` or (src present and `dwidth<1||dheight<1`) (`:49-52`).
  3. **Auto-alpha:** if no blend bits set (`param_14 & 0x3f == 0`) and the src bitmap flags `param_4[0xe] & 0x30000` are set, OR-in `4` into the mode (`:58-60`). `param_4[0xe]` = the src bitmap's flags word.
  4. **Mosaic / tiled src path** (`SurfaceType()==0x10 = 16`, `:62`): iterates the source's sub-tile array (`param_4[0x1a]` = tile count, `param_4[0x1b]`/`[0x1c]` = tile geometry/pointer arrays). For each tile it intersects against the requested dest rect, computes clipped sub-rects, and calls the inner kernel `FUN_00414550(...)` per tile (`:71-138`). Returns accumulated result.
  5. **Plain path** (`:141-153`): reads src dims `param_4[1]`(w)/`[2]`(h), gathers src/dst pixel-buffer accessors via `(*vtable+0xc)`/`(*vtable+0x10)`, and calls `FUN_00414550(param_1,param_2,…,param_4[1],param_4[2],…,param_14)` once.
- **Key parameters:**

  | Param | Role |
  |---|---|
  | `param_1, param_2` | dest x, y |
  | `param_4` | source bitmap/surface (`[0xe]`=flags, `[0x1a]`=tilecount, `[1]/[2]`=w/h) |
  | `param_5` | dest bitmap/surface (nullable) |
  | `param_6, param_7` | source width, height (the rect to read) |
  | `param_8` | drawmode |
  | `param_9, param_10` | source x, y offset |
  | `param_11, param_12` | dest width, height (stretch target) |
  | `param_14` | flags (low 6 bits = blend selector; `&4` auto-set for alpha bitmaps) |

- **Gotchas:** the 16-type "mosaic" branch is the multi-tile surface (`TMosaicSurface`, `SURFACE_MOSAIC=5` in modern enum but `0x10` is the retail `SurfaceType()` return for the tiled case — treat `0x10` as "this is a tiled/mosaic source, iterate its tiles"). Stretch happens when `param_11/12 != param_6/7`. Inner pixel work is `FUN_00414550` (not extracted; it is the actual rasterizer).
- **Port mapping:** for HUD use, `Renderer->DrawBitmapSubrect(bm, dstX, dstY, sx, sy, sw, sh)` (opaque) or `Renderer->DrawBitmapSubrectTinted(...)` (the bar path packs an alpha in the color, §8 / TPlyrStatusBar §6). Surface-to-surface usage maps to `TSurface::Blit(x,y,surface,sx,sy,sw,sh,drawmode)` (`src/surface.h:197-203`). The mosaic-source tiling is handled internally by `TMultiSurface`/`TMosaicSurface` in the port (`src/multisurface.h`, `src/bmsurface.h`).

---

## 3. `FUN_004bd490_Blit` — low-level blit dispatch

- **Symbol + addr + recon:** `FUN_004bd490_Blit` @ `0x004bd490`, `recon/discovered/FUN_004bd490_BlitPrimitive.cpp` (size 292). `__thiscall` (`this`=`param_1`).
- **Identified signature:** `undefined4 FUN_004bd490_Blit(TSurface* this, SDrawParam* dp, TSurface* source, p4, p5)`.
- **One-line purpose:** the blit dispatcher — validates surfaces, then routes the per-pixel work to the right handler (source `GetBlit` for complex sources, or this surface's own blit). This is the legacy `TSurface::BlitHandler`/`ParamBlit` core.
- **Behavior:**
  1. If `source != null`: checks `this->(*+0x14)()` (a "is-video / needs-same-format" predicate) and global `DAT_005db0b8`; if set and `this[1]/[2]` (w/h) differ from `source[1]/[2]`, emits warning `"Unable to blit from different si…"` (`:12-17`).
  2. **Complex-source redirect:** if `source->(*+0x54)() != 0` (= `UseGetBlit()`, see `src/surface.h:166`), call `source->(*+0x60)(dp, this, p4, p5)` (= the source's `ParamGetBlit`) and return (`:18-22`). This is the mosaic/multi-surface "source draws into dest" path.
  3. **Format-mismatch flag:** if `(source[0xe] ^ this[0xe]) & 0x3001f` (pixel-format bits differ) → set `dp->drawmode |= 0x8000000` (`DM_NOHARDWARE`, `:23-25`).
  4. **Clip + dispatch:** unless `dp->drawmode & 0x400000` (`DM_NODRAW`), call `this->(*+0x50)(dp, source, p4, p5)` (the clip/setup) — if it returns 0, return 0 (`:27-30`).
  5. Resolve graphics buffers via `(*+0x38)` (= `GetGraphicsBuffer`) on both surfaces; if buffers identical, no-op return 1 (`:31-45`).
  6. If `dp->drawmode & 0x400` (`DM_ZBUFFER`): also blit the z-buffer via the buffers' `(*+0x50)` (`:46-56`).
- **Key parameters:** `param_2` = the `SDrawParam` (drawmode at `*param_2`); `param_3` = source surface.
- **Gotchas:** `(*+0x54)`=`UseGetBlit`, `(*+0x60)`=`ParamGetBlit`, `(*+0x50)`=`ParamDraw`/clip-setup, `(*+0x38)`=`GetGraphicsBuffer` — these slot identities are the load-bearing cross-reference (§15). The `0x3001f` mask is the pixel-format-identity bits used everywhere for "are these two surfaces compatible".
- **Port mapping:** internal — `TSurface::ParamBlit` (`src/surface.h:175`) + `BlitHandler` (`:153`) already encapsulate this dispatch. Panel specs never call this directly; it sits under `Blit`.

---

## 4. `FUN_004bd680` — sprite stamp helper (NO DEDICATED DROP — signature from call sites)

- **Symbol + addr:** `FUN_004bd680` @ `0x004bd680`. **No extracted body exists** in `recon/discovered/` (verified: `grep -rl FUN_004bd680` returns only call sites — the PlyrStatusBar helpers and `TLogoScreen_Animate`). Per AGENT_PROTOCOL §"Sibling rule", bare helpers are commonly missing; **extract on demand** with `DecompileAddr.java 0x004bd680` if exact behavior is needed.
- **Observed call signature:** `FUN_004bd680(int x, int y, resourceHandle, int drawmode, 0)`.
  - Player-side stamps (`recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp`):
    - `:18` `FUN_004bd680(0,0,iVar1,0x80000000,0)` — live char render, opaque.
    - `:25` `FUN_004bd680(0,0,uVar2,0x80000000,0)` — `BackPanel` sprite, opaque.
    - `:30` `FUN_004bd680(0x1a-(*piVar3>>1), 0x1f-(piVar3[1]>>1), piVar3, 0x2000, 0)` — `Ring`, centered, alpha.
    - `:31-33` `FUN_004bd680(0x2b,0x20,…,0x2000,0)` etc. — icons, alpha.
- **Inferred one-line purpose:** stamp a sprite/resource at (x,y) into the **implicit current draw surface** with the given drawmode. It takes no explicit dest surface, so it draws into a global/ambient "current surface" (the surface that the surrounding code set up — here the `+0x6c` panel buffer; the helper sets that up via the `(*+100)(…,0x80000000)` clear at `:15` before the stamps).
- **Key parameters:**

  | Arg | Role |
  |---|---|
  | 1 | dest x (panel-local) |
  | 2 | dest y |
  | 3 | resource handle (from `FUN_0046d710`, or a vtable `+0x130` live-render surface) |
  | 4 | drawmode: `0x80000000`=`DM_USEDEFAULT` (opaque copy), `0x2000`=`DM_ALPHA` |
  | 5 | always `0` (likely color/intensity or a flags tail) |

- **Gotchas:** the `Ring` centering math `(anchor - w/2, anchor - h/2)` reads the sprite's own dims `*piVar3`(w)/`piVar3[1]`(h) — these are the bitmap-header width/height ints. Confirms `FUN_0046d710` returns a handle whose `[0]`=width, `[1]`=height.
- **Port mapping:** opaque (`0x80000000`) → `Renderer->DrawBitmap(bm, x, y)`; alpha (`0x2000`) → `Renderer->DrawBitmapSubrect(bm, x, y, 0,0, w,h)` with the source's own alpha (these sprites carry alpha, not magenta key — see TPlyrStatusBar §10 / §16). The "implicit current surface" becomes an explicit `TSurface*` target in the port; the panel spec should name which cached surface is active (e.g. the `+0x6c` panel buffer).
- **Confidence:** signature MEDIUM-HIGH (consistent across 6 call sites + TLogoScreen); body **UNCONFIRMED** — drawmode tail arg 5 and the exact "current surface" mechanism are inferred, not read.

---

## 5. `FUN_004be2b0_CompositeBuffer` — the GDI `DrawTextA` text renderer (DETAILED)

- **Symbol + addr + recon:** `FUN_004be2b0_CompositeBuffer` @ `0x004be2b0`, `recon/discovered/FUN_004be2b0_CompositeBlit.cpp` (size 3377). `__thiscall`.
- **Identified signature (from recon `:3-5`):**
  `undefined4 FUN_004be2b0(int* surface /*this=param_1*/, int dstX /*param_2*/, int dstY /*param_3*/, int w /*param_4*/, int h /*param_5*/, char* text /*param_6*/, LONG* p7 /*color/shadow ptr*/, uint fontIndex /*param_8*/, LONG* p9 /*param_9 line-rect/-1*/, uint flags /*param_10*/, uint p11 /*param_11 color/drawmode*/)`.
  > Note: the original brief lists `param_11=color`; in the body, the colored-pass color is computed from `param_7` (the byte-swap at `:378`), and `param_11` is the surface drawmode word. Both are "the color/format inputs"; see Gotchas.
- **One-line purpose:** render a text string into a surface using a GDI `DrawTextA` into an offscreen HDC, with optional 3-pass black drop shadow, then composite the rendered text back onto the surface.
- **Behavior:**
  1. **Empty guard:** if `text==null || *text=='\0'` return 1 (`:112-114`).
  2. **Font HDC lookup:** `hdc = *(HDC*)(DAT_0065b020 + param_8*4)`; if null fall back to `DAT_0065b024` (`:115-118`, repeated `:130-133`). So **`DAT_0065b020` is the base of an HDC table indexed by `fontIndex`** (4 bytes/entry); `DAT_0065b024` is the default font HDC (entry 1).
  3. **Video-surface guard:** `this->(*+0x14)()` ≠ 0 → error `"Can't draw text in video surface"` (`:121-125`).
  4. **Two builds:** a "fast" path when `hdc[8].unused != 2` (`:134-206`) and the full composite path otherwise (`:207+`). The full path is the HUD path (offscreen scratch).
  5. **Allocate/clear scratch:** if no usable cached text surface (`this[0x17]`), allocate one (`FUN_00482fb0` + `FUN_004bcb00` ctor + `FUN_004a5740(w,h,0x400,…)`), and clear it via `(*+100)(0,0,w,h,…,0xffff,0x7f7f)` (the `Box`/fill, `:218-256`).
  6. **Format flag construction** (`:301-331`) — builds `uStack_300` (the `DrawTextA` uFormat):
     - base `uStack_300 = 0x2810` (`:301`) = `DT_NOCLIP(0x100? )|DT_NOPREFIX(0x800)|DT_SINGLELINE(0x20)|…` (the retail base = `0x2810`).
     - **Horizontal align** from `param_10`: `&1` → leave base (left); `&4` → `0x2812` (sets `DT_RIGHT=0x2`); `&2` → `0x2811` (sets `DT_CENTER=0x1`) (`:302-311`).
     - **Vertical align** from `param_10`: `&0x10` → `|0x20`; `&0x20` → `|0x28` (vcenter); `&0x40` → `&~0x2010 |0x24` (bottom) (`:312-324`).
     - `param_10 & 0x80` → `&~0x2010 |0x20` (a single-line override, `:325-328`).
     - `param_10 & 0x100` → `|0x400` (`:329-331`) — **enables the drop-shadow path** (the shadow uses `uStack_300` with this bit; see step 8).
  7. **Text length / wrap:** computes string length (`:342-350`); if not the `0x80` override, runs `FUN_004aceb0` for wrap offset (`:336-341`).
  8. **SHADOW PASS** (`:387-414`) — runs **only when `piStack_204 != 0`**, where `piStack_204 = param_8 & 0x400` (`:377`). So **`fontIndex & 0x400` (= bit 10) is the drop-shadow flag**:
     - `SetTextColor(hdc, 0)` (black), `SetBkMode(hdc, 1)` (TRANSPARENT) (`:387-388`).
     - **Pass 1 (base):** `DrawTextA(hdc, text, uStack_300, &rect@base, …)` (`:395`).
     - **Pass 2 (+1 in x):** rect.left = base.left + 1; `DrawTextA(...)` (`:397-401`).
     - **Pass 3 (+1 in y):** rect.top = base.top + 1; `DrawTextA(...)` (`:402-406`).
     - Then **colored pass:** `SetTextColor(hdc, uStack_208)` where `uStack_208` = the BGR-swapped color (`:378-379`, `:409`); `DrawTextA(hdc, text, uStack_300, &rect@base, …)` (`:410-414`).
     - Net: a **1px black drop shadow toward right + bottom**, colored text on top.
  9. **Color packing** (`:378`): `uStack_208 = ((param_7&0xff)<<8 | param_7>>8&0xff)<<8 | param_7>>0x10&0xff` — this is an **RGB→BGR byte swap** (`COLORREF` for GDI is `0x00BBGGRR`). So `param_7` carries the text color as RGB and it is swapped to GDI's BGR for `SetTextColor`.
  10. **Composite back:** the rendered HDC bitmap is selected/blitted back onto the surface via `(*+0x5c)(&dp, scratch, 0, 0)` (`:476` / `:503`) and finalized through `FUN_004acb80` (`:536`).
- **Key parameters / semantics:**

  | Param | Meaning |
  |---|---|
  | `param_1` | dest surface (`this`) |
  | `param_2,param_3` | dest x, y |
  | `param_4,param_5` | cell w, h |
  | `param_6` | text (C string) |
  | `param_7` | text color, **RGB**; byte-swapped to GDI BGR at `:378` |
  | `param_8` | **font index** (low bits → HDC table index; **bit `0x400`=drop-shadow flag**, `:377`) |
  | `param_9` | per-line rect / `0xffffffff` = use font default line metrics (`:185-197`) |
  | `param_10` | **format flags**: `&1`=left,`&2`=center,`&4`=right; `&0x10/0x20/0x40`=top/vcenter/bottom; `&0x80`=single-line override; `&0x100`=enable shadow build (`:302-331`) |
  | `param_11` | surface drawmode word (composited via `uStack_2c0`, `:149-153`) |

- **Gotchas:**
  - **Two shadow gates.** `param_10 & 0x100` builds the shadow-capable format word (`:329`), but the actual 3-pass shadow loop is gated on **`param_8 & 0x400`** (`:377,389`). In TPlyrStatusBar all font ids (`0x401/0x402/0x404`) have bit `0x400` set, so every panel text element gets the shadow (cross-check: `TPlyrStatusBar_SPEC.md:222-229`).
  - **Font index → HDC:** `DAT_0065b020 + fontIndex*4` is the HDC table; the **low byte of the font id (1/2/4)** is the table index, while bit `0x400` is the shadow flag, so e.g. `0x401` = HDC[1] + shadow. (`:115,130`.)
  - **Align bits live in `param_10`, NOT in the font id.** TPlyrStatusBar passes align via the low nibble of the font id which the slot23 caller maps into `param_10` — verify per call site. (Cross-check `TPlyrStatusBar_SPEC.md:241-244`.)
  - **Pink-halo bug:** the magenta-cleared scratch + the antialiased shadow edge produce a pink fringe in retail — **do not reproduce.** Full analysis is in `TPlyrStatusBar_SPEC.md:295` (§10 item 1). Reference, don't re-derive.
- **Port mapping:** render the string into a TTF coverage atlas (white RGB, coverage in alpha) via `BuildTTFAtlas(...)`, then `Renderer->CompositeSwapchainTinted(atlasTex, dst…, src…, tr,tg,tb,ta)`. The 3-pass black shadow = three black-tinted composites at base / +1x / +1y, then one colored composite at base — exactly what `CompositeSwapchainTinted` is documented for (`src/renderer.h:703-714`). Use real alpha (no magenta key) to avoid the pink fringe.
- **Confidence:** behavior HIGH; a few HDC sub-struct field offsets (`hdc[0x14..0x17].unused` line metrics, `hdc[8].unused` mode) are read literally but their exact GDI meaning is MEDIUM.

---

## 6. `FUN_00429950_BlitEffect_PackRGB` — 3-byte RGB pack

- **Symbol + addr + recon:** `FUN_00429950_BlitEffect_PackRGB` @ `0x00429950`, `recon/discovered/FUN_00429950_TextEffect_GlowSetup.cpp` (size 29). `__thiscall`.
- **Identified signature:** `void FUN_00429950(uint8_t* dst, uint8_t b, uint8_t g, uint8_t r)`.
- **One-line purpose:** pack 3 color bytes into a 4-byte struct field.
- **Behavior (exact, `:7-10`):** `dst[2]=param_2; dst[1]=param_3; dst[0]=param_4; dst[3]=0;`
  - i.e. **`dst[0]=R`, `dst[1]=G`, `dst[2]=B`, `dst[3]=0`** when called as `(dst, B, G, R)` — the call order is `(dst, blue, green, red)` so the **stored layout is R,G,B,0** (RGBA with A=0). In TPlyrStatusBar the caller passes `0xff,0xff,0xff` for white text (`TPlyrStatusBar_SPEC.md:240`).
- **Gotchas:** the *parameter* order is B,G,R but the *stored* order is R,G,B — read the indices, not the names. `dst[3]` is zeroed (alpha/pad).
- **Port mapping:** inline — set the draw call's color to `(r,g,b)`; no helper needed.

---

## 7. `FUN_00419dd0_BlitEffect_SetField` — 1-int field copy

- **Symbol + addr + recon:** `FUN_00419dd0_BlitEffect_SetField` @ `0x00419dd0`, `recon/discovered/FUN_00419dd0_TextEffect_Render.cpp` (size 13). `__thiscall`.
- **Identified signature:** `void FUN_00419dd0(uint32_t* dst, uint32_t* src)`.
- **Behavior (exact, `:5`):** `*dst = *src;` — a single 4-byte copy.
- **One-line purpose:** copy one int field into a struct slot (used to write the packed color from a temp into the effect/draw struct).
- **Port mapping:** inline assignment; no helper needed.

---

## 8. Shadow / blit-effect pipeline (the global 10-slot effect registry)

This is a deferred-effect system: a blit registers an "effect" (shadow/glow) into a global 10-slot table; later a flush pass walks the table and stamps the effects. The modern port replaces the whole registry with per-call shadow helpers (`DrawBitmap…Shadowed`, `src/renderer.h:786-799`), so **panel specs usually only need the (dx,dy) semantics**, not the registry mechanics. Document them anyway so tracers recognize the calls.

### 8.0 The global tables (from `recon/discovered/renames/agent_ui_wave5_blit_effect_pipeline.txt:10-14`)
- Effect slots: `DAT_00669af0 .. DAT_00669e0c`, **10 entries, stride `0x13 = 19` ints (`0x4c = 76` bytes)**.
- Pending-effect count: `DAT_0066a8fc`.
- Slot field bases (each `+ slot*0x13`): `af0, af4, af8, afc, b00, b04, b08, b0c, b10, b14, b18, b1c, b20, …` (see §8.2 layout).

### 8.1 `FUN_00438d80_BlitEffect_StructInit` — descriptor struct init

- **Symbol + addr + recon:** `FUN_00438d80_BlitEffect_StructInit` @ `0x00438d80`, `recon/discovered/FUN_00438d80_ShadowSetup.cpp` (size 110).
- **Signature:** `void FUN_00438d80(uint32_t* buf, p2, p3, p4, p5, p6, p7, p8)`.
- **Purpose:** initialize a 22-int draw/effect descriptor — the `(dx,dy)` shadow offset + a dest rect + bookkeeping zeros.
- **Behavior (which offset gets which param, `:7-28`):**

  | `buf[idx]` (byte off) | Value | Meaning (inferred) |
  |---|---|---|
  | `buf[10]` (0x28) | `param_2` | **shadow dx** (where shadow lands, x) |
  | `buf[0xb]` (0x2c) | `param_3` | **shadow dy** (where shadow lands, y) |
  | `buf[0xc]` (0x30) | `param_6` | dst x (also mirrored to `buf[0x10]`) |
  | `buf[0x10]` (0x40) | `param_6` | dst x copy |
  | `buf[0xd]` (0x34) | `param_7` | dst y (also `buf[0x11]`) |
  | `buf[0x11]` (0x44) | `param_7` | dst y copy |
  | `buf[0xe]` (0x38) | `param_4` | width / src x |
  | `buf[0xf]` (0x3c) | `param_5` | height / src y |
  | `buf[0]` (0x00) | `param_8` | **drawmode** (struct starts with drawmode, §1) |
  | `buf[1..9]` | 0 | cleared |
  | `buf[0x12]` (0x48) | 0 | cleared |
  | `buf[0x13]` (0x4c) | `0x1f = 31` | intensity (full) |
  | `(int)buf+0x52` (u16) | 0 | normal hi |
  | `buf+0x14` (u16) | 0 | normal lo |

- **KEY semantic — `(dx,dy)` is where the SHADOW lands.** The sprite/content blit dst is computed as **`(clipped_rect - (dx,dy))`** inside the iterate step. Verified at `recon/discovered/FUN_004aacb0_EffectPostProcess.cpp:46-47`: `FUN_004aaeb0_BlitEffect_Apply(slot, (slot.field_4 - field_c?) + clipX, (slot.field_8 - field_0) + clipY, …)` — the effect's stored origin minus the live clip origin gives the displaced draw point. (The iterate uses `local_c[1]/[2]` minus `local_c[-1]/*local_c`, i.e. the registered (offX,offY) minus (srcX,srcY).)
  - Concretely (TPlyrStatusBar): player `FUN_00438d80(buf, 4, 4, …)` → shadow casts **down-right by (4,4)**; the bar content draws at **recon − (4,4)** (cross-check `TPlyrStatusBar_SPEC.md:214-217`).
- **Gotcha:** do not confuse `buf[10]/[0xb]` (shadow offset) with `buf[0xc]/[0xd]` (dst rect). The shadow offset is the *displacement*; the dst rect is the *anchor*.
- **Port mapping:** `Renderer->DrawBitmapShadowed(bm, x, y, off_x=4, off_y=4, shadow_a=0.6)` or the subrect variant (`src/renderer.h:786-799`) reproduces the whole "shadow at +offset, content on top". Fill an `SDrawParam` (`src/graphics.h:58`) if a surface-level blit is needed.

### 8.2 `FUN_004aa850_BlitEffect_Setup` — register an effect slot

- **Symbol + addr + recon:** `FUN_004aa850_BlitEffect_Setup` @ `0x004aa850`, `recon/discovered/FUN_004aa850_BlitEffect_Setup.cpp` (size 213).
- **Signature:** `int FUN_004aa850(srcX /*param_1*/, srcY /*param_2*/, offX /*param_3*/, offY /*param_4*/, type /*param_5*/)` → slot index `0..9` or `-1` if full / `0` if `type==0`.
- **Behavior:** scans `DAT_00669b14`-stride-`0x13` for a free slot (`type` field == 0, `:15-20`); if all 10 used → return `-1` (`:21-23`). Else zero the slot's transient fields, then store:

  | Slot field (`+ slot*0x13`) | Value |
  |---|---|
  | `DAT_00669af4` (`af0+4`) | `param_1` = srcX |
  | `DAT_00669af8` | `param_2` = srcY |
  | `DAT_00669b0c` | `param_3` = offX |
  | `DAT_00669b10` | `param_4` = offY |
  | `DAT_00669b14` | `param_5` = type (the "slot occupied" sentinel) |
  | `DAT_00669af0` | 0 (flags) |
  | `DAT_00669afc/b00/b04/b08` | 0 (live clip state) |
  | `DAT_00669b1c` / `DAT_00669b20` | two `malloc(0xa00 = 2560)` scratch buffers (`:35-38`) |
  | `DAT_00669b2c/b30/b34/b38` | 0 (ring-buffer indices) |

- **Gotcha:** each slot owns **two 2560-byte scratch buffers** (`0xa00`) — these hold up to 128 dirty rects (`0xa00 / 0x14 = 128`; each rect is `0x14=20` bytes, matching the `* 0x14` stride in Apply `:128`). The `0x7c=124`/`0x7f=127` caps in Apply are the ring-buffer limits.
- **Port mapping:** no equivalent registry; the shadow is drawn inline by `DrawBitmap…Shadowed`.

### 8.3 `FUN_004aa7c0_BlitEffect_AddOrUpdate` — patch slot fields

- **Symbol + addr + recon:** `FUN_004aa7c0_BlitEffect_AddOrUpdate` @ `0x004aa7c0`, `recon/discovered/FUN_004aa7c0_BlitEffect_AddOrUpdate.cpp` (size 75).
- **Signature:** `void FUN_004aa7c0(int slot, srcX, srcY, offX, offY)`.
- **Behavior (`:7-12`):** bounds-check `0 <= slot < 10` and slot occupied (`DAT_00669b14[slot*0x13] != 0`), then overwrite `af4=srcX, af8=srcY, b0c=offX, b10=offY`. Same four fields as Setup.
- **Port mapping:** none (per-call params replace it).

### 8.4 `FUN_004aa280_BlitWithEffects` — base blit + iterate

- **Symbol + addr + recon:** `FUN_004aa280_BlitWithEffects` @ `0x004aa280`, `recon/discovered/FUN_004aa280_BlitWithEffect.cpp` (size 145).
- **Signature:** `undefined4 FUN_004aa280(params /*param_1*/, source /*param_2*/, p3)` — note `this`/dst arrives as `unaff_retaddr` (a Ghidra calling-convention artifact; the real `this` is the display surface).
- **Behavior:**
  1. Pre-check `DAT_0066a8f8->(*+0x60)()` (the display's `ParamGetBlit`) for a sentinel `-0x7789fe3e` → early out (`:11-13`).
  2. `DAT_0066a8fc = 0` (reset pending count), then do the **base blit** `FUN_004bd490_Blit(this, params, source, p3)` (`:15-16`).
  3. If `DAT_0066a8fc > 0`, walk the pending list `DAT_00669df4`-stride-5 and call `FUN_004aacb0_BlitEffect_Iterate(...)` for each (`:18-25`).
  4. Reset `DAT_0066a8fc = 0`; return base-blit result.
- **Gotcha:** this is the implementation behind the surface vtable slot **`+0x5c`** on the display-like global (per the renames file: "`vtable[+0x5c]` on the TDisplay-like global"). So **`(*+0x5c)(&dp, src, 0, 0)` everywhere in panel code = "blit src with effects applied"**.
- **Port mapping:** `Renderer->DrawSurface` / `DrawBitmap…Shadowed` — the modern call does the base draw and the shadow in one.

### 8.5 `FUN_004aacb0_BlitEffect_Iterate` — per-slot dst computation

- **Symbol + addr + recon:** `FUN_004aacb0_BlitEffect_Iterate` @ `0x004aacb0`, `recon/discovered/FUN_004aacb0_EffectPostProcess.cpp` (size 275).
- **Signature:** `void FUN_004aacb0(dstX /*param_1*/, dstY /*param_2*/, dstW /*param_3*/, dstH /*param_4*/, srcId /*param_5*/)`.
- **Behavior:** if global effects-enabled (`DAT_005e91bc == 0`), loop all 10 slots (`:18-53`); for each occupied slot (`local_c[7] != 0`) whose registered rect intersects `(dstX,dstY,dstW,dstH)`, clip the rect and call `FUN_004aaeb0_BlitEffect_Apply(slot, (offX - srcX)+clippedX, (offY - srcY)+clippedY, clippedW, clippedH, srcId)` (`:46-47`).
- **KEY:** the dst passed to Apply is **`(registered_offset − registered_src) + clipped_origin`** — this is the (dx,dy) displacement applied (§8.1). The intersection math at `:24-45` is standard rect-clip.
- **Port mapping:** internal to the shadow helper.

### 8.6 `FUN_004aaeb0_BlitEffect_Apply` — dirty-rect + actual effect blit

- **Symbol + addr + recon:** `FUN_004aaeb0_BlitEffect_Apply` @ `0x004aaeb0`, `recon/discovered/FUN_004aaeb0_BlitEffect_Apply.cpp` (size 2737). `__thiscall`-ish.
- **Signature:** `undefined4 FUN_004aaeb0(int param_1 /*display*/, int slot /*param_2*/, x /*param_3*/, y /*param_4*/, w /*param_5*/, h /*param_6*/, uint flags /*param_7*/)`.
- **One-line purpose:** the workhorse — clips the effect rect to the slot bounds, then either does the effect blit or recurses to handle a complex dirty-rect overlap, maintaining a per-slot 128-entry ring buffer of already-drawn rects.
- **Behavior (outer logic clear; inner switch partial):**
  1. Guard: slot in `[0,10)`, occupied, effects-enabled (`:30-31`).
  2. Read slot origin `afc`(x)/`b00`(y) and size from either the registered off (`flags & 4`) or the source bitmap dims (`b14+4`/`b14+8`) (`:34-41`).
  3. Intersect `(x,y,w,h)` with the slot rect; bail if disjoint (`:42-58`).
  4. **`flags & 4` branch** (`:59-83`): re-fetch the slot's source bitmap, build a fresh descriptor via `FUN_00438d80_BlitEffect_StructInit` with the displaced origin, and blit via `(*+0x5c)` then resolve buffers via `(*+0x44)`.
  5. **`flags & 8` branch** (`:84-111`): the "shadow at offset" pass — same shape, different `(dx,dy)` derivation.
  6. **`flags & 1 / flags & 2` branch** (`:112-317`): the **dirty-rect ring buffer** logic — a giant `switch(local_90)` (0..0xe) that splits the new rect against already-drawn rects so overlapping regions aren't double-blitted (the recursion calls `FUN_004aaeb0_BlitEffect_Apply` on the sub-rects). `DAT_00669b24`/`b2c`/`b34` are the per-slot ring counters; `0x7c=124` / `0x7f=127` are the wrap limits.
- **Gotchas:** the 15-case `switch` (`:177-260`) is rect-subtraction bookkeeping, **not** distinct visual effects — it's just avoiding overdraw. The actual pixel blit is the `(*+0x5c)` call in the `flags&4`/`flags&8` branches. Do not try to map each switch case to a visual feature.
- **Port mapping:** none of this is needed — the GPU composites with proper blending and overdraw is harmless. `DrawBitmap…Shadowed` covers the visible result.
- **Confidence:** MEDIUM — outer flow + the (dx,dy)/ring-buffer roles are clear; the per-case sub-rect math is documented as "overdraw avoidance" without verifying each branch (low value to do so).

---

## 9. `FUN_0046d710` — resource-by-name lookup (NO DEDICATED DROP)

- **Symbol + addr:** `FUN_0046d710` @ `0x0046d710` (a.k.a. `cls_0x46d6b0::meth_0x46d710`). **No standalone extracted body** in `recon/discovered/`; appears only as a call. Extract with `DecompileAddr.java 0x0046d710` if exact behavior needed.
- **Observed signatures (two call shapes):**
  - **By-name:** `handle = FUN_0046d710(s_BackPanel_005e573c)` — takes a string-literal name, returns a bitmap/sprite handle (`recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp:20-24`). The returned handle's `[0]`=width, `[1]`=height (used in the Ring centering `*piVar3 / piVar3[1]`, `:30`).
  - **Iterator on a resource container:** `FUN_0046d710(DAT_0065c5c8)` called repeatedly returns the **next** bitmap entry from a multi-archive (`TSideTabsPane_SPEC.md:100,380` — 3 sequential calls → entries 0,1,2). Same address, the arg here is a container handle not a name string.
- **One-line purpose:** resolve a named resource (or the next entry of a container) to a usable bitmap/sprite handle.
- **Gotchas:** the dual call shape means the function likely dispatches on arg type (string ptr vs container handle), OR there are two overloads at adjacent addresses Ghidra merged. The **by-name** form is what the StatusBar/StatPane panels use for `BackPanel/Ring/HealthIcon/...`; the **iterator** form is what TSideTabsPane uses. Treat handle layout as `{int width; int height; …}`.
- **Port mapping:** asset-cache lookup by name → `PTBitmap`. Panel specs should resolve the named sprite once at init.
- **Confidence:** signature MEDIUM (consistent at many call sites); body **UNCONFIRMED** — the name-vs-iterator dispatch and exact handle struct are inferred.

---

## 10. `FUN_0058b100` — `sprintf`-family format-into-buffer (NO DEDICATED DROP)

- **Symbol + addr:** `FUN_0058b100` @ `0x0058b100`. **No standalone extracted body**; appears widely as a call (script, save/load, and the name-format helper). Extract with `DecompileAddr.java 0x0058b100` if needed.
- **Observed signature:** `FUN_0058b100(char* buffer, const char* format, args...)` — variadic.
  - `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PortraitDraw_54ae10.cpp:28` `FUN_0058b100(param_1, pcVar4, param_3, uVar5, iVar6, iVar2)` — buffer + format + 4 args.
  - `:41` `FUN_0058b100(param_1, pcVar4, param_3, uVar5)` — buffer + format + 2 args.
  - The formats used are the literals `"%s\nLevel %d"` (`s_%s_Level_%d_005e57dc`) and DM `"%s\nLv:%d P:%d M:%d"` (`TPlyrStatusBar_SPEC.md:248-250`).
- **One-line purpose:** format a value/name string into a stack buffer (CRT `sprintf` or a thin wrapper).
- **Gotcha:** it is the **`sprintf`** family (writes into the first-arg buffer), not `printf` (no stdout). The PlyrStatusBar value cells use the trivial format `"%d"` (`DAT_005e57e8/ec/f0/f4`).
- **Port mapping:** `snprintf` / `std::format` into a local buffer.
- **Confidence:** signature MEDIUM (buffer-first variadic, format literals confirmed); body **UNCONFIRMED**.

---

## 11. `FUN_0054a5d0` — the 4-section bar-fill kernel

- **Symbol + addr + recon:** `FUN_0054a5d0` @ `0x0054a5d0`, `recon/discovered/FUN_0054a5d0_TPlyrStatusBar_BarRender_Helper.cpp` (size 1446). `__thiscall`.
- **Signature (recon `:3-6`):** `void FUN_0054a5d0(int this /*param_1*/, int value /*param_2*/, uint max /*param_3*/, int dstX /*param_4*/, dstY /*param_5*/, srcX /*param_6*/, srcXbright /*param_7*/, capX2 /*param_8*/, srcDim /*param_9*/, fullW /*param_10*/, drawmode /*param_11*/, capW /*param_12*/, sectW /*param_13*/, dir /*param_14*/)`.
- **One-line purpose:** composite a horizontal stat bar in 4 sections (left cap, bright/full interior, dim/empty interior, right cap) from the `Bars` atlas, with the fill split at `value/max`.
- **Behavior summary:**
  - Clamp `value` to `[0, max]` (`:40-48`).
  - Alpha = `(animCounter * 0xff) / 6` packed as `<<0x18 | 0xffffff` (`:97`); animCounter from `this+0xd4` (player) or `this+0xdc` (target), selected by `dir = param_14` sign (`:49-54`).
  - `fillW = value*fullW/max` (`:55`).
  - Two render paths: **Classic** (`DAT_006680c8 == 0`) uses 4× `FUN_00414d70` (`:96-111`); **hi-res** uses 4× `(*+0x5c)` mosaic blits on `PTR_DAT_005d79e0` (`:113-192`).
  - `dir` (+1 player / −1 target) mirrors the fill-growth direction so target bars drain right-to-left.
- **Port mapping & full per-bar literal-arg semantics** are in `TPlyrStatusBar_SPEC.md:153-208` (§6) — including the **UNCONFIRMED-D** caveat that the 14 kernel params don't map 1:1 to the 10 call-site literals. **Panel tracers must read TPlyrStatusBar §6** before porting this; do not re-derive here.
- **Confidence:** the section structure is HIGH; exact param↔atlas-band mapping is MEDIUM (UNCONFIRMED-D in the panel spec).

---

## 12. Canonical port primitives (THE shared toolbox — use these, don't hand-roll)

Every retail draw/blit/text/shadow above maps to a shared port primitive. The
reconstruction agent **uses these**; it does not open-code shadow passes or
glyph walks in a panel, and it does not invent panel-local variants. Two rules:

- **Use the common method.** If it's in this table, call it.
- **Missing → implement once in the canonical home, register here, reuse.**
  Pixel/image primitives live on `TRenderer` (`src/renderer.{h,cpp}` —
  game-agnostic). Text/font primitives live in the UI/font layer
  (`src/font.{h,cpp}`). Never a panel-local copy.

Panels use the **compose-to-target contract** (compose the chip into one
offscreen `TSurface` RT, then `DrawSurface` it to the HUD), so the per-element
calls are the `…ToTarget` family.

| retail primitive (this doc) | canonical port method | home |
|---|---|---|
| `FUN_00414d70` / `FUN_004bd490` / `FUN_004bd680` plain blit | `DrawBitmapToTarget` / `DrawBitmapSubrectToTarget` | renderer |
| tinted blit (PackRGB color applied) | `DrawBitmapTintedToTarget` / `DrawBitmapSubrectTintedToTarget` | renderer |
| blit-effect drop shadow (§8, `FUN_00438d80` (dx,dy)=shadow-lands-here) | `DrawBitmapShadowedToTarget` / `DrawBitmapSubrectShadowedToTarget(... off_x,off_y,a)` | renderer |
| `FUN_004be2b0` text (align + color) | `DrawTextToTarget(atlas,text,cellX,cellY,cellW,ETextAlign,r,g,b,tw,th)` | font.cpp |
| `FUN_004be2b0` text + flag-0x400 3-pass shadow | `DrawTextShadowedToTarget(... same args)` | font.cpp |
| text measure / align / baseline | `TextWidth(atlas,text)` / `TextAscent(atlas)` | font.cpp |
| compose chip RT → HUD | `DrawSurface` / `DrawSurfaceTinted` | renderer |
| build TTF / bitmap font atlas | `BuildTTFAtlas(path,px)` / `BuildFontAtlas(TFont*)` | font.cpp |

Swapchain twins (`DrawBitmap`, `DrawBitmapSubrectShadowed`,
`CompositeSwapchain{,Tinted}`, …) exist for drawing straight to the HUD
swapchain (cursor/overlays) — not the usual panel path. Don't mix the two
families in one panel (scale drift).

Shadow direction is the caller's via `off_x` sign: player casts down-right
(`+dx,+dy`), target mirrors down-left (`-dx,+dy`) — NOMENCLATURE §4, and the
recon (dx,dy) is where the SHADOW lands so pass the sprite's own dst and let the
primitive offset the shadow ([[project-retail-shadow-semantics]]). The pink halo
some retail captures show is a chroma-key artifact, NOT reproduced
([[project-retail-pink-halo-bug]]).

## 13. (reserved)

## 14. (reserved)

---

## 15. Surface / pane vtable offset map (CRITICAL)

These offsets are the **compiled vtable slot offsets** of the retail surface class (ancestor of `src/surface.h:TSurface`) and of the character object. Mapping derived by cross-referencing the recon usage with `FUN_004bd490_Blit`'s internal vtable calls and the ported `TSurface` API. **Where an offset maps to a named method, both citations are given. Unmapped ones are UNCONFIRMED.**

### 15a. Surface vtable offsets (on `TSurface`-like objects, e.g. `+0x6c`/`+0x70` panel buffers, `PTR_DAT_005d79e0` display)

| Offset | Observed call | Identified method | Evidence |
|---|---|---|---|
| `+0x08` | `(*+8)()` returns a fmt id compared for equality | `SurfaceType()` / `BitsPerPixel()` (format-class getter) | `FUN_00414d70:39-40` (compat check); `src/surface.h:44,53` |
| `+0x0c` | `(*+0xc)()` → pixel/data accessor | `Lock()` / data ptr (`GetGraphicsBuffer`-ish data) | `FUN_00414d70:141,149`; returns scratch in `FUN_004be2b0:352` |
| `+0x10` | `(*+0x10)()` → second accessor | height/stride or z-buffer ptr | `FUN_00414d70:142,150` (paired with +0xc) |
| `+0x14` | `(*+0x14)()` → "is video surface" predicate | `SurfaceType()==SURFACE_VIDEO` test | `FUN_004bd490_Blit:13`; `FUN_004be2b0:121` ("Can't draw text in video surface") |
| `+0x38` | `(*+0x38)()` → graphics buffer | `GetGraphicsBuffer()` | `FUN_004bd490_Blit:31,40,41`; `src/surface.h:94` |
| `+0x44` | `(*+0x44)(x,y,w,h)` → unlock/resolve sub-region | `Unlock()` / commit-region (resolves CPU edits to GPU) | `FUN_004be2b0:370` (error "Surface.cpp 0x38f"); `FUN_004aaeb0_BlitEffect_Apply:64,81`; `src/surface.h:89` |
| `+0x4c` | `(*+0x4c)()` → draw-callback getter | `GetDrawCallBack()` | `FUN_004be2b0:505`; `src/surface.h:135` |
| `+0x50` | `(*+0x50)(dp,src,p4,p5)` → clip/setup + low-level draw | `ParamDraw()` / clip-setup (`BlitHandler` core) | `FUN_004bd490_Blit:28,52`; `src/surface.h:173` |
| `+0x54` | `(*+0x54)()` → "use source GetBlit" predicate | `UseGetBlit()` | `FUN_004bd490_Blit:18`; `src/surface.h:166` |
| `+0x58` | `(*+0x58)(&dp)` → param-blit entry | `ParamBlit` setup variant | `FUN_004be2b0:203` |
| **`+0x5c`** | `(**(code**)(*surface + 0x5c))(&dp, src, 0, 0)` | **`ParamBlit` with effects** = `FUN_004aa280_BlitWithEffects` | renames file `:31-37`; `src/surface.h:175`. **The canonical "blit a surface" call in all panel paint code.** |
| `+0x60` | `(*+0x60)(dp,this,p4,p5)` → source-driven blit | `ParamGetBlit()` | `FUN_004bd490_Blit:20`; `src/surface.h:179` |
| **`+0x64` (= `100`)** | `(**(code**)(*surface + 100))(dx,dy,w,h, color, 0xffff, 0x7f7f [,drawmode])` | **`Box()` / fill-rect** (note args: `color, zpos=0xffff, normal=0x7f7f`) | `cls_..._helper_PlayerSide_54a0a0.cpp:15`; `FUN_004be2b0:243,251`; `src/surface.h:258-260` (`Box(dx,dy,dw,dh,color,zpos=0xFFFF,normal=0x7F7F,drawmode)`) |
| `+0x68` | `(*+0x68)(hdc)` → finalize after GDI draw | `Unlock`/`SelectObject` restore commit | `FUN_004be2b0:429` (error "Surface.cpp 0x3cc") |

> Note: `+0x64`/`100` is **`Box`/fill**, not a "sub-surface accessor". The `0xffff`/`0x7f7f` literals are the default `zpos`/`normal` args (`src/surface.h:259-260`), which is the decisive fingerprint. The `0x80000000` trailing arg in `helper_PlayerSide_54a0a0.cpp:15` is the `drawmode=DM_USEDEFAULT`.

### 15b. Character-object vtable offsets (on the CHARACTER, NOT the surface)

These appear in TPlyrStatusBar slot23 as `(**(code**)(*charObj + 0xNNN))()`. They are stat getters on the character/object class. Full evidence in `TPlyrStatusBar_SPEC.md:252-265` (§8 stat-getter map):

| Offset | Meaning | Evidence |
|---|---|---|
| `+0x130` | **live character render surface** (the face) | `helper_PlayerSide_54a0a0.cpp:16`; `helper_TargetSide_54a310.cpp:19` |
| `+0x1c0` | **Health** (current) | `TPlyrStatusBar_SPEC.md:255` |
| `+0x1c8` | **Fatigue** (stamina) | `TPlyrStatusBar_SPEC.md:257` |
| `+0x1d0` | **Mana** | `TPlyrStatusBar_SPEC.md:256` |
| `+0x1d8` | HP **bar source-rect** getter | `TPlyrStatusBar_SPEC.md:259` |
| `+0x1e0` | FT **bar source-rect** getter | `TPlyrStatusBar_SPEC.md:261` |
| `+0x1e8` | MP **bar source-rect** getter | `TPlyrStatusBar_SPEC.md:260` |
| `+0x354` | **Level** | `TPlyrStatusBar_SPEC.md:258` |

> CRITICAL: `+0x1c0`/`+0x1c8`/`+0x1d0`/`+0x1d8`/`+0x1e0`/`+0x1e8`/`+0x354` are on the **object**, not the surface — a tracer who confuses these with surface slots will mis-spec the panel. Always check what the `*(…)` base is: a panel buffer field (`+0x6c`/`+0x70`) → surface vtable; the player/target char pointer (`DAT_00667fcc`, `piStack_e8`) → object vtable.

### 15c. Offsets the brief asked about but that need care
- **`+0x1c0/+0x1c8/+0x1d0/+0x354`** — object stat getters (15b), **NOT** surface methods. Confirmed.
- **`+0x130`** — object's live-render surface getter (15b). Confirmed.
- **`+0x14`, `+0x38`, `+0x44`, `+0x5c`, `+0x64`** — surface methods (15a). Confirmed.

---

## 16. Chroma key / pixel format

- **Convention:** magenta is the implicit transparency key. `0xFF00FF` in RGB888 = **`0xF81F = 63519`** in RGB565 = **`0x7C1F = 31775`** in RGB555 (R=31,G=0,B=31).
- **Which format Revenant uses:** the retail HUD sprites are **15-bit (RGB555)** (`BM_15BIT 0x2`), so the live key is **`0x7C1F`**. Confirmed in the port's decoder: `src/bitmapdecode.cpp:59-66` — `constexpr uint16_t MAGENTA_KEY = 0x7c1f;` with the comment that the bitmap's `keycolor` field is often 0 even when magenta is intended, so magenta is treated as transparent globally. The dump tool agrees: `tools/ui/dump_dat.py:112` `MAGENTA = 0x7c1f`.
- **Bit-depth flags** (`src/revdefs.h` BM_*): `BM_15BIT=0x2`, `BM_16BIT` (companion), `BM_5BITPAL=0x1000`, `BM_COMPRESSED=0x4000`, `BM_CHUNKED=0x8000`. The 16-bit (RGB565) variant keys on `0xF81F`.
- **Per-surface override:** `TSurface::KeyColor()/SetKeyColor()` (`src/surface.h:75-76`); `SDrawBlock.keycolor` (`src/graphics.h:51`). When a bitmap declares a non-magenta key (e.g. TSideTabsPane strips use `kc=0x0` and rely on **alpha**, not chroma — `TSideTabsPane_SPEC.md:63,290`), do **not** apply the global magenta key or you punch holes in black pixels.
- **Known pink-halo artifact:** the antialiased text/shadow edge pixels sample the magenta-cleared cell and the magenta is keyed out, leaving a pink fringe. This is a **retail bug, do not reproduce.** Full analysis lives in `docs/ui/specs/TPlyrStatusBar_SPEC.md:295` (§10 item 1) — reference it, do not re-derive. The fix is to render text/sprites with real alpha (no magenta colorkey).
- **Port mapping:** `Renderer->Composite(..., chroma_key=true, chroma_key_rgb=...)` (`src/renderer.h:673-680`) supports per-call chroma key for the cases that genuinely need it; prefer real alpha for HUD sprites that carry it.

---

## 17. UNCONFIRMED / open questions

1. **`FUN_004bd680` body (§4).** No extracted drop. Signature `(x, y, resource, drawmode, 0)` is consistent across 6 call sites, and drawmode literals decode cleanly (`0x80000000`=opaque, `0x2000`=alpha). The trailing `0` arg and the "implicit current surface" mechanism are inferred. **Resolve:** `DecompileAddr.java 0x004bd680`.

2. **`FUN_0046d710` body + dual call shape (§9).** Used both by-name (`"BackPanel"`) and as a container-iterator (TSideTabsPane). Whether this is type-dispatch, two overloads, or an iterator with a hidden index arg is unknown. Handle struct `{width, height, …}` is inferred from the Ring centering math. **Resolve:** `DecompileAddr.java 0x0046d710` (a.k.a. `cls_0x46d6b0::meth_0x46d710`).

3. **`FUN_0058b100` body (§10).** Confirmed buffer-first variadic format function (sprintf family) from call shapes + format literals. Exact CRT identity / return value unread. **Resolve:** `DecompileAddr.java 0x0058b100`.

4. **`FUN_004be2b0` HDC sub-struct offsets (§5).** The line-metric reads `hdc[0x14..0x17].unused`, the mode test `hdc[8].unused != 2`, and the default-glyph fields `hdc[10..0x12].unused` are read literally but their precise GDI/font-table meaning is inferred. The DrawTextA flag construction, the `param_8 & 0x400` shadow gate, and the 3-pass shadow are HIGH confidence. **Resolve:** cross-reference the font-table init that populates `DAT_0065b020`.

5. **`FUN_004be2b0` color source: `param_7` vs `param_11` (§5).** The colored pass uses `param_7` byte-swapped (`:378`); the original brief listed `param_11=color`. `param_11` is the surface drawmode word in the body. Treat `param_7` as the text color (RGB→BGR) and `param_11` as the composite drawmode. **Resolve:** check a TPlyrStatusBar slot23 call site arg order against the cached white-pack (`FUN_00429950`).

6. **`FUN_004aaeb0_BlitEffect_Apply` inner switch (§8.6).** The 15-case `switch(local_90)` is characterized as overdraw-avoidance rect subtraction (the recursion blits sub-rects) rather than distinct effects. Each individual case's geometry was not verified branch-by-branch (low value — the GPU port discards the registry). **Resolve only if** a panel needs exact retail dirty-rect parity (unlikely).

7. **`FUN_00414550` (the inner rasterizer called by `FUN_00414d70`).** Not extracted; it does the actual per-pixel blit/stretch. The outer `FUN_00414d70` clipping/tiling is HIGH confidence; the pixel kernel is a black box here. **Resolve:** `DecompileAddr.java 0x00414550` if exact blend math (e.g. alpha rounding) matters.

8. **`FUN_0054a5d0` param↔atlas-band mapping (§11).** Inherited UNCONFIRMED-D from `TPlyrStatusBar_SPEC.md:382` — the 14 kernel params don't map 1:1 to the 10 call-site literals; cap width, full width, and bright/dim band selection are inferred by color/width match. **Resolve:** extract the char-object bar-source getters `vtable[0x1d8/0x1e8/0x1e0]`.

9. **Surface vtable slots `+0x08/+0x0c/+0x10/+0x58/+0x68` (§15a).** Mapped by behavior (format getter, data accessors, param-blit, finalize) but not against a dumped surface vtable (the recon `*vtable.txt` files are panes, not surfaces). Confidence MEDIUM. **Resolve:** `DumpVtable.java` on the retail `TSurface`/`TDisplay` vtable address and align slot order with `src/surface.h`.
