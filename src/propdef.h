// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      propdef.h - DEF text <-> annotated properties (the bake step)    *
// *************************************************************************
//
// The generic walk that replaces every hand-written Parse*Def: a defdoc
// block's scalar keys are matched against the host's property table by
// name and applied through SetProp. Sub-blocks (objects / arrays of
// objects) are structural and left to the caller -- a node's children and
// component blocks are read by the prefab loader, not here.
//
// *************************************************************************

#pragma once

#include "defdoc.h"
#include "refl.h"

#include <string>

namespace propdef {

// Apply every scalar key of `block` to `obj` (described by `type`).
// Unknown keys and unconvertible values are reported through `error` (the
// first one) and make the call return false, after applying everything
// else -- the bake is strict so a typo in a DEF file is a build-time
// failure, not a silent default.
bool Apply(const refl::SType& type, void* obj, const defdoc::Node& block, std::string* error = nullptr);

// Write every saveable property of `obj` into `block` (an Object node),
// replacing keys of the same name. NoSave and ReadOnly fields are skipped.
void Write(const refl::SType& type, const void* obj, defdoc::Node& block);

// Convenience for a statically-known annotated type.
template <class T>
bool Apply(T& obj, const defdoc::Node& block, std::string* error = nullptr)
{
    const refl::SType* t = refl::Of(obj);
    if (!t) { if (error) *error = "type is not annotated"; return false; }
    return Apply(*t, &obj, block, error);
}
template <class T>
void Write(const T& obj, defdoc::Node& block)
{
    if (const refl::SType* t = refl::Of(obj))
        Write(*t, &obj, block);
}

} // namespace propdef
