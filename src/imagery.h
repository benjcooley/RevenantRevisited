// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  imagery.h - TObjectImagery module                    *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "assetcache.h"
#include "objectcomponent.h"
#include "imageres.h"

#include <stdio.h>
#include <time.h>

// *************************************************************************
// * TObjectImagery/TImageryBuilder - Represents the imagery for an object *
// *************************************************************************

// The TObjectImagery class encapsulates the imagery information and functions
// for an object.  When an object is initialized, the object class creates an
// object imagery object for that type.  If an object needs to be animated, the
// object imagery object creates a TObjectAnimator object to handle the animation
// for the object.  This system allows the object to ignore the details of its own
// graphics and animation, and decouples the graphics/animation system from the
// more abstract object system.

#define MAXIMAGERYTYPES 16

// *******************
// * TImageryBuilder *
// *******************

// Builder which builds imagery objects after imagery buffer is loaded
_CLASSDEF(TImageryBuilder)
class TImageryBuilder
{
  public:
    TImageryBuilder(int32_t newid);
        // Sets id and adds builder to builder array
    virtual TObjectImagery* Build(int32_t id) = 0;
        // Builds an imagery object given an imagery buffer
    static PTImageryBuilder GetBuilder(int32_t imageryid)
      { if ((uint32_t)imageryid < (uint32_t)numimagerytypes) return builders[imageryid]; else return nullptr; }
        // Gets a pointer to a builder in the builder array

  private:
    static int32_t numimagerytypes;                             // Number of object types
    static PTImageryBuilder builders[MAXIMAGERYTYPES];      // Imagery builder for each type

    int32_t imageryid;
};

#define DEFINE_IMAGERYBUILDER(id, obj)                                          \
class obj##Builder : public TImageryBuilder                                     \
{                                                                               \
  public:                                                                       \
    obj##Builder() : TImageryBuilder(id) {}                                     \
    virtual TObjectImagery* Build(int32_t id)                                   \
        { return new obj(id); }                                                 \
};

#define REGISTER_IMAGERYBUILDER(obj) obj##Builder obj##BuilderInstance;

_CLASSDEF(TObjectAnimator);
_CLASSDEF(TObjectImagery)

#define MAXIMFNAMELEN       80

// ******************
// * TObjectImagery *
// ******************

_STRUCTDEF(SImageryEntry);
struct SImageryEntry
{
    char                filename[MAXIMFNAMELEN];    // Filename of resource
    uint32_t            status;        // Status of loading
    SImageryHeader*     header;        // Imagery header
    int32_t             headersize;    // Size of header
    bool                headerdirty;   // Header has been changed
    SImageryBody*       body;          // Imagery body
    bool                bodydirty;     // Body has been changed
    int32_t             ressize;       // Size of body resource
    int32_t             usecount;      // usecount
    TObjectImagery*     imagery;       // Pointer to imagery
};

class TObjectImagery : public TAsset
{
  public:
    TObjectImagery(int32_t id);
    ~TObjectImagery() override;
    virtual bool Restore() { return true; }
      // Restors lost surfaces, textures, etc.
    int32_t ImageryId() { return imageryid; }
      // Returns the id of this imagery (OBJIMAGE_ANIMATION, OBJIMAGE_MESH3D, etc.)

    // TAsset contract. TObjectImagery is the source asset: TAnimImagery and
    // T3DImagery specialize behavior, but ownership/ref tracking starts here.
    [[nodiscard]] AssetUid AssetId() const override;
    [[nodiscard]] EAssetKind AssetKind() const override;
    [[nodiscard]] const char* AssetPath() const override;
    [[nodiscard]] const char* AssetName() const override;
    [[nodiscard]] const char* AssetDebugName() const override;
    [[nodiscard]] TAssetRef<TObjectImagery> AssetRef() const { return TAssetRef<TObjectImagery>(this); }

