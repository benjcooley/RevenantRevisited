"""Exact seven TeleportDoorInside atlas animators; native call preflight first."""
import argparse,json,struct,time,zipfile,subprocess
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from static_mesh_probe import ROOT,StaticFixture,build_port,production_spans,OWNERS
from default_static_probe import parse_parts
from software_probe import RETAIL_SHA,save_rgb565_png
from fizzle_probe import sha

def native_mode_mapping(vm):
    calls=[];vm.put_u32(0x5d7a28,1);vm.put_u32(0x5e8790,0);vm.put_u32(0x66818c,0)
    def observe(uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);calls.append([vm.u32(sp+4),vm.u32(sp+8)])
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+12);uc.reg_write(UC_X86_REG_EAX,1)
    h=vm.uc.hook_add(UC_HOOK_CODE,observe,begin=0x417060,end=0x417060)
    try:vm.call(0x417d60,(2,1),this=0x65a57c)
    finally:vm.uc.hook_del(h)
    if dict(calls)!={14:0,7:1,22:1,19:5,20:6,21:2}:raise AssertionError("Original atlas mode mapping changed")
    return dict(mode=2,original_function="0x417d60",observed_setter="0x417060",states=calls)

PROFILES=tuple(dict(name='TeleportDoorInside'+letter,id=tid,asset='Magic/Warp'+letter+'.I3D',asset_sha256=digest,
 vertices=4,faces=3,textures=1,objects=1,frames=1,registration=0x4ff8f0+index*32,
 builder=builder,factory=0x501010+index*0xd0,animator_vtable=0x5afe1c+index*0x64)
 for index,(letter,tid,builder,digest)in enumerate((
 ('B','0xad92bc28',0x66cfb0,'0ffad51668c2c39c45df92d7174e053475a25c62ca70d1e4d9cf0ecda943a836'),
 ('O','0xad92bc29',0x66cf98,'8f04e17b51636cad5195592fdac1c3ed70c0092edab2e351d589f10a1cf0f34e'),
 ('P','0xad92bc30',0x66cf90,'680379069432189e9a05c5cb92cf417c7a194feed37b1951cb90264bf631f391'),
 ('R','0xad92bc31',0x66cf30,'f8ca877ffeffb67fd571ec9044c4b6636dcd3c638356d0d8e4747e83c4c94d66'),
 ('W','0xad92bc32',0x66cfb8,'e4efcf96b48ffed4c601d271bda6926bd1c7f67389de5459f51035c463022d96'),
 ('Y','0xad92bc33',0x66cf20,'63e48d799e3daa058e57bab26d1d71e70dc9a5b0ac58cb3766fa98e608bd7e4d'),
 ('G','0xad92bc34',0x66cfd8,'916432dceb4f4fe5571530601c818338442145357f8fcfc15551444bc27446b2'))))


