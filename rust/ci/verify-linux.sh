#!/bin/sh
set -eu
cargo fmt --all -- --check
cargo clippy --locked --all-targets --all-features -- -D warnings
cargo test --locked --all-targets
cargo test --locked --no-default-features
cargo test --locked --test gpu -- --ignored --test-threads=1
xvfb-run -a cargo run --locked --example gpu-smoke
sh ci/smoke-wayland.sh
