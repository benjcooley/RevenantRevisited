// =========================================================================
// *                   Revenant Revisited (port) - 2026                   *
// *  fireball.cpp - TFireBallAnimator_SHIM port via the d3d::* shim      *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/F07_TFireBallEffect.md          *
// *  Snapshot Initialize:  src/missileeffect.cpp:501-546                  *
// *  Snapshot Animate:     src/missileeffect.cpp:549-759                  *
// *  Snapshot Render:      src/missileeffect.cpp:1060-1085                *
// *  Snapshot RenderFireBall:      src/missileeffect.cpp:975-1020         *
// *  Snapshot RenderFireBallGlow:  src/missileeffect.cpp:1022-1057        *
// *  Snapshot RenderFireBallTrail: src/missileeffect.cpp:871-973          *
// *  Snapshot RenderFireBallBurst: src/missileeffect.cpp:793-869          *
// *  Snapshot SetAnimFrame:        src/missileeffect.cpp:761-779          *
// *  Snapshot TSubParticleAnimator::Animate: src/effectcomp.cpp:361-403   *
// *  Snapshot TSubParticleAnimator::Create:  src/effectcomp.cpp:455-524   *
// *  Snapshot TShockAnimator::Animate/Render: src/effectcomp.cpp:609-770  *
// *                                                                       *
// *  Retail fidelity: retail-partial (see forensics §2.1).                *
// *                                                                       *
// *  LIGHTING CATEGORY: self-lit (fire IS the light emitter).             *
// *  Blend = Alpha for ball/glow/trail/burst/spark (forensics §7 cites    *
// *  SetBlendState() = MODULATE + SRCALPHA/INVSRCALPHA). Ring blend =     *
// *  Alpha too: TShockAnimator explicitly sets SRCALPHA/INVSRCALPHA.      *
// *  Black-keyed atlas supplies transparency; no DoLighting call in       *
// *  original render body.                                                *
// *                                                                       *
// *  RENDER PIPELINE: FB particle (SubmitFxParticle with                  *
// *  EFxBillboardOrientation::ScreenAligned + per-instance rotation_rad)  *
// *  for ball/glow/trail/burst/spark — air sprites face the camera.       *
// *  Shockwave ring via SubmitHelperMesh (cylinder01 sub-mesh, WorldXY    *
// *  ground-plane geometry). All billboard draws issue from regular       *
// *  submit hook; the helper mesh ring goes through submit_world.         *
// *                                                                       *
// *  WHY direct SubmitFxParticle (not d3d::RenderObject WorldMesh):       *
// *  - The d3d::RenderObject WorldMesh path strips per-vertex UV control  *
// *    (it submits SHelperMeshSubmit which has no uv_rect override), so   *
// *    the box01 4×4 atlas cell cannot be selected per tick (round-1     *
// *    bug). The d3d:: ScreenAligned billboard path honors Flag_Verts but *
// *    lacks rotation_rad. d3d::RenderObjectSpinning is hardcoded         *
// *    ScreenAligned (WAVE_1_LESSONS §3.1) but doesn't expose the         *
// *    in-plane spin angle cleanly enough. So we go direct                *
// *    SubmitFxParticle. The snapshot's matrix tilt RotX(-30°)+RotY(+60°) *
// *    was the 1998 D3D fake-billboard hack — superseded by ScreenAligned *
// *    which faces the camera natively (round-3 fix).                     *
// *                                                                       *
// *  TEXTURE ANIMATION (forensics §8 — THE CORE FIX FROM ROUND 1):        *
// *  - The 4×4 atlas cycles cell per tick. SetAnimFrame (snapshot         *
// *    :761-779) writes per-vertex UVs into lverts[]; we encode the same  *
// *    cell sub-rect into SParticleDrawItem.uv_rect[0..3] = {u, v, .25,   *
// *    .25} per FillAtlasUv() below. Ball frame advances +1/tick (skips   *
// *    cell 3 = glow_frame). Glow always uses cell 3. Trail copies use    *
// *    the stale frame from when they were recorded. Burst uses a random  *
// *    starting frame, then advances per tick.                             *
// *                                                                       *
// *  DELIVERY (full Phase A + B + C — all visible passes implemented):    *
// *  Phase A (DONE): LAUNCH→FLY ball + glow + spark trail                 *
// *  Phase B (DONE): impact burst + shockwave ring (split-band Alpha      *
// *    cylinder approximating TShockAnimator's 4-stop vertex gradient)     *
// *  Phase C (DONE): 10-slot decaying mesh trail                          *
// *                                                                       *
// *  TEST HARNESS NOTES:                                                  *
// *  - Anchor at world origin. Launch direction = +X (facing byte = 0).   *
// *  - No TMissileEffect physics in test rig; state machine driven by     *
// *    timer (LAUNCH for kLaunchTicks, then FLY for kFlyTicks, then       *
// *    EXPLODE). Real game uses the snapshot's range/MOVE_BLOCKED checks. *
// *  - Audio FIRESHOOT/FIREHIT (retail-confirmed) skipped — no audio in   *
// *    the test rig.                                                       *
// *  - Spell point light (255,130,0) INT 255 MULT 20 skipped — no         *
// *    dynamic-light path in the test rig.                                *
// *  - RefreshZBuffer not available via shim (snapshot calls RestoreZ).   *
// =========================================================================

#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

// Forward decl: ExtractSubMeshTextureSlot lives in 3dimage.cpp. We use it to
// extract cylinder01 geometry, bind it to a white albedo, and supply the ring
// color through helper-mesh material state, matching TShockAnimator's
// textureless path.
struct SMeshVertex;
extern bool ExtractSubMeshTextureSlot(T3DImagery* img3d,
                                      int32_t obj_num,
                                      int32_t tex_slot,
                                      std::vector<SMeshVertex>& verts_out,
                                      std::vector<uint16_t>& indices_out);

