// Decompiled methods and structure for class: cls_0x5a5c60

/*
/OOAnalyzer/cls_0x5a5c60
pack(disabled)
Structure cls_0x5a5c60 {
   0   cls_0x5a5c60::vftable_5a5c60 *   4   vftptr_0x0   "pointer to cls_0x5a5c60::vftable_5a5c60"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   472   dword   4   mbr_0x1d8   "Unsigned Double-Word (ddw, 4-bytes)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   484   dword   4   mbr_0x1e4   "Unsigned Double-Word (ddw, 4-bytes)"
   488   dword   4   mbr_0x1e8   "Unsigned Double-Word (ddw, 4-bytes)"
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 496 Alignment: 1

*/

// Function at 00488cf0

cls_0x5a5c60 * __thiscall OOAnalyzer::cls_0x5a5c60::cls_0x5a5c60(cls_0x5a5c60 *this)

{
  dword dVar1;
  dword dVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  dVar2 = DAT_00667c30;
  dVar1 = DAT_0065c5c4;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d758;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0xc = DAT_00667c30;
  this->mbr_0x1c = dVar2;
  this->vftptr_0x0 = (cls_0x5a5c60__vftable_5a5c60 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = dVar1;
  this->mbr_0x20 = dVar1;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = (cls_0x5a5c60__vftable_5a5c60 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x17c,4);
  this->mbr_0x194 = 0;
  this->vftptr_0x0 = &cls_0x5a5c60__vftable_5a5c60_005a5c60;
  ExceptionList = local_c;
  return this;
}



// Function at 00488d90

cls_0x5a5c60 * __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x488d90(cls_0x5a5c60 *this)

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5c60(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5c60 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00488dd0

cls_0x5a5c60 * __thiscall OOAnalyzer::cls_0x5a5c60::~cls_0x5a5c60(cls_0x5a5c60 *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5c60 *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d77e;
  local_c = ExceptionList;
  pvVar1 = *(LPCVOID *)&this->field_0x18c;
  ExceptionList = &local_c;
  if (pvVar1 != (LPCVOID)0x0) {
    ExceptionList = &local_c;
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  this->vftptr_0x0 = (cls_0x5a5c60__vftable_5a5c60 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5c60 *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00534fd0

undefined4 __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x534fd0(cls_0x5a5c60 *this)

{
  int iVar1;
  dword *pdVar2;
  uint uVar3;
  undefined4 *puVar4;
  
  if (this->mbr_0x40 != 0) {
    return 1;
  }
  iVar1 = cls_0x5a4494::virt_meth_0x434e40((cls_0x5a4494 *)this);
  if (iVar1 == 0) {
    return 0;
  }
  this->mbr_0x1dc = 0xffffffff;
  this->mbr_0x1e4 = 0;
  pdVar2 = &this->mbr_0x198;
  iVar1 = 8;
  do {
    pdVar2[8] = 0;
    *pdVar2 = 0;
    pdVar2 = pdVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  if (*(undefined4 **)&this->field_0x18c != (undefined4 *)0x0) {
    puVar4 = *(undefined4 **)&this->field_0x18c;
    for (uVar3 = *(uint *)&this->field_0x184 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  *(undefined4 *)&this->field_0x180 = 0;
  *(undefined4 *)&this->field_0x17c = 0;
  *(undefined4 *)&this->field_0x190 = 0;
  this->mbr_0x60 = this->mbr_0x60 | 2;
  return 1;
}



// Function at 00535060

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535060(cls_0x5a5c60 *this)

{
  undefined *this_00;
  cls_0x4367d0 **ppcVar1;
  dword *pdVar2;
  int iVar3;
  
  if (this->mbr_0x40 != 0) {
    iVar3 = 0;
    if (0 < (int)this->mbr_0x1d8) {
      pdVar2 = &this->mbr_0x1b8;
      do {
        if ((LPCVOID)pdVar2[-8] != (LPCVOID)0x0) {
          FUN_00482f80((LPCVOID)pdVar2[-8]);
        }
        if ((LPCVOID)*pdVar2 != (LPCVOID)0x0) {
          FUN_00482f80((LPCVOID)*pdVar2);
        }
        iVar3 = iVar3 + 1;
        pdVar2 = pdVar2 + 1;
      } while (iVar3 < (int)this->mbr_0x1d8);
    }
    this_00 = &this->field_0x17c;
    iVar3 = 0;
    this->mbr_0x1d8 = 0;
    this->mbr_0x1dc = 0xffffffff;
    this->mbr_0x1e4 = 0;
    if (0 < *(int *)&this->field_0x17c) {
      do {
        if ((-1 < iVar3) &&
           (ppcVar1 = *(cls_0x4367d0 ***)(*(int *)&this->field_0x18c + iVar3 * 4),
           ppcVar1 != (cls_0x4367d0 **)0x0)) {
          FUN_005343e0(ppcVar1);
          FUN_004830f0(ppcVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)this_00);
    }
    *(undefined4 *)this_00 = 0;
    *(undefined4 *)&this->field_0x180 = 0;
    *(undefined4 *)&this->field_0x190 = 0;
    cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  }
  return;
}



// Function at 00535120

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535120(cls_0x5a5c60 *this)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  dword *pdVar4;
  
  this->mbr_0x48 = 1;
  this->mbr_0x4c = 1;
  if (*(undefined4 **)&this->field_0x18c != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)&this->field_0x18c;
    for (uVar1 = *(uint *)&this->field_0x184 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  *(undefined4 *)&this->field_0x180 = 0;
  *(undefined4 *)&this->field_0x17c = 0;
  *(undefined4 *)&this->field_0x190 = 0;
  if (this->mbr_0x40 != 0) {
    iVar2 = 0;
    if (0 < (int)this->mbr_0x1d8) {
      pdVar4 = &this->mbr_0x1b8;
      do {
        if ((LPCVOID)pdVar4[-8] != (LPCVOID)0x0) {
          FUN_00482f80((LPCVOID)pdVar4[-8]);
        }
        if ((LPCVOID)*pdVar4 != (LPCVOID)0x0) {
          FUN_00482f80((LPCVOID)*pdVar4);
        }
        iVar2 = iVar2 + 1;
        pdVar4 = pdVar4 + 1;
      } while (iVar2 < (int)this->mbr_0x1d8);
    }
    this->mbr_0x1d8 = 0;
    this->mbr_0x1dc = 0xffffffff;
    this->mbr_0x1e4 = 0;
  }
  return;
}



// Function at 00535500

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535500(cls_0x5a5c60 *this)

{
  int iVar1;
  
  if (this->mbr_0x50 != 0) {
    iVar1 = 0;
    if (0 < *(int *)&this->field_0x17c) {
      do {
        FUN_00534470(*(int *)(*(int *)&this->field_0x18c + iVar1 * 4));
        iVar1 = iVar1 + 1;
      } while (iVar1 < *(int *)&this->field_0x17c);
    }
    cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return;
  }
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  return;
}



// Function at 00535550

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535550(cls_0x5a5c60 *this)

{
  uint uVar1;
  
  uVar1 = 0;
  if (0 < *(int *)&this->field_0x17c) {
    do {
      if (((*(int *)&this->field_0x18c != 0) && (uVar1 < *(uint *)&this->field_0x17c)) &&
         (*(int *)(*(int *)&this->field_0x18c + uVar1 * 4) != 0)) {
        FUN_00534b60(*(int *)(*(int *)&this->field_0x18c + uVar1 * 4));
      }
      uVar1 = uVar1 + 1;
    } while ((int)uVar1 < *(int *)&this->field_0x17c);
  }
  cls_0x5b93c4::virt_meth_0x436010((cls_0x5b93c4 *)this);
  return;
}



// Function at 005355b0

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x5355b0(cls_0x5a5c60 *this)

{
  uint uVar1;
  
  uVar1 = 0;
  if (0 < *(int *)&this->field_0x17c) {
    do {
      if (((*(int *)&this->field_0x18c != 0) && (uVar1 < *(uint *)&this->field_0x17c)) &&
         (*(int *)(*(int *)&this->field_0x18c + uVar1 * 4) != 0)) {
        cls_0x534d40::meth_0x534d40(*(cls_0x534d40 **)(*(int *)&this->field_0x18c + uVar1 * 4));
      }
      uVar1 = uVar1 + 1;
    } while ((int)uVar1 < *(int *)&this->field_0x17c);
  }
  cls_0x5b93c4::virt_meth_0x436090((cls_0x5b93c4 *)this);
  return;
}



// Function at 00535610

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535610(cls_0x5a5c60 *this,int param_1)

{
  cls_0x4367d0 **ppcVar1;
  cls_0x4d6000 *this_00;
  int iVar2;
  cls_0x4367d0 **ppcVar3;
  int in_stack_00000008;
  uint local_4;
  
  if (in_stack_00000008 != 0) {
    if (param_1 == 0x20) {
      if (((this->mbr_0x40 != 0) && (*(int *)&this->field_0x190 == 0)) &&
         (local_4 = 0, 0 < *(int *)&this->field_0x17c)) {
        do {
          iVar2 = 0;
          if (((*(int *)&this->field_0x18c != 0) && (local_4 < *(uint *)&this->field_0x17c)) &&
             (*(int *)(*(int *)&this->field_0x18c + local_4 * 4) != 0)) {
            this_00 = (cls_0x4d6000 *)
                      FUN_00452690(*(int *)(*(int *)(*(int *)&this->field_0x18c + local_4 * 4) + 4),
                                   2);
            if (this_00 != (cls_0x4d6000 *)0x0) {
              cls_0x4d6000::meth_0x4d6000(this_00);
            }
            ppcVar1 = *(cls_0x4367d0 ***)(*(int *)&this->field_0x18c + local_4 * 4);
            ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
            ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
            if (0 < (int)ppcVar1[0x17]) {
              ppcVar3 = ppcVar1 + 0x40;
              do {
                if (*ppcVar3 != (cls_0x4367d0 *)0x0) {
                  cls_0x4367d0::meth_0x4367d0(*ppcVar1);
                  *ppcVar3 = (cls_0x4367d0 *)0x0;
                }
                iVar2 = iVar2 + 1;
                ppcVar3 = ppcVar3 + 1;
              } while (iVar2 < (int)ppcVar1[0x17]);
            }
          }
          local_4 = local_4 + 1;
        } while ((int)local_4 < *(int *)&this->field_0x17c);
      }
    }
    else if (((0x30 < param_1) && (param_1 < 0x37)) &&
            (((int)(param_1 - 0x31U) < (int)this->mbr_0x1d8 && (*(int *)&this->field_0x190 != 0))))
    {
      this->mbr_0x1dc = param_1 - 0x31U;
      this->mbr_0x1e4 = 1;
      return;
    }
  }
  cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  return;
}



// Function at 00535760

void __thiscall OOAnalyzer::cls_0x5a5c60::virt_meth_0x535760(cls_0x5a5c60 *this,int param_1)

{
  cls_0x4367d0 **ppcVar1;
  cls_0x4d6000 *this_00;
  int iVar2;
  cls_0x4367d0 **ppcVar3;
  int in_stack_00000008;
  uint local_4;
  
  if ((((in_stack_00000008 != 0) && (param_1 == 0x40a)) && (this->mbr_0x40 != 0)) &&
     ((*(int *)&this->field_0x190 == 0 && (local_4 = 0, 0 < *(int *)&this->field_0x17c)))) {
    do {
      iVar2 = 0;
      if ((*(int *)&this->field_0x18c != 0) &&
         ((local_4 < *(uint *)&this->field_0x17c &&
          (*(int *)(*(int *)&this->field_0x18c + local_4 * 4) != 0)))) {
        this_00 = (cls_0x4d6000 *)
                  FUN_00452690(*(int *)(*(int *)(*(int *)&this->field_0x18c + local_4 * 4) + 4),2);
        if (this_00 != (cls_0x4d6000 *)0x0) {
          cls_0x4d6000::meth_0x4d6000(this_00);
        }
        ppcVar1 = *(cls_0x4367d0 ***)(*(int *)&this->field_0x18c + local_4 * 4);
        ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
        ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
        if (0 < (int)ppcVar1[0x17]) {
          ppcVar3 = ppcVar1 + 0x40;
          do {
            if (*ppcVar3 != (cls_0x4367d0 *)0x0) {
              cls_0x4367d0::meth_0x4367d0(*ppcVar1);
              *ppcVar3 = (cls_0x4367d0 *)0x0;
            }
            iVar2 = iVar2 + 1;
            ppcVar3 = ppcVar3 + 1;
          } while (iVar2 < (int)ppcVar1[0x17]);
        }
      }
      local_4 = local_4 + 1;
    } while ((int)local_4 < *(int *)&this->field_0x17c);
  }
  cls_0x5b93c4::virt_meth_0x436340((cls_0x5b93c4 *)this,param_1);
  return;
}



