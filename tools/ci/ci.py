#!/usr/bin/env python3
"""CI driver: configure, build, test, package, publish and report.

The workflow (.github/workflows/ci.yml) runs one subcommand per step. Each
step records what it saw in ci-out/<step>.json, and the report steps build
the run's summary from those records. Every step runs locally the same way:

    python3 tools/ci/ci.py configure --platform osx   # REVENANT_VERSION from `version`
    python3 tools/ci/ci.py build
    python3 tools/ci/ci.py test
    python3 tools/ci/ci.py package --platform osx
    python3 tools/ci/ci.py report --platform osx

docs/CI.md describes the pipeline, the download layout and the merge gate.
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import os
import re
import shutil
import string
import subprocess
import sys
import tempfile
import time
import xml.etree.ElementTree as ET
import zipfile
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
OUT = ROOT / "ci-out"
DIST = ROOT / "dist"
CONFIG = "RelWithDebInfo"          # what the dev checkouts build

MAIN_BRANCH = "main"
PRODUCT = "RevenantRevisited"
SHORT_HASH = 7                     # GitHub's abbreviated commit hash


@dataclass(frozen=True)
class Platform:
    key: str                # the download folder: osx/, windows/, linux/
    label: str
    exe: str                # what CMake builds
    ship: str               # its name in the package (README "Distribution model");
                            # never the retail Revenant.exe it sits beside
    configure: tuple[str, ...]

    @property
    def zip_name(self) -> str:
        return f"{PRODUCT}-{self.key}.zip"


PLATFORMS = {
    p.key: p
    for p in (
        Platform("osx", "macOS", "Revenant", f"{PRODUCT}OSX",
                 ("-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0",)),
        Platform("windows", "Windows", "Revenant.exe", f"{PRODUCT}Windows.exe",
                 ("-DCMAKE_C_COMPILER=clang-cl", "-DCMAKE_CXX_COMPILER=clang-cl",
                  # Static C runtime: no Visual C++ redistributable to install.
                  "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded")),
        Platform("linux", "Linux", "Revenant", f"{PRODUCT}Linux",
                 ("-DCMAKE_C_COMPILER=clang", "-DCMAKE_CXX_COMPILER=clang++")),
    )
}


# --------------------------------------------------------------------------
# Plumbing
# --------------------------------------------------------------------------

def write_record(name: str, record: dict) -> None:
    OUT.mkdir(exist_ok=True)
    (OUT / f"{name}.json").write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")


def read_record(name: str, folder: Path = OUT) -> dict | None:
    """ci-out/<name>.json, or in the publish job, wherever under the
    downloaded artifacts (one folder per artifact) it landed."""
    path = next(folder.rglob(f"{name}.json"), None)
    return json.loads(path.read_text(encoding="utf-8")) if path else None


def run_logged(cmd: list[str], log: Path) -> tuple[int, float]:
    """Runs `cmd`, streaming its output to the console and to `log`."""
    OUT.mkdir(exist_ok=True)
    print("+ " + " ".join(cmd), flush=True)
    start = time.monotonic()
    with open(log, "wb") as out:
        proc = subprocess.Popen(cmd, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        assert proc.stdout
        for line in proc.stdout:
            out.write(line)
            sys.stdout.buffer.write(line)
        sys.stdout.flush()
        rc = proc.wait()
    return rc, time.monotonic() - start


def git(*args: str) -> str:
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True,
                          check=True).stdout.strip()


def commit_info() -> dict:
    # The workflow names the commit under test (a pull request's head, not
    # GitHub's merge commit) and the branch it is on.
    sha = os.environ.get("REVENANT_COMMIT") or os.environ.get("GITHUB_SHA") or git("rev-parse", "HEAD")
    branch = (os.environ.get("REVENANT_BRANCH") or os.environ.get("GITHUB_REF_NAME")
              or git("rev-parse", "--abbrev-ref", "HEAD"))
    server = os.environ.get("GITHUB_SERVER_URL", "https://github.com")
    repo = os.environ.get("GITHUB_REPOSITORY", "benjcooley/RevenantRevisited")
    run_id = os.environ.get("GITHUB_RUN_ID")
    return {
        "sha": sha,
        "short": sha[:SHORT_HASH],
        "branch": branch,
        "subject": git("log", "-1", "--format=%s", sha),
        "author": git("log", "-1", "--format=%an", sha),
        "time": int(git("log", "-1", "--format=%ct", sha)),
        "repo_url": f"{server}/{repo}",
        "commit_url": f"{server}/{repo}/commit/{sha}",
        "run_url": f"{server}/{repo}/actions/runs/{run_id}" if run_id else None,
    }


def emit_summary(markdown: str) -> None:
    """Prints `markdown` to the log and adds it to the run's summary page."""
    print(markdown, flush=True)
    path = os.environ.get("GITHUB_STEP_SUMMARY")
    if path:
        with open(path, "a", encoding="utf-8") as f:
            f.write(markdown + "\n")


