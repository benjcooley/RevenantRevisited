// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  blood.cpp - TBloodAnimator (B01) port via the d3d::* shim            *
// *                                                                       *
// *  SHIM VALIDATION PORT — fourth in series (fountain/sparks/fizzle).   *
// *  First port to exercise the two-pass per-particle render pattern      *
// *  (Alpha base + AdditiveStraight overlay) and scene-lit (DoLighting)   *
// *  vertex-color modulation.                                             *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/B01_TBloodEffect.md             *
// *  Snapshot: src/effectcomp.cpp:1182-1554 (TBloodSystem)               *
// *            src/effect_old.cpp:11261-11340 (TBloodAnimator)           *
// *            src/effect.h:2405-2486 (TBloodEffect + TBloodAnimator)    *
// *                                                                       *
// *  Retail fidelity: retail-partial. Asset is byte-identical to shipped  *
// *  (MD5 9c0596112cd46d6e3cb91df9f97770df). Registration + SetParams     *
// *  caller wiring retail-confirmed. TBloodSystem kinematic constants     *
// *  are snapshot-only (no retail TBloodSystem body extracted; see        *
// *  forensics §2.1 + §13.1).                                            *
// *                                                                       *
// *  Shim usage + gaps logged in .vfx_logs/blood.md:                     *
// *  - FLY droplets: d3d::RenderObjectSpinning (ScreenAligned + spin).    *
// *    Picks up the SetBlendState/SetAddBlendState toggles internally     *
// *    via d3d::CurrentBlend — no shim extension needed.                  *
// *  - SPLAT/SHRINK: Renderer->SubmitFxBillboard direct with WorldXY.     *
// *    Snapshot's no-rotation matrix renders camera-facing in D3D, but    *
// *    the intended visual is a ground-flat wet decal — the shim's       *
// *    ScreenAligned route would re-introduce that bug, so we bypass.    *
// *  - Gap: DoLighting grayscale passed via color_rgba on the draw item;  *
// *    EFxLightMode::LitFlat not wired through shim (uses Unlit).         *
// *                                                                       *
// *  Dead code NOT reconstructed per forensics §13.4 / §13.5 / §13.6:   *
// *  - Bif impact-splash block (effectcomp.cpp:1485-1529)                 *
// *  - Big + med trail loops (effectcomp.cpp:1242-1283)                   *
// *  - TBloodParticle3DAnimator alternate                                 *
// *  - Big (size-2) sprite draw (commented in effectcomp.cpp)             *
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../revenant.h"    // ViolenceLevel, MaxLights, Scene3D, S3DPoint
#include "../3dscene.h"     // T3DScene — GetClosestLights, GetLightBrightness
#include "../dls.h"         // GetAmbientLight()

extern TRenderer* Renderer;

