// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               prefab.cpp - prefab library and stamping               *
// *************************************************************************

#include "engine/scene/prefab.h"

#include "engine/refl/defio.h"
#include "engine/core/logging.h"
#include "engine/scene/node.h"
#include "engine/refl/nodepack.h"
#include "engine/refl/packed.h"
#include "engine/fx/particlecomponent.h"
#include "engine/core/revengine.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

bool IEquals(std::string_view a, std::string_view b)
{
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    return true;
}

std::string PrefabPath(std::string_view name)
{
    return rev_engine_asset(("prefabs/" + std::string(name) + ".def").c_str());
}

bool ReadFile(const std::string& path, std::string& out)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    std::stringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

} // namespace

TPrefabLibrary& PrefabLibrary()
{
    static TPrefabLibrary library;
    return library;
}

bool TPrefabLibrary::Bake(std::string_view name, std::string_view text, SPrefab& out, std::string* error)
{
    // Deserialize the text as data only (no PostLoad -- that belongs to
    // stamping, when the objects are real), serialize it into one buffer,
    // drop the scratch tree. A typo fails here, with a line.
    TNode scratch{std::string(name)};
    std::vector<std::unique_ptr<TNode>> owned;
    if (!defio::Read(text, scratch, owned, error, /*post_load=*/false))
        return false;
    TPackedWriter w;
    nodepack::Pack(scratch, w);
    out.name = std::string(name);
    out.packed = std::move(w.Buffer());
    return true;
}

const SPrefab* TPrefabLibrary::Find(std::string_view name)
{
    for (const auto& p : prefabs_)
        if (IEquals(p->name, name))
            return p.get();
    return Reload(name);
}

const SPrefab* TPrefabLibrary::Reload(std::string_view name)
{
    const std::string path = PrefabPath(name);
    std::string text;
    if (path.empty() || !ReadFile(path, text))
    {
        log_error("[prefab] no prefab '%.*s' (looked for prefabs/%.*s.def in the engine assets)",
                  int(name.size()), name.data(), int(name.size()), name.data());
        return nullptr;
    }
    auto baked = std::make_unique<SPrefab>();
    std::string err;
    if (!Bake(name, text, *baked, &err))
    {
        log_error("[prefab] %s: %s", path.c_str(), err.c_str());
        return nullptr;
    }
    baked->source_path = path;
    for (auto& p : prefabs_)
        if (IEquals(p->name, name))
        {
            *p = std::move(*baked);
            return p.get();
        }
    prefabs_.push_back(std::move(baked));
    log_info("[prefab] loaded '%s' (%zu bytes packed) from %s",
             prefabs_.back()->name.c_str(), prefabs_.back()->packed.size(), path.c_str());
    return prefabs_.back().get();
}

std::vector<std::string> TPrefabLibrary::List() const
{
    namespace fs = std::filesystem;
    std::vector<std::string> names;
    const std::string dir = rev_engine_asset("prefabs");
    std::error_code ec;
    if (dir.empty() || !fs::is_directory(dir, ec))
        return names;
    for (const fs::directory_entry& e : fs::directory_iterator(dir, ec))
    {
        if (!e.is_regular_file(ec)) continue;
        const fs::path& p = e.path();
        if (IEquals(p.extension().string(), ".def"))
            names.push_back(p.stem().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}

// ---- stamping ---------------------------------------------------------------

void TPrefabComponent::OnAttach()
{
    if (!prefab_ || !Owner())
        return;
    TPackedReader r(prefab_->packed);
    std::string err;
    if (!nodepack::Unpack(r, *Owner(), nodes_, &err, /*apply_root_props=*/false))
        log_warn("[prefab] '%s': %s", prefab_->name.c_str(), err.c_str());
    // This stamp's nodes are its own: fresh uids, links between them followed.
    nodepack::Reidentify(*Owner(), nodes_);
    // Child nodes activate their components on add; the root's own
    // components attached here go live with the owner's activation.
}

void TPrefabComponent::OnDetach()
{
    nodes_.clear();   // destroying a node detaches its components
}

void TPrefabComponent::Submit(EFxDebugMode debug_mode)
{
    for (const auto& node : nodes_)
    {
        if (!node || !node->Active()) continue;
        for (int32_t i = 0; i < node->NumComponents(); ++i)
            if (auto* particles = dynamic_cast<TParticleEffectComponent*>(node->GetComponent(i)))
                particles->Submit(debug_mode);
    }
}

void TPrefabComponent::DrawPulse()
{
    for (const auto& node : nodes_)
    {
        if (!node || !node->Active()) continue;
        for (int32_t i = 0; i < node->NumComponents(); ++i)
            if (auto* particles = dynamic_cast<TParticleEffectComponent*>(node->GetComponent(i)))
                particles->DrawPulse();
    }
}
