// Decompiled methods and structure for class: cls_0x42e5b0

/*
/OOAnalyzer/cls_0x42e5b0
pack(disabled)
Structure cls_0x42e5b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   byte   1   mbr_0x14   "Unsigned Byte (db)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 352 Alignment: 1

*/

// Function at 0042e5b0

void __thiscall OOAnalyzer::cls_0x42e5b0::meth_0x42e5b0(cls_0x42e5b0 *this)

{
  int *in_stack_00000004;
  
  if ((((this->mbr_0x14 & 2) == 0) && (this->mbr_0x8 != 0)) && (this->mbr_0x10c != 0)) {
    (**(code **)(this->mbr_0x0 + 0x44))();
    (**(code **)(*in_stack_00000004 + 0x20))
              (this->mbr_0x158,this->mbr_0x15c,*(undefined4 *)this->mbr_0x10c,
               ((undefined4 *)this->mbr_0x10c)[1]);
  }
  return;
}



