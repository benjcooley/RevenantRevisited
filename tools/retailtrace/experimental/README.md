# Retail trace hooks — experimental, not yet in the shared catalogue

Parked 2026-10-07 from the retail-trace work (docs/gameflow/RETAIL_TRACE.md
§5). Not proven enough for `recon/retail_asm/patches/`:

- `multi_hook.py`: `tools/retail_asm/function_hook.py` (the shared
  single-hook tool) grown into a hook-spec compiler: many hooks, one wrapper
  area, one freestanding C unit. Run it with `tools/retail_asm` (main
  checkout) on `PYTHONPATH`, for `reconstruct.pe_layout`. It is a separate
  copy so the shared tool stays as it is.
- `trace_hooks.py`: the gameflow spec and handlers (`Say` 0x004d0610,
  `CommandInterpreter` 0x0041e8e0, trigger start 0x00492440, `End`
  0x00493e40), a C runtime that formats with retail's `sprintf` 0x0058b100
  and writes through the IAT.
- `make_revtrace.py`, `verify_trace.py`: the DOSBox `RevTrace.exe` build
  (the lab's Win98 compatibility bytes applied) and its check.

State: only the `Say` handler has run, in Unicorn, with exact output. The
others never ran. `RevTrace.exe` booted to the title in the DOSBox lab and
then hung Win98 in the New Game opening; the cause was not found, so treat
the hooks as suspect until each runs under the emulator against the
unchanged control. To share: prove each hook in the emulator, then move the
spec compiler and the hooks into the catalogue (RETAIL_AB.md §3).
