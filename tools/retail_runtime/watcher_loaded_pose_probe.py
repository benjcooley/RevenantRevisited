"""Observe actual loaded watcher FVF112 matrices, streams and render states."""
from pathlib import Path
import json,struct
import watcher_qualified_device_probe as q
from watcher_surface_flow_probe import ObservedWorld
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
worlds=[]
class PoseWorld(ObservedWorld):
 def __init__(self,*a,**k):
  super().__init__(*a,**k);worlds.append(self);v=self.vm;self.transforms={};self.states={};self.draws=[]
  def observe(uc,address,*unused):
   sp=uc.reg_read(UC_X86_REG_ESP);args=[v.u32(sp+4+i*4)for i in range(7)]
   if address==0x56d5f0:self.transforms[args[0]]=list(struct.unpack('<16f',v.uc.mem_read(args[1],64)))
   elif address==0x56d400:self.states[args[0]]=args[1]
   else:self.draws.append(dict(name=self.name,args=args,transforms=dict(self.transforms),states=dict(self.states),vertices=list(struct.iter_unpack('<8f',v.uc.mem_read(args[2],args[3]*32))),indices=list(struct.unpack('<'+'H'*args[5],v.uc.mem_read(args[4],args[5]*2)))))
  for a in(0x56d5f0,0x56d400,0x56eb30):v.uc.hook_add(UC_HOOK_CODE,observe,begin=a,end=a)
def run(boot,out):
 old=q.ObservedWorld;q.ObservedWorld=PoseWorld
 try:q.run(boot,out,True,True)
 finally:q.ObservedWorld=old
 (out/'draws.json').write_text(json.dumps(worlds[-1].draws,indent=2)+'\n')
 return worlds[-1].draws
if __name__=='__main__':
 import argparse
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.output)
