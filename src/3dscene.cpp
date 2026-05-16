// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dscene.cpp - 3D Scene module                       *
// *                                                                       *
// *  Ported (Phase 2): D3D3 execute-buffer recording path, viewport,      *
// *  D3D light objects, and matrix-list cache have been removed. The      *
// *  full legacy code lives in attic/src/3dscene_d3d3.cpp. The methods    *
// *  declared in 3dscene.h are now bool stubs that return success -- the  *
// *  renderer backend will replace the inner bodies in Phase 3.           *
// *************************************************************************

#include "3dscene.h"

#include <cmath>
#include <cstring>

#include "3dimage.h"
#include "mappane.h"

// 3D Render State Flags (still referenced by the rest of the game code).
bool FlatShade = false;
bool SimpleLight = false;
bool UseTextures = true;
bool DitherEnable = true;
bool BlendEnable = true;
bool SpecularEnable = true;
bool ZEnable = true;
bool BilinearFilter = false;
bool NoUpdateRects = false;
int32_t MaxLights = 1;
bool UseDirLight = true;
int32_t DirLightPercent = 85;
bool Double3D, Triple3D;

int32_t Ambient3D = 100;
int32_t LightRange3D = 100;
int32_t LightMult3D = 250;       // retail revenant.ini default; per-light intensity scale (percent)
bool    EnhancedLighting = true; // retail revenant.ini default

// DLS brightness routine (gives brightness given distance).
extern double GetLightBrightness(int32_t dist, int32_t intensity, int32_t multiplier);

#define LIGHT3DEXTRADIST ((float)LightRange3D / 100.0f)

// quick hacks — still used by effect code.
float RotateX(float h, float k, float x, float y, float angle)
{ return (float)(h + (x - h) * cosf(angle) - (y - k) * sinf(angle)); }
float RotateY(float h, float k, float x, float y, float angle)
{ return (float)(k + (y - k) * cosf(angle) + (x - h) * sinf(angle)); }

// ************************************************************
// * T3DLight - Information for a light in a 3D scene object *
// ************************************************************
//
// Phase-2 replacement: pure CPU light model — color, position, attenuation
// coefficients, and a lit/unlit flag. The Phase-1 version owned a
// LPDIRECT3DLIGHT attached to a viewport; that's gone. The GetBrightness /
// GetColor helpers still do the same math the character lighting code
// expects (T3DScene::GetLightBrightness / GetLightColor forward to them).

class T3DLight
{
  public:
    float red, green, blue;     // Normalized (max component = 1.0)
    float px, py, pz;           // Position (point light)
    bool  lighton;
    int32_t intensity;
    int32_t multiplier;

    T3DLight(float r, float g, float b, float x, float y, float z,
             int32_t newintensity, int32_t newmultiplier)
    {
        red = r; green = g; blue = b;
        px = x; py = y; pz = z;
        intensity = newintensity;
        multiplier = newmultiplier;
        lighton = false;
        Normalize();
    }
    ~T3DLight() {}

    void Normalize()
    {
        float m = red; if (green > m) m = green; if (blue > m) m = blue;
        if (m <= 0.0f) return;
        float lm = 1.0f / m;
        red *= lm; green *= lm; blue *= lm;
    }

    void SetIntensity(int32_t v) { intensity = v; }
    void SetMultiplier(int32_t v) { multiplier = v; }

    void SetColor(float r, float g, float b)
    { red = r; green = g; blue = b; }

    void SetNormalizedColor(float r, float g, float b)
    { red = r; green = g; blue = b; Normalize(); }

    void SetPosition(float x, float y, float z)
    { px = x; py = y; pz = z; }

    float DistanceToObject(float x, float y, float z)
    {
        x -= px; y -= py; z -= pz;
        float d = sqrtf(x * x + y * y);
        return sqrtf(d * d + z * z);
    }

    float GetBrightness(float x, float y, float z)
    {
        float d = DistanceToObject(x, y, z) / LIGHT3DEXTRADIST;
        if (d < intensity)
        {
            float b = (float)::GetLightBrightness((int32_t)d, intensity, multiplier);
            return b < 1.0f ? b : 1.0f;
        }
        return 0.0f;
    }

