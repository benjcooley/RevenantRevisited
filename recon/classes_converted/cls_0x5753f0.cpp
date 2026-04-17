#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5753f0



// Function at 005753f0

cls_0x5753f0::cls_0x5753f0()

{
  char cVar1;
  dword dVar2;
  undefined4 *puVar3;
  HANDLE pvVar4;
  DWORD DVar5;
  cls_0x5b9f54 *pcVar6;
  undefined4 *puVar7;
  uint uVar8;
  uint uVar9;
  dword *pdVar10;
  char *pcVar11;
  char *pcVar12;
  dword *pdVar13;
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
  puVar3 = FUN_00482fb0(0x20);
  this->mbr_0x8 = (dword)puVar3;
  this->mbr_0xc = (dword)puVar3;
  this->mbr_0x10 = (dword)(puVar3 + this->mbr_0x14);
  this->mbr_0x158 = (dword)&this->mbr_0x150;
  this->mbr_0x150 = (dword)&this->mbr_0x154;
  this->mbr_0x154 = 0;
  local_4._0_1_ = 1;
  local_4._1_3_ = 0;
  cls_0x588410::cls_0x588410(&this->cls_0x588410);
  local_4._0_1_ = 2;
  *(undefined4 *)&(this->cls_0x588410).field_0x2c = 8;
  puVar3 = FUN_00482fb0(0x20);
  (this->cls_0x588410).mbr_0x20 = (dword)puVar3;
  (this->cls_0x588410).mbr_0x24 = (dword)puVar3;
  (this->cls_0x588410).mbr_0x28 = (dword)(puVar3 + *(int *)&(this->cls_0x588410).field_0x2c);
  local_4._0_1_ = 3;
  this->mbr_0x198 = 8;
  puVar3 = FUN_00482fb0(0x20);
  dVar2 = this->mbr_0x198;
  *(undefined4 **)&(this->cls_0x588410).field_0x30 = puVar3;
  (this->cls_0x588410).mbr_0x34 = (dword)puVar3;
  this->mbr_0x194 = (dword)(puVar3 + dVar2);
  local_4._0_1_ = 4;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba088__vftable_5ba088_005ba088;
  pdVar10 = &(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
  local_4._0_1_ = 5;
  cls_0x5ba0a0::cls_0x5ba0a0((cls_0x5ba0a0 *)pdVar10);
  *pdVar10 = (dword)&cls_0x5ba074__vftable_5ba074_005ba074;
  local_4._0_1_ = 6;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba060__vftable_5ba060_005ba060;
  pdVar10 = &(this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
  local_4._0_1_ = 7;
  cls_0x5ba0a0::cls_0x5ba0a0((cls_0x5ba0a0 *)pdVar10);
  *pdVar10 = (dword)&cls_0x5ba04c__vftable_5ba04c_005ba04c;
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
  uVar8 = 0xffffffff;
  this->mbr_0x18 = 0xffffffff;
  this->mbr_0x28 = 0xffffffff;
  this->mbr_0x13c = 0xffffffff;
  pcVar11 = s_Unknown_005e5bac;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
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
  pdVar10 = (dword *)(pcVar12 + -uVar8);
  pdVar13 = &this->mbr_0x104;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pdVar13 = *pdVar10;
    pdVar10 = pdVar10 + 1;
    pdVar13 = pdVar13 + 1;
  }
  local_4._0_1_ = 9;
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)pdVar13 = *(undefined *)pdVar10;
    pdVar10 = (dword *)((int)pdVar10 + 1);
    pdVar13 = (dword *)((int)pdVar13 + 1);
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
  puVar3 = FUN_00482fb0(0x10);
  local_4 = CONCAT31(local_4._1_3_,0xb);
  if (puVar3 == (undefined4 *)0x0) {
    puVar3 = (undefined4 *)0x0;
  }
  else {
    puVar3[3] = 8;
    pcStack_30 = (cls_0x5b9f54 *)puVar3;
    puVar7 = FUN_00482fb0(0x620);
    *puVar3 = puVar7;
    puVar3[1] = puVar7;
    puVar3[2] = puVar7 + puVar3[3] * 0x31;
  }
  this->mbr_0x20 = (dword)puVar3;
  this->mbr_0x24 = 0;
  this->mbr_0x1a0 = 0;
  this->mbr_0x19c = 0;
  ExceptionList = pvStack_c;
  return this;
}



