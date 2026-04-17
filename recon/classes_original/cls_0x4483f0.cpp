// Decompiled methods and structure for class: cls_0x4483f0

/*
/OOAnalyzer/cls_0x4483f0
pack(disabled)
Structure cls_0x4483f0 {
   0   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
}
Length: 24 Alignment: 1

*/

// Function at 004483f0

cls_0x4483f0 * __thiscall OOAnalyzer::cls_0x4483f0::cls_0x4483f0(cls_0x4483f0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cee8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x40);
  local_4 = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(0x7c);
  (this->cls_0x41c7f0).mbr_0x14 = (dword)puVar1;
  for (iVar2 = 0x1f; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  ExceptionList = local_c;
  return this;
}



