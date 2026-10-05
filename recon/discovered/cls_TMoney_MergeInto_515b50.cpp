// FUN_00515b50 @ 00515b50 size=175

undefined4 __thiscall FUN_00515b50(int *param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  FUN_0046fee0(param_2);
  piVar1 = (int *)(**(code **)(*param_2 + 0xa8))(&DAT_005e1d98);
  if (((piVar1 != (int *)0x0) && ((short)param_1[1] == (short)piVar1[1])) &&
     (*(short *)((int)param_1 + 6) == *(short *)((int)piVar1 + 6))) {
    iVar2 = (**(code **)(*piVar1 + 0x198))();
    iVar3 = (**(code **)(*param_1 + 0x198))();
    (**(code **)(*piVar1 + 0x19c))(iVar2 + iVar3);
    return 1;
  }
  if ((param_1[0x19] == DAT_0065d674) && (piVar1 = (int *)param_1[0x15], piVar1 != (int *)0x0)) {
    iVar2 = param_1[3];
    uVar4 = (**(code **)(*param_1 + 0x198))();
    uVar4 = (**(code **)(*piVar1 + 0xd4))((short)iVar2,0,(int)*(short *)((int)param_1 + 6),uVar4);
    FUN_00515fb0(uVar4);
  }
  return 0;
}


