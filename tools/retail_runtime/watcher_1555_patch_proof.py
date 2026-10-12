"""Execute qualified x86 decode exhaustively plus unchanged original raster controls."""
import argparse,json,struct,subprocess
from pathlib import Path
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP
from watcher_retail_1555_patch import apply_vm,sha
from software_probe import SoftwareFixture

def run(executable,patch,output):
 output.mkdir(parents=True,exist_ok=False);r=json.loads(patch.read_text());image=Path(r['executable']).read_bytes();base=r['section_address'];uc=Uc(UC_ARCH_X86,UC_MODE_32);uc.mem_map(base,4096);uc.mem_write(base,image[r['section_file_offset']:r['section_file_offset']+r['section_size']]);uc.mem_map(0x1000000,0x200000);uc.reg_write(UC_X86_REG_ESP,0x11ffff0);table=[(i*113+37)&65535 for i in range(4096)];uc.mem_write(0x1010000,struct.pack('<4096H',*table));checks=[]
 for branch in (0,1):
  for semantic in (0,1):
   uc.mem_write(r['flag_address'],struct.pack('<I',semantic));asm=f'''BITS 32
ORG 0x1000000
mov ecx,0
.loop:
push ecx
mov esi,ecx
mov eax,0x1010000
xor edx,edx
call {base+0x100+branch*0x100}
pop ecx
mov [0x1020000+ecx*8],edx
mov [0x1020004+ecx*8],edi
inc ecx
cmp ecx,65536
jne .loop
nop
''';p=output/f'decode-{branch}-{semantic}.asm';p.write_text(asm);b=p.with_suffix('.bin');subprocess.run(['nasm','-f','bin',str(p),'-o',str(b)],check=True);code=b.read_bytes();uc.mem_write(0x1000000,code);uc.emu_start(0x1000000,0x1000000+len(code));actual=struct.unpack('<131072I',uc.mem_read(0x1020000,65536*8))
   for word in range(65536):
    expected=(((word>>10)&31)<<11)|(((((word>>5)&31)*63+15)//31)<<5)|(word&31) if semantic else table[word&4095]
    alpha=(15 if word&0x8000 else 0xffffffff)if semantic else word>>12
    assert actual[word*2:word*2+2]==(expected,alpha),(branch,semantic,word,actual[word*2:word*2+2],expected,alpha)
   checks.append(dict(branch=branch,semantic1555=bool(semantic),words=65536,output_sha256=sha(bytes(uc.mem_read(0x1020000,65536*8)))))
 # Real unchanged original kernels before/after patch for other supported formats.
 f=SoftwareFixture(executable,64,64);vertices=[(8,8,20,23,19,13,31,0,0),(54,8,20,23,19,13,31,1,0),(54,54,20,23,19,13,31,1,1),(8,54,20,23,19,13,31,0,1)];controls=[]
 def draw(fmt):
  f.set_texture(8,8,struct.pack('<64H',*[(i*991+0x8123)&65535 for i in range(64)]),fmt);f.clear(0x52a5);return f.draw(vertices,[0,1,2,0,2,3],z_enabled=True,z_write=False)
 before={fmt:draw(fmt)for fmt in('RGB565','ARGB4444')};apply_vm(f.vm,patch)
 for fmt in before:
  after=draw(fmt);assert after==before[fmt];assert f.vm.u32(r['flag_address'])==0;controls.append(dict(format=fmt,color_sha256=sha(after[0]),depth_sha256=sha(after[1]),identical=True))
 # Exact descriptor gate and full original integer blending for binary-alpha literals.
 fmt1555=struct.pack('<8I',32,65,0,16,0x7c00,0x3e0,0x1f,0x8000);literals=[]
 for word in(0,0x7fff,0x8000,0xffff,0x8864,0x83e0,0xfc00,0x801f):
  f.set_texture(8,8,struct.pack('<H',word)*64,'ARGB4444');f.pixel_format=fmt1555;f.vm.call(0x56d3a0,(1,f.texture_object));assert f.vm.u32(r['flag_address'])==1
  f.clear(0x52a5);color,depth=f.draw(vertices,[0,1,2,0,2,3],z_enabled=True,z_write=False);pixel=struct.unpack_from('<H',color,(30*64+30)*2)[0];expected=((((word>>10)&31)<<11)|(((((word>>5)&31)*63+15)//31)<<5)|(word&31))if word&0x8000 else 0x52a5;assert pixel==expected,(hex(word),hex(pixel),hex(expected));literals.append(dict(word=word,pixel=pixel))
 for field in(3,4,5,6,7):
  changed=list(struct.unpack('<8I',fmt1555));changed[field]^=1;f.pixel_format=struct.pack('<8I',*changed);f.vm.call(0x56d3a0,(1,f.texture_object));assert f.vm.u32(r['flag_address'])==0
 report=dict(status='pass',patch_manifest_sha256=sha(patch.read_bytes()),patched_executable_sha256=r['patched_sha256'],decode_checks=checks,channel_assertions=524288,original_raster_controls=controls,binary_alpha_literals=literals,exact_format_rejections=5,scope='All65536 sourcewords execute both exact assembled x86 decode branches, against independent semantic1555 and original4444 lookup expectations. Actual original RGB565/ARGB4444 fullraster before/after is byte-identical including depth. Qualified patch only; not unchanged retail fidelity.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['channel_assertions']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,required=True);p.add_argument('--patch',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.patch,a.output)
