// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           renderer.cpp  - Core Render Engine Implementation           *
// *************************************************************************
//
// This file implements TRenderer -- Revenant's game-agnostic core render
// engine. See renderer.h for the full architecture doc.
//
// Pass pipeline per-frame (see renderer.h ARCHITECTURE):
//
//      [1] G-buffer fill          -> default_pass (MRT)
//      [2] Ambient occlusion      -> ao_pass
//      [3] Sun shadow mask        -> shadow_pass + blur ping-pong
//      [4] Deferred lighting      -> lit_pass
//      [7] Composite to swapchain <- TDisplay::FlipPage via PresentToSwapchain
//
// Passes [4]-[6] (water, effects, debug 3D) are reserved.
// Passes [8] (debug UI) is the sokol_imgui overlay, rendered by TDisplay
// inside the swapchain pass after PresentToSwapchain.
//
// *************************************************************************

#include "renderer.h"

#include <sokol_gfx.h>
#include <sokol_app.h>

#include "bitmap.h"
#include "bitmapdecode.h"
#include "logging.h"
#include "surface.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>

#include "logging.h"
#include "particlefx.h"
#include "revenant.h"
#include "renderer_readback.h"
#include "shaders/shaders.h"
#include "surface.h"

// *************************************************************************
// * Global Renderer Pointer                                               *
// *************************************************************************
TRenderer* Renderer = nullptr;

// *************************************************************************
// * Shared GPU Resources (Unit Quad)                                      *
// *************************************************************************
//
// Shader source text lives in src/shaders/*.metal.h (plus future *.glsl.h
// and *.hlsl.h siblings). shaders.h picks the right backend at compile
// time and exposes kCompositeVs / kCompositeFs / kTileVs / kTileFs /
// kAOVs / kAOFs / kShadowVs / kShadowFs / kShadowBlurFs / kLightVs /
// kLightFs.

namespace {

struct SCompositeVertex { float x, y, u, v; };

constexpr int32_t kMaxTileProxyVertices = 524288;
constexpr int32_t kMaxTileProxyVerticesPerHull = 384;
constexpr int32_t kMaxTileProxyVerticesPerDraw = 2048;
constexpr int32_t kMaxTileTightProxyHullVertices = 64;
constexpr int32_t kMaxClipPolygonPoints = 512;
constexpr float kTileProxyHullPadPx = 1.5f;

struct SScreenPoint { float x, y; };

struct SHeightMinMaxMipChain
{
    std::vector<std::vector<float>> levels;
    uint64_t bytes = 0;
};

constexpr float kIsoCos30 = 0.867f;

static bool ValidHeight(float h)
{
    return std::fabs(h) > 0.0001f;
}

static bool ValidHeightRange(float mn, float mx)
{
    return ValidHeight(mn) || ValidHeight(mx);
}

static float Cross(const SScreenPoint& o, const SScreenPoint& a, const SScreenPoint& b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

static float PolygonArea(const SScreenPoint* p, int32_t n)
{
    if (!p || n < 3)
        return 0.0f;
    float twice_area = 0.0f;
    for (int32_t i = 0; i < n; ++i)
    {
        const SScreenPoint& a = p[i];
        const SScreenPoint& b = p[(i + 1) % n];
        twice_area += a.x * b.y - b.x * a.y;
    }
    return 0.5f * std::fabs(twice_area);
}

static int32_t ConvexHull(SScreenPoint* points, int32_t count, SScreenPoint* hull)
{
    if (!points || !hull || count <= 0)
        return 0;

    std::sort(points, points + count, [](const SScreenPoint& a, const SScreenPoint& b) {
        if (a.x == b.x) return a.y < b.y;
        return a.x < b.x;
    });

    int32_t n = 0;
    for (int32_t i = 0; i < count; ++i)
    {
        while (n >= 2 && Cross(hull[n - 2], hull[n - 1], points[i]) <= 0.0f)
            --n;
        hull[n++] = points[i];
    }
    const int32_t lower_n = n;
    for (int32_t i = count - 2; i >= 0; --i)
    {
        while (n > lower_n && Cross(hull[n - 2], hull[n - 1], points[i]) <= 0.0f)
            --n;
        hull[n++] = points[i];
    }
    if (n > 1)
        --n;
    return n;
}

static bool ClipInside(const SScreenPoint& p, int32_t edge, float bound)
{
    switch (edge)
    {
    case 0: return p.x >= bound;
    case 1: return p.x <= bound;
    case 2: return p.y >= bound;
    default: return p.y <= bound;
    }
}

static SScreenPoint ClipIntersect(const SScreenPoint& a, const SScreenPoint& b,
                                  int32_t edge, float bound)
{
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    float t = 0.0f;
    if (edge < 2)
        t = (std::fabs(dx) > 1e-6f) ? ((bound - a.x) / dx) : 0.0f;
    else
        t = (std::fabs(dy) > 1e-6f) ? ((bound - a.y) / dy) : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);
    return { a.x + dx * t, a.y + dy * t };
}

static int32_t ClipPolygonAgainstEdge(const SScreenPoint* in, int32_t in_count,
                                      SScreenPoint* out, int32_t out_cap,
                                      int32_t edge, float bound)
{
    if (!in || !out || in_count <= 0 || out_cap <= 0)
        return 0;

    int32_t out_count = 0;
    bool overflow = false;
    auto emit = [&](const SScreenPoint& p) {
        if (out_count >= out_cap)
        {
            overflow = true;
            return;
        }
        out[out_count++] = p;
    };
    SScreenPoint prev = in[in_count - 1];
    bool prev_inside = ClipInside(prev, edge, bound);
    for (int32_t i = 0; i < in_count; ++i)
    {
        const SScreenPoint cur = in[i];
        const bool cur_inside = ClipInside(cur, edge, bound);
        if (cur_inside != prev_inside)
            emit(ClipIntersect(prev, cur, edge, bound));
        if (cur_inside)
            emit(cur);
        prev = cur;
        prev_inside = cur_inside;
    }
    if (overflow)
        return 0;
    return out_count;
}

static int32_t ClipPolygonToRect(const SScreenPoint* in, int32_t in_count,
                                 float x0, float y0, float x1, float y1,
                                 SScreenPoint* out)
{
    if (!in || !out || in_count <= 0 || in_count > kMaxClipPolygonPoints)
        return 0;

    SScreenPoint tmp_a[kMaxClipPolygonPoints] = {};
    SScreenPoint tmp_b[kMaxClipPolygonPoints] = {};
    int32_t count = in_count;
    for (int32_t i = 0; i < count; ++i)
        tmp_a[i] = in[i];

    count = ClipPolygonAgainstEdge(tmp_a, count, tmp_b, kMaxClipPolygonPoints, 0, x0);
    count = ClipPolygonAgainstEdge(tmp_b, count, tmp_a, kMaxClipPolygonPoints, 1, x1);
    count = ClipPolygonAgainstEdge(tmp_a, count, tmp_b, kMaxClipPolygonPoints, 2, y0);
    count = ClipPolygonAgainstEdge(tmp_b, count, tmp_a, kMaxClipPolygonPoints, 3, y1);
    for (int32_t i = 0; i < count; ++i)
        out[i] = tmp_a[i];
    return count;
}

static int32_t BuildProjectedProxyVertices(const SScreenPoint* hull,
                                           int32_t hull_count,
                                           int32_t gbw,
                                           int32_t gbh,
                                           float* out_vertices,
                                           int32_t max_vertices,
                                           float& projected_area,
                                           float& clipped_area,
                                           float raster_scale = 1.0f,
                                           bool wireframe = false)
{
    clipped_area = 0.0f;
    projected_area = 0.0f;
    if (!out_vertices || !hull || hull_count < 3 ||
        hull_count > kMaxClipPolygonPoints || raster_scale <= 0.0f)
        return 0;

    SScreenPoint scaled[kMaxClipPolygonPoints] = {};
    const SScreenPoint* draw_hull = hull;
    if (std::fabs(raster_scale - 1.0f) > 1.0e-4f)
    {
        float cx = 0.0f;
        float cy = 0.0f;
        for (int32_t i = 0; i < hull_count; ++i)
        {
            cx += hull[i].x;
            cy += hull[i].y;
        }
        cx /= float(hull_count);
        cy /= float(hull_count);
        for (int32_t i = 0; i < hull_count; ++i)
        {
            scaled[i].x = cx + (hull[i].x - cx) * raster_scale;
            scaled[i].y = cy + (hull[i].y - cy) * raster_scale;
        }
        draw_hull = scaled;
    }

    projected_area = PolygonArea(draw_hull, hull_count);
    if (projected_area <= 0.5f)
        return 0;

    SScreenPoint clipped[kMaxClipPolygonPoints] = {};
    const int32_t clipped_count = ClipPolygonToRect(draw_hull, hull_count,
                                                   0.0f, 0.0f,
                                                   float(gbw), float(gbh),
                                                   clipped);
    clipped_area = PolygonArea(clipped, clipped_count);
    if (clipped_count < 3 || clipped_area <= 0.5f)
        return 0;

    if (wireframe)
    {
        constexpr float kProxyWireWidthPx = 2.0f;
        const float half_width = 0.5f * kProxyWireWidthPx;
        int32_t valid_edges = 0;
        for (int32_t i = 0; i < clipped_count; ++i)
        {
            const SScreenPoint& a = clipped[i];
            const SScreenPoint& b = clipped[(i + 1) % clipped_count];
            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            if (dx * dx + dy * dy > 1.0e-4f)
                ++valid_edges;
        }

        const int32_t vertex_count = valid_edges * 6;
        if (vertex_count <= 0 || vertex_count > max_vertices)
            return 0;

        auto emit_px = [&](int32_t& out, const SScreenPoint& p) {
            out_vertices[out++] = 2.0f * p.x / float(gbw) - 1.0f;
            out_vertices[out++] = 1.0f - 2.0f * p.y / float(gbh);
            out_vertices[out++] = 0.5f;
            out_vertices[out++] = 0.5f;
        };

        int32_t out = 0;
        for (int32_t i = 0; i < clipped_count; ++i)
        {
            const SScreenPoint& a = clipped[i];
            const SScreenPoint& b = clipped[(i + 1) % clipped_count];
            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            const float len = std::sqrt(dx * dx + dy * dy);
            if (len <= 1.0e-4f)
                continue;
            const float nx = -dy / len * half_width;
            const float ny =  dx / len * half_width;
            const SScreenPoint p0 = { a.x + nx, a.y + ny };
            const SScreenPoint p1 = { b.x + nx, b.y + ny };
            const SScreenPoint p2 = { b.x - nx, b.y - ny };
            const SScreenPoint p3 = { a.x - nx, a.y - ny };
            emit_px(out, p0);
            emit_px(out, p1);
            emit_px(out, p2);
            emit_px(out, p0);
            emit_px(out, p2);
            emit_px(out, p3);
        }
        return out / 4;
    }

    const int32_t vertex_count = (clipped_count - 2) * 3;
    if (vertex_count > max_vertices)
        return 0;

    auto emit = [&](int32_t& out, const SScreenPoint& p, float u, float v) {
        out_vertices[out++] = 2.0f * p.x / float(gbw) - 1.0f;
        out_vertices[out++] = 1.0f - 2.0f * p.y / float(gbh);
        // Proxy draws use ray-reconstructed UVs in normal relief mode. In
        // filled debug mode these slots carry per-triangle barycentrics.
        // Wire debug uses explicit edge quads and does not expose fan edges.
        out_vertices[out++] = u;
        out_vertices[out++] = v;
    };

    int32_t out = 0;
    for (int32_t i = 1; i + 1 < clipped_count; ++i)
    {
        emit(out, clipped[0],     1.0f, 0.0f);
        emit(out, clipped[i],     0.0f, 1.0f);
        emit(out, clipped[i + 1], 0.0f, 0.0f);
    }
    return vertex_count;
}

static SScreenPoint PadProxyHullPoint(const SScreenPoint& p, float cx, float cy)
{
    const float dx = p.x - cx;
    const float dy = p.y - cy;
    const float len = std::sqrt(dx * dx + dy * dy);
    if (len <= 1.0e-4f)
        return p;
    const float s = kTileProxyHullPadPx / len;
    return { p.x + dx * s, p.y + dy * s };
}

static void AddPaddedHull(std::vector<float>& out,
                          const SScreenPoint* hull,
                          int32_t hull_count)
{
    out.clear();
    if (!hull || hull_count < 3)
        return;

    float cx = 0.0f;
    float cy = 0.0f;
    for (int32_t i = 0; i < hull_count; ++i)
    {
        cx += hull[i].x;
        cy += hull[i].y;
    }
    cx /= float(hull_count);
    cy /= float(hull_count);

    out.reserve(size_t(hull_count) * 2u);
    for (int32_t i = 0; i < hull_count; ++i)
    {
        const SScreenPoint p = PadProxyHullPoint(hull[i], cx, cy);
        out.push_back(p.x);
        out.push_back(p.y);
    }
}

static void AddIsoLocalPointForSourcePixel(float sx, float sy, float zraw,
                                           float& min_wx, float& min_wy, float& min_wz,
                                           float& max_wx, float& max_wy, float& max_wz)
{
    const float sum = 2.0f * (sy + zraw * kIsoCos30);
    const float wx = 0.5f * (sum + sx);
    const float wy = 0.5f * (sum - sx);
    const float wz = zraw;
    min_wx = (std::min)(min_wx, wx);
    min_wy = (std::min)(min_wy, wy);
    min_wz = (std::min)(min_wz, wz);
    max_wx = (std::max)(max_wx, wx);
    max_wy = (std::max)(max_wy, wy);
    max_wz = (std::max)(max_wz, wz);
}

static SScreenPoint IsoLocalToSourcePoint(float wx, float wy, float wz)
{
    return {
        wx - wy,
        0.5f * (wx + wy) - wz * kIsoCos30
    };
}

static std::vector<float> BuildTileTightProxyHull(const float* depth,
                                                  int32_t width,
                                                  int32_t height)
{
    std::vector<float> out;
    if (!depth || width <= 0 || height <= 0)
        return out;

    // Build a source-space front-face proxy once per immutable image pair.
    // The common grid signatures fall out of the blurred occupied-depth mask:
    //   - flat ground tiles become diamond hulls,
    //   - wall tiles become trapezoids,
    //   - boxes/columns become compact rectangular hulls,
    //   - trees/large complex objects first close noisy holes, then use the
    //     convex simplified silhouette plus the projected local box.
    // Runtime draws only transform this small 3D outline slab; no per-frame
    // allocation or per-cell proxy generation is allowed here.
    std::vector<uint8_t> raw_mask(size_t(width) * size_t(height), 0);
    std::vector<uint8_t> proxy_mask(size_t(width) * size_t(height), 0);
    std::vector<float> proxy_depth(size_t(width) * size_t(height), 0.0f);
    std::vector<SScreenPoint> boundary_points;
    boundary_points.reserve(size_t(width + height) * 4u);
    std::vector<SScreenPoint> box_points;
    box_points.reserve(8u);

    int32_t bbox_x0 = width;
    int32_t bbox_y0 = height;
    int32_t bbox_x1 = 0;
    int32_t bbox_y1 = 0;
    float min_wx =  FLT_MAX, min_wy =  FLT_MAX, min_wz =  FLT_MAX;
    float max_wx = -FLT_MAX, max_wy = -FLT_MAX, max_wz = -FLT_MAX;
    bool any = false;

    auto add_boundary_corners = [&](int32_t x, int32_t y) {
        boundary_points.push_back({ float(x),     float(y) });
        boundary_points.push_back({ float(x + 1), float(y) });
        boundary_points.push_back({ float(x),     float(y + 1) });
        boundary_points.push_back({ float(x + 1), float(y + 1) });
    };

    for (int32_t y = 0; y < height; ++y)
    for (int32_t x = 0; x < width; ++x)
    {
        const float zraw = depth[size_t(y) * size_t(width) + size_t(x)];
        if (!ValidHeight(zraw))
            continue;

        any = true;
        raw_mask[size_t(y) * size_t(width) + size_t(x)] = 1;
        bbox_x0 = (std::min)(bbox_x0, x);
        bbox_y0 = (std::min)(bbox_y0, y);
        bbox_x1 = (std::max)(bbox_x1, x + 1);
        bbox_y1 = (std::max)(bbox_y1, y + 1);

        AddIsoLocalPointForSourcePixel(float(x),     float(y),     zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x + 1), float(y),     zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x),     float(y + 1), zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x + 1), float(y + 1), zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
    }

    if (!any)
        return out;

    auto raw_valid_at = [&](int32_t x, int32_t y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return false;
        return raw_mask[size_t(y) * size_t(width) + size_t(x)] != 0;
    };

    // Asset-side blur/close over the occupancy map. This intentionally uses a
    // small box filter instead of preserving every alpha/depth notch. It gives
    // tree silhouettes and other complex props a stable convex proxy without
    // pushing detailed bitmap noise into the per-frame stream buffer.
    constexpr int32_t kProxyBlurRadius = 2;
    constexpr int32_t kProxyBlurThreshold = 5;
    for (int32_t y = 0; y < height; ++y)
    for (int32_t x = 0; x < width; ++x)
    {
        int32_t count = 0;
        float zsum = 0.0f;
        for (int32_t yy = y - kProxyBlurRadius; yy <= y + kProxyBlurRadius; ++yy)
        for (int32_t xx = x - kProxyBlurRadius; xx <= x + kProxyBlurRadius; ++xx)
            if (raw_valid_at(xx, yy))
            {
                ++count;
                zsum += depth[size_t(yy) * size_t(width) + size_t(xx)];
            }
        if (raw_valid_at(x, y) || count >= kProxyBlurThreshold)
        {
            proxy_mask[size_t(y) * size_t(width) + size_t(x)] = 1;
            proxy_depth[size_t(y) * size_t(width) + size_t(x)] =
                count > 0 ? (zsum / float(count)) : depth[size_t(y) * size_t(width) + size_t(x)];
        }
    }

    auto proxy_valid_at = [&](int32_t x, int32_t y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return false;
        return proxy_mask[size_t(y) * size_t(width) + size_t(x)] != 0;
    };

    for (int32_t y = 0; y < height; ++y)
    for (int32_t x = 0; x < width; ++x)
    {
        if (!proxy_valid_at(x, y))
            continue;
        const float zraw = proxy_depth[size_t(y) * size_t(width) + size_t(x)];
        AddIsoLocalPointForSourcePixel(float(x),     float(y),     zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x + 1), float(y),     zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x),     float(y + 1), zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        AddIsoLocalPointForSourcePixel(float(x + 1), float(y + 1), zraw, min_wx, min_wy, min_wz, max_wx, max_wy, max_wz);
        const bool boundary = !proxy_valid_at(x - 1, y) || !proxy_valid_at(x + 1, y) ||
                              !proxy_valid_at(x, y - 1) || !proxy_valid_at(x, y + 1);
        if (boundary)
            add_boundary_corners(x, y);
    }

    if (boundary_points.empty())
    {
        boundary_points.push_back({ float(bbox_x0), float(bbox_y0) });
        boundary_points.push_back({ float(bbox_x1), float(bbox_y0) });
        boundary_points.push_back({ float(bbox_x1), float(bbox_y1) });
        boundary_points.push_back({ float(bbox_x0), float(bbox_y1) });
    }

    for (float wx : { min_wx, max_wx })
    for (float wy : { min_wy, max_wy })
    for (float wz : { min_wz, max_wz })
        box_points.push_back(IsoLocalToSourcePoint(wx, wy, wz));

    std::vector<SScreenPoint> points = boundary_points;
    points.insert(points.end(), box_points.begin(), box_points.end());
    std::vector<SScreenPoint> hull(points.size());
    int32_t hull_count = ConvexHull(points.data(), int32_t(points.size()), hull.data());
    if (hull_count >= 3 && hull_count <= kMaxTileTightProxyHullVertices)
    {
        AddPaddedHull(out, hull.data(), hull_count);
        return out;
    }

    // Complex silhouettes are usually trees/large props. A huge detailed hull
    // costs more stream vertices than it saves, so use the projected local box
    // front-face instead of a rect grid. It remains conservative and keeps one
    // proxy draw per tile.
    std::vector<SScreenPoint> box_hull(box_points.size());
    hull_count = ConvexHull(box_points.data(), int32_t(box_points.size()), box_hull.data());
    if (hull_count >= 3)
    {
        AddPaddedHull(out, box_hull.data(), hull_count);
        return out;
    }

    const SScreenPoint rect[4] = {
        { float(bbox_x0), float(bbox_y0) },
        { float(bbox_x1), float(bbox_y0) },
        { float(bbox_x1), float(bbox_y1) },
        { float(bbox_x0), float(bbox_y1) },
    };
    AddPaddedHull(out, rect, 4);
    return out;
}

static int32_t BuildProjectedSourceHullProxy(const float* source_hull_xy,
                                             int32_t source_point_count,
                                             float anchor_px_x,
                                             float anchor_px_y,
                                             float anchor_cam_x,
                                             float anchor_cam_y,
                                             float focal_zoom,
                                             float tile_scale,
                                             float screen_ox,
                                             float screen_oy,
                                             float z0,
                                             float z1,
                                             int32_t gbw,
                                             int32_t gbh,
                                             float* out_vertices,
                                             int32_t max_vertices,
                                             float& projected_area,
                                             float& clipped_area,
                                             float raster_scale = 1.0f,
                                             bool wireframe = false,
                                             float* out_min_x = nullptr,
                                             float* out_min_y = nullptr,
                                             float* out_max_x = nullptr,
                                             float* out_max_y = nullptr)
{
    projected_area = 0.0f;
    clipped_area = 0.0f;
    if (!source_hull_xy || source_point_count < 3 ||
        source_point_count > kMaxTileTightProxyHullVertices ||
        z0 <= 0.0f || z1 <= 0.0f)
        return 0;

    if (z1 < z0)
        std::swap(z0, z1);

    SScreenPoint projected[kMaxClipPolygonPoints] = {};
    int32_t projected_count = 0;
    float min_x =  FLT_MAX, min_y =  FLT_MAX;
    float max_x = -FLT_MAX, max_y = -FLT_MAX;

    for (float z : { z0, z1 })
    {
        for (int32_t i = 0; i < source_point_count; ++i)
        {
            const float sx = (source_hull_xy[i * 2 + 0] - anchor_px_x) * tile_scale;
            const float sy = (source_hull_xy[i * 2 + 1] - anchor_px_y) * tile_scale;
            const float qx = screen_ox + (anchor_cam_x + sx) * focal_zoom / z;
            const float qy = screen_oy + (anchor_cam_y + sy) * focal_zoom / z;
            projected[projected_count++] = { qx, qy };
            min_x = (std::min)(min_x, qx);
            min_y = (std::min)(min_y, qy);
            max_x = (std::max)(max_x, qx);
            max_y = (std::max)(max_y, qy);
        }
    }

    if (out_min_x) *out_min_x = min_x;
    if (out_min_y) *out_min_y = min_y;
    if (out_max_x) *out_max_x = max_x;
    if (out_max_y) *out_max_y = max_y;

    SScreenPoint hull[kMaxClipPolygonPoints] = {};
    const int32_t hull_count = ConvexHull(projected, projected_count, hull);
    return BuildProjectedProxyVertices(hull,
                                       hull_count,
                                       gbw,
                                       gbh,
                                       out_vertices,
                                       max_vertices,
                                       projected_area,
                                       clipped_area,
                                       raster_scale,
                                       wireframe);
}

