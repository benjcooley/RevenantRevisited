// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  icebolt.cpp - TIceBoltAnimator port via the d3d::* shim              *
// *                                                                       *
// *  SHIM VALIDATION PORT (I21 TIceBoltEffect) — wave-2 shim port.       *
// *  Forensics source: docs/vfx/forensics/I21_TIceBoltEffect.md           *
// *  Snapshot source:  src/effect_old.cpp:7968-8713                      *
// *                    src/effect.h:1651-1685 (TIceBoltAnimator fields)   *
// *                                                                       *
// *  Retail fidelity: retail-partial. Asset icebolt.i3d byte-identical    *
// *  (MD5 72c35ab169970e59c25fe663702bdebe), "IceBolt" registration +    *
// *  spell wiring confirmed, TEffect base confirmed, subspell=3 init      *
// *  confirmed. Animate/render kinematic constants are snapshot-only      *
// *  (retail body not recovered in Ghidra). See forensics §2.1.           *
// *                                                                       *
// *  Architecture notes:                                                  *
// *  - Stationary freeze-beam (NOT a flying projectile). Snaps into       *
// *    existence from caster to first hit. Duration ~5 s (120 frames).   *
// *  - 7 sub-objects of icebolt.I3D — split between two render paths:    *
// *      WorldMesh (3D mesh draws):                                       *
// *        0-3 = cylinder04/01/02/03 (4 nested core beam cylinders)       *
// *          5 = cylinder05 (rings sliding along beam)                    *
// *          6 = cylinder06 (counter-rotating spirals)                    *
// *      ScreenAligned billboards (camera-facing sprites):                *
// *          4 = box01 (4-vert flat quad — manifest.txt:25)               *
// *              → used 3× for: glow end-caps, frost particles,           *
// *                snow flurry. The snapshot draws box01 as a WorldMesh   *
// *                with a baked screen-facing rotation hack (RotateX·     *
// *                RotateZ·−facing sequence at :8587-8590, :8615-8618,    *
// *                :8642-8645). That's the 1998 fake-billboarding pattern *
// *                for a flat quad; we use the renderer's true camera-    *
// *                alignment path. (WAVE_1_LESSONS §2: flat textured quad *
// *                meant to read as a glow/sprite = ScreenAligned.)       *
// *  - Unified composite render: mesh + billboards both fire from         *
// *    submit_world. The orchestrator's IceBoltShimSubmit wrapper only    *
// *    calls Tick (not Submit); SubmitWorld is the only render path.     *
// *    Per WAVE_1_LESSONS §4.1, ScreenAligned billboards work fine from   *
// *    submit_world (FxBillboard queue isn't cleared by BeginTilePass).   *
// *  - No texture animation (forensics §8: motion = geometry only).       *
// *  - No dynamic light in pre-release source (forensics §9).             *
// *  - No audio code-driven (forensics §11).                              *
// *  - Sub-effect I22 Iced posting: log the post point only (no spawn     *
// *    in test harness — MapPane/character not wired).                    *
// *                                                                       *
// *  Blend = Alpha (SetBlendState — MODULATE — forensics §7).            *
// *  Lit-mode = Self-lit/Unlit. No material zeroing. No per-vertex        *
// *  color write. Asset color (ice-blue textures) passes through          *
// *  under MODULATE. Emissive zeroed to prevent white-wash (WAVE_1        *
// *  LESSONS §1.1 — D3DMATERIAL emissive is additive self-illumination   *
// *  in our shader, NOT the original D3D convention).                     *
// *  Depth = TestNoWrite (SetBlendState — forensics §7).                 *
// *                                                                       *
// *  Test rig anchor:                                                     *
// *    caster = world origin (0,0,0)                                      *
// *    target = +X direction, beam length = kTestBeamLength = 500 wu     *
// *  The length march (frameon==0, effect_old.cpp:8152-8233) requires    *
// *  MapPane::GetWalkHeight which is unavailable in the VFX test rig.    *
// *  A fixed beam length is substituted; document at Spawn.              *
// *  The beam runs along local -Y per the I3D conventions; facing angle  *
// *  in the rig anchors it along +X.                                     *
// *                                                                       *
// *  REGISTRATION block: see .vfx_logs/icebolt.md § wave-2.             *
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace icebolt_shim {

// ==========================================================================
// Constants — every value cites forensics §3 + snapshot line.
// All snapshot-only unless noted. "wu" = world units.
// ==========================================================================

// Master timing beat (effect_old.cpp:7972; forensics §3).
constexpr int32_t kGrowDuration   = 10;  // GROW_DURATION — frames per phase beat

// Scale/animation rates (effect_old.cpp:7973-7982; forensics §3).
constexpr float kSphereStep       = 0.5f;   // SPHERE_STEP — scale/frame for glow spheres
constexpr float kRingSpeed        = 20.0f;  // RING_SPEED  — wu/frame ring slide
constexpr float kSpiralStep       = 1.0f;   // SPIRAL_STEP — rad/frame spiral spin
constexpr float kSpiralScaleStep  = 0.1f;   // SPIRAL_SCALE_STEP — scale/frame
constexpr float kFrostInitSize    = 0.5f;   // FROST_INIT_SIZE — initial particle scale
constexpr float kFrostStep        = 0.03f;  // FROST_STEP — scale/frame shrink

// Particle counts (effect.h:1651-1652; forensics §3 divergence signal noted).
// Retail ctor allocates 300-entry buffers; snapshot uses 50. Using snapshot
// value as starting point; bump toward 300 if frost cloud looks sparse vs retail.
constexpr int32_t kMaxFrostParticles = 50;  // MAX_FROST_PARTICLES (effect.h:1651)
constexpr int32_t kMaxSnowParticles  = 50;  // MAX_SNOW_PARTICLES  (effect.h:1652)

// Lifetime = GROW_DURATION * 12 = 120 frames (~5 s at 24 Hz) (effect_old.cpp:8451).
constexpr int32_t kLifetimeFrames = kGrowDuration * 12;  // 120 frames

// Frost/snow particle init parameters (effect_old.cpp:8037-8044; forensics §3).
constexpr float kFrostPosJitter   = 10.0f;  // ±10 wu position seed
constexpr float kFrostVelJitter   = 3.0f;   // ±3 wu/frame velocity seed
// Frost delay at seed: random(0, FROST_INIT_SIZE*2/FROST_STEP) = random(0,33)
// (effect_old.cpp:8044). After beat 3, hard 100 (effect_old.cpp:8352).
constexpr int32_t kFrostDelayMax  = 33;     // = int(kFrostInitSize*2/kFrostStep)
constexpr int32_t kFrostDelayPost3 = 100;   // t[i]=100 after frameon>3*GD (effect_old.cpp:8353)

// Snow flurry init parameters (effect_old.cpp:8123-8128; forensics §3).
constexpr float kSnowHMin  = -50.0f;  // h[i] z range (wu)
constexpr float kSnowHMax  =  50.0f;
constexpr float kSnowRsMin =  0.05f;  // rs[i] = random(5,15)/100 rad/frame
constexpr float kSnowRsMax =  0.15f;
constexpr float kSnowRMin  =  25.0f;  // r[i] orbit radius range (wu)
constexpr float kSnowRMax  =  50.0f;
// sz[i] = random(FROST_STEP*100, FROST_STEP*GD*100)/100 = random(3,30)/100 = 0.03..0.30
constexpr float kSnowSzMin = kFrostStep;                       // 0.03
constexpr float kSnowSzMax = kFrostStep * kGrowDuration;       // 0.30

// Initialize z-lift for bolt origin (effect_old.cpp:8023).
constexpr float kOriginZLift = 50.0f;  // +50 wu above caster pos

// Imagery path (forensics §4 — "IceBolt" registry name).
constexpr const char* kImageryPath = "Magic\\icebolt.I3D";

// Sub-object indices (forensics §4 — file-order verified vs manifest.txt).
//   0  = cylinder04  (core beam, outermost)
//   1  = cylinder01  (core beam)
//   2  = cylinder02  (core beam)
//   3  = cylinder03  (core beam, innermost)
//   4  = box01       (glow sphere / frost / snow billboard quad)
//   5  = cylinder05  (rings)
//   6  = cylinder06  (spirals)
constexpr int32_t kSubObjCyl0     = 0;
constexpr int32_t kSubObjCyl3     = 3;  // last core cylinder
constexpr int32_t kSubObjBox01    = 4;
constexpr int32_t kSubObjRing     = 5;
constexpr int32_t kSubObjSpiral   = 6;
constexpr int32_t kTotalSubObjs   = 7;

// Test-rig beam length (see header note — length march unavailable in test rig).
// 500 wu = 50 steps × 10 wu/step, middle of the 0..1000 range.
constexpr float kTestBeamLength = 500.0f;

// Box01 billboard base size (Gotcha #3).
// The box01 sub-object (sub-obj 4) is a 4-vert flat quad authored with
// bbox (-10.4237, 0, -10.4237)..(10.4237, 0, 10.4237) per manifest.txt —
// full extent ≈ 20.85 wu. The snapshot's RenderObject(obj) scales the
// authored vertex positions by obj->scl. SubmitFxBillboard takes a full
// WU size (size_wu) and the renderer handles projection. So:
//   size_wu = snapshot_scl × full_extent
//
// Per-consumer base sizes (per user feedback round 5 — reference frames
// show the target-end glow MUCH bigger than my round-4 had):
//   GLOW caps  → kBox01GlowBaseSizeWu  = 36  (~75% larger than mesh-extent)
//   FROST particles → kBox01FrostBaseSizeWu = 21  (mesh-extent — small sparkles)
//   SNOW flurry → kBox01SnowBaseSizeWu = 21   (mesh-extent — small flakes)
//
// At GROW peak (frameon=10): spherescale[0]=5.5 → caster glow = 5.5/2 × 36 = 99 wu;
// spherescale[1]=11 → target glow = 11/2 × 36 = 198 wu. The asymmetry
// (target larger than caster) is preserved from snapshot's spherescale[1]
// growing 2× faster than spherescale[0] — matches reference frames where
// the target-end ground burst is the dominant visual element.
constexpr float kBox01GlowBaseSizeWu  = 36.0f;   // glow end-caps (bigger to hide beam termination geometry)
constexpr float kBox01FrostBaseSizeWu = 21.0f;   // frost particles (small sparkles)
constexpr float kBox01SnowBaseSizeWu  = 21.0f;   // snow flurry (small flakes)

// 24 Hz sim tick gate (framerate-independent, WAVE_1_LESSONS §1.4).
constexpr int32_t kSimTickMs = 1000 / 24;  // ~41.6 ms/tick

// Useful math constants.
constexpr float kM_PI  = 3.14159265358979323846f;
constexpr float kM_2PI = 2.0f * kM_PI;
constexpr float kToRad = kM_PI / 180.0f;

// ==========================================================================
// ResolveSubObjTextureSlot — walk a sub-object's texfaces[] table to find
// which 0-based texture index it samples.
//
// icebolt.I3D manifest (per RevenantRevisited/i3d_dump_all/icebolt/manifest.txt):
//   object[0..3] cylinder04/01/02/03  texslot=0 → no texture (WhiteTexture)
//   object[4]    box01                texslot=1 → texture index 0 (glow)
//   object[5]    cylinder05 (rings)   texslot=2 → texture index 1 (icicles)
//   object[6]    cylinder06 (spirals) texslot=3 → texture index 2 (stripes)
//
// Mirrors the canonical helper at fireball.cpp:226. Returns -1 if no slot
// found (defaults to 0 in caller).
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
            return s - 1;     // 1-based texfaces slot → 0-based texture index
    return -1;
}

