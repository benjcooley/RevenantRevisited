// Decompiled methods and structure for class: cls_0x5753f0

/*
/OOAnalyzer/cls_0x5753f0
pack(disabled)
Structure cls_0x5753f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   byte   1   mbr_0xfc   "Unsigned Byte (db)"
   253   byte   1   mbr_0xfd   "Unsigned Byte (db)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   byte   1   mbr_0x138   "Unsigned Byte (db)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   340   dword   4   mbr_0x154   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   cls_0x588410   56   cls_0x588410   "Component (member) class."
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   byte   1   mbr_0x19c   "Unsigned Byte (db)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   424   cls_0x5ba0a0   60   cls_0x5ba0a0   "Component (member) class."
   504   cls_0x5ba0a0   60   cls_0x5ba0a0   "Component (member) class."
   584   cls_0x5ba0a0   60   cls_0x5ba0a0   "Component (member) class."
}
Length: 644 Alignment: 1

*/

// Function at 005753f0

cls_0x5753f0 * __thiscall OOAnalyzer::cls_0x5753f0::cls_0x5753f0(cls_0x5753f0 *this)

{
  char cVar1;
  dword dVar2;
  dword dVar3;
  HANDLE pvVar4;
  DWORD DVar5;
  cls_0x5b9f54 *pcVar6;
  int *piVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  dword *pdVar11;
  char *pcVar12;
  char *pcVar13;
  dword *pdVar14;
  DWORD DStack_38;
  cls_0x5753f0 *local_34;
  cls_0x5b9f54 *pcStack_30;
  CHAR aCStack_2c [32];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2565;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->mbr_0x14 = 8;
  local_34 = this;
  dVar2 = FUN_00482fb0(0x20);
  this->mbr_0x8 = dVar2;
  this->mbr_0xc = dVar2;
  this->mbr_0x10 = dVar2 + this->mbr_0x14 * 4;
  this->mbr_0x158 = (dword)&this->mbr_0x150;
  this->mbr_0x150 = (dword)&this->mbr_0x154;
  this->mbr_0x154 = 0;
  local_4._0_1_ = 1;
  local_4._1_3_ = 0;
  cls_0x588410::cls_0x588410(&this->cls_0x588410);
  local_4._0_1_ = 2;
  *(undefined4 *)&(this->cls_0x588410).field_0x2c = 8;
  dVar2 = FUN_00482fb0(0x20);
  (this->cls_0x588410).mbr_0x20 = dVar2;
  (this->cls_0x588410).mbr_0x24 = dVar2;
  (this->cls_0x588410).mbr_0x28 = dVar2 + *(int *)&(this->cls_0x588410).field_0x2c * 4;
  local_4._0_1_ = 3;
  this->mbr_0x198 = 8;
  dVar3 = FUN_00482fb0(0x20);
  dVar2 = this->mbr_0x198;
  *(dword *)&(this->cls_0x588410).field_0x30 = dVar3;
  (this->cls_0x588410).mbr_0x34 = dVar3;
  this->mbr_0x194 = dVar3 + dVar2 * 4;
  local_4._0_1_ = 4;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba088__vftable_5ba088_005ba088;
  pdVar11 = &(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
  local_4._0_1_ = 5;
  cls_0x5ba0a0::cls_0x5ba0a0((cls_0x5ba0a0 *)pdVar11);
  *pdVar11 = (dword)&cls_0x5ba074__vftable_5ba074_005ba074;
  local_4._0_1_ = 6;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba060__vftable_5ba060_005ba060;
  pdVar11 = &(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
  local_4._0_1_ = 7;
  cls_0x5ba0a0::cls_0x5ba0a0((cls_0x5ba0a0 *)pdVar11);
  *pdVar11 = (dword)&cls_0x5ba04c__vftable_5ba04c_005ba04c;
  local_4._0_1_ = 8;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba038__vftable_5ba038_005ba038;
  pvVar4 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCSTR)0x0);
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24 = (dword)pvVar4;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = 0;
  DVar5 = GetTickCount();
  *(short *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = (short)DVar5;
  *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x34 = 1;
  uVar9 = 0xffffffff;
  this->mbr_0x18 = 0xffffffff;
  this->mbr_0x28 = 0xffffffff;
  this->mbr_0x13c = 0xffffffff;
  pcVar12 = s_Unknown_005e5bac;
  do {
    pcVar13 = pcVar12;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar13 = pcVar12 + 1;
    cVar1 = *pcVar12;
    pcVar12 = pcVar13;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0xf0 = 0;
  this->mbr_0xf4 = 0;
  this->mbr_0xf8 = 0;
  this->mbr_0xfc = 0;
  this->mbr_0xfd = 0;
  this->mbr_0x100 = 3;
  this->mbr_0x14c = 0;
  this->mbr_0x12c = 0;
  this->mbr_0x124 = 0;
  this->mbr_0x128 = 0;
  this->mbr_0x130 = 0;
  this->mbr_0x134 = 0;
  this->mbr_0x148 = 0;
  this->mbr_0x138 = 0;
  pdVar11 = (dword *)(pcVar13 + -uVar9);
  pdVar14 = &this->mbr_0x104;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *pdVar14 = *pdVar11;
    pdVar11 = pdVar11 + 1;
    pdVar14 = pdVar14 + 1;
  }
  local_4._0_1_ = 9;
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined *)pdVar14 = *(undefined *)pdVar11;
    pdVar11 = (dword *)((int)pdVar11 + 1);
    pdVar14 = (dword *)((int)pdVar14 + 1);
  }
  DStack_38 = 0x20;
  GetUserNameA(aCStack_2c,&DStack_38);
  GetPrivateProfileStringA
            (s_Network_005e5ab4,s_Handle_005e5bb4,aCStack_2c,(LPSTR)&this->mbr_0x104,0x20,
             &DAT_006663e0);
  pcStack_30 = (cls_0x5b9f54 *)FUN_00482fb0(0x20);
  local_4._0_1_ = 10;
  if (pcStack_30 == (cls_0x5b9f54 *)0x0) {
    pcVar6 = (cls_0x5b9f54 *)0x0;
  }
  else {
    pcVar6 = cls_0x5b9f54::cls_0x5b9f54(pcStack_30);
  }
  local_4._0_1_ = 9;
  (this->cls_0x588410).mbr_0x1c = (dword)pcVar6;
  piVar7 = (int *)FUN_00482fb0(0x10);
  local_4 = CONCAT31(local_4._1_3_,0xb);
  if (piVar7 == (int *)0x0) {
    piVar7 = (int *)0x0;
  }
  else {
    piVar7[3] = 8;
    pcStack_30 = (cls_0x5b9f54 *)piVar7;
    iVar8 = FUN_00482fb0(0x620);
    *piVar7 = iVar8;
    piVar7[1] = iVar8;
    piVar7[2] = iVar8 + piVar7[3] * 0xc4;
  }
  this->mbr_0x20 = (dword)piVar7;
  this->mbr_0x24 = 0;
  this->mbr_0x1a0 = 0;
  this->mbr_0x19c = 0;
  ExceptionList = pvStack_c;
  return this;
}



