// Revenant Revisited port - modern sprite art lookup. See spritetexoverride.h.

#include "spritetexoverride.h"

#include "3dimage.h"
#include "imagery.h"
#include "logging.h"
#include "revenant.h"
#include "revisited_settings.h"
#include "revutils.h"

#include <stb_image.h>

#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::string lower(std::string s)
{
    for (char& ch : s)
        ch = char(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

// The path's own text as UTF-8, for the log.
std::string to_utf8(const std::filesystem::path& p)
{
#if defined(__cpp_lib_char8_t)
    const std::u8string u8 = p.u8string();
    return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
#else
    return p.u8string();
#endif
}

// The ini is read as UTF-8 (simpleini SetUnicode), so the art root may hold
// non-ASCII directories. Build the path from that UTF-8 text, whatever the
// platform's native encoding is.
std::filesystem::path path_from_utf8(const std::string& s)
{
    if (s.empty())
        return {};
#if defined(__cpp_lib_char8_t)
    std::u8string u8;
    u8.reserve(s.size());
    for (const char ch : s)
        u8.push_back(static_cast<char8_t>(static_cast<unsigned char>(ch)));
    return std::filesystem::path(u8);
#else
    return std::filesystem::u8path(s);
#endif
}

// The PNG's own colour type: 4 (grey + alpha) and 6 (RGBA) carry an alpha
// channel, 2 (RGB) and 3 (palette) do not. The IHDR is the ninth chunk of
// a PNG, its colour type the byte at 25.
bool PngHasAlphaChannel(const std::vector<uint8_t>& png)
{
    static const uint8_t kSignature[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
    if (png.size() < 26 || std::memcmp(png.data(), kSignature, sizeof(kSignature)) != 0)
        return false;
    const uint8_t color_type = png[25];
    return color_type == 4 || color_type == 6;
}

// "texture_00_frame_03.png" -> (0, 3);  "texture_00.png" -> (0, -1), the
// art for every frame of that texture.
bool parse_texture_name(const std::string& filename, int32_t& tex, int32_t& frame)
{
    if (filename.size() < 4)
        return false;
    std::string name = lower(filename);
    if (name.compare(name.size() - 4, 4, ".png") != 0)
        return false;
    name = name.substr(0, name.size() - 4);
    static const char* kPrefix = "texture_";
    if (name.size() <= 8 || name.compare(0, 8, kPrefix) != 0)
        return false;
    name = name.substr(8);

    size_t i = 0;
    auto read_number = [&name, &i](int32_t& out) -> bool {
        if (i >= name.size() || !std::isdigit(static_cast<unsigned char>(name[i])))
            return false;
        // More digits than an int32 can hold is not a real index; reject it
        // instead of wrapping negative. Only the digit run counts: the name
        // continues after this number ("texture_00_frame_00").
        size_t j = i;
        while (j < name.size() && std::isdigit(static_cast<unsigned char>(name[j])))
            ++j;
        if (j - i > 9)
            return false;
        int32_t value = 0;
        while (i < j)
            value = value * 10 + (name[i++] - '0');
        out = value;
        return true;
    };

    if (!read_number(tex))
        return false;
    frame = -1;
    if (i == name.size())
        return true;
    if (name.compare(i, 7, "_frame_") != 0)
        return false;
    i += 7;
    return read_number(frame);
}

// "<folder>|<texture index>|<frame index>"
std::string make_key(const std::string& folder, int32_t tex, int32_t frame)
{
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "|%d|%d", tex, frame);
    return folder + buffer;
}

std::filesystem::path s_root;
std::string  s_root_display;
std::string  s_art_path_display;
bool         s_enabled = false;
int32_t      s_count = 0;
std::unordered_map<std::string, std::filesystem::path> s_by_key;

// The folders an imagery file's art may live in, most specific first:
// "Chars\locke.i3d" -> "chars/locke", "chars/locke.i3d", "locke",
// "locke.i3d".
std::vector<std::string> candidate_folders(const char* imagery_filename)
{
    std::vector<std::string> folders;
    if (!imagery_filename || !*imagery_filename)
        return folders;

    std::string norm = imagery_filename;
    for (char& ch : norm)
        if (ch == '\\') ch = '/';
    norm = lower(norm);

    std::string no_ext = norm;
    const size_t dot   = no_ext.find_last_of('.');
    const size_t slash = no_ext.find_last_of('/');
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
        no_ext = no_ext.substr(0, dot);

    folders.push_back(no_ext);
    folders.push_back(norm);

    const size_t base_slash = no_ext.find_last_of('/');
    const std::string base = (base_slash == std::string::npos) ? no_ext : no_ext.substr(base_slash + 1);
    folders.push_back(base);
    folders.push_back((base_slash == std::string::npos) ? norm : norm.substr(base_slash + 1));
    return folders;
}

const std::filesystem::path* FindArt(const char* imagery_filename, int32_t tex, int32_t frame)
{
    for (const std::string& folder : candidate_folders(imagery_filename))
    {
        auto it = s_by_key.find(make_key(folder, tex, frame));
        if (it != s_by_key.end())
            return &it->second;
        it = s_by_key.find(make_key(folder, tex, -1)); // the art for every frame
        if (it != s_by_key.end())
            return &it->second;
    }
    return nullptr;
}

// Read a PNG file into an SImage. Returns false when the file is missing or
// undecodable.
bool DecodePng(const std::filesystem::path& path, SpriteTexOverride::SImage& out)
{
    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> encoded;
    if (file)
    {
        file.seekg(0, std::ios::end);
        const std::streamoff size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (size > 0 && size < (64 << 20))
        {
            encoded.resize(size_t(size));
            file.read(reinterpret_cast<char*>(encoded.data()), size);
        }
    }
    if (encoded.empty())
    {
        log_warn("[spriteart] cannot read %s", to_utf8(path).c_str());
        return false;
    }

    int32_t w = 0, h = 0, comp = 0;
    stbi_uc* pixels = stbi_load_from_memory(encoded.data(), int32_t(encoded.size()), &w, &h, &comp, 4);
    if (!pixels || w <= 0 || h <= 0)
    {
        log_warn("[spriteart] undecodable sprite art %s", to_utf8(path).c_str());
        if (pixels)
            stbi_image_free(pixels);
        return false;
    }

    out.width = w;
    out.height = h;
    out.rgba.assign(pixels, pixels + size_t(w) * size_t(h) * 4);
    stbi_image_free(pixels);

    // The alpha counts as the art's own when the PNG carries one: a colour
    // type with an alpha channel (grey+alpha, RGBA), or a palette PNG with
    // tRNS (which decodes to alpha below 255), or any pixel that is not
    // fully opaque.
    out.has_alpha = PngHasAlphaChannel(encoded);
    if (!out.has_alpha)
    {
        for (size_t i = 3; i < out.rgba.size(); i += 4)
        {
            if (out.rgba[i] < 255)
            {
                out.has_alpha = true;
                break;
            }
        }
    }
    return true;
}

} // namespace

int32_t SpriteTexOverride::Init()
{
    s_root.clear();
    s_root_display.clear();
    s_by_key.clear();
    s_count = 0;
    s_enabled = false;

    // The ini keys (SpriteOverride / SpriteRoot) were read once at boot (the
    // gated loader with --revisited, revmain's explicit read without it). A
    // re-init during the session -- the art reload -- must not pull them
    // from the ini again: the debug panel edits the root and the mode live.
    if (!RevisitedSettings.sprite_override)
    {
        log_info("[spriteart] sprite art override disabled (SpriteOverride=0)");
        return 0;
    }

    namespace fs = std::filesystem;
    fs::path root;
    if (!RevisitedSettings.sprite_root.empty())
        root = path_from_utf8(RevisitedSettings.sprite_root);
    else if (const char* overlay = rev_resolve_revisited_overlay(); overlay && *overlay)
        root = fs::path(overlay) / "Sprites";
    else if (!RevisitedSettings.high_res_tile_root.empty())
        root = path_from_utf8(RevisitedSettings.high_res_tile_root) / "sprites";
    else
        return 0;

    std::error_code ec;
    s_root = root;
    s_root_display = to_utf8(root);
    if (!fs::is_directory(root, ec))
    {
        log_info("[spriteart] no sprite art tree at %s", s_root_display.c_str());
        return 0;
    }

    int32_t duplicates = 0;
    std::error_code walk_ec;
    for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, walk_ec), end;
         it != end; it.increment(walk_ec))
    {
        if (walk_ec)
        {
            log_warn("[spriteart] scan stopped at %s: %s", to_utf8(it->path()).c_str(), walk_ec.message().c_str());
            break;
        }
        const fs::directory_entry& de = *it;
        if (de.is_directory(walk_ec))
            continue;
        if (!de.is_regular_file(walk_ec))
            continue;

        int32_t tex = 0, frame = 0;
        if (!parse_texture_name(de.path().filename().string(), tex, frame))
            continue;

        // The folder the art sits in is the imagery's path without its
        // extension: "<root>/chars/locke" -> "chars/locke".
        std::string folder = lower(de.path().parent_path().lexically_relative(root).generic_string());
        while (!folder.empty() && folder.front() == '/')
            folder.erase(folder.begin());
        while (!folder.empty() && folder.back() == '/')
            folder.pop_back();
        if (folder == ".")
            folder.clear();

        const std::string key = make_key(folder, tex, frame);
        auto found = s_by_key.find(key);
        if (found == s_by_key.end())
            s_by_key.emplace(key, de.path());
        else
            ++duplicates;
    }

    s_count = int32_t(s_by_key.size());
    if (s_count == 0)
    {
        log_info("[spriteart] no sprite art under %s", s_root_display.c_str());
        return 0;
    }

    s_enabled = true;
    log_info("[spriteart] %d sprite texture(s) from %s (%d duplicate entries)",
             s_count, s_root_display.c_str(), duplicates);
    return s_count;
}

