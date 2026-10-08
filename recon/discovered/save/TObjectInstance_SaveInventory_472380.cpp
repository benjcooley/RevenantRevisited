// FUN_00472380 @ 00472380 size=175

/* WARNING: Removing unreachable block (ram,0x0047240a) */

void __thiscall FUN_00472380(int *param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = (**(code **)(*param_1 + 0x170))();
  if (iVar2 == 0) {
    iVar2 = param_1[0x1a];
  }
  else {
    (**(code **)(*param_1 + 0x170))();
    iVar2 = FUN_00470040();
  }
  if ((*(int *)(param_2 + 0xc) + *(int *)(param_2 + 4)) - *(int *)(param_2 + 8) < 4) {
    FUN_0049cc70(4);
  }
  piVar1 = *(int **)(param_2 + 8);
  *piVar1 = iVar2;
  *(int **)(param_2 + 8) = piVar1 + 1;
  if (0 < iVar2) {
    FUN_0046dfb0();
  }
  return;
}


