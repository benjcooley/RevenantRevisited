// FUN_00420710 @ 00420710 size=226

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00420710(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = param_2;
  iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_3,param_4);
  FUN_00479580();
  iVar2 = FUN_0047a410(iVar3,&DAT_005caed4,&param_4);
  if (iVar2 == 0) {
    return 4;
  }
  iVar3 = FUN_0047a410(iVar3,&DAT_005caed8,&param_2);
  if (iVar3 == 0) {
    param_2 = 0;
  }
  if (iVar1 == 0) {
    FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005caedc);
    return 4;
  }
  iVar3 = (uint)*(byte *)(iVar1 + 0x36) + param_2;
  iVar2 = *(int *)(iVar1 + 0x10);
  fcos((float10)iVar3 * (float10)_DAT_005a3a90);
  param_3 = iVar3;
  iVar4 = __ftol();
  param_3 = iVar3 + 0x7f;
  iVar3 = *(int *)(iVar1 + 0x14);
  fsin((float10)param_3 * (float10)_DAT_005a3a90);
  iVar1 = __ftol();
  FUN_004cedb0(iVar2 + iVar1,iVar4 + iVar3,0);
  return 1;
}


