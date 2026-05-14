// FUN_00471ce0_CreateObjectFromStream @ 00471ce0 size=939

int * __fastcall
FUN_00471ce0_CreateObjectFromStream(undefined4 param_1,undefined4 param_2,int *param_3,int param_4,undefined4 param_5)

{
  short *psVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  undefined4 *puVar8;
  bool bVar9;
  int local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  int local_8c;
  int local_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined1 auStack_50 [80];
  
  local_90 = 0;
  local_8c = 0;
  local_88 = 0;
  if (7 < param_4) {
    sVar5 = *(short *)param_3[1];
    local_90 = CONCAT22((short)((uint)param_1 >> 0x10),sVar5);
    param_3[1] = (int)((short *)param_3[1] + 1);
    if (sVar5 < 0) {
      return (int *)0x0;
    }
  }
  psVar1 = (short *)param_3[1];
  sVar5 = *psVar1;
  local_9c = CONCAT22((short)((uint)param_2 >> 0x10),sVar5);
  piVar3 = (int *)(psVar1 + 1);
  param_3[1] = (int)piVar3;
  if (sVar5 < 0) {
    return (int *)0x0;
  }
  if (param_4 < 1) {
    sVar6 = (short)*piVar3;
    param_3[1] = (int)(psVar1 + 2);
    local_98 = 0xffffffff;
  }
  else {
    if (param_4 < 4) {
      local_a0 = *piVar3;
      param_3[1] = (int)(psVar1 + 3);
      local_98 = 0xffffffff;
    }
    else {
      local_a0 = *piVar3;
      local_98 = CONCAT22(local_98._2_2_,psVar1[3]);
      param_3[1] = (int)(psVar1 + 4);
    }
    sVar6 = -1;
  }
  if (param_4 < 0xe) {
    local_94 = 0xffffffff;
  }
  else {
    local_94 = CONCAT22(local_94._2_2_,*(undefined2 *)param_3[1]);
    param_3[1] = (int)((undefined2 *)param_3[1] + 1);
  }
  sVar7 = (short)local_98;
  if ((((uint)(int)sVar5 < DAT_0065a258_TObjectClass_numclasses) && ((&DAT_0065a148_TObjectClass_classes)[sVar5] != 0)) &&
     (*(int *)((&DAT_0065a148_TObjectClass_classes)[sVar5] + 4) != 0)) {
LAB_00471de9:
    if (((short)local_9c == 0x1a) && (-1 < sVar7)) goto LAB_00471e57;
  }
  else {
    if (DAT_0066814c != 0) {
      FUN_00481c10(s_Object_in_map_file_has_invalid_c_005d4954,0);
      goto LAB_00471de9;
    }
    if (-1 < sVar7) goto LAB_00471e57;
    local_9c = 9;
    local_88 = 1;
  }
  if (sVar6 < 0) {
    iVar2 = FUN_00477f20(&DAT_00659d18,local_a0,&local_a0);
    if ((iVar2 == 0) || (iVar2 = *(int *)(DAT_00659d28 + local_a0 * 4), iVar2 == 0)) {
      sVar6 = -1;
    }
    else {
      bVar9 = *(byte *)(iVar2 + 0x22) != (ushort)local_9c;
      if (bVar9) {
        local_8c = 1;
        local_9c = CONCAT22((short)((uint)local_a0 >> 0x10),(ushort)*(byte *)(iVar2 + 0x22));
      }
      sVar6 = *(short *)(iVar2 + 0x20);
      if (-1 < sVar6) goto LAB_00471ed1;
    }
    if (-1 < sVar7) {
LAB_00471e57:
      uVar4 = ((int)sVar7 - *param_3) + param_3[1];
      if ((uint)param_3[2] < uVar4) {
        return (int *)0x0;
      }
      param_3[1] = *param_3 + uVar4;
      return (int *)0x0;
    }
    FUN_00481c10(s_Bad_object_005d4994,0);
  }
LAB_00471ed1:
  puVar8 = &uStack_84;
  for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  uStack_84._0_2_ = (ushort)local_9c;
  uStack_80 = 0x8000000;
  uStack_84._2_2_ = sVar6;
  piVar3 = (int *)FUN_00474bb0_NewObject(&uStack_84,0xffffffff,param_5);
  if (piVar3 == (int *)0x0) {
    FUN_00481c10(s_Trouble_creating_loaded_object___005d49a0,0);
  }
  local_a0 = param_3[1] - *param_3;
  if (local_8c == 0) {
    (**(code **)(*piVar3 + 0x160))(param_3,param_4,(int)(short)local_90);
  }
  else {
    FUN_00472430(param_3,param_4,(int)(short)local_90);
  }
  iVar2 = param_3[1] - *param_3;
  sVar5 = (short)local_94;
  if (((byte)DAT_0065a254 & 2) == 0) {
    if (sVar5 < 1) goto LAB_00471fa3;
  }
  else {
    if (-1 < sVar5) {
      uVar4 = sVar5 + iVar2;
      if (uVar4 <= (uint)param_3[2]) {
        param_3[1] = *param_3 + uVar4;
      }
      goto LAB_00471fbc;
    }
LAB_00471fa3:
    if (0xd < param_4) goto LAB_00471fbc;
  }
  (**(code **)(*piVar3 + 0x168))(param_3,param_4,param_5);
LAB_00471fbc:
  sVar5 = (short)local_98;
  if (((-1 < sVar5) && ((param_3[1] - *param_3) - local_a0 != (int)sVar5)) ||
     ((-1 < (short)local_94 && ((param_3[1] - *param_3) - iVar2 != (int)(short)local_94)))) {
    if (piVar3[0xe] == 0) {
      FUN_0058b100(auStack_50,s_Block_size_error_005d49f8);
    }
    else {
      FUN_0058b100(auStack_50,s_Block_size_error_for_object__s_005d49d8,piVar3[0xe]);
    }
    FUN_004820b0(auStack_50);
  }
  if ((-1 < sVar5) && ((uint)(sVar5 + local_a0) <= (uint)param_3[2])) {
    param_3[1] = *param_3 + sVar5 + local_a0;
  }
  if ((local_88 == 0) &&
     (((((byte)DAT_0065a254 & 1) == 0 || ((piVar3[2] & 0x80000U) == 0)) &&
      ((short)piVar3[1] != 0x1a)))) {
    return piVar3;
  }
  if (piVar3 == (int *)0x0) {
    return (int *)0x0;
  }
  (**(code **)*piVar3)(1);
  return (int *)0x0;
}


