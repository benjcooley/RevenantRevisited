// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 3dimagebody.h - S3dImageryBody object                 *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "imageres.h"

// To-do list for 3D imagery:
// - Alpha channel textures
// - Morph animations
// - Texture states 
// - Object replacement (for weaponry mainly)
// - Dynamicly set size etc of various objects (spell effects)

// ******************
// * S3DImageryBody *
// ******************

// Stores the loadable data for a 3d image resource.  This data is
// copied into internal mesh structures in the imagery object, then
// discarded.

// Maximimums: Note: these maximums are only used in the exporter, and the Textures variables
// for the 3DAnimObj structure.  All other 3D system arrays are unlimited.
// NOTE: EVENTUALLY ALL SYSTEM ARRAYS WILL BE UNLIMITED!

#define MAXOBJECTS          64
#define MAXMATERIALS        8
#define MAXTEXTURES         8
#define MAXTAGS             128

struct S3DMaterial;

typedef TOffset<hmm_vec3> OVEC3;
typedef TOffset<S3DMaterial> OMATERIAL;

typedef TOffset<hmm_vec3> OVEC3;      // Offset to an array of vertices
typedef TOffset<OVEC3> OOVEC3;        // Offset to an array of offsets to array of vertices (Frame array) 
typedef TOffset<OOVEC3> OOOVEC3;      // Offset to an array of offsets to array of offsets to vertex array (State array)

_STRUCTDEF(S3DFace)
struct S3DFace
{
    uint16_t v1, v2, v3;
};

// Animation keys...
// All keys are 32 bit, and can contain either 10 bit x,y,z information, or 24 bit
// fixed point individual values.  Ordinarily, the first frame will have the 24 bit
// values, and the remaining frames will have the 10 bit values.  Next frame is 
// implied when a key repeats, or a SKIP is encountered.  Keys are garanteed to 
// repeat for next frames (i.e no POS frame1, then SCL frame 2, then ROT frame 3...
// will have (POS frame 1, POS frame2 SCL frame2, SCL frame3 ROT frame 3)..

#define SMALLKEYBITS        10                      // Number of bits for small key value
#define SMALLKEYMAX         ((1 << (SMALLKEYBITS - 1)) - 1)
#define SMALLKEYMIN         (-(1 << (SMALLKEYBITS - 1)))

#define ANIKEY32_POSSCALE   (4.0)                   // Object pos's must be in range 511 to -512
#define MAXPOSKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_POSSCALE)
#define MINPOSKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_POSSCALE)

#define ANIKEY32_ROTSCALE   ((float)(1 << (SMALLKEYBITS - 1)) / 3.1415965359f) // Angles must be in range -PI to PI (511 to -512)
#define MAXROTKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_ROTSCALE)
#define MINROTKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_ROTSCALE)

#define ANIKEY32_SCLSCALE   (64.0)                  // Scales are in range of 7.0 to -8.0
#define MAXSCLKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_SCLSCALE)
#define MINSCLKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_SCLSCALE)

#define ANIKEY32_CODESCALE  (256.0)                 // Codes in range 65535 to -65536 with 8 bit fixed

// Compressed format is very small..
#define ANIFLAG32_CODE      0       // Key uses code format
#define ANIFLAG32_POS       1       // Key is 10 bit x,y,z pos
#define ANIFLAG32_ROT       2       // Key is 10 bit x,y,z rot
#define ANIFLAG32_SCL       3       // Key is 10 bit x,y,z scl

// Code format trades space for precision... First frame ususally is code.
#define ANICODE32_NEXTFRAME 0
#define ANICODE32_SKIP      1
#define ANICODE32_POSX      2
#define ANICODE32_POSY      3
#define ANICODE32_POSZ      4
#define ANICODE32_ROTX      5
#define ANICODE32_ROTY      6
#define ANICODE32_ROTZ      7
#define ANICODE32_SCLX      8
#define ANICODE32_SCLY      9
#define ANICODE32_SCLZ      10