namespace fireball_shim {

// =========================================================================
// §3 Constants — every value cites forensics §3 + snapshot source line.
// Snapshot-only constants flagged in trailing comment; retail-confirmed
// values (asset path, state enum) need no flag.
// =========================================================================

// --- State enum (missileeffect.h:20-23) ---
constexpr int32_t kStateLaunch  = 0;   // MISSILE_LAUNCH
constexpr int32_t kStateFly     = 1;   // MISSILE_FLY
constexpr int32_t kStateExplode = 2;   // MISSILE_EXPLODE

// --- Fireball specifics (missileeffect.h:121-177) [snapshot-only tuning] ---
constexpr float   kGrowRate          = 0.05f; // FIREBALL_GROW_RATE
constexpr float   kMaxSize           = 0.60f; // FIREBALL_MAX_SIZE
constexpr int32_t kMaxFrame          = 16;    // FIREBALL_MAX_FRAME — atlas frames
constexpr int32_t kGlowFrame         = 3;     // FIREBALL_GLOW_FRAME — cell used by glow, skipped by ball
constexpr float   kTrailScale        = 0.85f; // FIREBALL_TRAIL_SCALE — per-slot shrink
constexpr int32_t kTrailSize         = 10;    // FIREBALL_TRAIL_NUM (no SPACEOUT)
constexpr int32_t kMaxSpark          = 40;    // FIREBALL_MAX_SPARK — particle cap
constexpr int32_t kNormSpark         = 30;    // FIREBALL_NORM_SPARK = MAX_SPARK*0.75
constexpr int32_t kMaxBurst          = 10;    // FIREBALL_MAX_BURST

// --- Spark trail params (missileeffect.cpp:598-619) [snapshot-only] ---
constexpr float   kSparkScale        = 0.15f; // missileeffect.cpp:598
constexpr float   kSparkScaleDec     = 0.90f; // missileeffect.cpp:599
constexpr float   kSparkGravity      = 0.37f; // missileeffect.cpp:613
constexpr int32_t kSparkMinLife      = 15;    // missileeffect.cpp:615
constexpr int32_t kSparkMaxLife      = 20;    // missileeffect.cpp:616
constexpr float   kSparkFlickerSize  = 1.75f; // missileeffect.cpp:619

// --- Ring params (missileeffect.cpp:711-723) [snapshot-only] ---
constexpr float   kRingInitScale     = 25.0f;
constexpr float   kRingGrowFactor    = 1.085f;
constexpr float   kRingMaxSize       = 120.0f;
constexpr int32_t kRingColorStopCount= 4;
constexpr int32_t kRingSegments      = 24;
constexpr int32_t kRingBandCount     = kRingColorStopCount - 1;
constexpr float   kRingStops[kRingColorStopCount][4] = {
    {0.62f, 0.06f, 0.05f, 0.00f},
    {0.78f, 0.24f, 0.06f, 1.00f},
    {0.97f, 0.61f, 0.06f, 0.75f},
    {0.99f, 0.83f, 0.52f, 0.00f},
};

// --- 4×4 atlas UV layout (missileeffect.cpp:763-764) [snapshot-only] ---
// u = (frame % 4) * 0.25,  v = (frame / 4) * 0.25,  cell = 0.25 × 0.25
constexpr float   kAtlasCellSize     = 0.25f;
constexpr int32_t kAtlasCols         = 4;
constexpr int32_t kBallAtlasPixels   = 256;   // newfireball texture[0]
constexpr int32_t kSparkTexturePixels= 64;    // newfireball texture[1]

// --- Billboard base size in world units (engine adaptation) ---
// SubmitFxParticle.size_wu is full WU on-screen extent (not authored
// half-extent). For box01 (authored bbox ≈ ±40 wu) at scale 0.30..0.60 the
// snapshot draws a ~24..48 wu mesh quad; engine equivalent at 192 wu reads
// as a "moderate fireball" at peak (115 wu). Matches the bespoke port's
// settled value (src/effect.h:3654).
constexpr float   kBaseQuadWu        = 192.0f;
// Spark quad size — kept small so the 30-40 sparks visibly separate. Spark
// scale starts 0.15 and decays. Bespoke composes 6.0 × scale × flicker ×
// 6.0; we collapse the static 6.0×6.0 factor to a single multiplier.
constexpr float   kSparkQuadWu       = 36.0f;

// --- Test harness flight simulation (replaces engine physics) ---
//
// Test harness: no Fighting() target → no real aim direction → no useful
// trajectory. We approximate flight by hand:
//   1. Spawn lifts +50 z (matches snapshot z-lift, missileeffect.cpp:513-514)
//      and offsets -96 wu on +X so the in-frame trajectory is centered
//      around world origin.
//   2. LAUNCH for kLaunchTicks ticks (ball grows above caster, no motion).
//   3. FLY for kFlyTicks ticks at FIREBALL_SPEED wu/tick (96 wu total →
//      explosion lands at +X = 0 approx, dead center of camera).
//   4. EXPLODE at the impact point.
//
// In production with a live caster, TMissileEffect::Pulse drives
// `effect_pos` via `ConvertToVector(aim, speed, vel)` — bypassed here.
// FIREBALL_SPEED=8 is the forensics §3 retail value (missileeffect.h:121);
// we use it directly so flight speed reads correctly.
constexpr int32_t kLaunchTicks       = 16;   // ~0.67 s @ 24 Hz — ball grows
constexpr int32_t kFlyTicks          = 12;   // 0.5 s — travels 96 wu, lands in frame
constexpr float   kFlySpeedWuPerTick = 8.0f; // matches FIREBALL_SPEED (§3)
constexpr float   kHarnessSpawnXOff  = -96.0f; // start offset so impact lands at world origin
constexpr uint32_t kHarnessRngSeed    = 0xF07F1EBAu; // deterministic filmstrip seed

// --- Framerate-independent conversion ---
constexpr float kTicksPerSec         = 24.0f;

// --- Asset / sub-object indices (forensics §4, manifest confirmed) ---
constexpr const char* kImageryPath   = "Magic\\NewFireBall.I3D";
// object[0]=box01 (fire-ball quad, texslot=1 → texture index 0 = 256×256 atlas)
// object[1]=box02 (spark quad, texslot=2 → texture index 1 = 64×64 glow)
// object[2]=cylinder01 (shockwave ring, texslot=0 → no texture; tinted)
constexpr int32_t kSubBall           = 0;   // box01
constexpr int32_t kSubSpark          = 1;   // box02
constexpr int32_t kSubRing           = 2;   // cylinder01

// =========================================================================
// snap_random — inclusive-range deterministic random matching snapshot
// semantics. Retail used the process RNG; this shim owns an instance-local
// stream so repeated filmstrips are frame-comparable.
// =========================================================================
static int32_t snap_random(uint32_t& rng_state, int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    rng_state = rng_state * 1103515245u + 12345u;
    const uint32_t r = (rng_state >> 16) & 0x7fffu;
    return lo + int32_t(r % uint32_t(hi - lo + 1));
}

// =========================================================================
// FillAtlasUv — write the 4×4 atlas cell sub-rect for a given frame index.
// Transcribed from missileeffect.cpp:761-779 (SetAnimFrame). Forensics §8.
// Wraps modulo kMaxFrame so trail/burst stale frames stay in bounds.
//
//   u = (frame % 4) * 0.25,  v = (frame / 4) * 0.25,  cell = 0.25 × 0.25
//   uv_rect = { u, v, 0.25, 0.25 }
//
// THIS IS THE FLIPBOOK MECHANISM (forensics §8): UV atlas-cell pick, NOT
// framehtexs handle-swap, NOT UV scroll. Same texture is sampled per tick
// at a different cell. Each tick the ball's frame advances and a new cell
// is selected here.
// =========================================================================
static void FillAtlasUv(int32_t frame_idx, float out_uv_rect[4])
{
    const int32_t f = ((frame_idx % kMaxFrame) + kMaxFrame) % kMaxFrame;
    out_uv_rect[0] = float(f % kAtlasCols) * kAtlasCellSize;     // u (left)
    out_uv_rect[1] = float(f / kAtlasCols) * kAtlasCellSize;     // v (top)
    out_uv_rect[2] = kAtlasCellSize;                              // uw (width)
    out_uv_rect[3] = kAtlasCellSize;                              // vh (height)
}

// =========================================================================
// ResolveSubObjTextureSlot — walk a sub-object's texfaces[] table to find
// which 0-based texture index it samples.
//
// NewFireBall.I3D manifest:
//   box01    texslot=1 (1-based texfaces) → texture index 0 (256×256 atlas)
//   box02    texslot=2 (1-based texfaces) → texture index 1 (64×64 glow)
//   cyl01    texslot=0 (no texture) → split into tinted shockwave bands
//
// Mirrors FireBallSubObjTextureSlot at src/effect.cpp:6767-6782. The
// texfaces[] table is 9 entries (slot 0 = no-tex, slots 1..8 = textures).
// =========================================================================
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

static bool ReadTexture(T3DImagery* img3d, int32_t tex_idx, S3DTex& out)
{
    if (!img3d || tex_idx < 0) return false;
    S3DTex tex = {};
    img3d->GetTexture(tex_idx, &tex);
    if (tex.htexture == kInvalidTexture) return false;
    out = tex;
    return true;
}

static bool TextureMatchesSquarePixels(const S3DTex& tex, int32_t pixels)
{
    return tex.htexture != kInvalidTexture &&
           int32_t(tex.desc.width) == pixels &&
           int32_t(tex.desc.height) == pixels;
}

static int32_t ResolveTextureByDimensions(T3DImagery* img3d,
                                          int32_t primary_idx,
                                          int32_t retail_idx,
                                          int32_t expected_pixels,
                                          S3DTex& out)
{
    S3DTex tex = {};
    if (ReadTexture(img3d, primary_idx, tex) &&
        TextureMatchesSquarePixels(tex, expected_pixels))
    {
        out = tex;
        return primary_idx;
    }
    if (retail_idx != primary_idx &&
        ReadTexture(img3d, retail_idx, tex) &&
        TextureMatchesSquarePixels(tex, expected_pixels))
    {
        out = tex;
        return retail_idx;
    }

    const int32_t num_tex = img3d ? img3d->NumTextures() : 0;
    for (int32_t i = 0; i < num_tex; ++i)
    {
        if (i == primary_idx || i == retail_idx) continue;
        if (ReadTexture(img3d, i, tex) &&
            TextureMatchesSquarePixels(tex, expected_pixels))
        {
            out = tex;
            return i;
        }
    }
    return -1;
}

// =========================================================================
// FireBallData — per-quad state (mirrors missileeffect.h:179-187).
// =========================================================================
struct FireBallData
{
    float   pos[3]   = {0, 0, 0};       // local offset from effect world pos
    float   scale    = kMaxSize / 2.0f; // 0.30 init (forensics §3, :519)
    int32_t rotation = 0;                // in-plane spin in DEGREES (0..359)
    float   frame    = 0.0f;             // atlas frame index (float; wraps at kMaxFrame)
    float   glow     = 1.5f;             // glow size multiplier
    int32_t used     = 0;                // burst-only: 1=active
};

// =========================================================================
// SparkParticle — inline TSubParticleAnimator particle.
// (effectcomp.cpp:361-570)
// =========================================================================
struct SparkParticle
{
    bool  used            = false;
    float pos[3]          = {0, 0, 0};   // absolute world pos
    float velocity[3]     = {0, 0, 0};   // wu/tick
    float scale[3]        = {0, 0, 0};   // per-axis (isotropic for sparks)
    float scale_dec[3]    = {0, 0, 0};
    float gravity         = kSparkGravity;
    int32_t life          = 0;
    bool  flicker         = false;
    int32_t flicker_status= 0;
    float flicker_size    = 1.0f;
};

// =========================================================================
// State — per-instance state, mirroring TFireBallAnimator (missileeffect.h:
// 193-205) + harness bookkeeping.
// =========================================================================
struct State
{
    // Core visual state
    FireBallData fireball;                  // the main ball
    FireBallData trail[kTrailSize];          // ring-buffer trail
    FireBallData burst[kMaxBurst];          // impact burst quads
    int32_t      trail_valid = 0;            // recorded trail samples only
    int32_t      frame_count = kMaxFrame;
    int32_t      glow_frame  = kGlowFrame;
    int32_t      explode     = 0;            // 0=pre, 1=first-tick, -1=after
    int32_t      firsttime   = 0;            // state-entry edge
    int32_t      old_state   = kStateLaunch;

