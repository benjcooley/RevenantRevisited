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
//   uid=0x0123456789abcdef                  ; the node's uid, the identity
//   pos=128,256,0
//   state=2
//   [obj]
//   ...
//
// `@type` is opaque to the parser; the leading segment names a SafeObject
// root (TObjectInstance today; TSector / assets / editor data later) and
// dispatches to the matching registered handler. Reflected properties
// (everything not `@`-prefixed) round-trip via the annotated-property registry (refl.h) --
// same machinery undo uses.
//
// Two callers:
//   1. Clipboard cut/copy/paste -- preserve_identity=false (fresh uids)
//   2. Delete-undo "before" snapshots -- preserve_identity=true
// *************************************************************************

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "engine/core/saferef.h"

class TObjectInstance;

namespace EditorSerialize {

// Cheap header sniff. Safe on arbitrary clipboard text.
[[nodiscard]] bool LooksLikeOurs(std::string_view text);

// Serialize `objs` to canonical text: the node's properties (the uid among
// them, the object's identity) then the object's.
[[nodiscard]] std::string ToText(const std::vector<TObjectInstance*>& objs);

// Parse `text` and instantiate fresh live objects in the world. Returns
// references to the instantiated objects, in input order. On parse
// failure or empty input, returns an empty vector.
//
// `preserve_identity`: when true the new instance takes the uid the text
// carries, so every serialized reference to the deleted object names it
// again (delete-undo). For clipboard paste, pass false: a fresh uid.
std::vector<TSafeRef<TObjectInstance>> FromText(std::string_view text, bool preserve_identity);

} // namespace EditorSerialize
