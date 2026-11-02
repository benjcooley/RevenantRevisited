"""Exercise actual map sprite-layer selection/cache refresh with renderer boundaries.

No retail assets are embedded. The compiled production functions use distinct
still/frame pointers, registrations, completion and transfer/state mutations.
"""
import subprocess
import tempfile
import unittest
from pathlib import Path
from test_vfxreviewspawn import function
ROOT = Path(__file__).resolve().parents[2]
STUB = r'''
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <strings.h>
#define stricmp strcasecmp
#define log_info(...) ((void)0)
constexpr int OBJIMAGE_ANIMATION=1,OBJIMAGE_MESH3D=2,OBJCLASS_TILE=9,OBJCLASS_EFFECT=25;
struct S3DPoint{int x=0,y=0,z=0;};
struct TBitmap{int regx=0,regy=0;};using PTBitmap=TBitmap*;
struct SImageryHeader{int imageryid=1,numstates=2;};
struct SImageryBody{};
struct SAnimImageryBody:SImageryBody{struct State{TBitmap*still=nullptr;}states[2];};
struct TAnimation{std::vector<TBitmap*>frames;TBitmap*GetFrame(int n){assert(n>=0);return n<int(frames.size())?frames[n]:nullptr;}};
using PTAnimation=TAnimation*;
struct TObjectImagery{virtual~TObjectImagery()=default;SImageryHeader hdr;SAnimImageryBody body;
 SImageryHeader*GetHeader(){return &hdr;}SImageryBody*GetBody(){return &body;}
 int GetRegX(int st){return 100+st;}int GetRegY(int st){return 200+st;}int GetRegZ(int st){return 300+st;}
 int GetAnimRegX(int st){return 3+st;}int GetAnimRegY(int st){return 7+st;}int GetAnimRegZ(int st){return 11+st;}};
struct TAnimImagery:TObjectImagery{TAnimation*animations[2]{};int NumStates(){return 2;}TAnimation*GetAnimation(int st){return animations[st];}};
struct T3DImagery:TObjectImagery{int NumObjects(){return 1;}bool IsHidden(int,int){return false;}};
struct TObjectAnimator{bool complete=false;bool IsComplete()const{return complete;}};
struct TObjectInstance{TObjectImagery*image=nullptr;TObjectAnimator*animator=nullptr;int cls=9,st=0,fr=0;bool animated=true;S3DPoint pos;
 TObjectImagery*GetImagery(){return image;}int ObjClass(){return cls;}int GetState(){return st;}int GetFrame(){return fr;}
 bool IsAnimated(){return animated;}TObjectAnimator*GetAnimator(){return animator;}S3DPoint Pos(){return pos;}
 const char*GetTypeName(){return "TwnBrazier";}int GetMapIndex(){return 10;}unsigned ObjId(){return 0x70dc006a;}int GetFrameRate(){return 1;}};
struct SSectorTileTex{TBitmap*bm_key=nullptr;int ref_count=0;};
struct SSectorMeshAsset{TObjectImagery*imagery_key=nullptr;int objnum=0,ref_count=0;};
struct Impl{std::vector<SSectorTileTex>sectorTileTex;std::unordered_map<TBitmap*,int>sectorTileTexByBitmap;std::vector<SSectorMeshAsset>sectorMeshAsset;};
enum class ESectorDrawableKind{Tile,Mesh,Billboard};
struct Ref{TObjectInstance*owner=nullptr;TObjectInstance*Get(){return owner;}};
struct SSectorDrawableInst{Ref src;ESectorDrawableKind kind=ESectorDrawableKind::Tile;bool animation_layer=false;int state=0,frame=0,asset_idx=-1,regx=0,regy=0,regz=0;S3DPoint world_pos;void UpdateFromInstance();};
int cache_calls=0;
int CacheSectorBitmap(std::vector<SSectorTileTex>&v,std::unordered_map<TBitmap*,int>&lookup,TBitmap*bm,TObjectInstance*,TObjectImagery*,int){
 ++cache_calls;if(!bm)return-1;auto i=lookup.find(bm);if(i!=lookup.end())return i->second;int n=v.size();v.push_back({bm,0});lookup[bm]=n;return n;}
'''
MAIN = r'''
int main(){
 TBitmap base{30,40};TBitmap frames[15];TAnimation animation;
 for(int i=0;i<15;++i){frames[i]={10+i,20+i};animation.frames.push_back(&frames[i]);}
 TAnimImagery image;image.body.states[0].still=&base;image.animations[0]=&animation;
 TObjectAnimator animator;TObjectInstance owner;owner.image=&image;owner.animator=&animator;
 assert(StateStillBitmap(&image,0)==&base);assert(StateAnimationBitmap(&image,0,4)==&frames[4]);
 assert(!StateAnimationBitmap(&image,0,-1));assert(!StateAnimationBitmap(&image,0,15));assert(!StateAnimationBitmap(&image,-1,0));assert(!StateAnimationBitmap(&image,1,0));
 assert(AnimationLayerVisible(&owner));animator.complete=true;assert(!AnimationLayerVisible(&owner));animator.complete=false;
 owner.animated=false;assert(!AnimationLayerVisible(&owner));owner.animated=true;owner.animator=nullptr;assert(!AnimationLayerVisible(&owner));owner.animator=&animator;assert(!AnimationLayerVisible(nullptr));
 Impl s;SSectorDrawableInst overlay;overlay.src.owner=&owner;overlay.animation_layer=true;
 SSectorDrawableInst still;still.src.owner=&owner;still.asset_idx=CacheSectorBitmap(s.sectorTileTex,s.sectorTileTexByBitmap,&base,&owner,&image,0);
 REFRESH
 for(int tick=0;tick<45;++tick){owner.fr=tick%15;refresh_draw_inst(overlay);refresh_draw_inst(still);
  assert(s.sectorTileTex[overlay.asset_idx].bm_key==&frames[owner.fr]);assert(s.sectorTileTex[still.asset_idx].bm_key==&base);
  assert(overlay.regx==frames[owner.fr].regx-3);assert(overlay.regy==frames[owner.fr].regy-7);assert(overlay.regz==11);
 }
 assert(s.sectorTileTex.size()==16);int calls=cache_calls;refresh_draw_inst(overlay);assert(cache_calls==calls);
 owner.fr=1;owner.pos={11,12,13};overlay.UpdateFromInstance();assert(overlay.frame==14);assert(overlay.world_pos.x==11);
 refresh_draw_inst(overlay);assert(s.sectorTileTex[overlay.asset_idx].bm_key==&frames[1]);
 owner.st=1;owner.fr=0;overlay.UpdateFromInstance();still.UpdateFromInstance();refresh_draw_inst(overlay);refresh_draw_inst(still);
 assert(overlay.asset_idx==-1&&still.asset_idx==-1);owner.st=0;owner.fr=2;refresh_draw_inst(overlay);refresh_draw_inst(still);
 assert(s.sectorTileTex[overlay.asset_idx].bm_key==&frames[2]);assert(s.sectorTileTex[still.asset_idx].bm_key==&base);
 for(auto&t:s.sectorTileTex)t.ref_count=0;CountExistingAssetRefForObject(s,&owner);
 assert(s.sectorTileTex[overlay.asset_idx].ref_count==1&&s.sectorTileTex[still.asset_idx].ref_count==1);
 for(auto&t:s.sectorTileTex)t.ref_count=0;owner.animated=false;CountExistingAssetRefForObject(s,&owner);
 assert(s.sectorTileTex[overlay.asset_idx].ref_count==0&&s.sectorTileTex[still.asset_idx].ref_count==1);
 for(auto&t:s.sectorTileTex)t.ref_count=0;owner.cls=OBJCLASS_EFFECT;CountExistingAssetRefForObject(s,&owner);
 assert(s.sectorTileTex[overlay.asset_idx].ref_count==1&&s.sectorTileTex[still.asset_idx].ref_count==0);
 assert(AnimationLayerVisible(&owner));
}
'''


class BrazierMapAnimationTests(unittest.TestCase):
    def test_actual_sprite_layers_cache_completion_and_transfer(self):
        text = (ROOT/'src/game/world/maprenderer.cpp').read_text()
        pieces = [function(text, name) for name in (
            'static TBitmap* StateStillBitmap(', 'static TBitmap* StateAnimationBitmap(',
            'static bool AnimationLayerVisible(', 'static int32_t FindSectorBitmap(',
            'void SSectorDrawableInst::UpdateFromInstance()')]
        pieces.append('template<typename TImpl>\n'+function(text, 'static void CountExistingAssetRefForObject('))
        refresh = function(text, 'auto refresh_draw_inst =')+';'
        with tempfile.TemporaryDirectory(prefix='brazier-map-animation-') as tmp:
            root = Path(tmp); cpp = root/'contract.cpp'; exe = root/'contract'
            cpp.write_text(STUB+'\n'+'\n'.join(pieces)+'\n'+MAIN.replace('REFRESH',refresh))
            result = subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',str(cpp),'-o',str(exe)],text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            result = subprocess.run([str(exe)],text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            self.assertEqual(result.stderr,'')


if __name__ == '__main__':
    unittest.main()
