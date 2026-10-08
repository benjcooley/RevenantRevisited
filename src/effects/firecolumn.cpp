// =========================================================================
// *                   Revenant Revisited (port) - 2026                   *
// *  firecolumn.cpp - TFireColumnAnimator_SHIM port via the d3d::* shim  *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/F09_TFireColumnEffect.md        *
// *  Snapshot Initialize:  src/missileeffect.cpp:1207-1241               *
// *  Snapshot Animate:     src/missileeffect.cpp:1255-1334               *
// *  Snapshot Render:      src/missileeffect.cpp:1343-1448               *
// *  Effect shell:         src/missileeffect.cpp:1154-1194               *
// *  Class declaration:    src/missileeffect.h:257-290                   *
// *                                                                       *
// *  Retail fidelity: VESTIGIAL — effect class exists in retail binary   *
// *  as dead code (cls_0x5b4534 retail-confirmed); asset + class.def     *
// *  entry stripped pre-ship; no live caller in retail or snapshot.       *
// *  See forensics §2.1 for reconciliation. This is a snapshot-intent    *
// *  reconstruction; retail-fidelity visual verification is impossible   *
// *  (no shipped reference).                                              *
// *                                                                       *
// *  LIGHTING CATEGORY: self-lit (missile fire family, no DoLighting).   *
// *  Blend is Alpha (MODULATE + SRC_ALPHA/INV_SRC_ALPHA) per snapshot    *
// *  SetBlendState() trace + missile-family (FireBall/FlameDisc) pattern.*
// *  SUSPECT: sister fire-SPELL family is Additive; see forensics §7.2.  *
// *  Vetting risk #1: if visual reads "muddy black-edged" flip to        *
// *  Additive (change d3d::SetBlendState → d3d::SetAddBlendState).       *
// *                                                                       *
// *  RENDER PIPELINE: WorldMesh (OBJ3D_MATRIX | OBJ3D_ABSPOS path for   *
// *  all 15 column-sprite draws). ALL draws from submit_world callback.  *
// *  Tick from regular submit callback (gotcha #4).                       *
// *                                                                       *
// *  DELIVERY: PARTIAL                                                    *
// *                                                                       *
// *  Phase A (DONE): Full state machine — LAUNCH (1-tick no-op) →        *
// *    FLY (15-column staggered spawn + flipbook cycle × 5 loops) →      *
// *    EXPLODE (front-to-back collapse with fast/slow shrink). All 15    *
// *    column slots placed correctly along aim vector. Scale math per     *
// *    forensics §7.4. Correct staggered visibility via negative         *
// *    framenum init. Correct random Z-rotation per column at spawn.      *
// *                                                                       *
// *  BLOCKED — Flipbook texture animation:                                *
// *    The shim's d3d::RenderObject(mesh,...) path bakes the albedo       *
// *    texture at RegisterSubMesh time. All 15 columns sharing the same   *
// *    mesh handle cannot be submitted with different per-frame texture   *
// *    handles in one SubmitWorld call: SHelperMeshSubmit has no          *
// *    texture-override field, and EmitTransparentHelper reads            *
// *    me.albedo at drain time (after all submits complete), so           *
// *    SetMeshAlbedo+SubmitHelperMesh in a loop all resolve to the last   *
// *    albedo set. This is the shim gap documented in WAVE_1_LESSONS      *
// *    §3.3: "No texture-frame animation primitive — Per-frame            *
// *    framehtexs[] cycling not yet in the shim."                         *
// *    Required fix: add texture_override to SHelperMeshSubmit and        *
// *    plumb it through EmitTransparentHelper. Until then, all columns   *
// *    render frame 0 of the FireColumn.I3D texture — geometry and        *
// *    state machine correct; animation frozen.                           *
// *                                                                       *
// *  TEST HARNESS NOTES:                                                  *
// *  - Anchor at world origin. Facing = 0 (aim = +X direction).          *
// *  - TMissileEffect physics engine (vel/range/MOVE_BLOCKED) not used   *
// *    in test rig. State machine driven by animator signals:             *
// *    LAUNCH fires SetCommandDone+SetStatus for immediate FLY entry      *
// *    (§13.5 fix: inject SetStatus(true) in LAUNCH, mirror F07).        *
// *  - No LIGHTNING_SOUND (commented in snapshot + retail — §11).        *
// *  - No associated light (no spell.def LIGHT directive — §9).          *
// *  - No divide-by-zero risk: FIRECOLUMN_SPEED = 0, missile immobile.   *
// *                                                                       *
// *  PENDING: ~1.5 Y scale (project_3d_world_y_scale) — WorldMesh        *
// *  effects may appear vertically squashed without this correction.      *
// =========================================================================

