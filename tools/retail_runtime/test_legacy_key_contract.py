"""Exact serialized legacy record, complete Snow decoder and setter contract."""
import tempfile,unittest
from pathlib import Path
from legacy_key_contract import run
class LegacyKeyContract(unittest.TestCase):
 def test_actual_serialized_decoder_and_all_setter_values(self):
  with tempfile.TemporaryDirectory()as d:
   report=run(Path(d)/'proof');self.assertEqual(report['status'],'pass');self.assertEqual(report['serialized_key_size'],8);self.assertEqual(report['channels'],45000);self.assertEqual(report['max_absolute_error'],0);self.assertEqual(report['setter_cases'],13824);self.assertEqual(report['errors'],[])
if __name__=='__main__':unittest.main()
