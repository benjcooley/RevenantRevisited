#pragma once
// Exact shipped combatflash ad92bd29 (misc\Impact.i3d): nine 19-frame start
// states, each followed by a one-frame end state. EffectCombatFlash
// (0x004c8500) starts the effect in an even state, 2*random(0, 9) capped at
// 16, and the generic TEffect::Pulse removes it when that state ends. Every
// start state carries "blendcont litadd" on its first frame and one partsys
// on the next; the controllers and the default animator do the rest.
// Fingerprints admit the original records only (3dimage.cpp).
namespace retail_combatflash_profile {
inline constexpr unsigned type_id = 0xad92bd29u;
inline constexpr int states = 18, start_frames = 19, end_frames = 1, state_flags = 0x2000;
inline constexpr const char* object_names[21] = {
    "#particle01","impact01","#impact01","#particle02","impact02","#impact02",
    "#particle03","impact03","#impact03","#impact06","#impact04","#impact05","impact04","impact05",
    "impact06","impact07","impact08","impact09","#impact08","#impact07","#impact09"};
// Per start state 2k: the particle prototype and the authored flare it
// animates (#impact07 and #impact09 carry no geometry: particles only).
inline constexpr int prototypes[9] = {0,3,6,0,3,6,0,3,6};
inline constexpr int flares[9] = {2,5,8,10,11,9,19,18,20};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[18] = {
{0,0,"blendcont","litadd"},
{0,1,"partsys","obj=(impact01),particle=#particle01,pps=[0:250,2:0],initialvelocity=15:30,scale=[0:2.5,100:0],lifespan=30:45,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.3,gravity=0.5,color=[0:(255,225,100),25:(255,175,50),99:(0,0,0)],spread=120"},
{2,20,"blendcont","litadd"},
{2,21,"partsys","obj=(impact02),particle=#particle02,pps=[0:250,2:0],initialvelocity=25:30,scale=[0:2,100:0],lifespan=15:25,rlocalrotation=[0:(0,0,0),100:(0,360,0)],friction=0.55,gravity=0.1,color=[0:(100,200,255),25:(50,125,255),99:(0,0,0)],spread=360,numtrails=2:3,azimuth=360"},
{4,40,"blendcont","litadd"},
{4,41,"partsys","obj=(impact03),particle=#particle03,pps=[0:250,2:0],initialvelocity=5:55,scale=[0:3,75:1.5,95:0],lifespan=15:20,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.20,gravity=0.2,color=[0:(255,255,255),15:(120,1,255),95:(0,0,0)],spread=360,azimuth=360,"},
{6,60,"blendcont","litadd"},
{6,61,"partsys","obj=(impact04),particle=#particle01,pps=[0:150,2:0],initialvelocity=-25:-40,scale=[0:3.5,100:0],lifespan=15:20,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.40,gravity=0.55,color=[0:(255,255,255),10:(1,1,255),100:(0,0,0)],spread=120"},
{8,80,"blendcont","litadd"},
{8,81,"partsys","obj=(impact05),particle=#particle02,pps=[0:200,2:0],initialvelocity=35:50,scale=[0:2,100:0],lifespan=15:25,rlocalrotation=[0:(0,0,0),100:(0,360,0)],friction=0.55,gravity=0.1,color=[0:(255,255,255),15:(100,1,255),99:(0,0,0)],spread=360,numtrails=0:0,azimuth=360"},
{10,100,"blendcont","litadd"},
{10,101,"partsys","obj=(impact06),particle=#particle03,pps=[0:150,2:0],initialvelocity=5:55,scale=[0:3,75:2,95:0],lifespan=15:20,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.20,gravity=0.5,color=[0:(255,255,255),15:(255,50,1),95:(0,0,0)],spread=360,azimuth=360,"},
{12,120,"blendcont","litadd"},
{12,121,"partsys","obj=(impact07),particle=#particle01,pps=[0:150,2:0],initialvelocity=-25:-40,scale=[0:3.5,100:0],lifespan=15:20,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.40,gravity=0.55,color=[0:(255,255,255),10:(115,240,255),100:(0,0,0)],spread=120"},
{14,140,"blendcont","litadd"},
{14,141,"partsys","obj=(impact08),particle=#particle02,pps=[0:200,2:0],initialvelocity=35:50,scale=[0:2,100:0],lifespan=15:25,rlocalrotation=[0:(0,0,0),100:(0,360,0)],friction=0.55,gravity=0.1,color=[0:(255,255,255),15:(255,1,1),99:(0,0,0)],spread=360,numtrails=0:0,azimuth=360"},
{16,160,"blendcont","litadd"},
{16,161,"partsys","obj=(impact09),particle=#particle03,pps=[0:150,2:0],initialvelocity=5:55,scale=[0:3,75:2,95:0],lifespan=15:20,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.20,gravity=0.5,color=[0:(255,255,255),15:(100,225,75),95:(0,0,0)],spread=360,azimuth=360,"},
};
}
