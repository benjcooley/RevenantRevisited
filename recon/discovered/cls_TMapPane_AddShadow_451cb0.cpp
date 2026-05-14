// FUN_00451cb0_TMapPane_AddShadow @ 00451cb0 size=180

int FUN_00451cb0_TMapPane_AddShadow(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_84;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined1 local_50 [80];
  
  if (*(short *)(param_1 + 4) == 0xe) {
    *(undefined4 *)(param_1 + 0x50) = 0xffffffff;
    return -1;
  }
  puVar2 = &local_84;
  for (iVar1 = 0xd; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  FUN_0058b100(local_50,s_shadow__s_005d0480,**(undefined4 **)(param_1 + 0x4c));
  local_84._0_2_ = 0xe;
  iVar1 = FUN_00475210(local_50,0);
  if (iVar1 < 0) {
    return -1;
  }
  local_78 = *(undefined4 *)(param_1 + 0x10);
  local_74 = *(undefined4 *)(param_1 + 0x14);
  local_84._2_2_ = (undefined2)iVar1;
  local_70 = *(undefined4 *)(param_1 + 0x18);
  iVar1 = FUN_00450e40_TMapPane_NewObject(&local_84,0xffffffff);
  if (-1 < iVar1) {
    *(int *)(param_1 + 0x50) = iVar1;
  }
  return iVar1;
}


