#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a3c68



// Function at 0042a820

void cls_0x5a3c68::meth_0x42a820()

{
  code *pcVar1;
  int *piVar2;
  int in_stack_00000008;
  
  DAT_00655f44 = DAT_00655f44 + 1;
  if (0x14 < DAT_00655f44) {
    FUN_00481c10((byte *)s_Control_message_recursion_overfl_005cd0bc);
  }
  if ((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x40) == 0) ||
      (pcVar1 = (code *)(this->cls_0x5a3ab8).mbr_0x80, pcVar1 == (code *)0x0)) ||
     ((in_stack_00000008 != 3000 && (in_stack_00000008 != 0xbb9)))) {
    pcVar1 = (code *)(this->cls_0x5a3ab8).mbr_0x80;
    if (pcVar1 != (code *)0x0) {
      (*pcVar1)(this,in_stack_00000008);
    }
  }
  else {
    (*pcVar1)();
  }
  piVar2 = (int *)(this->cls_0x5a3ab8).mbr_0x78;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 0x5c))(this,in_stack_00000008);
    DAT_00655f44 = DAT_00655f44 + -1;
    return;
  }
  (**(code **)(*(int *)(this->cls_0x5a3ab8).mbr_0x8 + 0x94))(this,in_stack_00000008);
  DAT_00655f44 = DAT_00655f44 + -1;
  return;
}



// Function at 0042c2d0

cls_0x5a3c68::cls_0x5a3c68 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,int param_7,dword param_8,
          dword param_9,undefined4 param_10,undefined4 param_11,undefined4 param_12,
          undefined4 param_13,dword param_14,undefined4 param_15)

{
  dword *pdVar1;
  uint uVar2;
  dword in_stack_00000040;
  dword in_stack_00000044;
  
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,0,4,param_1,0,0xffffffff,0,param_2,param_3,param_4,param_5,
             -(ushort)(param_7 != 0) & 0x40,param_6,param_13,0,0,0,0xffffffff);
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3c68__vftable_5a3c68_005a3c68;
  this->mbr_0x98 = 0;
  this->mbr_0x90 = param_14;
  if (0 < (int)param_14) {
    (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 | 0x80;
  }
  if (-1 < (int)(this->cls_0x5a3ab8).mbr_0x74) {
    pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
    *pdVar1 = *pdVar1 | 0x80000;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar2 & 0x80000) != 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x40000;
  }
  this->mbr_0x9c = param_9;
  this->mbr_0xa4 = in_stack_00000040;
  this->mbr_0xac = 0;
  this->mbr_0xe4 = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0xdc = 0;
  this->mbr_0xd8 = 0;
  this->mbr_0xa0 = param_8;
  this->mbr_0xa8 = in_stack_00000044;
  this->mbr_0xd4 = 0x80000000;
  return this;
}



// Function at 0042c400

cls_0x5a3c68::cls_0x5a3c68 *this,int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
          undefined4 param_9)

{
  dword dVar1;
  uint uVar2;
  dword *pdVar3;
  dword dVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca5e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,0,4,param_2,0,0xffffffff,0,0,0,0,0,(undefined2)param_8,param_3,
             param_9,param_4,param_5,param_6,param_7);
  dVar1 = (this->cls_0x5a3ab8).mbr_0x74;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3c68__vftable_5a3c68_005a3c68;
  this->mbr_0x98 = 0;
  this->mbr_0x90 = 0;
  if (-1 < (int)dVar1) {
    pdVar3 = &(this->cls_0x5a3ab8).mbr_0x14;
    *pdVar3 = *pdVar3 | 0x80000;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar2 & 0x80000) != 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x40000;
  }
  if (param_1 != 0) {
    meth_0x42c850(this,&(this->cls_0x5a3ab8).mbr_0x18,param_1);
  }
  pdVar3 = (dword *)this->mbr_0x9c;
  if (pdVar3 != (dword *)0x0) {
    dVar1 = pdVar3[3];
    (this->cls_0x5a3ab8).mbr_0x60 = -pdVar3[2];
    dVar4 = *pdVar3;
    (this->cls_0x5a3ab8).mbr_0x64 = -dVar1;
    dVar1 = pdVar3[1];
    (this->cls_0x5a3ab8).mbr_0x68 = dVar4;
    (this->cls_0x5a3ab8).mbr_0x6c = dVar1;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0042c500

cls_0x5a3c68::cls_0x5a3c68 *this,int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
          undefined4 param_9,undefined4 param_10,undefined4 param_11)

