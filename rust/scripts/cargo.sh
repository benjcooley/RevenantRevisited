#!/bin/sh
# Use the normal toolchain when healthy. A broken package-manager rustc can
# coexist with a working rustup installation; use that without global changes.
set -eu
works() { sh -c '"$1" --version >/dev/null 2>&1' _ "$1" >/dev/null 2>&1; }
if ! works rustc; then
    found=false
    for candidate in "${RUSTUP_HOME:-$HOME/.rustup}"/toolchains/stable-*/bin; do
        if [ -x "$candidate/rustc" ] && works "$candidate/rustc"; then
            PATH="$candidate:$PATH"
            export PATH
            found=true
            break
        fi
    done
    if [ "$found" = false ]; then
        echo 'No working Rust toolchain found. Install stable Rust (1.85 or later).' >&2
        exit 1
    fi
fi
exec cargo "$@"
