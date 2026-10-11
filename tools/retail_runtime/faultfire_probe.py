"""Actual native FaultFire UV-scroll/two-pass mesh versus shared production.

Null spell, identity owner and explicit white-vertex software environment.
No native constructor/device/caller or complete effect acceptance is inferred.
"""
import argparse,json,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='475179128ede7f08dc19c33c7cb8d5bf4a42597d964cb65a065e82995d411418'


def parse_asset(asset):
 if sha(asset)!=ASSET_SHA:raise ValueError('Wrong FaultFire asset')
 u=lambda a:struct.unpack_from('<I',asset,a)[0];r=lambda a:a+u(a);body=20+u(16)
 if(u(body),u(body+12),u(body+20),u(body+36),u(body+44),u(body+52))!=(0x3c,4,2,1,1,0):raise ValueError('Topology changed')
 v=r(r(r(body+16)));faces=r(body+24);t=r(body+40);pixels=r(r(t+108));indices=struct.unpack_from('<6H',asset,faces)
 if indices!=(2,0,3,1,3,0):raise ValueError('Topology changed')
 if struct.unpack_from('<4I',asset,t+88)!=(0xf00,0xf0,0xf,0xf000):raise ValueError('ARGB4444 masks changed')
 return dict(vertices=list(struct.iter_unpack('<8f',asset[v:v+128])),indices=list(indices),vertex_bytes=asset[v:v+128],index_bytes=asset[faces:faces+12],texture=asset[pixels:pixels+32768],vertex_offset=v,index_offset=faces,texture_offset=pixels)


def production_spans():
 s=(ROOT/'src/effect.cpp').read_text();h=(ROOT/'src/effect.h').read_text()
 return dict(types=span(h,'class TFaultFireEffect_Bespoke :','// ========================================================================='),
  initialize=span(s,'    // SetupObjects preserves each authored V','    if (attach_runtime_component) {'),
  advance_submit=span(s,'void TFaultFireEffect_Bespoke::Advance(','void TFaultFireEffect_Bespoke::TickAndSubmitForTest_BESPOKE('),
  map_dispatch=span(s,'class TFaultFireReferenceComponent final','void TFaultFireEffect_Bespoke::Initialize('),math=(ROOT/'src/math3d.cpp').read_text())


def build_port(output,mesh,ticks,rng):
 pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
 prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
std::ifstream rng;int random(int lo,int hi){int a,b,v;if(!(rng>>a>>b>>v)||a!=lo||b!=hi)throw std::runtime_error("RNG contract mismatch");return v;}
enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct SQuadDrawItem{bool retail_argb4444=false,retail_software_projection=false;int corner_count=4,retail_texture=0;float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};struct S3DTex{TTextureHandle htexture=1;};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect{World world;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){};void SetCommandDone(bool){};const World&Transform()const{return world;}};
'''
 init='void TFaultFireEffect_Bespoke::Initialize(bool attach_runtime_component){S3DTex tex;\n'+pieces['initialize']+'}\n'
 tail=r'''
