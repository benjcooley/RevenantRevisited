#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x42aed0



// Function at 0042aed0

undefined4 cls_0x42aed0::meth_0x42aed0(cls_0x42aed0 *param_1, int *param_2)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined4 uVar2;
  dword dVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar4;
  cls_0x42aed0 *pcVar5;
  dword *pdVar6;
  cls_0x42aed0 *pcVar7;
  dword *pdVar8;
  uint *in_stack_0000000c;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42aed0 *)0x0) {
    pcVar5 = this;
    for (iVar4 = 0xb; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar5->mbr_0x0 = 0;
      pcVar5 = (cls_0x42aed0 *)&pcVar5->mbr_0x4;
    }
    this->mbr_0x28 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar5 = param_1;
    pcVar7 = this;
    for (iVar4 = 0xb; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar7->mbr_0x0 = pcVar5->mbr_0x0;
      pcVar5 = (cls_0x42aed0 *)&pcVar5->mbr_0x4;
      pcVar7 = (cls_0x42aed0 *)&pcVar7->mbr_0x4;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BITMAP_005cd124,0);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    if (this->mbr_0x0 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x98))(s_BITMAP_tag_expected_005cd13c,0);
      return uVar2;
    }
  }
  else {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd12c);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_BITMAP_005cd134);
      return uVar2;
    }
    dVar3 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
    this->mbr_0x0 = dVar3;
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd150,0);
  if (CONCAT31(extraout_var_00,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if (param_1 == (cls_0x42aed0 *)0x0) {
      pdVar6 = (dword *)0x0;
    }
    else {
      pdVar6 = &this->mbr_0x4;
    }
    pdVar8 = &this->mbr_0x4;
    if (pdVar6 == (dword *)0x0) {
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *pdVar8 = 0;
        pdVar8 = pdVar8 + 1;
      }
    }
    else if (pdVar8 != pdVar6) {
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *pdVar8 = *pdVar6;
        pdVar6 = pdVar6 + 1;
        pdVar8 = pdVar8 + 1;
      }
    }
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar4 == 0) &&
       (iVar4 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar4 == 0))
    {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd158,0);
  if (CONCAT31(extraout_var_01,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd164);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd168);
      return uVar2;
    }
  }
  return 1;
}