{
  dword dVar1;
  uint uVar2;
  dword *pdVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca70;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,0,4,param_2,0,0xffffffff,0,param_3,param_4,0,0,(undefined2)param_10
             ,param_5,param_11,param_6,param_7,param_8,param_9);
  dVar1 = (this->cls_0x5a3ab8).mbr_0x74;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3c68__vftable_5a3c68_005a3c68;
  this->mbr_0x98 = 0;
  this->mbr_0x90 = 0;
  if (-1 < (int)dVar1) {
    pdVar3 = &(this->cls_0x5a3ab8).mbr_0x14;
    *pdVar3 = *pdVar3 | 0x80000;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar2 & 0x80000) != 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x40000;
  }
  if (param_1 != 0) {
    meth_0x42c850(this,&(this->cls_0x5a3ab8).mbr_0x18,param_1);
  }
  pdVar3 = (dword *)this->mbr_0x9c;
  if (pdVar3 != (dword *)0x0) {
    dVar1 = pdVar3[1];
    (this->cls_0x5a3ab8).mbr_0x68 = *pdVar3;
    (this->cls_0x5a3ab8).mbr_0x6c = dVar1;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0042c600

cls_0x5a3c68::cls_0x5a3c68 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined4 param_7,
          undefined4 param_8,undefined4 param_9,undefined4 param_10,undefined4 param_11)

{
  dword *pdVar1;
  dword dVar2;
  uint uVar3;
  undefined4 in_stack_00000030;
  
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,0,4,param_1,0,0xffffffff,0,param_2,param_3,param_4,param_5,
             (undefined2)param_11,param_6,in_stack_00000030,param_7,param_8,param_9,param_10);
  dVar2 = (this->cls_0x5a3ab8).mbr_0x74;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3c68__vftable_5a3c68_005a3c68;
  this->mbr_0x98 = 0;
  this->mbr_0x90 = 0;
  if (-1 < (int)dVar2) {
    pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
    *pdVar1 = *pdVar1 | 0x80000;
  }
  uVar3 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar3 & 0x80000) != 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar3 | 0x40000;
  }
  return this;
}



// Function at 0042c6a0

cls_0x5a3c68::cls_0x5a3c68 *this,undefined4 param_1,undefined4 param_2)

{
  dword *pdVar1;
  dword dVar2;
  int iVar3;
  uint uVar4;
  int in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca82;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,4);
  uVar4 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3c68__vftable_5a3c68_005a3c68;
  this->mbr_0x98 = 0;
  this->mbr_0x90 = 0;
  if ((uVar4 & 1) == 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar4 | 1;
    if (in_stack_0000000c == 0) {
      if ((uVar4 & 0x200000) == 0) {
        in_stack_0000000c = 0x656220;
        if ((uVar4 & 0x400000) == 0) {
          in_stack_0000000c = 0x655910;
        }
      }
      else {
        in_stack_0000000c = 0x6562d8;
      }
    }
    iVar3 = cls_0x42bd90::meth_0x42bd90((cls_0x42bd90 *)&this->mbr_0x9c,in_stack_0000000c,param_1);
    if (iVar3 != 0) {
      pdVar1 = (dword *)this->mbr_0x9c;
      if (pdVar1 != (dword *)0x0) {
        if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
          dVar2 = pdVar1[3];
          (this->cls_0x5a3ab8).mbr_0x60 = -pdVar1[2];
          (this->cls_0x5a3ab8).mbr_0x64 = -dVar2;
        }
        if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
          dVar2 = pdVar1[1];
          (this->cls_0x5a3ab8).mbr_0x68 = *pdVar1;
          (this->cls_0x5a3ab8).mbr_0x6c = dVar2;
        }
      }
      uVar4 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar4;
      if (-1 < (int)(this->cls_0x5a3ab8).mbr_0x74) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar4 | 0x80000;
      }
      uVar4 = (this->cls_0x5a3ab8).mbr_0x14;
      if ((uVar4 & 0x80000) != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar4 | 0x40000;
      }
    }
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0042c850

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a3c68::meth_0x42c850(char *param_1, cls_0x46d6b0 *param_2)