    // Shockwave ring (TShockAnimator-equivalent state)
    bool   ring_active       = false;
    bool   ring_done         = true;
    float  ring_scale        = kRingInitScale;
    float  ring_pos[3]       = {0, 0, 0};   // world pos

    // Spark sub-particle pool (inline TSubParticleAnimator)
    SparkParticle sparks[kMaxSpark];

    // Effect world position (tracks ball travel during FLY)
    // Initial: base_pos + (0,0,50) per missileeffect.cpp:513-514 z-lift.
    float  effect_pos[3] = {0, 0, 50};

    // Test-harness state machine (replaces TMissileEffect::Pulse)
    int32_t missile_state  = kStateLaunch;
    int32_t state_tick     = 0;
    float   facing_byte    = 0.0f;          // 0..255 caster-facing (snapshot byte-angle)

    // Effect spawn world pos
    float  base_pos[3] = {0, 0, 0};

    // Launch & impact landmarks (computed at Initialize; used by diagnostics
    // and not by the sim itself). Both are world-space positions; in the
    // test rig they should differ by ~kFlyTicks * kFlySpeedWuPerTick wu so
    // the user can verify the LAUNCH/grow phase is at the source and the
    // EXPLODE/ring phase is at the target.
    float  launch_pos[3] = {0, 0, 0};
    float  impact_pos_predicted[3] = {0, 0, 0};

    // Lifecycle
    bool alive             = true;
    bool spawn_log_done    = false;
    bool ring_log_done     = false;   // log first SubmitWorldImpl ring draw once
    bool fly_transition_log_done    = false;   // log LAUNCH→FLY edge once
    bool explode_transition_log_done = false;  // log FLY→EXPLODE edge once
    int32_t ring_draw_count           = 0;     // counts each SubmitWorldImpl ring draw

    // Framerate-independence
    float tick_accum = 0.0f;

    // Deterministic test animation RNG.
    uint32_t rng_state = kHarnessRngSeed;

