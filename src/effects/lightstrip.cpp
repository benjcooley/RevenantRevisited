// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  lightstrip.cpp - LightStrip (Lightning spell) port via fx pipeline   *
// *                                                                       *
// *  RETAIL-ASSET PORT (S01/S04 TStripEffect + TLightningAnimator).       *
// *  Forensics source: docs/vfx/forensics/LIGHTNING_TLightningAnimator.md *
// *  Snapshot source:  src/stripeffect.cpp:381-1420 (#if 0 D3D body)      *
// *                                                                       *
// *  Retail fidelity: retail-partial — VISUALS retail-asset-driven,       *
// *  TIMING snapshot-only. The forensics §2.1 finding is decisive here:   *
// *  the shipped Magic\NewLightStrip.I3D (50,643 B) was REWORKED from     *
// *  the snapshot dev asset (18,680 B). The snapshot code procedurally    *
// *  extrudes a smooth ribbon (TStripAnimator) textured with the dev      *
// *  "oldlightstrip" beam gradient — that implementation never shipped.   *
// *  The retail asset instead carries AUTHORED ART:                       *
// *    texture[1] 128x128 — EIGHT pre-drawn jagged purple bolt columns    *
// *    texture[0] 64x64   — flare sheet (4-point star / ball / ellipse)   *
// *    object[0]     '#flare'        30x30 quad, UV = full-w x half-h     *
// *                  band of texture[0] (the purple lens ball)            *
// *    object[1..10] 'rectangle02..11' 15 x ~90 wu quads, one bolt        *
// *                  column each (rect09/10/11 wrap to the 8th column)    *
// *    object[11]    '#sparks'       20x20 quad, quarter-sheet star       *
// *    object[12]    'csparks'       empty container node                 *
// *  Per i3d_dump_all/newlightstrip/{manifest.txt,newlightstrip.obj}.     *
// *                                                                       *
// *  Reference identity: images/vfx/05_LightStrip/01..05.png — confirmed  *
// *  2026-06-09 by exclusivity sweep of every shipped electric-family     *
// *  asset (energyspray/espray/yenergy/oldlightstrip/lightningbow):       *
// *  ONLY newlightstrip carries jagged purple bolt art. Reference shows   *
// *  a purple ball at the cast hand with 3-6 jagged violet strands        *
// *  radiating simultaneously + one dominant strand to the target.        *
// *                                                                       *
// *  Render design (asset-driven; snapshot supplies timing/state only):   *
// *  - Bolt strands  = ScreenAligned PARTICLE quads (SubmitFxParticle).   *
// *    Each strand picks one authored rectangleNN column per tick (the    *
// *    retail crackle = swapping which pre-drawn bolt draws — ten         *
// *    variants exist precisely for this). Strand rotation_rad is the     *
// *    iso-projected screen angle of its world direction; the iso         *
// *    projection (S = wx-wy, T = 0.5(wx+wy) - 0.867 wz — fx.metal.h     *
// *    billboard VS) is linear in world coords, so anchoring the quad     *
// *    at the world midpoint of the strand span projects exactly.        *
// *  - Purple ball   = '#flare' quad drawn TWICE counter-rotating         *
// *    (snapshot two-half glow: rotdegree +12, morrotdegree +16 per tick, *
// *    stripeffect.cpp:814-815,861-902) + once smaller at the impact end. *
// *  - Sparks        = two 20-slot TSubParticleAnimator pools (cast hand  *
// *    + impact), constants per forensics §3, sprite = '#sparks' star.    *
// *  - Blend AdditiveStraight everywhere (self-lit; black-keyed authored  *
// *    art; sister TWindStripAnimator additive halo, stripeffect.cpp:1333)*
// *  - Depth TestNoWrite. No DoLighting (self-lit per WAVE_1 §1.2a).      *
// *                                                                       *
// *  State machine (snapshot Pulse, stripeffect.cpp:406-457):             *
// *    LAUNCH (1 tick) -> FLY (20 ticks) -> EXPLODE (drain) -> dead.      *
// *  24 Hz sim accumulator per WAVE_1_LESSONS §1.4.                       *
// *                                                                       *
// *  Test rig anchor: origin + z+50 (hand height, stripeffect.cpp:402),   *
// *  random yaw + range per spawn (no caster/raycast in the rig — the     *
// *  retail SetupObjects ground-walk needs MapPane::GetWalkHeight).       *
// *                                                                       *
// *  REGISTRATION block: see .vfx_logs/lightstrip.md.                     *
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../3dimage.h"
#include "../imagery.h"
#include "../logging.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace lightstrip_shim {

// ==========================================================================
// Constants — snapshot-only unless marked otherwise (forensics §3).
// ==========================================================================

constexpr const char* kImageryPath = "Magic\\NewLightStrip.I3D";  // Class.Def:2034 (retail-confirmed)

constexpr int32_t kSimTickMs        = 1000 / 24;  // 24 Hz animator cadence

