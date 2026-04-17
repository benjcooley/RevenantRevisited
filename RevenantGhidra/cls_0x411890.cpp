// Decompiled methods and structure for class: cls_0x411890

/*
/OOAnalyzer/cls_0x411890
pack(disabled)
Structure cls_0x411890 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 88 Alignment: 1

*/

// Function at 00411890

void __thiscall OOAnalyzer::cls_0x411890::meth_0x411890(cls_0x411890 *this)

{
  int *piVar1;
  
  if (this->mbr_0x54 != 0) {
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,this->mbr_0x0);
    }
    else {
      FUN_0056d120((int)&this->mbr_0x4);
    }
    this->mbr_0x54 = 0;
  }
  piVar1 = (int *)this->mbr_0x0;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 8))(piVar1);
    this->mbr_0x0 = 0;
  }
  return;
}



