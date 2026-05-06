// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 3dimage.h - 3D Object Include File                    *
// *                                                                       *
// *  Ported (2026): D3D3-era handle/surface types have been replaced with *
// *  render3d_types.h vocabulary. Material and texture handles are now    *
// *  engine-issued ids (TMaterialHandle / TTextureHandle) that the        *
// *  renderer maps to sokol_gfx images + material uniform slots. Per-     *
// *  imagery execute-buffer caching is gone (see 3dscene.h note).         *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "3dimagebody.h"
#include "3dscene.h"
#include "display.h"
#include "object.h"
#include "parse.h"
#include "render3d_types.h"
#include "resource.h"

// **************************************************************
// * T3DImagery - Shared imagery resource for a given 3D object *
// **************************************************************

_CLASSDEF(T3DImagery)
_CLASSDEF(T3DAnimator)
_CLASSDEF(T3DImageryRes)

_STRUCTDEF(S3DMat)
struct S3DMat
{
    S3DMaterial     matdesc;     // Color properties (diffuse/ambient/specular/etc)
    TMaterialHandle hmaterial;   // Engine-issued material id
    int32_t         texture;     // Linked texture index (-1 if none)
};
typedef TVirtualArray<S3DMat, 4, 4> T3DMatArray;

_STRUCTDEF(S3DTex)
struct S3DTex
{
    SSurfaceDesc    desc;         // Source surface description
    int32_t         numframes;    // Number of frames
    int32_t         framenum;     // Current frame index

    sg_image*       framesurfs;   // Per-frame graphics images (size = numframes)
    TTextureHandle* framehtexs;   // Per-frame engine texture handles

    bool            copyframes;   // If true, surface/htexture below are the
                                  // currently-bound frame copied into one slot;
                                  // if false, they alias into the frame arrays.

    sg_image        surface;      // Current frame graphics image
    TTextureHandle  htexture;     // Current frame engine texture handle
};
typedef TVirtualArray<S3DTex, 4, 4> T3DTexArray;

_STRUCTDEF(S3DObj)
struct S3DObj
{
    char name[RESNAMELEN];
    int32_t *parent;                        // Parent object for each state
    int32_t numverts;
    int32_t startvert;
    int32_t numfaces;
    int32_t startface;
    int32_t *texfaces;                      // 0 is no texture, 1 is texture 0, etc.
    int32_t *numtexfaces;
    int32_t material;
    int32_t *numanikeys;
    void **anikeys;
};
typedef TVirtualArray<S3DObj, 16, 16> T3DObjArray;

_STRUCTDEF(S3DTag)
struct S3DTag
{
    int32_t state;
    int32_t frame;
    char *name;
    char *str;
    S3DTag() { state = frame = 0; name = str = nullptr; }
    S3DTag(int32_t pstate, int32_t pframe, char *pname, char *pstr)
      { state = pstate; frame = pframe; name = strdup(pname); str = strdup(pstr); }
    ~S3DTag() { if (name) free(name); if (str) free(str); }
};
typedef TVirtualArray<S3DTag, 16, 16> T3DTagArray;

_STRUCTDEF(S3DImageryIcons)
struct S3DImageryIcons
{
    TBitmap* invitem;
    TAnimation* invanim;
};

#define MAX3DANIMOBJECTS 256