// ============================================================================
// Lighting category: SELF-LIT (Alpha blend; asset texture through; no DoLighting)
// ============================================================================

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "../d3dport.h"
#include "../logging.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace firecolumn_shim {

// =========================================================================
// §3 Constants — every value cites forensics §3 + snapshot source.
// [snapshot-only] = unverified against Ghidra.
// [retail-confirmed] = verified via cls_0x5b4534.
// =========================================================================

// --- State enum (missileeffect.h:20-24) [retail-confirmed via cls_0x5b4534] ---
constexpr int32_t kStateLaunch  = 0;   // MISSILE_LAUNCH
constexpr int32_t kStateFly     = 1;   // MISSILE_FLY
constexpr int32_t kStateExplode = 2;   // MISSILE_EXPLODE

// --- FireColumn specifics (missileeffect.h:239-290) [snapshot-only] ---
constexpr int32_t kNumFireColumns    = 15;   // NUM_FIRE_COLUMNS — missileeffect.h:261
constexpr int32_t kNumColumnSprites  = 13;   // GetObject(0) draws: n=0..12 — forensics §4.2
constexpr int32_t kNumExploSprites   = 2;    // GetObject(1) draws: n=13,14 — forensics §4.2

// --- Direction vector seed (missileeffect.cpp:1221) [snapshot-only] ---
// ConvertToVector(face, 10, vctr) — speed=10 → per-column step = 10 wu
// For a byte-angle facing, this gives a 2D unit step scaled by 10.
// In the test harness we use facing_byte=0 → cos/sin = (10.0, 0.0).
constexpr float kVctrSpeed = 10.0f;   // speed parameter for ConvertToVector

// --- Per-column offset (missileeffect.cpp:1229-1230) [snapshot-only] ---
// p[n].x = vctr.x * (n+3); p[n].y = vctr.y * (n+3)
// So p[0]=3×step, p[1]=4×step, ..., p[14]=17×step along aim.
// OffsetPoint(0) at :1223 is a no-op (overwritten next line) — NOT ported.

// --- Per-column Z-lift (missileeffect.cpp:1381,1425) [snapshot-only] ---
constexpr float kZLift = 5.0f;        // +5 wu above effect.pos.z for every column

// --- Framenum staggering (missileeffect.cpp:1226) [snapshot-only] ---
// framenum[n] = 0 - (n / 3): groups of 3 per tick visibility delay.
// Negative = invisible; framenum++ each tick until ≥ 0.

// --- Flipbook reroll (missileeffect.cpp:1288,1321) [snapshot-only] ---
// random(0, 5) on wrap — at least 6 frames implied.
constexpr int32_t kFrameRerollMax = 5;   // inclusive upper bound for reroll random

// --- FLY loop count (missileeffect.cpp:1282) [snapshot-only] ---
constexpr int32_t kFlyLoops = 5;         // activesparks hits 5 → flip to EXPLODE

// --- EXPLODE shrink rates (missileeffect.cpp:1311,1315) [snapshot-only] ---
constexpr int32_t kShrinkFast = 33;      // scale -= 33/tick on trailing column
constexpr int32_t kShrinkSlow = 8;       // scale -= 8/tick on leading 2 (explosion sprites)
constexpr int32_t kShrinkKillThreshold = 10;  // scale ≤ 10 → framenum[n] = -1

