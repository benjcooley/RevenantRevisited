// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   prefab.h - prefab library and stamping (docs/vfx/PREFAB_SYSTEM.md)  *
// *************************************************************************
//
// A prefab is a DEF document (assets/prefabs/<Name>.def) whose body is a
// root node: properties, components and child `node` blocks. The library
// reads it once through the DEF reader into a scratch node tree and keeps
// the packed form (nodepack). Stamping decodes that buffer under a live
// root node -- an effect instance in the map, or a harness object -- so
// a spawn never touches text.
//
//     TPrefabComponent* c = new TPrefabComponent;
//     c->Configure(PrefabLibrary().Find("Mist"));
//     root->AddComponent(std::unique_ptr<TObjectComponent>(c));   // stamps on attach
//
// The root's own transform is left alone on stamp (it is the spawn
// position); the prefab's child nodes hang under it and are owned by the
// component, so detaching the component tears the effect down.
//
// *************************************************************************

#pragma once

#include "objectcomponent.h"
#include "renderer.h"      // EFxDebugMode

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TNode;
class TParticleEffectComponent;

struct SPrefab
{
    std::string          name;
    std::string          source_path;   // where the DEF came from
    std::vector<uint8_t> packed;        // nodepack of the root body
};

class TPrefabLibrary
{
  public:
    // The prefab by name, loading assets/prefabs/<name>.def on first use.
    // nullptr (with a logged error) if there is no such file or it does
    // not read.
    [[nodiscard]] const SPrefab* Find(std::string_view name);
    // Names of every prefab file in the prefabs directory (sorted).
    [[nodiscard]] std::vector<std::string> List() const;
    // Re-read a prefab from its file (editor: after a save). Existing
    // stamps keep their nodes; new stamps use the new buffer.
    const SPrefab* Reload(std::string_view name);
    // Bake DEF text into a prefab record. Exposed for tools and tests.
    static bool Bake(std::string_view name, std::string_view text, SPrefab& out, std::string* error);

  private:
    std::vector<std::unique_ptr<SPrefab>> prefabs_;
};

TPrefabLibrary& PrefabLibrary();

class TPrefabComponent : public TObjectComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "prefab"; }

    void Configure(const SPrefab* prefab) { prefab_ = prefab; }
    [[nodiscard]] const SPrefab* Prefab() const { return prefab_; }

    void OnAttach() override;      // stamps the child nodes under the owner
    void OnDetach() override;      // destroys them

    // Per render frame, from whoever draws the owner: pulses every
    // particle component on the stamped child nodes. The owner's own
    // components are the owner's business (the map walk pulses them).
    void DrawPulse();
    // Standalone drivers only (harness): submit the stamped nodes' particles.
    void Submit(EFxDebugMode debug_mode);

    [[nodiscard]] int32_t NodeCount() const { return int32_t(nodes_.size()); }
    [[nodiscard]] TNode* NodeAt(int32_t index) const
        { return index >= 0 && index < NodeCount() ? nodes_[size_t(index)].get() : nullptr; }

  private:
    const SPrefab* prefab_ = nullptr;
    std::vector<std::unique_ptr<TNode>> nodes_;
};