bool SpriteTexOverride::Enabled()
{
    return s_enabled;
}

bool SpriteTexOverride::Active()
{
    return s_enabled && RevisitedSettings.sprite_mode != 0;
}

const char* SpriteTexOverride::Root()
{
    return s_root_display.c_str();
}

int32_t SpriteTexOverride::Count()
{
    return s_count;
}

bool SpriteTexOverride::Has(const char* imagery_filename, int32_t texture_index, int32_t frame_index)
{
    if (!s_enabled)
        return false;
    return FindArt(imagery_filename, texture_index, frame_index) != nullptr;
}

const char* SpriteTexOverride::ArtPath(const char* imagery_filename, int32_t texture_index, int32_t frame_index)
{
    if (!s_enabled)
        return "";
    const std::filesystem::path* art = FindArt(imagery_filename, texture_index, frame_index);
    if (!art)
        return "";
    s_art_path_display = to_utf8(*art);
    return s_art_path_display.c_str();
}

bool SpriteTexOverride::Load(const char* imagery_filename, int32_t texture_index, int32_t frame_index,
                             SImage& out)
{
    if (!s_enabled)
        return false;
    const std::filesystem::path* art = FindArt(imagery_filename, texture_index, frame_index);
    if (!art)
        return false;
    return DecodePng(*art, out);
}

