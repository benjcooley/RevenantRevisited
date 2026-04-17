// Decompiled methods and structure for class: cls_0x5b5354

/*
/OOAnalyzer/cls_0x5b5354
pack(disabled)
Structure cls_0x5b5354 {
   0   cls_0x5b5354::vftable_5b5354 *   4   vftptr_0x0   "pointer to cls_0x5b5354::vftable_5b5354"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 00520090

cls_0x5b5354 * __thiscall OOAnalyzer::cls_0x5b5354::~cls_0x5b5354(cls_0x5b5354 *this)

{
  dword *this_00;
  int iVar1;
  cls_0x5b5354 *pcVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a125b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b5354__vftable_5b5354_005b5354;
  this_00 = &this->mbr_0x24;
  iVar3 = 0;
  local_4 = 0;
  if (0 < (int)this->mbr_0x24) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)(this->mbr_0x34 + iVar3 * 4), iVar1 != 0)) {
        FUN_005295b0();
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x28 = 0;
  local_4 = 0xffffffff;
  pcVar2 = (cls_0x5b5354 *)FUN_004830f0(this->mbr_0x34);
  ExceptionList = local_c;
  return pcVar2;
}