def annotate(level: str, title: str, message: str) -> None:
    if os.environ.get("GITHUB_ACTIONS"):
        message = message.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
        print(f"::{level} title={title}::{message}", flush=True)


def fmt_seconds(s: float | None) -> str:
    if s is None:
        return "—"
    s = int(round(s))
    return f"{s // 60}m {s % 60:02d}s" if s >= 60 else f"{s}s"


def fmt_bytes(n: int | None) -> str:
    if n is None:
        return "—"
    for unit in ("B", "KB", "MB", "GB"):
        if n < 1024 or unit == "GB":
            return f"{n:.0f} {unit}" if unit == "B" else f"{n:.1f} {unit}"
        n /= 1024
    return ""


# --------------------------------------------------------------------------
# version: every CI build claims the next vX.Y.Z tag
# --------------------------------------------------------------------------

VERSION_TAG = re.compile(r"^v(\d+)\.(\d+)\.(\d+)$")
TAGGER = ("-c", "user.name=github-actions[bot]",
          "-c", "user.email=41898282+github-actions[bot]@users.noreply.github.com")


def remote_versions() -> dict[str, tuple[str, tuple[int, int, int]]]:
    """origin's vX.Y.Z tags: name -> (the commit it marks, (X, Y, Z))."""
    tags: dict[str, tuple[str, tuple[int, int, int]]] = {}
    for line in git("ls-remote", "--tags", "origin", "refs/tags/v*").splitlines():
        obj, ref = line.split("\t")
        name = ref.removeprefix("refs/tags/")
        peeled = name.endswith("^{}")           # an annotated tag's commit
        name = name.removesuffix("^{}")
        if m := VERSION_TAG.match(name):
            if peeled or name not in tags:
                tags[name] = (obj, tuple(int(g) for g in m.groups()))
    return tags


def claim_version(info: dict) -> str:
    """Tags the commit with the version after the highest vX.Y.Z on origin
    (last number + 1; v0.0.1 first) and returns it. Pushing the tag is the
    claim: when a build running alongside took the number first, the push
    is refused and the next number is tried. A commit keeps the version it
    already has (a rerun, or the same commit pushed to another branch)."""
    sha = info["sha"]
    for _ in range(10):
        tags = remote_versions()
        own = [name for name, (commit, _) in tags.items() if commit == sha]
        if own:
            return max(own, key=lambda name: tags[name][1])
        x, y, z = max((v for _, v in tags.values()), default=(0, 0, 0))
        name = f"v{x}.{y}.{z + 1}"
        subprocess.run(["git", *TAGGER, "tag", "-a", name, sha,
                        "-m", f"{PRODUCT} {name}: build of {info['branch']}"],
                       cwd=ROOT, check=True)
        if subprocess.run(["git", "push", "origin", f"refs/tags/{name}"], cwd=ROOT).returncode == 0:
            return name
        git("tag", "-d", name)
    raise SystemExit("could not claim a version tag")


def cmd_version(args: argparse.Namespace) -> int:
    info = commit_info()
    version = claim_version(info)
    print(f"{info['short']} on {info['branch']} is {version}")
    if out := os.environ.get("GITHUB_OUTPUT"):
        with open(out, "a", encoding="utf-8") as f:
            f.write(f"version={version}\n")
    return 0