{
  char cVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  char *pcVar5;
  int in_stack_0000000c;
  char local_50 [78];
  char acStack_2 [2];
  
  if (in_stack_0000000c == 0) {
    in_stack_0000000c = _DAT_006668d0;
  }
  _strncpy(local_50,param_1,0x4f);
  uVar4 = 0xffffffff;
  acStack_2[1] = 0;
  pcVar5 = local_50;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar4 - 1);
  _strncpy(local_50 + (~uVar4 - 1),&DAT_005cd2cc,iVar2 + 0x4f);
  (local_50 + (~uVar4 - 1))[iVar2 + 0x4f] = '\0';
  dVar3 = cls_0x46d6b0::meth_0x46d710(param_2);
  this->mbr_0x9c = dVar3;
  if ((dVar3 != 0) && (*(int *)(dVar3 + 0x18) == 0)) {
    *(int *)(dVar3 + 0x18) = in_stack_0000000c;
  }
  _strncpy(local_50,param_1,0x4f);
  uVar4 = 0xffffffff;
  acStack_2[1] = 0;
  pcVar5 = local_50;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar4 - 1);
  _strncpy(local_50 + (~uVar4 - 1),&DAT_005cd2d0,iVar2 + 0x4f);
  (local_50 + (~uVar4 - 1))[iVar2 + 0x4f] = '\0';
  dVar3 = cls_0x46d6b0::meth_0x46d710(param_2);
  this->mbr_0xa0 = dVar3;
  if ((dVar3 != 0) && (*(int *)(dVar3 + 0x18) == 0)) {
    *(int *)(dVar3 + 0x18) = in_stack_0000000c;
  }
  _strncpy(local_50,param_1,0x4f);
  uVar4 = 0xffffffff;
  acStack_2[1] = 0;
  pcVar5 = local_50;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar4 - 1);
  _strncpy(local_50 + (~uVar4 - 1),&DAT_005cd2d4,iVar2 + 0x4f);
  (local_50 + (~uVar4 - 1))[iVar2 + 0x4f] = '\0';
  dVar3 = cls_0x46d6b0::meth_0x46d6b0(param_2);
  this->mbr_0xa4 = dVar3;
  if ((dVar3 != 0) && (*(int *)(dVar3 + 0x18) == 0)) {
    *(int *)(dVar3 + 0x18) = in_stack_0000000c;
  }
  _strncpy(local_50,param_1,0x4f);
  uVar4 = 0xffffffff;
  acStack_2[1] = 0;
  pcVar5 = local_50;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar4 - 1);
  _strncpy(local_50 + (~uVar4 - 1),&DAT_005cd2d8,iVar2 + 0x4f);
  (local_50 + (~uVar4 - 1))[iVar2 + 0x4f] = '\0';
  dVar3 = cls_0x46d6b0::meth_0x46d6b0(param_2);
  this->mbr_0xa8 = dVar3;
  if ((dVar3 != 0) && (*(int *)(dVar3 + 0x18) == 0)) {
    *(int *)(dVar3 + 0x18) = in_stack_0000000c;
  }
  this->mbr_0xd4 = 0x80000000;
  this->mbr_0xac = 0;
  this->mbr_0xe4 = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0xdc = 0;
  this->mbr_0xd8 = 0;
  return;
}



// Function at 0042ca50

void cls_0x5a3c68::virt_meth_0x42ca50()

