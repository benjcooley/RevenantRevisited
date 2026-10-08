// FUN_004dd3e0 @ 004dd3e0 size=129

void __thiscall FUN_004dd3e0(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  FUN_00472430(param_2,param_3,param_4);
  if ((1 < param_3) && (param_3 < 5)) {
    piVar1 = *(int **)(param_2 + 4);
    iVar2 = *param_1;
    iVar3 = *piVar1;
    iVar4 = piVar1[1];
    *(int **)(param_2 + 4) = piVar1 + 2;
    (**(code **)(iVar2 + 0xe0))(s_Locked_005e0c74,iVar3 != 0);
    (**(code **)(*param_1 + 0xe0))(s_PickDifficulty_005e0c7c,iVar4);
  }
  if ((param_1[2] & 0x40000000U) == 0) {
    (**(code **)(*param_1 + 0x158))(1);
    (**(code **)(*param_1 + 0x18))((short)param_1[3]);
  }
  return;
}


