// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dscene.cpp - 3D Scene module                       *
// *************************************************************************

#include "3dscene.h"

#include "3dimage.h"
#include "display.h"
#include "mainwnd.h"
#include "mappane.h"
#include "resource.h"

// 3D Render State Flags
bool FlatShade = false;         // Use flat shading
bool SimpleLight = false;       // Use simple lights (not used)
bool UseTextures = true;        // Turn textures on/off
bool DitherEnable = true;       // Enable color dithering
bool BlendEnable = true;        // Enable texture blending
bool SpecularEnable = true;     // Enable specular highlights
bool ZEnable = true;            // Enable Zbuffer drawing
bool BilinearFilter = false;    // Bilinear filtering on/off
bool NoUpdateRects = false;     // Never update 2D bounding rects for objects
bool MaxLights = 1;             // Maximum number of lights that can affect a 3D obj
bool UseDirLight = true;        // Illuminates objects with a directional light
int32_t DirLightPercent = 85;   // Use this percentage directional/ambient light
bool Double3D, Triple3D;        // Doubles or tripples 3D objects

// 3D Lighting adjustment values
int32_t Ambient3D = 100;            // Adjust this below 100 for darker ambient, or above for lighter
int32_t LightRange3D = 100;         // Adjust this below 100 to decrease 3D light range, or above to increase

// quick hacks
// rotate x coords
float RotateX(float h, float k, float x, float y, float angle)
{ return (float)(h + (x - h) * cosf(angle) - (y - k) * sinf(angle)); }
// rotate y coords
float RotateY(float h, float k, float x, float y, float angle)
{ return (float)(k + (y - k) * cosf(angle) + (x - h) * sinf(angle)); }
// add z rotation here

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int32_t dist, int32_t intensity, int32_t multiplier);

// Increase 3D light affect radius by this value
#define LIGHT3DEXTRADIST ((float)LightRange3D / 100.0f)

// ************************************************************
// * T3DLight - Information for an light in a 3D scene object *
// ************************************************************

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

// ***************************************
// * T3DScene - 3DScene global variables *
// ***************************************

static SColor Ambient;                      // Ambient Light Value
static S3DPoint CameraPos;                  // Camera source pos

static int32_t scenex, sceney, scenewidth, sceneheight; // Scene screen position

static int32_t dirlight;

#define MAX3DANIMATORS 128
typedef TPointerArray<T3DAnimator, 32, 32> T3DAnimatorArray;
static T3DAnimatorArray AnimatorArray;

#define MAX3DLIGHTS 64
typedef TPointerArray<T3DLight, 32, 32> T3DLightArray;
static T3DLightArray LightArray;

static hmm_mat4 proj = {
    1.0/65536.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0/65536.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f/65536.0f/ZSCALE, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

static hmm_mat4 view = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 200.0f, 1.0f
};

static hmm_mat4 world;

hmm_mat4 identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

D3DMATRIXHANDLE hProj, hView, hWorld;

static LPDIRECT3DLIGHT lpD3DLight;
static D3DEXECUTEDATA d3dExData;
static D3DEXECUTEBUFFERDESC debDesc;
static LPDIRECT3DMATERIAL lpBmat;
static int32_t AmbientValue;
static SColor AmbientColor256;
static D3DCOLOR AmbientColor;

// Variables used by texture format info thingy
#define MAXTEXFORMATS 64
static int32_t numtexformats;
static DDSURFACEDESC texformats[MAXTEXFORMATS];

// **********************
// * T3DLight Functions *
// **********************

T3DLight::T3DLight(D3DLIGHT2 &newlight, int32_t newintensity, int32_t newmultiplier)
{
    memcpy(&lightdata, &newlight, sizeof(D3DLIGHT2));
    intensity = newintensity;
    multiplier = newmultiplier;
    
    Initialize();
}

T3DLight::~T3DLight()
{
    Close();
}

bool T3DLight::Initialize()
{
    TRY_D3D(Direct3D2->CreateLight(&light, nullptr));
    TRY_D3D(light->SetLight((LPD3DLIGHT)&lightdata));
    lighton = false;

    float lm = (float)1.0 / max(lightdata.dcvColor.r, 
        max(lightdata.dcvColor.g, lightdata.dcvColor.b));
    lightdata.dcvColor.r = lightdata.dcvColor.r * lm;
    lightdata.dcvColor.g = lightdata.dcvColor.g * lm;
    lightdata.dcvColor.b = lightdata.dcvColor.b * lm;

    return true;
}

bool T3DLight::Close()
{
    LightOff();
    RELEASE(light);

    return true;
}       

void T3DLight::SetIntensity(int32_t newintensity)
{
    intensity = newintensity;
}

void T3DLight::SetMultiplier(int32_t newmultiplier)
{
    multiplier = newmultiplier;
}

void T3DLight::SetColor(float red, float green, float blue)
{
    lightdata.dcvColor.r = red;
    lightdata.dcvColor.g = green;
    lightdata.dcvColor.b = blue;
    lightdata.dcvColor.a = 1.0f;

    light->SetLight((LPD3DLIGHT)&lightdata);
}

void T3DLight::SetNormalizedColor(float red, float green, float blue)
{
    lightdata.dcvColor.r = red;
    lightdata.dcvColor.g = green;
    lightdata.dcvColor.b = blue;
    lightdata.dcvColor.a = 1.0f;

    float lm = (float)1.0 / max(lightdata.dcvColor.r, 
        max(lightdata.dcvColor.g, lightdata.dcvColor.b));
    lightdata.dcvColor.r = lightdata.dcvColor.r * lm;
    lightdata.dcvColor.g = lightdata.dcvColor.g * lm;
    lightdata.dcvColor.b = lightdata.dcvColor.b * lm;

    light->SetLight((LPD3DLIGHT)&lightdata);
}

void T3DLight::SetPosition(float x, float y, float z)
{
    lightdata.dvPosition.x = x;
    lightdata.dvPosition.y = y;
    lightdata.dvPosition.z = z;

    light->SetLight((LPD3DLIGHT)&lightdata);
}

float T3DLight::DistanceToObject(float x, float y, float z)
{
    x -= lightdata.dvPosition.x;
    y -= lightdata.dvPosition.y;
    z -= lightdata.dvPosition.z;

    float d = (float)sqrt((double)(x * x + y * y));
    return (float)sqrt((double)(d * d + z * z));
}

float T3DLight::GetBrightness(float x, float y, float z)
{
    x -= lightdata.dvPosition.x;
    y -= lightdata.dvPosition.y;
    z -= lightdata.dvPosition.z;

    float d = (float)sqrt((double)(x * x + y * y));
    d = (float)sqrt((double)(d * d + z * z)) / LIGHT3DEXTRADIST;

    if (d < intensity)
        return min((float)GetLightBrightness((int32_t)d, intensity, multiplier), (float)1.0);
    else
        return 0.0f;
}

void T3DLight::GetColor(float x, float y, float z, float &r, float &g, float &b)
{
    float i = GetBrightness(x, y, z);

    r = lightdata.dcvColor.r * i;
    g = lightdata.dcvColor.g * i;
    b = lightdata.dcvColor.b * i;
}

bool T3DLight::AffectObject(float x, float y, float z)
{
    float lightr, lightg, lightb, r, g, b;
    GetColor(x, y, z, lightr, lightg, lightb);

    if (lightr > 0.0f || lightg > 0.0f || lightb > 0.0f)
    {
        r = lightdata.dcvColor.r;
        lightdata.dcvColor.r = lightr;
        g = lightdata.dcvColor.g;
        lightdata.dcvColor.g = lightg;
        b = lightdata.dcvColor.b;
        lightdata.dcvColor.b = lightb;
        lightdata.dcvColor.a = 1.0f;

        light->SetLight((LPD3DLIGHT)&lightdata);

        lightdata.dcvColor.r = r;
        lightdata.dcvColor.g = g;
        lightdata.dcvColor.b = b;
        
        return LightOn();
    }
    return false;
}

bool T3DLight::LightOn()
{
    if (!lighton)
    {
        //>>>>>>>>>>>>>>>>>>>>>
        if (UseBlue) 
            Scene3DSw.TurnOnLight(&lightdata);
        else
        //<<<<<<<<<<<<<<<<<<<<<
            TRY_D3D(Viewport->AddLight(light));
        lighton = true;
    }

    return true;
}

bool T3DLight::LightOff()
{
    if (!lighton)
        return true;

    //>>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue) 
        Scene3DSw.TurnOffLight(&lightdata);
    else
    //<<<<<<<<<<<<<<<<<<<<<<<<
        TRY_D3D(Viewport->DeleteLight(light));
    
    lighton = false;

    return true;
}

// *********************************
// * 3DScene - Main 3D interface!! *
// *********************************

// Constructor. Marks all frames and light free
T3DScene::T3DScene()
{
    initialized = false;
}

T3DScene::~T3DScene()
{
    Close();
}

