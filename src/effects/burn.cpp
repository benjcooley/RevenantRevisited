// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  burn.cpp - TBurnAnimator port via the d3d::* shim                   *
// *                                                                       *
// *  Effect ID: M04  Class: TBurnAnimator                                 *
// *  Source of truth: docs/vfx/forensics/M04_TBurnEffect.md              *
// *                                                                       *
// *  Retail fidelity: retail-partial.                                     *
// *    - Asset burnbabyburn.I3D is byte-identical to shipped imagery.rvi  *
// *      (md5 7a4fc36ecb49d13b8cf7612256ca34ca, 25 908 B, §2.1.2).       *
// *    - Registration strings "BURN"/"Burn" confirmed in retail binary    *
// *      (§2.1.4). BURN_FRAME=50 corroborated at                         *
// *      cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn (§2.1.1).          *
// *    - Animator bodies (Initialize/Animate/Render) are snapshot-only   *
// *      (not recovered from Ghidra; §2.1). All per-particle physics     *
// *      constants are marked // snapshot-only below.                    *
// *                                                                       *
// *  Snapshot source:                                                     *
// *    Constants:    src/effect_old.cpp:3201-3212 + src/effect.h:1494    *
// *    Initialize:   src/effect_old.cpp:3292-3313                        *
// *    Animate:      src/effect_old.cpp:3315-3447                        *
// *    Render:       src/effect_old.cpp:3449-3460                        *
// *    Particle sys: src/effectcomp.cpp:1031-1131                        *
// *                                                                       *
// *  TParticleSystem encoding:                                            *
// *    Two stacked particle systems — fire (GetObject(1) = "smoke01",    *
// *    bright flame sprite) + smoke (GetObject(0) = "smoke", dim trail   *
// *    sprite). Fire particles are promoted to smoke when they expire:   *
// *    scale×1.25, life=(life*2)/3, re-added to smoke system.           *
// *                                                                       *
// *  Render bracket:                                                      *
// *    AdditiveStraight (ONE/ONE) — SetAddBlendState() (§7).             *
// *    ScreenAligned billboards per particle. No lighting, no audio.     *
// *                                                                       *
// *  Character anchor:                                                    *
// *    In-game, particles spawn at ca->GetObjectMatrix(j) per-bone       *
// *    positions on the victim's animated rig (effect_old.cpp:3367-3412).*
// *    In the test harness there is no character; particles spawn at the  *
// *    world origin (test harness anchor — real spawn site is per-bone   *
// *    in-game).                                                          *
// *                                                                       *
// *  Sub-object convention (do not swap — §13.5):                        *
// *    fire system: GetObject(1) = "smoke01" (the BRIGHT fire sprite)    *
// *    smoke system: GetObject(0) = "smoke"  (the DIM smoke sprite)      *
// *                                                                       *
// *  Delta-time conversion (framerate-independent):                       *
// *    Snapshot runs ungated at 24 Hz. All per-tick values are converted *
// *    to per-second and integrated by dt. BURN_FRAME=50 ticks/24 =     *
// *    2.0833 s. Velocity [3.0,7.5] wu/tick × 24 = [72,180] wu/s.      *
// *    BURN_DEC=0.97/tick → pow(0.97,24) ≈ 0.479/s scale halving rate.  *
// *************************************************************************

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"

extern TRenderer* Renderer;