static int32_t BuildProjectedSourceRectProxy(float src_x0,
                                             float src_y0,
                                             float src_x1,
                                             float src_y1,
                                             float anchor_px_x,
                                             float anchor_px_y,
                                             float anchor_cam_x,
                                             float anchor_cam_y,
                                             float focal_zoom,
                                             float tile_scale,
                                             float screen_ox,
                                             float screen_oy,
                                             float z0,
                                             float z1,
                                             int32_t gbw,
                                             int32_t gbh,
                                             float* out_vertices,
                                             int32_t max_vertices,
                                             float& projected_area,
                                             float& clipped_area,
                                             float raster_scale = 1.0f,
                                             bool wireframe = false,
                                             float* out_min_x = nullptr,
                                             float* out_min_y = nullptr,
                                             float* out_max_x = nullptr,
                                             float* out_max_y = nullptr)
{
    if (!(src_x0 < src_x1 && src_y0 < src_y1) || z0 <= 0.0f || z1 <= 0.0f)
    {
        projected_area = 0.0f;
        clipped_area = 0.0f;
        return 0;
    }
    const float hull[8] = {
        src_x0, src_y0,
        src_x1, src_y0,
        src_x1, src_y1,
        src_x0, src_y1,
    };
    return BuildProjectedSourceHullProxy(hull,
                                         4,
                                         anchor_px_x, anchor_px_y,
                                         anchor_cam_x, anchor_cam_y,
                                         focal_zoom, tile_scale,
                                         screen_ox, screen_oy,
                                         z0, z1,
                                         gbw, gbh,
                                         out_vertices,
                                         max_vertices,
                                         projected_area,
                                         clipped_area,
                                         raster_scale,
                                         wireframe,
                                         out_min_x, out_min_y,
                                         out_max_x, out_max_y);
}

static SHeightMinMaxMipChain BuildHeightMinMaxMipChain(const float* depth,
                                                       int32_t width,
                                                       int32_t height)
{
    SHeightMinMaxMipChain chain = {};
    if (!depth || width <= 0 || height <= 0)
        return chain;

    std::vector<float> level(size_t(width) * size_t(height) * 2u, 0.0f);
    for (int32_t y = 0; y < height; ++y)
    for (int32_t x = 0; x < width; ++x)
    {
        const float z = depth[size_t(y) * size_t(width) + size_t(x)];
        if (ValidHeight(z))
        {
            const size_t i = (size_t(y) * size_t(width) + size_t(x)) * 2u;
            level[i + 0] = z;
            level[i + 1] = z;
        }
    }

    int32_t w = width;
    int32_t h = height;
    for (int32_t mip = 0; mip < SG_MAX_MIPMAPS; ++mip)
    {
        chain.bytes += uint64_t(w) * uint64_t(h) * uint64_t(sizeof(float) * 2u);
        chain.levels.push_back(std::move(level));
        if (w == 1 && h == 1)
            break;

        const int32_t prev_w = w;
        const int32_t prev_h = h;
        const std::vector<float>& prev = chain.levels.back();
        w = (std::max)(1, prev_w / 2);
        h = (std::max)(1, prev_h / 2);
        level.assign(size_t(w) * size_t(h) * 2u, 0.0f);

        for (int32_t y = 0; y < h; ++y)
        for (int32_t x = 0; x < w; ++x)
        {
            float mn =  FLT_MAX;
            float mx = -FLT_MAX;
            bool any = false;
            const int32_t src_x0 = (std::max)(0, int32_t(std::floor(float(x) * float(prev_w) / float(w))));
            const int32_t src_y0 = (std::max)(0, int32_t(std::floor(float(y) * float(prev_h) / float(h))));
            const int32_t src_x1 = (std::min)(prev_w, int32_t(std::ceil(float(x + 1) * float(prev_w) / float(w))));
            const int32_t src_y1 = (std::min)(prev_h, int32_t(std::ceil(float(y + 1) * float(prev_h) / float(h))));
            for (int32_t py = src_y0; py < src_y1; ++py)
            for (int32_t px = src_x0; px < src_x1; ++px)
            {
                const size_t pi = (size_t(py) * size_t(prev_w) + size_t(px)) * 2u;
                const float cmn = prev[pi + 0];
                const float cmx = prev[pi + 1];
                if (!ValidHeightRange(cmn, cmx))
                    continue;

                mn = (std::min)(mn, cmn);
                mx = (std::max)(mx, cmx);
                any = true;
            }

            if (any)
            {
                const size_t di = (size_t(y) * size_t(w) + size_t(x)) * 2u;
                level[di + 0] = mn;
                level[di + 1] = mx;
            }
        }
    }

    return chain;
}

#if defined(SOKOL_METAL)
inline constexpr const char* kHelperMeshVs = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
struct helper_vs_params {
    float4 w0; float4 w1; float4 w2; float4 w3;
    float4 vp; float4 camz; float4 camw;
};
struct vs_in {
    float3 pos    [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 uv     [[attribute(2)]];
};
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant helper_vs_params& p [[buffer(0)]]) {
    float4 ph = float4(in.pos, 1.0);
    float3 wp = float3(dot(p.w0, ph), dot(p.w1, ph), dot(p.w2, ph));
    float3 wn = normalize(float3(dot(p.w0.xyz, in.normal),
                                 dot(p.w1.xyz, in.normal),
                                 dot(p.w2.xyz, in.normal)));
    float wx = wp.x - p.camw.x;
    float wy = wp.y - p.camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * ISO_COS30;
    float scene_z_wu = p.camz.z - ISO_COS30 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - p.camz.x) / max(p.camz.y, 1e-6);
    float zoom = max(p.camw.z, 0.0001);
    float persp_scale = ((p.camz.w > 0.5) ? (p.camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = p.vp.x + S * persp_scale;
    float spy = p.vp.y + T * persp_scale;
    vs_out o;
    o.pos.x = 2.0 * spx / max(p.vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(p.vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    o.wpos = wp;
    o.wnormal = wn;
    o.uv = in.uv;
    return o;
}
)MSL";
inline constexpr const char* kHelperMeshFs = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float  scene_z;
};
struct helper_params {
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 emissive;
    float4 light_dir_i;
    float4 light_col_a;
    float4 view_dir_power;
};
fragment float4 _main(vs_out in [[stage_in]],
                      constant helper_params& hp [[buffer(0)]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    float4 tex = albedo_tex.sample(smp, in.uv);
    float alpha = tex.a * max(max(hp.diffuse.a, hp.ambient.a), max(hp.specular.a, hp.emissive.a));
    if (alpha < 0.01) discard_fragment();
    float3 N = normalize(in.wnormal);
    float3 L = normalize(hp.light_dir_i.xyz);
    float3 V = normalize(hp.view_dir_power.xyz);
    float3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float spec = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(hp.view_dir_power.w, 1.0)) : 0.0;
    float3 base = tex.rgb * hp.diffuse.rgb;
    float3 col = base * (hp.ambient.rgb * hp.light_col_a.w + hp.light_col_a.rgb * hp.light_dir_i.w * ndl)
               + hp.specular.rgb * spec * hp.light_dir_i.w
               + hp.emissive.rgb;
    return float4(col, alpha);
}
)MSL";
inline constexpr const char* kTransparentTileFs = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;
                float4 filter; float4 obj_id; float4 camera; float4 proj; float4 tile_rect;
                float4 cam_tile; };
struct vs_out { float4 pos [[position]]; float2 uv; };
struct fs_out { float4 color [[color(0)]];
                float  depth [[depth(any)]]; };
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> color_tex [[texture(0)]],
                      texture2d<float> depth_tex [[texture(1)]],
                      sampler smp [[sampler(0)]],
                      constant params& p [[buffer(0)]]) {
    fs_out o;
    float4 c = color_tex.sample(smp, in.uv);
    if (c.a < 0.01) discard_fragment();
    float zraw = depth_tex.sample(smp, in.uv).r;
    float d    = p.zparams.x + zraw * p.zparams.y;
    if (d < 0.0 || d > 1.0) discard_fragment();
    o.color = c;
    o.depth = d;
    return o;
}
)MSL";
#elif defined(SOKOL_GLCORE33) || defined(SOKOL_GLES3)
inline constexpr const char* kHelperMeshVs = R"GLSL(
#version 330
#define ISO_COS30 0.867
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(std140) uniform helper_vs_params {
    vec4 w0; vec4 w1; vec4 w2; vec4 w3;
    vec4 vp; vec4 camz; vec4 camw;
};
out vec3  v_wpos;
out vec3  v_wnormal;
out vec2  v_uv;
void main() {
    vec4 ph = vec4(pos, 1.0);
    vec3 wp = vec3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    vec3 wn = normalize(vec3(dot(w0.xyz, normal), dot(w1.xyz, normal), dot(w2.xyz, normal)));
    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * ISO_COS30;
    float scene_z_wu = camz.z - ISO_COS30 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale;
    float spy = vp.y + T * persp_scale;
    gl_Position = vec4(2.0 * spx / max(vp.z, 1.0) - 1.0,
                       1.0 - 2.0 * spy / max(vp.w, 1.0),
                       scene_z_n, 1.0);
    v_wpos = wp;
    v_wnormal = wn;
    v_uv = uv;
}
)GLSL";
inline constexpr const char* kHelperMeshFs = R"GLSL(
#version 330
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
layout(std140) uniform helper_params {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 light_dir_i;
    vec4 light_col_a;
    vec4 view_dir_power;
};
uniform sampler2D albedo_tex;
out vec4 fragColor;
void main() {
    vec4 tex = texture(albedo_tex, v_uv);
    float alpha = tex.a * max(max(diffuse.a, ambient.a), max(specular.a, emissive.a));
    if (alpha < 0.01) discard;
    vec3 N = normalize(v_wnormal);
    vec3 L = normalize(light_dir_i.xyz);
    vec3 V = normalize(view_dir_power.xyz);
    vec3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float spec = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(view_dir_power.w, 1.0)) : 0.0;
    vec3 base = tex.rgb * diffuse.rgb;
    vec3 col = base * (ambient.rgb * light_col_a.w + light_col_a.rgb * light_dir_i.w * ndl)
             + specular.rgb * spec * light_dir_i.w
             + emissive.rgb;
    fragColor = vec4(col, alpha);
}
)GLSL";
inline constexpr const char* kTransparentTileFs = R"GLSL(
#version 330
layout(std140) uniform params {
    vec4 rect;
    vec4 zparams;
    vec4 tile_root;
    vec4 tile_sprite;
    vec4 filter_;
    vec4 obj_id;
    vec4 camera;
};
in vec2 v_uv;
uniform sampler2D color_tex;
uniform sampler2D depth_tex;
out vec4 fragColor;
void main() {
    vec4 c = texture(color_tex, v_uv);
    if (c.a < 0.01) discard;
    float zraw = texture(depth_tex, v_uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    fragColor = c;
    gl_FragDepth = d;
}
)GLSL";
#elif defined(SOKOL_D3D11)
inline constexpr const char* kHelperMeshVs = R"HLSL(
#define ISO_COS30 0.867
cbuffer helper_vs_params : register(b0) {
    float4 w0; float4 w1; float4 w2; float4 w3;
    float4 vp; float4 camz; float4 camw;
};
struct vs_in {
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD0;
};
struct vs_out {
    float4 pos     : SV_Position;
    float3 wpos    : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv      : TEXCOORD2;
};
vs_out main_vs(vs_in i) {
    float4 ph = float4(i.pos, 1.0);
    float3 wp = float3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    float3 wn = normalize(float3(dot(w0.xyz, i.normal), dot(w1.xyz, i.normal), dot(w2.xyz, i.normal)));
    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * ISO_COS30;
    float scene_z_wu = camz.z - ISO_COS30 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale;
    float spy = vp.y + T * persp_scale;
    vs_out o;
    o.pos = float4(2.0 * spx / max(vp.z, 1.0) - 1.0,
                   1.0 - 2.0 * spy / max(vp.w, 1.0),
                   scene_z_n, 1.0);
    o.wpos = wp;
    o.wnormal = wn;
    o.uv = i.uv;
    return o;
}
)HLSL";
inline constexpr const char* kHelperMeshFs = R"HLSL(
cbuffer helper_params : register(b0) {
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 emissive;
    float4 light_dir_i;
    float4 light_col_a;
    float4 view_dir_power;
};
Texture2D albedo_tex : register(t0);
SamplerState smp : register(s0);
struct ps_in {
    float4 pos : SV_POSITION;
    float3 wpos : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tint : TEXCOORD3;
    float  scene_z : TEXCOORD4;
};
float4 main_ps(ps_in input) : SV_Target0 {
    float4 tex = albedo_tex.Sample(smp, input.uv);
    float alpha = tex.a * max(max(diffuse.a, ambient.a), max(specular.a, emissive.a));
    clip(alpha - 0.01);
    float3 N = normalize(input.wnormal);
    float3 L = normalize(light_dir_i.xyz);
    float3 V = normalize(view_dir_power.xyz);
    float3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float specPow = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(view_dir_power.w, 1.0)) : 0.0;
    float3 base = tex.rgb * diffuse.rgb;
    float3 col = base * (ambient.rgb * light_col_a.w + light_col_a.rgb * light_dir_i.w * ndl)
               + specular.rgb * specPow * light_dir_i.w
               + emissive.rgb;
    return float4(col, alpha);
}
)HLSL";
inline constexpr const char* kTransparentTileFs = R"HLSL(
cbuffer params : register(b0) {
    float4 rect;
    float4 zparams;
    float4 tile_root;
    float4 tile_sprite;
    float4 filter_;
    float4 obj_id;
    float4 camera;
};
Texture2D    color_tex : register(t0);
Texture2D    depth_tex : register(t1);
SamplerState smp       : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
struct fs_out {
    float4 color : SV_Target0;
    float  depth : SV_Depth;
};
fs_out main_ps(vs_out in_) {
    fs_out o;
    float4 c = color_tex.Sample(smp, in_.uv);
    if (c.a < 0.01) discard;
    float zraw = depth_tex.Sample(smp, in_.uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    o.color = c;
    o.depth = d;
    return o;
}
)HLSL";
#endif

// Unit quad in [0,1]^2 with UVs. Shared by composite, tile, AO, and light
// pipelines. UV.y is flipped so row 0 of a source image lands at the top.
const SCompositeVertex kCompositeQuad[6] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 0.0f },
};


}  // namespace

// *************************************************************************
// * TRenderer - Lifecycle                                                 *
// *************************************************************************

TRenderer::TRenderer() = default;

TRenderer::~TRenderer()
{
    Shutdown();
}

bool TRenderer::Initialize(int32_t dwidth, int32_t dheight)
{
    if (width > 0)
        return true;

    width  = dwidth;
    height = dheight;

    // The G-buffer is allocated larger than the display (kGBufPad on every
    // side) so off-screen geometry can still contribute to effects that
    // sample neighbors -- sun-shadow ray march in particular.
    const int32_t pad = kGBufPad;
    const int32_t gbw = width  + 2 * pad;
    const int32_t gbh = height + 2 * pad;
    shadow_width = (std::max)(1, (gbw + 1) / 2);
    shadow_height = (std::max)(1, (gbh + 1) / 2);

    sg_image_desc rt_desc = {};
    rt_desc.width         = gbw;
    rt_desc.height        = gbh;
    rt_desc.sample_count  = 1;
    rt_desc.render_target = true;
    rt_desc.min_filter    = SG_FILTER_LINEAR;
    rt_desc.mag_filter    = SG_FILTER_LINEAR;

    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    color_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    depth_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    normal_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    scene_z_target = sg_make_image(&rt_desc);

    // id_target -- per-pixel object instance id, packed into RGBA8.
    // Used by the editor for pixel-perfect picking + selection outline.
    // Pixels covered by no drawable read back as (0,0,0,0) which we
    // reserve for "no object".
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    rt_desc.min_filter   = SG_FILTER_NEAREST;
    rt_desc.mag_filter   = SG_FILTER_NEAREST;
    id_target = sg_make_image(&rt_desc);
    rt_desc.min_filter   = SG_FILTER_LINEAR;
    rt_desc.mag_filter   = SG_FILTER_LINEAR;

    // lit_target -- final shaded RGBA8. Nearest filter because we composite
    // 1:1 onto the swapchain and don't want resample blur.
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    rt_desc.min_filter   = SG_FILTER_NEAREST;
    rt_desc.mag_filter   = SG_FILTER_NEAREST;
    lit_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    rt_desc.min_filter   = SG_FILTER_LINEAR;
    rt_desc.mag_filter   = SG_FILTER_LINEAR;
    ao_target = sg_make_image(&rt_desc);

    // Low-resolution shadow visibility. It covers the same normalized
    // padded G-buffer rect as the full-size targets; lighting samples it
    // linearly back at full resolution.
    rt_desc.width         = shadow_width;
    rt_desc.height        = shadow_height;
    rt_desc.pixel_format  = SG_PIXELFORMAT_R32F;
    rt_desc.min_filter    = SG_FILTER_LINEAR;
    rt_desc.mag_filter    = SG_FILTER_LINEAR;
    shadow_target = sg_make_image(&rt_desc);
    shadow_blur_target = sg_make_image(&rt_desc);
    rt_desc.width         = gbw;
    rt_desc.height        = gbh;

    // Pass [1] -- G-buffer MRT fill.
    sg_pass_desc gbuf_desc = {};
    gbuf_desc.color_attachments[0].image     = color_target;
    gbuf_desc.color_attachments[1].image     = normal_target;
    gbuf_desc.color_attachments[2].image     = scene_z_target;
    gbuf_desc.color_attachments[3].image     = id_target;
    gbuf_desc.depth_stencil_attachment.image = depth_target;
    default_pass = sg_make_pass(&gbuf_desc);

    // Pass [4] -- deferred lighting.
    sg_pass_desc lit_desc = {};
    lit_desc.color_attachments[0].image = lit_target;
    lit_pass = sg_make_pass(&lit_desc);

    sg_pass_desc helper_desc = {};
    helper_desc.color_attachments[0].image = lit_target;
    helper_desc.depth_stencil_attachment.image = depth_target;
    helper_pass = sg_make_pass(&helper_desc);

    // Pass [5] -- transparent FX (billboards, particles, ribbons). Reads
    // scene depth (no write), blends over the lit color. See
    // docs/vfx/PHASE1_SPINE.md §5.
    sg_pass_desc fx_desc = {};
    fx_desc.color_attachments[0].image = lit_target;
    fx_desc.depth_stencil_attachment.image = depth_target;
    fx_pass = sg_make_pass(&fx_desc);

    // Pass [2] -- screen-space AO.
    sg_pass_desc ao_desc = {};
    ao_desc.color_attachments[0].image = ao_target;
    ao_pass = sg_make_pass(&ao_desc);

    // Pass [3] -- low-resolution sun-shadow mask + blur ping-pong.
    sg_pass_desc shadow_desc = {};
    shadow_desc.color_attachments[0].image = shadow_target;
    shadow_pass = sg_make_pass(&shadow_desc);
    sg_pass_desc shadow_blur_desc = {};
    shadow_blur_desc.color_attachments[0].image = shadow_blur_target;
    shadow_blur_pass = sg_make_pass(&shadow_blur_desc);

    // Reserved for a depth-only prepass in Phase 3.
    depth_pass = {};

    InitCompositePipeline();
    InitTilePipeline();
    InitMeshPipeline();
    InitAOPipeline();
    InitShadowPipeline();
    InitLightPipeline();
    InitFxPipeline();

    return true;
}

void TRenderer::Shutdown()
{
    ShutdownFxPipeline();
    ShutdownLightPipeline();
    ShutdownShadowPipeline();
    ShutdownAOPipeline();
    ShutdownMeshPipeline();
    ShutdownTilePipeline();
    ShutdownCompositePipeline();

    for (auto& asset : image_pair_assets) {
        if (asset.color.id) sg_destroy_image(asset.color);
        if (asset.depth.id) sg_destroy_image(asset.depth);
        if (asset.height_minmax.id) sg_destroy_image(asset.height_minmax);
    }
    image_pair_assets.clear();
    image_pair_by_key.clear();
    for (auto& asset : texture_assets) {
        if (asset.image.id) sg_destroy_image(asset.image);
    }
    texture_assets.clear();
    texture_by_key.clear();
    white_texture = kInvalidTexture;

    if (default_pass.id) { sg_destroy_pass(default_pass); default_pass = {}; }
    if (lit_pass.id)     { sg_destroy_pass(lit_pass);     lit_pass     = {}; }
    if (helper_pass.id)  { sg_destroy_pass(helper_pass);  helper_pass  = {}; }
    if (fx_pass.id)      { sg_destroy_pass(fx_pass);      fx_pass      = {}; }
    if (ao_pass.id)      { sg_destroy_pass(ao_pass);      ao_pass      = {}; }
    if (shadow_pass.id)  { sg_destroy_pass(shadow_pass);  shadow_pass  = {}; }
    if (shadow_blur_pass.id) { sg_destroy_pass(shadow_blur_pass); shadow_blur_pass = {}; }

    if (color_target.id)   { sg_destroy_image(color_target);   color_target   = {}; }
    if (depth_target.id)   { sg_destroy_image(depth_target);   depth_target   = {}; }
    if (normal_target.id)  { sg_destroy_image(normal_target);  normal_target  = {}; }
    if (scene_z_target.id) { sg_destroy_image(scene_z_target); scene_z_target = {}; }
    if (id_target.id)      { sg_destroy_image(id_target);      id_target      = {}; }
    if (ao_target.id)      { sg_destroy_image(ao_target);      ao_target      = {}; }
    if (shadow_target.id)  { sg_destroy_image(shadow_target);  shadow_target  = {}; }
    if (shadow_blur_target.id) { sg_destroy_image(shadow_blur_target); shadow_blur_target = {}; }
    if (lit_target.id)     { sg_destroy_image(lit_target);     lit_target     = {}; }

    width = height = 0;
    shadow_width = shadow_height = 0;
    color_target_dirty = false;
    lit_target_dirty   = false;
}

// *************************************************************************
// * Composite Pipeline  (textured quad, UI + swapchain present)           *
// *************************************************************************

void TRenderer::InitCompositePipeline()
{
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "renderer.composite.vbuf";
    composite_vbuf = sg_make_buffer(&vb);

    sg_shader_desc sh = {};
    sh.attrs[0].name             = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name             = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source                 = kCompositeVs;
    sh.vs.entry                  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = sizeof(float) * 12;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source                 = kCompositeFs;
    sh.fs.entry                  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = sizeof(float) * 12;
    sh.fs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "chroma_key";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.composite.shader";
    composite_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = composite_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    // Variant 1: drawing into an RGBA8 render target (e.g. the backbuffer).
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.depth.pixel_format     = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.composite.pipeline.rt";
    composite_pip_rt = sg_make_pipeline(&pip);

    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.label = "renderer.composite.pipeline.rt.add";
    composite_pip_add_rt = sg_make_pipeline(&pip);

    // Variant 2: drawing into the swapchain (sokol default pass).
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].pixel_format = _SG_PIXELFORMAT_DEFAULT;
    pip.depth.pixel_format     = _SG_PIXELFORMAT_DEFAULT;
    pip.label = "renderer.composite.pipeline.swap";
    composite_pip_swap = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownCompositePipeline()
{
    if (composite_pip_add_rt.id) { sg_destroy_pipeline(composite_pip_add_rt); composite_pip_add_rt = {}; }
    if (composite_pip_rt.id)   { sg_destroy_pipeline(composite_pip_rt);   composite_pip_rt   = {}; }
    if (composite_pip_swap.id) { sg_destroy_pipeline(composite_pip_swap); composite_pip_swap = {}; }
    if (composite_shader.id)   { sg_destroy_shader(composite_shader);     composite_shader   = {}; }
    if (composite_vbuf.id)     { sg_destroy_buffer(composite_vbuf);       composite_vbuf     = {}; }
}

