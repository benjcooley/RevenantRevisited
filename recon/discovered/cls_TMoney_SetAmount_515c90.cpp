// FUN_00515c90 @ 00515c90 size=465

void __thiscall FUN_00515c90(int *param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  
  iVar1 = (**(code **)(*param_1 + 0x198))();
  if (iVar1 == param_2) {
    return;
  }
  if (param_2 < 1) {
    return;
  }
  if (param_1[0x19] == DAT_0065d674) {
    uVar2 = (**(code **)(*param_1 + 0x198))();
    if (uVar2 < 0x40) {
      if (uVar2 == 0) {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 0x40;
    }
    iVar1 = uVar2 + *(short *)((int)param_1 + 6) * 0x41;
    if ((((&DAT_0066d584)[iVar1] != 0) &&
        (iVar4 = (&DAT_0066d584)[iVar1] + -1, (&DAT_0066d584)[iVar1] = iVar4, iVar4 < 1)) &&
       (uVar2 * 0x104 != -0x66d6a0)) {
      FUN_004830f0((&DAT_0066d6a0)[iVar1]);
      (&DAT_0066d6a0)[iVar1] = 0;
    }
    if ((int *)param_1[0x15] != (int *)0x0) {
      uVar3 = (**(code **)(*(int *)param_1[0x15] + 0xd4))
                        ((short)param_1[3],0,(int)*(short *)((int)param_1 + 6),param_2);
      FUN_00515fb0(uVar3);
    }
  }
  if (param_1[0x19] == 0) {
    uVar2 = (**(code **)(*param_1 + 0x198))();
    if (uVar2 < 0x40) {
      if (uVar2 == 0) {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 0x40;
    }
    iVar1 = uVar2 + *(short *)((int)param_1 + 6) * 0x41;
    if ((((&DAT_0066d480)[iVar1] != 0) &&
        (iVar4 = (&DAT_0066d480)[iVar1] + -1, (&DAT_0066d480)[iVar1] = iVar4, iVar4 < 1)) &&
       (uVar2 * 0x104 != -0x66d7e4)) {
      FUN_004830f0((&DAT_0066d7e4)[iVar1]);
      (&DAT_0066d7e4)[iVar1] = 0;
    }
    if ((int *)param_1[0x15] != (int *)0x0) {
      uVar3 = (**(code **)(*(int *)param_1[0x15] + 0xcc))
                        ((short)param_1[3],0,(int)*(short *)((int)param_1 + 6),param_2);
      FUN_00516130(uVar3);
    }
  }
  if (param_2 < 0xb) {
    iVar1 = *param_1;
    uVar3 = 0;
  }
  else {
    if (param_2 < 0x65) {
      (**(code **)(*param_1 + 0x18))(2);
      goto LAB_00515e4b;
    }
    if (param_2 < 0x12d) {
      iVar1 = *param_1;
      uVar3 = 4;
    }
    else {
      if (param_2 < 0x1f5) {
        (**(code **)(*param_1 + 0x18))(6);
        goto LAB_00515e4b;
      }
      iVar1 = *param_1;
      uVar3 = 8;
    }
  }
  (**(code **)(iVar1 + 0x18))(uVar3);
LAB_00515e4b:
  (**(code **)(*param_1 + 0xe8))(DAT_0066d694,param_2);
  return;
}


