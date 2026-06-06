// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fireflash.cpp - TFireFlashAnimator port via the d3d::* shim          *
// *                                                                       *
// *  Fourth SHIM VALIDATION PORT. First port to exercise the full         *
// *  explicit-matrix (OBJ3D_MATRIX) path with WorldMesh orientation —    *
// *  fixed-tilt world-axis rotation, per-particle scale + translate,      *
// *  additive blend, two sub-objects, 3-phase state machine.              *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/F-FIREFLASH_TFireFlashAnimator.md *
// *  Snapshot Initialize: effect_old.cpp:2113-2162                        *
// *  Snapshot Animate:    effect_old.cpp:2164-2390                        *
// *  Snapshot Render:     effect_old.cpp:2392-2479                        *
// *                                                                       *
// *  Retail fidelity: retail-partial.                                     *
// *  Retail-confirmed: registration name "FireFlash", animator name       *
// *    "FireFlash", lifetime cap 100 frames, particle pool ~150,          *
// *    asset Magic\fireflash.I3D + both sub-objects, audio key            *
// *    "FireFlash" → Sound/effects/fireflash.wav.                         *
// *  Snapshot-only: all phase choreography constants (FFLASH_F1_*,        *
// *    FFLASH_GS_*, FFLASH_RI_*), render matrix Euler triple,             *
// *    additive blend choice (corroborated by fire-spell sister family).  *
// *                                                                       *
// *  NOTES                                                                *
// *  - Audio: PLAY("FireFlash") at Initialize is SKIPPED — no audio path  *
// *    in the test harness. The shipped fireflash.wav (192,972 B) is      *
// *    confirmed; wire through the SoundPlayer registry when the audio    *
// *    subsystem is plumbed (forensics §11).                              *
// *  - Target-locked anchor: when the casting spell has a Fighting()      *
// *    target, particles render at target world pos (OBJ3D_ABSPOS). In    *
// *    the test harness there is no target — anchor at world origin.      *
// *    The target parameter of Spawn() accepts a pre-captured world pos   *
// *    so the caller can supply one when wiring the full spell path later. *
// *  - Dead RING branch (forensics §13.1): the state=RING code path is    *
// *    unreachable in the snapshot's active code. At explosion-frame 35    *
// *    the implementation uses the active path: free GSPHERE → USEME,    *
// *    spawn 150 particles as SMOKEY1 at rsize around the pivot. If       *
// *    visual-vetting against retail shows a flat orbiting ring, flip to  *
// *    the commented-out RING path.                                        *
// *  - goffset (forensics §13.2): computed in Initialize but never used.  *
// *    Omitted per forensics.                                              *
// *  - Framerate-independent: per-tick deltas from the snapshot are       *
// *    converted to per-second rates anchored at 24 Hz so the effect      *
// *    plays identically at any frame rate.                                *
// *  - Render pipeline: WorldMesh (d3d::Orientation::WorldMesh). Draw     *
// *    calls MUST come from the submit_world callback (after               *
// *    BeginTilePass). Tick runs from the regular submit callback.        *
// *                                                                       *
// *  KNOWN GAPS / BLOCKERS                                                *
// *  - Audio: PLAY("FireFlash") not hooked up (no SoundPlayer in rig).   *
// *  - RefreshZBuffer: no Z-restore path in the shim/harness for shim    *
// *    ports; the bespoke port's TEffect framework handles this.          *
// *  - No OBJ3D_ABSPOS world-space translation shim primitive: the       *
// *    FireFlash render uses OBJ3D_ABSPOS to place particles at an        *
// *    absolute world coordinate. The WorldMesh overload of RenderObject  *
// *    composes local * inst_world; we achieve the absolute world effect  *
// *    by building the full world-space matrix directly (no inst_world)   *
// *    — equivalent when inst_world = identity. In test harness the       *
// *    anchor is world origin so inst_world=identity is correct.         *
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

