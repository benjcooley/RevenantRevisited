// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        editoricons.cpp - Object-category icons (PNG -> renderer tex)  *
// *************************************************************************

#include "editoricons.h"

#include "logging.h"
#include "object.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#include <cstdio>

#ifndef REV_EDITOR_DATA_DIR
#  define REV_EDITOR_DATA_DIR "data/editor"
#endif

namespace {

// Cached images keyed by category name.
struct SCat {
    const char* name;
    TTextureHandle texture = kInvalidTexture;
};

SCat s_item;
SCat s_container;
SCat s_lightsource;
SCat s_tile;
SCat s_exit;
SCat s_player;
SCat s_character;
SCat s_trap;
SCat s_effect;
SCat s_helper;
SCat s_sector;

TTextureHandle LoadPNG(const char* relpath)
{
    char path[1024];
    std::snprintf(path, sizeof(path), "%s/icons/%s", REV_EDITOR_DATA_DIR, relpath);

    int w = 0, h = 0, n = 0;
    stbi_uc* px = stbi_load(path, &w, &h, &n, 4);
    if (!px) {
        log_warn("[icons] failed to load '%s' (%s)", path, stbi_failure_reason());
        return kInvalidTexture;
    }

    TTextureHandle texture = kInvalidTexture;
    if (Renderer)
    {
        const size_t bytes = size_t(w) * size_t(h) * 4u;
        texture = Renderer->RegisterTextureAsset(0,
                                                 px,
                                                 bytes,
                                                 w,
                                                 h,
                                                 ERendererTextureFormat::RGBA8,
                                                 bytes,
                                                 ERendererTextureFilter::Linear);
        if (texture != kInvalidTexture)
            Renderer->AddTextureAssetRef(texture);
    }
    stbi_image_free(px);

    if (texture == kInvalidTexture)
        log_warn("[icons] texture upload failed for '%s'", path);
    return texture;
}

void Free(SCat& c)
{
    if (Renderer && c.texture != kInvalidTexture) {
        Renderer->ReleaseTextureAssetRef(c.texture);
        c.texture = kInvalidTexture;
    }
}

} // namespace

void EditorIcons::Build()
{
    Shutdown();

    s_item.texture        = LoadPNG("item.png");
    s_container.texture   = LoadPNG("container.png");
    s_lightsource.texture = LoadPNG("lightsource.png");
    s_tile.texture        = LoadPNG("tile.png");
    s_exit.texture        = LoadPNG("exit.png");
    s_player.texture      = LoadPNG("player.png");
    s_character.texture   = LoadPNG("character.png");
    s_trap.texture        = LoadPNG("trap.png");
    s_effect.texture      = LoadPNG("effect.png");
    s_helper.texture      = LoadPNG("helper.png");
    // Sector falls back to tile if no dedicated icon exists.
    s_sector.texture      = s_tile.texture;
}

void EditorIcons::Shutdown()
{
    // s_sector aliases tile; clear without a double-destroy.
    s_sector.texture = kInvalidTexture;
    Free(s_item);
    Free(s_container);
    Free(s_lightsource);
    Free(s_tile);
    Free(s_exit);
    Free(s_player);
    Free(s_character);
    Free(s_trap);
    Free(s_effect);
    Free(s_helper);
}

TTextureHandle EditorIcons::ForObjClass(int32_t c)
{
    // Inventory-bearing classes all collapse onto ITEM.
    switch (c) {
        case OBJCLASS_ITEM:
        case OBJCLASS_WEAPON:
        case OBJCLASS_ARMOR:
        case OBJCLASS_TALISMAN:
        case OBJCLASS_FOOD:
        case OBJCLASS_TOOL:
        case OBJCLASS_MONEY:
        case OBJCLASS_KEY:
        case OBJCLASS_INVCONTAINER:
        case OBJCLASS_POTION:
        case OBJCLASS_AMMO:
        case OBJCLASS_SCROLL:
        case OBJCLASS_RANGEDWEAPON:
        case OBJCLASS_MAPSCROLL:    return s_item.texture;
        case OBJCLASS_CONTAINER:    return s_container.texture;
        case OBJCLASS_LIGHTSOURCE:  return s_lightsource.texture;
        case OBJCLASS_TILE:         return s_tile.texture;
        case OBJCLASS_EXIT:         return s_exit.texture;
        case OBJCLASS_PLAYER:       return s_player.texture;
        case OBJCLASS_CHARACTER:    return s_character.texture;
        case OBJCLASS_TRAP:         return s_trap.texture;
        case OBJCLASS_EFFECT:       return s_effect.texture;
        case OBJCLASS_SHADOW:
        case OBJCLASS_HELPER:       return s_helper.texture;
        default:                    return kInvalidTexture;
    }
}

TTextureHandle EditorIcons::Sector() { return s_sector.texture; }
