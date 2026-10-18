// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *     defio.h - DEF reader / writer over reflection: text in, classes   *
// *               out; classes in, text out. No document tree.            *
// *************************************************************************
//
// Reading streams the defdoc lexer / parser (its Sink interface) straight
// into live objects:
//
//   key = value         -> refl::Set on the current object (table lookup by name)
//   node <name> { }     -> a child TNodeInst (any DEF_BLOCK type), parented to
//                          the current node, owned by the caller's `owned` list
//   <component> [label] { }
//                       -> a DEF_COMPONENT type, created through its factory
//                          and attached to the current node
//
// Unknown keys and blocks are errors with line numbers (the bake is
// strict). Reading never builds a defdoc::Node tree; the only Nodes that
// exist are the per-statement leaf values the parser hands over.
//
// Writing walks the same tables the other way and emits text in the house
// style, one `key = value` per saveable property, components and child
// nodes as nested blocks.
//
// *************************************************************************

#pragma once

#include "refl.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TNodeInst;
namespace defdoc { class Node; }

namespace defio {

// Read a node-tree document. Top-level statements apply to `root`. Child
// nodes are appended to `owned`. Returns false with the first error (and
// keeps going so later errors don't hide behind it).
//
// `post_load = false` deserializes without running any PostLoad(): the
// objects are data only, for baking to a buffer or for tools. Stamping
// from the buffer is where PostLoad belongs.
bool Read(std::string_view text, TNodeInst& root,
          std::vector<std::unique_ptr<TNodeInst>>& owned, std::string* error = nullptr,
          bool post_load = true);

// Read a flat document into one reflected object (no sub-blocks allowed).
bool ReadObject(std::string_view text, const refl::SType& type, void* obj, std::string* error = nullptr,
                bool post_load = true);
template <class T>
bool ReadObject(std::string_view text, T& obj, std::string* error = nullptr, bool post_load = true)
{
    const refl::SType* t = refl::Of(obj);
    if (!t) { if (error) *error = "type is not annotated"; return false; }
    return ReadObject(text, *t, &obj, error, post_load);
}

// Write a node tree as a document: the root's properties, components and
// child `node` blocks at top level (the form Read expects for its root).
[[nodiscard]] std::string Write(const TNodeInst& root);

// Write one object's saveable properties, one `key = value` line each,
// then nested Struct / List records as `key [name] { }` blocks, at
// `indent` levels (4 spaces). NoSave and ReadOnly fields and values equal
// to their defaults are skipped. `skip_name` omits a String `name`
// property (it is written as the block label by the caller).
void WriteObject(const refl::SType& type, const void* obj, std::string& out, int32_t indent = 0,
                 bool skip_name = false);
template <class T>
void WriteObject(const T& obj, std::string& out, int32_t indent = 0)
{
    if (const refl::SType* t = refl::Of(obj))
        WriteObject(*t, &obj, out, indent);
}

// Leaf conversion the reader uses: a parsed DEF value into a typed property
// value (`storage` backs a String's view).
bool ConvertValue(const refl::SProp& prop, const defdoc::Node& value, SPropValue& out,
                  std::string& storage, std::string* error = nullptr);

// Text form of one value (what Write emits after `key = `).
[[nodiscard]] std::string FormatValue(const refl::SProp& prop, const SPropValue& v);

// Parse one value in that text form. `storage` backs the string view of a
// String value; keep it alive as long as `out` is used.
bool ParseValue(const refl::SProp& prop, std::string_view text, SPropValue& out,
                std::string& storage, std::string* error = nullptr);

} // namespace defio
