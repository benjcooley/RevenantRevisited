// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visualcomponent.cpp - what a node shows                             *
// *************************************************************************

#include "engine/render/visualcomponent.h"

#include "engine/scene/node.h"

TVisualComponent* ReplacingVisual(const TNode& node)
{
    for (const auto& component : node.Components())
        if (auto* visual = dynamic_cast<TVisualComponent*>(component.get()))
            if (visual->ReplacesDefaultVisual())
                return visual;
    return nullptr;
}
