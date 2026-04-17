// Decompiled methods and structure for class: cls_0x5723a0

/*
/OOAnalyzer/cls_0x5723a0
pack(disabled)
Structure cls_0x5723a0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 005723a0

uint __thiscall
OOAnalyzer::cls_0x5723a0::meth_0x5723a0(cls_0x5723a0 *this,char param_1,undefined4 param_2)

{
  int *piVar1;
  int in_stack_0000000c;
  
  piVar1 = (int *)this->mbr_0x0;
  if (*piVar1 != 0) {
    for (piVar1 = (int *)piVar1[2]; piVar1 != (int *)0x0; piVar1 = (int *)piVar1[2]) {
      if (((*(char *)(piVar1 + 3) == param_1) && (*(char *)((int)piVar1 + 0xd) == (char)param_2)) &&
         (piVar1[4] == in_stack_0000000c)) {
        return CONCAT31((int3)((uint)piVar1 >> 8),1);
      }
      piVar1 = (int *)*piVar1;
      if (*piVar1 == 0) break;
    }
  }
  return (uint)piVar1 & 0xffffff00;
}