// STAGING — USER-DIRECTED HYBRID (final, 2026-06-09): the retail video
// shows missile staging (ball scales at hand, travels, bursts at the
// target, NO connector). The user chose to keep the connector as a
// deliberate non-canon improvement. Final staging: the full ball
// composite (ball + crackle fan + riding sparks) forms AT THE HAND,
// TRAVELS to the target at the retail bolt speed, stays CONNECTED to
// the source by the jagged strand, and the source hand plays a purple
// sparkle emitter throughout. Do not "fix" toward either pure variant
// without asking.
constexpr float   kTravelWuPerTick  = 16.0f;      // STRIP_SPEED (stripeffect.h:47)
constexpr int32_t kLaunchTicks      = 1;          // LAUNCH -> FLY next tick
// Crackle variant swap cadence: every 2nd sim tick (12 Hz). Per-tick
// (24 Hz) reads as frantic vs the reference video.
constexpr int32_t kCrackleTickDiv   = 2;

// Source sparkle tint: the hand emitter reads PURPLE in retail (user
// review 2026-06-09); the white star art is tinted violet to match the
// flare/bolt palette. Impact sparks riding the ball stay white (they
// sit inside the violet ball glow).
constexpr float kSourceSparkTint[3] = { 0.65f, 0.40f, 1.0f };

// Impact burst (user review 2026-06-09): on arrival the glow AND the
// crackle strands EXPAND while fading out, plus a big dense star-
// particle burst. Fade rate = snapshot glow decay x0.8/tick
// (stripeffect.cpp:700); expansion + burst density reference-tuned.
constexpr float   kGlowExplodeDecay = 0.8f;       // fade per tick
constexpr float   kBurstExpand      = 1.15f;      // glow+fan scale per tick
// Hold the central glow at full brightness for the first burst ticks so
// the explosion reads BEFORE the fade begins (user review 2026-06-09:
// "it fades out before it explodes").
constexpr int32_t kBurstHoldTicks   = 4;          // ~0.17 s at 24 Hz
// The WHITE CORE outlasts the outer halo + bolts (user reviews
// 2026-06-09): a third flare pass PINNED at ~arrival-ball size (it does
// NOT ride the burst expansion) on its own slower, later fade track —
// fills the otherwise-empty center while the halo expands away.
constexpr float   kCoreScale        = 0.8f;       // of the arrival ball size, pinned
constexpr float   kCoreFadeDecay    = 0.90f;      // slower than 0.8 outer
constexpr int32_t kCoreExtraHold    = 3;          // ticks beyond kBurstHoldTicks
constexpr int32_t kBurstSparkCount  = 48;         // burst pool ("reasonably dense")
constexpr int32_t kBurstVelZ        = 2;          // burst vertical spread

// Snapshot strip width taper: SetWidth(32, 20) (stripeffect.cpp:642) —
// 32 wu at the source (hilt) tapering to 20 wu at the tip. Applied to
// the connector strand as a hilt:tip ratio on the uniform-scale width.
constexpr float kHiltTipTaper       = 20.0f / 32.0f;
constexpr float kGlowDrawThreshold  = 0.005f;     // stripeffect.cpp:861

constexpr float kGlowScaleInit      = 3.4f;       // stripeffect.cpp:530
constexpr float kRotDegPerTick      = 12.0f;      // stripeffect.cpp:814
constexpr float kMorRotDegPerTick   = 16.0f;      // stripeffect.cpp:815

// Ambient strand fan: reference frames 04/05 show 3-5 simultaneous
// strands besides the main bolt. Count/cycle are reference-tuned (no
// retail body to cite — forensics §13).
constexpr int32_t kAmbientStrands   = 3;
constexpr int32_t kAmbientLifeMin   = 2;          // ticks before re-roll
constexpr int32_t kAmbientLifeMax   = 5;
constexpr float   kAmbientReachMin  = 0.25f;      // fraction of main range
constexpr float   kAmbientReachMax  = 0.55f;
// Fan strand roots are pulled INWARD past the ball center by this
// fraction of their reach, so during the burst expansion the strokes
// stay planted under the glow and just lengthen — instead of their
// near ends visibly sliding outward with the scale (user review
// 2026-06-09).
constexpr float   kFanRootInset     = 0.3f;
// NOTE (in-game integration): the reference video shows the purple
// target-end MOVING during the effect. The retail mechanism is the ball
// riding the STRUCK CHARACTER as it staggers (end_p = target character
// anchor) — a combat property, not effect code. The rig has no target
// character, so the ball stays put here; wire end_p to the target
// character per tick at integration.
//
// Strand widths: connector visibly wider than the fan arcs in the
// reference — produced naturally by uniform per-quad scaling (width
// proportional to strand length, as retail's single matrix scale per
// rectangleNN quad would give). No separate width tuning.

