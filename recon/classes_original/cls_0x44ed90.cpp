// Decompiled methods and structure for class: cls_0x44ed90

/*
/OOAnalyzer/cls_0x44ed90
pack(disabled)
Structure cls_0x44ed90 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 128 Alignment: 1

*/

// Function at 0044ed90

void __thiscall OOAnalyzer::cls_0x44ed90::meth_0x44ed90(cls_0x44ed90 *this)

{
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined local_c [12];
  
  local_18 = *(undefined4 *)(DAT_00667fcc + 0x10);
  local_14 = *(undefined4 *)(DAT_00667fcc + 0x14);
  local_10 = *(undefined4 *)(DAT_00667fcc + 0x18);
  FUN_0046dad0((this->mbr_0x78 - this->mbr_0x4) + DAT_00668510,
               (this->mbr_0x7c - this->mbr_0x8) + DAT_00668514,local_c,
               *(int *)(DAT_00667fcc + 0x18) + 0x32);
  FUN_0046dc60_AngleToPP(&local_18,local_c);
  return;
}