namespace fireflash_shim {

// =========================================================================
// Constants — every value cites forensics §3 with retail/snapshot marker.
// =========================================================================

// --- lifecycle (retail-confirmed) ---
constexpr int32_t kNomoreOfThisEffect = 100;  // effect_old.cpp:2016, retail: cls_0x5a9194.cpp:89
constexpr int32_t kNumParticles       = 150;  // effect_old.cpp:2017, retail: layout match

// --- particle states ---
constexpr int32_t kStateUseme   = 0;  // effect_old.cpp:2020
constexpr int32_t kStateSmokey1 = 1;  // effect_old.cpp:2021
constexpr int32_t kStateGsphere = 2;  // effect_old.cpp:2022
constexpr int32_t kStateRing    = 3;  // effect_old.cpp:2023 — UNREACHABLE in active code

// --- Phase 1: rising flame column (snapshot-only) ---
constexpr int32_t kF1On          = 0;      // effect_old.cpp:2026
constexpr int32_t kF1Off         = 25;     // effect_old.cpp:2027
constexpr float   kF1MaxDist     = 15.0f;  // effect_old.cpp:2028 (wu, XY disc radius)
constexpr int32_t kF1FadeMin     = 20;     // effect_old.cpp:2029 (frames, min particle life)
constexpr int32_t kF1MaxFade     = 25;     // effect_old.cpp:2030 (frames, max life add)
constexpr float   kF1FadePct     = 20.0f;  // effect_old.cpp:2031 (% of life before size-decay)
constexpr float   kF1Bottom      = 1.0f;   // effect_old.cpp:2032 (wu, spawn z)
constexpr float   kF1ColorPct    = 70.0f;  // effect_old.cpp:2033 (% fade elapsed → switch to smoke sub-obj)
constexpr float   kF1MinSize     = 0.05f;  // effect_old.cpp:2034 (minimum scale at end-of-life)

// Spawn-rate table (snapshot-only):
constexpr int32_t kF1Time1       = 5;      // effect_old.cpp:2037
constexpr int32_t kF1Add1        = 4;      // effect_old.cpp:2038 (frames 0..4)
constexpr float   kF1MainScale1  = 0.3f;   // effect_old.cpp:2039
constexpr int32_t kF1Time2       = 15;     // effect_old.cpp:2040
constexpr int32_t kF1Add2        = 7;      // effect_old.cpp:2041 (frames 5..14)
constexpr float   kF1MainScale2  = 0.3f;   // effect_old.cpp:2042
constexpr int32_t kF1Time3       = 16;     // effect_old.cpp:2043
constexpr int32_t kF1Add3        = 7;      // effect_old.cpp:2044 (frame 15)
constexpr float   kF1MainScale3  = 0.3f;   // effect_old.cpp:2045
constexpr int32_t kF1Time4       = 25;     // effect_old.cpp:2046 (= kF1Off)
constexpr int32_t kF1Add4        = 2;      // effect_old.cpp:2047 (frames 16..24)
constexpr float   kF1MainScale4  = 0.3f;   // effect_old.cpp:2048

// --- Phase 2: glowing sphere (snapshot-only) ---
constexpr int32_t kGsOn           = 26;    // effect_old.cpp:2052
constexpr int32_t kGsNumParticles = 75;    // effect_old.cpp:2053
constexpr float   kGsMinSize1     = 1.0f;  // effect_old.cpp:2054 (initial gsize)
constexpr float   kGsMaxSize1     = 20.0f; // effect_old.cpp:2055 (max gsize)
constexpr float   kGsSizeVel1     = 2.0f;  // effect_old.cpp:2056 (wu/tick growth)
constexpr float   kGsSizeToScale  = 0.01f; // effect_old.cpp:2057 (per-particle scale = gsize * 0.01)
constexpr float   kGsMinAngVel    = 0.12f; // effect_old.cpp:2058 (rad/tick per axis)
// gsphere pivot = (0, 0, 30): effect_old.cpp:2130-2132

// --- Phase 3: explosion / ring burst (snapshot-only) ---
constexpr int32_t kExplosion      = 35;    // effect_old.cpp:2050
constexpr int32_t kRiNumParticles = 150;   // effect_old.cpp:2062 (= kNumParticles)
constexpr float   kRiMaxSize      = 300.0f;// effect_old.cpp:2063 (ring kill radius)
constexpr float   kRiSizeVel1     = 8.0f;  // effect_old.cpp:2064 (wu/tick growth)
// kRiMinAngVel = 0.3f; RING branch unreachable, not used in active code.

// --- Render matrix Euler angles (snapshot-only, per forensics §7) ---
constexpr float kRotX1 = -(float(M_PI) * 2.0f / 3.0f); // effect_old.cpp:2455  -120° = -2π/3
constexpr float kRotZ1 = -(float(M_PI) / 4.0f);         // effect_old.cpp:2456  -45°  = -π/4
// RotZ #2 = -(facing / 256.0f * 2π), built per-instance (facing captured at Initialize).
// Note: snapshot has typo '256.0ff'; intent is 256.0f — forensics §13.3.

// --- Asset ---
constexpr const char* kImageryPath = "Magic\\fireflash.I3D";  // Class.Def:2048
constexpr int32_t kSubObjSmoke  = 0;  // smoke  = GetObject(0), forensics §4
constexpr int32_t kSubObjSmoke01= 1;  // smoke01= GetObject(1), forensics §4
constexpr int32_t kNumSubObjs   = 2;

// --- Framerate-independent conversion ---
// Snapshot tick rate = 24 Hz (per feedback-framerate-independent-anim).
// Per-tick deltas are multiplied by (deltaTime * 24) to get time-based motion.
constexpr float kTicksPerSec = 24.0f;

// --- Base size for billboard/mesh scale ---
// The snapshot's particle scale (0.05 .. 0.3 for SMOKEY1; gsize*0.01 for GSPHERE)
// is a multiplier on the authored I3D quad geometry. For the mesh-based render
// we pass scale directly to the matrix (Scale(s,s,s)); the authored verts of the
// fireflash.I3D sub-objects define the base geometry extents.
// No additional base size multiplier needed — the world-space mesh render
// uses the authored vertex positions scaled by the particle's scale factor.
// This differs from the ScreenAligned billboard path (gotcha #3) because
// here we build the full matrix rather than feeding a "size_wu" to the
// billboard renderer.

// =========================================================================
// Helper: inclusive-range random, matching snapshot random(lo, hi) semantics.
// =========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// =========================================================================
// ConvertToVector — matches snapshot's ConvertToVector(angle8, dist, s3dp).
// angle is a byte-angle in [0,255] (256 steps = 360°).
// Returns (x, y) via s3dp; z is unchanged.
// =========================================================================
static void ConvertToVector(int32_t angle8, float dist, float out_xy[2])
{
    const float rad = float(angle8) * (float(M_PI) * 2.0f / 256.0f);
    out_xy[0] = std::cos(rad) * dist;
    out_xy[1] = std::sin(rad) * dist;
}

// =========================================================================
// Per-particle state — matches snapshot FFLASH_PARTICLE struct.
// effect_old.cpp:2070-2084.
// =========================================================================
struct Particle
{
    int32_t state    = kStateUseme;  // :2080
    float   pos[3]  = {0, 0, 0};    // :2071
    float   vel[3]  = {0, 0, 0};    // :2072  (only vel.z used for SMOKEY1)
    float   angle[3]= {0, 0, 0};    // :2073  (Euler angles for GSPHERE/RING, radians)
    float   angvel[3]={0, 0, 0};    // :2074  (angular velocity for GSPHERE/RING)
    float   pivot[3]= {0, 0, 0};    // :2075  (orbit pivot, set each tick to gsphere)
    float   dist    = 0.0f;         // :2076  (orbit radius for GSPHERE/RING)
    float   scale   = 0.0f;         // :2077  (per-particle render scale)
    int32_t life    = 0;            // :2078  (countdown ticks remaining)
    int32_t startfade = 0;          // :2079  (tick count at which size-decay starts)
    int32_t stopfade  = 0;          // (same as life at spawn — end of fade window)
};

// =========================================================================
// Per-instance state.
// =========================================================================
struct State
{
    Particle sm[kNumParticles];     // effect_old.cpp:2097

