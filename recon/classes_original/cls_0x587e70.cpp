// Decompiled methods and structure for class: cls_0x587e70

/*
/OOAnalyzer/cls_0x587e70
pack(disabled)
Structure cls_0x587e70 {
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 00587e70

undefined4 __thiscall OOAnalyzer::cls_0x587e70::meth_0x587e70(cls_0x587e70 *this,uint param_1)

{
  uint uVar1;
  dword dVar2;
  
  uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&this->mbr_0x14);
  if (param_1 < this->mbr_0x24) {
    dVar2 = *(dword *)(this->mbr_0x34 + param_1 * 4);
    if (dVar2 == 0) {
      dVar2 = this->mbr_0x38;
    }
    if (uVar1 < (uint)(int)*(short *)(dVar2 + 0xc)) {
      dVar2 = *(dword *)(this->mbr_0x34 + param_1 * 4);
      if (dVar2 == 0) {
        dVar2 = this->mbr_0x38;
      }
      return *(undefined4 *)(*(int *)(dVar2 + 0x10) + uVar1 * 4);
    }
  }
  return 0;
}