// *************************************************************************
// * Tile (G-buffer Fill) Pipeline  --  Pass [1]                           *
// *************************************************************************

void TRenderer::InitTilePipeline()
{
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "renderer.tile.vbuf";
    tile_vbuf = sg_make_buffer(&vb);

    sg_buffer_desc proxy_vb = {};
    proxy_vb.size = kMaxTileProxyVertices * int32_t(sizeof(SCompositeVertex));
    proxy_vb.usage = SG_USAGE_STREAM;
    proxy_vb.label = "renderer.tile.proxy_vbuf";
    tile_proxy_vbuf = sg_make_buffer(&proxy_vb);

    // 12 vec4s: rect, zparams, tile_root, tile_sprite, filter, obj_id,
    // camera, proj, tile_rect, cam_tile, debug, raycast.
    constexpr int32_t kUB_vec4s = 12;
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kTileVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = kUB_bytes;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "zparams";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[2].name = "tile_root";
    sh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[3].name = "tile_sprite";
    sh.vs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[4].name = "filter";
    sh.vs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[5].name = "obj_id";
    sh.vs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[6].name = "camera";
    sh.vs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[7].name = "proj";
    sh.vs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[8].name = "tile_rect";
    sh.vs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[9].name = "cam_tile";
    sh.vs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[10].name = "debug";
    sh.vs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[11].name = "raycast";
    sh.vs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source = kTileFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = kUB_bytes;
    sh.fs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "zparams";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "tile_root";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "tile_sprite";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "filter";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "obj_id";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].name = "camera";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[7].name = "proj";
    sh.fs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[8].name = "tile_rect";
    sh.fs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[9].name = "cam_tile";
    sh.fs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[10].name = "debug";
    sh.fs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[11].name = "raycast";
    sh.fs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "color_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "depth_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[2].name         = "height_minmax_tex";
    sh.fs.images[2].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[2].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.tile.shader";
    tile_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = tile_shader;
    pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type         = SG_PRIMITIVETYPE_TRIANGLES;
    // MRT: albedo (RGBA8) + world normal (RGBA16F) + scene_z (R32F) +
    // obj_id (RGBA8 packed instance id). Opaque tiles fill the padded
    // border for depth/id so offscreen occluders still cast screen-space
    // shadows; the tile shader masks albedo outside the display rect.
    pip.color_count            = 4;
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[1].pixel_format = SG_PIXELFORMAT_RGBA16F;
    pip.colors[1].blend.enabled = false;
    pip.colors[2].pixel_format = SG_PIXELFORMAT_R32F;
    pip.colors[2].blend.enabled = false;
    pip.colors[3].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[3].blend.enabled = false;
    pip.depth.pixel_format     = SG_PIXELFORMAT_DEPTH;
    pip.depth.compare          = SG_COMPAREFUNC_LESS_EQUAL;
    pip.depth.write_enabled    = true;
    pip.label = "renderer.tile.pipeline";
    tile_pipeline = sg_make_pipeline(&pip);

    sg_shader_desc tsh = sh;
    tsh.fs.source = kTransparentTileFs;
    tsh.fs.entry  = kShaderFsEntry;
    tsh.label = "renderer.transparent_tile.shader";
    transparent_tile_shader = sg_make_shader(&tsh);

    sg_pipeline_desc tpip = {};
    tpip.shader = transparent_tile_shader;
    tpip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
    tpip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    tpip.primitive_type         = SG_PRIMITIVETYPE_TRIANGLES;
    tpip.color_count            = 1;
    tpip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    tpip.colors[0].blend.enabled = true;
    tpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    tpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    tpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    tpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    tpip.depth.pixel_format     = SG_PIXELFORMAT_DEPTH;
    tpip.depth.compare          = SG_COMPAREFUNC_LESS_EQUAL;
    tpip.depth.write_enabled    = false;
    tpip.label = "renderer.transparent_tile.pipeline";
    transparent_tile_pipeline = sg_make_pipeline(&tpip);
}

void TRenderer::ShutdownTilePipeline()
{
    if (transparent_tile_pipeline.id) { sg_destroy_pipeline(transparent_tile_pipeline); transparent_tile_pipeline = {}; }
    if (transparent_tile_shader.id)   { sg_destroy_shader(transparent_tile_shader);     transparent_tile_shader   = {}; }
    if (tile_pipeline.id) { sg_destroy_pipeline(tile_pipeline); tile_pipeline = {}; }
    if (tile_shader.id)   { sg_destroy_shader(tile_shader);     tile_shader   = {}; }
    if (tile_proxy_vbuf.id) { sg_destroy_buffer(tile_proxy_vbuf); tile_proxy_vbuf = {}; }
    if (tile_vbuf.id)     { sg_destroy_buffer(tile_vbuf);       tile_vbuf     = {}; }
}

// *************************************************************************
// * Mesh Pipeline  --  3D rigid mesh into G-buffer                        *
// *************************************************************************
//
// Writes the same MRT targets as the tile pipeline (albedo / world normal /
// scene_z + HW depth) from real 3D vertices. Per-instance data (world matrix
// + tint) comes through a dynamic instance vertex buffer filled every
// DrainMeshQueue; sorting by MeshHandle groups instances of the same asset
// into single instanced draws.
//
// *************************************************************************

void TRenderer::InitMeshPipeline()
{
    // Shader: 8 attribute slots. Slot 0..2 per-vertex; slot 3..7 per-instance
    // (4 rows of the world matrix + tint). Single uniform block of 3 vec4s
    // (vp / camz / camw) shared VS-side.
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";       sh.attrs[0].sem_name = "POSITION"; sh.attrs[0].sem_index = 0;
    sh.attrs[1].name = "normal";    sh.attrs[1].sem_name = "NORMAL";   sh.attrs[1].sem_index = 0;
    sh.attrs[2].name = "uv";        sh.attrs[2].sem_name = "TEXCOORD"; sh.attrs[2].sem_index = 0;
    sh.attrs[3].name = "w0";        sh.attrs[3].sem_name = "TEXCOORD"; sh.attrs[3].sem_index = 1;
    sh.attrs[4].name = "w1";        sh.attrs[4].sem_name = "TEXCOORD"; sh.attrs[4].sem_index = 2;
    sh.attrs[5].name = "w2";        sh.attrs[5].sem_name = "TEXCOORD"; sh.attrs[5].sem_index = 3;
    sh.attrs[6].name = "w3";        sh.attrs[6].sem_name = "TEXCOORD"; sh.attrs[6].sem_index = 4;
    sh.attrs[7].name = "tint";        sh.attrs[7].sem_name = "TEXCOORD"; sh.attrs[7].sem_index = 5;
    sh.attrs[8].name = "inst_obj_id"; sh.attrs[8].sem_name = "TEXCOORD"; sh.attrs[8].sem_index = 6;

    sh.vs.source = kMeshVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = 3 * sizeof(float) * 4;
    sh.vs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "camz";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[2].name = "camw";
    sh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;

    sh.fs.source = kMeshFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.mesh.shader";
    mesh_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = mesh_shader;
    // Vertex buffer 0: per-vertex (pos/normal/uv).
    pip.layout.buffers[0].stride    = sizeof(SMeshVertex);
    pip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
    // Vertex buffer 1: per-instance (world rows + tint + obj_id rgba8).
    pip.layout.buffers[1].stride    = sizeof(float) * 24;   // 4*vec4 world + vec4 tint + vec4 obj_id
    pip.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
    pip.layout.attrs[0].buffer_index = 0;
    pip.layout.attrs[0].offset       = offsetof(SMeshVertex, pos);
    pip.layout.attrs[0].format       = SG_VERTEXFORMAT_FLOAT3;
    pip.layout.attrs[1].buffer_index = 0;
    pip.layout.attrs[1].offset       = offsetof(SMeshVertex, normal);
    pip.layout.attrs[1].format       = SG_VERTEXFORMAT_FLOAT3;
    pip.layout.attrs[2].buffer_index = 0;
    pip.layout.attrs[2].offset       = offsetof(SMeshVertex, uv);
    pip.layout.attrs[2].format       = SG_VERTEXFORMAT_FLOAT2;
    for (int32_t i = 0; i < 6; ++i) {
        pip.layout.attrs[3 + i].buffer_index = 1;
        pip.layout.attrs[3 + i].offset       = i * int32_t(sizeof(float)) * 4;
        pip.layout.attrs[3 + i].format       = SG_VERTEXFORMAT_FLOAT4;
    }
    pip.index_type     = SG_INDEXTYPE_UINT16;
    pip.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    pip.cull_mode      = SG_CULLMODE_NONE;
    pip.color_count            = 4;
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[1].pixel_format = SG_PIXELFORMAT_RGBA16F;
    pip.colors[1].blend.enabled = false;
    pip.colors[2].pixel_format = SG_PIXELFORMAT_R32F;
    pip.colors[2].blend.enabled = false;
    pip.colors[3].pixel_format = SG_PIXELFORMAT_RGBA8;   // obj_id
    pip.colors[3].blend.enabled = false;
    pip.depth.pixel_format  = SG_PIXELFORMAT_DEPTH;
    pip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
    pip.depth.write_enabled = true;
    pip.label = "renderer.mesh.pipeline";
    mesh_pipeline = sg_make_pipeline(&pip);

    sg_shader_desc hsh = {};
    hsh.attrs[0].name = "pos";       hsh.attrs[0].sem_name = "POSITION"; hsh.attrs[0].sem_index = 0;
    hsh.attrs[1].name = "normal";    hsh.attrs[1].sem_name = "NORMAL";   hsh.attrs[1].sem_index = 0;
    hsh.attrs[2].name = "uv";        hsh.attrs[2].sem_name = "TEXCOORD"; hsh.attrs[2].sem_index = 0;
    hsh.vs.source = kHelperMeshVs;
    hsh.vs.entry  = kShaderVsEntry;
    hsh.vs.uniform_blocks[0].size = 7 * sizeof(float) * 4;
    hsh.vs.uniform_blocks[0].uniforms[0].name = "w0";
    hsh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[1].name = "w1";
    hsh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[2].name = "w2";
    hsh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[3].name = "w3";
    hsh.vs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[4].name = "vp";
    hsh.vs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[5].name = "camz";
    hsh.vs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[6].name = "camw";
    hsh.vs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.source = kHelperMeshFs;
    hsh.fs.entry  = kShaderFsEntry;
    hsh.fs.uniform_blocks[0].size = 7 * sizeof(float) * 4;
    hsh.fs.uniform_blocks[0].uniforms[0].name = "diffuse";
    hsh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[1].name = "ambient";
    hsh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[2].name = "specular";
    hsh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[3].name = "emissive";
    hsh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[4].name = "light_dir_i";
    hsh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[5].name = "light_col_a";
    hsh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[6].name = "view_dir_power";
    hsh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.images[0].name         = "albedo_tex";
    hsh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    hsh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    hsh.label = "renderer.helper.shader";
    helper_mesh_shader = sg_make_shader(&hsh);

    sg_pipeline_desc hpip = {};
    hpip.shader = helper_mesh_shader;
    hpip.layout.buffers[0].stride    = sizeof(SMeshVertex);
    hpip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
    hpip.layout.attrs[0].buffer_index = 0;
    hpip.layout.attrs[0].offset       = offsetof(SMeshVertex, pos);
    hpip.layout.attrs[0].format       = SG_VERTEXFORMAT_FLOAT3;
    hpip.layout.attrs[1].buffer_index = 0;
    hpip.layout.attrs[1].offset       = offsetof(SMeshVertex, normal);
    hpip.layout.attrs[1].format       = SG_VERTEXFORMAT_FLOAT3;
    hpip.layout.attrs[2].buffer_index = 0;
    hpip.layout.attrs[2].offset       = offsetof(SMeshVertex, uv);
    hpip.layout.attrs[2].format       = SG_VERTEXFORMAT_FLOAT2;
    hpip.index_type     = SG_INDEXTYPE_UINT16;
    hpip.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    hpip.cull_mode      = SG_CULLMODE_FRONT;
    hpip.color_count = 1;
    hpip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    hpip.colors[0].blend.enabled = true;
    hpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    hpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    hpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    hpip.depth.pixel_format  = SG_PIXELFORMAT_DEPTH;
    hpip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
    hpip.depth.write_enabled = false;
    hpip.label = "renderer.helper.back.pipeline";
    helper_mesh_back_pipeline = sg_make_pipeline(&hpip);

    hpip.cull_mode = SG_CULLMODE_BACK;
    hpip.label = "renderer.helper.front.pipeline";
    helper_mesh_front_pipeline = sg_make_pipeline(&hpip);

    hpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.cull_mode = SG_CULLMODE_FRONT;
    hpip.label = "renderer.helper.add.back.pipeline";
    helper_mesh_add_back_pipeline = sg_make_pipeline(&hpip);

    hpip.cull_mode = SG_CULLMODE_BACK;
    hpip.label = "renderer.helper.add.front.pipeline";
    helper_mesh_add_front_pipeline = sg_make_pipeline(&hpip);

    sg_buffer_desc ivb = {};
    ivb.size  = kMaxMeshInstances * int32_t(sizeof(float)) * 24;   // 4*vec4 world + tint + obj_id
    ivb.usage = SG_USAGE_STREAM;
    ivb.label = "renderer.mesh.instance_vbuf";
    mesh_instance_vb = sg_make_buffer(&ivb);
}

void TRenderer::ShutdownMeshPipeline()
{
    for (auto& m : meshes) {
        if (m.vbuf.id) sg_destroy_buffer(m.vbuf);
        if (m.ibuf.id) sg_destroy_buffer(m.ibuf);
    }
    meshes.clear();
    mesh_by_key.clear();
    if (mesh_instance_vb.id) { sg_destroy_buffer(mesh_instance_vb); mesh_instance_vb = {}; }
    if (helper_mesh_back_pipeline.id) { sg_destroy_pipeline(helper_mesh_back_pipeline); helper_mesh_back_pipeline = {}; }
    if (helper_mesh_front_pipeline.id) { sg_destroy_pipeline(helper_mesh_front_pipeline); helper_mesh_front_pipeline = {}; }
    if (helper_mesh_add_back_pipeline.id) { sg_destroy_pipeline(helper_mesh_add_back_pipeline); helper_mesh_add_back_pipeline = {}; }
    if (helper_mesh_add_front_pipeline.id) { sg_destroy_pipeline(helper_mesh_add_front_pipeline); helper_mesh_add_front_pipeline = {}; }
    if (helper_mesh_shader.id)   { sg_destroy_shader(helper_mesh_shader); helper_mesh_shader = {}; }
    if (mesh_pipeline.id)    { sg_destroy_pipeline(mesh_pipeline);  mesh_pipeline    = {}; }
    if (mesh_shader.id)      { sg_destroy_shader(mesh_shader);      mesh_shader      = {}; }
}

RendererImagePairHandle TRenderer::RegisterImagePairAsset(uint64_t key,
                                                          const void* color_rgba8,
                                                          size_t color_bytes,
                                                          const void* depth_r32f,
                                                          size_t depth_bytes,
                                                          int32_t width,
                                                          int32_t height,
                                                          uint64_t gpu_bytes)
{
    if (!color_rgba8 || !depth_r32f || width <= 0 || height <= 0)
        return 0;

    if (key != 0)
    {
        auto it = image_pair_by_key.find(key);
        if (it != image_pair_by_key.end())
            return it->second;
    }

    sg_image_desc color_desc = {};
    color_desc.width = width;
    color_desc.height = height;
    color_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    color_desc.min_filter = SG_FILTER_LINEAR;
    color_desc.mag_filter = SG_FILTER_LINEAR;
    color_desc.wrap_u = color_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    color_desc.data.subimage[0][0] = { color_rgba8, color_bytes };
    color_desc.label = "renderer.imagepair.color";

    sg_image_desc depth_desc = {};
    depth_desc.width = width;
    depth_desc.height = height;
    depth_desc.pixel_format = SG_PIXELFORMAT_R32F;
    // Depth images are numeric surface data, not color. Filtering them blends
    // valid tile depth with invalid/silhouette texels and creates false seams
    // in scene-z, reconstructed normals, and screen-space shadows.
    depth_desc.min_filter = SG_FILTER_NEAREST;
    depth_desc.mag_filter = SG_FILTER_NEAREST;
    depth_desc.wrap_u = depth_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    depth_desc.data.subimage[0][0] = { depth_r32f, depth_bytes };
    depth_desc.label = "renderer.imagepair.depth";

    const SHeightMinMaxMipChain height_minmax =
        BuildHeightMinMaxMipChain(static_cast<const float*>(depth_r32f), width, height);
    sg_image_desc height_desc = {};
    height_desc.width = width;
    height_desc.height = height;
    height_desc.num_mipmaps = int(height_minmax.levels.size());
    height_desc.pixel_format = SG_PIXELFORMAT_RG32F;
    height_desc.min_filter = SG_FILTER_NEAREST;
    height_desc.mag_filter = SG_FILTER_NEAREST;
    height_desc.wrap_u = height_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    for (int32_t mip = 0; mip < height_desc.num_mipmaps; ++mip)
    {
        const std::vector<float>& pixels = height_minmax.levels[size_t(mip)];
        height_desc.data.subimage[0][mip] = {
            pixels.data(),
            pixels.size() * sizeof(float)
        };
    }
    height_desc.label = "renderer.imagepair.height_minmax";

    SRendererImagePairEntry asset = {};
    asset.key = key;
    asset.color = sg_make_image(&color_desc);
    asset.depth = sg_make_image(&depth_desc);
    asset.height_minmax = sg_make_image(&height_desc);
    asset.width = width;
    asset.height = height;
    asset.height_minmax_mips = height_desc.num_mipmaps;
    asset.tight_proxy_hull_px =
        BuildTileTightProxyHull(static_cast<const float*>(depth_r32f), width, height);
    asset.tight_proxy_points = uint32_t(asset.tight_proxy_hull_px.size() / 2u);
    asset.gpu_bytes = gpu_bytes + height_minmax.bytes;
    if (sg_query_image_state(asset.color) != SG_RESOURCESTATE_VALID ||
        sg_query_image_state(asset.depth) != SG_RESOURCESTATE_VALID ||
        sg_query_image_state(asset.height_minmax) != SG_RESOURCESTATE_VALID)
    {
        if (asset.color.id) sg_destroy_image(asset.color);
        if (asset.depth.id) sg_destroy_image(asset.depth);
        if (asset.height_minmax.id) sg_destroy_image(asset.height_minmax);
        return 0;
    }

    image_pair_assets.push_back(asset);
    const RendererImagePairHandle handle = RendererImagePairHandle(image_pair_assets.size());
    if (key != 0)
        image_pair_by_key[key] = handle;
    return handle;
}

const SRendererImagePairInfo* TRenderer::ImagePairInfo(RendererImagePairHandle handle) const
{
    if (handle == 0 || handle > image_pair_assets.size())
        return nullptr;
    return &image_pair_assets[handle - 1];
}

void TRenderer::AddImagePairAssetRef(RendererImagePairHandle handle, uint32_t count)
{
    if (handle == 0 || handle > image_pair_assets.size())
        return;
    image_pair_assets[handle - 1].ref_count += count;
}

void TRenderer::ReleaseImagePairAssetRef(RendererImagePairHandle handle, uint32_t count)
{
    if (handle == 0 || handle > image_pair_assets.size())
        return;
    uint32_t& refs = image_pair_assets[handle - 1].ref_count;
    refs = (count >= refs) ? 0 : (refs - count);
}

TTextureHandle TRenderer::RegisterTextureAsset(uint64_t key,
                                               const void* pixels,
                                               size_t pixel_bytes,
                                               int32_t width,
                                               int32_t height,
                                               ERendererTextureFormat format,
                                               uint64_t gpu_bytes,
                                               ERendererTextureFilter filter)
{
    if (!pixels || width <= 0 || height <= 0)
        return kInvalidTexture;

    if (key != 0)
    {
        auto it = texture_by_key.find(key);
        if (it != texture_by_key.end())
            return it->second;
    }

    sg_image_desc desc = {};
    desc.width = width;
    desc.height = height;
    desc.pixel_format = format == ERendererTextureFormat::R32F ? SG_PIXELFORMAT_R32F : SG_PIXELFORMAT_RGBA8;
    desc.min_filter = filter == ERendererTextureFilter::Nearest ? SG_FILTER_NEAREST : SG_FILTER_LINEAR;
    desc.mag_filter = filter == ERendererTextureFilter::Nearest ? SG_FILTER_NEAREST : SG_FILTER_LINEAR;
    desc.wrap_u = desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    desc.data.subimage[0][0] = { pixels, pixel_bytes };
    desc.label = "renderer.texture";

    SRendererTextureEntry asset = {};
    asset.key = key;
    asset.image = sg_make_image(&desc);
    asset.width = width;
    asset.height = height;
    asset.gpu_bytes = gpu_bytes;
    if (sg_query_image_state(asset.image) != SG_RESOURCESTATE_VALID)
    {
        if (asset.image.id) sg_destroy_image(asset.image);
        return kInvalidTexture;
    }

    texture_assets.push_back(asset);
    const TTextureHandle handle = TTextureHandle(texture_assets.size());
    if (key != 0)
        texture_by_key[key] = handle;
    return handle;
}

const SRendererTextureInfo* TRenderer::TextureInfo(TTextureHandle handle) const
{
    if (handle == 0 || handle > texture_assets.size())
        return nullptr;
    return &texture_assets[handle - 1];
}

sg_image TRenderer::TextureImage(TTextureHandle handle) const
{
    const SRendererTextureInfo* info = TextureInfo(handle);
    if (!info)
        return sg_image{};
    return texture_assets[handle - 1].image;
}

sg_image TRenderer::ImagePairColor(RendererImagePairHandle handle) const
{
    if (handle == 0 || handle > image_pair_assets.size())
        return sg_image{};
    return image_pair_assets[handle - 1].color;
}

sg_image TRenderer::ImagePairDepth(RendererImagePairHandle handle) const
{
    if (handle == 0 || handle > image_pair_assets.size())
        return sg_image{};
    return image_pair_assets[handle - 1].depth;
}

sg_image TRenderer::ImagePairHeightMinMax(RendererImagePairHandle handle) const
{
    if (handle == 0 || handle > image_pair_assets.size())
        return sg_image{};
    return image_pair_assets[handle - 1].height_minmax;
}

uintptr_t TRenderer::TextureImGuiId(TTextureHandle texture) const
{
    const sg_image img = TextureImage(texture);
    return uintptr_t(img.id);
}

uintptr_t TRenderer::LitTargetTextureId() const
{
    return uintptr_t(lit_target.id);
}

bool TRenderer::ReadIdTargetPixel(int32_t x, int32_t y, uint8_t out_rgba[4]) const
{
    if (!id_target.id || !out_rgba)
        return false;
    return RendererReadback::ReadPixel(id_target, x, y, out_rgba);
}

void TRenderer::AddTextureAssetRef(TTextureHandle handle, uint32_t count)
{
    if (handle == 0 || handle > texture_assets.size())
        return;
    texture_assets[handle - 1].ref_count += count;
}

