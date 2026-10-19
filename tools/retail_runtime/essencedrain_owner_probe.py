#!/usr/bin/env python3
"""Actual complete EssenceDrain submit composition under translated owners."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from essencedrain_native_preflight import ROOT,ASSET_SHA
from speed_controller_preflight import read_asset
from speed_profile_contract import sha
from ribbon_owner_contract import MAIN

def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/essencedrainpreview.cpp').read_text();a=source.index('hmm_mat4 world;MtxMultiply(');b=source.index('++draws;',a);branch=source[a:b];main=MAIN.replace('void SubmitRetailSourceAlphaAfterFx','void SubmitGoldFlareAfterFx').replace('bool retail_source_alpha,retail_software_projection;','bool retail_source_alpha,retail_software_projection,additive_blend,retail_additive_cull_none;float uv_offset[2]{};').replace('S3DAnimObj o{local};int mesh=1;COMPOSITION','struct{int renders=3;}holder;auto*s=&holder;struct{int mesh=1;}part;struct{int keys=49;}profile;COMPOSITION').replace('186*8','314*8').replace('5952','10048').replace('i<186','i<314').replace('capture.draw.retail_lighting==3&&capture.draw.retail_source_alpha','capture.draw.retail_lighting==1&&capture.draw.additive_blend&&capture.draw.retail_additive_cull_none');driver=output/'owner.cpp';driver.write_text(main.replace('COMPOSITION',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/worm.i3d'))
 a=read_asset(data);u=lambda p:int.from_bytes(data[p:p+4],'little');rel=lambda p:p+u(p);raw=rel(rel(rel(a['body_offset']+16)));fixture=output/'vertices.fixture';fixture.write_bytes(data[raw:raw+10048]);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;report=dict(status='pass',accepted=False,geometry_checks=3768,authored_vertices=314,asset_sha256=ASSET_SHA,actual_submit_span_sha256=sha(branch.encode()),sanitizers='ASan/UBSan',scope='Actual production mesh composition/policies across all314authored vertices, three nontrivial local poses andnormal/native origin/translated10000,10000,16 owners. Sequential local+owner independently matches composed submitted matrix. Litadd/cull-none/native projection and sourceUV path retained. No natural-device/actor/lighting or pixel equality claim.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',3768);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