// Spark sub-emitters (forensics §3, stripeffect.cpp:503-527).
constexpr int32_t kMaxSparks        = 20;         // SUBPARTICLE_MAX_PARTICLE
constexpr int32_t kSparkChancePct   = 25;         // spark.chance
constexpr float   kSparkGravity     = 0.18f;      // spark.gravity wu/tick^2
constexpr float   kSparkScaleInit   = 2.0f;       // spark.scale
constexpr float   kSparkScaleDec    = 0.90f;      // spark.scale_dec
constexpr int32_t kSparkLifeMin     = 25;         // spark.min_life ticks
constexpr int32_t kSparkLifeMax     = 35;         // spark.max_life
constexpr float   kSparkFlickerMul  = 2.5f;       // spark.flicker_size
constexpr float   kSparkVelXY       = 2.0f;       // velocity_spread.x/y
constexpr float   kSparkVelZ        = 1.0f;       // velocity_spread.z
// Per-asset correction (NOT a snapshot constant): the snapshot's
// scale=2.0 was tuned for the dev asset's spark sub-object; on the
// reworked retail '#sparks' 20 wu quad it reads as ~40-100 wu white
// stars that blow out the composite center (user foreground review
// 2026-06-09). Same correction class as the iter9 flare-scale finding.
constexpr float   kSparkQuadCorrection = 0.5f;

// Test-rig spawn parameters (rig-only; retail raycasts the real range).
// Staging per user ground-truth 2026-06-09: this is a TARGET/SOURCE
// effect — the purple ball + radiating fan + impact sparks live at the
// TARGET; one strand stretches BACK to the caster's hand. The harness
// origin (SpellGround, centered) is the TARGET; the caster end is
// offset outward by `range`. Range capped so the hand stays in frame
// (visible area ~±240 wu).
constexpr float kHandZLift          = 50.0f;      // stripeffect.cpp:402
constexpr float kTestRangeMin       = 140.0f;     // wu — rig framing only
constexpr float kTestRangeMax       = 220.0f;

// ==========================================================================
// Inclusive-range random — snapshot random(lo,hi) (revutils.cpp:1597-1613).
// ==========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

static float frand01()
{
    return float(std::rand()) / float(RAND_MAX);
}

// ==========================================================================
// ResolveSubObjTextureSlot — texfaces[] walk, 1-based slot -> 0-based
// texture index. Mirrors icebolt.cpp:195 / fireball.cpp:226.
// ==========================================================================
static int32_t ResolveSubObjTextureSlot(T3DImagery* img3d, int32_t sub_obj_num)
{
    if (!img3d || sub_obj_num < 0 || sub_obj_num >= img3d->NumObjects())
        return -1;
    const int32_t nfaces = img3d->NumObjFaces(sub_obj_num);
    if (nfaces <= 0) return -1;
    std::vector<S3DFace> face_buf(static_cast<size_t>(nfaces));
    int32_t texfaces[9]    = {};
    int32_t numtexfaces[9] = {};
    img3d->GetObjFaces(sub_obj_num, face_buf.data(), texfaces, numtexfaces);
    for (int32_t s = 1; s <= 8; ++s)
        if (numtexfaces[s] > 0)
            return s - 1;
    return -1;
}

// ==========================================================================
// Per-sub-object sprite description read from the authored asset: UV rect
// + local quad extents + resolved texture handle. (UI: "the asset is the
// spec" — UV bands and quad sizes come from the I3D, never hardcoded.)
// ==========================================================================
struct SSprite
{
    TTextureHandle texture   = kInvalidTexture;
    float uv_rect[4]         = {0.0f, 0.0f, 1.0f, 1.0f};  // x,y,w,h
    float width_wu           = 0.0f;   // authored local extent
    float height_wu          = 0.0f;
    bool  valid              = false;
};

// Read authored UV min/max + local bbox extents for one sub-object.
static SSprite ReadSprite(T3DImagery* img3d, int32_t obj, int32_t num_tex)
{
    SSprite s;
    const int32_t nv = img3d->NumObjVerts(obj);
    if (nv <= 0) return s;
    std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
    img3d->GetObjVerts(obj, vbuf.data(), 0, 0, ERender3DVertex::Vertex);

    float minu = vbuf[0].tu,    maxu = vbuf[0].tu;
    float minv = vbuf[0].tv,    maxv = vbuf[0].tv;
    float minx = vbuf[0].pos.X, maxx = vbuf[0].pos.X;
    float miny = vbuf[0].pos.Y, maxy = vbuf[0].pos.Y;
    float minz = vbuf[0].pos.Z, maxz = vbuf[0].pos.Z;
    for (int32_t i = 1; i < nv; ++i)
    {
        minu = (std::min)(minu, vbuf[i].tu);    maxu = (std::max)(maxu, vbuf[i].tu);
        minv = (std::min)(minv, vbuf[i].tv);    maxv = (std::max)(maxv, vbuf[i].tv);
        minx = (std::min)(minx, vbuf[i].pos.X); maxx = (std::max)(maxx, vbuf[i].pos.X);
        miny = (std::min)(miny, vbuf[i].pos.Y); maxy = (std::max)(maxy, vbuf[i].pos.Y);
        minz = (std::min)(minz, vbuf[i].pos.Z); maxz = (std::max)(maxz, vbuf[i].pos.Z);
    }
    // rect09/10/11 carry a wrapped negative-u band ([-0.1315..0.0049] =
    // the 8th column); normalize into [0,1) so uv_rect addresses the
    // same texels the wrap did.
    if (minu < 0.0f) { minu += 1.0f; maxu += 1.0f; }
    s.uv_rect[0] = minu;
    s.uv_rect[1] = minv;
    s.uv_rect[2] = maxu - minu;
    s.uv_rect[3] = maxv - minv;

    // Quad extents: bolts + flare are XY-plane quads (z=0); #sparks is an
    // XZ-plane quad (y=0). Take the two largest spans.
    const float ex = maxx - minx, ey = maxy - miny, ez = maxz - minz;
    if (ez > ey) { s.width_wu = ex; s.height_wu = ez; }   // XZ quad (#sparks)
    else         { s.width_wu = ex; s.height_wu = ey; }   // XY quad

    const int32_t slot = ResolveSubObjTextureSlot(img3d, obj);
    const int32_t tidx = (slot >= 0 && slot < num_tex) ? slot : 0;
    S3DTex tex = {};
    img3d->GetTexture(tidx, &tex);
    s.texture = tex.htexture;
    s.valid   = (s.texture != kInvalidTexture) &&
                (s.uv_rect[2] > 0.0f) && (s.uv_rect[3] > 0.0f) &&
                (s.width_wu > 0.01f) && (s.height_wu > 0.01f);
    return s;
}

