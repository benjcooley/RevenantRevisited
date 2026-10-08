"""Named VFX images must pin real bytes and preserve fixed-address contracts."""
import hashlib
from pathlib import Path
import struct
import tempfile
import unittest
from build_contract import BASELINE,RETAIL_SHA,verify_build
from runtime import pe_layout


class BuildContractTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.path=Path(self.temp.name)/'Revenant.private.exe'
        self.original=BASELINE.read_bytes();self.data=bytearray(self.original)

    def tearDown(self):self.temp.cleanup()

    def record(self,spans=None):
        self.path.write_bytes(self.data)
        patch=dict(purpose='bounded fixture identity test')
        if spans is not None:patch['allowed_modified_spans']=spans
        return dict(name='private',build_id='private-test',executable=str(self.path),
            executable_sha256=hashlib.sha256(self.data).hexdigest(),baseline_sha256=RETAIL_SHA,patch_manifest=patch)

    def test_baseline_and_explicit_variant_identity(self):
        self.assertFalse(verify_build(BASELINE)['experimental'])
        self.data[-1]^=1;record=self.record([[len(self.data)-1,len(self.data)]])
        with self.assertRaisesRegex(ValueError,'explicit named'):verify_build(self.path)
        self.assertTrue(verify_build(self.path,record)['experimental'])
        self.assertEqual(hashlib.sha256(BASELINE.read_bytes()).hexdigest(),RETAIL_SHA)

    def test_wrong_hash_path_and_parent_are_rejected(self):
        record=self.record()
        for field,value in [('executable_sha256','wrong'),('executable',str(BASELINE)),('baseline_sha256','wrong')]:
            bad=dict(record);bad[field]=value
            with self.subTest(field=field),self.assertRaises(ValueError):verify_build(self.path,bad)

    def test_fixed_address_layout_and_undeclared_changes_are_rejected(self):
        layout=pe_layout(self.data)
        struct.pack_into('<I',self.data,layout['optional_offset']+16,layout['entry_rva']+1)
        with self.assertRaisesRegex(ValueError,'unchanged PE'):verify_build(self.path,self.record())
        self.data=bytearray(self.original);self.data[-1]^=1
        with self.assertRaisesRegex(ValueError,'Undeclared'):verify_build(self.path,self.record([]))


if __name__=='__main__':unittest.main(verbosity=2)