#define OBJ3D_HIDE      0x000001    // Hide this object
#define OBJ3D_ANIMTRACK 0x000002    // Override objects default anim track with given track
#define OBJ3D_POS1      0x000004    // Override object animation - Sets object position first
#define OBJ3D_POS2      0x000008    // Sets object position second (after either rot or scl)
#define OBJ3D_POS3      0x00000C    // Sets object position third (after both rot and scl)
#define OBJ3D_POSMASK   0x00000C    // Pos override mask (2 bits)
#define OBJ3D_ROT1      0x000010    // Override object animation - Sets object rotation first
#define OBJ3D_ROT2      0x000020    // Sets object rotation second (after either pos or scl)
#define OBJ3D_ROT3      0x000030    // Sets object rotation third (after both pos and scl)
#define OBJ3D_ROTMASK   0x000030    // Rot override mask (2 bits)
#define OBJ3D_SCL1      0x000040    // Override objects animation - Sets object scale first
#define OBJ3D_SCL2      0x000080    // Sets object scale second (after either rot or pos)
#define OBJ3D_SCL3      0x0000C0    // Sets object scale third (after both rot and pos)
#define OBJ3D_SCLMASK   0x0000C0    // Scale override mask (2 bits)
#define OBJ3D_MATRIX    0x000100    // Override object matrix with given matrix (ignores anikey, pos, rot, and scl)
#define OBJ3D_ADDTOANI  0x000200    // Adds above transforms to default obj anim (otherwise overrides)
#define OBJ3D_PRIMTYPE  0x000400    // Override objects default primitive type with given type
#define OBJ3D_VERTTYPE  0x000800    // Override objects default vertex type with given type
#define OBJ3D_VERTONLY  0x001000    // Ignores face list (uses DrawPrim insted of DrawIndexPrim)
#define OBJ3D_VERTS     0x002000    // Overrides objects default vertex list stored in T3DImagery
#define OBJ3D_COPYVERTS 0x004000    // Causes NewObject() to allocate local copy of vertices for this obj
#define OBJ3D_OWNSVERTS 0x008000    // Set if object should delete its own vertex buffer when deleted
#define OBJ3D_FACES     0x010000    // Overrides objects default face list stored in T3DImagery
#define OBJ3D_COPYFACES 0x020000    // Causes NewObject() to allocate local copy of faces for this obj
#define OBJ3D_OWNSFACES 0x040000    // Set if object should delete its own face buffer when deleted
#define OBJ3D_MAT       0x080000    // Overrides objects default material handle
#define OBJ3D_TEXFRAME  0x100000    // Overrides objects default texture frame number
#define OBJ3D_TEX       0x200000    // Overrides objects default texture handles
#define OBJ3D_ABSPOS    0x400000    // Treats the position as absolute (already converted to world space)
#define OBJ3D_PARENT    0x800000    // Override parent pointer

#define OBJ3D_RENDERED  0x80000000  // Used internally to indicate if object was rendered

_STRUCTDEF(S3DAnimObj)
struct S3DAnimObj
{
    int32_t       flags;
    int32_t       objnum;                               // Always set (not an override)
    S3DAnimObj*   parent;                               // Pointer to a parent object (concatenates matrices)
    int32_t       animtrack;                            // Borrow another object's anim track
    hmm_vec3      pos;                                  // Used with OBJ3D_POS / ROT / SCL
    hmm_vec3      rot;
    hmm_vec3      scl;
    hmm_mat4      matrix;                               // Directly via OBJ3D_MATRIX, or derived from pos/rot/scl
    ERender3DPrim primtype;                             // Topology
    ERender3DVertex verttype;                           // Layout selector for `verts`
    int32_t       numverts;
    void*         verts;                                // Points at array of S3DVertex / S3DLVertex / S3DTLVertex
                                                        // (based on `verttype`)
    int32_t       numfaces;
    S3DFace*      faces;
    int32_t       texfaces[MAXTEXTURES + 1];            // Start face for each texture (index 0 is no texture)
    int32_t       numtexfaces[MAXTEXTURES + 1];         // Num faces to render for each texture
    int32_t       textureframe[MAXTEXTURES + 1];        // Frame number for animating textures
    TTextureHandle htextures[MAXTEXTURES];              // Texture handles per texture slot
    sg_image      surfaces[MAXTEXTURES];                // Bound sg_image per texture slot
    TMaterialHandle hmaterial;                          // Material handle (only 1 per obj)
};
typedef TPointerArray<S3DAnimObj, 16, 16> T3DAnimObjArray;

class T3DImagery : public TObjectImagery
{
  protected:
    bool meshinitialized;
    bool hastextures;

    int32_t baseobj;

    uint32_t flags;                       // Flags for this 3D object
    uint32_t version;

    int32_t numverts;                     // Global retail-style vertex list
    S3DVertex ***verts;

    int32_t numfaces;                     // Global face list
    S3DFace *faces;

    T3DMatArray materials;                // Object resource arrays
    T3DTexArray textures;
    T3DObjArray objects;
    T3DTagArray tags;