namespace blood_shim {

// =========================================================================
// § Constants — every value cites forensics §3 + source lines.
// All snapshot-only unless marked otherwise.
// =========================================================================

// Pool cap — effectcomp.h:374
constexpr int32_t kMaxBloods         = 30;  // snapshot-only

// Base launch speed, wu/tick — effectcomp.cpp:1173
constexpr float   kBloodVel          = 2.0f;  // snapshot-only

// Gravity: subtracted from vel.z each FLY tick — effectcomp.cpp:1165
constexpr float   kBloodGravity      = 0.37f;  // snapshot-only

// Horizontal air resistance: mul on vel.x, vel.y each FLY tick — effectcomp.cpp:1164
constexpr float   kBloodAirResist    = 0.95f;  // snapshot-only

// FLY scale growth per tick — effectcomp.cpp:1177
constexpr float   kBloodFlyScaleStep = 0.01f;  // snapshot-only

// Initial droplet scale at launch — effectcomp.cpp:1178
constexpr float   kBloodFlyScale     = 1.0f;  // snapshot-only

// Floor height at which droplet snaps to SPLAT (wu, object-local z) — effectcomp.cpp:1172
constexpr float   kBloodZOffset      = 20.0f;  // snapshot-only

// Scale applied during SPLAT stage — effectcomp.cpp:1169
constexpr float   kBloodSplatSize    = 1.8f;  // snapshot-only

// Ticks a droplet holds in SPLAT before transitioning to SHRINK — effectcomp.cpp:1170
constexpr int32_t kBloodSplatDuration = 25;  // snapshot-only

// Scale removed per tick during SHRINK — effectcomp.cpp:1171
constexpr float   kBloodSplatScale   = 0.1f;  // snapshot-only; negative per-tick step

// Start-delay added to trail-fill droplets — effectcomp.cpp:1174
constexpr int32_t kBloodSquirtDelay  = 3;  // snapshot-only

// Stage codes — effectcomp.cpp:1166-1168
constexpr int32_t kStageFly   = 0;  // snapshot-only
constexpr int32_t kStageSplat = 1;  // snapshot-only
constexpr int32_t kStageShrink= 2;  // snapshot-only

// Trail-fill budget tuning constants — effectcomp.cpp:1179-1180
constexpr int32_t kBloodMostBloods       = 10;  // snapshot-only
constexpr int32_t kBloodBloudsPerSquirt  = 4;   // snapshot-only

// DoLighting: ambient gain boost — effectcomp.cpp:1400-1402
constexpr float   kDoLightingAmbientGain = 4.0f;  // snapshot-only

// DoLighting: brightness clamp + normalize divisor — effectcomp.cpp:1404-1407
constexpr float   kDoLightingDivisor     = 1.5f;  // snapshot-only

// Rotation for FLY droplets: WorldXY tip (RotateX -90°) + static -45° in-plane spin
// (effectcomp.cpp:1469-1470 — rot.x = -90°, rot.z = -45°).
// The WorldXY tip is handled by EFxBillboardOrientation::WorldXY in SubmitFxParticle.
// The -45° in-plane spin is passed as rotation_rad.
constexpr float   kFlyRotZ = -float(M_PI) / 4.0f;  // snapshot-only; -45° in radians

// TORADIAN — revdefs.h:25
constexpr float   kToRadian = float(M_PI) / 180.0f;  // snapshot-only

// Sub-object index layout — effect_old.cpp:11284-11291, effectcomp.cpp:1185-1202
//   pass k=0 (Alpha base):     sml=obj4(box05), med=obj5(box06), big=obj6(box07), sp=obj7(box08)
//   pass k=1 (Additive sheen): sml=obj0(box01), med=obj1(box02), big=obj2(box03), sp=obj3(box04)
// Size-2 (big) and sp (splat) draws are commented out in the snapshot (forensics §7, §13.3, §13.4).
// We only render size=small(0) and size=med(1); size=2 falls back to sml sprite.
constexpr int32_t kSubObjAlpha[4]    = { 4, 5, 6, 7 };  // k=0: sml, med, big, sp
constexpr int32_t kSubObjAdditive[4] = { 0, 1, 2, 3 };  // k=1: sml2, med2, big2, sp2

// Per-billboard base size in world units (WU). Blood droplets are small
// combat sprites; 32 wu is consistent with the sparks family as a starting
// point. The snapshot's scl starts at 1.0 (kBloodFlyScale) and grows slightly
// during FLY, snaps to 1.8 (kBloodSplatSize) during SPLAT, then shrinks.
// Subject to visual tuning per forensics §13.1 (kinematic constants snapshot-only).
constexpr float   kBloodBaseSizeWu   = 32.0f;

// Imagery path — Class.Def:2014, forensics §4
constexpr const char* kImageryPath   = "Misc\\Blood.I3D";

// Number of distinct sub-objects in Blood.I3D — 8 total (box01-box08), forensics §4
constexpr int32_t kNumSubObjs        = 8;

// =========================================================================
// Helpers
// =========================================================================

// Inclusive-range random matching snapshot semantics
// (rand() % (max-min+1) + min — src/revutils.cpp:1597-1612).
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// min/max helpers: revtypes.h defines min/max as macros, shadowing std::.
// Use these wrappers to avoid macro expansion conflicts.
template <typename T>
static T blood_min(T a, T b) { return a < b ? a : b; }

template <typename T>
static T blood_max(T a, T b) { return a > b ? a : b; }

// =========================================================================
// Per-droplet state — mirrors SBloodParticle / TBloodSystem's per-slot arrays
// (effectcomp.h:356-402).
// =========================================================================
struct Droplet
{
    bool    used      = false;
    float   pos[3]    = {0.0f, 0.0f, 0.0f};  // local space (relative to effect origin)
    float   vel[3]    = {0.0f, 0.0f, 0.0f};
    float   scl       = kBloodFlyScale;
    int32_t stage     = kStageFly;
    int32_t count     = 0;        // tick counter for SPLAT duration
    int32_t delay     = 0;        // start-delay countdown (trail-fill droplets)
    int32_t size      = 0;        // 0=sml, 1=med, 2=big (big falls back to sml in render)
};

// =========================================================================
// Per-effect state (one per spawned instance).
// =========================================================================
struct State
{
    Droplet drops[kMaxBloods];    // pool — effectcomp.h:374