    float   gsphere[3] = {0, 0, 30}; // fixed pivot, effect_old.cpp:2130-2132
    float   gsize      = kGsMinSize1; // effect_old.cpp:2133
    float   rsize      = 0.0f;       // ring radius, set at explosion frame
    float   facing     = 0.0f;       // byte-angle 0..255, captured at Initialize
    int32_t framenum   = 0;          // effect_old.cpp:2121

    // mainscale tracks the current SMOKEY1 spawn-slice scale (0.3 in all
    // slices, kept as a field for render access — effect_old.cpp:2415-2417).
    float   mainscale  = 0.3f;

    // Target anchor: when a target position is available (from the casting
    // spell's Fighting() target), particles render at that world pos via
    // OBJ3D_ABSPOS. In the test harness there is no target — anchor is (0,0,0).
    // effect_old.cpp:2143-2447.
    bool    has_target = false;
    float   origin[3]  = {0, 0, 0};  // world-space render anchor

    float   base_pos[3]= {0, 0, 0};  // effect instance world position (spawn origin)
    bool    alive      = true;

    // Time accumulator for framerate-independent simulation.
    // Each real-time second we advance by 24 nominal ticks.
    float   tick_accum = 0.0f;

    // Resolved asset data.
    T3DImagery*    imagery    = nullptr;
    MeshHandle     mesh[kNumSubObjs]  = {0, 0};   // registered mesh handles
    float          diffuse[kNumSubObjs][4]  = {{1,1,1,1},{1,1,1,1}};
    float          emissive[kNumSubObjs][4] = {{0,0,0,1},{0,0,0,1}};

