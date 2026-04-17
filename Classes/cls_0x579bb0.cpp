// Decompiled methods and structure for class: cls_0x579bb0

/*
/OOAnalyzer/cls_0x579bb0
pack(disabled)
Structure cls_0x579bb0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   312   byte   1   mbr_0x138   "Unsigned Byte (db)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 328 Alignment: 1

*/

// Function at 00579bb0

void __thiscall OOAnalyzer::cls_0x579bb0::meth_0x579bb0(cls_0x579bb0 *this)

{
  int iVar1;
  bool bVar2;
  cls_0x579bb0 *local_4;
  
  bVar2 = DAT_00676828 != 0;
  this->mbr_0x138 = 0;
  if (bVar2) {
    local_4 = this;
    if (this->mbr_0x144 != 0) {
      cls_0x5756d0::meth_0x578210((cls_0x5756d0 *)&DAT_00676738);
      return;
    }
    while ((iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 200))
                              ((int *)this->mbr_0x0,0,0,1,0,&local_4), iVar1 == 0 &&
           ((cls_0x579bb0 *)0x708 < local_4))) {
      Sleep(0x50);
      this->mbr_0x138 = 1;
    }
    cls_0x5756d0::meth_0x578b80((cls_0x5756d0 *)&DAT_00676738);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_006768d5.field_0x5b);
    cls_0x5ba0a0::virt_meth_0x57c7c0((cls_0x5ba0a0 *)&UNK_00676268.field_0x400);
    cls_0x57dd50::meth_0x57dd50((cls_0x57dd50 *)&UNK_00676e04.field_0x4);
    iVar1 = ReleaseMutex((HANDLE)this->mbr_0x12c);
    while (iVar1 != 0) {
      iVar1 = ReleaseMutex((HANDLE)this->mbr_0x12c);
    }
  }
  return;
}