  // Main imagery functions..
    static int32_t RegisterImagery(char *filename, SImageryHeader* header = nullptr, uint32_t headersize = 0);
      // Adds imagery file to internal image array.  Called by object class system when
      // object definitions are loaded from 'class.def' file.  Register's imagery file and'
      // returns an id number for that imagery.
    static SImageryEntry* GetImageryEntry(int32_t id);
      // Return the entery array structure for the given imagery id
    static int32_t NumEntries();
      // Number of slots in the imagery registry (sparse — combine with IsUsed()).
    static bool IsUsed(int32_t id);
      // True if `id` is a live entry; entries can be free'd, leaving holes.
    static void FreeImageryEntry(int32_t imageryentry);
      // Frees an individual imagery entry
    static void FreeAllImagery();
      // Called when program closes to cause all imagery to be freed (also optionally saves headers)
    static void ReloadImagery();
      // Deletes imagery body.
    static void SaveAllHeaders();
      // Causes header data for all imagery to be dumped to the imagery files
    static void SetImageryPath(char *path);
      // Set the imagery filepath
    static char *GetImageryPath();
      // Returns the current imagery path
    static int32_t FindImagery(const char *imageryname);
      // Finds the id of imagery given imagery file name
    static bool RenameImageryFile(int32_t imgid, const char *newfile);
      // Renames the file that a specific imagery id uses
    static TObjectImagery* LoadImagery(int32_t imageryid);
      // Loads an imagery object given the imagery id (same id returned from RegisterImagery)
    static TObjectImagery* LoadImagery(const char *imageryname) { return LoadImagery(FindImagery(imageryname)); }
      // Loads an imagery object given the object name and class
    static void FreeImagery(TObjectImagery* imagery);
      // Reduces use count of imagery by 1, and frees if no longer used
    static void SaveHeader(int32_t imageryid);
      // Saves the header info for the given imagery id
    static void SetEntryReg(int32_t imageryid, int32_t state, int32_t regx, int32_t regy, int32_t regz);
      // Sets the registration points without having an instance of the imagery
    static void RestoreAll();
      // Restores all lost imagery surfaces, etc.

    static bool QuickLoadHeaders(time_t iflater);
      // Does a quck load of imagery headers if IMAGERY.DAT file is later than 'iflater'
    static bool QuickSaveHeaders();
      // Rewrites IMAGERY.DAT file with current header data

  // Access functions for private data
    char *GetResFilename() { return entry->filename; }
      // Filename
    SImageryHeader* GetHeader() const { return entry->header; }
      // Returns imagery header info
    SImageryBody* GetBody() { if (entry->body) return entry->body; else return LoadBody(true); }
      // Gets the pointer to the imagery body.. loads it now if it's not there
    int32_t GetHeaderSize() const { return entry->headersize; }
      // Returns imagery header info
    uint32_t GetResSize() const { return entry->ressize; }
      // Size for memory tracking

  // Progressive loading stuff
    static void BeginLoaderThread();
      // Starts the progressive load thread going.. call this at the beginning of the game
    static void EndLoaderThread();
      // Ends the progressive loader thread
    static uint_fast32_t LoaderThread(void *args);
      // Progressive loading thread for imagery.
    static void PauseLoader();
      // Causes the imagery loader to pause temporarily
    static void ResumeLoader();
      // Resumes the imagery loader
    SImageryBody* LoadBody(bool wait = true);
      // Adds body load to the loader thread's load queue, waits for load or returns nullptr.
    void FreeBody();
      // Free imagery body and sets body pointer to nullptr

  // Caching functions
    virtual void CacheImagery() {}
      // Causes imagery to load itself and get ready for drawing
      // (Called by the SectorCache system to cause imagery to be loaded and decompressed)
      // AnimImagery uses this to call the TBitmap::CacheChunks() function  

