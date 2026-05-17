// FUN_0042aed0 @ 0042aed0 size=441

undefined4 __thiscall FUN_0042aed0(int *param_1,int *param_2,int *param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  undefined1 local_50 [80];
  
  if (param_2 == (int *)0x0) {
    piVar3 = param_1;
    for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
      *piVar3 = 0;
      piVar3 = piVar3 + 1;
    }
    param_1[10] = -0x80000000;
  }
  else if (param_1 != param_2) {
    piVar3 = param_2;
    piVar4 = param_1;
    for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
      *piVar4 = *piVar3;
      piVar3 = piVar3 + 1;
      piVar4 = piVar4 + 1;
    }
  }
  iVar2 = FUN_00479700(s_BITMAP_005cd124,0);
  if (iVar2 == 0) {
    if (*param_1 == 0) {
      uVar1 = (**(code **)(*param_3 + 0x98))(s_BITMAP_tag_expected_005cd13c,0);
      return uVar1;
    }
  }
  else {
    FUN_00479580();
    iVar2 = FUN_0047a410(param_4,&DAT_005cd12c,local_50);
    if (iVar2 == 0) {
      uVar1 = (**(code **)(*param_3 + 0x9c))(s_BITMAP_005cd134);
      return uVar1;
    }
    iVar2 = (**(code **)(*param_3 + 0xa0))(local_50,0xffffffff);
    *param_1 = iVar2;
  }
  iVar2 = FUN_00479700(s_FRAME_005cd150,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (param_2 == (int *)0x0) {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = param_1 + 1;
    }
    piVar4 = param_1 + 1;
    if (piVar3 == (int *)0x0) {
      piVar3 = piVar4;
      for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1) {
        *piVar3 = 0;
        piVar3 = piVar3 + 1;
      }
    }
    else if (piVar4 != piVar3) {
      piVar5 = piVar4;
      for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1) {
        *piVar5 = *piVar3;
        piVar3 = piVar3 + 1;
        piVar5 = piVar5 + 1;
      }
    }
    iVar2 = FUN_0047a410(param_4,s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24,param_1 + 2,param_1 + 3
                         ,param_1 + 4,param_1 + 5,param_1 + 6,param_1 + 7,param_1 + 8,param_1 + 9,
                         piVar4);
    if ((iVar2 == 0) &&
       (iVar2 = (**(code **)(*param_3 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar2 == 0))
    {
      return 0;
    }
  }
  iVar2 = FUN_00479700(s_DRAWMODE_005cd158,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410(param_4,&DAT_005cd164,param_1 + 10);
    if (iVar2 == 0) {
      uVar1 = (**(code **)(*param_3 + 0x9c))(s_DRAWMODE_005cd168);
      return uVar1;
    }
  }
  return 1;
}


