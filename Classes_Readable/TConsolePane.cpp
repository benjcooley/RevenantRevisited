#include "TConsolePane.h"

// Decompiled methods and structure for class: TConsolePane



// Function at 004a22f0

uint TConsolePane::meth_0x4a22f0(uint param_1, undefined2 param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined2 in_stack_0000000a;
  uint *local_ac;
  code *local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  dword local_8c;
  dword local_88;
  undefined4 local_84;
  undefined4 local_80;
  dword local_7c;
  dword local_78;
  undefined4 local_74;
  undefined4 local_70;
  dword local_6c;
  dword local_68;
  uint local_64;
  uint local_60;
  undefined2 local_5c;
  undefined2 local_5a;
  undefined4 local_58 [4];
  dword *local_48;
  int local_44;
  int local_40;
  dword local_3c;
  dword local_38;
  dword local_34;
  dword local_30;
  
  local_7c = this->head;
  if ((((int)local_7c < 1) || (local_78 = this->chained, (int)local_78 < 1)) ||
     (this->axis == 0)) {
switchD_004a2405_caseD_1:
    return 0;
  }
  local_ac = (uint *)CONCAT22(in_stack_0000000a,param_2);
  if (local_ac == (uint *)0x80000000) {
    local_ac = (uint *)0xc00;
  }
  puVar3 = local_58;
  for (iVar2 = 0x16; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_48 = &this->cmdthreadhandle;
  local_a8 = (code *)0x0;
  local_a4 = 0;
  local_a0 = 0;
  local_84 = 0;
  local_80 = 0;
  local_74 = 0;
  local_70 = 0;
  local_5a = 0;
  local_5c = 0;
  local_98 = 0;
  local_9c = 0;
  local_88 = 0;
  local_8c = 0;
  local_90 = 0;
  local_94 = 0;
  local_60 = 0x1f;
  local_64 = 0;
  if (this->oldbuflen == 0) {
    local_44 = 0;
  }
  else {
    local_44 = (int)&this->oldbuflen + this->oldbuflen;
  }
  if (this->box == 0) {
    local_40 = 0;
  }
  else {
    local_40 = (int)&this->box + this->box;
  }
  local_6c = local_7c;
  local_68 = local_78;
  local_3c = local_7c;
  local_38 = local_78;
  local_34 = local_7c;
  local_30 = local_7c;
  switch(this->tail & 0x1f) {
  default:
    goto switchD_004a2405_caseD_1;
  case 2:
  case 4:
    local_58[3] = 4;
    uVar1 = FUN_004384a0(param_1);
    local_64 = uVar1 & 0xffff | (uVar1 & 0xffff) << 0x10;
    goto LAB_004a2471;
  case 8:
    local_58[3] = 8;
    break;
  case 0x10:
    local_58[3] = 0x10;
  }
  local_64 = (((param_1 >> 0x10 & 0xff) << 8 | param_1 >> 8 & 0xff) << 8 | param_1 & 0xff) << 8;
LAB_004a2471:
  local_8c = this->head;
  local_88 = this->chained;
  local_a8 = FUN_004b8410;
  local_94 = 0;
  local_90 = 0;
  local_60 = local_64;
  uVar1 = FUN_004ad0d0(local_58,&local_ac);
  return uVar1;
}



// Function at 004a2510

uint TConsolePane::meth_0x4a2510(int param_1, int param_2, undefined4 *param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, uint param_8, undefined4 param_9, undefined2 param_10, undefined4 param_11, undefined4 param_12)

{
  uint uVar1;
  int iVar2;
  dword *pdVar3;
  uint **ppuVar4;
  undefined2 in_stack_0000002a;
  dword local_ac [4];
  dword *local_9c;
  int local_98;
  int local_94;
  dword local_90;
  dword local_8c;
  dword local_88;
  dword local_84;
  undefined4 *local_80;
  int local_7c;
  int local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  int local_64;
  int local_60;
  int local_5c;
  undefined4 local_58;
  uint *local_54 [4];
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  dword local_34;
  dword local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  
  if (param_3 == (undefined4 *)0x0) {
    return 0;
  }
  pdVar3 = local_ac;
  for (iVar2 = 0x16; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pdVar3 = 0;
    pdVar3 = pdVar3 + 1;
  }
  ppuVar4 = local_54;
  for (iVar2 = 0x15; iVar2 != 0; iVar2 = iVar2 + -1) {
    *ppuVar4 = (uint *)0x0;
    ppuVar4 = ppuVar4 + 1;
  }
  local_ac[2] = param_3[4];
  local_ac[3] = this->tail;
  local_9c = &this->cmdthreadhandle;
  if (local_9c == (dword *)0x0) {
    return 0;
  }
  local_80 = param_3 + 0x12;
  if (local_80 == (undefined4 *)0x0) {
    return 0;
  }
  local_70 = param_3[1];
  local_74 = *param_3;
  local_90 = this->head;
  local_8c = this->chained;
  if (param_3[0xc] == 0) {
    local_7c = 0;
  }
  else {
    local_7c = param_3[0xc] + 0x30 + (int)param_3;
  }
  if (this->oldbuflen == 0) {
    local_98 = 0;
  }
  else {
    local_98 = (int)&this->oldbuflen + this->oldbuflen;
  }
  if (param_3[0xe] == 0) {
    local_78 = 0;
  }
  else {
    local_78 = param_3[0xe] + 0x38 + (int)param_3;
  }
  if (this->box == 0) {
    local_94 = 0;
  }
  else {
    local_94 = (int)&this->box + this->box;
  }
  if (param_3[0x10] == 0) {
    local_64 = 0;
  }
  else {
    local_64 = param_3[0x10] + 0x40 + (int)param_3;
  }
  if (param_3[10] == 0) {
    local_60 = 0;
  }
  else {
    local_60 = param_3[10] + 0x28 + (int)param_3;
  }
  if (param_3[8] == 0) {
    local_5c = 0;
  }
  else {
    local_5c = param_3[8] + 0x20 + (int)param_3;
  }
  local_58 = param_3[6];
  if (param_8 == 0x80000000) {
    param_8 = param_3[5];
  }
  local_54[0] = (uint *)(param_8 | 0x20);
  local_1c = param_4;
  local_18 = param_5;
  local_44 = 0;
  local_40 = 0;
  local_3c = 0;
  local_38 = 0;
  local_2c = param_1;
  local_28 = param_2;
  if ((param_8 & 0x20000) != 0) {
    local_2c = param_1 - param_3[2];
    local_28 = param_2 - param_3[3];
  }
  local_24 = param_6;
  local_14 = param_6;
  local_54[1] = (uint *)param_12;
  local_20 = param_7;
  local_10 = param_7;
  local_c = param_9;
  local_8 = _param_10;
  local_4 = (undefined2)param_11;
  local_88 = local_90;
  local_84 = local_90;
  local_6c = local_74;
  local_68 = local_74;
  local_34 = local_90;
  local_30 = local_8c;
  uVar1 = FUN_004ad0d0(local_ac,local_54);
  return uVar1;
}



