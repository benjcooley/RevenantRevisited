// Generated baseline defaults — see revisited_defaults.h.
//
// The blocks fenced by BEGIN BAKED / END BAKED markers are auto-managed
// by BakeRevisitedDefaultsToSource (debug panel button). Hand edits
// inside those fences will be silently overwritten on the next bake.
// Hand-editable code lives outside the fences.

#include "revisited_defaults.h"

#include "area.h"
#include "logging.h"
#include "revisited_settings.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#if defined(__APPLE__)
#  include <mach-o/dyld.h>   // _NSGetExecutablePath
#endif

namespace {

struct SBakedAreaRow {
    const char *name;
    int32_t     amblight;
    SColor      ambcolor;
    int32_t     nightamblight;
    SColor      nightambcolor;
    double      point_light_int_mul;
    double      point_light_range_mul;
};

// BEGIN BAKED AREAS
static const SBakedAreaRow kBakedAreas[] = {};
// END BAKED AREAS

constexpr const char *kBeginGlobals = "// BEGIN BAKED GLOBALS";
constexpr const char *kEndGlobals   = "// END BAKED GLOBALS";
constexpr const char *kBeginAreas   = "// BEGIN BAKED AREAS";
constexpr const char *kEndAreas     = "// END BAKED AREAS";

// Walk up from the executable directory looking for a sibling that owns
// src/revisited_defaults.cpp. Mirrors revutils.cpp's find_repo_root probe
// but uses this exact file path as the sentinel so we only succeed when
// we can actually write where we expect.
std::filesystem::path find_self_source_path()
{
    namespace fs = std::filesystem;
    fs::path exe_dir;
#if defined(__APPLE__)
    char raw[1024];
    uint32_t sz = sizeof(raw);
    if (_NSGetExecutablePath(raw, &sz) == 0)
    {
        std::error_code ec;
        fs::path p = fs::weakly_canonical(fs::path(raw), ec);
        exe_dir = (ec ? fs::path(raw) : p).parent_path();
    }
#endif
    if (exe_dir.empty()) return {};

    std::error_code ec;
    fs::path p = exe_dir;
    for (int i = 0; i < 8 && !p.empty() && p != p.root_path(); ++i)
    {
        const fs::path cand = p / "src" / "revisited_defaults.cpp";
        if (fs::exists(cand, ec)) return cand;
        p = p.parent_path();
    }
    return {};
}

} // namespace


// BEGIN BAKED GLOBALS
void GetBakedRevisitedDefaults(SRevisitedSettings &out)
{
    (void)out;  // No bake yet — SRevisitedSettings field inits remain authoritative.
}
// END BAKED GLOBALS


bool ApplyBakedAreaDefaults(const char *area_name, TArea *area)
{
    if (!area_name || !*area_name || !area) return false;
    for (const SBakedAreaRow &row : kBakedAreas)
    {
        if (std::strcmp(row.name, area_name) == 0)
        {
            area->SetClassicLighting(row.amblight, row.ambcolor,
                                     row.nightamblight, row.nightambcolor,
                                     row.point_light_int_mul, row.point_light_range_mul);
            return true;
        }
    }
    return false;
}


// ----------------------------------------------------------------------------
// Bake writer
// ----------------------------------------------------------------------------

