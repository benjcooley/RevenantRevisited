#!/bin/sh
# Isolated compositor: no game data, real desktop session or physical GPU needed.
set -eu
: "${XDG_RUNTIME_DIR:?Set a private XDG_RUNTIME_DIR before starting Weston}"
revenant_socket="revenant-smoke-$$"
revenant_log="$XDG_RUNTIME_DIR/$revenant_socket.log"
weston --backend=headless-backend.so --use-pixman --idle-time=0 \
  --socket="$revenant_socket" --log="$revenant_log" &
revenant_weston_pid=$!
trap 'kill "$revenant_weston_pid" 2>/dev/null || true; wait "$revenant_weston_pid" 2>/dev/null || true' EXIT
revenant_attempt=0
while [ ! -S "$XDG_RUNTIME_DIR/$revenant_socket" ]; do
  if ! kill -0 "$revenant_weston_pid" 2>/dev/null || [ "$revenant_attempt" -ge 100 ]; then
    cat "$revenant_log"
    exit 1
  fi
  revenant_attempt=$((revenant_attempt + 1))
  sleep 0.1
done
env -u DISPLAY WAYLAND_DISPLAY="$revenant_socket" cargo run --locked --example gpu-smoke