// ==========================================================================
// Inclusive-range random — matches snapshot random(lo, hi) semantics.
// (revutils.cpp:1597-1613: rand() % (max-min+1) + min)
// ==========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// ==========================================================================
// Per-frost-particle state (effect.h:1665-1666 — p/v/s/t arrays).
//
// `base_y` is a per-particle anchor along the beam axis (local-y in
// [0, -length]) — NOT present in snapshot effect_old.cpp.
//
// Snapshot init places ALL frost at the target end (`p[i].y - length`
// in render, with p[i].y a tight ±10 wu jitter from Initialize :8038).
// That clusters every particle in a small ball at the impact point
// (forensics §6.3 "clustered at the TARGET end of the beam").
//
// The retail reference frames (images/vfx/25_IceBolt/05.png, 08.png,
// 12.png) show frost sparkles **distributed all along the beam length**
// — not clustered at one end. Per [feedback_snapshot_vs_retail] +
// forensics §2.1.4 ("animate/render bodies were NOT recovered in
// readable form" / "kinematic constants are snapshot-only, unverified
// against shipped"), the snapshot's clustering is dev-WIP behavior that
// shipped retail apparently changed.
//
// `base_y` per particle, initialized at Spawn time to a random distance
// along the beam, captures the visual distribution the user (original
// developer) reports as the shipped behavior. The snapshot's p[i].y
// micro-jitter is then added on top, giving each particle a stable
// position along the beam plus drifting fine motion.
// ==========================================================================
struct FrostParticle
{
    float pos[3] = {0,0,0};  // p[i] (wu, local)
    float vel[3] = {0,0,0};  // v[i] (wu/frame)
    float scl    = 0.0f;     // s[i] (scale, 0..kFrostInitSize)
    float delay  = 0.0f;     // t[i] (countdown; >0 = not yet active)
    float base_y = 0.0f;     // per-particle anchor along beam axis (round-5)
};

// ==========================================================================
// Per-snow-particle state (effect.h:1669-1670 — h/th/r/rs/sz arrays).
// ==========================================================================
struct SnowParticle
{
    float h  = 0.0f;  // z position (wu)
    float th = 0.0f;  // orbit angle (rad)
    float r  = 0.0f;  // orbit radius (wu)
    float rs = 0.0f;  // angular velocity (rad/frame)
    float sz = 0.0f;  // scale
};

// ==========================================================================
// Per-effect state — mirrors TIceBoltAnimator member fields (effect.h:1655-1670).
// ==========================================================================
struct State
{
    // Core animator state (effect.h:1656-1664)
    int32_t frameon    = 0;      // per-sim-tick counter
    int32_t angle      = 0;      // beam facing as byte-angle (0..255)
    float   length     = 0.0f;   // beam length in wu (set at frameon==0 march)
    float   spherescale[2] = {kSphereStep, 1.0f};  // glow sphere scales (effect_old.cpp:8029-8030)
    float   cylscale   = kSphereStep;   // core cylinder x/y scale (effect_old.cpp:8031)
    float   ringout    = 0.0f;          // ring slide position (wu along beam) (effect_old.cpp:8032)
    float   spiralang  = 0.0f;          // spiral rotation angle (rad) (effect_old.cpp:8033)
    float   spiralscale = kSpiralScaleStep;  // spiral x/y scale (effect_old.cpp:8034)
    float   cy         = 0.0f;   // snow sweep position along beam (effect_old.cpp:8129)

    // Frost particle array (effect.h:1665-1666)
    FrostParticle frost[kMaxFrostParticles];

    // Snow flurry particle array (effect.h:1669-1670)
    SnowParticle  snow[kMaxSnowParticles];

    // World position (anchor = caster pos + z_lift in production;
    // in test harness = world origin + kOriginZLift in z).
    float base_pos[3] = {0,0,0};

    // Facing angle as radians (computed once from angle at frameon==0).
    float face_rad = 0.0f;