namespace {

std::string format_globals_block()
{
    const SRevisitedSettings &s = RevisitedSettings;
    std::ostringstream o;
    o.setf(std::ios::fixed);
    o.precision(4);
    o << kBeginGlobals << "\n"
      << "void GetBakedRevisitedDefaults(SRevisitedSettings &out)\n"
      << "{\n"
      << "    out.lighting_mode        = " << s.lighting_mode        << ";\n"
      << "    out.sun_dir_x            = " << s.sun_dir_x            << ";\n"
      << "    out.sun_dir_y            = " << s.sun_dir_y            << ";\n"
      << "    out.sun_dir_z            = " << s.sun_dir_z            << ";\n"
      << "    out.sun_color_r          = " << int(s.sun_color_r)     << ";\n"
      << "    out.sun_color_g          = " << int(s.sun_color_g)     << ";\n"
      << "    out.sun_color_b          = " << int(s.sun_color_b)     << ";\n"
      << "    out.sun_intensity        = " << s.sun_intensity        << ";\n"
      << "    out.light_ceiling        = " << s.light_ceiling        << ";\n"
      << "    out.sun_shadow_enable    = " << (s.sun_shadow_enable ? "true" : "false") << ";\n"
      << "    out.sun_shadow_step_wu   = " << s.sun_shadow_step_wu   << ";\n"
      << "    out.sun_shadow_soft_px   = " << s.sun_shadow_soft_px   << ";\n"
      << "    out.sun_shadow_max_steps = " << s.sun_shadow_max_steps << ";\n"
      << "    out.sun_shadow_bias_wu   = " << s.sun_shadow_bias_wu   << ";\n"
      << "    out.sun_shadow_depth_cut = " << s.sun_shadow_depth_cut << ";\n"
      << "    out.ao_enable            = " << (s.ao_enable ? "true" : "false") << ";\n"
      << "    out.ao_radius_px         = " << s.ao_radius_px         << ";\n"
      << "    out.ao_strength          = " << s.ao_strength          << ";\n"
      << "    out.ao_bias              = " << s.ao_bias              << ";\n"
      << "    out.ao_max_dist_wu       = " << s.ao_max_dist_wu       << ";\n"
      << "    out.parallax_enable      = " << (s.parallax_enable ? "true" : "false") << ";\n"
      << "    out.parallax_fov_deg     = " << s.parallax_fov_deg     << ";\n"
      << "    out.parallax_steps       = " << s.parallax_steps       << ";\n"
      << "    out.parallax_refine      = " << s.parallax_refine      << ";\n"
      << "    out.parallax_proxy_scale = " << s.parallax_proxy_scale << ";\n"
      << "    out.normal_radius        = " << s.normal_radius        << ";\n"
      << "    out.normal_hardness      = " << s.normal_hardness      << ";\n"
      << "    out.edge_threshold       = " << s.edge_threshold       << ";\n"
      << "    out.depth_mul            = " << s.depth_mul            << ";\n"
      << "    out.camera_zoom          = " << s.camera_zoom          << ";\n"
      << "    out.point_light_int_mul  = " << s.point_light_int_mul  << ";\n"
      << "    out.point_light_range_mul= " << s.point_light_range_mul<< ";\n"
      << "}\n"
      << kEndGlobals;
    return o.str();
}

// Read existing baked-area rows by scanning the BEGIN/END region for our
// known row format. Lets us *accumulate*: visiting Caverns then baking
// after having baked Forest earlier keeps Forest's row intact.
struct ParsedRow {
    std::string name;
    int32_t  amblight       = 30;
    SColor   ambcolor       = {255,255,255};
    int32_t  nightamblight  = 10;
    SColor   nightambcolor  = {255,255,255};
    double   point_int_mul  = 1.0;
    double   point_range_mul= 1.0;
};

bool parse_existing_areas(const std::string &file_text, std::vector<ParsedRow> &out)
{
    const auto a = file_text.find(kBeginAreas);
    const auto b = file_text.find(kEndAreas);
    if (a == std::string::npos || b == std::string::npos || b < a) return false;
    const std::string region = file_text.substr(a, b - a);

    // Each row pattern: { "Name", amb, {r,g,b}, nightamb, {r,g,b}, pi, pr },
    std::istringstream in(region);
    std::string line;
    while (std::getline(in, line))
    {
        const auto q1 = line.find('"');
        if (q1 == std::string::npos) continue;
        const auto q2 = line.find('"', q1 + 1);
        if (q2 == std::string::npos) continue;
        ParsedRow row;
        row.name = line.substr(q1 + 1, q2 - q1 - 1);

        const char *rest = line.c_str() + q2 + 1;
        int amb = 0, nr = 0, ng = 0, nb = 0, namb = 0, nnr = 0, nng = 0, nnb = 0;
        double pi = 1.0, pr = 1.0;
        // ", %d, {%d,%d,%d}, %d, {%d,%d,%d}, %lf, %lf },"
        if (std::sscanf(rest, " , %d, {%d,%d,%d}, %d, {%d,%d,%d}, %lf, %lf",
                        &amb, &nr, &ng, &nb, &namb, &nnr, &nng, &nnb, &pi, &pr) == 10)
        {
            row.amblight       = amb;
            row.ambcolor       = { (uint8_t)nr,  (uint8_t)ng,  (uint8_t)nb };
            row.nightamblight  = namb;
            row.nightambcolor  = { (uint8_t)nnr, (uint8_t)nng, (uint8_t)nnb };
            row.point_int_mul  = pi;
            row.point_range_mul= pr;
            out.push_back(std::move(row));
        }
    }
    return true;
}

std::string format_areas_block(const std::vector<ParsedRow> &rows)
{
    std::ostringstream o;
    o.setf(std::ios::fixed);
    o.precision(4);
    o << kBeginAreas << "\n"
      << "static const SBakedAreaRow kBakedAreas[] = {\n";
    for (const ParsedRow &r : rows)
    {
        o << "    { \"" << r.name << "\""
          << ", " << r.amblight
          << ", {" << int(r.ambcolor.red)        << "," << int(r.ambcolor.green)        << "," << int(r.ambcolor.blue)        << "}"
          << ", " << r.nightamblight
          << ", {" << int(r.nightambcolor.red)   << "," << int(r.nightambcolor.green)   << "," << int(r.nightambcolor.blue)   << "}"
          << ", " << r.point_int_mul
          << ", " << r.point_range_mul
          << " },\n";
    }
    o << "};\n"
      << kEndAreas;
    return o.str();
}

bool replace_region(std::string &text, const char *begin_tag, const char *end_tag,
                    const std::string &replacement)
{
    const auto a = text.find(begin_tag);
    if (a == std::string::npos) return false;
    const auto b = text.find(end_tag, a);
    if (b == std::string::npos) return false;
    const auto end_of_end = b + std::strlen(end_tag);
    text.replace(a, end_of_end - a, replacement);
    return true;
}

} // namespace