    void GetColor(float x, float y, float z, float &r, float &g, float &b)
    {
        float i = GetBrightness(x, y, z);
        r = red * i; g = green * i; b = blue * i;
    }

    bool AffectObject(float x, float y, float z)
    {
        float lr, lg, lb;
        GetColor(x, y, z, lr, lg, lb);
        if (lr > 0.0f || lg > 0.0f || lb > 0.0f)
            return LightOn();
        return false;
    }

    bool LightOn()  { lighton = true;  return true; }
    bool LightOff() { lighton = false; return true; }
};

// ***************************************
// * T3DScene - 3DScene global variables *
// ***************************************

static SColor AmbientColor256;
static int32_t AmbientValue;
static uint32_t AmbientColor;         // Packed ARGB (was D3DCOLOR)
static S3DPoint CameraPos;
static int32_t scenex, sceney, scenewidth, sceneheight;
static int32_t dirlight;

#define MAX3DANIMATORS 128
typedef TPointerArray<T3DAnimator, 32, 32> T3DAnimatorArray;
static T3DAnimatorArray AnimatorArray;

#define MAX3DLIGHTS 64
typedef TPointerArray<T3DLight, 32, 32> T3DLightArray;
static T3DLightArray LightArray;

// Current transform / state caches (fed by SetTransform / SetRenderState /
// SetLightState so GetTransform etc. still return something sensible).
static hmm_mat4 curtransmatrix[4];
static uint32_t currenderstate[32];
static uint32_t curlightstate[8];

// *********************************
// * 3DScene - Main 3D interface!! *
// *********************************

T3DScene::T3DScene()
{
    initialized = false;
}

// Trivial dtor: explicit OnMapUnloaded() must be called via the owner's
// teardown path (TMapPane::Close → Scene3D.OnMapUnloaded()) before the
// global destructs. Walking AnimatorArray/LightArray from a global dtor
// would race other TUs that hold references into those arrays.
T3DScene::~T3DScene() = default;

bool T3DScene::OnMapLoaded()
{
    if (initialized || Ignore3D)
        return true;

    initialized = true;

    // NOTE(port): viewport + device + execute-buffer subsystem + background
    // material + directional D3D light all lived here. Retired in Phase 2.
#if 0 // TODO(port): real renderer-backend path -- Phase 3
    // CreateViewport, CreateExecuteBuffer, CreateMaterial, CreateLight, etc.
#endif

    // Still need to size the scene and record an initial directional
    // ambient light (pure CPU — used by the character lighting code).
    SetSize(0, 0, 0, 0);
    InitializeMatrices();

    T3DLight* dir = new T3DLight(
        1.0f, 1.0f, 1.0f,           // White
        0.0f, 0.0f, 200.0f,         // Position
        1, 1);                       // Intensity / multiplier
    dirlight = LightArray.Add(dir);

    GetTextureFormats();

    return true;
}

bool T3DScene::SetSize(int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (!initialized || Ignore3D)
        return true;

    scenex = x; sceney = y;
    scenewidth = width; sceneheight = height;

#if 0 // TODO(port): push viewport dims into the sokol pipeline — Phase 3
    // D3DVIEWPORT setup / Viewport->SetViewport() lived here.
#endif
    return true;
}

bool T3DScene::InitializeMatrices()
{
    // View / projection / world start as identity; Phase-3 code will upload
    // these into a uniform buffer each frame.
    hmm_mat4 I = HMM_Mat4d(1.0f);
    curtransmatrix[(int)ERender3DTransform::World]      = I;
    curtransmatrix[(int)ERender3DTransform::View]       = I;
    curtransmatrix[(int)ERender3DTransform::Projection] = I;
    return true;
}

bool T3DScene::OnMapUnloaded()
{
    if (!initialized)
        return false;

    AnimatorArray.Clear();
    LightArray.DeleteAll();

#if 0 // TODO(port): tear down sokol pipelines + images — Phase 3
#endif

    initialized = false;
    return true;
}

void T3DScene::RestoreZBuffer(const SRect& r)
{
    if (NoScrollZBuffer)
        MapPane.DrawRestoreRect(r.x(), r.y(), r.w(), r.h(),
            DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);

#if 0 // TODO(port): rectangle copy from the real z-buffer to the scroll
      // z-buffer used to live here; requires sokol blit path — Phase 3.
    if (Display.UsingClearZBuffer()) { ... }
#endif
}