# --------------------------------------------------------------------------
# configure / build
# --------------------------------------------------------------------------

def compiler_from_cache() -> str | None:
    for path in sorted((BUILD / "CMakeFiles").glob("*/CMakeCXXCompiler.cmake")):
        text = path.read_text(encoding="utf-8", errors="replace")
        ident = re.search(r'set\(CMAKE_CXX_COMPILER_ID "([^"]*)"\)', text)
        version = re.search(r'set\(CMAKE_CXX_COMPILER_VERSION "([^"]*)"\)', text)
        variant = re.search(r'set\(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "([^"]*)"\)', text)
        if ident:
            name = ident.group(1)
            if variant and variant.group(1) == "MSVC" and name == "Clang":
                name = "clang-cl"
            return f"{name} {version.group(1) if version else ''}".strip()
    return None


def cmd_configure(args: argparse.Namespace) -> int:
    platform = PLATFORMS[args.platform]
    options = list(platform.configure)
    # ccache, where installed, makes a commit recompile only what it changed.
    # Not on Windows: RelWithDebInfo's /Zi shares one .pdb across objects,
    # which ccache can't cache.
    ccache = platform.key != "windows" and shutil.which("ccache") is not None
    if ccache:
        options += ["-DCMAKE_C_COMPILER_LAUNCHER=ccache", "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache"]
    # The version the build job was handed (BuildInfo.cmake works one out
    # from the tags when there is none).
    version = os.environ.get("REVENANT_VERSION", "")
    if version:
        options.append(f"-DREVENANT_VERSION={version}")
    cmd = ["cmake", "-S", str(ROOT), "-B", str(BUILD), "-G", "Ninja",
           f"-DCMAKE_BUILD_TYPE={CONFIG}", *options]
    rc, seconds = run_logged(cmd, OUT / "configure.log")
    cmake_version = subprocess.run(["cmake", "--version"], capture_output=True,
                                   text=True).stdout.splitlines()[0].split()[-1]
    write_record("configure", {
        "ok": rc == 0, "seconds": seconds, "config": CONFIG,
        "generator": "Ninja", "cmake": cmake_version, "compiler": compiler_from_cache(),
        "options": options, "ccache": ccache, "version": version or None,
    })
    return rc


# file:line:col: warning: message [-Wflag]   (Clang, GCC)
# file(line,col): warning: message [-Wflag]  (clang-cl)
DIAGNOSTIC = re.compile(
    r"^(?P<file>.+?)(?::(?P<line>\d+):(?P<col>\d+):|\((?P<line2>\d+),(?P<col2>\d+)\):)"
    r" (?P<kind>warning|error|fatal error): (?P<msg>.*?)(?: \[(?P<flag>[^\]]+)\])?\s*$")
LINK_ERROR = re.compile(r"undefined reference to|undefined symbol|error LNK\d+|ld: error|"
                        r"ld returned|linker command failed", re.IGNORECASE)


def scan_build_log(text: str) -> dict:
    """Unique compiler diagnostics (a header warns once, not once per
    includer), the targets Ninja failed, and the link errors."""
    warnings: dict[tuple, str] = {}
    flags: dict[str, int] = {}
    errors: list[str] = []
    seen_errors: set[tuple] = set()
    failed_targets: list[str] = []
    link_errors: list[str] = []
    for raw in text.splitlines():
        line = raw.strip()
        if line.startswith("FAILED: "):
            failed_targets.append(line[len("FAILED: "):])
            continue
        m = DIAGNOSTIC.match(line)
        if m:
            file = os.path.normpath(m["file"]).replace("\\", "/")
            key = (file, m["line"] or m["line2"], m["col"] or m["col2"], m["msg"])
            if m["kind"] == "warning":
                if key not in warnings:
                    warnings[key] = line
                    flag = (m["flag"] or "other").split(",")[0]
                    flags[flag] = flags.get(flag, 0) + 1
            elif key not in seen_errors:
                seen_errors.add(key)
                errors.append(line)
        elif LINK_ERROR.search(line) and line not in link_errors:
            link_errors.append(line)
    return {
        "warnings": len(warnings),
        "warning_flags": dict(sorted(flags.items(), key=lambda kv: -kv[1])),
        "errors": errors,
        "failed_targets": failed_targets,
        "link_errors": link_errors,
    }