    // Resolved asset data
    T3DImagery*    imagery       = nullptr;
    TTextureHandle box01_tex     = kInvalidTexture;  // 4×4 fire atlas
    TTextureHandle box02_tex     = kInvalidTexture;  // spark glow blob
    bool           box02_tex_verified = false;       // true only for 64×64 blob
    MeshHandle     ring_mesh[kRingBandCount] = {};   // cylinder01 bands (Phase B)
    int32_t        ring_band_count = 0;
    d3d::Matrix    ring_parent_mat;                  // cylinder01 authored object matrix
};

static bool ResolveSparkTexture(State* st, bool log_rebind)
{
    if (!st || !st->imagery) return false;

    const int32_t slot = ResolveSubObjTextureSlot(st->imagery, kSubSpark);
    const int32_t primary = (slot >= 0) ? slot : 1;
    S3DTex tex = {};
    const int32_t actual = ResolveTextureByDimensions(st->imagery,
                                                      primary,
                                                      1,
                                                      kSparkTexturePixels,
                                                      tex);
    if (actual < 0)
        return false;

    if (log_rebind && st->box02_tex != tex.htexture)
    {
        log_info("[fireball-shim] box02 lazy rebind -> texidx=%d handle=%u %ux%u",
                 actual, tex.htexture, tex.desc.width, tex.desc.height);
    }
    st->box02_tex = tex.htexture;
    st->box02_tex_verified = true;
    return true;
}

static bool BuildRingBandIndices(const std::vector<uint16_t>& src_indices,
                                 int32_t band,
                                 std::vector<uint16_t>& out)
{
    out.clear();
    for (size_t i = 0; i + 2 < src_indices.size(); i += 3)
    {
        const int32_t g0 = int32_t(src_indices[i + 0]) / kRingSegments;
        const int32_t g1 = int32_t(src_indices[i + 1]) / kRingSegments;
        const int32_t g2 = int32_t(src_indices[i + 2]) / kRingSegments;

        int32_t min_g = g0;
        int32_t max_g = g0;
        if (g1 < min_g) min_g = g1;
        if (g2 < min_g) min_g = g2;
        if (g1 > max_g) max_g = g1;
        if (g2 > max_g) max_g = g2;

        if (min_g == band && max_g == band + 1)
        {
            out.push_back(src_indices[i + 0]);
            out.push_back(src_indices[i + 1]);
            out.push_back(src_indices[i + 2]);
        }
    }
    return !out.empty();
}

static void FillRingBandColor(int32_t band, int32_t band_count,
                              float fade, float out_rgba[4])
{
    if (band_count == kRingBandCount && band >= 0 && band < kRingBandCount)
    {
        for (int32_t c = 0; c < 4; ++c)
            out_rgba[c] = 0.5f * (kRingStops[band][c] + kRingStops[band + 1][c]);
        out_rgba[3] *= fade;
        return;
    }

    for (int32_t c = 0; c < 4; ++c)
    {
        float sum = 0.0f;
        for (int32_t s = 0; s < kRingColorStopCount; ++s)
            sum += kRingStops[s][c];
        out_rgba[c] = sum / float(kRingColorStopCount);
    }
    out_rgba[3] *= fade;
}

// =========================================================================
// CountActiveBurst / CountActiveSparks / IsTrail — death-gate helpers.
// =========================================================================
static int32_t CountActiveBurst(const State* st)
{
    int32_t count = 0;
    for (int32_t i = 0; i < kMaxBurst; ++i)
        if (st->burst[i].used) ++count;
    return count;
}

static int32_t CountActiveSparks(const State* st)
{
    int32_t count = 0;
    for (int32_t i = 0; i < kMaxSpark; ++i)
        if (st->sparks[i].used) ++count;
    return count;
}

// missileeffect.cpp:781-791
static bool IsTrail(const State* st)
{
    if (st->trail_valid <= 1)
        return false;
    const int32_t tail = (st->trail_valid < kTrailSize) ? st->trail_valid - 1 : kTrailSize - 1;
    if (st->trail[tail].pos[0] != st->trail[0].pos[0]) return true;
    if (st->trail[tail].pos[1] != st->trail[0].pos[1]) return true;
    if (st->trail[tail].pos[2] != st->trail[0].pos[2]) return true;
    return false;
}

// =========================================================================
// SpawnSparks — inline TSubParticleAnimator::Create semantics.
// effectcomp.cpp:455-524
// =========================================================================
static void SpawnSparks(State* st, int32_t target_particles, int32_t chance_pct,
                         float vel_spread_xy, float vel_spread_z,
                         const float emit_pos[3])
{
    int32_t active   = CountActiveSparks(st);
    int32_t to_spawn = target_particles - active;

    for (int32_t i = 0; i < kMaxSpark && to_spawn > 0; ++i)
    {
        if (st->sparks[i].used) continue;

        // effectcomp.cpp:458-459 — chance roll per spawn slot
        if (snap_random(st->rng_state, 1, 100) > chance_pct) continue;

        SparkParticle& p = st->sparks[i];
        p.used = true;

        // effectcomp.cpp:472-474 — emit at ball world pos (pos_spread=0)
        p.pos[0] = emit_pos[0];
        p.pos[1] = emit_pos[1];
        p.pos[2] = emit_pos[2];

        // effectcomp.cpp:477-480
        p.scale[0] = p.scale[1] = p.scale[2] = kSparkScale;
        p.scale_dec[0] = p.scale_dec[1] = p.scale_dec[2] = kSparkScaleDec;

        // effectcomp.cpp:483-508 — isotropic spread (velocity_dir=0)
        p.velocity[0] = float(snap_random(st->rng_state, int32_t(-vel_spread_xy), int32_t(vel_spread_xy)));
        p.velocity[1] = float(snap_random(st->rng_state, int32_t(-vel_spread_xy), int32_t(vel_spread_xy)));
        p.velocity[2] = float(snap_random(st->rng_state, int32_t(-vel_spread_z),  int32_t(vel_spread_z)));

        p.gravity       = kSparkGravity;
        p.life          = snap_random(st->rng_state, kSparkMinLife, kSparkMaxLife);
        p.flicker       = true;
        p.flicker_status= snap_random(st->rng_state, 0, 1);
        p.flicker_size  = kSparkFlickerSize;

        --to_spawn;
    }
}

// =========================================================================
// AnimateSparks — inline TSubParticleAnimator::Animate per-particle update.
// effectcomp.cpp:361-403
// =========================================================================
static void AnimateSparks(State* st)
{
    for (int32_t i = 0; i < kMaxSpark; ++i)
    {
        SparkParticle& p = st->sparks[i];
        if (!p.used) continue;

        p.flicker_status = snap_random(st->rng_state, 0, 1);   // :383
        --p.life;                                 // :386
        if (p.life < 0) { p.used = false; continue; } // :387-388

        p.scale[0] *= p.scale_dec[0];            // :391-393
        p.scale[1] *= p.scale_dec[1];
        p.scale[2] *= p.scale_dec[2];

        p.pos[0] += p.velocity[0];               // :396-398
        p.pos[1] += p.velocity[1];
        p.pos[2] += p.velocity[2];

        p.velocity[2] -= p.gravity;              // :401
    }
}

// =========================================================================
// Initialize — transcribed from missileeffect.cpp:501-546.
// =========================================================================
static void Initialize(State* st)
{
    st->rng_state   = kHarnessRngSeed;
    st->trail_valid = 0;

    // missileeffect.cpp:505 — state = LAUNCH
    st->missile_state = kStateLaunch;
    st->old_state     = kStateLaunch;

    // missileeffect.cpp:513-514 — z-lift +50 (ball grows above caster).
    // test harness: short trajectory + in-frame impact — additionally
    // offset the spawn -X so flight crosses world origin and the impact
    // lands centered in the camera frame (see kHarnessSpawnXOff above).
    // In production the caster's GetPos() supplies base_pos and no offset
    // is applied. LAUNCH/grow phase happens AT launch_pos (the source,
    // above the caster). FLY traverses to impact_pos. EXPLODE fires there.
    st->effect_pos[0] = st->base_pos[0] + kHarnessSpawnXOff;
    st->effect_pos[1] = st->base_pos[1];
    st->effect_pos[2] = st->base_pos[2] + 50.0f;

    // Record landmarks for diagnostic logs (sim doesn't use these).
    st->launch_pos[0] = st->effect_pos[0];
    st->launch_pos[1] = st->effect_pos[1];
    st->launch_pos[2] = st->effect_pos[2];
    st->impact_pos_predicted[0] = st->launch_pos[0] + (kFlyTicks * kFlySpeedWuPerTick);
    st->impact_pos_predicted[1] = st->launch_pos[1];
    st->impact_pos_predicted[2] = st->launch_pos[2];

    // missileeffect.cpp:516-530 — ball init
    st->fireball.pos[0] = st->fireball.pos[1] = st->fireball.pos[2] = 0.0f;
    st->fireball.scale    = kMaxSize / 2.0f;  // 0.30 (:519)
    st->fireball.rotation = 0;
    st->fireball.frame    = 0.0f;
    st->fireball.glow     = 1.5f;             // (:530)

    // missileeffect.cpp:526-527
    st->frame_count = kMaxFrame;
    st->glow_frame  = kGlowFrame;

    // missileeffect.cpp:540-543
    st->explode   = 0;
    st->firsttime = 0;
    for (int32_t i = 0; i < kMaxBurst; ++i)
        st->burst[i].used = 0;

    // Trail init (trail copies of ball state)
    for (int32_t i = 0; i < kTrailSize; ++i)
    {
        st->trail[i] = FireBallData{};
        st->trail[i].scale = kMaxSize / 2.0f;
    }

    // Clear sparks
    for (int32_t i = 0; i < kMaxSpark; ++i)
        st->sparks[i] = SparkParticle{};

    // Ring inactive at spawn
    st->ring_active = false;
    st->ring_done   = true;
    st->ring_scale  = kRingInitScale;
}

// =========================================================================
// AnimateTick — per-nominal-24Hz-tick update.
// Transcribed from missileeffect.cpp:549-759 (TFireBallAnimator::Animate).
// =========================================================================
static void AnimateTick(State* st)
{
    // Test-harness state machine (replaces TMissileEffect::Pulse).
    ++st->state_tick;
    if (st->missile_state == kStateLaunch && st->state_tick >= kLaunchTicks)
    {
        // LAUNCH → FLY (in snapshot: animator SetStatus(true) → base flips state)
        st->missile_state = kStateFly;
        st->state_tick    = 0;
        if (!st->fly_transition_log_done)
        {
            log_info("[fireball-shim] LAUNCH -> FLY transition: effect_pos=(%.1f,%.1f,%.1f) "
                     "ball_scale=%.2f sparks=%d (ball should begin moving +X now)",
                     double(st->effect_pos[0]), double(st->effect_pos[1]), double(st->effect_pos[2]),
                     double(st->fireball.scale), CountActiveSparks(st));
            st->fly_transition_log_done = true;
        }
    }
    else if (st->missile_state == kStateFly && st->state_tick >= kFlyTicks)
    {
        st->missile_state = kStateExplode;
        st->state_tick    = 0;
        if (!st->explode_transition_log_done)
        {
            log_info("[fireball-shim] FLY -> EXPLODE transition: effect_pos=(%.1f,%.1f,%.1f) "
                     "ball_scale=%.2f sparks=%d "
                     "(impact pos = effect_pos + fireball.pos = (%.1f,%.1f,%.1f))",
                     double(st->effect_pos[0]), double(st->effect_pos[1]), double(st->effect_pos[2]),
                     double(st->fireball.scale), CountActiveSparks(st),
                     double(st->effect_pos[0] + st->fireball.pos[0]),
                     double(st->effect_pos[1] + st->fireball.pos[1]),
                     double(st->effect_pos[2] + st->fireball.pos[2]));
            st->explode_transition_log_done = true;
        }
    }

    // Update effect world pos during FLY (ball travels +X at FIREBALL_SPEED wu/tick)
    if (st->missile_state == kStateFly)
        st->effect_pos[0] += kFlySpeedWuPerTick;

    const int32_t state = st->missile_state;

    // missileeffect.cpp:558-561 — state-entry edge
    st->firsttime = (st->old_state != state) ? 1 : 0;

    // missileeffect.cpp:563-566 — explode tracker
    if (st->explode == 1)
        st->explode = -1;
    else if (st->explode == 0 && state == kStateExplode)
        st->explode = 1;

    // ─── SPARK PARAMS (missileeffect.cpp:572-624) ─────────────────────────
    int32_t target_particles;
    if (st->explode == -1)      target_particles = 0;
    else if (st->explode == 1)  target_particles = kMaxSpark;       // :577-578
    else                         target_particles = kNormSpark;      // :580

    int32_t chance_pct;
    if (state == kStateLaunch)      chance_pct = 25;  // :583
    else if (state == kStateFly)    chance_pct = 30;  // :585
    else                             chance_pct = 100; // :587

    float vel_spread_xy, vel_spread_z;
    if (state == kStateLaunch) {
        vel_spread_xy = 3.0f;    // :607
        vel_spread_z  = 1.0f;    // :606
    } else {
        vel_spread_xy = 1.0f;    // :610
        vel_spread_z  = 1.0f;
    }

    // Spawn at current ball world pos (missileeffect.cpp:593-596)
    SpawnSparks(st, target_particles, chance_pct, vel_spread_xy, vel_spread_z, st->effect_pos);
    AnimateSparks(st);

    // ─── TRAIL RING-BUFFER SHIFT (missileeffect.cpp:627-636) ──────────────
    for (int32_t i = kTrailSize - 1; i > 0; --i)
    {
        st->trail[i]       = st->trail[i - 1];
        st->trail[i].scale *= kTrailScale;
    }
    st->trail[0]       = st->fireball;
    st->trail[0].scale *= kTrailScale;
    // Record absolute world pos (:634-636)
    st->trail[0].pos[0] += st->effect_pos[0];
    st->trail[0].pos[1] += st->effect_pos[1];
    st->trail[0].pos[2] += st->effect_pos[2];
    if (st->trail_valid < kTrailSize)
        ++st->trail_valid;

    // ─── BALL SELF-ANIMATION (missileeffect.cpp:638-655) ──────────────────
    // Glow flicker × 1.0..1.75 (:639)
    st->fireball.glow = 1.0f + (0.05f * float(snap_random(st->rng_state, 0, 15)));

    // ── FLIPBOOK FRAME ADVANCE (forensics §8) ──
    // Frame advances +1/tick, wrap at frame_count, skip glow_frame (:642-650)
    st->fireball.frame += 1.0f;
    if (st->fireball.frame > float(st->frame_count))
        st->fireball.frame = 0.0f;
    if (int32_t(st->fireball.frame) == st->glow_frame)
    {
        st->fireball.frame += 1.0f;
        if (st->fireball.frame > float(st->frame_count))
            st->fireball.frame = 0.0f;
    }

    // Rotation +2°/tick (:654 — normal path; +12 in WHITE_FADE mode)
    st->fireball.rotation = (st->fireball.rotation + 2) % 360;

    // ─── STATE MACHINE (missileeffect.cpp:657-757) ────────────────────────
    int32_t burst_live_count = 0;

    switch (state)
    {
        case kStateLaunch:
            // missileeffect.cpp:661-663 — animator SetStatus(true) → base launches
            break;

        case kStateFly:
            // missileeffect.cpp:665-668 — grow toward max size
            if (st->fireball.scale < kMaxSize)
                st->fireball.scale += kGrowRate;
            break;

        case kStateExplode:
            if (st->fireball.scale < kMaxSize)
                st->fireball.scale += kGrowRate;   // :671-673

            if (st->firsttime)
            {
                // ── Impact (missileeffect.cpp:674-731) — fires ONCE ─────────
                // BlastCharactersInRange: no MapPane in test rig → skip damage.
                // PLAY("fireball explosion") / FIREHIT: no audio → skip.

                // Burst spawn (:687-699)
                for (int32_t i = 0; i < kMaxBurst; ++i)
                {
                    st->burst[i].used = 1;
                    st->burst[i].pos[0] = st->fireball.pos[0] + float(snap_random(st->rng_state, -15, 20));
                    st->burst[i].pos[1] = st->fireball.pos[1] + float(snap_random(st->rng_state, -15, 20));
                    st->burst[i].pos[2] = st->fireball.pos[2] + float(snap_random(st->rng_state, -15, 20));
                    st->burst[i].scale = 0.75f + float(snap_random(st->rng_state, 0, 5)) * 0.15f;
                    st->burst[i].rotation = 0;
                    st->burst[i].glow = 1.0f + (0.05f * float(snap_random(st->rng_state, 0, 15)));
                    st->burst[i].frame = float(snap_random(st->rng_state, 0, st->frame_count - 1));
                    if (int32_t(st->burst[i].frame) == st->glow_frame)
                        ++st->burst[i].frame;
                }

                // Ring init (missileeffect.cpp:701-731) — TShockAnimator::Set
                st->ring_pos[0] = st->fireball.pos[0] + st->effect_pos[0];
                st->ring_pos[1] = st->fireball.pos[1] + st->effect_pos[1];
                st->ring_pos[2] = st->fireball.pos[2] + st->effect_pos[2];
                st->ring_scale  = kRingInitScale;
                st->ring_active = true;
                st->ring_done   = false;

                log_info("[fireball-shim] EXPLODE entered impact_pos=(%.1f,%.1f,%.1f) "
                         "ring_pos=(%.1f,%.1f,%.1f) ring_scale=%.1f ring_bands=%d ring_will_draw=%d",
                         double(st->fireball.pos[0] + st->effect_pos[0]),
                         double(st->fireball.pos[1] + st->effect_pos[1]),
                         double(st->fireball.pos[2] + st->effect_pos[2]),
                         double(st->ring_pos[0]), double(st->ring_pos[1]), double(st->ring_pos[2]),
                         double(st->ring_scale), st->ring_band_count,
                         int(st->ring_band_count > 0 && st->ring_mesh[0] != 0 &&
                             st->ring_active && !st->ring_done));
            }
            else
            {
                // Ring grow (effectcomp.cpp:641-657 — TShockAnimator::Animate)
                if (st->ring_active && !st->ring_done)
                {
                    st->ring_scale *= kRingGrowFactor;
                    if (st->ring_scale > kRingMaxSize)
                        st->ring_done = true;
                }

                // Burst update (missileeffect.cpp:736-749)
                for (int32_t i = 0; i < kMaxBurst; ++i)
                {
                    if (!st->burst[i].used) continue;
                    ++burst_live_count;
                    st->burst[i].scale *= 0.90f;       // :741
                    if (st->burst[i].scale < 0.50f)    // :743
                    {
                        st->burst[i].used = 0;
                        --burst_live_count;
                        continue;
                    }
                    st->burst[i].glow = 1.0f + (0.05f * float(snap_random(st->rng_state, 0, 15)));
                    ++st->burst[i].frame;
                    if (int32_t(st->burst[i].frame) == st->glow_frame)
                        ++st->burst[i].frame;
                    st->burst[i].frame = float(int32_t(st->burst[i].frame) % st->frame_count);
                }
            }

            // Death gate (missileeffect.cpp:752-755)
            if (!IsTrail(st) && CountActiveSparks(st) == 0 &&
                burst_live_count == 0 && st->ring_done)
            {
                log_info("[fireball-shim] KillThisEffect");
                st->alive = false;
            }
            break;
    }

    st->old_state = state;
}

// =========================================================================
// Animate — framerate-independent entry.
// Per feedback-framerate-independent-anim: convert per-tick deltas to
// per-second integration via DeltaTime() accumulator.
// =========================================================================
static void Animate(State* st)
{
    if (!st->alive) return;

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
// SubmitBillboardQuad — emit one SParticleDrawItem for a ball/glow/trail/
// burst quad. Uses ScreenAligned orientation (camera-facing billboard) +
// per-instance rotation_rad (in-plane spin from snapshot's +2°/tick).
//
// ORIENTATION: ScreenAligned (NOT WorldXY).
// The snapshot's matrix `RotZ(spin)·RotX(-30°)·RotY(+60°)·RotZ(facing)·…`
// was a 1998 D3D fake-billboard hack — the fixed-tilt RotX+RotY plane was
// the closest approximation to a camera-facing quad the fixed-function
// pipeline could express without per-frame view-matrix introspection.
// Our SubmitFxParticle ScreenAligned path supersedes that hack: the
// engine billboards the quad to face the camera natively per frame.
// Round-2's WorldXY choice put the ball flat on the ground; ScreenAligned
// is correct for air sprites (matches fizzle, blood FLY, sparks family).
// Forensics §13.6 flagged the exact tilt as snapshot ambiguity; we
// resolve it ScreenAligned per the in-air convention.
//
// The facing_rad term (RotZ(-facing/256·2π)) from the snapshot is dropped
// here. It was the caster-facing direction, meaningful only when the
// in-plane RotX/RotY hack pinned the quad to a world axis; for a true
// ScreenAligned billboard the camera frame already supplies "up", so a
// caster-facing offset is moot. We keep ONLY the per-instance +2°/tick
// in-plane spin (tumbling animation), which reads correctly as the
// fireball rolling toward the viewer.
//
// THE UV RECT IS THE FLIPBOOK CELL (forensics §8).
//
// BLEND: Alpha for every box01 draw. Forensics §7 traces the animator's
// SaveBlendState(); SetBlendState(); ... RestoreBlendState() bracket to
// SRCALPHA/INVSRCALPHA, and RenderFireBallBurst does not override it.
// =========================================================================
static void SubmitBillboardQuad(State* /*st*/,
                                  TTextureHandle texture,
                                  int32_t frame_idx,
                                  float rotation_deg,
                                  float size_wu,
                                  const float world_pos[3])
{
    if (texture == kInvalidTexture) return;
    if (size_wu <= 0.0f) return;

    SParticleDrawItem item = {};
    item.world_pos[0] = world_pos[0];
    item.world_pos[1] = world_pos[1];
    item.world_pos[2] = world_pos[2];
    item.size_wu[0]   = size_wu;
    item.size_wu[1]   = size_wu;

    // Atlas cell sub-rect (forensics §8 — 4×4 grid, cell 0.25 × 0.25)
    FillAtlasUv(frame_idx, item.uv_rect);

    // White diffuse — warm orange comes from atlas texels (forensics §10)
    item.color_rgba[0] = item.color_rgba[1] = item.color_rgba[2] = item.color_rgba[3] = 1.0f;

    // Per-instance in-plane spin only. The snapshot's matrix `RotZ(-rotation·
    // TORADIAN)` (missileeffect.cpp:846) survives in the engine path as a
    // rotation around the screen-plane axis (camera-Z under ScreenAligned).
    // facing_rad (snapshot :851) is dropped — it composed with the matrix
    // tilt that's no longer present.
    const float kDegToRad = float(M_PI) / 180.0f;
    item.rotation_rad     = -rotation_deg * kDegToRad;

    item.key.texture     = texture;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    // Alpha per forensics §7 (snapshot bracket: SaveBlendState();
    // SetBlendState(); ... RestoreBlendState() at missileeffect.cpp:1062-
    // 1082 → MODULATE + SRCALPHA/INVSRCALPHA). The 4×4 atlas is authored
    // with chroma-key transparency on opaque fire pixels; Alpha composites
    // crisp fire shapes onto the scene. Additive stacked overlapping
    // ball/glow/trail/burst cards into yellow-red saturation.
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    // ScreenAligned: camera-facing billboard. Air sprites face the camera
    // per the family convention (fizzle, blood, sparks). The snapshot's
    // RotX(-30°)+RotY(+60°) tilt hack is superseded.
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = EFxDebugMode::Normal;
    Renderer->SubmitFxParticle(item);
}

// =========================================================================
// SubmitSparkQuad — emit one spark particle (box02 quad).
// effectcomp.cpp:526-570 (TSubParticleAnimator::Render).
//
// ORIENTATION: ScreenAligned (camera-facing). Sparks are in-air embers, not
// ground decals — same family convention as fizzle / blood FLY droplets /
// sparks: small bright air sprites face the camera.
// =========================================================================
static void SubmitSparkQuad(State* st, const SparkParticle& p)
{
    if (st->box02_tex == kInvalidTexture) return;

    const float cur_scale = p.flicker && p.flicker_status
                              ? p.scale[0] * p.flicker_size
                              : p.scale[0];
    if (cur_scale <= 0.0f) return;

    SParticleDrawItem item = {};
    item.world_pos[0] = p.pos[0];
    item.world_pos[1] = p.pos[1];
    item.world_pos[2] = p.pos[2];
    item.size_wu[0] = kSparkQuadWu * cur_scale;
    item.size_wu[1] = kSparkQuadWu * cur_scale;
    // Spark texture is a single soft glow blob (64×64) — full quad sample
    item.uv_rect[0] = 0.0f;
    item.uv_rect[1] = 0.0f;
    item.uv_rect[2] = 1.0f;
    item.uv_rect[3] = 1.0f;
    item.color_rgba[0] = item.color_rgba[1] = item.color_rgba[2] = item.color_rgba[3] = 1.0f;
    item.rotation_rad = 0.0f;    // sparks: no per-particle spin in snapshot

    item.key.texture     = st->box02_tex;
    item.key.pipeline_id = uint16_t(EFxPipeline::Particle);
    // Alpha per forensics §7 — sparks share the main render's blend bracket
    // (effectcomp.cpp:526-570 runs inside the animator's SetBlendState()
    // scope at missileeffect.cpp:1062-1082). Round-5's additive override
    // saturated the trail and was reverted in round 6. The 64×64 spark
    // glow blob has a black-keyed soft halo; Alpha composites it crisply.
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode      = EFxLightMode::Unlit;
    item.orientation     = EFxBillboardOrientation::ScreenAligned;
    item.debug_mode      = EFxDebugMode::Normal;
    Renderer->SubmitFxParticle(item);
}

// =========================================================================
// SubmitImpl — emit all billboard draws for this frame.
// Transcribed from TFireBallAnimator::Render (missileeffect.cpp:1060-1085).
//
// Draw order (snapshot :1065-1080):
//   spark.Render()                                       — always (first)
//   LAUNCH/FLY: RenderFireBallGlow, RenderFireBallTrail, RenderFireBall
//   EXPLODE:    RenderFireBallTrail, RenderFireBallBurst, ring.Render()
// =========================================================================
static void SubmitImpl(State* st)
{
    if (!Renderer || !st->alive) return;
    if (st->box01_tex == kInvalidTexture) return;
    if (!st->box02_tex_verified)
        ResolveSparkTexture(st, /*log_rebind=*/true);

    if (!st->spawn_log_done)
    {
        log_info("[fireball-shim] first Submit: state=%d effect_pos=(%.0f,%.0f,%.0f) "
                 "ball_frame=%d scale=%.2f rot=%d glow=%.2f box01_tex=%u box02_tex=%u",
                 st->missile_state,
                 double(st->effect_pos[0]),
                 double(st->effect_pos[1]),
                 double(st->effect_pos[2]),
                 int32_t(st->fireball.frame),
                 double(st->fireball.scale),
                 st->fireball.rotation,
                 double(st->fireball.glow),
                 st->box01_tex, st->box02_tex);
        st->spawn_log_done = true;
    }

    const int32_t state = st->missile_state;

    // ── SPARKS (missileeffect.cpp:1065 — spark.Render()) ──────────────────
    for (int32_t i = 0; i < kMaxSpark; ++i)
    {
        if (st->sparks[i].used)
            SubmitSparkQuad(st, st->sparks[i]);
    }

    if (state == kStateLaunch || state == kStateFly)
    {
        // ── GLOW (RenderFireBallGlow — missileeffect.cpp:1022-1057) ────────
        // box01 at glow_frame (cell 3), scale × glow. No per-instance spin.
        {
            const float gsize = kBaseQuadWu * st->fireball.scale * st->fireball.glow;
            float gpos[3] = {
                st->fireball.pos[0] + st->effect_pos[0],
                st->fireball.pos[1] + st->effect_pos[1],
                st->fireball.pos[2] + st->effect_pos[2]
            };
            SubmitBillboardQuad(st, st->box01_tex, st->glow_frame,
                                /*rotation_deg=*/0.0f, gsize, gpos);
        }

        // ── TRAIL (RenderFireBallTrail — missileeffect.cpp:871-973) ────────
        // Snapshot draws two passes for each recorded slot: glow_frame at
        // scale*glow, then the stale ball frame at scale. Restrict to slots
        // that have actually been recorded so the bounded test trajectory
        // does not show a zero-initialized pre-impact ghost at world origin.
        for (int32_t i = st->trail_valid - 1; i >= 0; --i)
        {
            const FireBallData& tr = st->trail[i];
            if (tr.scale <= 0.0001f) continue;
            // Trail pos was set absolute (= effect_pos at time of recording).
            const float tsize = kBaseQuadWu * tr.scale * tr.glow;
            SubmitBillboardQuad(st, st->box01_tex, st->glow_frame,
                                /*rotation_deg=*/0.0f, tsize, tr.pos);
        }
        for (int32_t i = st->trail_valid - 1; i >= 0; --i)
        {
            const FireBallData& tr = st->trail[i];
            if (tr.scale <= 0.0001f) continue;
            const float tsize = kBaseQuadWu * tr.scale;
            SubmitBillboardQuad(st, st->box01_tex, int32_t(tr.frame),
                                float(tr.rotation), tsize, tr.pos);
        }

        // ── BALL (RenderFireBall — missileeffect.cpp:975-1020) ─────────────
        {
            const float bsize = kBaseQuadWu * st->fireball.scale;
            float bpos[3] = {
                st->fireball.pos[0] + st->effect_pos[0],
                st->fireball.pos[1] + st->effect_pos[1],
                st->fireball.pos[2] + st->effect_pos[2]
            };
            // FLIPBOOK: int32_t(fireball.frame) picks the current 4×4 cell
            SubmitBillboardQuad(st, st->box01_tex, int32_t(st->fireball.frame),
                                float(st->fireball.rotation), bsize, bpos);
        }
    }
    else /* kStateExplode */
    {
        // ── TRAIL (still draining; same glow/core two-pass order) ─────────
        for (int32_t i = st->trail_valid - 1; i >= 0; --i)
        {
            const FireBallData& tr = st->trail[i];
            if (tr.scale <= 0.0001f) continue;
            const float tsize = kBaseQuadWu * tr.scale * tr.glow;
            SubmitBillboardQuad(st, st->box01_tex, st->glow_frame,
                                /*rotation_deg=*/0.0f, tsize, tr.pos);
        }
        for (int32_t i = st->trail_valid - 1; i >= 0; --i)
        {
            const FireBallData& tr = st->trail[i];
            if (tr.scale <= 0.0001f) continue;
            const float tsize = kBaseQuadWu * tr.scale;
            SubmitBillboardQuad(st, st->box01_tex, int32_t(tr.frame),
                                float(tr.rotation), tsize, tr.pos);
        }

        // ── BURST (RenderFireBallBurst — missileeffect.cpp:793-869) ────────
        // Snapshot draws glow_frame first (rot.z=-60°, scale*glow), then the
        // random stale burst frame at scale. Both inherit the animator's
        // SetBlendState Alpha bracket.
        for (int32_t i = 0; i < kMaxBurst; ++i)
        {
            if (!st->burst[i].used) continue;
            float bpos[3] = {
                st->burst[i].pos[0] + st->effect_pos[0],
                st->burst[i].pos[1] + st->effect_pos[1],
                st->burst[i].pos[2] + st->effect_pos[2]
            };
            const float gsize = kBaseQuadWu * st->burst[i].scale * st->burst[i].glow;
            SubmitBillboardQuad(st, st->box01_tex, st->glow_frame,
                                /*rotation_deg=*/60.0f, gsize, bpos);

            const float bsize = kBaseQuadWu * st->burst[i].scale;
            SubmitBillboardQuad(st, st->box01_tex, int32_t(st->burst[i].frame),
                                float(st->burst[i].rotation), bsize, bpos);
        }
        // (Ring is drawn from SubmitWorld.)
    }
}

// =========================================================================
// SubmitWorldImpl — submit the shockwave ring (cylinder01) via helper mesh.
// MUST be called from submit_world callback (gotcha #4).
//
// TShockAnimator::Render sets Alpha (SRCALPHA/INVSRCALPHA) and paints a
// 4-stop per-vertex ARGB gradient. SHelperMeshSubmit cannot override vertex
// colors, so the shim splits the 4-ring cylinder into 3 annular bands and
// submits each band with the adjacent retail color-stop average.
// =========================================================================
static void SubmitWorldImpl(State* st)
{
    if (!Renderer || !st->alive) return;
    if (!st->ring_active || st->ring_done) return;
    if (st->ring_band_count <= 0 || st->ring_mesh[0] == 0) return;

    ++st->ring_draw_count;

    if (!st->ring_log_done)
    {
        log_info("[fireball-shim] shockwave ring FIRST submit: "
                 "orient=WorldXY(helper-mesh, cylinder01 authored flat in XY plane) "
                 "pos=(%.1f,%.1f,%.1f) radius=%.1f bands=%d alpha_blend=1 "
                 "color_stops=4 alpha_stops=(0,1,.75,0)",
                 double(st->ring_pos[0]), double(st->ring_pos[1]), double(st->ring_pos[2]),
                 double(st->ring_scale), st->ring_band_count);
        st->ring_log_done = true;
    }
    // Periodic ring growth log every 8 draws so we can confirm the ring
    // expands (radius should grow from 25 → 120 over ~19 ticks).
    if ((st->ring_draw_count % 8) == 1)
    {
        log_info("[fireball-shim] shockwave ring draw #%d: radius=%.1f (fade gate: %s)",
                 st->ring_draw_count, double(st->ring_scale),
                 (st->ring_scale >= kRingMaxSize) ? "DONE" : "growing");
    }

    // Compose world matrix: Translate(ring_pos) * Scale(ring_scale, *, *) ·
    // cylinder01's authored object matrix.
    // Per missileeffect.cpp:715-717, ring grows isotropic on x/y/z.
    float t[16] = {
        st->ring_scale, 0.0f,           0.0f,           st->ring_pos[0],
        0.0f,           st->ring_scale, 0.0f,           st->ring_pos[1],
        0.0f,           0.0f,           st->ring_scale, st->ring_pos[2],
        0.0f,           0.0f,           0.0f,           1.0f
    };
    float world[16];
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t k = 0; k < 4; ++k)
                s += t[r * 4 + k] * st->ring_parent_mat.m[k * 4 + c];
            world[r * 4 + c] = s;
        }