void T3DScene::SetCameraPos(const S3DPoint& pos, int32_t /*zdist*/)
{
    if (!initialized)
        return;
    CameraPos = pos;

#if 0 // TODO(port): build the view matrix from camera pos + CAMERAANGLE
      // and push into the view-proj uniform buffer — Phase 3.
#endif
}

bool T3DScene::UpdateCamera()
{
    // Stub — Phase-3 will rebuild view-proj uniforms here.
    return true;
}

void T3DScene::RefreshZBuffer()
{
    for (int32_t c = 0; c < AnimatorArray.NumItems(); c++)
        if (AnimatorArray[c] != nullptr)
            AnimatorArray[c]->RefreshZBuffer();
}

bool T3DScene::DrawScene()
{
    if (!initialized)
        return false;
    if (!Show3D)
        return true;

    // NOTE(port): the render-state dirty check, device Begin/End scene,
    // viewport clear, and per-animator DrawIndexedPrimitive loop all lived
    // here. For Phase-2 compile we still walk the animator list so that
    // Pre/Render/PostRender keep getting called — they're the code path the
    // rest of the game watches for Updated extents and light resets.
    BeginScene();

    for (int32_t c = 0; c < AnimatorArray.NumItems(); c++)
    {
        if (AnimatorArray[c] == nullptr)
            continue;

        TObjectInstance* inst = AnimatorArray[c]->GetObjInst();
        TObjectImagery*  img  = AnimatorArray[c]->GetImagery();

        if ((int32_t)inst->GetFrame() == -1)
        {
            char buf[512];
            sprintf(buf, "Zero frame on state %d in object \'%s\'",
                inst->GetState(), inst->GetName());
            FatalError(buf);
        }

        if ((inst->GetFlags() & OF_INVISIBLE) ||
            (uint32_t)inst->GetState() >= (uint32_t)img->GetHeader()->numstates ||
            (uint32_t)inst->GetFrame() >= (uint32_t)img->GetHeader()->states[inst->GetState()].frames)
            continue;

        AnimatorArray[c]->PreRender();
        AnimatorArray[c]->Render();
        AnimatorArray[c]->PostRender();
    }

    EndScene();
    return true;
}

void T3DScene::SetAmbientLight(int32_t ambient)
{
    if (!initialized)
        return;

    AmbientValue = (ambient * 8) * Ambient3D / 100;
    if (AmbientValue > 255) AmbientValue = 255;

    int32_t red   = AmbientColor256.red   * AmbientValue / 256;
    int32_t green = AmbientColor256.green * AmbientValue / 256;
    int32_t blue  = AmbientColor256.blue  * AmbientValue / 256;

    AmbientColor = (uint32_t)((255u << 24) | ((red & 0xFF) << 16) |
                              ((green & 0xFF) << 8) | (blue & 0xFF));
}

void T3DScene::SetAmbientColor(RSColor color)
{
    if (!initialized)
        return;

    AmbientColor256 = color;

    int32_t maxc = color.red;
    if (color.green > maxc) maxc = color.green;
    if (color.blue  > maxc) maxc = color.blue;
    if (maxc < 1) maxc = 1;

    int32_t nr = (int32_t)AmbientColor256.red   * 256 / maxc; if (nr > 255) nr = 255;
    int32_t ng = (int32_t)AmbientColor256.green * 256 / maxc; if (ng > 255) ng = 255;
    int32_t nb = (int32_t)AmbientColor256.blue  * 256 / maxc; if (nb > 255) nb = 255;
    AmbientColor256.red   = (uint8_t)nr;
    AmbientColor256.green = (uint8_t)ng;
    AmbientColor256.blue  = (uint8_t)nb;

    int32_t red   = AmbientColor256.red   * AmbientValue / 256;
    int32_t green = AmbientColor256.green * AmbientValue / 256;
    int32_t blue  = AmbientColor256.blue  * AmbientValue / 256;

    AmbientColor = (uint32_t)((255u << 24) | ((red & 0xFF) << 16) |
                              ((green & 0xFF) << 8) | (blue & 0xFF));
}

// ************ Light Functions **************