def ccache_stats() -> dict:
    """This build's compiler-cache hits and misses (stats zeroed before it)."""
    out = subprocess.run(["ccache", "--print-stats"], capture_output=True, text=True).stdout
    stats = dict(line.split("\t", 1) for line in out.splitlines() if "\t" in line)
    count = lambda key: int(stats.get(key, "0").strip() or 0)
    return {"hits": count("direct_cache_hit") + count("preprocessed_cache_hit"),
            "misses": count("cache_miss")}


def cmd_build(args: argparse.Namespace) -> int:
    log = OUT / "build.log"
    ccache = (read_record("configure") or {}).get("ccache")
    if ccache:
        subprocess.run(["ccache", "--zero-stats"], capture_output=True)
    # -k 0: build everything that can be built, so one run reports every
    # failing file rather than the first.
    rc, seconds = run_logged(["cmake", "--build", str(BUILD), "--", "-k", "0"], log)
    record = {"ok": rc == 0, "seconds": seconds,
              **scan_build_log(log.read_text(encoding="utf-8", errors="replace"))}
    if ccache:
        record["ccache"] = ccache_stats()
    write_record("build", record)
    for error in (record["errors"] + record["link_errors"])[:20]:
        annotate("error", "Build", error)
    return rc


# --------------------------------------------------------------------------
# test
# --------------------------------------------------------------------------

def cmd_test(args: argparse.Namespace) -> int:
    junit = OUT / "ctest-junit.xml"
    cmd = ["ctest", "--test-dir", str(BUILD), "--output-on-failure", "--no-tests=error",
           "--timeout", "300", "--output-junit", str(junit),
           "--parallel", str(os.cpu_count() or 2)]
    rc, seconds = run_logged(cmd, OUT / "test.log")
    record = {"ok": rc == 0, "seconds": seconds, **read_junit(junit)}
    write_record("test", record)
    for case in record["cases"]:
        if case["status"] == "failed":
            annotate("error", f"Test failed: {case['name']}", case["output"][-1500:])
    return rc


def read_junit(path: Path) -> dict:
    if not path.exists():
        return {"counts": {}, "cases": []}
    suite = ET.parse(path).getroot()
    cases = []
    for case in suite.iter("testcase"):
        status = case.get("status", "run")
        if case.find("failure") is not None or status == "fail":
            outcome = "failed"
        elif status == "disabled":
            outcome = "disabled"
        elif case.find("skipped") is not None or status == "notrun":
            outcome = "skipped"
        else:
            outcome = "passed"
        out = case.findtext("system-out") or ""
        cases.append({
            "name": case.get("name", "?"),
            "status": outcome,
            "seconds": float(case.get("time") or 0),
            # The tail holds the assertion; the head is gtest's banner.
            "output": "\n".join(out.splitlines()[-60:]) if outcome == "failed" else "",
        })
    counts = {k: sum(1 for c in cases if c["status"] == k)
              for k in ("passed", "failed", "skipped", "disabled")}
    return {"counts": counts, "cases": cases}


# --------------------------------------------------------------------------
# package
# --------------------------------------------------------------------------

def is_lfs_pointer(path: Path) -> bool:
    with open(path, "rb") as f:
        return f.read(40).startswith(b"version https://git-lfs.github.com/spec/")


