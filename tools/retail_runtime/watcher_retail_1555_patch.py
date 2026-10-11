"""Build an explicit qualified retail PE: exact1555 decode in watcher alpha leaf.

No proprietary executable is distributed. The local pinned input is required.
Original geometry, UV selection, scan conversion and blend loop remain intact.
"""
import argparse,hashlib,json,struct,subprocess
from pathlib import Path
RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
SITES=(0x56d3ea,0x55048f,0x5507b8)
EXPECTED=('8b44241c85c00f95c0a254606700','81e6ffff00008bfe81e6ff0f0000c1ef0c668b1470','81e6ffff00008bfe81e6ff0f0000c1ef0c668b1470')
def sha(b):return hashlib.sha256(b).hexdigest()
def align(n,a):return (n+a-1)//a*a
class PE:
 def __init__(self,data):
  self.data=data;self.pe=struct.unpack_from('<I',data,0x3c)[0];self.optional=self.pe+24;self.count=struct.unpack_from('<H',data,self.pe+6)[0];self.table=self.optional+struct.unpack_from('<H',data,self.pe+20)[0];self.base=struct.unpack_from('<I',data,self.optional+28)[0];self.sections=[struct.unpack_from('<8s8I',data,self.table+i*40)for i in range(self.count)]
 def offset(self,address):
  rva=address-self.base
  for name,size,start,rawsize,raw,*unused in self.sections:
   if start<=rva<start+rawsize:return raw+rva-start
  raise ValueError(hex(address))
def build(executable,output):
 output.mkdir(parents=True,exist_ok=False);raw=executable.read_bytes();assert sha(raw)==RETAIL_SHA;pe=PE(raw);data=bytearray(raw);section_align,file_align=struct.unpack_from('<2I',data,pe.optional+32);rva=align(max(s[2]+max(s[1],s[3])for s in pe.sections),section_align);base=pe.base+rva;flag=base+0x300
 assert not (struct.unpack_from('<H',data,pe.optional+70)[0]&0x40), 'Fixed-base qualified patch; ASLR image is unsupported'
 assert pe.table+(pe.count+1)*40<=min(s[4]for s in pe.sections if s[4])
 # New header uses previously unmapped header padding, which contains stale bytes in this rebuilt PE.
 assert all(not (pe.table+pe.count*40<=struct.unpack_from("<I",data,pe.optional+96+i*8)[0]<pe.table+(pe.count+1)*40)for i in range(16))
 asm=f'''BITS 32
ORG {base}
; Reproduce original SetTexture's alpha boolean, then remember exact1555 masks.
mov eax,[esp+0x20] ; CALL adds4; descriptor alpha was original esp+1c.
test eax,eax
setne al
mov [0x676054],al
pushfd
push eax
mov dword [{flag}],0
cmp dword [esp+0x18],16 ; original descriptor RGBBitCount at+0c, plus CALL/pushfd/push.
jne .done
cmp dword [esp+0x1c],0x7c00
jne .done
cmp dword [esp+0x20],0x3e0
jne .done
cmp dword [esp+0x24],0x1f
jne .done
cmp dword [esp+0x28],0x8000
jne .done
mov dword [{flag}],1
.done:
pop eax
popfd
ret
times 0x100-($-$$) db 0x90
'''
 for i in range(2):
  asm+=f'''decode{i}:
cmp dword [{flag}],1
je .argb1555
; Unchanged original ARGB4444 instructions for every other format.
{chr(10).join('db '+','.join(hex(x)for x in bytes.fromhex(EXPECTED[1])[j:j+10])for j in range(0,len(bytes.fromhex(EXPECTED[1])),10))}
ret
.argb1555:
movzx esi,si
mov edi,esi
shr edi,15
shl edi,4
 dec edi ; original INC below turns this into exact alpha0 or16.
mov edx,esi
and edx,0x7fe0
shl edx,1
and esi,0x1f
or edx,esi
; Expand green5 to green6 by duplicating its high bit (full normalized range).
mov esi,edx
and esi,0x400
shr esi,5
or edx,esi
ret
times {hex((i+2)*0x100)}-($-$$) db 0x90
'''
 asm+='dd 0\n';source=output/'watcher1555.asm';source.write_text(asm);code=output/'watcher1555.bin';subprocess.run(['nasm','-f','bin',str(source),'-o',str(code)],check=True);body=code.read_bytes();assert len(body)==0x304
 patches=[]
 for i,(site,expected)in enumerate(zip(SITES,EXPECTED)):
  old=bytes.fromhex(expected);off=pe.offset(site);assert data[off:off+len(old)]==old;target=base+i*0x100;new=b'\xe8'+struct.pack('<i',target-site-5)+b'\x90'*(len(old)-5);data[off:off+len(old)]=new;patches.append(dict(address=site,file_offset=off,original=old.hex(),patched=new.hex(),target=target))
 rawoffset=align(len(data),file_align);rawsize=align(len(body),file_align);data.extend(b'\0'*(rawoffset-len(data))+body+b'\0'*(rawsize-len(body)));section=struct.pack('<8s8I',b'.w1555\0\0',len(body),rva,rawsize,rawoffset,0,0,0,0xe0000040);data[pe.table+pe.count*40:pe.table+(pe.count+1)*40]=section;struct.pack_into('<H',data,pe.pe+6,pe.count+1);struct.pack_into('<I',data,pe.optional+56,align(rva+len(body),section_align));struct.pack_into('<I',data,pe.optional+64,0)
 binary=output/'Revenant.watcher1555.qualified.exe';binary.write_bytes(data)
 report=dict(status='explicit_qualified_patch',original_sha256=sha(raw),patched_sha256=sha(data),assembly_sha256=sha(source.read_bytes()),section_sha256=sha(body),section_address=base,flag_address=flag,section_file_offset=rawoffset,section_size=len(body),patches=patches,executable=str(binary),scope='Modified retail build, not unchanged device fidelity. Exact selected ARGB1555 masks enable direct RGB5-to-RGB565 and binary-alpha0/1 decode at both scan halves of actual watcher54ff60 alpha leaf. Other formats execute original decode. Loaded texels/descriptors, owner/keys/frame/geometry/UV selection/scan conversion and remaining integer blend loop unchanged. Other primitive/blend raster branches are outside this patch scope.')
 (output/'patch-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['patched_sha256']);return report

def apply_vm(vm,manifest):
 r=json.loads(manifest.read_text());data=Path(r['executable']).read_bytes();assert sha(data)==r['patched_sha256'];base=r['section_address'];size=align(r['section_size'],4096)
 vm.uc.mem_map(base,size);vm.uc.mem_write(base,data[r['section_file_offset']:r['section_file_offset']+r['section_size']])
 for p in r['patches']:
  old=bytes.fromhex(p['original']);assert bytes(vm.uc.mem_read(p['address'],len(old)))==old;vm.uc.mem_write(p['address'],bytes.fromhex(p['patched']));vm.uc.ctl_remove_cache(p['address'],p['address']+len(old))
 return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();build(a.executable,a.output)
