// FUN_00529770 @ 00529770 size=181

void __thiscall FUN_00529770(int param_1,int param_2)

{
  undefined4 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined2 *puVar5;
  int iVar6;
  
  iVar3 = param_2;
  FUN_0049ce00(param_1 + 4);
  iVar6 = **(int **)(param_2 + 4);
  *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
  param_2 = iVar6;
  if (0 < iVar6) {
    do {
      puVar4 = (undefined4 *)FUN_00482fb0(0xc);
      if (puVar4 == (undefined4 *)0x0) {
        puVar4 = (undefined4 *)0x0;
      }
      else {
        *puVar4 = 0xffffffff;
        puVar4[2] = 0;
        puVar4[1] = 0;
      }
      puVar1 = *(undefined4 **)(iVar3 + 4);
      iVar6 = puVar1[1];
      *puVar4 = *puVar1;
      *(undefined4 **)(iVar3 + 4) = puVar1 + 2;
      puVar4[1] = iVar6;
      puVar5 = (undefined2 *)FUN_00482ef0(iVar6 << 1);
      iVar6 = puVar4[1];
      puVar4[2] = puVar5;
      if (0 < iVar6) {
        do {
          puVar2 = *(undefined2 **)(iVar3 + 4);
          *puVar5 = *puVar2;
          iVar6 = iVar6 + -1;
          *(undefined2 **)(iVar3 + 4) = puVar2 + 1;
          puVar5 = puVar5 + 1;
        } while (iVar6 != 0);
      }
      FUN_0041c840(puVar4);
      param_2 = param_2 + -1;
    } while (param_2 != 0);
  }
  return;
}


