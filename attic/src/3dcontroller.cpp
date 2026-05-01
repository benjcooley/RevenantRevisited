// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      3dcontroller.cpp - retired 3D-tag-driven controller system      *
// *************************************************************************
//
// Retired from src/3dcont.cpp + src/3dimage.{h,cpp} on 2026-04-30 during
// the animator-component sweep (commit "animator: pass B.1 -- drop
// T3DController hierarchy"). T3DController was the tag-driven side-effect
// system attached to T3DAnimator: each animation tag in an I3D file (e.g.
// "scrolltex obj=water,du=0.01,dv=0" or "animtex obj=fire,frames=4")
// would build a controller during state changes; the controller's Pulse
// and Render fired during the animator's per-frame pulse / render walk
// to do things like UV scrolling or texture flipbooks.
//
// Why retired here: the controller hooks ran inside the legacy d3d
// render walk on T3DAnimator. The drawable / mesh-pipeline rewrite owns
// per-instance pose and texture state directly off the imagery; the
// tag-callback approach didn't fit. If we want tag-driven side effects
// in the new world, the home is a small dispatcher invoked by the same
// place that samples pose -- not a polymorphic controller hierarchy
// stored on the animator.
//
// What may carry over to a new tag-callback path:
//   * The state/frame -> tag matching (RefreshControllers logic): walk
//     the imagery's tags, prune controllers whose tagstate doesn't
//     match the new state, build new controllers for tags whose state
//     newly matches.
//   * The two registered controllers (scrolltex, animtex) as concrete
//     examples of the side-effect shape.
//   * The OBJ list parsing in T3DController::ParseItem (handles
//     "obj=foo" and "obj=(foo,bar,baz)" forms).
//
// What does NOT carry over:
//   * Anything that calls into the legacy 3D render path (RenderObject,
//     blend state save/restore, S3DAnimObj manipulation).
//   * The "controllers as members of T3DAnimator" ownership model.
//
// This file is reference-only; it does not participate in the build
// (attic/ is excluded). When a tag-callback dispatcher lands in the
// new path, lift the matching/parsing code identified above and delete
// this file.
//
// *************************************************************************

#if 0  // attic-only -- never compiled

// =========================================================================
// Originally in src/3dimage.h:
// =========================================================================

// *******************************************************************************
// * T3DControllerBuilder - Used to register and automatically build controllers *
// *******************************************************************************

#define MAX3DCONTROLLERTYPES 128

_CLASSDEF(T3DControllerBuilder)
class T3DControllerBuilder
{
  public:
    T3DControllerBuilder();
    T3DControllerBuilder(const char* name);
    virtual T3DController* Build(int32_t ptagstate, int32_t ptagframe,
        T3DAnimator* panimator, T3DImagery* pimagery, TObjectInstance* pinst) { return nullptr; }
    static T3DControllerBuilder* GetBuilder(const char* name);

  private:
    static int32_t numconttypes;
    static T3DControllerBuilder* builders[MAX3DCONTROLLERTYPES];

    char* controllername;
};

#define REGISTER_3DCONTROLLER(name, obj)                                        \
class obj##Builder : public T3DControllerBuilder                                \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DControllerBuilder(name) {}                              \
    virtual T3DController* Build(int32_t ptagstate, int32_t ptagframe,          \
      T3DAnimator* panimator, T3DImagery* pimagery, TObjectInstance* pinst)     \
        { return new obj(ptagstate, ptagframe, panimator, pimagery, pinst); }   \
};                                                                              \
obj##Builder obj##BuilderInstance;

// *********************************************************************
// * T3DController - Controls Animated Effects Based on Animation Tags *
// *********************************************************************

_CLASSDEF(T3DController)
class T3DController
{
  protected:
    int32_t tagstate, tagframe;
    T3DAnimator* animator;
    T3DImagery* imagery;
    TObjectInstance* inst;
    T3DAnimObjArray animobjs;

    virtual bool ParseParams(TToken &t);
    virtual bool ParseItem(char *param, TToken &t);

  public:
    T3DController(int32_t ptagstate, int32_t ptagframe, T3DAnimator* panimator, T3DImagery* pimagery, TObjectInstance* pinst)
      { tagstate = ptagstate; tagframe = ptagframe;
        animator = panimator; imagery = pimagery; inst = pinst; }
    virtual ~T3DController() { Close();}

    virtual bool Initialize(char *params);
    virtual void Close();