// Initialize 3D scene
bool T3DScene::Initialize()
{
    if (initialized || Ignore3D)
        return true;

    initialized = true;

    // 3D units per pixel is equal to the total screen pixels for the hypotenuse of a tile
    // from the left screen corner to the right screen corner (512.0) divided by the actual
    // 3D width of the hypotenuse in 3D units.
    float unitsperpixel = (float)512.0 / (float)sqrt(256.0 * 256.0 + 256.0 * 256.0);

    // Create the scene (parent) frame
    if (UseDirect3D2)
    {
        TRY_D3D(Direct3D2->CreateViewport(&Viewport2, nullptr));
        TRY_D3D(Viewport2->QueryInterface(IID_IDirect3DViewport, (LPVOID *)&Viewport));
        TRY_D3D(Device2->AddViewport(Viewport2));
        TRY_D3D(Device2->SetCurrentViewport(Viewport2));
    }
    else
    {
        TRY_D3D(Direct3D->CreateViewport(&Viewport, nullptr));
        TRY_D3D(Device->AddViewport(Viewport));
    }

    SetSize(0, 0, Display->Width(), Display->Height());

    TExecuteBuf::Initialize();  // Initializes execute buffer stuff

    CreateMatrixList();         // Creates the buffer system matrix list

    D3DMATERIAL bmat;
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.diffuse.r = (float)0.0;
    bmat.diffuse.g = (float)0.0;
    bmat.diffuse.b = (float)0.0;
    bmat.ambient.r = (float)0.0;
    bmat.ambient.g = (float)0.0;
    bmat.ambient.b = (float)0.0;
    bmat.hTexture = nullptr;
    bmat.dwRampSize = 1;

    D3DMATERIALHANDLE hBmat;
    TRY_D3D(Direct3D->CreateMaterial(&lpBmat, nullptr));
    TRY_D3D(lpBmat->SetMaterial(&bmat));
    TRY_D3D(lpBmat->GetHandle(Device, &hBmat));
    TRY_D3D(Viewport->SetBackground(hBmat));

    if (Display->UsingClearZBuffer())
    {
        TRY_D3D(Viewport->SetBackgroundDepth(Display->GetZBuffer()->GetDDSurface()));
    }

    /*
     * Setup view, projection, world, and pos matrices
     */
    InitializeMatrices();

    /*
     * Setup ambient directional light
     */

    D3DLIGHT2 lightdata;
    memset(&lightdata, 0, sizeof(D3DLIGHT2));
    lightdata.dwSize = sizeof(D3DLIGHT2);
    lightdata.dltType = D3DLIGHT_DIRECTIONAL;
    lightdata.dcvColor.r = 1.0f;
    lightdata.dcvColor.g = 1.0f;
    lightdata.dcvColor.b = 1.0f;
    lightdata.dcvColor.a = 1.0f;
    lightdata.dvPosition.x = 0.0f;
    lightdata.dvPosition.y = 0.0f;
    lightdata.dvPosition.z = 200.0f;
    lightdata.dvDirection.x = 0.0f;
    //>>>>>>>>>>>>>>>>>>
    lightdata.dvDirection.y = -0.78125f;
    lightdata.dvDirection.z = -0.625f;
    //<<<<<<<<<<<<<<<<<<
//  lightdata.dvDirection.y = -1.0f;
//  lightdata.dvDirection.z = -0.8f;
    lightdata.dvRange = 1000000.0f;
    lightdata.dvFalloff = 0.0f;
    lightdata.dvAttenuation0 = 1.0f;
    lightdata.dvAttenuation1 = 0.0f;
    lightdata.dvAttenuation2 = 0.0f;
    lightdata.dvTheta = 0.0f;
    lightdata.dvPhi = 0.0f;
    lightdata.dwFlags = D3DLIGHT_ACTIVE;

    PT3DLight light = new T3DLight(lightdata, 1, 1);
    dirlight = LightArray.Add(light);

    /*
     * Get the texture formats
     */

    GetTextureFormats();

    return true;
}

bool T3DScene::SetSize(int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (!initialized || Ignore3D)
        return true;

    scenex      = x;
    sceney      = y;
    scenewidth  = width;
    sceneheight = height;

    // 3D units per pixel is equal to the total screen pixels for the hypotenuse of a tile
    // from the left screen corner to the right screen corner (512.0) divided by the actual
    // 3D width of the hypotenuse in 3D units.
    float unitsperpixel = (float)512.0 / (float)sqrt(256.0 * 256.0 + 256.0 * 256.0);

    D3DVIEWPORT viewData;
    memset(&viewData, 0, sizeof(D3DVIEWPORT));
    viewData.dwSize = sizeof(D3DVIEWPORT);
    viewData.dwX = x;
    viewData.dwY = y;
    viewData.dwWidth = width;
    viewData.dwHeight = height;
    viewData.dvScaleX = 65536.0 * unitsperpixelf;
    viewData.dvScaleY = 65536.0 * unitsperpixelf;
    viewData.dvMaxX = 1.0f;
    viewData.dvMaxY = 1.0f;

    TRY_D3D(Viewport->SetViewport(&viewData));

    return true;
}

bool T3DScene::InitializeMatrices()
{
    /*
     * Set the view, world and projection matrices
     * Create a buffer for matrix set commands etc.
     */
    MAKE_MATRIX(Device, hView, identity);
    MAKE_MATRIX(Device, hProj, proj);
    MAKE_MATRIX(Device, hWorld, identity);

    TExecuteBuf exbuf;

    exbuf.BeginRecord();
    exbuf.SetTransform(D3DTRANSFORMSTATE_VIEW, hView);
    exbuf.SetTransform(D3DTRANSFORMSTATE_PROJECTION, hProj);
    exbuf.SetTransform(D3DTRANSFORMSTATE_WORLD, hWorld);
    exbuf.SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(64, 64, 64, 64));
    exbuf.EndRecord();
    BeginScene();
    exbuf.Render();
    EndScene();

    return true;
}

bool T3DScene::Close()
{
    if (!initialized)
        return false;

  // Release execute buffer buffers and matrices
    TExecuteBuf::Close();

    AnimatorArray.Clear();
    LightArray.DeleteAll();

    RELEASE(lpBmat);

    TRY_D3D(Device->DeleteViewport(Viewport));
    RELEASE(Viewport);
    Viewport = nullptr;
    Viewport2 = nullptr;

    initialized = false;

    return true;
}

// Clears a rectangle in the screen zbuffer... uses either DrawRestoreRect, or
// Viewport->Clear() depending on whether the display has a secondary background zbuffer
// set for it or not.  
void T3DScene::RestoreZBuffer(SRect &r)
{
    if (NoScrollZBuffer)
            MapPane.DrawRestoreRect(r.x(), r.y(), r.w(), r.h(),
                DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);

    if (Display->UsingClearZBuffer())
    {
        SRect sr, pr;
        sr = r;
        MapPane.PaneToScreen(sr);
        MapPane.GetRect(pr);
        if (!ClipRect(sr, pr, sr))
            return;
        Display->GetRealZBuffer()->Blit(sr.x(), sr.y(), 
            Display->GetZBuffer(), sr.x(), sr.y(), sr.w(), sr.h());
    }
}

void T3DScene::SetCameraPos(hmm_vec3 pos, int32_t zdist)
{
    if (!initialized)
        return;

    CameraPos = pos;

    hmm_vec3 v1;//, v2;

    
    hmm_mat4 view;
    D3DMATRIXClear(&view);

    v1.x = CameraPos.xf;
    v1.y = CameraPos.yf;
    v1.z = CameraPos.zf;
    D3DMATRIXMove(&view, &v1); 

    D3DMATRIXRotateZ(&view, 45.0 * TORADIANf);
    D3DMATRIXRotateX(&view, -(90.0 + CAMERAANGLEf * TORADIAN));

    v1.x = 0.0f;
    v1.y = 0.0f;
    v1.z = -zdistf;
    D3DMATRIXMove(&view, &v1); 

    Device->SetMatrix(hView, &view);
}

void T3DScene::RefreshZBuffer()
{
  // Draw ZBuffer behind objects
    for (int32_t c = 0; c < AnimatorArray.NumItems(); c++)
        if (AnimatorArray[c] != nullptr)
            AnimatorArray[c]->RefreshZBuffer();
}

