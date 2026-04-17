// Decompiled methods and structure for class: cls_0x4a1050

/*
/OOAnalyzer/cls_0x4a1050
pack(disabled)
Structure cls_0x4a1050 {
   0   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
}
Length: 24 Alignment: 1

*/

// Function at 004a1050

cls_0x4a1050 * __thiscall OOAnalyzer::cls_0x4a1050::cls_0x4a1050(cls_0x4a1050 *this)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ddca;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  puVar3 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  local_4 = 0;
  if (puVar3 != (undefined4 *)0x0) {
    for (uVar1 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  puVar3 = &DAT_00668814;
  do {
    puVar3[-1] = 0xffffffff;
    *puVar3 = 0xffffffff;
    puVar3[2] = 0;
    cls_0x56fe20::meth_0x56fe20((cls_0x56fe20 *)(puVar3 + 4));
    puVar3 = puVar3 + 6;
  } while ((int)puVar3 < 0x668b14);
  ExceptionList = local_c;
  return this;
}



