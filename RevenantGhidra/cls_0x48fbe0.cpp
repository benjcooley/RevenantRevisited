// Decompiled methods and structure for class: cls_0x48fbe0

/*
/OOAnalyzer/cls_0x48fbe0
pack(disabled)
Structure cls_0x48fbe0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 72 Alignment: 1

*/

// Function at 0048fbe0

void __thiscall OOAnalyzer::cls_0x48fbe0::meth_0x48fbe0(cls_0x48fbe0 *this,undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  undefined4 in_stack_00000008;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5fc0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar1 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar1 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar1 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar1 != 0)) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar2 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar2 & 8) == 0) {
      local_8 = 0;
      (**(code **)(this->mbr_0x0 + 0x34))(param_1,in_stack_00000008);
    }
    ExceptionList = local_14;
    return;
  }
  return;
}