bool T3DScene::DrawScene()
{
    int32_t c;

    if (!initialized )
        return false;

    if (!Show3D)
        return true;

    static bool oldFlatShade, oldDitherEnable, oldBlendEnable,
        oldSpecularEnable, oldZEnable, oldBilinearFilter, oldUseDirLight;
    static D3DCOLOR oldAmbientColor;
    static bool b;
    static int32_t oldDirLightPercent;
    HRESULT DirectDrawReturn;

    if (FlatShade != oldFlatShade ||                // Render state has changed?
        DitherEnable != oldDitherEnable ||
        BlendEnable != oldBlendEnable ||
        SpecularEnable != oldSpecularEnable ||
        ZEnable != oldZEnable ||
        BilinearFilter != oldBilinearFilter ||
        oldAmbientColor != AmbientColor ||
        oldUseDirLight != UseDirLight ||
        oldDirLightPercent != DirLightPercent)
    {
        oldFlatShade = FlatShade;
        oldDitherEnable = DitherEnable;
        oldBlendEnable = BlendEnable;
        oldSpecularEnable = SpecularEnable;
        oldZEnable = ZEnable;
        oldBilinearFilter = BilinearFilter;
        oldAmbientColor = AmbientColor;
        oldUseDirLight = UseDirLight;
        oldDirLightPercent = DirLightPercent;

      // Reset scene params
        BeginScene();
        BeginRender();

        TRY_D3D(SetRenderState(D3DRENDERSTATE_ZENABLE, ZEnable));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_SHADEMODE, (FlatShade ? D3DSHADE_FLAT : D3DSHADE_GOURAUD)));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_DITHERENABLE, DitherEnable));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_BLENDENABLE, BlendEnable));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_SPECULARENABLE, SpecularEnable));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (BilinearFilter ? D3DFILTER_LINEAR : D3DFILTER_NEAREST)));
        TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (BilinearFilter ? D3DFILTER_LINEAR : D3DFILTER_NEAREST)));

        if (UseDirLight) // Use a combination of the directional light and ambient
        {                // This gives us better highlights, but it is slower
          // Set color of ambient
            int32_t ared = min((int32_t)RGBA_GETRED(AmbientColor) * (100 - DirLightPercent) / 100, 255);
            int32_t agreen = min((int32_t)RGBA_GETGREEN(AmbientColor) * (100 - DirLightPercent) / 100, 255);
            int32_t ablue = min((int32_t)RGBA_GETBLUE(AmbientColor) * (100 - DirLightPercent) / 100, 255);

            //>>>>>>>>>>>>>>>>>>>
            if (UseBlue) 
                Scene3DSw.SetAmbientLightColor( RGBA_MAKE(ared, agreen, ablue, 0));
            else
            //<<<<<<<<<<<<<<<<<<<
                TRY_D3D(SetLightState(D3DLIGHTSTATE_AMBIENT, 
                    RGBA_MAKE(ared, agreen, ablue, 0)));

          // Set color of directional light
            float extra = 1.5f; // Make dir light a little brighter
            float lightpcnt = (float)DirLightPercent / 100.0f * extra;
            LightArray[dirlight]->SetColor(
                min(RGBA_GETRED(AmbientColorf) / 256.0f * lightpcnt, 1.0f),
                min(RGBA_GETGREEN(AmbientColorf) / 256.0f * lightpcnt, 1.0f),
                min(RGBA_GETBLUE(AmbientColorf) / 256.0f * lightpcnt, 1.0f));
        }
        else            // Just use the ambient light for ambient
        {
            //>>>>>>>>>>>>>>>>>>>
            if (UseBlue) 
                Scene3DSw.SetAmbientLightColor( AmbientColor );
            else
            //<<<<<<<<<<<<<<<<<<<
                TRY_D3D(SetLightState(D3DLIGHTSTATE_AMBIENT, AmbientColor));
        }

        EndRender();
        EndScene();
    }

    // Verify both surfaces
    if (!front) 
        return false;

    if (!zbuffer) 
        return false;

    if (b) 
    {
        b = false;
    }

    // Restore the primary surface if it has been lost
    if (front->IsLost())
    {
        DirectDrawReturn = front->Restore();
        
        if (DirectDrawReturn != DD_OK) 
            return false;

        b = true;
    }
    
    // Restore the ZBuffer if it has been lost
    if (zbuffer->IsLost())
    {
        DirectDrawReturn = zbuffer->Restore();

        if (DirectDrawReturn != DD_OK) 
            return false;

        b = true;
    }

  // Draw objects
    BeginScene();

    for (c = 0; c < AnimatorArray.NumItems(); c++)
    {
        if (AnimatorArray[c] != nullptr) 
        {
            PTObjectInstance inst = AnimatorArray[c]->GetObjInst();
            PTObjectImagery imagery = AnimatorArray[c]->GetImagery();

            if ((int32_t)inst->GetFrame() == -1)
            {
                char buf[512];
                sprintf(buf, "Zero frame on state %d in object \'%s\'", inst->GetState(), inst->GetName());
                FatalError(buf);
            }

            if (inst->GetFlags() & OF_INVISIBLE ||
              (uint32_t)inst->GetState() >= (uint32_t)imagery->GetHeader()->numstates ||
              (uint32_t)inst->GetFrame() >= (uint32_t)imagery->GetHeader()->states[inst->GetState()].frames)
                continue;

            TRY_D3D(SetRenderState(D3DRENDERSTATE_BLENDENABLE, BlendEnable));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, true));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_ZENABLE, true));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW));
            TRY_D3D(SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID));

            AnimatorArray[c]->PreRender();      // Start rendering
            AnimatorArray[c]->Render();         // Object Render
            AnimatorArray[c]->PostRender();     // End rendering
        }
    }

    EndScene();

    return true;
}

void T3DScene::SetAmbientLight(int32_t ambient)  // What the heck 
{
    if (!initialized)
        return;

    AmbientValue = min((ambient * 8) * Ambient3D / 100, 255);

    int32_t red = AmbientColor256.red * AmbientValue / 256;
    int32_t green = AmbientColor256.green * AmbientValue / 256;
    int32_t blue = AmbientColor256.blue * AmbientValue / 256;
    
    AmbientColor = RGBA_MAKE(red, green, blue, 255);
}

void T3DScene::SetAmbientColor(SColor &color)
{
    if (!initialized)
        return;

    AmbientColor256 = color;

    int32_t maxcolor = max(0,max(color.red, max(color.green, color.blue)));
    AmbientColor256.red = (uint8_t)min((int32_t)AmbientColor256.red * 256 / maxcolor, 255);
    AmbientColor256.green = (uint8_t)min((int32_t)AmbientColor256.green * 256 / maxcolor, 255);
    AmbientColor256.blue = (uint8_t)min((int32_t)AmbientColor256.blue * 256 / maxcolor, 255);

    int32_t red = AmbientColor256.red * AmbientValue / 256;
    int32_t green = AmbientColor256.green * AmbientValue / 256;
    int32_t blue = AmbientColor256.blue * AmbientValue / 256;
    
    AmbientColor = RGBA_MAKE(red, green, blue, 255);
}

// ************ Light Functions **************

int32_t T3DScene::AddLight(RS3DPoint pos, SColor color, int32_t intensity, int32_t multiplier)
{
    if (!initialized)
        return -1;

    /*
     * Setup lights
     */
    D3DLIGHT2 lightdata;
    memset(&lightdata, 0, sizeof(D3DLIGHT2));
    lightdata.dwSize = sizeof(D3DLIGHT2);
    lightdata.dltType = D3DLIGHT_POINT;
    lightdata.dcvColor.r = color.redf / 256.0f;
    lightdata.dcvColor.g = color.greenf / 256.0f;
    lightdata.dcvColor.b = color.bluef / 256.0f;
    lightdata.dcvColor.a = 1.0f;
    lightdata.dvPosition.x = pos.xf;
    lightdata.dvPosition.y = pos.yf;
    lightdata.dvPosition.z = pos.zf;
    lightdata.dvDirection.x = 0.0f;
    lightdata.dvDirection.y = 0.0f;
    lightdata.dvDirection.z = 0.0f;
    lightdata.dvRange = 1000000.0f;
    lightdata.dvFalloff = 0.0f;
    lightdata.dvAttenuation0 = 1.0f;
    lightdata.dvAttenuation1 = 0.0f;
    lightdata.dvAttenuation2 = 0.0f;
    lightdata.dvTheta = 0.0f;
    lightdata.dvPhi = 0.0f;
    lightdata.dwFlags = D3DLIGHT_ACTIVE;

    return LightArray.Add(new T3DLight(lightdata, intensity, multiplier));
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

    // Change light in the frame
    LightArray[lightid]->SetNormalizedColor(
            color.redf / 256.0f,
            color.greenf / 256.0f,
            color.bluef / 256.0f);

    return true;
}

bool T3DScene::SetLightPosition(int32_t lightid, RS3DPoint pos)
{
    if (!initialized)
        return false;

    if (!initialized || lightid < 0 || 
      lightid >= LightArray.NumItems() ||
      LightArray[lightid] == nullptr)
        return false;

    // Change light in the frame
    LightArray[lightid]->SetPosition(
            pos.xf,
            pos.yf,
            pos.zf);

    return true;
}

int32_t T3DScene::GetNumLights()
{
    if (!initialized)
        return 0;

    return LightArray.NumItems();
}

  // The following functions are called by the character system to do lighting

bool T3DScene::GetClosestLights(int32_t x, int32_t y, int32_t z, int32_t &minlight1, int32_t &minlight2, int32_t &minlight3)
{
    if (!initialized)
        return false;

    float mindist = (float)10000.0;
    minlight1 = -1;
    float mindist2 = (float)10000.0;
    minlight2 = -1;
    float mindist3 = (float)10000.0;
    minlight3 = -1;
    for (int32_t c = 0; c < LightArray.NumItems(); c++)
    {
        if (LightArray[c] == nullptr)
            continue;

        float dist = LightArray[c]->DistanceToObject((float)x, (float)y, (float)z) / LIGHT3DEXTRADIST;
        if (dist < mindist)
        {
            mindist3 = mindist2;
            minlight3 = minlight2;
            mindist2 = mindist;
            minlight2 = minlight1;
            mindist = dist;
            minlight1 = c;
        }
        else if (dist < mindist2)
        {
            mindist3 = mindist2;
            minlight3 = minlight2;
            mindist2 = dist;
            minlight2 = c;
        }
        else if (dist < mindist3)
        {
            mindist3 = dist;
            minlight3 = c;
        }
    }

    return minlight1 >= 0;
}