namespace burn_shim {

// ==========================================================================
// Constants — every value cites the forensics §3 / source line.
// All snapshot-only unless noted "retail-confirmed".
// ==========================================================================

// Per-system particle cap (both fire and smoke arrays are sized to this).
// effect.h:1494
constexpr int32_t kBurnCount     = 70;          // snapshot-only

// Max particles spawned per tick during the steady-burn phase.
// effect_old.cpp:3203
constexpr int32_t kBurnAdd       = 8;           // snapshot-only

// Per-axis position jitter around the spawn origin (±wu).
// effect_old.cpp:3204
constexpr int32_t kBurnSpread    = 2;           // snapshot-only (wu)

// Scale range: random(MIN,MAX)*0.01 → [0.15, 0.40].
// effect_old.cpp:3205-3206
constexpr int32_t kBurnMinScl    = 15;          // snapshot-only
constexpr int32_t kBurnMaxScl    = 40;          // snapshot-only

// Upward velocity range: random(MIN,MAX)*0.1 → [3.0, 7.5] wu/tick.
// Converted to per-second: ×24 → [72.0, 180.0] wu/s.
// effect_old.cpp:3207-3208
constexpr int32_t kBurnMinZ      = 30;          // snapshot-only
constexpr int32_t kBurnMaxZ      = 75;          // snapshot-only

// Particle lifespan range in sim-ticks.
// effect_old.cpp:3209-3210
constexpr int32_t kBurnMinLife   = 5;           // snapshot-only (ticks)
constexpr int32_t kBurnMaxLife   = 15;          // snapshot-only (ticks)

// Per-tick scale decay multiplier (-3% per tick).
// Converted to per-second: pow(0.97, 24) ≈ 0.4788 — see Animate().
// effect_old.cpp:3211
constexpr float   kBurnDec       = 0.97f;       // snapshot-only (per tick)

// Master phase boundary in sim-ticks (spawn-cap ramp pivot + damage gate).
// Retail-confirmed: cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn at
// recon/classes/cls_0x5a7b98.cpp:10797 writes mbr_0x1bc = 0x32 (= 50).
// effect_old.cpp:3212
constexpr int32_t kBurnFrame     = 50;          // retail-confirmed (ticks)

// Fire→smoke promotion constants (snapshot-only, §6.3).
// effect_old.cpp:3349,3351-3353
constexpr float   kPromoteScaleBoost = 1.25f;  // snapshot-only — 25% larger as smoke
// life_span = (life_span * 2) / 3 (integer division, effect_old.cpp:3349)

// Sim rate for per-tick → per-second conversion (framerate-independent).
// The snapshot ran ungated at 24 Hz.
constexpr float   kSimHz         = 24.0f;

// Base billboard size in world units for the SubmitFxBillboard path.
// The snapshot's scale [0.15, 0.40] is a multiplier on the authored
// I3D vertex half-extent. Per gotcha #3, we multiply by a base constant.
// Burn sprites are soft ~flame-sprite sized; using 64 wu (same as Fountain
// sparkle family) gives a peak billboard of 0.40 * 64 = 25.6 wu, which is
// consistent with the rising-flame column visual.
constexpr float   kBurnBaseSizeWu = 64.0f;

// Sub-object indices for burnbabyburn.I3D (§4 + §13.5 — do NOT swap):
//   index 0 = "smoke"   → smoke system (dim sprite)
//   index 1 = "smoke01" → fire system  (bright sprite)
constexpr int32_t kSubObjSmoke  = 0;    // dim trail sprite
constexpr int32_t kSubObjFire   = 1;    // bright flame sprite

// Asset registry path (§4).
constexpr const char* kImageryPath = "Magic\\burnbabyburn.I3D";

// ==========================================================================
// Per-particle state — mirrors SParticleSystemInfo from effectcomp.h:308-354.
// ==========================================================================
struct Particle
{
    bool    used        = false;
    float   pos[3]      = {0, 0, 0};   // world-space position (abs)
    float   vel[3]      = {0, 0, 0};   // wu/s after per-second conversion
    float   scl         = 0.0f;        // uniform scale (snapshot writes scl.x=y=z)
    float   life        = 0.0f;        // accumulated seconds
    float   life_span   = 0.0f;        // total lifespan in seconds
    // acc = (1,1,1) → no damping; velocity preserved (§5 + effectcomp.cpp:1063)
};

// ==========================================================================
// Per-effect state — two particle arrays + asset handles + ramp state.
// ==========================================================================
struct State
{
    Particle fire[kBurnCount]   = {};   // fire system (smoke01 / GetObject(1))
    Particle smoke_[kBurnCount] = {};   // smoke system (smoke / GetObject(0))
                                        // note: named smoke_ to avoid conflict
                                        //       with smoke sub-object literal

    float    base_pos[3]        = {0, 0, 0};  // effect world anchor (test: origin)
    bool     alive              = true;

    // Ramp state — TBurnAnimator::frame + to_add (effect_old.cpp:3310-3312).
    float    frame_sec          = 0.0f;  // accumulated simulated seconds
    int32_t  to_add             = 0;     // current per-tick spawn cap (0→8→0)

