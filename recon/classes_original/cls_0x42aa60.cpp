// Decompiled methods and structure for class: cls_0x42aa60

/*
/OOAnalyzer/cls_0x42aa60
pack(disabled)
Structure cls_0x42aa60 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 96 Alignment: 1

*/

// Function at 0042aa60

undefined4 __thiscall OOAnalyzer::cls_0x42aa60::meth_0x42aa60(cls_0x42aa60 *this)

{
  undefined4 uVar1;
  
  if (this->mbr_0x5c != 0) {
    uVar1 = FUN_0058b42c(this->mbr_0x5c);
    return uVar1;
  }
  return 0;
}



// Function at 0042aa80

void __thiscall OOAnalyzer::cls_0x42aa60::meth_0x42aa80(cls_0x42aa60 *this)

{
  undefined local_14 [20];
  
  FUN_0058d252();
  (**(code **)(this->mbr_0x0 + 0x18))(local_14);
  return;
}