class WarpFixture(StaticFixture):
    def __init__(self,exe):
        super().__init__(exe,512,512,5000000)
        v=self.vm;self.blend_mapping=native_mode_mapping(v);self.mesh=v.allocate(0x200);self.vertices=v.allocate(128);self.owner=v.allocate(0x200)
        self.imagery=v.allocate(0x200);self.packets=[];self.mode=[];self.calls={}
        self.boundaries={0x445940:4,0x41c7f0:8,0x40dd60:0,0x40e2e0:0,
            0x40eef0:4,0x40a0c0:8,0x417d60:8,0x40a8f0:24}
        for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        self.dispatch=[]
        for p in PROFILES:
            v.call(p['registration']);ptr=v.allocate(32);v.write(ptr,p['name'].encode()+b'\0')
            builder=v.call(0x40dca0,(ptr,));factory=v.u32(v.u32(builder))
            if builder!=p['builder']or factory!=p['factory']:raise AssertionError('Actual custom builder changed')
            animator=v.call(factory,(self.owner,),this=builder)
            if v.u32(animator)!=p['animator_vtable']:raise AssertionError('Actual factory leaf vtable changed')
            if [v.u32(p['animator_vtable']+i)for i in(0x18,0x2c,0x34)]!=[0x4ff9d0,0x4ffa60,0x4ffae0]:
                raise AssertionError('Family no longer shares native atlas bodies')
            self.dispatch.append(dict(name=p['name'],builder=hex(builder),factory=hex(factory),
                animator=animator,animator_vtable=hex(v.u32(animator))))
        self.animator=self.dispatch[0]['animator']
        v.put_u32(self.mesh+0xa0,4);v.put_u32(self.mesh+0xa4,self.vertices)
        v.put_u32(self.animator+4,self.owner);v.put_u32(self.animator+8,self.imagery)

    def external(self,uc,address,size,user):
        v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);self.calls[hex(address)]=self.calls.get(hex(address),0)+1
        if address==0x40eef0:
            if v.u32(sp+4)!=0:raise AssertionError('Atlas requested a substituted mesh')
            uc.reg_write(UC_X86_REG_EAX,self.mesh)
        elif address==0x40a0c0:
            if(v.u32(sp+4),v.u32(sp+8))!=(self.mesh,0x1e2):raise AssertionError('Native mutable-UV binding changed')
        elif address==0x417d60:self.mode.append([v.u32(sp+4),v.u32(sp+8)])
        elif address==0x40a8f0:
            if v.u32(sp+4)!=self.mesh or v.u32(self.mesh)!=0x2000:raise AssertionError('Native UV override packet changed')
            rows=list(struct.iter_unpack('<8f',bytes(v.uc.mem_read(self.vertices,128))))
            self.packets.append([list(row[6:])for row in rows])
        uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def raster(self,points,uvs,indices):
        self.software.clear()
        for state,value in self.blend_mapping['states']:self.vm.call(0x56d400,(state,value))
        projected=self.software.project(points,camera=(0,0,0),zdist=1925)
        if any(not(0<=p[0]<512 and 0<=p[1]<512)for p in projected):raise AssertionError('Authored geometry clips fixed viewport; do not fit')
        return self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,uvs)],indices,
            z_enabled=True,z_write=False,instruction_limit=5000000)

    def trace(self,m,ticks=40,profile=None):
        if profile is not None:
            self.animator=next(row['animator']for row in self.dispatch if row['name']==profile['name'])
        v=self.vm;v.put_u32(self.animator+4,self.owner);v.put_u32(self.animator+8,self.imagery);v.write(self.vertices,bytes.fromhex(m['vertex_hex']));v.call(0x4ff9d0,this=self.animator);states=[]
        for tick in range(ticks+1):
            if tick:v.call(0x4ffa60,this=self.animator)
            self.mode=[];self.packets=[];v.call(0x4ffae0,this=self.animator)
            if self.mode!=[[2,1]]or len(self.packets)!=1:raise AssertionError('Expected native atlas render mode2/one packet')
            values=list(struct.unpack('<4f2i',v.uc.mem_read(self.animator+0x11c,24)))
            states.append(dict(tick=tick,offset=values[:2],velocity=values[2:4],delay=values[4:],uvs=self.packets[0]))
        return states