void TRenderer::ReleaseTextureAssetRef(TTextureHandle handle, uint32_t count)
{
    if (handle == 0 || handle > texture_assets.size())
        return;
    uint32_t& refs = texture_assets[handle - 1].ref_count;
    refs = (count >= refs) ? 0 : (refs - count);
}

void TRenderer::ResetAssetRefCounts()
{
    for (auto& asset : image_pair_assets)
        asset.ref_count = 0;
    for (auto& asset : texture_assets)
        asset.ref_count = 0;
    for (auto& mesh : meshes)
        mesh.ref_count = 0;
}

MeshHandle TRenderer::RegisterMeshAsset(uint64_t key,
                                        const SMeshVertex* verts, int32_t num_verts,
                                        const uint16_t*  indices, int32_t num_indices,
                                        TTextureHandle albedo_texture)
{
    if (!verts || !indices || num_verts <= 0 || num_indices <= 0) return 0;

    if (key != 0)
    {
        // Mesh handles are stable for the renderer lifetime. Reuse a matching
        // source key instead of re-uploading immutable vertex/index buffers.
        auto it = mesh_by_key.find(key);
        if (it != mesh_by_key.end())
            return it->second;
    }

    SMeshEntry e = {};
    e.key = key;
    e.vertex_bytes = uint64_t(num_verts) * uint64_t(sizeof(SMeshVertex));
    e.index_bytes  = uint64_t(num_indices) * uint64_t(sizeof(uint16_t));
    sg_buffer_desc vbd = {};
    vbd.size  = int(e.vertex_bytes);
    vbd.data  = { verts, size_t(vbd.size) };
    vbd.label = "renderer.mesh.asset.vbuf";
    e.vbuf = sg_make_buffer(&vbd);

    sg_buffer_desc ibd = {};
    ibd.type  = SG_BUFFERTYPE_INDEXBUFFER;
    ibd.size  = int(e.index_bytes);
    ibd.data  = { indices, size_t(ibd.size) };
    ibd.label = "renderer.mesh.asset.ibuf";
    e.ibuf = sg_make_buffer(&ibd);

    if (sg_query_buffer_state(e.vbuf) != SG_RESOURCESTATE_VALID ||
        sg_query_buffer_state(e.ibuf) != SG_RESOURCESTATE_VALID)
    {
        if (e.vbuf.id) sg_destroy_buffer(e.vbuf);
        if (e.ibuf.id) sg_destroy_buffer(e.ibuf);
        static int32_t logged_failures = 0;
        if (logged_failures < 16)
        {
            ++logged_failures;
            log_error("[renderer-assets] mesh upload failed key=%llu verts=%d indices=%d buffers=%zu",
                      (unsigned long long)key, num_verts, num_indices, meshes.size());
        }
        return 0;
    }

    e.num_indices = num_indices;
    e.albedo      = albedo_texture;

    meshes.push_back(e);
    const MeshHandle handle = MeshHandle(meshes.size());   // index+1
    if (key != 0)
        mesh_by_key[key] = handle;
    return handle;
}

void TRenderer::AddMeshAssetRef(MeshHandle mesh, uint32_t count)
{
    if (mesh == 0 || mesh > meshes.size())
        return;
    meshes[mesh - 1].ref_count += count;
}

void TRenderer::ReleaseMeshAssetRef(MeshHandle mesh, uint32_t count)
{
    if (mesh == 0 || mesh > meshes.size())
        return;
    uint32_t& refs = meshes[mesh - 1].ref_count;
    refs = (count >= refs) ? 0 : (refs - count);
}

void TRenderer::SetMeshAlbedo(MeshHandle mesh, TTextureHandle albedo_texture)
{
    if (mesh == 0 || mesh > meshes.size() || albedo_texture == kInvalidTexture)
        return;
    meshes[mesh - 1].albedo = albedo_texture;
}

TTextureHandle TRenderer::WhiteTextureHandle()
{
    if (white_texture == kInvalidTexture)
        white_texture = SolidColorTexture(0xFFFFFFFFFFFFFFF0ull, 0xFFFFFFFFu, "renderer.asset.white");
    return white_texture;
}

TTextureHandle TRenderer::SolidColorTexture(uint64_t key, uint32_t rgba, const char* /*debug_name*/)
{
    return RegisterTextureAsset(key, &rgba, sizeof(rgba), 1, 1,
                                ERendererTextureFormat::RGBA8,
                                sizeof(rgba));
}

SRendererAssetStats TRenderer::GetAssetStats() const
{
    SRendererAssetStats out = {};
    out.image_pair_count = uint32_t(image_pair_assets.size());
    out.texture_count = uint32_t(texture_assets.size());
    out.mesh_count = uint32_t(meshes.size());

    for (const auto& asset : image_pair_assets)
    {
        out.image_pair_ref_total += asset.ref_count;
        out.image_pair_gpu_bytes += asset.gpu_bytes;
        if (asset.ref_count == 0)
            ++out.image_pair_zero_ref_count;
    }
    for (const auto& asset : texture_assets)
    {
        out.texture_ref_total += asset.ref_count;
        out.texture_gpu_bytes += asset.gpu_bytes;
        if (asset.ref_count == 0)
            ++out.texture_zero_ref_count;
    }
    for (const auto& mesh : meshes)
    {
        out.mesh_ref_total += mesh.ref_count;
        out.mesh_vertex_bytes += mesh.vertex_bytes;
        out.mesh_index_bytes += mesh.index_bytes;
        if (mesh.ref_count == 0)
            ++out.mesh_zero_ref_count;
        if (mesh.key != 0)
            ++out.keyed_mesh_count;
        else
            ++out.unkeyed_mesh_count;
    }
    out.mesh_gpu_bytes = out.mesh_vertex_bytes + out.mesh_index_bytes;

    out.renderer_buffer_count = uint32_t(meshes.size() * 2);
    if (composite_vbuf.id) ++out.renderer_buffer_count;
    if (tile_vbuf.id) ++out.renderer_buffer_count;
    if (tile_proxy_vbuf.id) ++out.renderer_buffer_count;
    if (mesh_instance_vb.id) ++out.renderer_buffer_count;

    out.renderer_image_count = uint32_t(image_pair_assets.size() * 3 + texture_assets.size());
    if (color_target.id) ++out.renderer_image_count;
    if (depth_target.id) ++out.renderer_image_count;
    if (normal_target.id) ++out.renderer_image_count;
    if (scene_z_target.id) ++out.renderer_image_count;
    if (id_target.id) ++out.renderer_image_count;
    if (ao_target.id) ++out.renderer_image_count;
    if (shadow_target.id) ++out.renderer_image_count;
    if (shadow_blur_target.id) ++out.renderer_image_count;
    if (lit_target.id) ++out.renderer_image_count;

    if (sg_isvalid())
    {
        const sg_desc desc = sg_query_desc();
        out.buffer_pool_size = desc.buffer_pool_size;
        out.image_pool_size = desc.image_pool_size;
    }
    return out;
}


void TRenderer::SubmitMesh(const SMeshSubmit& m)
{
    if (m.mesh == 0 || m.mesh > meshes.size()) return;
    mesh_queue.push_back(m);
}

void TRenderer::SubmitTransparentTile(const STileSubmit& t)
{
    if (t.image_pair == 0) return;
    STransparentWorldSubmit sub = {};
    sub.kind = ETransparentWorldKind::Tile;
    sub.sort_depth = t.sort_depth;
    sub.tile = t;
    transparent_world_queue.push_back(sub);
}

void TRenderer::SubmitHelperMesh(const SHelperMeshSubmit& m)
{
    if (m.mesh == 0 || m.mesh > meshes.size()) return;
    STransparentWorldSubmit sub = {};
    sub.kind = ETransparentWorldKind::Helper;
    sub.sort_depth = m.sort_depth;
    sub.helper = m;
    transparent_world_queue.push_back(sub);
}

void TRenderer::DrainMeshQueue()
{
    if (mesh_queue.empty() || !mesh_pipeline.id) return;

    // Cap to what the instance buffer can hold.
    if (int32_t(mesh_queue.size()) > kMaxMeshInstances)
        mesh_queue.resize(kMaxMeshInstances);

    // Sort by mesh handle so same-asset instances are contiguous.
    std::sort(mesh_queue.begin(), mesh_queue.end(),
              [](const SMeshSubmit& a, const SMeshSubmit& b) { return a.mesh < b.mesh; });

    // Pack instance rows (24 floats each: w0..w3 + tint + obj_id rgba8).
    // Capacity is retained by the renderer; the draw loop must not allocate
    // just because animated/visible mesh count changes within the usual range.
    mesh_instance_scratch.resize(mesh_queue.size() * 24);
    for (size_t i = 0; i < mesh_queue.size(); ++i) {
        const SMeshSubmit& s = mesh_queue[i];
        float* dst = &mesh_instance_scratch[i * 24];
        std::memcpy(dst,      s.world, sizeof(s.world));
        std::memcpy(dst + 16, s.tint,  sizeof(s.tint));
        dst[20] = float((s.obj_id >>  0) & 0xFFu) / 255.0f;
        dst[21] = float((s.obj_id >>  8) & 0xFFu) / 255.0f;
        dst[22] = float((s.obj_id >> 16) & 0xFFu) / 255.0f;
        dst[23] = float((s.obj_id >> 24) & 0xFFu) / 255.0f;
    }
    const sg_range r = { mesh_instance_scratch.data(), mesh_instance_scratch.size() * sizeof(float) };
    sg_update_buffer(mesh_instance_vb, &r);

    sg_apply_pipeline(mesh_pipeline);

    // Uniform block: vp/camz/camw.
    float u[12] = {};
    u[0] = recon.ox;
    u[1] = recon.oy;
    u[2] = float(width  + 2 * kGBufPad);
    u[3] = float(height + 2 * kGBufPad);
    u[4] = recon.z_near;
    u[5] = recon.zspan;
    u[6] = recon.kcam_forward;
    u[7] = recon.reserved;
    u[8] = recon.center_wx;
    u[9] = recon.center_wy;
    u[10] = recon.zoom;
    u[11] = 0.0f;
    const sg_range u_range = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);

    // Emit one instanced draw per contiguous run of equal mesh handles.
    size_t i = 0;
    while (i < mesh_queue.size()) {
        size_t j = i + 1;
        while (j < mesh_queue.size() && mesh_queue[j].mesh == mesh_queue[i].mesh) ++j;

        const SMeshEntry& me = meshes[mesh_queue[i].mesh - 1];
        sg_bindings bind = {};
        bind.vertex_buffers[0]        = me.vbuf;
        bind.vertex_buffers[1]        = mesh_instance_vb;
        bind.vertex_buffer_offsets[1] = int(i) * int(sizeof(float)) * 24;
        bind.index_buffer             = me.ibuf;
        bind.fs_images[0]             = TextureImage(me.albedo);
        sg_apply_bindings(&bind);
        sg_draw(0, me.num_indices, int(j - i));

        i = j;
    }
}

