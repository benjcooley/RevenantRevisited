// Revenant Revisited port - high-res tile art lookup. See tiletexoverride.h.

#include "tiletexoverride.h"

#include "logging.h"
#include "revenant.h"
#include "revisited_settings.h"
#include "revutils.h"

#include <stb_image.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

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

namespace {

std::string lower(std::string s)
{
    for (char& ch : s)
        ch = char(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

// Same mapping the tile dumper applies before writing "<NNNN>_<Name>.png"
// (testmodes.cpp SanitizeFilenameComponent), so engine names and file names
// agree even when the type name carries punctuation.
std::string sanitize_component(const char* name)
{
    std::string s = name ? name : "unnamed";
    for (char& ch : s)
    {
        const unsigned char u = static_cast<unsigned char>(ch);
        if (!(std::isalnum(u) || ch == '_' || ch == '-' || ch == '.'))
            ch = '_';
    }
    if (s.empty())
        s = "unnamed";
    return s;
}

std::string to_utf8(const std::filesystem::path& p)
{
    const std::wstring w = p.wstring();
    if (w.empty())
        return {};
    const int32_t len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), int32_t(w.size()),
                                            nullptr, 0, nullptr, nullptr);
    if (len <= 0)
        return {};
    std::string out(size_t(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), int32_t(w.size()), out.data(), len, nullptr, nullptr);
    return out;
}

// The ini is read as UTF-8 (simpleini SetUnicode), so the tiles root may hold
// non-ASCII directories (the upscale working trees are named in Russian).
// Build a wide fs::path from it when the narrow interpretation finds nothing.
std::filesystem::path path_from_utf8(const std::string& s)
{
    if (s.empty())
        return {};
    std::error_code ec;
    std::filesystem::path narrow(s);
    if (std::filesystem::exists(narrow, ec))
        return narrow;

    const int32_t len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), int32_t(s.size()), nullptr, 0);
    if (len > 0)
    {
        std::wstring w(size_t(len), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), int32_t(s.size()), w.data(), len);
        std::filesystem::path wide(w);
        if (std::filesystem::exists(wide, ec))
            return wide;
        return wide;
    }
    return narrow;
}

// "<NNNN>_<Name>.png" -> lowercase stem, or "" when the file is not one of
// ours (masks, visualisations, stray art).
std::string tile_stem(const std::string& filename)
{
    const size_t n = filename.size();
    if (n < 10)
        return {};
    if (lower(filename.substr(n - 4)) != ".png")
        return {};
    for (int32_t i = 0; i < 4; ++i)
        if (!std::isdigit(static_cast<unsigned char>(filename[size_t(i)])))
            return {};
    if (filename[4] != '_')
        return {};
    for (size_t i = 5; i < n - 4; ++i)
    {
        const char ch = filename[i];
        if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-' || ch == '.'))
            return {};
    }
    return lower(filename.substr(0, n - 4));
}

// "Final" holds the hand-finished picks, "Remade" the hand-remade art; both
// beat the bulk upscale when the same tile exists in several sets.
bool preferred_folder(const std::filesystem::path& p)
{
    for (const auto& part : p)
    {
        const std::string s = lower(part.string());
        if (s == "final" || s == "remade")
            return true;
    }
    return false;
}

// Directories that hold something other than tile albedo art.
bool skipped_dir(const std::string& name)
{
    static const char* kSkip[] = {"alpha", "depth", "remade", "original", "gemini", "raw", "icons"};
    const std::string lowered = lower(name);
    for (const char* skip : kSkip)
        if (lowered == skip)
            return true;
    return false;
}

std::filesystem::path s_root;
std::string  s_root_display;
bool         s_enabled = false;
int32_t      s_count = 0;
std::unordered_map<std::string, std::filesystem::path> s_by_key;  // "0763_kep" -> file
std::unordered_map<std::string, std::string> s_by_name;           // "kep" -> key ("" when ambiguous)
std::unordered_map<std::string, std::filesystem::path> s_depth_by_key;  // "0763_kep" -> depth png