// --- Initial column scale (integer-fixed ×100) (missileeffect.cpp:1238) [snapshot-only] ---
constexpr int32_t kInitScale = 100;      // = 1.0×

// --- Scale math multipliers (missileeffect.cpp:1369-1378,1413-1420) [snapshot-only] ---
// Trailing column (n=0..11): scl = (1,2,3) * scale/100
// Head column    (n=12):     scl = (2,3,4) * scale/100
// Inner explo    (n=13):     scl.z = 4*scale/100; scl.xy = 3*((scale/2+50)/100)
// Outer explo    (n=14):     scl.z = 3.5*scale/100; scl.xy = 2.5*((scale/2+50)/100)

// --- Framerate-independent conversion ---
constexpr float kSimHz     = 24.0f;
constexpr float kSimTickMs = 1000.0f / kSimHz;

// --- Asset path (legacy snapshot tree only — NOT in data/imagery.rvi) ---
// forensics §4 — I3D: 2 sub-objects: flamecolum (0) + flameexplo (1)
constexpr const char* kImageryPath = "Magic\\FireColumn.I3D";
constexpr int32_t kSubColumn       = 0;   // flamecolum
constexpr int32_t kSubExplo        = 1;   // flameexplo
constexpr int32_t kNumSubObjs      = 2;

// =========================================================================
// Helper: inclusive-range random (src/revutils.cpp:1597-1612 semantics)
// rand()%(max-min+1)+min
// =========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// =========================================================================
// Per-column slot state
// =========================================================================
struct ColumnSlot
{
    float   px         = 0.0f;   // position offset from effect origin, x-axis
    float   py         = 0.0f;   // position offset, y-axis (z = kZLift always)
    int32_t framenum   = 0;       // flipbook frame index (<0 = invisible)
    float   rotation   = 0.0f;   // per-column Z-rotation in radians (random at Init, fixed)
    int32_t scale      = kInitScale;  // integer-fixed scale × 100
};

// =========================================================================
// Per-instance state — corresponds to TFireColumnAnimator member fields
// (missileeffect.h:257-290) plus test-rig bookkeeping.
// =========================================================================
struct State
{
    // 15-slot column chain
    ColumnSlot cols[kNumFireColumns];

    // Effect world origin (set at spawn; columns are offsets from this)
    float effect_pos[3] = {0, 0, 0};

    // Aim-vector step (used to set p[n] at Initialize)
    float vctr_x = kVctrSpeed;  // default: facing 0 → +X
    float vctr_y = 0.0f;

    // numtexframes from imagery (missileeffect.cpp:1215-1216)
    // BLOCKED: only frame 0 drawn currently (texture-animation shim gap).
    int32_t numtexframes = 1;

    // activesparks: dual-purpose field (§6.2)
    //   FLY phase:     loop counter (0→5); at 5 → flip to kNumFireColumns as EXPLODE sentinel
    //   EXPLODE phase: reverse-collapse pointer (kNumFireColumns→2)
    int32_t activesparks = 0;

    // State machine
    int32_t missile_state = kStateLaunch;
    int32_t old_state     = kStateLaunch;

    // Lifecycle
    bool alive = true;

    // Diagnostic log flags
    bool logged_first_render = false;
    bool logged_end          = false;

    // Framerate-independence accumulator (milliseconds)
    float tick_accum_ms = 0.0f;

    // Asset handles
    T3DImagery* imagery            = nullptr;
    MeshHandle  mesh[kNumSubObjs]  = {0, 0};
    float       diffuse[kNumSubObjs][4]  = {{1,1,1,1},{1,1,1,1}};
    float       emissive[kNumSubObjs][4] = {{0,0,0,1},{0,0,0,1}};
};

