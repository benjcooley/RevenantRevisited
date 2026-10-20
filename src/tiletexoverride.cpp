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
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

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

} // namespace

int32_t TileTexOverride::Init()
{
    s_root.clear();
    s_root_display.clear();
    s_by_key.clear();
    s_by_name.clear();
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
    log_info("[tiletex] high-res tile art: %d tiles from %s (%d duplicate entries, 'Final' wins)",
             s_count, s_root_display.c_str(), duplicates);
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

    const std::string safe = sanitize_component(type_name);
    if (safe.size() > 200)
        return false;

    char key_buffer[256];
    std::snprintf(key_buffer, sizeof(key_buffer), "%04d_%s", objtype, safe.c_str());
    auto it = s_by_key.find(lower(key_buffer));
    if (it == s_by_key.end())
    {
        // Names are unique per class in retail, but objtype indices are not
        // unique across classes; fall back to the name when it is unambiguous.
        auto named = s_by_name.find(lower(safe));
        if (named == s_by_name.end() || named->second.empty())
        {
            static std::unordered_set<std::string> reported;
            if (reported.insert(lower(safe)).second)
                log_info("[tiletex] no art for '%s' (objtype %d)", safe.c_str(), objtype);
            return false;
        }
        it = s_by_key.find(named->second);
        if (it == s_by_key.end())
            return false;
    }

    std::ifstream file(it->second, std::ios::binary);
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
        log_warn("[tiletex] cannot read %s", to_utf8(it->second).c_str());
        return false;
    }

    int32_t w = 0, h = 0, comp = 0;
    stbi_uc* pixels = stbi_load_from_memory(encoded.data(), int32_t(encoded.size()), &w, &h, &comp, 4);
    if (!pixels || w <= 0 || h <= 0)
    {
        log_warn("[tiletex] undecodable tile art %s", to_utf8(it->second).c_str());
        if (pixels)
            stbi_image_free(pixels);
        return false;
    }

    out.width = w;
    out.height = h;
    out.rgba.assign(pixels, pixels + size_t(w) * size_t(h) * 4);
    stbi_image_free(pixels);
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