    // Resolved asset data (two sub-objects, two texture handles).
    T3DImagery*    imagery           = nullptr;
    TTextureHandle tex_fire          = kInvalidTexture;   // smoke01 texture
    TTextureHandle tex_smoke         = kInvalidTexture;   // smoke  texture
    float          diffuse_fire[4]   = {1, 1, 1, 1};
    float          emissive_fire[4]  = {0, 0, 0, 1};
    float          diffuse_smoke[4]  = {1, 1, 1, 1};
    float          emissive_smoke[4] = {0, 0, 0, 1};
    float          uv_rect_fire[4]   = {0, 0, 1, 1};   // UV bbox for fire sub-obj
    float          uv_rect_smoke[4]  = {0, 0, 1, 1};   // UV bbox for smoke sub-obj

    int32_t  spawn_log_done     = 0;
};

// ==========================================================================
// Helpers
// ==========================================================================

// Inclusive-range random matching snapshot semantics:
// rand() % (max-min+1) + min — src/revutils.cpp:1609-1610.
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// Find a free slot in a particle array; returns nullptr if full.
static Particle* FindFreeSlot(Particle* arr, int32_t count)
{
    for (int32_t i = 0; i < count; ++i)
    {
        if (!arr[i].used) return &arr[i];
    }
    return nullptr;
}

// ==========================================================================
// SpawnFireParticle — called up to to_add times per tick.
// Transcribed from TBurnAnimator::Animate spawn loop,
// effect_old.cpp:3362-3413.
//
// In-game: spawn at ca->GetObjectMatrix(j) (random bone world position) ±
// BURN_SPREAD jitter (§5). In the test harness there is no character, so
// we use base_pos ± BURN_SPREAD jitter around the world origin.
// ==========================================================================
static void SpawnFireParticle(State* st)
{
    Particle* p = FindFreeSlot(st->fire, kBurnCount);
    if (!p) return;  // fire array full; drop spawn (bounded capacity)

    // Spawn position: test harness anchor at base_pos + per-axis jitter.
    // Real game spawn: transform (0,0,0) through bone×root matrix (§5).
    // Per-axis jitter ±BURN_SPREAD (=2 wu), effect_old.cpp:3387-3389.
    p->pos[0] = st->base_pos[0] + float(snap_random(-kBurnSpread, kBurnSpread));
    p->pos[1] = st->base_pos[1] + float(snap_random(-kBurnSpread, kBurnSpread));
    p->pos[2] = st->base_pos[2] + float(snap_random(-kBurnSpread, kBurnSpread));

    // Scale: random(BURN_MIN_SCL=15, BURN_MAX_SCL=40)*0.01 → [0.15, 0.40].
    // effect_old.cpp:3392-3395
    const float scl = float(snap_random(kBurnMinScl, kBurnMaxScl)) * 0.01f;
    p->scl = scl;

    // Velocity: pure upward [3.0, 7.5] wu/tick → per-second ×24.
    // effect_old.cpp:3406-3408; acc=(1,1,1) so vel is preserved each tick.
    p->vel[0] = 0.0f;
    p->vel[1] = 0.0f;
    const float vz_per_tick = float(snap_random(kBurnMinZ, kBurnMaxZ)) * 0.1f;
    p->vel[2] = vz_per_tick * kSimHz;  // convert wu/tick → wu/s

    // Lifespan: random(5,15) ticks → convert to seconds.
    // effect_old.cpp:3410
    const int32_t life_ticks = snap_random(kBurnMinLife, kBurnMaxLife);
    p->life_span = float(life_ticks) / kSimHz;

    // p.rot = (-90, 0, -45) deg is used by TParticleSystem::Render for the
    // per-particle matrix build (effectcomp.cpp:1083-1104, §13.2). In our
    // shim we use ScreenAligned billboards which do not apply a literal
    // per-particle rotation matrix — the double -π/2 × -π/4 convention is
    // the snapshot's encoding of a ScreenAligned pose, which our
    // SubmitFxBillboard handles natively. Rotation field not stored.

    p->life = 0.0f;
    p->used = true;
}

// ==========================================================================
// PromoteFireToSmoke — fire→smoke promotion pass.
// Transcribed from effect_old.cpp:3339-3361.
//
// For each live fire particle: if life >= life_span (just expired),
// mark the fire slot free, boost scale×1.25, shorten life=(life_span*2)/3,
// and add to the smoke system.
// ==========================================================================
static void PromoteFireToSmoke(State* st)
{
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        Particle& fp = st->fire[i];
        if (!fp.used) continue;

        // Check if this fire particle has just reached end-of-life.
        // effect_old.cpp:3341: if (particle->life == particle->life_span)
        if (fp.life < fp.life_span) continue;

        // Free the fire slot.
        fp.used = false;

        // Find a smoke slot; if smoke is also full, the particle just dies.
        Particle* sp = FindFreeSlot(st->smoke_, kBurnCount);
        if (!sp) continue;

        // Copy the expired fire particle into smoke with adjusted params.
        *sp = fp;  // copy all fields as a base

        // Scale boost: 25% larger as smoke (effect_old.cpp:3351-3353).
        sp->scl *= kPromoteScaleBoost;  // snapshot-only

        // Shortened smoke lifespan: (life_span * 2) / 3 in ticks → convert.
        // Original is integer division on tick counts.
        // effect_old.cpp:3349: particle->life_span = (particle->life_span * 2) / 3
        const int32_t orig_ticks   = int32_t(std::roundf(fp.life_span * kSimHz));
        const int32_t smoke_ticks  = (orig_ticks * 2) / 3;  // integer division
        sp->life_span = float(smoke_ticks) / kSimHz;

        // Reset life so smoke starts fresh.
        sp->life  = 0.0f;
        sp->used  = true;
    }
}