// ==========================================================================
// State
// ==========================================================================

enum EPhase : int32_t { kLaunch = 0, kFly = 1, kExplode = 2 };

struct SSparkParticle
{
    float   pos[3]  = {0.0f, 0.0f, 0.0f};
    float   vel[3]  = {0.0f, 0.0f, 0.0f};
    float   scale   = 0.0f;
    int32_t life    = 0;
    bool    flicker = false;
    bool    alive   = false;
};

struct SAmbientStrand
{
    float   yaw       = 0.0f;   // world XY heading (rad)
    float   reach     = 0.3f;   // fraction of main range
    int32_t life      = 0;      // ticks until yaw/reach re-roll
    int32_t variant   = 0;      // rectangleNN pick (re-rolled every tick)
};

struct State
{
    T3DImagery* imagery = nullptr;

    // Authored sprites (read from the retail asset at Spawn).
    SSprite flare;                       // '#flare'  — purple lens ball
    SSprite spark;                       // '#sparks' — 4-point star
    std::vector<SSprite> bolts;          // 'rectangleNN' — jagged bolt columns

    // World anchors. TARGET/SOURCE staging: target = harness origin
    // (centered); cast_pos = target - dir*range (the caster's hand).
    float target_pos[3] = {0.0f, 0.0f, 0.0f};   // ball + fan + impact sparks
    float cast_pos[3]   = {0.0f, 0.0f, 0.0f};   // hand point (z+50)
    float dir[3]        = {1.0f, 0.0f, 0.0f};   // unit hand->target heading
    float range         = 200.0f;                // wu hand->target

    // Sim.
    double  accum_ms    = 0.0;
    EPhase  phase       = kLaunch;
    int32_t phase_ticks = 0;
    int32_t total_ticks = 0;
    float   traveled    = 0.0f;   // wu along hand->target
    float   ball_pos[3] = {0.0f, 0.0f, 0.0f};   // live traveling-ball anchor
    float   fade        = 1.0f;   // EXPLODE color fade (halo + bolts)
    float   core_fade   = 1.0f;   // white core's slower fade track
    float   glow_scale  = kGlowScaleInit;       // full from spawn
    float   rotdeg      = 0.0f;
    float   morrotdeg   = 0.0f;
    int32_t main_variant = 0;     // connector strand rectangleNN pick

    SAmbientStrand ambient[kAmbientStrands] = {};

    // Spark pools: source emitter (purple, at the hand, 20-cap trickle)
    // + impact burst (white, 48 stars popped at once on arrival).
    SSparkParticle cast_sparks[kMaxSparks]         = {};
    SSparkParticle impact_sparks[kBurstSparkCount] = {};

    bool alive        = false;
    bool spawn_logged = false;
};

// ==========================================================================
// Spark emitter tick — TSubParticleAnimator::Animate + Create
// (effectcomp.cpp:361-402 / :452-524; constants forensics §3).
// ==========================================================================
static int32_t CountLiveSparks(const SSparkParticle* arr, int32_t count)
{
    int32_t n = 0;
    for (int32_t i = 0; i < count; ++i)
        if (arr[i].alive) ++n;
    return n;
}

static void TickSparkEmitter(SSparkParticle* arr, int32_t count,
                             int32_t desired, const float emit_pos[3])
{
    const int32_t live = CountLiveSparks(arr, count);
    for (int32_t c = live; c < desired; ++c)
    {
        if (snap_random(1, 100) > kSparkChancePct)
            continue;                      // 25% trickle (effectcomp.cpp:458)
        for (int32_t i = 0; i < count; ++i)
        {
            if (arr[i].alive) continue;
            SSparkParticle& p = arr[i];
            p.alive  = true;
            p.pos[0] = emit_pos[0];        // pos_spread = (0,0,0)
            p.pos[1] = emit_pos[1];
            p.pos[2] = emit_pos[2];
            p.vel[0] = float(snap_random(-2, 2));   // ±velocity_spread.xy
            p.vel[1] = float(snap_random(-2, 2));
            p.vel[2] = float(snap_random(-1, 1));   // ±velocity_spread.z
            p.scale  = kSparkScaleInit;
            p.life   = snap_random(kSparkLifeMin, kSparkLifeMax);
            p.flicker = (snap_random(0, 1) != 0);
            break;
        }
    }
    for (int32_t i = 0; i < count; ++i)
    {
        SSparkParticle& p = arr[i];
        if (!p.alive) continue;
        p.flicker = (snap_random(0, 1) != 0);       // re-rolled every tick
        if (--p.life < 0) { p.alive = false; continue; }
        p.scale  *= kSparkScaleDec;
        p.pos[0] += p.vel[0];
        p.pos[1] += p.vel[1];
        p.pos[2] += p.vel[2];
        p.vel[2] -= kSparkGravity;
    }
}

