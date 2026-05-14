// FUN_004c3260_Move @ 004c3260 size=549

void __thiscall FUN_004c3260_Move(int *param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  void *unaff_EDI;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e321;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  iVar2 = FUN_0047e920_FrameTick();
  if ((iVar2 % 0x18 != 0) ||
     ((((param_1[0x20] != 0 && (param_1[0x38] != param_1[0x36])) || (param_1[0x4c] < 0)) ||
      (param_1[0x4d] < 0)))) {
    FUN_004cdbb0_ResetStealthValues();
  }
  if (*(int *)param_1[0x38] != 0x17) {
    iVar2 = (**(code **)(*param_1 + 0x1b8))();
    if (iVar2 != 0) {
      (**(code **)(*param_1 + 0x1bc))(0);
    }
  }
  iVar2 = (**(code **)(*param_1 + 0x21c))(param_2);
  if (iVar2 == 0) {
    iVar2 = 2;
    if (param_1[0x20] != 0) {
      iVar2 = 0;
    }
    if (param_1[0x16] == 0) {
      iVar2 = 0;
    }
    if ((*(byte *)(param_1 + 2) & 0x80) != 0) {
      iVar2 = 0;
    }
    iVar3 = param_1[0x36];
    if ((iVar3 == param_1[0x38]) && ((*(byte *)(iVar3 + 0x60) & 2) == 0)) {
      iVar2 = 0;
    }
    else if (iVar2 != 0) goto LAB_004c334b;
    if ((*(uint *)(iVar3 + 0x60) & 0x10) != 0) {
      *(uint *)(iVar3 + 0x60) = *(uint *)(iVar3 + 0x60) & 0xffffffef;
    }
  }
LAB_004c334b:
  uVar1 = *(uint *)(param_1[0x36] + 0x60);
  if ((uVar1 & 1) != 0) {
    *(uint *)(param_1[0x36] + 0x60) = uVar1 & 0xfffffffe;
  }
  if (param_1[0x21] != 0) {
    FUN_004933d0_TScript_Continue(iVar2 == 0);
  }
  if ((param_2 & 4) == 0) {
    if ((*(byte *)(param_1 + 0x44) & 2) == 0) {
      (**(code **)(*param_1 + 0x214))(param_1[0x37],param_2,0);
    }
    else {
      (**(code **)(*param_1 + 0x214))(param_1[0x37],param_2,1);
    }
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x1f0))(&DAT_005df7f4,0);
    if ((iVar2 != 0) && ((*(byte *)(param_1[0x36] + 0x60) & 0x10) == 0)) {
      puStack_8 = (undefined1 *)FUN_00482fb0(100);
      if (puStack_8 == (undefined1 *)0x0) {
        iVar2 = 0;
      }
      else {
        iVar2 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005df7fc,2);
      }
      iVar3 = (**(code **)(*param_1 + 0x218))(iVar2,0,0);
      if ((((iVar3 != 2) && (iVar2 != param_1[0x38])) && (iVar2 != param_1[0x36])) &&
         ((iVar2 != param_1[0x37] && (iVar2 != 0)))) {
        if (*(int *)(iVar2 + 0x5c) != 0) {
          FUN_00482f80(*(int *)(iVar2 + 0x5c));
        }
        FUN_004830f0(iVar2);
      }
    }
  }
  iVar2 = param_1[0x36];
  if ((iVar2 != 0) && (0 < *(int *)(iVar2 + 0x28))) {
    *(int *)(iVar2 + 0x28) = *(int *)(iVar2 + 0x28) + -1;
  }
  ExceptionList = unaff_EDI;
  return;
}


