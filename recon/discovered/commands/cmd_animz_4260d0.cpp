// FUN_004260d0 @ 004260d0 size=118

undefined4 FUN_004260d0(int param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 unaff_retaddr;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cbf6c,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  piVar2 = (int *)FUN_0046e8a0();
  if (piVar2 == (int *)0x0) {
    return 0;
  }
  uVar3 = (**(code **)(*piVar2 + 0x74))(*(undefined2 *)(param_1 + 0xc));
  uVar4 = (**(code **)(*piVar2 + 0x78))(*(undefined2 *)(param_1 + 0xc));
  (**(code **)(*piVar2 + 0x80))(*(undefined2 *)(param_1 + 0xc),uVar3,uVar4,unaff_retaddr);
  return 0;
}


