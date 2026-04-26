// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *             editorprefs.cpp - Editor preference key-value store       *
// *************************************************************************

#include "editorprefs.h"

#include "logging.h"

#include <cstring>
#include <fstream>
#include <sstream>

namespace EditorPrefs {

namespace {

defdoc::Document g_doc;
std::string      g_default_path = "editor.def";

const char* PathOrDefault(const char* path)
{
    return (path && *path) ? path : g_default_path.c_str();
}

// Walk dotted segments. Returns nullptr if any intermediate segment is
// not an object and `create` is false.
defdoc::Node* WalkTo(const char* path, bool create)
{
    if (!path) return nullptr;
    defdoc::Node* node = &g_doc.root();
    if (!node->is_object()) {
        if (!create) return nullptr;
        node->as_object();
    }
    const char* p = path;
    while (*p) {
        // Pull the next segment up to '.' or end of string.
        const char* dot = std::strchr(p, '.');
        std::string seg = dot ? std::string(p, dot - p) : std::string(p);
        if (seg.empty()) {
            if (!dot) break;
            p = dot + 1;
            continue;
        }

        if (!node->is_object()) {
            if (!create) return nullptr;
            node->as_object();
        }
        defdoc::Node::Object& obj = node->as_object();
        auto it = obj.find(seg);
        if (it == obj.end()) {
            if (!create) return nullptr;
            obj[seg] = defdoc::Node{};
            it = obj.find(seg);
        }
        node = &it->second;
        if (!dot) break;
        p = dot + 1;
    }
    return node;
}

} // namespace

defdoc::Document& Doc() { return g_doc; }

bool Load(const char* path)
{
    const char* p = PathOrDefault(path);
    std::ifstream in(p);
    if (!in.is_open()) {
        log_info("[editorprefs] no '%s'; starting with defaults", p);
        return false;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    try {
        g_doc = defdoc::Document::parse(ss.str());
        log_info("[editorprefs] loaded '%s'", p);
        return true;
    } catch (const std::exception& e) {
        log_warn("[editorprefs] parse '%s' failed: %s", p, e.what());
        return false;
    }
}

bool Save(const char* path)
{
    const char* p = PathOrDefault(path);
    if (!g_doc.write_file(p)) {
        log_warn("[editorprefs] could not write '%s'", p);
        return false;
    }
    return true;
}

void Reset()
{
    g_doc = defdoc::Document{};
}

bool Has(const char* path)
{
    const defdoc::Node* n = WalkTo(path, /*create=*/false);
    return n && !n->is_null();
}

defdoc::Node& Section(const char* path)
{
    defdoc::Node* n = WalkTo(path, /*create=*/true);
    if (!n->is_object()) n->as_object();
    return *n;
}

// ---- Typed accessors --------------------------------------------------

int32_t GetInt(const char* path, int32_t fallback)
{
    const defdoc::Node* n = WalkTo(path, false);
    return n ? int32_t(n->as_int(int64_t(fallback))) : fallback;
}

float GetFloat(const char* path, float fallback)
{
    const defdoc::Node* n = WalkTo(path, false);
    return n ? float(n->as_double(double(fallback))) : fallback;
}

double GetDouble(const char* path, double fallback)
{
    const defdoc::Node* n = WalkTo(path, false);
    return n ? n->as_double(fallback) : fallback;
}

bool GetBool(const char* path, bool fallback)
{
    const defdoc::Node* n = WalkTo(path, false);
    return n ? n->as_bool(fallback) : fallback;
}

std::string GetString(const char* path, const std::string& fallback)
{
    const defdoc::Node* n = WalkTo(path, false);
    return n ? n->as_string(fallback) : fallback;
}

void SetInt(const char* path, int32_t value)
{
    defdoc::Node* n = WalkTo(path, true);
    *n = defdoc::Node(int64_t(value));
}

void SetFloat(const char* path, float value)
{
    defdoc::Node* n = WalkTo(path, true);
    *n = defdoc::Node(double(value));
}

void SetDouble(const char* path, double value)
{
    defdoc::Node* n = WalkTo(path, true);
    *n = defdoc::Node(value);
}

void SetBool(const char* path, bool value)
{
    defdoc::Node* n = WalkTo(path, true);
    *n = defdoc::Node(value);
}

void SetString(const char* path, const std::string& value)
{
    defdoc::Node* n = WalkTo(path, true);
    *n = defdoc::Node(value);
}

} // namespace EditorPrefs
