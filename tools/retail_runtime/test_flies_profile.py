"""Exact Flies guard independent mutation checks."""
import tempfile,unittest
from pathlib import Path
from flies_profile_contract import run
class FliesProfile(unittest.TestCase):
 def test_actual_asset_guard(self):
  with tempfile.TemporaryDirectory()as d:
   r=run(Path(d)/'proof');self.assertEqual(r['status'],'pass');self.assertEqual(r['positive'],3);self.assertEqual(r['negative'],83)
if __name__=='__main__':unittest.main()
