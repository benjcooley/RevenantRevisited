"""Compile the actual Icedsparks profile and compare every fingerprint to its own asset."""
import json,struct,subprocess,tempfile,unittest,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset

def fingerprint(raw):
 value=1469598103934665603
 for byte in raw:value=((value^byte)*1099511628211)&0xffffffffffffffff
 return value

class IcedsparksProfile(unittest.TestCase):
 def test_literal_identity_and_all_fifty_roots(self):
  header=(ROOT/'src/game/effects/genericstaticprofile.h').read_text();declarations=header[header.index('namespace generic_static_profile {'):header.index('inline uint64_t Hash(')]+'}\n'
  source='#include <cstdint>\n#include <cstdio>\n#include <cassert>\n'+declarations+r'''
int main(){using namespace generic_static_profile;const auto*p=Find(0xacabfadau);assert(p&&p->frames==100&&p->objects==50&&p->key_bytes==8);assert(!Find(0xffffffffu));assert(!Find(0xacabfadau^1u));printf("%s\n",p->path);
for(int i=0;i<p->objects;++i){const auto&o=p->object[i];printf("%s %d %d %d %d %llu %llu %llu\n",o.name,o.material,o.vertices,o.faces,o.keys,(unsigned long long)o.key_hash,(unsigned long long)o.vertex_hash,(unsigned long long)o.face_hash);}}
'''
  with tempfile.TemporaryDirectory()as directory:
   d=Path(directory);(d/'profile.cpp').write_text(source);subprocess.run(['clang++','-std=c++17',str(d/'profile.cpp'),'-o',str(d/'profile')],check=True);lines=subprocess.check_output([str(d/'profile')],text=True).splitlines()
  self.assertEqual(lines[0],'magic/iced.i3d')
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/iced.i3d'))
  asset=read_asset(data);faces=bytes.fromhex(asset['face_hex'])
  for line,obj in zip(lines[1:],asset['objects']):
   fields=line.split();counts=obj['texture_runs'][1::2];starts=obj['texture_runs'][::2];rawfaces=b''.join(faces[start*6:(start+count)*6]for start,count in zip(starts,counts))
   self.assertEqual(fields[0],obj['name'])
   self.assertEqual(list(map(int,fields[1:])),[obj['material'],obj['vertex_count'],sum(counts),100,fingerprint(bytes.fromhex(obj['states'][0]['legacy_key_hex'])),fingerprint(bytes.fromhex(obj['vertex_hex'])),fingerprint(rawfaces)])
  self.assertEqual(len(lines),51)

if __name__=='__main__':unittest.main()