int32_t SpriteReload::ReloadAll(int32_t* out_images)
{
    int32_t frames = 0;
    int32_t images = 0;
    int32_t visited = 0;
    int32_t with_art = 0;

    // Every loaded 3D imagery: the characters, monsters, props and effect
    // billboards on screen are all T3DImagery behind the object registry.
    for (int32_t id = 0; id < TObjectImagery::NumEntries(); ++id)
    {
        if (!TObjectImagery::IsUsed(id))
            continue;
        SImageryEntry* entry = TObjectImagery::GetImageryEntry(id);
        if (!entry || !entry->imagery)
            continue;
        T3DImagery* img3d = dynamic_cast<T3DImagery*>(entry->imagery);
        if (!img3d)
            continue;
        ++visited;
        const int32_t updated = img3d->ReloadTextures();
        if (updated > 0)
        {
            frames += updated;
            ++images;
            ++with_art;
        }
    }

    log_info("[spriteart] reload: %d imagery visited, %d with art, %d texture frame(s) updated in %d image(s)",
             visited, with_art, frames, images);
    if (out_images)
        *out_images = images;
    return frames;
}

// Switch the art mode and reload. The one place the mode change happens, so
// the debug panel's radio buttons and the console command can not drift
// apart: re-index the tree (files may have been added since the last index)
// and then put the wanted art into the textures the game already uses.
int32_t SpriteReload::ApplyMode(int32_t mode, int32_t* out_images)
{
    log_info("[spriteart] art mode: %s", (mode != 0) ? "modern" : "retail");
    // Re-index first, and set the mode only then: without --revisited the
    // index reads the ini's art keys, which must not overwrite the mode
    // this reload was asked for.
    SpriteTexOverride::Init();
    RevisitedSettings.sprite_mode = (mode != 0) ? 1 : 0;
    return ReloadAll(out_images);
}
