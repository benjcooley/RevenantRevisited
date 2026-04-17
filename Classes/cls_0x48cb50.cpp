// Decompiled methods and structure for class: cls_0x48cb50

/*
/OOAnalyzer/cls_0x48cb50
pack(disabled)
Structure cls_0x48cb50 {
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 84 Alignment: 1

*/

// Function at 0048cb50

int __thiscall OOAnalyzer::cls_0x48cb50::meth_0x48cb50(cls_0x48cb50 *this,undefined4 param_1)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  int in_stack_00000008;
  
  switch(param_1) {
  case 1:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x2c) {
      piVar2 = (int *)this->mbr_0x3c;
      piVar3 = piVar2;
      while (in_stack_00000008 != *(int *)(*piVar3 + 0xc4)) {
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
        if ((int)this->mbr_0x2c <= iVar1) {
          return 0;
        }
      }
LAB_0048cbf0:
      return piVar2[iVar1];
    }
    break;
  case 2:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x40) {
      piVar2 = (int *)this->mbr_0x50;
      piVar3 = piVar2;
      while (in_stack_00000008 != *(int *)(*piVar3 + 0xc0)) {
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
        if ((int)this->mbr_0x40 <= iVar1) {
          return 0;
        }
      }
      goto LAB_0048cbf0;
    }
    break;
  case 0xb:
  case 0xc:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x18) {
      piVar2 = (int *)this->mbr_0x28;
      piVar3 = piVar2;
      do {
        if (in_stack_00000008 == *(int *)(*piVar3 + 0xc0)) goto LAB_0048cbf0;
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
      } while (iVar1 < (int)this->mbr_0x18);
    }
  }
  return 0;
}