// Impact BURST — an ELECTRICAL EXPLOSION (user review 2026-06-09): the
// whole pool pops at once with fast RADIAL outward velocities and
// short bright flickering lives. Distinct from the emitter trickle's
// floaty box-random spread; all rates reference-tuned (no retail body).
static void SpawnSparkBurst(SSparkParticle* arr, int32_t count,
                            const float emit_pos[3])
{
    for (int32_t i = 0; i < count; ++i)
    {
        SSparkParticle& p = arr[i];
        const float yaw   = frand01() * 6.2831853f;
        const float speed = 4.0f + frand01() * 5.0f;          // 4..9 wu/tick
        p.alive  = true;
        p.pos[0] = emit_pos[0];
        p.pos[1] = emit_pos[1];
        p.pos[2] = emit_pos[2];
        p.vel[0] = std::cos(yaw) * speed;                     // radial pop
        p.vel[1] = std::sin(yaw) * speed;
        p.vel[2] = float(snap_random(-kBurstVelZ, kBurstVelZ));
        p.scale  = kSparkScaleInit;
        p.life   = snap_random(10, 18);                       // short + bright
        p.flicker = (snap_random(0, 1) != 0);
    }
}

// ==========================================================================
// 24 Hz sim tick
// ==========================================================================
static void SimTick(State* st)
{
    // --- State machine: LAUNCH (ball forms at hand) -> FLY (ball
    // travels at STRIP_SPEED=16 wu/tick, stripeffect.h:47, connector
    // stretching back to the hand) -> EXPLODE on arrival (fade in
    // place). Hybrid staging per the file banner.
    ++st->total_ticks;
    ++st->phase_ticks;
    switch (st->phase)
    {
        case kLaunch:
            if (st->phase_ticks >= kLaunchTicks)
            {
                st->phase = kFly;
                st->phase_ticks = 0;
            }
            break;
        case kFly:
            st->traveled += kTravelWuPerTick;
            if (st->traveled >= st->range)
            {
                st->traveled    = st->range;
                st->phase       = kExplode;
                st->phase_ticks = 0;
                // Impact BURST: the star sparkles pop outward at the
                // target all at once (electrical explosion).
                SpawnSparkBurst(st->impact_sparks, kBurstSparkCount,
                                st->target_pos);
            }
            break;
        case kExplode:
            // Arrival: the ball EXPANDS immediately; the fade is held
            // back kBurstHoldTicks so the explosion reads at full
            // brightness first. The white core fades later and slower.
            // Burst sparks drain on their own lives.
            st->glow_scale *= kBurstExpand;
            if (st->phase_ticks > kBurstHoldTicks)
                st->fade *= kGlowExplodeDecay;
            if (st->phase_ticks > kBurstHoldTicks + kCoreExtraHold)
                st->core_fade *= kCoreFadeDecay;
            break;
    }

    // --- Live ball anchor: rides hand -> target. NOTE (in-game
    // integration): on arrival the ball should anchor to the struck
    // character; the rig has no target character.
    const float t = st->traveled / (std::max)(st->range, 1.0f);
    st->ball_pos[0] = st->cast_pos[0] + (st->target_pos[0] - st->cast_pos[0]) * t;
    st->ball_pos[1] = st->cast_pos[1] + (st->target_pos[1] - st->cast_pos[1]) * t;
    st->ball_pos[2] = st->cast_pos[2] + (st->target_pos[2] - st->cast_pos[2]) * t;

    // --- Crackle: re-pick the authored bolt variant on the 12 Hz
    // sub-beat (kCrackleTickDiv); heading re-rolls every few ticks.
    // Ten authored variants exist for exactly this swap.
    const int32_t nvar = int32_t(st->bolts.size());
    if (nvar > 0 && (st->total_ticks % kCrackleTickDiv) == 0)
    {
        st->main_variant = snap_random(0, nvar - 1);
        for (auto& a : st->ambient)
            a.variant = snap_random(0, nvar - 1);
    }
    for (auto& a : st->ambient)
    {
        if (--a.life <= 0)
        {
            a.yaw   = frand01() * 6.2831853f;
            a.reach = kAmbientReachMin +
                      frand01() * (kAmbientReachMax - kAmbientReachMin);
            a.life  = snap_random(kAmbientLifeMin, kAmbientLifeMax);
        }
    }

    // --- Glow rotation (stripeffect.cpp:814-815)
    st->rotdeg    = std::fmod(st->rotdeg    + kRotDegPerTick,    360.0f);
    st->morrotdeg = std::fmod(st->morrotdeg + kMorRotDegPerTick, 360.0f);

    // --- Spark pools: the purple source emitter trickles at the hand
    // through LAUNCH/FLY (params forensics §6.2/§6.4) and drains in
    // EXPLODE; the impact pool only integrates here — it is filled by
    // SpawnSparkBurst on the arrival tick.
    const int32_t cast_desired = (st->phase == kExplode) ? 0 : kMaxSparks;
    TickSparkEmitter(st->cast_sparks,   kMaxSparks,       cast_desired, st->cast_pos);
    TickSparkEmitter(st->impact_sparks, kBurstSparkCount, 0,            st->ball_pos);

    // --- Self-destruct (stripeffect.cpp:818-819: EXPLODE + both spark
    // pools drained; fade floor added so the burst always finishes its
    // visual collapse).
    if (st->phase == kExplode &&
        st->fade < 0.05f &&
        st->core_fade < 0.05f &&
        CountLiveSparks(st->cast_sparks,   kMaxSparks)       == 0 &&
        CountLiveSparks(st->impact_sparks, kBurstSparkCount) == 0)
    {
        st->alive = false;
    }
}

