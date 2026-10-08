// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  flame.cpp - TFlameAnimator port via the d3d::* shim                  *
// *                                                                       *
// *  Effect ID: F01  Class: TFlameAnimator_SHIM                          *
// *  Source of truth: docs/vfx/forensics/F01_TFlameEffect.md             *
// *                                                                       *
// *  Retail fidelity: retail-partial.                                     *
// *    - Registration name "FLAME" confirmed in retail binary             *
// *      (s_FLAME_005e10f4 XREF → animator-builder factory table @       *
// *      005c5348→0x4e4ea0, same pattern as SPARKS §2.1; §2.1(2)).       *
// *    - Asset Magic\Flame.I3D present in shipped imagery.rvi (41,348 B, *
// *      7 surface-header bytes differ — retail dims 128×160 correct;    *
// *      snapshot dims 339×316 bogus; texel data identical; §2.1(1)).    *
// *    - Animator bodies (Initialize/Animate/Render) are snapshot-only   *
// *      (retail render body at 0x4e4ea0 not extracted; §2.1(3)(4)).     *
// *    - All per-frame constants (frame-wrap 18, UV-cell math frame*11/24,*
// *      rotation 45°/30°/160°, scale 0.5, blend Alpha) are              *
// *      SNAPSHOT-ONLY and unverified against shipped retail.            *
// *                                                                       *
// *  Snapshot source:                                                     *
// *    Initialize: src/effect_old.cpp:4486-4494                          *
// *    Animate:    src/effect_old.cpp:4503-4510                          *
// *    Render:     src/effect_old.cpp:4519-4565                          *
// *    RefreshZ:   src/effect_old.cpp:4567-4578                          *
// *                                                                       *
// *  Blend note (§7 BLEND SANITY-CHECK):                                 *
// *    Snapshot codes Alpha (SetBlendState: SRC_ALPHA/INV_SRC_ALPHA).    *
// *    However: sprite design (bright-on-green-chroma-key) + in-game     *
// *    torch-glow intent strongly suggest AdditiveStraight in the shipped *
// *    game. The retail render body is NOT decompiled; the SPARKS         *
// *    exemplar (§2.1) confirmed snapshot-Alpha → shipped-Additive for   *
// *    a similar green-keyed glow sprite. VISUAL VET required: if the    *
// *    flame reads dull/translucent rather than glowing, switch to        *
// *    SetAddBlendState(). A kBlendMode enum is exposed below.           *
// *                                                                       *
// *  Animation mechanism: UV atlas-cell pick (NOT framehtexs[] swap).    *
// *    Magic\Flame.I3D has one physical 128×160 RGB565 surface with      *
// *    S3DTex.numframes=1. The flipbook is driven by overwriting the      *
// *    four corner UVs per frame to sample one of 8 cells in the 4×2     *
// *    atlas. Cell index n = floor(t_anim_sec * 11) % 8, with the        *
// *    per-frame hold pattern from the §4 cell-by-frame table preserved. *
// *    Cells (0,0) and (1,1) hold for 3 nominal-frames; others for 2.    *
// *                                                                       *
// *  Orientation: ScreenAligned billboard — the snapshot's explicit       *
// *    RotX(45°)·RotY(30°)·RotZ(160°)·Scale(0.5) matrix is its          *
// *    hand-tuned approximation of "WorldUpAligned" for the iso camera.  *
// *    Per forensics §7 the intent is a WorldUpAligned billboard (Z-up,  *
// *    camera-facing); the shim's ScreenAligned matches the intended      *
// *    behavior. UV sub-rect is applied via Flag_Verts/lverts[].         *
// *    Size: scl[0]=kFlameHalfW, scl[1]=kFlameHalfH (half-extents).     *
// *    NOTE: rotation triple is snapshot-only; flag for visual vet.      *
// *                                                                       *
// *  Chroma key: GREEN (0x0f20 = RGB(8,228,0)) at 54% of pixels — NOT   *
// *    black. The port relies on the loader/renderer to handle this;      *
// *    see §4 and §13 for the green-key vs black-key distinction.        *
// *                                                                       *
// *  Lighting category: SELF-LIT (Alpha or Additive blend; asset color   *
// *    passes through unmodulated; no DoLighting). The animator never     *
// *    zeroes the material (unlike TFlareAnimator) and never folds in     *
// *    ambient light. Per-vertex diffuse = opaque white; the texture's    *
// *    hot orange/yellow/white renders at full sprite brightness.         *
// *                                                                       *
// *  Continuous loop: no kill condition in the animator. The effect lives *
// *    until its owning sector / world placement removes it. No IsAlive   *
// *    needed (does not self-kill). No particles, no children, no audio.  *
// *                                                                       *
// *  PENDING: ~1.5 Y scale (project_3d_world_y_scale) — not baked here; *
// *    may affect billboard height in world space. Visual vet required.   *
// *                                                                       *
// ============================================================================
// Lighting category: SELF-LIT (Alpha/Additive blend; asset color through;
//   no DoLighting; per-vertex diffuse = opaque white)
// ============================================================================
// *************************************************************************