    // Diagnostic: log first Submit call once.
    int32_t spawn_log_done    = 0;
};

// =========================================================================
// Initialize — transcribed from effect_old.cpp:2113-2162.
// =========================================================================
static void Initialize(State* st)
{
    // effect_old.cpp:2121
    st->framenum = 0;

    // effect_old.cpp:2124 — facing byte-angle from caster.
    // In the test harness, facing=0 (caster faces along +X per convention).
    st->facing = 0.0f;

    // effect_old.cpp:2130-2132 — fixed pivot (goffset is computed but unused, §13.2)
    st->gsphere[0] = 0.0f;
    st->gsphere[1] = 0.0f;
    st->gsphere[2] = 30.0f;

    // effect_old.cpp:2133
    st->gsize = kGsMinSize1;

    // effect_old.cpp:2135 — target. Test harness has no target → origin=(0,0,0).
    st->has_target = false;
    st->origin[0]  = 0.0f;
    st->origin[1]  = 0.0f;
    st->origin[2]  = 0.0f;

    // AUDIO: PLAY("FireFlash") — effect_old.cpp:2119.
    // SKIPPED: no audio path in test harness. The shipped fireflash.wav
    // (192,972 B, Sound/effects/fireflash.wav) is the correct asset.
    // Wire through SoundPlayer registry when audio subsystem is plumbed.

    // effect_old.cpp:2148 — zero all particles
    std::memset(st->sm, 0, sizeof(st->sm));

    // effect_old.cpp:2151-2160 — seed slots 0..74 as GSPHERE
    for (int32_t i = 0; i < kGsNumParticles; ++i)
    {
        // effect_old.cpp:2153-2155 — random Euler angles in [0, 2π)
        st->sm[i].angle[0] = float(snap_random(0, 359)) / 360.0f * (float(M_PI) * 2.0f);
        st->sm[i].angle[1] = float(snap_random(0, 359)) / 360.0f * (float(M_PI) * 2.0f);
        st->sm[i].angle[2] = float(snap_random(0, 359)) / 360.0f * (float(M_PI) * 2.0f);
        // effect_old.cpp:2156-2158 — uniform angular velocity
        st->sm[i].angvel[0] = kGsMinAngVel;
        st->sm[i].angvel[1] = kGsMinAngVel;
        st->sm[i].angvel[2] = kGsMinAngVel;
        // effect_old.cpp:2160
        st->sm[i].state = kStateGsphere;
    }
    // Slots 75..149 remain kStateUseme (zeroed by memset).
}

// =========================================================================
// Animate (per-tick) — transcribed from effect_old.cpp:2164-2390.
//
// Runs once per nominal 24-Hz tick. The caller integrates real delta time
// and accumulates fractional ticks; Animate is called for each whole tick.
// =========================================================================
static void AnimateTick(State* st)
{
    // effect_old.cpp:2169
    st->framenum++;

    int32_t newringparts = 0;  // effect_old.cpp:2171

    // ─── PHASE STATE MACHINE ───────────────────────────────────────────────
    if (st->framenum >= kGsOn && st->framenum < kExplosion)
    {
        // effect_old.cpp:2174-2179 — sphere growing
        if (st->gsize < kGsMaxSize1)
            st->gsize += kGsSizeVel1;
        else
            st->gsize = kGsMaxSize1;
    }
    else if (st->framenum == kExplosion)
    {
        // effect_old.cpp:2181 — explosion snap (one tick)
        // Free all GSPHERE → USEME
        for (int32_t i = 0; i < kNumParticles; ++i)
        {
            if (st->sm[i].state == kStateGsphere)     // effect_old.cpp:2216
                st->sm[i].state = kStateUseme;         // effect_old.cpp:2217
        }
        st->rsize    = st->gsize;                      // effect_old.cpp:2221
        newringparts = kRiNumParticles;                // effect_old.cpp:2223
    }
    else if (st->framenum > kExplosion)
    {
        // effect_old.cpp:2225-2232 — ring expanding
        st->rsize += kRiSizeVel1;                      // effect_old.cpp:2227
        if (st->rsize >= kRiMaxSize)
        {
            for (int32_t i = 0; i < kNumParticles; ++i)
                st->sm[i].state = kStateUseme;         // effect_old.cpp:2232
        }
    }

    // ─── SMOKEY1 SPAWN RATE TABLE (column) ─────────────────────────────────
    // effect_old.cpp:2239-2262
    int32_t newsmokey1s = 0;
    if (st->framenum >= kF1On && st->framenum < kF1Time1)
    {
        newsmokey1s    = kF1Add1;
        st->mainscale  = kF1MainScale1;
    }
    else if (st->framenum < kF1Time2)
    {
        newsmokey1s    = kF1Add2;
        st->mainscale  = kF1MainScale2;
    }
    else if (st->framenum < kF1Time3)
    {
        newsmokey1s    = kF1Add3;
        st->mainscale  = kF1MainScale3;
    }
    else if (st->framenum < kF1Time4)
    {
        newsmokey1s    = kF1Add4;
        st->mainscale  = kF1MainScale4;
    }
    // frames >= 25: newsmokey1s stays 0 (column off)

    // ─── PER-PARTICLE LOOP ─────────────────────────────────────────────────
    for (int32_t i = 0; i < kNumParticles; ++i)
    {
        Particle& p = st->sm[i];

        if (p.state == kStateUseme)  // effect_old.cpp:2266
        {
            if (newsmokey1s > 0)  // effect_old.cpp:2270
            {
                // ── Spawn SMOKEY1 (rising flame) ──────────────────────────
                const float dist = float(snap_random(0, int32_t(kF1MaxDist)));  // :2275
                const int32_t angle = snap_random(0, 255);                       // :2276
                float xy[2];
                ConvertToVector(angle, dist, xy);                                // :2277
                p.pos[0] = xy[0];                                                // :2278
                p.pos[1] = xy[1];                                                // :2279
                p.pos[2] = kF1Bottom;                                            // :2280

                // life = FADEMIN + MAXFADE * (MAXDIST - rand(0,dist)) / MAXDIST
                // effect_old.cpp:2281
                const float rand_dist = float(snap_random(0, int32_t(dist)));
                p.life       = int32_t(float(kF1FadeMin) +
                                float(kF1MaxFade) * (kF1MaxDist - rand_dist) / kF1MaxDist);

                // effect_old.cpp:2282-2283
                p.startfade  = int32_t(float(p.life) * kF1FadePct / 100.0f);
                p.stopfade   = p.life;

                // effect_old.cpp:2284 — vel.z = 2..4 wu/tick, no XY drift, no gravity
                p.vel[0] = 0.0f;
                p.vel[1] = 0.0f;
                p.vel[2] = 2.0f + float(snap_random(0, 2));

                p.scale  = st->mainscale;                                        // :2285 (via render)
                p.state  = kStateSmokey1;                                        // :2285
                newsmokey1s--;                                                   // :2287
            }
            else if (newringparts > 0)  // effect_old.cpp:2289
            {
                // ── Spawn ring particle as SMOKEY1 (active code path, §13.1) ──
                // Note: the explicit RING state path is commented-out in the
                // snapshot (effect_old.cpp:2184-2211). The active code at :2289-2305
                // spawns these as SMOKEY1 — a radial burst of rising flame particles
                // at rsize around the pivot, not an orbiting flat ring.
                const float rdist = st->rsize;                                   // :2292
                const int32_t rang = snap_random(0, 255);                        // :2293
                float rxy[2];
                ConvertToVector(rang, rdist, rxy);                               // :2294
                p.pos[0] = st->gsphere[0] + rxy[0];                             // :2295
                p.pos[1] = st->gsphere[1] + rxy[1];                             // :2296
                p.pos[2] = st->gsphere[2];                                       // :2297  (z=30, pivot height)

                // effect_old.cpp:2298-2300 (same formula as SMOKEY1 but dist=rsize)
                const float rrand_dist = float(snap_random(0, int32_t(rdist > kF1MaxDist ? kF1MaxDist : rdist)));
                p.life      = int32_t(float(kF1FadeMin) +
                               float(kF1MaxFade) * (kF1MaxDist - rrand_dist) / kF1MaxDist);
                p.startfade = int32_t(float(p.life) * kF1FadePct / 100.0f);     // :2299
                p.stopfade  = p.life;                                            // :2300

                p.vel[0] = 0.0f;
                p.vel[1] = 0.0f;
                p.vel[2] = 2.0f + float(snap_random(0, 2));                     // :2301

                p.scale  = st->mainscale;
                p.state  = kStateSmokey1;                                        // :2302
                newringparts--;                                                  // :2304
            }
        }
        else if (p.state == kStateSmokey1)  // effect_old.cpp:2307
        {
            // ── SMOKEY1 motion: rise + age ───────────────────────────────
            p.pos[2] += p.vel[2];                                                // :2311
            p.life--;                                                             // :2312
            if (p.life <= 0)                                                     // :2314
                p.state = kStateUseme;                                           // :2315
        }
        else if (p.state == kStateGsphere)  // effect_old.cpp:2317
        {
            // ── GSPHERE motion: orbit pivot on 3 axes ────────────────────
            p.pivot[0] = st->gsphere[0];                                         // :2321-2323
            p.pivot[1] = st->gsphere[1];
            p.pivot[2] = st->gsphere[2];

            p.angle[0] += p.angvel[0];                                           // :2325
            p.angle[1] += p.angvel[1];                                           // :2326
            p.angle[2] += p.angvel[2];                                           // :2327

            p.dist  = st->gsize;                                                 // :2329
            p.scale = st->gsize * kGsSizeToScale;                                // :2330

            // Compute position via RotX(angle.x) · RotY(angle.y) · RotZ(angle.z)
            // applied to (0, dist, 0), then add pivot.
            // effect_old.cpp:2333-2349
            // Build rotation matrix via d3d shim to match snapshot order.
            d3d::Matrix mat;
            d3d::MatrixIdentity(mat);
            d3d::MatrixRotateX(mat, p.angle[0]);
            d3d::MatrixRotateY(mat, p.angle[1]);
            d3d::MatrixRotateZ(mat, p.angle[2]);

            // Transform (0, dist, 0) by the rotation matrix.
            // d3dport.h stores in row-major with column-vector convention:
            //   m[row*4 + col], wp = M * vp
            //   wp[r] = sum_k M[r*4+k] * vp[k]
            // For vp = (0, dist, 0, 0):
            //   wp[0] = M[0]*0 + M[1]*dist + M[2]*0 + M[3]*0 = M[1]*dist
            //   wp[1] = M[4]*0 + M[5]*dist + M[6]*0 + M[7]*0 = M[5]*dist
            //   wp[2] = M[8]*0 + M[9]*dist + M[10]*0 + M[11]*0 = M[9]*dist
            const float vx = mat.m[1] * p.dist;   // row 0, col 1 = M[0*4+1]
            const float vy = mat.m[5] * p.dist;   // row 1, col 1 = M[1*4+1]
            const float vz = mat.m[9] * p.dist;   // row 2, col 1 = M[2*4+1]

            p.pos[0] = p.pivot[0] + vx;            // :2347-2349
            p.pos[1] = p.pivot[1] + vy;
            p.pos[2] = p.pivot[2] + vz;
        }
        // kStateRing branch (effect_old.cpp:2351-2382) is unreachable in
        // the snapshot's active code — see forensics §13.1.
    }

    // ─── DEATH ─────────────────────────────────────────────────────────────
    if (st->framenum >= kNomoreOfThisEffect)  // effect_old.cpp:2386 (retail-confirmed)
    {
        log_info("[fireflash-shim] alive=false at framenum=%d", st->framenum);
        st->alive = false;                    // effect_old.cpp:2387 → KillThisEffect()
    }
}

// =========================================================================
// Animate (delta-time entry) — integrates real time to nominal 24-Hz ticks.
// Per feedback-framerate-independent-anim: convert per-tick deltas to
// per-second rates so the effect plays identically at any frame rate.
// =========================================================================
static void Animate(State* st)
{
    if (!st->alive) return;

    // Per-second rate = 24 ticks/s. The tick accumulator carries sub-tick
    // remainder across frames. Call AnimateTick once per whole accumulated tick.
    const float delta = float(TTime::DeltaTime());
    st->tick_accum += delta * kTicksPerSec;

    while (st->tick_accum >= 1.0f)
    {
        st->tick_accum -= 1.0f;
        AnimateTick(st);
        if (!st->alive) break;
    }
}

// =========================================================================
// Render — transcribed from effect_old.cpp:2392-2479.
//
// MUST be called from the harness submit_world callback (after BeginTilePass)
// because it uses d3d::Orientation::WorldMesh → SubmitHelperMesh.
// =========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // Check mesh handles registered.
    // Note: the WorldMesh RenderObject overload embeds the texture in the
    // mesh handle (via RegisterSubMesh) — no separate texture-handle lookup
    // is needed here. The old lazy-texture guard was fetching a handle that
    // was never passed to any draw call; removed to avoid a false early-return
    // if the imagery structure is queried before upload completes.
    for (int32_t s = 0; s < kNumSubObjs; ++s)
        if (!st->mesh[s]) return;