// ==========================================================================
// Render helpers — all AdditiveStraight ScreenAligned (self-lit composite).
// ==========================================================================

static void SubmitSpriteQuadTinted(const SSprite& spr,
                                   const float world_pos[3],
                                   float size_w_wu, float size_h_wu,
                                   float rotation_rad,
                                   const float tint[3],
                                   float rgb_fade,
                                   EFxDebugMode debug_mode)
{
    SParticleDrawItem item = {};
    item.world_pos[0] = world_pos[0];
    item.world_pos[1] = world_pos[1];
    item.world_pos[2] = world_pos[2];
    item.size_wu[0]   = size_w_wu;
    item.size_wu[1]   = size_h_wu;
    item.color_rgba[0] = tint[0] * rgb_fade;   // additive: fade via rgb
    item.color_rgba[1] = tint[1] * rgb_fade;
    item.color_rgba[2] = tint[2] * rgb_fade;
    item.color_rgba[3] = 1.0f;
    std::memcpy(item.uv_rect, spr.uv_rect, sizeof(item.uv_rect));
    item.rotation_rad  = rotation_rad;
    item.key.texture     = spr.texture;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = debug_mode;
    Renderer->SubmitFxParticle(item);
}

// One bolt strand: an authored rectangleNN bolt column stretched from
// `from` to `to` via ONE strip segment. The strip pipeline anchors both
// world endpoints exactly and expands width perpendicular in screen
// space — no rotation composition at all (the particle pipeline's
// rotate-then-scale corner math can't rotate tall-thin quads rigidly).
// uv_swapped routes along-length to texture V so the authored column
// (which runs down the image) maps down the strand, and v_left/v_right
// select the column's U band.
// `head_at_to`: the authored columns carry a bright white "head" baked
// at their v-min end. In the reference that head is always buried inside
// the ball glow — orient it toward whichever strand end sits at the
// ball (user foreground review 2026-06-09: stray white highlights at
// strand ends = the head rendering at the caster end).
static void SubmitBoltStrand(const State* st, int32_t variant,
                             const float from[3], const float to[3],
                             bool head_at_to,
                             float rgb_fade, EFxDebugMode debug_mode)
{
    if (variant < 0 || variant >= int32_t(st->bolts.size()))
        return;
    const SSprite& spr = st->bolts[size_t(variant)];
    if (!spr.valid)
        return;

    const float dx = to[0] - from[0];
    const float dy = to[1] - from[1];
    const float dz = to[2] - from[2];
    const float reach_wu = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (reach_wu < 1.0f)
        return;

    // Width scales with reach like retail's uniform matrix scale on the
    // authored 15x~90 quad. Snapshot SetWidth(32,20) taper: wide at the
    // source end (`from`), narrowing toward the tip (stripeffect.cpp:642).
    const float width_wu = spr.width_wu * (reach_wu / spr.height_wu);

    SStripSegment seg = {};
    seg.world_a[0] = from[0];
    seg.world_a[1] = from[1];
    seg.world_a[2] = from[2];
    seg.world_b[0] = to[0];
    seg.world_b[1] = to[1];
    seg.world_b[2] = to[2];
    seg.width_a_wu = width_wu;
    seg.width_b_wu = width_wu * kHiltTipTaper;
    for (int32_t k = 0; k < 3; ++k)
    {
        seg.color_a[k] = rgb_fade;
        seg.color_b[k] = rgb_fade;
    }
    seg.color_a[3] = 1.0f;
    seg.color_b[3] = 1.0f;
    // Along-length: full authored column v span, head end oriented per
    // head_at_to. The bright white head is baked at the column's v-MAX
    // end (user-confirmed 2026-06-09: first guess of v-min rendered the
    // white spots at the outer strand ends; the head belongs at the
    // effect center / ball). Across-width: the column's u band.
    // uv_swapped transposes at the emit site.
    const float v_min = spr.uv_rect[1];
    const float v_max = spr.uv_rect[1] + spr.uv_rect[3];
    seg.u_a        = head_at_to ? v_min : v_max;
    seg.u_b        = head_at_to ? v_max : v_min;
    seg.v_left     = spr.uv_rect[0];
    seg.v_right    = spr.uv_rect[0] + spr.uv_rect[2];
    seg.uv_swapped = 1;

    SStripDrawItem item = {};
    item.segments     = &seg;
    item.num_segments = 1;
    item.key.texture     = spr.texture;
    item.key.pipeline_id = uint16_t(EFxPipeline::Strip);
    item.key.blend       = uint8_t(EFxBlend::AdditiveStraight);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.debug_mode      = debug_mode;
    Renderer->SubmitFxStrip(item);
}

