// Generated baseline defaults — written by the "Bake Defaults to Source"
// button in the debug panel's Lighting tab. The .cpp side of this pair has
// auto-edited regions between BEGIN/END markers; everything else in the
// .cpp is hand-edited and safe to modify normally.
//
// Layering (lowest priority -> highest):
//   1. Field initializers in revisited_settings.h / area.h  (minimum sane)
//   2. revisited_defaults.cpp baked tables                  (tuned in editor)
//   3. data/Modules/<...>/area.def                          (authored content)
//   4. Revenant.ini [Revisited] section                     (user overlay)
//
// Higher levels overwrite lower. The bake button captures the live
// debug-panel state -- which already reflects (1)+(2)+(3)+(4) applied to
// the renderer -- and writes it into level (2). Recompile, and (2) becomes
// the new shipped defaults visible to retail-mode boots that don't read
// (4) and may not have (3) for every area.

#pragma once

#include <cstdint>

class TArea;
struct SRevisitedSettings;

// Populate `out` with the baked global Revisited defaults. Called once at
// boot, AFTER SRevisitedSettings's field initializers run, BEFORE the INI
// is read. A do-nothing implementation is fine (just leaves field inits
// in place); the button regenerates this with real values.
void GetBakedRevisitedDefaults(SRevisitedSettings &out);

// Look up `area_name` in the baked per-area table; if found, push baked
// values into `area` via TArea::SetClassicLighting. Called at the end of
// TArea::Load so area.def values are present first and the bake overrides
// them. Returns true when an entry was found and applied.
bool ApplyBakedAreaDefaults(const char *area_name, TArea *area);

// Re-emit src/revisited_defaults.cpp using the live state of
// RevisitedSettings and (optionally) one TArea snapshot. The function:
//   * Locates the source file under the dev checkout (walks up from the
//     exe dir looking for src/revisited_defaults.cpp); fails when not in
//     a dev checkout (no src/ visible).
//   * Reads the existing file, surgically replaces the auto-managed
//     BEGIN/END regions, preserves hand-edited content + comments
//     elsewhere.
//   * Appends/updates the current area's row in the per-area table
//     keyed by name.
// Returns the path written into out_path (caller-provided buffer); false
// on any failure (logs a descriptive line).
bool BakeRevisitedDefaultsToSource(const TArea *current_area,
                                   char *out_path, int32_t out_len);
