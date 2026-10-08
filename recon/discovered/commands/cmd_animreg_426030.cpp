// FUN_00426030 @ 00426030 size=153

undefined4 FUN_00426030(int param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int unaff_EBX;
  int local_4;
  
  iVar1 = FUN_0047a410(param_2,s__d__d_005cbf64,&local_4,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  piVar2 = (int *)FUN_0046e8a0();
  if (piVar2 == (int *)0x0) {
    return 0;
  }
  iVar1 = (**(code **)(*piVar2 + 0x74))(*(undefined2 *)(param_1 + 0xc));
  iVar3 = (**(code **)(*piVar2 + 0x78))(*(undefined2 *)(param_1 + 0xc));
  uVar4 = (**(code **)(*piVar2 + 0x7c))(*(undefined2 *)(param_1 + 0xc));
  (**(code **)(*piVar2 + 0x80))
            (*(undefined2 *)(param_1 + 0xc),iVar1 - unaff_EBX,iVar3 - local_4,uVar4);
  return 0;
}


