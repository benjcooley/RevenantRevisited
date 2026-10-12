"""Original machine loop versus bulk emulation; flags, bytes and restore."""
import struct,unittest
from pathlib import Path
from runtime import Runtime
from native_store_loop import install,START,END,COPY_START,COPY_END,REGISTER_LOOPS
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_EBP,UC_X86_REG_EDI,UC_X86_REG_ESI,UC_X86_REG_EFLAGS
ROOT=Path(__file__).resolve().parents[2]

class StoreLoopTests(unittest.TestCase):
 def test_modified_last_opcode_block_rejects_before_installing_any_hook(self):
  vm=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');added=[];original=vm.uc.hook_add
  def observed(*args,**kwargs):added.append((args,kwargs));return original(*args,**kwargs)
  vm.uc.hook_add=observed;vm.write_code(REGISTER_LOOPS[-1][0],b'\x90')
  with self.assertRaisesRegex(ValueError,'register-store opcodes differ'):install(vm)
  self.assertEqual(added,[])
 def test_all_original_register_copy_and_fill_loops(self):
  raw=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');fast=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');stats=install(fast)
  for vm in(raw,fast):vm.test_source=vm.allocate(1100);vm.test_buffer=vm.allocate(1100);vm.checkpoint()
  for start,end,_,register,width,kind in REGISTER_LOOPS:
   for count in(1,2,3,31,60,121,255):
    for offset in(0,1,3):
     for value in(0,0x7f7f7f7f,0xffffffff,0x12345678):
      for flags in(0x202,0x203,0x602,0xad7):
       results=[]
       for vm in(raw,fast):
        vm.restore();vm.write(vm.test_source,bytes((i*13+7)&255 for i in range(1100)));vm.write(vm.test_buffer,b'\xa5'*1100)
        for reg,data in((register,count),(UC_X86_REG_ESI,vm.test_source),(UC_X86_REG_EDI,vm.test_buffer+offset),(UC_X86_REG_EAX,value),(UC_X86_REG_EFLAGS,flags)):vm.uc.reg_write(reg,data)
        vm.uc.emu_start(start,end,count=7*count+10)
        results.append((bytes(vm.uc.mem_read(vm.test_buffer,1100)),vm.uc.reg_read(register),vm.uc.reg_read(UC_X86_REG_ESI)-vm.test_source,vm.uc.reg_read(UC_X86_REG_EDI)-vm.test_buffer,vm.uc.reg_read(UC_X86_REG_EAX),vm.uc.reg_read(UC_X86_REG_EFLAGS)))
       self.assertEqual(*results,(hex(start),count,kind))
  self.assertEqual(stats['register_loops'],1680)
 def test_original_byte_to_word_copy_registers_flags_and_bytes(self):
  raw=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');fast=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');stats=install(fast)
  for vm in(raw,fast):vm.test_frame=vm.allocate(64)+16;vm.test_source=vm.allocate(300);vm.test_buffer=vm.allocate(600);vm.checkpoint()
  for count in(1,2,3,31,60,121,255):
   for offset in(0,1,3):
    for value in(0,0x7f7f7f7f,0xffffffff,0x12345678):
     for flags in(0x202,0x203,0x602,0xad7):
      results=[]
      for vm in(raw,fast):
       vm.restore();vm.put_u32(vm.test_frame-8,count);vm.write(vm.test_source,bytes((i*13+7)&255 for i in range(300)));vm.write(vm.test_buffer,b'\xa5'*600)
       for reg,data in((UC_X86_REG_EBP,vm.test_frame),(UC_X86_REG_ESI,vm.test_source),(UC_X86_REG_EDI,vm.test_buffer+offset),(UC_X86_REG_EAX,value),(UC_X86_REG_EFLAGS,flags)):vm.uc.reg_write(reg,data)
       vm.uc.emu_start(COPY_START,COPY_END,count=7*count+10)
       results.append((bytes(vm.uc.mem_read(vm.test_buffer,600)),vm.u32(vm.test_frame-8),vm.uc.reg_read(UC_X86_REG_ESI)-vm.test_source,vm.uc.reg_read(UC_X86_REG_EDI)-vm.test_buffer,vm.uc.reg_read(UC_X86_REG_EAX),vm.uc.reg_read(UC_X86_REG_EFLAGS)))
      self.assertEqual(*results)
  self.assertEqual(stats['copy_loops'],336);fast.restore();self.assertEqual(bytes(fast.uc.mem_read(fast.test_buffer,600)),bytes(600))
 def test_original_memory_registers_flags_and_checkpoint(self):
  raw=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');fast=Runtime(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');stats=install(fast)
  for vm in (raw,fast):vm.test_frame=vm.allocate(64)+16;vm.test_buffer=vm.allocate(1100);vm.checkpoint()
  checks=0
  for count in (1,2,3,31,60,121,255):
   for offset in (0,1,3):
    for value in (0,0x7f7f7f7f,0xffffffff,0x12345678):
     for flags in (0x202,0x203,0x602,0xad7):
      results=[]
      for vm in (raw,fast):
       vm.restore();vm.put_u32(vm.test_frame-8,count);vm.write(vm.test_buffer,b'\xa5'*1100)
       for reg,data in ((UC_X86_REG_EBP,vm.test_frame),(UC_X86_REG_EDI,vm.test_buffer+offset),(UC_X86_REG_EAX,value),(UC_X86_REG_EFLAGS,flags)):vm.uc.reg_write(reg,data)
       vm.uc.emu_start(START,END,count=5*count+10)
       results.append((bytes(vm.uc.mem_read(vm.test_buffer,1100)),vm.u32(vm.test_frame-8),vm.uc.reg_read(UC_X86_REG_EDI)-vm.test_buffer,vm.uc.reg_read(UC_X86_REG_EAX),vm.uc.reg_read(UC_X86_REG_EFLAGS)))
      self.assertEqual(*results);checks+=1
  self.assertEqual(checks,336);self.assertEqual(stats['loops'],336)
  fast.restore();self.assertEqual(bytes(fast.uc.mem_read(fast.test_buffer,1100)),bytes(1100));self.assertEqual(fast.u32(fast.test_frame-8),0)

if __name__=='__main__':unittest.main()
