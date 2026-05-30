// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  framesnap.cpp - Headless snapshot + filmstrip output                 *
// *************************************************************************

#include "framesnap.h"

#include "display.h"
#include "logging.h"
#include "renderer_readback.h"
#include "surface.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <sokol_app.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace FrameSnap {

namespace {

SConfig g_cfg;
int32_t g_frameCount        = 0;
int32_t g_capturesTaken     = 0;
int32_t g_lastCaptureFrame  = -1000000;

struct SCapturedFrame {
    std::vector<uint8_t> pixels;   // RGBA8, captureW * captureH * 4
    std::string          label;    // optional caption (manual mode); may be empty
};
std::vector<SCapturedFrame> g_capturedFrames;

int32_t g_captureW          = 0;
int32_t g_captureH          = 0;
bool    g_done              = false;
int32_t g_batchNumber       = 1;          // increments per manual-mode flush

// Pending label for the next capture — set by TriggerSnapshot(label) before
// it calls CaptureCurrentFrame so the captured frame stores the label.
std::string g_pendingLabel;

// True iff cfg.is_filmstrip && cfg.interval_sec == 0.
bool IsManualMode()
{
    return g_cfg.is_filmstrip && g_cfg.interval_sec == 0.0;
}

// Create the parent directory of `path` if it doesn't exist. Recursive
// (`mkdir -p` equivalent). Only used to materialize the default
// `filmstrips/` folder so unprefixed snaps land somewhere predictable.
void EnsureParentDir(const std::string& path)
{
    const auto slash = path.find_last_of('/');
    if (slash == std::string::npos || slash == 0) return;
    std::string parent = path.substr(0, slash);
    std::string acc;
    size_t i = 0;
    if (parent[0] == '/') { acc = "/"; i = 1; }
    while (i < parent.size())
    {
        const size_t next = parent.find('/', i);
        const size_t end  = (next == std::string::npos) ? parent.size() : next;
        if (end > i)
        {
            if (!acc.empty() && acc.back() != '/') acc += '/';
            acc += parent.substr(i, end - i);
            struct stat st {};
            if (stat(acc.c_str(), &st) != 0)
                (void) mkdir(acc.c_str(), 0755);
        }
        if (next == std::string::npos) break;
        i = next + 1;
    }
}

// Strip "--key=" / "-key=" / "/key=" / "key=" prefixes and yield the
// value substring (or empty string if no value).
const char* GetFlagValue(const char* arg, const char* key)
{
    const char* p = arg;
    while (*p == '-' || *p == '/') ++p;
    const size_t klen = std::strlen(key);
    if (std::strncmp(p, key, klen) != 0) return nullptr;
    if (p[klen] != '=' && p[klen] != ':') return nullptr;
    return p + klen + 1;
}

int32_t ChooseGridDim(int32_t n)
{
    // smallest d such that d*d >= n
    int32_t d = 1;
    while (d * d < n) ++d;
    return d;
}

} // namespace