    // Framerate-independence accumulator (24 Hz gate; WAVE_1_LESSONS §1.4).
    double sim_accum_ms = 0.0;

    // Lifecycle
    bool alive = true;

    // Asset handles — loaded at Spawn.
    T3DImagery* imagery = nullptr;

    // Pre-registered mesh handles, indexed by sub-object index.
    // Used for the WorldMesh draws (sub-objects 0..3, 5, 6). Sub-object 4
    // (box01) is rendered as ScreenAligned billboards instead — see below.
    MeshHandle handles[kTotalSubObjs] = {};

    // Per-sub-obj material (diffuse/emissive) — loaded at Spawn.
    float diffuse[kTotalSubObjs][4]  = {};
    float emissive[kTotalSubObjs][4] = {};

    // box01 billboard assets — sub-object 4 is a single 4-vertex flat quad
    // (manifest.txt: bbox=(-10.4237,0,-10.4237)..(10.4237,0,10.4237) — flat
    // in Y, ≈21 wu across). The snapshot draws it as a WorldMesh with a
    // baked screen-facing rotation sequence (RotateX(-π/2)·RotateX(-π/6)·
    // RotateZ(-π/4)·RotateZ(-face)). That's the 1998 convention for "fake"
    // screen-aligning a flat quad in the legacy D3D pipeline.
    //
    // Per WAVE_1_LESSONS §2 orientation chooser, a flat 4-vert quad with
    // a glow/sprite texture authored to face the camera = ScreenAligned.
    // We render box01 as a true ScreenAligned billboard via SubmitFxBillboard
    // (the d3d::Orientation::ScreenAligned path) instead of replicating the
    // legacy rotation hack as a mesh draw. This is used for all three box01
    // consumers: glow end-caps, frost particles, snow flurry.
    //
    // box01 texture: per manifest.txt:25 box01 has texslot=1 (1-based texfaces
    // slot). RegisterSubMesh convention (d3dport.cpp:156-172) treats texslot 0
    // as "untextured / WhiteTexture" and slots 1..N as textures 0..N-1. So
    // box01 texslot=1 → texture index 0 (the glow circle, texture_00_frame_00.png
    // per i3d_dump_all). Resolved at Spawn via ResolveSubObjTextureSlot (which
    // walks the sub-object's texfaces[] table — see fireball.cpp pattern).
    //
    // Earlier iter (round 3) hardcoded box01_tex_index=1, which is WRONG — it
    // sampled texture[1] (icicles / ring texture) instead of texture[0] (glow),
    // making the glow billboard render as a ring-texture quad at the bolt end
    // (the round-3 regression the user reported).
    TTextureHandle box01_texture   = kInvalidTexture;
    float          box01_uv_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    int32_t        box01_tex_index = 0;  // resolved at Spawn; placeholder is correct fallback

    // Diagnostic flags
    bool spawn_logged        = false;
    bool first_render_logged = false;
    int32_t second_log_done  = 0;   // 0 = none; 1 = frame=5 logged; 2 = frame=15 logged
};

// ==========================================================================
// Initialize — transcribed from snapshot effect_old.cpp:8015-8135.
//
// In the test harness:
//   - angle = 192 (byte-angle for +X world direction: GetFace()*360/256 = 270°,
//     sin(270°)=−1, cos(270°)=0, but facing along +X in iso = angle ~192 wraps
//     into roughly (sin≈1, cos≈0) for local-y beam direction). We use a fixed
//     test angle representing "beam runs along world +X", which in the local
//     frame runs along −y. The actual GetAngle() in production resolves
//     invoker→target facing (effect_old.cpp:434-455).
//   - length = kTestBeamLength (fixed; march not available — see header note).
//   - subspell is forced to 3 (effect_old.cpp:8049; forensics §6.1 §13.2;
//     retail-confirmed via cls_0x5aaf28.cpp:131).
// ==========================================================================
static void Initialize(State* st)
{
    st->frameon     = 0;                                   // :8028
    st->spherescale[0] = kSphereStep;                      // :8029
    st->spherescale[1] = 1.0f;                             // :8030
    st->cylscale    = kSphereStep;                         // :8031
    st->ringout     = 0.0f;                                // :8032
    st->spiralang   = 0.0f;                                // :8033
    st->spiralscale = kSpiralScaleStep;                    // :8034
    st->cy          = 0.0f;                                // :8129 (snow sweep)

    // Test-rig beam length (production = length-march result, effect_old.cpp:8152-8232)
    // Set BEFORE frost particle seed because base_y derivation needs length.
    st->length = kTestBeamLength;

    // angle from GetAngle() (:8047) — test harness: fixed facing for +X beam.
    // byte angle 192 → face*360/256 = 270°; sin(270°)=−1, cos(270°)=0 in std.
    // In the iso coordinate system: dx=10*sin(θ), dy=−10*cos(θ).
    // For beam along +world-X: dx>0, dy~0 → sin(θ)>0, cos(θ)~0 → θ~90° → byte~64.
    // Use byte angle 64 (90°): sin(90°)=1, cos(90°)=0 → dx=10, dy=0. ✓
    st->angle = 64;   // test harness only; in production = GetAngle()
    st->face_rad = float(st->angle) * (360.0f / 256.0f) * kToRad;

    // Frost particle seed. Snapshot effect_old.cpp:8035-8045 sets p.{xyz} to
    // random ±10 wu and clusters every particle at the target end via the
    // render-time `p[i].y - length` offset (effect_old.cpp:8623). Per round-5
    // user feedback + reference frames 05/08/12, the shipped retail distributes
    // frost along the entire beam, not clustered at one end. So we add a
    // per-particle base_y in [-length, 0] (anchor along beam axis). The
    // snapshot's ±10 wu p.y jitter is preserved on top, giving each particle
    // a stable beam-axis anchor + fine drifting motion. See FrostParticle
    // struct comment for citation.
    const int32_t length_int = int32_t(st->length);  // snap_random takes ints
    for (int32_t i = 0; i < kMaxFrostParticles; ++i)
    {
        st->frost[i].pos[0] = float(snap_random(-10, 10));  // :8037
        st->frost[i].pos[1] = float(snap_random(-10, 10));  // :8038
        st->frost[i].pos[2] = float(snap_random(-10, 10));  // :8039
        st->frost[i].vel[0] = float(snap_random(-3, 3));    // :8040
        st->frost[i].vel[1] = float(snap_random(-3, 3));    // :8041
        st->frost[i].vel[2] = float(snap_random(-3, 3));    // :8042
        st->frost[i].scl    = kFrostInitSize;               // :8043
        st->frost[i].delay  = float(snap_random(0, kFrostDelayMax));  // :8044
        // Per-particle anchor along beam axis (round-5): random distance from
        // caster (y=0) to target end (y=-length). Negative because beam runs
        // along local -Y.
        st->frost[i].base_y = -float(snap_random(0, length_int > 0 ? length_int : 1));
    }

    // Snow particle seed (subspell>2 branch, effect_old.cpp:8119-8134)
    // subspell is forced to 3 (§13.2), so this always runs in the pre-release build.
    for (int32_t i = 0; i < kMaxSnowParticles; ++i)
    {
        st->snow[i].h  = float(snap_random(int32_t(kSnowHMin), int32_t(kSnowHMax)));  // :8123
        st->snow[i].th = float(snap_random(0, 359)) * kToRad;                         // :8124
        st->snow[i].rs = float(snap_random(5, 15)) / 100.0f;                          // :8125
        st->snow[i].sz = float(snap_random(int32_t(kFrostStep * 100.0f),
                                            int32_t(kFrostStep * kGrowDuration * 100.0f))) / 100.0f; // :8126
        st->snow[i].r  = float(snap_random(int32_t(kSnowRMin), int32_t(kSnowRMax)));  // :8127
    }
}

