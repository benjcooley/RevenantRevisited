// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 3dscene.h - 3D Scene Include File                     *
// *                                                                       *
// *  Ported (2026): the D3D3 execute-buffer recording path is gone. The   *
// *  TExecuteBuf class and T3DScene's BeginRender/EndRender/              *
// *  CopyExecuteBuf/UseExecuteBuf/CreateMatrixList API (used to cache     *
// *  Immediate Mode command streams per-animator) no longer have a        *
// *  sokol_gfx equivalent — pipelines + dynamic vertex buffers replace    *
// *  it. Callers that were holding PTExecuteBuf now just re-record each   *
// *  frame. The legacy code lives in attic/src/3dscene_d3d3.cpp.          *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"
#include "render3d_types.h"

_CLASSDEF(T3DScene)
_CLASSDEF(T3DAnimator)

struct SSurfaceDesc;

// ************************************
// * T3DScene - 3D System Main Object *
// ************************************
//
// Owns the global 3D view: viewport, camera, ambient light, dynamic light
// list, and the animator list that gets ticked each frame. Also mediates
// rendering state — the DrawPrimitive / SetRenderState calls below used to
// forward to a Direct3D device; they now forward to the sokol_gfx pipeline
// that T3DScene owns. Calls are valid between BeginScene() / EndScene().

class T3DScene
{
  protected:
    bool initialized;

  public:
    T3DScene();
    ~T3DScene();

    [[nodiscard]] bool Initialize();
      // Sets up viewport, ambient light, etc.
    [[nodiscard]] bool SetSize(int32_t x, int32_t y, int32_t width, int32_t height);
      // Sets the 3D viewport size
    [[nodiscard]] bool InitializeMatrices();
      // Initialize view matrices
    bool Close();

    void RestoreZBuffer(const SRect& r);
      // Restores a rectangle in the screen zbuffer.  Called by
      // T3DImage::RefreshZBuffer() before a 3D object is drawn.

  // Ambient light
    void SetAmbientLight(int32_t ambient);
    void SetAmbientColor(RSColor color);

  // Camera / scene draw
    void SetCameraPos(const S3DPoint& pos, int32_t zdist);
    bool UpdateCamera();
    void RefreshZBuffer();
    bool DrawScene();

  // Dynamic lights
    int32_t AddLight(const S3DPoint& pos, SColor color, int32_t intensity, int32_t multiplier);
    bool DeleteLight(int32_t lightid);
    bool SetLightIntensity(int32_t lightid, int32_t intensity);
    bool SetLightMultiplier(int32_t lightid, int32_t multiplier);
    bool SetLightColor(int32_t lightid, SColor color);
    bool SetLightPosition(int32_t lightid, const S3DPoint& pos);
    int32_t GetNumLights() const;
    bool GetClosestLights(int32_t x, int32_t y, int32_t z, int32_t& minlight1, int32_t& minlight2, int32_t& minlight3);
    bool LightAffectObject(int32_t x, int32_t y, int32_t z);
    void GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, int32_t& brightness);
    void GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, float& brightness);
    void GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z, int32_t& r, int32_t& g, int32_t& b);
    void GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z, float& r, float& g, float& b);
    void ResetAllLights();

  // Animator list
    int32_t AddAnimator(T3DAnimator* animator);
    bool RemoveAnimator(uint32_t animid);
    int32_t GetNumAnimators() const;

  // Texture format conversion (still needed — on-disk textures are in a
  // small set of legacy pixel formats, we convert to sokol-friendly ones).
    void GetTextureFormats();
    void GetClosestTextureFormat(const SSurfaceDesc* srcsd, SSurfaceDesc* dstsd);
    void ConvertTexture(
        const SSurfaceDesc* srcsd, const void* srcpixels, const void* srcpal,
        SSurfaceDesc* dstsd, void* dstpixels, void* dstpal);
      // Also generates mask/alpha data if a color key is supplied on srcsd.

  // Draw primitives. Valid only between BeginScene() / EndScene().
    bool BeginScene();
    bool EndScene();
    bool SetRenderState(ERender3DState rs, uint32_t data);
    bool GetRenderState(ERender3DState rs, uint32_t* data) const;
    bool SetLightState(ERender3DLightState ls, uint32_t data);
    bool GetLightState(ERender3DLightState ls, uint32_t* data) const;
    bool SetTransform(ERender3DTransform ts, const hmm_mat4* matrix);
    bool GetTransform(ERender3DTransform ts, hmm_mat4* matrix) const;
    bool DrawIndexedPrimitive(
        ERender3DPrim pt, ERender3DVertex vt, const void* v, uint32_t vc, const uint16_t* i, uint32_t ic, uint32_t flags);
    bool DrawPrimitive(
        ERender3DPrim pt, ERender3DVertex vt, const void* v, uint32_t vc, uint32_t flags);

  // Binds a texture for subsequent draws. Texture handle is an engine-side
  // key; the sg_image is resolved from it during pipeline binding.
    bool SetTexture(TTextureHandle hTexture, sg_image surface);
};

// Inline rotations about a center (h, k). Still used by effect code.
float RotateX(float h, float k, float x, float y, float angle);
float RotateY(float h, float k, float x, float y, float angle);