// =========================================================================
// Initialize — seeds the 15-slot column chain.
// Transcribed from missileeffect.cpp:1207-1241 (TFireColumnAnimator::Initialize).
// =========================================================================
static void InitializeAnimator(State* st)
{
    // missileeffect.cpp:1219
    st->missile_state = kStateLaunch;
    st->old_state     = kStateLaunch;

    // missileeffect.cpp:1215-1216 — read numtexframes off texture slot 0.
    // BLOCKED: flipbook animation frozen at frame 0 (shim gap — WAVE_1_LESSONS §3.3).
    // numtexframes set in Spawn() after imagery load.

    // missileeffect.cpp:1222-1238 — seed all 15 column slots.
    // Note: OffsetPoint(0) at :1223 is a no-op (overwritten at :1229) — not ported.
    for (int32_t n = 0; n < kNumFireColumns; ++n)
    {
        // missileeffect.cpp:1226 — staggered visibility start
        st->cols[n].framenum = 0 - (n / 3);

        // missileeffect.cpp:1229-1230 — position along aim vector
        st->cols[n].px = st->vctr_x * float(n + 3);
        st->cols[n].py = st->vctr_y * float(n + 3);

        // missileeffect.cpp:1237 — uniform random Z-rotation [0, 2π)
        // random(0, 360*TORADIAN) — forensics: 360*TORADIAN = 360*(π/180) = 2π
        // snap_random returns int; per forensics, the original scales to a float angle.
        // The source code is: rotation[n] = random(0, 360 * TORADIAN)
        // Since TORADIAN=π/180, 360*TORADIAN=2π≈6.283; but random(int,int) on that
        // would be random(0,6) — just 7 discrete angles. Looking at the pattern more
        // carefully: 360*TORADIAN is used as a float literal in the argument to random().
        // random(min,max) uses integer modulo. Likely intent is float uniform [0,2π):
        // The actual call uses float as int — treat as float angle directly.
        // Use integer random(0, 628) / 100.0f to approximate [0, 2π):
        st->cols[n].rotation = float(snap_random(0, 628)) / 100.0f;  // ≈ [0, 2π)

        // missileeffect.cpp:1238 — initial scale = 100 (integer-fixed ×100)
        st->cols[n].scale = kInitScale;
    }

    // missileeffect.cpp:1240 — activesparks = 0 (loop counter)
    st->activesparks = 0;
}