bool T3DScene::LightAffectObject(int32_t x, int32_t y, int32_t z)
{
    if (!initialized)
        return false;

  // If we are using the directional ambient light, turn it on here...
    if (UseDirLight)
        LightArray[dirlight]->LightOn();

    int32_t minlight, minlight2, minlight3;
    GetClosestLights(x, y, z, minlight, minlight2, minlight3);

    if (minlight >= 0)
        if (!LightArray[minlight]->AffectObject((float)x, (float)y, (float)z))
            return false;
    if (MaxLights > 1 && minlight2 >= 0)
        if (!LightArray[minlight2]->AffectObject((float)x, (float)y, (float)z))
            return false;
    if (MaxLights > 2 && minlight3 >= 0)
        if (!LightArray[minlight3]->AffectObject((float)x, (float)y, (float)z))
            return false;

    return false;
}

// Gets the light brightness value for the given light in (0-255) range.
void T3DScene::GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, int32_t &brightness)
{
    brightness = 0;

    if (!initialized || lightid < 0 || 
      lightid >= LightArray.NumItems() ||
      LightArray[lightid] == nullptr)
        return;

    float i = LightArray[lightid]->GetBrightness((float)x, (float)y, (float)z);

    brightness = max(0, min(255, (int32_t)(i * 255.0f))); 
}

// Gets the light brightness value for the given light in 0.0-1.0.
void T3DScene::GetLightBrightness(int32_t lightid, int32_t x, int32_t y, int32_t z, float &brightness)
{
    brightness = 0;

    if (!initialized || lightid < 0 || 
      lightid >= LightArray.NumItems() ||
      LightArray[lightid] == nullptr)
        return;

    brightness = LightArray[lightid]->GetBrightness((float)x, (float)y, (float)z);
}

// Gets the light color value for the given light in (0-255) range.
void T3DScene::GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z, int32_t &r, int32_t &g, int32_t &b)
{
    r = g = b = 0;  

    if (!initialized || lightid < 0 || 
      lightid >= LightArray.NumItems() ||
      LightArray[lightid] == nullptr)
        return;

    float lightr, lightg, lightb;
    LightArray[lightid]->GetColor((float)x, (float)y, (float)z, lightr, lightg, lightb);

    r = max(0, min(255, (int32_t)(lightr * 255.0f))); 
    g = max(0, min(255, (int32_t)(lightg * 255.0f))); 
    b = max(0, min(255, (int32_t)(lightb * 255.0f))); 
}

// Gets the light color value for the given light in (0-255) range.
void T3DScene::GetLightColor(int32_t lightid, int32_t x, int32_t y, int32_t z, float &r, float &g, float &b)
{
    r = g = b = 0;  

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
    {
        if (LightArray[c] != nullptr)  // Stop on first light that affects char
            LightArray[c]->LightOff();
    }
}

// ************ Object Functions **************

int32_t T3DScene::AddAnimator(PT3DAnimator animator)
{
    if (!initialized)
        return -1;

    return AnimatorArray.Add(animator);
}
        
bool T3DScene::RemoveAnimator(uint32_t animid)
{
    if (!initialized || animid >= (uint32_t)AnimatorArray.NumItems() || AnimatorArray[animid] == nullptr)
        return false;

    AnimatorArray.Remove(animid);   
    
    return true;
}

int32_t T3DScene::GetNumAnimators()
{
    if (!initialized)
        return 0;

    return AnimatorArray.NumItems();
}

// ************************
// * Texture Format Stuff *
// ************************

HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSURFACEDESC lpDDSD, LPVOID lpContext)
{
    PT3DScene scene = (PT3DScene)lpContext;

    if (numtexformats < MAXTEXFORMATS)
    {
        memcpy(&texformats[numtexformats], lpDDSD, sizeof(DDSURFACEDESC));
        numtexformats++;
    }

    return DDENUMRET_OK;
}

void T3DScene::GetTextureFormats()
{
    numtexformats = 0;

    Device->EnumTextureFormats(EnumTextureFormatsCallback, (LPVOID)this);
}

inline int32_t countbits(uint32_t d)
{
    int32_t numbits = 0;
    for (int32_t c = 0; c < 32; c++)
    {
        if (d & (1 << c))
            numbits++;
    }
    return numbits;
}   

