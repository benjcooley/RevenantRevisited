// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        editorserialize.cpp - Text round-trip for SafeObjects          *
// *************************************************************************

#include "editorserialize.h"

#include "editoradapters.h"
#include "iobject.h"
#include "logging.h"
#include "mappane.h"
#include "maprenderer.h"
#include "object.h"
#include "playscreen.h"
#include "sector.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <sstream>
#include <unordered_map>

namespace {

constexpr const char* kHeaderV1 = "revenant.objects.v1";
constexpr const char* kBlockMarker = "[obj]";

// ---------------------------------------------------------------------------
// String <-> SPropertyValue
// ---------------------------------------------------------------------------

std::string EscapeString(const std::string& s)
{
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if      (c == '\\') out += "\\\\";
        else if (c == '"')  out += "\\\"";
        else if (c == '\n') out += "\\n";
        else                out.push_back(c);
    }
    out.push_back('"');
    return out;
}

std::string UnescapeString(std::string_view in)
{
    std::string s;
    s.reserve(in.size());
    bool quoted = !in.empty() && in.front() == '"' && in.back() == '"';
    size_t a = quoted ? 1 : 0;
    size_t b = quoted ? in.size() - 1 : in.size();
    for (size_t i = a; i < b; ++i) {
        char c = in[i];
        if (c == '\\' && i + 1 < b) {
            char n = in[++i];
            if      (n == 'n')  s.push_back('\n');
            else if (n == '\\') s.push_back('\\');
            else if (n == '"')  s.push_back('"');
            else                s.push_back(n);
        } else {
            s.push_back(c);
        }
    }
    return s;
}

std::string PropValueToText(const SPropertyValue& v)
{
    char buf[64];
    if (auto p = std::get_if<bool>(&v))     return *p ? "true" : "false";
    if (auto p = std::get_if<int64_t>(&v))  { std::snprintf(buf, sizeof(buf), "%lld", (long long)*p); return buf; }
    if (auto p = std::get_if<double>(&v))   { std::snprintf(buf, sizeof(buf), "%g", *p); return buf; }
    if (auto p = std::get_if<std::string>(&v)) return EscapeString(*p);
    if (auto p = std::get_if<SVec3i>(&v))   { std::snprintf(buf, sizeof(buf), "%d,%d,%d", p->x, p->y, p->z); return buf; }
    if (auto p = std::get_if<SVec3f>(&v))   { std::snprintf(buf, sizeof(buf), "%g,%g,%g", p->x, p->y, p->z); return buf; }
    if (auto p = std::get_if<SColor4f>(&v)) { std::snprintf(buf, sizeof(buf), "%g,%g,%g,%g", p->r, p->g, p->b, p->a); return buf; }
    if (auto p = std::get_if<IObject*>(&v)) {
        // Cross-object refs round-trip as @refid:<mapindex>; null becomes empty.
        // The mapindex IS the universal id (see memory/project_object_ref.md);
        // restoring with original ids heals these refs implicitly.
        IObject* obj = *p;
        if (!obj) return "";
        std::snprintf(buf, sizeof(buf), "@refid:%d", obj->GetId());
        return buf;
    }
    return "";
}

