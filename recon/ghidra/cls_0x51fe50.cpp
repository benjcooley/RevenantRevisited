// Decompiled methods and structure for class: cls_0x51fe50

/*
/OOAnalyzer/cls_0x51fe50
pack(disabled)
Structure cls_0x51fe50 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   872   dword   4   mbr_0x368   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 876 Alignment: 1

*/

// Function at 0051fe50

void __thiscall OOAnalyzer::cls_0x51fe50::meth_0x51fe50(cls_0x51fe50 *this)

{
  int iVar1;
  uint uVar2;
  int in_stack_00000004;
  
  iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(in_stack_00000004 + 0x22);
  if (iVar1 < 0x1e) {
    if ((in_stack_00000004 == *(int *)(&this->field_0x360 + this->mbr_0x368 * 4)) &&
       (iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(in_stack_00000004 + 0x22), 0x1c < iVar1)) {
      return;
    }
    uVar2 = (uint)(this->mbr_0x368 == 0);
    this->mbr_0x368 = uVar2;
    *(int *)(&this->field_0x360 + uVar2 * 4) = in_stack_00000004;
    (**(code **)(this->mbr_0x0 + 0xd0))(s_trainstat_005e2bc4);
  }
  return;
}