void T3DScene::GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd)
{
    if (!initialized)
        return;

    //>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue) {
        Scene3DSw.GetClosestTextureFormat( srcsd, dstsd );
        return;
    }
    //<<<<<<<<<<<<<<<<<<<<<<

    LPDDPIXELFORMAT spf = &srcsd->ddpfPixelFormat;

    int32_t redbits, greenbits, bluebits, alphabits; // Yum.. Alphabits is a tasty part of a
                                                 // balanced breakfast
    if (srcsd->ddpfPixelFormat.dwFlags & DDPF_RGB)
    {   
        redbits = countbits(srcsd->ddpfPixelFormat.dwRBitMask);
        greenbits = countbits(srcsd->ddpfPixelFormat.dwGBitMask);
        bluebits = countbits(srcsd->ddpfPixelFormat.dwBBitMask);
        alphabits = countbits(srcsd->ddpfPixelFormat.dwRGBAlphaBitMask);
    }
    else if (srcsd->ddpfPixelFormat.dwFlags & 
        (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
    {
        redbits = 5;    // Allows match system to find an RGB format if no matching
        greenbits = 6;  // palettized format can be found
        bluebits = 5;
        alphabits = 0;
    }   

    int32_t diff;
    int32_t closest = 0;
    int32_t closestdiff = 10000;
    LPDDSURFACEDESC sd = texformats;
    for (int32_t c = 0; c < numtexformats; c++, sd++)
    {
        LPDDPIXELFORMAT pf = &sd->ddpfPixelFormat;

      // If palettized format exactly matches, return immediately
        if ((pf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
          | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) != 0 &&
            (pf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
          | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) ==
            (spf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
          | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) &&
            (pf->dwRGBBitCount == spf->dwRGBBitCount))
        {
            closest = c;
            closestdiff = 0;
            break;
        }

        int32_t dredbits, dgreenbits, dbluebits, dalphabits;
        if (pf->dwFlags & DDPF_PALETTEINDEXED8)
        {
          // Rank palettized 8 bit above 434 RGB 8 bit, but below everything else
            dredbits = dgreenbits = dbluebits = 4; 
            dalphabits = 0;
        }
        else if (pf->dwFlags & DDPF_PALETTEINDEXED4)
        {
          // Rank palettized 4 bit above palettized 2 bit
            dredbits = dgreenbits = dbluebits = 2; 
            dalphabits = 0;
        }
        else if (pf->dwFlags & DDPF_PALETTEINDEXED2)
        {
          // Rank palettized 2 bit above palettized 1 bit
            dredbits = dgreenbits = dbluebits = 1; 
            dalphabits = 0;
        }
        else if (pf->dwFlags & DDPF_PALETTEINDEXED1)
        {
          // Rank palettized 1 bit below everything else
            dredbits = 1;
            dgreenbits = dbluebits = dalphabits = 0;
        }
        else if (pf->dwFlags & DDPF_RGB)
        {
            dredbits = countbits(pf->dwRBitMask);
            dgreenbits = countbits(pf->dwGBitMask);
            dbluebits = countbits(pf->dwBBitMask);
            dalphabits = countbits(pf->dwRGBAlphaBitMask);
        }
        else
            dredbits = dgreenbits = dbluebits = dalphabits = 10000; // Forget it

        diff = abs(redbits - dredbits) + 
               abs(greenbits - dgreenbits) +
               abs(bluebits - dbluebits) +
               abs(alphabits - dalphabits);

        if (diff < closestdiff)
        {   
            closest = c;
            closestdiff = diff;
        }
    }

    memcpy(dstsd, &texformats[closest], sizeof(DDSURFACEDESC));
}

inline int32_t getmaskshift(uint32_t mask)
{
    if (!mask)
        return 0;

    int32_t shift = 0;
    while (!(mask & 1) && shift < 32)
    {
        mask = mask >> 1;
        shift++;
    }
    return shift;
}

inline int32_t getmaskbits(uint32_t mask)
{
    if (!mask)
        return 0;

    int32_t bits = 0;
    for (int32_t c = 0; c < 32; c++)
    {
        if (mask & (1 << c))
            bits++;
    }
    return bits;
}

void T3DScene::ConvertTexture(
    LPDDSURFACEDESC srcsd, LPVOID srcpixels, LPPALETTEENTRY srcpal,  
    LPDDSURFACEDESC dstsd, LPVOID dstpixels, LPPALETTEENTRY dstpal)
{
    if (!initialized)
        return;

  // How many colors do our palettes have
    int32_t srcbufsize = (srcsd->ddpfPixelFormat.dwRGBBitCount * srcsd->lPitch * srcsd->dwHeight) >> 3;
    int32_t srcpalcolors = 0;
    if (srcsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
      DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
        srcpalcolors = 1 << srcsd->ddpfPixelFormat.dwRGBBitCount;

    int32_t dstbufsize = (dstsd->ddpfPixelFormat.dwRGBBitCount * dstsd->lPitch * dstsd->dwHeight) >> 3;
    int32_t dstpalcolors = 0;
    if (dstsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
      DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
        dstpalcolors = 1 << dstsd->ddpfPixelFormat.dwRGBBitCount;
    int32_t dstnumcolors = 0; // How many colors are in dst palette right now

  // Copy surface and return if it is already in the correct format (QUICK)
    if ( srcsd->dwWidth == dstsd->dwWidth && srcsd->dwHeight == dstsd->dwHeight && 
         srcsd->lPitch == dstsd->lPitch &&
         !memcmp(&srcsd->ddpfPixelFormat, &dstsd->ddpfPixelFormat, sizeof(DDPIXELFORMAT)) )
    {
        memcpy(dstpixels, srcpixels, dstbufsize);
        memcpy(dstpal, srcpal, sizeof(PALETTEENTRY) * dstpalcolors);
        return;
    }
    
  // Get shifts/etc. for source
    int32_t srcbits = srcsd->ddpfPixelFormat.dwRGBBitCount;
    uint32_t srcredshift, srcredmask, srcredbits,
          srcgreenshift, srcgreenmask, srcgreenbits,
          srcblueshift, srcbluemask, srcbluebits;
    uint32_t srcalphashift, srcalphamask, srcalphabits;
    bool srcisrgb = !(srcsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
      DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1));
    bool srchasalpha = srcsd->ddpfPixelFormat.dwRGBAlphaBitMask != 0;
    if (srcisrgb)
    {
        srcredmask = srcsd->ddpfPixelFormat.dwRBitMask;
        srcredshift = getmaskshift(srcredmask);
        srcredbits = 8 - getmaskbits(srcredmask);
        srcgreenmask = srcsd->ddpfPixelFormat.dwGBitMask;
        srcgreenshift = getmaskshift(srcgreenmask);
        srcgreenbits = 8 - getmaskbits(srcgreenmask);
        srcbluemask = srcsd->ddpfPixelFormat.dwBBitMask;
        srcblueshift = getmaskshift(srcbluemask);
        srcbluebits = 8 - getmaskbits(srcbluemask);
        if (srchasalpha)
        {
            srcalphamask = srcsd->ddpfPixelFormat.dwRGBAlphaBitMask;
            srcalphashift = getmaskshift(srcalphamask);
            srcalphabits = 8 - getmaskbits(srcalphamask);
        }
        else
            srcalphamask = srcalphashift = srcalphabits = 0;
    }

  // Get shifts/etc. for dest
    int32_t dstbits = dstsd->ddpfPixelFormat.dwRGBBitCount;
    uint32_t dstredshift, dstredmask, dstredbits,
          dstgreenshift, dstgreenmask, dstgreenbits,
          dstblueshift, dstbluemask, dstbluebits;
    uint32_t dstalphashift, dstalphamask, dstalphabits;
    bool dstisrgb = !(dstsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
      DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1));
    bool dsthasalpha = dstsd->ddpfPixelFormat.dwRGBAlphaBitMask != 0;
    if (dstisrgb)
    {
        dstredmask = dstsd->ddpfPixelFormat.dwRBitMask;
        dstredshift = getmaskshift(dstredmask);
        dstredbits = 8 - getmaskbits(dstredmask);
        dstgreenmask = dstsd->ddpfPixelFormat.dwGBitMask;
        dstgreenshift = getmaskshift(dstgreenmask);
        dstgreenbits = 8 - getmaskbits(dstgreenmask);
        dstbluemask = dstsd->ddpfPixelFormat.dwBBitMask;
        dstblueshift = getmaskshift(dstbluemask);
        dstbluebits = 8 - getmaskbits(dstbluemask);
        if (dsthasalpha)
        {
            dstalphamask = dstsd->ddpfPixelFormat.dwRGBAlphaBitMask;
            dstalphashift = getmaskshift(dstalphamask);
            dstalphabits = 8 - getmaskbits(dstalphamask);
        }
        else
        {
            dstalphamask = dstalphashift = 0;
            dstalphabits = 8; // Don't use alpha!
        }
    }

    uint8_t *sline = (uint8_t *)srcpixels;
    uint8_t *dline = (uint8_t *)dstpixels;

    for (int32_t l = 0; l < (int32_t)srcsd->dwHeight; l++,
         sline += (srcsd->lPitch * srcbits) >> 3,
         dline += (dstsd->lPitch * dstbits) >> 3)
    {
      int32_t srcbitpos = 0;
      int32_t dstbitpos = 0;
      for (int32_t c = 0; c < (int32_t)srcsd->dwWidth; c++, srcbitpos += srcbits, dstbitpos += dstbits)
      {
        uint8_t *s = sline + (srcbitpos >> 3); 
        uint8_t *d = dline + (dstbitpos >> 3); 
        uint32_t r, g, b, a;
        int32_t clrindex;

        clrindex = 0;

        if (srcisrgb)  // Do rgba extraction of any 8-32 bit pixel (including alpha/mask data)
        {
            uint32_t s32;
            if (srcbits == 32)
            {
                s32 = *(uint32_t *)s;
            }
            else if (srcbits == 24)
            {
                s32 = 0;
                *(((uint8_t *)&s32) + 0) = *(s + 0);
                *(((uint8_t *)&s32) + 1) = *(s + 1);
                *(((uint8_t *)&s32) + 2) = *(s + 2);
            }
            else if (srcbits == 16)
            {
                s32 = *(uint16_t *)s;
            }
            else if (srcbits <= 8)
            {   
                s32 = *s;
            }
            r = ((s32 >> srcredshift) << srcredbits) & 0xFF;
            g = ((s32 >> srcgreenshift) << srcgreenbits) & 0xFF;
            b = ((s32 >> srcblueshift) << srcbluebits) & 0xFF;
            if (srchasalpha)
                a = ((s32 >> srcalphashift) << srcalphabits) & 0xFF;
            else
                a = 255;
        }
        else  // This is a palettized image.. get rgba info if dest is rgba, otherwise get clrindex
        {
            clrindex = *(uint8_t *)s; // All indexed formats are less than 8 bits (duh)
            if (srcbits < 8)
                clrindex = clrindex >> (8 - ((srcbitpos & 7) + srcbits));
            if (dstisrgb)
            {
                r = srcpal[clrindex].peRed;
                g = srcpal[clrindex].peGreen;
                b = srcpal[clrindex].peBlue;
            }
            a = 255;
        }

        if (dstisrgb)   // Put rgba data back into dest pixel
        {
            uint32_t d32 =
                ((r >> dstredbits) << dstredshift) |
                ((g >> dstgreenbits) << dstgreenshift) |
                ((b >> dstbluebits) << dstblueshift) |
                ((a >> dstalphabits) << dstalphashift);
            if (dstbits == 32)
            {
                *(uint32_t *)d = d32;
            }
            else if (dstbits == 24)
            {
                *(d + 0) = *(((uint8_t *)&d32) + 0);
                *(d + 1) = *(((uint8_t *)&d32) + 1);
                *(d + 2) = *(((uint8_t *)&d32) + 2);
            }
            else if (dstbits == 16)
            {
                *(uint16_t *)d = (uint16_t)d32;
            }
            else if (dstbits == 8)
            {
                *(uint8_t *)d = (uint8_t)d32;
            }
            else if (dstbits < 8)
            {
                if (!(dstbitpos & 7))
                    *(uint8_t *)d = (uint8_t)d32;
                else 
                    *(uint8_t *)d |= (uint8_t)d32;
            }
        }
        else  // clrindex dest.. put clrindex into bits, build palette if necessary
        {
            if (srcisrgb)  // Build palette (note: doesn't do 16->8 or 24->8 color remapping!!)
            {
                PALETTEENTRY pe;
                pe.peRed = (uint8_t)r;
                pe.peGreen = (uint8_t)g;
                pe.peBlue = (uint8_t)b;
                pe.peFlags = 0;
                for (clrindex = 0; clrindex < dstnumcolors; clrindex++)
                {
                    if (*(uint32_t *)&pe == *(uint32_t *)&dstpal[clrindex])
                        break;
                }
                if (clrindex >= dstnumcolors && clrindex < dstpalcolors)
                {
                    dstpal[clrindex] = pe;
                    dstnumcolors++;
                }
            }

            if (dstbits < 8)
                clrindex = clrindex << (8 - ((dstbitpos & 7) + dstbits));
            
            if (!(dstbitpos & 7)) 
                *d = (uint8_t)clrindex;
            else
                *d |= (uint8_t)clrindex;
        }
      }
    }
}   

// ************************************
// * 3DScene Draw Primitive Functions *
// ************************************

// The functions below are a subset of the D3D Device interface, and are used to 
// map calls to that interface into calls to the execute buffer recorder object
// if UseDrawPrimitive is false.  The theory is that a good D3DEXECUTEBUFFER is
// substantially faster than the DrawPrimitive functions.

// Current execute buffer
static TExecuteBuf InternalExecuteBuf;  // Internal execute buffer used by 3DScene
static PTExecuteBuf ExecuteBuf;         // Pointer to current execute buffer (records only if points to Internal)
static bool InRecord, WasRendered;      // Did we render this buffer yet?
static D3DCLIPSTATUS ClipStatus;        // Current clip status
static bool WasFlushed;                 // Was this render reset by a call to GetClipStatus()

// Arrays to store current states...
static uint32_t currenderstate[63]; // These ranges should be good for the life of this program
static uint32_t curlightstate[16];
static hmm_mat4 curtransmatrix[4];

// Matrix lists
#define EXBUF_MAXMATRICES     128
static D3DMATRIXHANDLE matrixlist[EXBUF_MAXMATRICES];
static int32_t curmatrix;   // This is incremented on each call to SetTransform()

void T3DScene::CreateMatrixList()
{
    for (int32_t c = 0; c < EXBUF_MAXMATRICES; c++)
    {
        TRY_D3D(Device->CreateMatrix(&matrixlist[c]));
    }
    curmatrix = 0;
}

void T3DScene::ClearMatrixList()
{
    for (int32_t c = 0; c > EXBUF_MAXMATRICES; c++)
    {
        TRY_D3D(Device->DeleteMatrix(matrixlist[c]));
    }
    curmatrix = 0;
}

D3DMATRIXHANDLE T3DScene::GetMatrix(int32_t matrixnum)
{
    return matrixlist[matrixnum];
}

int32_t T3DScene::GetCurMatrix()
{
    return curmatrix;
}

HRESULT T3DScene::BeginRender()
{
    //>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.BeginRender();
    //<<<<<<<<<<<<<<<<<<<<<<<

    if (InRecord)
        return DD_OK;

    curmatrix = 0; // Set current matrix in matrix list to 0

    if (!UseDrawPrimitive && ExecuteBuf == &InternalExecuteBuf)
        ExecuteBuf->BeginRecord();  // Record if set to internal buf, otherwise just render

    WasRendered = false;
    WasFlushed = false;
    InRecord = true;

    currenderstate[D3DRENDERSTATE_TEXTUREHANDLE] = 0xFFFFFFFF;
    curlightstate[D3DLIGHTSTATE_MATERIAL] = 0;

    return DD_OK;
}

HRESULT T3DScene::EndRender()
{
    //>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.EndRender();
    //<<<<<<<<<<<<<<<<<<<<<<

    if (!InRecord)
        return DD_OK;

    if (!UseDrawPrimitive)
    {
        if (ExecuteBuf == &InternalExecuteBuf)
            ExecuteBuf->EndRecord();// End record if doing internal buf, otherwise just render

        if (WasRendered == false)   // Prevent object from being rendered in both EndMesh & EndScene
        {
            ExecuteBuf->Render(&ClipStatus);
            WasRendered = true;
        }
    }

    ExecuteBuf = &InternalExecuteBuf;
    InternalExecuteBuf.EndRecord(); // Make sure internal buffer isn't recording

    curmatrix = 0;  // Clear matrix list counter to 0
    
    InRecord = false;

    return DD_OK;
}

HRESULT T3DScene::BeginScene()
{
    HRESULT err;

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.BeginScene();
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        err = Device2->BeginScene();
    else
        err = Device->BeginScene();

    if (err != DD_OK)
        return err;

    ExecuteBuf = &InternalExecuteBuf;

    return DD_OK;
}

HRESULT T3DScene::EndScene()
{
    HRESULT err;

    err = EndRender();
    if (err != DD_OK)
        return err;
        
    //>>>>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.EndScene();
    //<<<<<<<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->EndScene();
    else
        return Device->EndScene();
}

HRESULT T3DScene::SetClipStatus(LPD3DCLIPSTATUS cs)
{
    //2>>>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.SetClipStatus(cs);
    //2<<<<<<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->SetClipStatus(cs);

    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->SetClipStatus(cs);
    else
        return DD_OK;
}

// Note: the GetClipStatus function forces the current execute buffer to be rendered, and a new
// one started!!!  If this is NOT what you want, don't call this function, or any derived function
// such as GetExtents() or UpdateExtents().  Also note that animations with complex extents
// calculations can NOT be buffered.

HRESULT T3DScene::GetClipStatus(LPD3DCLIPSTATUS cs)
{
    //2>>>>>>>>>>>>>>>>>>>>>>>>>>
    if (UseBlue) 
        return Scene3DSw.GetClipStatus(cs);
    //2<<<<<<<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->GetClipStatus(cs);

    bool isrecording = ExecuteBuf->IsRecording();

    if (isrecording)
        EndRender(); // Force whatever mesh was last recording to be drawn to screen

    memcpy(cs, &ClipStatus, sizeof(D3DCLIPSTATUS));

    if (isrecording)
    {
        BeginRender();
        WasFlushed = true;  // Flag that we had to flush the buffer in the middle of a render
    }

    return DD_OK;
}

HRESULT T3DScene::SetRenderState(D3DRENDERSTATETYPE rs, uint32_t data)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.SetRenderState(rs, data);
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->SetRenderState(rs, data);

    currenderstate[rs] = data;

    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->SetRenderState(rs, data);
    else 
        return DD_OK;
}                  