    // Alpha fade per SHOCKWAVE_FLAG_FADE (effectcomp.cpp:732-748):
    //   alpha = (max_size - scale) / (max_size - init_scale)
    const float fade_num = kRingMaxSize - st->ring_scale;
    const float fade_den = kRingMaxSize - kRingInitScale;
    const float fade     = (fade_den > 0.0f)
                              ? std::fmax(0.0f, std::fmin(1.0f, fade_num / fade_den))
                              : 1.0f;
    for (int32_t band = 0; band < st->ring_band_count; ++band)
    {
        if (st->ring_mesh[band] == 0) continue;

        float rgba[4] = {};
        FillRingBandColor(band, st->ring_band_count, fade, rgba);

        SHelperMeshSubmit m = {};
        m.mesh           = st->ring_mesh[band];
        m.additive_blend = false;  // TShockAnimator uses SRCALPHA/INVSRCALPHA
        m.shadow_plane   = false;
        std::memcpy(m.world, world, sizeof(world));
        m.diffuse[0]  = 0.0f; m.diffuse[1]  = 0.0f; m.diffuse[2]  = 0.0f; m.diffuse[3]  = rgba[3];
        m.ambient[0]  = 0.0f; m.ambient[1]  = 0.0f; m.ambient[2]  = 0.0f; m.ambient[3]  = rgba[3];
        m.specular[0] = 0.0f; m.specular[1] = 0.0f; m.specular[2] = 0.0f; m.specular[3] = rgba[3];
        m.emissive[0] = rgba[0];
        m.emissive[1] = rgba[1];
        m.emissive[2] = rgba[2];
        m.emissive[3] = rgba[3];
        m.power       = 1.0f;
        m.sort_depth  = st->ring_pos[2] + float(band) * 0.001f;

        Renderer->SubmitHelperMesh(m);
    }
}