def compile_warp(output,metadata,ticks=40):
    code=r"""
#include <cstdio>
#include <fstream>
#include "effects/warp_atlas.h"
enum class Shade{Material=0,Texture=1,TextureLit=2};
struct Draw {float uv_offset[2]{};bool additive_blend=true,premultiply_alpha=true;Shade shade=Shade::Material;
 int retail_lighting=0;float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{};float power=9;};
int main(int argc,char**argv){float original[8];std::ifstream f(argv[1],std::ios::binary);f.read((char*)original,32);
 retail_warp::State state,step,partition;for(int tick=0;tick<=40;++tick){if(tick){state.Advance(1.0/24.0);step.Step();partition.Advance(1.0/48.0);partition.Advance(1.0/48.0);}
 if(state.u!=step.u||state.v!=step.v||state.u!=partition.u||state.v!=partition.v)return 3;
 Draw draw;retail_warp::ConfigureDraw(draw,state);printf("D %d %.9g %.9g %d %d %d %d",tick,state.u,state.v,int(draw.additive_blend),int(draw.premultiply_alpha),int(draw.shade),draw.retail_lighting);
 for(int i=0;i<8;++i)printf(" %.9g",original[i]+draw.uv_offset[i%2]);
 for(auto*colors:{draw.diffuse,draw.ambient,draw.specular,draw.emissive})for(int i=0;i<4;++i)printf(" %.9g",colors[i]);printf(" %.9g\n",draw.power);}
 state.Reset();if(state.u||state.v||state.accumulator)return 4;
 for(auto&p:retail_warp::profiles)printf("P %08x %s\n",p.id,p.asset);
 if(retail_warp::AssetPath(0xffffffffu))return 5;
}
"""
    source=output/'warp-production.cpp';source.write_text(code);binary=output/'warp-production'
    command=['clang++','-std=c++17','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)]
    result=subprocess.run(command,text=True,capture_output=True);(output/'warp-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Production atlas header compilation failed')
    traces={}
    for p in PROFILES:
        m=metadata[p['name']];inputs=output/(p['name']+'.uv-input');inputs.write_bytes(struct.pack('<8f',*(n for v in m['vertices']for n in v[6:])))
        trace=subprocess.check_output([str(binary),str(inputs)],text=True);(output/(p['name']+'-atlas-port-trace.txt')).write_text(trace)
        rows=[];paths=[]
        for line in trace.splitlines():
            values=line.split()
            if values[0]=='P':paths.append([values[1],values[2]])
            else:rows.append(dict(tick=int(values[1]),offset=list(map(float,values[2:4])),flags=list(map(int,values[4:8])),
                uvs=[list(map(float,values[8+i*2:10+i*2]))for i in range(4)],material=list(map(float,values[16:]))))
        if paths!=[[p['id'][2:],p['asset'].replace('/','\\')]for p in PROFILES]:raise AssertionError('Production exact ID/asset mapping changed')
        if any(row['flags']!=[0,0,1,3]or row['material']!=[1.]*8+[0.]*9 for row in rows):raise AssertionError('Actual ConfigureDraw contract changed')
        traces[p['name']]=rows
    return traces,dict(command=command,driver_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        header_sha256=sha((ROOT/'src/effects/warp_atlas.h').read_bytes()),bodies=['retail_warp::State::Step','Advance','Reset','ConfigureDraw','AssetPath'],
        timing_control='Step equals Advance(1/24) and two Advance(1/48) calls for every tick; reset clears state')


def compare(args,fixture,metadata,states):
    before={k:sha(v.encode())for k,v in production_spans().items()}
    atlas,compiled=compile_warp(args.output,metadata)
    port,mesh_compiled=build_port(args.output,metadata,frames=(0,))
    state_field_count=uv_field_count=0
    for name,rows in states.items():
        for native,modern in zip(rows,atlas[name]):
            if native['tick']!=modern['tick']or native['offset']!=modern['offset']:raise AssertionError('Full native/compiled state changed')
            state_field_count+=2
            for x,y in zip(native['uvs'],modern['uvs']):
                for a,b in zip(x,y):
                    if struct.pack('<f',a)!=struct.pack('<f',b):raise AssertionError('Full native/compiled UV bits changed')
                    uv_field_count+=1
    failures=[];cases=[];timings=[];controls=[];samples=(0,1,3,4,7,8,12,16,20,32,40)
    for p in PROFILES:
        name=p['name'];m=metadata[name];fixture.setup(m)
        for moved in(0,1):
            fixture.software.restore();matrix,points=fixture.original(0,moved)
            packet=port[name]['cases'][(0,moved)];candidate_points=[v[:3]for v in packet['vertices']]
            if port[name]['indices']!=m['indices']:raise AssertionError('Production mesh lost authored faces')
            max_position_error=max(abs(a-b)for x,y in zip(points,candidate_points)for a,b in zip(x,y))
            warm_hashes={}
            for tick in samples:
                row=states[name][tick];modern=atlas[name][tick]
                uv_errors=sum(struct.pack('<f',a)!=struct.pack('<f',b)for x,y in zip(row['uvs'],modern['uvs'])for a,b in zip(x,y))
                if row['offset']!=modern['offset']:raise AssertionError('Native/actual State offset mismatch')
                prior=None
                for repeat in range(2):
                    started=time.perf_counter();fixture.software.restore()
                    original,z=fixture.raster(points,row['uvs'],m['indices'])
                    candidate,cz=fixture.raster(candidate_points,modern['uvs'],port[name]['indices'])
                    hashes=[sha(original),sha(candidate),sha(z),sha(cz)];timings.append((time.perf_counter()-started)*1000)
                    if prior is not None and prior!=hashes:raise AssertionError('Native/production warm image/depth replay changed')
                    prior=hashes
                visible=sum(v!=0 for v in struct.unpack('<262144H',original));different=sum(a!=b for a,b in zip(original,candidate))
                if different or z!=cz or not visible or max_position_error>5e-5 or uv_errors:
                    failures.append(dict(name=name,moved=moved,tick=tick,pixel_bytes=different,visible=visible,position=max_position_error,uv_errors=uv_errors))
                for label,data in (()if args.no_images else(('retail',original),('port',candidate))):save_rgb565_png(args.output/f'{name}-{label}-{tick:02d}-{moved}.png',data,512,512)
                if moved==0 and tick==1:
                    fixture.software.restore();frozen,fz=fixture.raster(candidate_points,atlas[name][0]['uvs'],port[name]['indices'])
                    changed=sum(a!=b for a,b in zip(original,frozen))
                    if not changed or fz!=z:raise AssertionError('Frozen UV negative control ineffective')
                    controls.append(dict(name=name,frozen_uv_changed_color_bytes=changed,depth_equal=fz==z))
                warm_hashes[tick]=hashes[0]
                cases.append(dict(name=name,id=p['id'],asset=p['asset'],asset_sha256=p['asset_sha256'],tick=tick,
                    owner_position=list(OWNERS[moved]),offset=row['offset'],native_uvs=row['uvs'],compiled_uvs=modern['uvs'],
                    original_local_matrix=matrix,compiled_world_matrix=packet['matrix'],max_position_error=max_position_error,
                    uv_bit_mismatches=uv_errors,color_different_bytes=different,depth_equal=z==cz,
                    nonzero_pixels=visible,image_hashes=hashes,compiled_draw_flags=modern['flags']))
            if len(set(warm_hashes.values()))<4:raise AssertionError('Atlas animation did not visibly change')
            if warm_hashes[4]!=warm_hashes[20]:raise AssertionError('Native 16tick looping row changed')
    for p in PROFILES:
        name=p['name']
        for tick in samples:
            paired=[c['image_hashes'][0]for c in cases if c['name']==name and c['tick']==tick]
            if paired[0]==paired[1]:raise AssertionError('Owner translation did not move visible image')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if after!=before:raise AssertionError('Production geometry source changed during capture')
    return dict(status='fail'if failures else'pass',failures=failures,case_count=len(cases),replays_per_case=2,
        compiled_atlas=compiled,compiled_geometry=mesh_compiled,geometry_source_span_sha256=after,images_written=not args.no_images,
        state_offset_field_count=state_field_count,uv_field_count=uv_field_count,negative_controls=controls,
        cases=cases,median_warm_pair_ms=sorted(timings)[len(timings)//2],rendered_comparison=True,
        scope='Seven exact custom atlas native Init/Animate/Render plus compiled actual State/ConfigureDraw; authored complete meshes and compiled generic production extraction/pose. Fixed512camera0/Z1925/whitevertexinputs; native software projection and alpha texture raster shared. Original material/illumination producer, map lifecycle/owner and modern GPU remain open.')


def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('executable',type=Path);ap.add_argument('--output',type=Path,required=True)
    ap.add_argument('--compare',action='store_true');ap.add_argument('--no-images',action='store_true',help='Render and verify images but omit PNG encoding');args=ap.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    fixture=WarpFixture(args.executable);metadata={};states={}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
        names={n.lower():n for n in archive.namelist()}
        for p in PROFILES:
            m=parse_parts(archive.read(names[('Imagery/'+p['asset']).lower()]),p)[0];metadata[p['name']]=m
            states[p['name']]=fixture.trace(m,profile=p)
    report=dict(status='native_preflight_pass',retail_sha256=RETAIL_SHA,dispatch=fixture.dispatch,
        original_functions=['Init0x4ff9d0','Animate0x4ffa60','Render0x4ffae0'],
        boundary_calls=fixture.calls,states=states,profiles=PROFILES,native_blend_mapping=fixture.blend_mapping,
        probe_sha256=sha(Path(__file__).read_bytes()),
        full_effect_accepted=False,rendered_comparison=False,dosbox_used=False,
        scope='Actual custom factories, shared Init/Animate/Render; decoded exact mutable mesh supplied; generic owner/imagery/dynamic-list constructors and base tick separate. No production comparison or rendered credit.')
    if args.compare:report.update(compare(args,fixture,metadata,states))
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in('states','cases')},indent=2))
    if report.get('failures'):raise SystemExit(1)

if __name__=='__main__':main()