//>>>>>>>>>>>>>>>>>>>>>>
HRESULT T3DScene::SetTexture( uint32_t hTexture, LPDIRECTDRAWSURFACE surface )
{
    if (UseBlue) {
        Scene3DSw.SetTexture( hTexture, surface );
        return D3D_OK;
    } else
        return SetRenderState( D3DRENDERSTATE_TEXTUREHANDLE, hTexture );
}
//<<<<<<<<<<<<<<<<<<<<<<

HRESULT T3DScene::GetRenderState(D3DRENDERSTATETYPE rs, uint32_t *data)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.GetRenderState(rs, data);
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->GetRenderState(rs, data);

    *data = currenderstate[rs];
    return DD_OK;
}

HRESULT T3DScene::SetLightState(D3DLIGHTSTATETYPE ls, uint32_t data)
{
    if (UseDrawPrimitive)
        return Device2->SetLightState(ls, data);

    curlightstate[ls] = data;

    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->SetLightState(ls, data);
    else
        return DD_OK;
}

HRESULT T3DScene::GetLightState(D3DLIGHTSTATETYPE ls, uint32_t *data)
{
    if (UseDrawPrimitive)
        return Device2->GetLightState(ls, data);

    *data = curlightstate[ls];
    return DD_OK;
}

HRESULT T3DScene::SetTransform(D3DTRANSFORMSTATETYPE ts, hmm_mat4* matrix)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue) {
        return Scene3DSw.SetTransform(ts, matrix);
    }
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->SetTransform(ts, matrix);

    if (curmatrix >= EXBUF_MAXMATRICES)
        return D3DERR_MATRIX_SETDATA_FAILED;
    Device->SetMatrix(matrixlist[curmatrix], matrix);
    curmatrix++;

    memcpy(&curtransmatrix[ts], matrix, sizeof(hmm_mat4)); // Save matrix for later

    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->SetTransform(ts, matrixlist[curmatrix - 1]);
    else
        return DD_OK;
}

HRESULT T3DScene::GetTransform(D3DTRANSFORMSTATETYPE ts, hmm_mat4* matrix)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.GetTransform(ts, matrix);
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->GetTransform(ts, matrix);

    memcpy(matrix, &curtransmatrix[ts], sizeof(hmm_mat4));

    return DD_OK;
}

HRESULT T3DScene::DrawIndexedPrimitive(
    D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, uint32_t vc, LPWORD i, uint32_t ic, uint32_t f)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.DrawIndexedPrimitive(pt, vt, v, vc, i, ic, f);
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->DrawIndexedPrimitive(pt, vt, v, vc, i, ic, f);
    
    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->DrawIndexedPrimitive(pt, vt, v, vc, i, ic, f);
    else
        return DD_OK;
}

HRESULT T3DScene::DrawPrimitive(
    D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, uint32_t vc, uint32_t f)
{
    //>>>>>>>>>>>>>>>>>>>>
    if (UseBlue)
        return Scene3DSw.DrawPrimitive(pt, vt, v, vc, f);
    //<<<<<<<<<<<<<<<<<<<<

    if (UseDrawPrimitive)
        return Device2->DrawPrimitive(pt, vt, v, vc, f);

    if (ExecuteBuf == &InternalExecuteBuf)
        return ExecuteBuf->DrawPrimitive(pt, vt, v, vc, f);
    else
        return DD_OK;
}

// Returns true if we had to flush the current execute buffer in the middle of a Render() so
// we could get the extents.  This means we won't be able to cache this render.
bool T3DScene::WasRenderFlushed()
{
    return WasFlushed;
}

// Returns a pointer to the current execute buffer
PTExecuteBuf T3DScene::GetExecuteBuf()
{
    return ExecuteBuf;

}

// Gets a copy of the previously recorded execute buffer
PTExecuteBuf T3DScene::CopyExecuteBuf()
{
    return new TExecuteBuf(*ExecuteBuf);
}

// Ignores all calls to DrawPrimitive functions, and just renders the supplied buffer
// to render when EndMesh() or EndScene() is called.
void T3DScene::UseExecuteBuf(PTExecuteBuf usebuf)
{
    if (InRecord) // Can't be recording when we set the buffer
        return;

    ExecuteBuf = usebuf;    
}

// **************************************
// * TExecuteBuf - Execute buffer stuff *
// **************************************

// The TExecuteBuf object basically contains a pointer to an execute buffer.  When BeginRecord()
// is called, the object will begin recording DrawPrimitive calls to the execute buffer.
// When EndRecord() is called, the data is copied into the execute buffer, and the execute buffer
// pointer is set.  After this, Render() can be called as many times as the user wants without
// having to re-record the buffer.