{
  uint uVar1;
  dword dVar2;
  int iVar3;
  uint uVar4;
  cls_0x5a3c68 *this_00;
  uint in_stack_00000004;
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if (((uVar1 ^ in_stack_00000004) & 0x70000) != 0) {
    this->mbr_0x94 = 0;
    in_stack_00000004 = in_stack_00000004 | 0x20;
  }
  if (((in_stack_00000004 & 0x10) == 0) || (uVar4 = in_stack_00000004, (in_stack_00000004 & 6) != 0)
     ) {
    uVar4 = in_stack_00000004 & 0xfffffff7;
  }
  if (((uVar4 ^ (this->cls_0x5a3ab8).mbr_0x14) & 0xe) != 0) {
    uVar4 = uVar4 | 0x20;
  }
  (this->cls_0x5a3ab8).mbr_0x14 = uVar4;
  if ((((uVar1 ^ in_stack_00000004) & 0x10000) != 0) && ((uVar4 & 0x40000) != 0)) {
    if (((in_stack_00000004 & 0x10000) != 0) &&
       (((0 < (int)(this->cls_0x5a3ab8).mbr_0x74 &&
         (dVar2 = (this->cls_0x5a3ab8).mbr_0x8, dVar2 != 0)) &&
        (iVar3 = 0, 0 < *(int *)(dVar2 + 0x88))))) {
      do {
        if ((iVar3 < 0) || (*(int *)(dVar2 + 0x88) <= iVar3)) {
          this_00 = (cls_0x5a3c68 *)0x0;
        }
        else {
          this_00 = *(cls_0x5a3c68 **)(*(int *)(dVar2 + 0x98) + iVar3 * 4);
        }
        if (((this_00 != this) && ((this_00->cls_0x5a3ab8).mbr_0x10 == 4)) &&
           ((this_00->cls_0x5a3ab8).mbr_0x74 == (this->cls_0x5a3ab8).mbr_0x74)) {
          (*((this_00->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this_00);
          (*((this_00->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this_00);
        }
        dVar2 = (this->cls_0x5a3ab8).mbr_0x8;
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(dVar2 + 0x88));
    }
    meth_0x42a820(this);
  }
  return;
}



// Function at 0042cc30

void cls_0x5a3c68::virt_meth_0x42cc30()

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  char cVar4;
  char cVar5;
  dword dVar6;
  uint uVar7;
  cls_0x5a68d8 *in_stack_00000004;
  dword local_78;
  dword local_70;
  dword local_6c;
  dword local_68;
  cls_0x429950 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  dword local_2c;
  dword local_28;
  dword local_24;
  dword local_20;
  dword local_1c;
  dword local_18;
  dword local_14;
  dword local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  uVar7 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((((uVar7 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) && ((uVar7 & 0x100000) == 0)) {
    if (in_stack_00000004 == (cls_0x5a68d8 *)0x0) {
      in_stack_00000004 = (cls_0x5a68d8 *)PTR_DAT_005d79e0;
    }
    local_78 = this->mbr_0x9c;
    if (local_78 == 0) {
      FUN_0042bbb0((int *)in_stack_00000004,(this->cls_0x5a3ab8).mbr_0x60);
      uVar7 = (this->cls_0x5a3ab8).mbr_0x14 >> 0x10 & 1;
      cVar4 = (char)uVar7;
      cVar5 = cVar4 * '\x02';
      cls_0x429950::cls_0x429950(&local_58,0,0x8c);
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff6c);
      cls_0x5a68d8::meth_0x4be2b0
                (in_stack_00000004,(char)(this->cls_0x5a3ab8).mbr_0x60 + cVar5,
                 cVar5 + (char)(this->cls_0x5a3ab8).mbr_0x64,
                 (char)(this->cls_0x5a3ab8).mbr_0x68 + cVar4 * -2,
                 (this->cls_0x5a3ab8).mbr_0x6c + uVar7 * -2,(char)this + '\x18',0);
      (*in_stack_00000004->vftptr_0x0->virt_meth_0x4bcc60_32)(in_stack_00000004);
    }
    else {
      if (((uVar7 & 0x10000) == 0) || (this->mbr_0xa0 == 0)) {
        if (((uVar7 & 8) == 0) || (this->mbr_0xa4 == 0)) {
          if (((uVar7 & 4) == 0) || (this->mbr_0xa8 == 0)) {
            local_6c = this->mbr_0xd8;
          }
          else {
            local_6c = this->mbr_0xe4;
            local_78 = this->mbr_0xa8;
          }
        }
        else {
          local_6c = this->mbr_0xe0;
          local_78 = this->mbr_0xa4;
        }
      }
      else {
        local_6c = this->mbr_0xdc;
        local_78 = this->mbr_0xa0;
      }
      local_70 = this->mbr_0xd4;
      if (local_70 == 0x80000000) {
        local_70 = *(uint *)(local_78 + 0x14) & 0xfffdffef | 0x20;
      }
      if (this->mbr_0xac == 0) {
        dVar1 = (this->cls_0x5a3ab8).mbr_0x60;
        dVar6 = (this->cls_0x5a3ab8).mbr_0x68;
        local_68 = (this->cls_0x5a3ab8).mbr_0x64;
        dVar2 = (this->cls_0x5a3ab8).mbr_0x6c;
      }
      else {
        dVar1 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0xc4;
        local_68 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0xc8;
        dVar6 = this->mbr_0xcc + this->mbr_0xc4 + (this->cls_0x5a3ab8).mbr_0x68;
        dVar2 = this->mbr_0xd0 + this->mbr_0xc8 + (this->cls_0x5a3ab8).mbr_0x6c;
      }
      if ((local_70 & 0x2100) != 0) {
        local_58 = *(cls_0x429950 *)((this->cls_0x5a3ab8).mbr_0x8 + 0x84);
        if (local_58 == (cls_0x429950)0x0) {
          (*in_stack_00000004->vftptr_0x0->virt_meth_0x4bde60_100)
                    (in_stack_00000004,(char)dVar1,(char)local_68,(char)dVar6,(char)dVar2,0,0,0);
        }
        else {
          local_50 = 0;
          local_4c = 0;
          local_48 = 0;
          local_2 = 0;
          local_4 = 0;
          local_40 = 0;
          local_44 = 0;
          local_30 = 0;
          local_34 = 0;
          local_38 = 0;
          local_3c = 0;
          local_c = 0;
          local_28 = local_68;
          local_18 = local_68;
          local_54 = 0x20;
          local_8 = 0x1f;
          local_2c = dVar1;
          local_24 = dVar6;
          local_20 = dVar2;
          local_1c = dVar1;
          local_14 = dVar6;
          local_10 = dVar2;
          (*in_stack_00000004->vftptr_0x0->virt_meth_0x4bd490_92)(in_stack_00000004,&local_54);
        }
      }
      if (this->mbr_0xac == 0) {
        cls_0x5a68d8::meth_0x4bd680
                  (in_stack_00000004,(this->cls_0x5a3ab8).mbr_0x60,(this->cls_0x5a3ab8).mbr_0x64,
                   local_78,local_70);
      }
      else {
        cls_0x429ac0::meth_0x429ac0
                  ((cls_0x429ac0 *)&this->mbr_0xb0,in_stack_00000004,local_78,
                   (this->cls_0x5a3ab8).mbr_0x60,(this->cls_0x5a3ab8).mbr_0x64,
                   (this->cls_0x5a3ab8).mbr_0x68,(this->cls_0x5a3ab8).mbr_0x6c);
      }
      if (local_6c != 0) {
        if (((this->cls_0x5a3ab8).mbr_0x14 & 0x10000) == 0) {
          local_6c._0_1_ = (char)this->mbr_0x12c + (char)(this->cls_0x5a3ab8).mbr_0x64;
          cVar5 = (char)this->mbr_0x128;
          cVar4 = cVar5 + (char)(this->cls_0x5a3ab8).mbr_0x60;
          cVar5 = ((char)(this->cls_0x5a3ab8).mbr_0x68 - (char)this->mbr_0x130) - cVar5;
          iVar3 = ((this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x134) - this->mbr_0x12c;
        }
        else {
          local_6c._0_1_ = (char)this->mbr_0x13c + (char)(this->cls_0x5a3ab8).mbr_0x64;
          cVar5 = (char)this->mbr_0x138;
          cVar4 = cVar5 + (char)(this->cls_0x5a3ab8).mbr_0x60;
          cVar5 = ((char)(this->cls_0x5a3ab8).mbr_0x68 - (char)this->mbr_0x140) - cVar5;
          iVar3 = ((this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x144) - this->mbr_0x13c;
        }
        local_58 = (cls_0x429950)(this->cls_0x5a3ab8).mbr_0x5c;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff6c);
        cls_0x5a68d8::meth_0x4be2b0
                  (in_stack_00000004,cVar4,(char)local_6c,cVar5,iVar3,local_58.mbr_0x0,0);
      }
      (*in_stack_00000004->vftptr_0x0->virt_meth_0x4bcc60_32)(in_stack_00000004);
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 0042d2d0

void cls_0x5a3c68::meth_0x42d2d0()

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  if ((-1 < iVar2) &&
     (iVar3 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar3 != 0)) {
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar2,0x7f,1);
  }
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar1 & 0x40000) == 0) {
    if ((uVar1 & 0x80000) == 0) goto LAB_0042d37e;
  }
  else if ((uVar1 & 0x80000) == 0) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    return;
  }
  if ((uVar1 & 0x10000) == 0) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    return;
  }
LAB_0042d37e:
  meth_0x42a820(this);
  return;
}



// Function at 0042d6c0

void cls_0x5a3c68::virt_meth_0x42d6c0(undefined4 param_1, undefined4 param_2)

{
  undefined uVar1;
  undefined3 extraout_var;
  
  if ((this->mbr_0x98 != 0) && (((this->cls_0x5a3ab8).mbr_0x14 & 0x40000) == 0)) {
    uVar1 = (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a7a0_84)(this);
    if (CONCAT31(extraout_var,uVar1) != 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
      return;
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 00432a50

void cls_0x5a3c68::meth_0x432a50()

{
  dword dVar1;
  
  if (this->mbr_0x90 != 0) {
    dVar1 = (this->cls_0x5a3ab8).mbr_0x8;
    this->mbr_0x90 = 0;
    *(undefined4 *)(dVar1 + 0xa0) = 0;
    *(undefined4 *)(dVar1 + 0xa8) = 0;
    *(undefined4 *)(dVar1 + 0xac) = 1;
    (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffeff;
    meth_0x42a820(this);
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x4388c0_64)(this);
  }
  return;
}



// Function at 004381d0

cls_0x5a3c68 * cls_0x5a3c68::virt_meth_0x4381d0()

{
  byte in_stack_00000004;
  
  cls_0x5a3ab8::~cls_0x5a3ab8(&this->cls_0x5a3ab8);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00438220

cls_0x5a3cd8 * cls_0x5a3c68::virt_meth_0x438220(undefined4 param_1)

{
  cls_0x5a3cd8 *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ccbb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3cd8 *)FUN_00482fb0(0x168);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3cd8 *)0x0) {
    pcVar1 = cls_0x5a3cd8::cls_0x5a3cd8(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3cd8 *)0x0;
}



// Function at 004387f0

void cls_0x5a3c68::meth_0x4387f0()

{
  cls_0x5a3ab8__vftable_5a3ab8 *pcVar1;
  int in_stack_00000004;
  
  pcVar1 = (this->cls_0x5a3ab8).vftptr_0x0;
  if (in_stack_00000004 != 0) {
    (*pcVar1->virt_meth_0x42a770_28)(this);
    return;
  }
  (*pcVar1->virt_meth_0x42a770_28)(this);
  return;
}



// Function at 00438a50

void cls_0x5a3c68::meth_0x438a50()

{
  int in_stack_00000004;
  
  if (in_stack_00000004 != 0) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
    return;
  }
  (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  return;
}



