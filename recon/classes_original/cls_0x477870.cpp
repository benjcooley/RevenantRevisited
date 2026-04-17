// Decompiled methods and structure for class: cls_0x477870

/*
/OOAnalyzer/cls_0x477870
pack(disabled)
Structure cls_0x477870 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 0046dfb0

dword __thiscall OOAnalyzer::cls_0x477870::meth_0x46dfb0(cls_0x477870 *this)

{
  short sVar1;
  int *piVar2;
  int iVar3;
  cls_0x5a7b98 *pcVar4;
  dword dVar5;
  int *piVar6;
  
  if (this->mbr_0x8 == 0) {
    return 0;
  }
  piVar6 = (int *)this->mbr_0x18;
  if ((piVar6 != (int *)0x0) && ((*(byte *)&this->mbr_0x0 & 1) != 0)) {
    iVar3 = (**(code **)(*piVar6 + 0x170))();
    if (iVar3 == 0) {
      iVar3 = piVar6[0x1a];
    }
    else {
      pcVar4 = (cls_0x5a7b98 *)(**(code **)(*piVar6 + 0x170))();
      iVar3 = cls_0x5a7b98::meth_0x470040(pcVar4);
    }
    if ((0 < iVar3) &&
       (((*(byte *)&this->mbr_0x0 & 2) != 0 ||
        (iVar3 = (**(code **)(*(int *)this->mbr_0x18 + 0x170))(), iVar3 == 0)))) {
      iVar3 = (**(code **)(*(int *)this->mbr_0x18 + 0x170))();
      piVar2 = (int *)this->mbr_0x18;
      if (iVar3 == 0) {
        this->mbr_0x8 = (dword)piVar2;
      }
      else {
        iVar3 = *piVar2;
        this->mbr_0x10 = (dword)piVar2;
        dVar5 = (**(code **)(iVar3 + 0x170))();
        this->mbr_0xc = dVar5;
        this->mbr_0x8 = dVar5;
      }
      this->mbr_0x14 = 0;
      this->mbr_0x18 = 0;
    }
  }
  do {
    piVar2 = (int *)this->mbr_0x8;
    iVar3 = (**(code **)(*piVar2 + 0x170))();
    if (iVar3 == 0) {
      iVar3 = piVar2[0x1a];
    }
    else {
      pcVar4 = (cls_0x5a7b98 *)(**(code **)(*piVar2 + 0x170))();
      iVar3 = cls_0x5a7b98::meth_0x470040(pcVar4);
    }
    dVar5 = this->mbr_0x14;
    if ((int)dVar5 < iVar3) {
      piVar2 = (int *)this->mbr_0x8;
      this->mbr_0x14 = dVar5 + 1;
      iVar3 = (**(code **)(*piVar2 + 0x170))();
      if (iVar3 == 0) {
        dVar5 = *(dword *)(piVar2[0x1e] + dVar5 * 4);
      }
      else {
        pcVar4 = (cls_0x5a7b98 *)(**(code **)(*piVar2 + 0x170))(dVar5);
        dVar5 = cls_0x5a7b98::meth_0x470110(pcVar4);
      }
      this->mbr_0x18 = dVar5;
    }
    else {
      if ((piVar6 == (int *)0x0) || (this->mbr_0x8 == this->mbr_0x4)) {
        this->mbr_0x18 = 0;
        goto LAB_0046e0df;
      }
      piVar6 = (int *)piVar6[0x19];
      if (piVar6 == (int *)this->mbr_0xc) {
        piVar6 = (int *)this->mbr_0x10;
      }
      sVar1 = *(short *)((int)piVar6 + 0x7e);
      this->mbr_0x8 = piVar6[0x19];
      this->mbr_0x14 = (int)sVar1 + 1;
      this->mbr_0x18 = 0;
    }
    if (this->mbr_0x18 != 0) {
LAB_0046e0df:
      return this->mbr_0x18;
    }
  } while( true );
}



// Function at 00477870

cls_0x477870 * __thiscall OOAnalyzer::cls_0x477870::cls_0x477870(cls_0x477870 *this,dword param_1)

{
  dword in_stack_00000008;
  
  this->mbr_0x0 = in_stack_00000008;
  this->mbr_0x8 = param_1;
  this->mbr_0x4 = param_1;
  this->mbr_0x10 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x18 = 0;
  meth_0x46dfb0(this);
  return this;
}



