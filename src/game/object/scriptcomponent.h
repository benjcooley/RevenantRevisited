// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   scriptcomponent.h - the script an object runs                       *
// *************************************************************************
//
// Retail hung a TScript off every object that could have one. It is a
// component now: the object adds it the first time a script is attached
// (InitScript) and keeps it for life, reached through a plain pointer,
// and the component owns the script. Nothing here is serialized: which
// script an object runs follows from its type and name, and the script's
// position is rebuilt on load (TScript::Reset), as retail did.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <memory>

_CLASSDEF(TScript)

class TScriptComponent final : public TComponent
{
  public:
    TScriptComponent();
    ~TScriptComponent() override;

    [[nodiscard]] const char* ComponentName() const override { return "script"; }

    [[nodiscard]] TScript* Script() const { return script_.get(); }
    void Set(std::unique_ptr<TScript> script);
        // Replaces the script (the old one is destroyed)
    void Clear() { Set(nullptr); }

  private:
    std::unique_ptr<TScript> script_;
};
