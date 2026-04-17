// Decompiled methods and structure for class: cls_0x57be50

/*
/OOAnalyzer/cls_0x57be50
pack(disabled)
Structure cls_0x57be50 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0057be50

void __thiscall OOAnalyzer::cls_0x57be50::meth_0x57be50(cls_0x57be50 *this)

{
  undefined4 *puVar1;
  dword dVar2;
  undefined4 *puVar3;
  dword dVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  dword in_stack_00000004;
  
  puVar3 = FUN_00482fb0(in_stack_00000004 * 4);
  if ((int)this->mbr_0xc <= (int)in_stack_00000004) {
    in_stack_00000004 = this->mbr_0xc;
  }
  puVar1 = (undefined4 *)this->mbr_0x0;
  puVar5 = puVar1;
  puVar6 = puVar3;
  for (; in_stack_00000004 != 0; in_stack_00000004 = in_stack_00000004 - 1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  FUN_004830f0(puVar1);
  dVar2 = this->mbr_0x0;
  dVar4 = this->mbr_0xc + 8;
  this->mbr_0xc = dVar4;
  this->mbr_0x0 = (dword)puVar3;
  this->mbr_0x4 = (dword)(puVar3 + ((int)(this->mbr_0x4 - dVar2) >> 2));
  this->mbr_0x8 = (dword)(puVar3 + dVar4);
  return;
}



