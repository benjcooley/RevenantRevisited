// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorfonts.cpp - Editor font atlas builder                 *
// *************************************************************************

#include "editorfonts.h"

#include "logging.h"

#include <imgui.h>
#include <sokol_gfx.h>

#include <cstdio>
#include <cstdlib>

#ifndef REV_FONTS_DIR
#  define REV_FONTS_DIR "thirdparty/fonts"
#endif

namespace {

ImFont*  s_mono_font     = nullptr;
ImFont*  s_icon_font     = nullptr;
sg_image s_atlas_image   = { SG_INVALID_ID };

// Uploads the current ImGui font atlas as an sg_image and assigns it as
// the atlas's texture id. Replaces whatever sokol_imgui set up (if any).
bool UploadAtlasToSokol(ImFontAtlas* atlas)
{
    unsigned char* pixels = nullptr;
    int w = 0, h = 0;
    atlas->GetTexDataAsRGBA32(&pixels, &w, &h);
    if (!pixels || w <= 0 || h <= 0) return false;

    sg_image_desc d = {};
    d.width        = w;
    d.height       = h;
    d.pixel_format = SG_PIXELFORMAT_RGBA8;
    d.wrap_u       = SG_WRAP_CLAMP_TO_EDGE;
    d.wrap_v       = SG_WRAP_CLAMP_TO_EDGE;
    d.min_filter   = SG_FILTER_LINEAR;
    d.mag_filter   = SG_FILTER_LINEAR;
    d.data.subimage[0][0].ptr  = pixels;
    d.data.subimage[0][0].size = size_t(w) * size_t(h) * 4u;
    d.label = "imgui-fonts";
    s_atlas_image = sg_make_image(&d);
    if (s_atlas_image.id == SG_INVALID_ID) return false;
#if defined(IMGUI_VERSION_NUM) && IMGUI_VERSION_NUM >= 19200
    atlas->SetTexID((ImTextureID)(uintptr_t) s_atlas_image.id);
#else
    atlas->TexID = (ImTextureID)(uintptr_t) s_atlas_image.id;
#endif
    return true;
}

ImFont* AddTTF(ImFontAtlas* atlas, const char* path, float px,
               const ImFontConfig* cfg = nullptr,
               const ImWchar* ranges = nullptr)
{
    ImFont* f = atlas->AddFontFromFileTTF(path, px, cfg, ranges);
    if (!f) log_warn("[fonts] failed to load '%s'", path);
    return f;
}

} // namespace

void EditorFonts::Build()
{
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    atlas->Clear();

    const float kUiPx   = 16.0f;
    const float kIconPx = 16.0f;
    const float kMonoPx = 14.0f;

    // -- Default UI font: Inter ---------------------------------------------
    ImFont* def = AddTTF(atlas, REV_FONTS_DIR "/Inter-Regular.ttf", kUiPx);
    if (!def) {
        atlas->AddFontDefault();
    }

    // -- Material Symbols merged into the default font ----------------------
    // Material Symbols glyphs live in the Unicode Private Use Area
    // 0xE000..0xF8FF; IconsMaterialSymbols.h's macros emit characters in
    // that range. Merging keeps icons on the same line as text.
    {
        static const ImWchar kIconRange[] = { 0xE000, 0xF8FF, 0 };
        ImFontConfig cfg;
        cfg.MergeMode        = true;
        cfg.PixelSnapH       = true;
        cfg.GlyphMinAdvanceX = kIconPx;     // monospace icon column
        cfg.GlyphOffset.y    = 2.0f;        // align baseline visually with text
        AddTTF(atlas, REV_FONTS_DIR "/MaterialSymbolsOutlined.ttf",
               kIconPx, &cfg, kIconRange);
    }

    // -- JetBrains Mono (separate font for log / console) -------------------
    s_mono_font = AddTTF(atlas, REV_FONTS_DIR "/JetBrainsMono-Regular.ttf", kMonoPx);
    if (!s_mono_font) s_mono_font = atlas->Fonts.empty() ? nullptr : atlas->Fonts[0];

    // -- Material Symbols at 28px for toolbar / icon-only buttons ----------
    // Material Symbols glyphs are designed inside the EM box and end up
    // visually low when ImGui centers them by line height; nudge up.
    {
        static const ImWchar kIconRange[] = { 0xE000, 0xF8FF, 0 };
        ImFontConfig cfg;
        cfg.PixelSnapH       = true;
        cfg.GlyphMinAdvanceX = 28.0f;
        cfg.GlyphOffset.y    = -3.0f;
        s_icon_font = AddTTF(atlas, REV_FONTS_DIR "/MaterialSymbolsOutlined.ttf",
                             28.0f, &cfg, kIconRange);
    }

    if (!UploadAtlasToSokol(atlas))
        log_error("[fonts] atlas upload failed -- text will be blank");
    else
        log_info("[fonts] atlas built: %d fonts uploaded to gpu", atlas->Fonts.Size);
}

ImFont* EditorFonts::GetMonoFont()
{
    if (s_mono_font) return s_mono_font;
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    return atlas->Fonts.empty() ? nullptr : atlas->Fonts[0];
}

ImFont* EditorFonts::GetIconFont()
{
    if (s_icon_font) return s_icon_font;
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    return atlas->Fonts.empty() ? nullptr : atlas->Fonts[0];
}