bool TextToPropValue(EPropertyType type, std::string_view text, SPropertyValue& out)
{
    auto eq = [](std::string_view a, const char* b) {
        return a.size() == std::strlen(b) && std::strncmp(a.data(), b, a.size()) == 0;
    };
    std::string s(text);
    switch (type) {
        case EPropertyType::Bool:
            out = eq(text, "true") || eq(text, "1");
            return true;
        case EPropertyType::Int:
        case EPropertyType::Enum:
            out = int64_t(std::strtoll(s.c_str(), nullptr, 10));
            return true;
        case EPropertyType::Float:
            out = std::strtod(s.c_str(), nullptr);
            return true;
        case EPropertyType::String:
            out = UnescapeString(text);
            return true;
        case EPropertyType::Vec3Int: {
            SVec3i v{};
            std::sscanf(s.c_str(), "%d,%d,%d", &v.x, &v.y, &v.z);
            out = v; return true;
        }
        case EPropertyType::Vec3Float: {
            SVec3f v{};
            std::sscanf(s.c_str(), "%f,%f,%f", &v.x, &v.y, &v.z);
            out = v; return true;
        }
        case EPropertyType::Color: {
            SColor4f c{};
            std::sscanf(s.c_str(), "%f,%f,%f,%f", &c.r, &c.g, &c.b, &c.a);
            out = c; return true;
        }
        case EPropertyType::ObjectRef:
            // Cross-object refs deserialize to nullptr at this layer; the
            // registry will resolve them implicitly once the referenced
            // object is restored (or stay null if it isn't).
            out = (IObject*)nullptr;
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Dispatcher: one registration per SafeObject root
// ---------------------------------------------------------------------------
//
// `@type` parsing: split on ':' once. The leading segment names the
// SafeObject root ("TObjectInstance"). The remainder is opaque payload
// passed to the root's handler.
//
// Today only TObjectInstance is registered. TSector, assets, and editor
// data objects will register their own handlers as they become saferef
// targets (see memory/project_safeobject_category.md).

struct SParsedBlock {
    std::string type_full;                                  // raw @type string
    std::string type_root;                                  // leading segment
    std::string type_payload;                               // rest, opaque
    int32_t     refid = -1;                                 // -1 if absent
    std::vector<std::pair<std::string, std::string>> props; // ordered
};

// Handler signature: instantiate a live object from the parsed block,
// add it to the world, return its mapindex (or -1 on failure). If
// preserve_refid is true and block.refid >= 0, the handler should
// re-key the new instance to that mapindex.
using FInstantiate = std::function<int32_t(const SParsedBlock&, bool preserve_refid)>;

// Handler signature: produce the @type string and emit reflected props
// for one live object. Returning false skips the object.
using FCapture = std::function<bool(TObjectInstance*, std::string& type_out,
                                    std::vector<std::pair<std::string,std::string>>& props_out)>;

struct SRootHandler {
    FInstantiate instantiate;
    FCapture     capture;
};

std::unordered_map<std::string, SRootHandler>& Handlers()
{
    static std::unordered_map<std::string, SRootHandler> s;
    return s;
}

// ---------------------------------------------------------------------------
// TObjectInstance handler
// ---------------------------------------------------------------------------

bool CaptureInstance(TObjectInstance* oi,
                     std::string& type_out,
                     std::vector<std::pair<std::string,std::string>>& props_out)
{
    if (!oi) return false;
    const char* cn = oi->GetClassName();
    const char* tn = oi->GetTypeName();
    if (!cn) cn = "";
    if (!tn) tn = "";
    type_out = std::string("TObjectInstance:") + cn + ":" + tn;

    // Walk reflected props via the existing IReflection adapter. Skip
    // ReadOnly entries (those are derived from ctor inputs / live state)
    // and skip "class" / "type" / "mapindex" which the meta keys cover.
    IObject* adapter = EditorAdapters().For(oi);
    if (!adapter) return false;
    const IReflection& r = adapter->GetRefl();
    const int32_t n = r.NumProperties();
    for (int32_t i = 0; i < n; ++i) {
        const SPropertyDesc& p = r.GetProperty(i);
        if (HasFlag(p.flags, EPropertyFlag::ReadOnly)) continue;
        if (HasFlag(p.flags, EPropertyFlag::Hidden))   continue;
        SPropertyValue v;
        r.GetValue(adapter, p, v);
        props_out.emplace_back(p.name, PropValueToText(v));
    }
    return true;
}

int32_t InstantiateInstance(const SParsedBlock& blk, bool preserve_refid)
{
    // Payload format: "<class_name>:<type_name>"
    const std::string& payload = blk.type_payload;
    const size_t colon = payload.find(':');
    if (colon == std::string::npos) {
        log_warn("[serialize] @type missing class:type payload (%s)", blk.type_full.c_str());
        return -1;
    }
    std::string class_name = payload.substr(0, colon);
    std::string type_name  = payload.substr(colon + 1);

    const int32_t objclass = TObjectClass::FindClass(class_name.data());
    if (objclass < 0) {
        log_warn("[serialize] unknown class '%s'", class_name.c_str());
        return -1;
    }
    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl) return -1;
    const int32_t objtype = cl->FindObjType(type_name.c_str());
    if (objtype < 0) {
        log_warn("[serialize] unknown type '%s' in class '%s'",
                 type_name.c_str(), class_name.c_str());
        return -1;
    }

    // We bypass MapPane.NewObject entirely. In editor mode
    // MapPane.sectors[][] is empty (the renderer owns the loaded
    // sector list, see memory/project_editor_object_model.md), so
    // MapPane.NewObject's sector-window check fails. Instead: build
    // the instance via the class factory, find the right loaded
    // sector via the renderer, and add directly.
    TMapRenderer* mr = PlayScreen.MapRenderer();
    if (!mr) {
        log_warn("[serialize] no map renderer; cannot instantiate");
        return -1;
    }

    SObjectDef def{};
    def.objclass = (short)objclass;
    def.objtype  = (short)objtype;
    def.level    = (uint16_t)mr->CameraLevel();
    for (const auto& kv : blk.props) {
        if (kv.first == "pos") {
            std::sscanf(kv.second.c_str(), "%d,%d,%d", &def.pos.x, &def.pos.y, &def.pos.z);
            break;
        }
    }

    TObjectInstance* live = cl->NewObject(&def);
    if (!live) {
        log_warn("[serialize] class factory NewObject failed for %s:%s",
                 class_name.c_str(), type_name.c_str());
        return -1;
    }

    const int32_t sx = def.pos.x >> SECTORWSHIFT;
    const int32_t sy = def.pos.y >> SECTORHSHIFT;
    TSector* sec = mr->FindLoadedSector(def.level, sx, sy);
    if (!sec) {
        log_warn("[serialize] no loaded sector at level=%d sec=%d_%d (pos %d,%d,%d)",
                 def.level, sx, sy, def.pos.x, def.pos.y, def.pos.z);
        delete live;
        return -1;
    }
    sec->AddObject(live);

    // Register in the saferef registry. Use the requested refid if
    // we're restoring identity (delete-undo); otherwise allocate
    // fresh via MakeIndex.
    if (preserve_refid && blk.refid >= 0)
        live->SetMapIndex(blk.refid);
    else
        live->SetMapIndex(MapPane.MakeIndex());
    const int32_t final_idx = live->GetMapIndex();

    // Apply remaining reflected props. Pos already landed via def, but
    // re-applying is a no-op so we don't bother filtering.
    IObject* adapter = EditorAdapters().For(live);
    if (adapter) {
        const IReflection& r = adapter->GetRefl();
        for (const auto& kv : blk.props) {
            const SPropertyDesc* p = r.FindProperty(kv.first.c_str());
            if (!p) continue;
            if (HasFlag(p->flags, EPropertyFlag::ReadOnly)) continue;
            SPropertyValue v;
            if (TextToPropValue(p->type, kv.second, v))
                r.SetValue(adapter, *p, v);
        }
    }
    return final_idx;
}

void EnsureHandlersRegistered()
{
    auto& h = Handlers();
    if (!h.empty()) return;
    h.emplace("TObjectInstance",
              SRootHandler{ &InstantiateInstance, &CaptureInstance });
}

// ---------------------------------------------------------------------------
// Parsing helpers
// ---------------------------------------------------------------------------

std::string_view Trim(std::string_view s)
{
    size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r')) ++a;
    while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r')) --b;
    return s.substr(a, b - a);
}

// Split @type "Root:rest" -> ("Root", "rest"). If no ':', payload is empty.
std::pair<std::string,std::string> SplitTypeRoot(const std::string& full)
{
    const size_t colon = full.find(':');
    if (colon == std::string::npos) return { full, "" };
    return { full.substr(0, colon), full.substr(colon + 1) };
}

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool EditorSerialize::LooksLikeOurs(std::string_view text)
{
    // Find first non-blank, non-comment line and check for our header.
    size_t i = 0;
    while (i < text.size()) {
        size_t eol = text.find('\n', i);
        if (eol == std::string_view::npos) eol = text.size();
        std::string_view line = Trim(text.substr(i, eol - i));
        if (!line.empty() && line.front() != ';' && line.front() != '#') {
            const size_t hlen = std::strlen(kHeaderV1);
            return line.size() >= hlen && std::strncmp(line.data(), kHeaderV1, hlen) == 0;
        }
        i = eol + 1;
    }
    return false;
}

std::string EditorSerialize::ToText(const std::vector<TObjectInstance*>& objs,
                                    bool include_refid)
{
    EnsureHandlersRegistered();
    std::ostringstream out;
    out << kHeaderV1 << '\n';
    for (TObjectInstance* oi : objs) {
        if (!oi) continue;
        // For now only TObjectInstance has a capture handler. Future
        // SafeObject roots (TSector etc.) will dispatch on their own
        // pointer types, not this loop.
        std::string type_str;
        std::vector<std::pair<std::string,std::string>> props;
        if (!CaptureInstance(oi, type_str, props)) continue;

        out << kBlockMarker << '\n';
        out << "@type=" << type_str << '\n';
        if (include_refid && oi->GetMapIndex() >= 0)
            out << "@refid=" << oi->GetMapIndex() << '\n';
        for (const auto& kv : props)
            out << kv.first << '=' << kv.second << '\n';
    }
    return out.str();
}

std::vector<int32_t> EditorSerialize::FromText(std::string_view text,
                                               bool preserve_refid)
{
    EnsureHandlersRegistered();
    std::vector<int32_t> result;
    if (!LooksLikeOurs(text)) return result;

    // Two-pass-style parse: walk lines once, accumulating into
    // SParsedBlock, and flush each block to its handler when [obj] or
    // EOF is reached.
    std::vector<SParsedBlock> blocks;
    SParsedBlock cur;
    bool have_block = false;
    bool seen_header = false;

    size_t i = 0;
    while (i < text.size()) {
        size_t eol = text.find('\n', i);
        if (eol == std::string_view::npos) eol = text.size();
        std::string_view raw = text.substr(i, eol - i);
        i = eol + 1;

        // Strip inline comments (; or #) and trim.
        size_t cut = raw.size();
        for (size_t j = 0; j < raw.size(); ++j)
            if (raw[j] == ';' || raw[j] == '#') { cut = j; break; }
        std::string_view line = Trim(raw.substr(0, cut));
        if (line.empty()) continue;

        if (!seen_header) {
            const size_t hlen = std::strlen(kHeaderV1);
            if (line.size() >= hlen && std::strncmp(line.data(), kHeaderV1, hlen) == 0) {
                seen_header = true;
                continue;
            }
            // Header missing -- LooksLikeOurs already passed, so this
            // shouldn't happen, but bail safely.
            return result;
        }

        if (line == kBlockMarker) {
            if (have_block) blocks.push_back(std::move(cur));
            cur = SParsedBlock{};
            have_block = true;
            continue;
        }
        if (!have_block) continue;     // stray content before first [obj]

        const size_t eq = line.find('=');
        if (eq == std::string_view::npos) continue;
        std::string key(Trim(line.substr(0, eq)));
        std::string val(Trim(line.substr(eq + 1)));

        if (key == "@type") {
            cur.type_full = val;
            auto [root, payload] = SplitTypeRoot(val);
            cur.type_root    = std::move(root);
            cur.type_payload = std::move(payload);
        } else if (key == "@refid") {
            cur.refid = (int32_t)std::strtol(val.c_str(), nullptr, 10);
        } else if (!key.empty() && key.front() != '@') {
            cur.props.emplace_back(std::move(key), std::move(val));
        }
        // Unknown @-meta keys are silently ignored (forward-compat).
    }
    if (have_block) blocks.push_back(std::move(cur));

    // Dispatch each block to its registered handler.
    auto& handlers = Handlers();
    for (const SParsedBlock& blk : blocks) {
        auto it = handlers.find(blk.type_root);
        if (it == handlers.end()) {
            log_warn("[serialize] no handler for @type root '%s'", blk.type_root.c_str());
            continue;
        }
        const int32_t mi = it->second.instantiate(blk, preserve_refid);
        if (mi >= 0) result.push_back(mi);
    }
    return result;
}
