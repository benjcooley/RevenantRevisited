// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  maprenderer.h - Map renderer object                  *
// *************************************************************************

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "debugui.h"

struct S3DPoint;
class TGameMap;

class TMapRenderer
    : public IDebugTabContributor
{
  public:
    TMapRenderer();
    ~TMapRenderer();

    // post_load_hook (optional) fires after the startup sectors are
    // loaded but before the renderer scans them to build its initial
    // drawable list. Callers use it to inject objects (e.g. spawn the
    // player into a loaded sector) so the scan picks them up. The
    // hook receives (level, sector_x, sector_y) of the renderer's
    // anchor sector, which is the natural "where the game opens" pin.
    bool InitializeFromStartupArgs(std::function<void(int32_t level,
                                                       int32_t sector_x,
                                                       int32_t sector_y)> post_load_hook = {});

    // Bind the renderer to a TGameMap (owned by TMapManager).
    // Subscribes to the map's Loaded/Updated/Unloaded events:
    //  * Loaded / Updated -> rebuild drawable / light / scene caches
    //  * Unloaded         -> clear the SafeRef + drop caches before
    //                        the sectors are freed
    // Pass nullptr to detach.
    //
    // Camera anchor for the new map:
    //   use_level_origin = true  -> auto-center on the level's tile
    //                                bounds (default for level swaps)
    //   use_level_origin = false -> explicit (anchor_sx, anchor_sy)
    //                                sector pin (boot --sector path)
    //
    // Calling SetMap on a fresh map triggers an immediate
    // RebuildForCurrentMap.
    void SetMap(TGameMap* map,
                bool use_level_origin = true,
                int32_t anchor_sx = 0,
                int32_t anchor_sy = 0);

    // Force a fresh build of the renderer's non-owning draw records against
    // whatever map the SafeRef currently points at. Called from SetMap
    // and from the Loaded/Updated event handler; callers can also
    // invoke explicitly after mutating sector contents (e.g. spawning
    // the player) so the new objects show up on the next frame.
    void RebuildForCurrentMap();

    void Shutdown();
    void RenderFrame();
    void HandleMouseClick(int32_t button, int32_t x, int32_t y);
    void HandleMouseMove(int32_t button, int32_t x, int32_t y);
    void HandleKeyPress(int32_t key, bool down);
    const char* GetDebugTabName() const override;
    void DrawDebugTab() override;

    // Read the camera's current level / sector / world position. -1 in
    // any field means "not available yet" (renderer not initialised).
    void GetCameraStatus(int32_t& level, int32_t& sector_x, int32_t& sector_y,
                         int32_t& world_x, int32_t& world_y, int32_t& world_z) const;

    // Center the camera on a given world point at level `level`. Used by
    // the editor's Frame Selection command (and anything else that wants
    // to teleport the view).
    void SetCameraWorld(int32_t level, int32_t world_x, int32_t world_y, int32_t world_z);

    // Sun-shadow toggle. The map renderer re-pushes its shadow params to
    // the renderer every frame, so a one-shot Renderer->SetSunShadow*()
    // gets clobbered. Editor calls this to keep shadows off across frames.
    void SetSunShadowEnabled(bool enable);
    [[nodiscard]] bool SunShadowEnabled() const;

    // Last-frame draw counts, exposed so the editor status bar can show
    // exactly what's hitting the GPU. All-zero before the first frame.
    struct SDrawCounts {
        int32_t total_drawables  = 0;   // resident draw records for the loaded map
        int32_t draw_candidates  = 0;   // records visited by this frame's sector-bin pass
        int32_t resident_lights  = 0;
        int32_t active_lights    = 0;
        int32_t point_lights_considered = 0;
        int32_t point_lights_submitted = 0;
        int32_t tiles_submitted  = 0;   // tile pass actually drew
        int32_t tiles_visible_submitted = 0;
        int32_t tiles_gbuffer_border_submitted = 0;
        int32_t meshes_submitted = 0;   // mesh pass actually drew
        int32_t offscreen_culled = 0;   // dropped outside visual/padded checks
    };
    [[nodiscard]] SDrawCounts GetLastDrawCounts() const;

    // OBJID-pass plumbing: the renderer encodes (drawable_index + 1) into
    // each tile's id_target pixel. The editor reads a pixel and asks us
    // to map it back to an instance. Returns nullptr if no match.
    [[nodiscard]] class TObjectInstance* InstanceFromDrawId(uint32_t obj_id) const;
    // Inverse: id matching the most recent drawable list. Returns 0 if oi
    // isn't currently drawn.
    [[nodiscard]] uint32_t DrawIdForInstance(class TObjectInstance* oi) const;

    // Editor-driven set of selected map indices (the universal id-space
    // backing every TSafeRef<T>). Submit OR's kObjFlagSelected into
    // obj_id for any drawable whose underlying ref is in the set, so
    // multi-select highlights all of them naturally and dangling refs
    // (deleted instances) are dropped automatically -- only currently-
    // valid map indices ever get matched.
    void SetSelectedMapIndices(const std::vector<int32_t>& indices);
    [[nodiscard]] bool IsMapIndexSelected(int32_t mapindex) const;

    // Iso view + orthographic projection matrices that match the rendered
    // image pixel-for-pixel. Used by ImGuizmo (which wants view and proj
    // separately). Most overlays should prefer GetWorldToPixel() below
    // which composes the whole world->screen chain into one matrix.
    // Both outputs are 16-float column-major (OpenGL/HMM convention).
    void GetViewProj(float view_out[16], float proj_out[16],
                     int32_t vp_w, int32_t vp_h) const;

    // Combined world-to-pixel transform for the destination rect that
    // the rendered scene image lands in (editor: Game View ImGui::Image
    // rect; game: swapchain present rect). Returns a column-major 4x4
    // that maps a world point (x, y, z, 1) directly to (pixel.x,
    // pixel.y, depth, w) -- callers do a single matrix multiply, no
    // hand-rolled iso projection + cam.z fixups + logical-to-physical
    // scaling. This is the canonical world->screen camera matrix.
    void GetWorldToPixel(int32_t dst_x, int32_t dst_y,
                         int32_t dst_w, int32_t dst_h,
                         float out_mat44[16]) const;

    // Loaded-sector access for the editor's scene tree. Pointers stay
    // valid as long as the map renderer doesn't unload them; copy if
    // you need to outlive a frame.
    void GetLoadedSectors(std::vector<class TSector*>& out) const;

    // Reconcile non-owning draw records against the current resident map
    // contents. Sector transfers refresh instance state and bins; object-set
    // changes rebuild draw/light records over already-resident assets. Called
    // automatically at the top of RenderFrame.
    void SyncContentsCache();

    // Camera-anchored level. The renderer manages its own sector load
    // window; in editor mode (where MapPane.sectors[][] is empty
    // because player-driven paging isn't running), this is the only
    // authoritative current-level signal.
    [[nodiscard]] int32_t CameraLevel() const;

    // World-space position the camera was anchored to at startup. This
    // is what InitializeFromStartupArgs picked from --sector / --level
    // (or its default), pre-resolved to world coords so callers can use
    // it as a "where the game opens" anchor without re-deriving the
    // sector window.
    [[nodiscard]] S3DPoint CameraWorld() const;

    // Find the loaded TSector covering (level, sector_x, sector_y).
    // Editor commands that create/remove instances need this because
    // MapPane's standard add/remove path scans MapPane.sectors[][]
    // (empty in editor mode). Returns nullptr if no matching loaded
    // sector exists, e.g. the position is outside the loaded window.
    [[nodiscard]] class TSector* FindLoadedSector(int32_t level,
                                                  int32_t sector_x,
                                                  int32_t sector_y) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