// =========================================================================
// CaptureRingParentMatrix — extract the authored static-pose matrix for the
// cylinder01 sub-object. The 3dimage API gives us this via the sub-object's
// vertex authoring (the renderer-managed mesh handle is already at world).
// We need the parent transform separately so the ring grows by scaling
// around the cylinder's authored origin, not around (0,0,0).
//
// Falls back to identity if no authored transform is available; the ring
// will then scale around world origin (visually still correct since the
// snapshot's TShockAnimator does its scale around params.pos, which we
// translate to directly via the world matrix).
// =========================================================================
static void CaptureRingParentMatrix(T3DImagery* /*img3d*/, int32_t /*sub_obj*/, d3d::Matrix& out)
{
    // Cylinder01 authored verts are within ±2.09 wu of origin (manifest);
    // the ring's visual position is set by ring_pos via the world matrix
    // we compose in SubmitWorldImpl. Identity parent works because the
    // mesh's authored verts are already centered around its local origin.
    d3d::MatrixIdentity(out);
}

// =========================================================================
// Spawn — load asset, resolve sub-object textures, run Initialize.
// =========================================================================
State* Spawn(const S3DPoint& origin, float facing_byte)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0]  = float(origin.x);
    st->base_pos[1]  = float(origin.y);
    st->base_pos[2]  = float(origin.z);
    st->facing_byte  = facing_byte;

    // Bind I3D (forensics §4 — Class.Def:2031 "FireBall" → NewFireBall.I3D)
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[fireball-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fireball-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    const int32_t num_tex = img3d->NumTextures();

    // Resolve box01 (ball atlas) via texfaces[].
    {
        const int32_t slot = ResolveSubObjTextureSlot(img3d, kSubBall);
        int32_t actual = (slot >= 0) ? slot : 0;
        S3DTex tex = {};
        const int32_t resolved = ResolveTextureByDimensions(img3d,
                                                            actual,
                                                            0,
                                                            kBallAtlasPixels,
                                                            tex);
        if (resolved >= 0)
        {
            actual = resolved;
        }
        else if (!ReadTexture(img3d, actual, tex) &&
                 actual != 0 &&
                 ReadTexture(img3d, 0, tex))
        {
            actual = 0;
        }
        st->box01_tex = tex.htexture;
        log_info("[fireball-shim] box01 sub=%d -> texidx=%d handle=%u %ux%u",
                 kSubBall, actual, tex.htexture, tex.desc.width, tex.desc.height);
    }

    // Resolve box02 (spark glow) via texfaces[].
    {
        const int32_t slot = ResolveSubObjTextureSlot(img3d, kSubSpark);
        int32_t actual = (slot >= 0) ? slot : 1;
        S3DTex tex = {};
        const int32_t resolved = ResolveTextureByDimensions(img3d,
                                                            actual,
                                                            1,
                                                            kSparkTexturePixels,
                                                            tex);
        if (resolved >= 0)
        {
            actual = resolved;
            st->box02_tex = tex.htexture;
            st->box02_tex_verified = true;
        }
        else
        {
            // Never bind the 256x256 ball atlas as box02: it produces a
            // retail-visible first-spawn spark mismatch. Leave sparks quiet
            // until the 64x64 texture can be resolved by the lazy retry path.
            ReadTexture(img3d, actual, tex);
            st->box02_tex = kInvalidTexture;
            log_warn("[fireball-shim] box02 64x64 spark texture unresolved at Spawn "
                     "(candidate texidx=%d handle=%u %ux%u)",
                     actual, tex.htexture, tex.desc.width, tex.desc.height);
        }
        log_info("[fireball-shim] box02 sub=%d -> texidx=%d handle=%u %ux%u verified=%d",
                 kSubSpark, actual, tex.htexture, tex.desc.width, tex.desc.height,
                 int(st->box02_tex_verified));
    }

    if (st->box01_tex == kInvalidTexture)
    {
        log_warn("[fireball-shim] box01 tex unresolved at Spawn");
    }
    if (st->box02_tex == kInvalidTexture)
    {
        log_warn("[fireball-shim] box02 tex unresolved at Spawn; sparks will lazy-resolve");
    }

    // Register cylinder01 mesh for the ring (Phase B). Manifest:
    // texslot=0, material[2].texture=-1 → no authored texture. Snapshot
    // TShockAnimator supplies the visual color through per-vertex ARGB. The
    // helper mesh path lacks vertex colors, so split the known 4x24 vertex
    // cylinder into 3 annular bands and tint each band independently.
    {
        bool got_mesh = false;
        const int32_t texslots = num_tex + 1;
        for (int32_t texslot = 0; texslot < texslots && !got_mesh; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(img3d, kSubRing, texslot, verts, indices))
                continue;
            if (verts.empty() || indices.empty()) continue;
            const TTextureHandle albedo = Renderer->WhiteTextureHandle();

            std::vector<uint16_t> band_indices[kRingBandCount];
            bool split_ok = int32_t(verts.size()) >= kRingColorStopCount * kRingSegments;
            for (int32_t band = 0; band < kRingBandCount && split_ok; ++band)
                split_ok = BuildRingBandIndices(indices, band, band_indices[band]);

            if (split_ok)
            {
                MeshHandle meshes[kRingBandCount] = {};
                bool all_ok = true;
                for (int32_t band = 0; band < kRingBandCount; ++band)
                {
                    meshes[band] = Renderer->RegisterMesh(
                        verts.data(), int32_t(verts.size()),
                        band_indices[band].data(), int32_t(band_indices[band].size()),
                        albedo);
                    if (meshes[band] == 0)
                        all_ok = false;
                }
                if (all_ok)
                {
                    for (int32_t band = 0; band < kRingBandCount; ++band)
                        st->ring_mesh[band] = meshes[band];
                    st->ring_band_count = kRingBandCount;
                    log_info("[fireball-shim] cylinder01 sub=%d texslot=%d "
                             "verts=%zu idx=%zu bands=3 meshes=(%u,%u,%u) albedo=%u",
                             kSubRing, texslot, verts.size(), indices.size(),
                             st->ring_mesh[0], st->ring_mesh[1], st->ring_mesh[2],
                             albedo);
                    got_mesh = true;
                }
            }

            if (!got_mesh)
            {
                st->ring_mesh[0] = Renderer->RegisterMesh(
                    verts.data(), int32_t(verts.size()),
                    indices.data(), int32_t(indices.size()),
                    albedo);
                if (st->ring_mesh[0] != 0)
                {
                    st->ring_band_count = 1;
                    log_info("[fireball-shim] cylinder01 sub=%d texslot=%d "
                             "verts=%zu idx=%zu bands=1 mesh=%u albedo=%u",
                             kSubRing, texslot, verts.size(), indices.size(),
                             st->ring_mesh[0], albedo);
                    got_mesh = true;
                }
            }
        }
        if (st->ring_band_count <= 0 || st->ring_mesh[0] == 0)
            log_warn("[fireball-shim] cylinder01 mesh registration failed; ring won't draw");
    }
    CaptureRingParentMatrix(img3d, kSubRing, st->ring_parent_mat);

    // Run Initialize (seeds ball, trail, burst, spark pools)
    Initialize(st);

    log_info("[fireball-shim] Spawn ok: alive=%d box01_tex=%u box02_tex=%u ring_bands=%d ring_mesh0=%u "
             "base_pos=(%d,%d,%d) facing=%.0f effect_pos=(%.0f,%.0f,%.0f)",
             int(st->alive), st->box01_tex, st->box02_tex,
             st->ring_band_count, st->ring_mesh[0],
             origin.x, origin.y, origin.z, double(facing_byte),
             double(st->effect_pos[0]), double(st->effect_pos[1]), double(st->effect_pos[2]));
    log_info("[fireball-shim] Spawn trajectory: launch_pos=(%.1f,%.1f,%.1f) "
             "impact_pos_predicted=(%.1f,%.1f,%.1f) delta_x=%.1f (launch should be left "
             "of impact by ~96 wu)",
             double(st->launch_pos[0]), double(st->launch_pos[1]), double(st->launch_pos[2]),
             double(st->impact_pos_predicted[0]), double(st->impact_pos_predicted[1]),
             double(st->impact_pos_predicted[2]),
             double(st->impact_pos_predicted[0] - st->launch_pos[0]));
    return st;
}

// =========================================================================
// Public entry points.
// =========================================================================

// Tick — advance simulation. Call from regular submit hook.
void Tick(State* st)
{
    if (st) Animate(st);
}

// Submit — emit billboard draws (ball/glow/trail/burst/spark).
// Call from regular submit hook.
void Submit(State* st)
{
    if (st && st->alive) SubmitImpl(st);
}

// SubmitWorld — emit the shockwave ring (helper mesh).
// MUST be called from submit_world callback (gotcha #4).
void SubmitWorld(State* st)
{
    if (st && st->alive) SubmitWorldImpl(st);
}

void Destroy(State* st)
{
    delete st;
}

// IsAlive — true while effect is running. Test rig uses this for auto-respawn.
bool IsAlive(const State* st)
{
    return st && st->alive;
}

} // namespace fireball_shim