// ==========================================================================
// SimTick — one 24 Hz sim step. Transcribed from snapshot
// effect_old.cpp:8144-8455. Called by Animate() once per accumulated tick.
//
// In the test harness, sections that require MapPane/spell/character
// (length march, DamageCharactersInRange, FindObjectsInRange, Iced spawn)
// are guarded out with // [HARNESS GATE] comments noting the production path.
// ==========================================================================
static void SimTick(State* st)
{
    // [HARNESS GATE] Production: length march at frameon==0 (effect_old.cpp:8152-8232).
    // Requires MapPane::GetWalkHeight + TMapIterator character enumeration.
    // In test harness: length was fixed at kTestBeamLength in Initialize.

    st->frameon++;  // :8234

    // -----------------------------------------------------------------------
    // Snow flurry update (subspell>2 — always active since subspell forced=3)
    // effect_old.cpp:8236-8273
    // -----------------------------------------------------------------------
    for (int32_t i = 0; i < kMaxSnowParticles; ++i)
    {
        st->snow[i].th += st->snow[i].rs;               // :8240 — orbit advance
        if (st->snow[i].th > kM_2PI)
            st->snow[i].th -= kM_2PI;
        st->snow[i].rs += 0.01f;                         // :8243 — angular acceleration
    }
    if (st->frameon > kGrowDuration * 2)                 // :8245 — sweep along beam
    {
        st->cy += st->length / float(kGrowDuration * 2);
        if (st->cy >= st->length)
            st->cy = st->length;
    }
    if (st->frameon > kGrowDuration * 5)                 // :8251 — "blow up"
    {
        for (int32_t i = 0; i < kMaxSnowParticles; ++i)
        {
            st->snow[i].h  += 5.0f;                      // :8266 — rise
            st->snow[i].r  -= 1.0f;                      // :8267 — tighten
            if (st->snow[i].r < 1.0f)
                st->snow[i].r = 1.0f;
            st->snow[i].rs += 0.05f;                     // :8270 — spin faster
        }
    }

    // -----------------------------------------------------------------------
    // Frost particle update (effect_old.cpp:8331-8354)
    // -----------------------------------------------------------------------
    for (int32_t i = 0; i < kMaxFrostParticles; ++i)
    {
        st->frost[i].delay -= 1.0f;                      // :8333 t[i]--
        if (st->frost[i].delay > 0.0f)
            continue;
        else
            st->frost[i].delay = 0.0f;                   // :8337 t[i]=0

        st->frost[i].pos[0] += st->frost[i].vel[0];      // :8338-8340
        st->frost[i].pos[1] += st->frost[i].vel[1];
        st->frost[i].pos[2] += st->frost[i].vel[2];
        st->frost[i].scl -= kFrostStep;                   // :8341

        if (st->frost[i].scl < 0.0f)                     // :8342 — respawn
        {
            st->frost[i].pos[0] = float(snap_random(-10, 10));  // :8344
            st->frost[i].pos[1] = float(snap_random(-10, 10));  // :8345
            st->frost[i].pos[2] = float(snap_random(-10, 10));  // :8346
            st->frost[i].vel[0] = float(snap_random(-3, 3));    // :8347
            st->frost[i].vel[1] = float(snap_random(-3, 3));    // :8348
            st->frost[i].vel[2] = float(snap_random(-3, 3));    // :8349
            st->frost[i].scl    = kFrostInitSize;                // :8350
            st->frost[i].delay  = float(snap_random(0, int32_t(kFrostInitSize / kFrostStep)));  // :8351 → random(0,16)
            if (st->frameon > kGrowDuration * 3)
                st->frost[i].delay = float(kFrostDelayPost3);   // :8352-8353
            // Re-seed beam-axis anchor on respawn so the distribution stays
            // even across the beam through the effect's lifetime (round-5).
            const int32_t length_int = int32_t(st->length);
            st->frost[i].base_y = -float(snap_random(0, length_int > 0 ? length_int : 1));
        }
    }

    // -----------------------------------------------------------------------
    // Transform-animation envelopes — cylscale / spherescale / spiralscale / spiralang
    // effect_old.cpp:8356-8392
    // -----------------------------------------------------------------------
    if (st->frameon < kGrowDuration)                    // :8356 — GROW phase
    {
        st->spherescale[0] += kSphereStep;              // :8358
        st->spherescale[1] += kSphereStep * 2.0f;       // :8359
        st->cylscale       += kSphereStep;              // :8360
        st->spiralscale    += kSpiralScaleStep;         // :8361
        st->spiralang      += kSpiralStep;              // :8362
        if (st->spiralang > kM_2PI) st->spiralang -= kM_2PI;
    }
    if (st->frameon >= kGrowDuration &&
        st->frameon < kGrowDuration * 2)               // :8366 — cyl shrinks, spheres still grow
    {
        st->cylscale       -= kSphereStep / 2.0f;      // :8368
        st->spherescale[0] += kSphereStep;             // :8369
        st->spherescale[1] += kSphereStep;             // :8370
        st->spiralang      += kSpiralStep;             // :8371
        if (st->spiralang > kM_2PI) st->spiralang -= kM_2PI;
    }
    if (st->frameon >= kGrowDuration * 2 &&
        st->frameon < kGrowDuration * 3)               // :8375 — cyl+spheres shrink
    {
        st->cylscale       -= kSphereStep / 2.0f;      // :8377
        st->spherescale[0] -= kSphereStep;             // :8378
        st->spherescale[1] -= kSphereStep * 1.0f;      // :8379
        st->spiralang      += kSpiralStep;             // :8380
        if (st->spiralang > kM_2PI) st->spiralang -= kM_2PI;
    }
    if (st->frameon >= kGrowDuration * 3 &&
        st->frameon < kGrowDuration * 4)               // :8384 — spheres+spiral shrink
    {
        st->spherescale[0] -= kSphereStep;             // :8386
        st->spherescale[1] -= kSphereStep * 2.0f;      // :8387
        st->spiralscale    -= kSpiralScaleStep;        // :8388
    }

    // Ring slide (effect_old.cpp:8390-8392)
    st->ringout += kRingSpeed;
    if (st->ringout > st->length)
        st->ringout -= st->length;

    // -----------------------------------------------------------------------
    // Damage beat @ frameon==GROW_DURATION — [HARNESS GATE]
    // Production: DamageCharactersInRange(caster, target_end, 200, min, max, DAMAGE_ICE)
    // (effect_old.cpp:8393-8401)
    // -----------------------------------------------------------------------
    if (st->frameon == kGrowDuration)
    {
        log_info("[icebolt-shim] DAMAGE beat (frameon==%d): "
                 "production would DamageCharactersInRange at target end "
                 "with DAMAGE_ICE. [HARNESS GATE — no MapPane in test rig]",
                 st->frameon);
    }

    // -----------------------------------------------------------------------
    // Freeze beat @ frameon==2*GROW_DURATION — [HARNESS GATE]
    // Production: FindObjectsInRange → spawn "Iced" sub-effect (I22) on each victim
    // via effect->CreateAnimator(); ia->InitIced(victim)
    // (effect_old.cpp:8409-8450)
    // -----------------------------------------------------------------------
    if (st->frameon == kGrowDuration * 2)
    {
        log_info("[icebolt-shim] FREEZE beat (frameon==%d): "
                 "production would FindObjectsInRange + spawn I22 Iced sub-effect. "
                 "[HARNESS GATE — no MapPane in test rig]",
                 st->frameon);
    }

    // -----------------------------------------------------------------------
    // Lifetime kill @ frameon==GROW_DURATION*12 = 120 frames (~5 s at 24 Hz)
    // (effect_old.cpp:8451-8454)
    // -----------------------------------------------------------------------
    if (st->frameon >= kLifetimeFrames)
    {
        log_info("[icebolt-shim] KillThisEffect @ frameon=%d (kLifetimeFrames=%d)",
                 st->frameon, kLifetimeFrames);
        st->alive = false;
    }
}