// The art file for a tile, or nullptr when the tree has none. Names are
// unique per class in retail, but objtype indices are not unique across
// classes, so the name is the fallback when it is unambiguous.
const std::filesystem::path* FindArt(int32_t objtype, const char* type_name)
{
    const std::string safe = sanitize_component(type_name);
    if (safe.empty() || safe.size() > 200)
        return nullptr;

    char key_buffer[256];
    std::snprintf(key_buffer, sizeof(key_buffer), "%04d_%s", objtype, safe.c_str());
    auto it = s_by_key.find(lower(key_buffer));
    if (it != s_by_key.end())
        return &it->second;

    auto named = s_by_name.find(lower(safe));
    if (named == s_by_name.end() || named->second.empty())
        return nullptr;
    it = s_by_key.find(named->second);
    if (it == s_by_key.end())
        return nullptr;
    return &it->second;
}

// Read a PNG file into an SImage. Returns false when the file is missing or
// undecodable.
bool DecodePng(const std::filesystem::path& path, TileTexOverride::SImage& out)
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
        log_warn("[tiletex] cannot read %s", to_utf8(path).c_str());
        return false;
    }

    int32_t w = 0, h = 0, comp = 0;
    stbi_uc* pixels = stbi_load_from_memory(encoded.data(), int32_t(encoded.size()), &w, &h, &comp, 4);
    if (!pixels || w <= 0 || h <= 0)
    {
        log_warn("[tiletex] undecodable tile art %s", to_utf8(path).c_str());
        if (pixels)
            stbi_image_free(pixels);
        return false;
    }

    out.width = w;
    out.height = h;
    out.rgba.assign(pixels, pixels + size_t(w) * size_t(h) * 4);
    stbi_image_free(pixels);
    // The alpha counts as the art's own when the PNG carries one: a colour
    // type with an alpha channel (grey+alpha, RGBA), or a palette with a
    // transparent entry, or any pixel that is not fully opaque. The colour
    // type alone is not enough -- a palette PNG with tRNS decodes to alpha
    // below 255, and the art's shape lives in that alpha.
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