def render_readme(platform: Platform, info: dict, downloads_url: str) -> str:
    """tools/ci/PACKAGE_README.md for one platform: its <!-- key -->…<!-- /key -->
    blocks kept, the other platforms' dropped, the placeholders filled."""
    text = (ROOT / "tools/ci/PACKAGE_README.md").read_text(encoding="utf-8")
    keys = "|".join(PLATFORMS)
    text = re.sub(rf"<!-- ({keys}) -->(.*?)<!-- /\1 -->",
                  lambda m: m[2] if m[1] == platform.key else "", text, flags=re.S)
    text = re.sub(r"\n{3,}", "\n\n", text)
    built = dt.datetime.fromtimestamp(time.time(), dt.timezone.utc)
    version = (read_record("configure") or {}).get("version") or "development build"
    return string.Template(text).substitute(
        platform=platform.label,
        version=version,
        exe=platform.ship,
        short=info["short"],
        sha=info["sha"],
        branch=info["branch"],
        commit_url=info["commit_url"],
        run=f"[CI run {os.environ.get('GITHUB_RUN_ID')}]({info['run_url']})" if info["run_url"] else "local build",
        built=built.strftime("%Y-%m-%d %H:%M UTC"),
        repo_url=info["repo_url"],
        latest_url=f"{downloads_url}/{platform.key}/latest/{platform.zip_name}",
    )


def staged_executable(platform: Platform, scratch: Path) -> Path:
    """The executable as shipped. Linux carries its DWARF inside the binary,
    so ship a stripped copy; macOS keeps debug info in the object files
    and Windows in the .pdb, neither of which ships."""
    exe = BUILD / platform.exe
    if platform.key != "linux":
        return exe
    stripped = scratch / platform.ship
    tool = shutil.which("llvm-strip") or shutil.which("strip")
    if not tool:
        return exe
    subprocess.run([tool, "--strip-debug", "-o", str(stripped), str(exe)], check=True)
    return stripped


def cmd_package(args: argparse.Namespace) -> int:
    platform = PLATFORMS[args.platform]
    info = commit_info()
    exe = BUILD / platform.exe
    if not exe.exists():
        print(f"error: {exe} was not built", file=sys.stderr)
        return 1

    assets = sorted(p for p in (ROOT / "assets").rglob("*")
                    if p.is_file() and not any(part.startswith(".")
                                               for part in p.relative_to(ROOT).parts))
    pointers = [p for p in assets if is_lfs_pointer(p)]
    if pointers:
        print("error: Git LFS pointers instead of files (run `git lfs pull --include=assets/**`):",
              *[f"  {p.relative_to(ROOT)}" for p in pointers], sep="\n", file=sys.stderr)
        return 1

    # Entries carry the commit's time, so the same commit packs the same bytes.
    stamp = dt.datetime.fromtimestamp(info["time"], dt.timezone.utc).timetuple()[:6]

    def add(zf: zipfile.ZipFile, name: str, data: bytes, mode: int) -> None:
        entry = zipfile.ZipInfo(name, date_time=stamp)
        entry.create_system = 3                     # Unix: honour the mode bits
        entry.external_attr = (0o100000 | mode) << 16
        entry.compress_type = zipfile.ZIP_DEFLATED
        zf.writestr(entry, data, compresslevel=9)

    DIST.mkdir(exist_ok=True)
    out = DIST / platform.zip_name
    with tempfile.TemporaryDirectory() as scratch, \
            zipfile.ZipFile(out, "w") as zf:
        add(zf, platform.ship, staged_executable(platform, Path(scratch)).read_bytes(), 0o755)
        for path in assets:
            add(zf, path.relative_to(ROOT).as_posix(), path.read_bytes(), 0o644)
        add(zf, "README.md", render_readme(platform, info, args.downloads_url).encode("utf-8"), 0o644)

    data = out.read_bytes()
    write_record("package", {
        "ok": True,
        "file": out.name,
        "bytes": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "exe_bytes": exe.stat().st_size,
        "entries": 2 + len(assets),
    })
    print(f"packaged {out.relative_to(ROOT)} ({fmt_bytes(len(data))})")
    return 0


# --------------------------------------------------------------------------
# report (per platform): one record for the publish job
# --------------------------------------------------------------------------

