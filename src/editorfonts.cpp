// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorfonts.cpp - Editor font atlas builder                 *
// *************************************************************************

#include "editorfonts.h"

#include "logging.h"
#include "renderer.h"

#include <imgui.h>

#ifndef REV_FONTS_DIR
#  define REV_FONTS_DIR "thirdparty/fonts"
#endif

namespace {

ImFont*  s_mono_font     = nullptr;
ImFont*  s_icon_font     = nullptr;
TTextureHandle s_atlas_texture = kInvalidTexture;

// Uploads the current ImGui font atlas through the renderer and assigns the
// backend texture id ImGui needs for draw commands.
bool UploadAtlasToRenderer(ImFontAtlas* atlas)
{
    unsigned char* pixels = nullptr;
    int w = 0, h = 0;
    atlas->GetTexDataAsRGBA32(&pixels, &w, &h);
    if (!Renderer || !pixels || w <= 0 || h <= 0) return false;

    const size_t bytes = size_t(w) * size_t(h) * 4u;
    s_atlas_texture = Renderer->RegisterTextureAsset(0,
                                                     pixels,
                                                     bytes,
                                                     w,
                                                     h,
                                                     ERendererTextureFormat::RGBA8,
                                                     bytes,
                                                     ERendererTextureFilter::Linear);
    if (s_atlas_texture == kInvalidTexture) return false;
    Renderer->AddTextureAssetRef(s_atlas_texture);

    const ImTextureID imgui_id = (ImTextureID)Renderer->TextureImGuiId(s_atlas_texture);
#if defined(IMGUI_VERSION_NUM) && IMGUI_VERSION_NUM >= 19200
    atlas->SetTexID(imgui_id);
#else
    atlas->TexID = imgui_id;
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
    Shutdown();

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

    if (!UploadAtlasToRenderer(atlas))
        log_error("[fonts] atlas upload failed -- text will be blank");
    else
        log_info("[fonts] atlas built: %d fonts uploaded to gpu", atlas->Fonts.Size);
}

void EditorFonts::Shutdown()
{
    if (Renderer && s_atlas_texture != kInvalidTexture)
        Renderer->ReleaseTextureAssetRef(s_atlas_texture);
    s_atlas_texture = kInvalidTexture;
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
