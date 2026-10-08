// FUN_004205c0 @ 004205c0 size=335

undefined4 FUN_004205c0(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_28;
  int iStack_24;
  int iStack_20;
  undefined4 uStack_1c;
  int iStack_18;
  int iStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar3 = param_2;
  iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_3,param_4);
  FUN_00479580();
  iVar2 = FUN_0047a410(iVar3,s__d__d_005cae9c,&param_2,&param_4);
  if (iVar2 == 0) {
    return 4;
  }
  if (iVar1 == 0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005caea4);
    return 4;
  }
  uStack_10 = *(undefined4 *)(iVar1 + 0x18);
  iStack_24 = *(int *)(iVar1 + 0x10);
  iStack_20 = *(int *)(iVar1 + 0x14);
  uStack_1c = *(undefined4 *)(iVar1 + 0x18);
  uStack_c = *(undefined4 *)(param_1 + 0x10);
  uStack_8 = *(undefined4 *)(param_1 + 0x14);
  uStack_4 = *(undefined4 *)(param_1 + 0x18);
  iStack_18 = *(int *)(iVar1 + 0x10) + param_2;
  iStack_14 = *(int *)(iVar1 + 0x14) + param_4;
  iVar3 = FUN_0047a410(iVar3,s__d__d_005caecc,&param_3,&iStack_28);
  if (iVar3 != 0) {
    iStack_20 = iStack_20 + iStack_28;
    iStack_24 = iStack_24 + param_3;
    iVar3 = FUN_0046de60(&uStack_c,&iStack_24);
    iVar1 = FUN_0046de60(&uStack_c,&iStack_18);
    if (iVar3 < iVar1) {
      iStack_18 = iStack_24;
      iStack_14 = iStack_20;
    }
  }
  FUN_004cedb0(iStack_18,iStack_14,0);
  return 1;
}


