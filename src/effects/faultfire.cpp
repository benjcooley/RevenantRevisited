// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  faultfire.cpp - TFaultFireAnimator port via the d3d::* shim          *
// *                                                                       *
// *  Effect ID: F06  Class: TFaultFireAnimator                            *
// *  Forensics: docs/vfx/forensics/F06_TFaultFireEffect.md                *
// *                                                                       *
// *  Retail fidelity: retail-partial.                                     *
// *    - Asset Magic\faultfire.i3d is byte-identical to shipped           *
// *      imagery.rvi (MD5 21cb69a1e4053d652dada99407885af1, 33,688 B).   *
// *    - "FaultFire" registration + factory table entries confirmed in    *
// *      retail (string 005e11dc, 2 XREFs → cls_0x5ac83c). See §2.1.     *
// *    - Animate/Render bodies are snapshot-only (not recovered from      *
// *      Ghidra). All per-tick/per-frame constants are marked             *
// *      // snapshot-only below.                                          *
// *    - Retail class size (400 bytes) is larger than the snapshot's      *
// *      near-empty TFaultFireEffect shell — possible retail-only light   *
// *      or sub-effect (§2.1/§9). Default reconstruction = no light.     *
// *                                                                       *
// *  Snapshot sources:                                                    *
// *    Registration:  src/effect_old.cpp:11127-11128,11145                *
// *    SetupObjects:  src/effect_old.cpp:11147-11157                      *
// *    Initialize:    src/effect_old.cpp:11159-11163                      *
// *    Animate:       src/effect_old.cpp:11165-11180                      *
// *    Render:        src/effect_old.cpp:11182-11224                      *
// *                                                                       *
// *  Architecture:                                                        *
// *    - Single quad (sub-object 0 / "launch" / "box01") of              *
// *      Magic\faultfire.i3d — 4 vertices, full 0..1 UV range.           *
// *    - Drawn TWICE per frame (two render passes) with 90°-phase-offset  *
// *      tv-scale (cos(th) vs cos(th+π/2)) for layered-flame beat.       *
// *    - U axis: continuous per-tick scroll (random(2,8)/100 per tick,   *
// *      ~1.2 UV/s average, wraps indefinitely). (§8)                    *
// *    - V axis: per-pass multiplicative scale (0.75..1.0 range,         *
// *      2.6 s cycle). (§8)                                               *
// *    - No particles, no sub-emitter, no state machine, no lifetime,    *
// *      no audio, no dynamic light (snapshot). Persists via             *
// *      SetCommandDone(false). (§5, §9, §11)                            *
// *                                                                       *
// *  Blend = Alpha (SetBlendState — MODULATE + SRC_ALPHA/INV_SRC_ALPHA). *
// *  Asset is alpha-masked ARGB4444 fire sprite (NOT chroma-keyed on     *
// *  black); additive would dump dim red over the quad (§7 sanity check).*
// *  Depth = TestNoWrite (ZENABLE on, ZWRITE off — SetBlendState). (§7)  *
// *                                                                       *
// *  Orientation note:                                                    *
// *    Original: OBJ3D_MATRIX | OBJ3D_VERTS — world-fixed column facing  *
// *    +X after RotateZ(π/2). (§7)                                        *
// *    Shim substitution: ScreenAligned billboard with per-draw UV rect   *
// *    carrying the tu-scroll + tv-scale. The shim's mesh-handle         *
// *    RenderObject has no per-draw UV override (SHelperMeshSubmit has    *
// *    no uv_rect); ScreenAligned captures the V-scale identity (throb)  *
// *    plus the texture's authored vertical fire gradient (white-hot     *
// *    bottom → invisible top, via alpha mask). For the vestigial effect *
// *    with no in-game ground truth and no live caller (§12), the        *
// *    camera-facing read is an acceptable substitution. The shim's     *
// *    enum has WorldUpAligned reserved as future for a true vertical    *
// *    card; until added, ScreenAligned is the only viable option        *
// *    that preserves the tall-narrow flame aspect and UV animation.    *
// *                                                                       *
// *  Texture address mode (round-2 fix):                                  *
// *    Renderer hardcodes SG_WRAP_CLAMP_TO_EDGE for textures              *
// *    (src/renderer.cpp:1894). The snapshot relied on D3DTADDRESS_WRAP   *
// *    for the U-scroll. To prevent the U coord from running off the     *
// *    texture (which would otherwise clamp the entire visible area to   *
// *    the texture's right-edge column past the first wrap ~0.83 s), we *
// *    modulo `tu` to [0, 1) per submit. With UV rect width = 1.0,       *
// *    the right portion of the billboard still clamps proportional to   *
// *    tu_lo (right ~tu_lo*100% of pixels sample the rightmost texture   *
// *    column). The visual effect is a continuously-shifting flame where *
// *    the left portion shows clean detail and the right portion shows  *
// *    a smear that cycles through the texture columns as tu wraps.     *
// *    This is the best port-side approximation; perfect fidelity would *
// *    require per-texture WRAP support in the renderer.                *
// *                                                                       *
// *  Size:                                                                *
// *    16 wu wide (64 wu × scl.x=0.25), 64 wu tall (64 wu × scl.z=1.0). *
// *    Rendered from 32 wu above the effect origin (local +Z = 32 wu     *
// *    translate). (§7)                                                   *
// *                                                                       *
// *  Persistent (no self-kill — IsAlive always returns true).            *
// *  vfxtest rig does NOT need auto-respawn for this effect.             *
// *                                                                       *
// *  PENDING: ~1.5 Y scale (project_3d_world_y_scale) — WorldMesh       *
// *  Y-scale correction not yet applied here (billboard path used;       *
// *  if effect is later ported to true WorldMesh this note applies).     *
// *                                                                       *
// *  REGISTRATION block: see .vfx_logs/faultfire.md § REGISTRATION.     *
// *************************************************************************

