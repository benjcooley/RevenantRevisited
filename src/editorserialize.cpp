// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        editorserialize.cpp - Text round-trip for SafeObjects          *
// *************************************************************************

#include "editorserialize.h"

#include "defio.h"
#include "logging.h"
#include "refl.h"
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

    // Walk the annotated properties (base chain first). Read-only ones
    // ("class" / "type" / "mapindex") are covered by the meta keys; values
    // use the DEF text form so the clipboard reads like a DEF.
    const refl::SType* type = refl::Of(*oi);
    if (!type) return false;
    std::vector<const refl::SType*> chain;
    for (const refl::SType* t = type; t; t = t->base) chain.push_back(t);
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        for (int32_t i = 0; i < (*it)->prop_count; ++i) {
            const refl::SProp& p = (*it)->props[i];
            if (p.read_only || !p.serialized || p.type == EPropType::Trigger) continue;
            SPropValue v;
            if (!refl::Get(*type, oi, p.id, v)) continue;
            props_out.emplace_back(p.name, defio::FormatValue(p, v));
        }
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
    // The spawn position decides the sector, so it is read before the
    // object exists; the node's `pos` property describes its text form.
    if (const refl::SType* node_type = refl::Of<TNode>()) {
        const refl::SProp* pos_prop = refl::FindProp(*node_type, "pos");
        for (const auto& kv : blk.props) {
            if (kv.first == "pos" && pos_prop) {
                SPropValue v;
                std::string storage;
                if (defio::ParseValue(*pos_prop, kv.second, v, storage)) {
                    def.pos.x = int32_t(v.v3[0]);
                    def.pos.y = int32_t(v.v3[1]);
                    def.pos.z = int32_t(v.v3[2]);
                }
                break;
            }
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

    // Apply the remaining annotated props. Pos already landed via def;
    // re-applying it is a no-op. Unknown keys are ignored (forward-compat).
    if (const refl::SType* type = refl::Of(*live)) {
        for (const auto& kv : blk.props) {
            const refl::SProp* p = refl::FindProp(*type, kv.first);
            if (!p || p->read_only) continue;
            SPropValue v;
            std::string storage, err;
            if (defio::ParseValue(*p, kv.second, v, storage, &err))
                refl::Set(*type, live, p->id, v);
            else
                log_warn("[serialize] %s: %s", kv.first.c_str(), err.c_str());
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
