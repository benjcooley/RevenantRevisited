// Decompiled methods and structure for class: cls_0x5877a0

/*
/OOAnalyzer/cls_0x5877a0
pack(disabled)
Structure cls_0x5877a0 {
   84   byte   1   mbr_0x54   "Unsigned Byte (db)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 168 Alignment: 1

*/

// Function at 005877a0

undefined4 __thiscall OOAnalyzer::cls_0x5877a0::meth_0x5877a0(cls_0x5877a0 *this)

{
  int iVar1;
  dword dVar2;
  cls_0x44ceb0 local_48;
  
  if (DAT_00676828 != 0) {
    if ((DAT_0067682c == 0) && (this->mbr_0x54 == 0)) {
      return 0;
    }
    iVar1 = cls_0x5a5320_TPlayScreen::meth_0x47e940((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
    if ((int)(this->mbr_0xa0 + 0x78) < iVar1) {
      this->mbr_0xa0 = 0;
    }
    iVar1 = cls_0x5a5320_TPlayScreen::meth_0x47e940((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
    if (iVar1 < (int)this->mbr_0xa0) {
      return 0;
    }
    if (this->mbr_0xa4 == 0xffffffff) {
      FUN_00482160(s_Multiplayer_005e6550);
      dVar2 = FUN_00482280(s_MaxMonsters_005e655c,8,0);
      this->mbr_0xa4 = dVar2;
    }
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0,2,0);
    iVar1 = 0;
    while (local_48.mbr_0xc != 0) {
      if ((*(short *)(local_48.mbr_0xc + 4) == 0xc) &&
         (iVar1 = iVar1 + 1, (int)this->mbr_0xa4 < iVar1)) {
        return 0;
      }
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
  }
  return 1;
}



// Function at 00587880

void __thiscall OOAnalyzer::cls_0x5877a0::meth_0x587880(cls_0x5877a0 *this)

{
  int iVar1;
  
  iVar1 = cls_0x5a5320_TPlayScreen::meth_0x47e940((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
  this->mbr_0xa0 = iVar1 + 0x60;
  return;
}