// ============================================================================
// Lighting category: SELF-LIT (Alpha blend; asset color through; no DoLighting)
//   Fire is a textbook self-lit emitter. No DoLighting call in the snapshot
//   animator (effect_old.cpp:11165-11224). The Alpha blend + authored ARGB4444
//   fire-gradient sprite provides the visual. (Forensics §7, §10.)
// ============================================================================

#include <cmath>
#include <cstdint>
#include <cstdlib>

#include "../d3dport.h"
#include "../logging.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace faultfire_shim {

// ==========================================================================
// Constants — every value cites forensics §3 + snapshot source line.
// All snapshot-only unless noted "retail-confirmed".
// ==========================================================================

// Phase advance per sim-tick (rad/tick).
// effect_old.cpp:11143 #define FF_STEP 0.1
// Per-second equivalent: FF_STEP * 24 Hz = 2.4 rad/s.
constexpr float kFFStep         = 0.1f;     // snapshot-only

// Modulo wrap point for th.
// effect_old.cpp:11171-11172
constexpr float kTwoPi          = 6.283185307179586f;

// Tu-scroll per tick: random(2,8)/100 → 0.02..0.08 UV/tick.
// Per-second equivalent: *24 Hz → 0.48..1.92 UV/s, mean ~1.2 UV/s.
// effect_old.cpp:11174
constexpr int32_t kDuMin        = 2;        // snapshot-only (inclusive)
constexpr int32_t kDuMax        = 8;        // snapshot-only (inclusive)

// Baseline tv shift applied once in SetupObjects (one-time −0.01 per vert).
// After this tvs[] = {-0.01, -0.01, 0.99, 0.99}.
// effect_old.cpp:11154-11155
constexpr float kTvShift        = 0.01f;    // snapshot-only

// Tv-scale envelope: 0.125 * (cos(th + i*π/2) + 7) ∈ [0.75, 1.0].
// effect_old.cpp:11193
constexpr float kTvScaleA       = 0.125f;   // snapshot-only
constexpr float kTvScaleB       = 7.0f;     // snapshot-only