int32_t T3DScene::AddLight(const S3DPoint& pos, SColor color, int32_t intensity, int32_t multiplier)
{
    if (!initialized)
        return -1;

    T3DLight* l = new T3DLight(
        (float)color.red   / 256.0f,
        (float)color.green / 256.0f,
        (float)color.blue  / 256.0f,
        (float)pos.x, (float)pos.y, (float)pos.z,
        intensity, multiplier);
    return LightArray.Add(l);
}

bool T3DScene::DeleteLight(int32_t lightid)
{
    if (!initialized || lightid < 0 || lightid >= LightArray.NumItems())
        return false;
    LightArray.Delete(lightid);
    return true;
}

bool T3DScene::SetLightIntensity(int32_t lightid, int32_t intensity)
{
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return false;
    LightArray[lightid]->SetIntensity(intensity);
    return true;
}

bool T3DScene::SetLightMultiplier(int32_t lightid, int32_t multiplier)
{
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return false;
    LightArray[lightid]->SetMultiplier(multiplier);
    return true;
}

bool T3DScene::SetLightColor(int32_t lightid, SColor color)
{
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return false;
    LightArray[lightid]->SetNormalizedColor(
        (float)color.red   / 256.0f,
        (float)color.green / 256.0f,
        (float)color.blue  / 256.0f);
    return true;
}

bool T3DScene::SetLightPosition(int32_t lightid, const S3DPoint& pos)
{
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return false;
    LightArray[lightid]->SetPosition((float)pos.x, (float)pos.y, (float)pos.z);
    return true;
}

int32_t T3DScene::GetNumLights() const
{
    if (!initialized) return 0;
    return LightArray.NumItems();
}

bool T3DScene::GetClosestLights(int32_t x, int32_t y, int32_t z,
    int32_t &minlight1, int32_t &minlight2, int32_t &minlight3)
{
    if (!initialized)
        return false;

    float mindist = 10000.0f, mindist2 = 10000.0f, mindist3 = 10000.0f;
    minlight1 = minlight2 = minlight3 = -1;

    for (int32_t c = 0; c < LightArray.NumItems(); c++)
    {
        if (LightArray[c] == nullptr)
            continue;

        float dist = LightArray[c]->DistanceToObject((float)x, (float)y, (float)z) /
                     LIGHT3DEXTRADIST;
        if (dist < mindist)
        {
            mindist3 = mindist2; minlight3 = minlight2;
            mindist2 = mindist;  minlight2 = minlight1;
            mindist  = dist;     minlight1 = c;
        }
        else if (dist < mindist2)
        {
            mindist3 = mindist2; minlight3 = minlight2;
            mindist2 = dist;     minlight2 = c;
        }
        else if (dist < mindist3)
        {
            mindist3 = dist; minlight3 = c;
        }
    }

    return minlight1 >= 0;
}

bool T3DScene::LightAffectObject(int32_t x, int32_t y, int32_t z)
{
    if (!initialized)
        return false;

    if (UseDirLight)
        LightArray[dirlight]->LightOn();

    int32_t ml, ml2, ml3;
    GetClosestLights(x, y, z, ml, ml2, ml3);

    if (ml  >= 0 && !LightArray[ml ]->AffectObject((float)x, (float)y, (float)z)) return false;
    if (MaxLights > 1 && ml2 >= 0 &&
        !LightArray[ml2]->AffectObject((float)x, (float)y, (float)z)) return false;
    if (MaxLights > 2 && ml3 >= 0 &&
        !LightArray[ml3]->AffectObject((float)x, (float)y, (float)z)) return false;

    return false;
}

void T3DScene::GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, int32_t &brightness)
{
    brightness = 0;
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return;

    float i = LightArray[lightid]->GetBrightness((float)x, (float)y, (float)z);
    int32_t v = (int32_t)(i * 255.0f);
    if (v < 0)   v = 0;
    if (v > 255) v = 255;
    brightness = v;
}

void T3DScene::GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, float &brightness)
{
    brightness = 0.0f;
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return;
    brightness = LightArray[lightid]->GetBrightness((float)x, (float)y, (float)z);
}

