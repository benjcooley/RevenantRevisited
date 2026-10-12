"""Actual shipped Essence Drain/ManaDrain named spell state and render packets.

Distinct from Essencedrain/Worm prototype0xdefeca7e. The genuine loaded variant
selects ManaDrain0xad92bc13,110..115; the existing core is otherwise unchanged.
"""
import argparse,json
from pathlib import Path
import manadrain_target_state_probe as base

def run(trace,output):
 native=json.loads(trace.read_text())
 assert native['variant']=={'name':'Essence Drain','maximum_damage':115}
 assert native['type_id']=='0xad92bc13'
 original=base.DRIVER
 assert 's.Pulse(60,62,rng,' in original
 try:
  base.DRIVER=original.replace('s.Pulse(60,62,rng,','s.Pulse(110,115,rng,')
  result=base.run(trace,output)
 finally:base.DRIVER=original
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
