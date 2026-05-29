// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           weaponswipe.cpp - S09 TWeaponSwipe Phase B body             *
// *************************************************************************
//
// See weaponswipe.h for the design overview + retail constant mapping.
//
// *************************************************************************

#include "weaponswipe.h"

#include <algorithm>
#include <cmath>

#include "logging.h"
#include "time.h"

// =========================================================================
// Catmull-Rom spline — local copy so we don't drag hmm_vec3 into the
// header. Identical math to src/math3d.h `Spline` (verified against the
// pre-release callsite in TWeaponSwipe::GenerateStrip's `Spline(&avert,
// ratio, &points[k][max(0,o-1)], &points[k][o], &points[k][o+1],
// &points[k][o+2])`):
//
//   p(t) = 0.5 * ((2 * p1)
//                 + (-p0 + p2) * t
//                 + (2*p0 - 5*p1 + 4*p2 - p3) * t^2
//                 + (-p0 + 3*p1 - 3*p2 + p3) * t^3)
//
// p1/p2 are the two anchors the segment lies between; p0/p3 are the
// neighbours that bias the tangent at the endpoints.
// =========================================================================
void TWeaponSwipe::CatmullRom(float out[3], float t,
                              const float p0[3], const float p1[3],
                              const float p2[3], const float p3[3])
{
    const float t2 = t * t;
    const float t3 = t2 * t;
    for (int32_t i = 0; i < 3; ++i)
    {
        const float a = 2.0f * p1[i];
        const float b = (-p0[i] + p2[i]) * t;
        const float c = (2.0f * p0[i] - 5.0f * p1[i] + 4.0f * p2[i] - p3[i]) * t2;
        const float d = (-p0[i] + 3.0f * p1[i] - 3.0f * p2[i] + p3[i]) * t3;
        out[i] = 0.5f * (a + b + c + d);
    }
}

// =========================================================================
// Public surface
// =========================================================================

void TWeaponSwipe::SetColour(float r, float g, float b)
{
    // Pre-release `SetupWeaponSwipe()` short-circuits when the
    // character's `swipecolor == (0,0,0)`. We preserve that: zero
    // colour means the trail is fully disabled even if Tick is called.
    colour_zero_ = (r <= 0.0f && g <= 0.0f && b <= 0.0f);
    if (colour_zero_)
    {
        colour_[0] = colour_[1] = colour_[2] = 0.0f;
        return;
    }

    // NormalizeColors(): divide by max channel so the brightest hue
    // hits 1.0 (preserves hue, boosts brightness). Matches retail
    // body verbatim.
    const float most = (std::max)(r, (std::max)(g, b));
    if (most > 0.0f)
    {
        colour_[0] = (std::min)(1.0f, r / most);
        colour_[1] = (std::min)(1.0f, g / most);
        colour_[2] = (std::min)(1.0f, b / most);
    }
    else
    {
        colour_[0] = colour_[1] = colour_[2] = 1.0f;
    }
}

void TWeaponSwipe::ResetForNewWeapon()
{
    ring_.clear();
    seg_scratch_.clear();
    tick_accum_  = 0;
    initialized_ = true;
}

