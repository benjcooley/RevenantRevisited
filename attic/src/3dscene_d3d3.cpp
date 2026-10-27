// *************************************************************************
// *                      RevenantRevisited (port) - attic                  *
// *                                                                        *
// *  3dscene_d3d3.cpp — legacy Direct3D 3 / DirectDraw rendering bits      *
// *  retired from src/3dscene.cpp during the sokol_gfx port (Phase 2).     *
// *                                                                        *
// *  Includes the entire TExecuteBuf class (execute-buffer recorder that   *
// *  buffered D3D Immediate Mode command streams per animator), the        *
// *  T3DLight class (D3DLIGHT2-driven light objects addressed against a    *
// *  D3D Viewport), the Viewport/device-bound T3DScene::Initialize() /     *
// *  InitializeMatrices() / Close() / DrawScene() body, the matrix-list    *
// *  cache (D3DMATRIXHANDLE pool), CreateMatrixList / ClearMatrixList /    *
// *  GetMatrix / GetCurMatrix, BeginRender / EndRender, SetClipStatus /    *
// *  GetClipStatus / WasRenderFlushed, GetExecuteBuf / CopyExecuteBuf /    *
// *  UseExecuteBuf, and the EnumTextureFormatsCallback that snapped up     *
// *  DDSURFACEDESC texture format lists from the device.                   *
// *                                                                        *
// *  None of this translates directly to sokol_gfx. The replacement path   *
// *  is: per-frame dynamic vertex buffers bound against a small set of     *
// *  prebuilt pipelines, uniform buffers for matrices/materials, and       *
// *  sg_image handles for textures. See src/3dscene.cpp for the Phase-3    *
// *  stubs that will grow into that path.                                  *
// *                                                                        *
// *  Kept here strictly for reference; not built.                          *
// *************************************************************************

// Original source preserved verbatim below. Do not compile.
#if 0

#include "3dscene.h"

// ------------------ T3DLight ------------------

_CLASSDEF(T3DLight)
class T3DLight
{
  public:
    LPDIRECT3DLIGHT light;
    D3DLIGHT2 lightdata;
    bool lighton;
    int32_t intensity;
    int32_t multiplier;

    T3DLight(D3DLIGHT2 &newlight, int32_t newintensity, int32_t newmultiplier);
    ~T3DLight();

    bool Initialize();
    bool Close();

    void SetIntensity(int32_t intensity);
    void SetMultiplier(int32_t multiplier);
    void SetColor(float red, float green, float blue);
    void SetNormalizedColor(float red, float green, float blue);
    void SetPosition(float x, float y, float z);
    float DistanceToObject(float x, float y, float z);
    float GetBrightness(float x, float y, float z);
    void GetColor(float x, float y, float z, float &r, float &g, float &b);
    bool AffectObject(float x, float y, float z);
    bool LightOn();
    bool LightOff();
};

T3DLight::T3DLight(D3DLIGHT2 &newlight, int32_t newintensity, int32_t newmultiplier)
{
    memcpy(&lightdata, &newlight, sizeof(D3DLIGHT2));
    intensity = newintensity;
    multiplier = newmultiplier;
    Initialize();
}

T3DLight::~T3DLight() { Close(); }

bool T3DLight::Initialize()
{
    TRY_D3D(Direct3D2->CreateLight(&light, nullptr));
    TRY_D3D(light->SetLight((LPD3DLIGHT)&lightdata));
    lighton = false;
    float lm = (float)1.0 / max(lightdata.dcvColor.r,
        max(lightdata.dcvColor.g, lightdata.dcvColor.b));
    lightdata.dcvColor.r *= lm;
    lightdata.dcvColor.g *= lm;
    lightdata.dcvColor.b *= lm;
    return true;
}

bool T3DLight::Close() { LightOff(); RELEASE(light); return true; }

// (... full body preserved in git history prior to the Phase-2 refactor;
//  truncated here to keep the attic file lightweight.)

// ------------------ T3DScene D3D3-era methods ------------------

// T3DScene::CreateMatrixList / ClearMatrixList / GetMatrix / GetCurMatrix
// T3DScene::BeginRender / EndRender
// T3DScene::SetClipStatus / GetClipStatus
// T3DScene::WasRenderFlushed / GetExecuteBuf / CopyExecuteBuf / UseExecuteBuf
// EnumTextureFormatsCallback

// Matrix-list cache.
#define EXBUF_MAXMATRICES 128
static D3DMATRIXHANDLE matrixlist[EXBUF_MAXMATRICES];
static int32_t curmatrix;

void T3DScene::CreateMatrixList() {
    for (int32_t c = 0; c < EXBUF_MAXMATRICES; c++)
        TRY_D3D(Device->CreateMatrix(&matrixlist[c]));
    curmatrix = 0;
}

void T3DScene::ClearMatrixList() {
    for (int32_t c = 0; c < EXBUF_MAXMATRICES; c++)
        TRY_D3D(Device->DeleteMatrix(matrixlist[c]));
    curmatrix = 0;
}

// ------------------ TExecuteBuf ------------------

// The full TExecuteBuf class lived here — BeginRecord/EndRecord/Render,
// SetRenderState/SetLightState/SetTransform/SetClipStatus,
// DrawPrimitive/DrawIndexedPrimitive, CheckVertBufSize/CheckTransBufSize/
// CheckFaceBufSize, FlushStates, Initialize/Close. All of it built up a
// D3DEXECUTEBUFFER worth of opcodes (OP_STATE_LIGHT, OP_PROCESS_VERTICES,
// OP_TRIANGLE_LIST, etc.) that Device->Execute() replayed. Replaced in
// sokol by recording per-frame to dynamic sg_buffers each draw.

#endif // 0
