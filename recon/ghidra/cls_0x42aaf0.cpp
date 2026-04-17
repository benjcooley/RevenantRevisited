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
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 uVar3;
  dword dVar4;
  undefined3 extraout_var_00;
  uint *in_stack_0000000c;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42aaf0 *)0x0) {
    this->mbr_0x0 = 0;
    this->mbr_0x4 = 0;
    this->mbr_0x4 = 0x80000000;
  }
  else if (this != param_1) {
    dVar4 = param_1->mbr_0x4;
    this->mbr_0x0 = param_1->mbr_0x0;
    this->mbr_0x4 = dVar4;
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BITMAP_005cd0f0,0);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar2 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd0f8);
    if (iVar2 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_BITMAP_005cd100);
      return uVar3;
    }
    dVar4 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
    this->mbr_0x0 = dVar4;
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd108,0);
  if (CONCAT31(extraout_var_00,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar2 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd114);
    if (iVar2 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd118);
      return uVar3;
    }
  }
  return 1;
}



