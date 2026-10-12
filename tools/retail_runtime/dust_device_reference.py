"""Qualified Dust D3D device: original packets, semantic RGB8 modulation/addition.

This is explicitly NOT the original software rasterizer or a period GPU. The
original hardware dispatch and COM texture are executed; its original software
transform supplies source-domain screen positions. Point/linear sampling and an
8-bit linear UNORM target are declared device controls, not inferred from the
cached software-only startup. No engine/port rendering code is imported.
"""
import argparse, hashlib, json, math, struct
from pathlib import Path
from dust_loaded_context_probe import DustLoadedScene
from character_native_preflight import ROOT, sha
from unicorn.x86_const import UC_X86_REG_ESI, UC_X86_REG_EDI
from PIL import Image

def digest(data): return hashlib.sha256(data).hexdigest()

def rgb565(word): return ((word>>11)/31., ((word>>5)&63)/63., (word&31)/31.)
def modulate(texel, diffuse): return tuple(t*d/255. for t,d in zip(rgb565(texel),diffuse))

def raster(packets, filtering, width=640, height=480, legacy_color=False, sample_center=0):
    """Independent affine orthographic triangle raster, integer-center top-left.

    Packed diffuse is interpolated in RGB8, texture is normalized from its actual
    RGB565 descriptor, then MODULATE and ONE/ONE are evaluated. Each fragment
    rounds to the declared RGB8 UNORM render target; alpha cannot affect ONE/ONE.
    """
    framebuffer=bytearray(width*height*3)
    for p in packets:
        vertices=p['projected']; raw=bytes.fromhex(p['vertices_hex'])
        colors=[tuple((struct.unpack_from('<I',raw,i*32+16)[0]>>shift)&255 for shift in(16,8,0)) for i in range(len(vertices))]
        if legacy_color: colors=[tuple((c>>3)*255/31 for c in rgb) for rgb in colors]
        uv=[struct.unpack_from('<2f',raw,i*32+24) for i in range(len(vertices))]
        texture=list(struct.unpack('<'+'H'*(p['texture_width']*p['texture_height']),bytes.fromhex(p['texture_hex'])))
        tw,th=p['texture_width'],p['texture_height']
        def sample(u,v):
            if filtering=='point':return rgb565(texture[(int(math.floor(v*th))%th)*tw+int(math.floor(u*tw))%tw])
            x,y=u*tw-.5,v*th-.5; ix,iy=math.floor(x),math.floor(y); fx,fy=x-ix,y-iy
            values=[rgb565(texture[((iy+dy)%th)*tw+(ix+dx)%tw]) for dy in(0,1) for dx in(0,1)]
            return tuple(sum(values[k][c]*weight for k,weight in enumerate(((1-fx)*(1-fy),fx*(1-fy),(1-fx)*fy,fx*fy)))for c in range(3))
        for tri in zip(*[iter(p['indices'])]*3):
            ids=list(tri); pts=[vertices[i][:2] for i in ids]
            def edge(a,b,x,y):return (b[0]-a[0])*(y-a[1])-(b[1]-a[1])*(x-a[0])
            area=edge(pts[0],pts[1],*pts[2])
            if area==0:continue
            if area<0:ids[1],ids[2]=ids[2],ids[1];pts[1],pts[2]=pts[2],pts[1];area=-area
            edges=[(pts[1],pts[2]),(pts[2],pts[0]),(pts[0],pts[1])]
            include=[b[1]<a[1] or (b[1]==a[1] and b[0]>a[0]) for a,b in edges]
            for y in range(max(0,math.ceil(min(p[1]for p in pts)-sample_center)),min(height-1,math.floor(max(p[1]for p in pts)-sample_center))+1):
                for x in range(max(0,math.ceil(min(p[0]for p in pts)-sample_center)),min(width-1,math.floor(max(p[0]for p in pts)-sample_center))+1):
                    w=[edge(a,b,x+sample_center,y+sample_center)for a,b in edges]
                    if any(e<0 or(e==0 and not flag)for e,flag in zip(w,include)):continue
                    w=[e/area for e in w];z=sum(w[k]*vertices[ids[k]][2]for k in range(3))
                    if not 0<=z<=65535:continue # declared clear depth, no writes
                    u,v=(sum(w[k]*uv[ids[k]][c]for k in range(3))for c in range(2));tex=sample(u,v)
                    for c in range(3):
                        diffuse=sum(w[k]*colors[ids[k]][c]for k in range(3));off=(y*width+x)*3+c
                        framebuffer[off]=min(255,int(math.floor(framebuffer[off]+tex[c]*diffuse+.5)))
    return bytes(framebuffer)

