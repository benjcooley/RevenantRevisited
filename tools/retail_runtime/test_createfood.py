"""Createfood genuine loaded150-slot owner/render and isolated context regression."""
import contextlib,io,json,tempfile,unittest
from pathlib import Path
from createfood_loaded_preflight import run,ROOT
from createfood_core_probe import run as compare
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
class Createfood(unittest.TestCase):
 @unittest.skipUnless(BOOT.exists(),'Requires source-matched original startup cache')
 def test_loaded_lifecycle_and_full_draws(self):
  with tempfile.TemporaryDirectory(prefix='createfood-test-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);r=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',BOOT,p/'native',85,samples=(),initialize_lighting=True)
   self.assertIsNone(r['failure']);self.assertEqual(r['initial']['capacity'],150)
   self.assertEqual([x['tick']for x in r['records']if x['state']['flags']&4096],list(range(80,86)))
   self.assertEqual([x['tick']for x in r['food_requests']],[60])
   self.assertEqual(r['index_events'][-1]['eax'],1297753000)
   self.assertEqual(r['raw_calls_tick60'][-2:],['0x44ce55','0x44ce6d'])
   compare(p/'native/manifest.json',p/'source');proof=json.loads((p/'source/manifest.json').read_text());self.assertEqual(proof['lookup_words'],1024);self.assertEqual(proof['pool_fields'],255000);self.assertEqual(proof['matrix_channels'],61168)
if __name__=='__main__':unittest.main()