def cmd_report(args: argparse.Namespace) -> int:
    platform = PLATFORMS[args.platform]
    report = {
        "platform": platform.key,
        "version": os.environ.get("REVENANT_VERSION") or None,
        "label": platform.label,
        "runner": " ".join(filter(None, (os.environ.get("ImageOS"), os.environ.get("ImageVersion"))))
                  or sys.platform,
        "configure": read_record("configure"),
        "build": read_record("build"),
        "test": read_record("test"),
        "package": read_record("package"),
    }
    write_record(f"report-{platform.key}", report)
    print(platform_detail(report), flush=True)
    return 0


# --------------------------------------------------------------------------
# publish: archive by commit, promote main's tip to latest/
# --------------------------------------------------------------------------

def tip_of(branch: str) -> str | None:
    out = git("ls-remote", "origin", f"refs/heads/{branch}")
    return out.split()[0] if out else None


def archive_folder(s3, bucket: str, sha: str) -> str:
    """The commit's archive folder: its short hash, lengthened (as git
    abbreviates) when another commit already holds that folder."""
    for n in range(SHORT_HASH, len(sha) + 1):
        prefix = sha[:n]
        held = s3.list_objects_v2(Bucket=bucket, Prefix=f"{prefix}/", MaxKeys=1).get("Contents")
        if not held:
            return prefix
        owner = s3.head_object(Bucket=bucket, Key=held[0]["Key"])["Metadata"].get("commit")
        if owner == sha:
            return prefix                           # a rerun of this commit
    return sha


def cmd_publish(args: argparse.Namespace) -> int:
    import boto3                                    # publish job only

    info = commit_info()
    s3 = boto3.client(
        "s3",
        endpoint_url=f"https://{os.environ['R2_ACCOUNT_ID']}.r2.cloudflarestorage.com",
        aws_access_key_id=os.environ["R2_ACCESS_KEY_ID"],
        aws_secret_access_key=os.environ["R2_SECRET_ACCESS_KEY"],
        region_name="auto",
    )
    bucket = args.bucket
    base = args.downloads_url.rstrip("/")

    # latest/ follows main, all platforms together: it moves only when the
    # commit passed everywhere and is still main's tip (a run finishing
    # after a newer push leaves latest/ to that push's run), so the three
    # latest/ links always name one commit.
    promote, why = False, f"not on {MAIN_BRANCH}"
    if info["branch"] == MAIN_BRANCH:
        tip = tip_of(MAIN_BRANCH)
        promote = tip == info["sha"]
        why = "" if promote else f"{MAIN_BRANCH} has moved on to {(tip or '?')[:SHORT_HASH]}"

    published = {}
    passed = {}
    for platform in PLATFORMS.values():
        report = read_record(f"report-{platform.key}", args.artifacts)
        package = (report or {}).get("package")
        test = (report or {}).get("test") or {}
        zip_path = next(args.artifacts.rglob(platform.zip_name), None)
        if package and zip_path and test.get("ok"):
            passed[platform.key] = (zip_path, package)
        else:
            published[platform.key] = {"published": False,
                                       "reason": "build or tests failed" if report else "no build"}
    if promote and len(passed) < len(PLATFORMS):
        failed = ", ".join(p.label for k, p in PLATFORMS.items() if k not in passed)
        promote, why = False, f"{failed} did not pass, so no latest/ moved"

    folder = archive_folder(s3, bucket, info["sha"])
    for key, (zip_path, package) in passed.items():
        platform = PLATFORMS[key]
        metadata = {"commit": info["sha"], "branch": info["branch"],
                    "version": os.environ.get("REVENANT_VERSION", ""),
                    "run": os.environ.get("GITHUB_RUN_ID", "local"), "sha256": package["sha256"]}
        archive_key = f"{folder}/{platform.zip_name}"
        s3.upload_file(str(zip_path), bucket, archive_key, ExtraArgs={
            "ContentType": "application/zip",
            "CacheControl": "public, max-age=31536000, immutable",
            "Metadata": metadata,
        })
        entry = {"published": True, "archive_url": f"{base}/{archive_key}",
                 "latest_url": None, "promoted": False, "reason": why}
        print(f"archived {platform.label}: {entry['archive_url']}")

        if promote:
            latest_key = f"{platform.key}/latest/{platform.zip_name}"
            s3.copy_object(Bucket=bucket, Key=latest_key,
                           CopySource={"Bucket": bucket, "Key": archive_key},
                           MetadataDirective="REPLACE", ContentType="application/zip",
                           CacheControl="no-cache", Metadata=metadata)
            entry.update(latest_url=f"{base}/{latest_key}", promoted=True)
            print(f"promoted {platform.label}: {entry['latest_url']}")
        published[key] = entry

    write_record("publish", {"commit": info["sha"], "platforms": published})
    return 0


