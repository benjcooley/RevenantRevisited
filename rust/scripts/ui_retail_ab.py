#!/usr/bin/env python3
"""Compare Rust UI captures with original retail pane code (requires retail lab).

Generate Rust frames with cargo test --test hud --test gameflow -- --ignored.
Uses the existing HUD emulator fixtures, including their documented D3D quad
boundary and tolerances. GDI glyph pixels, unimplemented binding-list content,
and known retail narrow-alpha blit bugs are excluded explicitly, not fitted.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import zipfile

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/retail_ab'))
from hud_ab import (compare, known_bug_mask, port_frame, retail_frame, screen_mask,
                    text_cells, text_draw_mask, text_draws, gdi_text_cells, triptych)


def portrait(data_dir):
    """The same state-zero Locke bitmap passed as the retail fixture's input."""
    with zipfile.ZipFile(data_dir / 'imagery.rvi') as archive:
        data = archive.read(next(n for n in archive.namelist()
                                 if n.lower() == 'imagery/chars/locke.i3d'))
    word = lambda p: struct.unpack_from('<I', data, p)[0]
    data = data[20 + word(16) + 4 * struct.unpack_from('<H', data, 4)[0]:]
    assert word(0) & 16 and word(0) & 2
    state = 8 + word(8)
    at = state + 4 + word(state + 4)
    fields = struct.unpack_from('<18I', data, at)
    length = max([72 + fields[17]] + [4 * p + fields[p] + fields[n]
                 for n, p in ((7, 8), (9, 10), (11, 12), (13, 14), (15, 16))
                 if fields[n] and fields[p]])
    return dict(tbitmap=data[at:at + length].hex())


def capture(args, out):
    settings = dict(Realtime=1, Auto=0, Dialog=0, Face=0, Enhanced=1, Limit=0,
                    NoCombatRes=0, Violence=0, Music=0, Sound=89, Gamma=0)
    # Rust's default 70 percent volume maps to 89 on retail's 0..127 scale.
    cases = {
        'options': [dict(name='options', settings=settings),
                    dict(name='options-game', settings=settings, from_game=True)],
        'plyrstatusbar': [dict(name='status', player=dict(name='Locke', health=0,
                             mana=0, fatigue=0, portrait=portrait(args.data)), pane_width=452)],
        'bottombar': [dict(name='bottom', player=dict(name='Locke'), belt=[], pane_width=452)],
    }
    for script, requests in cases.items():
        process = subprocess.Popen([args.retail_python,
            str(ROOT / f'tools/retail_runtime/slots/hud/{script}.py'),
            str(args.retail_exe), '--serve'], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, text=True)
        try:
            hello = json.loads(process.stdout.readline())
            assert hello.get('ok'), hello
            for case in requests:
                process.stdin.write(json.dumps(dict(id=case['name'], case=case, out=str(out))) + '\n')
                process.stdin.flush()
                reply = json.loads(process.stdout.readline())
                if not reply.get('ok'):
                    raise RuntimeError(reply)
                result = reply['result']
                result['retail_sha256'] = hello['retail_sha256']
                (out / f'{case["name"]}.json').write_text(json.dumps(result, indent=2) + '\n')
        finally:
            process.stdin.close()
            process.wait(timeout=30)


def run(args):
    out = args.captures / 'retail-ui'
    out.mkdir(parents=True, exist_ok=True)
    if not args.reuse_retail:
        capture(args, out)
    reports = []
    for name in ('options', 'options-game', 'status', 'bottom'):
        result = json.loads((out / f'{name}.json').read_text())
        rust = args.captures / ('hud-chrome-640.png' if name in ('status', 'bottom')
                               else f'screens-{name}-640.png')
        expected, actual = retail_frame(out / f'{name}.rgb565'), port_frame(rust)
        if name.startswith('options'):
            mask, band = text_draw_mask(text_draws(result), result['pane'])
            # The Rust list currently shows its movement keys; retail lists
            # commands whose gameplay/rebinding system has not been ported.
            mask[224:385, 70:550] = True
        elif name == 'status':
            mask, band, _ = text_cells(result['primitives'])
            mask[112:, :] = True
            mask[:, 196:] = True
        else:
            mask, band, _ = gdi_text_cells(result['text_calls'])
            mask[:420, :] = True
            mask[:, 452:] = True
        tolerance = np.zeros((480, 640), dtype=np.int32)
        for kind, limit in [('alpha', 17), ('screen', 9), ('composed', 43)]:
            if kind in result['blend']:
                tolerance = np.maximum(tolerance, np.where(screen_mask(result['blend'][kind]), limit, 0))
        for bug in known_bug_mask(result, result.get('pane', [0, 0, 640, 480])).values():
            mask |= bug
        report, delta = compare(expected, actual, mask, band, tolerance)
        report.update(case=name, rust_capture_sha256=hashlib.sha256(rust.read_bytes()).hexdigest(),
                      retail_sha256=result.get('retail_sha256'))
        triptych(out / f'{name}-ab.png', expected, actual, delta, mask, band, tolerance,
                 rows=slice(0, 480))
        reports.append(report)
        print(f'{name}: {report["compared"]} pixels, {report["defects"]} defects, '
              f'{report["text_overflow"]} pixels outside text cells', flush=True)
    (out / 'report.json').write_text(json.dumps(dict(results=reports,
        scope='Static chrome: options (title/in-game), empty status bars with Locke portrait, empty bottom bar.',
        limits='GDI glyph pixels and unported binding-list contents excluded. Fonts are Arimo with retail GDI metrics. '
               'Retail texture overlays use the existing HUD fixture D3D quad boundary; no hardware filtering claim.',
        tolerances=dict(alpha=17, screen=9, composed=43)), indent=2) + '\n')
    return int(any(r['defects'] or r['text_overflow'] for r in reports))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--retail-exe', type=Path, default=ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    parser.add_argument('--retail-python', default=os.environ.get('RETAIL_PY', sys.executable))
    parser.add_argument('--data', type=Path, default=Path(os.environ.get('REVENANT_DATA_PATH', ROOT / 'data')))
    parser.add_argument('--captures', type=Path, default=ROOT / 'rust/captures')
    parser.add_argument('--reuse-retail', action='store_true')
    sys.exit(run(parser.parse_args()))
