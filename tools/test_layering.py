#!/usr/bin/env python3
"""The engine never includes the game.

src/engine is the engine; src/game is this game. A file under src/engine may
include engine headers, third-party headers and the standard library, and
nothing under src/game. Every include is spelled from src/ ("engine/scene/node.h",
"game/object/object.h"), so the rule is a string check over every engine source.
docs/SOURCE_LAYOUT.md has the layout.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
SOURCE_SUFFIXES = {".h", ".cpp", ".mm", ".m", ".c", ".inc"}


def violations() -> list[str]:
    found = []
    for path in sorted((SRC / "engine").rglob("*")):
        if path.suffix not in SOURCE_SUFFIXES or not path.is_file():
            continue
        text = path.read_bytes().decode("utf-8", "replace")
        for match in INCLUDE.finditer(text):
            target = match.group(1)
            if target.startswith("game/") or "/game/" in target:
                line = text.count("\n", 0, match.start()) + 1
                found.append(f"{path.relative_to(ROOT)}:{line}: includes {target}")
    return found


def main() -> int:
    found = violations()
    for line in found:
        print(line)
    if found:
        print(f"FAIL: {len(found)} engine file(s) include game code")
        return 1
    print("ok: nothing under src/engine includes src/game")
    return 0


if __name__ == "__main__":
    sys.exit(main())