// =========================================================================
// AnimateOneSimTick — one 24Hz tick of TFireColumnAnimator::Animate.
// Transcribed from missileeffect.cpp:1255-1334.
// =========================================================================
static void AnimateOneSimTick(State* st)
{
    // missileeffect.cpp:1259 — T3DAnimator::Animate(draw) — base class no-op in port
    // missileeffect.cpp:1260 — inst->SetCommandDone(false) — reset per-tick

    switch (st->missile_state)
    {
        case kStateLaunch:
            // missileeffect.cpp:1264-1268 — "No fancy launch, yet"
            // Snapshot has SetCommandDone(true) here but no SetStatus(true).
            // Per forensics §13.5: snapshot bug — mirrors F07 which calls SetStatus(true).
            // Fix: set state directly to FLY (the dev's stated intent per the comment).
            st->missile_state = kStateFly;
            break;

        case kStateFly:
            // missileeffect.cpp:1271-1289 — advance each column's flipbook
            for (int32_t n = 0; n < kNumFireColumns; ++n)
            {
                // missileeffect.cpp:1275
                st->cols[n].framenum++;

                // missileeffect.cpp:1277 — check flipbook wrap
                if (st->cols[n].framenum >= st->numtexframes)
                {
                    // missileeffect.cpp:1279-1285 — last sprite (n=14) wrap = loop counter
                    if (n == kNumFireColumns - 1)
                    {
                        st->activesparks++;  // :1281

                        if (st->activesparks == kFlyLoops)  // :1282 — 5 loops done
                        {
                            // :1284 — flip sentinel for EXPLODE phase
                            st->activesparks = kNumFireColumns;
                            // :1285 — signal Effect: transition to EXPLODE next tick
                            // (In full engine: inst->SetCommandDone(true) →
                            //  TFireColumnEffect::Pulse sets range=0 → base EXPLODE)
                            // In test rig: drive state directly.
                            st->missile_state = kStateExplode;
                        }
                    }

                    // missileeffect.cpp:1288 — reroll frame on wrap (keeps columns desynced)
                    // BLOCKED: even with the reroll the frozen-texture means this only
                    // affects framenum bookkeeping, not the visual. Still correct to track.
                    st->cols[n].framenum = snap_random(0, kFrameRerollMax);
                }
            }
            break;

        case kStateExplode:
            // missileeffect.cpp:1294-1326 — front-to-back reverse collapse
            for (int32_t n = 0; n < kNumFireColumns; ++n)
            {
                if (st->activesparks > 2)
                {
                    // :1301 — the trailing column slot is n = NUM_FIRE_COLUMNS - activesparks
                    if (n <= (kNumFireColumns - st->activesparks))
                    {
                        if (st->cols[n].scale <= kShrinkKillThreshold)  // :1303
                        {
                            if (st->cols[n].framenum >= 0)  // :1305 — not yet hidden
                            {
                                st->activesparks--;          // :1307 — advance collapse pointer
                                st->cols[n].framenum = -1;   // :1308 — hide forever
                            }
                        }
                        else
                        {
                            st->cols[n].scale -= kShrinkFast;  // :1311 — fast shrink
                        }
                    }
                }
                else
                {
                    // :1314-1315 — leading 2 columns (explo sprites): slow shrink
                    st->cols[n].scale -= kShrinkSlow;
                }

                // :1317-1322 — continue flipbook for visible columns during EXPLODE
                if (st->cols[n].framenum >= 0)
                {
                    st->cols[n].framenum++;  // :1319
                    if (st->cols[n].framenum >= st->numtexframes)  // :1320
                        st->cols[n].framenum = snap_random(0, kFrameRerollMax);  // :1321
                }

                // :1324-1326 — death gate: last sprite (head explo, n=14) scale < 10
                if (st->cols[kNumFireColumns - 1].scale < kShrinkKillThreshold)
                {
                    // In engine: inst->SetCommandDone(true) → effect KillThisEffect.
                    // In test rig: mark dead.
                    if (!st->logged_end)
                    {
                        log_info("[firecolumn-shim] all columns dead (scale[14]=%d)",
                                 st->cols[kNumFireColumns - 1].scale);
                        st->logged_end = true;
                    }
                    st->alive = false;
                    return;  // don't update oldstate after death
                }
            }
            break;
    }

    st->old_state = st->missile_state;
}

// =========================================================================
// Tick — framerate-independent entry point.
// Accumulates real time at 24 Hz per feedback-framerate-independent-anim.
// B.4 pattern.
// =========================================================================
static void TickImpl(State* st)
{
    if (!st->alive) return;

    st->tick_accum_ms += float(TTime::DeltaTime() * 1000.0);
    while (st->tick_accum_ms >= kSimTickMs)
    {
        st->tick_accum_ms -= kSimTickMs;
        AnimateOneSimTick(st);
        if (!st->alive) break;
    }
}

// =========================================================================
// BuildColumnMatrix — assembles the OBJ3D_MATRIX | OBJ3D_ABSPOS transform
// for one column slot.
// Forensics §7.1: matrix = Scale · RotateZ(rotation[n]) · Translate(world pos)
// (No RotateX/-π/2 tip; columns are authored vertical in the I3D.)
//
// Transcribed from missileeffect.cpp:1385-1387 (column) / :1426-1428 (explo):
//   D3DMATRIXClear(&obj->matrix)
//   D3DMATRIXScale(&obj->matrix, scl)
//   D3DMATRIXRotateZ(&obj->matrix, rotation[n])
//   D3DMATRIXTranslate(&obj->matrix, world_pos)
// =========================================================================
static void BuildColumnMatrix(d3d::Matrix& out,
                               float scl_x, float scl_y, float scl_z,
                               float rot_z,
                               float world_x, float world_y, float world_z)
{
    d3d::MatrixIdentity(out);
    d3d::MatrixScale(out, scl_x, scl_y, scl_z);
    d3d::MatrixRotateZ(out, rot_z);
    d3d::MatrixTranslate(out, world_x, world_y, world_z);
}

