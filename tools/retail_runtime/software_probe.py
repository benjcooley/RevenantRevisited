#!/usr/bin/env python3
"""Execute the original retail software triangle dispatcher into RGB565 RAM.

This fixture supplies transformed vertices and a minimal locked texture surface.
Original culling, float conversion, depth selection, interpolation, texture fetch,
lighting and pixel writes execute from the verified EXE. No raster is intercepted.
It does not yet execute effect animation, asset decoding, HUD or game startup.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import time
import zlib
from runtime import Runtime, MissingAPI
from build_contract import verify_build

RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'


class SoftwareFixture:
    """Independent Runtime and surfaces: workers never share mutable guest state."""
    def __init__(self,executable,width=64,height=64,build=None):
        self.build=verify_build(executable,build)
        self.vm=Runtime(executable)
        if hashlib.sha256(self.vm.image).hexdigest()!=self.build['executable_sha256']:
            raise ValueError('Executable changed while creating the software fixture')
        # Actual retail CRT heap/thread initialization. Stop before locale and
        # static game constructors, then original table builders can allocate.
        self.vm.call(0x58ed0d,stop_address=0x58ed8e)
        if width%2 or height<1:raise ValueError('The fixture requires an even positive width')
        self.width=width;self.height=height
        self.screen=self.vm.surface('screen',width,height)
        self.depth=self.vm.surface('depth',width,height)
        self.camera=self.vm.allocate(0x3c)
        self.vertices=self.vm.allocate(4096*36)
        self.indices=self.vm.allocate(16384*2)
        self.texture=None;self.texture_object=self.vm.allocate(4)
        self.vtable=self.vm.allocate(33*4)
        self.vm.put_u32(self.texture_object,self.vtable)
        self.com_trace=[];self.locked=False;self.references=1
        # The existing virtual API trap registry is reused for these explicitly
        # named COM fixture methods; they are not actual kernel32 exports.
        for slot in range(33):
            name=f'SoftwareProbeSurfaceUnsupported_{slot}'
            def unsupported(args,slot=slot):
                raise MissingAPI(f'Software probe surface method {slot} is unsupported')
            self.vm.handlers[name]=(1,unsupported)
            self.vm.put_u32(self.vtable+4*slot,self.vm.api_address('kernel32.dll',name))
        for slot,name,argc,method in [(1,'AddRef',1,self._addref),(2,'Release',1,self._release),
            (21,'GetPixelFormat',2,self._pixel_format),(25,'Lock',5,self._lock),(32,'Unlock',2,self._unlock)]:
            label='SoftwareProbeSurface'+name
            self.vm.handlers[label]=(argc,method)
            self.vm.put_u32(self.vtable+4*slot,self.vm.api_address('kernel32.dll',label))
        self.vm.call(0x54dd40,this=self.camera)  # Actual constructor.
        self.vm.call(0x54dd70,(0,width-1,0,height-1,width),this=self.camera)
        self.vm.call(0x54de20,(self.screen.address,self.depth.address),this=self.camera)
        self.vm.call(0x54df10,(0xf81f,0x07e0),this=self.camera)
        # Actual RGB565 modulation table generation, not a host transcription.
        self.vm.call(0x56c5a0,(65536,),this=0x670690)
        self.vm.call(0x54df30,(0x670690,0x670e90,0x672e90),this=self.camera)
        self.vm.call(0x56ca60,(65536,),this=0x675ea0,instruction_limit=5000000)
        self.vm.call(0x54df90,(self.vm.u32(0x675ea0),),this=self.camera)
        self.vm.put_u32(0x676044,self.camera)
        self.vm.put_u32(0x675fe4,0);self.vm.put_u32(0x675fe8,width)
        self.vm.put_u32(0x675fec,0);self.vm.put_u32(0x675fe0,height)
        self.vm.put_u32(0x675fb4,self.vertices)
        self.vm.call(0x56d400,(22,1))  # D3DCULL_NONE.

    def _identity(self,args,name):
        if args[0]!=self.texture_object:raise ValueError('Wrong COM surface receiver')
        self.com_trace.append(name)

    def _addref(self,args):
        self._identity(args,'AddRef');self.references+=1;return self.references

    def _release(self,args):
        self._identity(args,'Release')
        if self.references<=0:raise ValueError('Surface reference underflow')
        self.references-=1;return self.references

    def _pixel_format(self,args):
        self._identity(args,'GetPixelFormat');self.vm.write(args[1],self.pixel_format);return 0

    def _lock(self,args):
        self._identity(args,'Lock')
        if self.locked or args[1] or args[3] or args[4]:
            raise MissingAPI('Fixture texture Lock supports unlocked full-surface/no-flags calls only')
        if self.vm.u32(args[2])!=124:raise MissingAPI('Expected DDSURFACEDESC2 size 124')
        description=bytearray(124)
        struct.pack_into('<5I',description,0,124,0x100f,self.texture.height,self.texture.width,self.texture.stride)
        struct.pack_into('<I',description,36,self.texture.address)
        description[72:104]=self.pixel_format
        self.vm.write(args[2],bytes(description));self.locked=True;return 0

    def _unlock(self,args):
        self._identity(args,'Unlock')
        if not self.locked or args[1]:raise MissingAPI('Unbalanced or partial texture Unlock')
        self.locked=False;return 0

    def set_texture(self,width,height,data,format='RGB565'):
        if min(width,height)<1 or width&(width-1) or height&(height-1):
            raise ValueError('Retail fixture textures require positive power-of-two dimensions')
        if format not in ('RGB565','ARGB4444'):raise ValueError('Unknown original texture format')
        if len(data)!=width*height*2:raise ValueError('Expected exact 16-bit texture bytes')
        if width<2:raise ValueError('Use width >= 2 for a tightly packed even pitch')
        self.texture=self.vm.surface('texture',width,height);self.texture.format=format
        self.vm.write(self.texture.address,data)
        self.pixel_format=(struct.pack('<8I',32,0x40,0,16,0xf800,0x07e0,0x001f,0) if format=='RGB565' else
            struct.pack('<8I',32,0x41,0,16,0x0f00,0x00f0,0x000f,0xf000))
        self.vm.call(0x56d3a0,(1,self.texture_object))

    def clear(self,color=0,depth=65535):
        self.vm.write(self.screen.address,struct.pack('<H',color)*(self.width*self.height))
        self.vm.write(self.depth.address,struct.pack('<H',depth)*(self.width*self.height))

    def project(self,points,world_matrix=None,camera=(0,0,0),zdist=1925):
        """Original world/view/projection math, explicit camera and frame center.

        Matrix ordering and all float/FPU arithmetic execute as retail code.
        The output center is a selected fixture viewport, not screenshot fitting.
        """
        if world_matrix is None:world_matrix=[float(i%5==0) for i in range(16)]
        if len(world_matrix)!=16 or len(camera)!=3:raise ValueError('Invalid projection inputs')
        world=self.vm.allocate(64);temporary=self.vm.allocate(64);combined=self.vm.allocate(64)
        position=self.vm.allocate(12);point=self.vm.allocate(12);result=self.vm.allocate(12)
        self.vm.write(world,struct.pack('<16f',*world_matrix))
        self.vm.write(position,struct.pack('<3i',*camera))
        self.vm.call(0x411640)
        self.vm.call(0x56d5f0,(3,0x5e88c8))
        self.vm.call(0x56d5f0,(1,world))
        self.vm.call(0x56cdc0,(position,zdist))
        self.vm.call(0x43aa90,(temporary,0x675ed0,0x676000))
        self.vm.call(0x43aa90,(combined,temporary,0x675f70))
        projected=[]
        for xyz in points:
            self.vm.write(point,struct.pack('<3f',*xyz))
            self.vm.call(0x43ad80,(combined,point,result))
            x,y,z=struct.unpack('<3f',self.vm.uc.mem_read(result,12))
            projected.append((x+self.width/2,-y+self.height/2,z))
        return projected

    def draw(self,vertices,indices,z_enabled=False,z_write=False,instruction_limit=1000000):
        """Vertices: x,y,z,R,G,B,A,u,v; RGBA are retail 5-bit integers 0..31.

        x/y are screen-relative to SetScreenCenter(0,0), with positive y down.
        World/view/projection and illumination are intentionally upstream inputs.
        """
        if not vertices or len(vertices)>4096 or not indices or len(indices)>16384 or len(indices)%3:
            raise ValueError('Vertex/index fixture capacity or triangle-list mismatch')
        if min(indices)<0 or max(indices)>=len(vertices):raise ValueError('Index out of bounds')
        records=[]
        for x,y,z,r,g,b,a,u,v in vertices:
            if not all(0<=c<=31 for c in (r,g,b,a)):raise ValueError('RGBA must be 5-bit')
            records.append(struct.pack('<3f4I2f',x,-y,z,r,g,b,a,u,v))
        self.vm.write(self.vertices,b''.join(records))
        self.vm.write(self.indices,struct.pack('<'+'H'*len(indices),*indices))
        self.vm.call(0x56d400,(7,int(z_enabled)))
        self.vm.call(0x56d400,(14,int(z_write)))
        with self.vm.bulk_writes([
            (self.screen.address,self.screen.stride*self.screen.height),
            (self.depth.address,self.depth.stride*self.depth.height),
            # This bounded retail dispatcher/raster also updates interpolation
            # locals per pixel. Snapshot its main-thread stack region once;
            # writes outside these declared ranges remain fully tracked.
            (self.vm.STACK+self.vm.STACK_SIZE-0x10000,0x10000),
        ]):
            self.vm.call(0x56d960,(len(vertices),self.indices,len(indices)),instruction_limit=instruction_limit)
        if self.locked:raise AssertionError('Retail failed to unlock its texture')
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')

    def checkpoint(self):
        if self.locked:raise ValueError('Cannot checkpoint a locked fixture surface')
        self.vm.checkpoint();self._references=self.references

    def restore(self):
        pages=self.vm.restore();self.references=self._references;self.locked=False;self.com_trace=[]
        return pages


def save_rgb565_png(path,raw,width,height):
    """Lossless view of RGB565 output; no Pillow or GUI dependencies."""
    pixels=struct.unpack('<'+'H'*(width*height),raw)
    rows=[]
    for y in range(height):
        row=bytearray([0])
        for p in pixels[y*width:(y+1)*width]:
            r=(p>>11)&31;g=(p>>5)&63;b=p&31
            row.extend(((r<<3)|(r>>2),(g<<2)|(g>>4),(b<<3)|(b>>2)))
        rows.append(bytes(row))
    def chunk(kind,data):
        return struct.pack('>I',len(data))+kind+data+struct.pack('>I',zlib.crc32(kind+data)&0xffffffff)
    path.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>2I5B',width,height,8,2,0,0,0))+
        chunk(b'IDAT',zlib.compress(b''.join(rows)))+chunk(b'IEND',b''))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--repeat',type=int,default=100);args=parser.parse_args()
    if args.repeat<1:parser.error('--repeat must be positive')
    start=time.perf_counter();fixture=SoftwareFixture(args.executable)
    texels=([0xf800,0xf800,0x07e0,0x07e0]*2+[0x001f,0x001f,0xffff,0xffff]*2)
    fixture.set_texture(4,4,struct.pack('<16H',*texels));fixture.clear();fixture.checkpoint()
    setup_ms=(time.perf_counter()-start)*1000
    quad=[(x,y,100,31,31,31,31,u,v) for x,y,u,v in [(8,8,0,0),(50,8,1,0),(50,50,1,1),(8,50,0,1)]]
    indices=[0,1,2,0,2,3];results=[];timings=[]
    args.output.mkdir(parents=True,exist_ok=True)
    for case,z_enabled,z_write,depth,expected_visible,expected_depth in [
        ('no_z',False,False,65535,True,65535),
        ('z_read_write',True,True,600,True,100),
        ('z_read_only',True,False,600,True,600),
        ('occluded',True,True,50,False,50)]:
        first=None
        for _ in range(args.repeat):
            start=time.perf_counter();fixture.restore();fixture.clear(depth=depth)
            color,z=fixture.draw(quad,indices,z_enabled,z_write)
            timings.append((time.perf_counter()-start)*1000)
            digest=(hashlib.sha256(color).hexdigest(),hashlib.sha256(z).hexdigest())
            if first is None:first=digest
            elif first!=digest:raise AssertionError('Software pixels/depth did not replay byte-identically')
            colors=set(struct.unpack('<4096H',color));depths=set(struct.unpack('<4096H',z))
            if expected_visible:
                if colors!={0,0xf800,0x07e0,0x001f,0xffff}:raise AssertionError(f'Wrong original textured output: {colors}')
                if fixture.vm.u32(fixture.depth.address+2*(20*64+20))&0xffff!=expected_depth:
                    raise AssertionError('Unexpected original raster depth at interior pixel')
            elif colors!={0}:raise AssertionError('Original depth test failed to occlude the quad')
            if fixture.com_trace!=['Lock','Unlock']:raise AssertionError('Original texture lock/unlock ABI mismatch')
        (args.output/f'{case}.rgb565').write_bytes(color);(args.output/f'{case}.depth-u16').write_bytes(z)
        save_rgb565_png(args.output/f'{case}.png',color,64,64)
        results.append(dict(case=case,color_sha256=first[0],depth_sha256=first[1],
            visible_pixels=sum(p!=0 for p in struct.unpack('<4096H',color)),colors=sorted(colors),depth_values=sorted(depths)))
    report=dict(status='pass',retail_sha256=RETAIL_SHA,
        probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),setup_ms=setup_ms,
        original_dispatch='0x56d960',original_table_builder='0x56c5a0',
        width=64,height=64,replays=len(timings),replays_per_case=args.repeat,
        mean_loop_ms=statistics.mean(timings),median_loop_ms=statistics.median(timings),
        p95_loop_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],
        guest_os_boots=0,intercepted_raster_functions=[],results=results,
        scope='Actual original textured triangle raster into RGB565 and unsigned16 depth RAM, original dispatch and modulation table generation, byte-identical warm replays. Procedural texture and transformed vertices are explicit fixtures; no authored asset conversion, effect animation, HUD, game startup or modern A/B parity claim.')
    (args.output/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))


if __name__=='__main__':main()