    int32_t TagState() const { return tagstate; }
    int32_t TagFrame() const { return tagframe; }
    T3DAnimator* GetAnimator() const { return animator; }
    T3DImagery* GetImagery() const { return imagery; }
    TObjectInstance* GetInstance() const { return inst; }
    int32_t NumObjects() const { return animobjs.NumItems(); }
    S3DAnimObj* GetObject(int32_t objnum) const { return animobjs[objnum]; }

    virtual void Pulse() {}
    virtual void Render() {}
    virtual bool KillMe() { return false; }
};
typedef TPointerArray<T3DController, 0, 16> T3DControllerArray;

// In T3DAnimator (member fields + methods):
//   T3DControllerArray controllers;
//   int32_t            contprevstate;
//   virtual void RefreshControllers(int32_t state);
//   int32_t NumControllers() const { return controllers.NumItems(); }
//   T3DController* GetController(int32_t num) const { return controllers[num]; }
//   void AddController(T3DController* cont) { controllers.Add(cont); }
//   void RemoveController(int32_t num) { controllers.Collapse(num, true); }

// =========================================================================
// Originally in src/3dimage.cpp -- T3DControllerBuilder + T3DController:
// =========================================================================

// *******************************************************************************
// * T3DControllerBuilder                                                         *
// *******************************************************************************

int32_t T3DControllerBuilder::numconttypes = 0;
T3DControllerBuilder* T3DControllerBuilder::builders[MAX3DCONTROLLERTYPES];

T3DControllerBuilder::T3DControllerBuilder(const char* name)
{
    if (numconttypes < MAX3DCONTROLLERTYPES)
        builders[numconttypes++] = this;

    controllername = _strdup(name);
}

T3DControllerBuilder* T3DControllerBuilder::GetBuilder(const char* name)
{
    if (!stricmp(name, "play") || !stricmp(name, "beg") || !stricmp(name, "end"))
        return nullptr;

    for (int32_t i = 0; i < numconttypes; i++)
        if (stricmp(name, builders[i]->controllername) == 0)
            return builders[i];

    return nullptr;
}

// *********************************************************************
// * T3DController                                                      *
// *********************************************************************

bool T3DController::ParseParams(TToken& t)
{
    while (t.Type() != TKN_EOF)
    {
        if (t.Type() != TKN_IDENT)
            return false;

        char param[80];
        strncpyz(param, t.Text(), 80);
        t.WhiteGet();

        if (t.Is("="))
            t.WhiteGet();

        if (!ParseItem(param, t))
            return false;

        if (!(t.Is(",") || t.Type() == TKN_EOF))
            return false;

        if (t.Is(","))
            t.WhiteGet();
    }
    return true;
}

bool T3DController::ParseItem(char* name, TToken& t)
{
    if (!stricmp(name, "obj"))
    {
        if (t.Is("("))
        {
            t.WhiteGet();
            while (t.Type() != TKN_EOF && !t.Is(")"))
            {
                if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
                    return false;

                int32_t objnum = animator->GetObjectNum(t.Text());
                if (objnum >= 0)
                    animobjs.Add(animator->GetObject(objnum));

                t.WhiteGet();
                if (t.Is(","))
                    t.WhiteGet();
            }
            if (t.Is(")"))
                t.WhiteGet();
        }
        else if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
        {
            int32_t objnum = animator->GetObjectNum(t.Text());
            if (objnum >= 0)
                animobjs.Add(animator->GetObject(objnum));
            t.WhiteGet();
        }
        else
            return false;

        return true;
    }

    return false;
}

bool T3DController::Initialize(char* params)
{
    TStringParseStream s(params, strlen(params));
    TToken t(s);

    t.WhiteGet();
    return ParseParams(t);
}

void T3DController::Close()
{
    animobjs.Clear();
}

// =========================================================================
// T3DAnimator integration -- the hooks that drove controllers:
// =========================================================================

// In T3DAnimator::Initialize() (after SetupObjects()):
//     contprevstate = -1;
//     RefreshControllers(inst->GetState());

// In T3DAnimator::Close() (before Scene3D.RemoveAnimator):
//     controllers.DeleteAll();