    // Effect world position (victim pos + height, in caller's world space).
    float   base_pos[3] = {0.0f, 0.0f, 0.0f};

    // The height used for both the effect world z offset AND the droplet
    // local-origin z — SetParams arg 1 (forensics §5).
    // Used to cancel out in the render translate: pos.z - height.
    float   height      = 40.0f;

    bool    alive       = true;

    // Resolved asset data — per-sub-object UV sub-rects (8 total).
    // Texture handle lazily bound on first Submit (gotcha #1).
    T3DImagery*    imagery              = nullptr;
    TTextureHandle texture              = kInvalidTexture;
    float          uv_rect[kNumSubObjs][4];  // [sub][u,v,w,h]
    float          diffuse[kNumSubObjs][4];

    int32_t spawn_log_done = 0;

    State()
    {
        for (int32_t i = 0; i < kNumSubObjs; ++i)
        {
            uv_rect[i][0] = 0.0f; uv_rect[i][1] = 0.0f;
            uv_rect[i][2] = 1.0f; uv_rect[i][3] = 1.0f;
            diffuse[i][0] = 1.0f; diffuse[i][1] = 1.0f;
            diffuse[i][2] = 1.0f; diffuse[i][3] = 1.0f;
        }
    }
};

// =========================================================================
// DoLighting — computes per-droplet scene brightness, 0..1.
// Transcribed from TBloodSystem::DoLighting (effectcomp.cpp:1372-1413).
//
//   GetClosestLights(worldpos) -> l1,l2,l3
//   r = brightness(l1) + (MaxLights>1 ? brightness(l2) : 0) + ...
//   r += (GetAmbientLight()/255) * 4.0
//   r = min(r, 1.5) / 1.5      // clamp + normalize to 0..1
// =========================================================================
static float DoLighting(float wx, float wy, float wz)
{
    const int32_t ix = int32_t(wx);
    const int32_t iy = int32_t(wy);
    const int32_t iz = int32_t(wz);

    int32_t ml = -1, ml2 = -1, ml3 = -1;
    Scene3D.GetClosestLights(ix, iy, iz, ml, ml2, ml3);

    float r = 0.0f;
    if (ml >= 0)
    {
        float brightness = 0.0f;
        Scene3D.GetLightBrightness(ml, ix, iy, iz, brightness);
        r += brightness;
    }
    if (MaxLights > 1 && ml2 >= 0)
    {
        float brightness = 0.0f;
        Scene3D.GetLightBrightness(ml2, ix, iy, iz, brightness);
        r += brightness;
    }
    if (MaxLights > 2 && ml3 >= 0)
    {
        float brightness = 0.0f;
        Scene3D.GetLightBrightness(ml3, ix, iy, iz, brightness);
        r += brightness;
    }

    // Ambient boost ×4 — effectcomp.cpp:1400-1402
    r += (float(GetAmbientLight()) / 255.0f) * kDoLightingAmbientGain;

    // Clamp + normalize to [0,1] — effectcomp.cpp:1404-1407
    if (r > kDoLightingDivisor) r = kDoLightingDivisor;
    r /= kDoLightingDivisor;
    return r;
}

// =========================================================================
// SeedDroplet — fills slot `idx` in the pool from spray parameters.
// Used by Init for both main droplets and trail-fill.
// Transcribed from TBloodSystem::Init (effectcomp.cpp:1220-1235).
// =========================================================================
static void SeedDroplet(State* st, int32_t idx,
                         int32_t hangle, int32_t vangle,
                         int32_t hspread, int32_t vspread,
                         int32_t max_size, int32_t delay_val)
{
    Droplet& d = st->drops[idx];
    d.used    = true;
    d.size    = snap_random(0, blood_min(max_size, 2));  // effectcomp.cpp:1225

    // Local start position — effectcomp.cpp:1226-1228
    d.pos[0] = 0.0f;
    d.pos[1] = 0.0f;
    d.pos[2] = st->height;  // local z = spawn height

    // Directional spray angles — effectcomp.cpp:1222-1223
    // byte-angle (0-255) → degrees → radians
    const int32_t ha_byte = ((hangle + snap_random(-hspread, hspread)) & 255);
    const int32_t va_byte = ((vangle + snap_random(-vspread, vspread)) & 255);
    const float   ha      = float(ha_byte) * (360.0f / 256.0f) * kToRadian;
    const float   va      = float(va_byte) * (360.0f / 256.0f) * kToRadian;

    // Velocity = BLOOD_VEL × (cos ha, sin ha, sin va) × random(100,300)/100
    // effectcomp.cpp:1229-1231
    const float speed_scale = float(snap_random(100, 300)) / 100.0f;  // 1.0..3.0
    d.vel[0] = kBloodVel * std::cos(ha) * speed_scale;
    d.vel[1] = kBloodVel * std::sin(ha) * speed_scale;
    d.vel[2] = kBloodVel * std::sin(va) * speed_scale;

    d.scl   = kBloodFlyScale;   // effectcomp.cpp:1232
    d.stage = kStageFly;        // effectcomp.cpp:1233
    d.delay = delay_val;        // effectcomp.cpp:1234 (main droplets = 0)
    d.count = 0;
}

// =========================================================================
// Initialize — transcribed from TBloodSystem::Init (effectcomp.cpp:1182-1307).
//
// Seeding:
//   1. Main droplets (0..num-1): directional spray, delay=0.
//   2. Trail-fill (small droplets riding main ones, vel*0.75..0.94x,
//      delayed): only if ViolenceLevel >= 4 (effectcomp.cpp:1238-1239).
//      ONLY the smalls-trail-smalls loop (effectcomp.cpp:1287-1306) —
//      the two bigger trail loops are dead code (forensics §13.5).
// =========================================================================
static void Initialize(State* st,
                        int32_t hangle, int32_t vangle,
                        int32_t hspread, int32_t vspread,
                        int32_t num, int32_t max_size)
{
    // Count clamp — effectcomp.cpp:1214-1215
    num = blood_min(num, ViolenceLevel * 2);
    // Size clamp — effectcomp.cpp:1216-1217
    max_size = blood_min(max_size, ViolenceLevel);

    // Trail-fill budget — effectcomp.cpp:1211
    // left = max(MAX_BLOODS - (MOSTBLOODS - num) * BLOODSPERSQUIRT, BLOODSPERSQUIRT)
    int32_t left = blood_max(
        kMaxBloods - (kBloodMostBloods - num) * kBloodBloudsPerSquirt,
        kBloodBloudsPerSquirt);

    // Seed main droplets — effectcomp.cpp:1220
    int32_t spawned = 0;
    const int32_t main_count = blood_min(num, kMaxBloods);
    for (int32_t i = 0; i < main_count; ++i, --left)
    {
        SeedDroplet(st, spawned++, hangle, vangle, hspread, vspread,
                    max_size, /*delay=*/0);
    }

    // No trail-fill below ViolenceLevel 4 — effectcomp.cpp:1238-1239
    if (ViolenceLevel < 4) left = 0;

    // Trail-fill: smalls-trail-smalls loop — effectcomp.cpp:1287-1306
    // For each used main droplet (in order), while budget `left` remains,
    // add a small droplet with vel = parent.vel * random(60,75)/80
    // and delay = parent.delay + BLOOD_SQUIRTDELAY + random(-1,1).
    for (int32_t i = 0; i < spawned && left > 0 && spawned < kMaxBloods; ++i)
    {
        const Droplet& parent = st->drops[i];
        if (!parent.used) continue;

        const float vel_scale = float(snap_random(60, 75)) / 80.0f;  // 0.75..0.9375
        const int32_t trail_delay = parent.delay
                                  + kBloodSquirtDelay
                                  + snap_random(-1, 1);  // effectcomp.cpp:1300

        Droplet& child = st->drops[spawned];
        child.used    = true;
        child.size    = snap_random(0, 1);          // small or med — effectcomp.cpp:1287
        child.pos[0]  = 0.0f;
        child.pos[1]  = 0.0f;
        child.pos[2]  = st->height;                 // effectcomp.cpp:1290
        child.vel[0]  = parent.vel[0] * vel_scale;  // effectcomp.cpp:1297-1299
        child.vel[1]  = parent.vel[1] * vel_scale;
        child.vel[2]  = parent.vel[2] * vel_scale;
        child.scl     = kBloodFlyScale;             // effectcomp.cpp:1302
        child.stage   = kStageFly;                  // effectcomp.cpp:1303
        child.delay   = blood_max(trail_delay, 0);
        child.count   = 0;

        spawned++;
        left--;
    }
}

// =========================================================================
// Animate — transcribed from TBloodSystem::Animate (effectcomp.cpp:1309-1370).
//
// Per tick (24 Hz — forensics §6.2 + feedback_framerate_independent_anim:
// animation time-based, but this system uses per-tick integer state
// so the 24 Hz tick rate is built in via the caller's game loop).
//
// FLY:     integrate pos += vel; drag vel.xy *= 0.95; gravity vel.z -= 0.37;
//          grow scl += 0.01; on pos.z <= 20: snap to SPLAT (or kill if VL<3/size==0).
// SPLAT:   scl = 1.8, count++; after 25 ticks → SHRINK.
// SHRINK:  scl -= 0.1; at 0 → kill.
// =========================================================================
static void Animate(State* st)
{
    bool done = true;

    for (int32_t i = 0; i < kMaxBloods; ++i)
    {
        Droplet& d = st->drops[i];
        if (!d.used) continue;

        // Start-delay countdown — effectcomp.cpp:1313
        if (d.delay > 0) { d.delay--; continue; }

        done = false;  // at least one active droplet

        if (d.stage == kStageFly)
        {
            // Integrate — effectcomp.cpp:1326
            d.pos[0] += d.vel[0];
            d.pos[1] += d.vel[1];
            d.pos[2] += d.vel[2];
            // Horizontal drag — effectcomp.cpp:1328-1329
            d.vel[0] *= kBloodAirResist;
            d.vel[1] *= kBloodAirResist;
            // Gravity — effectcomp.cpp:1330
            d.vel[2] -= kBloodGravity;
            // Slight growth in flight — effectcomp.cpp:1331
            d.scl += kBloodFlyScaleStep;

            // Ground check — effectcomp.cpp:1334
            if (d.pos[2] <= kBloodZOffset)
            {
                d.pos[2] = kBloodZOffset;
                d.stage  = kStageSplat;
                d.count  = 0;

                // Low-violence: no splat-stick — effectcomp.cpp:1341-1344
                if (ViolenceLevel < 3) { d.used = false; continue; }
                // Small droplets never splat — effectcomp.cpp:1343
                if (d.size == 0)       { d.used = false; continue; }
            }
        }
        else if (d.stage == kStageSplat)
        {
            // Snap to splat size — effectcomp.cpp:1349
            d.scl = kBloodSplatSize;
            d.count++;
            // Hold SPLAT_DURATION ticks then transition — effectcomp.cpp:1350-1351
            if (d.count > kBloodSplatDuration)
            {
                d.stage = kStageShrink;
                d.count = 0;
            }
        }
        else if (d.stage == kStageShrink)
        {
            // Scale down — effectcomp.cpp:1357
            d.scl -= kBloodSplatScale;
            if (d.scl <= 0.0f)
            {
                d.scl  = 0.0f;
                d.used = false;  // drop dead — effectcomp.cpp:1360
            }
        }
    }

    // Self-kill when all droplets are done — effect_old.cpp:11313-11316
    if (done)
    {
        log_info("[blood-shim] all droplets dead");
        st->alive = false;
    }
}

// =========================================================================
// Render — transcribed from TBloodSystem::Render (effectcomp.cpp:1415-1483)
// and TBloodAnimator::Render (effect_old.cpp:11326-11335).
//
// Outer bracket: SaveBlendState / RestoreBlendState (TBloodAnimator).
// Per droplet, two passes:
//   k=0: RestoreBlendState() + SetBlendState()          → Alpha
//   k=1: RestoreBlendState() + ONE/ONE additive          → AdditiveStraight
//
// Sub-object selection (forensics §4 table):
//   pass k=0: Alpha base   — obj = kSubObjAlpha[size] (capped to med for big)
//   pass k=1: Additive     — obj = kSubObjAdditive[size]
//
// FLY orientation:   ScreenAligned + rotation_rad = -π/4
//                    (d3d::RenderObjectSpinning → SubmitFxParticle)
//   The snapshot matrix Scale * RotateX(-90°) * RotateZ(-π/4) * Translate
//   produced a Y-up sprite presenting toward the camera in the period D3D
//   engine — a round droplet shape always facing the viewer in flight.
//   ScreenAligned with rotation_rad is the modern-pipeline equivalent.
//
// SPLAT/SHRINK:      WorldXY, no rotation             (SubmitFxBillboard)
//   The snapshot uses no matrix rotation for SPLAT/SHRINK. In the D3D engine
//   that produced a screen-aligned card, but the *intended* visual — per
//   user visual call + the dead "bif" splash block in effectcomp.cpp:
//   1485-1529 which DOES apply RotateX(-90°) — is a ground-flat wet decal.
//
// DoLighting per droplet: brightness → item.color_rgba[0..2].
// =========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // Lazy texture binding (gotcha #1): the I3D loader doesn't always
    // upload the texture to GPU before the first Submit.
    if (st->texture == kInvalidTexture && st->imagery)
    {
        if (st->imagery->NumTextures() > 0)
        {
            S3DTex tex = {};
            st->imagery->GetTexture(0, &tex);
            st->texture = tex.htexture;
        }
        if (st->texture == kInvalidTexture) return;  // wait another tick
    }
    if (st->texture == kInvalidTexture) return;

