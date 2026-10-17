"""Validate retained Tornado gallery lifetime/RNG/empty-map-query evidence.

Kills are driven by each unreset RNG stream, not hardcoded to seed1's360ticks.
The source query fails closed if real loaded-sector characters enter its scope.
"""
import argparse,hashlib,json,re
from pathlib import Path
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(capture,out):
 source=capture/'manifest.json';r=json.loads(source.read_text());log=(capture/'run.log').read_text();out.mkdir(parents=True,exist_ok=False)
 deaths=re.findall(r'\[tornado-phase\] tick=(\d+) rng=(\d+) .*?alive=0',log)
 cleanup=re.findall(r'\[tornado-cleanup\] tick=(\d+) alive=(\d+) health=(\d+)/(\d+) flags=([0-9a-f]+)/([0-9a-f]+) spell_timer=(-?\d+) damage_events=(\d+)',log)
 spawns=re.findall(r'\[tornado-rng\] seeded=(\d+) seed=(\d+) initial_draws=(\d+)',log)
 queries=re.findall(r'\[tornado-damage-query\] tick=(\d+) radius=200 loaded=1 characters=0',log)
 assert r['exit_code']==0 and not r['forced_shutdown'] and r['binary_unchanged'] and len(r['frames'])==48
 assert len(deaths)>=3 and len(spawns)==len(cleanup)==len(deaths)+1
 assert all(int(t)>=301 for t,_ in deaths) and len({rng for _,rng in deaths})>1
 assert all(seed=='0' and draws=='875'for seed,_,draws in spawns)
 assert all(a==b=='102' and f==g=='406c028'for _,_,a,b,f,g,_,_ in cleanup)
 assert all(timer=='0'if alive=='0'else timer=='-1'for _,alive,_,_,_,_,timer,_ in cleanup)
 assert sum(int(row[-1])for row in cleanup)==len(queries)
 assert 'unsupported nonempty damage query'not in log and r['visible']>0 and r['distinct']>1
 report=dict(status='pass',accepted=False,visual_pass=False,capture_manifest_sha256=sha(source),capture_status=r['status'],capture_status_note='Original reused checker expected seed1 kill360 on every unseeded cycle; retained as failed checker, superseded by these actual-live lifecycle assertions.',binary_sha256=r['binary_sha256'],frames=48,visible=r['visible'],distinct=r['distinct'],death_ticks=[int(t)for t,_ in deaths],death_rng_draws=[int(rng)for _,rng in deaths],empty_loaded_sector_queries=len(queries),spawns=len(spawns),all_spawns_unseeded=True,cleanup=cleanup,log_sha256=sha(capture/'run.log'),tool_sha256=sha(__file__),scope='Actual translated common-world gallery, real source actor/Spell construction and cleanup, continuous RNG, loaded-sector query empty. Early teardown preserves live Spell timer until owned destruction; no actor map residency or successful damage claim.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('capture',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.capture,a.output)