#define EXBUF_MAXRENDERSTATES 32
#define EXBUF_MAXLIGHTSTATES  32
#define EXBUF_MAXTRANSSTATES  32
#define EXBUF_BUFGROWSIZE     8192  

struct STATEDATA
{
    uint32_t state;
    uint32_t data;
};

// General stuff
static PTExecuteBuf recording;  // Points to buffer we are currently recording

// Vertex values    
static uint32_t vertmode;
static int32_t vertsize;

// Vertex buffer
static int32_t vertbufsize;     // Current vertex buf size
static uint8_t *vertbuf, *vertptr; // Buffer
static LPVOID vb;           // Current vertex pointer
static int32_t numverts;        // Current number of vertices 

// Transform op buffer (For vertices)
static int32_t transbufsize;    // Current buf size
static uint8_t *transbuf, *transptr; // Buffer
static LPVOID tb;           // Current op pointer
static LPVOID lasttb;       // Pointer to previous op
static uint32_t tblastop;      // Id of previous op

// Face op buffer (For triangles)
static int32_t facebufsize; // Current buf size
static uint8_t *facebuf, *faceptr; // Buffer
static LPVOID fb;           // Current op pointer
static LPVOID lastfb;       // Pointer to previous op
static uint32_t fblastop;      // Id of previous op

// State lists
static int32_t numrenderstates;
static STATEDATA renderstates[EXBUF_MAXRENDERSTATES];
static int32_t numlightstates;
static STATEDATA lightstates[EXBUF_MAXLIGHTSTATES];
static int32_t numtransstates;
static STATEDATA transstates[EXBUF_MAXTRANSSTATES];

TExecuteBuf::TExecuteBuf()
{
    exBuf = nullptr;
    verttype = -1;
}

TExecuteBuf::TExecuteBuf(TExecuteBuf &eb) // Copy another execute buffer
{
    exBuf = nullptr;
    verttype = -1;

    if (eb.exBuf == nullptr)
        return;

  // Copy vert type
    verttype = eb.verttype;
    exBuf = eb.exBuf;
    memcpy(&debDesc, &eb.debDesc, sizeof(D3DEXECUTEBUFFERDESC));

  // Delete source
    eb.verttype = -1;
    eb.exBuf = nullptr;
    memset(&eb.debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
}

TExecuteBuf::~TExecuteBuf()
{
    if (recording == this)
        EndRecord();
    if (exBuf)
        exBuf->Release();
}

void TExecuteBuf::BeginRecord()
{
    if (recording && recording != this)
        FatalError("Can only record 1 execute buffer at a time");
    if (recording == this)
        return;

    if (exBuf)
        exBuf->Release();
    exBuf = nullptr;

  // Initialize vertex stuff
    verttype = -1;
    vertmode = 0;
    vertsize = 0;
    numverts = 0;
    CheckVertBufSize(128);  // Make sure we have some space to record to
    vb = vertbuf;           // Vertbuf pointer

  // Initialize buffers
    CheckTransBufSize(128); // Make sure we have some space to record to
    lasttb = tb = transbuf; // Set current pos
    tblastop = 0xFFFFFFFF;  // Set last op to crazy
    CheckFaceBufSize(128);  // Make sure we have some space to record to
    lastfb = fb = facebuf;  // Set current pos
    fblastop = 0xFFFFFFFF;  // Set last op to crazy

  // Initialize states
    numrenderstates = numlightstates = numtransstates = 0;

  // Initialize matrices
    curmatrix = 0;

    recording = this;
}

void TExecuteBuf::EndRecord()
{
    if (recording != this)  // Not recording.. no big deal..
        return;

  // Put last instructions into buffer
    FlushStates();  // Make sure all states have been flushed to buffers
    OP_EXIT(fb);    // Put exit instruction after faces

  // Kill old execute buf
    if (exBuf)
        exBuf->Release();
    exBuf = nullptr;

  // Get sizes
    int32_t vbufsize = (numverts * vertsize + 7) & 0xFFFFFFF8;
    int32_t bufsize = (char *)tb - (char *)transbuf + (char *)fb - (char *)facebuf;

  // Make new buf
    size_t size = vbufsize + bufsize + 8; // Allow extra space for alignment
    debDesc;
    memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    debDesc.dwFlags = D3DDEB_BUFSIZE;
    debDesc.dwBufferSize = size;
    TRY_D3D(Device->CreateExecuteBuffer(&debDesc, &exBuf, nullptr));

  // Lock it so it can be filled
    TRY_D3D(exBuf->Lock(&debDesc));

  // Grab verts 
    LPVOID lpBufStart = debDesc.lpData;
    memset(lpBufStart, 0, size);
    LPVOID lpPointer = lpBufStart;
    memcpy(lpPointer, vertbuf, numverts * vertsize);
    lpPointer = (char *)lpPointer + vbufsize;

  // Do transform and face buffers
    LPVOID lpInsStart = lpPointer;
    if (!QWORD_ALIGNED(lpPointer))      // Make sure quadword aligned
    {
        OP_NOP(lpPointer);
    }
    memcpy(lpPointer, transbuf, (char *)tb - (char *)transbuf); // Transform buffer
    lpPointer = (char *)lpPointer + (int32_t)((char *)tb - (char *)transbuf);
    if (!QWORD_ALIGNED(lpPointer))      // Make sure quadword aligned
    {
        OP_NOP(lpPointer);
    }
    memcpy(lpPointer, facebuf, (char *)fb - (char *)facebuf);
    lpPointer = (char *)lpPointer + (int32_t)((char *)fb - (char *)facebuf);

  // Setup the execute data describing the buffer
    exBuf->Unlock();
    D3DEXECUTEDATA d3dExData;
    memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
    d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
    d3dExData.dwVertexOffset = 0;
    d3dExData.dwVertexCount = numverts;
    d3dExData.dwInstructionOffset = (ULONG) ((char *)lpInsStart - (char *)lpBufStart);
    d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer  - (char *)lpInsStart);
    d3dExData.dwHVertexOffset = 0;
    exBuf->SetExecuteData(&d3dExData);

  // End recording
    recording = nullptr;
    curmatrix = 0;
}

bool TExecuteBuf::IsRecording()
{
    return (recording == this);
}

void TExecuteBuf::Render(LPD3DCLIPSTATUS cs)
{
    if (recording || !exBuf) // Can't render while we're recording (stupid! stupid!)
        return;

  // Execute the buffer
    Device->Execute(exBuf, Viewport, D3DEXECUTE_UNCLIPPED);

  // Get bounding rectangle
    if (cs)
    {
        D3DEXECUTEDATA d3dExData;
        d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
        TRY_D3D(exBuf->GetExecuteData(&d3dExData));
        cs->dwFlags = D3DCLIPSTATUS_EXTENTS2;
        cs->dwStatus = d3dExData.dsStatus.dwStatus;
        cs->minx = (float)d3dExData.dsStatus.drExtent.x1;
        cs->miny = (float)d3dExData.dsStatus.drExtent.y1;
        cs->minz = (float)0;
        cs->maxx = (float)d3dExData.dsStatus.drExtent.x2;
        cs->maxy = (float)d3dExData.dsStatus.drExtent.y2;
        cs->maxz = (float)65535;
    }
}

