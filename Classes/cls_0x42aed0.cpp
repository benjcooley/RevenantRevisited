// Decompiled methods and structure for class: cls_0x42aed0

/*
/OOAnalyzer/cls_0x42aed0
pack(disabled)
Structure cls_0x42aed0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 44 Alignment: 1

*/

// Function at 0042aed0

undefined4 __thiscall
OOAnalyzer::cls_0x42aed0::meth_0x42aed0(cls_0x42aed0 *this,cls_0x42aed0 *param_1,int *param_2)

{
  undefined4 uVar1;
  dword dVar2;
  int iVar3;
  cls_0x42aed0 *pcVar4;
  dword *pdVar5;
  cls_0x42aed0 *pcVar6;
  dword *pdVar7;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42aed0 *)0x0) {
    pcVar4 = this;
    for (iVar3 = 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
      pcVar4->mbr_0x0 = 0;
      pcVar4 = (cls_0x42aed0 *)&pcVar4->mbr_0x4;
    }
    this->mbr_0x28 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar4 = param_1;
    pcVar6 = this;
    for (iVar3 = 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
      pcVar6->mbr_0x0 = pcVar4->mbr_0x0;
      pcVar4 = (cls_0x42aed0 *)&pcVar4->mbr_0x4;
      pcVar6 = (cls_0x42aed0 *)&pcVar6->mbr_0x4;
    }
  }
  iVar3 = FUN_00479700(s_BITMAP_005cd124,0);
  if (iVar3 == 0) {
    if (this->mbr_0x0 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x98))(s_BITMAP_tag_expected_005cd13c,0);
      return uVar1;
    }
  }
  else {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(s_BITMAP_005cd134);
      return uVar1;
    }
    dVar2 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
    this->mbr_0x0 = dVar2;
  }
  iVar3 = FUN_00479700(s_FRAME_005cd150,0);
  if (iVar3 != 0) {
    FUN_00479580();
    if (param_1 == (cls_0x42aed0 *)0x0) {
      pdVar5 = (dword *)0x0;
    }
    else {
      pdVar5 = &this->mbr_0x4;
    }
    pdVar7 = &this->mbr_0x4;
    if (pdVar5 == (dword *)0x0) {
      for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
        *pdVar7 = 0;
        pdVar7 = pdVar7 + 1;
      }
    }
    else if (pdVar7 != pdVar5) {
      for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
        *pdVar7 = *pdVar5;
        pdVar5 = pdVar5 + 1;
        pdVar7 = pdVar7 + 1;
      }
    }
    iVar3 = FUN_0047a410();
    if ((iVar3 == 0) &&
       (iVar3 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar3 == 0))
    {
      return 0;
    }
  }
  iVar3 = FUN_00479700(s_DRAWMODE_005cd158,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd168);
      return uVar1;
    }
  }
  return 1;
}



