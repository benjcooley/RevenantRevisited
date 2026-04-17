// Decompiled methods and structure for class: cls_0x415ad0

/*
/OOAnalyzer/cls_0x415ad0
pack(disabled)
Structure cls_0x415ad0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00415ad0

undefined4 __thiscall OOAnalyzer::cls_0x415ad0::meth_0x415ad0(cls_0x415ad0 *this,int param_1)

{
  int **ppiVar1;
  int iVar2;
  int *in_stack_00000008;
  
  if ((((this->mbr_0x0 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) &&
     (ppiVar1 = *(int ***)(DAT_005e8870 + param_1 * 4), ppiVar1 != (int **)0x0)) {
    ppiVar1[7] = (int *)(float)*in_stack_00000008;
    ppiVar1[8] = (int *)(float)in_stack_00000008[1];
    iVar2 = **ppiVar1;
    ppiVar1[9] = (int *)(float)in_stack_00000008[2];
    (**(code **)(iVar2 + 0x10))(*ppiVar1,ppiVar1 + 1);
    return 1;
  }
  return 0;
}



