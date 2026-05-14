// FUN_00498f60_TSector_GetMaxMapRect @ 00498f60 size=71

void __thiscall FUN_00498f60_TSector_GetMaxMapRect(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  
  if ((*(uint *)(param_1 + 8) < 0x20) && (*(uint *)(param_1 + 0xc) < 0x20)) {
    iVar1 = *(uint *)(param_1 + 8) * 0x400;
    iVar2 = *(uint *)(param_1 + 0xc) * 0x400;
    *param_2 = iVar1;
    param_2[1] = iVar2;
    param_2[2] = iVar1 + 0x3ff;
    param_2[3] = iVar2 + 0x3ff;
    return;
  }
  param_2[3] = 0;
  param_2[2] = 0;
  param_2[1] = 0;
  *param_2 = 0;
  return;
}