void TRenderer::EmitTransparentTile(const STileSubmit& t)
{
    const sg_image color = ImagePairColor(t.image_pair);
    const sg_image depth = ImagePairDepth(t.image_pair);
    const sg_image height_minmax = ImagePairHeightMinMax(t.image_pair);
    if (!transparent_tile_pipeline.id || !color.id || !depth.id || !height_minmax.id) return;

    const int32_t gbw = width  + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    const int32_t px  = t.dst_x + kGBufPad;
    const int32_t py  = t.dst_y + kGBufPad;
    const int32_t src_w = t.src_w > 0 ? t.src_w : t.dst_w;
    const int32_t src_h = t.src_h > 0 ? t.src_h : t.dst_h;
    float rect_x = float(px);
    float rect_y = float(py);
    float rect_w = float(t.dst_w);
    float rect_h = float(t.dst_h);
    float cov_x0 = std::clamp(t.coverage_px_x0, 0.0f, float(src_w));
    float cov_y0 = std::clamp(t.coverage_px_y0, 0.0f, float(src_h));
    float cov_x1 = std::clamp(t.coverage_px_x1, 0.0f, float(src_w));
    float cov_y1 = std::clamp(t.coverage_px_y1, 0.0f, float(src_h));
    if (!(cov_x0 < cov_x1 && cov_y0 < cov_y1))
    {
        cov_x0 = 0.0f;
        cov_y0 = 0.0f;
        cov_x1 = float(src_w);
        cov_y1 = float(src_h);
    }
    if (recon.reserved > 0.5f)
    {
        const float focal_zoom = recon.kcam_forward * recon.zoom;
        const float anchor_cam_x = t.anchor_cam_x;
        const float anchor_cam_y = t.anchor_cam_y;
        float z0 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_min * t.zraw_to_wu * recon.z_scale;
        float z1 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_max * t.zraw_to_wu * recon.z_scale;
        if (z1 < z0) std::swap(z0, z1);
        z0 = (std::max)(z0, 1.0f);
        z1 = (std::max)(z1, z0 + 1.0f);
        const float tile_scale = recon.tile_scale > 0.0f ? recon.tile_scale : 1.0f;
        const float sx_min = (cov_x0 - t.anchor_px_x) * tile_scale;
        const float sx_max = (cov_x1 - t.anchor_px_x) * tile_scale;
        const float sy_min = (cov_y0 - t.anchor_px_y) * tile_scale;
        const float sy_max = (cov_y1 - t.anchor_px_y) * tile_scale;
        const float sxv[2] = { sx_min, sx_max };
        const float syv[2] = { sy_min, sy_max };
        const float zv[2] = { z0, z1 };
        float min_x =  FLT_MAX, min_y =  FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX;
        for (float z : zv)
        for (float sx : sxv)
        for (float sy : syv)
        {
            const float qx = recon.ox + (anchor_cam_x + sx) * focal_zoom / z;
            const float qy = recon.oy + (anchor_cam_y + sy) * focal_zoom / z;
            min_x = (std::min)(min_x, qx);
            min_y = (std::min)(min_y, qy);
            max_x = (std::max)(max_x, qx);
            max_y = (std::max)(max_y, qy);
        }
        constexpr float kPerspectiveTilePadPx = 2.0f;
        rect_x = std::floor(min_x - kPerspectiveTilePadPx);
        rect_y = std::floor(min_y - kPerspectiveTilePadPx);
        rect_w = std::ceil(max_x + kPerspectiveTilePadPx) - rect_x;
        rect_h = std::ceil(max_y + kPerspectiveTilePadPx) - rect_y;
    }
    const bool uv_independent_projection =
        recon.reserved > 0.5f && perspective_projection_mode > 0;
    if (!ClipTileRasterRect(rect_x, rect_y, rect_w, rect_h,
                            uv_independent_projection))
        return;

    sg_apply_pipeline(transparent_tile_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = tile_vbuf;
    bind.fs_images[0]      = color;
    bind.fs_images[1]      = depth;
    bind.fs_images[2]      = height_minmax;
    sg_apply_bindings(&bind);

    const float nx = (2.0f * rect_x / gbw)  - 1.0f;
    const float nw = (2.0f * rect_w) / gbw;
    const float ny = 1.0f - (2.0f * (rect_y + rect_h) / gbh);
    const float nh = (2.0f * rect_h) / gbh;

    // Keep layout identical to the opaque tile shader. Transparent tiles do
    // not write obj_id, but still need the shared camera projection params.
    float uniforms[12 * 4] = {};
    int32_t off = 0;
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    uniforms[off++] = t.anchor_z;
    uniforms[off++] = t.depth_mul;
    uniforms[off++] = recon.tile_scale;
    uniforms[off++] = recon.zoom;
    uniforms[off++] = t.root_wx; uniforms[off++] = t.root_wy;
    uniforms[off++] = t.root_wz; uniforms[off++] = t.zraw_to_wu;
    uniforms[off++] = t.anchor_px_x; uniforms[off++] = t.anchor_px_y;
    uniforms[off++] = float(src_w); uniforms[off++] = float(src_h);
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = t.zraw_min;
    uniforms[off++] = t.zraw_max;
    // obj_id slot: zeros for transparent tiles (no need to pick them).
    uniforms[off++] = 0.0f; uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f; uniforms[off++] = 0.0f;
    uniforms[off++] = recon.center_wx; uniforms[off++] = recon.center_wy;
    uniforms[off++] = recon.kcam_forward; uniforms[off++] = recon.reserved;
    uniforms[off++] = recon.ox; uniforms[off++] = recon.oy;
    uniforms[off++] = float(gbw); uniforms[off++] = float(gbh);
    uniforms[off++] = cov_x0; uniforms[off++] = cov_y0;
    uniforms[off++] = cov_x1; uniforms[off++] = cov_y1;
    uniforms[off++] = t.anchor_cam_x;
    uniforms[off++] = t.anchor_cam_y;
    uniforms[off++] = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset;
    uniforms[off++] = t.zraw_to_wu * recon.z_scale;
    {
        const uint32_t h = t.obj_id * 1103515245u + 12345u;
        const float r = 0.25f + 0.70f * float((h >>  0) & 0xFFu) / 255.0f;
        const float g = 0.25f + 0.70f * float((h >>  8) & 0xFFu) / 255.0f;
        const float b = 0.25f + 0.70f * float((h >> 16) & 0xFFu) / 255.0f;
        uniforms[off++] = float(perspective_debug_mode);
        uniforms[off++] = r; uniforms[off++] = g; uniforms[off++] = b;
    }
    uniforms[off++] = float(perspective_steps);
    uniforms[off++] = float(perspective_refine);
    // raycast.z is the visible-albedo padding in G-buffer pixels. Depth,
    // scene_z, normals, and ids still render in this border by design.
    uniforms[off++] = float(kGBufPad);
    uniforms[off++] = recon.reserved > 0.5f ? float(perspective_projection_mode) : 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::EmitTransparentHelper(const SHelperMeshSubmit& s)
{
    sg_pipeline back_pipeline = s.additive_blend ? helper_mesh_add_back_pipeline : helper_mesh_back_pipeline;
    sg_pipeline front_pipeline = s.additive_blend ? helper_mesh_add_front_pipeline : helper_mesh_front_pipeline;
    if (!back_pipeline.id || !front_pipeline.id) return;

    const float view_dir[4] = { -0.5f, -0.5f, 1.0f, 1.0f };
    const SMeshEntry& me = meshes[s.mesh - 1];

    sg_apply_pipeline(back_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = me.vbuf;
    bind.index_buffer = me.ibuf;
    bind.fs_images[0] = TextureImage(me.albedo);
    sg_apply_bindings(&bind);

    float vsu[28] = {};
    int vo = 0;
    std::memcpy(&vsu[vo], s.world + 0,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 4,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 8,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 12, sizeof(float) * 4); vo += 4;
    vsu[vo++] = recon.ox;
    vsu[vo++] = recon.oy;
    vsu[vo++] = float(width  + 2 * kGBufPad);
    vsu[vo++] = float(height + 2 * kGBufPad);
    vsu[vo++] = recon.z_near;
    vsu[vo++] = recon.zspan;
    vsu[vo++] = recon.kcam_forward;
    vsu[vo++] = recon.reserved;
    vsu[vo++] = recon.center_wx;
    vsu[vo++] = recon.center_wy;
    vsu[vo++] = recon.zoom;
    vsu[vo++] = 0.0f;
    const sg_range vsr = { vsu, sizeof(vsu) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vsr);

    float fsu[28] = {};
    int o = 0;
    std::memcpy(&fsu[o], s.diffuse, sizeof(s.diffuse)); o += 4;
    std::memcpy(&fsu[o], s.ambient, sizeof(s.ambient)); o += 4;
    std::memcpy(&fsu[o], s.specular, sizeof(s.specular)); o += 4;
    std::memcpy(&fsu[o], s.emissive, sizeof(s.emissive)); o += 4;
    fsu[o++] = light.dir[0]; fsu[o++] = light.dir[1]; fsu[o++] = light.dir[2]; fsu[o++] = light.intensity;
    fsu[o++] = light.color[0]; fsu[o++] = light.color[1]; fsu[o++] = light.color[2]; fsu[o++] = light.ambient;
    fsu[o++] = view_dir[0]; fsu[o++] = view_dir[1]; fsu[o++] = view_dir[2]; fsu[o++] = s.power;
    const sg_range fsr = { fsu, sizeof(fsu) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &fsr);

    sg_draw(0, me.num_indices, 1);
    sg_apply_pipeline(front_pipeline);
    sg_apply_bindings(&bind);
    sg_draw(0, me.num_indices, 1);
}

void TRenderer::DrainTransparentWorldQueue()
{
    if (transparent_world_queue.empty() || !helper_pass.id) return;

    std::stable_sort(transparent_world_queue.begin(), transparent_world_queue.end(),
                     [](const STransparentWorldSubmit& a, const STransparentWorldSubmit& b)
                     {
                         return a.sort_depth < b.sort_depth;
                     });

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_LOAD;
    pa.depth.action = SG_ACTION_LOAD;
    pa.stencil.action = SG_ACTION_DONTCARE;
    sg_begin_pass(helper_pass, &pa);
    for (const auto& s : transparent_world_queue)
    {
        if (s.kind == ETransparentWorldKind::Tile)
            EmitTransparentTile(s.tile);
        else
            EmitTransparentHelper(s.helper);
    }
    sg_end_pass();
    transparent_world_queue.clear();
    lit_target_dirty = true;
}

// *************************************************************************
// * Screen-Space Ambient Occlusion  --  Pass [2]                          *
// *************************************************************************

void TRenderer::InitAOPipeline()
{
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kAOVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.fs.source = kAOFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = sizeof(float) * 16;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "ao_settings";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "camera";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "normal_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[2].name         = "depth_tex";
    sh.fs.images[2].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[2].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.ao.shader";
    ao_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = ao_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_R32F;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.ao.pipeline";
    ao_pipeline = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownAOPipeline()
{
    if (ao_pipeline.id) { sg_destroy_pipeline(ao_pipeline); ao_pipeline = {}; }
    if (ao_shader.id)   { sg_destroy_shader(ao_shader);     ao_shader   = {}; }
}

void TRenderer::RunAOPass()
{
    if (!ao_pass.id || !ao_pipeline.id) return;

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    sg_begin_pass(ao_pass, &pa);

    sg_apply_pipeline(ao_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = color_target;
    bind.fs_images[1]      = normal_target;
    bind.fs_images[2]      = scene_z_target;
    sg_apply_bindings(&bind);

    float u[16] = {};
    int32_t o = 0;
    u[o++] = recon.ox; u[o++] = recon.oy; u[o++] = recon.z_near; u[o++] = recon.zspan;
    u[o++] = recon.center_wx; u[o++] = recon.center_wy; u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    u[o++] = light.ao_radius_px;
    u[o++] = light.ao_enable ? light.ao_strength : 0.0f;
    u[o++] = light.ao_bias;
    u[o++] = light.ao_max_dist_wu;
    u[o++] = recon.zoom;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();
}

// *************************************************************************
// * Sun Shadow Mask Pipeline  --  Pass [3]                                *
// *************************************************************************

void TRenderer::InitShadowPipeline()
{
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kShadowVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.fs.source = kShadowFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = sizeof(float) * 28;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "camera";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "shadow";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "shadow_dir";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "shadow_world_dir";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].name = "normal_lighting";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "depth_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.shadow.shader";
    shadow_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = shadow_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_R32F;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.shadow.pipeline";
    shadow_pipeline = sg_make_pipeline(&pip);

    sg_shader_desc blur_sh = {};
    blur_sh.attrs[0].name = "pos";
    blur_sh.attrs[0].sem_name  = "POSITION";
    blur_sh.attrs[0].sem_index = 0;
    blur_sh.attrs[1].name = "uv";
    blur_sh.attrs[1].sem_name  = "TEXCOORD";
    blur_sh.attrs[1].sem_index = 0;
    blur_sh.vs.source = kShadowVs;
    blur_sh.vs.entry  = kShaderVsEntry;
    blur_sh.fs.source = kShadowBlurFs;
    blur_sh.fs.entry  = kShaderFsEntry;
    blur_sh.fs.uniform_blocks[0].size = sizeof(float) * 4;
    blur_sh.fs.uniform_blocks[0].uniforms[0].name = "blur";
    blur_sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    blur_sh.fs.images[0].name         = "shadow_tex";
    blur_sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    blur_sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    blur_sh.label = "renderer.shadow_blur.shader";
    shadow_blur_shader = sg_make_shader(&blur_sh);

    sg_pipeline_desc blur_pip = {};
    blur_pip.shader = shadow_blur_shader;
    blur_pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    blur_pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    blur_pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    blur_pip.colors[0].pixel_format  = SG_PIXELFORMAT_R32F;
    blur_pip.colors[0].blend.enabled = false;
    blur_pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    blur_pip.label = "renderer.shadow_blur.pipeline";
    shadow_blur_pipeline = sg_make_pipeline(&blur_pip);
}

void TRenderer::ShutdownShadowPipeline()
{
    if (shadow_blur_pipeline.id) { sg_destroy_pipeline(shadow_blur_pipeline); shadow_blur_pipeline = {}; }
    if (shadow_pipeline.id)      { sg_destroy_pipeline(shadow_pipeline);      shadow_pipeline      = {}; }
    if (shadow_blur_shader.id)   { sg_destroy_shader(shadow_blur_shader);     shadow_blur_shader   = {}; }
    if (shadow_shader.id)        { sg_destroy_shader(shadow_shader);          shadow_shader        = {}; }
}

void TRenderer::RunShadowBlurPass(sg_pass pass, sg_image source, float dir_x, float dir_y)
{
    if (!pass.id || !source.id || !shadow_blur_pipeline.id) return;

    const int32_t gbw = width + 2 * kGBufPad;
    const float shadow_scale = gbw > 0 ? float(shadow_width) / float(gbw) : 1.0f;
    const float radius_px = light.sun_shadow_softness_px * shadow_scale;

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    sg_begin_pass(pass, &pa);

    sg_apply_pipeline(shadow_blur_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = source;
    sg_apply_bindings(&bind);

    float u[4] = { dir_x, dir_y, radius_px, 0.0f };
    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();
}

void TRenderer::RunShadowPass()
{
    if (!shadow_pass.id || !shadow_pipeline.id) return;

    // The shadow buffer stores only cast-shadow visibility. Normal-facing
    // sun visibility is applied later in the lighting pass, so the blur does
    // not smear N.L silhouettes across geometry.
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    sg_begin_pass(shadow_pass, &pa);

    if (light.mode == 1 && light.sun_shadow_enable)
    {
        sg_apply_pipeline(shadow_pipeline);
        sg_bindings bind = {};
        bind.vertex_buffers[0] = composite_vbuf;
        bind.fs_images[0]      = color_target;
        bind.fs_images[1]      = scene_z_target;
        sg_apply_bindings(&bind);

        float u[28] = {};
        int32_t o = 0;
        u[o++] = recon.ox; u[o++] = recon.oy; u[o++] = recon.z_near; u[o++] = recon.zspan;
        u[o++] = recon.center_wx; u[o++] = recon.center_wy; u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
        u[o++] = recon.zoom; u[o++] = 0.0f; u[o++] = 0.0f; u[o++] = 0.0f;
        u[o++] = light.sun_shadow_step_wu;
        u[o++] = 0.0f;
        u[o++] = float(light.sun_shadow_max_steps);
        u[o++] = 1.0f;
        u[o++] = 1.0f;
        u[o++] = light.sun_shadow_depth_cutoff_wu;
        u[o++] = light.sun_shadow_wz_scale;
        u[o++] = 0.0f;
        u[o++] = light.shadow_world_dir[0];
        u[o++] = light.shadow_world_dir[1];
        u[o++] = light.shadow_world_dir[2];
        u[o++] = 0.0f;
        u[o++] = light.normal_lighting_hardness;
        u[o++] = light.sun_shadow_bias_wu;
        u[o++] = 0.0f;
        u[o++] = 0.0f;
        const sg_range r = { u, sizeof(u) };
        sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
        sg_draw(0, 6, 1);
    }

    sg_end_pass();

    if (light.mode == 1 && light.sun_shadow_enable && light.sun_shadow_softness_px > 0.01f)
    {
        RunShadowBlurPass(shadow_blur_pass, shadow_target, 1.0f, 0.0f);
        RunShadowBlurPass(shadow_pass, shadow_blur_target, 0.0f, 1.0f);
    }
}

// *************************************************************************
// * Deferred Lighting Pipeline  --  Pass [4]                              *
// *************************************************************************

void TRenderer::InitLightPipeline()
{
    constexpr int32_t kPL = TRenderer::kMaxPointLights;
    // 6 scalar vec4s + kPL plight_pos + kPL plight_col
    //                + shadow + shadow_dir + shadow_world_dir + normal_lighting
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4 + 1;   // +1 = selected_obj_id (editor outline)
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kLightVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.fs.source = kLightFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = kUB_bytes;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "light_dir";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "light_col";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "ambient_col";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "settings";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].name = "plight_pos";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].array_count = kPL;
    sh.fs.uniform_blocks[0].uniforms[7].name = "plight_col";
    sh.fs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[7].array_count = kPL;
    sh.fs.uniform_blocks[0].uniforms[8].name = "shadow";
    sh.fs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[9].name = "shadow_dir";
    sh.fs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[10].name = "shadow_world_dir";
    sh.fs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[11].name = "normal_lighting";
    sh.fs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[12].name = "selected_obj_id";
    sh.fs.uniform_blocks[0].uniforms[12].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "normal_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[2].name         = "depth_tex";
    sh.fs.images[2].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[2].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[3].name         = "ao_tex";
    sh.fs.images[3].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[3].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[4].name         = "id_tex";
    sh.fs.images[4].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[4].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[5].name         = "shadow_tex";
    sh.fs.images[5].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[5].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.light.shader";
    light_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = light_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.light.pipeline";
    light_pipeline = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownLightPipeline()
{
    if (light_pipeline.id) { sg_destroy_pipeline(light_pipeline); light_pipeline = {}; }
    if (light_shader.id)   { sg_destroy_shader(light_shader);     light_shader   = {}; }
}

void TRenderer::SetReconstructionParams(float ox, float oy,
                                        float z_near, float z_far,
                                        float center_wx, float center_wy,
                                        float kcam_forward, float perspective,
                                        float zoom,
                                        float z_offset,
                                        float z_scale,
                                        float tile_scale)
{
    // Callers pass the world-origin screen offset in display-space pixels.
    // The light shader samples the padded G-buffer, so shift by +pad to
    // keep the world->screen math consistent.
    recon.ox           = ox + float(kGBufPad);
    recon.oy           = oy + float(kGBufPad);
    recon.z_near       = z_near;
    recon.zspan        = z_far - z_near;
    recon.center_wx    = center_wx;
    recon.center_wy    = center_wy;
    recon.kcam_forward = kcam_forward;
    recon.reserved     = perspective;
    recon.zoom         = zoom > 0.0f ? zoom : 1.0f;
    recon.z_offset     = z_offset;
    recon.z_scale      = z_scale > 0.0f ? z_scale : 1.0f;
    recon.tile_scale   = tile_scale > 0.0f ? tile_scale : 1.0f;
}

void TRenderer::SetPerspectiveRaycastParams(int32_t steps, int32_t refine)
{
    perspective_steps = std::clamp(steps, 4, 128);
    perspective_refine = std::clamp(refine, 0, 8);
}

void TRenderer::SetPerspectiveProjectionMode(int32_t mode)
{
    perspective_projection_mode = std::clamp(mode, 0, 2);
}

void TRenderer::SetPerspectiveProxyRasterScale(float scale)
{
    perspective_proxy_raster_scale = std::clamp(scale, 0.25f, 1.25f);
}

void TRenderer::RunLightingPass()
{
    if (!lit_pass.id || !light_pipeline.id) return;

    RunAOPass();
    RunShadowPass();

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { tile_clear_rgba[0], tile_clear_rgba[1],
                            tile_clear_rgba[2], tile_clear_rgba[3] };
    sg_begin_pass(lit_pass, &pa);

    sg_apply_pipeline(light_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = color_target;
    bind.fs_images[1]      = normal_target;
    bind.fs_images[2]      = scene_z_target;
    bind.fs_images[3]      = ao_target;
    bind.fs_images[4]      = id_target;
    bind.fs_images[5]      = shadow_target;
    sg_apply_bindings(&bind);

    constexpr int32_t kPL = TRenderer::kMaxPointLights;
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4 + 1;   // +1 = selected_obj_id (editor outline)
    float u[kUB_vec4s * 4] = {};
    int32_t o = 0;
    u[o++] = recon.ox;    u[o++] = recon.oy;
    u[o++] = recon.z_near; u[o++] = recon.zspan;
    u[o++] = recon.center_wx; u[o++] = recon.center_wy;
    u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    u[o++] = light.dir[0]; u[o++] = light.dir[1]; u[o++] = light.dir[2]; u[o++] = light.intensity;
    u[o++] = light.color[0]; u[o++] = light.color[1]; u[o++] = light.color[2]; u[o++] = light.ambient;
    u[o++] = light.ambient_color[0]; u[o++] = light.ambient_color[1];
    u[o++] = light.ambient_color[2]; u[o++] = light.light_ceiling;
    u[o++] = float(light.view_mode);
    u[o++] = float(light.plight_count);
    u[o++] = float(light.mode);
    u[o++] = recon.zoom;
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_pos[i][k];
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_col[i][k];
    u[o++] = light.sun_shadow_step_wu;
    u[o++] = light.sun_shadow_softness_px;
    u[o++] = float(light.sun_shadow_max_steps);
    u[o++] = light.sun_shadow_enable ? 1.0f : 0.0f;
    u[o++] = 1.0f;
    u[o++] = light.sun_shadow_depth_cutoff_wu;
    u[o++] = light.sun_shadow_wz_scale;
    u[o++] = 0.0f;
    u[o++] = light.shadow_world_dir[0];
    u[o++] = light.shadow_world_dir[1];
    u[o++] = light.shadow_world_dir[2];
    u[o++] = 0.0f;
    u[o++] = light.normal_lighting_hardness;
    u[o++] = light.sun_shadow_bias_wu;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    // selected_obj_id packed into RGBA8 channels for the outline shader.
    // 0 means "no selection -- skip outline".
    u[o++] = float((selected_obj_id >>  0) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >>  8) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >> 16) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >> 24) & 0xFFu) / 255.0f;

    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();

    DrainTransparentWorldQueue();
    DrainFxQueue();
    DrainOverlayQueue();
    lit_target_dirty = true;
}

// *************************************************************************
// * Tile Pass Submission  (BeginTilePass / SubmitTile / EndTilePass)      *
// *************************************************************************

bool TRenderer::ClipTileRasterRect(float& rect_x, float& rect_y,
                                   float& rect_w, float& rect_h,
                                   bool uv_independent_projection)
{
    if (!(std::isfinite(rect_x) && std::isfinite(rect_y) &&
          std::isfinite(rect_w) && std::isfinite(rect_h)) ||
        rect_w <= 0.0f || rect_h <= 0.0f)
    {
        ++current_tile_pass_stats.tile_rects_culled;
        return false;
    }

    const float gbw = float(width + 2 * kGBufPad);
    const float gbh = float(height + 2 * kGBufPad);
    current_tile_pass_stats.tile_projected_pixels += uint64_t(std::ceil(rect_w * rect_h));

    const float x0 = (std::max)(rect_x, 0.0f);
    const float y0 = (std::max)(rect_y, 0.0f);
    const float x1 = (std::min)(rect_x + rect_w, gbw);
    const float y1 = (std::min)(rect_y + rect_h, gbh);
    if (x1 <= x0 || y1 <= y0)
    {
        ++current_tile_pass_stats.tile_rects_culled;
        return false;
    }

    ++current_tile_pass_stats.tile_draws;
    current_tile_pass_stats.tile_clipped_pixels += uint64_t(std::ceil((x1 - x0) * (y1 - y0)));

    // Only shrink the submitted quad when UV is reconstructed from screen
    // position in the fragment shader. The flat/ortho path linearly
    // interpolates source UVs over the quad, so partial rect clipping would
    // remap the sprite.
    if (uv_independent_projection)
    {
        rect_x = x0;
        rect_y = y0;
        rect_w = x1 - x0;
        rect_h = y1 - y0;
    }
    return true;
}

void TRenderer::BeginTilePass(float r, float g, float b, float a)
{
    if (!default_pass.id) return;
    tile_clear_rgba[0] = r; tile_clear_rgba[1] = g;
    tile_clear_rgba[2] = b; tile_clear_rgba[3] = a;
    current_tile_pass_stats = {};
    tile_queue.clear();
    transparent_world_queue.clear();
    overlay_queue.clear();
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 0.0f };
    pa.colors[1].action = SG_ACTION_CLEAR;
    pa.colors[1].value  = { 0.5f, 0.5f, 0.5f, 1.0f };
    pa.colors[2].action = SG_ACTION_CLEAR;
    pa.colors[2].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    pa.colors[3].action = SG_ACTION_CLEAR;
    pa.colors[3].value  = { 0.0f, 0.0f, 0.0f, 0.0f };  // id=0 means "empty"
    pa.depth.action     = SG_ACTION_CLEAR;
    pa.depth.value      = 1.0f;
    pa.stencil.action   = SG_ACTION_DONTCARE;
    sg_begin_pass(default_pass, &pa);
    color_target_dirty = true;
}

void TRenderer::SubmitTile(const STileSubmit& t)
{
    tile_queue.push_back(t);
}

void TRenderer::SubmitOverlay(const SOverlaySubmit& t)
{
    if (t.texture == kInvalidTexture && t.image_pair == 0) return;
    overlay_queue.push_back(t);
}

void TRenderer::EmitTile(const STileSubmit& t)
{
    const sg_image color = ImagePairColor(t.image_pair);
    const sg_image depth = ImagePairDepth(t.image_pair);
    const sg_image height_minmax = ImagePairHeightMinMax(t.image_pair);
    const SRendererImagePairEntry* image_pair_entry =
        (t.image_pair > 0 && t.image_pair <= image_pair_assets.size())
            ? &image_pair_assets[t.image_pair - 1]
            : nullptr;
    if (!tile_pipeline.id || !color.id || !depth.id || !height_minmax.id) return;
    if (width <= 0 || height <= 0) return;

    const int32_t gbw = width  + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    const int32_t px  = t.dst_x + kGBufPad;
    const int32_t py  = t.dst_y + kGBufPad;
    const int32_t src_w = t.src_w > 0 ? t.src_w : t.dst_w;
    const int32_t src_h = t.src_h > 0 ? t.src_h : t.dst_h;
    float rect_x = float(px);
    float rect_y = float(py);
    float rect_w = float(t.dst_w);
    float rect_h = float(t.dst_h);
    float proxy_vertices[kMaxTileProxyVerticesPerDraw * 4] = {};
    float proxy_projected_area = 0.0f;
    float proxy_clipped_area = 0.0f;
    int32_t proxy_vertex_count = 0;
    int32_t proxy_source_point_count = 0;
    bool proxy_from_tight_mesh = false;
    const bool debug_proxy_wire = perspective_debug_mode == 3;
    float cov_x0 = std::clamp(t.coverage_px_x0, 0.0f, float(src_w));
    float cov_y0 = std::clamp(t.coverage_px_y0, 0.0f, float(src_h));
    float cov_x1 = std::clamp(t.coverage_px_x1, 0.0f, float(src_w));
    float cov_y1 = std::clamp(t.coverage_px_y1, 0.0f, float(src_h));
    if (!(cov_x0 < cov_x1 && cov_y0 < cov_y1))
    {
        cov_x0 = 0.0f;
        cov_y0 = 0.0f;
        cov_x1 = float(src_w);
        cov_y1 = float(src_h);
    }
    if (recon.reserved > 0.5f)
    {
        const float focal_zoom = recon.kcam_forward * recon.zoom;
        const float anchor_cam_x = t.anchor_cam_x;
        const float anchor_cam_y = t.anchor_cam_y;
        float z0 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_min * t.zraw_to_wu * recon.z_scale;
        float z1 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_max * t.zraw_to_wu * recon.z_scale;
        if (z1 < z0) std::swap(z0, z1);
        z0 = (std::max)(z0, 1.0f);
        z1 = (std::max)(z1, z0 + 1.0f);
        const float tile_scale = recon.tile_scale > 0.0f ? recon.tile_scale : 1.0f;
        float min_x =  FLT_MAX, min_y =  FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX;
        float whole_projected_area = 0.0f;
        float whole_clipped_area = 0.0f;
        float whole_proxy_vertices[kMaxTileProxyVerticesPerHull * 4] = {};
        const int32_t whole_proxy_vertex_count = BuildProjectedSourceRectProxy(
            cov_x0, cov_y0, cov_x1, cov_y1,
            t.anchor_px_x, t.anchor_px_y,
            anchor_cam_x, anchor_cam_y,
            focal_zoom, tile_scale,
            recon.ox, recon.oy,
            z0, z1,
            gbw, gbh,
            whole_proxy_vertices,
            kMaxTileProxyVerticesPerHull,
            whole_projected_area,
            whole_clipped_area,
            perspective_proxy_raster_scale,
            debug_proxy_wire,
            &min_x, &min_y, &max_x, &max_y);
        constexpr float kPerspectiveTilePadPx = 2.0f;
        rect_x = std::floor(min_x - kPerspectiveTilePadPx);
        rect_y = std::floor(min_y - kPerspectiveTilePadPx);
        rect_w = std::ceil(max_x + kPerspectiveTilePadPx) - rect_x;
        rect_h = std::ceil(max_y + kPerspectiveTilePadPx) - rect_y;

        // Relief projection reconstructs UV from the screen-space ray, so the
        // raster proxy does not need to be an axis-aligned sprite rect. The
        // preferred path uses the asset-built source-space front-face hull,
        // then projects its min/max-depth slab into one screen-space draw.
        // If the hull is absent or invalid, fall back to the whole coverage
        // box rather than underdrawing.
        if (perspective_projection_mode > 0 && tile_proxy_vbuf.id)
        {
            if (image_pair_entry && !image_pair_entry->tight_proxy_hull_px.empty())
            {
                const std::vector<float>& hull = image_pair_entry->tight_proxy_hull_px;
                const int32_t hull_points = int32_t(hull.size() / 2u);
                if (hull_points >= 3 && hull_points <= kMaxTileTightProxyHullVertices)
                {
                    proxy_vertex_count = BuildProjectedSourceHullProxy(
                        hull.data(), hull_points,
                        t.anchor_px_x, t.anchor_px_y,
                        anchor_cam_x, anchor_cam_y,
                        focal_zoom, tile_scale,
                        recon.ox, recon.oy,
                        z0, z1,
                        gbw, gbh,
                        proxy_vertices,
                        kMaxTileProxyVerticesPerDraw,
                        proxy_projected_area,
                        proxy_clipped_area,
                        perspective_proxy_raster_scale,
                        debug_proxy_wire);
                    if (proxy_vertex_count > 0)
                    {
                        proxy_from_tight_mesh = true;
                        proxy_source_point_count = hull_points;
                    }
                }
            }

            if (!proxy_from_tight_mesh && whole_proxy_vertex_count > 0)
            {
                std::memcpy(proxy_vertices,
                            whole_proxy_vertices,
                            size_t(whole_proxy_vertex_count) * 4u * sizeof(float));
                proxy_vertex_count = whole_proxy_vertex_count;
                proxy_projected_area = whole_projected_area;
                proxy_clipped_area = whole_clipped_area;
                proxy_source_point_count = 4;
            }
        }
    }

    int32_t draw_vertex_count = 6;
    int32_t draw_vertex_offset = 0;
    sg_buffer draw_vertex_buffer = tile_vbuf;
    bool proxy_draw = false;
    if (proxy_vertex_count > 0)
    {
        const sg_range proxy_range = {
            proxy_vertices,
            size_t(proxy_vertex_count) * sizeof(float) * 4u
        };
        const int32_t offset = sg_append_buffer(tile_proxy_vbuf, &proxy_range);
        if (offset >= 0 && !sg_query_buffer_overflow(tile_proxy_vbuf))
        {
            proxy_draw = true;
            draw_vertex_count = proxy_vertex_count;
            draw_vertex_offset = offset;
            draw_vertex_buffer = tile_proxy_vbuf;
            ++current_tile_pass_stats.tile_draws;
            ++current_tile_pass_stats.tile_proxy_draws;
            if (proxy_from_tight_mesh)
            {
                ++current_tile_pass_stats.tile_tight_proxy_draws;
                current_tile_pass_stats.tile_tight_proxy_points += uint32_t(proxy_source_point_count);
            }
            current_tile_pass_stats.tile_projected_pixels += uint64_t(std::ceil(proxy_projected_area));
            current_tile_pass_stats.tile_clipped_pixels += uint64_t(std::ceil(proxy_clipped_area));
        }
    }

    if (!proxy_draw)
    {
        const bool uv_independent_projection =
            recon.reserved > 0.5f && perspective_projection_mode > 0;
        if (!ClipTileRasterRect(rect_x, rect_y, rect_w, rect_h,
                                uv_independent_projection))
            return;
    }

    sg_bindings bind = {};
    bind.vertex_buffers[0] = draw_vertex_buffer;
    bind.vertex_buffer_offsets[0] = draw_vertex_offset;
    bind.fs_images[0]      = color;
    bind.fs_images[1]      = depth;
    bind.fs_images[2]      = height_minmax;
    sg_apply_bindings(&bind);

    const float nx = proxy_draw ? 0.0f : (2.0f * rect_x / gbw)  - 1.0f;
    const float nw = proxy_draw ? 1.0f : (2.0f * rect_w) / gbw;
    const float ny = proxy_draw ? 0.0f : 1.0f - (2.0f * (rect_y + rect_h) / gbh);
    const float nh = proxy_draw ? 1.0f : (2.0f * rect_h) / gbh;

    float uniforms[12 * 4] = {};
    int32_t off = 0;
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    uniforms[off++] = t.anchor_z;
    uniforms[off++] = t.depth_mul;
    uniforms[off++] = recon.tile_scale;
    uniforms[off++] = recon.zoom;
    uniforms[off++] = t.root_wx; uniforms[off++] = t.root_wy;
    uniforms[off++] = t.root_wz; uniforms[off++] = t.zraw_to_wu;
    uniforms[off++] = t.anchor_px_x; uniforms[off++] = t.anchor_px_y;
    uniforms[off++] = float(src_w); uniforms[off++] = float(src_h);
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = t.zraw_min;
    uniforms[off++] = t.zraw_max;
    // obj_id packed into 4 bytes (RGBA8) for the gbuffer ID target.
    uniforms[off++] = float((t.obj_id >>  0) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >>  8) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >> 16) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >> 24) & 0xFFu) / 255.0f;
    uniforms[off++] = recon.center_wx; uniforms[off++] = recon.center_wy;
    uniforms[off++] = recon.kcam_forward; uniforms[off++] = recon.reserved;
    uniforms[off++] = recon.ox; uniforms[off++] = recon.oy;
    uniforms[off++] = float(gbw); uniforms[off++] = float(gbh);
    uniforms[off++] = cov_x0; uniforms[off++] = cov_y0;
    uniforms[off++] = cov_x1; uniforms[off++] = cov_y1;
    uniforms[off++] = t.anchor_cam_x;
    uniforms[off++] = t.anchor_cam_y;
    uniforms[off++] = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset;
    uniforms[off++] = t.zraw_to_wu * recon.z_scale;
    {
        const uint32_t h = t.obj_id * 1103515245u + 12345u;
        const float r = 0.25f + 0.70f * float((h >>  0) & 0xFFu) / 255.0f;
        const float g = 0.25f + 0.70f * float((h >>  8) & 0xFFu) / 255.0f;
        const float b = 0.25f + 0.70f * float((h >> 16) & 0xFFu) / 255.0f;
        uniforms[off++] = float(perspective_debug_mode);
        uniforms[off++] = r; uniforms[off++] = g; uniforms[off++] = b;
    }
    uniforms[off++] = float(perspective_steps);
    uniforms[off++] = float(perspective_refine);
    // raycast.z is the visible-albedo padding in G-buffer pixels. Depth,
    // scene_z, normals, and ids still render in this border by design.
    uniforms[off++] = float(kGBufPad);
    uniforms[off++] = recon.reserved > 0.5f ? float(perspective_projection_mode) : 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, draw_vertex_count, 1);
}

