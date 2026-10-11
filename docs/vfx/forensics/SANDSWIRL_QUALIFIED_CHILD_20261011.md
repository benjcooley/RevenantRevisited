# Sandswirl missing binding and explicit substitute-child diagnostic

The missing `sand` child is not a failed class-registry load. Original
`EffectClass.FindObjType` returns −1 for `sand`, while finding `sparks` at type34
and `Sandswirl` at type61. Original animator lookup also finds no sand animator:
it returns default factory `40dc00`; sparks resolves actual particle factory
`4f5eb0`, and Sandswirl resolves `4f8380`. These lookups execute after genuine
cached startup and use the loaded shipped registry.

The supplied older source makes the dependency explicit:
`effect_old.cpp:9475` requests class EFFECT `sand`, then casts the returned child
animator to `TParticle3DAnimator` and initializes its particles. At lines4677–4681,
the sparks registration for that particle animator is enabled, while the sand
registration is inside a block comment. The packed class definition has no sand
entry. No exact sand resource exists in the imagery archive; only the distinct
Sandswirl and Quicksand assets are present. The parent Sandswirl asset contains one
photon quad, which does not establish the missing child's identity. The old
reconstruction note grouping these effects as TStormAnimator is superseded by
the executed distinct factories and particle-animator interface.

## Original negative with active map window

The earlier loaded-sector probe correctly established the immediate missing-type
failure, but a registered sector alone does not make a created child visible to
`MapPane.GetInstance`. This probe additionally executes original SetMapPos
`450d20` and UpdateSectors `459220`, as the complete Strike fixture does. The
active window is empty and centered on declared source `(3185,3185,16)`.

Unmodified Init still requests class25/type−1 and returns no child. First original
owner Pulse faults at `4ee0bc`, reading address0. Thus supplying the genuine active
map window does not cure the shipped missing binding.

## Explicitly modified retail diagnostic

With `--substitute-sparks`, the tool changes only the immediate pointer in the
five-byte `push` at `4ede49`: the original literal `sand` becomes a separately
allocated literal `sparks`. The patch's before/after bytes are recorded. The
licensed executable file and all resource bytes remain unchanged; guest effect
code is modified. This is an explicit substitute using a known compatible
particle animator, **not an inferred sand alias, production fix or original
Sandswirl appearance reference**.

The original parent then creates a real Sparks owner through MapPane.NewObject
and GetInstance. Actual particle animator vtable `5a9ec0` receives all parameters
from original Sandswirl Init; it allocates85 particles and performs772 CRT random
draws. Original parent Pulse, child Pulse, owner matrices, both Render methods,
texture upload/selection and software raster execute for74 ticks. The parent
submits zero draws; the child supplies the visible points. Nonzero pixel counts
range385–5419, ending5317. Nine whole frames show the moving/collecting colored
Sparks points, without a changed scale, position, brightness or timer.

The original parameter struct contains stack-derived `seekz=41`; this is retained
and recorded, not silently initialized or normalized. The diagnostic therefore
does not establish a stable intended caller contract for that uninitialized
field. Tick75 reaches a separate missing dependency: original code at `4ee13d`
reads the null spell's invoker and faults at address4. No spell/actor/target return
is fabricated to pass that boundary. Natural caller, later Quicksand spawning,
full lifetime, intended child imagery and production comparison remain open.

Two independent modified runs agree in every one of74 tick/seed/frame/nonzero
pixel-count records and all nine selected PNG hashes. All original Render calls
and raster operations execute; PNG output is sampled only for storage.

## Reproduction and pins

Run `tools/retail_runtime/sandswirl_qualified_child_probe.py` with the source-matched
`--boot-image` and a fresh `--output`. Its default is the unchanged negative.
Add `--substitute-sparks` explicitly for the qualified diagnostic. Both modes
assert their expected native fault and retain a machine manifest. No engine,
renderer, class definition, gallery mapping, ledger or approval file is changed.

Private evidence is under `research/vfx-wide-20261010/hard-astra/`:

- `sandswirl-active-negative/manifest.json`:
  `240ac245b9ee62e975c448600e801d5287820a81eefdae2028e188021edcb444`.
- `sandswirl-qualified-final/manifest.json`:
  `21f385c6738c740f235d653ed63115830c03206af8ab93ee08114692b7859ec4`.
- `sandswirl-qualified-repeat.json`:
  `a3bb0192b5b185b3cf2e3985e41811625c25d8eb4dd88b08dd6e5defe053c303`.

Earlier private first/second attempts retain the active-window discovery; the
third mistakenly called the owner initialization vtable slot rather than Pulse
and provides no motion evidence. The fourth uses actual Pulse and supplies the
independent74-tick repeat above. The final committed tool checks the parent tick
on every iteration to prevent that mistake from passing.