  // Drawing functions, etc.
    virtual void DrawUnlit(TObjectInstance* oi, TSurface* surface) {}
        // Draws unlit imagery to background
    virtual void DrawLit(TObjectInstance* oi, TSurface* surface) {}
        // Draws lit imagery to background
    virtual bool GetZ(TObjectInstance* oi, TSurface* surface) { return false; }
        // Find first uncliped portion of zbuffer
    virtual void DrawSelected(TObjectInstance* oi, TSurface* surface) { }
        // Causes image to draw selection (hilighting) around itself
    virtual void DrawLight(TObjectInstance* oi, TSurface* surface) {}
        // Draws a light
    virtual void DrawInvItem(TObjectInstance* oi, int32_t x, int32_t y);
        // Draws object's inventory bitmap to the screen
    int32_t GetUseCount();
        // Returns number of times imagery is used by objects
    virtual bool AlwaysOnTop(TObjectInstance* oi) { return false; }
        // Returns true if this is nonzbuffered imagery that should always be considered 'on top'

    virtual void SaveHeader() { SaveHeader(imageryid); }
        // Saves header data for this imagery object
    virtual bool SaveBitmap(char *path, int32_t state = 0, bool zbuffer = true) { return false; }
        // Saves a bitmap of the current object given the current state

    virtual PTObjectAnimator NewObjectAnimator(TObjectInstance* oi) { return nullptr; }
      // Creates an animator for the given object
    virtual bool NeedsAnimator(const TObjectInstance* oi) const { return false; }
      // Returns whether or not an animator is necessary

  // State functions
    virtual int32_t NumStates() const { return entry->header->numstates; }
      // Returns the state structure for the given imagery
    virtual int32_t FindState(const char *name, int32_t pcnt = -1) const;
      // Returns state number for a state name equal to character string.  If 'pcnt'
      // is specified, uses specified 'pcnt' value instead of random percent to 
      // find random states...
      // --------------------- RANDOM STATES -----------------------
      // If states are in the format "##%state" where ## is a number from 1-100,
      // FindState() will use 'pcnt', or generate a 1-100 number if pcnt=-1, and find
      // the state which has the percent value closest to, but not above, the random 1-100
      // pcnt value.  For three 'walk' states with a frequency of 20%, 20%, and 60%, you 
      // would use "20%walk","40%walk", and "100%walk".  NOTE: you can't use two "20%walk"
      // states as the state names would then not be unique.  The closest to but not
      // above way of specifying percentages in the name ensures that each state name IS
      // unique.
    virtual int32_t FindTransitionState(const char *from, const char *to, int32_t pcnt = -1) const;
      // Returns state number for transition state matching "<from> to <to>".  The
      // 'pcnt' value specifies the 'pcnt' used to find a random state (see above).  If 
      // pcnt is -1, function will generate a value from 1-100 to find a random state.
      // Note: random states must be in the format "##%state" where ## is value from 1-100.
    PSImageryStateHeader GetState(int32_t index) const
        { if ((uint32_t)index >= (uint32_t)entry->header->numstates) return nullptr;
            else return &(entry->header->states[index]); }
        // Returns the state structure for the given imagery

  // Motion functions
    virtual bool GetMotion(int32_t state, int32_t frame, int32_t &dist, int32_t &ang, int32_t &face) const
      { return false; }
      // Gets the motion data for the given state and frame
    virtual void SetObjectMotion(TObjectInstance* inst) { } 
      // Sets the motion for an object based on its state, frame, and 3D motion data

