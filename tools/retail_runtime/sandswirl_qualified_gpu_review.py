"""Verify the explicitly substituted Sandswirl/Sparks GPU diagnostic.

Never grants Sandswirl appearance credit. Pool/RNG/packets are compared to the
complete original qualified reference; images remain a separate visual review.
"""
import argparse,hashlib,json,re,struct
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native_path,metal_path,out,previous=None):
 out.mkdir(parents=True,exist_ok=False);native=json.loads(native_path.read_text());metal=json.loads(metal_path.read_text())
 assert native['guest_code_modified'] and native['named_spell_control']=='Quicksand'
 assert metal['status']=='pass' and metal['exit_code']==0 and len(metal['frames'])==310
 assert metal['scenario']['effect']=='SANDSWIRL_QUALIFIED_SPARKS' and metal['scenario']['native_domain']
 assert metal['actual_viewport']==native['viewport']==[1280,960]
 assert metal['sample_simulation_ticks']==list(range(1,311))
 logpath=metal_path.parent/'run.log';assert sha(logpath)==metal['log_sha256'];log=logpath.read_text()
 assert '[dragonattack-camera] world=3185,3185,0' in log
 assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1)' in log
 rows={r['tick']:r for r in native['records']};seen=set();states=0
 for t,parent,kill,seed,rng,before,after in re.findall(r'\[sandswirl-qualified-state\] tick=(\d+) parent=(\d+) child_killed=(\d+) seed=(\d+) rng=(\d+) global_before=(\d+) global_after=(\d+)',log):
  t,parent,kill,seed,rng,before,after=map(int,(t,parent,kill,seed,rng,before,after));st=rows[t]['state']
  assert (parent,kill,seed,rng)==(st['parent_frame'],int(bool(st['child_flags']&4096)),st['seed'],st['rng_calls']),t
  assert before==after,(t,before,after);states+=1
 assert states==310
 for t,slot,data in re.findall(r'\[sandswirl-qualified-pool\] tick=(\d+) slot=(\d+) data=([0-9a-f]+)',log):
  t,slot=int(t),int(slot);assert (t,slot) not in seen;seen.add((t,slot));pool=rows[t]['state']['pool']
  expected=b''.join(bytes.fromhex(pool[k])[slot*size:(slot+1)*size] for k,size in [('position',12),('velocity',12),('life',4),('start',4),('variant',4),('target',4)])
  assert bytes.fromhex(data)==expected,(t,slot)
 assert len(seen)==310*85
 drawseen=set();worlds=0
 for t,draw,obj,pos,world in re.findall(r'\[sandswirl-qualified-draw\] tick=(\d+) draw=(\d+) object=(\d+) pos=([0-9a-f]+) world=([0-9a-f]+)',log):
  t,draw,obj=int(t),int(draw),int(obj);assert (t,draw) not in drawseen;drawseen.add((t,draw));row=rows[t];packet=row['particle_packets'][draw]
  assert (obj,pos)==(packet['variant'],packet['position_hex'])
  if 'draws' in row:assert world==row['draws'][draw]['world_hex'];worlds+=1
 assert len(drawseen)==sum(len(r['particle_packets']) for r in rows.values())
 render=re.findall(r'\[sandswirl-qualified-render\] tick=(\d+) draws=(\d+)',log);assert len(render)==310
 for tick,count in render:assert int(count)==len(rows[int(tick)]['particle_packets'])
 expected_hashes=['9ee0e036','7b4a7030','f424a66a','597d591f']
 meshes=re.findall(r'\[sandswirl-qualified-mesh\] object=(\d+) source_hash=([0-9a-f]+) vertices=(\d+) indices=(\d+)',log)
 assert meshes==[(str(i),h,'4','6') for i,h in enumerate(expected_hashes)]
 imagepairs=[];bounds=[]
 for row in native['records']:
  if 'image' not in row:continue
  nf=native_path.parent/row['image'];mf=Path(metal['frames'][row['tick']-1]['path'])
  assert sha(nf)==row['image_sha256'];assert sha(mf)==metal['frames'][row['tick']-1]['sha256']
  ni=Image.open(nf).convert('RGB');mi=Image.open(mf).convert('RGB');assert ni.size==mi.size==(1280,960)
  for image in (ni,mi):
   box=image.getbbox()
   if box:bounds.append(box)
  imagepairs.append(dict(tick=row['tick'],native=str(nf),native_sha256=sha(nf),metal=str(mf),metal_sha256=sha(mf),pixel_identical=ImageChops.difference(ni,mi).getbbox() is None))
 roi=(min(x[0] for x in bounds)-12,min(x[1] for x in bounds)-12,max(x[2] for x in bounds)+12,max(x[3] for x in bounds)+12)
 whole=Image.new('RGB',(640,len(imagepairs)*256));cw,ch=roi[2]-roi[0],roi[3]-roi[1];detail=Image.new('RGB',(cw*2,(ch+20)*len(imagepairs)))
 for i,pair in enumerate(imagepairs):
  for j,key in enumerate(('native','metal')):
   im=Image.open(pair[key]).convert('RGB');whole.paste(im.resize((320,240)),(j*320,i*256+16));detail.paste(im.crop(roi),(j*cw,i*(ch+20)+20))
  ImageDraw.Draw(whole).text((4,i*256),f"tick {pair['tick']} original qualified | Metal qualified",fill='white')
  ImageDraw.Draw(detail).text((4,i*(ch+20)),f"tick {pair['tick']} original qualified | Metal qualified",fill='white')
 whole.save(out/'contact.png');detail.save(out/'detail.png')
 repeat=None
 if previous:
  old=json.loads(previous.read_text());assert old['status']=='pass'
  for p,q in zip(old['frames'],metal['frames']):assert p['sha256']==q['sha256']
  repeat=dict(frames=len(old['frames']),all_pngs_byte_identical=True,previous_manifest_sha256=sha(previous))
 report=dict(status='pass',accepted=False,visual_pass=False,qualified_sparks_substitute=True,frames=310,pool_rows_bit_exact=len(seen),pool_fields_bit_exact=10*len(seen),rng_and_parent_fields_exact=4*states,global_rng_invariance_ticks=states,render_packets_exact=len(drawseen),original_sampled_world_matrices_bit_exact=worlds,fixed_meshes=4,native_manifest_sha256=sha(native_path),metal_manifest_sha256=sha(metal_path),binary_sha256=metal['binary_sha256'],native_sha256=native['executable_sha256'],source_sha256=sha(ROOT/'src/game/effects/legacy/sandswirlqualifiedpreview.cpp'),core_sha256=sha(ROOT/'src/game/effects/legacy/seekingparticles.h'),probe_sha256=sha(__file__),images=imagepairs,contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),display_crop=roi,previous_policy_repeat=repeat,limits='Explicit Sparks child substitution and Quicksand named-spell control. No production sand alias or retail mapping; missing intended imagery and natural Sandswirl caller remain unresolved. Source private RNG boundary receives observed particle-entry and tick75 genuine map-allocation seed/call count; does not allocate map objects or simulate Quicksand damage. No ground queries. Source fixed target input is the proven real-actor query result; actual actor scheduler and moving-target GPU scenario outside this run. Existing source-alpha helper sampling/interpolation/framebuffer differences remain; no exact pixel claim.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='images'}));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--previous',type=Path);a=p.parse_args();run(a.native,a.metal,a.output,a.previous)
