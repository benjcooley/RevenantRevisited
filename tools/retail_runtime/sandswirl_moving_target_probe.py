"""Causal target-move controls over two complete restored-Sand original traces."""
import argparse,hashlib,json,struct
from pathlib import Path

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(stationary,moving,output):
 output.mkdir(parents=True,exist_ok=False);a=json.loads(stationary.read_text());b=json.loads(moving.read_text())
 assert a['legacy_sand_restoration'] and b['legacy_sand_restoration']
 assert not a['actor_moves'] and [x['tick']for x in b['actor_moves']]==[60,85]
 assert len(a['records'])==len(b['records'])==310 and a['initial']==b['initial']
 assert a['source_map_allocation_control'] and b['source_map_allocation_control']
 assert a['actor_map_indices']==b['actor_map_indices']
 for event in b['actor_moves']:assert event['result']==b['actor_map_indices'][1]
 before=0;changed=[]
 for old,new in zip(a['records'],b['records']):
  tick=old['tick'];assert tick==new['tick']
  if tick<75:
   assert old['state']==new['state'],tick
   assert old['particle_packets']==new['particle_packets'],tick
   assert old['rgb565_sha256']==new['rgb565_sha256'],tick
   before+=1
  elif old['state']['pool']!=new['state']['pool']:changed.append(tick)
  if 'image'in new:
   assert sha(moving.parent/new['image'])==new['image_sha256']
 assert before==74 and changed and changed[0]==75
 first=b['records'][74]['actors'][1]['position'];last=b['records'][84]['actors'][1]['position'];origin=b['source'];assert first!=last
 cached=struct.pack('<3f',first[0]-origin[0],first[1]-origin[1],first[2]-origin[2]-32)
 assert all(bytes.fromhex(row['state']['parameters_hex'])[100:112]==cached for row in b['records'][74:])
 assert a['queries'][0]['found']==b['queries'][0]['found']==2
 assert a['queries'][0]['indices']==b['queries'][0]['indices']
 report=dict(status='pass',accepted=False,visual_pass=False,unchanged_before_query_frames=before,first_changed_pool_tick=changed[0],changed_pool_frames=len(changed),actual_actor_moves=b['actor_moves'],cached_target=list(struct.unpack('<3f',cached)),cached_target_rows=236,post85_move_does_not_retarget=True,query_uses_actual_moved_actor=True,stationary_manifest_sha256=sha(stationary),moving_manifest_sha256=sha(moving),probe_sha256=sha(__file__),scope=__doc__,limits='Explicit two public SetPos calls, not actor AI or natural Sandswirl spell scheduling. Original query samples at75; later controller resets reuse that cached point. Missing shipped binding is restored from evidenced legacy data. Actor IDs remain original; the existing parent/particle/Quicksand allocator normalization is unchanged.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--stationary',type=Path,required=True);p.add_argument('--moving',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.stationary,a.moving,a.output)