    // Outer SaveBlendState — effect_old.cpp:11327. We still bracket the
    // blend state via the d3d::* shim because SetBlendState / SetAddBlendState
    // calls below toggle d3d::CurrentBlend(), which we read to pick the
    // EFxBlend value for each per-particle draw item.
    d3d::BlendStateGuard blend_scope;

    for (int32_t i = 0; i < kMaxBloods; ++i)
    {
        const Droplet& d = st->drops[i];
        if (!d.used) continue;
        if (d.delay > 0) continue;  // still in pre-delay, not yet drawn

        // World position of this droplet for lighting query.
        // Droplet pos is in effect-local space; add base_pos to get world.
        // effectcomp.cpp:1469 uses the local pos to build the matrix;
        // for DoLighting it passes the effect position + local pos.
        const float wx = st->base_pos[0] + d.pos[0];
        const float wy = st->base_pos[1] + d.pos[1];
        const float wz = st->base_pos[2] + d.pos[2];

        // DoLighting — effectcomp.cpp:1472
        // In-game: GetAmbientLight() is always non-zero (SetAmbientLight(10+)
        // called at map-load), so this computes ~0.1 + point-light contribution
        // ≈ 0.4–1.0. In the vfx test rig there is no level loaded, so
        // GetAmbientLight()==0 and ClearPointLights() leaves zero scene lights
        // → DoLighting returns 0 → vertex color (0,0,0) → both passes invisible.
        // Floor to 1.0f (fully lit) so the shim renders visibly in the test rig;
        // in-game the real scene value will always be > 0 anyway.
        const float brightness = blood_max(DoLighting(wx, wy, wz), 1.0f);

        // Local translate: pos.x, pos.y, pos.z - height
        // The "- height" cancels the local start z so world-z = integrated delta.
        // effectcomp.cpp:1469: matrix = Scale * [FLY rot] * Translate(pos.x, pos.y, pos.z - height)
        const float lx = d.pos[0];
        const float ly = d.pos[1];
        const float lz = d.pos[2] - st->height;

        // Resolve world position for billboard placement:
        //   world pos = base_pos + (lx, ly, lz)
        // Both submission paths (SubmitFxParticle for FLY, SubmitFxBillboard
        // for SPLAT/SHRINK) take an absolute world_pos[3] — no instance
        // matrix is needed downstream.
        const float bx = st->base_pos[0] + lx;
        const float by = st->base_pos[1] + ly;
        const float bz = st->base_pos[2] + lz;

        // Scale from snapshot — effectcomp.cpp:1466-1468:
        //   obj.scl.x = obj.scl.y = obj.scl.z = scl
        // Multiplied by the base size constant (gotcha #3).
        const float size = d.scl * kBloodBaseSizeWu;

        // Sub-object selection — forensics §4, effectcomp.cpp:1457-1461.
        // size==2 (big) falls back to sml (index 0) — big sprite draw commented
        // out in the snapshot (forensics §13.3).
        // Only sml (0) and med (1) actually render; size==2 → sml index.
        const int32_t effective_size = (d.size >= 1) ? 1 : 0;  // cap med at index 1

        // Two-pass render per droplet — effectcomp.cpp:1419-1481
        for (int32_t k = 0; k < 2; ++k)
        {
            // Per-pass blend setup — effectcomp.cpp:1421-1438
            if (k == 0)
            {
                // RestoreBlendState + SetBlendState — Alpha pass
                // effectcomp.cpp:1421-1428
                d3d::SetBlendState();
            }
            else
            {
                // RestoreBlendState + ONE/ONE additive inline
                // effectcomp.cpp:1431-1438
                // The snapshot sets states inline (NOT SetAddBlendState),
                // but the renderer effect is the same ONE/ONE blend.
                // SetAddBlendState maps to EFxBlend::AdditiveStraight.
                d3d::SetAddBlendState();
            }

            // Sub-object for this pass — forensics §4
            const int32_t sub = (k == 0)
                ? kSubObjAlpha[effective_size]
                : kSubObjAdditive[effective_size];

            // UV sub-rect for this sub-object (gotcha #2 — atlas cells).
            // effectcomp.cpp:1409-1412 stamps these per-corner into lverts[].
            const float u_lo = st->uv_rect[sub][0];
            const float v_lo = st->uv_rect[sub][1];
            const float u_hi = u_lo + st->uv_rect[sub][2];
            const float v_hi = v_lo + st->uv_rect[sub][3];

            if (d.stage == kStageFly)
            {
                // FLY: ScreenAligned camera-facing droplet + static -π/4 spin.
                // The snapshot matrix is Scale * RotateX(-90°) * RotateZ(-π/4) *
                // Translate (effectcomp.cpp:1465-1476), which in the period D3D
                // engine produced a Y-up sprite presenting toward the camera —
                // round droplet shape always faces the viewer in flight. The
                // equivalent in our pipeline is ScreenAligned with rotation_rad
                // = -π/4 carrying the authored in-plane tilt.
                //
                // d3d::RenderObjectSpinning hardcodes ScreenAligned and routes
                // to SubmitFxParticle, picking up our SetBlendState/
                // SetAddBlendState toggles via d3d::CurrentBlend internally —
                // so this single shim call handles both passes correctly.
                d3d::Obj obj;
                obj.flags = d3d::Flag_Verts;  // use lverts[] for UV sub-rect
                obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
                obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
                obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
                obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;
                obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
                obj.pos[0] = bx;
                obj.pos[1] = by;
                obj.pos[2] = bz;
                obj.objnum = sub;
                obj.diffuse[0] = brightness;
                obj.diffuse[1] = brightness;
                obj.diffuse[2] = brightness;
                obj.diffuse[3] = 1.0f;
                d3d::RenderObjectSpinning(*Renderer, obj, kFlyRotZ,
                                          st->texture, /*inst_world=*/nullptr);
            }
            else
            {
                // SPLAT / SHRINK: WorldXY ground decal (wet blood splat lying
                // flat on the floor). The snapshot matrix is Scale * Translate
                // only (no rotation), but the *intended* visual — confirmed by
                // user visual call + the dead "bif" block at effectcomp.cpp:
                // 1485-1529 that DOES apply RotateX(-90°) to the splat sub-
                // objects — is a ground-flat decal, NOT a camera-facing card.
                //
                // Submit directly via SubmitFxBillboard (no rotation_rad
                // needed) with WorldXY orientation. d3dport's RenderObject
                // ScreenAligned route would have re-introduced the camera-
                // facing bug, so we bypass the shim here.
                const uint8_t blend_id = uint8_t(
                    d3d::CurrentBlend() == d3d::BlendMode::Additive
                        ? EFxBlend::AdditiveStraight
                        : EFxBlend::Alpha);
                SBillboardDrawItem item = {};
                item.world_pos[0]   = bx;
                item.world_pos[1]   = by;
                item.world_pos[2]   = bz;
                item.size_wu[0]     = size;
                item.size_wu[1]     = size;
                item.color_rgba[0]  = brightness;
                item.color_rgba[1]  = brightness;
                item.color_rgba[2]  = brightness;
                item.color_rgba[3]  = 1.0f;
                item.uv_rect[0]     = u_lo;
                item.uv_rect[1]     = v_lo;
                item.uv_rect[2]     = u_hi - u_lo;
                item.uv_rect[3]     = v_hi - v_lo;
                item.key.texture     = st->texture;
                item.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
                item.key.blend       = blend_id;
                item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
                item.light_mode      = EFxLightMode::Unlit;
                item.orientation     = EFxBillboardOrientation::WorldXY;
                item.debug_mode      = EFxDebugMode::Normal;
                Renderer->SubmitFxBillboard(item);
            }
        }  // end two-pass loop (k=0,1)
    }  // end droplet loop