_STRUCTDEF(SAniKey32)
struct SAniKey32
{
    union
    {
      struct {
        uint32_t flags : 2; // Indicates what kind of structure this is
        int32_t x : 10;
        int32_t y : 10;
        int32_t z : 10;
      };
      struct {
        uint32_t dummy : 2; // Flags dummy
        uint32_t code : 6;
        int32_t value : 24;
      };
    };
};

// Old animation key stuff

#define ANIKEY_POSSCALE  (4.0)                      // Object pos's must be in range 0-512
#define ANIKEY_ANGSCALE  (256.0 / 3.14159265359)    // Angles must be in range -PI to PI

_STRUCTDEF(SAniKey)
struct SAniKey
{
    union
    {
        int32_t data[2];
        struct
        {
            unsigned int x : 12;    // Use macros below!!
            unsigned int y : 12;
            unsigned int z : 12;
            unsigned int rx : 9;
            unsigned int ry : 9;
            unsigned int rz : 9;
            unsigned int dummy : 1; // So equals 64 bits
        };
    };
};

#define ak_x(ak)  ((int32_t)((ak).x) - 0x800)
#define ak_y(ak)  ((int32_t)((ak).y) - 0x800)
#define ak_z(ak)  ((int32_t)((ak).z) - 0x800)
#define ak_rx(ak) ((int32_t)((ak).rx) - 0x100)
#define ak_ry(ak) ((int32_t)((ak).ry) - 0x100)
#define ak_rz(ak) ((int32_t)((ak).rz) - 0x100)

#define ak_setx(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFFFFF000) | ((((int32_t)(d) + 0x800) & 0xFFF) << 0))
#define ak_sety(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFF000FFF) | ((((int32_t)(d) + 0x800) & 0xFFF) << 12))
#define ak_setz(ak, d)  (ak).data[0] = (((ak).data[0] & 0x00FFFFFF) | ((((int32_t)(d) + 0x800) & 0x0FF) << 24));\
                        (ak).data[1] = (((ak).data[1] & 0xFFFFFFF0) | ((((int32_t)(d) + 0x800) & 0xF00) >> 8))
#define ak_setrx(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFFFE00F) | ((((int32_t)(d) + 0x100) & 0x1FF) << 4))
#define ak_setry(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFC01FFF) | ((((int32_t)(d) + 0x100) & 0x1FF) << 13))
#define ak_setrz(ak, d) (ak).data[1] = (((ak).data[1] & 0x003FFFFF) | ((((int32_t)(d) + 0x100) & 0x1FF) << 22))

// Very old motion data structure without facing angle
_STRUCTDEF(SOldOldMotionData)
struct SOldOldMotionData
{
    unsigned int dist: 24;
    unsigned int ang : 8;
};

// Old motion data structure
_STRUCTDEF(SOldMotionData)
struct SOldMotionData
{
    unsigned int dist : 16;
    unsigned int face : 8;
    unsigned int ang : 8;
};

// New 64 bit full motion data structure (has x,y rotation and vertical motion)
_STRUCTDEF(SMotionData)
struct SMotionData
{
    unsigned int dist : 16;     // Distance forward on x,y plane (regular movement)
    int vert : 16;              // Vertical movement on z axis (flying/jumping)
    unsigned int ang : 8;       // Angle forward on x,y plane (moveangle)
    unsigned int rotx : 8;      // Rotation x for object (rarely used)
    unsigned int roty : 8;      // Rotation y for object (rarely used)
    unsigned int rotz : 8;      // Rotation z for object (face)
};

#define md_dist(md)         ((md).dist << 8)
#define md_vert(md)         ((md).vert << 8)

#define md_setdist(md, d)   ((md).dist = (d) >> 8)
#define md_setvert(md, v)   ((md).data = (v) >> 8)

_STRUCTDEF(SColorKey)
struct SColorKey
{
    uint32_t       colorSpaceLowValue;   // low boundary of color space that is to
                                         // be treated as Color Key, inclusive
    uint32_t       colorSpaceHighValue;  // high boundary of color space that is
                                         // to be treated as Color Key, inclusive
};