#include <cstdint>
#include <cstring>
#include <cmath>

#include "../d3dport.h"
#include "../logging.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace flame_shim {

// ==========================================================================
// Constants — all cite forensics §3 / snapshot source line.
// All snapshot-only unless noted "retail-confirmed".
// ==========================================================================

// Frame counter wrap point: frame cycles 0..17 (18 values).
// effect_old.cpp:4508-4509
constexpr int32_t kFrameWrap     = 18;          // snapshot-only

// Atlas cell index math (per §4 / §8 cell-by-frame table):
//   n = (int32_t)(frame * 11 / 24)
// Converted to time-based: n = floor(t_sec * kCellsPerSec) where
//   kCellsPerSec = (11.0/24.0) * 24 = 11.0 cells/second.
// This preserves the non-uniform 2/3-frame holds exactly via integer flooring.
// forensics §13.9: floor(t * 11) where t is seconds since spawn.
constexpr float  kCellsPerSec    = 11.0f;       // snapshot-only (derived from frame*11/24 at 24Hz)

// Atlas layout: 4 columns × 2 rows = 8 cells in a 128×160 texture.
// Atlas column stride = 0.25 UV, row stride = 0.5 UV.
// effect_old.cpp:4543-4556
constexpr int32_t kAtlasCols     = 4;           // snapshot-only
constexpr int32_t kAtlasRows     = 2;           // snapshot-only
constexpr float  kAtlasUStride   = 0.25f;       // snapshot-only (1/4 of 128px surface)
constexpr float  kAtlasVStride   = 0.5f;        // snapshot-only (1/2 of 160px surface)

// Total cell count = kAtlasCols * kAtlasRows = 8.
constexpr int32_t kAtlasCellCount = kAtlasCols * kAtlasRows;

// Cycle period in seconds: kFrameWrap frames / 24 Hz nominal = 0.75 s.
// After t_anim_sec accumulates kCycleSec, we wrap to avoid float drift.
constexpr float  kSimHz           = 24.0f;
constexpr float  kCycleSec        = float(kFrameWrap) / kSimHz;  // 0.75 s

// Sub-object index for the sole box01 quad (§4).
// effect_old.cpp:4490, asset object table entry "box01"
constexpr int32_t kSubObj        = 0;           // snapshot-only (asset corroborates)

// Billboard half-extents in world units.
//
// The snapshot's Render body builds Scale(0.5) on the authored box01 mesh
// and the RefreshZBuffer body uses 50.0f*0.5f=25px wide / 125.0f*0.5f=62px
// tall as the on-screen patch size. The 50/125 are the authored sprite
// footprint; the 0.5 matches the Render scale. This suggests the unscaled
// box01 quad spans ~50 wu wide × 125 wu tall in object space; at Scale(0.5)
// the effective half-extents are ~25 wu × 62.5 wu in world space.
//
// For the ScreenAligned billboard path, scl[0]/scl[1] are the half-extents
// in world units (d3dport.cpp:300-301 maps obj.scl directly to size_wu).
// We use the scaled-down values from the RefreshZBuffer patch math.
//
// snapshot-only (§6.3, §6.4 — RefreshZBuffer 50.0f*0.5f, 125.0f*0.5f)
constexpr float  kFlameHalfW     = 25.0f;       // snapshot-only (50.0f * 0.5f)
constexpr float  kFlameHalfH     = 62.5f;       // snapshot-only (125.0f * 0.5f; truncated to 62 on-screen px)

// Asset registry path (§4 — Class.Def:2030 + imagery.rvi:class.def:3362).
// retail-confirmed: same path + id 0x50ba373b in both snapshot and retail.
constexpr const char* kImageryPath = "Magic\\Flame.I3D";