// Number of render passes per frame (i = 0, 1).
// effect_old.cpp:11191
constexpr int32_t kNumPasses    = 2;        // snapshot-only

// Phase offset between the two passes (90°).
// effect_old.cpp:11193 (i * M_PI / 2)
constexpr float kPassPhase      = 1.5707963267948966f;  // π/2, snapshot-only

// Object scale: scl = (0.25, 0.25, 1.0).
// effect_old.cpp:11202-11203 (scl.z = 4.0 * 0.25)
constexpr float kSclX           = 0.25f;    // snapshot-only
constexpr float kSclZ           = 1.0f;     // snapshot-only (= 4.0 * 0.25)

// World-unit sizes after scale: X = 64 wu * 0.25 = 16 wu, Z = 64 wu * 1.0 = 64 wu.
constexpr float kBillboardWidthWu  = 16.0f; // 64 wu authored × kSclX = 16 wu
constexpr float kBillboardHeightWu = 64.0f; // 64 wu authored × kSclZ = 64 wu

// +Z translation applied per pass before RenderObject: pos = (0, 0, 32).
// effect_old.cpp:11211-11214
constexpr float kPosZ           = 32.0f;    // snapshot-only (wu, local Z)

// Sim rate (24 Hz engine default — Animate runs once per tick).
// Framerate-independent reconstruction: accumulate wall-clock ms,
// step once per kSimTickMs.
constexpr float kSimHz          = 24.0f;
constexpr float kSimTickMs      = 1000.0f / kSimHz;  // 41.666... ms

// Asset registry path (§4, §2).
constexpr const char* kImageryPath = "Magic\\faultfire.i3d";

// Sub-object index: only sub-object 0 ("launch" / "box01") is drawn.
// effect_old.cpp:11150 (NewObject(0)) + :11165-11224 (GetObject(0))
constexpr int32_t kSubObj = 0;  // snapshot-only

// Baseline tv values after SetupObjects one-time shift.
// Per the authored OBJ (recon dump), verts 0,1 are at low WORLD-Z (z=-28,
// quad bottom) with tv=0, and verts 2,3 are at high WORLD-Z (z=+36, quad
// top) with tv=1. After tv -= 0.01 in SetupObjects:
//   tvs[0] = tvs[1] = -0.01   (world-bottom edge, samples texture v≈0)
//   tvs[2] = tvs[3] =  0.99   (world-top edge,    samples texture v≈1)
// The texture has white-hot at v=0 (texture top = flame BODY, opaque) and
// invisible at v=1 (texture bottom = flame TIP, A=0). When mapped to the
// quad: bright at world-bottom (anchored flame body), fades to transparent
// at world-top (flame tip). A natural flame.
// (forensics §4 / §6.1; i3d_dump_all/faultfire/faultfire.obj for vert order)
constexpr float kTvsWorldBottom = -0.01f;  // tvs[0] = tvs[1] (low-Z verts)
constexpr float kTvsWorldTop    =  0.99f;  // tvs[2] = tvs[3] (high-Z verts)

// ==========================================================================
// State — minimal: float th, float tvs[4] + tu accumulator + asset handle.
// Original TFaultFireAnimator state: th, tvs[4] (effect.h:2226).
// We add tu[4] (per-vertex tu accumulators, all advance in lock-step).
// ==========================================================================
struct State
{
    float   th          = 0.0f;   // phase, advances by kFFStep per tick (rad)
    float   tvs[4]      = { kTvsWorldBottom, kTvsWorldBottom,
                            kTvsWorldTop,    kTvsWorldTop    };
                                  // cached baseline tv after −0.01 shift (§6.1)
    float   tu          = 0.0f;   // all 4 verts share one tu (they advance
                                  // lock-step — forensics §8)

    float   sim_accum_ms = 0.0f;  // framerate-independent accumulator (B.4)

    // Spawn anchor (world position).
    float   base_pos[3] = {0, 0, 0};

