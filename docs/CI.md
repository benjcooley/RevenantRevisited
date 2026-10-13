# Continuous integration

Every commit pushed to GitHub, on any branch, is built and tested on macOS,
Windows and Linux. Packages that pass are published to
`https://downloads.revenantrevisited.com`, and `main` only takes pull
requests that pass on all three.

- Workflow: [.github/workflows/ci.yml](../.github/workflows/ci.yml)
- Driver: [tools/ci/ci.py](../tools/ci/ci.py), one subcommand per workflow step
- Package README: [tools/ci/PACKAGE_README.md](../tools/ci/PACKAGE_README.md)

## 1. Jobs

| Job | Runs on | Does |
|---|---|---|
| `Build & test (osx)` | `macos-latest` (Apple silicon) | configure, build, test, package |
| `Build & test (windows)` | `windows-latest` | the same |
| `Build & test (linux)` | `ubuntu-24.04` (x86-64) | the same |
| `Publish & report` | `ubuntu-24.04` | uploads passing packages to R2, writes the run summary |

A push to a branch other than `main` cancels that branch's run in flight;
runs on `main` are never cancelled, so each commit there gets its archive.
`workflow_dispatch` reruns any branch from the Actions tab.

### Toolchains

Clang everywhere, so warnings and language extensions match the macOS
development builds:

| Platform | Compiler | Graphics | Notes |
|---|---|---|---|
| macOS | Apple Clang | Metal | `CMAKE_OSX_DEPLOYMENT_TARGET=13.0` |
| Windows | clang-cl, MSVC libraries | Direct3D 11 | static C runtime (`/MT`) |
| Linux | Clang, libstdc++ | OpenGL 3.3 | X11 + ALSA |

All three build `RelWithDebInfo` with Ninja, the configuration the
development checkouts use. `CMakeLists.txt` pins `char` to signed on every
compiler (the 1998 code was written for MSVC on x86) and defines `NOMINMAX`
on Windows.

## 2. Steps

The workflow calls `tools/ci/ci.py`; each step leaves a record in
`ci-out/<step>.json`, and the report steps work from those records. The
same commands run locally:

```sh
python3 tools/ci/ci.py configure --platform osx   # cmake -G Ninja into build/
python3 tools/ci/ci.py build                      # cmake --build; counts warnings
python3 tools/ci/ci.py test                       # ctest, JUnit to ci-out/
python3 tools/ci/ci.py package --platform osx     # dist/RevenantRevisited-osx.zip
python3 tools/ci/ci.py report --platform osx      # ci-out/report-osx.json
```

`publish` and `summary` run in the publish job.

## 3. Tests: the merge gate

`ctest --test-dir build` runs every test registered at the end of
`CMakeLists.txt`:

- plain executables: pass when they exit 0;
- GoogleTest executables, each case its own test
  (`test_particlefx.ParticleFxDefs.TorchFlame`);
- the Python source-slice tests (`tools/test_*.py`), which compile a
  function cut out of `src/` into a fixture. POSIX hosts only: Windows
  skips them.

To add a test, add its executable as usual and its name to one of the
lists there. A test must pass without the retail install. One that needs
game data checks `REVENANT_DATA_PATH` and skips itself when it is unset
(`GTEST_SKIP()`), as the shipped-voice cases in `test_audio_decode` do.
CI has no game data.

`test_fx_source_alpha.py` is registered but disabled: its fixture predates
the prelit RGBA5 path in `TRenderer::SubmitFxQuad`. Reports list it as
disabled until the VFX track brings it up to date.

`cpu_tile_perspective_test` is a diagnostic image generator, not a test,
so it is built but not registered.

## 4. Packages

Each platform's zip holds:

```
RevenantRevisitedOSX        (RevenantRevisitedWindows.exe, RevenantRevisitedLinux)
assets/                     engine-owned data: effects.def, render_metadata.def, fonts, editor icons
README.md                   tools/ci/PACKAGE_README.md, filled in for the platform and commit
```

The program is renamed so the zip can be unpacked into the root of a
retail install (the README asks players for a copy of theirs) without
touching the game's own `Revenant.exe`; run from there it shares the
folder's `Revenant.ini` and saves, as retail does (docs/DATA_LAYOUT.md
§1). The Linux binary ships with its debug sections
stripped. Zip entries carry the commit's timestamp, so packing the same
build twice gives identical bytes.

Only `assets/**` is fetched from Git LFS. The rest of LFS is the retail
install under `data/`, which neither the build nor the tests read.

## 5. Downloads

Bucket `revenant` on Cloudflare R2, served at
`https://downloads.revenantrevisited.com` (`download.revenantrevisited.com`
serves the same bucket):

| Path | What | Cache |
|---|---|---|
| `<hash>/RevenantRevisited-<platform>.zip` | every passing build, on every branch | immutable, one year |
| `<platform>/latest/RevenantRevisited-<platform>.zip` | the newest build of `main` that passed on all three platforms | `no-cache` |

`<hash>` is the commit's 7-character short hash, as GitHub shows it;
if another commit already holds that folder, publish lengthens it the way
git lengthens an ambiguous abbreviation. `<platform>` is `osx`, `windows`
or `linux`. A package is published only
when its build and tests passed. `latest/` moves for all three platforms
together, only when the commit passed on all three and is still the tip of
`main` at publish time (a slow run never overwrites a newer one). The three
`latest/` links therefore always name the same commit; a commit that fails
anywhere leaves them all where they were.

The archived object's metadata records the commit, branch, CI run and
SHA-256.

## 6. The run summary

The `Publish & report` job writes the run's summary page, which also appears
in its log:

- one row per platform: build result, test counts, unique warnings, build
  time, download links;
- download URLs, and whether `latest/` moved;
- per platform: runner image, compiler, CMake, step times, package size and
  SHA-256, failed targets and the first build errors, warnings by kind, each
  failing test's output, and the full test list.

Download links and failures are also posted as annotations at the top of the
run.

## 7. Protecting `main`

A repository ruleset on `main`:

- changes arrive through pull requests, from everyone, administrators
  included; approvals are not required;
- `Build & test (osx)`, `Build & test (windows)` and `Build & test (linux)`
  must pass on the pull request's head commit;
- the branch must be up to date with `main` (merge `main` in, push, let CI
  pass);
- no force pushes, and `main` cannot be deleted.

The checks come from the push of the branch, so a pull request is ready to
merge once its branch's latest push has passed. Pull requests from forks
don't trigger the workflow; a maintainer pushes the fork's branch to this
repository to run it.

## 8. Secrets

Repository secrets used by the publish job:

| Secret | Use |
|---|---|
| `R2_ACCOUNT_ID` | R2 S3 endpoint `https://<account>.r2.cloudflarestorage.com` |
| `R2_ACCESS_KEY_ID`, `R2_SECRET_ACCESS_KEY` | bucket-scoped R2 key (object read and write) |

`R2_API_TOKEN` is also stored but unused: the bucket key can't administer
the bucket or its domains, which is done in the Cloudflare dashboard.

## 9. Known gaps

- GPU readback (`RendererReadback`) exists only for Metal. On Windows and
  Linux the stubs in `src/platform/` report it as unsupported, so
  `--snap`, `--filmstrip` and editor picking do nothing there.
- The editor's window resize is macOS-only; elsewhere the window keeps its
  size (`src/platform/editorplatform_stub.cpp`).
- Windows and Linux builds are compiled and unit-tested on every commit,
  but the game is played on macOS.
