# Sandswirl genuine child-residency boundary

Exact Sandswirl0xbecefeda class NewObject constructs owner5ab72c and actual
CreateAnimator constructs5ab930 in the source-matched original startup VM.
Original Init4eddb0 requests its `sand` child through MapPane.NewObject450e40.
Read-only return observers record `0xffffffff` at4edeb1 and then null from
MapPane.GetInstance452690 at4edebc. The cached PlayScreen prefix has no loaded
sectors. The animator's actual child pointer+104 remains null.

First complete owner Pulse4edd80 then enters actual animator4ee030 and faults
at4ee0bc dereferencing that missing child. This is a concrete caller boundary,
not a complete effect or appearance proof. No original return, child pointer,
particle pool, actor, raster or simulation method was replaced. The old bespoke
pool does not establish genuine child behavior. A real sector-resident child or
a proven public attachment path is required before completing the parent.

Reproducer: `tools/retail_runtime/sandswirl_authentic_preflight.py` with the
source-matched `--boot-image` and fresh `--output`. Local manifest:
`hard-alpha/sandswirl-authentic-boundary/manifest.json`, SHA256
`46c84ae9f09d1ab374b5b8b15da94ab6c9d2243235859d9537668474aa214211`.
Accepted=false; no visual credit. This bounded preflight leaves ordinary engine
behavior unchanged and does not attempt a synthetic fallback.