// ==========================================================================
// Initialize — TBurnAnimator::Initialize, effect_old.cpp:3292-3313.
//
// Sets up both particle systems (all slots zeroed), initializes ramp state.
// The snapshot's fire.Init(this, GetObject(1), ...) + smoke.Init(this,
// GetObject(0), ...) are covered by asset load in Spawn().
// ==========================================================================
static void Initialize(State* st)
{
    // Zero all particle slots (all .used = false).
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        st->fire[i]   = Particle{};
        st->smoke_[i] = Particle{};
    }

    // effect_old.cpp:3311-3312
    st->frame_sec = 0.0f;
    st->to_add    = 0;
}

// ==========================================================================
// Animate — TBurnAnimator::Animate, effect_old.cpp:3315-3447.
//
// Delta-time integration:
//   - frame_sec tracks elapsed simulated seconds (replaces the integer
//     frame counter from the snapshot; BURN_FRAME=50 ticks / 24 Hz = 2.0833 s)
//   - to_add ramp: integer counter, incremented/decremented once per
//     sim-tick equivalent (accumulated from dt)
//   - Particle positions integrated continuously by vel * dt
//   - Scale decay: pow(BURN_DEC, 24) per second = kBurnDecPerSec
// ==========================================================================
static void Animate(State* st, float dt)
{
    if (!st->alive) return;

    // Convert BURN_FRAME from ticks to seconds.
    constexpr float kBurnFrameSec = float(kBurnFrame) / kSimHz;  // ~2.0833 s

    // Per-second scale decay: pow(0.97, 24) ≈ 0.4788. Apply as pow(base, dt).
    // This is the dt-accurate version of the per-tick ×0.97 from the snapshot.
    const float dec_per_sec = std::pow(kBurnDec, kSimHz);  // snapshot-only
    const float scale_decay = std::pow(dec_per_sec, dt);

    // Advance the animator's frame counter (effect_old.cpp:3328: ++frame).
    st->frame_sec += dt;

    // Ramp to_add: one step per tick (dt-paced via accumulation).
    // effect_old.cpp:3329-3332:
    //   if frame<BURN_FRAME and to_add<BURN_ADD: ++to_add
    //   else if frame>=BURN_FRAME and to_add>0: --to_add
    //
    // We track ramp via real time; the increment/decrement fires once per
    // sim-tick equivalent. For test harness the caller sets dt to 1/24 per
    // frame, so this is functionally one step per call.
    if (st->frame_sec < kBurnFrameSec && st->to_add < kBurnAdd)
    {
        st->to_add++;
    }
    else if (st->frame_sec >= kBurnFrameSec && st->to_add > 0)
    {
        st->to_add--;
    }

    // === Promotion pass (before system Animate) ===
    // Check each fire particle for expiry; promote expired ones to smoke.
    // effect_old.cpp:3339-3361
    PromoteFireToSmoke(st);

    // === Per-system Animate (TParticleSystem::Animate, effectcomp.cpp:1041-1068) ===
    // For each live particle: life += dt; pos += vel*dt; if life>=life_span: kill.
    // acc=(1,1,1) → vel unchanged each tick (no damping, no gravity — §5).
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        Particle& fp = st->fire[i];
        if (!fp.used) continue;
        if (fp.life >= fp.life_span)
        {
            // effectcomp.cpp:1046-1049 — kill when life >= life_span
            fp.used = false;
            continue;
        }
        fp.life    += dt;
        fp.pos[0]  += fp.vel[0] * dt;
        fp.pos[1]  += fp.vel[1] * dt;
        fp.pos[2]  += fp.vel[2] * dt;
        // vel *= acc (acc=1.0) → no change
    }
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        Particle& sp = st->smoke_[i];
        if (!sp.used) continue;
        if (sp.life >= sp.life_span)
        {
            sp.used = false;
            continue;
        }
        sp.life    += dt;
        sp.pos[0]  += sp.vel[0] * dt;
        sp.pos[1]  += sp.vel[1] * dt;
        sp.pos[2]  += sp.vel[2] * dt;
    }

    // === Spawn loop — up to to_add new fire particles ===
    // effect_old.cpp:3362-3413
    for (int32_t i = 0; i < st->to_add; ++i)
    {
        SpawnFireParticle(st);
    }

    // === Decay pass — scale *= BURN_DEC (dt-integrated) ===
    // effect_old.cpp:3415-3440
    bool done = true;
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        Particle& fp = st->fire[i];
        if (!fp.used) continue;
        done    = false;
        fp.scl *= scale_decay;  // snapshot-only per-tick ×0.97 integrated over dt
    }
    for (int32_t i = 0; i < kBurnCount; ++i)
    {
        Particle& sp = st->smoke_[i];
        if (!sp.used) continue;
        done    = false;
        sp.scl *= scale_decay;
    }

    // === End-of-life check ===
    // effect_old.cpp:3442-3446:
    //   if frame>=BURN_FRAME AND to_add==0 AND done → KillThisEffect()
    // In the standalone test rig we set alive=false instead of KillThisEffect.
    if (st->frame_sec >= kBurnFrameSec && st->to_add == 0 && done)
    {
        st->alive = false;
    }
}