  // Get/set Header Info (will also cause header to reallocate itself)
    virtual void SetHeaderDirty(bool newdirty) { entry->headerdirty = newdirty; }
      // Set the header's update status
    virtual bool GetHeaderDirty() const { return entry->headerdirty; }
      // Get the header's update status
    virtual void SetBodyDirty(bool newdirty) { entry->bodydirty = newdirty; }
      // Set the body's update status
    virtual bool GetBodyDirty() const { return entry->bodydirty; }
      // Get the body's update status
    virtual const uint8_t *GetWalkMap(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            ((uint8_t *)entry->header->states[state].walkmap.ptr()) : nullptr; }
      // Returns the walk map for the item
    virtual int32_t GetRegX(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].regx) : (0); }
      // Gets the screen registration point for the state
    virtual int32_t GetRegY(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].regy) : (0); }
      // Gets the screen registration point for the state
    virtual int32_t GetRegZ(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].regz) : (0); }
      // Gets the screen registration point for the state
    virtual void SetReg(int32_t state, int32_t regx, int32_t regy, int32_t regz)
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
        {entry->header->states[state].regx = regx;
         entry->header->states[state].regy = regy;
         entry->header->states[state].regz = regz;
         entry->headerdirty = true; } }
      // Sets the registration point for the state
    virtual int32_t GetAnimRegX(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].animregx) : (0); }
      // Gets the screen registration point for the state
    virtual int32_t GetAnimRegY(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].animregy) : (0); }
      // Gets the screen registration point for the state
    virtual int32_t GetAnimRegZ(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].animregz) : (0); }
      // Gets the screen registration point for the state
    virtual void SetAnimReg(int32_t state, int32_t regx, int32_t regy, int32_t regz)
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
        {entry->header->states[state].animregx = regx;
         entry->header->states[state].animregy = regy;
         entry->header->states[state].animregz = regz;
         entry->headerdirty = true; } }
      // Sets the registration point for the state
    virtual int32_t GetFlags(int32_t state) const
      { return entry->header->states[state].flags; }
      // Returns imagery state flags.
    virtual const char *GetAniName(int32_t state) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
            return entry->header->states[state].animname;
            return nullptr; }
      // Returns state animation name
    virtual int32_t GetAniFlags(int32_t state) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
            return entry->header->states[state].aniflags;
            return 0; }
      // Returns state animation flags (ping pong, reverse, etc.)
    virtual int32_t GetAniLength(int32_t state) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
            return entry->header->states[state].frames;
            return 0; }
      // Returns length (in frames) of animation for this state
    virtual int32_t GetInvAniFlags(int32_t state) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
            return entry->header->states[state].invaniflags;
            return 0; }
      // Returns inventory animation flags (ping pong, reverse, etc.)
    virtual int32_t GetInvAniLength(int32_t state) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
            return entry->header->states[state].invframes;
            return 0; }
      // Returns length (in frames) of inventory animation for this state
    virtual int32_t GetWidth(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].width) : (0); }
      // Returns the maximum width of states graphics imagery for all frames in screen pixels
    virtual int32_t GetHeight(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].height) : (0); }
      // Returns the maximum height of states graphics screen imagery for all frames    in screen pixels
    virtual void SetWidthHeight(int32_t state, int32_t width, int32_t height)
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
        {entry->header->states[state].width = width;
         entry->header->states[state].height = height;
         entry->headerdirty = true; } }
      // Sets the imagery width and height for a state
    virtual void GetScreenRect(TObjectInstance* oi, SRect &r) const;
      // Returns screen rectangle for object given the values returned by GetWidth(),GetHeight()
    virtual void GetAnimRect(TObjectInstance* oi, SRect &r) const;
      // Returns screen rectangle for object given the values returned by GetWidth(),GetHeight()
    virtual void ResetScreenRect(TObjectInstance* oi, int32_t state = -1, bool frontonly = false);
      // Reset screen rectangle for object for given state, or all states if -1
    virtual void GetWorldBoundBox(int32_t state, int32_t &width, int32_t &length, int32_t &height) const
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
        { width = entry->header->states[state].wwidth;
          length = entry->header->states[state].wlength;
          height = entry->header->states[state].wheight;} }
      // Returns the width, length, and height of the object in 3D world space
    virtual void SetWorldBoundBox(int32_t state, int32_t width, int32_t length, int32_t height);
      // Sets the world bounding box for this state.  The width and length is used for walk map size
    virtual int32_t GetWorldRegX(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].wregx) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual int32_t GetWorldRegY(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].wregy) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual int32_t GetWorldRegZ(int32_t state) const
      { return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
            (entry->header->states[state].wregz) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual void SetWorldReg(int32_t state, int32_t regx, int32_t regy, int32_t regz)
      { if ((uint32_t)state < (uint32_t)entry->header->numstates)
        { entry->header->states[state].wregx = regx;
          entry->header->states[state].wregy = regy;
          entry->header->states[state].wregz = regz;
          entry->headerdirty = true; } }
      // Sets the world registration in world space
    virtual PTBitmap GetStillImage(int32_t state, int32_t num = 0) const { return nullptr; }
        // Get still bitmap, if any
    virtual PTAnimation GetAnimation(int32_t state) const { return nullptr; }
        // Get animation for state
    virtual PTBitmap GetInvImage(int32_t state, int32_t num = 0) const { return nullptr; }
        // Returns the inventory image for the item
    virtual PTAnimation GetInvAnimation(int32_t state) const { return nullptr; }
        // Get inventory animation for state
    virtual uint32_t GetImageFlags(int32_t state) const { return 0; }
        // Gets imagery flags for state
    virtual const char *GetObjName(int32_t obj) const { return nullptr; }
        // Get the name of an object within the 3d mesh
    virtual int32_t GetWeaponObj() const { return -1; }
        // Get the object number for the character's weapon
    virtual int32_t GetNumObjs() const { return 0; }
        // Get the number of objects in the mesh

  protected:
    SImageryEntry*          entry;                      // Pointer to entry in entry array

  private:
    int32_t                 imageryid;                  // Index into imagery entry array
};