    // Diagnostic: log first Submit call with framenum, phase, and particle count.
    if (!st->spawn_log_done)
    {
        // Count active particles for diagnostics (SMOKEY1 + GSPHERE visible + RING).
        int32_t n_visible = 0;
        for (int32_t i = 0; i < kNumParticles; ++i)
        {
            const Particle& p = st->sm[i];
            if (p.state == kStateSmokey1 && p.life > 0)        ++n_visible;
            else if (p.state == kStateGsphere && st->framenum >= kGsOn) ++n_visible;
            else if (p.state == kStateRing)                     ++n_visible;
        }
        const int32_t phase = (st->framenum >= kExplosion) ? 3
                            : (st->framenum >= kGsOn)       ? 2
                            :                                  1;
        log_info("[fireflash-shim] first submit: framenum=%d phase=%d "
                 "N particles drawn=%d alive=%d mesh=(%u,%u)",
                 st->framenum, phase, n_visible, int(st->alive),
                 st->mesh[0], st->mesh[1]);
        st->spawn_log_done = 1;
    }

    // effect_old.cpp:2397-2398 — SaveBlendState + SetAddBlendState
    d3d::BlendStateGuard blend_scope;
    d3d::SetAddBlendState();   // Additive: SRC=ONE, DST=ONE, ZWRITE=off, ZTEST=on

