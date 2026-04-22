// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   testmodes.cpp - Test mode helpers                   *
// *************************************************************************

#include "testmodes.h"

#include "bitmapatlas.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "imagery.h"
#include "imageres.h"
#include "logging.h"
#include "maprenderer.h"
#include "renderer.h"
#include "revenant.h"
#include "time.h"

#include <cmath>
#include <cstring>

namespace {

TMapRenderer g_mapRenderer;
SBitmapAtlas g_uiAtlas;

bool InitializeTTFMode()
{
    const char* paths[] = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Times New Roman.ttf",
        "/System/Library/Fonts/Supplemental/Georgia.ttf",
    };
    const int sizes[] = { 28, 48 };
    for (const char* p : paths)
        for (int s : sizes)
            if (!BuildTTFAtlas(p, s))
                log_warn("[ttf] skipping unavailable '%s'", p);
    return true;
}

bool InitializeTextMode()
{
    if (!SystemFont)
    {
        log_error("[text] SystemFont is null");
        return true;
    }
    const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
    if (!atlas || !atlas->image.id)
        log_error("[text] SystemFont atlas build failed");
    return true;
}

bool InitializeIconMode()
{
    if (!SystemFont)
    {
        log_error("[icon] SystemFont is null");
        return true;
    }
    const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
    if (!atlas || !atlas->image.id)
    {
        log_error("[icon] SystemFont atlas build failed");
        return true;
    }
    log_info("[icon] atlas %dx%d for %d glyphs",
        atlas->width, atlas->height, (int)atlas->numchars);
    return true;
}

bool InitializeI3DMode()
{
    const char* probes[] = {
        "Misc\\Blood.I3D", "Misc\\Sparks.I3D", nullptr
    };
    for (int i = 0; probes[i]; i++)
    {
        const int32_t id = TObjectImagery::FindImagery(probes[i]);
        log_info("[i3d] FindImagery('%s') -> id=%d", probes[i], id);
        if (id < 0)
            continue;
        TObjectImagery* img = TObjectImagery::LoadImagery(id);
        if (!img)
        {
            log_error("[i3d]   LoadImagery returned null for '%s'", probes[i]);
            continue;
        }
        SImageryHeader* hdr = img->GetHeader();
        if (!hdr)
        {
            log_error("[i3d]   header null for '%s'", probes[i]);
            continue;
        }
        log_info("[i3d]   loaded type=%d numstates=%d hdrsize=%d ressize=%u",
            img->ImageryId(), (int)hdr->numstates, img->GetHeaderSize(),
            img->GetResSize());
        for (int s = 0; s < hdr->numstates && s < 4; s++)
        {
            PSImageryStateHeader st = img->GetState(s);
            if (!st) break;
            log_info("[i3d]   state[%d] name='%s' w=%d h=%d frames=%d flags=0x%x",
                s, st->animname, st->width, st->height, st->frames, st->flags);
        }
    }
    return true;
}

bool InitializeFontMode()
{
    log_info("[font] FontTable=%p SystemFont=%p DialogFont=%p SmallFont=%p GameFont=%p",
        (void*)FontTable, (void*)SystemFont, (void*)DialogFont,
        (void*)SmallFont, (void*)GameFont);

    const char* probes[] = {
        "System", "Scroll", "Book", "Dialog", "Small", "Med", "Gold", nullptr
    };
    for (int i = 0; probes[i]; i++)
        LogFontInfo(probes[i]);

    LogFontGlyphAsciiArt("System", 'A');
    if (FontTable)
    {
        for (const char* fn : { "System", "Gold" })
            LogFontGlyphHexDump(fn, 'A');
    }
    return true;
}

