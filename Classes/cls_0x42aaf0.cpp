// Decompiled methods and structure for class: cls_0x42aaf0

/*
/OOAnalyzer/cls_0x42aaf0
pack(disabled)
Structure cls_0x42aaf0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0042aaf0

undefined4 __thiscall
OOAnalyzer::cls_0x42aaf0::meth_0x42aaf0(cls_0x42aaf0 *this,cls_0x42aaf0 *param_1,int *param_2)

{
  int iVar1;
  undefined4 uVar2;
  dword dVar3;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42aaf0 *)0x0) {
    this->mbr_0x0 = 0;
    this->mbr_0x4 = 0;
    this->mbr_0x4 = 0x80000000;
  }
  else if (this != param_1) {
    dVar3 = param_1->mbr_0x4;
    this->mbr_0x0 = param_1->mbr_0x0;
    this->mbr_0x4 = dVar3;
  }
  iVar1 = FUN_00479700(s_BITMAP_005cd0f0,0);
  if (iVar1 != 0) {
    FUN_00479580();
    iVar1 = FUN_0047a410();
    if (iVar1 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_BITMAP_005cd100);
      return uVar2;
    }
    dVar3 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
    this->mbr_0x0 = dVar3;
  }
  iVar1 = FUN_00479700(s_DRAWMODE_005cd108,0);
  if (iVar1 != 0) {
    FUN_00479580();
    iVar1 = FUN_0047a410();
    if (iVar1 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd118);
      return uVar2;
    }
  }
  return 1;
}