    // Build the render transform constants.
    // RotZ #2 angle: -(facing / 256.0f * 2π) — effect_old.cpp:2457 (typo fixed)
    const float rot_z2 = -(st->facing / 256.0f * float(M_PI) * 2.0f);

    // The combined Z rotation is RotZ(kRotZ1 + rot_z2) since both
    // post-multiply the same matrix. Split as in snapshot for clarity.

    // inst_world = identity (test harness: no instance transform; origin=(0,0,0))
    // For the OBJ3D_ABSPOS case the particle world position is already in world
    // space (particle_pos + origin), so we pass no inst_world.
    d3d::Matrix inst_identity;
    d3d::MatrixIdentity(inst_identity);

    d3d::Obj obj;

    // effect_old.cpp:2403 — per-particle loop
    for (int32_t i = 0; i < kNumParticles; ++i)
    {
        const Particle& p = st->sm[i];

        // ── PICK SUB-OBJECT based on state + life stage ──────────────────
        // effect_old.cpp:2405-2436
        int32_t sub = -1;
        float   cur_scale = p.scale;

        if (p.state == kStateSmokey1)
        {
            // effect_old.cpp:2407 — switch to smoke (GetObject(0)) once
            // (stopfade - life) >= (stopfade - startfade) * 70 / 100
            const float fade_elapsed = float(p.stopfade - p.life);
            const float fade_window  = float(p.stopfade - p.startfade);
            if (fade_window > 0.0f && fade_elapsed >= fade_window * kF1ColorPct / 100.0f)
                sub = kSubObjSmoke;    // smoke (cool)
            else
                sub = kSubObjSmoke01;  // smoke01 (hot)

            // ── PER-PARTICLE SCALE FADE ─────────────────────────────────
            // effect_old.cpp:2412-2417
            if (p.life > 0)
            {
                if (fade_elapsed >= float(p.startfade))
                {
                    // In decay window: scale = mainscale * life / fade_window + MINSIZE
                    // effect_old.cpp:2415
                    if (fade_window > 0.0f)
                        cur_scale = st->mainscale * float(p.life) / fade_window + kF1MinSize;
                    else
                        cur_scale = kF1MinSize;
                }
                else
                {
                    cur_scale = st->mainscale;  // full size: effect_old.cpp:2417
                }
            }
            else
            {
                continue;  // dead, skip
            }
        }
        else if (p.state == kStateGsphere)
        {
            // effect_old.cpp:2420-2428 — skip before frame 26
            if (st->framenum < kGsOn) continue;
            sub = kSubObjSmoke01;   // smoke01 (hot): effect_old.cpp:2425
        }
        else if (p.state == kStateRing)
        {
            // effect_old.cpp:2430-2433 — unreachable in active code
            sub = kSubObjSmoke01;   // smoke01 (hot): effect_old.cpp:2433
        }
        else
        {
            continue;  // USEME/dead: effect_old.cpp:2436
        }

        if (cur_scale <= 0.0f) continue;  // effect_old.cpp:2438

        // ── RENDER ANCHOR ──────────────────────────────────────────────────
        // effect_old.cpp:2442-2450
        // In test harness: no target → origin = (0,0,0) → world particle pos
        // = sm[i].pos + origin = sm[i].pos (local-space coords are world-space
        // when anchored at origin). When a target is present (live game),
        // OBJ3D_ABSPOS makes positions absolute — same net result here since
        // we add origin to particle pos before building the matrix.
        const float wx = p.pos[0] + st->origin[0];
        const float wy = p.pos[1] + st->origin[1];
        const float wz = p.pos[2] + st->origin[2];

        // ── BUILD OBJECT MATRIX ────────────────────────────────────────────
        // Snapshot order (effect_old.cpp:2453-2468):
        //   Identity
        //   · RotX(-2π/3 = -120°)        — fixed world-axis tilt
        //   · RotZ(-π/4)                 — 45° Z spin
        //   · RotZ(-(facing/256 * 2π))   — caster-facing Z spin
        //   · Scale(cur_scale uniform)
        //   · Translate(wx, wy, wz)
        //
        // The two RotZ calls combine. Snapshot applies them sequentially;
        // we do the same to match exactly.
        d3d::Matrix local;
        d3d::MatrixIdentity(local);                            // :2453
        d3d::MatrixRotateX(local, kRotX1);                     // :2455  -120°
        d3d::MatrixRotateZ(local, kRotZ1);                     // :2456  -45°
        d3d::MatrixRotateZ(local, rot_z2);                     // :2457  -facing
        d3d::MatrixScale(local, cur_scale, cur_scale, cur_scale); // :2459-2462
        d3d::MatrixTranslate(local, wx, wy, wz);               // :2464-2468

        // effect_old.cpp:2470 — RenderObject(obj)
        // Sub-object mesh + material.
        std::memcpy(obj.diffuse,  st->diffuse[sub],  sizeof(obj.diffuse));
        std::memcpy(obj.emissive, st->emissive[sub], sizeof(obj.emissive));
        obj.objnum = sub;
        obj.mesh   = st->mesh[sub];
        obj.flags  = d3d::Flag_Matrix;

        d3d::RenderObject(*Renderer,
                          st->mesh[sub],
                          local,
                          st->diffuse[sub],
                          st->emissive[sub],
                          d3d::CurrentBlend(),
                          nullptr,       // inst_world = nullptr (matrix is already world-space)
                          wz);           // sort_z = world Z for depth ordering
    }

}