void RenderUiMode()
{
    const int32_t tw = Display->Width();
    const int32_t th = Display->Height();
    int32_t cols = 1;
    const int32_t item_count = int32_t(g_uiAtlas.items.size());
    while (cols * cols < item_count) cols++;
    const int32_t rows = cols;

    constexpr int32_t pad = 4;
    int32_t cell_w = (tw - pad) / cols;
    if (cell_w < 16) cell_w = 16;
    const int32_t cell_h = cell_w;
    const int32_t grid_w = cols * cell_w;
    const int32_t total_h = rows * cell_h;
    const int32_t ox = (tw - grid_w) / 2;

    constexpr double kAnimHz = 8.0;
    constexpr double kScrollPxS = 40.0;
    const int32_t tick = (int32_t)(TTime::Time() * kAnimHz);
    const int32_t scroll = total_h > 0
        ? ((int32_t)(TTime::Time() * kScrollPxS) % total_h + total_h) % total_h
        : 0;

    Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < item_count; i++)
    {
        const auto& it = g_uiAtlas.items[i];
        const int32_t fi = it.firstFrame + (it.numFrames > 1 ? (tick % it.numFrames) : 0);
        const auto& fr = g_uiAtlas.frames[fi];

        const int32_t col = i % cols;
        const int32_t row = i / cols;
        const int32_t cx = ox + col * cell_w + cell_w / 2;
        const int32_t dx = cx - fr.w / 2;

        for (int k = 0; k < 2; k++)
        {
            const int32_t cy = row * cell_h + cell_h / 2 - scroll + k * total_h;
            const int32_t dy = cy - fr.h / 2;
            if (dy + fr.h <= 0 || dy >= th) continue;
            Renderer->Composite(
                g_uiAtlas.image,
                dx, dy, fr.w, fr.h,
                tw, th,
                fr.ax, fr.ay, fr.w, fr.h,
                g_uiAtlas.width, g_uiAtlas.height);
        }
    }
    Display->BackBuffer()->EndPass();
}

void RenderIconMode()
{
    const SFontAtlas* atlas = SystemFont ? FindFontAtlas(SystemFont) : nullptr;
    if (!atlas || !atlas->image.id)
        return;

    const int32_t tw = Display->Width();
    const int32_t th = Display->Height();
    const int32_t dx = (tw - atlas->width) / 2;
    const int32_t dy = (th - atlas->height) / 2;

    Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Renderer->Composite(atlas->image, dx, dy, atlas->width, atlas->height, tw, th);
    Display->BackBuffer()->EndPass();
}

void RenderTTFMode()
{
    const int32_t tw = Display->Width();
    const int32_t th = Display->Height();
    const char* msg = "Revenant 0123 !?";
    struct STtfLine { const char* path; int size; };
    const STtfLine lines[] = {
        { "/System/Library/Fonts/Supplemental/Arial.ttf",           28 },
        { "/System/Library/Fonts/Supplemental/Times New Roman.ttf", 48 },
    };

    Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

    int32_t baseline = 20 + lines[0].size;
    for (const auto& L : lines)
    {
        const SFontAtlas* atlas = BuildTTFAtlas(L.path, L.size);
        if (!atlas || !atlas->image.id) continue;

        float line_w = 0.0f;
        for (const char* p = msg; *p; ++p)
            line_w += atlas->Rect((unsigned char)*p).xadvance;

        float pen_x = (float)(tw - (int32_t)line_w) * 0.5f;
        const float pen_y = (float)baseline;

        for (const char* p = msg; *p; ++p)
        {
            const auto& r = atlas->Rect((unsigned char)*p);
            if (r.w > 0 && r.h > 0)
            {
                const int32_t gx = (int32_t)(pen_x + r.xoff + 0.5f);
                const int32_t gy = (int32_t)(pen_y + r.yoff + 0.5f);
                const int32_t gw = (int32_t)(r.xoff2 - r.xoff + 0.5f);
                const int32_t gh = (int32_t)(r.yoff2 - r.yoff + 0.5f);
                Renderer->Composite(
                    atlas->image,
                    gx, gy, gw, gh,
                    tw, th,
                    r.x, r.y, r.w, r.h,
                    atlas->width, atlas->height);
            }
            pen_x += r.xadvance;
        }
        baseline += L.size + 16;
    }

    Display->BackBuffer()->EndPass();
}