// *******************
// * TObjectAnimator *
// *******************

// TObjectAnimator is a TObjectComponent: stored on the owning instance's
// component array (one slot, looked up via GetComponent<TObjectAnimator>()).
// The owning instance drives Pulse() / Animate() explicitly from its own
// per-frame hooks so ordering relative to other instance work is preserved
// -- so the animator does NOT auto-register with the global update list.
//
// In Pass A of the animator-component move this class still owns Pulse() /
// Animate(bool draw) / state-mirror data; the legacy subclasses (T3DAnimator,
// TCharAnimator, TAnimAnimator, the effect animators) keep their existing
// shape on top. Pass B will collapse it down to transition logic only --
// the renderer's drawable system already does pose sampling, blending and
// matrix construction off (state, frame), so most of what the legacy
// subclasses do here is dead.
class TObjectAnimator : public TObjectComponent
{
  protected:
    TObjectInstance *inst;      // Instance associated with animator
    TObjectImagery  *image;     // Imagery for animation
    int32_t prevstate;          // Previous state (set when state changes, or animation loops)
    int32_t state;              // Current state (checks against object every frame)
    int32_t frame;              // Current anim frame
    int32_t framerate;          // Direction of animation
    bool newstate;              // True if new state has just been entered
    bool complete;              // When a (non-looping) animation has exhausted its frames

  public:
    TObjectAnimator(TObjectInstance* oi);
      // Constructor. Sets objectimagery. and object instance to nullptr.
    ~TObjectAnimator() override;
      // Destructor.

    [[nodiscard]] const char* ComponentName() const override { return "animator"; }

    TObjectInstance* GetObjInst() { return inst; }
      // Returns the object instance for this animator
    TObjectImagery* GetImagery() { return image; }
      // Returns the animators imagery

    virtual void Initialize() {}
      // Initializes the animator for use
    virtual void Close() {}
      // Frees all allocated memory, etc.

    virtual void ResetState();
      // Restart the current state
    virtual void SetComplete(bool comp) { complete = comp; }
      // The animation is complete (set to false when ResetState() caled and true when anim is done)
    virtual void SetNewState(bool newst) { newstate = newst; }
      // The this is a new state (set to true when ResetState() is called, and false after first frame)
    virtual void Pulse() {}
      // Called during each timer Pulse() to update animaton.
    virtual void Animate(bool draw);
      // Animates object

    int32_t GetFrame() { return frame; }
      // Gets the current animation frame
    int32_t GetFrameRate() { return framerate; }
      // Returns the direction and rate of animation

    void ClearPrevState() { state = prevstate = -1; }
      // Clears previous state (prevents interpolation)
};