// =========================================================================
// Spawn — load asset, register meshes, run Initialize.
//
// anchor_world_pos: if the caller has a target world position, pass it here
//   and set has_target=true to enable the OBJ3D_ABSPOS path. Test harness
//   passes nullptr → anchors at world origin.
// facing_byte: caster's facing angle 0..255. Test harness passes 0.
// =========================================================================
State* Spawn(const S3DPoint& origin, const float* anchor_world_pos, float facing_byte)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    if (anchor_world_pos)
    {
        st->has_target = true;
        st->origin[0]  = anchor_world_pos[0];
        st->origin[1]  = anchor_world_pos[1];
        st->origin[2]  = anchor_world_pos[2];
    }
    // else: origin stays (0,0,0) and has_target = false.

    st->facing = facing_byte;

    // Bind the I3D asset (forensics §4).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[fireflash-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fireflash-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Register both sub-object meshes (forensics §4):
    //   GetObject(0) = smoke   (cool/late-life SMOKEY1)
    //   GetObject(1) = smoke01 (hot/GSPHERE/RING/early SMOKEY1)
    const int32_t num_tex = img3d->NumTextures();
    for (int32_t s = 0; s < kNumSubObjs; ++s)
    {
        st->mesh[s] = d3d::RegisterSubMesh(*Renderer, img3d, s, num_tex);
        if (!st->mesh[s])
            log_warn("[fireflash-shim] RegisterSubMesh(sub=%d) returned 0 — "
                     "sub-object will not draw", s);
        d3d::LoadMaterial(img3d, s, st->diffuse[s], st->emissive[s]);
        // fireflash.I3D material[1] (smoke01) authored emissive = (1,1,1)
        // (i3d_dump_all/fireflash/manifest.txt:12). In the helper-mesh
        // fragment shader (src/renderer.cpp kHelperMeshFs, ~line 962) the
        // emissive contribution is ADDITIVE: col = tex.rgb*diffuse*lit + emissive.rgb.
        // Passing (1,1,1) saturates every fragment to white, washing out
        // the authored fire texture color. Forensics §7 (lit/unlit): the
        // animator "does not zero the material" in the snapshot, but in
        // D3D the Emissive field there did not feed an unconditional +1
        // additive — the fixed-function pipeline modulated it by the lit
        // term. Our deferred shader has no such gate, so the same asset
        // produces a different signal. Zero emissive so MODULATE-style
        // texture-color passthrough drives the additive blend bracket
        // exactly as forensics §7 specifies ("sprite reads at its
        // authored brightness"). Same fix Iced applied (see iced.cpp:543).
        st->emissive[s][0] = 0.0f;
        st->emissive[s][1] = 0.0f;
        st->emissive[s][2] = 0.0f;
        // st->emissive[s][3] left at 1.0 (alpha channel, not used in shader formula)
    }

    // Run Initialize — seeds GSPHERE slots + sets framenum=0.
    Initialize(st);

    // tex=%u: the texture handle embedded in mesh[0] (via RegisterSubMesh).
    // Retrieve it by re-calling GetTexture(0) here for diagnostic purposes only
    // (the mesh holds its own copy; this is read-only at spawn time).
    TTextureHandle spawn_tex = kInvalidTexture;
    if (st->imagery && st->imagery->NumTextures() > 0)
    {
        S3DTex spawn_tex_s = {};
        st->imagery->GetTexture(0, &spawn_tex_s);
        spawn_tex = spawn_tex_s.htexture;
    }
    log_info("[fireflash-shim] Spawn ok: framenum=%d alive=%d tex=%u "
             "mesh=(%u,%u) origin=(%d,%d,%d) facing=%.0f has_target=%d",
             st->framenum, int(st->alive), spawn_tex,
             st->mesh[0], st->mesh[1],
             origin.x, origin.y, origin.z,
             double(st->facing), int(st->has_target));
    return st;
}

// =========================================================================
// Public entry points.
// =========================================================================

// Tick — advance simulation (call from regular submit hook).
void Tick(State* st)
{
    if (st) Animate(st);
}

// Submit — draw (call from submit_world hook, after BeginTilePass).
void Submit(State* st)
{
    if (st && st->alive) Render(st);
}

void Destroy(State* st)
{
    delete st;
}

// IsAlive — true while the 100-frame effect is still running. After
// KillThisEffect (framenum >= 100), state stays allocated but alive=false;
// the test harness uses this to drive auto-respawn.
bool IsAlive(const State* st)
{
    return st && st->alive;
}

} // namespace fireflash_shim