# --------------------------------------------------------------------------
# summary: the run's report page
# --------------------------------------------------------------------------

STATUS = {True: "✅", False: "❌", None: "⚪"}


def step_ok(record: dict | None) -> bool | None:
    return None if record is None else bool(record.get("ok"))


def test_cell(test: dict | None) -> str:
    if not test:
        return "⚪ not run"
    c = test.get("counts", {})
    parts = [f"{c.get('passed', 0)} passed"]
    for key in ("failed", "skipped", "disabled"):
        if c.get(key):
            parts.append(f"{c[key]} {key}")
    return f"{STATUS[step_ok(test)]} " + " · ".join(parts)


def ccache_note(stats: dict | None) -> str:
    if not stats:
        return ""
    total = stats["hits"] + stats["misses"]
    return f" · ccache {stats['hits']}/{total} hits" if total else ""


def platform_detail(report: dict) -> str:
    """The build and test report of one platform, as markdown."""
    configure = report.get("configure") or {}
    build = report.get("build") or {}
    test = report.get("test") or {}
    package = report.get("package") or {}
    lines = [f"#### {report['label']}", ""]
    lines += [
        "| | |", "|---|---|",
        f"| Runner | {report.get('runner') or '—'} |",
        f"| Compiler | {configure.get('compiler') or '—'} |",
        f"| CMake | {configure.get('cmake') or '—'} · {configure.get('config', CONFIG)} · Ninja |",
        f"| Configure | {STATUS[step_ok(configure or None)]} {fmt_seconds(configure.get('seconds'))} |",
        f"| Build | {STATUS[step_ok(build or None)]} {fmt_seconds(build.get('seconds'))} · "
        f"{build.get('warnings', '—')} unique warnings{ccache_note(build.get('ccache'))} |",
        f"| Tests | {test_cell(test or None)} · {fmt_seconds(test.get('seconds'))} |",
        f"| Package | {package.get('file', '—')} · {fmt_bytes(package.get('bytes'))} · "
        f"executable {fmt_bytes(package.get('exe_bytes'))} |",
        "",
    ]
    if package.get("sha256"):
        lines += [f"SHA-256 `{package['sha256']}`", ""]

    problems = build.get("errors", []) + build.get("link_errors", [])
    if build.get("failed_targets"):
        lines += ["**Failed targets:** " + ", ".join(f"`{t}`" for t in build["failed_targets"][:20]), ""]
    if problems:
        lines += ["**Build errors** (first 30):", "", "```", *problems[:30], "```", ""]

    flags = build.get("warning_flags") or {}
    if flags:
        lines += ["<details><summary>Warnings by kind</summary>", "",
                  "| Count | Kind |", "|---:|---|",
                  *[f"| {n} | `{flag}` |" for flag, n in list(flags.items())[:25]],
                  "", "</details>", ""]

    cases = test.get("cases") or []
    for case in (c for c in cases if c["status"] == "failed"):
        lines += [f"<details open><summary>❌ <code>{case['name']}</code></summary>", "",
                  "```", case["output"] or "(no output)", "```", "", "</details>", ""]
    if cases:
        icon = {"passed": "✅", "failed": "❌", "skipped": "⏭️", "disabled": "🚫"}
        lines += [f"<details><summary>All {len(cases)} tests</summary>", "",
                  "| | Test | Time |", "|---|---|---:|",
                  *[f"| {icon[c['status']]} | `{c['name']}` | {c['seconds']:.2f}s |"
                    for c in sorted(cases, key=lambda c: c["name"])],
                  "", "</details>", ""]
    return "\n".join(lines)