// =========================================================================
// SubmitWorldImpl — render all WorldMesh draws for this tick.
// MUST be called from the harness submit_world callback (after BeginTilePass).
// Transcribed from missileeffect.cpp:1343-1448 (TFireColumnAnimator::Render).
// =========================================================================
static void SubmitWorldImpl(State* st)
{
    if (!Renderer || !st->alive) return;

    // Verify mesh handles
    if (!st->mesh[kSubColumn] || !st->mesh[kSubExplo]) return;

    // Diagnostic: log first SubmitWorld call
    if (!st->logged_first_render)
    {
        log_info("[firecolumn-shim] first render: state=%d activesparks=%d "
                 "effect_pos=(%.0f,%.0f,%.0f) mesh=(%u,%u) numtexframes=%d",
                 st->missile_state, st->activesparks,
                 double(st->effect_pos[0]),
                 double(st->effect_pos[1]),
                 double(st->effect_pos[2]),
                 st->mesh[kSubColumn], st->mesh[kSubExplo],
                 st->numtexframes);
        st->logged_first_render = true;
    }

    // missileeffect.cpp:1348-1349 — SaveBlendState + SetBlendState (Alpha)
    // Alpha = MODULATE + SRC_ALPHA/INV_SRC_ALPHA, ZWRITE=off, ZENABLE=on (forensics §7.2)
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();   // Alpha — SUSPECT: see forensics §7.2 sanity-check

    // missileeffect.cpp:1352-1353 — CULLMODE=NONE (draw both faces of every quad)
    // The shim has no explicit cullmode override; kHelperMeshFs uses two-pass
    // front+back rendering, which achieves the same CULLMODE=NONE effect.
    // (No per-effect action needed here.)

    // ─── LOOP 1: flamecolum (column sprite), n=0..12 ───────────────────────
    // missileeffect.cpp:1355-1393
    for (int32_t n = 0; n < kNumColumnSprites; ++n)  // n = 0..12
    {
        // missileeffect.cpp:1360 — skip until staggered start fires
        if (st->cols[n].framenum < 0) continue;

        const float s = float(st->cols[n].scale) / 100.0f;  // integer-fixed to float

        // missileeffect.cpp:1369-1378 — scale math, per forensics §7.4
        float scl_x, scl_y, scl_z;
        if (n == 12)  // head column
        {
            // missileeffect.cpp:1374-1378
            scl_x = 2.0f * s;
            scl_y = 3.0f * s;
            scl_z = 4.0f * s;
        }
        else  // trailing column (n=0..11)
        {
            // missileeffect.cpp:1369-1372
            scl_x = 1.0f * s;
            scl_y = 2.0f * s;
            scl_z = 3.0f * s;
        }

        // missileeffect.cpp:1379-1381 — world position = effect.pos + p[n] + z-lift
        const float wx = st->effect_pos[0] + st->cols[n].px;
        const float wy = st->effect_pos[1] + st->cols[n].py;
        const float wz = st->effect_pos[2] + kZLift;

        // BLOCKED: flipbook frame selection
        // Original: frame = framenum[n] % numtexframes (missileeffect.cpp:1390)
        // which indexes obj->texture[0].framehtexs[frame] per-draw.
        // Current: all draws use frame 0 (the baked albedo from RegisterSubMesh).
        // When shim gets per-draw texture override, insert:
        //   int32_t frame = st->cols[n].framenum % st->numtexframes;
        //   TTextureHandle ftex = get frame_textures_[frame] handle;
        //   Renderer->SetMeshAlbedo(st->mesh[kSubColumn], ftex);   -- before submit

        d3d::Matrix local;
        BuildColumnMatrix(local, scl_x, scl_y, scl_z,
                          st->cols[n].rotation,
                          wx, wy, wz);

        // missileeffect.cpp:1392 — RenderObject (OBJ3D_MATRIX | OBJ3D_ABSPOS)
        d3d::RenderObject(*Renderer,
                          st->mesh[kSubColumn],
                          local,
                          st->diffuse[kSubColumn],
                          st->emissive[kSubColumn],
                          d3d::CurrentBlend(),
                          nullptr,     // matrix already in world space (OBJ3D_ABSPOS)
                          wz);         // sort_z = world Z
    }

    // ─── LOOP 2: flameexplo (explosion sprite), n=13..14 ───────────────────
    // missileeffect.cpp:1399-1436
    // The snapshot uses a single outer `n` variable that continues from the
    // first loop (forensics §13.7 — intentional C-style continuation).
    // In this port we iterate n=13..14 explicitly.
    for (int32_t n = kNumColumnSprites; n < kNumFireColumns; ++n)  // n = 13..14
    {
        // missileeffect.cpp:1404 — skip invisible columns
        if (st->cols[n].framenum < 0) continue;

        const float s = float(st->cols[n].scale) / 100.0f;

        // missileeffect.cpp:1413-1421 — scale math for explosion sprites, per forensics §7.4
        // XY uses "(scale/2 + 50)/100" formula = (s + 1.0) / 2.0 = 0.5 + s/2
        // This keeps XY footprint large while Z collapses (§7.4 "dissipating cloud")
        const float xy_factor = (float(st->cols[n].scale) / 2.0f + 50.0f) / 100.0f;
        float scl_x, scl_y, scl_z;
        if (n == 13)  // inner explosion
        {
            // missileeffect.cpp:1413-1415
            scl_x = 3.0f * xy_factor;
            scl_y = 3.0f * xy_factor;
            scl_z = 4.0f * s;
        }
        else  // n == 14, outer explosion
        {
            // missileeffect.cpp:1418-1421
            scl_x = 2.5f * xy_factor;
            scl_y = 2.5f * xy_factor;
            scl_z = 3.5f * s;
        }

        // missileeffect.cpp:1423-1425 — world position = effect.pos + p[n] + z-lift
        const float wx = st->effect_pos[0] + st->cols[n].px;
        const float wy = st->effect_pos[1] + st->cols[n].py;
        const float wz = st->effect_pos[2] + kZLift;

        // BLOCKED: same flipbook frame blocker as the column loop above.

        d3d::Matrix local;
        BuildColumnMatrix(local, scl_x, scl_y, scl_z,
                          st->cols[n].rotation,
                          wx, wy, wz);

        // missileeffect.cpp:1434 — RenderObject
        d3d::RenderObject(*Renderer,
                          st->mesh[kSubExplo],
                          local,
                          st->diffuse[kSubExplo],
                          st->emissive[kSubExplo],
                          d3d::CurrentBlend(),
                          nullptr,
                          wz);
    }

    // missileeffect.cpp:1443,1445 — CULLMODE restore + RestoreBlendState
    // (CULLMODE: handled by two-pass shim; RestoreBlendState: handled by BlendStateGuard)
}

