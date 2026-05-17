#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5750_TSideTabsPane



// Function at 00487c60

cls_0x5a5750_TSideTabsPane::cls_0x5a5750_TSideTabsPane()

{
  this->vftptr_0x0 = (cls_0x5a5750__vftable_5a5750 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x10 = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5750__vftable_5a5750_005a5750;
  return this;
}



// Function at 00487cb0

cls_0x5a5750_TSideTabsPane * cls_0x5a5750_TSideTabsPane::virt_meth_0x487cb0()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5750_TSideTabsPane(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5750_TSideTabsPane *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00487cf0

cls_0x5a5750_TSideTabsPane * __thiscall cls_0x5a5750_TSideTabsPane::~cls_0x5a5750_TSideTabsPane(cls_0x5a5750_TSideTabsPane *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5750_TSideTabsPane *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d57e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a5750__vftable_5a5750 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5750_TSideTabsPane *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 0053cc30

undefined4 cls_0x5a5750_TSideTabsPane::virt_meth_0x53cc30_TSideTabsPane_Initialize()

{
  undefined4 uVar1;
  undefined4 uVar2;
  cls_0x429950 cVar3;
  cls_0x46d6b0 *pcVar4;
  cls_0x429950 *pcVar5;
  dword dVar6;
  cls_0x5a68d8 *pcVar7;
  int iVar8;
  cls_0x5a3c68 *pcVar9;
  dword *pdVar10;
  bool bVar11;
  cls_0x429950 local_14;
  cls_0x5a68d8 *local_10;
  cls_0x5a3c68 *pcStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1d44;
  pcStack_c = (cls_0x5a3c68 *)ExceptionList;
  ExceptionList = &pcStack_c;
  this->virt_meth_0x434e40();
  pcVar5 = cls_0x429950::cls_0x429950(&local_14,0xff,0);
  FUN_004384a0((uint)*pcVar5);
  dVar6 = cls_0x46d6b0::meth_0x46d710(DAT_0065c5c8);
  pcVar4 = DAT_0065c5c8;
  this->mbr_0x17c = dVar6;
  dVar6 = cls_0x46d6b0::meth_0x46d710(pcVar4);
  pcVar4 = DAT_0065c5c8;
  this->mbr_0x180 = dVar6;
  dVar6 = cls_0x46d6b0::meth_0x46d710(pcVar4);
  this->mbr_0x184 = dVar6;
  if (DAT_006680c8 == 0) {
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 6;
    local_10 = pcVar7;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)this->mbr_0x17c)[1];
      uVar2 = *(undefined4 *)this->mbr_0x17c;
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,7);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,uVar2,uVar1);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x188 = (dword)pcVar7;
    pcVar9 = (cls_0x5a3c68 *)FUN_00482fb0(0x74);
    pcStack_c = pcVar9;
    if (pcVar9 == (cls_0x5a3c68 *)0x0) {
      pcVar9 = (cls_0x5a3c68 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)this->mbr_0x17c)[1];
      uVar2 = *(undefined4 *)this->mbr_0x17c;
      cls_0x5a68d8::cls_0x5a68d8((cls_0x5a68d8 *)pcVar9);
      (pcVar9->cls_0x5a3ab8).vftptr_0x0 =
           (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0((cls_0x5a68d8 *)pcVar9,uVar2,uVar1);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x18c = (dword)pcVar9;
    pcVar9 = (cls_0x5a3c68 *)FUN_00482fb0(0x74);
    pcStack_c = pcVar9;
    if (pcVar9 == (cls_0x5a3c68 *)0x0) {
      pcVar9 = (cls_0x5a3c68 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)this->mbr_0x17c)[1];
      uVar2 = *(undefined4 *)this->mbr_0x17c;
      cls_0x5a68d8::cls_0x5a68d8((cls_0x5a68d8 *)pcVar9);
      (pcVar9->cls_0x5a3ab8).vftptr_0x0 =
           (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0((cls_0x5a68d8 *)pcVar9,uVar2,uVar1);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x190 = (dword)pcVar9;
  }
  else {
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 0;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)this->mbr_0x17c)[1];
      uVar2 = *(undefined4 *)this->mbr_0x17c;
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,1);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,uVar2,uVar1);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x188 = (dword)pcVar7;
    pcVar9 = (cls_0x5a3c68 *)FUN_00482fb0(0x74);
    pcStack_c = pcVar9;
    if (pcVar9 == (cls_0x5a3c68 *)0x0) {
      pcVar9 = (cls_0x5a3c68 *)0x0;
    }
    else {
      local_14 = *(cls_0x429950 *)this->mbr_0x17c;
      cVar3 = ((cls_0x429950 *)this->mbr_0x17c)[1];
      cls_0x5a68d8::cls_0x5a68d8((cls_0x5a68d8 *)pcVar9);
      (pcVar9->cls_0x5a3ab8).vftptr_0x0 =
           (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0((cls_0x5a68d8 *)pcVar9,local_14,cVar3);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x18c = (dword)pcVar9;
    pcVar9 = (cls_0x5a3c68 *)FUN_00482fb0(0x74);
    pcStack_c = pcVar9;
    if (pcVar9 == (cls_0x5a3c68 *)0x0) {
      pcVar9 = (cls_0x5a3c68 *)0x0;
    }
    else {
      cVar3 = ((cls_0x429950 *)this->mbr_0x17c)[1];
      local_14 = *(cls_0x429950 *)this->mbr_0x17c;
      cls_0x5a68d8::cls_0x5a68d8((cls_0x5a68d8 *)pcVar9);
      (pcVar9->cls_0x5a3ab8).vftptr_0x0 =
           (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0((cls_0x5a68d8 *)pcVar9,local_14,cVar3);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    this->mbr_0x190 = (dword)pcVar9;
  }
  bVar11 = DAT_0065d1b8 != 2;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,&DAT_005e4720,3,2,0x24,0x23,0,0,0,0,0xffffffff,
               bVar11 - 1 & 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x19c = dVar6;
  bVar11 = DAT_0065d1b8 != 1;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,s_Stats_005e4728,3,0x27,0x24,0x23,0,0,0,0,0xffffffff,
               (-(uint)bVar11 & 0xffff0000) + 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x198 = dVar6;
  bVar11 = DAT_0065d1b8 != 0;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,s_Equip_005e4730,3,0x4c,0x24,0x23,0,0,0,0,0xffffffff,
               (-(uint)bVar11 & 0xffff0000) + 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x194 = dVar6;
  bVar11 = DAT_0065d1bc != 2;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,s_Spell_005e4738,3,0x71,0x24,0x23,0,0,0,0,0xffffffff,
               bVar11 - 1 & 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x1a4 = dVar6;
  bVar11 = DAT_0065d1bc != 0;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,&DAT_005e4740,3,0x96,0x24,0x23,0,0,0,0,0xffffffff,
               (-(uint)bVar11 & 0xffff0000) + 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x1a0 = dVar6;
  bVar11 = DAT_0065d1bc != 1;
  pcStack_c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  if (pcStack_c != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcStack_c,&DAT_005e4744,3,0xbb,0x24,0x23,0,0,0,0,0xffffffff,
               (-(uint)bVar11 & 0xffff0000) + 0x10000 | 0x180010);
  }
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  dVar6 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  this->mbr_0x1a8 = dVar6;
  this->mbr_0x60 = this->mbr_0x60 | 2;
  iVar8 = 0;
  if (0 < *(int *)&this->field_0x88) {
    pdVar10 = &this->mbr_0x1ac;
    do {
      pdVar10[6] = 0;
      *pdVar10 = 0;
      iVar8 = iVar8 + 1;
      pdVar10 = pdVar10 + 1;
    } while (iVar8 < *(int *)&this->field_0x88);
  }
  dVar6 = DAT_00667c60 - DAT_0065be60;
  this->mbr_0x14 = (*(int *)(PTR_DAT_005d79e0 + 4) - DAT_0066614c) - DAT_0065be5c;
  this->mbr_0x18 = dVar6;
  (*this->vftptr_0x0->virt_meth_0x444f90_32)(this);
  ExceptionList = local_10;
  return 1;
}



// Function at 0053d360

void cls_0x5a5750_TSideTabsPane::virt_meth_0x53d360()

{
  if ((undefined4 *)this->mbr_0x188 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x188)(1);
  }
  if ((undefined4 *)this->mbr_0x18c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x18c)(1);
  }
  if ((undefined4 *)this->mbr_0x190 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x190)(1);
  }
  this->virt_meth_0x434f30();
  return;
}



