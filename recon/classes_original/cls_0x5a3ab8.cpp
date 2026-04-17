// Decompiled methods and structure for class: cls_0x5a3ab8

/*
/OOAnalyzer/cls_0x5a3ab8
pack(disabled)
Structure cls_0x5a3ab8 {
   0   cls_0x5a3ab8::vftable_5a3ab8 *   4   vftptr_0x0   "pointer to cls_0x5a3ab8::vftable_5a3ab8"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   55   byte   1   mbr_0x37   "Unsigned Byte (db)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   87   byte   1   mbr_0x57   "Unsigned Byte (db)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   word   2   mbr_0x70   "Unsigned Word (dw, 2-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 144 Alignment: 1

*/

// Function at 0042a210

cls_0x5a3ab8 * __thiscall
OOAnalyzer::cls_0x5a3ab8::cls_0x5a3ab8
          (cls_0x5a3ab8 *this,dword param_1,dword param_2,char *param_3,char *param_4,dword param_5,
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
    puVar2 = (undefined4 *)FUN_00482ef0(~uVar3);
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

cls_0x5a3ab8 * __thiscall
OOAnalyzer::cls_0x5a3ab8::cls_0x5a3ab8(cls_0x5a3ab8 *this,int *param_1,uint param_2)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  char *pcVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  char *pcVar8;
  undefined4 *puVar9;
  int in_stack_0000000c;
  
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
  iVar2 = FUN_00479700(&DAT_005ccfb8,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410();
    if (iVar2 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_pos_005ccfc4,0);
      return this;
    }
    if (*(int *)(in_stack_0000000c + 0x10) == 8) {
      iVar2 = FUN_0047a410();
      if (iVar2 == 0) {
        (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_size_005ccfe8,0);
        return this;
      }
    }
  }
  iVar2 = FUN_00479700(&DAT_005cd008,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(in_stack_0000000c + 0x10) != 2) {
      (**(code **)(*param_1 + 0x98))(0,0);
      return this;
    }
    _strncpy((char *)&this->mbr_0x18,*(char **)(in_stack_0000000c + 0x28),0x1f);
    this->mbr_0x37 = 0;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_FIELD_005cd010,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(in_stack_0000000c + 0x10) != 2) goto LAB_0042a4fe;
    _strncpy((char *)&this->mbr_0x38,*(char **)(in_stack_0000000c + 0x28),0x1f);
    this->mbr_0x57 = 0;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_INDEX_005cd018,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(in_stack_0000000c + 0x10) != 8) {
LAB_0042a4fe:
      (**(code **)(*param_1 + 0x98))(0,0);
      return this;
    }
    this->mbr_0x58 = *(dword *)(in_stack_0000000c + 0x14);
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cd020,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (this->mbr_0x5c != 0) {
      FUN_00482f80(this->mbr_0x5c);
    }
    iVar2 = *(int *)(in_stack_0000000c + 0x10);
    this->mbr_0x5c = 0;
    if (iVar2 != 2) {
      (**(code **)(*param_1 + 0x98))(s_Text_expected_005cd028,0);
      return this;
    }
    pcVar4 = *(char **)(in_stack_0000000c + 0x28);
    uVar5 = 0xffffffff;
    pcVar8 = pcVar4;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
    } while (cVar1 != '\0');
    puVar3 = (undefined4 *)FUN_00482ef0(~uVar5);
    uVar5 = 0xffffffff;
    do {
      pcVar8 = pcVar4;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar8 = pcVar4 + 1;
      cVar1 = *pcVar4;
      pcVar4 = pcVar8;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    puVar7 = (undefined4 *)(pcVar8 + -uVar5);
    puVar9 = puVar3;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *puVar9 = *puVar7;
      puVar7 = puVar7 + 1;
      puVar9 = puVar9 + 1;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined *)puVar9 = *(undefined *)puVar7;
      puVar7 = (undefined4 *)((int)puVar7 + 1);
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    this->mbr_0x5c = (dword)puVar3;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cd038,0);
  if (iVar2 != 0) {
    FUN_00479580();
    pcVar4 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (pcVar4 == (char *)0x0) {
      this->mbr_0x5c = 0;
    }
    else {
      uVar5 = 0xffffffff;
      pcVar8 = pcVar4;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar8 + 1;
      } while (cVar1 != '\0');
      puVar3 = (undefined4 *)FUN_00482ef0(~uVar5);
      uVar5 = 0xffffffff;
      do {
        pcVar8 = pcVar4;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        pcVar8 = pcVar4 + 1;
        cVar1 = *pcVar4;
        pcVar4 = pcVar8;
      } while (cVar1 != '\0');
      uVar5 = ~uVar5;
      puVar7 = (undefined4 *)(pcVar8 + -uVar5);
      puVar9 = puVar3;
      for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *puVar9 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar9 = puVar9 + 1;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *(undefined *)puVar9 = *(undefined *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
      this->mbr_0x5c = (dword)puVar3;
    }
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_FLAGS_005cd03c,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410();
    if (iVar2 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_flags_005cd048,0);
      return this;
    }
    this->mbr_0x14 = this->mbr_0x14 | param_2;
  }
  iVar2 = FUN_00479700(s_GROUP_005cd068,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410();
    if (iVar2 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_group_005cd074,0);
      return this;
    }
  }
  iVar2 = FUN_00479700(&DAT_005cd094,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410();
    if (iVar2 == 0) {
      (**(code **)(*param_1 + 0x98))(s_Unable_to_parse_control_hotkey_005cd09c,0);
      return this;
    }
    this->mbr_0x70 = (short)(char)param_2;
  }
  this->mbr_0x14 = this->mbr_0x14 & 0xfffffffe;
  return this;
}



// Function at 00437dc0

cls_0x5a3ab8 * __thiscall OOAnalyzer::cls_0x5a3ab8::~cls_0x5a3ab8(cls_0x5a3ab8 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (this->mbr_0x5c != 0) {
    FUN_00482f80(this->mbr_0x5c);
  }
  this->mbr_0x5c = 0;
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00437e00

cls_0x5a3b24 * __thiscall
OOAnalyzer::cls_0x5a3ab8::virt_meth_0x437e00(cls_0x5a3ab8 *this,undefined4 param_1)

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
  pcVar1 = (cls_0x5a3b24 *)FUN_00482fb0(0x98,this);
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

cls_0x5a3ab8 * __thiscall OOAnalyzer::cls_0x5a3ab8::~cls_0x5a3ab8(cls_0x5a3ab8 *this)

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