class DustDevice(DustLoadedScene):
    def __init__(self,exe,boot,name="Dust",source=(10000,10000,16)):
        super().__init__(exe,boot,0,38,source,name=name)
        self.packets=[];self.device_states={};self.stage_states={};self.bound_textures={};self.matrix=None
        original=self.world.ddraw._device_method;v=self.vm
        def device(interface,method,args):
            if method=='SetRenderState':self.device_states[args[1]]=args[2]
            if method=='SetTextureStageState':self.stage_states[(args[1],args[2])]=args[3]
            if method=='SetTransform' and args[1]==1:self.matrix=bytes(v.uc.mem_read(args[2],64))
            if method=='SetTexture':self.bound_textures[args[1]]=args[2]
            if method=='GetTexture':v.put_u32(args[2],self.bound_textures.get(args[1],0));return 0
            if method=='GetClipStatus':v.write(args[1],struct.pack('<I6f',2,0,0,0,0,0,0));return 0
            if method=='DrawIndexedPrimitive':
                _,primitive,fvf,vertices,count,indices,ni,flags=args
                assert (primitive,fvf,count,ni,flags)==(4,0x1e2,4,6,0)
                assert all(self.device_states.get(k)==value for k,value in ((27,1),(19,2),(20,2),(22,1),(14,0),(7,1)))
                assert all(self.stage_states.get((0,k))==value for k,value in ((1,4),(2,2),(3,0)))
                d=self.world.ddraw;surface=d.surface(d.textures[self.bound_textures[0]])
                assert struct.unpack('<8I',surface.pixel_format)==(32,64,0,16,0xf800,0x7e0,31,0)
                texture=b''.join(bytes(v.uc.mem_read(surface.memory+y*surface.pitch,surface.width*2))for y in range(surface.height))
                self.packets.append(dict(vertices_hex=bytes(v.uc.mem_read(vertices,count*32)).hex(),indices=list(struct.unpack('<6H',v.uc.mem_read(indices,12))),world_matrix_hex=self.matrix.hex(),texture_hex=texture.hex(),texture_width=surface.width,texture_height=surface.height,states=dict(self.device_states),stages={str(k):value for k,value in self.stage_states.items()}))
                return 0
            return original(interface,method,args)
        self.world.ddraw._device_method=device
        v.put_u32(0x5d7a28,0) # explicit qualified device route, original4174fd dispatch
        # Execute original scene state producer. Success path uses ESI=scene,
        # EDI=0; stop before unrelated lighting traversal and stack-frame locals.
        v.uc.reg_write(UC_X86_REG_ESI,0x65a57c);v.uc.reg_write(UC_X86_REG_EDI,0)
        v.call(0x4132c7,stop_address=0x413342)
        assert self.device_states[27]==1
        self.matrix_buffer=v.allocate(64)
    def render_device(self):
        self.packets=[];super().render();v=self.vm;s=self.software
        # Transform exact saved hardware vertices by the original transform,
        # zero indices prevents any original raster from manufacturing pixels.
        for p in self.packets:
            v.write(s.authored,bytes.fromhex(p['vertices_hex']));v.write(self.matrix_buffer,bytes.fromhex(p['world_matrix_hex']))
            v.call(0x56d5f0,(1,self.matrix_buffer));v.put_u32(0x675fb8,s.vertices+4*36)
            v.call(0x56eb30,(4,0x1e2,s.authored,4,s.indices,0,0))
            transformed=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(s.vertices,4*36)))
            p['projected']=[[row[0],-row[1],row[2]]for row in transformed]
        return self.packets

def run(exe,boot,out):
    out.mkdir(parents=True,exist_ok=False);f=DustDevice(exe,boot);frames=[]
    for tick in range(25):
        if tick:f.step(tick)
        packets=f.render_device();images={}
        for mode in ('point','linear'):
            raw=raster(packets,mode);path=out/f'{mode}-{tick:03}.png';Image.frombytes('RGB',(640,480),raw).save(path)
            images[mode]=dict(path=str(path),sha256=sha(path),nonzero_pixels=sum(any(raw[i:i+3])for i in range(0,len(raw),3)),max_channel=max(raw))
        frames.append(dict(tick=tick,state=f.state(),packets=packets,images=images))
        print(tick,len(packets),images['point']['nonzero_pixels'],images['linear']['nonzero_pixels'],flush=True)
    report=dict(scope=__doc__,qualified=True,unchanged_retail_device=False,visual_pass=False,source_position=[10000,10000,16],camera=[10000,10000,0],screen_center=[320,320],dimensions=[640,480],target='RGB8_UNORM-linear-round-nearest-per-fragment',filtering='point and linear explicit alternatives; software boot did not initialize hardware filtering',executable_sha256=sha(exe),boot_sha256=sha(boot),tool_sha256=sha(__file__),frames=frames)
    (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
def software_control(exe,boot,reference,out):
    out.mkdir(parents=True,exist_ok=False);expected=json.loads(reference.read_text());f=DustLoadedScene(exe,boot,0,38,(10000,10000,16));rows=[]
    for tick in range(25):
        if tick:f.step(tick)
        raw=f.render();state=f.state();assert state==expected['frames'][tick]['state'],tick;assert not any(raw),tick
        rows.append(dict(tick=tick,state=state,blank=True))
    report=dict(status='pass',states_exact=25,software_blank=25,reference_sha256=sha(reference),executable_sha256=sha(exe),boot_sha256=sha(boot),tool_sha256=sha(__file__),rows=rows)
    (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('25 exact states/pool hashes/RNG;25 blank original software frames')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--software-control-reference',type=Path);a=p.parse_args();exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    if a.software_control_reference:software_control(exe,a.boot_image,a.software_control_reference,a.output)
    else:run(exe,a.boot_image,a.output)