_STRUCTDEF(SPixelFormat)
struct SPixelFormat
{
    uint32_t       dwSize;                 // size of structure
    uint32_t       dwFlags;                // pixel format flags
    uint32_t       dwFourCC;               // (FOURCC code)
    union
    {
        uint32_t   dwRGBBitCount;          // how many bits per pixel
        uint32_t   dwYUVBitCount;          // how many bits per pixel
        uint32_t   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        uint32_t   dwAlphaBitDepth;        // how many bits for alpha channels
        uint32_t   dwLuminanceBitCount;    // how many bits per pixel
        uint32_t   dwBumpBitCount;         // how many bits per "buxel", total
        uint32_t   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
                                           // format list and if DDPF_D3DFORMAT is set
    };
    union
    {
        uint32_t   dwRBitMask;             // mask for red bit
        uint32_t   dwYBitMask;             // mask for Y bits
        uint32_t   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        uint32_t   dwLuminanceBitMask;     // mask for luminance bits
        uint32_t   dwBumpDuBitMask;        // mask for bump map U delta bits
        uint32_t   dwOperations;           // DDPF_D3DFORMAT Operations
    };
    union
    {
        uint32_t   dwGBitMask;             // mask for green bits
        uint32_t   dwUBitMask;             // mask for U bits
        uint32_t   dwZBitMask;             // mask for Z bits
        uint32_t   dwBumpDvBitMask;        // mask for bump map V delta bits
        struct
        {
            uint16_t    wFlipMSTypes;      // Multisample methods supported via flip for this D3DFORMAT
            uint16_t    wBltMSTypes;       // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;
    };
    union
    {
        uint32_t   dwBBitMask;             // mask for blue bits
        uint32_t   dwVBitMask;             // mask for V bits
        uint32_t   dwStencilBitMask;       // mask for stencil bits
        uint32_t   dwBumpLuminanceBitMask; // mask for luminance in bump map
    };
    union
    {
        uint32_t   dwRGBAlphaBitMask;      // mask for alpha channel
        uint32_t   dwYUVAlphaBitMask;      // mask for alpha channel
        uint32_t   dwLuminanceAlphaBitMask;// mask for alpha channel
        uint32_t   dwRGBZBitMask;          // mask for Z channel
        uint32_t   dwYUVZBitMask;          // mask for Z channel
    };
};

_STRUCTDEF(SSurfaceDesc)
struct SSurfaceDesc
{
    uint32_t            dize;                   // size of the DDSURFACEDESC structure
    uint32_t            flags;                  // determines what fields are valid
    uint32_t            height;                 // height of surface to be created
    uint32_t            width;                  // width of input surface
    union
    {
        int32_t         pitch;                  // distance to start of next line (return value only)
        uint32_t        linearSize;             // Formless late-allocated optimized surface size
    };
    uint32_t            backBufferCount;        // number of back buffers requested
    union
    {
        uint32_t        mipMapCount;            // number of mip-map levels requested
        uint32_t        zBufferBitDepth;        // depth of Z buffer requested
        uint32_t        refreshRate;            // refresh rate (used when display mode is described)
    };
    uint32_t            alphaBitDepth;          // depth of alpha buffer requested
    uint32_t            reserved;               // reserved
    void*               surface;                // pointer to the associated surface memory
    SColorKey           ddckCKDestOverlay;      // color key for destination overlay use
    SColorKey           ddckCKDestBlt;          // color key for destination blt use
    SColorKey           ddckCKSrcOverlay;       // color key for source overlay use
    SColorKey           ddckCKSrcBlt;           // color key for source blt use
    SPixelFormat        pixelFormat;            // pixel format description of the surface
    uint32_t            ddsCaps;                // direct draw surface capabilities
};

_STRUCTDEF(S3DImageryTexture)
struct S3DImageryTexture
{
    SSurfaceDesc desc;                  // Direct draw surface desc for texture
    OFFSET bits;                        // Offset to array of texture frame offsets 
    OFFSET pals;                        // Texture palette (if not null, 1 per texture)
    int32_t frames;                     // Number of texture frames
};

_STRUCTDEF(S3DImageryObjectTexture)
struct S3DImageryObjectTexture
{
    uint16_t facepos;
    uint16_t facenum;
};

// Previous state with old animation keys
_STRUCTDEF(S3DOldImageryObjectState)
struct S3DOldImageryObjectState
{
    int32_t parent;                 // Contains object hierarchy for state where
                                    // Hierarchy is array of signed byte indexes for each
                                    // obj indicating its parent, -1 means root};
    OSAniKey anikeys;               // Offset to array of SAniKeys for this object's state
                                    // If this is nullptr, object is HIDDEN!
};

_STRUCTDEF(S3DImageryObjectState)
struct S3DImageryObjectState
{
    int32_t parent;                 // Contains object hierarchy for state where
                                    // Hierarchy is array of signed byte indexes for each
                                    // obj indicating its parent, -1 means root};
    int32_t numanikeys;             // Number of keys for this state
    OSAniKey32 anikeys;             // Offset to array of SAniKeys for this object's state
                                    // If this is nullptr, object is HIDDEN!
};

_STRUCTDEF(S3DImageryObject)
struct S3DImageryObject
{
    char name[RESNAMELEN];
    uint16_t material;
    uint16_t vertpos;                   // Position of object verts in vert array
    uint16_t vertnum;                   // Num object verts in vert array
    OS3DImageryObjectTexture textures;  // Offset to array of S3DImageryObjectTextures for
                                        // each texture in imagery
    OS3DImageryObjectState states;      // Offset to array of S3DImageryObjectState for
                                        // each state in imagery
};

_STRUCTDEF(S3DOldImageryState)
struct S3DOldImageryState
{
    int32_t     invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSMotionData motion;                // Offset to motion data for this state
};

_STRUCTDEF(S3DOldImageryState2)
struct S3DOldImageryState2
{
    int32_t     invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32  motion;                 // Offset to motion data for this state
};

_STRUCTDEF(S3DOldImageryState3)
struct S3DOldImageryState3
{
    int32_t     invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32   motion;                // Offset to motion data for this state
    char        begstate[RESNAMELEN];   // Object starts with animation matching end of this state
    char        endstate[RESNAMELEN];   // Object ends with animation matchin start of this state
};

_STRUCTDEF(S3DImageryState)
struct S3DImageryState
{
    int32_t     invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32  motion;                 // Offset to motion data for this state
};

_STRUCTDEF(S3DImageryPlaySound)
struct S3DImageryPlaySound
{
    int32_t state;                      // State to play sound at
    int32_t frame;                      // Frame number to play sounds at
    bool mount;                         // Should we mount this sound when imagery is loaded
    char sounds[SOUNDLISTLEN];          // List of comma delimited sounds to randomly play
};

_STRUCTDEF(S3DImageryTag)
struct S3DImageryTag
{
    int32_t state;                      // State to play sound at
    int32_t frame;                      // Frame number to play sounds at
    OFFSET name;                        // Name of tag
    OFFSET str;                         // String for tag
};

// Version 2 - Added begstate,endstate to state structure
// Version 3 - Removed begstate,endstate and changed playsounds to animation tags
#define VERSION3DIMAGEBODY 3

#define I3D_ISMORPH      1  // If this flag is set, the vertex array will have multiple images
                            // stored sequentially for each object state, otherwise there will
                            // be only 1 vertex image buffer for object state [0] which will be
                            // used for all animations

// The following flags indicate the presence of additional imagery data
#define I3D_HASICONS     2  // True if imagery has icons
#define I3D_HASHIERARCHY 4  // True if imagery has a hierarchy
#define I3D_FACINGMOTION 8  // Has facing motion data (stores hip facing deltas)
#define I3D_3DIMAGEBODY2 16 // Uses 3D image body 2 header format
#define I3D_HASPLAYSOUND 32 // Has playsound structure
#define I3D_ROOTMOTION   64 // Uses root motion and SAniKey motion data
#define I3D_ANIKEY32     128// New compressed animation key format

// *************** OLD 3DImagery Structures ****************

// Old version of icon structure
_STRUCTDEF(S3DStateImagery)
struct S3DStateImagery
{
    int32_t     invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
};

_STRUCTDEF(S3DMaterial)
struct S3DMaterial
{
    uint32_t    size;
    sg_color    diffuse;
    sg_color    ambient;
    sg_color    specular;
    sg_color    emissive;
    float       power;
    uint32_t    hTexture;
    uint32_t    rampSize;
};

// OLD Version of S3DImagery (SUCKS)
#define OLDMAXOBJSTATES         64
#define OLDMAXOBJECTS           64
#define OLDMAXMATERIALS         8
#define OLDMAXTEXTURES          8
_STRUCTDEF(SOld3DImageryBody)
struct SOld3DImageryBody : public SImageryBody
{
    uint32_t flags;                        // If true, verts array points to multple vert images