static void SubmitSparks(const State* st, const SSparkParticle* arr,
                         int32_t count, const float tint[3],
                         EFxDebugMode debug_mode)
{
    if (!st->spark.valid)
        return;
    for (int32_t i = 0; i < count; ++i)
    {
        const SSparkParticle& p = arr[i];
        if (!p.alive) continue;
        const float s = p.scale * (p.flicker ? kSparkFlickerMul : 1.0f) *
                        kSparkQuadCorrection;
        SubmitSpriteQuadTinted(st->spark, p.pos,
                               st->spark.width_wu * s, st->spark.height_wu * s,
                               0.0f, tint, st->fade, debug_mode);
    }
}

// ==========================================================================
// Public harness entry points
// ==========================================================================

State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();

    // TARGET/SOURCE staging: harness origin = the TARGET (centered by
    // SpellGround); cast_pos derived after dir/range roll below.
    st->target_pos[0] = float(origin.x);
    st->target_pos[1] = float(origin.y);
    st->target_pos[2] = float(origin.z) + kHandZLift;

    // --- Load the RETAIL Magic\NewLightStrip.I3D (forensics §2.1: the
    // snapshot dev asset never shipped; the authored bolt art is the
    // visual identity — no procedural stand-ins).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[lightstrip-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[lightstrip-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();

    // --- Sub-object discovery BY NAME (retail layout per manifest:
    // '#flare' + 10x 'rectangleNN' + '#sparks' + empty 'csparks').
    std::string names_log;
    for (int32_t o = 0; o < num_obj; ++o)
    {
        const char* nm_c = img3d->GetObjectName(o);
        const std::string nm = nm_c ? std::string(nm_c) : std::string();
        if (!names_log.empty()) names_log += ", ";
        names_log += std::to_string(o) + "='" + nm + "'";

        if (nm == "#flare" || nm == "flare")
            st->flare = ReadSprite(img3d, o, num_tex);
        else if (nm == "#sparks" || nm == "sparks")
            st->spark = ReadSprite(img3d, o, num_tex);
        else if (nm.rfind("rectangle", 0) == 0)
        {
            SSprite b = ReadSprite(img3d, o, num_tex);
            if (b.valid)
                st->bolts.push_back(b);
            else
                log_warn("[lightstrip-shim] bolt sub-obj %d '%s' invalid "
                         "(tex=%u uvw=%.3f uvh=%.3f w=%.1f h=%.1f)",
                         o, nm.c_str(), b.texture,
                         double(b.uv_rect[2]), double(b.uv_rect[3]),
                         double(b.width_wu), double(b.height_wu));
        }
    }
    log_info("[lightstrip-shim] '%s' sub-objects: %s", kImageryPath,
             names_log.c_str());

    if (st->bolts.empty() || !st->flare.valid)
    {
        // The authored bolt columns + flare ARE the effect. Per
        // AGENT_GUIDE §4.2.1 a missing asset is a blocker, not an
        // invitation to draw procedural stand-ins.
        log_error("[lightstrip-shim] asset missing required sub-objects "
                  "(bolts=%zu flare_valid=%d spark_valid=%d) — aborting spawn",
                  st->bolts.size(), int(st->flare.valid), int(st->spark.valid));
        delete st;
        return nullptr;
    }
    if (!st->spark.valid)
        log_warn("[lightstrip-shim] '#sparks' sprite invalid — sparks skipped");

    // --- Test-rig heading + range (retail raycasts via MapPane).
    // dir = unit hand->target; the caster sits range wu behind the target.
    const float yaw = frand01() * 6.2831853f;
    st->dir[0] = std::cos(yaw);
    st->dir[1] = std::sin(yaw);
    st->dir[2] = 0.0f;
    st->range  = kTestRangeMin + frand01() * (kTestRangeMax - kTestRangeMin);
    st->cast_pos[0] = st->target_pos[0] - st->dir[0] * st->range;
    st->cast_pos[1] = st->target_pos[1] - st->dir[1] * st->range;
    st->cast_pos[2] = st->target_pos[2];

    for (auto& a : st->ambient)
    {
        a.yaw     = frand01() * 6.2831853f;
        a.reach   = kAmbientReachMin +
                    frand01() * (kAmbientReachMax - kAmbientReachMin);
        a.life    = snap_random(kAmbientLifeMin, kAmbientLifeMax);
        a.variant = snap_random(0, int32_t(st->bolts.size()) - 1);
    }
    st->main_variant = snap_random(0, int32_t(st->bolts.size()) - 1);

    st->phase     = kLaunch;
    st->alive     = true;

    log_info("[lightstrip-shim] spawned at (%d,%d,%d) yaw=%.2f range=%.0f "
             "bolts=%zu flare(tex=%u uv=%.3f,%.3f,%.3f,%.3f %g x %g wu) "
             "spark(tex=%u uv=%.3f,%.3f,%.3f,%.3f %g x %g wu)",
             origin.x, origin.y, origin.z, double(yaw), double(st->range),
             st->bolts.size(),
             st->flare.texture,
             double(st->flare.uv_rect[0]), double(st->flare.uv_rect[1]),
             double(st->flare.uv_rect[2]), double(st->flare.uv_rect[3]),
             double(st->flare.width_wu),   double(st->flare.height_wu),
             st->spark.texture,
             double(st->spark.uv_rect[0]), double(st->spark.uv_rect[1]),
             double(st->spark.uv_rect[2]), double(st->spark.uv_rect[3]),
             double(st->spark.width_wu),   double(st->spark.height_wu));
    st->spawn_logged = true;
    return st;
}