// ==========================================================================
// Animate — framerate-independent 24 Hz wrapper over SimTick.
// Accumulates real wall-clock delta; fires SimTick once per ~41.6 ms.
// ==========================================================================
static void Animate(State* st, double delta_ms)
{
    if (!st->alive) return;
    st->sim_accum_ms += delta_ms;
    while (st->sim_accum_ms >= double(kSimTickMs))
    {
        st->sim_accum_ms -= double(kSimTickMs);
        SimTick(st);
        if (!st->alive) break;
    }
}

// ==========================================================================
// RenderBeamCylinders — 4 nested core beam cylinders (sub-objects 0..3).
//
// Transcribed from effect_old.cpp:8473-8504.
//
// Each cylinder:
//   D3DMATRIXClear; flags=OBJ3D_MATRIX
//   Scale(cylscale/2, cylscale/2, length/64)
//   RotateX(+π/2)  — tips the unit cylinder along local −Y (beam axis)
//   RotateY(0); RotateZ(0)
//   Translate(0,0,0)
//   RenderObject
//
// Only rendered when subspell>1 (:8473) — always true (subspell forced=3).
// ==========================================================================
static void RenderBeamCylinders(State* st, const d3d::Matrix& inst_world,
                                 d3d::BlendMode blend)
{
    if (st->cylscale <= 0.0f) return;  // guard against negative scale during shrink phase

    for (int32_t i = kSubObjCyl0; i <= kSubObjCyl3; ++i)
    {
        if (st->handles[i] == 0) continue;

        d3d::Matrix local;
        d3d::MatrixIdentity(local);                          // D3DMATRIXClear
        d3d::MatrixScale(local,
                         st->cylscale / 2.0f,               // :8483 scl.x
                         st->cylscale / 2.0f,               // :8484 scl.y
                         st->length / 64.0f);               // :8485 scl.z = length/64
        d3d::MatrixRotateX(local, kM_PI / 2.0f);            // :8488 RotateX(+π/2)
        d3d::MatrixRotateY(local, 0.0f);                     // :8489 RotateY(0)
        d3d::MatrixRotateZ(local, 0.0f);                     // :8490 RotateZ(0)
        d3d::MatrixTranslate(local, 0.0f, 0.0f, 0.0f);      // :8499 Translate(0,0,0)

        d3d::RenderObject(*Renderer, st->handles[i], local,
                          st->diffuse[i], st->emissive[i], blend,
                          &inst_world, st->base_pos[2]);
    }
}

// ==========================================================================
// RenderSpirals — counter-rotating spiral segments (sub-object 6).
//
// Transcribed from effect_old.cpp:8507-8544.
//
// Only rendered while frameon < 3*GROW_DURATION (:8507).
// numrevs = int(length/100); one segment per revolution, two helices
// (mirrored at spiralang and π−spiralang).
//
//   Scale(spiralscale, spiralscale, zscale/64)
//   RotateX(+π/2)
//   RotateY(spiralang)  [or π−spiralang for mirror helix]
//   RotateZ(0)
//   Translate(0, ypos, 0)  — ypos = −j * zscale
// ==========================================================================
static void RenderSpirals(State* st, const d3d::Matrix& inst_world,
                          d3d::BlendMode blend)
{
    if (st->frameon >= kGrowDuration * 3) return;  // :8507 gate
    if (st->spiralscale <= 0.0f) return;           // guard against negative scale
    if (st->handles[kSubObjSpiral] == 0) return;

    const int32_t numrevs_raw = int32_t(st->length / 100.0f);
    const int32_t numrevs = (numrevs_raw > 1) ? numrevs_raw : 1;
    const float   zscale  = st->length / float(numrevs);  // :8514

    for (int32_t j = 0; j < numrevs; ++j)
    {
        const float ypos = float(-j) * zscale;  // :8518 ypos = −j*zscale

        for (int32_t i = 0; i < 2; ++i)  // :8519 two mirrored helices
        {
            const float rot_y = (i != 0)
                                 ? (kM_PI - st->spiralang)  // :8531 mirror helix
                                 : st->spiralang;           // :8531 primary helix

            d3d::Matrix local;
            d3d::MatrixIdentity(local);                           // D3DMATRIXClear (:8521)
            d3d::MatrixScale(local,
                             st->spiralscale,                    // :8524 scl.x
                             st->spiralscale,                    // :8525 scl.y
                             zscale / 64.0f);                    // :8527 zscale/64
            d3d::MatrixRotateX(local, kM_PI / 2.0f);             // :8530 RotateX(+π/2)
            d3d::MatrixRotateY(local, rot_y);                    // :8531 RotateY
            d3d::MatrixRotateZ(local, 0.0f);                     // :8532 RotateZ(0)
            d3d::MatrixTranslate(local, 0.0f, ypos, 0.0f);       // :8537 Translate(0,ypos,0)

            d3d::RenderObject(*Renderer, st->handles[kSubObjSpiral], local,
                              st->diffuse[kSubObjSpiral], st->emissive[kSubObjSpiral],
                              blend, &inst_world, st->base_pos[2]);
        }
    }
}

// ==========================================================================
// RenderRings — rings sliding along the beam (sub-object 5).
//
// Transcribed from effect_old.cpp:8546-8570.
//
// Only rendered while frameon < 3*GROW_DURATION (:8507 gate applies).
// Count = int(length/100); each ring slides at RING_SPEED, position mod length.
//
//   Scale(cylscale/2, cylscale/2, 1.0)
//   RotateX(+π/2)
//   RotateY(spiralang)
//   RotateZ(0)
//   Translate(0, −((ringout + 100*i) mod length), 0)
// ==========================================================================
static void RenderRings(State* st, const d3d::Matrix& inst_world,
                        d3d::BlendMode blend)
{
    if (st->frameon >= kGrowDuration * 3) return;  // :8507 gate (same block)
    if (st->cylscale <= 0.0f) return;
    if (st->handles[kSubObjRing] == 0) return;
    if (st->length < 1.0f) return;

    const int32_t ring_count_raw = int32_t(st->length / 100.0f);
    const int32_t ring_count = (ring_count_raw > 1) ? ring_count_raw : 1;

    for (int32_t i = 0; i < ring_count; ++i)
    {
        // :8563 — position = −((ringout + 100*i) mod length)
        float ring_y = -std::fmod(st->ringout + 100.0f * float(i), st->length);

        d3d::Matrix local;
        d3d::MatrixIdentity(local);                           // D3DMATRIXClear (:8550)
        d3d::MatrixScale(local,
                         st->cylscale / 2.0f,               // :8553 scl.x
                         st->cylscale / 2.0f,               // :8554 scl.y
                         1.0f);                              // :8555 scl.z=1.0
        d3d::MatrixRotateX(local, kM_PI / 2.0f);             // :8558 RotateX(+π/2)
        d3d::MatrixRotateY(local, st->spiralang);            // :8559 RotateY(spiralang)
        d3d::MatrixRotateZ(local, 0.0f);                     // :8560 RotateZ(0)
        d3d::MatrixTranslate(local, 0.0f, ring_y, 0.0f);    // :8562-8564 Translate

        d3d::RenderObject(*Renderer, st->handles[kSubObjRing], local,
                          st->diffuse[kSubObjRing], st->emissive[kSubObjRing],
                          blend, &inst_world, st->base_pos[2]);
    }
}

