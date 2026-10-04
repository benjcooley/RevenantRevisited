// FUN_00428ab0 @ 00428ab0 size=100

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00428ab0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  bool bVar3;
  int local_4;
  
  local_4 = 0;
  iVar1 = FUN_0047a410(param_2,&DAT_005ccbc8,&local_4);
  if (iVar1 == 0) {
    return 4;
  }
  uVar2 = (uint)(local_4 != 0);
  bVar3 = DAT_00668120 != uVar2;
  DAT_00668120 = uVar2;
  if (bVar3) {
    _DAT_00656ec8 = 1;
    (**(code **)(DAT_00656e78 + 0x90))();
  }
  return 0;
}


