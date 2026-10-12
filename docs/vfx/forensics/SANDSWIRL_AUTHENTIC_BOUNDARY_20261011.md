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

## Corrected immediate cause: missing shipped child type

`sandswirl_child_lookup_probe.py` executes original
TSector.LoadSector4982b0(0,3,3,0). It constructs/loads/registers an empty sector;
original FindLoadedSector499e10 returns that exact pointer. Residency is therefore
positively established without injected sector pointers or return substitution.

Original Init calls475210 for literal `sand` and returns−1 at4ede62. Its actual
MapPane.NewObject definition has class25/type−1. The loaded shipped registry
contains Quicksand(type58) and Sandswirl(type61), but no effect named sand.
MapPane.NewObject consequently fails before any sector query; GetInstance returns
null and the first owner Pulse faults at4ee0bc as before. This supersedes absent
sector residency as the immediate cause of the previous failure. No sand alias,
child pointer or authored behavior is fabricated. Primary appearance remains open.

Private `hard-alpha/sandswirl-child-lookup-final/manifest.json` SHA256:
`5cc3da65e8de2d317ec2835e2014b0b1a74aeecb37f89deb05d57785178d2a5c`. This public sector-loading API is reusable for other
reference callers whose child types actually exist.