// ==========================================================================
// RenderGlowSpheres — 2× end-cap glow billboards (box01, sub-object 4).
//
// Source: effect_old.cpp:8573-8600.
//
// **CHANGE FROM SNAPSHOT TRANSCRIPTION**: the snapshot draws box01 as a
// WorldMesh with a baked screen-facing rotation sequence:
//   Scale → RotateX(−π/2) → RotateX(−π/6) → RotateZ(−π/4) →
//   RotateZ(−facing) → Translate(0, −length*i, 0)
//
// That sequence is the 1998 D3D pipeline's way of fake-screen-aligning
// a flat 4-vert quad — the result reads as a glow sphere when the camera
// happens to be at the authored angle, and reads as a flat plane edge-on
// otherwise. Per WAVE_1_LESSONS §2 orientation chooser, a flat textured
// quad meant to read as a glow = ScreenAligned billboard.
//
// We drop the baked rotation and translate-only, submitting via the
// d3d::Orientation::ScreenAligned path → SubmitFxBillboard. The renderer
// camera-aligns automatically every frame; the glow now reads round at
// every viewing angle.
//
// Position semantics preserved: caster end at (0,0,0), target end at
// (0,−length,0) in local space, projected through inst_world. Size from
// snapshot's spherescale[i]/2 × box01 base extent (Gotcha #3).
//
// (Forensics §13.3: the broken GetFace(f*360) at snapshot :8590 was the
// `-RotateZ(facing)` for the legacy WorldMesh path — now obsolete since
// the billboard is automatically camera-aligned.)
//
// Only rendered when subspell>0 (:8574) — always true (subspell=3).
// ==========================================================================
static void RenderGlowSpheres(State* st, const d3d::Matrix& inst_world)
{
    if (st->box01_texture == kInvalidTexture) return;

    // Shared obj setup — pos varies per glow, scale varies per index.
    d3d::Obj obj;
    obj.objnum = kSubObjBox01;
    std::memcpy(obj.diffuse,  st->diffuse[kSubObjBox01],  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive[kSubObjBox01], sizeof(obj.emissive));

    // Encode the box01 UV sub-rect into lverts[0..3] (Flag_Verts → atlas cell).
    const float u_lo = st->box01_uv_rect[0];
    const float v_lo = st->box01_uv_rect[1];
    const float u_hi = u_lo + st->box01_uv_rect[2];
    const float v_hi = v_lo + st->box01_uv_rect[3];
    obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
    obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
    obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
    obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

    for (int32_t i = 0; i < 2; ++i)
    {
        if (st->spherescale[i] <= 0.0f) continue;  // guard negative scale

        // Size: snapshot's scl uniform = spherescale[i]/2 (:8584).
        // Multiply by kBox01GlowBaseSizeWu (per-consumer base — glow caps need
        // to be big enough to hide the bare beam termination geometry per the
        // reference frames; user feedback round 5).
        const float size = (st->spherescale[i] / 2.0f) * kBox01GlowBaseSizeWu;

        // Position: caster end (i=0) at origin, target end (i=1) at y=−length.
        // (Snapshot :8595 y=−length*i; same here.)
        obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
        obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
        obj.pos[0] = 0.0f;
        obj.pos[1] = -st->length * float(i);
        obj.pos[2] = 0.0f;

        d3d::RenderObject(*Renderer, obj,
                          d3d::Orientation::ScreenAligned,
                          st->box01_texture,
                          &inst_world);
    }
}

// ==========================================================================
// RenderFrostParticles — frost spray at target end (box01, sub-object 4).
//
// Source: effect_old.cpp:8602-8628.
//
// Same architecture as RenderGlowSpheres — box01 is a flat 4-vert quad
// rendered as a ScreenAligned billboard. The snapshot's matrix sequence
// (Scale × RotateX(−π/2) × RotateX(−π/6) × RotateZ(−π/4) × RotateZ(−facing)
// × Translate) is the legacy fake-billboard hack; we use the renderer's
// true camera-alignment path instead.
//
// Per-particle (round-5 — frost particles distributed along entire beam,
// NOT clustered at target end per snapshot:8623):
//   pos = (p[i].x, p[i].y + base_y[i], p[i].z)
//     where base_y[i] is a per-particle anchor in [-length, 0] seeded at
//     Spawn + on each respawn (FrostParticle struct comment). The snapshot's
//     p[i].y is the ±10 wu micro-jitter from Initialize, preserved as
//     drifting fine motion on top of the stable beam-axis anchor.
//   size = s[i] × kBox01FrostBaseSizeWu  — snapshot scl × authored extent (sparkle)
// Active particles only (delay <= 0).
// ==========================================================================
static void RenderFrostParticles(State* st, const d3d::Matrix& inst_world)
{
    if (st->box01_texture == kInvalidTexture) return;

    // Shared obj setup — UV sub-rect + material colors fixed; per-particle
    // varies pos + size only.
    d3d::Obj obj;
    obj.objnum = kSubObjBox01;
    std::memcpy(obj.diffuse,  st->diffuse[kSubObjBox01],  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive[kSubObjBox01], sizeof(obj.emissive));
    const float u_lo = st->box01_uv_rect[0];
    const float v_lo = st->box01_uv_rect[1];
    const float u_hi = u_lo + st->box01_uv_rect[2];
    const float v_hi = v_lo + st->box01_uv_rect[3];
    obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
    obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
    obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
    obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

    for (int32_t i = 0; i < kMaxFrostParticles; ++i)
    {
        if (st->frost[i].delay > 0.0f) continue;  // :8605 inactive
        if (st->frost[i].scl <= 0.0f)  continue;  // guard

        const float size = st->frost[i].scl * kBox01FrostBaseSizeWu;

        obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
        obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
        obj.pos[0] = st->frost[i].pos[0];                            // :8622
        // Round-5: per-particle beam-axis anchor instead of snapshot's
        // tight cluster at target end. base_y in [-length, 0].
        obj.pos[1] = st->frost[i].pos[1] + st->frost[i].base_y;
        obj.pos[2] = st->frost[i].pos[2];                            // :8624

        d3d::RenderObject(*Renderer, obj,
                          d3d::Orientation::ScreenAligned,
                          st->box01_texture,
                          &inst_world);
    }
}

// ==========================================================================
// RenderSnowFlurry — orbiting snow flurry (box01, sub-object 4).
//
// Source: effect_old.cpp:8630-8657.
//
// Same architecture as glow/frost — box01 is a flat 4-vert quad rendered
// as a ScreenAligned billboard, not a WorldMesh draw. Legacy fake-billboard
// rotation hack dropped; renderer handles camera alignment.
//
// Per-particle polar position around the beam axis:
//   pos = (r[i]*cos(th[i]), r[i]*sin(th[i]) − cy_if_past_2GD, h[i])
//   size = sz[i] × kBox01SnowBaseSizeWu
//
// subspell>2 branch — always active (subspell forced=3).
// ==========================================================================
static void RenderSnowFlurry(State* st, const d3d::Matrix& inst_world)
{
    if (st->box01_texture == kInvalidTexture) return;

    d3d::Obj obj;
    obj.objnum = kSubObjBox01;
    std::memcpy(obj.diffuse,  st->diffuse[kSubObjBox01],  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive[kSubObjBox01], sizeof(obj.emissive));
    const float u_lo = st->box01_uv_rect[0];
    const float v_lo = st->box01_uv_rect[1];
    const float u_hi = u_lo + st->box01_uv_rect[2];
    const float v_hi = v_lo + st->box01_uv_rect[3];
    obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
    obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
    obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
    obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

    for (int32_t i = 0; i < kMaxSnowParticles; ++i)
    {
        if (st->snow[i].sz <= 0.0f) continue;

        const float sx = st->snow[i].r * std::cos(st->snow[i].th);  // :8649
        float       sy = st->snow[i].r * std::sin(st->snow[i].th);  // :8650
        if (st->frameon > kGrowDuration * 2)
            sy -= st->cy;                                             // :8651-8652
        const float pz = st->snow[i].h;                              // :8653

        const float size = st->snow[i].sz * kBox01SnowBaseSizeWu;

        obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
        obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
        obj.pos[0] = sx;
        obj.pos[1] = sy;
        obj.pos[2] = pz;

        d3d::RenderObject(*Renderer, obj,
                          d3d::Orientation::ScreenAligned,
                          st->box01_texture,
                          &inst_world);
    }
}

