#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4494



// Function at 00434e40

undefined4 cls_0x5a4494::virt_meth_0x434e40()

{
  undefined *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  
  virt_meth_0x491900(this);
  if (*(undefined4 **)&this->field_0x98 != (undefined4 *)0x0) {
    puVar4 = *(undefined4 **)&this->field_0x98;
    for (uVar2 = *(uint *)&this->field_0x90 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar1 = PTR_DAT_005d79e0;
  *(undefined4 *)&this->field_0x8c = 0;
  *(undefined4 *)&this->field_0x88 = 0;
  *(undefined4 *)&this->field_0x9c = 0;
  this->mbr_0xa4 = 0;
  this->mbr_0xa0 = 0;
  this->mbr_0x60 = 0;
  this->mbr_0xc8 = 0;
  this->mbr_0xcc = 0;
  this->mbr_0xd0 = 0;
  this->mbr_0x178 = 0;
  this->mbr_0x158 = 0;
  this->mbr_0x150 = 0;
  this->mbr_0x154 = 0;
  this->mbr_0x64 = (dword)puVar1;
  this->mbr_0x68 = 0;
  this->mbr_0x6c = 0;
  this->mbr_0x70 = 0;
  this->mbr_0x74 = 0;
  this->mbr_0x78 = 0;
  this->mbr_0x84 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0xa8 = 0;
  this->mbr_0xac = 0;
  this->mbr_0xbc = 0;
  this->mbr_0xb8 = 0;
  this->mbr_0xb0 = 0xffffffff;
  this->mbr_0xb4 = 0xffffffff;
  this->mbr_0xc0 = 5;
  this->mbr_0xc4 = 0xffffffff;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return 1;
}



// Function at 00445390

cls_0x5a4494 * cls_0x5a4494::virt_meth_0x445390()

{
  byte in_stack_00000004;
  
  cls_0x4453b0::~cls_0x4453b0((cls_0x4453b0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00488620

cls_0x5a4494::cls_0x5a4494()

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  tagMSG tStack_90;
  byte local_74 [104];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d6ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x4 = 0x1c4;
  this->mbr_0x14 = 0x1c4;
  this->mbr_0xc = 0xbc;
  this->mbr_0x1c = 0xbc;
  this->vftptr_0x0 = &cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x132;
  this->mbr_0x20 = 0x132;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&PTR_virt_meth_0x434e40_005a45c8;
  iVar2 = DAT_00667fbc;
  DAT_0065ba08 = DAT_0065ba08 + 0x100;
  local_4 = 0;
  this->mbr_0x1a4 = 0;
  this->mbr_0x1a6 = 0x40;
  this->mbr_0x1a8 = 0;
  if (iVar2 < DAT_0065ba08) {
    DAT_00667fbc = DAT_0065ba08;
  }
  puVar1 = (undefined4 *)_malloc(0x100);
  puVar4 = puVar1;
  for (iVar2 = 0x40; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  if (puVar1 == (undefined4 *)0x0) {
    if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    DAT_0065b9f8 = 1;
    FUN_0058b100((char *)local_74,(byte *)s_OUT_OF_MEMORY___Increase_your_vi_005d7cd8);
    FUN_004820b0(local_74);
    FUN_004820b0((byte *)s_Press_any_key_to_exit_005d7bdc);
    DAT_00667fd0 = 0;
    cls_0x49e370::meth_0x49e410((cls_0x49e370 *)&UNK_0065c55c.field_0x60);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
    FUN_004a7460();
    cls_0x4489a0::meth_0x448c90((cls_0x4489a0 *)&DAT_0065b8e8);
    iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    while (iVar2 != 0) {
      TranslateMessage(&tStack_90);
      DispatchMessageA(&tStack_90);
      iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    }
    ShowWindow(DAT_0065b8ec,0);
    MessageBoxA((HWND)0x0,(LPCSTR)local_74,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  this->mbr_0x1a8 = (dword)puVar1;
  for (uVar3 = (int)(short)this->mbr_0x1a6 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  this->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&PTR_virt_meth_0x5449e0_005a5ae8;
  ExceptionList = local_c;
  return this;
}



// Function at 00488820

cls_0x5a4494 * cls_0x5a4494::virt_meth_0x488820()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a4494(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a4494 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00488860

cls_0x5a4494 * __thiscall cls_0x5a4494::~cls_0x5a4494(cls_0x5a4494 *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a4494 *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d6d6;
  local_c = ExceptionList;
  pvVar1 = (LPCVOID)this->mbr_0x1a8;
  ExceptionList = &local_c;
  if (pvVar1 != (LPCVOID)0x0) {
    ExceptionList = &local_c;
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
    FUN_0058cfab(pvVar1);
  }
  this->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a4494 *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00491900

undefined4 cls_0x5a4494::virt_meth_0x491900()

{
  if (this->mbr_0x40 == 0) {
    this->mbr_0x4 = this->mbr_0x14;
    this->mbr_0xc = this->mbr_0x1c;
    this->mbr_0x8 = this->mbr_0x18;
    this->mbr_0x38 = 0;
    this->mbr_0x30 = 0;
    this->mbr_0x28 = 0;
    this->mbr_0x34 = 0;
    this->mbr_0x2c = 0;
    this->mbr_0x24 = 0;
    this->mbr_0x48 = 0;
    this->mbr_0x4c = 0;
    this->mbr_0x54 = 0;
    this->mbr_0x58 = 0xffffffff;
    this->mbr_0x5c = 0xffffffff;
    this->mbr_0x10 = this->mbr_0x20;
    this->mbr_0x50 = 1;
    (*this->vftptr_0x0->virt_meth_0x444f40_12)(this);
    this->mbr_0x40 = 1;
  }
  return 1;
}



// Function at 005449e0

void cls_0x5a4494::virt_meth_0x5449e0()

{
  cls_0x46d6b0 *pcVar1;
  cls_0x5a68d8 *pcVar2;
  undefined4 uVar3;
  dword dVar4;
  int iVar5;
  undefined4 uVar6;
  cls_0x5a3c68 *pcVar7;
  int iVar8;
  void *unaff_ESI;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1f74;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  virt_meth_0x434e40(this);
  pcVar2 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  iVar8 = 0;
  local_4 = 0;
  if (pcVar2 == (cls_0x5a68d8 *)0x0) {
    pcVar2 = (cls_0x5a68d8 *)0x0;
  }
  else {
    uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
    cls_0x5a68d8::cls_0x5a68d8(pcVar2);
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar2->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar2->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5ca0(pcVar2,uVar3);
    pcVar2->mbr_0x70 = 1;
  }
  local_4 = 0xffffffff;
  this->mbr_0x184 = (dword)pcVar2;
  pcVar2 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 2;
  if (pcVar2 == (cls_0x5a68d8 *)0x0) {
    pcVar2 = (cls_0x5a68d8 *)0x0;
  }
  else {
    uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
    cls_0x5a68d8::cls_0x5a68d8(pcVar2);
    local_4 = CONCAT31(local_4._1_3_,3);
    pcVar2->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar2->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5ca0(pcVar2,uVar3);
    pcVar2->mbr_0x70 = 1;
  }
  local_4 = 0xffffffff;
  this->mbr_0x188 = (dword)pcVar2;
  pcVar2 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 4;
  if (pcVar2 == (cls_0x5a68d8 *)0x0) {
    pcVar2 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x5a68d8::cls_0x5a68d8(pcVar2);
    local_4 = CONCAT31(local_4._1_3_,5);
    pcVar2->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar2->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(pcVar2,0x94,0xe5);
    pcVar2->mbr_0x70 = 1;
  }
  local_4 = 0xffffffff;
  this->mbr_0x180 = (dword)pcVar2;
  pcVar2 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 6;
  if (pcVar2 == (cls_0x5a68d8 *)0x0) {
    pcVar2 = (cls_0x5a68d8 *)0x0;
  }
  else {
    uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
    cls_0x5a68d8::cls_0x5a68d8(pcVar2);
    local_4 = CONCAT31(local_4._1_3_,7);
    pcVar2->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar2->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5ca0(pcVar2,uVar3);
    pcVar2->mbr_0x70 = 1;
  }
  local_4 = 0xffffffff;
  this->mbr_0x18c = (dword)pcVar2;
  pcVar2 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 8;
  if (pcVar2 == (cls_0x5a68d8 *)0x0) {
    pcVar2 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x5a68d8::cls_0x5a68d8(pcVar2);
    local_4 = CONCAT31(local_4._1_3_,9);
    pcVar2->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar2->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(pcVar2,0x84,200);
    pcVar2->mbr_0x70 = 1;
  }
  pcVar1 = DAT_00666640;
  dVar4 = this->mbr_0x180;
  local_4 = 0xffffffff;
  *(cls_0x5a68d8 **)(dVar4 + 0x5c) = pcVar2;
  *(undefined4 *)(dVar4 + 0x60) = 1;
  dVar4 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  this->mbr_0x190 = dVar4;
  cls_0x5b98b8::meth_0x435660((cls_0x5b98b8 *)this);
  (**(code **)(*(int *)this->mbr_0x180 + 0x48))(1);
  iVar5 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  iVar5 = (-(uint)(iVar5 != 0x10) & 0xffff8400) + 0xf81f;
  (**(code **)(*(int *)this->mbr_0x180 + 0x1c))(iVar5);
  (**(code **)(*(int *)this->mbr_0x184 + 0x1c))(iVar5);
  (**(code **)(*(int *)this->mbr_0x188 + 0x1c))(iVar5);
  (**(code **)(*(int *)this->mbr_0x18c + 0x1c))(iVar5);
  pcVar1 = DAT_00666640;
  this->mbr_0x17c = 0;
  this->mbr_0x1cc = 0xffffffff;
  this->mbr_0x1c8 = 0xffffffff;
  this->mbr_0x1d0 = 0;
  this->mbr_0x1a0 = 0;
  this->mbr_0x19c = 1;
  this->mbr_0x198 = 1;
  uVar3 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  uVar6 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
  cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,&DAT_005e52ac,0xa9,0x96,0x18,0x18,0,&LAB_005449c0,uVar6,uVar3,0,0,1,0xffffffff
               ,0,0);
  }
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  if (*(int *)&this->field_0x88 == 0) {
    iVar5 = 0;
  }
  else {
    iVar5 = **(int **)&this->field_0x98;
  }
  *(uint *)(iVar5 + 0xd4) = *(uint *)(iVar5 + 0xd4) | 0x2000;
  pcVar1 = DAT_00666640;
  uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
  uVar6 = cls_0x46d6b0::meth_0x46d710(DAT_00666640);
  cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar7 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcVar7 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar7,&DAT_005e52d4,0xa9,0xae,0x18,0x18,0,&LAB_005449d0,uVar6,uVar3,0,0,1,0xffffffff
               ,0,0);
  }
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  if (1 < *(uint *)&this->field_0x88) {
    iVar8 = *(int *)(*(int *)&this->field_0x98 + 4);
  }
  *(uint *)(iVar8 + 0xd4) = *(uint *)(iVar8 + 0xd4) | 0x2000;
  ExceptionList = unaff_ESI;
  return;
}



// Function at 00544e40

void cls_0x5a4494::virt_meth_0x544e40()

{
  this->virt_meth_0x434f30();
  if ((undefined4 *)this->mbr_0x18c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x18c)(1);
    this->mbr_0x18c = 0;
  }
  if ((undefined4 *)this->mbr_0x180 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x180)(1);
    this->mbr_0x180 = 0;
  }
  if ((undefined4 *)this->mbr_0x184 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x184)(1);
    this->mbr_0x184 = 0;
  }
  if ((undefined4 *)this->mbr_0x188 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x188)(1);
    this->mbr_0x188 = 0;
  }
  return;
}



// Function at 00544eb0

void cls_0x5a4494::virt_meth_0x544eb0()

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x4451e0_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  this->virt_meth_0x435de0();
  return;
}



// Function at 00546390

void cls_0x5a4494::virt_meth_0x546390(int param_1, dword param_2)

{
  char cVar1;
  dword dVar2;
  int *piVar3;
  char *pcVar4;
  undefined4 *puVar5;
  int iVar6;
  char *pcVar7;
  dword in_stack_0000000c;
  
  this->mbr_0x1ac = param_2;
  this->mbr_0x1b0 = in_stack_0000000c;
  this->virt_meth_0x436660(param_1,param_2);
  if ((((((int)param_2 < 0) || ((int)in_stack_0000000c < 0)) || ((int)this->mbr_0xc <= (int)param_2)
       ) || (((int)this->mbr_0x10 <= (int)in_stack_0000000c || ((int)param_2 < 0x13)))) ||
     ((0xa5 < (int)param_2 || (((int)in_stack_0000000c < 0x13 || (0x113 < (int)in_stack_0000000c))))
     )) {
    if (param_1 != 1) goto LAB_0054651e;
  }
  else {
    if (param_1 != 1) goto LAB_0054651e;
    if (this->mbr_0x1c8 == 0xffffffff) {
      dVar2 = this->mbr_0x194;
      this->mbr_0x194 = in_stack_0000000c;
      this->mbr_0x1d0 = this->mbr_0x1d0 + (dVar2 - in_stack_0000000c);
      return;
    }
  }
  if (-1 < (int)this->mbr_0x1c8) {
    pcVar4 = FUN_0043a410();
    iVar6 = -1;
    do {
      if (iVar6 == 0) break;
      iVar6 = iVar6 + -1;
      cVar1 = *pcVar4;
      pcVar4 = pcVar4 + 1;
    } while (cVar1 != '\0');
    if ((iVar6 == -2) &&
       (pcVar4 = *(char **)(*(int *)(DAT_00667fcc + 0x2fc) + this->mbr_0x1c8 * 4),
       pcVar4 != (char *)0x0)) {
      iVar6 = -1;
      pcVar7 = pcVar4;
      do {
        if (iVar6 == 0) break;
        iVar6 = iVar6 + -1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar7 + 1;
      } while (cVar1 != '\0');
      if (iVar6 != -2) {
        iVar6 = cls_0x53ed70::meth_0x53ed70((cls_0x53ed70 *)((int)&UNK_00667c34 + 4));
        if (iVar6 != 0) {
          FUN_0043a140((int *)0x0);
          puVar5 = (undefined4 *)cls_0x46d6b0::meth_0x46d6b0(DAT_0065bc3c);
          if (puVar5 == (undefined4 *)0x0) {
            puVar5 = (undefined4 *)cls_0x46d6b0::meth_0x46d6b0(DAT_0065bc3c);
          }
          FUN_0043a240(puVar5,0x14,0x14);
          FUN_0043a170(pcVar4);
          piVar3 = (int *)this->mbr_0x180;
          this->mbr_0x1c8 = 0xffffffff;
          (**(code **)(*piVar3 + 0x44))(piVar3[5],piVar3[6],0x94,0xe5);
          UNK_0065a9dc._76_4_ = 1;
          (**(code **)(DAT_0065a9d8 + 0x90))();
        }
      }
    }
  }
LAB_0054651e:
  this->mbr_0x194 = in_stack_0000000c;
  return;
}



// Function at 00546620

void cls_0x5a4494::virt_meth_0x546620()

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  dword dVar7;
  int iVar8;
  dword dVar9;
  int aiStack_38 [2];
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int iStack_20;
  int iStack_1c;
  int aiStack_18 [6];
  
  if ((this->mbr_0x198 != 0) && (this->mbr_0x19c != 0)) {
    dVar9 = this->mbr_0x1d0;
    dVar2 = dVar9;
    if ((int)dVar9 < 0) {
      dVar2 = -dVar9;
    }
    if ((int)dVar2 < 0x28) {
      this->mbr_0x1d0 = 0;
      if (dVar9 == 0) goto LAB_005467c9;
    }
    else if ((int)dVar9 < 1) {
      this->mbr_0x1d0 = dVar9 + 0x28;
      dVar9 = 0xffffffd8;
    }
    else {
      this->mbr_0x1d0 = dVar9 - 0x28;
      dVar9 = 0x28;
    }
    piVar1 = (int *)this->mbr_0x180;
    local_30 = piVar1[5];
    local_2c = piVar1[6];
    local_28 = local_30 + 0x93;
    local_24 = local_2c + 0xe4;
    dVar2 = this->mbr_0x17c;
    dVar9 = dVar2 + dVar9;
    dVar7 = this->mbr_0x1a0 - 0xe5;
    this->mbr_0x17c = dVar9;
    if ((int)dVar7 < (int)dVar9) {
      this->mbr_0x17c = dVar7;
    }
    if ((int)this->mbr_0x17c < 0) {
      this->mbr_0x17c = 0;
    }
    if (this->mbr_0x17c != dVar2) {
      (**(code **)(*piVar1 + 0x40))(0,this->mbr_0x17c);
      iVar8 = *(int *)(this->mbr_0x180 + 0x14);
      iVar5 = *(int *)(this->mbr_0x180 + 0x18);
      iVar3 = 0xe4;
      iVar6 = iVar8 + 0x93;
      iVar4 = FUN_004ad610((int *)&stack0xffffffb8,aiStack_38,aiStack_18);
      if (iVar4 == 0) {
        iVar4 = *(int *)this->mbr_0x180;
      }
      else {
        FUN_004ad6a0((int *)&stack0xffffffb8,aiStack_18,&local_28,(int *)&stack0xffffffb4);
        iVar3 = iStack_1c - local_24;
        iVar4 = *(int *)this->mbr_0x180;
        iVar5 = local_24;
        iVar8 = local_28;
        iVar6 = iStack_20;
      }
      (**(code **)(iVar4 + 0x44))(iVar8,iVar5,(iVar6 - iVar8) + 1,iVar3 + 1);
      UNK_0065a9dc._76_4_ = 1;
      (**(code **)(DAT_0065a9d8 + 0x90))();
      this->mbr_0x198 = 0;
    }
  }
LAB_005467c9:
  this->virt_meth_0x435d70();
  return;
}