void T3DAnimator::RefreshControllers(int32_t newstate)
{
    int32_t c;

    if (newstate == contprevstate)
        return;

    for (c = controllers.NumItems() - 1; c >= 0; c--)
    {
        int32_t tagstate = controllers[c]->TagState();
        if (tagstate != -1 && tagstate != newstate)
            RemoveController(c);
    }

    int32_t numtags = Get3DImagery()->NumTags();
    for (c = 0; c < numtags; c++)
    {
        S3DTag* tag = Get3DImagery()->GetTag(c);
        if (!((contprevstate == -1 && tag->state == -1) ||
              tag->state == newstate))
            continue;

        T3DControllerBuilder* cbuilder = T3DControllerBuilder::GetBuilder(tag->name);
        if (cbuilder)
        {
            T3DController* cont = cbuilder->Build(tag->state, tag->frame,
                this, Get3DImagery(), inst);
            if (cont)
            {
                if (!cont->Initialize(tag->str))
                    delete cont;
                else
                    controllers.Add(cont);
            }
        }
    }

    contprevstate = newstate;
}

// In T3DAnimator::Pulse() (before PlaySound):
//     RefreshControllers(inst->GetState());
//     for (int32_t c = 0; c < controllers.NumItems(); c++)
//         controllers[c]->Pulse();

// In T3DAnimator::Render() (before the OBJ3D_RENDERED traversal):
//     for (c = 0; c < controllers.NumItems(); c++)
//         controllers[c]->Render();

// In T3DAnimator::PostRender() (after Scene3D.ResetAllLights):
//     for (int32_t c = 0; c < controllers.NumItems(); c++)
//     {
//         if (controllers[c]->KillMe())
//             controllers.Collapse(c, true);
//     }

// =========================================================================
// Original src/3dcont.cpp -- the two registered controllers:
// =========================================================================

// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               3dcont.cpp - Effect components module                   *
// *************************************************************************

#include "3dscene.h"
#include "3dimage.h"
#include "playscreen.h"
#include "object.h"
#include "parse.h"
#include "render3d_types.h"

// **************************************************************************
// * TScrollTexController - Scrolls a texture for an object given delta u,v *
// **************************************************************************

// TAG FORMAT
//
//      scrolltex:obj=<objlist>,du=<udelta>,dv=<vdelta>
//
//      <objlist> = Single object, or list of objects surrounded by ()
//      <udelta>  = U texture delta floating point value
//      <vdelta>  = V texture delta floating point value

struct UV
{
    float u, v;
};

_CLASSDEF(TScrollTexController)
class TScrollTexController : public T3DController
{
  private:
    float du, dv;           // Floating point delta u and v
    UV **olduv;             // Original uv's for object

  protected:
    virtual bool ParseItem(char *param, TToken &t);
      // Parses a parameter item

  public:
    TScrollTexController(int32_t s, int32_t f, T3DAnimator* a, T3DImagery* i, TObjectInstance* o) :
      T3DController(s, f, a, i, o) {}
      // Constructor
    virtual ~TScrollTexController() { Close(); }
      // Destructor
    virtual bool Initialize(char *params);
      // Initialize the controller
    virtual void Close();
      // Closes the controller
    virtual void Render();
      // Called during render phase to render controller objects
};

REGISTER_3DCONTROLLER("scrolltex", TScrollTexController)

bool TScrollTexController::ParseItem(char *param, TToken &t)
{
    double d;

    if (!stricmp(param, "du"))
    {
        if (!Parse(t, "%f", &d))
            return false;
        du = (float)d;
    }
    else if (!stricmp(param, "dv"))
    {
        if (!Parse(t, "%f", &d))
            return false;
        dv = (float)d;
    }
    else
        return T3DController::ParseItem(param, t);

    return true;
}

bool TScrollTexController::Initialize(char *params)
{
    if (!T3DController::Initialize(params))
        return false;

  // Add all objects by default
    if (animobjs.NumItems() <= 0)
    {
        for (int32_t c = 0; c < animator->NumObjects(); c++)
            animobjs.Add(animator->GetObject(c));
    }

  // No objects??
    if (animobjs.NumItems() <= 0)
        return false;

  // Get UV's pointer array
    olduv = new UV*[animobjs.NumItems()];

  // Override vertices for this object
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        S3DAnimObj* obj = animobjs[o];

        animator->GetVerts(obj);
        olduv[o] = new UV[obj->numverts];

      // Save original uv's for this object
        S3DVertex* v = (S3DVertex*)obj->verts;
        UV *uv = olduv[o];
        for (int32_t c = 0; c < obj->numverts; c++, v++, uv++)
        {
            uv->u = v->tu;
            uv->v = v->tv;
        }
    }
    
    return true;
}   

void TScrollTexController::Close()
{
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        delete olduv[o];
    }
    delete olduv;

    T3DController::Close();
}
      
