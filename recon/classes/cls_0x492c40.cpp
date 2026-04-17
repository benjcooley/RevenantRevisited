// Decompiled methods and structure for class: cls_0x492c40

/*
/OOAnalyzer/cls_0x492c40
pack(disabled)
Structure cls_0x492c40 {
   180   byte   1   mbr_0xb4   "Unsigned Byte (db)"
   181   byte   1   mbr_0xb5   "Unsigned Byte (db)"
   182   byte   1   mbr_0xb6   "Unsigned Byte (db)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 192 Alignment: 1

*/

// Function at 00492c40

void __thiscall OOAnalyzer::cls_0x492c40::meth_0x492c40(cls_0x492c40 *this)

{
  short sVar1;
  int *piVar2;
  byte in_stack_00000004;
  
  this->mbr_0xb6 = in_stack_00000004;
  this->mbr_0xb5 = 0;
  if ((((this->mbr_0xb4 == 3) || (this->mbr_0xb4 == 8)) &&
      (piVar2 = (int *)this->mbr_0xbc, piVar2 != (int *)0x0)) && ((piVar2[2] & 0x20000U) != 0)) {
    piVar2 = (int *)(**(code **)(*piVar2 + 0x1fc))();
    if (*piVar2 == 0x10) {
      this->mbr_0xb4 = 0;
      sVar1 = *(short *)&((cls_0x4d6000 *)this->mbr_0xbc)->field_0x4;
      if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
        cls_0x4d6000::meth_0x4d6000((cls_0x4d6000 *)this->mbr_0xbc);
      }
      this->mbr_0xbc = 0;
    }
  }
  return;
}



