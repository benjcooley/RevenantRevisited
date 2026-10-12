#!/usr/bin/env python3
"""Capture retail in the dosbox-x lab at each lighting A/B location.

Run with the lab's Python:

  P=~/RevenantRetailLab/.venv/bin/python
  $P tools/lighting_ab/lab_capture.py start [KEY=VALUE ...]     # take the lab, launch retail with the INI keys
  $P tools/lighting_ab/lab_capture.py base                       # upload the base slot once (CurMap, thumbnail)
  $P tools/lighting_ab/lab_capture.py probe out.png [--click X,Y ...] [--key K ...]
  $P tools/lighting_ab/lab_capture.py run --run NAME --config no|yes [--row N] [location ...]
  $P tools/lighting_ab/lab_capture.py relaunch no|yes            # retail hung: close it and start it again
  $P tools/lighting_ab/lab_capture.py restore                    # INI back, editor relaunched, lab released

`run` captures each location in turn: upload the location's game.sav into
the guest slot LightAB, load it (title screen -> Load Game -> its row ->
Load; between locations, Esc -> the in-game menu's Load Game), wait until
the loading box is gone and the map holds still, then a few seconds more,
capture. The order alternates levels so no load lands
in the same place as the one before (retail's in-game load into the same
place fades to the area's ambient without the gamma offset,
LIGHTING_FIDELITY.md §2.5). Shots go to
~/RevenantRevisited-lightab/<run>/retail-<config>/<id>.png.
docs/lighting/LIGHTING_AB.md §4.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'retaillab'))
import retail  # noqa: E402  (the lab: lock, Lua jobs, keys, capture)
from dosbox_control import Control  # noqa: E402
from guest_serial import Guest  # noqa: E402

SLOTS = Path.home() / 'RevenantRevisited-lightab' / 'slots'
OUT = Path.home() / 'RevenantRevisited-lightab'
SAVES = 'C:\\REVENANT\\Save\\Single'
SLOT = 'LightAB'

TITLE_LOAD = (430, 212)             # title screen: Load Game
MENU_LOAD = (407, 141)              # the in-game menu (Esc): Load Game
LIST_X, ROW0_Y, ROW_H = 400, 100, 16
LOAD_CONFIRM = (226, 421)           # the Load Game dialog's Load Game button

CONFIGS = {
    'no':  {'RealTimeLight': 'No',  'GammaLevel': '3', 'EnhancedLighting': 'No', 'Software3D': 'Yes', 'AutoCombat': 'No'},
    'yes': {'RealTimeLight': 'Yes', 'GammaLevel': '3', 'EnhancedLighting': 'No', 'Software3D': 'Yes', 'AutoCombat': 'No'},
}


def click(x, y):
    retail.run_lua(retail.LUA_CLICK, {'x': x, 'y': y}, 'gf_click')


def key(name):
    with Control() as c:
        retail.press(c, name)


def capture(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with Control() as c:
        return c.capture(str(path))


def upload(files: list[tuple[Path, str]]):
    with Guest() as g:
        for d in (SAVES, f'{SAVES}\\{SLOT}', f'{SAVES}\\{SLOT}\\CurMap'):
            try:
                g.mkdir(d)
            except RuntimeError:
                pass                                    # already there
        for local, remote in files:
            target = f'{SAVES}\\{SLOT}\\{remote}'
            try:
                g.rpc('delete ' + g.path(target))      # a shorter file mustn't keep the old tail
            except RuntimeError:
                pass
            g.upload(str(local), target)


def upload_base(slot: Path):
    files = [(slot / 'game.sav', 'game.sav'), (slot / 'ss.bmp', 'ss.bmp')]
    files += [(f, 'CurMap\\' + f.name) for f in sorted((slot / 'CurMap').iterdir())]
    t = time.time()
    upload(files)
    print(f'uploaded {len(files)} files in {time.time() - t:.0f} s')


def interleave(locations):
    """Order the locations so no two in a row share a level."""
    pending, out = list(locations), []
    while pending:
        last = out[-1]['level'] if out else None
        pick = next((l for l in pending if l['level'] != last), pending[0])
        pending.remove(pick)
        out.append(pick)
    return out


def load(row: int, first: bool, screen: Path):
    """Opens the Load Game dialog from wherever the game is (the title
    screen, play, Game Over, or the dialog already up), picks LightAB's row
    and loads it."""
    if loading(screen):
        pass                            # the dialog is up already
    elif first:
        click(*TITLE_LOAD)
    elif game_over(screen):
        click(*GAMEOVER_LOAD)           # Locke died: the Game Over screen's Load
    else:
        key('esc')
        time.sleep(1.0)
        click(*MENU_LOAD)
    time.sleep(2.5)
    click(LIST_X, ROW0_Y + row * ROW_H)
    time.sleep(0.8)
    click(*LOAD_CONFIRM)


TEMPLATES = HERE / 'templates'
LOADING_BUTTONS = (150, 400, 500, 442)   # the loading box's Load Game / Exit buttons
GAMEOVER_BUTTONS = (10, 440, 630, 478)   # the Game Over screen's Restart / Load / Exit
GAMEOVER_LOAD = (320, 457)


def matches(path: Path, box, template: str) -> bool:
    """The screen's `box` is the template (a crop of the same box)."""
    from PIL import Image, ImageChops, ImageStat
    img = Image.open(path).convert('L').crop(box)
    ref = Image.open(TEMPLATES / template).convert('L')
    return ImageStat.Stat(ImageChops.difference(img, ref)).mean[0] < 8.0


