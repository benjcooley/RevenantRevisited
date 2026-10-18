#!/usr/bin/env python3
"""Select C++ CI from the files a push or pull request changed, without skipping its checks."""

from __future__ import annotations

import json
import os
import subprocess
from pathlib import Path


# Sources, tests, build inputs, packaged engine assets and this workflow.
# Retail data, documentation and the independent Rust port do not build C++.
CPP_PATHS = (
    "src", "thirdparty", "assets", "cmake", "tools", "CMakeLists.txt",
    "editor.def", ".gitattributes", ".gitmodules", ".github/workflows/ci.yml",
)


def cpp_changed(root: Path, event_name: str, event: dict, ref: str) -> bool:
    # Manual runs deliberately rebuild. Unknown events also build conservatively.
    if event_name not in ("push", "pull_request"):
        return True

    def git(*args: str) -> subprocess.CompletedProcess:
        return subprocess.run(["git", *args], cwd=root, capture_output=True, text=True)

    default_branch = event["repository"]["default_branch"]
    if event_name == "push" and ref == f"refs/heads/{default_branch}":
        base = event["before"]
        if not base or set(base) == {"0"}:
            return True
    else:
        # Use the whole branch's diff against the branch it will merge into:
        # a Rust-only follow-up must not turn a previous failed C++ build into
        # skipped checks on the same PR.
        base_branch = event["pull_request"]["base"]["ref"] if event_name == "pull_request" else default_branch
        merge_base = git("merge-base", "HEAD", f"refs/remotes/origin/{base_branch}")
        if merge_base.returncode:
            print("::warning::Cannot determine the branch base; running C++ CI.")
            return True
        base = merge_base.stdout.strip()

    diff = git("diff", "--quiet", "--no-ext-diff", base, "HEAD", "--", *CPP_PATHS)
    if diff.returncode not in (0, 1):
        print("::warning::Cannot compare changed files; running C++ CI.")
    return diff.returncode != 0


def main() -> None:
    event = json.loads(Path(os.environ["GITHUB_EVENT_PATH"]).read_text(encoding="utf-8"))
    changed = cpp_changed(Path(__file__).resolve().parents[2],
                          os.environ["GITHUB_EVENT_NAME"], event, os.environ["GITHUB_REF"])
    value = str(changed).lower()
    print(f"C++ build required: {value}")
    with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
        output.write(f"cpp={value}\n")


if __name__ == "__main__":
    main()
