// FUN_00427f60 @ 00427f60 size=185

undefined4 FUN_00427f60(uint param_1,undefined4 param_2,uint param_3,uint *param_4)

{
  int iVar1;
  uint uVar2;
  
  if (DAT_0066829c == 0) {
    if (DAT_0065cb30 != (int *)0x0) {
      (**(code **)(*DAT_0065cb30 + 0x28))();
    }
    if (param_4 != (uint *)0x0) {
      *param_4 = *param_4 & 0xfffffffd;
    }
    return 1;
  }
  uVar2 = 0;
  if (((param_1 == 0) || (*(short *)(param_1 + 4) != 0xb)) &&
     ((param_3 == 0 || (param_1 = param_3, *(short *)(param_3 + 4) != 0xb)))) {
    if ((param_4 == (uint *)0x0) || (param_4[0x31] == 0)) goto LAB_00427fdc;
    iVar1 = FUN_0059a530(param_4[0x33],&DAT_005cabd8);
    if ((iVar1 != 0) || (param_1 = param_4[0x31], *(short *)(param_1 + 4) != 0xb))
    goto LAB_00427fdc;
  }
  uVar2 = param_1;
  if (param_1 != 0) {
    FUN_0051d680(*(uint *)(param_1 + 0x36c) & 0xfffffff7);
  }
LAB_00427fdc:
  if ((uVar2 != DAT_00667fcc) && (param_4 != (uint *)0x0)) {
    FUN_00492cc0(0x20);
  }
  return 1;
}


