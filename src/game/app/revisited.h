// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *           revisited.h - Runtime toggles for Revisited features        *
// *************************************************************************
//
// The single accessor for the Revisited enhancement-layer toggle namespace
// documented in revisited/README.md ("Toggle model for behavioral changes").
//
// Toggles live in <SavePath>/Revenant.ini under [Revisited]:
//
//   [Revisited]
//   BugFix.MonsterImpactAniFallback = true
//   Visual.HighResTextures          = true
//   UI.AnchoredLayout               = false
//
// Default per key is **false** -- vanilla retail behavior. An installed
// overlay still produces retail behavior unless the user explicitly opts
// in. This keeps the vanilla path live and reviewable; revisited features
// are layered on top via call-site gates:
//
//   if (Revisited::IsEnabled("BugFix.MonsterImpactAniFallback")) {
//       // revisited path
//   } else {
//       // verbatim retail path
//   }
//
// Key convention is dotted "Category.FlagName" (Category is conventional,
// not enforced). Suggested top-level categories per the README: BugFix.*,
// Tweak.*, Visual.*, UI.*, Loc.*. See revisited/README.md for full
// rationale.
//
// *************************************************************************

#pragma once

namespace Revisited {

// Returns true if the named [Revisited] toggle is enabled in
// <SavePath>/Revenant.ini. Default = false (vanilla).
//
// Safe to call from any subsystem -- internally sets the INI section
// to "Revisited" before each read, matching the per-call section
// convention used elsewhere (see src/ctrlmap.cpp). nullptr / empty
// key returns false.
bool IsEnabled(const char* key);

}  // namespace Revisited
