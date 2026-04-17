// Decompiled methods and structure for class: cls_0x57dd50

/*
/OOAnalyzer/cls_0x57dd50
pack(disabled)
Structure cls_0x57dd50 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   85   byte   1   mbr_0x55   "Unsigned Byte (db)"
   124   byte   1   mbr_0x7c   "Unsigned Byte (db)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 160 Alignment: 1

*/

// Function at 0057dd50

void __thiscall OOAnalyzer::cls_0x57dd50::meth_0x57dd50(cls_0x57dd50 *this)

{
  dword **ppdVar1;
  dword *pdVar2;
  dword **ppdVar3;
  int **ppiVar4;
  dword dVar5;
  
  dVar5 = cls_0x5a5320::meth_0x47e940((cls_0x5a5320 *)&DAT_0065caf0);
  if ((int)dVar5 < (int)this->mbr_0x98) {
    while ((*(int *)this->mbr_0x8c != 0 &&
           (ppdVar1 = (dword **)((int *)this->mbr_0x8c)[2], ppdVar1 != (dword **)0x0))) {
      pdVar2 = *ppdVar1;
      if (pdVar2 != (dword *)0x0) {
        ppdVar3 = (dword **)ppdVar1[1];
        pdVar2[1] = (dword)ppdVar3;
        *ppdVar3 = pdVar2;
        *ppdVar1 = (dword *)0x0;
      }
      *ppdVar1 = &this->mbr_0x84;
      ppiVar4 = (int **)this->mbr_0x88;
      ppdVar1[1] = (dword *)ppiVar4;
      this->mbr_0x88 = (dword)ppdVar1;
      *ppiVar4 = (int *)ppdVar1;
    }
  }
  else {
    while (((*(int *)this->mbr_0x8c != 0 &&
            (ppdVar1 = (dword **)((int *)this->mbr_0x8c)[2], ppdVar1 != (dword **)0x0)) &&
           ((int)ppdVar1[5] < (int)dVar5))) {
      pdVar2 = *ppdVar1;
      if (pdVar2 != (dword *)0x0) {
        ppdVar3 = (dword **)ppdVar1[1];
        pdVar2[1] = (dword)ppdVar3;
        *ppdVar3 = pdVar2;
        *ppdVar1 = (dword *)0x0;
      }
      *ppdVar1 = &this->mbr_0x84;
      ppiVar4 = (int **)this->mbr_0x88;
      ppdVar1[1] = (dword *)ppiVar4;
      this->mbr_0x88 = (dword)ppdVar1;
      *ppiVar4 = (int *)ppdVar1;
    }
  }
  this->mbr_0x98 = dVar5;
  this->mbr_0x9c = 0;
  this->mbr_0x55 = 0;
  this->mbr_0x7c = 0;
  (**(code **)(this->mbr_0x0 + 8))();
  return;
}



