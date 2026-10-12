"""Review fixed 77-update references and 75-image films for exact RGB8 profiles."""
import argparse,json
from pathlib import Path
from fspray_device_review import review
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','repeat','software','before','after','sw-before','sw-after','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();limits=()
 if json.loads(a.native.read_text())['type_id']=='0xad92bd38':limits=('Nullifier retains a visible bright-core coverage/shape residual near phases5/30 (central notch at30 also present before the color correction); color correction does not certify whole hardware geometry/coverage.',)
 review(a.native,a.repeat,a.software,a.before,a.after,a.sw_before,a.sw_after,a.output,77,75,(250,180,400,340),limits)
