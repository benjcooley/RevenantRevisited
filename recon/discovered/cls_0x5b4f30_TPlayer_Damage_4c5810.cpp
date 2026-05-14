// FUN_004c5810_Damage @ 004c5810 size=100

void __thiscall FUN_004c5810_Damage(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  if (-1 < param_3) {
    param_2 = (**(code **)(*param_1 + 0x224))(param_2,param_3,param_4);
  }
  iVar1 = (**(code **)(*param_1 + 0x1c8))();
  if (iVar1 <= param_2) {
    (**(code **)(*param_1 + 0x1cc))(0);
    return;
  }
  iVar1 = *param_1;
  iVar2 = (**(code **)(iVar1 + 0x1c8))();
  (**(code **)(iVar1 + 0x1cc))(iVar2 - param_2);
  return;
}