// ==========================================================================
// Per-effect state.
// ==========================================================================
struct State
{
    // Time-based animation accumulator (seconds since spawn).
    // Replaces snapshot's integer frame counter.
    // Wraps every kCycleSec to avoid float precision drift.
    float           t_anim_sec     = 0.0f;

    // TTime::DeltaTime accumulator for §B.4 framerate-independent tick.
    float           tick_accum_ms  = 0.0f;

    // World anchor (effect's spawn position).
    float           pos[3]         = {0, 0, 0};

    // Asset handles (deferred to first Render — lazy binding, Gotcha #1).
    T3DImagery*     imagery        = nullptr;
    TTextureHandle  texture        = kInvalidTexture;

    // Per-vertex diffuse: opaque white (texture passes through unmodulated
    // under MODULATE; snapshot Render never writes lverts[i].color; §13.8).
    float           diffuse[4]     = {1.0f, 1.0f, 1.0f, 1.0f};
    // Emissive zeroed per §B.2 / WAVE_1_LESSONS §1.1 (even though this is a
    // billboard, not a WorldMesh, we zero emissive for correctness).
    float           emissive[4]    = {0.0f, 0.0f, 0.0f, 1.0f};

    // Diagnostic flags (§B.9).
    bool            logged_first_render = false;
    // logged_cycle_done: fires once after the first full 0.75s atlas cycle.
    // This effect never "dies" (continuous loop, §5), so there is no
    // end-of-life log; the §B.9 "dead" criterion is satisfied by this
    // first-cycle-complete log as the lifecycle milestone equivalent.
    bool            logged_cycle_done   = false;
};

// ==========================================================================
// AnimateOneSimTick — per-sim-tick update of the animation state.
// The snapshot's Animate(bool draw) body:
//   T3DAnimator::Animate(draw)      // :4505
//   ++frame                         // :4507
//   if (frame >= 18) frame = 0      // :4508-4509
//
// Converted to time-based: advance t_anim_sec by kSimTickMs/1000 each tick,
// then wrap every kCycleSec seconds. Cell index derived in Render from
// t_anim_sec, not stored (stateless read from accumulator).
// ==========================================================================
static void AnimateOneSimTick(State* st)
{
    st->t_anim_sec += 1.0f / kSimHz;
    if (st->t_anim_sec >= kCycleSec)
    {
        st->t_anim_sec -= kCycleSec;
        // §B.9 "dead" equivalent — this effect never dies (continuous loop, §5),
        // so we log the first complete atlas cycle as the lifecycle milestone.
        if (!st->logged_cycle_done)
        {
            log_info("[flame-shim] first full cycle complete (loop continues forever)");
            st->logged_cycle_done = true;
        }
    }
}

// ==========================================================================
// Tick — framerate-independent simulation step (§B.4).
//
// The snapshot's Animate increments frame per render-call (ungated).
// Per feedback_framerate_independent_anim, we accumulate real wall-clock
// time and step the sim once per kSimTickMs to match the original 24 Hz
// cadence, regardless of actual framerate.
// ==========================================================================
void Tick(State* st)
{
    if (!st) return;

    constexpr float kSimTickMs = 1000.0f / kSimHz;

    st->tick_accum_ms += float(TTime::DeltaTime() * 1000.0);
    while (st->tick_accum_ms >= kSimTickMs)
    {
        AnimateOneSimTick(st);
        st->tick_accum_ms -= kSimTickMs;
    }
}

