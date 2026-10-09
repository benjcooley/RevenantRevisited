#!/usr/bin/env python3
"""Six actual authored water scroll controllers: native parser/UV/pose/software A/B."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_EDI
from static_mesh_probe import ROOT,StaticFixture,build_port,production_spans as mesh_spans
from default_static_probe import original_registry
from software_probe import RETAIL_SHA,save_rgb565_png
from fizzle_probe import sha,span,f32
TICKS=(0,1,24,50,99,100,120);POSE_FRAMES=(0,1,20,24,50,99);WIDTH=1280;HEIGHT=720


def controller_span(source=None):
    if source is None:source=(ROOT/'src/3dimage.cpp').read_text()
    return span(source,'static bool ParseScrollTexParams','static inline uint8_t ExpandBitsTo8')


def parse_asset(data,row):
    if sha(data)!=row['asset_sha256']:raise ValueError('Shipped scroll asset changed')
    u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a);b=20+u(16)
    if (u(b),u(b+4),u(b+36),u(b+44),u(b+52),u(24))!=(0xdc,3,1,1,1,1):raise ValueError('Only selected one-object no-morph scroll tags supported')
    nv,nf=u(b+12),u(b+20);v=r(r(r(b+16)));f=r(b+24);o=r(b+48);state=r(o+44);keys=r(state+8)
    if u(b+28)!=1 or struct.unpack_from('<3H',data,o+32)!=(0,0,nv):raise ValueError('Selected material/local vertex run changed')
    if struct.unpack_from('<2i',data,state)!=(-1,6):raise ValueError('Nonconstant/hierarchical pose deferred')
    words=list(struct.unpack_from('<6I',data,keys))
    if [x&255 for x in words]!=[8,12,16,20,24,28]:raise ValueError('Nonconstant scalar pose deferred')
    if struct.unpack_from('<4H',data,r(o+40))!=(0,0,0,nf):raise ValueError('Authored face run changed')
    texture=r(b+40);pixels=r(r(texture+108));h,w=struct.unpack_from('<2I',data,texture+8);pf=struct.unpack_from('<8I',data,texture+72)
    if u(texture+116)!=1 or pf[3]!=16:raise ValueError('Texture animation/unsupported format deferred')
    if pf[4:8]==(0xf00,0xf0,0xf,0xf000):fmt='ARGB4444'
    elif pf[4:8]==(0xf800,0x7e0,0x1f,0):fmt='RGB565'
    else:raise ValueError('Unexpected raw texture masks')
    tag=r(b+56);ts,tf=struct.unpack_from('<2i',data,tag);tn=data[r(tag+8):].split(b'\0')[0].decode();params=data[r(tag+12):].split(b'\0')[0].decode()
    if (ts,tf,tn)!=(0,0,'scrolltex'):raise ValueError('Different controller/gating deferred')
    vb=data[v:v+nv*32];fb=data[f:f+nf*6];raw=data[pixels:pixels+w*h*2];indices=list(struct.unpack('<'+'H'*(nf*3),fb))
    if len(vb)!=nv*32 or len(raw)!=w*h*2 or max(indices)>=nv:raise ValueError('Truncated mesh/texture')
    return dict(vertices=[list(x)for x in struct.iter_unpack('<8f',vb)],indices=indices,keys=words,vertex_hex=vb.hex(),face_hex=fb.hex(),frames=struct.unpack_from('<h',data,70)[0],width=w,height=h,texture=raw,format=fmt,object_name=data[o:o+32].split(b'\0')[0].decode(),tag_state=ts,tag_frame=tf,tag_name=tn,tag_params=params,texture_sha256=sha(raw),vertices_offset=v,faces_offset=f,keys_offset=keys,texture_offset=pixels)


class ScrollFixture(StaticFixture):
    def __init__(self,image):
        super().__init__(image,WIDTH,HEIGHT,20000000);v=self.vm;self.animator=v.allocate(0x100);self.owner=v.allocate(0x100);self.drawobj=v.allocate(0x200);self.drawverts=v.allocate(128*32);self.drawptrs=v.allocate(4);self.controller=0;self.pre_fallback=None
        v.put_u32(self.drawptrs,self.drawobj);v.put_u32(self.animator+8,self.context);v.put_u32(self.animator+0x44,1);v.put_u32(self.animator+0x54,self.drawptrs);v.put_u32(self.context+0x64,1);v.put_u32(self.drawobj+0xa4,self.drawverts)
        for a in(0x40eef0,0x40a0c0):v.uc.hook_add(UC_HOOK_CODE,self.boundary,begin=a,end=a)
        v.uc.hook_add(UC_HOOK_CODE,self.observe_fallback,begin=0x401839,end=0x401839)
    def boundary(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Unknown native animator object')
            uc.reg_write(UC_X86_REG_EAX,self.drawobj)
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+(4 if address==0x40eef0 else 8))
    def observe_fallback(self,uc,address,size,user):self.pre_fallback=self.vm.u32(uc.reg_read(UC_X86_REG_EDI)+0x18)
    def setup_scroll(self,m):
        self.setup(m,checkpoint=False);v=self.vm;imageobj=v.u32(v.u32(self.context+0x74));v.write(imageobj,m['object_name'].encode()+b'\0');v.put_u32(self.drawobj+0xa0,len(m['vertices']));v.write(self.drawverts,bytes.fromhex(m['vertex_hex']));params=m['tag_params'].encode();p=v.allocate(len(params)+1);v.write(p,params+b'\0');self.pre_fallback=None
        self.controller=v.call(0x4059e0,(m['tag_state'],m['tag_frame'],self.animator,self.context,self.owner));ok=v.call(0x401820,(p,),this=self.controller)
        if ok!=1 or self.pre_fallback not in(0,1)or v.u32(self.controller+0x18)!=1:raise AssertionError('Original selector/init rejected')
        original=v.u32(v.u32(self.controller+0x34));copied=bytes(v.uc.mem_read(original,8*len(m['vertices'])))
        if copied!=b''.join(struct.pack('<2f',*x[6:8])for x in m['vertices']):raise AssertionError('Native Initialize failed original UV copy')
        self.rates=list(struct.unpack('<2f',v.uc.mem_read(self.controller+0x2c,8)));self.software.checkpoint()
    def original_uvs(self,tick,owner_frame):
        self.vm.write(self.owner+0x5c,struct.pack('<h',owner_frame));self.vm.put_u32(0x65cb38,tick);self.vm.call(0x401990,this=self.controller)
        return [list(struct.unpack('<2f',self.vm.uc.mem_read(self.drawverts+i*32+24,8)))for i in range(len(self.metadata['vertices']))]


def build_controller(output,metadata,source=None,label='current'):
    pre=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <strings.h>
#define stricmp strcasecmp
int warnings=0;void log_warn(const char*,...){++warnings;}void log_info(const char*,...){}
struct Ref{const char*value;void*ptr(){return const_cast<char*>(value);}};struct S3DImageryTag{int state,frame;Ref name,str;};struct S3DImageryBody{int version=3,numtags=1;std::vector<S3DImageryTag>tags;};
struct Obj{const char*name;};struct Objects{std::vector<Obj>rows;int NumItems()const{return int(rows.size());}bool Used(int)const{return true;}const Obj&operator[](int i)const{return rows[i];}};
struct T3DImagery{bool meshinitialized=true;Objects objects;struct SScrollTexTrack{int state=0,tagframe=0,object=-1;float du=0,dv=0;};std::vector<SScrollTexTrack>scrolltex_tracks;
 int NumObjects()const{return objects.NumItems();}const char*GetResFilename(){return "shipped-scroll-water";}void*GetBody(){std::abort();}void InitializeMesh(S3DImageryBody*){std::abort();}
 int GetObjectNum(char*);void InitializeScrollTexTracks(S3DImageryBody*);bool ScrollTexOffset(int,int,int,int64_t,float[2])const;};
'''
    getter=span((ROOT/'src/3dimage.cpp').read_text(),'int32_t T3DImagery::GetObjectNum(','char* T3DImagery::GetObjectName(').replace('int32_t T3DImagery','int T3DImagery')
    trailer='int main(){\n'
    for name,m in metadata.items():
        trailer+='{T3DImagery image;image.objects.rows={{'+json.dumps(m['object_name'])+'}};S3DImageryBody body;body.tags={{0,0,{"scrolltex"},{'+json.dumps(m['tag_params'])+'}}};image.InitializeScrollTexTracks(&body);\n'
        trailer+='for(int tick:{'+','.join(map(str,TICKS))+'}){float uv[2];bool active=image.ScrollTexOffset(0,0,tick%'+str(m['frames'])+',tick,uv);printf("'+name+' %d %d %zu %.9g %.9g\\n",tick,int(active),image.scrolltex_tracks.size(),uv[0],uv[1]);}}\n'
    trailer+='}\n';src=output/f'controller-{label}.cpp';src.write_text(pre+'\n'+getter+'\n'+controller_span(source)+'\n'+trailer);binary=output/f'controller-{label}';cmd=['clang++','-std=c++17',str(src),'-o',str(binary)];r=subprocess.run(cmd,capture_output=True,text=True);(output/f'controller-{label}-compile.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError('Actual production controller compilation failed')
    trace=subprocess.check_output([str(binary)],text=True);(output/f'controller-{label}-trace.txt').write_text(trace);rows={}
    for line in trace.splitlines():
        p=line.split();rows[(p[0],int(p[1]))]=dict(active=bool(int(p[2])),tracks=int(p[3]),offset=list(map(float,p[4:6])))
    return rows,dict(command=cmd,source_sha256=sha(src.read_bytes()),binary_sha256=sha(binary.read_bytes()))



def build_scroll_mesh(output,metadata):
    # Start from the tested generic mesh boundary, then execute the new actual
    # production Submit's scroll_imagery_ branch with actual controller methods.
    _,base=build_port(output,metadata,frames=POSE_FRAMES)
    code=(output/'static-port-components.cpp').read_text()
    extras=r'''
#include <string>
#include <cctype>
#include <strings.h>
#undef stricmp
#define stricmp strcasecmp
void log_warn(const char*,...){};void log_info(const char*,...){}
struct Ref{const char*value;void*ptr(){return const_cast<char*>(value);}};
struct ScrollTagFixture{int state,frame;Ref name,str;};struct ScrollBodyFixture{int version=3,numtags=1;std::vector<ScrollTagFixture>tags;};
'''
    code=code.replace('struct T3DImagery {',extras+'\nstruct T3DImagery {')
    code=code.replace('struct S3DObj {int* numanikeys;void** anikeys;};','struct S3DObj {int* numanikeys;void** anikeys;char name[32]{};};struct S3DObjCollection{S3DObj rows[1];int NumItems()const{return 1;}bool Used(int)const{return true;}S3DObj&operator[](int i){return rows[i];}};')
    code=code.replace('S3DObj objects[1];','S3DObjCollection objects;')
    code=code.replace('bool ScrollTexOffset(int,int,int,int64_t,float*)const{throw std::runtime_error("Unexpected scroll in static fixture");}',r'''struct SScrollTexTrack{int state=0,tagframe=0,object=-1;float du=0,dv=0;};std::vector<SScrollTexTrack>scrolltex_tracks;int GetObjectNum(char*);void InitializeScrollTexTracks(ScrollBodyFixture*);bool ScrollTexOffset(int,int,int,int64_t,float*)const;const char*GetResFilename(){return "audited-scroll-water";}''')
    code=code.replace('struct TTime{static int64_t LegacyFrameCount(){return 0;}};','int64_t global_tick=0;struct TTime{static int64_t LegacyFrameCount(){return global_tick;}};')
    getter=span((ROOT/'src/3dimage.cpp').read_text(),'int32_t T3DImagery::GetObjectNum(','char* T3DImagery::GetObjectName(')
    where=code.index('void TAuthoredStaticMeshEffect::SubmitWorldMeshForTest_BESPOKE(')
    code=code[:where]+getter+'\n'+controller_span().replace('S3DImageryBody*','ScrollBodyFixture*').replace('S3DImageryTag&','ScrollTagFixture&')+'\n'+code[where:]
    init='img.key_count=nk;img.key_pointer=img.keys.data();img.objects[0]={&img.key_count,&img.key_pointer};'
    code=code.replace(init,init+'std::strncpy(img.objects[0].name,argv[2],31);ScrollBodyFixture body;body.tags={{0,0,{"scrolltex"},{argv[3]}}};img.InitializeScrollTexTracks(&body);')
    old='for(int frame:{'+','.join(map(str,POSE_FRAMES))+'})for(int moved:{0,1}){'
    code=code.replace(old,'for(int tick:{'+','.join(map(str,TICKS))+'})for(int moved:{0,1}){int frame=tick%frames;global_tick=tick;')
    code=code.replace('effect.parts_.resize(1); BuildStaticObjectMatrix','effect.parts_.resize(1);effect.frame_=frame;effect.scroll_imagery_=&img;effect.parts_[0].object_index=0; BuildStaticObjectMatrix')
    code=code.replace('frame,moved','tick,moved').replace('v.uv[0],v.uv[1]','v.uv[0]+renderer.last.uv_offset[0],v.uv[1]+renderer.last.uv_offset[1]')
    src=output/'scroll-mesh-production.cpp';src.write_text(code);binary=output/'scroll-mesh-production';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(src),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    r=subprocess.run(cmd,capture_output=True,text=True);(output/'scroll-mesh-compile.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError('Combined actual Submit/controller compile failed')
    traces={}
    for name,m in metadata.items():
        trace=subprocess.check_output([str(binary),str(output/(name+'.mesh-input')),m['object_name'],m['tag_params']],text=True);(output/(name+'-scroll-mesh-trace.txt')).write_text(trace);cases={};indices=[]
        for line in trace.splitlines():
            row=line.split()
            if row[0]=='I':indices=list(map(int,row[1:]))
            elif row[0]=='M':cases[(int(row[1]),int(row[2]))]=dict(matrix=list(map(float,row[3:])),vertices=[])
            elif row[0]=='V':cases[(int(row[1]),int(row[2]))]['vertices'].append(list(map(float,row[4:])))
        traces[name]=dict(indices=indices,cases=cases)
    return traces,dict(command=cmd,source_sha256=sha(src.read_bytes()),binary_sha256=sha(binary.read_bytes()),generic_boundary=base,actual_scroll_submit_branch=True)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    profiles=json.loads(Path(__file__).with_name('scrolltex_water_profiles.json').read_text())['profiles'];metadata={}
    with zipfile.ZipFile(args.archive)as z:
        names={n.lower():n for n in z.namelist()}
        for row in profiles:metadata[row['name']]=parse_asset(z.read(names[('Imagery/'+row['asset']).lower()]),row)
    before={k:sha(v.encode())for k,v in mesh_spans().items()};before['scrolltex']=sha(controller_span().encode());port,mesh=build_scroll_mesh(args.output,metadata);controller,compiled=build_controller(args.output,metadata);baseline=subprocess.check_output(['git','show','466fbba:src/3dimage.cpp'],cwd=ROOT,text=True);old,oldcompile=build_controller(args.output,metadata,source=baseline,label='before-fallback-fix');fixture=ScrollFixture(args.executable);dispatch=original_registry(fixture.vm,list(metadata));cases=[];failures=[];times=[];pre={}
    for row in profiles:
        name=row['name'];m=metadata[name];fixture.setup_scroll(m);pre[name]=dict(selected_before_fallback=fixture.pre_fallback,selected_after_fallback=fixture.vm.u32(fixture.controller+0x18),rates=fixture.rates,object=m['object_name'],tag=m['tag_params'])
        for tick in TICKS:
            frame=tick%m['frames']
            for moved in(0,1):
                first=None
                for repeat in range(2):
                    start=time.perf_counter();fixture.software.restore();matrix,points=fixture.original(frame,moved);uvs=fixture.original_uvs(tick,frame);packet=port[name]['cases'][(tick,moved)];candidate=[v[:3]for v in packet['vertices']];baseuv=[v[6:8]for v in packet['vertices']];off=controller[(name,tick)]['offset'];candidate_uv=baseuv
                    if len(points)!=len(candidate)or len(uvs)!=len(candidate_uv):raise AssertionError('Mesh/controller cardinality mismatch')
                    position=max(abs(a-b)for p,q in zip(points,candidate)for a,b in zip(p,q));uv_exact=all(f32(a)==f32(b)for u,v in zip(uvs,candidate_uv)for a,b in zip(u,v));original,z=fixture.pixels(points,uvs,m['indices']);modern,mz=fixture.pixels(candidate,candidate_uv,port[name]['indices']);hashes=[sha(original),sha(modern),sha(z),sha(mz)];times.append((time.perf_counter()-start)*1000)
                    if first is None:first=hashes
                    elif first!=hashes:raise AssertionError('Warm native controller/mesh image changed')
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',original),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',modern)));visible=sum(x!=0 for x in struct.unpack('<'+str(WIDTH*HEIGHT)+'H',original));errors=[]
                if position>5e-5:errors.append('geometry')
                if not uv_exact:errors.append('controller UV')
                if diff or z!=mz:errors.append('image/depth')
                if not visible:errors.append('invisible')
                if errors:failures.append(dict(name=name,tick=tick,moved=moved,errors=errors))
                if not moved:
                    save_rgb565_png(args.output/f'{name}-retail-{tick:03d}.png',original,WIDTH,HEIGHT);save_rgb565_png(args.output/f'{name}-port-{tick:03d}.png',modern,WIDTH,HEIGHT)
                cases.append(dict(name=name,type_id=row['id'],global_tick=tick,owner_frame=frame,moved=moved,geometry_error=position,uv_bit_exact=uv_exact,hashes=hashes,differing_rgb565_pixels=diff,depth_equal=z==mz,nonzero_native_pixels=visible,native_uvs=uvs,port_uvs=candidate_uv,production_offset=off,before_fix_offset=old[(name,tick)],errors=errors))
    after={k:sha(v.encode())for k,v in mesh_spans().items()};after['scrolltex']=sha(controller_span().encode())
    if after!=before:raise AssertionError('Relevant source changed while running')
    report=dict(status='pass'if not failures else'fail',failures=failures,profiles=profiles,native_selector_initialization=pre,case_count=len(cases),replays_per_case=2,median_warm_pair_ms=statistics.median(times),native_dispatch=dispatch,production_mesh=mesh,production_controller=compiled,before_fix_controller=oldcompile,source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),retail_sha256=RETAIL_SHA,cases=cases,scope='Actual original scrolltex factory/full tag parser/Initialize/Render, original copied authored UVs and native constant keys/matrices versus compiled actual parser/GetObjectNum/InitializeScrollTexTracks/ScrollTexOffset inside actual generic mesh extraction/Submit scroll-imagery branch. Fixed1280x720 RAM viewport, unchanged z distance1925 and camera; common original software raster, white input lighting, two selected owner positions, legal ticks0..120. Copy-instance-vertices imagery adapter, map install/sampler/native device/Metal and long-clock precision separate.',dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if failures:raise SystemExit(1)

if __name__=='__main__':main()
