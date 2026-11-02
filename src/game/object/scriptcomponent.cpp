// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   scriptcomponent.cpp - the script an object runs                     *
// *************************************************************************

#include "game/object/scriptcomponent.h"

#include "game/script/script.h"

TScriptComponent::TScriptComponent() = default;
TScriptComponent::~TScriptComponent() = default;

void TScriptComponent::Set(std::unique_ptr<TScript> script)
{
    script_ = std::move(script);
}
