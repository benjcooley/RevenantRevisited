#!/usr/bin/env python3
"""Drive the retail game in the dosbox-x lab for gameflow reference captures.

The lab (~/RevenantRetailLab) runs Windows 98 with Revenant installed at
C:\\REVENANT and is shared with the VFX track, whose sessions expect the
editor running with the lab's own INI. `start` backs that INI up once
(C:\\MCP\\GFPRE.INI), applies overrides and launches the game; `restore`
puts the INI back and relaunches the editor. Always finish with `restore`.

Run with the lab's Python, from anywhere:

  ~/RevenantRetailLab/.venv/bin/python tools/retaillab/retail.py start ShowDialog=Yes AutoCombat=No
  ... newgame | key 3 | prompt alreadydead | capture out.png | record start ...
  ~/RevenantRetailLab/.venv/bin/python tools/retaillab/retail.py restore

docs/gameflow/RETAIL_CAPTURE.md has the recipe and the traps.
"""
import argparse
import json
import os
import re
import sys
import time
from pathlib import Path

LAB = Path(os.environ.get('REVENANT_RETAIL_LAB', str(Path.home() / 'RevenantRetailLab'))).resolve()
sys.path.insert(0, str(LAB))

from PIL import Image                                      # noqa: E402
from dosbox_control import Control, KEYS                   # noqa: E402
from guest_serial import Guest                             # noqa: E402
from guest_mcp_tools import lua_value, job                 # noqa: E402

INI = r'C:\REVENANT\revenant.ini'
BACKUP = r'C:\MCP\GFPRE.INI'
SCRIPT_DIR = 'C:\\MCP\\SCRIPTS\\'
NEW_GAME_BUTTON = (430, 162)        # title screen, 640x480
LOAD_GAME_BUTTON = (430, 212)       # title screen
LOAD_ROW0_Y, LOAD_ROW_H = 100, 16   # the Load Game list's first row, its pitch
LOAD_LIST_X = 400
LOAD_CONFIRM = (226, 421)           # the Load Game dialog's Load Game button
SAVES = 'C:\\REVENANT\\Save\\Single'
# The combat arena's sectors (docs/gameplay/COMBAT_ARENA.md): level 2's empty
# block, sector x 22..31. An arena slot's other CurMap sectors are retail's
# own map as the port saved it; the guest reads those from its base map.
ARENA_SECTOR = re.compile(r'2_(2[2-9]|3[01])_\d+\.DAT', re.I)

# One driver at a time: `start` takes the lock, `restore` releases it. Agents
# and sessions share the lab; a second `start` refuses instead of stealing
# the guest from a run in progress.
LOCK = LAB / 'gameflow-lab.lock'


def take_lock(owner):
    try:
        fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        raise SystemExit(f'the lab is in use: {LOCK.read_text().strip()} '
                         f'(remove {LOCK} only if that run is gone)')
    with os.fdopen(fd, 'w') as f:
        json.dump({'owner': owner, 'pid': os.getppid(), 'since': time.strftime('%Y-%m-%d %H:%M:%S')}, f)


def release_lock():
    LOCK.unlink(missing_ok=True)

# Closes retail (game or editor) through Alt+F4 and waits for its window to go.
LUA_CLOSE = r'''
local function close_retail()
    if not guest.window_exists('Revenant') then return end
    assert(guest.focus('Revenant'), 'Cannot focus retail')
    guest.sleep(300)
    guest.key(18,115)
    for i=1,100 do
        guest.sleep(200)
        if not guest.window_exists('Revenant') then break end
    end
    guest.sleep(1500)
    assert(not guest.window_exists('Revenant'), 'Retail did not close')
end
local function wait_window()
    for i=1,600 do
        guest.sleep(200)
        if guest.window_exists('Revenant') then return end
    end
    error('No retail window after launch')
end
'''

