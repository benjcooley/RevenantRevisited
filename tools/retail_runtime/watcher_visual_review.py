"""Freeze explicitly qualified watcher reference/actual Metal pairs; no approval."""
import argparse,hashlib,json
from pathlib import Path
from PIL import Image,ImageDraw

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def run(root,output):
 output.mkdir(parents=True,exist_ok=False);cases=[];pins={}
 def pin(p):pins[str(p)]=sha(p)
 for i in(1,2,3):
  name=f'ogrokwatcher{i}';reference=root/'watcher-qualified-device-semantic-final';metal=root/f'watcher{i}-metal-native-final';j=json.loads((metal/'manifest.json').read_text());assert j['status']=='pass' and len(j['frames'])==8 and j['actual_viewport']==[640,480];pin(metal/'manifest.json');pin(reference/'qualified-device.json')
  detail=Image.new('RGB',(800,1640),'#222222');d=ImageDraw.Draw(detail);whole=Image.new('RGB',(1280,8*500),'#222222');w=ImageDraw.Draw(whole);frames=[]
  for index,frame in enumerate(j['frames']):
   paths=[reference/name/f'native-{index+1:03d}.png',Path(frame['path'])];bboxes=[]
   for column,(p,label)in enumerate(zip(paths,['Adapted device1555→4444 / semantic alpha15','Actual Metal correct1555 / native owner+projector'])):
    pin(p);im=Image.open(p).convert('RGB');assert im.size==(640,480);bbox=im.getbbox();assert bbox and bbox[0]>=250 and bbox[1]>=70 and bbox[2]<=390 and bbox[3]<=260;bboxes.append(bbox)
    whole.paste(im,(column*640,index*500+20));w.text((column*640+5,index*500+3),f'{label}; tick{index+1}',fill='white')
    if index<4:
     detail.paste(im.crop((250,70,390,260)).resize((280,380),Image.Resampling.NEAREST),(column*400+60,index*410+25));d.text((column*400+5,index*410+5),f'{label}; tick{index+1}',fill='white')
   frames.append(dict(tick=index+1,authored_frame=(index+1)%4,reference=str(paths[0]),metal=str(paths[1]),bboxes=bboxes))
  for suffix,image in [('detail',detail),('whole',whole)]:p=output/f'{name}-{suffix}.png';image.save(p);pin(p)
  cases.append(dict(name=name,binary_sha256=j['binary_sha256'],frames=frames))
 old=json.loads((root/'watcher1-metal-final/manifest.json').read_text());new=json.loads((root/'watcher1-metal-common-regression/manifest.json').read_text());assert old['pixel_summary']['pixel_sha256']==new['pixel_summary']['pixel_sha256'];pin(root/'watcher1-metal-common-regression/manifest.json')
 for p in[root/'watcher-pose-contract-final/manifest.json',root/'watcher-pose-native/draws.json',root/'watcher-decoder-final/manifest.json',root/'watcher-surface-flow-final/surface-flow.json']:pin(p)
 report=dict(accepted=False,device_identical=False,qualified_reference=True,cases=cases,pins=pins,common_regression_exact_frames=8,crop=[250,70,390,260],enlargement=2,scope='Qualified adapted-device appearance plus actual original fullRender pose/state and exhaustive actual production1555 decoder contracts. Original software1555 corruption and original converter alpha8 remain negative controls. Original/software device equality, RGB5→RGB4 loss, sampling/raster/exactpixels, generic ownerPulse and natural placement remain open.')
 (output/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(sha(output/'pair-manifest.json'));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--root',required=True,type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args();run(a.root,a.output)