void RenderTextMode()
{
    if (!FontTable)
        return;

    const int32_t tw = Display->Width();
    const int32_t th = Display->Height();
    const char* msg = "Revenant 0123 !?.";

    struct SLine { TFont* font; const SFontAtlas* atlas; };
    SLine lines[2] = {
        { SystemFont, SystemFont ? BuildFontAtlas(SystemFont) : nullptr },
        { FontTable->Bitmap("Gold"), FontTable->Bitmap("Gold") ? BuildFontAtlas(FontTable->Bitmap("Gold")) : nullptr },
    };

    int32_t total_h = 0;
    for (const auto& L : lines)
        if (L.font) total_h += (int32_t)((TFontData*)L.font)->height + 4;

    Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

    int32_t line_top = (th - total_h) / 2;
    for (const auto& L : lines)
    {
        if (!L.font || !L.atlas || !L.atlas->image.id) continue;

        const int32_t line_h = (int32_t)((TFontData*)L.font)->height;
        int32_t line_w = 0;
        for (const char* p = msg; *p; ++p)
        {
            const unsigned char ch = (unsigned char)*p;
            line_w += (int32_t)L.font->DrawRight(ch) - (int32_t)L.font->DrawLeft(ch);
        }

        int32_t pen_x = (tw - line_w) / 2;
        const int32_t pen_y = line_top + line_h;
        for (const char* p = msg; *p; ++p)
        {
            const unsigned char ch = (unsigned char)*p;
            const auto& r = L.atlas->Rect(ch);
            if (r.w > 0 && r.h > 0)
            {
                const int32_t gx = pen_x - (int32_t)L.font->DrawLeft(ch);
                const int32_t gy = pen_y - (int32_t)L.font->StartHeight(ch);
                Renderer->Composite(
                    L.atlas->image,
                    gx, gy, r.w, r.h,
                    tw, th,
                    r.x, r.y, r.w, r.h,
                    L.atlas->width, L.atlas->height);
            }
            pen_x += (int32_t)L.font->DrawRight(ch) - (int32_t)L.font->DrawLeft(ch);
        }

        line_top += line_h + 4;
    }

    Display->BackBuffer()->EndPass();
}

void RenderBlankMode()
{
    const double t = TTime::Time();
    const float r = 0.5f + 0.5f * float(std::sin(t * 1.0));
    const float g = 0.5f + 0.5f * float(std::sin(t * 1.3 + 2.0));
    const float b = 0.5f + 0.5f * float(std::sin(t * 1.7 + 4.0));
    Display->BackBuffer()->StartPass(r, g, b, 1.0f);
    Display->BackBuffer()->EndPass();
}

}  // namespace

namespace TestModes {

bool Initialize(const char* mode)
{
    if (strcmp(mode, "blank") == 0 || strcmp(mode, "ticker") == 0)
        return true;
    if (strcmp(mode, "sector") == 0)
        return g_mapRenderer.InitializeFromStartupArgs();
    if (strcmp(mode, "ttf") == 0)
        return InitializeTTFMode();
    if (strcmp(mode, "text") == 0)
        return InitializeTextMode();
    if (strcmp(mode, "icon") == 0)
        return InitializeIconMode();
    if (strcmp(mode, "ui") == 0)
    {
        BuildImageryBitmapAtlas(&g_uiAtlas);
        return true;
    }
    if (strcmp(mode, "i3d") == 0)
        return InitializeI3DMode();
    if (strcmp(mode, "font") == 0)
        return InitializeFontMode();

    log_error("[test] unknown mode '%s' — falling back to blank", mode);
    return true;
}

void Close(const char* mode)
{
    if (strcmp(mode, "sector") == 0)
        g_mapRenderer.Shutdown();
    DestroyBitmapAtlas(&g_uiAtlas);
}

void Render(const char* mode)
{
    if (!Display || !Display->BackBuffer())
        return;

    if (strcmp(mode, "sector") == 0)
        return g_mapRenderer.RenderFrame();
    if (strcmp(mode, "ui") == 0 && g_uiAtlas.image.id && !g_uiAtlas.items.empty())
        return RenderUiMode();
    if (strcmp(mode, "icon") == 0)
        return RenderIconMode();
    if (strcmp(mode, "ttf") == 0)
        return RenderTTFMode();
    if (strcmp(mode, "text") == 0)
        return RenderTextMode();
    return RenderBlankMode();
}

void HandleMouseClick(const char* mode, int32_t button, int32_t x, int32_t y)
{
    if (strcmp(mode, "sector") != 0) return;
    g_mapRenderer.HandleMouseClick(button, x, y);
}

void HandleMouseMove(const char* mode, int32_t button, int32_t x, int32_t y)
{
    if (strcmp(mode, "sector") != 0) return;
    g_mapRenderer.HandleMouseMove(button, x, y);
}

}  // namespace TestModes