void TRenderer::DrainOverlayQueue()
{
    if (overlay_queue.empty() || !lit_pass.id) return;
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_LOAD;
    pa.depth.action = SG_ACTION_DONTCARE;
    pa.stencil.action = SG_ACTION_DONTCARE;
    sg_begin_pass(lit_pass, &pa);
    const int32_t gbw = width + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    for (const auto& t : overlay_queue)
    {
        const sg_image image = (t.texture != kInvalidTexture)
            ? TextureImage(t.texture)
            : ImagePairColor(t.image_pair);
        if (!image.id)
            continue;
        Composite(image,
                  t.dst_x + kGBufPad, t.dst_y + kGBufPad,
                  t.dst_w, t.dst_h,
                  gbw, gbh,
                  t.src_x, t.src_y, t.src_w, t.src_h,
                  t.src_tex_w, t.src_tex_h,
                  t.additive_blend,
                  t.chroma_key,
                  t.chroma_key_rgb);
    }
    sg_end_pass();
    overlay_queue.clear();
    lit_target_dirty = true;
}

void TRenderer::EndTilePass()
{
    // Opaque tile fill is a single pipeline. Keep per-tile work to binding the
    // source textures, uploading the small uniform block, and drawing. If this
    // path becomes draw-call bound, the next architectural step is instanced
    // batches grouped by image pair.
    if (!tile_queue.empty() && tile_pipeline.id)
        sg_apply_pipeline(tile_pipeline);
    for (const auto& t : tile_queue)
        EmitTile(t);
    DrainMeshQueue();
    mesh_queue.clear();
    sg_end_pass();
    last_tile_pass_stats = current_tile_pass_stats;
}

// *************************************************************************
// * Lighting State Setters                                                *
// *************************************************************************

void TRenderer::SetLight(float dx, float dy, float dz, float intensity,
                         float r, float g, float b, float ambient)
{
    light.dir[0] = dx; light.dir[1] = dy; light.dir[2] = dz;
    light.shadow_world_dir[0] = dx;
    light.shadow_world_dir[1] = dy;
    light.shadow_world_dir[2] = dz;
    light.intensity   = intensity;
    light.color[0] = r; light.color[1] = g; light.color[2] = b;
    light.ambient     = ambient;
}

void TRenderer::SetAmbientColor(float r, float g, float b)
{
    light.ambient_color[0] = r;
    light.ambient_color[1] = g;
    light.ambient_color[2] = b;
}

void TRenderer::SetLightCeiling(float ceiling)
{
    light.light_ceiling = (ceiling > 0.0f) ? ceiling : 0.0f;
}

void TRenderer::SetAmbientOcclusion(bool enable, float radius_px, float strength,
                                    float bias, float max_dist_wu)
{
    light.ao_enable      = enable;
    light.ao_radius_px   = radius_px;
    light.ao_strength    = strength;
    light.ao_bias        = bias;
    light.ao_max_dist_wu = max_dist_wu;
}

void TRenderer::SetShadowWorldDir(float dx, float dy, float dz)
{
    light.shadow_world_dir[0] = dx;
    light.shadow_world_dir[1] = dy;
    light.shadow_world_dir[2] = dz;
}

void TRenderer::SetShadowVariance(float sx, float sy, float sz)
{
    (void)sx;
    (void)sy;
    light.shadow_dir[0] = sx;
    light.shadow_dir[1] = sy;
    light.shadow_dir[2] = sz;
    light.sun_shadow_wz_scale = std::clamp(sz, 0.05f, 8.0f);
}

void TRenderer::SetTileViewMode(int32_t mode)  { light.view_mode = mode; }
void TRenderer::SetLightingMode(int32_t mode)  { light.mode = mode; }

void TRenderer::SetSunShadow(bool enable, float step_wu, float softness_px,
                             int32_t max_steps)
{
    light.sun_shadow_enable      = enable;
    light.sun_shadow_step_wu     = std::clamp(step_wu, 1.0f, 256.0f);
    light.sun_shadow_softness_px = std::clamp(softness_px, 0.0f, 48.0f);
    light.sun_shadow_max_steps   = std::clamp(max_steps, 4, 512);
}

void TRenderer::SetSunShadowRaycast(int32_t samples, float depth_cutoff_wu,
                                    float bias_wu, float wz_scale)
{
    (void)samples;
    light.sun_shadow_samples       = 1;
    light.sun_shadow_depth_cutoff_wu = std::clamp(depth_cutoff_wu, 1.0f, 128.0f);
    light.sun_shadow_bias_wu       = std::clamp(bias_wu, -10.0f, 10.0f);
    light.sun_shadow_wz_scale      = std::clamp(wz_scale, 0.05f, 8.0f);
}

void TRenderer::SetSunShadowEnabled(bool enable) { light.sun_shadow_enable = enable; }
bool TRenderer::SunShadowEnabled() const         { return light.sun_shadow_enable; }

void TRenderer::SetNormalRadius(float texels)   { light.normal_radius = texels; }
void TRenderer::SetEdgeThreshold(float zraw_units) { light.edge_threshold = zraw_units; }

void TRenderer::SetNormalLightingHardness(float hardness)
{
    if (hardness < 0.0f) hardness = 0.0f;
    if (hardness > 1.0f) hardness = 1.0f;
    light.normal_lighting_hardness = hardness;
}

void TRenderer::ClearPointLights()
{
    light.plight_count = 0;
}

void TRenderer::AddPointLight(float wx, float wy, float wz, float radius_wu,
                              float r, float g, float b, float intensity)
{
    if (light.plight_count >= kMaxPointLights) return;
    const int32_t i = light.plight_count++;
    light.plight_pos[i][0] = wx;
    light.plight_pos[i][1] = wy;
    light.plight_pos[i][2] = wz;
    light.plight_pos[i][3] = radius_wu;
    light.plight_col[i][0] = r;
    light.plight_col[i][1] = g;
    light.plight_col[i][2] = b;
    light.plight_col[i][3] = intensity;
}

// *************************************************************************
// * Composite Submission                                                  *
// *************************************************************************

void TRenderer::Composite(TSurface* src)
{
    if (!src) return;
    const sg_image img = src->GetSGImage();
    if (!img.id || !composite_pip_swap.id) return;

    sg_apply_pipeline(composite_pip_swap);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = img;
    sg_apply_bindings(&bind);

    const float uniforms[12] = {
        -1.0f, -1.0f, 2.0f, 2.0f,
         0.0f,  0.0f, 1.0f, 1.0f,
         0.0f,  0.0f, 0.0f, 0.0f,
    };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::Composite(TTextureHandle texture,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h)
{
    Composite(TextureImage(texture), dst_x, dst_y, dst_w, dst_h, target_w, target_h);
}

void TRenderer::Composite(TTextureHandle texture,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h,
                          int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                          int32_t src_tex_w, int32_t src_tex_h,
                          bool additive_blend,
                          bool chroma_key,
                          const float* chroma_key_rgb)
{
    Composite(TextureImage(texture),
              dst_x, dst_y, dst_w, dst_h,
              target_w, target_h,
              src_x, src_y, src_w, src_h,
              src_tex_w, src_tex_h,
              additive_blend, chroma_key, chroma_key_rgb);
}

void TRenderer::Composite(sg_image img,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h)
{
    Composite(img,
              dst_x, dst_y, dst_w, dst_h,
              target_w, target_h,
              0, 0, 1, 1,
              1, 1);
}

void TRenderer::CompositeSwapchain(sg_image img,
                                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                                   int32_t target_w, int32_t target_h,
                                   int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                                   int32_t src_tex_w, int32_t src_tex_h)
{
    // Same blit math as the RT variant below, but uses the swapchain-
    // format pipeline so it can be called inside sg_begin_default_pass.
    if (!img.id || !composite_pip_swap.id) return;
    if (target_w <= 0 || target_h <= 0) return;
    if (src_tex_w <= 0 || src_tex_h <= 0) return;

    sg_apply_pipeline(composite_pip_swap);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = img;
    sg_apply_bindings(&bind);

    const float nx = (2.0f * dst_x / target_w) - 1.0f;
    const float nw = (2.0f * dst_w) / target_w;
    const float ny = 1.0f - (2.0f * (dst_y + dst_h) / target_h);
    const float nh = (2.0f * dst_h) / target_h;

    const float u0 = float(src_x) / float(src_tex_w);
    const float v0 = float(src_y) / float(src_tex_h);
    const float uw = float(src_w) / float(src_tex_w);
    const float vh = float(src_h) / float(src_tex_h);

    const float uniforms[12] = { nx, ny, nw, nh,  u0, v0, uw, vh,  0.0f, 0.0f, 0.0f, 0.0f };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::Composite(sg_image img,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h,
                          int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                          int32_t src_tex_w, int32_t src_tex_h,
                          bool additive_blend,
                          bool chroma_key,
                          const float* chroma_key_rgb)
{
    const sg_pipeline pip = additive_blend ? composite_pip_add_rt : composite_pip_rt;
    if (!img.id || !pip.id) return;
    if (target_w <= 0 || target_h <= 0) return;
    if (src_tex_w <= 0 || src_tex_h <= 0) return;

    sg_apply_pipeline(pip);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = img;
    sg_apply_bindings(&bind);

    const float nx = (2.0f * dst_x / target_w) - 1.0f;
    const float nw = (2.0f * dst_w) / target_w;
    const float ny = 1.0f - (2.0f * (dst_y + dst_h) / target_h);
    const float nh = (2.0f * dst_h) / target_h;

    const float u0 = float(src_x) / float(src_tex_w);
    const float v0 = float(src_y) / float(src_tex_h);
    const float uw = float(src_w) / float(src_tex_w);
    const float vh = float(src_h) / float(src_tex_h);

    float uniforms[12] = { nx, ny, nw, nh,  u0, v0, uw, vh, 0.0f, 0.0f, 0.0f, 0.0f };
    if (chroma_key && chroma_key_rgb)
    {
        uniforms[8] = 1.0f;
        uniforms[9] = chroma_key_rgb[0];
        uniforms[10] = chroma_key_rgb[1];
        uniforms[11] = chroma_key_rgb[2];
    }
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

// *************************************************************************
// * HUD layer  --  DrawBitmap / DrawSurface / AddHudItem / DrawHud         *
// *                                                                        *
// * Composites cached HUD content on top of the 3D scene during the        *
// * swapchain pass. Called from TDisplay::FlipPage between PresentToSwap-  *
// * chain (Scene3D) and simgui_render (debug UI). Items are sorted by z    *
// * ascending; ties break by registration sequence. See renderer.h API     *
// * and docs/FRAME_PIPELINE.md.                                            *
// *************************************************************************

TTextureHandle TRenderer::BitmapAsTexture(PTBitmap bm)
{
    if (!bm || bm->width <= 0 || bm->height <= 0)
        return kInvalidTexture;

    const uintptr_t key = uintptr_t(bm);
    if (auto it = bitmap_texture_cache.find(key); it != bitmap_texture_cache.end())
        return it->second;

    const int32_t w = bm->width;
    const int32_t h = bm->height;
    const int32_t pitch = w * 4;
    std::vector<uint8_t> rgba(size_t(pitch) * size_t(h), 0);
    if (!DecodeBitmapToRGBA(bm, rgba.data(), pitch, 0, 0))
    {
        log_warn("[renderer] DrawBitmap: DecodeBitmapToRGBA failed for bitmap %p (%dx%d, flags=0x%x)",
                 (void*)bm, w, h, bm->flags);
        bitmap_texture_cache[key] = kInvalidTexture;
        return kInvalidTexture;
    }

    const TTextureHandle tex = RegisterTextureAsset(
        key, rgba.data(), rgba.size(), w, h,
        ERendererTextureFormat::RGBA8,
        uint64_t(rgba.size()),
        ERendererTextureFilter::Nearest);
    bitmap_texture_cache[key] = tex;
    return tex;
}

void TRenderer::DrawBitmap(PTBitmap bm, int32_t x, int32_t y)
{
    if (!bm) return;
    const TTextureHandle tex = BitmapAsTexture(bm);
    if (tex == kInvalidTexture) return;
    const sg_image img = TextureImage(tex);
    if (!img.id) return;
    const int32_t target_w = sapp_width();
    const int32_t target_h = sapp_height();
    CompositeSwapchain(img, x, y, bm->width, bm->height, target_w, target_h,
                       0, 0, bm->width, bm->height,
                       bm->width, bm->height);
}

void TRenderer::DrawBitmapSubrect(PTBitmap bm,
                                  int32_t dst_x, int32_t dst_y,
                                  int32_t src_x, int32_t src_y,
                                  int32_t src_w, int32_t src_h)
{
    if (!bm || src_w <= 0 || src_h <= 0) return;
    const TTextureHandle tex = BitmapAsTexture(bm);
    if (tex == kInvalidTexture) return;
    const sg_image img = TextureImage(tex);
    if (!img.id) return;
    const int32_t target_w = sapp_width();
    const int32_t target_h = sapp_height();
    CompositeSwapchain(img, dst_x, dst_y, src_w, src_h, target_w, target_h,
                       src_x, src_y, src_w, src_h,
                       bm->width, bm->height);
}

void TRenderer::DrawSurface(TSurface* surf, int32_t x, int32_t y)
{
    if (!surf) return;
    const sg_image img = surf->GetSGImage();
    if (!img.id) return;
    const int32_t target_w = sapp_width();
    const int32_t target_h = sapp_height();
    const int32_t sw = surf->Width();
    const int32_t sh = surf->Height();
    CompositeSwapchain(img, x, y, sw, sh, target_w, target_h,
                       0, 0, sw, sh, sw, sh);
}

sg_image TRenderer::GetOrCreateSolidColorImage(uint32_t rgba)
{
    auto it = solid_color_cache.find(rgba);
    if (it != solid_color_cache.end())
        return it->second;

    // RGBA layout: caller passes (r << 24) | (g << 16) | (b << 8) | a.
    // SG_PIXELFORMAT_RGBA8 expects bytes R, G, B, A in that order.
    const uint8_t bytes[4] = {
        uint8_t((rgba >> 24) & 0xff),
        uint8_t((rgba >> 16) & 0xff),
        uint8_t((rgba >>  8) & 0xff),
        uint8_t( rgba        & 0xff),
    };

    sg_image_desc desc{};
    desc.width  = 1;
    desc.height = 1;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0].ptr  = bytes;
    desc.data.subimage[0][0].size = sizeof(bytes);

    const sg_image img = sg_make_image(&desc);
    solid_color_cache[rgba] = img;
    return img;
}

void TRenderer::DrawSolidRect(int32_t x, int32_t y, int32_t w, int32_t h,
                              uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (w <= 0 || h <= 0) return;
    const uint32_t key = (uint32_t(r) << 24) | (uint32_t(g) << 16) |
                         (uint32_t(b) <<  8) |  uint32_t(a);
    const sg_image img = GetOrCreateSolidColorImage(key);
    if (!img.id) return;
    const int32_t target_w = sapp_width();
    const int32_t target_h = sapp_height();
    CompositeSwapchain(img, x, y, w, h, target_w, target_h,
                       0, 0, 1, 1, 1, 1);
}

void TRenderer::DrawNineSlice(PTBitmap bm,
                              int32_t l, int32_t t, int32_t r, int32_t b,
                              int32_t dx, int32_t dy, int32_t dw, int32_t dh)
{
    if (!bm) return;
    if (dw <= 0 || dh <= 0) return;

    const TTextureHandle tex = BitmapAsTexture(bm);
    if (tex == kInvalidTexture) return;
    const sg_image img = TextureImage(tex);
    if (!img.id) return;

    const int32_t sw = bm->width;
    const int32_t sh = bm->height;
    const int32_t target_w = sapp_width();
    const int32_t target_h = sapp_height();

    // Clamp insets to half the source (and ensure non-negative) so any
    // crazy caller input stays sane. Same clamp applies to dest pieces.
    l = (std::max)(0, (std::min)(l, sw / 2));
    r = (std::max)(0, (std::min)(r, sw / 2));
    t = (std::max)(0, (std::min)(t, sh / 2));
    b = (std::max)(0, (std::min)(b, sh / 2));

    // Source columns: [0..l), [l..sw-r), [sw-r..sw)
    // Source rows:    [0..t), [t..sh-b), [sh-b..sh)
    const int32_t sx0 = 0,         sx1 = l,            sx2 = sw - r;
    const int32_t sy0 = 0,         sy1 = t,            sy2 = sh - b;
    const int32_t sw0 = l,         sw1 = sw - l - r,   sw2 = r;
    const int32_t sh0 = t,         sh1 = sh - t - b,   sh2 = b;

    // Dest pieces: corners fixed at their source size; edges/center
    // get the remainder. If target is narrower/shorter than the corners,
    // the middle pieces collapse and the lambda below skips them.
    const int32_t dx0 = dx,        dx1 = dx + l,       dx2 = dx + dw - r;
    const int32_t dy0 = dy,        dy1 = dy + t,       dy2 = dy + dh - b;
    const int32_t dw0 = l,         dw1 = dw - l - r,   dw2 = r;
    const int32_t dh0 = t,         dh1 = dh - t - b,   dh2 = b;

    auto blit = [&](int32_t bdx, int32_t bdy, int32_t bdw, int32_t bdh,
                    int32_t bsx, int32_t bsy, int32_t bsw, int32_t bsh)
    {
        if (bdw <= 0 || bdh <= 0 || bsw <= 0 || bsh <= 0)
            return;
        CompositeSwapchain(img, bdx, bdy, bdw, bdh, target_w, target_h,
                           bsx, bsy, bsw, bsh, sw, sh);
    };

    blit(dx0, dy0, dw0, dh0, sx0, sy0, sw0, sh0);  // TL corner
    blit(dx1, dy0, dw1, dh0, sx1, sy0, sw1, sh0);  // top edge
    blit(dx2, dy0, dw2, dh0, sx2, sy0, sw2, sh0);  // TR corner
    blit(dx0, dy1, dw0, dh1, sx0, sy1, sw0, sh1);  // left edge
    blit(dx1, dy1, dw1, dh1, sx1, sy1, sw1, sh1);  // center
    blit(dx2, dy1, dw2, dh1, sx2, sy1, sw2, sh1);  // right edge
    blit(dx0, dy2, dw0, dh2, sx0, sy2, sw0, sh2);  // BL corner
    blit(dx1, dy2, dw1, dh2, sx1, sy2, sw1, sh2);  // bottom edge
    blit(dx2, dy2, dw2, dh2, sx2, sy2, sw2, sh2);  // BR corner
}

void TRenderer::AddHud(THudDrawable* d, float z)
{
    if (!d) return;
    // Defensive: avoid duplicate registration (same drawable twice
    // would draw itself twice each frame, almost always a caller bug).
    auto it = std::find_if(hud_drawables.begin(), hud_drawables.end(),
                           [&](const SHudRegistration& r){ return r.drawable == d; });
    if (it != hud_drawables.end())
    {
        it->z = z;  // Re-AddHud with new z is allowed; updates in place.
        return;
    }
    hud_drawables.push_back({ d, z });
}

void TRenderer::RemoveHud(THudDrawable* d)
{
    if (!d) return;
    auto it = std::find_if(hud_drawables.begin(), hud_drawables.end(),
                           [&](const SHudRegistration& r){ return r.drawable == d; });
    if (it != hud_drawables.end())
        hud_drawables.erase(it);
}

void TRenderer::SetHudZ(THudDrawable* d, float z)
{
    if (!d) return;
    auto it = std::find_if(hud_drawables.begin(), hud_drawables.end(),
                           [&](const SHudRegistration& r){ return r.drawable == d; });
    if (it != hud_drawables.end())
        it->z = z;
}

void TRenderer::DrawHud()
{
    if (hud_drawables.empty()) return;
    // Stable sort by z ascending; ties keep insertion order. List is
    // tiny (cursor + a few HUD panels in practice) so the sort is
    // effectively free.
    std::stable_sort(hud_drawables.begin(), hud_drawables.end(),
                     [](const SHudRegistration& a, const SHudRegistration& b) {
                         return a.z < b.z;
                     });
    for (const SHudRegistration& r : hud_drawables)
        if (r.drawable) r.drawable->Draw();
}

// *************************************************************************
// * Present to Swapchain  --  Pass [7]                                    *
// *************************************************************************

bool TRenderer::PresentToSwapchain()
{
    // Caller (TDisplay::FlipPage) has already opened sokol's default pass
    // for this frame. Nothing to do if neither the tile pass nor lighting
    // pass wrote anything; let FlipPage fall back to the backbuffer.
    if (!lit_target_dirty && !color_target_dirty)
        return false;

    // Editor mode opts out: the game image is composited inside the
    // ImGui Game View panel via ImGui::Image of lit_target. Just mark
    // the targets clean so the next frame's pass actions don't fire.
    if (suppress_present) {
        color_target_dirty = false;
        lit_target_dirty   = false;
        return false;
    }

    sg_apply_pipeline(composite_pip_swap);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = lit_target_dirty ? lit_target : color_target;
    sg_apply_bindings(&bind);

    // Sample only the centered display-sized sub-rect of the padded
    // G-buffer / lit target. See kGBufPad.
    const int32_t pad = kGBufPad;
    const float   gbw = float(width  + 2 * pad);
    const float   gbh = float(height + 2 * pad);
    const float   u0  = float(pad)   / gbw;
    const float   v0  = float(pad)   / gbh;
    const float   uw  = float(width) / gbw;
    const float   vh  = float(height) / gbh;
    const float u[12] = {
        present_ndc[0], present_ndc[1], present_ndc[2], present_ndc[3],
        u0, v0, uw, vh,
        0.0f, 0.0f, 0.0f, 0.0f,
    };
    const sg_range ur = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &ur);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &ur);
    sg_draw(0, 6, 1);

    color_target_dirty = false;
    lit_target_dirty   = false;
    return true;
}

