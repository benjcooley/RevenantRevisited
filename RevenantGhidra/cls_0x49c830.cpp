// Decompiled methods and structure for class: cls_0x49c830

/*
/OOAnalyzer/cls_0x49c830
pack(disabled)
Structure cls_0x49c830 {
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 128 Alignment: 1

*/

// Function at 0049c830

void __thiscall OOAnalyzer::cls_0x49c830::meth_0x49c830(cls_0x49c830 *this)

{
  int iVar1;
  dword *pdVar2;
  int iVar3;
  
  pdVar2 = &this->mbr_0x7c;
  iVar3 = 0x10;
  do {
    if (pdVar2[-0x10] != 0) {
      iVar1 = _AIL_sample_status_4(pdVar2[-0x10]);
      if (iVar1 == 4) {
        _AIL_stop_sample_4(pdVar2[-0x10]);
      }
    }
    if (*pdVar2 != 0) {
      iVar1 = _AIL_3D_sample_status_4(*pdVar2);
      if (iVar1 == 4) {
        _AIL_stop_3D_sample_4(*pdVar2);
      }
    }
    pdVar2 = pdVar2 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



// Function at 0049c890

void __thiscall OOAnalyzer::cls_0x49c830::meth_0x49c890(cls_0x49c830 *this)

{
  int iVar1;
  int iVar2;
  dword *pdVar3;
  
  pdVar3 = &this->mbr_0x7c;
  iVar2 = 0x10;
  do {
    if (pdVar3[-0x10] != 0) {
      iVar1 = _AIL_sample_status_4(pdVar3[-0x10]);
      if (iVar1 == 8) {
        _AIL_resume_sample_4(pdVar3[-0x10]);
      }
    }
    if (*pdVar3 != 0) {
      iVar1 = _AIL_3D_sample_status_4(*pdVar3);
      if (iVar1 == 8) {
        _AIL_resume_3D_sample_4(*pdVar3);
      }
    }
    pdVar3 = pdVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



