"""Original static key/mesh plus actual texture-frame selector vs shared preview.

Input owner frames0..3 are explicit fixture inputs; natural owner cadence is
separate. Native40c520 executes its copyframes=false opaque-handle branch.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
from static_mesh_probe import ROOT,parse_asset,build_port,StaticFixture,production_spans
from software_probe import save_rgb565_png,RETAIL_SHA


def parse_frames(data,profile):
    m=parse_asset(data,profile);u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a)
    body=20+u(16);texture=r(body+40);offsets=r(texture+108)
    m['texture_frame_count']=u(texture+116);m['frames']=struct.unpack_from('<h',data,28+42)[0]
    m['state_name']=data[28:60].split(b'\0')[0].decode();m['state_flags']=struct.unpack_from('<h',data,28+40)[0]
    expected=(profile['header_states'],profile['header_state_name'],profile['header_animation_flags'],profile['header_frames'],profile['texture_frames'])
    actual=(u(24),m['state_name'],m['state_flags']&0xffff,m['frames'],m['texture_frame_count'])
    if actual!=expected:raise ValueError(f"{profile['name']}: exact header cardinality mismatch {actual} != {expected}")
    if profile['active_texture_frames']!=list(range(m['frames']))or m['frames']>m['texture_frame_count']:raise ValueError('Active frame contract mismatch')
    m['frame_offsets']=[];m['textures']=[]
    for frame in range(m['texture_frame_count']):
        pixel=r(offsets+4*frame)
        raw=data[pixel:pixel+m['width']*m['height']*2]
        if len(raw)!=m['width']*m['height']*2:raise ValueError('Truncated original texture')
        m['frame_offsets'].append(pixel);m['textures'].append(raw)
    return m


class TextureFixture(StaticFixture):
    def __init__(self,exe):
        super().__init__(exe,512,512,5000000)
        self.texture_record=self.vm.allocate(0x100);self.texture_slots=self.vm.allocate(4)
        self.texture_surfaces=self.vm.allocate(32);self.texture_handles=self.vm.allocate(32)

    def setup_frames(self,m):
        self.setup(m,checkpoint=False);vm=self.vm
        vm.put_u32(self.context+0x4c,1);vm.put_u32(self.context+0x5c,self.texture_slots)
        vm.put_u32(self.texture_slots,self.texture_record);vm.put_u32(self.texture_record+0x7c,m['texture_frame_count'])
        vm.put_u32(self.texture_record+0x80,0xffffffff);vm.put_u32(self.texture_record+0x84,self.texture_surfaces)
        vm.put_u32(self.texture_record+0x88,self.texture_handles);vm.put_u32(self.texture_record+0x8c,0)
        vm.put_u32(self.texture_record+0x90,1000);vm.put_u32(self.texture_record+0x94,1000)
        for f in range(m['texture_frame_count']):vm.put_u32(self.texture_surfaces+4*f,1000+f);vm.put_u32(self.texture_handles+4*f,1000+f)
        vm.put_u32(self.frame_count_value,m['frames'])
        self.software.checkpoint()

    def select(self,frame):
        result=self.vm.call(0x40c520,(0,frame),this=self.context)
        if result!=1:raise AssertionError('Native frame selector failed')
        return self.vm.u32(self.texture_record+0x94)

    def pixels_for(self,points,uvs,indices,frame,m):
        self.software.set_texture(m['width'],m['height'],m['textures'][frame],format=m['format'])
        return self.pixels(points,uvs,indices)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--profiles',type=Path,default=Path(__file__).with_name('static_additional_profiles.json'));p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    profiles=json.loads(args.profiles.read_text())['profiles'];metadata={}
    with zipfile.ZipFile(args.archive)as z:
        names={n.lower():n for n in z.namelist()}
        for row in profiles:metadata[row['name']]=parse_frames(z.read(names[('Imagery/'+row['asset'].replace('\\','/')).lower()]),row)
    before={k:hashlib.sha256(v.encode()).hexdigest()for k,v in production_spans().items()}
    port,compiled=build_port(args.output,metadata,frames=(0,1,2,3),texture_frames=True);fixture=TextureFixture(args.executable)
    cases=[];errors=[];selector_checks=0;packet_checks=0;warm=0
    for row in profiles:
        name=row['name'];m=metadata[name];fixture.setup_frames(m);uvs=[v[6:8]for v in m['vertices']]
        if port[name]['indices']!=m['indices']:raise AssertionError('Production changed authored topology')
        for frame in range(m['frames']):
            for moved in(0,1):
                first=None
                for replay in range(2):
                    fixture.software.restore();native_handle=fixture.select(frame);matrix,points=fixture.original(frame,moved);packet=port[name]['cases'][(frame,moved)]
                    port_handle=packet['texture_handle'];selector_checks+=1
                    if native_handle!=port_handle:errors.append(dict(name=name,frame=frame,field='texture_handle',original=native_handle,port=port_handle))
                    candidate=[v[:3]for v in packet['vertices']];candidate_uvs=[v[6:8]for v in packet['vertices']]
                    for a,b in zip(points,candidate):
                        for x,y in zip(a,b):
                            packet_checks+=1
                            if abs(x-y)>3e-5:errors.append(dict(name=name,frame=frame,field='position',original=x,port=y))
                    for a,b in zip(uvs,candidate_uvs):
                        for x,y in zip(a,b):
                            packet_checks+=1
                            if abs(x-y)>1e-7:errors.append(dict(name=name,frame=frame,field='uv',original=x,port=y))
                    original,depth=fixture.pixels_for(points,uvs,m['indices'],native_handle-1000,m)
                    candidate_image,candidate_depth=fixture.pixels_for(candidate,candidate_uvs,port[name]['indices'],port_handle-1000,m)
                    sha=lambda b:hashlib.sha256(b).hexdigest();hashes=[sha(original),sha(candidate_image),sha(depth),sha(candidate_depth)]
                    if first is None:first=hashes
                    elif first!=hashes:raise AssertionError('Texture frame image/depth warm replay changed')
                    if replay:warm+=1;continue
                    pixels=sum(a!=b for a,b in zip(struct.unpack('<262144H',original),struct.unpack('<262144H',candidate_image)))
                    if pixels:errors.append(dict(name=name,frame=frame,field='pixels',different=pixels))
                    if depth!=candidate_depth:errors.append(dict(name=name,frame=frame,field='depth'))
                    save_rgb565_png(args.output/f'{name}-retail-F{frame}-P{moved}.png',original,512,512)
                    save_rgb565_png(args.output/f'{name}-port-F{frame}-P{moved}.png',candidate_image,512,512)
                    cases.append(dict(name=name,type_id=row['id'],frame=frame,owner_translated=bool(moved),native_handle=native_handle,port_handle=port_handle,hashes=hashes,pixel_differences=pixels,
                        native_vertices=points,port_vertices=candidate,texture_frame_offset=hex(m['frame_offsets'][frame]),texture_frame_sha256=sha(m['textures'][frame])))
    after={k:hashlib.sha256(v.encode()).hexdigest()for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Relevant production source changed')
    report=dict(status='pass'if not errors else'differences_found',errors=errors,compiled_production=compiled,source_span_sha256=after,probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),profiles_sha256=hashlib.sha256(args.profiles.read_bytes()).hexdigest(),checkout=str(ROOT),
        profile_count=len(profiles),selector_checks=selector_checks,packet_checks=packet_checks,image_depth_pairs=len(cases),exact_original_warm_image_depth_replays=warm,cases=cases,
        profiles={name:dict(header_frames=m['frames'],state_name=m['state_name'],animation_flags=m['state_flags'],texture_frames=m['texture_frame_count'],frame_offsets=list(map(hex,m['frame_offsets'])),active_frames=list(range(m['frames'])),stored_inactive_frames=list(range(m['frames'],m['texture_frame_count'])),frame_sha256=[hashlib.sha256(raw).hexdigest()for raw in m['textures']],distinct_stored_frame_hashes=len(set(hashlib.sha256(raw).hexdigest()for raw in m['textures'])),asset_sha256=next(r['asset_sha256']for r in profiles if r['name']==name))for name,m in metadata.items()},
        original_functions=['0x40c520','0x40a420','0x409950','0x43aa90','0x43ad80','0x56d960'],retail_sha256=RETAIL_SHA,
        scope='Exact STILL header/constant pose and every active authored texture frame: StillWater1,watchers4each. Native original texture selector uses explicit copyframes=false opaque surface/texture handles; candidate actual generic SubmitMesh picks registered per-frame mesh. Original matrix/raster and selected owner translation, white lighting/cullNONE/depthtest-write. Natural owner cadence, default-builder/caller/map placement and modernGPU parity are separate; no texture fitting.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
