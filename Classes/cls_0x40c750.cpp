// Decompiled methods and structure for class: cls_0x40c750

/*
/OOAnalyzer/cls_0x40c750
pack(disabled)
Structure cls_0x40c750 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 100 Alignment: 1

*/

// Function at 0040c750

void __thiscall OOAnalyzer::cls_0x40c750::meth_0x40c750(cls_0x40c750 *this)

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  if ((int)this->mbr_0x4c < 1) {
    return;
  }
  do {
    if (((this->mbr_0x5c != 0) && (uVar3 < this->mbr_0x4c)) &&
       (*(int *)(this->mbr_0x5c + uVar3 * 4) != 0)) {
      dVar1 = *(dword *)(this->mbr_0x5c + uVar3 * 4);
      if (dVar1 == 0) {
        dVar1 = this->mbr_0x60;
      }
      if (*(int *)(dVar1 + 0x90) != 0) {
        dVar1 = *(dword *)(this->mbr_0x5c + uVar3 * 4);
        if (dVar1 == 0) {
          dVar1 = this->mbr_0x60;
        }
        iVar2 = (**(code **)(**(int **)(dVar1 + 0x90) + 0x60))(*(int **)(dVar1 + 0x90));
        if (iVar2 == -0x7789fe3e) {
          (**(code **)(this->mbr_0x0 + 4))();
          return;
        }
      }
    }
    uVar3 = uVar3 + 1;
    if ((int)this->mbr_0x4c <= (int)uVar3) {
      return;
    }
  } while( true );
}



