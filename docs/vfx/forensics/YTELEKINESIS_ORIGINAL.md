# Original YTelekinesis

Exact type0xaeaeeb28 uses Misc/chunk.i3d, four independently textured chunk
meshes. It binds animator factory509eb0/vtable5b1ab8: Init505f90,
Animate506240 and Render506510. The owner505f60 delegates to generic Effect
Pulse. This is distinct from the three-state JhagaAttack owner505840.

The bounded reference supplies a stationary global player position at origin,
a null spell and normal original Scene1 caller state. Original Init centers the
owner on that position, randomly chooses2–4 rocks, assigns initial radius480–640,
velocity6–12 toward the center, scale8.5, random rotation/angular velocity and
per-rock lifetime1000/speed. Animate accelerates linear/angular velocity by1.01,
marks an XY hit inside30 units, and kills the owner when every lifetime ends.
Dead rocks remain in Render until owner kill; the port must preserve this.
The null-spell branch marks hits without executing actor damage/reaction APIs.

Seed1 chooses four rocks and consumes45 RNG draws. A compiled production core
matches180 original ticks plus initialization:14,480 scalar fields, maximum
printed-float error4.532e-6, exact integer state/hit/lifetime/RNG. The original
owner kills at zero-based record165. Full original Render executes all four
normal-lit textured meshes; resource residency, base animator initialization
and map notification services remain declared fixture boundaries.

Private feasibility evidence is under RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/{ytelekinesis-first,ytelekinesis-core-first,ytelekinesis-raster-first}`.
This checkpoint grants no Metal appearance credit. Natural player movement,
spell damage/actors, device and exact framebuffer equality remain open.
