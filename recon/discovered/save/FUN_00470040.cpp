// FUN_00470040 @ 00470040 size=46

int __fastcall FUN_00470040(int *param_1)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x170))();
  while (iVar1 != 0) {
    param_1 = (int *)(**(code **)(*param_1 + 0x170))();
    iVar1 = (**(code **)(*param_1 + 0x170))();
  }
  return param_1[0x1a];
}