    int32_t numverts;                       // Verts stored in object order...
    OFFSET verts[OLDMAXOBJSTATES];          // There will be 'frames' number of sequentially stored 

    int32_t numfaces;                       // vert images per state if morph true
    OFFSET faces;                       // Faces stored by texture, then object order

    int32_t nummaterials;                    
    S3DMaterial material[OLDMAXMATERIALS];

    int32_t numtextures;
    SSurfaceDesc texturedesc[OLDMAXTEXTURES];
    OFFSET texturebits[OLDMAXTEXTURES]; // Offset to array of texture frame offsets 
    OFFSET texturepals[OLDMAXTEXTURES]; // Texture palette (if not null, 1 per texture)
    int32_t textureframes[OLDMAXTEXTURES]; // Number of texture frames

    int32_t numobjects;
    char objname[OLDMAXOBJECTS][RESNAMELEN];
    uint16_t objmaterial[OLDMAXOBJECTS];
    uint16_t objvertpos[OLDMAXOBJECTS];     // Position of object verts in vert array
    uint16_t objvertnum[OLDMAXOBJECTS];     // Num object verts in vert array
    uint16_t objfacepos[OLDMAXOBJECTS][OLDMAXTEXTURES];// Offset to array of face pos WORDs for each texture (0=none, 1=tex 0, 2 = tex 1, etc.)
    uint16_t objfacenum[OLDMAXOBJECTS][OLDMAXTEXTURES];// Offset to array of face num WORDs for each texture (0=none, 1=tex 0, 2 = tex 1, etc.)
//  SLinearBounds objbounds[OLDMAXOBJECTS];

