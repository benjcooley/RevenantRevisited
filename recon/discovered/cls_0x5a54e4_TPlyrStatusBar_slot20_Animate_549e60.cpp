// FUN_00549e60 @ 00549e60 size=571

void __fastcall FUN_00549e60(int *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  void *pvStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  void *pvStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined2 uStack_10;
  undefined2 uStack_e;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a21a4;
  pvStack_c = ExceptionList;
  iVar4 = 0;
  if (DAT_00667fcc == 0) {
    return;
  }
  if (param_1[0x14] == 0) {
    piVar1 = *(int **)(DAT_00667fcc + 0xe0);
    if ((piVar1 != (int *)0x0) && ((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19))))))
    {
      iVar4 = piVar1[0x11];
    }
    ExceptionList = &pvStack_c;
    if (DAT_00667fcc != param_1[0x25]) {
      ExceptionList = &pvStack_c;
      FUN_0054a0a0();
    }
    if (iVar4 == param_1[0x2a]) {
      ExceptionList = pvStack_c;
      return;
    }
    FUN_0054a310();
    ExceptionList = pvStack_c;
    return;
  }
  if (DAT_006680c8 == 0) {
    ExceptionList = &pvStack_c;
    puVar2 = (undefined4 *)FUN_00482fb0(0x78);
    uStack_4 = 2;
    if (puVar2 != (undefined4 *)0x0) {
      uVar3 = FUN_0046d710(&DAT_005e5734);
      FUN_004bcb00();
      uStack_4 = CONCAT31(uStack_4._1_3_,3);
      *puVar2 = &PTR_FUN_005a3980;
      puVar2[0x1a] = 0;
      FUN_004a5ca0(uVar3,0x288);
      puVar2[0x1c] = 1;
      goto LAB_00549f52;
    }
  }
  else {
    ExceptionList = &pvStack_c;
    puVar2 = (undefined4 *)FUN_00482fb0(0x78);
    uStack_4 = 0;
    if (puVar2 != (undefined4 *)0x0) {
      uVar3 = FUN_0046d710(&DAT_005e572c);
      FUN_004bcb00();
      uStack_4 = CONCAT31(uStack_4._1_3_,1);
      *puVar2 = &PTR_FUN_005a3980;
      puVar2[0x1a] = 0;
      FUN_004a5ca0(uVar3,0x200);
      puVar2[0x1c] = 1;
      goto LAB_00549f52;
    }
  }
  puVar2 = (undefined4 *)0x0;
LAB_00549f52:
  pvStack_30 = (void *)puVar2[1];
  uStack_2c = puVar2[2];
  uStack_4 = 0xffffffff;
  uStack_60 = 0x80000000;
  uStack_5c = 0;
  uStack_58 = 0;
  uStack_54 = 0;
  uStack_38 = 0;
  uStack_34 = 0;
  uStack_28 = 0;
  uStack_24 = 0;
  uStack_e = 0;
  uStack_10 = 0;
  uStack_4c = 0;
  uStack_50 = 0;
  uStack_3c = 0;
  uStack_40 = 0;
  uStack_44 = 0;
  uStack_48 = 0;
  uStack_14 = 0x1f;
  uStack_18 = 0;
  pvStack_20 = pvStack_30;
  uStack_1c = uStack_2c;
  (**(code **)(*(int *)param_1[0x1a] + 0x5c))(&uStack_60,puVar2,0,0);
  if (puVar2 != (undefined4 *)0x0) {
    (**(code **)*puVar2)(1);
  }
  FUN_0054a0a0();
  FUN_0054a310();
  param_1[0x25] = 0;
  param_1[0x28] = -1;
  param_1[0x27] = -1;
  param_1[0x26] = -1;
  param_1[0x2a] = 0;
  param_1[0x2d] = -1;
  param_1[0x2c] = -1;
  param_1[0x2b] = -1;
  param_1[0x29] = -1;
  (**(code **)(*param_1 + 0x2c))(0);
  ExceptionList = pvStack_20;
  return;
}


