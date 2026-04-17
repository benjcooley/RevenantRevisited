// Decompiled methods and structure for class: cls_0x4367d0

/*
/OOAnalyzer/cls_0x4367d0
pack(disabled)
Structure cls_0x4367d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 168 Alignment: 1

*/

// Function at 004367d0

void __thiscall OOAnalyzer::cls_0x4367d0::meth_0x4367d0(cls_0x4367d0 *this)

{
  undefined4 *puVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) && (in_stack_00000004 < (int)this->mbr_0x88)) {
    if (*(dword *)(this->mbr_0x98 + in_stack_00000004 * 4) == this->mbr_0x9c) {
      this->mbr_0x9c = 0;
    }
    if (*(dword *)(this->mbr_0x98 + in_stack_00000004 * 4) == this->mbr_0xa4) {
      this->mbr_0xa4 = 0;
    }
    if (*(dword *)(this->mbr_0x98 + in_stack_00000004 * 4) == this->mbr_0xa0) {
      this->mbr_0xa0 = 0;
    }
    puVar1 = *(undefined4 **)(this->mbr_0x98 + in_stack_00000004 * 4);
    if (puVar1 != (undefined4 *)0x0) {
      (**(code **)*puVar1)(1);
    }
    cls_0x45f7c0::meth_0x41cb80((cls_0x45f7c0 *)&this->mbr_0x88);
    dVar2 = this->mbr_0x88;
    iVar4 = 0;
    if (0 < (int)dVar2) {
      piVar5 = (int *)this->mbr_0x98;
      do {
        iVar3 = *piVar5;
        piVar5 = piVar5 + 1;
        *(int *)(iVar3 + 0xc) = iVar4;
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)dVar2);
    }
    (**(code **)(this->mbr_0x0 + 0x2c))(1);
  }
  return;
}



