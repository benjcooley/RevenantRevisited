// Decompiled methods and structure for class: cls_0x50f2d0

/*
/OOAnalyzer/cls_0x50f2d0
pack(disabled)
Structure cls_0x50f2d0 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
   232   dword   4   mbr_0xe8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 240 Alignment: 1

*/

// Function at 0050f2d0

void __thiscall OOAnalyzer::cls_0x50f2d0::meth_0x50f2d0(cls_0x50f2d0 *this)

{
  dword dVar1;
  dword *in_stack_00000004;
  
  this->mbr_0xd8 = *in_stack_00000004;
  dVar1 = in_stack_00000004[2];
  this->mbr_0xdc = in_stack_00000004[1];
  this->mbr_0xe0 = dVar1;
  this->mbr_0xe4 = this->mbr_0xd8 - this->mbr_0x10;
  this->mbr_0xe8 = this->mbr_0xdc - this->mbr_0x14;
  this->mbr_0xec = this->mbr_0xe0 - this->mbr_0x18;
  cls_0x497c40::meth_0x497c40((cls_0x497c40 *)&DAT_0065def0);
  return;
}



