# Local retail test setup

The repository contains source, test tools and audit documentation. Generated
retail executables, assembly listings, captures and large comparison reports
stay local under ignored `recon/retail_asm/` directories.

Use Python 3.12 and install the test dependencies:

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r tools/retail_runtime/requirements.txt
```

Install NASM and a C/C++ compiler for assembly roundtrip and production-code
probes. Clang must support `--target=i686-pc-windows-msvc` for the small COFF
instrumentation example. The dirty-page helper builds with `cc`; the runtime
has a Python fallback if that helper cannot be built.

Provide your own unchanged retail executable. Fixed-address fixtures require
SHA-256 `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Generate the byte-identical baseline locally:

```sh
.venv/bin/python tools/retail_asm/reconstruct.py /path/to/Revenant.exe \
  --output recon/retail_asm/baseline
```

The generated NASM source and `Revenant.rebuilt.exe` stay ignored. Effect probes
also need the installed `data/imagery.rvi` archive and game resources. Tests do
not fetch reference reports or instrumented images.

Run the synthetic PE tests without a retail fixture:

```sh
.venv/bin/python -m unittest discover -s tools/retail_asm -p 'test_*.py'
```

Runtime tests require the local baseline. Effect tests also compile current
game methods against the installed imagery. The compiled logger test requires
`recon/retail_asm/experiments/winmain_skip`; evidence-routing tests require the
retained reports referenced by `docs/vfx/EFFECT_BURNDOWN.json`. With those lab
fixtures available, run:

```sh
.venv/bin/python -m unittest discover -s tools/retail_runtime -p 'test_*.py'
```

Use `tools/retail_asm/function_hook.py --help` for private hook generation and
generate the return-hook fixture used by the logger regression:

```sh
.venv/bin/python tools/retail_asm/function_hook.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/experiments/winmain_skip \
  --target 0x4865a0 --mode return --stack-cleanup 16 --return-value 0
```

On the current Mac, baseline,
experiments and reports can be linked from the original lab checkout into
those three ignored directories. They are validation inputs, not repository
deliverables. The runtime rejects mismatched or modified executables unless
an explicit named-build contract is provided.
