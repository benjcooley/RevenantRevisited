// Decompiled methods and structure for class: cls_0x5abe4c

/*
/OOAnalyzer/cls_0x5abe4c
pack(disabled)
Structure cls_0x5abe4c {
   0   cls_0x5abe4c::vftable_5abe4c *   4   vftptr_0x0   "pointer to cls_0x5abe4c::vftable_5abe4c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   352   dword   4   mbr_0x160   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
   360   dword   4   mbr_0x168   "Unsigned Double-Word (ddw, 4-bytes)"
   364   dword   4   mbr_0x16c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 392 Alignment: 1

*/

// Function at 004efcc0

void __thiscall
OOAnalyzer::cls_0x5abe4c::virt_meth_0x4efcc0
          (cls_0x5abe4c *this,int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  dword dVar2;
  int iVar3;
  float *pfVar4;
  int *piVar5;
  int iVar6;
  undefined4 in_stack_00000014;
  
  dVar2 = (&this->mbr_0x160)[param_1];
  if (dVar2 != 0) {
    iVar6 = -1;
    iVar3 = 0;
    piVar5 = (int *)(dVar2 + 0x10);
    do {
      if (*piVar5 < 1) {
        iVar6 = iVar3;
      }
      iVar3 = iVar3 + 1;
      piVar5 = piVar5 + 5;
    } while (iVar3 < 0x32);
    if (-1 < iVar6) {
      puVar1 = (undefined4 *)(dVar2 + iVar6 * 0x14);
      *puVar1 = param_2;
      puVar1[1] = param_3;
      puVar1[2] = param_4;
      puVar1[3] = in_stack_00000014;
      puVar1[4] = 0x32;
    }
    iVar6 = 0x32;
    pfVar4 = (float *)(dVar2 + 0xc);
    do {
      iVar6 = iVar6 + -1;
      pfVar4[1] = (float)((int)pfVar4[1] + -1);
      *pfVar4 = *pfVar4 - *(float *)(&this->field_0x100 + param_1 * 4);
      pfVar4 = pfVar4 + 5;
    } while (iVar6 != 0);
  }
  return;
}



// Function at 004efd50

void __thiscall OOAnalyzer::cls_0x5abe4c::virt_meth_0x4efd50(cls_0x5abe4c *this)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  dword *pdVar6;
  int iVar7;
  
  cls_0x5a7e38::virt_meth_0x40e2e0((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  iVar3 = 4;
  iVar5 = 0;
  this->mbr_0xfc = this->mbr_0xfc + 1;
  iVar4 = iVar3;
  do {
    do {
      iVar7 = iVar5;
      (*this->vftptr_0x0->virt_meth_0x4efa90_100)(this);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    iVar5 = iVar5 + 1;
    iVar3 = iVar4 + -1;
    iVar4 = iVar3;
  } while (0 < iVar3);
  if (100 < (int)this->mbr_0xfc) {
    pdVar6 = &this->mbr_0x160;
    iVar4 = 4;
    do {
      FUN_004830f0((LPCVOID)*pdVar6);
      *pdVar6 = 0;
      pdVar6 = pdVar6 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    piVar1 = (int *)this->mbr_0x4;
    iVar4 = piVar1[0x36];
    if ((((iVar4 != 0) && (*(int *)(iVar4 + 0x11c) != 0)) &&
        (*(int *)(*(int *)(iVar4 + 0x11c) + 200) != 0)) &&
       ((0 < *(int *)(iVar4 + 0xc) && (*(int *)(iVar4 + 0x10) != 0)))) {
      if (*(int *)(iVar4 + 0xc) < 1) {
        piVar2 = (int *)0x0;
      }
      else {
        piVar2 = *(int **)(iVar4 + 0x10);
      }
      (**(code **)(*piVar2 + 0x1b4))(0,iVar7);
    }
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x1000);
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x8000);
    if ((piVar1[0x36] != 0) && (iVar4 = (**(code **)(*piVar1 + 0x3c))(), iVar4 != 0)) {
      (**(code **)(*(int *)piVar1[0x36] + 0xc))();
    }
  }
  return;
}



// Function at 004f8790

cls_0x5abe4c * __thiscall OOAnalyzer::cls_0x5abe4c::virt_meth_0x4f8790(cls_0x5abe4c *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f87c0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5abe4c::virt_meth_0x4f87c0(cls_0x5abe4c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fa5e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x170);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5abe4c__vftable_5abe4c_005abe4c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8860

cls_0x5abe4c * __thiscall OOAnalyzer::cls_0x5abe4c::virt_meth_0x4f8860(cls_0x5abe4c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5abe4c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8880

cls_0x5abe4c * __thiscall OOAnalyzer::cls_0x5abe4c::~cls_0x5abe4c(cls_0x5abe4c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fa78;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5abe4c__vftable_5abe4c_005abe4c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5abe4c *)pcVar1;
}