    int32_t numframes;                    // Mesh/Ani/Icon/etc. arrays
    SMotionData **motion;
    S3DImageryIcons *icons;
    unsigned char *hierarchy;

  // State data for rendering
    int32_t prevstate;                    // Used to control interpolation between states
    int32_t prevframe;                    // Last frame drawn in previous state

  public:
    T3DImagery(int32_t imageid);
    virtual ~T3DImagery();

    bool OldInitializeMesh(SOld3DImageryBody* mesh);
      // Initializes old style 3DImageBody mesh
    bool InitializeMesh(PS3DImageryBody mesh);
      // Creates renderer objects from the given S3DMesh pointer
    void ClearMesh();
      // Deletes 3D objects from the given mesh pointer
    bool MeshInitialized() const { return meshinitialized; }
    virtual bool Restore();

  // GENERAL NOTE: T3DImagery is instantiated once per imagery file and shared
  // by every T3DAnimator built from it.  Vertices, faces, objects, materials,
  // and textures are shared state — mutating them changes every animator.

  // Vertex/Face Functions (Note: vertices can't be added or deleted by outside program)
    int32_t NumVerts();
    void GetVerts(void* vertbuf, int32_t state = 0, int32_t frame = 0,
        ERender3DVertex verttype = ERender3DVertex::Vertex, int32_t beg = 0, int32_t len = -1);
      // Copies a single frame of vertices (default is state 0, frame 0, all verts).
      // Vertex returns stored retail-style S3DVertex records; Lit/TL are adapted.
    int32_t NumObjVerts(int32_t objnum);
    void GetObjVerts(int32_t objnum, void* vertbuf, int32_t state = 0, int32_t frame = 0,
        ERender3DVertex verttype = ERender3DVertex::Vertex);
    int32_t NumFaces();
    void GetFaces(S3DFace* facesbuf);
    int32_t NumObjFaces(int32_t objnum);
    void GetObjFaces(int32_t objnum, S3DFace* facesbuf, int32_t *texfaces = nullptr, int32_t *numtexfaces = nullptr);

  // Object functions (Note: objects can't be added or deleted by outside program)
  private:
    int32_t AddObject(S3DObj* obj);
    void RemoveObject(int32_t objnum);
    void ClearObjects();
  public:
    int32_t NumObjects();
    char *GetObjectName(int32_t objnum);
    int32_t GetObjectNum(char *objname);
    void GetObject(int32_t objnum, S3DObj* obj);
    bool IsHidden(int32_t objnum, int32_t state);
    bool GetUninterpolatedAniKey(int32_t objnum, int32_t state, int32_t frame,
        hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl);
    bool GetAniKey(int32_t objnum, int32_t state, int32_t frame,
        hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl);
    void SetPrevState(int32_t pstate, int32_t pframe)
      { prevstate = pstate; prevframe = pframe; }
    int32_t GetObjectParent(int32_t objnum, int32_t state) const { return objects[objnum].parent[state]; }
    void BeginRender(bool clearbuf);
      // Called before any RenderObject() calls for an animator.
      // (The legacy second `cachebuf` argument is gone — the execute-buffer
      // cache path has been removed.)
    bool CalcObjectMatrix(S3DAnimObj* animobj, int32_t state, int32_t frame, hmm_mat4* pos,
        bool calcparents = true);
    bool CalcObjectMatrixCopy(S3DAnimObj* animobj, int32_t state, int32_t frame, hmm_mat4* pos,
        bool calcparents = true);
    bool RenderObject(S3DAnimObj* animobj, int32_t state, int32_t frame, hmm_mat4* pos,
        int32_t tex = -1, bool uselastmatrix = false);
    void EndRender();
    void PlaySound(TObjectInstance* inst, int32_t state, int32_t frame);

  // Material functions (Note: materials can't be added or deleted by outside program)
  private:
    int32_t AddMaterial(S3DMaterial* newmat, int32_t tex = -1);
    void RemoveMaterial(int32_t matnum);
    void ClearMaterials();
  public:
    int32_t NumMaterials() const { return materials.NumItems(); }
    void GetMaterial(int32_t matnum, S3DMat* mat);
    TMaterialHandle GetMaterialHandle(int32_t matnum);
    void SetMaterial(int32_t matnum, S3DMat* mat);

