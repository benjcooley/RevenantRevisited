// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   nodepack.h - packed (binary) reader / writer for a node tree.       *
// *                The baked form of a DEF node document.                 *
// *************************************************************************
//
// Message layout (packed.h wire format), per node:
//
//   <the node's own properties, tags from its reflection table>
//   kTagComponent  Bytes { 1: def_name  2: label  3: Bytes(component props) }   (repeated)
//   kTagChild      Bytes { 1: name      2: Bytes(child node message) }          (repeated)
//
// Pack walks live objects through the reflection tables; Unpack creates
// child nodes and components through the registry factories and feeds
// each sub-message to refl::Unpack. Unknown tags skip; a component whose
// type is no longer registered is dropped with a warning in `error`.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TNode;
class TPackedWriter;
class TPackedReader;

namespace nodepack {

constexpr uint16_t kTagComponent = 16000;
constexpr uint16_t kTagChild     = 16001;
// Inside a component / child sub-message.
constexpr uint16_t kSubName  = 1;    // def_name (component) / name (child)
constexpr uint16_t kSubLabel = 2;    // component label
constexpr uint16_t kSubBody  = 3;    // component props
constexpr uint16_t kSubNode  = 2;    // child node message


void Pack(const TNode& root, TPackedWriter& w);
// `apply_root_props = false` leaves the root's own properties alone
// (stamping a prefab under an object that already has its position).
// `post_load = false` deserializes data only (no PostLoad), for tools.
bool Unpack(TPackedReader& r, TNode& root,
            std::vector<std::unique_ptr<TNode>>& owned, std::string* error = nullptr,
            bool apply_root_props = true, bool post_load = true);

} // namespace nodepack
