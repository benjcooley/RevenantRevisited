// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *       scriptvalue.h - object names and values in command lines        *
// *************************************************************************
//
// The two pieces of the command language every command shares (ARCHITECTURE
// §6.2): naming an object and evaluating an expression. Both are retail ports;
// docs/gameflow/forensics/COMMAND_SYSTEM.md §2.4 has the grammar.

#pragma once

#include <cstdint>
#include <optional>

class TObjectInstance;
class TScript;
class TToken;

// The object a command line names (retail 0x0041e690). In order:
//   this        the object running the line (`caller`)
//   user        the script's user (TScript::User)
//   player      the main player
//   target      a fighting character's opponent (root COMBAT or BOW action)
//   current     the object `setcurrent` chose (not ported: always none)
//   party<N>    the Nth member of the user's party (multiplayer)
//   an alias    an object the running trigger names: "item", "enemy", ...
//   <name>      the closest object with exactly that name, from `caller`
// Returns nullptr when nothing answers to the name.
[[nodiscard]] TObjectInstance* ResolveScriptObject(const char* name,
                                                   TObjectInstance* caller,
                                                   TScript* script);

// Evaluates the rest of the line as a value or condition (retail 0x0041f230):
// numbers, quoted text (hashed), game states, `<object>.<member>` values
// (state, stats, position, inventory amounts, distances, ...) combined left to
// right with = <> > < >= <= and or not + - * /. `context` is the object the
// command applies to, `caller` the one whose script runs it. Returns nullopt
// when the expression can't be evaluated (bad syntax, an object that isn't
// there, a member that isn't ported); retail's `if` takes that as false.
[[nodiscard]] std::optional<int32_t> EvaluateExpression(TToken& t,
                                                        TObjectInstance* context,
                                                        TObjectInstance* caller);