  // Texture functions (Note: textures can't be added or deleted by outside program)
  private:
    int32_t AddTexture(SSurfaceDesc* srcsd,
        OFFSET *pixels, int32_t frames, void *palette);
    bool LoadTexture(S3DTex* tex, SSurfaceDesc* srcsd,
        OFFSET *pixels, int32_t frames, void *palette, bool copyframes);
    void RemoveTexture(int32_t texnum);
    void ClearTextures();
  public:
    int32_t NumTextures();
    void GetTexture(int32_t texnum, S3DTex* tex);
    TTextureHandle GetTextureHandle(int32_t texnum);
    int32_t GetTextureFrame(int32_t texnum);
    bool SetTextureFrame(int32_t texnum, int32_t framenum);
    bool SurfacesLost();
    void RestoreSurfaces();

  // Miscellaneous support functions
    void RefreshZBuffer(TObjectInstance* oi);
    void ResetExtents();
    void GetExtents(SRenderRect* extents);
    void AddUpdateRect(SRenderRect* extents, int32_t uflags = UPDATE_RESTORE);
    void UpdateBoundingRect(TObjectInstance* oi, int32_t state, SRenderRect* extents);
    int32_t NumTags() const { return tags.NumItems(); }
    S3DTag* GetTag(int32_t tagnum) { return &(tags[tagnum]); }
    char* FindTag(const char* name, int32_t state, int32_t frame = -1, int32_t* foundstate = nullptr, int32_t* foundframe = nullptr);

    virtual bool GetZ(TObjectInstance* oi, TSurface* surface) { return true; }
    virtual bool AlwaysOnTop(TObjectInstance* oi) { return true; }
    virtual TBitmap* GetInvImage(int32_t state, int32_t num = 0);
    virtual TAnimation* GetInvAnimation(int32_t state);

    void AttachAnimatorComponents(TObjectInstance* oi);
    virtual TObjectAnimator* NewObjectAnimator(TObjectInstance* oi);
    bool NeedsAnimator(const TObjectInstance* oi) const override;

  // Motion functions
    bool GetMotion(int32_t state, int32_t frame,
        int32_t &dist, int32_t &vert, int32_t &ang, int32_t &rotx, int32_t &roty, int32_t &rotz);
    virtual void SetObjectMotion(TObjectInstance* inst);
};

DEFINE_IMAGERYBUILDER(OBJIMAGE_MESH3D, T3DImagery);

// T3DController + T3DControllerBuilder removed: the legacy 3D-tag-driven
// controller hierarchy (scrolltex etc.) ran during the old D3D render path
// that the drawable/mesh pipeline has fully replaced. No external consumers.

// ***************************************************************************
// * T3DAnimatorBuilder - Used to register and automatically build animators *
// ***************************************************************************

#define MAX3DANIMATORTYPES 128

_CLASSDEF(T3DAnimatorBuilder)
class T3DAnimatorBuilder
{
  public:
    T3DAnimatorBuilder();
    T3DAnimatorBuilder(const char* name);
    virtual T3DAnimator* Build(TObjectInstance* oi);
    virtual void AttachComponents(TObjectInstance* oi) { (void)oi; }
    static T3DAnimatorBuilder* GetBuilder(const char* name);

  private:
    static int32_t numanimtypes;
    static T3DAnimatorBuilder* builders[MAX3DANIMATORTYPES];

    char* animatorname;
};

#define REGISTER_3DANIMATOR(name, obj)                                          \
class obj##Builder : public T3DAnimatorBuilder                                  \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DAnimatorBuilder(name) {}                                \
    virtual T3DAnimator* Build(TObjectInstance* oi)                             \
        { return new obj(oi); }                                                 \
};                                                                              \
obj##Builder obj##BuilderInstance;

#define REGISTER_MULTI_3DANIMATOR_(name, obj, anim)                             \
class obj##Builder : public T3DAnimatorBuilder                                  \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DAnimatorBuilder(name) {}                                \
    virtual T3DAnimator* Build(TObjectInstance* oi)                             \
        { return new anim(oi); }                                                \
};                                                                              \
obj##Builder obj##BuilderInstance;

