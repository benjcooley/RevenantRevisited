// FUN_00498eb0_TSector_GetMaxScreenRect @ 00498eb0 size=169

void __thiscall FUN_00498eb0_TSector_GetMaxScreenRect(int param_1,int *param_2)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int local_18;
  int local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  undefined4 local_4;
  
  if ((*(uint *)(param_1 + 8) < 0x20) && (*(uint *)(param_1 + 0xc) < 0x20)) {
    local_18 = *(uint *)(param_1 + 8) * 0x400;
    local_14 = *(uint *)(param_1 + 0xc) * 0x400;
    local_c = local_18 + 0x3ff;
    local_8 = local_14 + 0x3ff;
  }
  else {
    local_8 = 0;
    local_c = 0;
    local_14 = 0;
    local_18 = 0;
  }
  local_10 = 0;
  local_4 = 0;
  piVar1 = param_2 + 1;
  FUN_0046d7a0(&local_18,param_2,piVar1);
  piVar2 = param_2 + 3;
  piVar3 = param_2 + 2;
  FUN_0046d7a0(&local_c,piVar3,piVar2);
  *param_2 = *param_2 + -0x600;
  *piVar1 = *piVar1 + -0x400;
  *piVar3 = *piVar3 + 0x600;
  *piVar2 = *piVar2 + 0x400;
  return;
}