// ==========================================================================
// Render — TBurnAnimator::Render, effect_old.cpp:3449-3460.
//
// SaveBlendState(); SetAddBlendState();
// fire.Render(false, true)   // flicker=false, abs_pos=true
// smoke.Render(false, true)
// RestoreBlendState()
//
// Per-particle matrix in TParticleSystem::Render (effectcomp.cpp:1070-1109):
// the double -π/2 X-tip + -π/4 Z-tip convention produces ScreenAligned
// behaviour for small soft sprites (§13.2). We use d3d::Orientation::ScreenAligned
// which handles camera-facing in the billboard path natively.
//
// abs_pos=true (OBJ3D_ABSPOS) means particle positions are absolute world
// coordinates — not relative to the animator's own pos. We pass nullptr for
// inst_world below to match this semantics (positions are already world-space).
// ==========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // === Lazy texture binding (gotcha #1) ===
    // Retry on every Submit until both texture handles are valid.
    if (st->imagery)
    {
        if (st->tex_fire == kInvalidTexture || st->tex_smoke == kInvalidTexture)
        {
            // burnbabyburn.I3D has two sub-objects each with its own embedded
            // texture. Resolve by material → texture index per sub-object
            // (same pattern as fizzle.cpp's per-system texture resolution).
            const int32_t sub_indices[2] = { kSubObjFire, kSubObjSmoke };
            TTextureHandle* handles[2]   = { &st->tex_fire, &st->tex_smoke };

            for (int32_t k = 0; k < 2; ++k)
            {
                if (*handles[k] != kInvalidTexture) continue;
                const int32_t sub = sub_indices[k];
                S3DObj o = {};
                st->imagery->GetObject(sub, &o);
                int32_t tex_idx = sub;  // fallback: use sub-object index
                if (o.material >= 0 && o.material < st->imagery->NumMaterials())
                {
                    S3DMat m = {};
                    st->imagery->GetMaterial(o.material, &m);
                    if (m.texture >= 0 && m.texture < st->imagery->NumTextures())
                        tex_idx = m.texture;
                }
                if (tex_idx >= st->imagery->NumTextures()) tex_idx = 0;
                S3DTex t = {};
                st->imagery->GetTexture(tex_idx, &t);
                *handles[k] = t.htexture;
            }
        }
    }
    // Wait until both handles resolve before drawing anything.
    if (st->tex_fire == kInvalidTexture || st->tex_smoke == kInvalidTexture)
        return;

    // === SaveBlendState / SetAddBlendState (ONE/ONE — §7) ===
    d3d::BlendStateGuard blend_scope;
    d3d::SetAddBlendState();  // AdditiveStraight; effect_old.cpp:3452

    // === fire.Render(flicker=false, abs_pos=true) ===
    // abs_pos=true: particle positions are absolute world coords.
    // We pass nullptr for inst_world — no additional transform needed.
    // effect_old.cpp:3454-3455

    // Shared obj for fire draws
    {
        d3d::Obj obj;
        obj.objnum = kSubObjFire;
        std::memcpy(obj.diffuse,  st->diffuse_fire,  sizeof(obj.diffuse));
        std::memcpy(obj.emissive, st->emissive_fire, sizeof(obj.emissive));

        // UV sub-rect for smoke01 (gotcha #2)
        const float u_lo = st->uv_rect_fire[0];
        const float v_lo = st->uv_rect_fire[1];
        const float u_hi = u_lo + st->uv_rect_fire[2];
        const float v_hi = v_lo + st->uv_rect_fire[3];
        obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
        obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
        obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
        obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

        for (int32_t i = 0; i < kBurnCount; ++i)
        {
            const Particle& fp = st->fire[i];
            if (!fp.used) continue;
            if (fp.scl <= 0.0f) continue;

            // OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS | OBJ3D_VERTS
            // Scale applied first, then translate; position is absolute
            // world (abs_pos=true, effectcomp.cpp:1081-1082).
            obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 |
                        d3d::Flag_AbsPos | d3d::Flag_Verts;

            // Scale: snapshot scale * base WU size (gotcha #3).
            const float sz = fp.scl * kBurnBaseSizeWu;
            obj.scl[0] = obj.scl[1] = obj.scl[2] = sz;

            // Position: absolute world coords (OBJ3D_ABSPOS path).
            obj.pos[0] = fp.pos[0];
            obj.pos[1] = fp.pos[1];
            obj.pos[2] = fp.pos[2];

            // abs_pos=true → inst_world is nullptr (no base offset)
            d3d::RenderObject(*Renderer, obj,
                              d3d::Orientation::ScreenAligned,
                              st->tex_fire,
                              nullptr);
        }
    }

    // === smoke.Render(flicker=false, abs_pos=true) ===
    // effect_old.cpp:3455-3456
    {
        d3d::Obj obj;
        obj.objnum = kSubObjSmoke;
        std::memcpy(obj.diffuse,  st->diffuse_smoke,  sizeof(obj.diffuse));
        std::memcpy(obj.emissive, st->emissive_smoke, sizeof(obj.emissive));

        // UV sub-rect for smoke (gotcha #2)
        const float u_lo = st->uv_rect_smoke[0];
        const float v_lo = st->uv_rect_smoke[1];
        const float u_hi = u_lo + st->uv_rect_smoke[2];
        const float v_hi = v_lo + st->uv_rect_smoke[3];
        obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
        obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
        obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
        obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

        for (int32_t i = 0; i < kBurnCount; ++i)
        {
            const Particle& sp = st->smoke_[i];
            if (!sp.used) continue;
            if (sp.scl <= 0.0f) continue;

            obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 |
                        d3d::Flag_AbsPos | d3d::Flag_Verts;

            const float sz = sp.scl * kBurnBaseSizeWu;
            obj.scl[0] = obj.scl[1] = obj.scl[2] = sz;

            obj.pos[0] = sp.pos[0];
            obj.pos[1] = sp.pos[1];
            obj.pos[2] = sp.pos[2];

            d3d::RenderObject(*Renderer, obj,
                              d3d::Orientation::ScreenAligned,
                              st->tex_smoke,
                              nullptr);
        }
    }

    if (!st->spawn_log_done)
    {
        log_info("[burn-shim] first render: frame_sec=%.3f to_add=%d "
                 "tex_fire=%u tex_smoke=%u base=(%.0f,%.0f,%.0f)",
                 double(st->frame_sec), st->to_add,
                 st->tex_fire, st->tex_smoke,
                 st->base_pos[0], st->base_pos[1], st->base_pos[2]);
        st->spawn_log_done = 1;
    }
}

