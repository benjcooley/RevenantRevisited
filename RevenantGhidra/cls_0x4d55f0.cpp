// Decompiled methods and structure for class: cls_0x4d55f0

/*
/OOAnalyzer/cls_0x4d55f0
pack(disabled)
Structure cls_0x4d55f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 284 Alignment: 1

*/

// Function at 004d55f0

void __thiscall OOAnalyzer::cls_0x4d55f0::meth_0x4d55f0(cls_0x4d55f0 *this)

{
  int iVar1;
  cls_0x40b770 *this_00;
  int iVar2;
  
  iVar2 = 0;
  this->mbr_0x118 = 0;
  iVar1 = (**(code **)(this->mbr_0x0 + 0x24))();
  this_00 = *(cls_0x40b770 **)(iVar1 + 8);
  if (0 < (int)this_00->mbr_0x34) {
    do {
      cls_0x40b770::meth_0x40b770(this_00,iVar2);
      cls_0x40b770::meth_0x40b830(*(cls_0x40b770 **)(iVar1 + 8),iVar2);
      this_00 = *(cls_0x40b770 **)(iVar1 + 8);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this_00->mbr_0x34);
  }
  return;
}



// Function at 004d5650

void __thiscall OOAnalyzer::cls_0x4d55f0::meth_0x4d5650(cls_0x4d55f0 *this)

{
  int iVar1;
  cls_0x40b770 *this_00;
  int iVar2;
  
  this->mbr_0x118 = 1;
  iVar1 = (**(code **)(this->mbr_0x0 + 0x24))();
  iVar2 = 0;
  this_00 = *(cls_0x40b770 **)(iVar1 + 8);
  if (0 < (int)this_00->mbr_0x34) {
    do {
      cls_0x40b770::meth_0x40b770(this_00,iVar2);
      cls_0x40b770::meth_0x40b830(*(cls_0x40b770 **)(iVar1 + 8),iVar2);
      this_00 = *(cls_0x40b770 **)(iVar1 + 8);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this_00->mbr_0x34);
  }
  return;
}



