#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a3ab8



// Function at 0042a210

cls_0x5a3ab8::cls_0x5a3ab8 *this,dword param_1,dword param_2,char *param_3,char *param_4,dword param_5,
          char *param_6,dword param_7,dword param_8,dword param_9,dword param_10,undefined2 param_11
          ,undefined4 param_12,dword param_13,dword param_14,dword param_15,dword param_16,
          dword param_17)

{
  char cVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  undefined4 *puVar7;
  undefined2 in_stack_0000002e;
  dword in_stack_00000048;
  
  this->mbr_0x8 = param_1;
  this->vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  this->mbr_0xc = 0xffffffff;
  if (param_3 == (char *)0x0) {
    *(undefined *)&this->mbr_0x18 = 0;
  }
  else {
    _strncpy((char *)&this->mbr_0x18,param_3,0x1f);
    this->mbr_0x37 = 0;
  }
  if (param_4 == (char *)0x0) {
    *(undefined *)&this->mbr_0x38 = 0;
  }
  else {
    _strncpy((char *)&this->mbr_0x38,param_4,0x1f);
    this->mbr_0x57 = 0;
  }
  this->mbr_0x58 = param_5;
  if (param_6 == (char *)0x0) {
    this->mbr_0x5c = 0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar6 = param_6;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    puVar2 = FUN_00482ef0(~uVar3);
    uVar3 = 0xffffffff;
    do {
      pcVar6 = param_6;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar6 = param_6 + 1;
      cVar1 = *param_6;
      param_6 = pcVar6;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    puVar5 = (undefined4 *)(pcVar6 + -uVar3);
    puVar7 = puVar2;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar7 = puVar7 + 1;
    }
    for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined *)puVar7 = *(undefined *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    this->mbr_0x5c = (dword)puVar2;
  }
  this->mbr_0x10 = param_2;
  this->mbr_0x60 = param_7;
  this->mbr_0x68 = param_9;
  this->mbr_0x6c = param_10;
  this->mbr_0x64 = param_8;
  this->mbr_0x74 = param_13;
  this->mbr_0x70 = (word)param_12;
  this->mbr_0x78 = 0;
  this->mbr_0x14 = _param_11;
  if (in_stack_00000048 != 0) {
    this->mbr_0x80 = in_stack_00000048;
    this->mbr_0x14 = _param_11 | 0x40;
    return this;
  }
  if (param_14 != 0) {
    this->mbr_0x14 = _param_11 & 0xffffffbf;
    this->mbr_0x80 = param_14;
    this->mbr_0x84 = param_15;
    this->mbr_0x88 = param_16;
    this->mbr_0x8c = param_17;
  }
  return this;
}



// Function at 0042a350

