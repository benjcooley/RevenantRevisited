// Decompiled methods and structure for class: cls_0x4881f0

/*
/OOAnalyzer/cls_0x4881f0
pack(disabled)
Structure cls_0x4881f0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 92 Alignment: 1

*/

// Function at 004881f0

void __thiscall OOAnalyzer::cls_0x4881f0::meth_0x4881f0(cls_0x4881f0 *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d647;
  local_c = ExceptionList;
  local_4 = 4;
  ExceptionList = &local_c;
  meth_0x48b860(this);
  local_4 = CONCAT31(local_4._1_3_,3);
  cls_0x49c970::meth_0x49c970((cls_0x49c970 *)&this->mbr_0x58);
  pvVar1 = (LPCVOID)this->mbr_0x50;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  pvVar1 = (LPCVOID)this->mbr_0x3c;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  pvVar1 = (LPCVOID)this->mbr_0x28;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  pvVar1 = (LPCVOID)this->mbr_0x14;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return;
}



// Function at 0048b860

void __thiscall OOAnalyzer::cls_0x4881f0::meth_0x48b860(cls_0x4881f0 *this)

{
  dword *pdVar1;
  dword *this_00;
  LPCVOID pvVar2;
  int iVar3;
  
  pdVar1 = &this->mbr_0x18;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x18) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x28 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004894f0((int)pvVar2);
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  this->mbr_0x1c = 0;
  pdVar1 = &this->mbr_0x4;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x14 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*pdVar1);
  }
  this_00 = &this->mbr_0x2c;
  *pdVar1 = 0;
  iVar3 = 0;
  this->mbr_0x8 = 0;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x3c + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        if (*(LPCVOID *)((int)pvVar2 + 0xcc) != (LPCVOID)0x0) {
          FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0xcc));
        }
        *(undefined4 *)((int)pvVar2 + 0xcc) = 0;
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x30 = 0;
  pdVar1 = &this->mbr_0x40;
  iVar3 = 0;
  if (0 < (int)this->mbr_0x40) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x50 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        if (*(LPCVOID *)((int)pvVar2 + 200) != (LPCVOID)0x0) {
          FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 200));
        }
        *(undefined4 *)((int)pvVar2 + 200) = 0;
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  this->mbr_0x44 = 0;
  return;
}