    // Asset + resolved texture handle (lazy: kInvalidTexture until first render).
    T3DImagery*    imagery = nullptr;
    TTextureHandle texture = kInvalidTexture;

    // Material colors (loaded from asset once at Spawn).
    float   diffuse[4]   = {1, 1, 1, 1};
    float   emissive[4]  = {0, 0, 0, 1};

    // Diagnostic flags (B.9).
    bool    logged_first_render = false;
    bool    logged_first_pass[2] = { false, false };  // per-pass log gate
};

// ==========================================================================
// snap_random — inclusive-range random matching snapshot semantics.
// src/revutils.cpp:1597-1612: rand() % (max-min+1) + min.
// ==========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// ==========================================================================
// AnimateOneSimTick — one 24 Hz tick of the animator.
// Transcribed from TFaultFireAnimator::Animate, effect_old.cpp:11165-11180.
//
// 1. Advance th by FF_STEP, wrap at 2π.
// 2. Pick du = random(2,8)/100.  Add du to all 4 verts' tu (lock-step).
// (SetCommandDone(false) is the engine's "stay alive" signal; in the shim
//  the effect persists until externally destroyed — no alive flag needed.)
// ==========================================================================
static void AnimateOneSimTick(State* st)
{
    // Advance phase: th += FF_STEP; wrap at 2π.
    // effect_old.cpp:11170-11172
    st->th += kFFStep;
    if (st->th > kTwoPi)
        st->th -= kTwoPi;

    // Pick this tick's tu scroll: uniform int [2..8] / 100.
    // effect_old.cpp:11174
    const float du = float(snap_random(kDuMin, kDuMax)) / 100.0f;

    // Add du to all 4 verts' tu (lock-step, no modulo — wraps via texture).
    // effect_old.cpp:11176-11179
    st->tu += du;
    // (tu drifts indefinitely; texture address wrap handles mod-1 seamlessly)
}

// ==========================================================================
// Tick — framerate-independent 24 Hz wrapper.
// Accumulates real wall-clock ms; calls AnimateOneSimTick once per ~41.6 ms.
// Per PORT_CHECKLIST B.4.
// ==========================================================================
void Tick(State* st)
{
    if (!st) return;
    st->sim_accum_ms += float(TTime::DeltaTime() * 1000.0);
    while (st->sim_accum_ms >= kSimTickMs)
    {
        AnimateOneSimTick(st);
        st->sim_accum_ms -= kSimTickMs;
    }
}

