// FUN_0042aaf0 @ 0042aaf0 size=232

undefined4 __thiscall
FUN_0042aaf0(undefined4 *param_1,undefined4 *param_2,int *param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 local_50 [80];
  
  if (param_2 == (undefined4 *)0x0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[1] = 0x80000000;
  }
  else if (param_1 != param_2) {
    uVar2 = param_2[1];
    *param_1 = *param_2;
    param_1[1] = uVar2;
  }
  iVar1 = FUN_00479700(s_BITMAP_005cd0f0,0);
  if (iVar1 != 0) {
    FUN_00479580();
    iVar1 = FUN_0047a410(param_4,&DAT_005cd0f8,local_50);
    if (iVar1 == 0) {
      uVar2 = (**(code **)(*param_3 + 0x9c))(s_BITMAP_005cd100);
      return uVar2;
    }
    uVar2 = (**(code **)(*param_3 + 0xa0))(local_50,0xffffffff);
    *param_1 = uVar2;
  }
  iVar1 = FUN_00479700(s_DRAWMODE_005cd108,0);
  if (iVar1 != 0) {
    FUN_00479580();
    iVar1 = FUN_0047a410(param_4,&DAT_005cd114,param_1 + 1);
    if (iVar1 == 0) {
      uVar2 = (**(code **)(*param_3 + 0x9c))(s_DRAWMODE_005cd118);
      return uVar2;
    }
  }
  return 1;
}


