"""Full loaded watcher reference using the explicitly patched original1555 leaf."""
import argparse,json
from pathlib import Path
import watcher_loaded_visual_reference as reference
from watcher_surface_flow_probe import ObservedWorld
from watcher_retail_1555_patch import apply_vm,sha

def run(boot,patch,output):
 worlds=[]
 def factory(*args,**kwargs):
  f=ObservedWorld(*args,**kwargs);f.qualified_patch=apply_vm(f.vm,patch);worlds.append(f);return f
 old=reference.AuthenticCharacters;reference.AuthenticCharacters=factory
 try:reference.run(boot,output)
 finally:reference.AuthenticCharacters=old
 f=worlds[0];r=json.loads((output/'manifest.json').read_text());r.update(status='qualified_patched_original_complete',modified_retail=True,device_identical=False,patch_manifest=str(patch.resolve()),patch_manifest_sha256=sha(patch.read_bytes()),patched_executable_sha256=f.qualified_patch['patched_sha256'],scope=f.qualified_patch['scope']+' The unchanged cached startup is restored, then exact PE patch bytes and added section are mapped before any scene/resource calls. This is the patched build state, with unchanged original startup cache as an explicit replay boundary. No host texel/descriptor/color/geometry conversion.')
 r['uploads']=[e for e in f.events if e['function']=='0x416790'];assert len(r['uploads'])==14 and all(e['source_pixels_sha256']==e['destination_pixels_sha256']for e in r['uploads'])
 r['selected_surfaces']=[e for e in f.events if e['function']=='SetTexture-return'];assert len(r['selected_surfaces'])==24;assert all(e['pixel_format']==[32,65,0,16,0x7c00,0x3e0,0x1f,0x8000]for e in r['selected_surfaces']);r['kernels']=f.kernels;r['patch_flag_after']=f.vm.u32(f.qualified_patch['flag_address']);assert r['patch_flag_after']==1
 (output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('QUALIFIED',r['patched_executable_sha256']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--patch',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.patch,a.output)
