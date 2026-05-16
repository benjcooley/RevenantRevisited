// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     mappane.h - TMapPane object                       *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "lightdef.h"
#include "multisurface.h"
#include "screen.h"
#include "sector.h"

// Grid snap
#define GRIDSHIFT   4
#define GRIDSIZE    (1 << GRIDSHIFT)
#define GRIDMASK    (0xFFFFFFFF - (GRIDSIZE - 1))

// Cursor modes
#define MODE_TARGET     0
#define MODE_CLONE      1
#define MODE_MOVE       2

#define NUMMODES        3

// ****************
// * TMapIterator *
// ****************

#define CHECK_NONE      (0)             // Don't bother checking flags, rects, etc.
#define CHECK_SECTRECT  (1 << 0)        // check sector rectangle
#define CHECK_SCRRECT   (1 << 1)        // check screen rectangle for intersection
#define CHECK_RECT      (CHECK_SECTRECT | CHECK_SCRRECT)
#define CHECK_LIGHT     (1 << 2)        // check light rectangle for intersection
#define CHECK_INVIS     (1 << 3)        // check OF_INVISIBLE
#define CHECK_MOVING    (1 << 4)        // check OF_MOVING
#define CHECK_NOINVENT  (1 << 5)        // no inventory recurse
#define CHECK_MAPRECT   (1 << 6)        // check screen rectangle for intersection

// This is the standard local range for an object when iterating through its neighbors
// Using this range prevents object searches from becoming too big when large numbers of
// sectors are loaded up during multiplayer, or MUD game modes since each player/object/etc.
// only looks within its local area of the map for its own stuff.
#define LOCALRANGE 1024

_CLASSDEF(TMapIterator)
class TMapIterator
{
  public:
    TMapIterator(PSRect sr = nullptr, int32_t fl = CHECK_NONE, int32_t objset = OBJSET_ALL)
      { Initialize(sr, fl, objset); }
      // Calls initialize function
    TMapIterator(const S3DPoint& pos, int32_t range = LOCALRANGE, int32_t fl = CHECK_NOINVENT | CHECK_MAPRECT, int32_t objset = OBJSET_ALL);
      // Initializes iterator with a range value
    TMapIterator(RTObjectInstance oi, int32_t fl = CHECK_NONE, int32_t objset = OBJSET_ALL);
      // Initializes iterator with a range value
    void Initialize(PSRect sr = nullptr, int32_t fl = CHECK_NONE, int32_t objset = OBJSET_ALL);
      // Initializes iterator
    TObjectInstance* Item() { return item; }
      // Returns current item.
    TObjectInstance* NextItem();
      // Advance to the next item, and return it (nullptr if end of list)
    bool operator ++ (int32_t) { return (NextItem() != nullptr); }
      // Allows moving forwards through array in sequential order
    //bool operator -- (int32_t) { return PrevItem(); }
      // Allows moving backwards through array in sequential order
//  operator bool () { return (item != nullptr); }
      // Returns false if at end of item list
    operator TObjectInstance* () { return item; }
      // Typecast operator for object
    TObjectInstance* operator -> () { return item; }
      // Allows iterator to be used as pointer
    RTObjectInstance operator * () { return *item; }
      // Dereferencing operator

    int32_t SectorX() const { return sx; }
    int32_t SectorY() const { return sy; }
    int32_t SectorIndex() const { return index; }
    TObjectInstance* Parent() const { return parent; }
    int32_t InventoryIndex() const { if (parent) return invindex - 1; return invindex; }

    void Nuke();
        // Delete this object

  protected:
    int32_t objset;                 // Object set to iterate through
    int32_t index;                  // Object index (in set)
    TObjectInstance* item;      // Current object
    int32_t sx, sy;                 // sector x and y
    TSector* sector;            // Current sector
    int32_t numitems;               // Number of items in current sector set
    TObjectInstance* parent;    // parent object, or nullptr when in map pane
    int32_t invindex;               // index into parent object's inventory

    int32_t flags;                  // flags for which objects are valid
    SRect r;                    // screen rectangle for CHECK_RECT and CHECK_LIGHT
};

// ********************************
// * TMapPane - Map gameplay pane *
// ********************************

