// Decompiled methods and structure for class: cls_0x53f680

/*
/OOAnalyzer/cls_0x53f680
pack(disabled)
Structure cls_0x53f680 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 292 Alignment: 1

*/

// Function at 0053f680

void __thiscall OOAnalyzer::cls_0x53f680::meth_0x53f680(cls_0x53f680 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  piVar1 = (int *)this->mbr_0x4;
  iVar5 = *(int *)(this->mbr_0x120 + 0x4c);
  if ((piVar1 != (int *)0x0) && ((DAT_0066810c == 0 || (*(short *)(piVar1 + 1) != 0xb)))) {
    if (*(short *)(piVar1 + 1) == 0xb) {
      iVar2 = (**(code **)(*piVar1 + 0x3c4))();
      iVar2 = (int)((ulonglong)((longlong)(iVar2 * iVar5) * -0x51eb851f) >> 0x20);
      iVar5 = iVar5 + ((iVar2 >> 5) - (iVar2 >> 0x1f));
    }
    iVar2 = *(int *)this->mbr_0x4;
    iVar3 = (**(code **)(iVar2 + 0x1d0))();
    (**(code **)(iVar2 + 0x1d4))(iVar3 - iVar5);
    piVar1 = (int *)this->mbr_0x4;
    iVar5 = (**(code **)(*piVar1 + 0x1e8))();
    iVar2 = (**(code **)(*piVar1 + 0x1d0))();
    if (iVar5 < iVar2) {
      iVar5 = *(int *)this->mbr_0x4;
      uVar4 = (**(code **)(iVar5 + 0x1e8))();
      (**(code **)(iVar5 + 0x1d4))(uVar4);
    }
  }
  return;
}



