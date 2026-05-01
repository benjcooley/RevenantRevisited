// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editoricons.cpp - Object-category icons (PNG -> sg_image)   *
// *************************************************************************

#include "editoricons.h"

#include "logging.h"
#include "object.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <array>
#include <cstdio>
#include <cstring>

#ifndef REV_EDITOR_DATA_DIR
#  define REV_EDITOR_DATA_DIR "data/editor"
#endif

namespace {

// Cached images keyed by category name.
struct SCat {
    const char* name;
    sg_image    img = { SG_INVALID_ID };
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

sg_image LoadPNG(const char* relpath)
{
    char path[1024];
    std::snprintf(path, sizeof(path), "%s/icons/%s", REV_EDITOR_DATA_DIR, relpath);

    int w = 0, h = 0, n = 0;
    stbi_uc* px = stbi_load(path, &w, &h, &n, 4);
    if (!px) {
        log_warn("[icons] failed to load '%s' (%s)", path, stbi_failure_reason());
        return sg_image{ SG_INVALID_ID };
    }

    // Generate a full mip chain via stb_image_resize2 so 128x128 source
    // textures still look clean when ImGui draws them at 16-32 px.
    constexpr int kMaxMips = 12;
    stbi_uc* mips[kMaxMips] = {};
    int      mip_w[kMaxMips] = {}, mip_h[kMaxMips] = {};
    int      num_mips = 0;
    mips[0]  = px;
    mip_w[0] = w;
    mip_h[0] = h;
    num_mips = 1;
    while (num_mips < kMaxMips
           && (mip_w[num_mips - 1] > 1 || mip_h[num_mips - 1] > 1))
    {
        const int prev_w = mip_w[num_mips - 1];
        const int prev_h = mip_h[num_mips - 1];
        const int next_w = (prev_w > 1) ? prev_w / 2 : 1;
        const int next_h = (prev_h > 1) ? prev_h / 2 : 1;
        stbi_uc* dst = (stbi_uc*)malloc(size_t(next_w) * size_t(next_h) * 4u);
        if (!dst) break;
        stbir_resize_uint8_srgb(mips[num_mips - 1], prev_w, prev_h, 0,
                                dst, next_w, next_h, 0,
                                STBIR_RGBA);
        mips[num_mips]  = dst;
        mip_w[num_mips] = next_w;
        mip_h[num_mips] = next_h;
        ++num_mips;
    }

    sg_image_desc d = {};
    d.width        = w;
    d.height       = h;
    d.num_mipmaps  = num_mips;
    d.pixel_format = SG_PIXELFORMAT_RGBA8;
    d.wrap_u       = SG_WRAP_CLAMP_TO_EDGE;
    d.wrap_v       = SG_WRAP_CLAMP_TO_EDGE;
    d.min_filter   = SG_FILTER_LINEAR_MIPMAP_LINEAR;
    d.mag_filter   = SG_FILTER_LINEAR;
    for (int i = 0; i < num_mips; ++i) {
        d.data.subimage[0][i].ptr  = mips[i];
        d.data.subimage[0][i].size = size_t(mip_w[i]) * size_t(mip_h[i]) * 4u;
    }
    d.label = relpath;
    sg_image img = sg_make_image(&d);

    // Free derived mips; the source mip[0] is owned by stbi.
    for (int i = 1; i < num_mips; ++i) free(mips[i]);
    stbi_image_free(px);

    if (img.id == SG_INVALID_ID)
        log_warn("[icons] sg_make_image failed for '%s'", path);
    return img;
}

void Free(SCat& c)
{
    if (c.img.id != SG_INVALID_ID) {
        sg_destroy_image(c.img);
        c.img = sg_image{ SG_INVALID_ID };
    }
}

} // namespace

void EditorIcons::Build()
{
    s_item.img        = LoadPNG("item.png");
    s_container.img   = LoadPNG("container.png");
    s_lightsource.img = LoadPNG("lightsource.png");
    s_tile.img        = LoadPNG("tile.png");
    s_exit.img        = LoadPNG("exit.png");
    s_player.img      = LoadPNG("player.png");
    s_character.img   = LoadPNG("character.png");
    s_trap.img        = LoadPNG("trap.png");
    s_effect.img      = LoadPNG("effect.png");
    s_helper.img      = LoadPNG("helper.png");
    // Sector falls back to tile if no dedicated icon exists.
    s_sector.img      = s_tile.img;
}

void EditorIcons::Shutdown()
{
    // s_sector aliases tile; clear without a double-destroy.
    s_sector.img = sg_image{ SG_INVALID_ID };
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

sg_image EditorIcons::ForObjClass(int32_t c)
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
        case OBJCLASS_MAPSCROLL:    return s_item.img;
        case OBJCLASS_CONTAINER:    return s_container.img;
        case OBJCLASS_LIGHTSOURCE:  return s_lightsource.img;
        case OBJCLASS_TILE:         return s_tile.img;
        case OBJCLASS_EXIT:         return s_exit.img;
        case OBJCLASS_PLAYER:       return s_player.img;
        case OBJCLASS_CHARACTER:    return s_character.img;
        case OBJCLASS_TRAP:         return s_trap.img;
        case OBJCLASS_EFFECT:       return s_effect.img;
        case OBJCLASS_SHADOW:
        case OBJCLASS_HELPER:       return s_helper.img;
        default:                    return sg_image{ SG_INVALID_ID };
    }
}

sg_image EditorIcons::Sector() { return s_sector.img; }
