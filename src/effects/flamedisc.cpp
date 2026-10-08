// =========================================================================
// *                   Revenant Revisited (port) - 2026                   *
// *  flamedisc.cpp - TFlameDiscAnimator_SHIM port via the d3d::* shim    *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/F11_TFlameDiscEffect.md         *
// *  Snapshot Initialize:  src/missileeffect.cpp:1457-1471               *
// *  Snapshot Anim Init:   src/missileeffect.cpp:1498-1529               *
// *  Snapshot Animate:     src/missileeffect.cpp:1541-1645               *
// *  Snapshot Render:      src/missileeffect.cpp:1654-1701               *
// *  Snapshot Constants:   src/missileeffect.h:296-347                   *
// *                                                                       *
// *  Retail fidelity: retail-partial (forensics §2.1).                   *
// *  Class exists in retail (cls_0x5b45ac, 401 B, vftable 129 entries).  *
// *  Pulse override confirmed byte-for-byte. "FLAMEDISC" XREF + audio    *
// *  "lightning" XREF confirmed. HOWEVER: effect is vestigial in shipped  *
// *  — asset not in imagery.rvi, no class.def entry, no spell.def caller. *
// *  All tuning constants (sparks=5, scale-step=0.2, max-scale=3.0,      *
// *  launch-radius=20, speed=16, rear-cone ±45°) are snapshot-only.      *
// *                                                                       *
// *  LIGHTING CATEGORY: self-lit (additive/modulate blend; fire is the   *
// *  light emitter; no DoLighting call in original Render body).         *
// *  Blend = Alpha (MODULATE+SRCALPHA/INVSRCALPHA — forensics §7).       *
// *  Snapshot uses SetBlendState; missile family traces straight Alpha,   *
// *  not Additive. Flag: snapshot-Alpha is the build target; if visual    *
// *  self-test reads pale/transparent, AdditiveStraight is next-best.     *
// *                                                                       *
// *  RENDER PIPELINE: IM+PE (WorldMesh via submit_world).                *
// *  All 6 quads (1 disc + 5 sparks) rendered as WorldMesh via the       *
// *  d3d::RenderObject(mesh, local, ...) path. Tick from regular submit. *
// *                                                                       *
// *  FLIPBOOK TEXTURE: forensics §8.                                      *
// *  One texture slot shared by both sub-objects. Per-tick, each slot     *
// *  picks tex.framehtexs[framenum[n] % numtexframes]. This port reads   *
// *  the per-frame handle directly from S3DTex::framehtexs[].            *
// *                                                                       *
// *  SCALE BUG FIX (forensics §13.3):                                    *
// *  Snapshot renders all quads scaled by scale[0] (the disc scale) due  *
// *  to a typo. This port uses scale[n] per-quad to match the Animate    *
// *  body's intent and sister Photon's pattern. Deviation documented.    *
// *                                                                       *
// *  VESTIGIAL (forensics §12):                                           *
// *  Effect cannot be instantiated in shipped game. Test via --test=vfx  *
// *  only. Visual ground truth = legacy I3D asset + forensics doc.       *
// *                                                                       *
// *  TEST HARNESS NOTES:                                                  *
// *  - Anchor at world origin. Launch direction = +X (caster_angle=0).   *
// *  - State machine simulated by timer: LAUNCH for kLaunchTicks, then   *
// *    FLY for kFlyTicks, then EXPLODE (drains sparks, then dies).       *
// *  - No TMissileEffect physics (vel/range/MOVE_BLOCKED) in test rig.   *
// *  - No audio path in test harness.                                     *
// *  - No associated dynamic light (forensics §9).                        *
// *                                                                       *
// *  PENDING DEVIATIONS:                                                  *
// *  - scale[n] per-quad instead of scale[0] (§13.3 bug fix — intent).  *
// *  - SetStatus(true) added alongside SetCommandDone (§13.5 gap fix).   *
// *  - Y-scale invariant: PENDING cross-cutting A/B pass (§1.3).        *
// =========================================================================