def cmd_summary(args: argparse.Namespace) -> int:
    info = commit_info()
    reports = {k: read_record(f"report-{k}", args.artifacts) for k in PLATFORMS}
    publish = read_record("publish") or {}
    published = publish.get("platforms", {})

    version = os.environ.get("REVENANT_VERSION")
    tag = f"[{version}]({info['repo_url']}/releases/tag/{version}) · " if version else ""
    md = [f"## {PRODUCT} {tag}[`{info['short']}`]({info['commit_url']}) on `{info['branch']}`", "",
          f"> {info['subject']} — {info['author']}", ""]

    md += ["| Platform | Build | Tests | Warnings | Build time | Download |",
           "|---|---|---|---:|---:|---|"]
    for key, platform in PLATFORMS.items():
        report = reports[key] or {}
        build = report.get("build")
        pub = published.get(key, {})
        if pub.get("published"):
            download = f"[zip]({pub['archive_url']}) ({fmt_bytes((report.get('package') or {}).get('bytes'))})"
            if pub.get("promoted"):
                download += f" · [latest]({pub['latest_url']})"
        else:
            download = "—"
        md.append(f"| {platform.label} | {STATUS[step_ok(build)]} | {test_cell(report.get('test'))} | "
                  f"{(build or {}).get('warnings', '—')} | {fmt_seconds((build or {}).get('seconds'))} | "
                  f"{download} |")
    md.append("")

    md += ["### Downloads", ""]
    for key, platform in PLATFORMS.items():
        pub = published.get(key, {})
        if pub.get("published"):
            line = f"- **{platform.label}**: {pub['archive_url']}"
            if pub.get("promoted"):
                line += f"  \n  latest: {pub['latest_url']}"
            elif pub.get("reason"):
                line += f"  \n  latest/ not updated: {pub['reason']}"
            annotate("notice", f"{platform.label} build", pub["archive_url"])
        else:
            line = f"- **{platform.label}**: not published ({pub.get('reason') or 'publish did not run'})"
        md.append(line)
    md.append("")

    md += ["### Build and test reports", ""]
    for key, platform in PLATFORMS.items():
        report = reports[key]
        md.append(platform_detail(report) if report else
                  f"#### {platform.label}\n\nNo report: the job did not finish.\n")

    if info["run_url"]:
        md += [f"<sub>[Run {os.environ.get('GITHUB_RUN_ID')}]({info['run_url']}) · "
               f"{info['repo_url']}</sub>", ""]
    emit_summary("\n".join(md))
    return 0


# --------------------------------------------------------------------------

def main() -> int:
    # The reports are UTF-8 (status marks); Windows consoles default to cp1252.
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="command", required=True)
    downloads = os.environ.get("DOWNLOADS_URL", "https://downloads.revenantrevisited.com")

    sub.add_parser("version")
    p = sub.add_parser("configure"); p.add_argument("--platform", choices=PLATFORMS, required=True)
    sub.add_parser("build")
    sub.add_parser("test")
    p = sub.add_parser("package"); p.add_argument("--platform", choices=PLATFORMS, required=True)
    p.add_argument("--downloads-url", default=downloads)
    p = sub.add_parser("report"); p.add_argument("--platform", choices=PLATFORMS, required=True)
    p = sub.add_parser("publish"); p.add_argument("--artifacts", type=Path, required=True)
    p.add_argument("--bucket", default=os.environ.get("R2_BUCKET", "revenant"))
    p.add_argument("--downloads-url", default=downloads)
    p = sub.add_parser("summary"); p.add_argument("--artifacts", type=Path, required=True)

    args = parser.parse_args()
    return {
        "version": cmd_version, "configure": cmd_configure, "build": cmd_build, "test": cmd_test,
        "package": cmd_package, "report": cmd_report, "publish": cmd_publish,
        "summary": cmd_summary,
    }[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