bool ParseArgs(int argc, char** argv, SConfig& cfg)
{
    cfg = SConfig{};
    bool found = false;

    for (int i = 1; i < argc; ++i)
    {
        if (!argv[i]) continue;

        if (const char* v = GetFlagValue(argv[i], "snap"))
        {
            cfg.active       = true;
            cfg.is_filmstrip = false;
            cfg.frames       = 1;
            cfg.out_path     = v;
            found = true;
        }
        else if (const char* v2 = GetFlagValue(argv[i], "filmstrip"))
        {
            // Expected: "N,DURATION" e.g. "32,5.0". DURATION==0 → manual mode
            // (game must call TriggerSnapshot to capture each frame).
            std::string s(v2);
            const auto comma = s.find(',');
            if (comma == std::string::npos)
            {
                log_warn("[framesnap] --filmstrip needs N,DURATION (got '%s')", v2);
                continue;
            }
            const int32_t n   = std::atoi(s.substr(0, comma).c_str());
            const double  dur = std::atof(s.substr(comma + 1).c_str());
            if (n <= 0 || dur < 0.0)
            {
                log_warn("[framesnap] --filmstrip parse failed: N=%d DUR=%f",
                         n, dur);
                continue;
            }
            cfg.active       = true;
            cfg.is_filmstrip = true;
            cfg.frames       = n;
            cfg.interval_sec = dur;
            found = true;
        }
        else if (const char* v3 = GetFlagValue(argv[i], "snapprefix"))
        {
            cfg.prefix = v3;
            found = true;
        }
        else if (const char* v4 = GetFlagValue(argv[i], "snapwarmup"))
        {
            cfg.warmup_frames = std::atoi(v4);
            if (cfg.warmup_frames < 0) cfg.warmup_frames = 0;
        }
        else if (const char* v5 = GetFlagValue(argv[i], "snaprect"))
        {
            // Expected: "x,y,w,h" e.g. "452,306,188,174"
            std::string s(v5);
            int32_t xs[4] = { -1, -1, -1, -1 };
            int32_t i_arg = 0;
            size_t  start = 0;
            for (size_t j = 0; j <= s.size() && i_arg < 4; ++j)
            {
                if (j == s.size() || s[j] == ',')
                {
                    xs[i_arg++] = std::atoi(s.substr(start, j - start).c_str());
                    start = j + 1;
                }
            }
            if (i_arg == 4 && xs[2] > 0 && xs[3] > 0)
            {
                cfg.rect_x = xs[0];
                cfg.rect_y = xs[1];
                cfg.rect_w = xs[2];
                cfg.rect_h = xs[3];
            }
            else
            {
                log_warn("[framesnap] --snaprect needs x,y,w,h (got '%s')", v5);
            }
        }
    }

    // --snap or --filmstrip is the trigger; --snapprefix alone is a no-op.
    if (!(found && cfg.active)) return false;

    // Default output folder: <cwd>/filmstrips/. Single-snap respects its
    // own --snap=PATH verbatim; only filmstrip uses prefix-based naming.
    //
    // PREFIX is interpreted as a path UNDER filmstrips/ unless absolute:
    //   --snapprefix (omitted)        → filmstrips/                (default)
    //   --snapprefix=tabs_            → filmstrips/tabs_
    //   --snapprefix=sidebar/tabs_    → filmstrips/sidebar/tabs_   (subfolder)
    //   --snapprefix=hud/v3/run-      → filmstrips/hud/v3/run-     (nested)
    //   --snapprefix=/tmp/foo_        → /tmp/foo_                  (absolute, untouched)
    //
    // The parent directory tree is mkdir'd on first write (recursive,
    // mkdir -p equivalent) so deep paths "just work".
    if (cfg.is_filmstrip)
    {
        if (cfg.prefix.empty())
            cfg.prefix = "filmstrips/";
        else if (cfg.prefix[0] != '/')
            cfg.prefix = std::string("filmstrips/") + cfg.prefix;
    }

    return true;
}

void SetConfig(const SConfig& cfg) { g_cfg = cfg; }
const SConfig& GetConfig()         { return g_cfg; }
bool Active()                      { return g_cfg.active && !g_done; }

bool WritePngRGBA(const char* path, int32_t w, int32_t h, const uint8_t* pixels)
{
    if (!path || !pixels || w <= 0 || h <= 0) return false;
    EnsureParentDir(path);
    const int stride = w * 4;
    const int ok = stbi_write_png(path, w, h, 4, pixels, stride);
    if (!ok)
    {
        log_warn("[framesnap] PNG write failed: %s", path);
        return false;
    }
    return true;
}