// The TMapPane object implements the display, scrolling, animation, and game
// control for the game map.  This includes loading and saving sectors, adding,
// removing, moving, and using objects, animating objects, and scrolling.

#define MAXMAPBGRECTS 64

_STRUCTDEF(SBgUpdateRect)
struct SBgUpdateRect
{
    SRect rect;
    int32_t bgdraw;
};

#define CENTERON_OBJ    1
#define CENTERON_POS    2
#define CENTERON_SCROLL 4

_STRUCTDEF(SCenterOnState)
struct SCenterOnState
{
    int32_t flags;
    TObjectInstance* obj;
    S3DPoint pos;
    int32_t level;
};

_CLASSDEF(TMapPane)
class TMapPane : public TPane
{
    friend class TMapIterator;

  // Function Members
  public:

  // Initialize and close the pane
    virtual bool Initialize();
        // Initializes pane and prepares to load map resources
    virtual void Close();
        // Releases pane data

  // Virtual handler functions  
    virtual void KeyPress(int32_t key, bool down);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);
    virtual void Update() { TPane::Update(); RedrawAll(); }

    virtual void CreateBackgroundBuffers();
        // Set up main background buffer and map pane surfaces
    virtual void FreeBackgroundBuffers();
        // Delete background buffer and previously allocated video surfaces

    void RedrawAll();   
      // Redraws entire screen

    void MoveObjScreen(int32_t index, int32_t x, int32_t y);
        // Object drag function for mouse
    void GetMouseMapPos(S3DPoint &p, int32_t zoffset = 50);
      // Gets the world position (given character's z pos) that mouse is hovering over
    int32_t GetMouseMapAngle();
      // Gets the angle of the mouse from the character's current position
    void UpdateMouseMovement(int32_t x, int32_t y);
      // Update moving Player around

  // Map management functions
    void LoadCurMap(char *from = nullptr);
      // Loads the current map in the "curmap" directory from the given directory 
      // (i.e. "savegame.001"), or clears the "curmap" directory if nullptr, forces
      // reload of all sectors.
    void SaveCurMap(char *to = nullptr);
      // Saves map in curmap to the given game subdirectory 
      //(i.e. "savegame.001") or "map" if null
    void ClearCurMap();
      // Deletes all files in the "curmap" directory, and forces sectors to reload.

  // Lighting functions
    void DrawDLight();
      // Called by DrawBackground() to draw the dynamic light to the display
    void SetDLightIntensity(uint8_t newintensity) 
      { dlight.intensity = dlight.color.red = dlight.color.green = dlight.color.blue = newintensity; }
      // Sets the dynamic light intensity
    uint8_t GetDLightIntensity() { return dlight.intensity; }
      // Gets the dynamic light intensity
    void SetDLightPos(S3DPoint& newpos) { dlight.pos = newpos; }
      // Sets light relative position
    S3DPoint& GetDLightPos(S3DPoint& newpos) { return dlight.pos; }
      // Sets light relative position
    void SetAmbientLight(int32_t light, bool stopfade = true);
      // Sets ambient light intensity.  If fading, sets target to new value unless stopfade
      // is true, in which case the value is set immediately.
    int32_t GetAmbientLight() { return ambient; }
      // Gets the ambient light intensity 
    void SetAmbientColor(SColor &color, bool stopfade = true);
      // Sets ambient light color.  If fading, sets target to new value unless stopfade
      // is true, in which case the value is set immediately.
    SColor &GetAmbientColor() { return ambcolor; }
      // Gets the ambient light color
    void SetAmbient(int32_t light, SColor &color, bool stopfade = true)
      { SetAmbientLight(light, stopfade); SetAmbientColor(color, stopfade); }
      // Sets total ambient environment in one shot
    void FadeAmbient(int32_t light, SColor &color, int32_t frames, int32_t steps);
      // Fades to the given ambient light in 'frames' frames with
      // 'steps' number of discreet steps.
    void UpdateLights();
      // Set up baselights

  // Background drawing functions
    virtual void DrawBackground();
      // TScreen virtual function which handles drawing to background.. root function for all
      // the crap below...
    void AddBgUpdateRect(SRect &r, int32_t bgdraw = BGDRAW_UNLIT);
      // Adds a background update rectangle to be added to the update rect queue
    void AddObjectUpdateRect(int32_t index);
      // User function called to add a background update rectangle for the given object
    
    void BeginUpdateThread();
      // Called by Initialize function to begin update thread
    void EndUpdateThread();
      // Called by Close function to end update thread
    int32_t GetUpdateSleep();
      // Returns the amount of milliseconds per frame to sleep for update thread
    void UpdateTimeSlice(bool draw);
      // Called by the Animate() function to release time to the update thread
    void AdjustTimeSlice(int32_t milliseconds);
      // Called whenever update has to wait for a queued rect
    void UpdateEdges(int32_t updatex, int32_t updatey);
      // Called by DrawBackground() to figure out which rectangles to send to the Draw
      // DrawBackgroundUpdates edges of scrollbuffer
    void QueueUpdateRect(RSRect rect, int32_t bgdraw);
      // Adds the update rect to the update rect queue... Call BeginUpdate() to start drawing...
      // (This func will wait for last queue to finish if previous queue is still drawing)
    void PutQueueRectsToDisplay();
      // Called by UpdateEdges() function to cause rects to draw to screen after they're
      // rendered by the update thread
    void ClearQueueRects();
      // Waits till current render is done, then clears out rects in the queue.
    void BeginUpdate();
      // After all rectangles have been added to queue, will start the queue drawing
    bool UpdateDone();
      // Returns true if the last queued set of update rectangles is done
    void CancelUpdate();
      // Cancels last queue of update rectangles.
    uint32_t WaitUpdate();
      // Waits for last queue of update rectangles to finish
    void PauseUpdate(const char *file, int32_t line);
      // Call to pause the update thread
    void ResumeUpdate();
      // Call to resume the update thread
    void FlushUpdate(bool updatetimeslice = false);
      // Call to flush the current update queue, put rects to screen, and update current
      // valid scroll rect from the queued rect.
    void LockSectors(const char *file, int32_t line);
      // Prevents update thread from accessing sector arrays.  Call when changing sector arrays.
    void UnlockSectors();
      // Unlocks update system access to sector arrays
    void LockObjects(const char *file, int32_t line);
      // Prevents update thread code from accessing object data. Call when changing object states or imagery
    void UnlockObjects();
      // Unlock objects

   // Update buffering functions
    void DrawUpdateRect(RSRect dr, int32_t bgdraw);
      // Draws an update rectangle using the ordinary tile draw system

    static unsigned UpdateThread(void *);
      // Loader thread (calls the loader functions below
    void UpdateLoop();
      // Main update thread loop
    void GetUpdateObjs(SRect &r);
      // Go's through sector list and gets all objects to draw (CALLED ONLY BY UPDATE THREAD!!)
    void DrawUnlitObjects(SRect &r);
      // Draws object to the unlit buffer (before lighting) (CALLED ONLY BY UPDATE THREAD!!)
    void DrawStaticLights(SRect &r);
      // Draws static lights and copies info to lit buffer (CALLED ONLY BY UPDATE THREAD!!)
    void TransferUnlitToLit(SRect &r);
      // Does conversion of unlit buffer with lighting to the lit buffer
    void DrawLitObjects(SRect &r);
      // Draws objects to the lit buffer (CALLED ONLY BY UPDATE THREAD!!)
    void DrawSelectedObjects(SRect &r);
      // Draws selected objects to the lit buffer (CALLED ONLY BY UPDATE THREAD!!)

    virtual void Pulse();
      // Called by screen to pulse system (called before DrawBackground())
    void MoveObjects();
      // Causes all objects to move (based on their moveanle and nextmove members)
    void PulseObjects();
      // Sends animation pulse to all objects with OF_PULSE set
    virtual void Animate(bool draw);
      // Does animations for all objects with OF_ANIMATE set
    void AnimateObjects(bool draw);
      // Does foreground/2D animations (calls Animate() function for objects on screen)
    void NextFrameObjects();
      // Advances all animated objects (regardless of whether they are on screen)
      // to the next frame of thier animation.  This allows offscreen objects to move,
      // and animations to show the correct passage of time when off screen.
    void AnimateBackground();
      // Does background animations
    void AnimateSelectedObjects();
      // Draws selected objects that are moving (usually being dragged)

  // Sector Functions
    void UpdateSectors();
        // Reload sectors based on pane x, y position

    // Populate the active window (sectors[SECTORWINDOWX][SECTORWINDOWY])
    // from MapManager.CurrentMap, centered on the Player's sector. The
    // active window is what TMapIterator walks for per-frame
    // PulseObjects / MoveObjects / NextFrameObjects -- so anything
    // inside it ticks; anything outside is idle this frame. Cheap when
    // the player hasn't crossed a sector boundary (no-op).
    void UpdateActiveWindow();
    void SaveAllSectors();
        // Save all sectors to disk without deallocating
    void FreeAllSectors();
        // Save all sectors to disk and then deallocate them
    void ReloadSectors();
        // Free and then reload all sectors

  // World position functions
    void SetMapPos(S3DPoint& newpos);
    void GetMapPos(S3DPoint& retpos) { retpos = center; }
    void SetMapLevel(int32_t newlev) { newlevel = newlev; if (newlev != level) RedrawAll(); }
    int32_t GetMapLevel() { return level; }

  // Center On Functions (causes game map to center on point or object)
    void CenterOnObj(TObjectInstance* obj, bool scroll)
        { centeron.obj = obj; centeron.flags = CENTERON_OBJ | (scroll ? CENTERON_SCROLL:0); }
      // Center on object
    TObjectInstance* GetCenterOnObj() { return centeron.obj; }
      // Gets centered object
    void CenterOnPos(S3DPoint& pos, int32_t level, bool scroll)
        { centeron.pos = pos; centeron.level = level; centeron.flags = CENTERON_POS | (scroll ? CENTERON_SCROLL:0); }
      // Center on a point
    void GetCenterOnPos(S3DPoint& pos, int32_t &level) { pos = centeron.pos; level = centeron.level; }
      // Gets current center on point
    bool IsScrollCenterOn() { return centeron.flags & CENTERON_SCROLL; }
      // True if we smooth scroll to object or point
    void SaveCenterOnState(PSCenterOnState state)
        { memcpy(state, &centeron, sizeof(SCenterOnState)); }
      // Saves the current center on state to structure
    void RestoreCenterOnState(PSCenterOnState state)
        { memcpy(&centeron, state, sizeof(SCenterOnState)); }
      // Restores the current center on state from structure

  // Scroll pos
    int32_t PosX() { return posx; }
    int32_t PosY() { return posy; }

  // Add/Remove objects
    int32_t MakeIndex();
      // Makes a unique id map index for an object
    int32_t NewObject(SObjectDef* def);
      // Create a new object with the given objdef
    int32_t AddObject(TObjectInstance* oi);
      // Adds the given object to the map and returns unique id
    void ObjectFlagsChanged(TObjectInstance* oi, uint32_t oldflags, uint32_t newflags);
      // Allows map to update lists, etc. when an objects flags change (mainly
      // for OF_LIGHT, OF_PULSE, and OF_ANIMATE changing objects location on 
      // OBJSET_xxx sector arrays.
    TObjectInstance* RemoveObject(int32_t index);
      // Removes the given object from the map, recursing through inventories
    TObjectInstance* RemoveObject(TObjectInstance* inst)
        { return RemoveObject(inst->GetMapIndex()); }
      // Removes the given object from the map, recursing through inventories
    void DeleteObject(TObjectInstance* obj);
      // Removes and deletes an object
    TObjectInstance* RemoveFromSector(TObjectInstance* inst, int32_t sx, int32_t sy, int32_t sectindex);
      // Removes the given object from the sector array (called by RemoveObject())
    void DeleteSector(TSector* sect);
      // Removes and deletes a sector (hey.. don't call this)
    void ReloadImagery();
      // Forces imagery system to reload imagery.
    int32_t AddShadow(TObjectInstance* oi);
      // Add shadow for object, if there is one

  // Object find functions
  // ---------------------
  // NOTE: Use the 'objset' variable to greatly reduce search times for common sets of
  // objects like MOVING objects and CHARACTERS. 
  
    TObjectInstance* FindObject(char *name, int32_t occurance = 1, int32_t objset = OBJSET_ALL);
        // Returns a pointer to the occurance of object answering to name
    TObjectInstance* FindClosestObject(char *name, S3DPoint pos, bool partial, int32_t objset = OBJSET_ALL);
        // Returns a pointer to the object answering to name closest to center
    TObjectInstance* FindClosestObject(char *name, TObjectInstance* from = nullptr, bool partial = false, int32_t objset = OBJSET_ALL);
        // Returns a pointer to the object answering to name closest to center
    int32_t FindObjectsInRange(S3DPoint pos, int32_t *array, int32_t width, int32_t height = 0, int32_t objclass = -1, int32_t maxnum = MAXFOUNDOBJS, int32_t objset = OBJSET_ALL);
      // Finds objects within given range. If height not given uses width as radius
    TObjectInstance* ObjectInCube(PS3DRect cube, int32_t objset = OBJSET_ALL);
        // Returns pointer to object in cube
    TObjectInstance* OnObject(int32_t screenx, int32_t screeny, TObjectInstance* with = nullptr);
        // Returns the index of the object the mouse is on
    TObjectInstance* GetInstance(int32_t index, int32_t objset = OBJSET_ALL);
        // Returns the instance given an object index. Backed by an
        // unordered_map keyed on mapindex (O(1) hit, falls back to a linear
        // TMapIterator scan if the registry misses). The registry is kept
        // in sync by TObjectInstance::SetMapIndex and the dtor.

    void RegisterInstance(TObjectInstance* oi, int32_t index);
    void UnregisterInstance(int32_t index);
        // Registry maintenance — only the TObjectInstance lifecycle should
        // call these; nothing else needs to touch the table.
    void FindClickPos(int32_t x, int32_t y, S3DPoint& start, S3DPoint& target);
        // Find the position clicked on, taking into account the walkmap heights
    bool SwapDrawOrder(TObjectInstance* inst0, TObjectInstance* inst1);
        // Exchange the sector positions of the two objects
    void PushToFront(TObjectInstance* inst);
        // Push object to the top of the sector
    void PushToBack(TObjectInstance* inst);
        // Push object to the bottom of the sector

  // Walk map functions
    void TransferWalkmap(TObjectInstance* inst) { WalkmapHandler(inst, WALK_TRANSFER); }
        // Transfer object's walkmap to the sector walkmap
    bool SaveWalkmap(TObjectInstance* oi)
        { WalkmapHandler(oi, WALK_CAPTURE); oi->SaveHeader(); return true; }
        // Write walkmap for object to disk (editor use only)
    void ClearWalkmap(TObjectInstance* oi) { WalkmapHandler(oi, WALK_CLEAR); }
        // Clear out area of walkmap that object resides in
    void ExtractWalkmap(TObjectInstance* inst) { WalkmapHandler(inst, WALK_EXTRACT); }
        // Remove object's walkmap from the sector walkmap (by redrawing its rect)
    int32_t GetWalkHeight(S3DPoint& pos);
        // Gets walk height for a given walk map
    int32_t GetWalkGridHeight(int32_t x, int32_t y);
        // Gets walk height given walk grid x and y
    int32_t GetWalkHeightArea(S3DPoint& pos, int32_t width = 0, int32_t height = 0);
        // Gets walk height for a given walk map CENTERED on pos with given with and height
        // If width == 0, pos is treated as a point
        // Otherwise pos, width, and height define the box of walk area CENTERED on pos
    void GetWalkHeightRadius(S3DPoint& pos, int32_t radius, 
        int32_t &mindelta, int32_t &maxdelta, int32_t &curheight);
        // Returns the minimum delta (fall), and maximum delta (rise) between any two walk grids 
        // within the radius, and also the height at the current position.
        // The mindelta value detects drops in the heightmap within the radius. (holes, downstairs)
        // The maxdelta value detects rises in the heightmap (walls, columns, obstructions)
        // The 'curheight' value gives the height under 'pos'
        // Note that the 'deltas' only indicate differences in hieght between adjacent walk
        // grids, so 'radius' may be scaled arbirarily large and still work correctly.
        // This routine is somewhat processor intensive, so only use for moving chars and objs.
    bool LineOfSight(S3DPoint& pos, S3DPoint& to, S3DPoint* obst = nullptr);
        // Returns line of sight flags for line between positions
    void CalculateWalkmap();
        // Call to recalculate the walkmap for the current sector
    void AdjustWalkmap(int32_t deltaz, bool absolute = false, bool nonzero = false);
        // Move the entire walkmap up or down based on deltaz
    void TransferAllWalkmaps();
        // Re-transfers all the walkmaps (does not clear walkmap first)
    void ClearWalkmaps();
        // Clears all sector walkmaps
    void RefreshWalkmaps() { ClearWalkmaps(); TransferAllWalkmaps(); }
        // Refreshes all sector walkmaps (clears, then transfers walkmaps)
    void SnapWalkDisplay(int32_t mapindex);
        // Snap the walk display to the given object (editor only)

  // Object manipulation functions
    void MoveObject(int32_t index, S3DPoint& newpos);
    void SetFlags(int32_t index, uint16_t newflags);
    void SetState(int32_t index, uint8_t newstate);
    void Damage(int32_t intdex, int32_t damage);
    void Throw(int32_t index, S3DPoint& velocity);
    void RepaintObject(int32_t index);

    int32_t MoneyHandler(TObjectInstance* oi, int32_t amount);
        // General purpose money handler, called by the two functions below
    int32_t SubtractMoney(TObjectInstance* oi, int32_t amount);
        // Take away the amount of money specified from oi's inventory
        // If they don't have enough, do nothing and return 0
    int32_t GetTotalMoney(TObjectInstance* oi);
        // Return the total about of money owned by the object

    int32_t CheckPos(TObjectInstance* inst, const S3DPoint& newpos, int32_t newlevel = -1);
        // Call before moving inst to newpos - returns index
    int32_t TransferObject(TObjectInstance* inst, int32_t sx, int32_t sy, int32_t newsx, int32_t newsy);
        // Transfer an object to a new sector, returns new itemnum
    bool Use(int32_t index, int32_t with);
    int32_t Face(int32_t index, int32_t newfacing);

    bool Clicked() { return clicked; }
        // Return whether mouse is clicked down in the mappane or not

  // Modes for editor
    void SetMode(void (*tfunc)(S3DPoint), void (*afunc)(), int32_t mode);
    void ResetDrag() { objx = objy = grabx = graby = onobject = -1; }
    void SetWalkmapReveal(int32_t nx, int32_t ny, int32_t nsx, int32_t nsy)
        { wmrevealx = nx; wmrevealy = ny; wmrevealsizex = nsx; wmrevealsizey = nsy; }

    uint32_t GetNotify() { return notifyflags; }
    void SetNotify( uint32_t newflags) { notifyflags |= newflags; }
    void Notify(uint32_t notify, void *ptr);

  private:

  // Private functions
    void UpdateMapPos();
        // Called by drawbackground to update the map pane position (Does CenterOn stuff)
        // calls SetMapPos())
    void Update3DScenePos();
        // Called by drawbackground to update the 3d scene position

    void WalkmapHandler(TObjectInstance* inst, int32_t mode);
        // Handles clipping of inst's walkmap to various sectors it may reside in
    void RedrawWalkmapRect(TObjectInstance* oi, int32_t x, int32_t y, int32_t w, int32_t l, TSector* dsect);
        // Redraws the given wm rect - must already be clipped to sector
    void DrawWalkMap(SRect &r);
        // Draws lit walmap crap
    
    void PulseFadeAmbient();
        // Called by Pulse() function to update ambient fade values
  
  // Data Members
    TSector* sectors[SECTORWINDOWX][SECTORWINDOWY]; // Currently loaded sectors
    int32_t oldsectorx, oldsectory;                 // Position of sector in last frame
    int32_t sectorx, sectory;                       // Position of sector in current frame
    int32_t newsectorx, newsectory;                 // Position of sector in next frame
    int32_t oldlevel, oldposx, oldposy;             // Position of screen in last frame
    int32_t level, posx, posy;                      // Current position (this frame)
    int32_t newlevel;                               // New position in map (next frame)
    int32_t oldscrollx, oldscrolly;                 // Scroll position of previous frame
    int32_t scrollx, scrolly;                       // Scroll position of current frame
    S3DPoint center;                                // World coordinates of pane center (current pos)
    SCenterOnState centeron;                        // Pane will attempt to scroll to this object or point
    int32_t onobject;                               // Object clicked on
    int32_t grabx, graby;                           // Click pos
    int32_t objx, objy;                             // Offset on the object
    int32_t oldz;                                   // For z-dragging
    int32_t mx, my;                                 // Mouse scrolling
    bool clicked;                                   // To allow movement outside of pane
    uint32_t notifyflags;                           // Notify Objects of changes
    int32_t lastkey;                                // Last virtual keycode generated by mouseclicks

  // Update system members...
    int32_t numbgrects;                             // Number of background update rectangles
    SBgUpdateRect bgrects[MAXMAPBGRECTS];           // Background update rectangles
    bool isrecqueued;                               // True if update system has a rectangle in queue
    SRect queuedrect;                               // Current rectangle in queue
    SRect scrollrect;                               // Currently valid (drawn) scroll area

    PTMosaicSurface unlitmulti;                     // 32 bit unlit buffer
    PTMosaicSurface litmulti;                       // 16 bit lit buffer (set to mosaic tile)
    PTMosaicSurface updatemulti;                    // Clone of 16 bit surface for updating
    bool zbufferiscloned;                           // true if zbuffer of unlitmulti is clone of litmulti

    SLightDef dlight;

    void (*targetcallback)(S3DPoint pos);           // Callback target function
    void (*abortcallback)();                        // Callback on abort
    bool dragmode;                                  // Drags objects without clicking
    int32_t mode;                                   // Mode number

    int32_t lightdist[MAXLIGHTS];                   // Array of nearest light distances
    int32_t lightindex[MAXLIGHTS];                  // Array of nearest light indexes

    int32_t wmrevealx, wmrevealy;                   // Start of walkmap cutaway
    int32_t wmrevealsizex, wmrevealsizey;           // Size of walkmap cutaway

  // Ambient lighting values. Defaults match TArea ctor (area.cpp:35-38)
  // so even uncovered levels (no area.def entry hitting the player's
  // current level) get a usable retail-default tint instead of black.
    int32_t ambient = 30;                           // Ambient light value
    SColor ambcolor = {255, 255, 255};              // Ambient light color
    bool ambientchanged = true;                     // true when ambient needs to be reset
    int32_t oldambient = 30, newambient = 30;       // Ambient fade old and new values
    SColor oldambcolor = {255, 255, 255};
    SColor newambcolor = {255, 255, 255};           // Ambient fade target color
    int32_t totambfadeframes = 0, ambfadeframes = 0;
    int32_t totambfadesteps  = 0, ambfadesteps  = 0;
};

inline void PosToScreen(int32_t posx, int32_t posy, int32_t posz, int32_t &screenx, int32_t &screeny)
{
    screenx = posx;
    screeny = (posy >> 1) - posz;
}

inline void ScreenToPos(int32_t screenx, int32_t screeny, int32_t &posx, int32_t &posy, int32_t posz)
{
    posx = screenx;
    posy = (screeny + posz) * 2;
}

// Converts a map pos to a walk grid pos
void PosToWalkGrid(S3DPoint& pos, int32_t &x, int32_t &y);

// Converts a walk grid pos to a map pos
void WalkGridToPos(int32_t x, int32_t y, S3DPoint& pos);

#define PAUSEUPDATE     MapPane.PauseUpdate(__FILE__, __LINE__)
#define RESUMEUPDATE    MapPane.ResumeUpdate()

#define LOCKOBJECTS     MapPane.LockObjects(__FILE__, __LINE__)
#define UNLOCKOBJECTS   MapPane.UnlockObjects()

#define LOCKSECTORS     MapPane.LockSectors(__FILE__, __LINE__)
#define UNLOCKSECTORS   MapPane.UnlockSectors()

