// Global Revisited tunables — the "set once per game session" parameters
// that drive the renderer's modern lighting / parallax / camera defaults.
//
// Per-area values (AMBLIGHT, AMBCOLOR, …) live in area.def and override
// the relevant defaults below for that location. Things tweakable in the
// debug panel that don't change between areas land here instead so we can
// author them once, save them, and ship the result.
//
// The defaults baked into SRevisitedSettings below double as the engine's
// retail-mode renderer state: Apply* runs unconditionally so even runs
// without --revisited get a coherent sun + AO + point-light setup. Only
// the INI *read* is gated on --revisited.

#pragma once

#include <cstdint>

class TMapRenderer;
class TRenderer;

struct SRevisitedSettings {
    // ---- Rendering pipeline ------------------------------------------------
    int32_t lighting_mode = 0;      // 0=classic retail, 1=modern (deferred sun+AO)

    // ---- Sun (global; one sun in the world; per-area can later modulate) --
    double  sun_dir_x = -0.4;       // receiver -> sun, unnormalized (GPU normalizes)
    double  sun_dir_y = -0.6;
    double  sun_dir_z = 0.7;
    uint8_t sun_color_r = 255;
    uint8_t sun_color_g = 240;
    uint8_t sun_color_b = 210;
    double  sun_intensity = 1.0;
    double  light_ceiling = 1.5;

    // ---- Sun shadows -------------------------------------------------------
    bool    sun_shadow_enable      = true;
    double  sun_shadow_step_wu     = 32.0;
    double  sun_shadow_soft_px     = 3.0;
    int32_t sun_shadow_max_steps   = 64;
    double  sun_shadow_bias_wu     = 2.0;
    double  sun_shadow_depth_cut   = 16.0;

    // ---- Ambient occlusion (per-area AOSTRENGTH can override `ao_strength`)
    bool    ao_enable      = true;
    double  ao_radius_px   = 12.0;
    double  ao_strength    = 1.0;
    double  ao_bias        = 0.15;
    double  ao_max_dist_wu = 96.0;

    // ---- Parallax / perspective camera ------------------------------------
    bool    parallax_enable      = false;
    double  parallax_fov_deg     = 7.0;
    int32_t parallax_steps       = 64;
    int32_t parallax_refine      = 8;
    double  parallax_proxy_scale = 1.0;

    // ---- Normal reconstruction --------------------------------------------
    double  normal_radius   = 1.5;
    double  normal_hardness = 0.5;
    double  edge_threshold  = 64.0;
    double  depth_mul       = 1.0;

    // ---- Camera defaults --------------------------------------------------
    double  camera_zoom = 1.0;

    // ---- Point lights (global default; per-area POINTLIGHTINT/RANGE
    //      compose multiplicatively on top of these) -----------------------
    double  point_light_int_mul   = 1.0;
    double  point_light_range_mul = 1.0;
};

// The live in-memory copy. Populated by LoadRevisitedSettings on boot; the
// debug panel mutates this directly so SaveRevisitedSettings round-trips
// whatever the user sees on screen.
extern SRevisitedSettings RevisitedSettings;

// Read revisited/resources/Revenant.ini (overlay) into RevisitedSettings.
// Missing keys keep their default; missing file is fine (uses all defaults).
// No-op when --revisited is off (RevisitedSettings stays at C++ defaults).
bool LoadRevisitedSettings();

// Push RevisitedSettings into renderer state. Two split functions so each
// can be called when its respective owner becomes live (TRenderer in
// display init, TMapRenderer in PlayScreen init). Both are null-safe and
// idempotent — always call to push current settings, even without
// --revisited (the defaults still matter).
void ApplyRevisitedSettingsToRenderer(TRenderer *r);
void ApplyRevisitedSettingsToMapRenderer(TMapRenderer *mr);

// Write RevisitedSettings back to revisited/resources/Revenant.ini. Round-
// trips through simpleini so existing comments + non-Revisited sections in
// that file (if any) survive. Creates the file if it doesn't exist.
// Returns the absolute path written (for the debug-panel toast).
// Fails (returns false) when no writable overlay is reachable — i.e. when
// running without --revisited or against a packed .rvr archive.
bool SaveRevisitedSettings(char *out_path, int32_t out_len);
