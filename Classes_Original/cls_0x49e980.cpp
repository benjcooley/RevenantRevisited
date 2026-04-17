// Decompiled methods and structure for class: cls_0x49e980

/*
/OOAnalyzer/cls_0x49e980
pack(disabled)
Structure cls_0x49e980 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   444   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   468   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
   496   dword   4   mbr_0x1f0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 500 Alignment: 1

*/

// Function at 0049e980

cls_0x49e980 * __thiscall OOAnalyzer::cls_0x49e980::cls_0x49e980(cls_0x49e980 *this)

{
  int iVar1;
  uint uVar2;
  dword *pdVar3;
  undefined4 *puVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dcda;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x100);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x200);
  local_4._0_1_ = 1;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x200);
  local_4 = CONCAT31(local_4._1_3_,2);
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x200);
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0x8 = 0xffffffff;
  pdVar3 = &this->mbr_0x110;
  for (iVar1 = 0x20; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar3 = 0;
    pdVar3 = pdVar3 + 1;
  }
  pdVar3 = &this->mbr_0xc;
  for (iVar1 = 0x41; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar3 = 0;
    pdVar3 = pdVar3 + 1;
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  this->mbr_0x190 = 0;
  this->mbr_0x1f0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar4 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  if (puVar4 != (undefined4 *)0x0) {
    for (uVar2 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  this->mbr_0x1ec = 0;
  ExceptionList = local_c;
  return this;
}