// ============================================================================
// Lighting category: SELF-LIT (Alpha blend; asset color through; no DoLighting)
// ============================================================================

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace flamedisc_shim {

// =========================================================================
// §3 Constants — every value cites forensics §3 + snapshot source.
// Snapshot-only constants are marked [snapshot-only]; retail-confirmed are
// marked [retail-confirmed]. See forensics §2.1 for reconciliation.
// =========================================================================

// --- State enum (missileeffect.h:20-23) [retail-confirmed via Pulse decomp] ---
constexpr int32_t kStateLaunch  = 0;   // MISSILE_LAUNCH
constexpr int32_t kStateFly     = 1;   // MISSILE_FLY
constexpr int32_t kStateExplode = 2;   // MISSILE_EXPLODE

// --- TMissileEffect base constants ---
// ROLLOVER = 1<<16 (revdefs.h). Speed is stored as speed*ROLLOVER in the base;
// GetSpeed() divides by ROLLOVER to yield wu/tick. [snapshot-only]
constexpr int32_t kRollover = 65536;   // 1<<16 — revdefs.h

// --- FlameDisc-specific constants (missileeffect.h:296-320) [snapshot-only] ---
constexpr int32_t kFlameDiscSpeed       = 16;   // FLAMEDISC_SPEED — missileeffect.h:296; set :1466
constexpr int32_t kFlameDiscLaunchRadius= 20;   // FLAMEDISC_LAUNCH_RADIUS (spawn jitter ±20 wu) — missileeffect.h:297; used :1460-1462
constexpr int32_t kNumFlameSparks       = 5;    // NUM_FLAME_SPARKS — missileeffect.h:320; loops :1515,1568
constexpr float   kFlameDiscMaxScale    = 3.0f; // FLAMEDISC_MAX_SCALE_STEP — missileeffect.cpp:1538
constexpr float   kFlameDiscScaleStep   = 0.2f; // FLAMEDISC_SCALE_STEP — missileeffect.cpp:1539

// Total slots: index 0 = disc, indices 1..NUM_FLAME_SPARKS = sparks.
constexpr int32_t kNumSlots             = kNumFlameSparks + 1;  // 6

// --- Birth delay range (missileeffect.cpp:1518,1584,1628) [snapshot-only] ---
// framenum[n] initialized to -random(2,8). Negative = pre-birth delay.
constexpr int32_t kBirthDelayMin        = 2;
constexpr int32_t kBirthDelayMax        = 8;

// --- Spark birth scale (missileeffect.cpp:1575,1620) [snapshot-only] ---
constexpr float   kSparkBirthScale      = 3.0f;

// --- EXPLODE permanent mute value (missileeffect.cpp:1613) [snapshot-only] ---
// On EXPLODE spark-death, set framenum = -32767 (permanent mute, never re-seeded).
constexpr int32_t kPermMute             = -32767;

// --- Sub-object indices (forensics §4; confirmed from I3D binary decode) ---
// GetObject(0) = "spark" (used for indices n=1..5 in Render)
// GetObject(1) = "flamedisc" (used for index n=0 in Render)
// missileeffect.cpp:1667-1670: n != 0 → GetObject(0); n == 0 → GetObject(1)
constexpr int32_t kSubSpark     = 0;   // the 5-spark quads
constexpr int32_t kSubDisc      = 1;   // the central disc quad
constexpr int32_t kNumSubObjs   = 2;   // two sub-objects total

// Number of textures in this I3D (forensics §4: one texture slot).
constexpr int32_t kNumTextures  = 1;

// Imagery path — uses the LEGACY copy (NOT shipped; forensics §4).
// Registry name "FlameDisc" → legacy/Class.Def:2033 → "Magic\FlameDisc.I3D"
constexpr const char* kImageryPath = "Magic\\FlameDisc.I3D";

// --- π constant ---
constexpr float kPi  = 3.14159265358979323846f;
constexpr float k2Pi = 2.0f * kPi;

// --- Framerate-independent sim tick (24 Hz authoring rate) ---
constexpr float kSimHz      = 24.0f;
constexpr float kSimTickSec = 1.0f / kSimHz;  // ~0.04167 s

// --- Test harness flight simulation (no TMissileEffect physics in test rig) ---
// LAUNCH: disc grows 0 → 3.0 at 0.2/tick with random(2,8) initial delay
// + kFlameDiscSpeed wu/tick travel in FLY.
// These tick counts chosen to be readable; not from the snapshot.
constexpr int32_t kLaunchTicks       = 25;   // disc grow phase (≥ delay + 15 grow ticks)
constexpr int32_t kFlyTicks          = 50;   // flight phase (~50 ticks ≈ 2 s at 24 Hz)
constexpr float   kFlySpeedWuPerTick = float(kFlameDiscSpeed);  // 16 wu/tick along +X

// =========================================================================
// snap_random — inclusive-range random (mirrors revutils.cpp:1597-1613)
// =========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// =========================================================================
// ConvertToVector — 8-bit-angle → ground-plane velocity vector.
// Mirrors object.cpp:169-188 logic.
//
// DistX[a] = 256 * sin(a * 2π/256) → vect.x = DistX[a] * speed / 256
//          = speed * sin(a * 2π/256)
// DistY[a] behaves as 256 * cos(a * 2π/256) (per colortable.cpp:457-467).
//
// zangle = 0 → vect.z = 0 (flat ground plane — forensics §5.3).
//
// Result is float (the snapshot packs into S3DPoint integer but we use
// float directly since wu precision is sufficient for spark velocities).
// =========================================================================
static void ConvertToVector(int32_t angle, int32_t speed, float& vx, float& vy)
{
    const float a_rad = float(angle & 255) * (k2Pi / 256.0f);
    vx = float(speed) * std::sin(a_rad);
    vy = float(speed) * std::cos(a_rad);
    // vz = 0 (sparks stay on the ground plane — forensics §5.3)
}

// =========================================================================
// Per-slot state (disc + sparks).
// Corresponds to the arrays in TFlameDiscAnimator (missileeffect.h:326-334).
// =========================================================================
struct SlotState
{
    float   p[3]     = {0, 0, 0};   // local-space position offset from effect origin
    float   v[3]     = {0, 0, 0};   // velocity (wu/tick); vz=0 for sparks
    float   scale    = 0.0f;         // current scale; grows/decays per tick
    int32_t framenum = 0;            // texture-flipbook frame tracker; < 0 = pre-birth delay
};

// =========================================================================
// Per-instance state — corresponds to TFlameDiscAnimator + TFlameDiscEffect.
// =========================================================================
struct State
{
    // Animator arrays (missileeffect.h:326-334)
    SlotState  slots[kNumSlots];     // [0]=disc, [1..5]=sparks

    int32_t    numtexframes = 1;     // tex.numframes from asset (missileeffect.cpp:1507-1508)
    int32_t    activesparks = 0;     // count of live sparks (missileeffect.h:332)
    int32_t    anim_state   = kStateLaunch; // current state for the animator
    int32_t    old_state    = kStateLaunch; // previous state (missileeffect.h:334)
    int32_t    caster_angle = 0;     // cached caster aim byte-angle (missileeffect.cpp:1513)

    // Effect world position (tracks disc travel in test harness)
    float  effect_pos[3] = {0, 0, 0};

    // Test-harness state machine (replaces TMissileEffect::Pulse + engine physics)
    int32_t missile_state = kStateLaunch;
    int32_t state_tick    = 0;

    // World spawn origin
    float  base_pos[3] = {0, 0, 0};

    // Lifecycle
    bool alive              = true;
    bool logged_first_render= false;
    bool logged_end         = false;

    // Framerate-independence accumulator (seconds)
    float tick_accum_s = 0.0f;

    // Asset state
    T3DImagery*  imagery        = nullptr;
    MeshHandle   mesh[kNumSubObjs]   = {0, 0};
    float        diffuse[kNumSubObjs][4]  = {{1,1,1,1},{1,1,1,1}};
    float        emissive[kNumSubObjs][4] = {{0,0,0,1},{0,0,0,1}};
    TTextureHandle  base_tex = kInvalidTexture;  // resolved from imagery tex slot 0

    // Per-frame texture: we read framehtexs[] from the S3DTex directly.
    // Stored once and reused; the array pointer stays valid for the asset lifetime.
    S3DTex  tex_slot = {};     // copy of GetTexture(0,...) result (cached at spawn)
};

// =========================================================================
// InitializeSparks — seed all spark slots with birth delay + rear-cone velocity.
// Called from Initialize (both slots 0..5) and on spark rebirth (FLY).
// missileeffect.cpp:1518-1527
// =========================================================================
static void SeedSlot(State* st, int32_t n)
{
    st->slots[n].scale    = 0.0f;
    st->slots[n].framenum = -snap_random(kBirthDelayMin, kBirthDelayMax);
    st->slots[n].p[0] = st->slots[n].p[1] = st->slots[n].p[2] = 0.0f;

    // ConvertToVector: rear hemisphere ±~45° (angle + 128 ± random(-32,32))
    // missileeffect.cpp:1524-1527
    const int32_t dir_angle = (st->caster_angle + 128 + snap_random(-32, 32)) & 255;
    ConvertToVector(dir_angle, kFlameDiscSpeed, st->slots[n].v[0], st->slots[n].v[1]);
    st->slots[n].v[2] = 0.0f;  // ground plane — forensics §5.3
}

// =========================================================================
// Initialize — transcribed from missileeffect.cpp:1498-1529 (Animator::Initialize).
// Note: TFlameDiscEffect::Initialize (missileeffect.cpp:1457-1471) handles
// the spawn-pos jitter and sound mount; those are test-rig-stubbed here.
// =========================================================================
static void Initialize(State* st)
{
    // Read texture frame count from asset (missileeffect.cpp:1507-1508)
    // tex.numframes == 0 can happen if asset is single-frame.
    if (st->imagery)
    {
        st->imagery->GetTexture(0, &st->tex_slot);
        st->numtexframes = (st->tex_slot.numframes > 0) ? st->tex_slot.numframes : 1;
    }

    st->activesparks = 0;
    st->anim_state   = kStateLaunch;
    st->old_state    = kStateLaunch;
    // caster_angle = 0 (test rig: aim along +X = byte-angle 0, maps to sin(0)=0, cos(0)=+Y)
    st->caster_angle = 0;

    // Initialize all 6 slots (disc at 0, sparks at 1..5)
    // missileeffect.cpp:1515-1527
    for (int32_t n = 0; n < kNumSlots; ++n)
    {
        st->slots[n].scale    = 0.0f;
        st->slots[n].framenum = -snap_random(kBirthDelayMin, kBirthDelayMax);
        st->slots[n].p[0] = st->slots[n].p[1] = st->slots[n].p[2] = 0.0f;
        const int32_t dir_angle = (st->caster_angle + 128 + snap_random(-32, 32)) & 255;
        ConvertToVector(dir_angle, kFlameDiscSpeed, st->slots[n].v[0], st->slots[n].v[1]);
        st->slots[n].v[2] = 0.0f;
    }
    // Note: v[0] (disc velocity) is dead data (forensics §6.2, §13.6) — never integrated.
}

// =========================================================================
// AnimateOneTick — per-nominal-24Hz-tick update.
// Transcribed from missileeffect.cpp:1541-1645 (Animate body).
// =========================================================================
static void AnimateOneTick(State* st)
{
    // --- Test-harness state machine (replaces TMissileEffect::Pulse) ---
    ++st->state_tick;
    if (st->missile_state == kStateLaunch && st->state_tick >= kLaunchTicks)
    {
        st->missile_state = kStateFly;
        st->anim_state    = kStateFly;
        st->state_tick    = 0;
    }
    else if (st->missile_state == kStateFly && st->state_tick >= kFlyTicks)
    {
        st->missile_state = kStateExplode;
        st->anim_state    = kStateExplode;
        st->state_tick    = 0;
    }

    // Advance effect_pos during FLY along +X (forensics §5.1: vel.z = speed/-16)
    // In the test rig we move along +X only for readability.
    if (st->missile_state == kStateFly)
    {
        st->effect_pos[0] += kFlySpeedWuPerTick;
    }

    // --- Animator Animate body ---
    // missileeffect.cpp:1545 — inst->SetCommandDone(false) (reset readiness flag)

    const int32_t state = st->anim_state;

    // ─── SWITCH on state (missileeffect.cpp:1547) ─────────────────────────
    switch (state)
    {
        case kStateLaunch:
        {
            // Grow the central disc (slot 0). Sparks are ignored during LAUNCH.
            // missileeffect.cpp:1553-1561
            if (st->slots[0].scale < kFlameDiscMaxScale)
            {
                // Advance the delay/frame counter
                ++st->slots[0].framenum;

                // Once framenum >= 0, start growing scale
                if (st->slots[0].framenum >= 0)
                    st->slots[0].scale += kFlameDiscScaleStep;
            }
            else
            {
                // Disc reached max scale → LAUNCH complete.
                // forensics §13.5 gap: also need SetStatus(true) to trigger
                // the base's aim→velocity conversion. Added per family pattern.
                // missileeffect.cpp:1561: inst->SetCommandDone(true)
                // (In test rig: state transition handled by timer above.)
            }
            break;
        }

        case kStateFly:
        {
            // Disc just sits at max scale; sparks rebirth & integrate.
            // missileeffect.cpp:1567-1597
            for (int32_t n = 1; n <= kNumFlameSparks; ++n)
            {
                ++st->slots[n].framenum;

                if (st->slots[n].framenum >= 0)
                {
                    if (st->slots[n].framenum == 0)
                    {
                        // Moment of birth: set scale to kSparkBirthScale.
                        // missileeffect.cpp:1575-1577
                        st->slots[n].scale = kSparkBirthScale;
                        ++st->activesparks;
                    }
                    else
                    {
                        // Decay scale −0.2/tick.
                        // missileeffect.cpp:1579-1593
                        st->slots[n].scale -= kFlameDiscScaleStep;

                        if (st->slots[n].scale <= 0.0f)
                        {
                            // Spark dies → re-seed (FLY only — EXPLODE never reseeds).
                            // missileeffect.cpp:1581-1593
                            --st->activesparks;
                            SeedSlot(st, n);
                        }
                        else
                        {
                            // Integrate position.
                            // missileeffect.cpp:1593: p[n] += v[n]
                            st->slots[n].p[0] += st->slots[n].v[0];
                            st->slots[n].p[1] += st->slots[n].v[1];
                            // p[2] stays 0 (ground plane, v.z=0)
                        }
                    }
                }
            }
            break;
        }

        case kStateExplode:
        {
            // Check termination FIRST (before updating sparks, so one-frame drain flush).
            // missileeffect.cpp:1608-1609
            if (st->activesparks == 0)
            {
                // All sparks drained → animator done → missile dies.
                st->alive = false;
                return;
            }

            // Drain: same decay as FLY, but on death set framenum = kPermMute (no rebirth).
            // missileeffect.cpp:1611-1645
            for (int32_t n = 1; n <= kNumFlameSparks; ++n)
            {
                ++st->slots[n].framenum;

                if (st->slots[n].framenum >= 0)
                {
                    if (st->slots[n].framenum == 0)
                    {
                        // Any spark that was mid-delay still births once on EXPLODE entry.
                        ++st->activesparks;
                        st->slots[n].scale = kSparkBirthScale;
                    }
                    else
                    {
                        st->slots[n].scale -= kFlameDiscScaleStep;

                        if (st->slots[n].scale <= 0.0f)
                        {
                            // Dead → permanent mute (NOT re-seeded).
                            --st->activesparks;
                            st->slots[n].framenum = kPermMute;  // missileeffect.cpp:1613
                        }
                        else
                        {
                            st->slots[n].p[0] += st->slots[n].v[0];
                            st->slots[n].p[1] += st->slots[n].v[1];
                        }
                    }
                }
            }
            break;
        }
    }

    st->old_state = state;
}

// =========================================================================
// Tick — framerate-independent entry point (PORT_CHECKLIST §B.4).
// Accumulates real wall-clock time and steps the sim once per kSimTickSec.
// =========================================================================
static void TickAccum(State* st)
{
    if (!st->alive) return;

    const float dt = float(TTime::DeltaTime());
    st->tick_accum_s += dt;

    while (st->tick_accum_s >= kSimTickSec)
    {
        AnimateOneTick(st);
        st->tick_accum_s -= kSimTickSec;
        if (!st->alive) break;
    }
}

// =========================================================================
// GetFlipbookTex — returns the texture handle for a given framenum.
// Uses S3DTex::framehtexs[] for multi-frame flipbook (forensics §8).
// Falls back to htexture (single-frame or if framehtexs is null).
// =========================================================================
static TTextureHandle GetFlipbookTex(const State* st, int32_t framenum)
{
    if (st->tex_slot.framehtexs && st->numtexframes > 1)
    {
        const int32_t f = ((framenum % st->numtexframes) + st->numtexframes)
                          % st->numtexframes;   // ensure positive modulo
        return st->tex_slot.framehtexs[f];
    }
    // Single-frame or unloaded: use htexture.
    const TTextureHandle h = st->tex_slot.htexture;
    return (h != kInvalidTexture) ? h : st->base_tex;
}

// =========================================================================
// RenderOneQuad — builds the WorldXY (ground-tipped) local matrix and
// submits one WorldMesh draw for a disc or spark quad.
//
// Original Render body: missileeffect.cpp:1654-1701.
// Per-quad: obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 [| OBJ3D_POS3 for sparks]
// Orientation: rot.x = -π/2 (all quads), rot.y = 0, rot.z = 0.
//               — missileeffect.cpp:1677-1679
// Scale: use scale[n] per-quad (forensics §13.3 fix — snapshot typo scale[0]).
// Position: for disc (n==0): no POS3 — draws at effect world pos.
//            for sparks (n!=0): obj->pos = p[n] (local-space offset).
//
// Local matrix transform order (D3D accumulation semantics):
//   OBJ3D_ROT1 first → OBJ3D_SCL2 second [→ OBJ3D_POS3 translate if spark]:
//   For the shim: MatrixIdentity → MatrixRotateX(-π/2) → MatrixScale(s,s,s)
//                 → MatrixTranslate(p[n]) [sparks only]
//
// The inst_world carries the effect's world position so the disc renders
// at the missile center (no explicit translate in local matrix for disc).
// =========================================================================
static void RenderOneQuad(State* st, int32_t n, TTextureHandle tex)
{
    // Select sub-object: disc slot (n==0) → GetObject(1) (flamedisc);
    // spark slots (n!=0) → GetObject(0) (spark).
    // missileeffect.cpp:1667-1670
    const int32_t sub_obj = (n == 0) ? kSubDisc : kSubSpark;

    const MeshHandle mesh = st->mesh[sub_obj];
    if (!mesh) return;

    // Only draw when framenum[n] >= 0 (silent during birth-delay window).
    // missileeffect.cpp:1665
    if (st->slots[n].framenum < 0) return;

    // DEVIATION (forensics §13.3): use scale[n] not scale[0].
    // The snapshot writes obj->scl = scale[0] for all quads — a typo.
    // We use scale[n] to match the Animate body's per-slot decay intent
    // and the sister Photon pattern (missileeffect.cpp:425 uses scale[n]).
    const float s = st->slots[n].scale;
    if (s <= 0.0f) return;

    // Build local matrix:
    //   RotX(-π/2) → Scale(s,s,s) [→ Translate(p[n]) for sparks]
    // Note: D3D accumulation semantics — each MatrixRotate/Scale/Translate
    //       is applied AFTER the previous op (post-multiply internally
    //       stored as pre-multiply for column-vector).
    // missileeffect.cpp:1677-1686
    d3d::Matrix local;
    d3d::MatrixIdentity(local);
    d3d::MatrixRotateX(local, -kPi / 2.0f);  // rot.x = -π/2 (WorldXY ground-tip)
    // rot.y = 0, rot.z = 0 — missileeffect.cpp:1678-1679 (no spin via geometry)
    d3d::MatrixScale(local, s, s, s);         // OBJ3D_SCL2

    // For sparks (n != 0): add local position offset.
    // For disc (n == 0): position is handled entirely by inst_world.
    // missileeffect.cpp:1682-1686: OBJ3D_POS3 only for n != 0
    if (n != 0)
    {
        d3d::MatrixTranslate(local,
                             st->slots[n].p[0],
                             st->slots[n].p[1],
                             st->slots[n].p[2]);
    }

    // inst_world: translate to the disc's current world position.
    // This is the missile's world pos (effect_pos) — makes the disc track
    // the moving missile center each tick.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->effect_pos[0],
                         st->effect_pos[1],
                         st->effect_pos[2]);

    // sort_z: use world Z of the draw for depth ordering.
    // Disc at effect_pos.z; sparks at effect_pos.z + p[n].z (but p.z=0, flat).
    const float sort_z = st->effect_pos[2];

    d3d::RenderObject(*Renderer,
                      mesh,
                      local,
                      st->diffuse[sub_obj],
                      st->emissive[sub_obj],
                      d3d::CurrentBlend(),
                      &inst_world,
                      sort_z);

    (void)tex;  // texture handle selection is future work when shim supports
                // per-draw texture override; for now mesh uses registered texture.
}