void T3DScene::GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z,
    int32_t &r, int32_t &g, int32_t &b)
{
    r = g = b = 0;
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return;

    float lr, lg, lb;
    LightArray[lightid]->GetColor((float)x, (float)y, (float)z, lr, lg, lb);

    auto clamp = [](float f) -> int32_t {
        int32_t v = (int32_t)(f * 255.0f);
        if (v < 0) return 0;
        if (v > 255) return 255;
        return v;
    };
    r = clamp(lr); g = clamp(lg); b = clamp(lb);
}

void T3DScene::GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z,
    float &r, float &g, float &b)
{
    r = g = b = 0.0f;
    if (!initialized || lightid < 0 ||
        lightid >= LightArray.NumItems() ||
        LightArray[lightid] == nullptr)
        return;
    LightArray[lightid]->GetColor((float)x, (float)y, (float)z, r, g, b);
}

void T3DScene::ResetAllLights()
{
    if (!initialized)
        return;
    for (int32_t c = 0; c < LightArray.NumItems(); c++)
        if (LightArray[c] != nullptr)
            LightArray[c]->LightOff();
}

// ************ Object Functions **************

int32_t T3DScene::AddAnimator(T3DAnimator* animator)
{
    if (!initialized)
        return -1;
    return AnimatorArray.Add(animator);
}

bool T3DScene::RemoveAnimator(uint32_t animid)
{
    if (!initialized || animid >= (uint32_t)AnimatorArray.NumItems() ||
        AnimatorArray[animid] == nullptr)
        return false;
    AnimatorArray.Remove(animid);
    return true;
}

int32_t T3DScene::GetNumAnimators() const
{
    if (!initialized) return 0;
    return AnimatorArray.NumItems();
}

// ************************
// * Texture Format Stuff *
// ************************
//
// The D3D3 path used EnumTextureFormats to discover supported DDSURFACEDESC
// pixel formats and snapped SSurfaceDesc descriptors to the closest match.
// With sokol we upload everything as RGBA8 (or the loader's chosen sokol
// pixel format) so the format-selection logic is vestigial — the
// GetClosestTextureFormat stub just copies the input.

void T3DScene::GetTextureFormats()
{
    // No device enumeration in the portable path — sokol chooses formats.
}

void T3DScene::GetClosestTextureFormat(const SSurfaceDesc* srcsd, SSurfaceDesc* dstsd)
{
    if (!srcsd || !dstsd)
        return;

#if 0 // TODO(port): real pixel-format selection — Phase 3
    // Original code enumerated texformats[] and ranked RGB / palettized
    // candidates by matching bit counts.
#endif

    // For now: feed the source format straight through; loaders will hand
    // normalized texture data to the renderer at upload time. This must not
    // depend on T3DScene::OnMapLoaded(): source assets can be realized before
    // legacy Scene3D viewport state exists, and callers still need a valid
    // descriptor for renderer-owned texture handles.
    *dstsd = *srcsd;
}

// ConvertTexture: legacy pixel-format converter. Kept because the texture
// loader still depends on it to turn whatever lives on-disk (palettized /
// 15-bit / 24-bit) into a canonical upload buffer. The implementation
// operates on the SPixelFormat bits inside SSurfaceDesc — unchanged logic,
// just no DDPIXELFORMAT flag names. For Phase 2, stub it to a straight
// memcpy when formats match and leave the full re-encoder for Phase 3.

static inline int32_t countbits(uint32_t d)
{
    int32_t n = 0;
    for (int32_t c = 0; c < 32; c++)
        if (d & (1u << c))
            n++;
    return n;
}
static inline int32_t getmaskshift(uint32_t mask)
{
    if (!mask) return 0;
    int32_t s = 0;
    while (!(mask & 1) && s < 32) { mask >>= 1; s++; }
    return s;
}

void T3DScene::ConvertTexture(
    const SSurfaceDesc* srcsd, const void* srcpixels, const void* /*srcpal*/,
    SSurfaceDesc* dstsd, void* dstpixels, void* /*dstpal*/)
{
    if (!initialized || !srcsd || !dstsd || !srcpixels || !dstpixels)
        return;

#if 0 // TODO(port): full palettized + RGB re-encoder — Phase 3
    // The original walked pixels translating between arbitrary DDPIXELFORMAT
    // bit layouts (including paletted-to-RGBA and RGBA-to-paletted), also
    // producing color-key-driven alpha masks on the fly.
#endif

    // Fast path: identical descriptors → raw memcpy.
    if (srcsd->width == dstsd->width &&
        srcsd->height == dstsd->height &&
        srcsd->pitch == dstsd->pitch &&
        std::memcmp(&srcsd->pixelFormat, &dstsd->pixelFormat, sizeof(srcsd->pixelFormat)) == 0)
    {
        int32_t bytes = (int32_t)(srcsd->pitch * srcsd->height);
        std::memcpy(dstpixels, srcpixels, bytes);
    }
    // Otherwise: leave dst as-is — texture will draw blank until Phase 3.
    (void)countbits; (void)getmaskshift;
}

