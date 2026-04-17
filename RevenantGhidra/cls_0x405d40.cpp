// Decompiled methods and structure for class: cls_0x405d40

/*
/OOAnalyzer/cls_0x405d40
pack(disabled)
Structure cls_0x405d40 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 64 Alignment: 1

*/

// Function at 00405d40

cls_0x405d40 * __thiscall OOAnalyzer::cls_0x405d40::~cls_0x405d40(cls_0x405d40 *this)

{
  int iVar1;
  LPCVOID pvVar2;
  cls_0x405d40 *pcVar3;
  int iVar4;
  int iVar5;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059c46e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x40d3e0_005a34ec;
  iVar4 = 0;
  local_4 = 1;
  if (0 < (int)this->mbr_0x2c) {
    do {
      iVar5 = 0;
      iVar1 = *(int *)(this->mbr_0x3c + iVar4 * 4);
      if (0 < *(int *)(iVar1 + 0x10)) {
        do {
          FUN_004830f0(*(LPCVOID *)(*(int *)(iVar1 + 0x20) + iVar5 * 4));
          iVar5 = iVar5 + 1;
        } while (iVar5 < *(int *)(iVar1 + 0x10));
      }
      pvVar2 = *(LPCVOID *)(this->mbr_0x3c + iVar4 * 4);
      if (pvVar2 != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0x20));
        FUN_004830f0(pvVar2);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x2c);
  }
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  local_4 = local_4 & 0xffffff00;
  FUN_004830f0((LPCVOID)this->mbr_0x3c);
  this->mbr_0x0 = (dword)&cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 2;
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  local_4 = 0xffffffff;
  pcVar3 = (cls_0x405d40 *)FUN_004830f0((LPCVOID)this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar3;
}