void TRenderer::SetPresentNDCRect(float x, float y, float w, float h)
{
    present_ndc[0] = x;
    present_ndc[1] = y;
    present_ndc[2] = w;
    present_ndc[3] = h;
}

// *************************************************************************
// * FX pipelines (Phase 1 VFX spine)                                       *
// *************************************************************************
//
// Three sokol pipelines + one fx_pass. All three share the same iso
// projection as the tile / mesh pipelines (uniform block layout vp/camz/
// camw); we resolve recon state at drain time so producers don't have to
// know about it.
//
//   fx_billboard : per-instance attributes (location 0 = corner unit
//                  quad, 1 = world_pos, 2 = size_wu, 3 = uv_rect, 4 =
//                  color, 5 = debug_mode).
//   fx_particle  : same plus per-instance rotation_rad (location 6).
//   fx_strip     : CPU expands segments into 4-vertex quads; per-vertex
//                  world_pos, world_tan, half_width, uv, color,
//                  debug_mode.
//
// One shared dynamic instance VB per pipeline, rebuilt per frame. Per-
// bucket sort is back-to-front in camera-forward space.
//
// *************************************************************************

namespace {

// Instance / vertex strides. Per PHASE1_SPINE.md §6 light_mode and
// orientation share the debug_mode lane convention (per-instance float,
// no pipeline variant) -- we add them as more floats per instance.
//   billboard : wp(3) + sz(2) + uv(4) + col(4) + dbg(1) + light(1) +
//               orient(1) = 16 floats (the original pad lane absorbs
//               orientation; stays 64B)
//   particle  : same + rotation_rad + orientation = 18 floats; pad to
//               20 to keep alignment (and to retain headroom for the
//               next per-instance lane).
//   strip     : wp(3) + wt(3) + hw(1) + uv(2) + col(4) + dbg(1) + light(1) + pad(1) = 16 floats (stays 64B)
constexpr int32_t kFxBillboardInstanceFloats = 16;
constexpr int32_t kFxParticleInstanceFloats  = 20;
constexpr int32_t kFxStripVertexFloats       = 16;

// Static corner quad for billboards / particles. CCW with z=0.
constexpr float kFxCorners[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
    -0.5f,  0.5f,
     0.5f,  0.5f,
};

void SetupFxBlend(sg_pipeline_desc& pip, EFxBlend blend)
{
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    switch (blend)
    {
        case EFxBlend::Additive:
            pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
            pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ZERO;
            pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
            break;
        case EFxBlend::AdditiveStraight:
            // Retail TBloodSystem second pass: D3DBLEND_ONE / D3DBLEND_ONE
            // on both rgb and alpha. Particle color is added at full
            // weight regardless of alpha -- pairs with LitFlat for the
            // "self-lit glow over the lit alpha base" effect.
            pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
            break;
        case EFxBlend::PremulAlpha:
            pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case EFxBlend::Alpha:
        default:
            pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
            pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
            pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            break;
    }
}

// Translate EFxDepthMode into sokol pipeline depth state. Depth format
// is fixed to the G-buffer depth attachment so the fx pass can read
// scene depth without a separate depth target.
void SetupFxDepth(sg_pipeline_desc& pip, EFxDepthMode depth_mode)
{
    pip.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
    switch (depth_mode)
    {
        case EFxDepthMode::TestWrite:
            pip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
            pip.depth.write_enabled = true;
            break;
        case EFxDepthMode::None:
            pip.depth.compare       = SG_COMPAREFUNC_ALWAYS;
            pip.depth.write_enabled = false;
            break;
        case EFxDepthMode::TestNoWrite:
        default:
            pip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
            pip.depth.write_enabled = false;
            break;
    }
}

// Shared FX VS uniform block (vp / camz / camw / sun_dir / sun_color /
// ambient_color). The first three vec4s match the pre-Phase-1.5 layout
// so iso projection in the VS is unchanged; the three new vec4s carry
// lighting state forward to the FS for LitFlat modulation.
//
// Layout (24 floats / 6 vec4s):
//   [ 0.. 3] vp     .xy = camera pixel origin, .zw = padded G-buffer dims
//   [ 4.. 7] camz   .x  = z_near wu, .y = zspan wu, .z = kcam_forward,
//                   .w  = perspective enable (0/1)
//   [ 8..11] camw   .xy = camera-center world xy, .z = zoom, .w unused
//   [12..15] sun_dir.xyz = world-space direction from surface toward sun
//                          (same convention as light_dir in the deferred
//                          light shader: dot(N, sun_dir) > 0 == lit).
//                   .w = 0 (reserved)
//   [16..19] sun_col.rgb = sun color (pre-multiplied by intensity), .a = 0
//   [20..23] amb_col.rgb = ambient color (pre-multiplied by ambient), .a = 0
void FillFxUniforms(float u[24], const TRenderer& /*r*/,
                    float ox, float oy, float gbw, float gbh,
                    float z_near, float zspan, float kcam_forward,
                    float perspective, float center_wx, float center_wy,
                    float zoom,
                    const float sun_dir[3],   const float sun_col[3],
                    const float amb_col[3])
{
    u[0]  = ox;
    u[1]  = oy;
    u[2]  = gbw;
    u[3]  = gbh;
    u[4]  = z_near;
    u[5]  = zspan;
    u[6]  = kcam_forward;
    u[7]  = perspective;
    u[8]  = center_wx;
    u[9]  = center_wy;
    u[10] = zoom;
    u[11] = 0.0f;
    u[12] = sun_dir[0]; u[13] = sun_dir[1]; u[14] = sun_dir[2]; u[15] = 0.0f;
    u[16] = sun_col[0]; u[17] = sun_col[1]; u[18] = sun_col[2]; u[19] = 0.0f;
    u[20] = amb_col[0]; u[21] = amb_col[1]; u[22] = amb_col[2]; u[23] = 0.0f;
}

}  // namespace

namespace {

// Shared VS uniform block descriptor for fx pipelines. Three vec4s of
// camera (vp/camz/camw) plus three vec4s of lighting state (sun_dir /
// sun_color / ambient_color). Lighting fields are forwarded through to
// the FS via varyings -- the FX shader is small enough that this beats
// a separate FS UBO.
void FillFxSharedVsUbo(sg_shader_uniform_block_desc& ub)
{
    ub.size = 6 * sizeof(float) * 4;
    ub.uniforms[0].name = "vp";            ub.uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    ub.uniforms[1].name = "camz";          ub.uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    ub.uniforms[2].name = "camw";          ub.uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    ub.uniforms[3].name = "sun_dir";       ub.uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    ub.uniforms[4].name = "sun_color";     ub.uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    ub.uniforms[5].name = "ambient_color"; ub.uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
}

}  // namespace

void TRenderer::InitFxPipeline()
{
    // ---- Shared corner VB (4 vertices, position-only) -------------------
    sg_buffer_desc cb = {};
    cb.size  = int(sizeof(kFxCorners));
    cb.data  = SG_RANGE(kFxCorners);
    cb.label = "renderer.fx.corner_vb";
    fx_corner_vb = sg_make_buffer(&cb);

    // ---- fx_billboard shader + pipelines --------------------------------
    {
        sg_shader_desc sh = {};
        sh.attrs[0].name = "corner";      sh.attrs[0].sem_name = "TEXCOORD"; sh.attrs[0].sem_index = 0;
        sh.attrs[1].name = "world_pos";   sh.attrs[1].sem_name = "TEXCOORD"; sh.attrs[1].sem_index = 1;
        sh.attrs[2].name = "size_wu";     sh.attrs[2].sem_name = "TEXCOORD"; sh.attrs[2].sem_index = 2;
        sh.attrs[3].name = "uv_rect";     sh.attrs[3].sem_name = "TEXCOORD"; sh.attrs[3].sem_index = 3;
        sh.attrs[4].name = "color_rgba";  sh.attrs[4].sem_name = "TEXCOORD"; sh.attrs[4].sem_index = 4;
        sh.attrs[5].name = "debug_mode";  sh.attrs[5].sem_name = "TEXCOORD"; sh.attrs[5].sem_index = 5;
        sh.attrs[6].name = "light_mode";  sh.attrs[6].sem_name = "TEXCOORD"; sh.attrs[6].sem_index = 6;
        sh.attrs[7].name = "orientation"; sh.attrs[7].sem_name = "TEXCOORD"; sh.attrs[7].sem_index = 7;
        sh.vs.source = kFxBillboardVs;
        sh.vs.entry  = kShaderVsEntry;
        FillFxSharedVsUbo(sh.vs.uniform_blocks[0]);
        sh.fs.source = kFxBillboardFs;
        sh.fs.entry  = kShaderFsEntry;
        sh.fs.images[0].name = "atlas";
        sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
        sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
        sh.label = "renderer.fx.billboard.shader";
        fx_billboard_shader = sg_make_shader(&sh);

        sg_pipeline_desc pip = {};
        pip.shader = fx_billboard_shader;
        pip.layout.buffers[0].stride    = 2 * sizeof(float);
        pip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
        pip.layout.buffers[1].stride    = kFxBillboardInstanceFloats * sizeof(float);
        pip.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
        pip.layout.attrs[0].buffer_index = 0; pip.layout.attrs[0].offset = 0;                          pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
        pip.layout.attrs[1].buffer_index = 1; pip.layout.attrs[1].offset = 0;                          pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
        pip.layout.attrs[2].buffer_index = 1; pip.layout.attrs[2].offset = 3  * sizeof(float);         pip.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
        pip.layout.attrs[3].buffer_index = 1; pip.layout.attrs[3].offset = 5  * sizeof(float);         pip.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT4;
        pip.layout.attrs[4].buffer_index = 1; pip.layout.attrs[4].offset = 9  * sizeof(float);         pip.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4;
        pip.layout.attrs[5].buffer_index = 1; pip.layout.attrs[5].offset = 13 * sizeof(float);         pip.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[6].buffer_index = 1; pip.layout.attrs[6].offset = 14 * sizeof(float);         pip.layout.attrs[6].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[7].buffer_index = 1; pip.layout.attrs[7].offset = 15 * sizeof(float);         pip.layout.attrs[7].format = SG_VERTEXFORMAT_FLOAT;
        pip.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
        pip.cull_mode      = SG_CULLMODE_NONE;
        // Build all (blend, depth_mode) variants. Light mode and
        // orientation are per-instance so they don't multiply the
        // pipeline count.
        for (int32_t b = 0; b < kFxBlendCount; ++b)
        {
            SetupFxBlend(pip, EFxBlend(b));
            for (int32_t d = 0; d < kFxDepthModeCount; ++d)
            {
                SetupFxDepth(pip, EFxDepthMode(d));
                pip.label = "renderer.fx.billboard.pip";
                fx_billboard_pip[b][d] = sg_make_pipeline(&pip);
            }
        }
    }

    // ---- fx_particle shader + pipelines ---------------------------------
    {
        sg_shader_desc sh = {};
        sh.attrs[0].name = "corner";       sh.attrs[0].sem_name = "TEXCOORD"; sh.attrs[0].sem_index = 0;
        sh.attrs[1].name = "world_pos";    sh.attrs[1].sem_name = "TEXCOORD"; sh.attrs[1].sem_index = 1;
        sh.attrs[2].name = "size_wu";      sh.attrs[2].sem_name = "TEXCOORD"; sh.attrs[2].sem_index = 2;
        sh.attrs[3].name = "uv_rect";      sh.attrs[3].sem_name = "TEXCOORD"; sh.attrs[3].sem_index = 3;
        sh.attrs[4].name = "color_rgba";   sh.attrs[4].sem_name = "TEXCOORD"; sh.attrs[4].sem_index = 4;
        sh.attrs[5].name = "debug_mode";   sh.attrs[5].sem_name = "TEXCOORD"; sh.attrs[5].sem_index = 5;
        sh.attrs[6].name = "rotation_rad"; sh.attrs[6].sem_name = "TEXCOORD"; sh.attrs[6].sem_index = 6;
        sh.attrs[7].name = "light_mode";   sh.attrs[7].sem_name = "TEXCOORD"; sh.attrs[7].sem_index = 7;
        sh.attrs[8].name = "orientation";  sh.attrs[8].sem_name = "TEXCOORD"; sh.attrs[8].sem_index = 8;
        sh.vs.source = kFxParticleVs;
        sh.vs.entry  = kShaderVsEntry;
        FillFxSharedVsUbo(sh.vs.uniform_blocks[0]);
        sh.fs.source = kFxParticleFs;
        sh.fs.entry  = kShaderFsEntry;
        sh.fs.images[0].name = "atlas";
        sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
        sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
        sh.label = "renderer.fx.particle.shader";
        fx_particle_shader = sg_make_shader(&sh);

        sg_pipeline_desc pip = {};
        pip.shader = fx_particle_shader;
        pip.layout.buffers[0].stride    = 2 * sizeof(float);
        pip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
        pip.layout.buffers[1].stride    = kFxParticleInstanceFloats * sizeof(float);
        pip.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
        pip.layout.attrs[0].buffer_index = 0; pip.layout.attrs[0].offset = 0;                          pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
        pip.layout.attrs[1].buffer_index = 1; pip.layout.attrs[1].offset = 0;                          pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
        pip.layout.attrs[2].buffer_index = 1; pip.layout.attrs[2].offset = 3  * sizeof(float);         pip.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
        pip.layout.attrs[3].buffer_index = 1; pip.layout.attrs[3].offset = 5  * sizeof(float);         pip.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT4;
        pip.layout.attrs[4].buffer_index = 1; pip.layout.attrs[4].offset = 9  * sizeof(float);         pip.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4;
        pip.layout.attrs[5].buffer_index = 1; pip.layout.attrs[5].offset = 13 * sizeof(float);         pip.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[6].buffer_index = 1; pip.layout.attrs[6].offset = 14 * sizeof(float);         pip.layout.attrs[6].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[7].buffer_index = 1; pip.layout.attrs[7].offset = 15 * sizeof(float);         pip.layout.attrs[7].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[8].buffer_index = 1; pip.layout.attrs[8].offset = 16 * sizeof(float);         pip.layout.attrs[8].format = SG_VERTEXFORMAT_FLOAT;
        pip.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
        pip.cull_mode      = SG_CULLMODE_NONE;
        for (int32_t b = 0; b < kFxBlendCount; ++b)
        {
            SetupFxBlend(pip, EFxBlend(b));
            for (int32_t d = 0; d < kFxDepthModeCount; ++d)
            {
                SetupFxDepth(pip, EFxDepthMode(d));
                pip.label = "renderer.fx.particle.pip";
                fx_particle_pip[b][d] = sg_make_pipeline(&pip);
            }
        }
    }

    // ---- fx_strip shader + pipelines ------------------------------------
    {
        sg_shader_desc sh = {};
        sh.attrs[0].name = "world_pos";   sh.attrs[0].sem_name = "TEXCOORD"; sh.attrs[0].sem_index = 0;
        sh.attrs[1].name = "world_tan";   sh.attrs[1].sem_name = "TEXCOORD"; sh.attrs[1].sem_index = 1;
        sh.attrs[2].name = "half_width";  sh.attrs[2].sem_name = "TEXCOORD"; sh.attrs[2].sem_index = 2;
        sh.attrs[3].name = "uv";          sh.attrs[3].sem_name = "TEXCOORD"; sh.attrs[3].sem_index = 3;
        sh.attrs[4].name = "color";       sh.attrs[4].sem_name = "TEXCOORD"; sh.attrs[4].sem_index = 4;
        sh.attrs[5].name = "debug_mode";  sh.attrs[5].sem_name = "TEXCOORD"; sh.attrs[5].sem_index = 5;
        sh.attrs[6].name = "light_mode";  sh.attrs[6].sem_name = "TEXCOORD"; sh.attrs[6].sem_index = 6;
        sh.vs.source = kFxStripVs;
        sh.vs.entry  = kShaderVsEntry;
        FillFxSharedVsUbo(sh.vs.uniform_blocks[0]);
        sh.vs.uniform_blocks[1].size = 2 * sizeof(float) * 4;
        sh.vs.uniform_blocks[1].uniforms[0].name = "cam_pos"; sh.vs.uniform_blocks[1].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
        sh.vs.uniform_blocks[1].uniforms[1].name = "cam_fwd"; sh.vs.uniform_blocks[1].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
        sh.fs.source = kFxStripFs;
        sh.fs.entry  = kShaderFsEntry;
        sh.fs.images[0].name = "atlas";
        sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
        sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
        sh.label = "renderer.fx.strip.shader";
        fx_strip_shader = sg_make_shader(&sh);

        sg_pipeline_desc pip = {};
        pip.shader = fx_strip_shader;
        pip.layout.buffers[0].stride    = kFxStripVertexFloats * sizeof(float);
        pip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
        pip.layout.attrs[0].buffer_index = 0; pip.layout.attrs[0].offset = 0;                  pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
        pip.layout.attrs[1].buffer_index = 0; pip.layout.attrs[1].offset = 3  * sizeof(float); pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
        pip.layout.attrs[2].buffer_index = 0; pip.layout.attrs[2].offset = 6  * sizeof(float); pip.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[3].buffer_index = 0; pip.layout.attrs[3].offset = 7  * sizeof(float); pip.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT2;
        pip.layout.attrs[4].buffer_index = 0; pip.layout.attrs[4].offset = 9  * sizeof(float); pip.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4;
        pip.layout.attrs[5].buffer_index = 0; pip.layout.attrs[5].offset = 13 * sizeof(float); pip.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT;
        pip.layout.attrs[6].buffer_index = 0; pip.layout.attrs[6].offset = 14 * sizeof(float); pip.layout.attrs[6].format = SG_VERTEXFORMAT_FLOAT;
        pip.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
        pip.cull_mode      = SG_CULLMODE_NONE;
        for (int32_t b = 0; b < kFxBlendCount; ++b)
        {
            SetupFxBlend(pip, EFxBlend(b));
            for (int32_t d = 0; d < kFxDepthModeCount; ++d)
            {
                SetupFxDepth(pip, EFxDepthMode(d));
                pip.label = "renderer.fx.strip.pip";
                fx_strip_pip[b][d] = sg_make_pipeline(&pip);
            }
        }
    }

    // ---- dynamic instance / strip VBs -----------------------------------
    sg_buffer_desc ib = {};
    ib.size  = kMaxFxInstances * kFxBillboardInstanceFloats * int(sizeof(float));
    ib.usage = SG_USAGE_STREAM;
    ib.label = "renderer.fx.billboard.ivb";
    fx_billboard_ivb = sg_make_buffer(&ib);

    sg_buffer_desc pb = {};
    pb.size  = kMaxFxInstances * kFxParticleInstanceFloats * int(sizeof(float));
    pb.usage = SG_USAGE_STREAM;
    pb.label = "renderer.fx.particle.ivb";
    fx_particle_ivb = sg_make_buffer(&pb);

    sg_buffer_desc sb = {};
    sb.size  = kMaxFxStripVerts * kFxStripVertexFloats * int(sizeof(float));
    sb.usage = SG_USAGE_STREAM;
    sb.label = "renderer.fx.strip.vb";
    fx_strip_vb = sg_make_buffer(&sb);

    log_info("[fx-pipeline] init blend=%d depth=%d variants=%d shaders=3",
             int(kFxBlendCount), int(kFxDepthModeCount),
             int(kFxBlendCount * kFxDepthModeCount * 3));
}

void TRenderer::ShutdownFxPipeline()
{
    for (int32_t b = 0; b < kFxBlendCount; ++b)
    {
        for (int32_t d = 0; d < kFxDepthModeCount; ++d)
        {
            if (fx_billboard_pip[b][d].id) { sg_destroy_pipeline(fx_billboard_pip[b][d]); fx_billboard_pip[b][d] = {}; }
            if (fx_particle_pip[b][d].id)  { sg_destroy_pipeline(fx_particle_pip[b][d]);  fx_particle_pip[b][d]  = {}; }
            if (fx_strip_pip[b][d].id)     { sg_destroy_pipeline(fx_strip_pip[b][d]);     fx_strip_pip[b][d]     = {}; }
        }
    }
    if (fx_billboard_shader.id)    { sg_destroy_shader(fx_billboard_shader);      fx_billboard_shader    = {}; }
    if (fx_particle_shader.id)     { sg_destroy_shader(fx_particle_shader);       fx_particle_shader     = {}; }
    if (fx_strip_shader.id)        { sg_destroy_shader(fx_strip_shader);          fx_strip_shader        = {}; }
    if (fx_corner_vb.id)           { sg_destroy_buffer(fx_corner_vb);             fx_corner_vb           = {}; }
    if (fx_billboard_ivb.id)       { sg_destroy_buffer(fx_billboard_ivb);         fx_billboard_ivb       = {}; }
    if (fx_particle_ivb.id)        { sg_destroy_buffer(fx_particle_ivb);          fx_particle_ivb        = {}; }
    if (fx_strip_vb.id)            { sg_destroy_buffer(fx_strip_vb);              fx_strip_vb            = {}; }
}

void TRenderer::SetFxCamera(const float right_wu[3], const float up_wu[3],
                            const float forward_wu[3], const float pos_wu[3])
{
    if (right_wu)   { fx_camera.right[0] = right_wu[0]; fx_camera.right[1] = right_wu[1]; fx_camera.right[2] = right_wu[2]; }
    if (up_wu)      { fx_camera.up[0]    = up_wu[0];    fx_camera.up[1]    = up_wu[1];    fx_camera.up[2]    = up_wu[2]; }
    if (forward_wu) { fx_camera.forward[0] = forward_wu[0]; fx_camera.forward[1] = forward_wu[1]; fx_camera.forward[2] = forward_wu[2]; }
    if (pos_wu)     { fx_camera.pos[0] = pos_wu[0]; fx_camera.pos[1] = pos_wu[1]; fx_camera.pos[2] = pos_wu[2]; }
    fx_camera.set = true;
}

void TRenderer::SubmitFxBillboard(const SBillboardDrawItem& item)
{
    if (int32_t(fx_billboard_queue.size()) >= kMaxFxInstances) return;
    SFxBillboardQueueEntry e{};
    e.item = item;
    if (e.item.key.texture == kInvalidTexture && white_texture != kInvalidTexture)
        e.item.key.texture = white_texture;
    // Sort along camera forward: greater distance = further away = draw first.
    if (fx_camera.set)
    {
        const float dx = item.world_pos[0] - fx_camera.pos[0];
        const float dy = item.world_pos[1] - fx_camera.pos[1];
        const float dz = item.world_pos[2] - fx_camera.pos[2];
        e.sort_z = dx * fx_camera.forward[0] + dy * fx_camera.forward[1] + dz * fx_camera.forward[2];
    }
    fx_billboard_queue.push_back(e);
}

void TRenderer::SubmitFxParticle(const SParticleDrawItem& item)
{
    if (int32_t(fx_particle_queue.size()) >= kMaxFxInstances) return;
    SFxParticleQueueEntry e{};
    e.item = item;
    if (e.item.key.texture == kInvalidTexture && white_texture != kInvalidTexture)
        e.item.key.texture = white_texture;
    if (fx_camera.set)
    {
        const float dx = item.world_pos[0] - fx_camera.pos[0];
        const float dy = item.world_pos[1] - fx_camera.pos[1];
        const float dz = item.world_pos[2] - fx_camera.pos[2];
        e.sort_z = dx * fx_camera.forward[0] + dy * fx_camera.forward[1] + dz * fx_camera.forward[2];
    }
    fx_particle_queue.push_back(e);
}