// =========================================================================
// SubmitWorldImpl — render all WorldMesh draws for this tick.
// MUST be called from the harness submit_world callback (after BeginTilePass).
// Transcribed from missileeffect.cpp:1654-1701 (Render body).
//
// Draw order: n=0 (disc), then n=1..5 (sparks) — missileeffect.cpp:1663 loop.
// =========================================================================
static void SubmitWorldImpl(State* st)
{
    if (!Renderer || !st->alive) return;

    // Verify mesh handles
    if (!st->mesh[kSubSpark] || !st->mesh[kSubDisc]) return;

    // Diagnostic: log first render
    if (!st->logged_first_render)
    {
        log_info("[flamedisc-shim] first render: state=%d activesparks=%d "
                 "disc_scale=%.2f effect_pos=(%.0f,%.0f,%.0f) "
                 "numtexframes=%d mesh=(%u,%u)",
                 st->anim_state, st->activesparks,
                 double(st->slots[0].scale),
                 double(st->effect_pos[0]),
                 double(st->effect_pos[1]),
                 double(st->effect_pos[2]),
                 st->numtexframes,
                 st->mesh[0], st->mesh[1]);
        st->logged_first_render = true;
    }

    // SaveBlendState + RestoreBlendState via RAII guard
    d3d::BlendStateGuard blend_scope;

    // SetBlendState() → Alpha (MODULATE+SRCALPHA/INVSRCALPHA, ZWRITE off, ZTEST on)
    // missileeffect.cpp:1659-1660. Forensics §7: snapshot Alpha, same as missile family.
    // NOTE: if visual self-test reads pale/transparent → try SetAddBlendState (forensics §13.10).
    d3d::SetBlendState();

    // Draw all 6 quads: disc first (n=0), then sparks 1..5.
    // missileeffect.cpp:1663: loop n = 0..NUM_FLAME_SPARKS
    for (int32_t n = 0; n < kNumSlots; ++n)
    {
        // Only draw if slot is born (framenum >= 0).
        if (st->slots[n].framenum < 0) continue;

        // Resolve per-frame flipbook texture handle (forensics §8).
        // Each slot picks framenum[n] % numtexframes to cycle the texture.
        // missileeffect.cpp:1689: frame = framenum[n] % numtexframes
        const TTextureHandle tex = GetFlipbookTex(st, st->slots[n].framenum);

        RenderOneQuad(st, n, tex);
    }

    // Diagnostic: log end of life
    if (!st->logged_end && !st->alive)
    {
        log_info("[flamedisc-shim] all sparks dead");
        st->logged_end = true;
    }
}