namespace {

bool CaptureCurrentFrame()
{
    if (!Display.IsActive()) return false;
    sg_image img = Display.SnapCaptureImage();
    if (img.id == SG_INVALID_ID)
    {
        // First few frames: FlipPage may not have allocated the snap RT
        // yet. Skip silently; we'll catch up on the next tick.
        return false;
    }
    sg_image_info info = sg_query_image_info(img);
    if (info.width <= 0 || info.height <= 0) return false;

    // Resolve the rect: use --snaprect if set, else the whole image.
    int32_t rx = g_cfg.rect_x;
    int32_t ry = g_cfg.rect_y;
    int32_t rw = g_cfg.rect_w;
    int32_t rh = g_cfg.rect_h;
    if (rx < 0 || ry < 0 || rw <= 0 || rh <= 0)
    {
        rx = 0;
        ry = 0;
        rw = info.width;
        rh = info.height;
    }
    // Clamp to image bounds; bail if the rect is out-of-bounds.
    if (rx >= info.width || ry >= info.height) return false;
    if (rx + rw > info.width)  rw = info.width  - rx;
    if (ry + rh > info.height) rh = info.height - ry;

    if (g_captureW == 0)
    {
        g_captureW = rw;
        g_captureH = rh;
    }
    else if (rw != g_captureW || rh != g_captureH)
    {
        log_warn("[framesnap] capture rect size changed mid-run: was %dx%d now %dx%d",
                 g_captureW, g_captureH, rw, rh);
        return false;
    }

    std::vector<uint8_t> buf(size_t(rw) * size_t(rh) * 4);
    if (!RendererReadback::ReadRect(img, rx, ry, rw, rh, buf.data()))
    {
        log_warn("[framesnap] readback failed at frame %d (capture %d)",
                 g_frameCount, g_capturesTaken);
        return false;
    }
    SCapturedFrame cap;
    cap.pixels = std::move(buf);
    cap.label  = g_pendingLabel;
    g_pendingLabel.clear();
    g_capturedFrames.push_back(std::move(cap));
    return true;
}

// ---- TTF label rendering ------------------------------------------------
//
// JetBrainsMono is a fixed-width font that's vendored in the tree for the
// editor; mono is ideal for our use (legible at small sizes, no kerning
// math). Loaded lazily on first label render, cached for the run.
stbtt_fontinfo g_labelFont;
std::vector<uint8_t> g_labelFontData;
bool g_labelFontLoaded = false;
bool g_labelFontFailed = false;

bool EnsureLabelFont()
{
    if (g_labelFontLoaded) return true;
    if (g_labelFontFailed) return false;

    static constexpr const char* kFontPath = "thirdparty/fonts/JetBrainsMono-Regular.ttf";
    FILE* f = std::fopen(kFontPath, "rb");
    if (!f) { g_labelFontFailed = true; log_warn("[framesnap] label font missing: %s", kFontPath); return false; }
    std::fseek(f, 0, SEEK_END);
    const long sz = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    g_labelFontData.assign(size_t(sz), 0);
    if (sz > 0 && std::fread(g_labelFontData.data(), 1, size_t(sz), f) != size_t(sz))
    {
        std::fclose(f);
        g_labelFontFailed = true;
        log_warn("[framesnap] label font short read");
        return false;
    }
    std::fclose(f);
    if (!stbtt_InitFont(&g_labelFont, g_labelFontData.data(),
                        stbtt_GetFontOffsetForIndex(g_labelFontData.data(), 0)))
    {
        g_labelFontFailed = true;
        log_warn("[framesnap] stbtt_InitFont failed");
        return false;
    }
    g_labelFontLoaded = true;
    return true;
}

// Stamp one glyph into `pixels` (RGBA8 W×H) at pen position with the
// given color. Coverage is the glyph's coverage byte (0..255).
void StampGlyph(uint8_t* pixels, int32_t W, int32_t H,
                int32_t pen_x, int32_t baseline,
                const uint8_t* glyph, int32_t gw, int32_t gh,
                int32_t x_off, int32_t y_off,
                uint8_t r, uint8_t g, uint8_t b)
{
    for (int gy = 0; gy < gh; ++gy)
    {
        const int32_t py = baseline + y_off + gy;
        if (py < 0 || py >= H) continue;
        for (int gx = 0; gx < gw; ++gx)
        {
            const int32_t px = pen_x + x_off + gx;
            if (px < 0 || px >= W) continue;
            const uint8_t a = glyph[size_t(gy) * size_t(gw) + size_t(gx)];
            if (!a) continue;
            uint8_t* dst = pixels + (size_t(py) * size_t(W) + size_t(px)) * 4;
            dst[0] = uint8_t(dst[0] + (((r - dst[0]) * a) >> 8));
            dst[1] = uint8_t(dst[1] + (((g - dst[1]) * a) >> 8));
            dst[2] = uint8_t(dst[2] + (((b - dst[2]) * a) >> 8));
        }
    }
}

// Stamp `text` onto `pixels` (RGBA8 W×H) as white glyphs with a black
// drop-shadow (8 neighbor offsets) so the label is legible against any
// background — no need for a dimming band. Top-left margin of (2, 2).
// Long labels are hard-truncated when they run out of width.
void DrawLabelOnCell(uint8_t* pixels, int32_t W, int32_t H,
                     const char* text, int32_t pxSize)
{
    if (!pixels || !text || !*text || W <= 0 || H <= 0) return;
    if (!EnsureLabelFont()) return;

    const float scale = stbtt_ScaleForPixelHeight(&g_labelFont, float(pxSize));
    int ascent = 0, descent = 0, lineGap = 0;
    stbtt_GetFontVMetrics(&g_labelFont, &ascent, &descent, &lineGap);
    const int32_t baseline = int32_t(float(ascent) * scale) + 2;

    int32_t penX = 2;
    for (const char* p = text; *p; ++p)
    {
        const int codepoint = (unsigned char)*p;
        if (codepoint < 0x20 || codepoint > 0x7E) continue;

        int advance = 0, lsb = 0;
        stbtt_GetCodepointHMetrics(&g_labelFont, codepoint, &advance, &lsb);
        const int32_t glyphAdv = int32_t(float(advance) * scale + 0.5f);
        if (penX + glyphAdv > W - 2) break;  // ran out of width — truncate

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&g_labelFont, codepoint, scale, scale,
                                    &x0, &y0, &x1, &y1);
        const int gw = x1 - x0;
        const int gh = y1 - y0;
        if (gw > 0 && gh > 0)
        {
            std::vector<uint8_t> glyph(size_t(gw) * size_t(gh), 0);
            stbtt_MakeCodepointBitmap(&g_labelFont, glyph.data(), gw, gh,
                                       gw, scale, scale, codepoint);
            // 8-neighbor black drop-shadow first, then white on top.
            // Ensures the text is readable on any background.
            for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
            {
                if (dx == 0 && dy == 0) continue;
                StampGlyph(pixels, W, H, penX + dx, baseline + dy,
                           glyph.data(), gw, gh, x0, y0, 0, 0, 0);
            }
            StampGlyph(pixels, W, H, penX, baseline,
                       glyph.data(), gw, gh, x0, y0, 255, 255, 255);
        }
        penX += glyphAdv;
    }
}