void TRenderer::SubmitFxParticleBucket(const TParticleBucket& bucket,
                                       EFxDebugMode debug_mode)
{
    const SParticleBucketDesc& desc = bucket.Desc();
    if (!bucket.Active() || desc.texture == kInvalidTexture)
        return;
    SFxBatchKey key = {};
    key.texture     = desc.texture;
    key.pipeline_id = uint16_t(EFxPipeline::Particle);
    // Map per-bucket EParticleBlendMode -> per-pipeline EFxBlend.
    // EParticleBlendMode enumerators line up 1:1 with EFxBlend (both
    // declare the same set in the same order); a direct cast is the
    // documented contract.
    switch (desc.blend)
    {
        case EParticleBlendMode::Alpha:            key.blend = uint8_t(EFxBlend::Alpha);            break;
        case EParticleBlendMode::Additive:         key.blend = uint8_t(EFxBlend::Additive);         break;
        case EParticleBlendMode::AdditiveStraight: key.blend = uint8_t(EFxBlend::AdditiveStraight); break;
        case EParticleBlendMode::PremulAlpha:      key.blend = uint8_t(EFxBlend::PremulAlpha);      break;
    }
    switch (desc.depth_mode)
    {
        case EParticleDepthMode::TestNoWrite: key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite); break;
        case EParticleDepthMode::TestWrite:   key.depth_mode = uint8_t(EFxDepthMode::TestWrite);   break;
        case EParticleDepthMode::None:        key.depth_mode = uint8_t(EFxDepthMode::None);        break;
    }
    const EFxLightMode light_mode = (desc.light_mode == EParticleLightMode::LitFlat)
                                      ? EFxLightMode::LitFlat
                                      : EFxLightMode::Unlit;
    const EFxBillboardOrientation orientation = (desc.orientation == EParticleOrientation::WorldXY)
                                                  ? EFxBillboardOrientation::WorldXY
                                                  : EFxBillboardOrientation::ScreenAligned;

    const int32_t count = bucket.Count();
    for (int32_t i = 0; i < count; ++i)
    {
        const float* dp = bucket.VarPtr(i, EParticleVar::DrawPos);
        if (!dp) continue;
        SParticleDrawItem item = {};
        item.world_pos[0] = dp[0];
        item.world_pos[1] = dp[1];
        item.world_pos[2] = dp[2];

        item.size_wu[0] = desc.default_width;
        item.size_wu[1] = desc.default_height;
        if (const float* ds = bucket.VarPtr(i, EParticleVar::DrawScl))
        {
            item.size_wu[0] = ds[0];
            item.size_wu[1] = ds[1];
        }

        item.color_rgba[0] = 1.0f;
        item.color_rgba[1] = 1.0f;
        item.color_rgba[2] = 1.0f;
        item.color_rgba[3] = 1.0f;
        if (const float* dc = bucket.VarPtr(i, EParticleVar::DrawColor))
        {
            item.color_rgba[0] = dc[0];
            item.color_rgba[1] = dc[1];
            item.color_rgba[2] = dc[2];
            item.color_rgba[3] = dc[3];
        }

        // UV priority: explicit DrawUvRect (per-particle override) >
        // sprite-grid pick from DrawFrame > whole texture.
        item.uv_rect[0] = 0.0f;
        item.uv_rect[1] = 0.0f;
        item.uv_rect[2] = 1.0f;
        item.uv_rect[3] = 1.0f;
        if (const float* du = bucket.VarPtr(i, EParticleVar::DrawUvRect))
        {
            item.uv_rect[0] = du[0];
            item.uv_rect[1] = du[1];
            item.uv_rect[2] = du[2];
            item.uv_rect[3] = du[3];
        }
        else if (desc.frame_cols > 1 || desc.frame_rows > 1)
        {
            const int32_t cols   = desc.frame_cols > 0 ? desc.frame_cols : 1;
            const int32_t rows   = desc.frame_rows > 0 ? desc.frame_rows : 1;
            const int32_t cells  = cols * rows;
            int32_t frame = 0;
            if (const float* df = bucket.VarPtr(i, EParticleVar::DrawFrame))
                frame = int32_t(*df);
            frame = ((frame % cells) + cells) % cells;
            const int32_t fx = frame % cols;
            const int32_t fy = frame / cols;
            const float cw = 1.0f / float(cols);
            const float ch = 1.0f / float(rows);
            item.uv_rect[0] = float(fx) * cw;
            item.uv_rect[1] = float(fy) * ch;
            item.uv_rect[2] = cw;
            item.uv_rect[3] = ch;
        }

        if (const float* dr = bucket.VarPtr(i, EParticleVar::DrawRot))
            item.rotation_rad = dr[0];

        item.key         = key;
        item.debug_mode  = debug_mode;
        item.light_mode  = light_mode;
        item.orientation = orientation;
        SubmitFxParticle(item);
    }
}

void TRenderer::SubmitFxStrip(const SStripDrawItem& item)
{
    if (item.num_segments <= 0 || !item.segments) return;
    SFxStripQueueEntry e{};
    e.key        = item.key;
    if (e.key.texture == kInvalidTexture && white_texture != kInvalidTexture)
        e.key.texture = white_texture;
    e.key.pipeline_id = uint16_t(EFxPipeline::Strip);
    e.debug_mode = item.debug_mode;
    e.light_mode = item.light_mode;
    e.segments.assign(item.segments, item.segments + item.num_segments);
    if (fx_camera.set && item.num_segments > 0)
    {
        const SStripSegment& s = item.segments[0];
        const float mx = 0.5f * (s.world_a[0] + s.world_b[0]) - fx_camera.pos[0];
        const float my = 0.5f * (s.world_a[1] + s.world_b[1]) - fx_camera.pos[1];
        const float mz = 0.5f * (s.world_a[2] + s.world_b[2]) - fx_camera.pos[2];
        e.sort_z = mx * fx_camera.forward[0] + my * fx_camera.forward[1] + mz * fx_camera.forward[2];
    }
    fx_strip_queue.push_back(std::move(e));
}

void TRenderer::DrainFxQueue()
{
    const bool empty = fx_billboard_queue.empty()
                    && fx_particle_queue.empty()
                    && fx_strip_queue.empty();
    if (empty || !fx_pass.id) return;

    // If no consumer set the FX camera basis this frame, derive one from
    // the deferred reconstruction state. The iso camera looks "down" the
    // sum-of-xy axis with a kcam_forward offset; this is enough for the
    // strip pipeline's side = cross(tangent, view_dir) and the back-to-
    // front sort in our submit helpers. Producers may override with
    // SetFxCamera.
    if (!fx_camera.set)
    {
        constexpr float kIsoCos30 = 0.867f;
        const float fwd[3]   = { -kIsoCos30, -kIsoCos30, -0.5f };
        const float up[3]    = {  0.0f,        0.0f,      1.0f };
        const float right[3] = {  0.707f,    -0.707f,     0.0f };
        const float pos[3]   = {
            recon.center_wx + kIsoCos30 * recon.kcam_forward,
            recon.center_wy + kIsoCos30 * recon.kcam_forward,
            0.5f * recon.kcam_forward,
        };
        SetFxCamera(right, up, fwd, pos);
    }

    // Begin the fx pass: load lit color so we blend over it, load depth
    // for read-only test.
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_LOAD;
    pa.depth.action     = SG_ACTION_LOAD;
    pa.stencil.action   = SG_ACTION_DONTCARE;
    sg_begin_pass(fx_pass, &pa);

    // Shared VS uniform block (vp/camz/camw + sun_dir/sun_color/
    // ambient_color). The lighting fields drive the LitFlat per-instance
    // path in the FS; they read directly from the renderer's SLightState
    // so anything the deferred light shader sees is also visible to FX.
    //
    // Convention matches the deferred light shader (light.metal.h): light.dir
    // is the direction from the surface toward the sun -- dot(N, dir) > 0
    // is "lit". Particles use world-up (0,0,1) as their implicit normal
    // (see EFxLightMode::LitFlat) so the FS reduces this to max(0, dir.z).
    //
    // sun_color and ambient_color are pre-multiplied by intensity / ambient
    // here so the FS can stay a single MUL+ADD per fragment.
    const float sun_dir[3] = { light.dir[0], light.dir[1], light.dir[2] };
    const float sun_col[3] = {
        light.color[0] * light.intensity,
        light.color[1] * light.intensity,
        light.color[2] * light.intensity,
    };
    const float amb_col[3] = {
        light.ambient_color[0] * light.ambient,
        light.ambient_color[1] * light.ambient,
        light.ambient_color[2] * light.ambient,
    };
    float u[24] = {};
    FillFxUniforms(u, *this,
                   recon.ox, recon.oy,
                   float(width + 2 * kGBufPad), float(height + 2 * kGBufPad),
                   recon.z_near, recon.zspan, recon.kcam_forward, recon.reserved,
                   recon.center_wx, recon.center_wy, recon.zoom,
                   sun_dir, sun_col, amb_col);
    const sg_range u_range = { u, sizeof(u) };

    // ---- Billboards: per-bucket sort, then coalesce by key --------------
    if (!fx_billboard_queue.empty() && fx_billboard_pip[0][0].id)
    {
        // Group adjacent submissions with equal keys; sort within each
        // group back-to-front. (Per-bucket sort granularity per
        // PHASE1_SPINE.md §9.)
        std::sort(fx_billboard_queue.begin(), fx_billboard_queue.end(),
                  [](const SFxBillboardQueueEntry& a, const SFxBillboardQueueEntry& b) {
                      const SFxBatchKey& ka = a.item.key;
                      const SFxBatchKey& kb = b.item.key;
                      if (ka.texture != kb.texture) return ka.texture < kb.texture;
                      if (ka.blend   != kb.blend)   return ka.blend   < kb.blend;
                      if (ka.depth_mode != kb.depth_mode) return ka.depth_mode < kb.depth_mode;
                      // Greater sort_z = further from camera = draw first.
                      return a.sort_z > b.sort_z;
                  });

        // Materialize the entire queue into one shared instance buffer
        // (count <= kMaxFxInstances).
        std::vector<float> scratch;
        scratch.reserve(fx_billboard_queue.size() * kFxBillboardInstanceFloats);
        for (const auto& e : fx_billboard_queue)
        {
            const auto& it = e.item;
            scratch.push_back(it.world_pos[0]);
            scratch.push_back(it.world_pos[1]);
            scratch.push_back(it.world_pos[2]);
            scratch.push_back(it.size_wu[0]);
            scratch.push_back(it.size_wu[1]);
            scratch.push_back(it.uv_rect[0]);
            scratch.push_back(it.uv_rect[1]);
            scratch.push_back(it.uv_rect[2]);
            scratch.push_back(it.uv_rect[3]);
            scratch.push_back(it.color_rgba[0]);
            scratch.push_back(it.color_rgba[1]);
            scratch.push_back(it.color_rgba[2]);
            scratch.push_back(it.color_rgba[3]);
            scratch.push_back(float(uint8_t(it.debug_mode)));
            scratch.push_back(float(uint8_t(it.light_mode)));
            scratch.push_back(float(uint8_t(it.orientation)));
            // kFxBillboardInstanceFloats (16) is now fully packed --
            // no trailing pad lane. If you add another per-instance
            // attribute, bump the constant + the pipeline attr table
            // + the shader's vs_in to match.
        }
        const sg_range r = { scratch.data(), scratch.size() * sizeof(float) };
        sg_update_buffer(fx_billboard_ivb, &r);

        // Emit one sg_draw per equal-key run.
        size_t i = 0;
        while (i < fx_billboard_queue.size())
        {
            size_t j = i + 1;
            while (j < fx_billboard_queue.size()
                   && fx_billboard_queue[j].item.key.Equals(fx_billboard_queue[i].item.key))
                ++j;
            const SFxBatchKey& key = fx_billboard_queue[i].item.key;
            const int32_t b = (key.blend < kFxBlendCount) ? key.blend : int32_t(EFxBlend::Alpha);
            const int32_t d = (key.depth_mode < kFxDepthModeCount) ? key.depth_mode : int32_t(EFxDepthMode::TestNoWrite);
            sg_pipeline pip = fx_billboard_pip[b][d];
            if (!pip.id) { i = j; continue; }
            sg_apply_pipeline(pip);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);

            sg_bindings bind = {};
            bind.vertex_buffers[0]        = fx_corner_vb;
            bind.vertex_buffers[1]        = fx_billboard_ivb;
            bind.vertex_buffer_offsets[1] = int(i) * kFxBillboardInstanceFloats * int(sizeof(float));
            bind.fs_images[0]             = TextureImage(key.texture);
            if (!bind.fs_images[0].id) { i = j; continue; }
            sg_apply_bindings(&bind);
            sg_draw(0, 4, int(j - i));
            i = j;
        }
    }

    // ---- Particles ------------------------------------------------------
    if (!fx_particle_queue.empty() && fx_particle_pip[0][0].id)
    {
        std::sort(fx_particle_queue.begin(), fx_particle_queue.end(),
                  [](const SFxParticleQueueEntry& a, const SFxParticleQueueEntry& b) {
                      const SFxBatchKey& ka = a.item.key;
                      const SFxBatchKey& kb = b.item.key;
                      if (ka.texture != kb.texture) return ka.texture < kb.texture;
                      if (ka.blend   != kb.blend)   return ka.blend   < kb.blend;
                      if (ka.depth_mode != kb.depth_mode) return ka.depth_mode < kb.depth_mode;
                      return a.sort_z > b.sort_z;
                  });

        std::vector<float> scratch;
        scratch.reserve(fx_particle_queue.size() * kFxParticleInstanceFloats);
        for (const auto& e : fx_particle_queue)
        {
            const auto& it = e.item;
            scratch.push_back(it.world_pos[0]);
            scratch.push_back(it.world_pos[1]);
            scratch.push_back(it.world_pos[2]);
            scratch.push_back(it.size_wu[0]);
            scratch.push_back(it.size_wu[1]);
            scratch.push_back(it.uv_rect[0]);
            scratch.push_back(it.uv_rect[1]);
            scratch.push_back(it.uv_rect[2]);
            scratch.push_back(it.uv_rect[3]);
            scratch.push_back(it.color_rgba[0]);
            scratch.push_back(it.color_rgba[1]);
            scratch.push_back(it.color_rgba[2]);
            scratch.push_back(it.color_rgba[3]);
            scratch.push_back(float(uint8_t(it.debug_mode)));
            scratch.push_back(it.rotation_rad);
            scratch.push_back(float(uint8_t(it.light_mode)));
            scratch.push_back(float(uint8_t(it.orientation)));
            // pad to kFxParticleInstanceFloats (20). Off-by-one here
            // would shift every subsequent instance's read by 4 bytes
            // (stride mismatch), corrupting world_pos / size_wu /
            // rotation_rad across instances and producing trapezoidal
            // quad distortions. Keep count synced with the constant.
            scratch.push_back(0.0f);
            scratch.push_back(0.0f);
            scratch.push_back(0.0f);
        }
        const sg_range r = { scratch.data(), scratch.size() * sizeof(float) };
        sg_update_buffer(fx_particle_ivb, &r);

        size_t i = 0;
        while (i < fx_particle_queue.size())
        {
            size_t j = i + 1;
            while (j < fx_particle_queue.size()
                   && fx_particle_queue[j].item.key.Equals(fx_particle_queue[i].item.key))
                ++j;
            const SFxBatchKey& key = fx_particle_queue[i].item.key;
            const int32_t b = (key.blend < kFxBlendCount) ? key.blend : int32_t(EFxBlend::Alpha);
            const int32_t d = (key.depth_mode < kFxDepthModeCount) ? key.depth_mode : int32_t(EFxDepthMode::TestNoWrite);
            sg_pipeline pip = fx_particle_pip[b][d];
            if (!pip.id) { i = j; continue; }
            sg_apply_pipeline(pip);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);

            sg_bindings bind = {};
            bind.vertex_buffers[0]        = fx_corner_vb;
            bind.vertex_buffers[1]        = fx_particle_ivb;
            bind.vertex_buffer_offsets[1] = int(i) * kFxParticleInstanceFloats * int(sizeof(float));
            bind.fs_images[0]             = TextureImage(key.texture);
            if (!bind.fs_images[0].id) { i = j; continue; }
            sg_apply_bindings(&bind);
            sg_draw(0, 4, int(j - i));
            i = j;
        }
    }

    // ---- Strips ---------------------------------------------------------
    if (!fx_strip_queue.empty() && fx_strip_pip[0][0].id)
    {
        // Strips are not auto-batched. Expand each strip's segments into
        // 6 vertices per segment (two triangles per quad) and emit a
        // separate sg_draw call per strip.
        std::vector<float> scratch;
        // Per strip: write vertices, then draw range [base, base+count).
        struct DrawSpan { int32_t first; int32_t count; SFxBatchKey key; EFxDebugMode dbg; EFxLightMode lit; };
        std::vector<DrawSpan> spans;

        for (const auto& e : fx_strip_queue)
        {
            const int32_t n = int32_t(e.segments.size());
            if (n <= 0) continue;
            const int32_t first_v = int32_t(scratch.size() / kFxStripVertexFloats);
            const float dbg = float(uint8_t(e.debug_mode));
            const float lit = float(uint8_t(e.light_mode));

            // Per-vertex tangent miter for continuous strips. At an
            // interior joint (seg[i].b ~ seg[i+1].a) both segments emit
            // their shared corner using the AVERAGED tangent of the
            // adjacent segments. The shader builds side = cross(tan,
            // view_dir), so identical tangents at the shared vertex
            // produce identical side offsets => the two segments' edges
            // line up exactly => no seam. Non-continuous strips (where
            // consecutive endpoints don't match) fall back to per-
            // segment tangent automatically.
            auto seg_tan = [](const SStripSegment& s, float t[3]) {
                t[0] = s.world_b[0] - s.world_a[0];
                t[1] = s.world_b[1] - s.world_a[1];
                t[2] = s.world_b[2] - s.world_a[2];
            };
            auto endpoints_match = [](const SStripSegment& a, const SStripSegment& b) {
                const float ex = a.world_b[0] - b.world_a[0];
                const float ey = a.world_b[1] - b.world_a[1];
                const float ez = a.world_b[2] - b.world_a[2];
                return (ex * ex + ey * ey + ez * ez) < 0.01f;   // ~0.1 wu tolerance
            };

            // Precompute per-vertex tangents (N+1 tangents for N segments).
            std::vector<float> tan_at(size_t(n + 1) * 3, 0.0f);
            for (int32_t s = 0; s < n; ++s)
            {
                float t_self[3]; seg_tan(e.segments[s], t_self);
                // 'a' side of segment s
                if (s > 0 && endpoints_match(e.segments[s - 1], e.segments[s]))
                {
                    // shared with previous segment's 'b' -- average
                    float t_prev[3]; seg_tan(e.segments[s - 1], t_prev);
                    tan_at[s * 3 + 0] = 0.5f * (t_prev[0] + t_self[0]);
                    tan_at[s * 3 + 1] = 0.5f * (t_prev[1] + t_self[1]);
                    tan_at[s * 3 + 2] = 0.5f * (t_prev[2] + t_self[2]);
                }
                else
                {
                    tan_at[s * 3 + 0] = t_self[0];
                    tan_at[s * 3 + 1] = t_self[1];
                    tan_at[s * 3 + 2] = t_self[2];
                }
                // 'b' side of last segment uses its own tangent.
                if (s == n - 1)
                {
                    tan_at[(s + 1) * 3 + 0] = t_self[0];
                    tan_at[(s + 1) * 3 + 1] = t_self[1];
                    tan_at[(s + 1) * 3 + 2] = t_self[2];
                }
            }

            for (int32_t s = 0; s < n; ++s)
            {
                const SStripSegment& seg = e.segments[s];
                const float* tan_a = &tan_at[size_t(s) * 3];
                const float* tan_b = &tan_at[size_t(s + 1) * 3];
                // 4 corners per segment: a-left, b-left, a-right, b-right.
                // Triangle list: (al, bl, ar) (bl, br, ar) -- 6 vertices.
                auto emit = [&](const float* wp, const float* color,
                                const float* tan,
                                float half_w, float u, float v) {
                    scratch.push_back(wp[0]); scratch.push_back(wp[1]); scratch.push_back(wp[2]);
                    scratch.push_back(tan[0]); scratch.push_back(tan[1]); scratch.push_back(tan[2]);
                    scratch.push_back(half_w);
                    scratch.push_back(u); scratch.push_back(v);
                    scratch.push_back(color[0]); scratch.push_back(color[1]); scratch.push_back(color[2]); scratch.push_back(color[3]);
                    scratch.push_back(dbg);
                    scratch.push_back(lit);
                    scratch.push_back(0.0f);  // pad to kFxStripVertexFloats (16)
                };
                emit(seg.world_a, seg.color_a, tan_a, -0.5f * seg.width_a_wu, seg.u_a, 0.0f);   // al
                emit(seg.world_b, seg.color_b, tan_b, -0.5f * seg.width_b_wu, seg.u_b, 0.0f);   // bl
                emit(seg.world_a, seg.color_a, tan_a, +0.5f * seg.width_a_wu, seg.u_a, 1.0f);   // ar
                emit(seg.world_b, seg.color_b, tan_b, -0.5f * seg.width_b_wu, seg.u_b, 0.0f);   // bl
                emit(seg.world_b, seg.color_b, tan_b, +0.5f * seg.width_b_wu, seg.u_b, 1.0f);   // br
                emit(seg.world_a, seg.color_a, tan_a, +0.5f * seg.width_a_wu, seg.u_a, 1.0f);   // ar
            }
            DrawSpan ds = {};
            ds.first = first_v;
            ds.count = n * 6;
            ds.key   = e.key;
            ds.dbg   = e.debug_mode;
            ds.lit   = e.light_mode;
            spans.push_back(ds);
        }

        if (!scratch.empty() && int32_t(scratch.size() / kFxStripVertexFloats) <= kMaxFxStripVerts)
        {
            const sg_range r = { scratch.data(), scratch.size() * sizeof(float) };
            sg_update_buffer(fx_strip_vb, &r);

            for (const DrawSpan& sp : spans)
            {
                const int32_t b = (sp.key.blend < kFxBlendCount) ? sp.key.blend : int32_t(EFxBlend::Alpha);
                const int32_t d = (sp.key.depth_mode < kFxDepthModeCount) ? sp.key.depth_mode : int32_t(EFxDepthMode::TestNoWrite);
                sg_pipeline pip = fx_strip_pip[b][d];
                if (!pip.id) continue;
                sg_apply_pipeline(pip);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
                float cam[8] = {
                    fx_camera.pos[0], fx_camera.pos[1], fx_camera.pos[2], 0.0f,
                    fx_camera.forward[0], fx_camera.forward[1], fx_camera.forward[2], 0.0f,
                };
                const sg_range cam_r = { cam, sizeof(cam) };
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 1, &cam_r);
                sg_bindings bind = {};
                bind.vertex_buffers[0]        = fx_strip_vb;
                bind.vertex_buffer_offsets[0] = sp.first * kFxStripVertexFloats * int(sizeof(float));
                bind.fs_images[0]             = TextureImage(sp.key.texture);
                if (!bind.fs_images[0].id) continue;
                sg_apply_bindings(&bind);
                sg_draw(0, sp.count, 1);
            }
        }
    }

    sg_end_pass();

    fx_billboard_queue.clear();
    fx_particle_queue.clear();
    fx_strip_queue.clear();
    fx_camera.set = false;
    lit_target_dirty = true;
}