    OFFSET motion[OLDMAXOBJSTATES];     // Contains motion info for state
    OFFSET anikeys[OLDMAXOBJSTATES];    // Contains all animation keys for state
    OFFSET imagery[OLDMAXOBJSTATES];    // Contains icon/iconani and other 2d imagery for state
    OFFSET objparent[OLDMAXOBJECTS];    // Contains object hierarchy for state where
                                        // Hierarchy is array of signed byte indexes for each
                                        // obj indicating its parent, -1 means root
};

// **********************************************************

// NEW Version of S3DImageryBody, no limits on arrays!
_STRUCTDEF(S3DImageryBody)
struct S3DImageryBody : public SImageryBody
{
    uint32_t flags;                     // If true, verts array points to multple vert images
    uint32_t version;                   // Version of imagery

    OS3DImageryState statedata;         // Array of S3DImageryState structures

    int32_t numverts;                   // Verts stored in state,frame order...
    OOOVEC3 verts;                      // Accessed via verts[state][frame][vert]
                                        // Where [state][frame] are always [0][0] if
                                        // I3D_ISMORPH is NOT set (not a morph animation)

    int32_t numfaces;                   // Number of faces
    OS3DFace faces;                     // Faces stored by texture, then object order

    int32_t nummaterials;                    
    OMATERIAL materials;                // Offset to D3DMATERIAL array

    int32_t numtextures;
    OS3DImageryTexture textures;        // Offset to S3DImageryTexture array

    int32_t numobjects;
    OS3DImageryObject objects;          // Offset to S3DImageryObject array

    int32_t numtags;
    OS3DImageryTag tags;                // Offset to array of animation tags (sorted by state/frame)
};