// ==========================================================================
// Render — full composite draw. Transcribed from effect_old.cpp:8464-8688.
//
// Called from submit_world callback. Per WAVE_1_LESSONS §4.1, ScreenAligned
// billboards work from either submit hook, so combining WorldMesh + billboard
// draws here is safe — and necessary, because the harness wrapper at vfxtest.cpp
// `IceBoltShimSubmit` only calls Tick (not Submit), so any draws moved into a
// separate Submit path would not render. (See .vfx_logs/icebolt.md iter3.)
//
// The beam runs along local −Y. inst_world translates to base_pos in world
// space; beam facing is encoded in each WorldMesh sub-object's local matrix.
// ScreenAligned billboards use their pos[xyz] within the translated frame
// (the renderer's camera-alignment math handles facing).
// ==========================================================================

// LazyBindBox01Texture — Gotcha #1 lazy binding for the box01 atlas texture.
// box01 → texture index 1 per manifest.txt:25 (texslot=1).
//
// The texture is registered during T3DImagery::LoadImagery (via AddTexture →
// LoadTexture → Renderer->RegisterTextureAsset, 3dimage.cpp:1758), so the
// handle is usually valid at Spawn time. But the I3D loader's upload pass
// is asynchronous in some configurations — defer until first Render anyway.
static void LazyBindBox01Texture(State* st)
{
    if (st->box01_texture != kInvalidTexture) return;
    if (!st->imagery) return;
    if (st->imagery->NumTextures() > st->box01_tex_index)
    {
        S3DTex tex = {};
        st->imagery->GetTexture(st->box01_tex_index, &tex);
        st->box01_texture = tex.htexture;
    }
}

static void Render(State* st)
{
    if (!Renderer || !st->alive) return;
    if (!st->imagery) return;

    LazyBindBox01Texture(st);

    // SaveBlendState / RestoreBlendState (RAII) (:8466, :8685)
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();  // Alpha (MODULATE) — forensics §7; NOT additive
    const d3d::BlendMode blend = d3d::CurrentBlend();

    // inst_world: translates from local-effect space to world space.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    // -----------------------------------------------------------------------
    // WorldMesh draws — beam cylinders + spirals + rings.
    // -----------------------------------------------------------------------
    RenderBeamCylinders(st, inst_world, blend);
    RenderSpirals(st, inst_world, blend);

    // Ring slide diagnostic — log count + first 3 ring_y values on first render.
    // If user reports "stationary single ring", we need concrete data: ring_count,
    // ringout value, and individual ring_y positions. Log once per spawn for sanity.
    int32_t rings_drawn = 0;
    int32_t ring_count_used = 0;
    float   ring_y_first[3] = {0.0f, 0.0f, 0.0f};
    if (st->frameon < kGrowDuration * 3 && st->cylscale > 0.0f
        && st->handles[kSubObjRing] != 0 && st->length >= 1.0f)
    {
        const int32_t ring_count_raw = int32_t(st->length / 100.0f);
        ring_count_used = (ring_count_raw > 1) ? ring_count_raw : 1;
        for (int32_t i = 0; i < ring_count_used && i < 3; ++i)
            ring_y_first[i] = -std::fmod(st->ringout + 100.0f * float(i), st->length);
        rings_drawn = ring_count_used;
    }
    RenderRings(st, inst_world, blend);

    // -----------------------------------------------------------------------
    // ScreenAligned billboard draws — glow end-caps + frost + snow flurry.
    // (All three use the box01 sub-object as a camera-facing sprite quad.)
    // Per WAVE_1_LESSONS §4.1, billboard submits from submit_world are OK
    // (they route through the FxBillboard queue which BeginTilePass doesn't
    // clear, unlike the helper_mesh queue).
    // -----------------------------------------------------------------------
    int32_t glow_drawn = 0, frost_drawn = 0, snow_drawn = 0;
    if (st->box01_texture != kInvalidTexture)
    {
        // Tally draw counts as we go for the diagnostic line below.
        // (Each render fn returns void; we count locally what would be drawn
        // by replicating the per-call gating logic. Cheap and traceable.)
        for (int32_t i = 0; i < 2; ++i)
            if (st->spherescale[i] > 0.0f) ++glow_drawn;
        for (int32_t i = 0; i < kMaxFrostParticles; ++i)
            if (st->frost[i].delay <= 0.0f && st->frost[i].scl > 0.0f) ++frost_drawn;
        for (int32_t i = 0; i < kMaxSnowParticles; ++i)
            if (st->snow[i].sz > 0.0f) ++snow_drawn;

        RenderGlowSpheres(st, inst_world);
        RenderFrostParticles(st, inst_world);
        RenderSnowFlurry(st, inst_world);
    }

    // -----------------------------------------------------------------------
    // Diagnostic: log first render state with concrete draw counts.
    // Tells us in the user's next run whether each layer is actually
    // submitting, whether box01_texture resolved, and what ringout is doing.
    // -----------------------------------------------------------------------
    if (!st->first_render_logged)
    {
        log_info("[icebolt-shim] first render: frameon=%d length=%.0f "
                 "cylscale=%.2f spherescale=(%.2f,%.2f) spiralscale=%.2f "
                 "ringout=%.2f ring_count=%d rings_drawn=%d "
                 "ring_y[0..2]=(%.1f,%.1f,%.1f) "
                 "glow_drawn=%d frost_drawn=%d snow_drawn=%d "
                 "handles=[%u,%u,%u,%u,_box01_SA_,%u,%u] "
                 "box01_tex=%u box01_uv=(%.3f,%.3f %.3f,%.3f) box01_base=%.1f",
                 st->frameon, double(st->length),
                 double(st->cylscale),
                 double(st->spherescale[0]), double(st->spherescale[1]),
                 double(st->spiralscale),
                 double(st->ringout), ring_count_used, rings_drawn,
                 double(ring_y_first[0]), double(ring_y_first[1]), double(ring_y_first[2]),
                 glow_drawn, frost_drawn, snow_drawn,
                 st->handles[0], st->handles[1], st->handles[2],
                 st->handles[3], st->handles[5], st->handles[6],
                 st->box01_texture,
                 double(st->box01_uv_rect[0]), double(st->box01_uv_rect[1]),
                 double(st->box01_uv_rect[2]), double(st->box01_uv_rect[3]),
                 double(kBox01GlowBaseSizeWu));
        st->first_render_logged = true;
    }

    // Second diagnostic at frameon=5 (mid-GROW) and frameon=15 (post-DAMAGE
    // beat, in shrink phase) — confirms ringout is advancing and rings are
    // still being drawn. (Triggered by st->second_log_frame which we bump
    // each time we log.)
    const int32_t log_frame = st->frameon;
    if ((log_frame == 5 && st->second_log_done == 0) ||
        (log_frame == 15 && st->second_log_done == 1))
    {
        log_info("[icebolt-shim] frame=%d update: ringout=%.2f ring_count=%d "
                 "rings_drawn=%d ring_y[0..2]=(%.1f,%.1f,%.1f) "
                 "cylscale=%.2f spherescale=(%.2f,%.2f) "
                 "glow_drawn=%d frost_drawn=%d snow_drawn=%d",
                 log_frame,
                 double(st->ringout), ring_count_used, rings_drawn,
                 double(ring_y_first[0]), double(ring_y_first[1]), double(ring_y_first[2]),
                 double(st->cylscale),
                 double(st->spherescale[0]), double(st->spherescale[1]),
                 glow_drawn, frost_drawn, snow_drawn);
        st->second_log_done++;
    }
}