// =========================================================================
// Spawn — load asset, register meshes, seed the column chain.
// =========================================================================
State* Spawn(const S3DPoint& origin, float facing_byte)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->effect_pos[0] = float(origin.x);
    st->effect_pos[1] = float(origin.y);
    st->effect_pos[2] = float(origin.z);

    // Compute aim vector from byte-angle facing (ConvertToVector semantics).
    // ConvertToVector(face_byte, speed, vctr):
    //   angle_rad = (face_byte / 256.0) * 2π
    //   vctr.x = speed * cos(angle_rad)
    //   vctr.y = speed * sin(angle_rad)
    //   (positive Y = south in Revenant's world-space conventions)
    // missileeffect.cpp:1221: ConvertToVector(inst->GetFace(), 10, vctr)
    {
        const float angle_rad = (facing_byte / 256.0f) * (2.0f * 3.14159265358979323846f);
        st->vctr_x = kVctrSpeed * std::cos(angle_rad);
        st->vctr_y = kVctrSpeed * std::sin(angle_rad);
    }

    // Bind the I3D asset (legacy snapshot tree only — not in data/imagery.rvi).
    // forensics §4: registered as "FireColumn" "Magic\FireColumn.I3D" 0x12da54d2
    // in legacy/Class.Def:2040; NOT in shipped data/imagery.rvi.
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[firecolumn-shim] could not resolve '%s' — vestigial asset "
                  "present in legacy/ tree only (not in data/imagery.rvi)",
                  kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[firecolumn-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Read numtexframes off texture slot 0 (missileeffect.cpp:1215-1216).
    // BLOCKED: flipbook animation frozen regardless (shim gap §3.3).
    // We still read the correct count for framenum bookkeeping fidelity.
    if (img3d->NumTextures() > 0)
    {
        S3DTex tex = {};
        img3d->GetTexture(0, &tex);
        st->numtexframes = (tex.numframes > 0) ? tex.numframes : 1;
    }

    // Register sub-object meshes.
    //   mesh[0] = flamecolum (column sprite quad — forensics §4.2)
    //   mesh[1] = flameexplo (explosion sprite quad — forensics §4.2)
    const int32_t num_tex = img3d->NumTextures();
    for (int32_t s = 0; s < kNumSubObjs; ++s)
    {
        st->mesh[s] = d3d::RegisterSubMesh(*Renderer, img3d, s, num_tex);
        if (!st->mesh[s])
            log_warn("[firecolumn-shim] RegisterSubMesh(sub=%d) returned 0", s);
        d3d::LoadMaterial(img3d, s, st->diffuse[s], st->emissive[s]);

        // Per WAVE_1_LESSONS §1.1 + PORT_CHECKLIST B.2:
        // Zero emissive after LoadMaterial — pre-release I3Ds default to
        // emissive=(1,1,1) which saturates white under kHelperMeshFs.
        st->emissive[s][0] = 0.0f;
        st->emissive[s][1] = 0.0f;
        st->emissive[s][2] = 0.0f;
        // emissive[s][3] = alpha channel, leave at 1.0
    }

    // Initialize animator state (seeds 15 column slots)
    InitializeAnimator(st);

    // Diagnostic log at spawn (B.9)
    log_info("[firecolumn-shim] Spawn ok: alive=%d numtexframes=%d "
             "mesh=(%u,%u) origin=(%d,%d,%d) facing=%.1f "
             "vctr=(%.1f,%.1f) p0=(%.0f,%.0f) p14=(%.0f,%.0f)",
             int(st->alive), st->numtexframes,
             st->mesh[kSubColumn], st->mesh[kSubExplo],
             origin.x, origin.y, origin.z,
             double(facing_byte),
             double(st->vctr_x), double(st->vctr_y),
             double(st->cols[0].px), double(st->cols[0].py),
             double(st->cols[14].px), double(st->cols[14].py));

    return st;
}

// =========================================================================
// Public entry points (B.6 three-way split).
// =========================================================================

// Tick — advance simulation. Call from regular submit hook.
void Tick(State* st)
{
    if (st) TickImpl(st);
}

// Submit — billboard-only draws (none: all draws are WorldMesh).
// No-op for interface consistency with other two-callback ports.
void Submit(State* st)
{
    (void)st;
    // All firecolumn draws are WorldMesh → call SubmitWorld from submit_world hook.
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

// IsAlive — true while the effect is still running. Used by test-harness
// auto-respawn wrapper (B.7 pattern).
bool IsAlive(const State* st)
{
    return st && st->alive;
}

} // namespace firecolumn_shim