// ==========================================================================
// Submit — two billboard passes per frame.
//
// Transcribed from TFaultFireAnimator::Render, effect_old.cpp:11182-11224.
//
// Per-pass:
//   scale = 0.125 * (cos(th + i * π/2) + 7)   range [0.75, 1.0]
//   tv = tvs[n] * scale  (V-compressed/stretched fire column)
//   tu = current accumulated scroll (all verts same offset)
//   Blend = Alpha (SetBlendState)
//   Draw quad via ScreenAligned billboard with UV rect carrying tu/tv.
//
// UV rect encoding (see orientation note in file header):
//   lverts[0] = TL: (tu,         tvs[0]*scale)
//   lverts[3] = BR: (tu + 1.0,   tvs[2]*scale)
// which yields uv_rect = (tu, tvs[0]*scale, 1.0, tvs[2]*scale − tvs[0]*scale).
//
// Both passes draw at the same billboard position: effect origin + kPosZ
// (raised 32 wu along world +Z, matching the Translate(0,0,32) transform
// in the original — §7).
//
// MUST be called from the harness submit callback (billboard path is safe
// outside BeginTilePass scope — unlike WorldMesh which needs submit_world).
// ==========================================================================
void Submit(State* st)
{
    if (!st || !Renderer) return;

    // --- Lazy texture binding (PORT_CHECKLIST B.8 / Gotcha #1) ---
    // TObjectImagery::LoadImagery returns valid imagery immediately; the GPU
    // texture handle in S3DTex.htexture is set during the subsequent upload
    // pass. Retry until valid.
    if (st->texture == kInvalidTexture && st->imagery)
    {
        S3DObj o = {};
        st->imagery->GetObject(kSubObj, &o);
        int32_t tex_idx = 0;
        if (o.material >= 0 && o.material < st->imagery->NumMaterials())
        {
            S3DMat m = {};
            st->imagery->GetMaterial(o.material, &m);
            if (m.texture >= 0 && m.texture < st->imagery->NumTextures())
                tex_idx = m.texture;
        }
        if (tex_idx < st->imagery->NumTextures())
        {
            S3DTex t = {};
            st->imagery->GetTexture(tex_idx, &t);
            st->texture = t.htexture;
        }
    }
    if (st->texture == kInvalidTexture) return;  // wait another frame

    // --- Resolve texture dimensions + alpha format (round-2 diagnostic) ---
    int32_t tex_w = -1, tex_h = -1;
    if (st->texture != kInvalidTexture)
    {
        const SRendererTextureInfo* info = Renderer->TextureInfo(st->texture);
        if (info)
        {
            tex_w = info->width;
            tex_h = info->height;
        }
    }

    // --- Diagnostic: first render ---
    if (!st->logged_first_render)
    {
        log_info("[faultfire-shim] first render: th=%.3f tu_raw=%.3f tex=%u "
                 "tex_size=%dx%d base=(%.0f,%.0f,%.0f) "
                 "size_wu=(%.1f,%.1f) center=(%.0f,%.0f,%.0f) "
                 "blend=Alpha orient=ScreenAligned (see DEVIATIONS)",
                 double(st->th), double(st->tu), st->texture,
                 tex_w, tex_h,
                 double(st->base_pos[0]), double(st->base_pos[1]),
                 double(st->base_pos[2]),
                 double(kBillboardWidthWu), double(kBillboardHeightWu),
                 double(st->base_pos[0]), double(st->base_pos[1]),
                 double(st->base_pos[2] + kPosZ));
        st->logged_first_render = true;
    }

    // --- SaveBlendState / SetBlendState (Alpha — §7) ---
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();  // SRC_ALPHA / INV_SRC_ALPHA; effect_old.cpp:11188

    // Billboard world position: effect origin + 32 wu along world +Z.
    // The original Translate(0,0,32) before RenderObject lifts the quad to
    // +32 wu above the spawn point. (§7, §5)
    const float wx = st->base_pos[0];
    const float wy = st->base_pos[1];
    const float wz = st->base_pos[2] + kPosZ;

    // Modulo tu to [0, 1) before submission (round-2 fix for CLAMP texture mode).
    // The renderer hardcodes SG_WRAP_CLAMP_TO_EDGE; under the snapshot's
    // WRAP-assumed math, the accumulating tu would seamlessly tile. Without
    // wrap, raw st->tu growing past 1.0 would push the entire UV rect outside
    // [0,1] and clamp to the texture's right-edge column (single-pixel-wide
    // smear across the whole quad — what produced "a quad with moving glow"
    // in round-1). Modulo keeps tu_lo cycling in [0,1).
    //
    // Remaining artifact: with UV rect width = 1.0, the right portion of the
    // quad still clamps proportional to tu_lo. As tu_lo cycles 0→1, the clamp
    // boundary sweeps from right edge to left edge of the quad. Net effect:
    // a continuously-shifting flame texture in the visible (non-clamped)
    // region, with the clamped region also varying. Perfect WRAP fidelity
    // would require per-texture address mode in the renderer.
    const float tu_lo = std::fmod(st->tu, 1.0f);
    const float tu_hi = tu_lo + 1.0f;

    // Two render passes — i=0 (th), i=1 (th + π/2).
    // effect_old.cpp:11191-11218 (for i in {0,1})
    for (int32_t i = 0; i < kNumPasses; ++i)
    {
        // tv-scale envelope: 0.125 * (cos(th + i*π/2) + 7) ∈ [0.75, 1.0].
        // effect_old.cpp:11193
        const float scale = kTvScaleA * (std::cos(st->th + float(i) * kPassPhase) + kTvScaleB);

        // Scaled tv values for this pass.
        // tvs[0/1] (world-bottom verts) → samples texture v≈0 (white-hot flame body)
        // tvs[2/3] (world-top verts)    → samples texture v=0.74..0.99 (transparent tip)
        // The V-scale varies the apparent flame height: scale=0.75 → top verts
        // sample v=0.74 (still some fire color visible); scale=1.0 → top verts
        // sample v=0.99 (transparent — taller flame fading out cleanly).
        // effect_old.cpp:11195-11196
        const float v_world_bottom = st->tvs[0] * scale;   // -0.01 * scale (≈ -0.0075..-0.01)
        const float v_world_top    = st->tvs[2] * scale;   //  0.99 * scale (0.7425..0.99)

        // Build the billboard draw.
        d3d::Obj obj;
        obj.flags = d3d::Flag_Verts | d3d::Flag_AbsPos;

        // Size: 16 wu wide × 64 wu tall (§7 — scaled quad extents).
        // size_wu maps to world-units (perspective-scaled in shader for
        // ScreenAligned).  size_wu[0]=width, size_wu[1]=height.
        obj.scl[0] = kBillboardWidthWu;    // 16 wu wide
        obj.scl[1] = kBillboardHeightWu;   // 64 wu tall — 4:1 tall narrow column
        obj.scl[2] = 1.0f;

        // Absolute world position (billboard center).
        obj.pos[0] = wx;
        obj.pos[1] = wy;
        obj.pos[2] = wz;

        // UV rect via lverts (Flag_Verts path in d3d::RenderObject ScreenAligned):
        // Per the shader, uv_n.y = rect.y + corner_uv01.y * rect.w. The corner
        // with uv01.y=0 is the SCREEN-BOTTOM of the billboard (corner.y=-0.5
        // → larger spy → lower on screen). So rect.y maps to SCREEN-BOTTOM
        // and rect.y+rect.w maps to SCREEN-TOP. Since the world quad's bottom
        // (low Z) is also where the camera-facing billboard's bottom lands,
        // we put the WORLD-BOTTOM tv (v_world_bottom ≈ 0) at the screen-bottom
        // and the WORLD-TOP tv (v_world_top ≈ 0.99) at the screen-top.
        //
        //   lverts[0] (u_lo, v_lo): corner (-0.5, -0.5) = screen bottom-left
        //   lverts[3] (u_hi, v_hi): corner (+0.5, +0.5) = screen top-right
        //
        // v at screen-bottom = v_world_bottom (= tvs[0]*scale, ≈ 0, white-hot)
        // v at screen-top    = v_world_top    (= tvs[2]*scale, ≈ 0.99, transparent)
        obj.lverts[0].tu = tu_lo; obj.lverts[0].tv = v_world_bottom;  // screen BL
        obj.lverts[1].tu = tu_lo; obj.lverts[1].tv = v_world_top;     // screen TL
        obj.lverts[2].tu = tu_hi; obj.lverts[2].tv = v_world_bottom;  // screen BR
        obj.lverts[3].tu = tu_hi; obj.lverts[3].tv = v_world_top;     // screen TR

        // Material colors (white passthrough — self-lit, no tint).
        obj.diffuse[0]  = st->diffuse[0];
        obj.diffuse[1]  = st->diffuse[1];
        obj.diffuse[2]  = st->diffuse[2];
        obj.diffuse[3]  = st->diffuse[3];
        obj.emissive[0] = st->emissive[0];
        obj.emissive[1] = st->emissive[1];
        obj.emissive[2] = st->emissive[2];
        obj.emissive[3] = st->emissive[3];

        // Per-pass diagnostic (logged once per pass on first frame only).
        if (!st->logged_first_pass[i])
        {
            log_info("[faultfire-shim] pass=%d scale=%.3f "
                     "uv_rect=(%.3f, %.3f, %.3f, %.3f) "
                     "size_wu=(%.1f, %.1f) wpos=(%.0f, %.0f, %.0f)",
                     i, double(scale),
                     double(tu_lo), double(v_world_bottom),
                     double(tu_hi - tu_lo), double(v_world_top - v_world_bottom),
                     double(obj.scl[0]), double(obj.scl[1]),
                     double(obj.pos[0]), double(obj.pos[1]), double(obj.pos[2]));
            st->logged_first_pass[i] = true;
        }

        d3d::RenderObject(*Renderer, obj,
                          d3d::Orientation::ScreenAligned,
                          st->texture,
                          nullptr);
    }
}