void TScrollTexController::Render()
{
  // Don't animate until first frame is reached
    if (inst->GetFrame() < tagframe)
        return;

  // This little trick keeps all textures synchronized everywhere always
    float f = (float)PlayScreen.FrameCount();
    float newdu = du * f;
    float newdv = dv * f;

  // Scroll texture vertices for each object
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        S3DAnimObj* obj = animobjs[o];

      // Add new deltau and deltav to original u and v for each object
        S3DVertex* v = (S3DVertex*)obj->verts;
        UV *uv = olduv[o];
        for (int32_t c = 0; c < obj->numverts; c++, v++, uv++)
        {
            v->tu = uv->u + newdu;
            v->tv = uv->v + newdv;
        }
    }
}           

// ******************************************************************
// * TAnimTexController - Animates texture from a uv grid of frames *
// ******************************************************************

// TAG FORMAT
//
//      animtex:obj=<objlist>,u=<uframes>,v=<vframes>
//
//      <objlist> = Single object, or list of objects surrounded by ()
//      <uframes>  = Number of frame grids in u direction
//      <vframes>  = Number of frame grids in v direction

_CLASSDEF(TAnimTexController)
class TAnimTexController : public T3DController
{
  private:
    int32_t ugrid, vgrid;       // Number of frames in u and v directions
    float usize, vsize;     // Size of each frame in u and v space
    UV **olduv;             // Original uv's for object

  protected:
    virtual bool ParseItem(char *param, TToken &t);
      // Parses a parameter item

  public:
    TAnimTexController(int32_t s, int32_t f, T3DAnimator* a, T3DImagery* i, TObjectInstance* o) :
      T3DController(s, f, a, i, o) {}
      // Constructor
    virtual ~TAnimTexController() { Close(); }
      // Destructor
    virtual bool Initialize(char *params);
      // Initialize the controller
    virtual void Close();
      // Closes the controller
    virtual void Render();
      // Called during render phase to render controller objects
};

REGISTER_3DCONTROLLER("animtex", TAnimTexController)

bool TAnimTexController::ParseItem(char *param, TToken &t)
{
    if (!stricmp(param, "u"))
    {
        if (!Parse(t, "%i", &ugrid))
            return false;
        usize = 1.0f / (float)ugrid;
    }
    else if (!stricmp(param, "v"))
    {
        if (!Parse(t, "%i", &vgrid))
            return false;
        vsize = 1.0f / (float)vgrid;
    }
    else
        return T3DController::ParseItem(param, t);

    return true;
}

bool TAnimTexController::Initialize(char *params)
{
    if (!T3DController::Initialize(params))
        return false;

  // Add all objects by default
    if (animobjs.NumItems() <= 0)
    {
        for (int32_t c = 0; c < animator->NumObjects(); c++)
            animobjs.Add(animator->GetObject(c));
    }

  // No objects??
    if (animobjs.NumItems() <= 0)
        return false;

  // Get UV's pointer array
    olduv = new UV*[animobjs.NumItems()];

  // Override vertices for this object
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        S3DAnimObj* obj = animobjs[o];

        animator->GetVerts(obj);
        olduv[o] = new UV[obj->numverts];

      // Save original uv's for this object
        S3DVertex* v = (S3DVertex*)obj->verts;
        UV *uv = olduv[o];
        for (int32_t c = 0; c < obj->numverts; c++, v++, uv++)
        {
            uv->u = v->tu;
            uv->v = v->tv;
        }
    }
    
    return true;
}   

void TAnimTexController::Close()
{
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        delete olduv[o];
    }
    delete olduv;

    T3DController::Close();
}
      
void TAnimTexController::Render()
{
  // Don't animate until first frame is reached
    if (inst->GetFrame() < tagframe)
        return;

  // This little trick keeps all textures synchronized everywhere always
    int32_t f = PlayScreen.FrameCount() % (ugrid * vgrid);
    int32_t u = f % ugrid;
    int32_t v = f - (u * ugrid);
    float newdu = usize * (float)u;
    float newdv = vsize * (float)v;

  // Scroll texture vertices for each object
    for (int32_t o = 0; o < animobjs.NumItems(); o++)
    {
        S3DAnimObj* obj = animobjs[o];

      // Add new deltau and deltav to original u and v for each object
        S3DVertex* v = (S3DVertex*)obj->verts;
        UV *uv = olduv[o];
        for (int32_t c = 0; c < obj->numverts; c++, v++, uv++)
        {
            v->tu = uv->u + newdu;
            v->tv = uv->v + newdv;
        }
    }
}           


#endif // attic-only