def loading(path: Path) -> bool:
    """The Load Game dialog (with its loading box) is up."""
    return matches(path, LOADING_BUTTONS, 'loading_buttons.png')


def game_over(path: Path) -> bool:
    return matches(path, GAMEOVER_BUTTONS, 'gameover_buttons.png')


def map_frame(path: Path):
    from PIL import Image
    return Image.open(path).convert('L').crop((0, 64, 400, 372))


def frame_change(a, b) -> float:
    """Mean absolute grey-level difference of two map frames."""
    from PIL import ImageChops, ImageStat
    return ImageStat.Stat(ImageChops.difference(a, b)).mean[0]


def wait_loaded(probe: Path, timeout: float, settle: float) -> float | None:
    """Polls until the loading box has come and gone and two map frames in
    a row are alike, then waits `settle`. Returns the seconds it took, or
    None when no load started (no loading box within 20 s)."""
    start, last, started = time.time(), None, False
    time.sleep(1.0)
    while time.time() - start < timeout:
        capture(probe)
        if loading(probe):
            started, last = True, None
        elif not started:
            if time.time() - start > 20.0 or game_over(probe):
                return None
        else:
            frame = map_frame(probe)
            if last is not None and frame_change(frame, last) < 6.0:
                time.sleep(settle)
                return time.time() - start
            last = frame
        time.sleep(1.5)
    raise RuntimeError(f'no map after {timeout:.0f} s')


def run(name: str, config: str, row: int, ids, settle: float, first: bool):
    locations = json.loads((HERE / 'locations.json').read_text())['locations']
    if ids:
        unknown = set(ids) - {l['id'] for l in locations}
        if unknown:
            raise SystemExit(f'unknown location(s): {", ".join(sorted(unknown))}')
        locations = [l for l in locations if l['id'] in ids]
    out = OUT / name / f'retail-{config}'
    for i, loc in enumerate(interleave(locations)):
        upload([(SLOTS / loc['id'] / 'game.sav', 'game.sav')])
        probe = out / '.probe.png'
        took = None
        for attempt in range(2):
            capture(probe)
            load(row, first and i == 0 and attempt == 0, probe)
            took = wait_loaded(probe, 150.0, settle)
            if took is not None:
                break
            print(f'{loc["id"]}: the load did not start; again', flush=True)
        if took is None:
            raise RuntimeError(f'{loc["id"]}: the load never started')
        shot = out / f'{loc["id"]}.png'
        capture(shot)
        print(f'{loc["id"]}: {shot} ({took:.0f} s)', flush=True)


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('start')
    s.add_argument('config', choices=CONFIGS)
    s.add_argument('--owner', default='gameflow lighting A/B')
    s = sub.add_parser('relaunch', help='close retail and start it again (the lab stays ours)')
    s.add_argument('config', choices=CONFIGS)
    sub.add_parser('restore')
    s = sub.add_parser('base')
    s.add_argument('--slot', type=Path, default=SLOTS / 'keep-pit')
    s = sub.add_parser('probe')
    s.add_argument('out', type=Path)
    s.add_argument('--click', action='append', default=[])
    s.add_argument('--key', action='append', default=[])
    s.add_argument('--wait', type=float, default=1.5)
    s = sub.add_parser('run')
    s.add_argument('locations', nargs='*')
    s.add_argument('--run', required=True)
    s.add_argument('--config', choices=CONFIGS, required=True)
    s.add_argument('--row', type=int, required=True, help="LightAB's row in the Load Game list")
    s.add_argument('--settle', type=float, default=6.0, help='seconds from a settled map to the capture')
    s.add_argument('--from-title', action='store_true', help='the first load starts on the title screen')
    a = p.parse_args(argv)

    if a.cmd == 'start':
        retail.take_lock(a.owner)
        print(retail.run_lua(retail.LUA_START, {'set': CONFIGS[a.config]}, 'gf_start'))
    elif a.cmd == 'relaunch':
        if not retail.LOCK.exists() or 'lighting' not in retail.LOCK.read_text():
            raise SystemExit('relaunch is for a lab this tool started (lab_capture.py start)')
        print(retail.run_lua(retail.LUA_START, {'set': CONFIGS[a.config]}, 'gf_start'))
    elif a.cmd == 'restore':
        print(retail.run_lua(retail.LUA_RESTORE, {}, 'gf_restore'))
        retail.release_lock()
    elif a.cmd == 'base':
        upload_base(a.slot)
    elif a.cmd == 'probe':
        for k in a.key:
            key(k)
            time.sleep(0.5)
        for c in a.click:
            click(*(int(v) for v in c.split(',')))
            time.sleep(0.5)
        time.sleep(a.wait)
        print(capture(a.out))
    elif a.cmd == 'run':
        run(a.run, a.config, a.row, a.locations, a.settle, a.from_title)


if __name__ == '__main__':
    main()
