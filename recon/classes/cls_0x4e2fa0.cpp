// Decompiled methods and structure for class: cls_0x4e2fa0

/*
/OOAnalyzer/cls_0x4e2fa0
pack(disabled)
Structure cls_0x4e2fa0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 392 Alignment: 1

*/

// Function at 004e2fa0

void __thiscall OOAnalyzer::cls_0x4e2fa0::meth_0x4e2fa0(cls_0x4e2fa0 *this)

{
  dword in_stack_00000004;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  this->mbr_0x184 = in_stack_00000004;
  if (in_stack_00000004 != 0) {
    local_c = *(undefined4 *)(in_stack_00000004 + 0x10);
    local_8 = *(undefined4 *)(in_stack_00000004 + 0x14);
    local_4 = *(undefined4 *)(in_stack_00000004 + 0x18);
    (**(code **)(this->mbr_0x0 + 8))(&local_c,0xffffffff,0);
  }
  return;
}