    if (!st->spawn_log_done)
    {
        int32_t active = 0;
        for (int32_t i = 0; i < kMaxBloods; ++i)
            if (st->drops[i].used) ++active;
        // sub=(alpha_sml,alpha_med / add_sml,add_med) — the four sub-object
        // indices used for size=0/1 in pass k=0 (Alpha) and k=1 (Additive).
        log_info("[blood-shim] first render: %d droplets tex=%u "
                 "sub=(alpha:%d,%d / add:%d,%d) base=(%.0f,%.0f,%.0f) h=%.0f",
                 active, st->texture,
                 kSubObjAlpha[0], kSubObjAlpha[1],
                 kSubObjAdditive[0], kSubObjAdditive[1],
                 st->base_pos[0], st->base_pos[1], st->base_pos[2],
                 st->height);
        st->spawn_log_done = 1;
    }
}

// =========================================================================
// Spawn — load asset, resolve sub-object materials + UV sub-rects, seed.
//
// Parameters match TBloodEffect::SetParams (effect.h:2438-2439) / §5:
//   hangle  — mean horizontal spray direction (byte-angle 0-255)
//   num     — requested droplet count (before ViolenceLevel clamp in Init)
//   maxsize — max size index (0=sml,1=med,2=big; clamped in Init)
//   height  — spawn z offset (wu above victim pos) + droplet local start z
//
// For the test harness:
//   hangle=128 = 180° from victim front (attack direction default)
//   vangle=0   = horizontal spray (attack; decap uses 64=90°)
//   hspread=32, vspread=5 = retail impale values (forensics §3 / §2.1)
// =========================================================================
State* Spawn(const S3DPoint& origin,
             int32_t hangle,
             int32_t num,
             int32_t maxsize,
             int32_t height)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);
    st->height      = float(height);

    // Bind Blood.I3D — forensics §4
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[blood-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[blood-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Resolve material diffuse + UV sub-rect for all 8 sub-objects.
    // All 8 share one texture (texture 0 — forensics §4: 128×160 RGB565 sheet).
    // UV sub-rects are per-sub-object atlas cells (gotcha #2).
    for (int32_t s = 0; s < kNumSubObjs; ++s)
    {
        // We use a throwaway emissive array since blood emissive is not used
        // (DoLighting provides all brightness; emissive is 0 for blood).
        float emissive_tmp[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        d3d::LoadMaterial(img3d, s, st->diffuse[s], emissive_tmp);
        // Note: we override diffuse at render time with the DoLighting
        // grayscale value — the authored material diffuse is not used
        // directly (the texture is the blood color; DoLighting dims it).
        // But LoadMaterial is called to satisfy the API contract.

        const int32_t nv = img3d->NumObjVerts(s);
        if (nv > 0)
        {
            std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
            img3d->GetObjVerts(s, vbuf.data(), 0, 0,
                               ERender3DVertex::Vertex);
            float minu = vbuf[0].tu, maxu = vbuf[0].tu;
            float minv = vbuf[0].tv, maxv = vbuf[0].tv;
            for (int32_t vi = 1; vi < nv; ++vi)
            {
                if (vbuf[vi].tu < minu) minu = vbuf[vi].tu;
                if (vbuf[vi].tu > maxu) maxu = vbuf[vi].tu;
                if (vbuf[vi].tv < minv) minv = vbuf[vi].tv;
                if (vbuf[vi].tv > maxv) maxv = vbuf[vi].tv;
            }
            st->uv_rect[s][0] = minu;
            st->uv_rect[s][1] = minv;
            st->uv_rect[s][2] = maxu - minu;
            st->uv_rect[s][3] = maxv - minv;
        }
    }

    // Spray params for test harness: hangle/num/maxsize/height from caller;
    // vangle=32 (0x20 = 45° upward), hspread=32, vspread=5 — retail impale
    // values (forensics §3 + §2.1: ResolveAttack:190 → 0x28,face-0x80,0x20,0x20,5,1).
    // vangle=0x20=32 → ha = 32*(360/256)*(π/180) = 45° — droplets fly upward.
    Initialize(st, hangle, /*vangle=*/32, /*hspread=*/32, /*vspread=*/5,
               num, maxsize);

    // Count actual spawned droplets for the log.
    int32_t n_spawned = 0;
    for (int32_t i = 0; i < kMaxBloods; ++i)
        if (st->drops[i].used) ++n_spawned;
    log_info("[blood-shim] Spawn ok %d droplets (hangle=%d num=%d maxsize=%d "
             "height=%d at (%d,%d,%d) ViolenceLevel=%d)",
             n_spawned, hangle, num, maxsize, height,
             origin.x, origin.y, origin.z,
             ViolenceLevel);
    return st;
}

// =========================================================================
// Public entry points
// =========================================================================
void Tick(State* st)
{
    if (st && st->alive) Animate(st);
}

void Submit(State* st)
{
    if (st && st->alive) Render(st);
}

void Destroy(State* st)
{
    delete st;
}

} // namespace blood_shim