// Sanitize a free-form label into something safe for a filename:
// lowercase, spaces→dashes, drop anything outside [a-z0-9._-].
std::string SanitizeLabel(const std::string& in)
{
    std::string out;
    out.reserve(in.size());
    for (char c : in)
    {
        if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
            || c == '.' || c == '_' || c == '-')
        {
            out += c;
        }
        else if (c == ' ' || c == '/' || c == ':' || c == ',')
        {
            if (!out.empty() && out.back() != '-') out += '-';
        }
    }
    while (!out.empty() && out.back() == '-') out.pop_back();
    return out;
}

std::string MakeIndexedPath(int32_t batch, int32_t idx, const std::string& label)
{
    char nbuf[32];
    if (IsManualMode())
        std::snprintf(nbuf, sizeof(nbuf), "%03d-%03d", batch, idx);
    else
        std::snprintf(nbuf, sizeof(nbuf), "%03d", idx);
    std::string out = g_cfg.prefix;
    out += nbuf;
    const std::string slug = SanitizeLabel(label);
    if (!slug.empty()) { out += '-'; out += slug; }
    out += ".png";
    return out;
}

std::string MakeFilmstripPath(int32_t batch)
{
    std::string out = g_cfg.prefix;
    out += "filmstrip";
    if (IsManualMode())
    {
        char nbuf[16];
        std::snprintf(nbuf, sizeof(nbuf), "-%03d", batch);
        out += nbuf;
    }
    out += ".png";
    return out;
}

