// Decompiled methods and structure for class: cls_0x53f730

/*
/OOAnalyzer/cls_0x53f730
pack(disabled)
Structure cls_0x53f730 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 336 Alignment: 1

*/

// Function at 0053f730

void __thiscall OOAnalyzer::cls_0x53f730::meth_0x53f730(cls_0x53f730 *this,int param_1)

{
  dword dVar1;
  dword *pdVar2;
  int iVar3;
  dword in_stack_00000008;
  
  if (param_1 == 1) {
    if (this->mbr_0x4 != in_stack_00000008) goto LAB_0053f772;
  }
  else if ((((param_1 != 2) || (dVar1 = this->mbr_0x4, dVar1 == 0)) ||
           (*(dword *)(dVar1 + 0x44) != in_stack_00000008)) ||
          ((*(uint *)(dVar1 + 8) & 0x80000) != 0)) goto LAB_0053f772;
  (**(code **)(this->mbr_0x0 + 0xc))();
  this->mbr_0x4 = 0;
LAB_0053f772:
  iVar3 = 0;
  if (0 < (int)this->mbr_0x13c) {
    pdVar2 = (dword *)this->mbr_0x14c;
    do {
      dVar1 = *pdVar2;
      if (param_1 == 1) {
        if (dVar1 == in_stack_00000008) {
LAB_0053f7af:
          (**(code **)(this->mbr_0x0 + 0xc))();
          break;
        }
      }
      else if (((param_1 == 2) && (dVar1 != 0)) &&
              ((*(dword *)(dVar1 + 0x44) == in_stack_00000008 && ((*(byte *)(dVar1 + 10) & 8) == 0))
              )) goto LAB_0053f7af;
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar3 < (int)this->mbr_0x13c);
  }
  iVar3 = 0;
  if (0 < (int)this->mbr_0xc) {
    pdVar2 = &this->mbr_0x10;
    do {
      if (param_1 == 1) {
        if (*pdVar2 == in_stack_00000008) {
LAB_0053f7e6:
          *pdVar2 = 0;
        }
      }
      else if (((param_1 == 2) && (dVar1 = *pdVar2, dVar1 != 0)) &&
              ((*(dword *)(dVar1 + 0x44) == in_stack_00000008 &&
               ((*(uint *)(dVar1 + 8) & 0x80000) == 0)))) goto LAB_0053f7e6;
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar3 < (int)this->mbr_0xc);
  }
  return;
}