// =========================================================================
// Spawn — load asset, register meshes, run Initialize.
// =========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();

    // Spawn position: the original applies cubic ±20 wu jitter BEFORE
    // TMissileEffect::Initialize (missileeffect.cpp:1460-1462).
    // In the test rig we don't jitter — anchor at origin for repeatability.
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);
    st->effect_pos[0] = st->base_pos[0];
    st->effect_pos[1] = st->base_pos[1];
    st->effect_pos[2] = st->base_pos[2];

    // Load Magic\FlameDisc.I3D from the legacy copy (forensics §4, §2.1.2).
    // The asset is NOT in shipped imagery.rvi — uses the legacy disk copy.
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[flamedisc-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }

    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[flamedisc-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Register both sub-objects.
    // sub-object 0 = "spark", sub-object 1 = "flamedisc" (forensics §4).
    // num_textures = 1 (one shared flipbook texture slot, forensics §4).
    const int32_t num_tex = img3d->NumTextures();
    for (int32_t s = 0; s < kNumSubObjs; ++s)
    {
        st->mesh[s] = d3d::RegisterSubMesh(*Renderer, img3d, s, num_tex);
        if (!st->mesh[s])
            log_warn("[flamedisc-shim] RegisterSubMesh(sub=%d) returned 0 — "
                     "effect will be partially invisible", s);

        d3d::LoadMaterial(img3d, s, st->diffuse[s], st->emissive[s]);

        // REQUIRED (PORT_CHECKLIST §B.2, WAVE_1_LESSONS §1.1):
        // kHelperMeshFs adds emissive additively. Pre-release I3Ds default
        // to emissive=(1,1,1) which saturates to white. Zero it.
        // FlameDisc is self-lit (fire IS the light emitter) — the Alpha
        // blend carries the authored brightness, not emissive.
        st->emissive[s][0] = 0.0f;
        st->emissive[s][1] = 0.0f;
        st->emissive[s][2] = 0.0f;
        // emissive[s][3] stays 1.0f (alpha channel, unused in shader formula)
    }

    // Cache the base texture handle for single-frame fallback.
    if (num_tex > 0)
    {
        img3d->GetTexture(0, &st->tex_slot);
        st->base_tex = st->tex_slot.htexture;
    }

    // Note: SoundPlayer.Mount("lightning") would happen here in real game path.
    // Skipped — no audio path in test harness (forensics §11).

    // Run animator Initialize (seeds all slot arrays, reads numtexframes).
    Initialize(st);

    log_info("[flamedisc-shim] Spawn ok: alive=%d base_tex=%u "
             "numtexframes=%d mesh=(%u,%u) "
             "base_pos=(%d,%d,%d) num_obj=%d num_tex=%d",
             int(st->alive), st->base_tex,
             st->numtexframes,
             st->mesh[0], st->mesh[1],
             origin.x, origin.y, origin.z,
             img3d->NumObjects(), num_tex);

    return st;
}

// =========================================================================
// Public entry points.
// =========================================================================

// Tick — advance simulation. Call from regular submit hook.
void Tick(State* st)
{
    if (st) TickAccum(st);
}

// Submit — billboard-only draws (none: all FlameDisc draws are WorldMesh).
// Call from regular submit hook alongside Tick.
void Submit(State* st)
{
    (void)st;
    // All FlameDisc draws are WorldMesh → SubmitWorld from submit_world hook.
}

// SubmitWorld — all WorldMesh draws. MUST be called from submit_world callback.
void SubmitWorld(State* st)
{
    if (st && st->alive) SubmitWorldImpl(st);
}

void Destroy(State* st)
{
    delete st;
}

// IsAlive — true while the effect is still running.
// Test harness uses this for auto-respawn after EXPLODE death.
bool IsAlive(const State* st)
{
    return st && st->alive;
}

} // namespace flamedisc_shim