// Function at 0053d400

void cls_0x5a5750_TSideTabsPane::virt_meth_0x53d400()

{
  if (this->mbr_0x50 != 0) {
    meth_0x53d8b0(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 0053d6e0

void cls_0x5a5750_TSideTabsPane::virt_meth_0x53d6e0(int param_1, int param_2)

{
  int in_stack_0000000c;
  
  if (((((param_1 == 5) || (param_1 == 4)) && (-1 < param_2)) &&
      ((-1 < in_stack_0000000c && (param_2 < (int)this->mbr_0xc)))) &&
     ((in_stack_0000000c < (int)this->mbr_0x10 &&
      (((-1 < DAT_0065d67c || (-1 < DAT_0065b090)) || (-1 < DAT_0065b878)))))) {
    FUN_0043a100(0,0,0);
    FUN_0043a140((int *)0x0);
    FUN_0043a170((char *)&DAT_00670218);
    if (-1 < DAT_0065d67c) {
      UNK_0065d53c._12_4_ = 1;
    }
    if (-1 < DAT_0065b090) {
      DAT_0065b02c._76_4_ = 1;
    }
    if (DAT_0065b878 != 0) {
      UNK_0065b824._12_4_ = 1;
    }
  }
  this->virt_meth_0x436530(param_1,param_2);
  return;
}



// Function at 0053d8b0

void cls_0x5a5750_TSideTabsPane::meth_0x53d8b0()

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 local_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined2 uStack_24;
  undefined2 uStack_22;
  
  cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)this->mbr_0x188,0,0,this->mbr_0x184,0x80000000);
  piVar1 = (int *)this->mbr_0x190;
  FUN_00438d80(&local_54,0,0,0,0,*(undefined4 *)(this->mbr_0x188 + 4),
               *(undefined4 *)(this->mbr_0x188 + 8),0x80000000);
  (**(code **)(*piVar1 + 0x5c))(&local_54);
  cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)this->mbr_0x188,0,0,this->mbr_0x17c,0x80000000);
  dVar2 = this->mbr_0x188;
  piVar1 = (int *)this->mbr_0x18c;
  FUN_00438d80((undefined4 *)&stack0xffffff9c,0,0,0,0,*(undefined4 *)(dVar2 + 4),
               *(undefined4 *)(dVar2 + 8),0x80000000);
  (**(code **)(*piVar1 + 0x5c))(&stack0xffffff9c,dVar2,0,0);
  cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)this->mbr_0x188,0,0,this->mbr_0x180,0x80000000);
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      iVar5 = iVar4;
      iVar3 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
      if ((*(uint *)(iVar3 + 0x14) & 0x10000) != 0) {
        uStack_40 = *(undefined4 *)(iVar3 + 0x6c);
        uStack_44 = *(undefined4 *)(iVar3 + 0x68);
        uStack_4c = *(undefined4 *)(iVar3 + 0x60);
        uStack_48 = *(undefined4 *)(iVar3 + 100);
        uStack_22 = 0;
        uStack_24 = 0;
        uStack_50 = 0;
        local_54 = 0;
        uStack_28 = 0x1f;
        uStack_2c = 0;
        iVar4 = iVar5;
        uStack_3c = uStack_4c;
        uStack_38 = uStack_48;
        uStack_34 = uStack_44;
        uStack_30 = uStack_40;
        (**(code **)(*(int *)this->mbr_0x18c + 0x5c))(&stack0xffffff8c,this->mbr_0x188,0,0);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x88);
  }
  return;
}