int32_t TileTexOverride::Init()
{
    s_root.clear();
    s_root_display.clear();
    s_by_key.clear();
    s_by_name.clear();
    s_depth_by_key.clear();
    s_count = 0;
    s_enabled = false;

    if (!RevisitedEnabled && !LoadTileArtOverrideSettings())
        return 0;
    if (!RevisitedSettings.high_res_tiles)
    {
        log_info("[tiletex] high-res tile art disabled (HighResTiles=0)");
        return 0;
    }

    namespace fs = std::filesystem;
    fs::path root;
    if (!RevisitedSettings.high_res_tile_root.empty())
        root = path_from_utf8(RevisitedSettings.high_res_tile_root);
    else if (const char* overlay = rev_resolve_revisited_overlay(); overlay && *overlay)
        root = fs::path(overlay) / "Imagery";
    else
        return 0;

    std::error_code ec;
    if (!fs::is_directory(root, ec))
    {
        log_info("[tiletex] no high-res tile tree at %s", to_utf8(root).c_str());
        return 0;
    }

    int32_t duplicates = 0;
    std::error_code walk_ec;
    for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, walk_ec), end;
         it != end; it.increment(walk_ec))
    {
        if (walk_ec)
        {
            log_warn("[tiletex] scan stopped at %s: %s", to_utf8(it->path()).c_str(), walk_ec.message().c_str());
            break;
        }
        const fs::directory_entry& de = *it;
        if (de.is_directory(walk_ec))
        {
            const std::string dir_name = lower(de.path().filename().string());
            if (dir_name == "depth")
            {
                // Depth maps carry the tile's relief, not its colour. They
                // live in a "depth" folder next to the art and are named
                // "<NNNN>_<Name>_depth.png"; index them on their own key.
                std::error_code fec;
                for (fs::directory_iterator fit(de.path(),
                                                fs::directory_options::skip_permission_denied, fec),
                     fend;
                     fit != fend; fit.increment(fec))
                {
                    if (fec)
                        break;
                    if (!fit->is_regular_file(fec))
                        continue;
                    const std::string dstem = tile_stem(fit->path().filename().string());
                    if (dstem.empty() || dstem.size() < 7 ||
                        dstem.compare(dstem.size() - 6, 6, "_depth") != 0)
                        continue;
                    const std::string key = dstem.substr(0, dstem.size() - 6);
                    auto found = s_depth_by_key.find(key);
                    if (found == s_depth_by_key.end())
                        s_depth_by_key.emplace(key, fit->path());
                    else if (preferred_folder(fit->path()) && !preferred_folder(found->second))
                        found->second = fit->path();
                }
                it.disable_recursion_pending();
                continue;
            }
            if (skipped_dir(de.path().filename().string()))
                it.disable_recursion_pending();
            continue;
        }
        if (!de.is_regular_file(walk_ec))
            continue;

        const std::string stem = tile_stem(de.path().filename().string());
        if (stem.empty())
            continue;

        auto found = s_by_key.find(stem);
        if (found == s_by_key.end())
        {
            s_by_key.emplace(stem, de.path());
        }
        else if (preferred_folder(de.path()) && !preferred_folder(found->second))
        {
            found->second = de.path(); // hand-finished art wins over the bulk set
            ++duplicates;
        }
        else
        {
            ++duplicates;
        }

        const size_t underscore = stem.find('_');
        if (underscore != std::string::npos)
        {
            const std::string name = stem.substr(underscore + 1);
            auto named = s_by_name.find(name);
            if (named == s_by_name.end())
                s_by_name.emplace(name, stem);
            else if (named->second != stem)
                named->second.clear(); // same name, several objtypes: objtype decides
        }
    }

    s_count = int32_t(s_by_key.size());
    if (s_count == 0)
    {
        log_info("[tiletex] no high-res tile art under %s", to_utf8(root).c_str());
        return 0;
    }

    s_root = root;
    s_root_display = to_utf8(root);
    s_enabled = true;
    log_info("[tiletex] high-res tile art: %d tiles from %s (%d duplicate entries, 'Final' wins, %d depth maps)",
             s_count, s_root_display.c_str(), duplicates, int32_t(s_depth_by_key.size()));
    return s_count;
}

bool TileTexOverride::Enabled()
{
    return s_enabled;
}

bool TileTexOverride::Load(int32_t objtype, const char* type_name, SImage& out)
{
    if (!s_enabled || objtype < 0 || !type_name || !*type_name)
        return false;

    const std::filesystem::path* art = FindArt(objtype, type_name);
    if (!art)
    {
        static std::unordered_set<std::string> reported;
        if (reported.insert(lower(sanitize_component(type_name))).second)
            log_info("[tiletex] no art for '%s' (objtype %d)", type_name, objtype);
        return false;
    }
    return DecodePng(*art, out);
}

bool TileTexOverride::LoadDepth(int32_t objtype, const char* type_name, SImage& out)
{
    if (!s_enabled || objtype < 0 || !type_name || !*type_name)
        return false;

    const std::filesystem::path* art = FindArt(objtype, type_name);
    if (!art)
        return false;

    // The map sits in a "depth" folder next to the art and is named after it.
    const std::filesystem::path depth_path =
        art->parent_path() / "depth" / std::filesystem::path(art->stem().string() + "_depth.png");
    auto it = s_depth_by_key.find(lower(art->stem().string()));
    if (it != s_depth_by_key.end())
    {
        if (!DecodePng(it->second, out))
            return false;
    }
    else
    {
        std::error_code ec;
        if (!std::filesystem::exists(depth_path, ec))
            return false;
        if (!DecodePng(depth_path, out))
            return false;
    }
    out.is_depth = true;
    return true;
}

const char* TileTexOverride::Root()
{
    return s_root_display.c_str();
}

int32_t TileTexOverride::Count()
{
    return s_count;
}
