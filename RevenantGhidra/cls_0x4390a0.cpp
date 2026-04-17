// Decompiled methods and structure for class: cls_0x4390a0

/*
/OOAnalyzer/cls_0x4390a0
pack(disabled)
Structure cls_0x4390a0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 004390a0

undefined4 __thiscall OOAnalyzer::cls_0x4390a0::meth_0x4390a0(cls_0x4390a0 *this,byte *param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if ((this->mbr_0x0 != 0) && (iVar2 = 0, 0 < (int)this->mbr_0x4)) {
    do {
      puVar3 = *(undefined4 **)(this->mbr_0x14 + iVar2 * 4);
      if (puVar3 == (undefined4 *)0x0) {
        puVar3 = (undefined4 *)this->mbr_0x18;
      }
      if ((puVar3 != (undefined4 *)0x0) &&
         (uVar1 = FUN_0059a530(puVar3[1],(byte *)puVar3[1],param_1), uVar1 == 0)) {
        for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
          *in_stack_00000008 = *puVar3;
          puVar3 = puVar3 + 1;
          in_stack_00000008 = in_stack_00000008 + 1;
        }
        return 1;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x4);
  }
  return 0;
}