int main(int argc,char**argv){std::ifstream input(argv[1],std::ios::binary);std::vector<SMeshVertex>vertices(4);std::vector<uint16_t>indices(6);input.read((char*)vertices.data(),128);input.read((char*)indices.data(),12);if(!input)return 2;
 for(int owner:{0,1}){rng.open(argv[2]);TFaultFireEffect_Bespoke effect(nullptr);effect.vertices_=vertices;effect.indices_=indices;effect.Initialize(bool(owner));
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
 printf("S %d %d %.9g %.9g\n",owner,tick,effect.th_,effect.u_offset_);
 for(const auto&d:renderer.draws){if(d.key.texture!=1||d.corner_count!=3)throw std::runtime_error("Authored binding mismatch");printf("D %d %d",owner,tick);for(int i=0;i<3;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}
 rng.close();rng.clear();}}
'''.replace('TICKS',str(ticks))
 src=output/'faultfire-port.cpp';src.write_text(prelude+types+init+pieces['advance_submit']+tail)
 data=output/'mesh.bin';data.write_bytes(mesh['vertex_bytes']+mesh['index_bytes']);rngfile=output/'rng.txt';rngfile.write_text(''.join(f'{a} {b} {v}\n'for a,b,v in rng))
 binary=output/'faultfire-port';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(src),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 p=subprocess.run(cmd,text=True,capture_output=True);(output/'compile.log').write_text(p.stdout+p.stderr)
 if p.returncode:raise RuntimeError('Production compilation failed')
 p=subprocess.run([str(binary),str(data),str(rngfile)],text=True,capture_output=True);(output/'port-trace.txt').write_text(p.stdout);(output/'port-run.log').write_text(p.stderr)
 if p.returncode:raise RuntimeError('Production trace failed')
 frames={}
 for line in p.stdout.splitlines():
  words=line.split();key=(int(words[1]),int(words[2]))
  if words[0]=='S':frames[key]=dict(theta=float(words[3]),u_offset=float(words[4]),draws=[])
  else:frames[key]['draws'].append(dict(positions=[[float(v)for v in words[3+i*5:6+i*5]]for i in range(3)],uvs=[[float(v)for v in words[6+i*5:8+i*5]]for i in range(3)]))
 return frames,dict(command=cmd,source_sha256=sha(src.read_bytes()),binary_sha256=sha(binary.read_bytes()),mesh_sha256=sha(data.read_bytes()),rng_sha256=sha(rngfile.read_bytes()))


class FaultFireFixture:
 def __init__(self,exe,asset):
  self.mesh=parse_asset(asset);self.software=SoftwareFixture(exe,256,256);self.vm=self.software.vm;vm=self.vm
  self.software.set_texture(128,128,self.mesh['texture'],format='ARGB4444');self.animator=vm.allocate(0x200);self.owner=vm.allocate(0x200);self.obj=vm.allocate(0x200);self.verts=vm.allocate(128);self.imagery=vm.allocate(0x200)
  vm.put_u32(self.animator,0x5acaa8);vm.put_u32(self.animator+4,self.owner);vm.put_u32(self.animator+8,self.imagery);vm.put_u32(self.obj+0xa0,4);vm.put_u32(self.obj+0xa4,self.verts);vm.write(self.verts,self.mesh['vertex_bytes'])
  vtable=vm.allocate(0x200);done=vm.allocate(16);vm.write_code(done,b'\xc2\x04\x00');vm.put_u32(vtable+0x158,done);vm.put_u32(self.owner,vtable)
  self.point=vm.allocate(12);self.result=vm.allocate(12);self.matrix=vm.allocate(64)
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x409ca0:8,0x40a0c0:8,0x40ed40:4,0x40eef0:4,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
  self.packets=[];self.random_calls=[];self.pending_rng=None
  for a in self.boundaries:vm.uc.hook_add(UC_HOOK_CODE,self.boundary,begin=a,end=a)
  for a in(0x483300,0x48332c,0x58c582):vm.uc.hook_add(UC_HOOK_CODE,self.observe_rng,begin=a,end=a)
  vm.put_u32(0x5d7a28,1);self.software.checkpoint()
 def boundary(self,uc,a,size,user):
  vm=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
  if a in(0x409ca0,0x40eef0):uc.reg_write(UC_X86_REG_EAX,self.obj)
  if a==0x40a8f0:
   if vm.u32(self.obj)!=0x2100:raise AssertionError('Original flags mismatch')
   self.packets.append(dict(matrix=bytes(vm.uc.mem_read(self.obj+0x58,64)),uvs=[list(struct.unpack('<2f',vm.uc.mem_read(self.verts+i*32+24,8)))for i in range(4)]))
  uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[a])
 def observe_rng(self,uc,a,size,user):
  vm=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
  if a==0x483300:self.pending_rng=struct.unpack('<2i',vm.uc.mem_read(sp+4,8))
  elif a==0x48332c:
   self.random_calls.append([*self.pending_rng,uc.reg_read(UC_X86_REG_EAX)]);self.pending_rng=None
  else:
   uc.reg_write(UC_X86_REG_EAX,vm.random_msvc());uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
 def reset(self):
  self.software.restore();self.packets=[];self.random_calls=[];self.pending_rng=None;self.vm.call(0x4f1590,this=self.animator);self.vm.call(0x4f15f0,this=self.animator)
 def animate(self):self.vm.call(0x4f1610,this=self.animator)
 def state(self):return dict(theta=struct.unpack('<f',self.vm.uc.mem_read(self.animator+0xfc,4))[0],u=[struct.unpack('<f',self.vm.uc.mem_read(self.verts+i*32+24,4))[0]for i in range(4)],baseline_v=list(struct.unpack('<4f',self.vm.uc.mem_read(self.animator+0x100,16))),rng_seed=self.vm.rng_seed)
 def draws(self):
  self.packets=[];self.vm.call(0x4f16b0,this=self.animator);out=[]
  for packet in self.packets:
   self.vm.write(self.matrix,packet['matrix']);points=[]
   for vertex in self.mesh['vertices']:
    self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(self.matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
   for f in range(0,6,3):
    ids=self.mesh['indices'][f:f+3];out.append(dict(positions=[points[i]for i in ids],uvs=[packet['uvs'][i]for i in ids]))
  return out
 def pixels(self,draws):
  self.software.clear();points=[p for d in draws for p in d['positions']];uvs=[u for d in draws for u in d['uvs']]
  projected=self.software.project(points,camera=(0,0,0));self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,uvs)],list(range(len(points))),True,False,instruction_limit=5000000)
  return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=128);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/faultfire.i3d')
 fixture=FaultFireFixture(a.executable,asset);fixture.reset();states=[]
 for tick in range(a.ticks+1):
  if tick:fixture.animate()
  states.append(fixture.state())
 port,compiled=build_port(a.output,fixture.mesh,a.ticks,fixture.random_calls);samples={t for t in(0,1,12,24,40,62,63,64,80,100,126,128)if t<=a.ticks};cases=[];state_trace=[];errors=[];pixel_errors=0;before={k:sha(v.encode())for k,v in production_spans().items()};fixture.reset()
 for tick in range(a.ticks+1):
  if tick:fixture.animate()
  state=fixture.state();candidate=port[(1,tick)]
  if state!=states[tick]:raise AssertionError('Original warm state mismatch')
  if f32(state['theta'])!=f32(candidate['theta']):errors.append(dict(tick=tick,field='theta',native=state['theta'],port=candidate['theta']))
  if port[(0,tick)]!=candidate:errors.append(dict(tick=tick,field='preview/map mismatch'))
  port_u=[None]*4
  for face,draw in enumerate(candidate['draws'][:2]):
   for index,uv in zip(fixture.mesh['indices'][face*3:face*3+3],draw['uvs']):port_u[index]=uv[0]
  if any(u is None for u in port_u):raise AssertionError('Missing candidate authored vertex')
  for vertex,(u,v)in enumerate(zip(state['u'],port_u)):
   if f32(u)!=f32(v):errors.append(dict(tick=tick,field='vertex_u',vertex=vertex,native=u,port=v))
  state_trace.append(dict(tick=tick,native=state,port_theta=candidate['theta'],port_u=port_u))
  if tick not in samples:continue
  native=fixture.draws();draws=candidate['draws'];maximum=0
  if len(native)!=len(draws):errors.append(dict(tick=tick,field='draw count'))
  for x,y in zip(native,draws):
   for field in('positions','uvs'):
    for q,r in zip(x[field],y[field]):
     for n,m in zip(q,r):
      maximum=max(maximum,abs(n-m))
      if abs(n-m)>(3e-5 if field=='positions'else 1e-7):errors.append(dict(tick=tick,field=field,native=n,port=m))
  color,depth=fixture.pixels(native);mcolor,mdepth=fixture.pixels(draws);diff=sum(n!=m for n,m in zip(struct.unpack('<65536H',color),struct.unpack('<65536H',mcolor)));pixel_errors+=diff
  if depth!=mdepth:errors.append(dict(tick=tick,field='depth'))
  save_rgb565_png(a.output/f'retail-{tick:03d}.png',color,256,256);save_rgb565_png(a.output/f'port-{tick:03d}.png',mcolor,256,256)
  cases.append(dict(tick=tick,native_state=state,port_state=candidate,native_draws=native,max_field_error=maximum,pixel_differences=diff,nonzero_pixels=sum(x!=0 for x in struct.unpack('<65536H',color)),hashes=[sha(color),sha(mcolor),sha(depth),sha(mdepth)]))
 # Replay original state, packets and pixels from the same warm snapshot.
 retained={case['tick']:case for case in cases};warm_images=0;fixture.reset()
 for tick in range(a.ticks+1):
  if tick:fixture.animate()
  if fixture.state()!=states[tick]:raise AssertionError('Full warm state replay changed')
  if tick not in retained:continue
  draws=fixture.draws();prior=retained[tick]
  if draws!=prior['native_draws']:raise AssertionError('Original two-pass packet replay changed')
  color,depth=fixture.pixels(draws)
  if(sha(color),sha(depth))!=(prior['hashes'][0],prior['hashes'][2]):raise AssertionError('Original image/depth replay changed')
  warm_images+=1
 after={k:sha(v.encode())for k,v in production_spans().items()}
 if before!=after:raise AssertionError('Relevant source changed')
 report=dict(status='pass'if not errors and not pixel_errors else'differences_found',errors=errors,pixel_differences=pixel_errors,cases=cases,state_trace=state_trace,exact_state_field_checks=5*len(states),exact_original_state_warm_replay_ticks=len(states),exact_original_packet_image_depth_warm_replays=warm_images,probe_sha256=sha(Path(__file__).read_bytes()),state_ticks=len(states),ordered_random_calls=fixture.random_calls,compiled_port=compiled,source_span_sha256=after,asset_sha256=sha(asset),retail_sha256=RETAIL_SHA,original_functions=['Setup4f1590','Init4f15f0','Animate4f1610','Render4f16b0','RandomRange483300'],scope='Nullspell stationaryTEffect, identityowner, actualauthoredfourvertices/twofaces/ARGB4444texture; sharedcompiledAdvance/Submit, originalSetup/FCOS/UV accumulation/matrix/projection/raster. OriginalsoftwareblendSave/Set/Restore execute; whitevertex fixture. Assetloader/completeconstructor/caller/device/normal-light/modernGPU remain separate.')
 (a.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in('cases','errors','ordered_random_calls','state_trace')},indent=2));print('errors',len(errors))
 if errors or pixel_errors:raise SystemExit(1)

if __name__=='__main__':main()