// ==========================================================================
// Spawn — load icebolt.I3D, register sub-object meshes, run Initialize.
// ==========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();

    // base_pos = origin.z raised by kOriginZLift (effect_old.cpp:8022-8026)
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z) + kOriginZLift;

    // Load Magic\icebolt.I3D (forensics §4 — "IceBolt" registry name, Class.Def:2055)
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[icebolt-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[icebolt-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();

    if (num_obj < kTotalSubObjs)
    {
        log_warn("[icebolt-shim] '%s' has %d sub-objects, expected %d",
                 kImageryPath, num_obj, kTotalSubObjs);
    }

    // Register mesh handles + load materials for each of the 7 sub-objects.
    // Sub-objects 0..3, 5, 6 are WorldMesh draws (cylinders/rings/spirals).
    // Sub-object 4 (box01) is a ScreenAligned billboard — we still load its
    // material for diffuse color but skip RegisterSubMesh (not needed for
    // the SubmitFxBillboard path).
    //
    // LoadMaterial returns authored D3DMATERIAL colors; emissive zeroed to
    // prevent white-wash (WAVE_1_LESSONS §1.1 — emissive is additive in shader;
    // forensics §7 says "no material zeroing" but that refers to the 1998 SetMaterial
    // path; the shader convention requires zero here for correct unlit pass-through).
    const int32_t reg_count = (kTotalSubObjs < num_obj) ? kTotalSubObjs : num_obj;
    for (int32_t i = 0; i < reg_count; ++i)
    {
        if (i != kSubObjBox01)
        {
            st->handles[i] = d3d::RegisterSubMesh(*Renderer, img3d, i, num_tex);
            if (st->handles[i] == 0)
            {
                log_warn("[icebolt-shim] sub-obj %d ('%s') mesh registration failed",
                         i, kImageryPath);
            }
        }
        d3d::LoadMaterial(img3d, i, st->diffuse[i], st->emissive[i]);
        // Zero emissive — see WAVE_1_LESSONS §1.1.
        st->emissive[i][0] = 0.0f;
        st->emissive[i][1] = 0.0f;
        st->emissive[i][2] = 0.0f;
    }

    // Resolve the box01 UV sub-rect from its authored vertex UVs. box01 is
    // a 4-vert quad (manifest.txt:25) so its UVs span one rect of the
    // texture[1] atlas — typically (0,0,1,1) since the texture is dedicated
    // to this sub-object, but we read the authored values to be safe (Gotcha #2).
    if (num_obj > kSubObjBox01)
    {
        const int32_t nv = img3d->NumObjVerts(kSubObjBox01);
        if (nv > 0)
        {
            std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
            img3d->GetObjVerts(kSubObjBox01, vbuf.data(), 0, 0,
                               ERender3DVertex::Vertex);
            float minu = vbuf[0].tu, maxu = vbuf[0].tu;
            float minv = vbuf[0].tv, maxv = vbuf[0].tv;
            for (int32_t i = 1; i < nv; ++i)
            {
                if (vbuf[i].tu < minu) minu = vbuf[i].tu;
                if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
                if (vbuf[i].tv < minv) minv = vbuf[i].tv;
                if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
            }
            st->box01_uv_rect[0] = minu;
            st->box01_uv_rect[1] = minv;
            st->box01_uv_rect[2] = maxu - minu;
            st->box01_uv_rect[3] = maxv - minv;
            // If the asset has degenerate (all-zero) UVs, fall back to full
            // (0..1) rect so the billboard isn't a black sample.
            if (st->box01_uv_rect[2] <= 0.0f || st->box01_uv_rect[3] <= 0.0f)
            {
                st->box01_uv_rect[0] = 0.0f;
                st->box01_uv_rect[1] = 0.0f;
                st->box01_uv_rect[2] = 1.0f;
                st->box01_uv_rect[3] = 1.0f;
                log_info("[icebolt-shim] box01 has degenerate UVs; "
                         "using full (0,0,1,1) rect");
            }
        }
    }

    // Resolve box01's texture index via texfaces[] walk. Per the manifest
    // box01 has texslot=1 (1-based texfaces slot) which maps to texture
    // index 0 (the glow PNG). ResolveSubObjTextureSlot returns the 0-based
    // texture index by walking the sub-object's texfaces[1..8] entries.
    // Fall back to 0 (first texture) if resolution fails — for icebolt this
    // is the correct fallback since texture[0] IS the glow image.
    {
        const int32_t slot = ResolveSubObjTextureSlot(img3d, kSubObjBox01);
        st->box01_tex_index = (slot >= 0 && slot < num_tex) ? slot : 0;
        // Bind the texture handle eagerly (T3DImagery::LoadImagery already
        // ran AddTexture/LoadTexture which uploads via Renderer->RegisterTextureAsset
        // — see 3dimage.cpp:1758 — so the handle should be valid at this point).
        if (st->box01_tex_index < num_tex)
        {
            S3DTex tex = {};
            img3d->GetTexture(st->box01_tex_index, &tex);
            st->box01_texture = tex.htexture;
        }
        log_info("[icebolt-shim] box01 sub=%d -> resolved texslot=%d (manifest "
                 "raw slot=%d) -> tex_idx=%d handle=%u",
                 kSubObjBox01, slot, slot + 1, st->box01_tex_index, st->box01_texture);
    }

    // Run Initialize (snapshot effect_old.cpp:8015-8135)
    Initialize(st);

    log_info("[icebolt-shim] spawned at (%d,%d,%d) "
             "base_pos=(%.0f,%.0f,%.0f) "
             "num_obj=%d num_tex=%d "
             "test_length=%.0f angle=%d face_rad=%.3f "
             "glow_base=%.1f frost_base=%.1f snow_base=%.1f "
             "frost_base_y[0..4]=(%.0f,%.0f,%.0f,%.0f,%.0f)",
             origin.x, origin.y, origin.z,
             double(st->base_pos[0]), double(st->base_pos[1]), double(st->base_pos[2]),
             num_obj, num_tex,
             double(st->length), st->angle, double(st->face_rad),
             double(kBox01GlowBaseSizeWu), double(kBox01FrostBaseSizeWu),
             double(kBox01SnowBaseSizeWu),
             double(st->frost[0].base_y), double(st->frost[1].base_y),
             double(st->frost[2].base_y), double(st->frost[3].base_y),
             double(st->frost[4].base_y));

    st->spawn_logged = true;
    return st;
}

// ==========================================================================
// Public harness entry points.
// ==========================================================================

// Tick — drives the 24 Hz accumulator; calls Animate.
// Called from harness submit callback (runs every render frame).
void Tick(State* st)
{
    if (st && st->alive)
        Animate(st, TTime::DeltaTime() * 1000.0);
}

// SubmitWorld — called from harness submit_world callback (BeginTilePass scope).
// Drives the full composite Render: WorldMesh cylinders/rings/spirals AND
// ScreenAligned billboard glow/frost/snow. Per WAVE_1_LESSONS §4.1, billboard
// submits from submit_world are safe (they route through the FxBillboard
// queue, which BeginTilePass doesn't clear). The harness `IceBoltShimSubmit`
// wrapper only calls Tick (not Submit), so this is the only render path that
// fires; combining everything here is the correct architecture given the
// orchestrator's wiring.
void SubmitWorld(State* st)
{
    if (st && st->alive)
        Render(st);
}

// Submit — no-op stub. The harness's `IceBoltShimSubmit` wrapper at
// vfxtest.cpp:3917 calls `Tick(state)` only and ignores Submit; all draws
// are routed through SubmitWorld. Kept for harness symmetry / future wiring.
void Submit(State* /*st*/) {}

// IsAlive — queried by harness auto-respawn wrapper (WAVE_1_LESSONS §4.2).
bool IsAlive(const State* st)
{
    return st && st->alive;
}

void Destroy(State* st) { delete st; }

} // namespace icebolt_shim
