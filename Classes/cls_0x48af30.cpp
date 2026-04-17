// Decompiled methods and structure for class: cls_0x48af30

/*
/OOAnalyzer/cls_0x48af30
pack(disabled)
Structure cls_0x48af30 {
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 200 Alignment: 1

*/

// Function at 0048af30

void __thiscall OOAnalyzer::cls_0x48af30::meth_0x48af30(cls_0x48af30 *this)

{
  cls_0x4741b0 *this_00;
  dword dVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int in_stack_00000004;
  
  this_00 = (cls_0x4741b0 *)(in_stack_00000004 + 0x14);
  dVar1 = this->mbr_0xa0;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xa4;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xa8;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xac;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xb0;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xb4;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xb8;
  dVar2 = this->mbr_0xc4;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = *(int *)(*(int *)(in_stack_00000004 + 0x34) + dVar2 * 4);
    if (iVar4 == 0) {
      iVar4 = *(int *)(in_stack_00000004 + 0x38);
    }
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  dVar1 = this->mbr_0xbc;
  iVar3 = cls_0x4741b0::meth_0x474210(this_00);
  if (-1 < iVar3) {
    iVar4 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)(in_stack_00000004 + 0x24));
    *(dword *)(*(int *)(iVar4 + 0x10) + iVar3 * 4) = dVar1;
  }
  return;
}