void TWeaponSwipe::TickAndSubmit(const float world_hilt[3],
                                 const float world_tip[3],
                                 bool        attack_visible,
                                 EFxDebugMode dbg)
{
    if (!Renderer)
        return;
    if (colour_zero_)
        return;
    if (!initialized_)
        ResetForNewWeapon();

    // ----- 1. Sim-tick gating -------------------------------------------
    // Retail animator runs at 24 Hz (`TCharacter::Pulse` cadence). We
    // gate ring advance on the same boundary so the trail history
    // resolution matches retail regardless of display rate (same
    // pattern as S01 stripeffect.cpp).
    const int32_t dt_ms = int32_t(double(TTime::DeltaTime()) * 1000.0);
    tick_accum_ += dt_ms;
    bool sim_tick = false;
    while (tick_accum_ >= kSimTickMs)
    {
        tick_accum_ -= kSimTickMs;
        sim_tick = true;
    }

    // ----- 2. Ring cycle + head insert -----------------------------------
    // CycleStrip equivalent: shift down, drop oldest pair off the end.
    // Then write the new (hilt, tip) at head[0]. We do this every sim
    // tick even when the weapon hasn't moved much — retail's
    // `Animate()` runs unconditionally on each Pulse() and the
    // resulting "stationary smear" on idle frames is the documented
    // death-by-overwrite behaviour (see Phase A forensics §3).
    if (sim_tick)
    {
        if (int32_t(ring_.size()) >= kMaxPoints)
            ring_.pop_back();
        // Shift via insert at front; ring is small (8), so the
        // O(n) shift is cheaper than the bookkeeping for a true
        // circular buffer (matches the retail naive memmove).
        ring_.insert(ring_.begin(), SPair{});
        SPair& head = ring_.front();
        head.hilt[0] = world_hilt[0];
        head.hilt[1] = world_hilt[1];
        head.hilt[2] = world_hilt[2];
        head.tip[0]  = world_tip[0];
        head.tip[1]  = world_tip[1];
        head.tip[2]  = world_tip[2];
    }

    // ----- 3. Visibility gate --------------------------------------------
    // Pre-release `TCharAnimator::Render()` only calls
    // `weaponswipe.Render()` when `IsAttack() && initialized && (player
    // ? primehand != nullptr : true)`. The ring still fills (so the
    // first attack frame has real history) but we only build + submit
    // segments when the attack flag is true and we have a non-trivial
    // ring (>= 2 pairs gives one Catmull-Rom interior segment).
    if (!attack_visible)
        return;
    if (ring_.size() < 2)
        return;

    // ----- 4. Build Catmull-Rom-splined segment list ---------------------
    // The pre-release `GenerateStrip()` iterates `o = 0 .. maxpoints-2`
    // (each `o` is the interval between ring[o+1] and ring[o+2] using
    // ring[o] / ring[o+3] as tangent biases). For each interval it
    // subdivides into `kSmooth` Catmull-Rom samples per row (hilt and
    // tip), packing per-vertex colour + alpha that fades along the
    // strip.
    //
    // Output mapping for SR pipeline:
    //   * Each consecutive smoothed sample yields one SStripSegment
    //     (hilt sample -> tip sample is the strip's *width*; adjacent
    //     samples along the strip's *length* are consecutive segments).
    //   * Width per segment = distance between hilt-row and tip-row at
    //     that sample. Retail derived this implicitly via per-vertex
    //     placement; we sample it directly so the SR shader expands it
    //     into the camera-aligned quad. This is the bespoke-vs-engine
    //     point — the engine SR pipeline needs the width up front.
    //
    // Colour curve (matches retail GenerateStrip):
    //   * `startr/g/b` begin at `min(colour + 0.4, 1.0)` (hilt-side
    //     brightness boost) and step toward the base colour at
    //     `wfade = 4.0 / maxverts` per *retail vertex*. Since one
    //     retail vertex pair corresponds to one of our segment
    //     endpoints, `kVertsPerEndpoint = 2` keeps the per-segment
    //     decay rate matching retail.
    //   * Alpha starts at `kAlphaHead = 0.3` and steps down at
    //     `fadeoutstep = 0.6 / maxverts` per vertex, clamped to 0.
    seg_scratch_.clear();
    const int32_t n_pairs = int32_t(ring_.size());
    if (n_pairs < 2)
        return;

    // Retail maxverts = (maxpoints-1) * smooth * 2 — match the same
    // per-vertex decay step so the strip's brightness/alpha curve has
    // the right cadence regardless of how full the ring currently is.
    constexpr int32_t kMaxVerts = (kMaxPoints - 1) * kSmooth * 2;
    constexpr float   kFadeOutStep = (kAlphaHead * 2.0f) / float(kMaxVerts);  // 0.6 / maxverts
    constexpr float   kWFade       = 4.0f / float(kMaxVerts);

    float startr = (std::min)(colour_[0] + kHiltBrightness, 1.0f);
    float startg = (std::min)(colour_[1] + kHiltBrightness, 1.0f);
    float startb = (std::min)(colour_[2] + kHiltBrightness, 1.0f);
    float alpha  = kAlphaHead;

    // Helper to advance the colour/alpha state by one retail "vertex".
    auto AdvanceCurve = [&]() {
        if (startr > colour_[0]) startr = (std::max)(colour_[0], startr - kWFade);
        if (startg > colour_[1]) startg = (std::max)(colour_[1], startg - kWFade);
        if (startb > colour_[2]) startb = (std::max)(colour_[2], startb - kWFade);
        alpha = (std::max)(0.0f, alpha - kFadeOutStep);
    };

    // Sample two interpolated points (hilt-row and tip-row) along the
    // current interval, return them in `out_hilt` / `out_tip` and
    // record the matching colour/alpha endpoint pair into `out_col`.
    auto SampleInterval = [&](int32_t o, int32_t i,
                              float out_hilt[3], float out_tip[3])
    {
        const float ratio = float(i) / float(kSmooth);
        const SPair& p0 = ring_[size_t((std::max)(0, o - 1))];
        const SPair& p1 = ring_[size_t(o)];
        const SPair& p2 = ring_[size_t((std::min)(o + 1, n_pairs - 1))];
        const SPair& p3 = ring_[size_t((std::min)(o + 2, n_pairs - 1))];
        CatmullRom(out_hilt, ratio, p0.hilt, p1.hilt, p2.hilt, p3.hilt);
        CatmullRom(out_tip,  ratio, p0.tip,  p1.tip,  p2.tip,  p3.tip);
    };

    // Build the segment list. For each adjacent (intra-interval or
    // interval-crossing) sample pair, emit one SStripSegment whose
    // width spans hilt -> tip and whose endpoint colours follow the
    // retail per-vertex curve.
    float prev_hilt[3] = {0.0f, 0.0f, 0.0f};
    float prev_tip [3] = {0.0f, 0.0f, 0.0f};
    float prev_r = startr, prev_g = startg, prev_b = startb, prev_a = alpha;
    bool  have_prev = false;

    for (int32_t o = 0; o < n_pairs - 1; ++o)
    {
        for (int32_t i = 0; i < kSmooth; ++i)
        {
            float h[3], t[3];
            SampleInterval(o, i, h, t);

            if (have_prev)
            {
                SStripSegment seg = {};
                // Segment runs *along the strip's length*, hilt-row to
                // hilt-row. World-A = previous hilt sample, world-B =
                // current hilt sample. The strip's *width* at each
                // endpoint = distance(hilt, tip). The SR shader
                // expands this to a camera-aligned quad of that
                // width around the centreline; for a sword swipe the
                // ribbon naturally tilts so its plane contains the
                // weapon axis (close enough to read as a slash arc).
                seg.world_a[0] = 0.5f * (prev_hilt[0] + prev_tip[0]);
                seg.world_a[1] = 0.5f * (prev_hilt[1] + prev_tip[1]);
                seg.world_a[2] = 0.5f * (prev_hilt[2] + prev_tip[2]);
                seg.world_b[0] = 0.5f * (h[0] + t[0]);
                seg.world_b[1] = 0.5f * (h[1] + t[1]);
                seg.world_b[2] = 0.5f * (h[2] + t[2]);

                const float dx_a = h[0] - prev_hilt[0];
                const float dy_a = h[1] - prev_hilt[1];
                const float dz_a = h[2] - prev_hilt[2];
                (void)dx_a; (void)dy_a; (void)dz_a;

                const float wa_dx = prev_hilt[0] - prev_tip[0];
                const float wa_dy = prev_hilt[1] - prev_tip[1];
                const float wa_dz = prev_hilt[2] - prev_tip[2];
                seg.width_a_wu = std::sqrt(wa_dx*wa_dx + wa_dy*wa_dy + wa_dz*wa_dz);

                const float wb_dx = h[0] - t[0];
                const float wb_dy = h[1] - t[1];
                const float wb_dz = h[2] - t[2];
                seg.width_b_wu = std::sqrt(wb_dx*wb_dx + wb_dy*wb_dy + wb_dz*wb_dz);

                seg.color_a[0] = prev_r; seg.color_a[1] = prev_g;
                seg.color_a[2] = prev_b; seg.color_a[3] = prev_a;
                seg.color_b[0] = startr; seg.color_b[1] = startg;
                seg.color_b[2] = startb; seg.color_b[3] = alpha;
                // Untextured retail strip uses pure per-vertex colour;
                // our SR pipeline still samples a texture so a uniform
                // u (0..1 across each segment) is fine — the renderer's
                // white texture will tint by the per-endpoint colour.
                seg.u_a = 0.0f;
                seg.u_b = 1.0f;
                seg_scratch_.push_back(seg);
            }

            // Save current sample as previous for next iteration, then
            // advance the retail per-vertex colour/alpha curve twice
            // (one decay step per retail vertex; one segment endpoint
            // == 2 retail vertices in the original triangle-strip
            // packing — hilt vertex + tip vertex).
            prev_hilt[0] = h[0]; prev_hilt[1] = h[1]; prev_hilt[2] = h[2];
            prev_tip [0] = t[0]; prev_tip [1] = t[1]; prev_tip [2] = t[2];
            prev_r = startr; prev_g = startg; prev_b = startb; prev_a = alpha;
            have_prev = true;

            AdvanceCurve();
            AdvanceCurve();
        }
    }

    if (seg_scratch_.empty())
        return;

    // ----- 5. Submit -----------------------------------------------------
    SStripDrawItem item = {};
    item.segments     = seg_scratch_.data();
    item.num_segments = int32_t(seg_scratch_.size());
    // Untextured retail strip uses per-vertex diffuse. SR pipeline
    // requires a texture handle; reuse the renderer's white texture so
    // the per-endpoint colour passes through unchanged. (Same approach
    // S01 took before its custom glow texture landed; see
    // stripeffect.cpp LightningGlowTexture() comment.)
    item.key.texture     = Renderer->WhiteTextureHandle();
    item.key.pipeline_id = uint16_t(EFxPipeline::Strip);
    // **Alpha blend (NOT additive)** — Phase A §4 spec: pre-release
    // explicitly sets SrcAlpha/InvSrcAlpha, and the per-vertex alpha
    // curve maxes at 0.3 so additive would never reach full intensity.
    // Divergence from S01 lightning (additive glow) is intentional.
    item.key.blend       = uint8_t(EFxBlend::Alpha);
    // ZWriteEnable = 0 in pre-release Render() — translucent ribbon
    // should not occlude geometry behind it.
    item.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    item.debug_mode      = dbg;
    item.light_mode      = EFxLightMode::Unlit;
    Renderer->SubmitFxStrip(item);

    // One-shot first-submit log so the harness shows the SR pipeline
    // is wired correctly (mirrors the [vfx] first_submit_logged
    // pattern in vfxtest.cpp). Subsequent submits are silent.
    if (!first_submit_logged_)
    {
        first_submit_logged_ = true;
        log_info("[weaponswipe] first submit: segments=%d ring=%zu "
                 "hilt=(%.0f,%.0f,%.0f) tip=(%.0f,%.0f,%.0f)",
                 int32_t(seg_scratch_.size()), ring_.size(),
                 double(world_hilt[0]), double(world_hilt[1]),
                 double(world_hilt[2]),
                 double(world_tip[0]),  double(world_tip[1]),
                 double(world_tip[2]));
    }
}
