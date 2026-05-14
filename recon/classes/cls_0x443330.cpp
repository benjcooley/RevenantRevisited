// Decompiled methods and structure for class: cls_0x443330

/*
/OOAnalyzer/cls_0x443330
pack(disabled)
Structure cls_0x443330 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 396 Alignment: 1

*/

// Function at 00443330

void __thiscall OOAnalyzer::cls_0x443330::meth_0x443330(cls_0x443330 *this)

{
  int iVar1;
  dword dVar2;
  undefined4 in_stack_00000004;
  
  iVar1 = FUN_00452690_TMapPane_GetInstance(in_stack_00000004,0);
  if ((iVar1 != 0) &&
     ((this->mbr_0x17c != (int)*(short *)(iVar1 + 4) ||
      (this->mbr_0x180 != (int)*(short *)(iVar1 + 6))))) {
    this->mbr_0x17c = (int)*(short *)(iVar1 + 4);
    dVar2 = (dword)*(short *)(iVar1 + 6);
    this->mbr_0x180 = dVar2;
    if ((((int)dVar2 < (int)this->mbr_0x184) ||
        ((int)this->mbr_0x184 <= (int)(dVar2 - this->mbr_0x188))) &&
       (dVar2 = dVar2 - (int)this->mbr_0x188 / 2, this->mbr_0x184 = dVar2, (int)dVar2 < 0)) {
      this->mbr_0x184 = 0;
    }
    (**(code **)(this->mbr_0x0 + 0x2c))(1);
  }
  return;
}



