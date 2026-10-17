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

class TNodeInst;
class TPackedWriter;
class TPackedReader;

namespace nodepack {

constexpr uint16_t kTagComponent = 16000;
constexpr uint16_t kTagChild     = 16001;

void Pack(const TNodeInst& root, TPackedWriter& w);
bool Unpack(TPackedReader& r, TNodeInst& root,
            std::vector<std::unique_ptr<TNodeInst>>& owned, std::string* error = nullptr);

} // namespace nodepack
