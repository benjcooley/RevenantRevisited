// Decompiled methods and structure for class: cls_0x4348b0

/*
/OOAnalyzer/cls_0x4348b0
pack(disabled)
Structure cls_0x4348b0 {
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 200 Alignment: 1

*/

// Function at 004348b0

void __thiscall OOAnalyzer::cls_0x4348b0::meth_0x4348b0(cls_0x4348b0 *this)

{
  int *piVar1;
  dword dVar2;
  uint uVar3;
  
  if (((((this->mbr_0x14 & 4) == 0) && (this->mbr_0x8 != 0)) && ((this->mbr_0x14 & 2) == 0)) &&
     (0 < (int)this->mbr_0xbc)) {
    (**(code **)(*(int *)this->mbr_0xc4 + 0x1c))(((int *)this->mbr_0xc4)[5] & 0xfffffffd);
    piVar1 = (int *)this->mbr_0xc4;
    dVar2 = this->mbr_0x8;
    uVar3 = piVar1[5];
    *(int **)(dVar2 + 0xa8) = piVar1;
    *(undefined4 *)(dVar2 + 0xac) = 1;
    (**(code **)(*piVar1 + 0x1c))(uVar3 | 0x20);
    this->mbr_0x94 = 1;
  }
  return;
}



