"""Genuine loaded hardware/software reference for three proven RGB8 profiles."""
import argparse,json
from pathlib import Path
from fspray_device_reference import run
from character_native_preflight import ROOT,sha
PROFILES={'Regeneration':'0x10ac03de','SwiftStrike':'0xe0a3bc43','nullifier':'0xad92bd38'}
SAMPLES=(0,1,5,10,15,20,25,30,39,45,59,60,65,71,76)
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--profile',choices=PROFILES,required=True);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--software',action='store_true');a=p.parse_args()
 run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.software,a.profile,PROFILES[a.profile],77,SAMPLES)
 path=a.output/'manifest.json';m=json.loads(path.read_text());m['profile_driver_sha256']=sha(__file__);path.write_text(json.dumps(m,indent=2)+'\n')