LUA_START = LUA_CLOSE + r'''
return function(args)
    close_retail()
    local f=assert(io.open('C:\\REVENANT\\revenant.ini','rb')); local ini=f:read('*a'); f:close()
    local b=io.open('C:\\MCP\\GFPRE.INI','rb')
    if b then b:close() else
        b=assert(io.open('C:\\MCP\\GFPRE.INI','wb')); assert(b:write(ini)); assert(b:close())
        guest.log('INI backed up')
    end
    for key,value in pairs(args.set or {}) do
        local n
        ini,n=ini:gsub(key..'=[^\r\n]*', key..'='..value)
        assert(n==1, 'Expected one '..key..' entry')
    end
    f=assert(io.open('C:\\REVENANT\\revenant.ini','wb')); assert(f:write(ini)); assert(f:close())
    guest.launch('C:\\REVENANT\\Rev98.exe','C:\\REVENANT')
    wait_window()
    guest.log('game window up')
end
'''

LUA_RESTORE = LUA_CLOSE + r'''
return function(args)
    close_retail()
    local b=assert(io.open('C:\\MCP\\GFPRE.INI','rb')); local ini=b:read('*a'); b:close()
    local f=assert(io.open('C:\\REVENANT\\revenant.ini','wb')); assert(f:write(ini)); assert(f:close())
    guest.mkdir('C:\\REVENANT\\CurMap')
    guest.launch('C:\\REVENANT\\Rev98.exe EDITOR DEVICE=display SOFTWARE3D','C:\\REVENANT')
    wait_window()
    guest.log('INI restored; editor window up')
end
'''

LUA_CLICK = r'''
return function(args)
    guest.move(args.x,args.y,250)
    local x,y=guest.cursor()
    assert(x==args.x and y==args.y,'Cursor position mismatch')
    guest.click(0)
end
'''


def run_lua(source, args=None, name='gf_job', timeout=240):
    """Uploads a Lua command to the guest, runs it and waits for it to finish."""
    path = SCRIPT_DIR + name + '.lua'
    data = source.encode('cp1252')
    with Guest() as g:
        if job(g)['running']:
            raise RuntimeError('a guest Lua job is already running')
        for off in range(0, len(data), 768):
            g.rpc(f'write {g.path(path)} {off} {data[off:off + 768].hex()}')
        jid = int(g.rpc('script ' + g.path(path) + ' ' + lua_value(args or {}).encode('cp1252').hex()))
    deadline = time.time() + timeout
    while time.time() < deadline:
        time.sleep(0.3)
        with Guest() as g:
            j = job(g)
        if j['id'] == jid and not j['running']:
            if j['error']:
                raise RuntimeError(j['error'])
            return j['log']
    raise TimeoutError(f'guest job {jid} still running')


def press(control, name):
    """One key press with no settle wait, so a burst stays inside a few frames."""
    control.command(f'key {KEYS[name]}')


# The prompt's "Message: " prefix: white text at the start of the text bar's
# last line (TTextBar, x 2..50, y 406..420 with the side panel open).
_MESSAGE_TEXT = None


def prompt_open(path):
    global _MESSAGE_TEXT
    if _MESSAGE_TEXT is None:
        tmpl = Image.open(Path(__file__).with_name('message_prefix.png')).convert('L')
        _MESSAGE_TEXT = [(x, y) for x in range(tmpl.width) for y in range(tmpl.height)
                         if tmpl.getpixel((x, y)) > 200]
    crop = Image.open(path).convert('L').crop((2, 406, 50, 420))
    return sum(1 for p in _MESSAGE_TEXT if crop.getpixel(p) > 200) / len(_MESSAGE_TEXT) > 0.85


def type_prompt(control, text, scratch, attempts=30):
    """Types a line into retail's Enter prompt (TTextBar CharPress 0x0054d4a0).

    The prompt only opens with control on and the player's action neither
    COMBAT nor BOW; with it open and the player back in combat, the next key
    commits the line as typed so far. So: C (leave combat), Enter, check the
    prompt is up, then the whole line in one burst.
    """
    for attempt in range(attempts):
        press(control, 'c')
        time.sleep(0.45)
        press(control, 'enter')
        time.sleep(0.08)
        shot = Path(scratch) / f'prompt_{attempt}.png'
        control.capture(shot)
        if prompt_open(shot):
            for ch in text:
                control.key(*Control.character(ch))     # shifted characters (@, :, ...) too
            press(control, 'enter')
            return attempt
    raise RuntimeError('the prompt never opened')