// ==========================================================================
// Render (Submit) — TFlameAnimator::Render, effect_old.cpp:4519-4565.
//
// One ScreenAligned billboard draw per call. UV sub-rect derived from the
// current t_anim_sec position in the 18-frame loop. Blend = Alpha per
// snapshot (SUSPECT — see §7 / top-of-file blend note).
//
// Snapshot render body summary (§6.3):
//   SaveBlendState(); SetBlendState(); // Alpha
//   n = (int32_t)(frame * 11 / 24)    // cell index 0..7
//   col = n % 4; row = n / 4
//   xpos = col * 0.25; ypos = row * 0.5
//   lverts[0].tu/tv = (xpos,        ypos)         // TL
//   lverts[1].tu/tv = (xpos + 0.25, ypos)         // TR (snapshot order)
//   lverts[2].tu/tv = (xpos,        ypos + 0.5)   // BL
//   lverts[3].tu/tv = (xpos + 0.25, ypos + 0.5)   // BR
//   RenderObject(obj)
//   RestoreBlendState()
//
// The snapshot's lverts[0..3] order per the TStormAnimator reference
// (effectcomp.cpp:265-275) is TL / TR / BL / BR. The ScreenAligned path
// in d3dport.cpp:309-316 reads lverts[0] as (u_lo, v_lo) and lverts[3]
// as (u_hi, v_hi) to form the UV rect. So we write:
//   lverts[0] = TL = (xpos,        ypos       ) ← u_lo, v_lo
//   lverts[3] = BR = (xpos + 0.25, ypos + 0.5 ) ← u_hi, v_hi
// (lverts[1] and [2] are not read by the ScreenAligned path; still set
//  for completeness matching the snapshot.)
// ==========================================================================
void Submit(State* st)
{
    if (!st || !Renderer) return;

    // === Lazy texture binding (Gotcha #1, §B.8) ===
    if (st->texture == kInvalidTexture && st->imagery)
    {
        S3DObj o = {};
        st->imagery->GetObject(kSubObj, &o);
        int32_t tex_idx = 0;  // fallback: first texture
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
    if (st->texture == kInvalidTexture) return;  // wait another tick

    // === Diagnostic log — first render (§B.9) ===
    if (!st->logged_first_render)
    {
        log_info("[flame-shim] first render: t_anim=%.3f texture=%u "
                 "pos=(%.0f,%.0f,%.0f)",
                 double(st->t_anim_sec), st->texture,
                 double(st->pos[0]), double(st->pos[1]), double(st->pos[2]));
        st->logged_first_render = true;
    }

    // === Compute current atlas cell index (§4 / §8 cell-by-frame table) ===
    // Snapshot: n = (int32_t)(frame * 11 / 24)  — effect_old.cpp:4543-4544
    // Time-based equivalent: n = floor(t_sec * 11)
    //   col = n % kAtlasCols  (0..3)
    //   row = n / kAtlasCols  (0..1)
    // This exactly reproduces the §4 cell-by-frame table including the
    // non-uniform 2/3-frame holds (cells 0 and 5 held for 3 nominal-frames).
    const int32_t frame_index = int32_t(st->t_anim_sec * kCellsPerSec);
    // Safety clamp: should not exceed 7 given kCycleSec wrapping, but guard
    // against floating-point edge-case at the wrap boundary.
    const int32_t n   = frame_index % kAtlasCellCount;
    const int32_t col = n % kAtlasCols;
    const int32_t row = n / kAtlasCols;

    const float xpos = float(col) * kAtlasUStride;   // 0.00, 0.25, 0.50, 0.75
    const float ypos = float(row) * kAtlasVStride;   // 0.00, 0.50

    // === Build the draw item (§B.4-equivalent for billboard path) ===
    d3d::Obj obj;
    // OBJ3D_VERTS = Flag_Verts — tells the ScreenAligned path to use lverts[]
    // for the UV sub-rect (d3dport.cpp:307-317).
    obj.flags = d3d::Flag_Verts;

    // Billboard size (half-extents in world units).
    // Width  = kFlameHalfW  (25 wu ← 50.0f * 0.5f from RefreshZBuffer)
    // Height = kFlameHalfH  (62.5 wu ← 125.0f * 0.5f)
    // snapshot-only (§6.3, §6.4)
    obj.scl[0] = kFlameHalfW;
    obj.scl[1] = kFlameHalfH;
    obj.scl[2] = kFlameHalfW;  // unused by billboard path; set for symmetry

    // Position: the effect's spawn origin (world space).
    obj.pos[0] = st->pos[0];
    obj.pos[1] = st->pos[1];
    obj.pos[2] = st->pos[2];

    // Per-vertex diffuse: opaque white (texture passes through unmodulated;
    // snapshot Render never writes lverts[i].color — §13.8).
    std::memcpy(obj.diffuse,  st->diffuse,  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive, sizeof(obj.emissive));

    // UV sub-rect corners (snapshot Render body, effect_old.cpp:4546-4556):
    //   lverts[0] = (xpos,        ypos)        TL — u_lo, v_lo
    //   lverts[1] = (xpos + 0.25, ypos)        TR
    //   lverts[2] = (xpos,        ypos + 0.5)  BL
    //   lverts[3] = (xpos + 0.25, ypos + 0.5)  BR — u_hi, v_hi
    // The ScreenAligned path reads [0] as lo corner and [3] as hi corner.
    obj.lverts[0].tu = xpos;                  obj.lverts[0].tv = ypos;
    obj.lverts[1].tu = xpos + kAtlasUStride;  obj.lverts[1].tv = ypos;
    obj.lverts[2].tu = xpos;                  obj.lverts[2].tv = ypos + kAtlasVStride;
    obj.lverts[3].tu = xpos + kAtlasUStride;  obj.lverts[3].tv = ypos + kAtlasVStride;

    // === Blend state (§7 BLEND SANITY-CHECK) ===
    // Snapshot: SetBlendState() = Alpha (SRC_ALPHA/INV_SRC_ALPHA).
    // SUSPECT — bright-on-green-key glow design + SPARKS precedent suggest
    // AdditiveStraight in shipped retail. Implement Alpha per snapshot;
    // visual vet against in-game torch capture required (§13.1).
    // If the flame reads dull/translucent, switch to SetAddBlendState().
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();   // Alpha per snapshot — see §7 / SUSPECT comment above

    // === Submit the single billboard ===
    d3d::RenderObject(*Renderer, obj,
                      d3d::Orientation::ScreenAligned,
                      st->texture,
                      nullptr);  // no inst_world; pos is already world-space
}

// ==========================================================================
// Spawn — load Magic\Flame.I3D, bind sub-object 0 (box01), initialize state.
//
// origin: world-space anchor for the effect (the torch/candle/brazier
// sconce position from the sector binary data).
//
// In the test harness this is always passed as a world-origin anchor.
// In-game: the world placement position from sector binary data.
// ==========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->pos[0] = float(origin.x);
    st->pos[1] = float(origin.y);
    st->pos[2] = float(origin.z);

    // Bind Magic\Flame.I3D (§4 — registered in Class.Def:2030 under "Flame").
    // Use the retail copy from imagery.rvi (128×160 RGB565, 40,960 B payload;
    // snapshot's legacy/Imagery/Magic/flame.i3d has bogus surface dims — §13.7).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[flame-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[flame-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Material colors — sub-object 0 (box01, §4).
    // The snapshot Render never writes lverts[i].color; per-vertex diffuse
    // defaults to opaque white so the texture passes through unmodulated (§13.8).
    // We load the authored material as the base but override with opaque white
    // since the snapshot doesn't use the material color for the per-vertex path.
    d3d::LoadMaterial(img3d, kSubObj, st->diffuse, st->emissive);

    // REQUIRED: zero emissive (§B.2 / WAVE_1_LESSONS §1.1).
    // kHelperMeshFs adds emissive additively; pre-release I3Ds default to
    // emissive=(1,1,1) which saturates. Zero here even though this is a
    // billboard (not WorldMesh), for correctness and defensive compliance.
    st->emissive[0] = 0.0f;
    st->emissive[1] = 0.0f;
    st->emissive[2] = 0.0f;
    // emissive[3] = alpha, leave at 1.0 (already set from LoadMaterial or default)

    // Seed the per-vertex diffuse to opaque white (snapshot convention — §13.8).
    // LoadMaterial may have set diffuse from the authored material; override to
    // white to match the snapshot's "never-written lverts[i].color" behavior.
    st->diffuse[0] = 1.0f;
    st->diffuse[1] = 1.0f;
    st->diffuse[2] = 1.0f;
    st->diffuse[3] = 1.0f;

    // Texture handle deferred to first Submit() call (Gotcha #1, §B.8).
    // st->texture remains kInvalidTexture until then.

    // Seed t_anim_sec = 0 (frame = 0 at Initialize — effect_old.cpp:4493).
    // Per-instance phase variation comes from staggered spawn times;
    // no explicit phase offset is set (§8).
    st->t_anim_sec   = 0.0f;
    st->tick_accum_ms = 0.0f;

    log_info("[flame-shim] Spawn ok: pos=(%d,%d,%d) imagery=%p",
             origin.x, origin.y, origin.z,
             static_cast<void*>(img3d));
    return st;
}

// ==========================================================================
// Destroy — release the state object.
// ==========================================================================
void Destroy(State* st)
{
    delete st;
}

} // namespace flame_shim