// ********************************************
// * T3DAnimator - Default 3D Animator Object *
// ********************************************

#define ANI3D_ADDUPDATERECT         1   // (default true) adds update rect for extents of all objects
#define ANI3D_UPDATEBOUNDRECT       2   // (default false) uses extents to reset obj state bounding rects
#define ANI3D_UPDATEALLSTATES       4   // (default false) resets extents for all states
#define ANI3D_UPDATEFRONTONLY       8   // (default false) resets extents for front rotation only

class T3DAnimator : public TObjectAnimator
{
  protected:
    uint32_t         flags;
    int32_t          animid;
    T3DAnimObjArray  animobjs;
    bool             changed;
    hmm_vec3         pos;
    hmm_vec3         rot;
    hmm_vec3         scl;
    SRenderRect      extents;
    hmm_mat4         matrix;
    bool             updated;

  public:
    T3DAnimator(TObjectInstance* oi) : TObjectAnimator(oi) {}
    virtual ~T3DAnimator();

    virtual void Initialize();
    virtual void Close();

    uint32_t GetFlags() const { return flags; }
    void SetFlags(uint32_t newflags) { flags = newflags; }
    T3DImagery* Get3DImagery() const { return (T3DImagery*)image; }

    virtual void Pulse();
    virtual void Animate(bool draw);

    virtual bool SurfacesLost();
    virtual void RestoreSurfaces();

  // Object Functions
    virtual void SetupObjects();
    void GetVerts(S3DAnimObj* obj, ERender3DVertex verttype = ERender3DVertex::Vertex);
    void FreeVerts(S3DAnimObj* obj);
    void GetFaces(S3DAnimObj* obj);
    void FreeFaces(S3DAnimObj* obj);
    int32_t NumObjects() const { return animobjs.NumItems(); }
    S3DAnimObj* NewObject(int32_t objnum, int32_t flags = 0);
    int32_t AddObject(S3DAnimObj* obj);
    void RemoveObject(int32_t objnum);
    int32_t GetObjectNum(const char* name);
    S3DAnimObj* GetObject(int32_t objnum);
    bool IsObjectEnabled(int32_t objnum);
    void EnableObject(int32_t objnum, bool enable);
    bool RenderObject(S3DAnimObj* animobj, int32_t tex = -1, bool uselastmatrix = false)
      { return Get3DImagery()->RenderObject(animobj, state, frame, &matrix, tex, uselastmatrix); }
    bool GetObjectMatrix(int32_t objnum, hmm_mat4* m);
    bool GetObjectPos(int32_t objnum, hmm_vec3& v, hmm_vec3* s = nullptr);
    bool GetObjectPos(const char* objname, hmm_vec3& v, hmm_vec3* s = nullptr);
    bool GetObjectMapPos(int32_t objnum, S3DPoint& pos);
    bool GetObjectMapPos(const char* objname, S3DPoint& pos);

  // Render functions
    virtual void RefreshZBuffer()
      { ((T3DImagery*)image)->RefreshZBuffer(inst); }
    void MakeMatrix(hmm_mat4* m);
    void PreRender();
    virtual bool Render();
    void PostRender();
    hmm_mat4* GetMatrix() { return &matrix; };

  // Extents
    void ResetExtents()
      { Get3DImagery()->ResetExtents(); updated = false; }
    void GetExtents(SRenderRect* extents)
      { Get3DImagery()->GetExtents(extents); }
    void AddUpdateRect(SRenderRect* extents, int32_t uflags = UPDATE_RESTORE)
      { if (flags & ANI3D_ADDUPDATERECT)
            Get3DImagery()->AddUpdateRect(extents, uflags); updated = true; }
    void UpdateBoundingRect(SRenderRect* extents)
      { if (flags & ANI3D_UPDATEBOUNDRECT)
            Get3DImagery()->UpdateBoundingRect(inst, state, extents); updated = true; }
    bool WasUpdated() const { return updated; }
    void UpdateExtents()
      { GetExtents(&extents); AddUpdateRect(&extents); UpdateBoundingRect(&extents); }
    void RecordNewExtents(TObjectInstance* oi, int32_t state = -1, bool frontonly = false);

  private:
    void AnimateResetBoundRect();
};
