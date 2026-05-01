// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editoradapters.h - IObject adapters for game classes        *
// *************************************************************************
//
// The editor walks the scene through IObject / IReflection. Game-side
// classes (TSector, TObjectInstance, ...) don't inherit from IObject --
// instead, we wrap each instance in a thin IObject adapter held by the
// editor. The adapters are owned by the editor's adapter cache (one per
// underlying pointer), so a TSectorAdapter and a TInstanceAdapter live
// only as long as the editor is interested in displaying them.
//
// *************************************************************************

#pragma once

#include "iobject.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

class TSector;
class TObjectInstance;

// ---------------------------------------------------------------------------
// Adapter classes. One per game-side concept the editor wants to view.
// ---------------------------------------------------------------------------

class TSectorAdapter : public IObject
{
public:
    explicit TSectorAdapter(TSector* sec) : sec_(sec) {}

    [[nodiscard]] int32_t            GetId()   const override;
    [[nodiscard]] const char*        GetName() const override;
    [[nodiscard]] const IReflection& GetRefl() const override;

    [[nodiscard]] TSector* Sector() const { return sec_; }

private:
    TSector* sec_;
    mutable char name_buf_[32] = {};   // computed on demand: "Sector L_X_Y"
};

class TInstanceAdapter : public IObject
{
public:
    explicit TInstanceAdapter(TObjectInstance* oi) : oi_(oi) {}

    [[nodiscard]] int32_t            GetId()   const override;
    [[nodiscard]] const char*        GetName() const override;
    [[nodiscard]] const IReflection& GetRefl() const override;

    [[nodiscard]] TObjectInstance* Instance() const { return oi_; }

private:
    TObjectInstance* oi_;
};

// ---------------------------------------------------------------------------
// Adapter cache. One global cache so two parts of the editor (scene tree
// + properties panel) get the same IObject* for the same TSector* /
// TObjectInstance*.
// ---------------------------------------------------------------------------

class TEditorAdapterCache
{
public:
    // Adapter for a sector. Created on first request, reused thereafter.
    [[nodiscard]] TSectorAdapter*   For(TSector* sec);
    [[nodiscard]] TInstanceAdapter* For(TObjectInstance* oi);

    // Drop adapters for objects that no longer exist (or just clear all).
    // Cheap; the editor can call this each frame if the scene churns.
    void Clear();

private:
    std::unordered_map<TSector*,         std::unique_ptr<TSectorAdapter>>   sectors_;
    std::unordered_map<TObjectInstance*, std::unique_ptr<TInstanceAdapter>> instances_;
};

// Single shared cache for the editor session.
TEditorAdapterCache& EditorAdapters();
