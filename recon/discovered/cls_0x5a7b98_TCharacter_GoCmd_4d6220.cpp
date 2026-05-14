// FUN_004d6220_SideStep @ 004d6220 size=398

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004d6220_SideStep(int *param_1,char param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  char local_18 [4];
  char local_14 [4];
  undefined2 local_10;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059ea13;
  local_c = ExceptionList;
  local_18 = (char  [4])s_sidestep_005e05c0._0_4_;
  local_14 = (char  [4])s_sidestep_005e05c0._4_4_;
  local_10 = ram0x005e05c8;
  ExceptionList = &local_c;
  iVar3 = FUN_0059a600(local_18,param_1[0x36] + 4,8);
  if (iVar3 != 0) {
    if ((param_2 != 'l') && (param_2 != 'r')) {
      iVar3 = FUN_00483300_RandomRange(0,1);
      param_2 = (-(iVar3 != 0) & 0xfaU) + 0x72;
    }
    local_10 = CONCAT11(local_10._1_1_,param_2);
    iVar3 = (**(code **)(*param_1 + 0x1f0))(local_18,0);
    if (iVar3 != 0) {
      iVar3 = FUN_00482fb0(100);
      uStack_4 = 0;
      if (iVar3 == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = FUN_004da9f0_TActionBlock_Ctor1(local_18,3);
      }
      iVar4 = param_1[0x2c];
      uVar1 = *(undefined4 *)(param_1[0x36] + 0x2c);
      *(undefined4 *)(iVar3 + 0x44) = *(undefined4 *)(param_1[0x36] + 0x44);
      iVar2 = *param_1;
      *(undefined4 *)(iVar3 + 0x2c) = uVar1;
      *(char *)((int)param_1 + 0x36) = (char)uVar1;
      *(uint *)(iVar3 + 0x30) = (-(uint)(param_2 != 'l') & 0xffffff80) + 0x40 + iVar4 & 0xff;
      uStack_4 = 0xffffffff;
      *(undefined4 *)(iVar3 + 0x34) = 8;
      *(uint *)(iVar3 + 0x60) = *(uint *)(iVar3 + 0x60) & 0xffffffdf | 0x210;
      iVar4 = (**(code **)(iVar2 + 0x208))(iVar3,0);
      if (iVar4 == 0) {
        if (iVar3 != 0) {
          if (*(int *)(iVar3 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar3 + 0x5c));
          }
          FUN_004830f0(iVar3);
          ExceptionList = local_c;
          return;
        }
      }
      else {
        FUN_00583e80_LogActionEvent(param_1,0x42,(int)param_2,1,0);
      }
    }
  }
  ExceptionList = local_c;
  return;
}


