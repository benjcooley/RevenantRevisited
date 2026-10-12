#!/usr/bin/env python3
"""Read-only shipped Labyrinth/Rift resource and natural-caller inventory."""
import argparse,hashlib,json,re,struct,zipfile
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from character_native_preflight import ROOT,sha
from map_effect_placements import scan
CD=Path('/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT')
SELECT={'LabyrinthEffect':0xad92bd20,'Rift1':0xadbcef18}

def run(out):
 out.mkdir(parents=True,exist_ok=False);records=[];assets=[];maps=[];loose=[]
 for label,base in [('GOG',ROOT/'data'),('CD',CD)]:
  archive=base/'imagery.rvi'
  with zipfile.ZipFile(archive)as z:
   for n in z.namelist():
    if n.lower()in('imagery/misc/starfield.i3d','imagery/misc/rift1.i3d'):
     b=z.read(n);assets.append(dict(distribution=label,member=n,sha256=hashlib.sha256(b).hexdigest(),bytes=len(b)))
  for archive in [base/'imagery.rvi',*sorted((base/'Modules').glob('*.rvm'))]:
   with zipfile.ZipFile(archive)as z:
    text=raw_count=0;hits=[];backgrounds=[];type_hits=[]
    for n in z.namelist():
     if n.lower().endswith(('.s','.def','.txt','.scr')):
      b=z.read(n);text+=1
      for i,line in enumerate(b.decode('latin1').splitlines(),1):
       if re.search(r'LabyrinthEffect|Starfield|Rift1',line,re.I):hits.append(dict(member=n,line=i,text=line))
       if re.search(r'\bBGEFFECT\b',line,re.I):backgrounds.append(dict(member=n,line=i,text=line.strip()))
     if re.fullmatch(r'Map/\d+_\d+_\d+\.dat',n,re.I):
      raw_count+=1;b=z.read(n)
      for name,value in SELECT.items():
       count=b.count(struct.pack('<I',value))
       if count:type_hits.append(dict(member=n,name=name,raw_occurrences=count))
    records.append(dict(distribution=label,archive=str(archive),archive_sha256=sha(archive),text_members_scanned=text,map_members_scanned=raw_count,named_references=hits,background_effects=backgrounds,raw_type_id_matches=type_hits))
   if archive.suffix=='.rvm':maps.append(scan(archive,set(SELECT.values())))
  for folder in(base/'Resources',base/'Modules',base/'Imagery'):
   if not folder.exists():continue
   for p in folder.rglob('*'):
    if not p.is_file():continue
    if p.name.lower()in('starfield.i3d','rift1.i3d'):loose.append(dict(distribution=label,path=str(p),sha256=sha(p)))
    if p.suffix.lower()in('.s','.def','.txt','.scr'):
     for i,line in enumerate(p.read_text(encoding='latin1').splitlines(),1):
      if re.search(r'LabyrinthEffect|Starfield|Rift1',line,re.I):loose.append(dict(distribution=label,path=str(p),sha256=sha(p),line=i,text=line))
 exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';b=exe.read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0];n=struct.unpack_from('<H',b,pe+6)[0];sz=struct.unpack_from('<H',b,pe+20)[0];base=struct.unpack_from('<I',b,pe+52)[0];sections=[struct.unpack_from('<8sIIII',b,pe+24+sz+i*40)for i in range(n)]
 def data(address,count):
  sec=next(s for s in sections if s[2]<=address-base<s[2]+max(s[1],s[3]));o=sec[4]+address-base-sec[2];return b[o:o+count]
 md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;md.skipdata=True;sec=next(s for s in sections if s[0].split(b'\0')[0]==b'.text');code=list(md.disasm(b[sec[4]:sec[4]+sec[3]],base+sec[2]));xrefs=[]
 for i in code:
  if i.id and any(o.type==2 and o.imm&0xffffffff in (0x5e15c4,0xad92bd20,0x509b00,0x509bc0)for o in i.operands):xrefs.append(dict(address=hex(i.address),mnemonic=i.mnemonic,operands=i.op_str))
 raw_text_reference_counts={hex(value):b[sec[4]:sec[4]+sec[3]].count(struct.pack('<I',value))for value in(0x5e15c4,0xad92bd20,0x509b00,0x509bc0)};disassembly={hex(a):[dict(address=hex(i.address),mnemonic=i.mnemonic,operands=i.op_str)for i in md.disasm(data(a,size),a)]for a,size in [(0x505290,0x41),(0x5052e0,0x20),(0x505300,0x20),(0x505320,0x10a),(0x505580,0x69),(0x509b00,0x70),(0x509bc0,0x90)]}
 report=dict(status='bounded_caller_resource_audit_complete',accepted=False,visual_pass=False,assets=assets,archives=records,placement_inventories=maps,loose_references=loose,native_immediate_xrefs=xrefs,raw_text_reference_counts=raw_text_reference_counts,linear_scan_scope="Entire raw text scanned with data skipping; not a complete control-flow or indirect-caller proof",native_disassembly=disassembly,executable_sha256=sha(exe),probe_sha256=sha(__file__),scope='GOG/CD unchanged licensed assets, all seven modules script/definition references and serialized top-level placements, loose text/resource overrides and pinned original immediate code references. No named natural Labyrinth caller found: Ahkuilon uses BGEFFECT labback. Absence in these finite inputs is not proof of universal unused status; dynamic scripts, runtime object/resource overrides or other revisions remain open. No fabricated prototype, seed/selector fitting or production change. Rift own tag/caller audit remains a separate failed-init boundary.');(out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(assets),len(records),sum(len(m['placements'])for m in maps),len(xrefs));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