// ==========================================================================
// ResolveUvRect — extract a sub-object's UV bbox from its authored vertices.
// Standard gotcha #2 pattern.
// ==========================================================================
static void ResolveUvRect(T3DImagery* img3d, int32_t sub, float rect_out[4])
{
    rect_out[0] = 0.0f; rect_out[1] = 0.0f;
    rect_out[2] = 1.0f; rect_out[3] = 1.0f;

    const int32_t nv = img3d->NumObjVerts(sub);
    if (nv <= 0) return;

    std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
    img3d->GetObjVerts(sub, vbuf.data(), 0, 0, ERender3DVertex::Vertex);

    float minu = vbuf[0].tu, maxu = vbuf[0].tu;
    float minv = vbuf[0].tv, maxv = vbuf[0].tv;
    for (int32_t i = 1; i < nv; ++i)
    {
        if (vbuf[i].tu < minu) minu = vbuf[i].tu;
        if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
        if (vbuf[i].tv < minv) minv = vbuf[i].tv;
        if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
    }
    rect_out[0] = minu;
    rect_out[1] = minv;
    rect_out[2] = maxu - minu;
    rect_out[3] = maxv - minv;
}

// ==========================================================================
// Spawn — load burnbabyburn.I3D, resolve sub-object materials + UV rects,
// run Initialize, return State.
//
// origin: world-space anchor for the effect. In-game this is the victim's
// position (TBurnEffect::Set calls SetPos(character->GetPos())).
// ==========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // Bind burnbabyburn.I3D (§4).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[burn-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[burn-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Material colors — sub-object 1 (smoke01 = fire sprite) + 0 (smoke sprite).
    // Effect never tints these; asset authored colors are the source (§10).
    d3d::LoadMaterial(img3d, kSubObjFire,  st->diffuse_fire,  st->emissive_fire);
    d3d::LoadMaterial(img3d, kSubObjSmoke, st->diffuse_smoke, st->emissive_smoke);

    // UV sub-rects (gotcha #2). burnbabyburn.I3D has two distinct sub-objects;
    // §4.1 confirms each is a single-quad single-cell — full-quad UVs (0..1)
    // expected. But we resolve anyway in case the authored UVs differ.
    ResolveUvRect(img3d, kSubObjFire,  st->uv_rect_fire);
    ResolveUvRect(img3d, kSubObjSmoke, st->uv_rect_smoke);

    // Texture handles deferred to first Render() call (gotcha #1).
    // st->tex_fire and st->tex_smoke remain kInvalidTexture until then.

    Initialize(st);

    log_info("[burn-shim] spawned at (%d,%d,%d) "
             "diffuse_fire=(%.2f,%.2f,%.2f) diffuse_smoke=(%.2f,%.2f,%.2f) "
             "uv_fire=(%.2f,%.2f,%.2f,%.2f) uv_smoke=(%.2f,%.2f,%.2f,%.2f)",
             origin.x, origin.y, origin.z,
             double(st->diffuse_fire[0]),  double(st->diffuse_fire[1]),
             double(st->diffuse_fire[2]),
             double(st->diffuse_smoke[0]), double(st->diffuse_smoke[1]),
             double(st->diffuse_smoke[2]),
             double(st->uv_rect_fire[0]),  double(st->uv_rect_fire[1]),
             double(st->uv_rect_fire[2]),  double(st->uv_rect_fire[3]),
             double(st->uv_rect_smoke[0]), double(st->uv_rect_smoke[1]),
             double(st->uv_rect_smoke[2]), double(st->uv_rect_smoke[3]));
    return st;
}

// ==========================================================================
// Public entry points called by the vfxtest harness.
//
// The harness calls Tick() (simulate one step) then Submit() (render).
// dt should be 1.0/24.0 per design sim-rate to match the snapshot's
// ungated 24 Hz tick cadence.
// ==========================================================================
void Tick(State* st, float dt) { if (st && st->alive) Animate(st, dt); }
void Submit(State* st)         { if (st && st->alive) Render(st); }
void Destroy(State* st)        { delete st; }

// Overload for harness compatibility (harness passes no dt — use 1/24 default).
void Tick(State* st)           { Tick(st, 1.0f / kSimHz); }

} // namespace burn_shim