void Tick(State* st)
{
    if (!st || !st->alive)
        return;
    st->accum_ms += TTime::DeltaTime() * 1000.0;
    while (st->accum_ms >= double(kSimTickMs))
    {
        st->accum_ms -= double(kSimTickMs);
        SimTick(st);
        if (!st->alive)
            break;
    }
}

void Submit(State* st)
{
    if (!st || !st->alive || !Renderer)
        return;

    const EFxDebugMode dbg = EFxDebugMode::Normal;

    // --- Connector strand: caster hand -> the live traveling ball
    // (hybrid staging per file banner — stays connected to the source).
    if (st->traveled > 1.0f)
    {
        // head_at_to: the bright head end rides the ball.
        SubmitBoltStrand(st, st->main_variant, st->cast_pos, st->ball_pos,
                         /*head_at_to=*/true, st->fade, dbg);
    }

    // --- Traveling-ball composite, alive from spawn ("the end stage
    // occurs at the beginning"): purple ball + crackle fan, riding
    // ball_pos from hand to target.
    if (st->glow_scale > kGlowDrawThreshold)
    {
        // Crackle fan: short authored strands radiating from the ball.
        // During the burst the strands EXPAND with the glow (glow_scale
        // grows x1.15/tick in EXPLODE) while fading.
        const float expand = st->glow_scale / kGlowScaleInit;
        for (const auto& a : st->ambient)
        {
            const float reach = st->range * a.reach * expand;
            const float cy = std::cos(a.yaw), sy = std::sin(a.yaw);
            // Root pulled inward past center (kFanRootInset) so the
            // stroke stays planted under the glow as it lengthens.
            const float root[3] = {
                st->ball_pos[0] - cy * reach * kFanRootInset,
                st->ball_pos[1] - sy * reach * kFanRootInset,
                st->ball_pos[2],
            };
            const float tip[3] = {
                st->ball_pos[0] + cy * reach,
                st->ball_pos[1] + sy * reach,
                st->ball_pos[2],
            };
            // head_at_to=false: head stays at the root (inside the
            // ball); width falls out of the uniform-scale rule (short
            // arcs are naturally thinner than the long connector).
            SubmitBoltStrand(st, a.variant, root, tip,
                             /*head_at_to=*/false, st->fade, dbg);
        }

        // Purple ball: '#flare' drawn twice counter-rotating (snapshot
        // two-half glow, stripeffect.cpp:861-902) + per-frame scale
        // jitter random(0, 0.4) (stripeffect.cpp:870).
        const float scl = st->glow_scale + float(snap_random(0, 4)) / 10.0f;
        const float w   = st->flare.width_wu  * scl;
        const float h   = st->flare.height_wu * scl;
        const float to_rad = 3.14159265f / 180.0f;
        constexpr float kNoTint[3] = {1.0f, 1.0f, 1.0f};
        SubmitSpriteQuadTinted(st->flare, st->ball_pos, w, h,
                               -st->rotdeg * to_rad, kNoTint, st->fade, dbg);
        SubmitSpriteQuadTinted(st->flare, st->ball_pos, w, h,
                               st->morrotdeg * to_rad, kNoTint, st->fade, dbg);

        // White core: pinned at ~arrival-ball size (does NOT ride the
        // burst expansion), on its slower core_fade track — outlasts
        // the halo + bolts and fills the center as the ring expands.
        const float core_w = st->flare.width_wu  * kGlowScaleInit * kCoreScale;
        const float core_h = st->flare.height_wu * kGlowScaleInit * kCoreScale;
        SubmitSpriteQuadTinted(st->flare, st->ball_pos, core_w, core_h,
                               -st->rotdeg * to_rad, kNoTint,
                               st->core_fade, dbg);
    }

    // --- Sparks: purple source twinkles at the hand + the white
    // electrical-explosion burst at the impact.
    SubmitSparks(st, st->cast_sparks, kMaxSparks, kSourceSparkTint, dbg);
    constexpr float kWhite[3] = {1.0f, 1.0f, 1.0f};
    SubmitSparks(st, st->impact_sparks, kBurstSparkCount, kWhite, dbg);
}

void SubmitWorld(State* /*st*/) {}   // billboard-only port; no world meshes

bool IsAlive(const State* st)
{
    return st && st->alive;
}

void Destroy(State* st) { delete st; }

}  // namespace lightstrip_shim
