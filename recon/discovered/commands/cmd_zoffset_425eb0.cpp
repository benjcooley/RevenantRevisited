// FUN_00425eb0 @ 00425eb0 size=205

undefined4 FUN_00425eb0(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 unaff_retaddr;
  
  if ((*(int *)(param_2 + 0x10) != 9) && (*(int *)(param_2 + 0x10) != 10)) {
    iVar1 = FUN_0047a410(param_2,&DAT_005cbf58,&param_2);
    if (iVar1 == 0) {
      return 4;
    }
    piVar2 = (int *)FUN_0046e8a0();
    if (piVar2 != (int *)0x0) {
      uVar3 = (**(code **)(*piVar2 + 100))(*(undefined2 *)(param_1 + 0xc));
      uVar4 = (**(code **)(*piVar2 + 0x68))(*(undefined2 *)(param_1 + 0xc));
      (**(code **)(*piVar2 + 0x70))(*(undefined2 *)(param_1 + 0xc),uVar3,uVar4,unaff_retaddr);
    }
    FUN_00454920(*(undefined4 *)(param_1 + 0x40));
    return 0;
  }
  piVar2 = (int *)FUN_0046e8a0();
  uVar3 = (**(code **)(*piVar2 + 0x6c))(*(undefined2 *)(param_1 + 0xc));
  FUN_0058b100(&DAT_00654a88,s_Current_zoffset___d_005cbf40,uVar3);
  FUN_0041ee50(&DAT_00654a88);
  FUN_00479580();
  return 0;
}


