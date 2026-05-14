// Decompiled methods and structure for class: cls_0x41b0b0

/*
/OOAnalyzer/cls_0x41b0b0
pack(disabled)
Structure cls_0x41b0b0 {
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   byte   1   mbr_0x44   "Unsigned Byte (db)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 76 Alignment: 1

*/

// Function at 0041b0b0

undefined4 __thiscall
OOAnalyzer::cls_0x41b0b0::meth_0x41b0b0_TArea_In(cls_0x41b0b0 *this,undefined4 *param_1)

{
  dword *this_00;
  undefined4 uVar1;
  cls_0x41c720 *this_01;
  int iVar2;
  int iVar3;
  dword in_stack_00000008;
  undefined4 uVar4;
  
  if ((this->mbr_0x44 & 1) != 0) {
    return 1;
  }
  if ((this->mbr_0x40 == 0xffffffff) || (in_stack_00000008 == this->mbr_0x40)) {
    this_00 = &this->mbr_0x48;
    iVar3 = 0;
    uVar1 = *param_1;
    if (0 < *(short *)this_00) {
      do {
        uVar4 = uVar1;
        this_01 = (cls_0x41c720 *)cls_0x41c780::meth_0x41c780((cls_0x41c780 *)this_00);
        iVar2 = cls_0x41c720::meth_0x41c720(this_01,uVar4);
        if (iVar2 != 0) {
          return 1;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(short *)this_00);
    }
  }
  return 0;
}



