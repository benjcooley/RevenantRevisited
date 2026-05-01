// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        editorserialize.h - Text round-trip for SafeObjects            *
// *************************************************************************
//
// Text serialization for any class registered as a SafeObject root. The
// format (see memory/project_serialization_format.md) is line-based:
//
//   revenant.objects.v1
//   [obj]
//   @type=TObjectInstance:TILE:floor_stone_01
//   @refid=12345                            ; optional -- delete-undo only
//   pos=128,256,0
//   state=2
//   [obj]
//   ...
//
// `@type` is opaque to the parser; the leading segment names a SafeObject
// root (TObjectInstance today; TSector / assets / editor data later) and
// dispatches to the matching registered handler. Reflected properties
// (everything not `@`-prefixed) round-trip via the IReflection schema --
// same machinery undo uses.
//
// Two callers:
//   1. Clipboard cut/copy/paste -- include_refid=false, preserve_refid=false
//   2. Delete-undo "before" snapshots -- include_refid=true, preserve_refid=true
// *************************************************************************

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class TObjectInstance;

namespace EditorSerialize {

// Cheap header sniff. Safe on arbitrary clipboard text.
[[nodiscard]] bool LooksLikeOurs(std::string_view text);

// Serialize `objs` to canonical text. `include_refid` emits @refid lines
// (use for delete-undo); omit them for clipboard copy so paste always
// allocates fresh ids.
[[nodiscard]] std::string ToText(const std::vector<TObjectInstance*>& objs,
                                 bool include_refid);

// Parse `text` and instantiate fresh live objects in the world. Returns
// the mapindices of the instantiated objects, in input order. On parse
// failure or empty input, returns an empty vector.
//
// `preserve_refid`: when true and a block carries `@refid`, the new
// instance is re-keyed via SetMapIndex to the original id. Use this for
// delete-undo (heals every TSafeRef pointing at the deleted object).
// For clipboard paste, pass false -- always fresh ids.
std::vector<int32_t> FromText(std::string_view text, bool preserve_refid);

} // namespace EditorSerialize