void FlushOutputs()
{
    if (g_capturedFrames.empty())
    {
        log_warn("[framesnap] no frames captured");
        return;
    }

    // Single-snap path: caller chose the out_path verbatim.
    if (!g_cfg.is_filmstrip)
    {
        WritePngRGBA(g_cfg.out_path.c_str(),
                     g_captureW, g_captureH,
                     g_capturedFrames[0].pixels.data());
        log_info("[framesnap] wrote %s (%dx%d)",
                 g_cfg.out_path.c_str(), g_captureW, g_captureH);
        return;
    }

    // Filmstrip path: per-frame PNGs + NxN composite grid.
    const int32_t n = int32_t(g_capturedFrames.size());
    for (int32_t i = 0; i < n; ++i)
    {
        const std::string p = MakeIndexedPath(g_batchNumber, i + 1,
                                              g_capturedFrames[size_t(i)].label);
        WritePngRGBA(p.c_str(), g_captureW, g_captureH,
                     g_capturedFrames[size_t(i)].pixels.data());
    }

    const int32_t dim = ChooseGridDim(n);

    // Target the composite at a "reasonable" max edge — 1920 px keeps the
    // grid file ≈ 1080p-ish at most, regardless of grid_dim. Individual
    // frame PNGs (above) stay at native capture resolution; only this
    // composite is downscaled so it doesn't balloon to tens of MB.
    constexpr int32_t kCompositeMaxEdge = 1920;
    int32_t cellW = g_captureW;
    int32_t cellH = g_captureH;
    if (g_captureW * dim > kCompositeMaxEdge
        || g_captureH * dim > kCompositeMaxEdge)
    {
        const double sx = double(kCompositeMaxEdge) / double(g_captureW * dim);
        const double sy = double(kCompositeMaxEdge) / double(g_captureH * dim);
        const double s  = (sx < sy) ? sx : sy;
        cellW = int32_t(g_captureW * s);
        cellH = int32_t(g_captureH * s);
        if (cellW < 1) cellW = 1;
        if (cellH < 1) cellH = 1;
    }

    const int32_t gridW = cellW * dim;
    const int32_t gridH = cellH * dim;
    std::vector<uint8_t> grid(size_t(gridW) * size_t(gridH) * 4, 0);

    // Scratch buffer for each downscaled cell — allocated once, reused.
    std::vector<uint8_t> cellBuf(size_t(cellW) * size_t(cellH) * 4);

    for (int32_t i = 0; i < n; ++i)
    {
        const int32_t cx = (i % dim);
        const int32_t cy = (i / dim);
        const uint8_t* src = g_capturedFrames[size_t(i)].pixels.data();
        const std::string& label = g_capturedFrames[size_t(i)].label;

        // Scale into cellBuf (or alias to src if no scaling needed).
        const uint8_t* cellSrc = src;
        int32_t cellSrcStride  = g_captureW * 4;
        if (cellW != g_captureW || cellH != g_captureH)
        {
            stbir_resize_uint8_srgb(src, g_captureW, g_captureH, g_captureW * 4,
                                    cellBuf.data(), cellW, cellH, cellW * 4,
                                    STBIR_RGBA);
            cellSrc       = cellBuf.data();
            cellSrcStride = cellW * 4;
        }

        // Stamp the label onto the SCALED cell so text stays readable
        // (per user 2026-05-30 — label drawn after scale reduction, not
        // before). Modify cellBuf in place; if we aliased to src, copy
        // it into cellBuf first so we don't mutate the captured frame.
        if (!label.empty())
        {
            if (cellSrc == src)
            {
                cellBuf.assign(src, src + size_t(cellW) * size_t(cellH) * 4);
                cellSrc       = cellBuf.data();
                cellSrcStride = cellW * 4;
            }
            // Font px height scales with cell width so small cells get
            // a small (but still legible) overlay. Clamp 10..22 px.
            int32_t pxSize = cellW / 28;
            if (pxSize < 10) pxSize = 10;
            if (pxSize > 22) pxSize = 22;
            DrawLabelOnCell(cellBuf.data(), cellW, cellH, label.c_str(), pxSize);
        }

        for (int32_t row = 0; row < cellH; ++row)
        {
            uint8_t* dst = grid.data()
                         + (size_t(cy * cellH + row) * size_t(gridW)
                            + size_t(cx * cellW)) * 4;
            std::memcpy(dst,
                        cellSrc + size_t(row * cellSrcStride),
                        size_t(cellW) * 4);
        }
    }

    const std::string strip = MakeFilmstripPath(g_batchNumber);
    WritePngRGBA(strip.c_str(), gridW, gridH, grid.data());
    log_info("[framesnap] batch %d: wrote %d frames + %s (%dx%d, %dx%d grid, cell %dx%d)",
             g_batchNumber, n, strip.c_str(), gridW, gridH, dim, dim, cellW, cellH);
}

