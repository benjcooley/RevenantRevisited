"""Verify actual map-runtime Sandswirl against an explicitly modified retail control.

This never approves missing sand imagery: the dependency is licensed Sparks.
Retail map-index generation is changed to the modern monotonic/RNG-neutral policy;
all real map factories, queries, owner calls and child controllers still execute.
"""
import argparse,hashlib,json,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
ROOT=Path(__file__).resolve().parents[2]
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def run(native_path,metal_path,output):
 output.mkdir(parents=True,exist_ok=False)
 n=json.loads(native_path.read_text());m=json.loads(metal_path.read_text())
 assert n['source_map_allocation_control'] and n['named_spell_control']=='Quicksand'
 assert len(n['records'])==310 and len(n['allocation_rng_controls'])==6
 assert all(a['suppressed_rng_calls']==2 for a in n['allocation_rng_controls'])
 assert len({a['source_monotonic_id'] for a in n['allocation_rng_controls']})==6
 assert m['status']=='pass' and m['exit_code']==0 and len(m['frames'])==310
 restored=bool(n.get('legacy_sand_restoration'))
 assert m['scenario']['effect']==('SANDSWIRL_MAP_RESTORED_SAND' if restored else 'SANDSWIRL_MAP_QUALIFIED_SPARKS')
 if restored:assert '[sandswirl-restored-binding] class=25 name=Sand id=fecaface' in (metal_path.parent/'run.log').read_text()
 assert m['actual_viewport']==n['viewport']==[1280,960]
 assert m['sample_simulation_ticks']==list(range(1,311))
 logpath=metal_path.parent/'run.log';assert sha(logpath)==m['log_sha256'];log=logpath.read_text()
 assert '[sandswirl-map-negative] missing_sand_failed_closed=1 no_child=1 rng_unchanged=1 actual_map_delete=1' in log
 assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1)' in log
 assert '[dragonattack-camera] world=3185,3185,0' in log
 rows={r['tick']:r for r in n['records']};seen=set();phases=0;offset=None
 for values in re.findall(r'\[sandswirl-map-phase\] tick=(\d+) parent=(\d+) parent_kill=(\d+) child_kill=(\d+) query=(\d+) quicksands=(\d+) rng=(\d+) spell_timer=(-?\d+)',log):
  tick,parent,pk,ck,query,qs,rng,timer=map(int,values);st=rows[tick]['state']
  assert (parent,pk,ck,query,qs)==(st['parent_frame'],int(bool(st['owner_flags']&4096)),int(bool(st['child_flags']&4096)),2 if tick>=75 else 0,1 if tick>=75 else 0),tick
  assert timer==(rows[tick]['spell_timer'] if rows[tick]['spell_timer']<0x80000000 else rows[tick]['spell_timer']-0x100000000),tick
  if offset is None:offset=rng-st['rng_calls']
  assert rng-st['rng_calls']==offset,tick
  phases+=1
 assert phases==310
 for tick,slot,data in re.findall(r'\[sandswirl-map-pool\] tick=(\d+) slot=(\d+) data=([0-9a-f]+)',log):
  tick,slot=int(tick),int(slot);assert (tick,slot) not in seen;seen.add((tick,slot));p=rows[tick]['state']['pool']
  expected=b''.join(bytes.fromhex(p[k])[slot*z:(slot+1)*z] for k,z in [('position',12),('velocity',12),('life',4),('start',4),('variant',4),('target',4)])
  assert bytes.fromhex(data)==expected,(tick,slot)
 assert len(seen)==310*n['initial']['particles']
 packets=set()
 for tick,draw,obj,pos in re.findall(r'\[sandswirl-map-packet\] tick=(\d+) draw=(\d+) object=(\d+) pos=([0-9a-f]+)',log):
  tick,draw,obj=int(tick),int(draw),int(obj);assert (tick,draw) not in packets;packets.add((tick,draw));p=rows[tick]['particle_packets'][draw]
  assert (obj,pos)==(p['variant'],p['position_hex']),(tick,draw)
 assert len(packets)==sum(len(r['particle_packets']) for r in n['records'])
 renders=re.findall(r'\[sandswirl-map-render\] tick=(\d+) draws=(\d+)',log);assert len(renders)==310
 for tick,draws in renders:assert int(draws)==len(rows[int(tick)]['particle_packets'])
 qsrows=re.findall(r'\[sandswirl-map-quicksand\] tick=(\d+) id=(\d+) frame=(\d+) kill=(\d+)',log)
 assert len(qsrows)==236
 for tick,index,frame,kill in qsrows:
  tick,frame,kill=int(tick),int(frame),int(kill);q=rows[tick]['quicksands'];assert len(q)==1
  assert (frame,kill)==(q[0]['frame']%3,int(bool(q[0]['flags']&4096))),tick
 cleanup=re.search(r'\[sandswirl-map-cleanup\] tick=(\d+) health=(\d+)/(\d+) flags=([0-9a-f]+)/([0-9a-f]+) spell_timer=(\d+)',log)
 assert cleanup and tuple(map(int,cleanup.group(2,3,6)))==(102,102,0)
 assert [int(value,16) for value in cleanup.group(4,5)]==[int(actor['flags'],0)for actor in n['actors_after']]
 actors=re.search(r'\[sandswirl-map-actors\] source=(\d+),(\d+),(\d+) target=(\d+),(\d+),(\d+)',log);assert actors
 assert list(map(int,actors.groups()))==[coordinate for actor in n['actors_after'] for coordinate in actor['position']]
 movement=None
 if n.get('actor_moves'):
  moves=re.findall(r'\[sandswirl-target-move\] tick=(\d+) position=(\d+),(\d+),(\d+) result=(-?\d+) rng_before=(\d+) rng_after=(\d+)',log)
  assert len(moves)==len(n['actor_moves'])==2
  move_results=[]
  for values,original in zip(moves,n['actor_moves']):
   tick,x,y,z,result,before,after=map(int,values)
   assert tick==original['tick'] and [x,y,z]==original['position'] and before==after
   move_results.append(dict(tick=tick,position=[x,y,z],native_return=original['result'],source_return=result,rng_unchanged=True))
  samples=re.findall(r'\[sandswirl-target-sample\] tick=(\d+) count=(\d+) target=([0-9a-f]+)',log);assert len(samples)==310
  for tick,count,target in samples:
   parameters=bytes.fromhex(rows[int(tick)]['state']['parameters_hex'])
   assert int(count)==struct.unpack_from('<i',parameters,96)[0] and bytes.fromhex(target)==parameters[100:112],tick
  sampled=n['records'][74]['actors'][1]['position'];origin=n['source'];relative=struct.pack('<3f',sampled[0]-origin[0],sampled[1]-origin[1],sampled[2]-origin[2]-32)
  assert all(bytes.fromhex(row['state']['parameters_hex'])[100:112]==relative for row in n['records'][74:])
  assert n['records'][84]['actors'][1]['position']!=sampled
  movement=dict(public_setpos=move_results,cached_target_rows_exact=310,query_samples_tick=75,post85_move_does_not_retarget=True)
 pairs=[];boxes=[]
 for row in n['records']:
  if 'image' not in row:continue
  a=native_path.parent/row['image'];b=Path(m['frames'][row['tick']-1]['path'])
  assert sha(a)==row['image_sha256'] and sha(b)==m['frames'][row['tick']-1]['sha256']
  for p in (a,b):
   im=Image.open(p).convert('RGB');assert im.size==(1280,960)
   if im.getbbox():boxes.append(im.getbbox())
  pairs.append(dict(tick=row['tick'],native=str(a),native_sha256=sha(a),metal=str(b),metal_sha256=sha(b)))
 roi=(min(x[0] for x in boxes)-12,min(x[1] for x in boxes)-12,max(x[2] for x in boxes)+12,max(x[3] for x in boxes)+12)
 cw,ch=roi[2]-roi[0],roi[3]-roi[1];contact=Image.new('RGB',(640,256*len(pairs)));detail=Image.new('RGB',(cw*2,(ch+20)*len(pairs)))
 for i,pair in enumerate(pairs):
  for j,key in enumerate(('native','metal')):
   im=Image.open(pair[key]).convert('RGB');contact.paste(im.resize((320,240)),(j*320,i*256+16));detail.paste(im.crop(roi),(j*cw,i*(ch+20)+20))
  title=f"tick {pair['tick']} modified retail map | actual source map"
  ImageDraw.Draw(contact).text((4,i*256),title,fill='white');ImageDraw.Draw(detail).text((4,i*(ch+20)),title,fill='white')
 contact.save(output/'contact.png');detail.save(output/'detail.png')
 report=dict(status='pass',accepted=False,visual_pass=False,qualified_sparks_dependency=not restored,legacy_sand_restoration=n.get('legacy_sand_restoration'),actual_map_runtime=True,frames=310,pool_rows_bit_exact=len(seen),pool_fields_bit_exact=len(seen)*10,phase_rng_rows_exact=phases,global_rng_counter_offset=offset,packet_rows_bit_exact=len(packets),quicksand_lifetime_rows_exact=len(qsrows),allocator_control=n['allocation_rng_controls'],native_seekz_raw=n['initial']['seekz_raw'],source_seekz_control=41,seekz_nonzero_semantics_only=True,missing_dependency_negative_pass=True,actor_cleanup=list(cleanup.groups()),native_manifest_sha256=sha(native_path),metal_manifest_sha256=sha(metal_path),binary_sha256=m['binary_sha256'],images=pairs,contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),source_sha256={p:sha(ROOT/p) for p in ['src/game/effects/legacy/sandswirlruntime.cpp','src/game/effects/legacy/sandswirlmapfixture.cpp','src/game/effects/legacy/seekingparticles.h']},limits='Missing intended sand is not aliased. Explicit Sparks and named Quicksand spell control; no natural Sandswirl variant. Original allocator IDs and its RNG side effects are deliberately replaced only at two proven index-generation boundaries; actual map APIs still execute. Stack-derived seekz remains undefined in retail; source uses explicit nonzero 41. Original unused parameter bytes are not compared. Snapshot owner-before-child scheduling and delayed first child pulse are declared; full world/actor AI scheduling remains outside fixture. Images are review evidence, not pixel equality or intended sand appearance.')
 if restored:
  report['source_sha256']['src/game/effects/legacy/sandswirlresource.h']=sha(ROOT/'src/game/effects/legacy/sandswirlresource.h')
  report['source_term_quantize']='[sandswirl-sand-raster] original_scene=2 argb4444=1 source_term_quantize=1' in log
  report['limits']=report['limits'].replace('Missing intended sand is not aliased. Explicit Sparks and named Quicksand spell control;', 'Explicit restored legacy Sand/fecaface binding to its own shipped Sandswirl resource; modified-retail configuration, not shipped-enabled. Named Quicksand spell control;').replace('intended sand appearance.', 'unmodified shipped appearance.')
 if movement:report['moving_target_control']=movement
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='images'},indent=2));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.output)
