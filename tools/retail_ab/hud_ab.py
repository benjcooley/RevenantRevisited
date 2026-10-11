#!/usr/bin/env python3
"""HUD A/B against the shipped game's own code (docs/ui/HUD_REBUILD.md).

    hud_ab.py TARGET [--out DIR] [--port BIN] [--only NAME ...]

One command per target. Per case it runs the port's production pane in a
headless --test=ab-* host, which reports what the pane showed (names,
stats, the portraits' bitmaps), then runs retail's original pane on exactly
those inputs in the emulator's HUD slot (one persistent fixture process: it
loads the boot image and restores a checkpoint per case). It compares the
two frames and writes, per case, a retail | port | diff triptych. Then it
prints a summary. DIR/report.json records the retail build hash, the port
binary hash, the case set and every case's result.

Targets:
  statusbar  TPlyrStatusBar (0x0065a8c0): the player's and opponent's chips
             over stat values, fills that hit the bar caps, target present /
             absent, and each fade tick.

             Text: the emulator draws no GDI glyphs yet (HUD_REBUILD.md P6),
             so the frames are compared outside retail's text cells, which
             retail blits once a chip passes half fade. Port pixels that
             differ within TEXT_BAND of a cell are reported as text overflow,
             open until P6, apart from the chrome defects.

             Retail composes the chips into its textures by its own code;
             the fixture draws the overlay quads as the D3D device does
             (overlayraster.py), fades included.
  bottombar  TBottomBarPane (0x0065b638) with the quick-spell rings and the
             potion shelf: rings empty, grey and castable, long spell names,
             still and animated items at several game frames, pouches,
             amounts (ammo, money), the bar at 640 and at 452 wide. A ring
             is castable on both sides when the port's player holds its
             talismans (the port reports which). Retail draws it in 2D
             straight onto the screen; every pixel outside its text cells
             (the GDI glyphs again, P6) must match exactly, but for the ones
             the fixture marks `alpha` (below).
  inventory  TInventory (0x0065d4f8) in the side panel's lower slot: the
             gold readout (0, 1, 64, large; the "Gold" bitmap font, which
             the emulator draws, so it is compared like any pixel), stacks,
             pouches, animated items, pages, an opened bag (its icon at
             half size). Compared as the bottom bar is.
  automap    TAutoMap (0x0065b4f0) in the same slot: its frame, with "+"
             and "-" released and held. Retail draws it with overlay quads
             (the D3D device's way, overlayraster.py): pixels a quad's alpha
             mixed are the `screen` class. The map inside needs a level in
             the fixture; not compared yet.
  spellpane  TSpellPane (0x006661b0) in the same slot: no pouch, an empty
             one, a few talismans and all twelve with names (the arrows)
             and without, first lines, spells of 1..5 talismans (lower
             case too), a pressed talisman and the held "spell" button.
             Compared as the bottom bar is; the names are GDI text.

Both frames are quantised to the 16-bit screen retail draws (RGB565). A
pixel must match exactly, unless the fixture marks it (the masks of
tools/retail_runtime/slots/hud/plyrstatusbar.py):
- `screen`: a quad's blend mixed it (partial alpha, or a fade). Both sides
  blend in float (the port's layers are stored premultiplied in 8 bits
  first); they match within SCREEN_TOLERANCE, one RGB565 step (8 or 9 of
  255: the channels widen by bit replication).
- `composed`: it is drawn from a texel retail blended into its ARGB4444
  texture in 4-bit steps (blendmap.py); the port blends those layers in
  float (HUD_REBUILD.md section 5). They match within COMPOSED_TOLERANCE:
  two nested 4-bit truncations (17 each: the Ring over the portrait over
  the BackPanel) and the RGB565 step.
- `alpha` (itempanes.py): a 2D DM_ALPHA Put mixed it, or a bitmap glyph's
  anti-aliased edge (DM_ALIAS), coverage strictly between 0 and 31. Retail mixes through 5-bit scale tables (0x004b3694):
  d * (31 - a) // 31 + s * a // 31 per field, green in two 3-bit halves --
  two truncations a field, four on green. The port blends in float. Over
  every source, destination and coverage the two are at most ALPHA_TOLERANCE
  apart, two RGB565 steps.
A channel off by more is a defect.

Environment / defaults:
  RETAIL_PY   the emulator's Python (the retail-asm venv)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import subprocess
import sys
import time
import zlib
from pathlib import Path

import numpy as np
from PIL import Image

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
SLOT = REPO / 'tools' / 'retail_runtime' / 'slots' / 'hud'
RETAIL_EXE = REPO / 'recon' / 'retail_asm' / 'baseline' / 'Revenant.rebuilt.exe'
RETAIL_PY = Path(os.environ.get('RETAIL_PY', '/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python'))
PORT = REPO / 'build' / 'Revenant'

SCREEN_TOLERANCE = 9
COMPOSED_TOLERANCE = 2 * 17 + SCREEN_TOLERANCE
ALPHA_TOLERANCE = 17              # two RGB565 steps: the worst case of retail's 2D DM_ALPHA tables
SCREEN = (640, 480)
TRIPTYCH_ROWS = 128               # the strip of the screen a triptych shows
TEXT_SHADOW = 2                   # the black shadow reaches 2 px right of and below a cell's text
TEXT_BAND = 3                     # port text that strays this far outside a cell is text overflow


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


# =====================================================================
# Fixture processes
# =====================================================================

class Fixture:
    """A slot fixture speaking JSONL ({"id", "case", ...} -> {"id", "ok", ...})."""

    def __init__(self, script, *args):
        self.process = subprocess.Popen([str(RETAIL_PY), str(script), str(RETAIL_EXE), *args],
                                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.hello = json.loads(self.process.stdout.readline())

    def run(self, case_id, case, **fields):
        self.process.stdin.write(json.dumps(dict(id=case_id, case=case, **fields)) + '\n')
        self.process.stdin.flush()
        return json.loads(self.process.stdout.readline())

    def close(self):
        self.process.stdin.close()
        self.process.wait(timeout=30)


# =====================================================================
# Frames
# =====================================================================

def quantise_565(rgb):
    """8-bit RGB -> the RGB565 screen retail draws, expanded back to 8 bits
    the way the capture PNGs are (bit replication)."""
    r, g, b = rgb[..., 0] >> 3, rgb[..., 1] >> 2, rgb[..., 2] >> 3
    return np.stack([(r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)], axis=-1)


def retail_frame(path):
    words = np.frombuffer(Path(path).read_bytes(), dtype='<u2').reshape(SCREEN[1], SCREEN[0]).astype(np.int32)
    r, g, b = words >> 11, (words >> 5) & 0x3f, words & 0x1f
    return np.stack([(r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)], axis=-1)


def port_frame(path):
    image = Image.open(path).convert('RGB')
    if image.size != SCREEN:
        raise ValueError(f'{path}: {image.size}, not the {SCREEN} Classic screen')
    return quantise_565(np.asarray(image).astype(np.int32))


def compare(retail, port, mask, band, tolerance):
    """Per-pixel channel deltas outside `mask` (True = not compared), each
    allowed up to `tolerance` there; one over that inside `band` counts as
    text overflow, elsewhere as a defect."""
    delta = np.abs(retail - port).max(axis=-1)
    delta[mask] = 0
    over = delta > tolerance
    bad = over & ~band
    worst = lambda where: int(np.where(where & ~band, delta, 0).max())  # noqa: E731
    result = dict(compared=int((~mask).sum()), exact=int(((delta == 0) & ~mask).sum()),
                  within_tolerance=int(((delta > 0) & ~over).sum()), defects=int(bad.sum()),
                  text_overflow=int((over & band).sum()),
                  max_delta=dict(exact=worst(tolerance == 0), screen=worst(tolerance == SCREEN_TOLERANCE),
                                 composed=worst(tolerance == COMPOSED_TOLERANCE),
                                 alpha=worst(tolerance == ALPHA_TOLERANCE)))
    if bad.any():
        ys, xs = np.nonzero(bad)
        result['defect_bbox'] = [int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1]
        y, x = int(ys[0]), int(xs[0])
        result['first_defect'] = dict(x=x, y=y, retail=retail[y, x].tolist(), port=port[y, x].tolist())
    return result, delta


def triptych(path, retail, port, delta, mask, band, tolerance, rows=slice(0, TRIPTYCH_ROWS)):
    """retail | port | diff, stacked, the screen `rows`, 2x. The diff shows
    deltas within tolerance in grey, defects in red, text overflow in yellow
    and the masked text cells in blue."""
    diff = np.zeros_like(retail[rows])
    d, m, b = delta[rows], mask[rows], band[rows]
    over = d > tolerance[rows]
    diff[(d > 0) & ~over] = (96, 96, 96)
    diff[over & ~b] = (255, 48, 48)
    diff[over & b] = (230, 200, 40)
    diff[m] = (24, 32, 96)
    strip = np.concatenate([retail[rows], port[rows], diff], axis=0).astype(np.uint8)
    image = Image.fromarray(strip, 'RGB')
    image.resize((image.width * 2, image.height * 2), Image.NEAREST).save(path)


# =====================================================================
# statusbar
# =====================================================================

# (name, player fills, opponent fills or None, ticks). Fills are health,
# mana and fatigue as fractions of the port's demo characters' maxima; the
# port reports the values they come to and retail runs on those. 'near-caps'
# and 'far-caps' land a bar's fill on either side of the cap thresholds;
# 'fade-N' stops at tick N of the six-tick fade (text shows from tick 4).
SAMPLE = ((0.95, 0.82, 0.50), (0.40, 0.10, 0.60))
STATUSBAR_CASES = [
    ('sample', *SAMPLE, 6),
    ('no-target', (1.0, 1.0, 1.0), None, 6),
    ('full', (1.0, 1.0, 1.0), (1.0, 1.0, 1.0), 6),
    ('empty', (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), 6),
    ('near-caps', (0.02, 0.03, 0.04), (0.03, 0.02, 0.05), 6),
    ('far-caps', (0.951, 0.95, 0.93), (0.95, 0.951, 0.97), 6),
    *[(f'sweep-{i:02d}', (i / 10, (10 - i) / 10, (i * 7 % 11) / 10),
       ((10 - i) / 10, i / 10, (i * 3 % 11) / 10), 6) for i in range(1, 10)],
    *[(f'fade-{tick}', *SAMPLE, tick) for tick in range(1, 6)],
]


def fills(values):
    return ','.join(f'{v:g}' for v in values)


def run_port_statusbar(port, name, player, target, ticks, workdir):
    """The production pane in --test=ab-plyrstatusbar -> (shown, frame path)."""
    shown, frame, log = (workdir / f'{name}.port.json', workdir / f'{name}.port.png',
                         workdir / f'{name}.port.log')
    for stale in (shown, frame):
        stale.unlink(missing_ok=True)
    spec = f'player={fills(player)};target={fills(target) if target else "none"};ticks={ticks}'
    with log.open('wb') as handle:
        subprocess.run([str(port), '--headless', '--test=ab-plyrstatusbar', f'--ab-case={spec}',
                        f'--ab-out={shown}', f'--snap={frame}', '--snapstep=0.03125', '--snapwarmup=12',
                        '--max-runtime=60'], cwd=REPO, stdout=handle, stderr=handle, timeout=120, check=False)
    if not shown.is_file() or not frame.is_file():
        raise RuntimeError(f'port wrote no {shown.name if not shown.is_file() else frame.name} (see {log})')
    return json.loads(shown.read_text()), frame


def text_cells(primitives):
    """The screen rects retail blits text cells to (its 2D pass, once a chip
    passes half fade) -> (mask: the cells widened by the shadow, band: the
    TEXT_BAND around the mask, cells)."""
    mask = np.zeros((SCREEN[1], SCREEN[0]), dtype=bool)
    band = np.zeros_like(mask)
    cells = []
    for p in primitives:
        if p['primitive'] == 'ParamBlit' and p['dst'] == 'display' and p['src'] == 'text':
            x, y, w, h = p['drawparam'][10:14]
            if [x, y, w, h] not in cells:
                cells.append([x, y, w, h])
                mask[y:y + h + TEXT_SHADOW, x:x + w + TEXT_SHADOW] = True
                band[max(0, y - TEXT_BAND):y + h + TEXT_SHADOW + TEXT_BAND,
                     max(0, x - TEXT_BAND):x + w + TEXT_SHADOW + TEXT_BAND] = True
    return mask, band & ~mask, cells


def screen_mask(packed):
    """A fixture mask: one byte per screen pixel, zlib, hex."""
    return np.frombuffer(zlib.decompress(bytes.fromhex(packed)), dtype=np.uint8).reshape(
        SCREEN[1], SCREEN[0]).astype(bool)


def tolerances(blend):
    """Per pixel: COMPOSED_TOLERANCE, SCREEN_TOLERANCE or 0 (exact)."""
    return np.where(screen_mask(blend['composed']), COMPOSED_TOLERANCE,
                    np.where(screen_mask(blend['screen']), SCREEN_TOLERANCE, 0))


def target_statusbar(args, workdir):
    cases = [c for c in STATUSBAR_CASES if not args.only or c[0] in args.only]
    retail = Fixture(SLOT / 'plyrstatusbar.py', '--serve')
    results, timing = [], dict(port_seconds=0.0, retail_seconds=0.0)
    for name, player, target, ticks in cases:
        entry = dict(case=name, ticks=ticks)
        try:
            started = time.perf_counter()
            shown, port_png = run_port_statusbar(args.port, name, player, target, ticks, workdir)
            timing['port_seconds'] += time.perf_counter() - started
            retail_case = dict(name=name, player=shown['player'], target=shown['target'],
                               frames=shown['ticks'], pane_width=shown['pane'][2])
            started = time.perf_counter()
            r = retail.run(name, retail_case, out=str(workdir / 'retail'))
            timing['retail_seconds'] += time.perf_counter() - started
            if not r.get('ok'):
                raise RuntimeError(f'retail: {r.get("error")}')
            result = r['result']
            mask, band, cells = text_cells(result['primitives'])
            retail_rgb = retail_frame(workdir / 'retail' / f'{name}.rgb565')
            port_rgb = port_frame(port_png)
            tolerance = tolerances(result['blend'])
            diff, delta = compare(retail_rgb, port_rgb, mask, band, tolerance)
            triptych(workdir / f'{name}.triptych.png', retail_rgb, port_rgb, delta, mask, band, tolerance)
            entry.update(diff, fades=result['fades'], text_cells=cells,
                         shown={side: None if not shown[side] else
                                {k: v for k, v in shown[side].items() if k != 'portrait'}
                                for side in ('player', 'target')})
        except Exception as error:      # one case's failure is reported, not fatal
            entry['error'] = f'{type(error).__name__}: {error}'
        results.append(entry)
    retail.close()
    return dict(retail_sha256=retail.hello.get('retail_sha256'), tolerance=dict(screen=SCREEN_TOLERANCE, composed=COMPOSED_TOLERANCE),
                case_set=[dict(name=c[0], player=c[1], target=c[2], ticks=c[3]) for c in cases],
                timing={k: round(v, 2) for k, v in timing.items()}, results=results)


# =====================================================================
# bottombar
# =====================================================================

# Belt slots: None empty, else (type, amount or None, contents or None). No
# two items of a type on one belt, a pouch's included: retail's AddToInventory
# stacks them.
LONG_NAMES = ['BJ', 'MAL', 'BLE', 'MH']           # Regeneration, HighPriest Bolt, ...
RINGS = ['LI', 'KJL', 'DEB', 'FL']                # Fire Flash, IronSkin, Advanced healing, Shadow Fist
STILL = [('Golden Sun Key', None, None), ('Ale Mug', None, None)]
POUCH = ('Pouch', None, [('Lesser Healing', None, None), ('Ale Mug', None, None)])
FULL = [('Lesser Healing', None, None), ('Golden Sun Key', None, None), POUCH, ('Arrow', 7, None),
        ('Gold', 42, None), None, ('Brown Leather Helmet', None, None), None, None]
BOTTOMBAR_CASES = [
    dict(name='empty-640', width=640),
    dict(name='empty-452', width=452),
    dict(name='rings-grey', width=452, spells=RINGS),
    dict(name='rings-castable', width=452, spells=RINGS, castable=['LI', 'DEB']),
    dict(name='rings-long-names', width=452, spells=LONG_NAMES, castable=['BJ']),
    dict(name='belt-still', width=640, belt=STILL),
    *[dict(name=f'belt-animated-{frame:02d}', width=640, belt=[('Lesser Healing', None, None)], frame=frame)
      for frame in (0, 1, 2, 3, 7, 13)],
    dict(name='pouch', width=640, belt=[None, POUCH]),
    dict(name='pouch-one', width=640, belt=[('Pouch', None, [('Golden Sun Key', None, None)])]),
    dict(name='amounts', width=640, belt=[('Arrow', 3, None), ('Gold', 42, None), None, ('Arrow', 12, None)]),
    dict(name='amounts-large', width=640, belt=[('Arrow', 45, None), ('Gold', 700, None)]),
    dict(name='pouch-gold', width=640, belt=[('Pouch', None, [('Gold', 42, None)]), None,
                                             ('Pouch', None, [('Arrow', 9, None)])]),
    dict(name='full-640', width=640, spells=RINGS, castable=['LI'], belt=FULL, frame=5),
    dict(name='full-452', width=452, spells=RINGS, castable=['LI'], belt=FULL, frame=5),
]


def port_item(item):
    if item is None:
        return '-'
    kind, amount, contents = item
    text = kind + (f'*{amount}' if amount else '')
    return text + (f'({",".join(port_item(c) for c in contents)})' if contents else '')


def retail_item(item):
    if item is None:
        return None
    kind, amount, contents = item
    entry = dict(type=kind)
    if amount:
        entry['amount'] = amount
    if contents:
        entry['contents'] = [retail_item(c) for c in contents]
    return entry


def run_port_pane(port, mode, spec, name, workdir):
    """A production pane in --test=<mode> on one case -> (shown, frame path)."""
    shown, frame, log = (workdir / f'{name}.port.json', workdir / f'{name}.port.png',
                         workdir / f'{name}.port.log')
    for stale in (shown, frame):
        stale.unlink(missing_ok=True)
    with log.open('wb') as handle:
        subprocess.run([str(port), '--headless', f'--test={mode}', f'--ab-case={spec}', f'--ab-out={shown}',
                        f'--snap={frame}', '--snapstep=0.03125', '--snapwarmup=12', '--max-runtime=60'],
                       cwd=REPO, stdout=handle, stderr=handle, timeout=120, check=False)
    if not shown.is_file() or not frame.is_file():
        raise RuntimeError(f'port wrote no {shown.name if not shown.is_file() else frame.name} (see {log})')
    return json.loads(shown.read_text()), frame


DT_CENTER, DT_RIGHT, DT_VCENTER, DT_BOTTOM = 0x1, 0x2, 0x4, 0x8


def text_cell(call):
    """The rect a GDI text call's glyphs can reach. A left- and top-aligned
    call laid out in an open-ended rect (the spell pane's names: (x, y, x +
    10000, y + 10000)) reaches no further than a cell height a character
    across and a cell height a line down (the font's height, "Arial 12")."""
    left, top, right, bottom = call['rect']
    font = call.get('font') or ''
    size = font.rsplit(' ', 1)[-1]
    if not size.isdigit():
        return [left, top, right, bottom]
    cell = int(size)
    lines = call['text'].split('\n')
    if right - left > SCREEN[0] and not call['format'] & (DT_CENTER | DT_RIGHT):
        right = left + cell * max(len(line) for line in lines)
    if bottom - top > SCREEN[1] and not call['format'] & (DT_VCENTER | DT_BOTTOM):
        bottom = top + cell * len(lines)
    return [left, top, right, bottom]


def gdi_text_cells(text_calls):
    """The screen rects retail's GDI text calls lay text in (each shadow pass
    has its own; text_cell) -> (mask, band: the TEXT_BAND around the mask,
    cells). Its bitmap fonts the emulator draws, so they are compared like
    any pixel."""
    mask = np.zeros((SCREEN[1], SCREEN[0]), dtype=bool)
    band = np.zeros_like(mask)
    cells = []
    for call in text_calls:
        left, top, right, bottom = text_cell(call)
        if [left, top, right, bottom] in cells:
            continue
        cells.append([left, top, right, bottom])
        mask[max(0, top):bottom, max(0, left):right] = True
        band[max(0, top - TEXT_BAND):bottom + TEXT_BAND, max(0, left - TEXT_BAND):right + TEXT_BAND] = True
    return mask, band & ~mask, cells


# 0x004be2b0's flags: the alignment in the low bits, a drop shadow (black
# passes at (1, 1), (2, 1), (1, 2) under the colour) at 0x400.
TEXT_ALIGN = {1: 'left', 2: 'center', 4: 'right'}
TEXT_SHADOWED = 0x400
# FONT.DEF in order (0x004be2b0's font is 1-based): the names the port
# records for a bitmap font; a GDI font is named as the emulator names it.
FONT_DEF = [None, 'System', 'Dialog', 'Scroll', 'Book', 'Numbers', 'Small', 'Med', 'Large', 'Gold', 'GoldMed',
            'SpellTitle', 'Version']


def surface_origins(primitives, pane):
    """Where each surface a pane composes lands in it: {name: (dx, dy, clip)}
    -- the display at the pane's origin, an offscreen surface where it is
    blitted to the display (clip: that blit's rect), all pane-local."""
    whole = (0, 0, pane[2], pane[3])
    origins = {'display': (0, 0, whole)}
    for p in primitives:
        if p['primitive'] == 'ParamBlit' and p['dst'] == 'display' and p['src'] and p['src'] not in origins:
            d = p['drawparam']
            x, y, w, h, sx, sy = d[10:16]
            origins[p['src']] = (x - sx, y - sy, (x, y, x + w, y + h))
    return origins


def text_draws(result):
    """Retail's text composes (0x004be2b0), pane-local, each with the lines
    its GDI colour pass laid out: [{text, face, rect, align, color,
    shadowed, wrapped, lines: [[x, y, line]], clip}]. A compose draws its
    string into the text surface at (0, 0) -- three black passes first when
    shadowed -- then blits it to `rect` on its surface."""
    origins = surface_origins(result['primitives'], result['pane'])
    calls = iter(result['text_calls'])
    draws = []
    for p in result['primitives']:
        if p['primitive'] != 'Text':
            continue
        shadowed = bool(p['format'] & TEXT_SHADOWED)
        gdi = None
        for _ in range(4 if shadowed else 1):
            gdi = next(calls, None)
        if gdi is None or gdi['text'] != p['text']:
            raise RuntimeError(f'retail text {p["text"]!r} has no GDI calls of its own')
        dx, dy, clip = origins.get(p['dst'], (None, None, None))
        if dx is None:
            raise RuntimeError(f'retail text {p["text"]!r} lands on {p["dst"]}, never shown')
        x, y, w, h = p['rect']
        color = 0xffffff if p['color'] == 0xffffffff else p['color']
        draws.append(dict(text=p['text'], face=gdi['font'], rect=[x + dx, y + dy, w, h],
                          align=TEXT_ALIGN.get(p['format'] & 7, hex(p['format'] & 7)),
                          color=[color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff], shadowed=shadowed,
                          wrapped=bool(gdi['format'] & 0x10),
                          lines=[[lx + x + dx, ly + y + dy, line, width] for lx, ly, line, width in gdi['layout']],
                          clip=list(clip), cell=int(gdi['font'].rsplit(' ', 1)[-1])))
    return draws


def text_draw_mask(draws, pane):
    """The screen pixels retail's text can touch -> (mask, band): each line's
    cell (its width, the font's height) and its shadow, inside the surface's
    visible rect."""
    mask = np.zeros((SCREEN[1], SCREEN[0]), dtype=bool)
    band = np.zeros_like(mask)
    shadow = TEXT_SHADOW
    for draw in draws:
        l, t, r, b = draw['clip']
        for x, y, _line, width in draw['lines']:
            x0, y0 = max(x, l) + pane[0], max(y, t) + pane[1]
            x1, y1 = min(x + width + shadow, r) + pane[0], min(y + draw['cell'] + shadow, b) + pane[1]
            if x1 > x0 and y1 > y0:
                mask[y0:y1, x0:x1] = True
                band[max(0, y0 - TEXT_BAND):y1 + TEXT_BAND, max(0, x0 - TEXT_BAND):x1 + TEXT_BAND] = True
    return mask, band & ~mask


def compare_text(retail, port):
    """Retail's text composes against the port's text draws (the A/B host's
    `text`, STextDraw): each must be there, the same font, place, colour and
    lines. -> {matched, missing, extra, mismatched: [{text, field: [retail,
    port]}]}"""
    unmatched = list(port)
    result = dict(matched=0, missing=[], extra=[], mismatched=[])
    for draw in retail:
        same = [d for d in unmatched if d['text'] == draw['text']]
        if not same:
            result['missing'].append(draw['text'])
            continue
        mine = min(same, key=lambda d: abs(d['x'] - draw['rect'][0]) + abs(d['y'] - draw['rect'][1]))
        unmatched.remove(mine)
        fields = dict(face=(draw['face'], mine['face']),
                      rect=(draw['rect'], [mine['x'], mine['y'], mine['w'], mine['h']]),
                      align=(draw['align'], mine['align']), color=(draw['color'], mine['color']),
                      shadowed=(draw['shadowed'], mine['shadowed']), wrapped=(draw['wrapped'], mine['wrapped']))
        if draw['wrapped']:
            fields['lines'] = ([line for _x, _y, line, _w in draw['lines']], mine.get('lines', []))
        wrong = {k: list(v) for k, v in fields.items() if v[0] != v[1]}
        if wrong:
            result['mismatched'].append(dict(text=draw['text'][:40], **wrong))
        else:
            result['matched'] += 1
    result['extra'] = [d['text'] for d in unmatched]
    return result


def pane_target(args, workdir, cases, fixture, run_port, retail_case, report,
                masks=(('alpha', ALPHA_TOLERANCE),), composed_text=False):
    """A pane's A/B over `cases`: run_port(case) -> (shown, frame);
    retail_case(case, shown) -> the fixture's case; report(shown) -> what the
    case's result records of it. Every pixel outside its GDI text must match
    exactly, but for the ones the fixture's blend `masks` mark: the 2D panes'
    `alpha` (ALPHA_TOLERANCE), the overlay panes' `screen`
    (SCREEN_TOLERANCE). A pane whose text retail composes (0x004be2b0;
    `composed_text`) masks where those composes can draw and compares them
    with the port's text draws (compare_text) -- a mismatch is a defect."""
    cases = [c for c in cases if not args.only or c['name'] in args.only]
    retail = Fixture(SLOT / fixture, '--serve')
    results, timing = [], dict(port_seconds=0.0, retail_seconds=0.0)
    for case in cases:
        name = case['name']
        entry = dict(case=name)
        try:
            started = time.perf_counter()
            shown, port_png = run_port(case)
            timing['port_seconds'] += time.perf_counter() - started
            started = time.perf_counter()
            r = retail.run(name, dict(retail_case(case, shown), name=name), out=str(workdir / 'retail'))
            timing['retail_seconds'] += time.perf_counter() - started
            if not r.get('ok'):
                raise RuntimeError(f'retail: {r.get("error")}')
            result = r['result']
            pane = result['pane']
            if pane != shown['pane']:
                raise RuntimeError(f'pane {shown["pane"]} in the port, {pane} in retail')
            if composed_text:
                draws = text_draws(result)
                mask, band = text_draw_mask(draws, pane)
                cells = [d['rect'] for d in draws]
                entry['text'] = compare_text(draws, shown.get('text', []))
            else:
                mask, band, cells = gdi_text_cells(result['text_calls'])
            retail_rgb = retail_frame(workdir / 'retail' / f'{name}.rgb565')
            port_rgb = port_frame(port_png)
            tolerance = np.zeros((SCREEN[1], SCREEN[0]), dtype=np.int32)
            for key, value in masks:
                tolerance = np.maximum(tolerance, np.where(screen_mask(result['blend'][key]), value, 0))
            diff, delta = compare(retail_rgb, port_rgb, mask, band, tolerance)
            rows = slice(max(0, pane[1] - 16), min(SCREEN[1], pane[1] + pane[3] + 16))
            triptych(workdir / f'{name}.triptych.png', retail_rgb, port_rgb, delta, mask, band, tolerance, rows)
            entry.update(diff, text_cells=cells, **report(shown))
        except Exception as error:      # one case's failure is reported, not fatal
            entry['error'] = f'{type(error).__name__}: {error}'
        results.append(entry)
    retail.close()
    return dict(retail_sha256=retail.hello.get('retail_sha256'), tolerance=dict(masks),
                case_set=cases, timing={k: round(v, 2) for k, v in timing.items()}, results=results)


def target_bottombar(args, workdir):
    def run_port(case):
        spells = (case.get('spells', []) + [''] * 4)[:4]
        spec = (f'width={case["width"]};spells={",".join(spells)};castable={",".join(case.get("castable", []))};'
                f'belt={"|".join(port_item(i) for i in case.get("belt", []))};frame={case.get("frame", 0)}')
        return run_port_pane(args.port, 'ab-bottombar', spec, case['name'], workdir)

    def retail_case(case, shown):
        rings = shown['rings']
        return dict(pane_width=shown['pane'][2], tick=shown['frame'],
                    player=dict(quickspells=[r['talismans'] for r in rings],
                                castable=[r['talismans'] for r in rings if r['castable']]),
                    belt=[retail_item(i) for i in case.get('belt', [])])

    return pane_target(args, workdir, BOTTOMBAR_CASES, 'bottombar.py', run_port, retail_case,
                       lambda shown: dict(rings=shown['rings'], boxes=shown['boxes']))


# =====================================================================
# inventory
# =====================================================================

# Items: (slot, item) with items as for the bottom bar. No two items of a
# type in one inventory, a pouch's included, except gold, which piles.
INVENTORY_SAMPLE = [(0, ('Lesser Healing', None, None)), (1, ('Ale Mug', None, None)),
                    (2, ('Gold', 14475, None)), (3, ('Short Sword', None, None)), (4, ('Arrow', 12, None)),
                    (5, ('Pouch', None, [('Golden Sun Key', None, None), ('Brown Leather Helmet', None, None)]))]
INVENTORY_FULL = [(slot, item) for slot, item in enumerate([
    ('Lesser Healing', None, None), ('Ale Mug', None, None), ('Gold', 42, None), ('Short Sword', None, None),
    ('Arrow', 31, None), ('Pouch', None, [('Golden Sun Key', None, None)]), ('Brown Leather Helmet', None, None),
    ('Brown Cloth Pants', None, None), ('Red Cloth Shirt', None, None), ('Brown Leather Boots', None, None),
    ('Brown Leather Gloves', None, None), ('Brown Leather Chest Plate', None, None)])]
INVENTORY_CASES = [
    dict(name='empty'),
    dict(name='sample', items=INVENTORY_SAMPLE, frame=3),
    dict(name='full', items=INVENTORY_FULL, frame=7),
    dict(name='gold-one', items=[(0, ('Gold', 1, None))]),
    dict(name='gold-pile', items=[(0, ('Gold', 64, None)), (4, ('Arrow', 45, None))]),
    dict(name='gold-large', items=[(11, ('Gold', 9999, None))]),
    *[dict(name=f'animated-{frame:02d}', items=[(4, ('Lesser Healing', None, None))], frame=frame)
      for frame in (0, 5, 11)],
    dict(name='pouches', items=[(0, ('Pouch', None, [('Gold', 42, None)])), (3, ('Pouch', None, [('Arrow', 9, None)])),
                                (6, ('Pouch', None, [('Ale Mug', None, None)]))]),
    dict(name='page-3', items=[(3, ('Ale Mug', None, None)), (14, ('Golden Sun Key', None, None))], first=3),
    dict(name='page-12', items=[(12, ('Short Sword', None, None)), (23, ('Ale Mug', None, None))], first=12),
    dict(name='page-last', items=[(243, ('Ale Mug', None, None)), (254, ('Golden Sun Key', None, None))], first=243),
    dict(name='bag', items=[(2, ('Pouch', None, [('Golden Sun Key', None, None), ('Arrow', 7, None)]))], bag=2),
]


def target_inventory(args, workdir):
    def run_port(case):
        items = '|'.join(f'{port_item(item)}@{slot}' for slot, item in case.get('items', []))
        spec = f'items={items};first={case.get("first", 0)};frame={case.get("frame", 0)}'
        if case.get('bag') is not None:
            spec += f';bag={case["bag"]}'
        return run_port_pane(args.port, 'ab-inventory', spec, case['name'], workdir)

    def retail_case(case, shown):
        items = [dict(retail_item(item), slot=slot) for slot, item in case.get('items', [])]
        return dict(items=items, bag=case.get('bag'), first=shown['first'], tick=shown['frame'])

    return pane_target(args, workdir, INVENTORY_CASES, 'inventory.py', run_port, retail_case,
                       lambda shown: dict(gold=shown['gold'], cells=shown['cells']))


# =====================================================================
# spellpane
# =====================================================================

# Talismans by code: A Sun, B Life, C Ocean, D Law, E Soul, F Stars, G Death,
# H Chaos, I Sky, J Earth, K Ward, L Moon. One of each type in a pouch
# (retail stacks a second).
TALISMANS = ['Sun', 'Life', 'Ocean', 'Law', 'Soul', 'Stars', 'Death', 'Chaos', 'Sky', 'Earth', 'Ward', 'Moon']


def pouch(*names):
    return (0, ('Spell Pouch', None, [(name, None, None) for name in names]))


SPELLPANE_CASES = [
    dict(name='no-pouch'),
    dict(name='empty-pouch', items=[pouch()], spell='A'),
    dict(name='sample', items=[pouch('Sun', 'Life', 'Moon')], spell='AL'),
    dict(name='spell-1', items=[pouch('Moon')], spell='L'),
    dict(name='spell-3', items=[pouch('Death', 'Chaos', 'Sky')], spell='GHI'),
    dict(name='spell-lower', items=[pouch('Sun', 'Moon')], spell='aL'),
    dict(name='all', items=[pouch(*TALISMANS)], spell='ABCDE'),
    dict(name='all-first-2', items=[pouch(*TALISMANS)], first=2),
    dict(name='all-first-6', items=[pouch(*TALISMANS)], first=6),
    dict(name='all-packed', items=[pouch(*TALISMANS)], names=0, spell='JKL'),
    dict(name='packed-first-4', items=[pouch(*TALISMANS)], names=0, first=4),
    dict(name='pressed', items=[pouch('Sun', 'Life', 'Moon')], pressed=1),
    dict(name='pressed-packed', items=[pouch(*TALISMANS)], names=0, pressed=11),
    dict(name='invoke', items=[pouch('Sun', 'Life', 'Moon')], spell='ABL', invoke=1),
    dict(name='pouch-slot-9', items=[(9, ('Spell Pouch', None, [('Ward', None, None), ('Stars', None, None)]))]),
]


def target_spellpane(args, workdir):
    def run_port(case):
        items = '|'.join(f'{port_item(item)}@{slot}' for slot, item in case.get('items', []))
        spec = (f'items={items};spell={case.get("spell", "")};names={case.get("names", 1)};'
                f'first={case.get("first", 0)};pressed={case.get("pressed", -1)};invoke={case.get("invoke", 0)}')
        return run_port_pane(args.port, 'ab-spellpane', spec, case['name'], workdir)

    def retail_case(case, shown):
        items = [dict(retail_item(item), slot=slot) for slot, item in case.get('items', [])]
        return dict(items=items, spell=shown['spell'], names=bool(shown['names']), first=shown['first'],
                    pressed=shown['pressed'], invoke=bool(shown['invoke']))

    return pane_target(args, workdir, SPELLPANE_CASES, 'spellpane.py', run_port, retail_case,
                       lambda shown: dict(spell=shown['spell'], names=shown['names'], first=shown['first'],
                                          pressed=shown['pressed'], invoke=shown['invoke']))


# =====================================================================
# automap
# =====================================================================

AUTOMAP_CASES = [
    dict(name='released'),
    dict(name='plus-held', plus=1),
    dict(name='minus-held', minus=1),
    dict(name='both-held', plus=1, minus=1),
]


def target_automap(args, workdir):
    def run_port(case):
        spec = f'plus={case.get("plus", 0)};minus={case.get("minus", 0)}'
        return run_port_pane(args.port, 'ab-automap', spec, case['name'], workdir)

    def retail_case(case, shown):
        return dict(plus=bool(shown['plus']), minus=bool(shown['minus']))

    return pane_target(args, workdir, AUTOMAP_CASES, 'automap.py', run_port, retail_case,
                       lambda shown: dict(zoom=shown['zoom'], plus=shown['plus'], minus=shown['minus']),
                       masks=(('screen', SCREEN_TOLERANCE),))


TARGETS = {'statusbar': target_statusbar, 'bottombar': target_bottombar, 'inventory': target_inventory,
           'spellpane': target_spellpane, 'automap': target_automap}


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('target', choices=sorted(TARGETS))
    parser.add_argument('--out', type=Path, default=REPO / 'build' / 'hud_ab')
    parser.add_argument('--port', type=Path, default=PORT)
    parser.add_argument('--only', nargs='*', help='run only the named cases')
    args = parser.parse_args()
    workdir = args.out / args.target
    workdir.mkdir(parents=True, exist_ok=True)
    report = TARGETS[args.target](args, workdir)
    report.update(target=args.target, port_sha256=sha256(args.port))
    (workdir / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    results = report['results']
    text_wrong = lambda r: bool(r.get('text') and (r['text']['missing'] or r['text']['extra']  # noqa: E731
                                                  or r['text']['mismatched']))
    failing = [r for r in results if r.get('error') or r.get('defects') or text_wrong(r)]
    print(f'{args.target}: {len(results) - len(failing)}/{len(results)} cases match '
          f'(port {report["timing"]["port_seconds"]}s, retail {report["timing"]["retail_seconds"]}s); '
          f'report {workdir / "report.json"}')
    for r in results:
        if r.get('error'):
            print(f'  {r["case"]}: {r["error"]}')
        else:
            worst = r['max_delta']
            print(f'  {r["case"]}: {r["defects"]} defects, {r["exact"]}/{r["compared"]} exact, '
                  f'{r["within_tolerance"]} within tolerance (max delta: exact {worst["exact"]}, '
                  f'screen {worst["screen"]}, composed {worst["composed"]}, alpha {worst["alpha"]}), '
                  f'{r["text_overflow"]} text overflow'
                  + (f', defects in {r["defect_bbox"]}' if r.get('defect_bbox') else ''))
            if r.get('text'):
                t = r['text']
                print(f'    text: {t["matched"]} match' + (f', missing {t["missing"]}' if t['missing'] else '')
                      + (f', extra {t["extra"]}' if t['extra'] else ''))
                for wrong in t['mismatched']:
                    print(f'      {json.dumps(wrong)}')
    sys.exit(1 if failing else 0)


if __name__ == '__main__':
    main()