// ==========================================================================
// SubmitWorld — no-op (FaultFire uses billboard path, not WorldMesh).
// Provided for the standard three-entry-point interface.
// ==========================================================================
void SubmitWorld(State* /*st*/) {}

// ==========================================================================
// IsAlive — FaultFire has no internal lifetime; always alive until
// externally destroyed. (§5, §13.8)
// ==========================================================================
bool IsAlive(const State* st) { return st != nullptr; }

// ==========================================================================
// Spawn — load Magic\faultfire.i3d, extract material, run Initialize.
//
// One-time setup (SetupObjects, effect_old.cpp:11147-11157):
//   - Build per-instance copy of sub-object 0 (box01 quad, 4 verts)
//   - Apply tv −= 0.01 to all 4 verts → cache as tvs[] = [-0.01,-0.01,0.99,0.99]
//   - (In the shim tvs[] is a State constant — kTvsWorldBottom/Top. No dynamic mesh
//     copy needed since we drive UVs via the billboard uv_rect path.)
//
// Initialize (effect_old.cpp:11159-11163): th = 0.0
// ==========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // th initialized to 0.0 (Initialize, effect_old.cpp:11162)
    st->th = 0.0f;
    st->tu = 0.0f;

    // tvs[] set from kTvsWorldBottom/kTvsWorldTop (one-time SetupObjects
    // shift encoded as constants — effect_old.cpp:11154-11155 tv -= 0.01).
    st->tvs[0] = kTvsWorldBottom;
    st->tvs[1] = kTvsWorldBottom;
    st->tvs[2] = kTvsWorldTop;
    st->tvs[3] = kTvsWorldTop;

    // Bind Magic\faultfire.i3d (§4).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[faultfire-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base_img = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base_img);
    if (!img3d)
    {
        log_error("[faultfire-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Material colors for sub-object 0 (launch / box01).
    // FaultFire writes no per-vertex tint; asset authored color passes through
    // under MODULATE (effectively white → texture direct). (§10)
    d3d::LoadMaterial(img3d, kSubObj, st->diffuse, st->emissive);

    // B.2 — zero emissive to prevent white-saturation from D3DMATERIAL defaults.
    // kHelperMeshFs adds emissive additively; pre-release I3Ds default to (1,1,1).
    // (For billboard path this zeroing is defensive — emissive is not submitted
    //  through the SBillboardDrawItem, but leave it clean for the mesh path if
    //  this port is ever upgraded to true WorldMesh.)
    st->emissive[0] = 0.0f;
    st->emissive[1] = 0.0f;
    st->emissive[2] = 0.0f;
    // emissive[3] (alpha) stays 1.0

    // Texture handle deferred to first Submit() call (Gotcha #1).
    // st->texture remains kInvalidTexture.

    log_info("[faultfire-shim] Spawn ok: origin=(%d,%d,%d) th=0 tu=0 "
             "diffuse=(%.2f,%.2f,%.2f) imagery=%p",
             origin.x, origin.y, origin.z,
             double(st->diffuse[0]), double(st->diffuse[1]), double(st->diffuse[2]),
             static_cast<void*>(st->imagery));
    return st;
}

// ==========================================================================
// Destroy — free State.
// ==========================================================================
void Destroy(State* st) { delete st; }

} // namespace faultfire_shim
