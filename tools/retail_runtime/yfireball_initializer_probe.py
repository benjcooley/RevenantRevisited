"""Execute the YFireBall initializer's conditional Yhagoro lift branch."""
import argparse,sys,json,zipfile,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
from fireball_visual_reference import FireBallScene
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
exe=args.executable;records=[]
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/Yfireball.I3D')
for name,flags,expected in [('fixture-invoker',0,128),('yhagoro',0,188),('yhagoro',0x4000,128),('YHAGORO',0,188)]:
 f=FireBallScene(exe,data,effect='YFireBall',invoker_name=name,invoker_flags=flags);actual=f.vm.u32(f.owner+0x18);assert actual==expected,(name,actual);records.append(dict(invoker=name,flags=flags,source_z=78,actual_z=actual,expected_z=expected));print(records[-1],flush=True)
args.output.mkdir(parents=True,exist_ok=True)
p=args.output/'initializer-controls.json';p.write_text(json.dumps(dict(status='pass',retail_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),asset_sha256=hashlib.sha256(data).hexdigest(),records=records,scope='Actual Y initializer branch; explicit invoker name/flags resource. Does not certify a natural NPC caller.'),indent=2)+'\n')