cls_0x5a3ab8::cls_0x5a3ab8 *this,int *param_1,uint param_2)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined4 *puVar4;
  undefined3 extraout_var_04;
  char *pcVar5;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  char *pcVar9;
  undefined4 *puVar10;
  uint *in_stack_0000000c;
  
  this->mbr_0xc = 0xffffffff;
  this->mbr_0x58 = 0xffffffff;
  this->mbr_0x74 = 0xffffffff;
  this->mbr_0x80 = 0;
  this->mbr_0x10 = param_2;
  this->mbr_0x84 = 0;
  this->mbr_0x8 = 0;
  *(undefined *)&this->mbr_0x18 = 0;
  *(undefined *)&this->mbr_0x38 = 0;
  this->mbr_0x5c = 0;
  this->mbr_0x70 = 0;
  this->mbr_0x78 = 0;
  this->mbr_0x88 = 0;
  this->mbr_0x8c = 0xffffffff;
  this->vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  this->mbr_0x64 = 0xffffd8f0;
  this->mbr_0x60 = 0xffffd8f0;
  this->mbr_0x6c = 0xffffd8f0;
  this->mbr_0x68 = 0xffffd8f0;
  this->mbr_0x14 = 1;
  bVar2 = FUN_00479700((uint)in_stack_0000000c,&DAT_005ccfb8,0);
  if (CONCAT31(extraout_var,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar3 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i_005ccfbc);
    if (iVar3 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_pos_005ccfc4,0);
      return this;
    }
    if (in_stack_0000000c[4] == 8) {
      iVar3 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i_005ccfe0);
      if (iVar3 == 0) {
        (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_size_005ccfe8,0);
        return this;
      }
    }
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd008,0);
  if (CONCAT31(extraout_var_00,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if (in_stack_0000000c[4] != 2) {
      (**(code **)(*param_1 + 0x98))(0,0);
      return this;
    }
    _strncpy((char *)&this->mbr_0x18,(char *)in_stack_0000000c[10],0x1f);
    this->mbr_0x37 = 0;
    FUN_00479580((byte *)in_stack_0000000c);
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FIELD_005cd010,0);
  if (CONCAT31(extraout_var_01,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if (in_stack_0000000c[4] != 2) goto LAB_0042a4fe;
    _strncpy((char *)&this->mbr_0x38,(char *)in_stack_0000000c[10],0x1f);
    this->mbr_0x57 = 0;
    FUN_00479580((byte *)in_stack_0000000c);
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_INDEX_005cd018,0);
  if (CONCAT31(extraout_var_02,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if (in_stack_0000000c[4] != 8) {
LAB_0042a4fe:
      (**(code **)(*param_1 + 0x98))(0,0);
      return this;
    }
    this->mbr_0x58 = in_stack_0000000c[5];
    FUN_00479580((byte *)in_stack_0000000c);
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd020,0);
  if (CONCAT31(extraout_var_03,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if ((LPCVOID)this->mbr_0x5c != (LPCVOID)0x0) {
      FUN_00482f80((LPCVOID)this->mbr_0x5c);
    }
    uVar6 = in_stack_0000000c[4];
    this->mbr_0x5c = 0;
    if (uVar6 != 2) {
      (**(code **)(*param_1 + 0x98))(s_Text_expected_005cd028,0);
      return this;
    }
    pcVar5 = (char *)in_stack_0000000c[10];
    uVar6 = 0xffffffff;
    pcVar9 = pcVar5;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    puVar4 = FUN_00482ef0(~uVar6);
    uVar6 = 0xffffffff;
    do {
      pcVar9 = pcVar5;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar9 = pcVar5 + 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar9;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    puVar8 = (undefined4 *)(pcVar9 + -uVar6);
    puVar10 = puVar4;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar10 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar10 = puVar10 + 1;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined *)puVar10 = *(undefined *)puVar8;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
      puVar10 = (undefined4 *)((int)puVar10 + 1);
    }
    this->mbr_0x5c = (dword)puVar4;
    FUN_00479580((byte *)in_stack_0000000c);
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd038,0);
  if (CONCAT31(extraout_var_04,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    pcVar5 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (pcVar5 == (char *)0x0) {
      this->mbr_0x5c = 0;
    }
    else {
      uVar6 = 0xffffffff;
      pcVar9 = pcVar5;
      do {
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      puVar4 = FUN_00482ef0(~uVar6);
      uVar6 = 0xffffffff;
      do {
        pcVar9 = pcVar5;
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        pcVar9 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar9;
      } while (cVar1 != '\0');
      uVar6 = ~uVar6;
      puVar8 = (undefined4 *)(pcVar9 + -uVar6);
      puVar10 = puVar4;
      for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        *puVar10 = *puVar8;
        puVar8 = puVar8 + 1;
        puVar10 = puVar10 + 1;
      }
      for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined *)puVar10 = *(undefined *)puVar8;
        puVar8 = (undefined4 *)((int)puVar8 + 1);
        puVar10 = (undefined4 *)((int)puVar10 + 1);
      }
      this->mbr_0x5c = (dword)puVar4;
    }
    FUN_00479580((byte *)in_stack_0000000c);
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FLAGS_005cd03c,0);
  if (CONCAT31(extraout_var_05,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar3 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd044);
    if (iVar3 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_flags_005cd048,0);
      return this;
    }
    this->mbr_0x14 = this->mbr_0x14 | param_2;
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_GROUP_005cd068,0);
  if (CONCAT31(extraout_var_06,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar3 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd070);
    if (iVar3 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_group_005cd074,0);
      return this;
    }
  }
  bVar2 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd094,0);
  if (CONCAT31(extraout_var_07,bVar2) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar3 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd098);
    if (iVar3 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_hotkey_005cd09c,0);
      return this;
    }
    this->mbr_0x70 = (short)(char)param_2;
  }
  this->mbr_0x14 = this->mbr_0x14 & 0xfffffffe;
  return this;
}



// Function at 00437dc0

cls_0x5a3ab8 * __thiscall cls_0x5a3ab8::~cls_0x5a3ab8(cls_0x5a3ab8 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if ((LPCVOID)this->mbr_0x5c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x5c);
  }
  this->mbr_0x5c = 0;
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00437e00

cls_0x5a3b24 * cls_0x5a3ab8::virt_meth_0x437e00(undefined4 param_1)

{
  cls_0x5a3b24 *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca0b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3b24 *)FUN_00482fb0(0x98);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3b24 *)0x0) {
    pcVar1 = cls_0x5a3b24::cls_0x5a3b24(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3b24 *)0x0;
}



// Function at 004381f0

cls_0x5a3ab8 * __thiscall cls_0x5a3ab8::~cls_0x5a3ab8(cls_0x5a3ab8 *this)

{
  cls_0x5a3ab8 *pcVar1;
  
  pcVar1 = (cls_0x5a3ab8 *)this->mbr_0x5c;
  this->vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar1 != (cls_0x5a3ab8 *)0x0) {
    pcVar1 = (cls_0x5a3ab8 *)FUN_00482f80(pcVar1);
  }
  this->mbr_0x5c = 0;
  return pcVar1;
}