bool BakeRevisitedDefaultsToSource(const TArea *current_area,
                                   char *out_path, int32_t out_len)
{
    if (out_path && out_len) out_path[0] = 0;

    namespace fs = std::filesystem;
    const fs::path src = find_self_source_path();
    if (src.empty())
    {
        log_error("[bake] cannot locate src/revisited_defaults.cpp under the exe "
                  "(not running from a dev checkout?)");
        return false;
    }

    std::ifstream in(src, std::ios::binary);
    if (!in)
    {
        log_error("[bake] cannot read %s", src.string().c_str());
        return false;
    }
    std::stringstream buf; buf << in.rdbuf();
    std::string text = buf.str();
    in.close();

    // ----- globals: re-emit the function body from RevisitedSettings -----
    const std::string globals_block = format_globals_block();
    if (!replace_region(text, kBeginGlobals, kEndGlobals, globals_block))
    {
        log_error("[bake] %s missing BEGIN/END GLOBALS markers", src.string().c_str());
        return false;
    }

    // ----- areas: parse, upsert current area's row, re-emit ---------------
    std::vector<ParsedRow> rows;
    parse_existing_areas(text, rows);

    if (current_area && current_area->GetName() && current_area->GetName()[0])
    {
        ParsedRow snap;
        snap.name            = current_area->GetName();
        snap.amblight        = current_area->Amblight();
        snap.ambcolor        = current_area->Ambcolor();
        snap.nightamblight   = current_area->Nightamblight();
        snap.nightambcolor   = current_area->Nightambcolor();
        snap.point_int_mul   = current_area->PointLightIntMul();
        snap.point_range_mul = current_area->PointLightRangeMul();

        auto it = std::find_if(rows.begin(), rows.end(),
                               [&](const ParsedRow &r) { return r.name == snap.name; });
        if (it != rows.end())
            *it = std::move(snap);
        else
            rows.push_back(std::move(snap));
    }

    const std::string areas_block = format_areas_block(rows);
    if (!replace_region(text, kBeginAreas, kEndAreas, areas_block))
    {
        log_error("[bake] %s missing BEGIN/END AREAS markers", src.string().c_str());
        return false;
    }

    std::ofstream out(src, std::ios::binary | std::ios::trunc);
    if (!out)
    {
        log_error("[bake] cannot write %s", src.string().c_str());
        return false;
    }
    out << text;
    out.close();

    log_info("[bake] wrote %s (areas baked: %zu)", src.string().c_str(), rows.size());
    if (out_path && out_len > 0)
        std::snprintf(out_path, out_len, "%s", src.string().c_str());
    return true;
}