// Reset for the next manual-mode batch. Auto mode goes to g_done==true
// instead and never resets.
void ResetForNextBatch()
{
    g_capturedFrames.clear();
    g_capturesTaken = 0;
    ++g_batchNumber;
}

} // namespace

void TickAfterRender()
{
    if (!g_cfg.active || g_done) return;
    ++g_frameCount;

    if (g_frameCount <= g_cfg.warmup_frames) return;

    // Single-snap: capture once on the first post-warmup frame.
    if (!g_cfg.is_filmstrip)
    {
        if (g_capturesTaken == 0)
        {
            if (CaptureCurrentFrame()) ++g_capturesTaken;
        }
    }
    else if (IsManualMode())
    {
        // Manual mode: only TriggerSnapshot drives captures. Nothing to
        // do per-tick.
    }
    else
    {
        // Auto filmstrip: capture every interval_sec of REAL time. We use
        // frame count converted via a nominal 60Hz cadence — good enough
        // for the animated synthetic state cycling that drives the test
        // modes; if wall-clock-precise timing matters, swap in a timer.
        const double secPerFrame = 1.0 / 60.0;
        const double now = double(g_frameCount - g_cfg.warmup_frames) * secPerFrame;
        const double next = g_capturesTaken * g_cfg.interval_sec;
        if (now + 1e-6 >= next)
        {
            // Stamp wall-clock time as the label so the filmstrip cell
            // shows exactly when each frame was captured.
            const std::time_t tt = std::time(nullptr);
            std::tm lt{};
#if defined(_WIN32)
            localtime_s(&lt, &tt);
#else
            localtime_r(&tt, &lt);
#endif
            char tbuf[32];
            std::strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", &lt);
            g_pendingLabel = tbuf;
            if (CaptureCurrentFrame())
            {
                ++g_capturesTaken;
                log_info("[framesnap] auto-capture %d/%d at %s",
                         g_capturesTaken, g_cfg.frames, tbuf);
            }
        }
    }

    if (g_capturesTaken >= g_cfg.frames)
    {
        FlushOutputs();
        // Auto mode exits; manual mode keeps running so the tester can
        // start another batch with more TriggerSnapshot calls.
        if (IsManualMode())
        {
            ResetForNextBatch();
        }
        else
        {
            g_done = true;
            sapp_request_quit();
        }
    }
}

bool TriggerSnapshot(const char* label)
{
    if (!g_cfg.active || g_done) return false;
    if (!IsManualMode())
    {
        log_warn("[framesnap] TriggerSnapshot called but not in manual mode "
                 "(use --filmstrip=N,0)");
        return false;
    }
    g_pendingLabel = (label && *label) ? label : std::string();
    // Skip warmup for manual mode — the caller decided this is the moment.
    if (!CaptureCurrentFrame()) { g_pendingLabel.clear(); return false; }
    ++g_capturesTaken;
    log_info("[framesnap] manual-capture %d/%d%s%s",
             g_capturesTaken, g_cfg.frames,
             (label && *label) ? " label=" : "",
             (label && *label) ? label : "");
    if (g_capturesTaken >= g_cfg.frames)
    {
        FlushOutputs();
        ResetForNextBatch();
    }
    return true;
}

} // namespace FrameSnap
