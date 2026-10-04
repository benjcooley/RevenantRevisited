// FUN_00425aa0 @ 00425aa0 size=355

undefined4 FUN_00425aa0(int param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 uStack_34;
  undefined2 uStack_2a;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  
  uVar1 = FUN_004751c0(*(undefined4 *)(param_2 + 0x28));
  if ((uVar1 < DAT_0065a258) && (iVar3 = (&DAT_0065a148)[uVar1], iVar3 != 0)) {
    FUN_00479580();
  }
  else {
    if (DAT_0065a258 <= (uint)(int)*(short *)(param_1 + 4)) goto LAB_00425bda;
    iVar3 = (&DAT_0065a148)[*(short *)(param_1 + 4)];
  }
  if (iVar3 != 0) {
    iVar2 = FUN_00475210(*(undefined4 *)(param_2 + 0x28),0);
    if (iVar2 < 0) {
      FUN_0058b100(&DAT_00654a88,s__s_has_no_object_named___s___005cbe98,*(undefined4 *)(iVar3 + 4),
                   *(undefined4 *)(param_2 + 0x28));
      FUN_0041ee50(&DAT_00654a88);
      return 0;
    }
    puVar6 = &uStack_34;
    for (iVar5 = 0xd; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
    }
    uStack_34._0_2_ = *(undefined2 *)(iVar3 + 8);
    uStack_34._2_2_ = (undefined2)iVar2;
    uStack_28 = *(undefined4 *)(param_1 + 0x10);
    uStack_2a = (undefined2)DAT_00666970;
    uStack_20 = *(undefined4 *)(param_1 + 0x18);
    uStack_24 = *(undefined4 *)(param_1 + 0x14);
    iVar3 = FUN_00450e40(&uStack_34,0xffffffff);
    if (iVar3 < 0) {
      FUN_0043f490(s_ERROR__Creating_object_005cbeb8);
      return 0;
    }
    uVar4 = *(undefined4 *)(param_1 + 0x40);
    FUN_00440840(uVar4);
    uVar4 = FUN_00452690(uVar4,0);
    FUN_00451840(uVar4);
    FUN_004405d0(iVar3,1);
    return 0;
  }
LAB_00425bda:
  FUN_0058b100(&DAT_00654a88,s_No_class_named___s___005cbed0,*(undefined4 *)(param_2 + 0x28));
  FUN_0041ee50(&DAT_00654a88);
  return 0;
}