// ************************************
// * 3DScene Draw Primitive Functions *
// ************************************
//
// These methods used to forward into the D3D device / execute-buffer
// recorder. Phase-2 keeps them as state-tracking stubs so the rest of the
// code compiles and runs its state machine. Phase 3 will wire them into a
// per-frame dynamic sokol pipeline.

bool T3DScene::BeginScene()
{
#if 0 // TODO(port): sokol sg_begin_pass — Phase 3
#endif
    return true;
}

bool T3DScene::EndScene()
{
#if 0 // TODO(port): sokol sg_end_pass / sg_commit — Phase 3
#endif
    return true;
}

bool T3DScene::SetRenderState(ERender3DState rs, uint32_t data)
{
    uint32_t idx = (uint32_t)rs;
    if (idx < (sizeof(currenderstate) / sizeof(currenderstate[0])))
        currenderstate[idx] = data;
    return true;
}

bool T3DScene::GetRenderState(ERender3DState rs, uint32_t* data) const
{
    if (!data) return false;
    uint32_t idx = (uint32_t)rs;
    if (idx < (sizeof(currenderstate) / sizeof(currenderstate[0])))
        *data = currenderstate[idx];
    else
        *data = 0;
    return true;
}

bool T3DScene::SetLightState(ERender3DLightState ls, uint32_t data)
{
    uint32_t idx = (uint32_t)ls;
    if (idx < (sizeof(curlightstate) / sizeof(curlightstate[0])))
        curlightstate[idx] = data;
    return true;
}

bool T3DScene::GetLightState(ERender3DLightState ls, uint32_t* data) const
{
    if (!data) return false;
    uint32_t idx = (uint32_t)ls;
    if (idx < (sizeof(curlightstate) / sizeof(curlightstate[0])))
        *data = curlightstate[idx];
    else
        *data = 0;
    return true;
}

bool T3DScene::SetTransform(ERender3DTransform ts, const hmm_mat4* matrix)
{
    if (!matrix) return false;
    uint32_t idx = (uint32_t)ts;
    if (idx < (sizeof(curtransmatrix) / sizeof(curtransmatrix[0])))
        curtransmatrix[idx] = *matrix;
    return true;
}

bool T3DScene::GetTransform(ERender3DTransform ts, hmm_mat4* matrix) const
{
    if (!matrix) return false;
    uint32_t idx = (uint32_t)ts;
    if (idx < (sizeof(curtransmatrix) / sizeof(curtransmatrix[0])))
        *matrix = curtransmatrix[idx];
    return true;
}

bool T3DScene::DrawIndexedPrimitive(
    ERender3DPrim /*pt*/, ERender3DVertex /*vt*/,
    const void* /*v*/, uint32_t /*vc*/,
    const uint16_t* /*i*/, uint32_t /*ic*/, uint32_t /*flags*/)
{
#if 0 // TODO(port): allocate from dynamic sg_buffer, bind pipeline, draw — Phase 3
#endif
    return true;
}

bool T3DScene::DrawPrimitive(
    ERender3DPrim /*pt*/, ERender3DVertex /*vt*/,
    const void* /*v*/, uint32_t /*vc*/, uint32_t /*flags*/)
{
#if 0 // TODO(port): allocate from dynamic sg_buffer, bind pipeline, draw — Phase 3
#endif
    return true;
}

bool T3DScene::SetTexture(TTextureHandle hTexture)
{
    // Tracked as the current "texture handle" render-state slot for
    // compatibility with the rest of the game code. Actual binding is done
    // by the Phase-3 sokol pipeline setup.
    return SetRenderState(ERender3DState::TexAddress, (uint32_t)hTexture);
}
