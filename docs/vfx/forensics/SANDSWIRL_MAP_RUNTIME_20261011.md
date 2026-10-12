# Sandswirl actual map runtime, qualified Sparks dependency

This implements the exact Sandswirl class builder (`0xbecefeda`) and parent /
seeking-particle flow. The intended `sand` type is absent from the licensed
registry. Ordinary construction therefore fails closed, sets the kill flag,
and creates no child. It never registers a global alias or silently substitutes
Sparks. A caller can explicitly opt one instance into `UseQualifiedSparks(41)`
before initialization. That admits only the previously verified four-object,
one-texture ARGB4444 Sparks asset and uses four stable mesh keys.

The particle animator runs from its real owner's virtual `Pulse`, using the
engine GameRand stream. Parent setup creates its child with actual
`MapPane.NewObject` and resolves it through `GetInstance`. Tick 75 uses the real
map range query and live character health/identity; the first five eligible
characters are selected in query order. Target reset uses the original 176-byte
particle contract. No captured seed, count, position, or target list is injected.
The global Sparks implementation remains unchanged.

Each Quicksand child is an actual map object, with its own verified resource.
Sandswirl's original public initializer `4edb30` is `ret 8`: it does not bind a
victim or spell. Only these newly created null-spell instances receive the
original three-frame / 56-pulse lifetime adapter. No global Quicksand builder,
accepted target-bound fixture, or damage behavior is replaced. In the declared
snapshot schedule a tick-75 child first pulses at 76 and kills at 131; the parent
kills at 300. The parent never submits its otherwise misleading prototype mesh.

`SANDSWIRL_MAP_QUALIFIED_SPARKS` is a direct diagnostic name only. It creates a
disposable source map, two real Acolytes, actual BeginFighting and the loaded
Quicksand TSpell constructor, then allocates the exact Sandswirl builder. No
named Sandswirl variant exists, so Quicksand remains an explicit caller control.
It also constructs and deletes a normal, unqualified Sandswirl instance and
checks missing-child failure, no child allocation and unchanged RNG.

## Deliberately modified retail allocator boundary

The modern map allocator intentionally uses monotonic, collision-checked IDs
without RNG. Original `44ce30` and the inline `451154..4511c7` allocator reseed
and consume two CRT draws each. The opt-in
`--source-map-allocation-control` runs those original bodies, then replaces only
the generated index and restores their incoming RNG state/counter. This matches
the modern policy while leaving the original NewObject, GetInstance, map
insertion, target query, public initializers, controllers and full renderer
executing. Every replacement records its addresses, incoming/outgoing seed,
raw draw count and supplied ID. It is not an unmodified-retail reference.

The existing unmodified allocator / qualified imagery reference stays separate.
An initial attempt to restore RNG alone retained duplicate time-based IDs and
hit the native allocator's retry limit; that failed run is retained. Unique IDs
are essential to this declared allocator control, not an effect/RNG fit.

Retail `seekz` is an uninitialized stack word. The map-resident parent changes
its observed raw value, but both that nonzero value and explicit source 41 take
the same native boolean branch. No equality claim covers that raw word or other
unused stack parameter bytes. Intended sand appearance, a natural Sandswirl
caller, arbitrary scene/AI scheduling, and undefined-zero `seekz` remain open.
