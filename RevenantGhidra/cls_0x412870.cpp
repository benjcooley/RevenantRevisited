// Decompiled methods and structure for class: cls_0x412870

/*
/OOAnalyzer/cls_0x412870
pack(disabled)
Structure cls_0x412870 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00412870

void __thiscall OOAnalyzer::cls_0x412870::meth_0x412870(cls_0x412870 *this,int *param_1)

{
  int in_stack_00000008;
  float local_4c;
  float local_48;
  float local_44;
  float local_40 [16];
  
  if (this->mbr_0x0 != 0) {
    if (DAT_005d7a28 != 0) {
      FUN_0056cdc0(param_1,in_stack_00000008);
      return;
    }
    UNK_005e891c._4_4_ = *param_1;
    UNK_005e891c._8_4_ = param_1[1];
    UNK_005e891c._12_4_ = param_1[2];
    FUN_0043a9f0(local_40);
    local_4c = (float)UNK_005e891c._4_4_;
    local_48 = (float)UNK_005e891c._8_4_;
    local_44 = (float)UNK_005e891c._12_4_;
    FUN_0043b0e0(local_40,&local_4c);
    FUN_0043b230(local_40,0.7853982);
    FUN_0043b180(local_40,-2.094395);
    local_44 = (float)-in_stack_00000008;
    local_4c = 0.0;
    local_48 = 0.0;
    FUN_0043b0e0(local_40,&local_4c);
    if (DAT_005d7a28 != 0) {
      FUN_0056d5f0(2,local_40);
      return;
    }
    (**(code **)(*DAT_00668f14 + 100))(DAT_00668f14,2,local_40);
  }
  return;
}