def upload_slot(slot: Path) -> list[str]:
    """An arena slot into the guest's saves: game.sav, its thumbnail and the
    arena's sectors (tools/combatarena/make_arena_slot.py)."""
    remote = SAVES + '\\' + slot.name
    files = [slot / 'game.sav', slot / 'ss.bmp']
    files += sorted(f for f in (slot / 'CurMap').iterdir() if ARENA_SECTOR.fullmatch(f.name))
    sent = []
    with Guest() as g:
        for d in (SAVES, remote, remote + '\\CurMap'):
            try:
                g.mkdir(d)
            except RuntimeError:
                pass                                    # already there
        for f in files:
            target = remote + '\\' + str(f.relative_to(slot)).replace('/', '\\')
            try:
                g.rpc('delete ' + g.path(target))       # a shorter file mustn't keep the old tail
            except RuntimeError:
                pass
            g.upload(str(f), target)
            sent.append(target)
    return sent


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('start', help='take the lab, close the editor, apply INI overrides, launch the game')
    s.add_argument('set', nargs='*', help='[Options] overrides, KEY=VALUE')
    s.add_argument('--owner', default=os.environ.get('USER', 'gameflow'), help='who holds the lab (shown to others)')
    sub.add_parser('restore', help='close the game, restore the INI, relaunch the editor')
    sub.add_parser('newgame', help='click New Game on the title screen')
    s = sub.add_parser('upload', help='copy an arena slot into the guest saves (game.sav, thumbnail, arena sectors)')
    s.add_argument('slot', type=Path)
    s = sub.add_parser('load', help='title screen -> Load Game -> the slot on --row -> load')
    s.add_argument('--row', type=int, default=0)
    s = sub.add_parser('click', help='left-click the game window at x y (640x480)')
    s.add_argument('x', type=int)
    s.add_argument('y', type=int)
    s = sub.add_parser('key', help='press keys one at a time (keyboard-map.json names)')
    s.add_argument('keys', nargs='+')
    s = sub.add_parser('prompt', help="type a line into retail's Enter prompt (cheats, @script)")
    s.add_argument('text')
    s.add_argument('--scratch', default='/tmp')
    s = sub.add_parser('capture', help='save the current guest frame')
    s.add_argument('path')
    s = sub.add_parser('record', help='DOSBox AVI capture (video and audio)')
    s.add_argument('action', choices=['start', 'stop', 'status'])
    a = p.parse_args()

    if a.cmd == 'start':
        overrides = dict(kv.split('=', 1) for kv in a.set)
        take_lock(a.owner)
        print(run_lua(LUA_START, {'set': overrides}, 'gf_start'))
    elif a.cmd == 'restore':
        print(run_lua(LUA_RESTORE, {}, 'gf_restore'))
        release_lock()
    elif a.cmd == 'upload':
        for target in upload_slot(a.slot.resolve()):
            print(target)
    elif a.cmd == 'load':
        run_lua(LUA_CLICK, {'x': LOAD_GAME_BUTTON[0], 'y': LOAD_GAME_BUTTON[1]}, 'gf_click')
        time.sleep(3)
        run_lua(LUA_CLICK, {'x': LOAD_LIST_X, 'y': LOAD_ROW0_Y + a.row * LOAD_ROW_H}, 'gf_click')
        time.sleep(1)
        run_lua(LUA_CLICK, {'x': LOAD_CONFIRM[0], 'y': LOAD_CONFIRM[1]}, 'gf_click')
    elif a.cmd == 'click':
        run_lua(LUA_CLICK, {'x': a.x, 'y': a.y}, 'gf_click')
    elif a.cmd == 'newgame':
        x, y = NEW_GAME_BUTTON
        run_lua(LUA_CLICK, {'x': x, 'y': y}, 'gf_click')
    else:
        with Control() as c:
            if a.cmd == 'key':
                for k in a.keys:
                    c.key(k)
            elif a.cmd == 'prompt':
                print('opened on attempt', type_prompt(c, a.text, a.scratch))
            elif a.cmd == 'capture':
                print(c.capture(a.path))
            elif a.cmd == 'record':
                print(c.command('video ' + a.action))


if __name__ == '__main__':
    main()
