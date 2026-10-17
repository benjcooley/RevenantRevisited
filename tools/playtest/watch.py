#!/usr/bin/env python3
"""Watch a playtest session live in a browser.

    watch.py SESSION_DIR [--port 8765] [--open]

Serves a page with the game's current frame (SESSION_DIR/watch/frame.png,
written by the host every tick: the emulator's --watch, the port's
--watch-frames), the director's caption and objective, the milestones,
and its decisions as they happen (SESSION_DIR/bundle/director.jsonl), with
the session's spend (SESSION_DIR/bundle/session.rvlog). play.py --watch
starts it for you. Standard library only; it only reads the session.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import threading
import time
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

PAGE = r"""<!doctype html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Jev plays Revenant</title>
<style>
:root { --bg:#111214; --panel:#1b1d21; --line:#2c2f35; --text:#e6e6e6; --dim:#9aa0a6;
        --accent:#d4a24c; --good:#6cc070; --warn:#e0b050; --bad:#e06060; }
* { box-sizing: border-box; }
body { margin:0; background:var(--bg); color:var(--text); font:14px/1.45 -apple-system, system-ui, sans-serif; }
main { display:grid; grid-template-columns:minmax(0, 1fr) 380px; gap:16px; padding:16px; height:100vh; }
.stage { display:flex; flex-direction:column; gap:10px; min-width:0; }
.frame { position:relative; background:#000; border:1px solid var(--line); border-radius:6px;
         aspect-ratio:4/3; max-height:calc(100vh - 120px); width:100%; overflow:hidden; }
.frame img { width:100%; height:100%; object-fit:contain; image-rendering:pixelated; display:block; }
.frame .none { position:absolute; inset:0; display:grid; place-items:center; color:var(--dim); text-align:center; padding:24px; }
.caption { background:var(--panel); border:1px solid var(--line); border-radius:6px; padding:10px 14px;
           font-size:16px; min-height:46px; }
.caption b { color:var(--accent); font-weight:600; }
aside { display:flex; flex-direction:column; gap:12px; min-height:0; }
.card { background:var(--panel); border:1px solid var(--line); border-radius:6px; padding:12px 14px; }
.card h2 { margin:0 0 6px; font-size:12px; letter-spacing:.06em; text-transform:uppercase; color:var(--dim); font-weight:600; }
.objective { font-size:15px; }
ol { margin:0; padding-left:20px; } li.done { color:var(--good); } li.now { color:var(--accent); font-weight:600; }
.log { flex:1; overflow:auto; min-height:0; padding:0; }
.log h2 { padding:12px 14px 0; }
.entry { padding:7px 14px; border-top:1px solid var(--line); }
.entry .t { color:var(--dim); font-variant-numeric:tabular-nums; margin-right:6px; }
.entry .k { color:var(--accent); margin-right:6px; }
.bar { display:inline-block; height:6px; border-radius:3px; background:var(--good); vertical-align:middle; margin-left:6px; }
.meta { color:var(--dim); font-size:12px; display:flex; gap:14px; flex-wrap:wrap; }
@media (max-width: 900px) { main { grid-template-columns:1fr; height:auto; } .log { max-height:50vh; } }
</style></head>
<body><main>
<section class="stage">
  <div class="frame"><img id="frame" alt="the game"><div class="none" id="noframe">Waiting for frames&hellip;<br>
    The port shows them in its own window; the emulator writes them here.</div></div>
  <div class="caption" id="caption">&nbsp;</div>
  <div class="meta"><span id="game"></span><span id="tick"></span><span id="spend"></span><span id="fps"></span></div>
</section>
<aside>
  <div class="card"><h2>Objective</h2><div class="objective" id="objective">&mdash;</div></div>
  <div class="card"><h2>Milestones</h2><ol id="milestones"></ol></div>
  <div class="card log"><h2>Decisions</h2><div id="log"></div></div>
</aside>
</main>
<script>
const img = document.getElementById('frame'), none = document.getElementById('noframe');
let frames = 0, last = performance.now();
function next() {
  const probe = new Image();
  probe.onload = () => { img.src = probe.src; none.style.display = 'none'; frames++; setTimeout(next, 33); };
  probe.onerror = () => setTimeout(next, 500);
  probe.src = 'frame.png?' + Date.now();
}
next();
setInterval(() => { const now = performance.now();
  document.getElementById('fps').textContent = (frames * 1000 / (now - last)).toFixed(1) + ' frames/s shown';
  frames = 0; last = now; }, 2000);

const log = document.getElementById('log');
const esc = (v) => String(v ?? '').replace(/[&<>"]/g, (c) => ({'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;'}[c]));
function words(e) {
  switch (e.kind) {
    case 'tactic': return `${esc(e.tactic)} <span class="bar" style="width:${Math.round((e.confidence||0)*60)}px"></span> <small>${esc(e.reason)}</small>`;
    case 'walk': return `walk to ${esc(e.words)} &rarr; ${esc(e.result)}`;
    case 'dialog': return `reply ${esc(e.choice)}: &ldquo;${esc(e.text)}&rdquo;`;
    case 'router': return `asked the router (${esc(e.why)}): ${e.call ? esc(e.call.name + ' ' + JSON.stringify(e.call.arguments)) : 'no action'}`;
    case 'milestone': return `<b>milestone reached: ${esc(e.name)}</b>`;
    case 'saved': return `saved &rarr; ${esc(e.path)}`;
    case 'calibrated': return 'measured the direction keys';
    default: return esc(Object.entries(e).filter(([k]) => k !== 't' && k !== 'kind').map(([k, v]) => `${k}=${typeof v === 'object' ? JSON.stringify(v) : v}`).join(' '));
  }
}
const source = new EventSource('events');
source.onmessage = (m) => {
  const e = JSON.parse(m.data);
  if (e.kind === 'mode') return;
  const row = document.createElement('div');
  row.className = 'entry';
  row.innerHTML = `<span class="t">${(e.t / 24).toFixed(1)}s</span><span class="k">${esc(e.kind)}</span>${words(e)}`;
  log.prepend(row);
  while (log.childElementCount > 400) log.lastChild.remove();
};
async function poll() {
  try {
    const s = await (await fetch('state.json?' + Date.now())).json();
    document.getElementById('caption').innerHTML = s.caption ? `<b>${esc(s.mode)}</b> ${esc(s.caption)}` : '&nbsp;';
    document.getElementById('objective').textContent = s.objective || '—';
    document.getElementById('game').textContent = s.game ? 'game: ' + s.game : '';
    document.getElementById('tick').textContent = s.tick != null ? 'game time ' + (s.tick / 24).toFixed(1) + ' s' : '';
    document.getElementById('spend').textContent = s.spend || '';
    const ol = document.getElementById('milestones'); ol.innerHTML = '';
    (s.milestones || []).forEach((name, i) => { const li = document.createElement('li'); li.textContent = name;
      li.className = i < s.current ? 'done' : i === s.current ? 'now' : ''; ol.appendChild(li); });
  } catch (_) {}
  setTimeout(poll, 400);
}
poll();
</script></body></html>
"""


class Handler(BaseHTTPRequestHandler):
    session: Path

    def log_message(self, *args) -> None:
        pass

    def _send(self, status: int, body: bytes, kind: str) -> None:
        self.send_response(status)
        self.send_header('Content-Type', kind)
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self) -> None:
        path = self.path.split('?', 1)[0]
        watch = self.session / 'watch'
        if path == '/':
            self._send(HTTPStatus.OK, PAGE.encode('utf-8'), 'text/html; charset=utf-8')
        elif path == '/frame.png':
            try:
                self._send(HTTPStatus.OK, (watch / 'frame.png').read_bytes(), 'image/png')
            except OSError:
                self._send(HTTPStatus.NOT_FOUND, b'', 'text/plain')
        elif path == '/state.json':
            try:
                body = (watch / 'state.json').read_bytes()
            except OSError:
                body = b'{}'
            self._send(HTTPStatus.OK, body, 'application/json')
        elif path == '/events':
            self._events()
        else:
            self._send(HTTPStatus.NOT_FOUND, b'', 'text/plain')

    def _events(self) -> None:
        """Server-sent events: every director.jsonl line, from the start."""
        self.send_response(HTTPStatus.OK)
        self.send_header('Content-Type', 'text/event-stream')
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        path = self.session / 'bundle' / 'director.jsonl'
        offset = 0
        try:
            while True:
                if path.exists():
                    with path.open('rb') as f:
                        f.seek(offset)
                        chunk = f.read()
                    whole = chunk[:chunk.rfind(b'\n') + 1]
                    offset += len(whole)
                    for line in whole.splitlines():
                        self.wfile.write(b'data: ' + line + b'\n\n')
                    if whole:
                        self.wfile.flush()
                time.sleep(0.2)
        except (BrokenPipeError, ConnectionResetError):
            return


def serve(session: Path, port: int = 8765) -> ThreadingHTTPServer:
    """Start the viewer in a background thread; returns the server."""
    handler = type('SessionHandler', (Handler,), {'session': Path(session)})
    server = ThreadingHTTPServer(('127.0.0.1', port), handler)
    server.daemon_threads = True
    threading.Thread(target=server.serve_forever, daemon=True).start()
    return server


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('session', type=Path)
    parser.add_argument('--port', type=int, default=8765)
    parser.add_argument('--open', action='store_true', help='open the page in the default browser')
    args = parser.parse_args()
    server = serve(args.session, args.port)
    url = f'http://127.0.0.1:{args.port}/'
    print(f'watching {args.session} at {url}')
    if args.open:
        subprocess.run(['open', url], check=False)
    try:
        while True:
            time.sleep(3600)
    except KeyboardInterrupt:
        server.shutdown()


if __name__ == '__main__':
    main()
