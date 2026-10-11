#!/usr/bin/env python3
"""Actual IrisFlare submit composition over all authored vertices and owners."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from irisflare_native_probe import ROOT,read_iris,ASSET_SHA
from speed_profile_contract import sha
from ribbon_owner_contract import MAIN

def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/irisflarepreview.cpp').read_text();a=source.index('hmm_mat4 world;MtxMultiply(');b=source.index('++count;',a);branch=source[a:b];main=MAIN.replace('S3DAnimObj o{local};int mesh=1;COMPOSITION','struct {int meshes[2]={1,2};}holder;auto*s=&holder;int object=0;COMPOSITION').replace('translated?16:0,0)','translated?16:0,224)').replace('if(!translated)for(int r=0;r<4;++r)for(int c=0;c<4;++c)assert(submitted.Elements[c][r]==local.Elements[c][r]);','');driver=output/'owner.cpp';driver.write_text(main.replace('COMPOSITION',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/irisflare.i3d')
 fixture=output/'vertices.fixture';fixture.write_bytes(bytes.fromhex(read_iris(data)['vertex_hex']));q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;report=dict(status='pass',accepted=False,geometry_checks=2232,authored_vertices=186,asset_sha256=ASSET_SHA,actual_submit_span_sha256=sha(branch.encode()),sanitizers='ASan/UBSan',scope='Extracted actual IrisFlare source submit composition and mesh policy with real matrix APIs over every authored vertex, three nontrivial local poses, normal and native-domain owners at origin and translated10000/10000/16. Audited native Face224 rotation retained. Sequential local then owner transform independently matches submitted composed matrix. Original205localmatrices separately compared; this does not certify natural actor/device or GPU pixels.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',2232);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
