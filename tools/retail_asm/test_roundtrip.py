#!/usr/bin/env python3
"""Independent layout and build-verifier checks on a generated PE fixture."""
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
from reconstruct import reconstruct


def fixture():
    data=bytearray(0x600)
    data[:2]=b'MZ';struct.pack_into('<I',data,0x3c,0x80)
    data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HHIIIHH',data,0x84,0x14c,1,0,0,0,224,0x10f)
    optional=0x98
    struct.pack_into('<H',data,optional,0x10b)
    struct.pack_into('<I',data,optional+16,0x1000)
    struct.pack_into('<I',data,optional+28,0x400000)
    struct.pack_into('<II',data,optional+32,0x1000,0x200)
    struct.pack_into('<II',data,optional+56,0x2000,0x200)
    header=optional+224
    data[header:header+8]=b'.text\0\0\0'
    # Forward/backward PC-relative branches and an alternative MOV encoding.
    code=bytes.fromhex('e801000000c38bc175f6c80000f9c3')
    struct.pack_into('<IIIIIIHHI',data,header+8,len(code),0x1000,0x200,0x200,0,0,0,0,0x60000020)
    data[0x200:0x200+len(code)]=code
    # Exercise NASM's dangerous comment-backslash continuation in data rows.
    data[0x1f0:0x200]=b'ABCDEFGHIJKLMNO\\'
    data.extend(b'Overlay identity\0')
    return bytes(data)


class RoundTrip(unittest.TestCase):
    def test_layout_standalone_build_and_rejection(self):
        nasm=shutil.which('nasm');self.assertIsNotNone(nasm)
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);exe=root/'fixture.exe';original=fixture();exe.write_bytes(original)
            source=root/'source';reconstruct(exe,source,nasm)
            self.assertEqual((source/'Revenant.rebuilt.exe').read_bytes(),original)
            exe.unlink()  # Build cannot rely on the original input existing.
            (source/'Revenant.rebuilt.exe').unlink()
            subprocess.run([sys.executable,str(source/'build.py')],check=True,capture_output=True)
            self.assertEqual((source/'Revenant.rebuilt.exe').read_bytes(),original)
            header=next((source/'sections').glob('*file_00000000.asm'))
            text=header.read_text();self.assertIn('db 0x4d, 0x5a',text)
            header.write_text(text.replace('db 0x4d, 0x5a','db 0x4e, 0x5a',1))
            bad=subprocess.run([sys.executable,str(source/'build.py')],capture_output=True)
            self.assertNotEqual(bad.returncode,0)
            self.assertFalse(json.loads((source/'verification.json').read_text())['byte_identical'])


if __name__=='__main__':unittest.main()
