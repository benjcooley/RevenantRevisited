// FUN_0046f250 @ 0046f250 size=335

undefined4 __thiscall FUN_0046f250(int *param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  undefined1 *puStack_1c;
  undefined1 local_10 [16];
  
  if ((int *)param_1[0x15] != (int *)0x0) {
    puStack_1c = (undefined1 *)0x46f267;
    uVar1 = (**(code **)(*(int *)param_1[0x15] + 0x3c))();
    if (uVar1 <= param_2) {
      puStack_1c = (undefined1 *)0x1;
      (**(code **)(*param_1 + 0x158))();
      return 0;
    }
  }
  uVar1 = param_1[2];
  if (((uVar1 & 8) == 0) &&
     ((((uVar1 & 0x400) == 0 || ((uVar1 & 4) != 0)) && ((short)param_1[0x1f] < 0)))) {
    puStack_1c = local_10;
    (**(code **)(*param_1 + 0xf4))();
    puStack_1c = (undefined1 *)((param_1[2] & 0xffU) >> 2 & 1);
    FUN_004548a0(local_10);
  }
  puStack_1c = (undefined1 *)0x0;
  FUN_00452750(param_1,3);
  puStack_1c = (undefined1 *)0x413;
  FUN_00456790(s_d__revenant_Object_cpp_005d4810);
  *(short *)((int)param_1 + 0x62) = (short)param_1[0x17];
  puStack_1c = (undefined1 *)0x0;
  *(short *)(param_1 + 0x18) = (short)param_1[3];
  *(short *)(param_1 + 3) = (short)param_2;
  (**(code **)(*param_1 + 0x158))();
  (**(code **)(*param_1 + 0x140))();
  FUN_004567c0();
  FUN_00452750(param_1,0,0);
  iVar2 = (**(code **)(*(int *)param_1[0x15] + 0x38))(param_1);
  if (iVar2 == 0) {
    (**(code **)(*param_1 + 0x2c))();
    uVar1 = param_1[2] & 0xffffbfff;
  }
  else {
    uVar1 = param_1[2] | 0x4000;
  }
  param_1[2] = uVar1;
  if ((((uVar1 & 8) == 0) && (((uVar1 & 0x400) == 0 || ((uVar1 & 4) != 0)))) &&
     ((short)param_1[0x1f] < 0)) {
    (**(code **)(*param_1 + 0xf4))(&stack0xffffffe8);
    FUN_004548a0(&puStack_1c,(param_1[2] & 0xffU) >> 2 & 1);
  }
  return 1;
}