void TExecuteBuf::CheckVertBufSize(int32_t size)
{
    if ((int32_t)((uint32_t)vb - (uint32_t)vertbuf) + size < vertbufsize)
        return;

    uint8_t *ptr = new uint8_t[vertbufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
    uint8_t *buf = (uint8_t *)(((uint32_t)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
    if (vertbuf)
    {
        int32_t cursize = numverts * vertsize;
        memcpy(buf, vertbuf, cursize);
        delete vertptr;
    }
    vertptr = ptr;
    vertbuf = buf;
    vertbufsize += EXBUF_BUFGROWSIZE;
}

void TExecuteBuf::CheckTransBufSize(int32_t size)
{
    if ((int32_t)((uint32_t)tb - (uint32_t)transbuf) + size < transbufsize)
        return;

    uint8_t *ptr = new uint8_t[transbufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
    uint8_t *buf = (uint8_t *)(((uint32_t)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
    if (transbuf)
    {
        int32_t cursize = (char *)tb - (char *)transbuf;
        memcpy(buf, transbuf, cursize);
        delete transptr;
        tb = buf + cursize;
    }
    transptr = ptr;
    transbuf = buf;
    transbufsize += EXBUF_BUFGROWSIZE;
    if (!tb)
        tb = transbuf;
}

void TExecuteBuf::CheckFaceBufSize(int32_t size)
{
    if ((int32_t)((uint32_t)fb - (uint32_t)facebuf) + size < facebufsize)
        return;

    uint8_t *ptr = new uint8_t[facebufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
    uint8_t *buf = (uint8_t *)(((uint32_t)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
    if (facebuf)
    {
        int32_t cursize = (char *)fb - (char *)facebuf;
        memcpy(buf, facebuf, cursize);
        delete faceptr;
        fb = buf + cursize;
    }
    faceptr = ptr;
    facebuf = buf;
    facebufsize += EXBUF_BUFGROWSIZE;
    if (!fb)
        fb = facebuf;
}

void TExecuteBuf::Initialize()
{
  // Set initial values to 0 (just in case)
    vertbuf = vertptr = nullptr;
    numverts = vertsize = vertbufsize = 0;
    transbuf = transptr = nullptr;
    tb = nullptr;
    transbufsize = 0;
    facebuf = faceptr = nullptr;
    fb = nullptr;
    facebufsize = 0;
}

void TExecuteBuf::Close()
{
    if (vertbuf)
        delete vertptr;
    vertbuf = vertptr = nullptr;
    numverts = vertsize = vertbufsize = 0;
    if (transbuf)
        delete transptr;
    transbuf = transptr = nullptr;
    tb = nullptr;
    transbufsize = 0;
    if (facebuf)
        delete faceptr;
    facebuf = faceptr = nullptr;
    fb = nullptr;
    facebufsize = 0;
}

HRESULT TExecuteBuf::SetClipStatus(LPD3DCLIPSTATUS cs)
{
    if (recording != this)
        return D3DERR_NOTINBEGIN;

  // Flush out any pending state changes
    FlushStates();

  // Setup clipping status
    if (cs)
    {
        uint32_t flags = 0;
        if (cs->dwFlags == D3DCLIPSTATUS_STATUS && 
            (cs->dwFlags == D3DCLIPSTATUS_EXTENTS2 || cs->dwFlags == D3DCLIPSTATUS_EXTENTS3))
            flags = D3DSETSTATUS_ALL;
        else if (cs->dwFlags == D3DCLIPSTATUS_STATUS)
            flags = D3DSETSTATUS_STATUS;
        else if (cs->dwFlags == D3DCLIPSTATUS_EXTENTS2 || cs->dwFlags == D3DCLIPSTATUS_EXTENTS3)
            flags = D3DSETSTATUS_EXTENTS;

        lasttb = tb;
        tblastop = D3DOP_SETSTATUS;
        OP_SET_STATUS(flags, cs->dwStatus, (int32_t)cs->minx, (int32_t)cs->miny, (int32_t)cs->maxx, (int32_t)cs->maxy, tb);
    }

    return DD_OK;
}

HRESULT TExecuteBuf::SetRenderState(D3DRENDERSTATETYPE rs, uint32_t data)                    
{
    if (recording != this)
        return D3DERR_NOTINBEGIN;

    if (numrenderstates >= EXBUF_MAXRENDERSTATES)
        FlushStates();

    int32_t c;
    for (c = 0; c < numrenderstates; c++)  // Never set the same render state twice!
    {
        if ((D3DRENDERSTATETYPE)renderstates[c].state == rs)
            break;
    }

    renderstates[c].state = rs;
    renderstates[c].data = data;

    if (numrenderstates < c + 1)
        numrenderstates = c + 1;

    return DD_OK;
}

HRESULT TExecuteBuf::SetLightState(D3DLIGHTSTATETYPE ls, uint32_t data)
{
    if (recording != this)
        return D3DERR_NOTINBEGIN;

    if (numlightstates >= EXBUF_MAXLIGHTSTATES)
        FlushStates();

    int32_t c;
    for (c = 0; c < numlightstates; c++)  // Never set the same light state twice!
    {
        if ((D3DLIGHTSTATETYPE)lightstates[c].state == ls)
            break;
    }

    lightstates[c].state = ls;
    lightstates[c].data = data;

    if (numlightstates < c + 1)
        numlightstates = c + 1;

    return DD_OK;
}

HRESULT TExecuteBuf::SetTransform(D3DTRANSFORMSTATETYPE ts, D3DMATRIXHANDLE matrix)
{
    if (recording != this)
        return D3DERR_NOTINBEGIN;

    if (numtransstates >= EXBUF_MAXTRANSSTATES)
        FlushStates();

    int32_t c;
    for (c = 0; c < numtransstates; c++)  // Never set the same light state twice!
    {
        if ((D3DTRANSFORMSTATETYPE)transstates[c].state == ts)
            break;
    }

    transstates[c].state = ts;
    transstates[c].data = (uint32_t)matrix;

    if (numtransstates < c + 1)
        numtransstates = c + 1;

    return DD_OK;
}

void TExecuteBuf::FlushStates()
{
  // Spew out any changes in the light state.. append to previous light state if possible
    if (numlightstates > 0)
    {
        if (tblastop == D3DOP_STATELIGHT)
        {
            ((LPD3DINSTRUCTION)lasttb)->wCount += numlightstates;
        }
        else 
        {
            lasttb = tb;
            tblastop = D3DOP_STATELIGHT;
            OP_STATE_LIGHT(numlightstates, tb);
        }
        for (int32_t c = 0; c < numlightstates; c++)
        {
          STATE_DATA(lightstates[c].state, lightstates[c].data, tb);
        }
        numlightstates = 0;
    }

  // Spew out any changes in the transform state.. append to previous transform state if possible
    if (numtransstates > 0)
    {
        if (tblastop == D3DOP_STATETRANSFORM)
        {
            ((LPD3DINSTRUCTION)lasttb)->wCount += numtransstates;
        }
        else 
        {
            lasttb = tb;
            tblastop = D3DOP_STATETRANSFORM;
            OP_STATE_TRANSFORM(numtransstates, tb);
        }
        for (int32_t c = 0; c < numtransstates; c++)
        {
          STATE_DATA(transstates[c].state, transstates[c].data, tb);
        }
        numtransstates = 0;
    }

  // Spew out any changes in the light state.. append to previous light state if possible
    if (numrenderstates > 0)
    {
        if (fblastop == D3DOP_STATERENDER)
        {
            ((LPD3DINSTRUCTION)lastfb)->wCount += numrenderstates;
        }
        else 
        {
            lastfb = fb;
            fblastop = D3DOP_STATELIGHT;
            OP_STATE_LIGHT(numrenderstates, fb);
        }
        for (int32_t c = 0; c < numrenderstates; c++)
        {
          STATE_DATA(renderstates[c].state, renderstates[c].data, fb);
        }
        numrenderstates = 0;
    }
}

HRESULT TExecuteBuf::DrawIndexedPrimitive(
    D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, uint32_t vc, LPWORD i, uint32_t ic, uint32_t f)
{
    if (recording != this)
        return D3DERR_NOTINBEGIN;

    if (vc == 0)
        return DD_OK;

    int32_t c;

  // Set the vertex type for this execute buffer if we don't know it yet
    if (verttype != -1 && verttype != vt)
        FatalError("Unable to create execute buffer with more than one type of vertex");
    else if (verttype == -1)
    {
        verttype = vt;
        if (verttype == D3DVT_VERTEX)
        {
            vertmode = D3DPROCESSVERTICES_TRANSFORMLIGHT | D3DPROCESSVERTICES_UPDATEEXTENTS;
            vertsize = sizeof(hmm_vec3);
        }
        else if (verttype == D3DVT_LVERTEX)
        {
            vertmode = D3DPROCESSVERTICES_TRANSFORM | D3DPROCESSVERTICES_UPDATEEXTENTS;
            vertsize = sizeof(D3DLVERTEX);
        }
        else if (verttype == D3DVT_TLVERTEX)
        {
            vertmode = D3DPROCESSVERTICES_COPY | D3DPROCESSVERTICES_UPDATEEXTENTS;
            vertsize = sizeof(D3DTLVERTEX);
        }
    }

  // Flush all states
    FlushStates();
    
  // Get triangle indices if no index array is provided
    int32_t startvert = numverts;
    if (!i)
        ic = vc;
    int32_t numtri = ic / 3;

  // Spew the verts
    int32_t size = vertsize * vc;
    CheckVertBufSize(size + 128);
    memcpy(vb, v, size);
    numverts += vc;
    vb = (char *)vb + size;

  // Spew the process vert op
    CheckTransBufSize(128);
    if (tblastop == D3DOP_PROCESSVERTICES) // Add to previous op
    {
        ((LPD3DPROCESSVERTICES)((uint8_t *)lasttb + sizeof(D3DINSTRUCTION)))->dwCount += vc;
    }
    else                                    // Make new op
    {
        lasttb = tb;
        tblastop = D3DOP_PROCESSVERTICES;
        OP_PROCESS_VERTICES(1, tb);
            PROCESSVERTICES_DATA(vertmode, startvert, vc, tb);
    }

  // Spew the triangle list op
    CheckFaceBufSize(sizeof(D3DTRIANGLE) * numtri + 128);

    if (fblastop == D3DOP_TRIANGLE)
    {
        ((LPD3DINSTRUCTION)lastfb)->wCount += (uint16_t)numtri;
    }
    else 
    {
        if (QWORD_ALIGNED(fb))
             OP_NOP(fb);
        lastfb = fb;
        fblastop = D3DOP_TRIANGLE;
        OP_TRIANGLE_LIST((uint16_t)numtri, fb);
    }

    LPD3DTRIANGLE t = (LPD3DTRIANGLE)fb;
    int32_t vi = startvert; // Uses vert index if 'i' is nullptr
    for (c = 0; c < numtri; c++, t++)
    {
        if (i)
        {
            t->v1 = *(i++) + startvert;
            t->v2 = *(i++) + startvert;
            t->v3 = *(i++) + startvert;
        }
        else
        {
            t->v1 = vi++;
            t->v2 = vi++;
            t->v3 = vi++;
        }
        t->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
    }
    fb = (char *)fb + (sizeof(D3DTRIANGLE) * numtri);

    return DD_OK;
}

