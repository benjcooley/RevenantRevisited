// Decompiled methods and structure for class: cls_0x49beb0

/*
/OOAnalyzer/cls_0x49beb0
pack(disabled)
Structure cls_0x49beb0 {
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 52 Alignment: 1

*/

// Function at 0049beb0

void __thiscall OOAnalyzer::cls_0x49beb0::meth_0x49beb0(cls_0x49beb0 *this,int param_1,int param_2)

{
  int in_stack_0000000c;
  
  if ((DAT_00668114 == 0) && (this->mbr_0x30 != 0)) {
    _AIL_set_3D_position_16(this->mbr_0x30,(float)param_1,(float)param_2,(float)in_stack_0000000c);
  }
  return;
}



