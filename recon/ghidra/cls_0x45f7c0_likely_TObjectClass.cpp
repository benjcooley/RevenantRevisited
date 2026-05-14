// Decompiled methods and structure for class: cls_0x45f7c0

/*
/OOAnalyzer/cls_0x45f7c0
pack(disabled)
Structure cls_0x45f7c0 {
   0   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   59   byte   1   mbr_0x3b   "Unsigned Byte (db)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   92   byte   1   mbr_0x5c   "Unsigned Byte (db)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 204 Alignment: 1

*/

// Function at 00410160

bool __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x410160(cls_0x45f7c0 *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x10;
  if ((dVar1 != 0) && (in_stack_00000004 < (this->cls_0x41c7f0).mbr_0x0)) {
    return *(int *)(dVar1 + in_stack_00000004 * 4) != 0;
  }
  return false;
}



// Function at 00410ed0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x410ed0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c728;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00410f60

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x410f60(cls_0x45f7c0 *this)

{
  dword dVar1;
  int in_stack_00000004;
  
  dVar1 = *(dword *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
  if (dVar1 == 0) {
    dVar1 = (this->cls_0x41c7f0).mbr_0x14;
  }
  return dVar1;
}



// Function at 00410f80

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x410f80(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c748;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00411010

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x411010(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c768;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 004110a0

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4110a0(cls_0x45f7c0 *this)

{
  dword dVar1;
  int in_stack_00000004;
  
  dVar1 = *(dword *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
  if (dVar1 == 0) {
    dVar1 = (this->cls_0x41c7f0).mbr_0x14;
  }
  return dVar1;
}



// Function at 004110c0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4110c0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c788;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        if (*(LPCVOID *)((int)pvVar1 + 8) != (LPCVOID)0x0) {
          FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 8));
        }
        if (*(LPCVOID *)((int)pvVar1 + 0xc) != (LPCVOID)0x0) {
          FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 0xc));
        }
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x14;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (pvVar1 != (LPCVOID)0x0) {
    if (*(LPCVOID *)((int)pvVar1 + 8) != (LPCVOID)0x0) {
      FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 8));
    }
    if (*(LPCVOID *)((int)pvVar1 + 0xc) != (LPCVOID)0x0) {
      FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 0xc));
    }
    FUN_004830f0(pvVar1);
  }
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00411280

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x411280(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     pvVar1 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 004112b0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4112b0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 0041c840

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x41c840(cls_0x45f7c0 *this)

{
  dword dVar1;
  uint uVar2;
  undefined4 *puVar3;
  dword dVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  bool bVar10;
  bool bVar11;
  int in_stack_00000004;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x8;
  if ((int)dVar1 <= (int)(this->cls_0x41c7f0).mbr_0x0) {
    puVar3 = FUN_00482fb0(((this->cls_0x41c7f0).mbr_0xc + dVar1) * 4);
    uVar2 = (this->cls_0x41c7f0).mbr_0x8;
    puVar8 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
    puVar9 = puVar3;
    for (uVar5 = uVar2 & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar9 = puVar9 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined *)puVar9 = *(undefined *)puVar8;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    puVar8 = puVar3 + uVar2;
    for (uVar5 = (this->cls_0x41c7f0).mbr_0xc & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined *)puVar8 = 0;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
    (this->cls_0x41c7f0).mbr_0x10 = (dword)puVar3;
    (this->cls_0x41c7f0).mbr_0x8 = (this->cls_0x41c7f0).mbr_0x8 + (this->cls_0x41c7f0).mbr_0xc;
  }
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  if ((int)(this->cls_0x41c7f0).mbr_0x4 < (int)dVar1) {
    dVar1 = (this->cls_0x41c7f0).mbr_0x0;
    piVar7 = (int *)(this->cls_0x41c7f0).mbr_0x10;
    dVar4 = 0;
    if (-1 < (int)dVar1) {
      bVar11 = false;
      bVar10 = 0 < (int)dVar1;
      do {
        if ((bVar11 == bVar10) || (*piVar7 == 0)) {
          *piVar7 = in_stack_00000004;
          if ((int)dVar4 < (int)(this->cls_0x41c7f0).mbr_0x0) {
            return dVar4;
          }
          (this->cls_0x41c7f0).mbr_0x0 = dVar4 + 1;
          (this->cls_0x41c7f0).mbr_0x4 = dVar4 + 1;
          return dVar4;
        }
        dVar1 = (this->cls_0x41c7f0).mbr_0x0;
        dVar4 = dVar4 + 1;
        piVar7 = piVar7 + 1;
        bVar11 = SBORROW4(dVar4,dVar1);
        bVar10 = (int)(dVar4 - dVar1) < 0;
      } while ((int)dVar4 <= (int)dVar1);
    }
    return 0xffffffff;
  }
  *(int *)((this->cls_0x41c7f0).mbr_0x10 + dVar1 * 4) = in_stack_00000004;
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  dVar4 = dVar1 + 1;
  (this->cls_0x41c7f0).mbr_0x0 = dVar4;
  (this->cls_0x41c7f0).mbr_0x4 = dVar4;
  return dVar1;
}



// Function at 0041c910

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x41c910(cls_0x45f7c0 *this,int param_1)

{
  dword *pdVar1;
  uint uVar2;
  undefined4 *puVar3;
  dword dVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  dword dVar8;
  undefined4 *puVar9;
  int in_stack_00000008;
  
  if (param_1 == 0) {
    dVar4 = (this->cls_0x41c7f0).mbr_0x0;
    if (in_stack_00000008 < (int)dVar4) {
      dVar8 = (this->cls_0x41c7f0).mbr_0x10;
      if (*(int *)(dVar8 + in_stack_00000008 * 4) != 0) {
        pdVar1 = &(this->cls_0x41c7f0).mbr_0x4;
        *pdVar1 = *pdVar1 - 1;
      }
      *(undefined4 *)(dVar8 + in_stack_00000008 * 4) = 0;
      if (0 < (int)dVar4) {
        while (dVar4 = (this->cls_0x41c7f0).mbr_0x0, *(int *)((dVar8 - 4) + dVar4 * 4) == 0) {
          dVar4 = dVar4 - 1;
          (this->cls_0x41c7f0).mbr_0x0 = dVar4;
          if ((int)dVar4 < 1) {
            return;
          }
        }
      }
    }
  }
  else {
    if ((int)(this->cls_0x41c7f0).mbr_0x8 <= in_stack_00000008) {
      dVar4 = (this->cls_0x41c7f0).mbr_0xc;
      do {
        puVar3 = FUN_00482fb0(((this->cls_0x41c7f0).mbr_0x8 + dVar4) * 4);
        uVar2 = (this->cls_0x41c7f0).mbr_0x8;
        puVar7 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
        puVar9 = puVar3;
        for (uVar5 = uVar2 & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar9 = *puVar7;
          puVar7 = puVar7 + 1;
          puVar9 = puVar9 + 1;
        }
        for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
          *(undefined *)puVar9 = *(undefined *)puVar7;
          puVar7 = (undefined4 *)((int)puVar7 + 1);
          puVar9 = (undefined4 *)((int)puVar9 + 1);
        }
        puVar7 = puVar3 + uVar2;
        for (uVar5 = (this->cls_0x41c7f0).mbr_0xc & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar7 = 0;
          puVar7 = puVar7 + 1;
        }
        for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
          *(undefined *)puVar7 = 0;
          puVar7 = (undefined4 *)((int)puVar7 + 1);
        }
        FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
        dVar4 = (this->cls_0x41c7f0).mbr_0xc;
        dVar8 = (this->cls_0x41c7f0).mbr_0x8 + dVar4;
        (this->cls_0x41c7f0).mbr_0x10 = (dword)puVar3;
        (this->cls_0x41c7f0).mbr_0x8 = dVar8;
      } while ((int)dVar8 <= in_stack_00000008);
    }
    dVar4 = (this->cls_0x41c7f0).mbr_0x10;
    if (*(int *)(dVar4 + in_stack_00000008 * 4) == 0) {
      pdVar1 = &(this->cls_0x41c7f0).mbr_0x4;
      *pdVar1 = *pdVar1 + 1;
    }
    *(int *)(dVar4 + in_stack_00000008 * 4) = param_1;
    if ((int)(this->cls_0x41c7f0).mbr_0x0 <= in_stack_00000008) {
      (this->cls_0x41c7f0).mbr_0x0 = in_stack_00000008 + 1;
    }
  }
  return;
}



// Function at 0041cb40

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x41cb40(cls_0x45f7c0 *this)

{
  dword *pdVar1;
  dword dVar2;
  dword dVar3;
  int in_stack_00000004;
  
  dVar3 = (this->cls_0x41c7f0).mbr_0x0;
  if (in_stack_00000004 < (int)dVar3) {
    dVar2 = (this->cls_0x41c7f0).mbr_0x10;
    if (*(int *)(dVar2 + in_stack_00000004 * 4) != 0) {
      pdVar1 = &(this->cls_0x41c7f0).mbr_0x4;
      *pdVar1 = *pdVar1 - 1;
    }
    *(undefined4 *)(dVar2 + in_stack_00000004 * 4) = 0;
    while ((0 < (int)dVar3 &&
           (dVar3 = (this->cls_0x41c7f0).mbr_0x0, *(int *)((dVar2 - 4) + dVar3 * 4) == 0))) {
      dVar3 = dVar3 - 1;
      (this->cls_0x41c7f0).mbr_0x0 = dVar3;
    }
  }
  return;
}



// Function at 0041cb80

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x41cb80(cls_0x45f7c0 *this)

{
  dword *pdVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  undefined4 *puVar5;
  int in_stack_00000004;
  
  if (in_stack_00000004 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    dVar2 = (this->cls_0x41c7f0).mbr_0x10;
    puVar5 = (undefined4 *)(dVar2 + in_stack_00000004 * 4);
    if (*(int *)(dVar2 + in_stack_00000004 * 4) != 0) {
      pdVar1 = &(this->cls_0x41c7f0).mbr_0x4;
      *pdVar1 = *pdVar1 - 1;
    }
    dVar3 = (this->cls_0x41c7f0).mbr_0x8;
    iVar4 = dVar3 - 1;
    if (in_stack_00000004 < iVar4) {
      iVar4 = iVar4 - in_stack_00000004;
      do {
        *puVar5 = puVar5[1];
        puVar5 = puVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    *(undefined4 *)((dVar2 - 4) + dVar3 * 4) = 0;
    (this->cls_0x41c7f0).mbr_0x0 = (this->cls_0x41c7f0).mbr_0x0 - 1;
  }
  return;
}



// Function at 004384d0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4384d0(cls_0x45f7c0 *this)

{
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  return;
}



// Function at 00448450

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x448450(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059cf08;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0044ce10

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x44ce10(cls_0x45f7c0 *this)

{
  dword dVar1;
  int in_stack_00000004;
  
  dVar1 = *(dword *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
  if (dVar1 == 0) {
    dVar1 = (this->cls_0x41c7f0).mbr_0x14;
  }
  return dVar1;
}



// Function at 0045f680

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x45f680(cls_0x45f7c0 *this)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  if (puVar3 != (undefined4 *)0x0) {
    for (uVar1 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    return;
  }
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  return;
}



// Function at 0045f6e0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x45f6e0(cls_0x45f7c0 *this)

{
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  return;
}



// Function at 0045f7c0

cls_0x45f7c0 * __thiscall OOAnalyzer::cls_0x45f7c0::cls_0x45f7c0(cls_0x45f7c0 *this)

{
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x1000);
  return this;
}



// Function at 00461bc0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x461bc0(cls_0x45f7c0 *this)

{
  char cVar1;
  byte bVar2;
  int *piVar3;
  dword dVar4;
  bool bVar5;
  undefined uVar6;
  HANDLE pvVar7;
  uint *puVar8;
  TPlayScreen *this_00;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  TPlayScreen **ppcVar9;
  uint uVar10;
  uint uVar11;
  TPlayScreen *pcVar12;
  int iVar13;
  uint *puVar14;
  undefined4 unaff_EDI;
  char *pcVar15;
  char *pcVar16;
  uint *puVar17;
  int in_stack_00000004;
  undefined4 local_32c;
  uint local_328 [64];
  char acStack_226 [2];
  uint local_224 [5];
  char local_210 [528];
  
  iVar13 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      piVar3 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + iVar13 * 4);
      if (*piVar3 != DAT_00659c60) {
        if ((-1 < iVar13) && (piVar3 != (int *)0x0)) {
          FUN_0046cec0(piVar3,1);
        }
        meth_0x41cb40(this);
      }
      iVar13 = iVar13 + 1;
    } while (iVar13 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  meth_0x45f680(this);
  bVar2 = *(byte *)&(this->cls_0x41c7f0).mbr_0x14;
  dVar4 = this->mbr_0x18;
  this->mbr_0x18 = 0xffffffff;
  this->mbr_0x1c = 0;
  if ((bVar2 & 2) != 0) {
    meth_0x462000(this,dVar4);
  }
  if ((*(byte *)&(this->cls_0x41c7f0).mbr_0x14 & 1) != 0) {
    FUN_00483120((char *)&DAT_0065da00,(char *)local_328,0x104);
    uVar10 = 0xffffffff;
    puVar8 = local_328;
    do {
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      cVar1 = *(char *)puVar8;
      puVar8 = (uint *)((int)puVar8 + 1);
    } while (cVar1 != '\0');
    iVar13 = -(~uVar10 - 1);
    pcVar15 = (char *)((int)local_328 + (~uVar10 - 1));
    _strncpy(pcVar15,s_Chars__005d2af0,iVar13 + 0x103);
    pcVar15[iVar13 + 0x103] = '\0';
    uVar10 = 0xffffffff;
    puVar8 = local_328;
    do {
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      cVar1 = *(char *)puVar8;
      puVar8 = (uint *)((int)puVar8 + 1);
    } while (cVar1 != '\0');
    iVar13 = -(~uVar10 - 1);
    pcVar15 = (char *)((int)local_328 + (~uVar10 - 1));
    _strncpy(pcVar15,s___CHR_005d2af8,iVar13 + 0x103);
    pcVar15[iVar13 + 0x103] = '\0';
    pvVar7 = FUN_004a19d0((LPCSTR)local_328,(uint *)(acStack_226 + 2));
    bVar5 = true;
    local_32c = pvVar7;
    puVar8 = FUN_0058ade0(local_328,'*');
    if (pvVar7 != (HANDLE)0xffffffff) {
      while (bVar5) {
        uVar10 = 0xffffffff;
        pcVar15 = local_210;
        do {
          pcVar16 = pcVar15;
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          pcVar16 = pcVar15 + 1;
          cVar1 = *pcVar15;
          pcVar15 = pcVar16;
        } while (cVar1 != '\0');
        uVar10 = ~uVar10;
        puVar14 = (uint *)(pcVar16 + -uVar10);
        puVar17 = puVar8;
        for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *puVar17 = *puVar14;
          puVar14 = puVar14 + 1;
          puVar17 = puVar17 + 1;
        }
        for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
          *(undefined *)puVar17 = *(undefined *)puVar14;
          puVar14 = (uint *)((int)puVar14 + 1);
          puVar17 = (uint *)((int)puVar17 + 1);
        }
        this_00 = (TPlayScreen *)FUN_0051e370(local_328,0);
        if (this_00 != (TPlayScreen *)0x0) {
          pcVar12 = (TPlayScreen *)0x1;
          if (in_stack_00000004 != 0) {
            if (((*(int *)(in_stack_00000004 + 0x18) < 1) ||
                (uVar6 = (*(this_00->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EDI),
                *(int *)(in_stack_00000004 + 0x18) <= CONCAT31(extraout_var,uVar6))) &&
               ((*(int *)(in_stack_00000004 + 0x1c) < 1 ||
                (uVar6 = (*(this_00->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EDI),
                CONCAT31(extraout_var_00,uVar6) <= *(int *)(in_stack_00000004 + 0x1c))))) {
              pcVar12 = (TPlayScreen *)0x1;
              if (*(int *)(in_stack_00000004 + 0xc) != 1) goto LAB_00461d99;
              uVar6 = (*(this_00->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
              if (CONCAT31(extraout_var_01,uVar6) == 1) {
                unaff_EDI = 0x461d93;
                iVar13 = TPlayScreen::meth_0x470040(this_00);
                if (iVar13 < 1) goto LAB_00461d99;
              }
            }
            pcVar12 = (TPlayScreen *)0x0;
          }
LAB_00461d99:
          ppcVar9 = (TPlayScreen **)FUN_00482fb0(0x8c);
          if (ppcVar9 != (TPlayScreen **)0x0) {
            *ppcVar9 = this_00;
            _strncpy((char *)(ppcVar9 + 1),(char *)puVar8,0x7f);
            *(undefined *)((int)ppcVar9 + 0x83) = 0;
            ppcVar9[0x21] = (TPlayScreen *)0x1;
            ppcVar9[0x22] = pcVar12;
          }
          meth_0x41c840(this);
          pvVar7 = local_32c;
        }
        iVar13 = FUN_004a1b20(pvVar7,(uint *)(acStack_226 + 2));
        if (iVar13 != 0) {
          bVar5 = false;
        }
      }
    }
  }
  FUN_0058c9ff((int *)(this->cls_0x41c7f0).mbr_0x10,(undefined4 *)(this->cls_0x41c7f0).mbr_0x0,4,
               &LAB_00461e30);
  return;
}



// Function at 00462000

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x462000(cls_0x45f7c0 *this,dword param_1)

{
  char cVar1;
  cls_0x46ce20 *this_00;
  TPlayScreen *this_01;
  bool bVar2;
  void **ppvVar3;
  undefined uVar4;
  HANDLE pvVar5;
  uint *_Source;
  SoftwareRenderer *pcVar6;
  SoftwareRenderer **ppcVar7;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  uint uVar8;
  uint uVar9;
  undefined4 unaff_EBP;
  int iVar10;
  int iVar11;
  uint *puVar12;
  undefined4 *puVar13;
  dword *pdVar14;
  char *pcVar15;
  char *pcVar16;
  uint *puVar17;
  dword in_stack_00000008;
  dword in_stack_0000000c;
  int in_stack_00000010;
  int iStack_34c;
  undefined local_348 [20];
  undefined4 local_334;
  dword adStack_330 [63];
  char acStack_232 [4];
  char acStack_22e [2];
  uint auStack_22c [5];
  char acStack_218 [524];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d13b;
  local_c = ExceptionList;
  if ((param_1 != this->mbr_0x18) || (in_stack_00000008 != this->mbr_0x1c)) {
    ExceptionList = &local_c;
    if ((*(byte *)&(this->cls_0x41c7f0).mbr_0x14 & 1) == 0) {
      iVar10 = 0;
      ExceptionList = &local_c;
      ppvVar3 = &local_c;
      if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
        do {
          if (**(int **)((this->cls_0x41c7f0).mbr_0x10 + iVar10 * 4) != DAT_00659c60) {
            meth_0x46ce80(this);
          }
          iVar10 = iVar10 + 1;
          ppvVar3 = (void **)ExceptionList;
        } while (iVar10 < (int)(this->cls_0x41c7f0).mbr_0x0);
      }
      ExceptionList = ppvVar3;
      meth_0x45f680(this);
    }
    if ((int)param_1 < 0) {
      iVar10 = 0;
    }
    else {
      iVar10 = cls_0x4609f0::meth_0x460ca0((cls_0x4609f0 *)&UNK_0065a630.field_0x138,param_1);
    }
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)local_348,0x10);
    iVar11 = 0;
    local_4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        this_00 = *(cls_0x46ce20 **)((this->cls_0x41c7f0).mbr_0x10 + iVar11 * 4);
        if (this_00[0x21].mbr_0x0 == 0) {
          if ((-1 < iVar11) && (this_00 != (cls_0x46ce20 *)0x0)) {
            cls_0x46ce20::meth_0x46ce20(this_00);
            FUN_004830f0(this_00);
          }
          meth_0x41cb40(this);
        }
        else {
          meth_0x41c840((cls_0x45f7c0 *)local_348);
          meth_0x41c910(this,0);
        }
        iVar11 = iVar11 + 1;
      } while (iVar11 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    puVar13 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
    if (puVar13 != (undefined4 *)0x0) {
      for (uVar8 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar8 != 0; uVar8 = uVar8 - 1) {
        *puVar13 = 0;
        puVar13 = puVar13 + 1;
      }
      for (iVar11 = 0; iVar11 != 0; iVar11 = iVar11 + -1) {
        *(undefined *)puVar13 = 0;
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
    }
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    if (iVar10 != 0) {
      FUN_00483120((char *)&DAT_0065d6a4,(char *)&local_334,0x104);
      uVar8 = 0xffffffff;
      puVar13 = &local_334;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *(char *)puVar13;
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      } while (cVar1 != '\0');
      iVar11 = -(~uVar8 - 1);
      _strncpy(local_348 + ~uVar8 + 0x13,(char *)(iVar10 + 0x58),iVar11 + 0x103);
      (local_348 + ~uVar8 + 0x13)[iVar11 + 0x103] = '\0';
      uVar8 = 0xffffffff;
      puVar13 = &local_334;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *(char *)puVar13;
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      } while (cVar1 != '\0');
      iVar11 = -(~uVar8 - 1);
      _strncpy(local_348 + ~uVar8 + 0x13,&DAT_005d2b00,iVar11 + 0x103);
      (local_348 + ~uVar8 + 0x13)[iVar11 + 0x103] = '\0';
      iVar11 = cls_0x49ead0::meth_0x49ee20((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c,&local_334);
      if (iVar11 != 0) {
        if ((UNK_0065b2dc._28_1_ & 8) != 0) {
          FUN_00481c10((byte *)s_Compressed_files_found_in_module_005d2b08);
        }
        iStack_34c = 1;
      }
      FUN_00483120((char *)&DAT_0065d6a4,(char *)adStack_330,0x104);
      uVar8 = 0xffffffff;
      pdVar14 = adStack_330;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *(char *)pdVar14;
        pdVar14 = (dword *)((int)pdVar14 + 1);
      } while (cVar1 != '\0');
      iVar11 = -(~uVar8 - 1);
      pcVar15 = (char *)((int)&local_334 + ~uVar8 + 3);
      _strncpy(pcVar15,(char *)(iVar10 + 0x58),iVar11 + 0x103);
      pcVar15[iVar11 + 0x103] = '\0';
      uVar8 = 0xffffffff;
      pdVar14 = adStack_330;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *(char *)pdVar14;
        pdVar14 = (dword *)((int)pdVar14 + 1);
      } while (cVar1 != '\0');
      iVar10 = -(~uVar8 - 1);
      pcVar15 = (char *)((int)&local_334 + ~uVar8 + 3);
      _strncpy(pcVar15,s____chr_005d2b34,iVar10 + 0x103);
      pcVar15[iVar10 + 0x103] = '\0';
      pvVar5 = FUN_004a19d0((LPCSTR)adStack_330,(uint *)(acStack_22e + 2));
      bVar2 = true;
      local_348._0_4_ = pvVar5;
      _Source = FUN_0058ade0(adStack_330,'*');
      if (pvVar5 != (HANDLE)0xffffffff) {
        while (bVar2) {
          uVar8 = 0xffffffff;
          pcVar15 = acStack_218;
          do {
            pcVar16 = pcVar15;
            if (uVar8 == 0) break;
            uVar8 = uVar8 - 1;
            pcVar16 = pcVar15 + 1;
            cVar1 = *pcVar15;
            pcVar15 = pcVar16;
          } while (cVar1 != '\0');
          uVar8 = ~uVar8;
          puVar12 = (uint *)(pcVar16 + -uVar8);
          puVar17 = _Source;
          for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *puVar17 = *puVar12;
            puVar12 = puVar12 + 1;
            puVar17 = puVar17 + 1;
          }
          for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
            *(undefined *)puVar17 = *(undefined *)puVar12;
            puVar12 = (uint *)((int)puVar12 + 1);
            puVar17 = (uint *)((int)puVar17 + 1);
          }
          pcVar6 = FUN_0051e370(adStack_330,0);
          if (pcVar6 != (SoftwareRenderer *)0x0) {
            ppcVar7 = (SoftwareRenderer **)FUN_00482fb0(0x8c);
            if (ppcVar7 != (SoftwareRenderer **)0x0) {
              *ppcVar7 = pcVar6;
              _strncpy((char *)(ppcVar7 + 1),(char *)_Source,0x7f);
              *(undefined *)((int)ppcVar7 + 0x83) = 0;
              ppcVar7[0x21] = (SoftwareRenderer *)0x0;
              ppcVar7[0x22] = (SoftwareRenderer *)0x1;
            }
            meth_0x41c840(this);
          }
          iVar10 = FUN_004a1b20((HANDLE)local_348._0_4_,(uint *)(acStack_22e + 2));
          if (iVar10 != 0) {
            bVar2 = false;
          }
        }
      }
      if (iStack_34c != 0) {
        cls_0x49ead0::meth_0x49eff0((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c);
      }
    }
    iVar10 = 0;
    if (0 < (int)local_348._4_4_) {
      do {
        if (in_stack_00000010 == 0) {
          *(undefined4 *)(*(int *)((int)local_334 + iVar10 * 4) + 0x88) = 1;
        }
        else {
          this_01 = **(TPlayScreen ***)((int)local_334 + iVar10 * 4);
          if ((*(int *)(in_stack_00000010 + 0x18) < 1) ||
             (uVar4 = (*(this_01->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EBP),
             *(int *)(in_stack_00000010 + 0x18) <= CONCAT31(extraout_var,uVar4))) {
            if ((*(int *)(in_stack_00000010 + 0x1c) < 1) ||
               (uVar4 = (*(this_01->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EBP),
               CONCAT31(extraout_var_00,uVar4) <= *(int *)(in_stack_00000010 + 0x1c))) {
              if (*(int *)(in_stack_00000010 + 0xc) == 1) {
                uVar4 = (*(this_01->TScreen).vftptr_0x0[1].FUN_00472980_356)(unaff_EBP);
                if (CONCAT31(extraout_var_01,uVar4) == 1) {
                  unaff_EBP = 0x4623d9;
                  iVar11 = TPlayScreen::meth_0x470040(this_01);
                  if (iVar11 < 1) goto LAB_004623ee;
                }
                *(undefined4 *)(*(int *)(adStack_330[0] + iVar10 * 4) + 0x88) = 0;
              }
              else {
LAB_004623ee:
                *(undefined4 *)(*(int *)((int)local_334 + iVar10 * 4) + 0x88) = 1;
              }
            }
            else {
              *(undefined4 *)(*(int *)(adStack_330[0] + iVar10 * 4) + 0x88) = 0;
            }
          }
          else {
            *(undefined4 *)(*(int *)(adStack_330[0] + iVar10 * 4) + 0x88) = 0;
          }
        }
        meth_0x41c840(this);
        iVar10 = iVar10 + 1;
      } while (iVar10 < (int)local_348._4_4_);
    }
    FUN_0058c9ff((int *)(this->cls_0x41c7f0).mbr_0x10,(undefined4 *)(this->cls_0x41c7f0).mbr_0x0,4,
                 &LAB_00461e30);
    this->mbr_0x18 = in_stack_00000008;
    this->mbr_0x1c = in_stack_0000000c;
    FUN_004830f0(local_334);
  }
  ExceptionList = puStack_8;
  return;
}



// Function at 0046ce80

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x46ce80(cls_0x45f7c0 *this)

{
  int *piVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (piVar1 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     piVar1 != (int *)0x0)) {
    if ((undefined4 *)*piVar1 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)*piVar1)(1);
    }
    FUN_004830f0(piVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 00478390

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x478390(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  LPCVOID pvVar2;
  void **ppvVar3;
  int local_18;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d3d4;
  local_c = ExceptionList;
  local_4 = 0;
  local_18 = 0;
  ExceptionList = &local_c;
  ppvVar3 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < local_18) &&
         (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + local_18 * 4),
         ppvVar1 != (LPCVOID *)0x0)) {
        local_4 = CONCAT31(local_4._1_3_,2);
        FUN_004830f0(*ppvVar1);
        cls_0x478320::meth_0x478340((cls_0x478320 *)(ppvVar1 + 3));
        cls_0x478320::meth_0x478340((cls_0x478320 *)(ppvVar1 + 5));
        if (ppvVar1[9] != (LPCVOID)0x0) {
          FUN_004830f0(ppvVar1[9]);
        }
        local_4._0_1_ = 1;
        if (ppvVar1[6] != (LPCVOID)0x0) {
          FUN_004830f0(ppvVar1[6]);
        }
        local_4 = (uint)local_4._1_3_ << 8;
        cls_0x478320::meth_0x478330((cls_0x478320 *)(ppvVar1 + 3));
        FUN_004830f0(ppvVar1);
      }
      meth_0x41cb40(this);
      local_18 = local_18 + 1;
      ppvVar3 = (void **)ExceptionList;
    } while (local_18 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar3;
  ppvVar1 = (LPCVOID *)(this->cls_0x41c7f0).mbr_0x14;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (ppvVar1 != (LPCVOID *)0x0) {
    local_4 = CONCAT31(local_4._1_3_,4);
    FUN_004830f0(*ppvVar1);
    if (ppvVar1[4] != (LPCVOID)0x0) {
      FUN_004830f0(ppvVar1[4]);
    }
    ppvVar1[4] = (LPCVOID)0x0;
    pvVar2 = ppvVar1[6];
    ((cls_0x478320 *)(ppvVar1 + 3))->mbr_0x0 = 0;
    *(undefined2 *)((int)ppvVar1 + 0xe) = 0;
    if (pvVar2 != (LPCVOID)0x0) {
      FUN_004830f0(pvVar2);
    }
    ppvVar1[6] = (LPCVOID)0x0;
    *(undefined2 *)(ppvVar1 + 5) = 0;
    *(undefined2 *)((int)ppvVar1 + 0x16) = 0;
    if (ppvVar1[9] != (LPCVOID)0x0) {
      FUN_004830f0(ppvVar1[9]);
    }
    local_4._0_1_ = 3;
    if (ppvVar1[6] != (LPCVOID)0x0) {
      FUN_004830f0(ppvVar1[6]);
    }
    local_4 = (uint)local_4._1_3_ << 8;
    cls_0x478320::meth_0x478330((cls_0x478320 *)(ppvVar1 + 3));
    FUN_004830f0(ppvVar1);
  }
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00478680

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x478680(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     ppvVar1 != (LPCVOID *)0x0)) {
    FUN_00477fe0(ppvVar1);
    FUN_004830f0(ppvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 00487970

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x487970(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  SIZE_T SVar3;
  int iVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d508;
  local_c = ExceptionList;
  iVar4 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar4) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4), pvVar1 != (LPCVOID)0x0))
      {
        SVar3 = FUN_0058cff3(pvVar1);
        DAT_0065ba08 = DAT_0065ba08 - SVar3;
        FUN_0058cfab(pvVar1);
      }
      meth_0x41cb40(this);
      iVar4 = iVar4 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x14;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar3 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar3;
  }
  FUN_0058cfab(pvVar1);
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar3 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar3;
  }
  FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return;
}



// Function at 0048d1a0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d1a0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d8a8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0048d230

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d230(cls_0x45f7c0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *in_stack_00000004;
  
  puVar1 = FUN_00482fb0(800);
  for (iVar2 = 200; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    puVar1 = puVar1 + 1;
  }
  meth_0x41c840(this);
  return;
}



// Function at 0048d260

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d260(cls_0x45f7c0 *this)

{
  char cVar1;
  bool bVar2;
  void **ppvVar3;
  HANDLE pvVar4;
  HANDLE pvVar5;
  FILE **ppFVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  uint uVar9;
  uint uVar10;
  undefined4 *puVar11;
  int iVar12;
  char *pcVar13;
  undefined4 *puVar14;
  int in_stack_00000004;
  char *pcVar15;
  undefined4 local_4c8;
  char local_4c4 [258];
  char acStack_3c2 [2];
  undefined4 local_3c0 [8];
  char local_3a0 [31];
  undefined local_381;
  char acStack_344 [262];
  char acStack_23e [2];
  uint local_23c [5];
  char local_228 [260];
  uint local_124 [70];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d8ce;
  local_c = ExceptionList;
  iVar12 = 0;
  ExceptionList = &local_c;
  ppvVar3 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      meth_0x48e820(this);
      iVar12 = iVar12 + 1;
      ppvVar3 = (void **)ExceptionList;
    } while (iVar12 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar3;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_00483120((char *)&DAT_0065da00,local_4c4,0x104);
  uVar9 = 0xffffffff;
  pcVar13 = local_4c4;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  if (local_4c4[~uVar9 - 2] == '\\') {
    uVar9 = 0xffffffff;
    pcVar13 = local_4c4;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar13;
      pcVar13 = pcVar13 + 1;
    } while (cVar1 != '\0');
    local_4c4[~uVar9 - 2] = '\0';
  }
  CreateDirectoryA(local_4c4,(LPSECURITY_ATTRIBUTES)0x0);
  if (in_stack_00000004 == 0) {
    uVar9 = 0xffffffff;
    pcVar13 = local_4c4;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar13;
      pcVar13 = pcVar13 + 1;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    pcVar13 = local_4c4 + (uVar9 - 1);
    pcVar15 = s__Single_005d9c80;
  }
  else {
    uVar9 = 0xffffffff;
    pcVar13 = local_4c4;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar13;
      pcVar13 = pcVar13 + 1;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    pcVar13 = local_4c4 + (uVar9 - 1);
    pcVar15 = s__Multi_005d9c78;
  }
  _strncpy(pcVar13,pcVar15,-(uVar9 - 1) + 0x103);
  pcVar13[-(uVar9 - 1) + 0x103] = '\0';
  CreateDirectoryA(local_4c4,(LPSECURITY_ATTRIBUTES)0x0);
  uVar9 = 0xffffffff;
  pcVar13 = local_4c4;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  iVar12 = -(~uVar9 - 1);
  _strncpy(local_4c4 + (~uVar9 - 1),&DAT_005d9c88,iVar12 + 0x103);
  (local_4c4 + (~uVar9 - 1))[iVar12 + 0x103] = '\0';
  pvVar4 = FUN_0058c680(local_4c4,(uint *)(acStack_23e + 2));
  bVar2 = true;
  if (pvVar4 != (HANDLE)0xffffffff) {
    while (bVar2) {
      if ((((byte)local_23c[0] & 0x10) != 0) && (local_228[0] != '.')) {
        _strncpy(acStack_344 + 4,local_4c4,0x103);
        uVar9 = 0xffffffff;
        acStack_23e[1] = 0;
        pcVar13 = acStack_344 + 4;
        do {
          if (uVar9 == 0) break;
          uVar9 = uVar9 - 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar13 + 1;
        } while (cVar1 != '\0');
        acStack_344[~uVar9] = '\0';
        uVar9 = 0xffffffff;
        pcVar13 = acStack_344 + 4;
        do {
          if (uVar9 == 0) break;
          uVar9 = uVar9 - 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar13 + 1;
        } while (cVar1 != '\0');
        iVar12 = -(~uVar9 - 1);
        _strncpy(acStack_344 + ~uVar9 + 3,local_228,iVar12 + 0x103);
        (acStack_344 + ~uVar9 + 3)[iVar12 + 0x103] = '\0';
        uVar9 = 0xffffffff;
        pcVar13 = acStack_344 + 4;
        do {
          if (uVar9 == 0) break;
          uVar9 = uVar9 - 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar13 + 1;
        } while (cVar1 != '\0');
        iVar12 = -(~uVar9 - 1);
        _strncpy(acStack_344 + ~uVar9 + 3,s__game_sav_005d9c90,iVar12 + 0x103);
        (acStack_344 + ~uVar9 + 3)[iVar12 + 0x103] = '\0';
        pvVar5 = FUN_0058c680(acStack_344 + 4,local_124);
        if (pvVar5 != (HANDLE)0xffffffff) {
          ppFVar6 = (FILE **)FUN_004a13f0(acStack_344 + 4,(uint *)&DAT_005d9c9c,0);
          if (ppFVar6 != (FILE **)0x0) {
            uVar9 = FUN_004a15a0((undefined4 *)(acStack_3c2 + 2),0x80,1,ppFVar6);
            if (uVar9 != 1) {
              local_3a0[0] = '\0';
            }
            FUN_004a1540(ppFVar6);
          }
          if (((local_3a0[0] == '\0') && (-1 < DAT_0065a780)) &&
             (iVar12 = *(int *)(DAT_0065a77c + DAT_0065a780 * 4), iVar12 != 0)) {
            _strncpy(local_3a0,(char *)(iVar12 + 0x58),0x1f);
            local_381 = 0;
          }
          puVar7 = FUN_00482fb0(0xc);
          local_4c8 = puVar7;
          local_4 = 0;
          if (puVar7 != (undefined4 *)0x0) {
            uVar9 = 0xffffffff;
            pcVar13 = local_228;
            do {
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar13 + 1;
            } while (cVar1 != '\0');
            puVar8 = FUN_00482ef0(~uVar9);
            uVar9 = 0xffffffff;
            pcVar13 = local_228;
            do {
              pcVar15 = pcVar13;
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              pcVar15 = pcVar13 + 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar15;
            } while (cVar1 != '\0');
            uVar9 = ~uVar9;
            puVar11 = (undefined4 *)(pcVar15 + -uVar9);
            puVar14 = puVar8;
            for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
              *puVar14 = *puVar11;
              puVar11 = puVar11 + 1;
              puVar14 = puVar14 + 1;
            }
            for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined *)puVar14 = *(undefined *)puVar11;
              puVar11 = (undefined4 *)((int)puVar11 + 1);
              puVar14 = (undefined4 *)((int)puVar14 + 1);
            }
            uVar9 = 0xffffffff;
            *puVar7 = puVar8;
            pcVar13 = acStack_344 + 4;
            do {
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar13 + 1;
            } while (cVar1 != '\0');
            puVar8 = FUN_00482ef0(~uVar9);
            uVar9 = 0xffffffff;
            pcVar13 = acStack_344 + 4;
            do {
              pcVar15 = pcVar13;
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              pcVar15 = pcVar13 + 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar15;
            } while (cVar1 != '\0');
            uVar9 = ~uVar9;
            puVar11 = (undefined4 *)(pcVar15 + -uVar9);
            puVar14 = puVar8;
            for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
              *puVar14 = *puVar11;
              puVar11 = puVar11 + 1;
              puVar14 = puVar14 + 1;
            }
            for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined *)puVar14 = *(undefined *)puVar11;
              puVar11 = (undefined4 *)((int)puVar11 + 1);
              puVar14 = (undefined4 *)((int)puVar14 + 1);
            }
            uVar9 = 0xffffffff;
            puVar7[1] = puVar8;
            pcVar13 = local_3a0;
            do {
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar13 + 1;
            } while (cVar1 != '\0');
            puVar8 = FUN_00482ef0(~uVar9);
            uVar9 = 0xffffffff;
            pcVar13 = local_3a0;
            do {
              pcVar15 = pcVar13;
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              pcVar15 = pcVar13 + 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar15;
            } while (cVar1 != '\0');
            uVar9 = ~uVar9;
            puVar11 = (undefined4 *)(pcVar15 + -uVar9);
            puVar14 = puVar8;
            for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
              *puVar14 = *puVar11;
              puVar11 = puVar11 + 1;
              puVar14 = puVar14 + 1;
            }
            for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined *)puVar14 = *(undefined *)puVar11;
              puVar11 = (undefined4 *)((int)puVar11 + 1);
              puVar14 = (undefined4 *)((int)puVar14 + 1);
            }
            puVar7[2] = puVar8;
          }
          local_4 = 0xffffffff;
          meth_0x41c840(this);
        }
      }
      iVar12 = FUN_0058c74d(pvVar4,(uint *)(acStack_23e + 2));
      if (iVar12 != 0) {
        bVar2 = false;
      }
    }
  }
  ExceptionList = local_c;
  return;
}



// Function at 0048d660

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d660(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  int iVar2;
  
  iVar2 = 0;
  if ((int)(this->cls_0x41c7f0).mbr_0x0 < 1) {
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    return;
  }
  do {
    if ((-1 < iVar2) &&
       (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4),
       ppvVar1 != (LPCVOID *)0x0)) {
      FUN_00482f80(*ppvVar1);
      FUN_00482f80(ppvVar1[1]);
      FUN_00482f80(ppvVar1[2]);
      FUN_004830f0(ppvVar1);
    }
    meth_0x41cb40(this);
    iVar2 = iVar2 + 1;
  } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 0048d6d0

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d6d0(cls_0x45f7c0 *this)

{
  uint uVar1;
  int iVar2;
  byte *in_stack_00000004;
  
  if ((in_stack_00000004 != (byte *)0x0) && (iVar2 = 0, 0 < (int)(this->cls_0x41c7f0).mbr_0x0)) {
    do {
      uVar1 = **(uint **)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4);
      uVar1 = FUN_0059a530_stricmp(uVar1,in_stack_00000004,(byte *)uVar1);
      if (uVar1 == 0) {
        return iVar2;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return -1;
}



// Function at 0048d720

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48d720_SaveGame(cls_0x45f7c0 *this,char *param_1)

{
  char cVar1;
  dword dVar2;
  uint uVar3;
  byte *pbVar4;
  char *pcVar5;
  HANDLE hFindFile;
  FILE *pFVar6;
  undefined4 uVar7;
  dword dVar8;
  int *piVar9;
  undefined4 *puVar10;
  uint uVar11;
  int iVar12;
  undefined4 *puVar13;
  undefined2 *puVar14;
  undefined2 *puVar15;
  int iVar16;
  undefined4 in_stack_00000008;
  char *pcVar17;
  FILE *pFVar18;
  cls_0x5a5ff0 local_81c;
  cls_0x45f7c0 *local_808;
  undefined4 uStack_804;
  undefined auStack_800 [28];
  undefined4 local_7e4 [64];
  char acStack_6e2 [2];
  undefined4 auStack_6e0 [5];
  int iStack_6cc;
  undefined4 uStack_6c8;
  undefined4 uStack_6c4;
  char acStack_6c0 [31];
  undefined uStack_6a1;
  char cStack_661;
  undefined4 local_660 [64];
  char acStack_55e [2];
  undefined4 auStack_55c [65];
  _WIN32_FIND_DATAA _Stack_458;
  undefined4 auStack_318 [65];
  undefined4 auStack_214 [65];
  CHAR local_110 [260];
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d8e3;
  local_c = ExceptionList;
  if (DAT_00667fcc == 0) {
    return 0;
  }
  ExceptionList = &local_c;
  local_808 = this;
  if ((DAT_00668154 != 0) &&
     (ExceptionList = &local_c,
     uVar3 = FUN_0059a530_stricmp((uint)this,(byte *)param_1,(byte *)s_newgame_005d9ca0), uVar3 == 0)) {
    pbVar4 = (byte *)0x0;
    if (-1 < DAT_0065a784) {
      pbVar4 = *(byte **)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    if ((*pbVar4 & 0x20) == 0) {
      uVar3 = 0xffffffff;
      puVar14 = &DAT_0065d6a4;
      do {
        puVar15 = puVar14;
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        puVar15 = (undefined2 *)((int)puVar14 + 1);
        cVar1 = *(char *)puVar14;
        puVar14 = puVar15;
      } while (cVar1 != '\0');
      uVar3 = ~uVar3;
      puVar10 = (undefined4 *)((int)puVar15 - uVar3);
      puVar13 = local_7e4;
      for (uVar11 = uVar3 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *puVar13 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
      iVar12 = -1;
      puVar10 = local_7e4;
      do {
        puVar13 = puVar10;
        if (iVar12 == 0) break;
        iVar12 = iVar12 + -1;
        puVar13 = (undefined4 *)((int)puVar10 + 1);
        cVar1 = *(char *)puVar10;
        puVar10 = puVar13;
      } while (cVar1 != '\0');
      *(undefined2 *)((int)puVar13 + -1) = DAT_005d9d18;
      if (DAT_0065a784 < 0) {
        iVar12 = 0;
      }
      else {
        iVar12 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
      }
      uVar3 = 0xffffffff;
      pcVar5 = (char *)(iVar12 + 0x58);
      do {
        pcVar17 = pcVar5;
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        pcVar17 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar17;
      } while (cVar1 != '\0');
      uVar3 = ~uVar3;
      iVar12 = -1;
      puVar10 = local_7e4;
      do {
        puVar13 = puVar10;
        if (iVar12 == 0) break;
        iVar12 = iVar12 + -1;
        puVar13 = (undefined4 *)((int)puVar10 + 1);
        cVar1 = *(char *)puVar10;
        puVar10 = puVar13;
      } while (cVar1 != '\0');
      puVar10 = (undefined4 *)(pcVar17 + -uVar3);
      puVar13 = (undefined4 *)((int)puVar13 + -1);
      for (uVar11 = uVar3 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *puVar13 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
      iVar12 = -1;
      puVar10 = local_7e4;
      do {
        puVar13 = puVar10;
        if (iVar12 == 0) break;
        iVar12 = iVar12 + -1;
        puVar13 = (undefined4 *)((int)puVar10 + 1);
        cVar1 = *(char *)puVar10;
        puVar10 = puVar13;
      } while (cVar1 != '\0');
      *(undefined4 *)((int)puVar13 + -1) = s__newgame_sav_005d9d1c._0_4_;
      *(undefined4 *)((int)puVar13 + 3) = s__newgame_sav_005d9d1c._4_4_;
      *(undefined4 *)((int)puVar13 + 7) = s__newgame_sav_005d9d1c._8_4_;
      *(char *)((int)puVar13 + 0xb) = s__newgame_sav_005d9d1c[0xc];
      goto LAB_0048dbde;
    }
  }
  if ((byte *)param_1 == (byte *)0x0) {
    param_1 = s_Default_Save_005d9ca8;
  }
  iVar12 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      uVar3 = (this->cls_0x41c7f0).mbr_0x10;
      uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)param_1,**(byte ***)(uVar3 + iVar12 * 4));
      if (uVar3 == 0) break;
      iVar12 = iVar12 + 1;
    } while (iVar12 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  FUN_00483120((char *)&DAT_0065da00,(char *)local_7e4,0x104);
  pcVar5 = _strrchr(local_7e4,(cls_0x49ead0 *)local_7e4,0x5c);
  if (pcVar5 != (char *)0x0) {
    *pcVar5 = '\0';
  }
  CreateDirectoryA((LPCSTR)local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  if (DAT_0066829c == 0) {
    uVar3 = 0xffffffff;
    puVar10 = local_7e4;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *(char *)puVar10;
      puVar10 = (undefined4 *)((int)puVar10 + 1);
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar5 = (char *)((int)local_7e4 + (uVar3 - 1));
    pcVar17 = s__Single_005d9cc0;
  }
  else {
    uVar3 = 0xffffffff;
    puVar10 = local_7e4;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *(char *)puVar10;
      puVar10 = (undefined4 *)((int)puVar10 + 1);
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar5 = (char *)((int)local_7e4 + (uVar3 - 1));
    pcVar17 = s__Multi_005d9cb8;
  }
  _strncpy(pcVar5,pcVar17,-(uVar3 - 1) + 0x103);
  pcVar5[-(uVar3 - 1) + 0x103] = '\0';
  CreateDirectoryA((LPCSTR)local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  uVar3 = 0xffffffff;
  puVar10 = local_7e4;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *(char *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
  } while (cVar1 != '\0');
  iVar12 = -(~uVar3 - 1);
  pcVar5 = (char *)((int)local_7e4 + (~uVar3 - 1));
  _strncpy(pcVar5,&DAT_005d9cc8,iVar12 + 0x103);
  pcVar5[iVar12 + 0x103] = '\0';
  uVar3 = 0xffffffff;
  puVar10 = local_7e4;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *(char *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
  } while (cVar1 != '\0');
  iVar12 = -(~uVar3 - 1);
  pcVar5 = (char *)((int)local_7e4 + (~uVar3 - 1));
  _strncpy(pcVar5,param_1,iVar12 + 0x103);
  pcVar5[iVar12 + 0x103] = '\0';
  _strncpy(&cStack_661 + 1,(char *)local_7e4,0x103);
  pcVar5 = &cStack_661;
  uVar3 = 0xffffffff;
  acStack_55e[1] = 0;
  do {
    pcVar5 = pcVar5 + 1;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
  } while (*pcVar5 != '\0');
  iVar12 = -(~uVar3 - 1);
  _strncpy(&cStack_661 + ~uVar3,s__CurMap_005d9ccc,iVar12 + 0x103);
  (&cStack_661 + ~uVar3)[iVar12 + 0x103] = '\0';
  CreateDirectoryA((LPCSTR)local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  CreateDirectoryA(&cStack_661 + 1,(LPSECURITY_ATTRIBUTES)0x0);
  GetCurrentDirectoryA(0x104,local_110);
  SetCurrentDirectoryA((LPCSTR)local_7e4);
  DeleteFileA(s_game_sav_005d9cd4);
  DeleteFileA(s_ss_bmp_005d9ce0);
  uVar3 = 0xffffffff;
  pcVar5 = &cStack_661 + 1;
  do {
    pcVar17 = pcVar5;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar17 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar17;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  puVar10 = (undefined4 *)(pcVar17 + -uVar3);
  puVar13 = auStack_318;
  for (uVar11 = uVar3 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  iVar12 = -1;
  puVar10 = auStack_318;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar13 + -1) = DAT_005d9ce8;
  *(char *)((int)puVar13 + 3) = DAT_005d9cec;
  SetCurrentDirectoryA(&cStack_661 + 1);
  hFindFile = FindFirstFileA((LPCSTR)auStack_318,&_Stack_458);
  if (hFindFile != (HANDLE)0xffffffff) {
    DeleteFileA(_Stack_458.cFileName);
    iVar12 = FindNextFileA(hFindFile,&_Stack_458);
    while (iVar12 != 0) {
      DeleteFileA(_Stack_458.cFileName);
      iVar12 = FindNextFileA(hFindFile,&_Stack_458);
    }
    FindClose(hFindFile);
  }
  SetCurrentDirectoryA(local_110);
  FUN_0044e250((undefined4 *)(&cStack_661 + 1));
  uVar3 = 0xffffffff;
  puVar10 = local_7e4;
  do {
    puVar13 = puVar10;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  puVar10 = (undefined4 *)((int)puVar13 - uVar3);
  puVar13 = auStack_55c;
  for (uVar11 = uVar3 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  iVar12 = -1;
  puVar10 = auStack_55c;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  *(undefined2 *)((int)puVar13 + -1) = DAT_005d9cf0;
  iVar12 = -1;
  puVar10 = auStack_55c;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar13 + -1) = DAT_005d9cf4;
  *(undefined2 *)((int)puVar13 + 3) = DAT_005d9cf8;
  *(char *)((int)puVar13 + 5) = DAT_005d9cfa;
  uVar3 = 0xffffffff;
  pcVar5 = &DAT_005d9cfc;
  do {
    pcVar17 = pcVar5;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar17 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar17;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  puVar10 = (undefined4 *)(pcVar17 + -uVar3);
  puVar13 = auStack_214;
  for (uVar11 = uVar3 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  iVar12 = -1;
  puVar10 = auStack_214;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar13 + -1) = DAT_005d9d00;
  *(undefined2 *)((int)puVar13 + 3) = DAT_005d9d04;
  *(char *)((int)puVar13 + 5) = DAT_005d9d06;
  FUN_004814d0((byte *)auStack_214,(byte *)auStack_55c);
  iVar12 = -1;
  puVar10 = local_7e4;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  iVar12 = -1;
  *(undefined2 *)((int)puVar13 + -1) = DAT_005d9d08;
  puVar10 = local_7e4;
  do {
    puVar13 = puVar10;
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    puVar13 = (undefined4 *)((int)puVar10 + 1);
    cVar1 = *(char *)puVar10;
    puVar10 = puVar13;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar13 + -1) = s_game_sav_005d9d0c._0_4_;
  *(undefined4 *)((int)puVar13 + 3) = s_game_sav_005d9d0c._4_4_;
  *(char *)((int)puVar13 + 7) = s_game_sav_005d9d0c[8];
  this = local_808;
LAB_0048dbde:
  _DAT_0065a250 = in_stack_00000008;
  cls_0x5a5ff0::cls_0x5a5ff0(&local_81c,0x8000);
  uStack_4 = 0;
  pFVar6 = (FILE *)FUN_0058b5db(local_7e4,&DAT_005d9d2c);
  local_808 = (cls_0x45f7c0 *)pFVar6;
  if (pFVar6 == (FILE *)0x0) {
    uStack_4 = 0xffffffff;
    cls_0x5a5ff0::~cls_0x5a5ff0(&local_81c);
    uVar7 = 0;
  }
  else {
    uStack_804 = 1;
    FUN_0058c3a5((char **)pFVar6,0,0);
    puVar10 = auStack_6e0;
    for (iVar12 = 0x20; iVar12 != 0; iVar12 = iVar12 + -1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    auStack_6e0[0] = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
    iStack_6cc = DAT_0066829c;
    uStack_6c8 = 2;
    uStack_6c4 = 0xf;
    if (DAT_0065a784 < 0) {
      iVar12 = 0;
    }
    else {
      iVar12 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    _strncpy(acStack_6c0,(char *)(iVar12 + 0x58),0x1f);
    uStack_6a1 = 0;
    uVar3 = FUN_0058beb8(auStack_6e0,0x80,1,(char **)pFVar6);
    if (uVar3 == 0) {
      uStack_804 = 0;
    }
    if (DAT_0066829c != 0) {
      puVar10 = &DAT_00676764;
      puVar13 = &DAT_00668300;
      for (iVar12 = 0x31; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar13 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar13 = puVar13 + 1;
      }
      uVar3 = FUN_0058beb8(&DAT_00668300,0x200,1,(char **)local_808);
      if (uVar3 == 0) {
        uStack_804 = 0;
      }
    }
    cls_0x4974d0::meth_0x4974d0_TGameState_SaveStream((cls_0x4974d0 *)&DAT_0065def0);
    dVar8 = this->mbr_0x18;
    if ((int)((local_81c.mbr_0x4 - local_81c.mbr_0x8) + local_81c.mbr_0xc) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(&local_81c);
    }
    dVar2 = this->mbr_0x18;
    *(dword *)local_81c.mbr_0x8 = dVar8;
    local_81c.mbr_0x8 = local_81c.mbr_0x8 + 4;
    iVar12 = 0;
    if (0 < (int)dVar2) {
      do {
        dVar8 = *(dword *)(this->mbr_0x1c + iVar12 * 8);
        if ((int)((local_81c.mbr_0x4 - local_81c.mbr_0x8) + local_81c.mbr_0xc) < 4) {
          cls_0x5a5ff0::meth_0x49cc70(&local_81c);
        }
        dVar2 = this->mbr_0x1c;
        *(dword *)local_81c.mbr_0x8 = dVar8;
        local_81c.mbr_0x8 = local_81c.mbr_0x8 + 4;
        dVar8 = *(dword *)(dVar2 + 4 + iVar12 * 8);
        if ((int)((local_81c.mbr_0x4 - local_81c.mbr_0x8) + local_81c.mbr_0xc) < 4) {
          cls_0x5a5ff0::meth_0x49cc70(&local_81c);
        }
        dVar2 = this->mbr_0x18;
        *(dword *)local_81c.mbr_0x8 = dVar8;
        local_81c.mbr_0x8 = local_81c.mbr_0x8 + 4;
        iVar12 = iVar12 + 1;
      } while (iVar12 < (int)dVar2);
    }
    dVar8 = meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
    if ((int)((local_81c.mbr_0x4 - local_81c.mbr_0x8) + local_81c.mbr_0xc) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(&local_81c);
    }
    iVar16 = 0;
    *(dword *)local_81c.mbr_0x8 = dVar8;
    local_81c.mbr_0x8 = local_81c.mbr_0x8 + 4;
    iVar12 = meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
    if (0 < iVar12) {
      do {
        piVar9 = (int *)meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,iVar16);
        (**(code **)(*piVar9 + 0x40))(piVar9[2] & 0xfbffffff);
        auStack_800._0_4_ = 0;
        auStack_800._16_4_ = 0;
        auStack_800._12_4_ = 0;
        auStack_800._20_4_ = 0;
        auStack_800._24_4_ = (int *)0x0;
        auStack_800._4_4_ = piVar9;
        auStack_800._8_4_ = piVar9;
        cls_0x477870::meth_0x46dfb0((cls_0x477870 *)auStack_800);
        while ((int *)auStack_800._24_4_ != (int *)0x0) {
          (**(code **)(*(int *)auStack_800._24_4_ + 0x40))
                    (*(uint *)(auStack_800._24_4_ + 8) & 0xfbffffff);
          cls_0x477870::meth_0x46dfb0((cls_0x477870 *)auStack_800);
        }
        FUN_00472110(piVar9,&local_81c);
        iVar16 = iVar16 + 1;
        iVar12 = meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
      } while (iVar16 < iVar12);
    }
    pFVar6 = (FILE *)local_808;
    uVar3 = local_81c.mbr_0x8 - local_81c.mbr_0x4;
    uVar11 = 1;
    pFVar18 = (FILE *)local_808;
    puVar10 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_81c);
    uVar3 = FUN_0058beb8(puVar10,uVar3,uVar11,&pFVar18->_ptr);
    uVar7 = uStack_804;
    if (uVar3 == 0) {
      uVar7 = 0;
    }
    FUN_0058b4f1(pFVar6);
    _DAT_0065a250 = 0;
    uStack_4 = 0xffffffff;
    cls_0x5a5ff0::~cls_0x5a5ff0(&local_81c);
  }
  ExceptionList = local_c;
  return uVar7;
}



// Function at 0048df40

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48df40(cls_0x45f7c0 *this,int param_1)

{
  dword dVar1;
  undefined4 uVar2;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  uVar2 = 0;
  if (((0 < (int)dVar1) && (-1 < param_1)) && (param_1 < (int)dVar1)) {
    uVar2 = **(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4);
  }
  meth_0x48d720_SaveGame(this,uVar2);
  return;
}



// Function at 0048df70

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48df70_LoadGame(cls_0x45f7c0 *this,char *param_1)

{
  char cVar1;
  dword dVar2;
  int iVar3;
  FILE **ppFVar4;
  DWORD DVar5;
  uint uVar6;
  undefined4 *puVar7;
  cls_0x5b4f30_TPlayer *pcVar8;
  uint uVar9;
  cls_0x45f7c0 *pcVar10;
  cls_0x45f7c0 *extraout_ECX;
  char *pcVar11;
  int iVar12;
  int *extraout_EDX;
  int *extraout_EDX_00;
  int *extraout_EDX_01;
  int *extraout_EDX_02;
  int *extraout_EDX_03;
  int *extraout_EDX_04;
  int *extraout_EDX_05;
  int *extraout_EDX_06;
  int *extraout_EDX_07;
  int *extraout_EDX_08;
  int *piVar13;
  int *piVar14;
  uint in_stack_00000008;
  int iStack_1a8;
  int *piStack_1a4;
  int *piStack_1a0;
  int *piStack_19c;
  uint local_198;
  int *piStack_194;
  int iStack_190;
  int iStack_18c;
  uint local_188;
  uint local_184;
  undefined4 auStack_180 [5];
  int iStack_16c;
  int iStack_168;
  int iStack_164;
  char acStack_160 [31];
  undefined uStack_141;
  char acStack_105 [259];
  char acStack_2 [2];
  
  local_198 = in_stack_00000008 & 1;
  if (local_198 == 0) {
    if ((byte *)param_1 == (byte *)0x0) {
      param_1 = s_Default_Save_005d9d40;
    }
    iVar12 = 0;
    pcVar10 = this;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar9 = FUN_0059a530_stricmp((uint)pcVar10,(byte *)param_1,
                             **(byte ***)((this->cls_0x41c7f0).mbr_0x10 + iVar12 * 4));
        if (uVar9 == 0) break;
        iVar12 = iVar12 + 1;
        pcVar10 = extraout_ECX;
      } while (iVar12 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    if ((int)(this->cls_0x41c7f0).mbr_0x0 <= iVar12) {
      return 0;
    }
    _strncpy(acStack_105 + 1,*(char **)(*(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar12 * 4) + 4),
             0x103);
    acStack_2[1] = 0;
    _strncpy(&DAT_00667eb8,acStack_105 + 1,0x103);
    uVar9 = 0xffffffff;
    UNK_00667eb9._258_1_ = 0;
    pcVar11 = &DAT_00667eb8;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    for (pcVar11 = (char *)((int)&DAT_00667eb0 + ~uVar9 + 7);
        (pcVar11 != &DAT_00667eb8 && (*pcVar11 != '\\')); pcVar11 = pcVar11 + -1) {
      *pcVar11 = '\0';
    }
    uVar9 = 0xffffffff;
    pcVar11 = &DAT_00667eb8;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    _strncpy((char *)((int)&DAT_00667eb0 + uVar9 + 7),s_CurMap_005d9d50,-(uVar9 - 1) + 0x103);
    *(undefined *)(uVar9 + -(uVar9 - 1) + 0x667fba) = 0;
  }
  else {
    FUN_00483120((char *)&DAT_0065d6a4,acStack_105 + 1,0x104);
    if (DAT_0065a780 < 0) {
      iVar12 = 0;
    }
    else {
      iVar12 = *(int *)(DAT_0065a77c + DAT_0065a780 * 4);
    }
    pcVar11 = acStack_105;
    uVar9 = 0xffffffff;
    do {
      pcVar11 = pcVar11 + 1;
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
    } while (*pcVar11 != '\0');
    iVar3 = -(~uVar9 - 1);
    _strncpy(acStack_105 + ~uVar9,(char *)(iVar12 + 0x58),iVar3 + 0x103);
    (acStack_105 + ~uVar9)[iVar3 + 0x103] = '\0';
    pcVar11 = acStack_105;
    uVar9 = 0xffffffff;
    do {
      pcVar11 = pcVar11 + 1;
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
    } while (*pcVar11 != '\0');
    iVar12 = -(~uVar9 - 1);
    _strncpy(acStack_105 + ~uVar9,s__newgame_sav_005d9d30,iVar12 + 0x103);
    (acStack_105 + ~uVar9)[iVar12 + 0x103] = '\0';
    DAT_00667eb8 = 0;
  }
  uVar9 = in_stack_00000008 & 2;
  (this->cls_0x41c7f0).mbr_0x14 = 1;
  local_188 = uVar9;
  if (uVar9 == 0) {
    TCharacter::meth_0x44e460((TCharacter *)&DAT_006668d8);
    if (local_198 == 0) {
      TCharacter::meth_0x44e050((TCharacter *)&DAT_006668d8);
    }
    TCharacter::meth_0x47ece0((TCharacter *)&DAT_0065caf0);
    UNK_00667c9c._12_4_ = 1;
    cls_0x535a10::meth_0x5360f0((cls_0x535a10 *)&DAT_00667cc8);
    cls_0x532f40::meth_0x532f40((cls_0x532f40 *)&DAT_0065a3b8);
    cls_0x496e20::meth_0x496e20((cls_0x496e20 *)&DAT_0065def0);
    FUN_004975c0_TScriptManager_ReloadStates((cls_0x495cf0 *)&DAT_0065def0);
    cls_0x41c600::meth_0x41c600((cls_0x41c600 *)&DAT_0065b8b0);
    meth_0x51eda0_TPlayerManager_Clear((cls_0x45f7c0 *)&DAT_0065a890);
    TCharacter::meth_0x47c580((TCharacter *)&DAT_0065caf0);
  }
  ppFVar4 = (FILE **)FUN_004a13f0(acStack_105 + 1,(uint *)&DAT_005d9d58,0);
  if (ppFVar4 != (FILE **)0x0) {
    DVar5 = FUN_004a17b0((int *)ppFVar4);
    if (DVar5 == 0) {
      FUN_00481c10((byte *)s_Invalid_save_file__s_in_save_dir_005d9d5c);
    }
    uVar6 = FUN_004a15a0(auStack_180,0x80,1,ppFVar4);
    if ((int)uVar6 < 1) {
      iStack_1a8 = 0;
    }
    local_188 = auStack_180[0];
    piVar13 = (int *)(DVar5 - 0x80);
    iStack_190 = iStack_16c;
    iStack_18c = iStack_168;
    local_198 = iStack_164;
    if ((iStack_16c == 0) || (iStack_168 < 1)) {
      puVar7 = &DAT_00668300;
      for (iVar12 = 0x80; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar7 = 0;
        puVar7 = puVar7 + 1;
      }
    }
    else {
      uVar6 = FUN_004a15a0(&DAT_00668300,0x200,1,ppFVar4);
      if ((int)uVar6 < 1) {
        iStack_1a8 = 0;
      }
      piVar13 = (int *)(DVar5 - 0x280);
      local_184 = uVar9;
    }
    if (local_184 != 0) {
      FUN_004a1540(ppFVar4);
      (this->cls_0x41c7f0).mbr_0x14 = 0;
      return 1;
    }
    piStack_194 = FUN_00482ef0((uint)piVar13);
    uVar9 = FUN_004a15a0(piStack_194,(uint)piVar13,1,ppFVar4);
    if ((int)uVar9 < 1) {
      iStack_1a8 = 0;
    }
    FUN_004a1540(ppFVar4);
    DAT_0065a254 = 0;
    piStack_1a0 = piStack_194;
    piStack_1a4 = piStack_194;
    piVar14 = extraout_EDX;
    piStack_19c = piVar13;
    if ((iStack_1a8 != 0) && (9 < (int)local_198)) {
      FUN_004974c0(&DAT_0065def0,(cls_0x49ce00 *)&piStack_1a4);
      piVar14 = extraout_EDX_00;
    }
    this->mbr_0x20 = 0x20;
    if ((iStack_1a8 == 0) || ((int)local_198 < 0xb)) {
      if ((LPCVOID)this->mbr_0x1c != (LPCVOID)0x0) {
        FUN_004830f0((LPCVOID)this->mbr_0x1c);
        piVar14 = extraout_EDX_03;
      }
      this->mbr_0x1c = 0;
      this->mbr_0x18 = 0;
    }
    else {
      piVar13 = (int *)*piStack_1a0;
      piStack_1a0 = piStack_1a0 + 1;
      this->mbr_0x18 = (dword)piVar13;
      piVar14 = piVar13;
      if (0x20 < (int)piVar13) {
        do {
          piVar14 = (int *)(this->mbr_0x20 + 0x20);
          this->mbr_0x20 = (dword)piVar14;
        } while ((int)piVar14 < (int)piVar13);
      }
      if ((LPCVOID)this->mbr_0x1c != (LPCVOID)0x0) {
        FUN_004830f0((LPCVOID)this->mbr_0x1c);
        piVar14 = extraout_EDX_01;
      }
      this->mbr_0x1c = 0;
      if (0 < (int)this->mbr_0x18) {
        puVar7 = FUN_00482fb0(this->mbr_0x20 << 3);
        dVar2 = this->mbr_0x18;
        iVar12 = 0;
        this->mbr_0x1c = (dword)puVar7;
        piVar14 = extraout_EDX_02;
        if (0 < (int)dVar2) {
          do {
            if (1 < iStack_18c) {
              puVar7[iVar12 * 2] = *piStack_1a0;
              piStack_1a0 = piStack_1a0 + 1;
              puVar7[iVar12 * 2 + 1] = *piStack_1a0;
            }
            piVar14 = piStack_1a0 + 1;
            iVar12 = iVar12 + 1;
            piStack_1a0 = piVar14;
          } while (iVar12 < (int)dVar2);
        }
      }
    }
    iVar12 = iStack_18c;
    if (iStack_18c < 2) {
      if ((LPCVOID)this->mbr_0x1c != (LPCVOID)0x0) {
        FUN_004830f0((LPCVOID)this->mbr_0x1c);
        piVar14 = extraout_EDX_04;
      }
      this->mbr_0x1c = 0;
      this->mbr_0x18 = 0;
    }
    if (iVar12 < 1) {
      if (((iStack_1a8 != 0) && (0xc < (int)local_198)) &&
         (piVar13 = (int *)((int)piStack_1a0 + (0x50 - (int)piStack_1a4)), piVar14 = piStack_19c,
         piVar13 <= piStack_19c)) {
        piStack_1a0 = (int *)((int)piVar13 + (int)piStack_1a4);
      }
      if ((-1 < DAT_0065a780) && (iVar3 = *(int *)(DAT_0065a77c + DAT_0065a780 * 4), iVar3 != 0)) {
        _strncpy(acStack_160,(char *)(iVar3 + 0x58),0x1f);
        uStack_141 = 0;
        piVar14 = extraout_EDX_05;
      }
    }
    if (iStack_1a8 != 0) {
      if (iVar12 < 1) {
        pcVar8 = (cls_0x5b4f30_TPlayer *)
                 FUN_00471ce0_CreateObjectFromStream((cls_0x49ce00 *)&piStack_1a4,piVar14,
                              (int *)(cls_0x49ce00 *)&piStack_1a4,local_198,1);
        if (pcVar8 != (cls_0x5b4f30_TPlayer *)0x0) {
          meth_0x51f0a0_TPlayerManager_AddPlayer((cls_0x45f7c0 *)&DAT_0065a890,pcVar8);
          cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(pcVar8);
          if (iStack_190 == 0) {
            meth_0x51f060_TPlayerManager_SetMainPlayer((cls_0x45f7c0 *)&DAT_0065a890);
          }
        }
      }
      else {
        iVar12 = *piStack_1a0;
        piStack_1a0 = piStack_1a0 + 1;
        if (0 < iVar12) {
          do {
            pcVar8 = (cls_0x5b4f30_TPlayer *)
                     FUN_00471ce0_CreateObjectFromStream((cls_0x49ce00 *)&piStack_1a4,piVar14,
                                  (int *)(cls_0x49ce00 *)&piStack_1a4,local_198,1);
            piVar14 = extraout_EDX_06;
            if (pcVar8 != (cls_0x5b4f30_TPlayer *)0x0) {
              meth_0x51f0a0_TPlayerManager_AddPlayer((cls_0x45f7c0 *)&DAT_0065a890,pcVar8);
              cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(pcVar8);
              piVar14 = extraout_EDX_07;
              if (iStack_190 == 0) {
                meth_0x51f060_TPlayerManager_SetMainPlayer((cls_0x45f7c0 *)&DAT_0065a890);
                piVar14 = extraout_EDX_08;
              }
            }
            iVar12 = iVar12 + -1;
          } while (iVar12 != 0);
        }
      }
    }
    if (piStack_194 != (int *)0x0) {
      FUN_00482f80(piStack_194);
    }
    iVar12 = meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
    if (iVar12 < 1) {
      iStack_1a8 = 0;
    }
    iVar12 = cls_0x4609f0::meth_0x460d60((cls_0x4609f0 *)&UNK_0065a630.field_0x138,acStack_160);
    if (iVar12 < 0) {
      iStack_1a8 = 0;
    }
    else {
      cls_0x4609f0::meth_0x4609f0((cls_0x4609f0 *)&UNK_0065a630.field_0x138);
    }
    TCharacter::meth_0x47e950((TCharacter *)&DAT_0065caf0);
    UNK_0065cb3c._4_4_ = 1;
    TCharacter::meth_0x47c580((TCharacter *)&DAT_0065caf0);
    (this->cls_0x41c7f0).mbr_0x14 = 0;
    DAT_0065a254 = 0;
    return iStack_1a8;
  }
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  return 0;
}



// Function at 0048e5b0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48e5b0(cls_0x45f7c0 *this,uint param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar1 = (this->cls_0x41c7f0).mbr_0x0;
  uVar2 = 0;
  if ((0 < (int)uVar1) && (param_1 < uVar1)) {
    uVar2 = **(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4);
  }
  meth_0x48df70_LoadGame(this,uVar2);
  return;
}



// Function at 0048e5e0

uint __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48e5e0(cls_0x45f7c0 *this)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint in_stack_00000004;
  
  uVar1 = (this->cls_0x41c7f0).mbr_0x0;
  uVar3 = 0;
  if ((0 < (int)uVar1) && (in_stack_00000004 < uVar1)) {
    uVar3 = **(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
  }
  iVar2 = meth_0x48df70_LoadGame(this,uVar3);
  return -(uint)(iVar2 != 0) & 0x668300;
}



// Function at 0048e760

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48e760(cls_0x45f7c0 *this)

{
  dword dVar1;
  LPCVOID *ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d8f5;
  local_c = ExceptionList;
  local_4 = 0;
  ExceptionList = &local_c;
  if ((LPCVOID)this->mbr_0x1c != (LPCVOID)0x0) {
    ExceptionList = &local_c;
    FUN_004830f0((LPCVOID)this->mbr_0x1c);
  }
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  iVar3 = 0;
  this->mbr_0x1c = 0;
  if (0 < (int)dVar1) {
    do {
      if ((-1 < iVar3) &&
         (ppvVar2 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4),
         ppvVar2 != (LPCVOID *)0x0)) {
        FUN_00482f80(*ppvVar2);
        FUN_00482f80(ppvVar2[1]);
        FUN_00482f80(ppvVar2[2]);
        FUN_004830f0(ppvVar2);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0048e820

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x48e820(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     ppvVar1 != (LPCVOID *)0x0)) {
    FUN_00482f80(*ppvVar1);
    FUN_00482f80(ppvVar1[1]);
    FUN_00482f80(ppvVar1[2]);
    FUN_004830f0(ppvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 00497ed0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x497ed0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dac8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00497f60

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x497f60(cls_0x45f7c0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *in_stack_00000004;
  
  puVar1 = FUN_00482fb0(0x50);
  for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    puVar1 = puVar1 + 1;
  }
  meth_0x41c840(this);
  return;
}



// Function at 00497f90

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x497f90(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 0049a100

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49a100(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dba8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0049a1a0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49a1a0(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     pvVar1 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 0049ceb0

/* WARNING: Removing unreachable block (ram,0x0049d0f0) */
/* WARNING: Removing unreachable block (ram,0x0049d13a) */
/* WARNING: Removing unreachable block (ram,0x0049d140) */
/* WARNING: Removing unreachable block (ram,0x0049d124) */
/* WARNING: Removing unreachable block (ram,0x0049d154) */
/* WARNING: Removing unreachable block (ram,0x0049d157) */

undefined4 __thiscall
OOAnalyzer::cls_0x45f7c0::meth_0x49ceb0
          (cls_0x45f7c0 *this,undefined4 param_1,undefined4 param_2,undefined param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined1 param_7,
          undefined4 param_8,undefined param_9,undefined1 param_10,undefined param_11,
          undefined param_12,undefined param_13)

{
  dword *this_00;
  undefined4 *puVar1;
  uint **ppuVar2;
  uint *puVar3;
  cls_0x45f7c0 *this_01;
  int iVar4;
  void *in_stack_00001174;
  void *local_c;
  undefined *local_8;
  FILE **local_4;
  
  local_4 = (FILE **)0xffffffff;
  local_8 = &LAB_0059dc0c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058c030();
  iVar4 = 0;
  if (0 < (int)(this_01->cls_0x41c7f0).mbr_0x0) {
    do {
      meth_0x49d920(this_01);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(this_01->cls_0x41c7f0).mbr_0x0);
  }
  this_00 = &(this_01->cls_0x41c7f0).mbr_0x14;
  (this_01->cls_0x41c7f0).mbr_0x0 = 0;
  iVar4 = 0;
  (this_01->cls_0x41c7f0).mbr_0x4 = 0;
  if (0 < (int)*this_00) {
    do {
      meth_0x49d920((cls_0x45f7c0 *)this_00);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*this_00);
  }
  *this_00 = 0;
  this_01->mbr_0x18 = 0;
  FUN_0058b100(&stack0x00000074,(byte *)s__s_s_def_005daa70);
  cls_0x478720::cls_0x478720((cls_0x478720 *)&param_11);
  local_c = (void *)0x0;
  local_8 = (undefined *)0x0;
  local_4 = (FILE **)0x0;
  puVar1 = FUN_00482fb0(0x2000);
  *(undefined *)puVar1 = 0;
  iVar4 = FUN_004789c0((int)&local_c,(cls_0x5a36f8__vftable_5a36f8 *)&stack0x00000074);
  if (iVar4 == 0) {
    FUN_004830f0(puVar1);
    if (local_8 == (undefined *)0x0) {
      if (local_4 != (FILE **)0x0) {
        FUN_004830f0((LPCVOID)0x0);
        FUN_004a1540(local_4);
      }
    }
    else {
      FUN_004830f0((LPCVOID)0x0);
      FUN_004830f0(local_8);
    }
    cls_0x478720::meth_0x478730((cls_0x478720 *)&param_11);
    ExceptionList = in_stack_00001174;
    return 0;
  }
  iVar4 = FUN_00479450((uint *)&local_c);
  if (iVar4 == 0) {
    FUN_00479950(&local_c,(byte *)s_Syntax_error_in_header_005daa7c);
  }
  do {
    iVar4 = FUN_0047a410((uint *)&local_c,(float **)s__63t__4091s_005daa94);
    if (iVar4 == 0) {
      FUN_00479950(&local_c,(byte *)s_tag__line__expected_005daaa0);
    }
    FUN_00479950(&local_c,(byte *)s_RETURN_expected_005daab4);
    FUN_0059be72((uint *)&param_13);
    ppuVar2 = (uint **)FUN_00482fb0(8);
    if (ppuVar2 == (uint **)0x0) {
      ppuVar2 = (uint **)0x0;
    }
    else {
      ppuVar2[1] = (uint *)0x0;
      *ppuVar2 = (uint *)0x0;
    }
    puVar3 = FUN_0059b6bc((uint *)&param_13);
    *ppuVar2 = puVar3;
    puVar3 = FUN_0059b6bc((uint *)&stack0x00000178);
    ppuVar2[1] = puVar3;
    meth_0x41c840(this_01);
    FUN_00479450((uint *)&local_c);
  } while( true );
}



// Function at 0049d1e0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d1e0(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  int iVar2;
  dword *this_00;
  
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4),
         ppvVar1 != (LPCVOID *)0x0)) {
        if (*ppvVar1 != (LPCVOID)0x0) {
          FUN_00482f80(*ppvVar1);
        }
        if (ppvVar1[1] != (LPCVOID)0x0) {
          FUN_00482f80(ppvVar1[1]);
        }
        FUN_004830f0(ppvVar1);
      }
      meth_0x41cb40(this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  this_00 = &(this->cls_0x41c7f0).mbr_0x14;
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      if ((-1 < iVar2) &&
         (ppvVar1 = *(LPCVOID **)(this->mbr_0x24 + iVar2 * 4), ppvVar1 != (LPCVOID *)0x0)) {
        if (*ppvVar1 != (LPCVOID)0x0) {
          FUN_00482f80(*ppvVar1);
        }
        if (ppvVar1[1] != (LPCVOID)0x0) {
          FUN_00482f80(ppvVar1[1]);
        }
        FUN_004830f0(ppvVar1);
      }
      meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x18 = 0;
  return;
}



// Function at 0049d6d0

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d6d0(cls_0x45f7c0 *this)

{
  uint uVar1;
  int iVar2;
  uint **in_stack_00000004;
  uint *local_30;
  undefined4 local_2c;
  uint local_28 [9];
  undefined local_1;
  
  _strncpy((char *)local_28,(char *)in_stack_00000004,0x27);
  local_1 = 0;
  FUN_0059be72(local_28);
  local_30 = local_28;
  in_stack_00000004 = &local_30;
  local_2c = 0;
  uVar1 = FUN_0058e8bb(&stack0x00000004,(this->cls_0x41c7f0).mbr_0x10,(this->cls_0x41c7f0).mbr_0x0,4
                       ,&LAB_0049d190);
  if ((uVar1 == 0) || (iVar2 = (int)(uVar1 - (this->cls_0x41c7f0).mbr_0x10) >> 2, iVar2 < 0)) {
    in_stack_00000004 = &local_30;
    uVar1 = FUN_0058e8bb(&stack0x00000004,this->mbr_0x24,(this->cls_0x41c7f0).mbr_0x14,4,
                         &LAB_0049d190);
    if (uVar1 == 0) {
      return -1;
    }
    iVar2 = (int)(uVar1 - this->mbr_0x24) >> 2;
    if (-1 < iVar2) {
      iVar2 = iVar2 + (this->cls_0x41c7f0).mbr_0x0;
    }
  }
  return iVar2;
}



// Function at 0049d780

char * __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d780(cls_0x45f7c0 *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  if ((this->cls_0x41c7f0).mbr_0x14 + dVar1 <= in_stack_00000004) {
    return s__badid__005dab4c;
  }
  if ((int)in_stack_00000004 < (int)dVar1) {
    return *(char **)(*(int *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4) + 4);
  }
  return *(char **)(*(int *)(this->mbr_0x24 + (in_stack_00000004 - dVar1) * 4) + 4);
}



// Function at 0049d7c0

char * __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d7c0(cls_0x45f7c0 *this)

{
  dword dVar1;
  uint in_stack_00000004;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  if ((this->cls_0x41c7f0).mbr_0x14 + dVar1 <= in_stack_00000004) {
    return s__badid__005dab54;
  }
  if ((int)in_stack_00000004 < (int)dVar1) {
    return **(char ***)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
  }
  return **(char ***)(this->mbr_0x24 + (in_stack_00000004 - dVar1) * 4);
}



// Function at 0049d800

undefined4 __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d800(cls_0x45f7c0 *this)

{
  int *piVar1;
  int iVar2;
  uint **in_stack_00000004;
  uint **local_40;
  uint *local_3c;
  LPCVOID local_38;
  uint local_34 [9];
  undefined local_d;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dc54;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  _strncpy((char *)local_34,(char *)in_stack_00000004,0x27);
  local_d = 0;
  FUN_0059be72(local_34);
  local_3c = local_34;
  in_stack_00000004 = &local_3c;
  local_38 = (LPCVOID)0x0;
  piVar1 = (int *)FUN_0058e8bb(&stack0x00000004,(this->cls_0x41c7f0).mbr_0x10,
                               (this->cls_0x41c7f0).mbr_0x0,4,&LAB_0049d190);
  if ((piVar1 == (int *)0x0) || (iVar2 = *piVar1, iVar2 == 0)) {
    local_40 = &local_3c;
    piVar1 = (int *)FUN_0058e8bb(&local_40,this->mbr_0x24,(this->cls_0x41c7f0).mbr_0x14,4,
                                 &LAB_0049d190);
    if (piVar1 == (int *)0x0) {
      iVar2 = 0;
    }
    else {
      iVar2 = *piVar1;
    }
  }
  local_38 = (LPCVOID)0x0;
  local_3c = (uint *)0x0;
  if (iVar2 != 0) {
    ExceptionList = local_c;
    return *(undefined4 *)(iVar2 + 4);
  }
  FUN_0058b100(&UNK_006687ac.field_0x14,&DAT_005dab5c);
  local_4 = 0xffffffff;
  if (local_3c != (uint *)0x0) {
    FUN_00482f80(local_3c);
  }
  if (local_38 != (LPCVOID)0x0) {
    FUN_00482f80(local_38);
  }
  ExceptionList = local_c;
  return 0x6687c0;
}



// Function at 0049d920

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x49d920(cls_0x45f7c0 *this)

{
  LPCVOID *ppvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (ppvVar1 = *(LPCVOID **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     ppvVar1 != (LPCVOID *)0x0)) {
    if (*ppvVar1 != (LPCVOID)0x0) {
      FUN_00482f80(*ppvVar1);
    }
    if (ppvVar1[1] != (LPCVOID)0x0) {
      FUN_00482f80(ppvVar1[1]);
    }
    FUN_004830f0(ppvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 004abb70

undefined4 __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4abb70(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  cls_0x45f7c0 *pcVar2;
  bool bVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  undefined3 extraout_var;
  uint uVar6;
  uint *puVar7;
  dword dVar8;
  undefined3 extraout_var_00;
  int *piVar9;
  int *piVar10;
  int iVar11;
  dword extraout_ECX;
  int *piVar12;
  dword extraout_ECX_00;
  dword dVar13;
  uint *puVar14;
  undefined4 *puVar15;
  cls_0x419dd0 cVar16;
  cls_0x419dd0 cVar17;
  undefined local_2c4 [44];
  undefined4 local_298 [7];
  undefined local_279;
  undefined4 local_278;
  undefined4 *local_270;
  undefined4 *local_26c;
  undefined4 *local_268;
  undefined4 *local_264;
  HFONT local_260;
  undefined4 local_25c;
  dword local_250;
  dword local_244;
  dword local_240;
  uint local_238;
  LPCVOID local_234;
  FILE **local_230;
  LPCVOID local_22c;
  int local_228;
  undefined4 local_224;
  dword local_220;
  dword local_218;
  dword local_214;
  undefined4 *local_210;
  undefined local_20c;
  dword local_208;
  cls_0x478720 local_200;
  uint local_1f8 [8];
  undefined local_1d5;
  undefined4 *local_1c8;
  byte local_1c4 [32];
  char local_1a4 [32];
  char local_184 [32];
  byte local_164 [32];
  undefined4 local_144;
  char acStack_140 [31];
  undefined uStack_121;
  dword dStack_120;
  dword dStack_11c;
  DWORD DStack_118;
  HFONT pHStack_114;
  cls_0x5a36f8__vftable_5a36f8 local_110 [13];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dfb8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  local_2c4._36_4_ = this;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)local_2c4,0x10);
  local_4 = 0;
  local_2c4._20_4_ = FUN_00482fb0(0x34);
  piVar10 = (int *)local_2c4._20_4_;
  for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
    *piVar10 = 0;
    piVar10 = piVar10 + 1;
  }
  local_4 = 1;
  meth_0x4ad030(this);
  puVar15 = local_298;
  for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
    *puVar15 = 0;
    puVar15 = puVar15 + 1;
  }
  _strncpy((char *)local_298,&DAT_005dece4,0x1f);
  local_279 = 0;
  local_278 = 0;
  puVar4 = FUN_00482fb0(0x60);
  puVar15 = local_298;
  for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
    *puVar4 = *puVar15;
    puVar15 = puVar15 + 1;
    puVar4 = puVar4 + 1;
  }
  meth_0x41c840((cls_0x45f7c0 *)local_2c4._36_4_);
  FUN_0058b100((char *)local_110,&DAT_005decf8);
  cls_0x478720::cls_0x478720(&local_200);
  local_4._0_1_ = 2;
  local_238 = 0;
  local_234 = (LPCVOID)0x0;
  local_230 = (FILE **)0x0;
  local_22c = (LPCVOID)0x0;
  local_228 = 0;
  local_224 = 0;
  local_220 = 0;
  local_218 = 0;
  local_214 = 0;
  local_20c = 0;
  local_208 = 1;
  local_210 = FUN_00482fb0(0x2000);
  *(undefined *)local_210 = 0;
  local_4._0_1_ = 3;
  iVar11 = FUN_004789c0((int)&local_238,local_110);
  if (iVar11 == 0) {
    local_4 = CONCAT31(local_4._1_3_,4);
    FUN_004830f0(local_210);
    if (local_234 == (LPCVOID)0x0) {
      if (local_230 != (FILE **)0x0) {
        FUN_004830f0(local_22c);
        FUN_004a1540(local_230);
      }
    }
    else {
      FUN_004830f0(local_22c);
      FUN_004830f0(local_234);
    }
    local_4 = CONCAT31(local_4._1_3_,1);
    cls_0x478720::meth_0x478730(&local_200);
    local_4 = 5;
    cls_0x41c7f0::meth_0x4acfe0((cls_0x41c7f0 *)local_2c4);
    FUN_004830f0((LPCVOID)local_2c4._20_4_);
    local_4 = 0xffffffff;
    FUN_004830f0((LPCVOID)local_2c4._16_4_);
    uVar5 = 0;
  }
  else {
    iVar11 = FUN_00479450(&local_238);
    if (iVar11 == 0) {
      FUN_00479950(&local_238,(byte *)s_Syntax_error_in_header_005ded00);
    }
    while (local_228 != 10) {
      bVar3 = FUN_00479700((uint)&local_238,(byte *)s_BMFONT_005ded18,0);
      if (CONCAT31(extraout_var,bVar3) == 0) {
        bVar3 = FUN_00479700((uint)&local_238,(byte *)s_WINFONT_005dede8,0);
        if (CONCAT31(extraout_var_00,bVar3) == 0) {
          FUN_00479950(&local_238,(byte *)s_Bad_tag_in_FONT_DEF_005deeb8);
        }
        else {
          puVar15 = local_298;
          for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar15 = 0;
            puVar15 = puVar15 + 1;
          }
          local_1c4[0] = 0;
          local_278 = 2;
          local_2c4._24_4_ = 0;
          local_2c4._32_4_ = 0;
          iVar11 = FUN_0047a410(&local_238,(float **)s_WINFONT__32s_FONT__32s__i_<BOLD>_005dedf0);
          if (iVar11 == 0) {
            FUN_00479950(&local_238,(byte *)s_Unable_to_parse_WINFONT_tag_in_F_005dee6c);
          }
          iVar11 = 0;
          dVar13 = local_2c4._24_4_;
          piVar10 = (int *)local_2c4._20_4_;
          if (0 < (int)local_2c4._0_4_) {
            do {
              piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
              piVar12 = piVar9;
              if (piVar9 == (int *)0x0) {
                piVar12 = piVar10;
              }
              if (*piVar12 == 2) {
                if (piVar9 == (int *)0x0) {
                  piVar9 = piVar10;
                }
                uVar6 = FUN_0059a530_stricmp((uint)local_1c4,(byte *)(piVar9 + 1),local_1c4);
                dVar13 = local_2c4._24_4_;
                piVar10 = (int *)local_2c4._20_4_;
                if (uVar6 == 0) {
                  piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                  piVar12 = piVar9;
                  if (piVar9 == (int *)0x0) {
                    piVar12 = (int *)local_2c4._20_4_;
                  }
                  if (piVar12[9] == local_2c4._40_4_) {
                    piVar12 = piVar9;
                    if (piVar9 == (int *)0x0) {
                      piVar12 = (int *)local_2c4._20_4_;
                    }
                    if (piVar12[10] == local_2c4._24_4_) {
                      if (piVar9 == (int *)0x0) {
                        piVar9 = (int *)local_2c4._20_4_;
                      }
                      if (piVar9[0xb] == local_2c4._32_4_) {
                        piVar10 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                        if (piVar10 == (int *)0x0) {
                          piVar10 = (int *)local_2c4._20_4_;
                        }
                        local_260 = (HFONT)piVar10[0xc];
                        if (iVar11 < (int)local_2c4._0_4_) goto LAB_004ac5a8;
                        break;
                      }
                    }
                  }
                }
              }
              iVar11 = iVar11 + 1;
            } while (iVar11 < (int)local_2c4._0_4_);
          }
          puVar15 = &local_144;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar15 = 0;
            puVar15 = puVar15 + 1;
          }
          local_260 = CreateFontA(local_2c4._40_4_,0,0,0,(-(uint)(dVar13 != 0) & 300) + 400,
                                  local_2c4._32_4_,0,0,1,0,0,0,0,(LPCSTR)local_1c4);
          pHStack_114 = local_260;
          if (local_260 == (HFONT)0x0) {
            FUN_00481c10((byte *)s_Unable_to_create_Windows_font__s_005dee94);
          }
          local_144 = 2;
          _strncpy(acStack_140,(char *)local_1c4,0x1f);
          uStack_121 = 0;
          dStack_120 = local_2c4._40_4_;
          dStack_11c = local_2c4._24_4_;
          DStack_118 = local_2c4._32_4_;
          puVar4 = FUN_00482fb0(0x34);
          puVar15 = &local_144;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar4 = *puVar15;
            puVar15 = puVar15 + 1;
            puVar4 = puVar4 + 1;
          }
          meth_0x41c840((cls_0x45f7c0 *)local_2c4);
LAB_004ac5a8:
          pcVar2 = (cls_0x45f7c0 *)local_2c4._36_4_;
          iVar11 = meth_0x4aca60((cls_0x45f7c0 *)local_2c4._36_4_);
          local_2c4._28_4_ = &stack0xfffffd24;
          cVar17.mbr_0x0 = local_250;
          cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffd24);
          local_2c4._28_4_ = &stack0xfffffd20;
          cVar16.mbr_0x0 = extraout_ECX_00;
          cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffd20);
          meth_0x4ad080(pcVar2,iVar11,cVar16.mbr_0x0,cVar17.mbr_0x0);
          dVar13 = *(dword *)((pcVar2->cls_0x41c7f0).mbr_0x10 + iVar11 * 4);
          if (dVar13 == 0) {
            dVar13 = (pcVar2->cls_0x41c7f0).mbr_0x14;
          }
          *(dword *)(dVar13 + 0x58) = local_240;
        }
      }
      else {
        puVar15 = local_298;
        for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
          *puVar15 = 0;
          puVar15 = puVar15 + 1;
        }
        local_1c4[0] = 0;
        local_1a4[0] = '\0';
        local_164[0] = 0;
        local_184[0] = '\0';
        local_278 = 1;
        local_25c = 0x1110;
        iVar11 = FUN_0047a410(&local_238,(float **)s_BMFONT__32s_<NORMAL__32s>_<SHADO_005ded20);
        if (iVar11 == 0) {
          FUN_00479950(&local_238,(byte *)s_Unable_to_parse_BMFONT_tag_in_FO_005dedc0);
        }
        if (local_1c4[0] != 0) {
          iVar11 = 0;
          piVar10 = (int *)local_2c4._20_4_;
          if (0 < (int)local_2c4._0_4_) {
            do {
              piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
              piVar12 = piVar9;
              if (piVar9 == (int *)0x0) {
                piVar12 = piVar10;
              }
              if (*piVar12 == 1) {
                if (piVar9 == (int *)0x0) {
                  piVar9 = piVar10;
                }
                uVar6 = FUN_0059a530_stricmp((uint)piVar12,(byte *)(piVar9 + 1),local_1c4);
                piVar10 = (int *)local_2c4._20_4_;
                if (uVar6 == 0) {
                  piVar10 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                  if (piVar10 == (int *)0x0) {
                    piVar10 = (int *)local_2c4._20_4_;
                  }
                  local_270 = (undefined4 *)piVar10[0xc];
                  if (iVar11 < (int)local_2c4._0_4_) goto LAB_004abfb6;
                  break;
                }
              }
              iVar11 = iVar11 + 1;
            } while (iVar11 < (int)local_2c4._0_4_);
          }
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          cVar16.mbr_0x0 = (dword)(local_1f8 + 1);
          local_1f8[0] = 1;
          _strncpy((char *)cVar16.mbr_0x0,(char *)local_1c4,0x1f);
          local_1d5 = 0;
          local_270 = FUN_0047f670((cls_0x49ead0 *)local_1c4,(void *)0xffffffff,0,
                                   (uint *)cVar16.mbr_0x0);
          local_1c8 = local_270;
          puVar7 = FUN_00482fb0(0x34);
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar7 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar7 = puVar7 + 1;
          }
          meth_0x41c840((cls_0x45f7c0 *)local_2c4);
        }
LAB_004abfb6:
        if (local_1a4[0] != '\0') {
          iVar11 = 0;
          piVar10 = (int *)local_2c4._20_4_;
          if (0 < (int)local_2c4._0_4_) {
            do {
              piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
              piVar12 = piVar9;
              if (piVar9 == (int *)0x0) {
                piVar12 = piVar10;
              }
              if (*piVar12 == 1) {
                if (piVar9 == (int *)0x0) {
                  piVar9 = piVar10;
                }
                uVar6 = FUN_0059a530_stricmp((uint)local_1a4,(byte *)(piVar9 + 1),(byte *)local_1a4);
                piVar10 = (int *)local_2c4._20_4_;
                if (uVar6 == 0) {
                  piVar10 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                  if (piVar10 == (int *)0x0) {
                    piVar10 = (int *)local_2c4._20_4_;
                  }
                  local_26c = (undefined4 *)piVar10[0xc];
                  if (iVar11 < (int)local_2c4._0_4_) goto LAB_004ac0a8;
                  break;
                }
              }
              iVar11 = iVar11 + 1;
            } while (iVar11 < (int)local_2c4._0_4_);
          }
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          cVar16.mbr_0x0 = (dword)(local_1f8 + 1);
          local_1f8[0] = 1;
          _strncpy((char *)cVar16.mbr_0x0,local_1a4,0x1f);
          local_1d5 = 0;
          local_26c = FUN_0047f670((cls_0x49ead0 *)local_1a4,(void *)0xffffffff,0,
                                   (uint *)cVar16.mbr_0x0);
          local_1c8 = local_26c;
          puVar7 = FUN_00482fb0(0x34);
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar7 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar7 = puVar7 + 1;
          }
          meth_0x41c840((cls_0x45f7c0 *)local_2c4);
        }
LAB_004ac0a8:
        if (local_164[0] != 0) {
          iVar11 = 0;
          piVar10 = (int *)local_2c4._20_4_;
          if (0 < (int)local_2c4._0_4_) {
            do {
              piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
              piVar12 = piVar9;
              if (piVar9 == (int *)0x0) {
                piVar12 = piVar10;
              }
              if (*piVar12 == 1) {
                if (piVar9 == (int *)0x0) {
                  piVar9 = piVar10;
                }
                uVar6 = FUN_0059a530_stricmp((uint)piVar12,(byte *)(piVar9 + 1),local_164);
                piVar10 = (int *)local_2c4._20_4_;
                if (uVar6 == 0) {
                  piVar10 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                  if (piVar10 == (int *)0x0) {
                    piVar10 = (int *)local_2c4._20_4_;
                  }
                  local_268 = (undefined4 *)piVar10[0xc];
                  if (iVar11 < (int)local_2c4._0_4_) goto LAB_004ac19a;
                  break;
                }
              }
              iVar11 = iVar11 + 1;
            } while (iVar11 < (int)local_2c4._0_4_);
          }
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          cVar16.mbr_0x0 = (dword)(local_1f8 + 1);
          local_1f8[0] = 1;
          _strncpy((char *)cVar16.mbr_0x0,(char *)local_164,0x1f);
          local_1d5 = 0;
          local_268 = FUN_0047f670((cls_0x49ead0 *)local_164,(void *)0xffffffff,0,
                                   (uint *)cVar16.mbr_0x0);
          local_1c8 = local_268;
          puVar7 = FUN_00482fb0(0x34);
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar7 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar7 = puVar7 + 1;
          }
          meth_0x41c840((cls_0x45f7c0 *)local_2c4);
        }
LAB_004ac19a:
        if (local_184[0] != '\0') {
          iVar11 = 0;
          piVar10 = (int *)local_2c4._20_4_;
          if (0 < (int)local_2c4._0_4_) {
            do {
              piVar9 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
              piVar12 = piVar9;
              if (piVar9 == (int *)0x0) {
                piVar12 = piVar10;
              }
              if (*piVar12 == 1) {
                if (piVar9 == (int *)0x0) {
                  piVar9 = piVar10;
                }
                uVar6 = FUN_0059a530_stricmp((uint)local_184,(byte *)(piVar9 + 1),(byte *)local_184);
                piVar10 = (int *)local_2c4._20_4_;
                if (uVar6 == 0) {
                  piVar10 = *(int **)(local_2c4._16_4_ + iVar11 * 4);
                  if (piVar10 == (int *)0x0) {
                    piVar10 = (int *)local_2c4._20_4_;
                  }
                  local_264 = (undefined4 *)piVar10[0xc];
                  if (iVar11 < (int)local_2c4._0_4_) goto LAB_004ac286;
                  break;
                }
              }
              iVar11 = iVar11 + 1;
            } while (iVar11 < (int)local_2c4._0_4_);
          }
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = 0;
            puVar14 = puVar14 + 1;
          }
          cVar16.mbr_0x0 = (dword)(local_1f8 + 1);
          local_1f8[0] = 1;
          _strncpy((char *)cVar16.mbr_0x0,local_184,0x1f);
          local_1d5 = 0;
          local_264 = FUN_0047f670((cls_0x49ead0 *)local_184,(void *)0xffffffff,0,
                                   (uint *)cVar16.mbr_0x0);
          local_1c8 = local_264;
          puVar7 = FUN_00482fb0(0x34);
          puVar14 = local_1f8;
          for (iVar11 = 0xd; iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar7 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar7 = puVar7 + 1;
          }
          meth_0x41c840((cls_0x45f7c0 *)local_2c4);
        }
LAB_004ac286:
        pcVar2 = (cls_0x45f7c0 *)local_2c4._36_4_;
        if ((local_26c != (undefined4 *)0x0) && (local_270 == (undefined4 *)0x0)) {
          local_270 = local_26c;
        }
        iVar11 = meth_0x4ac990((cls_0x45f7c0 *)local_2c4._36_4_,local_298,local_270,local_26c,
                               local_268,local_264,local_25c);
        local_2c4._28_4_ = &stack0xfffffd24;
        cVar17.mbr_0x0 = local_250;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffd24);
        local_2c4._28_4_ = &stack0xfffffd20;
        cVar16.mbr_0x0 = extraout_ECX;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffd20);
        meth_0x4ad080(pcVar2,iVar11,cVar16.mbr_0x0,cVar17.mbr_0x0);
        dVar13 = *(dword *)((pcVar2->cls_0x41c7f0).mbr_0x10 + iVar11 * 4);
        dVar8 = dVar13;
        if (dVar13 == 0) {
          dVar8 = (pcVar2->cls_0x41c7f0).mbr_0x14;
        }
        *(dword *)(dVar8 + 0x54) = local_244;
        if (dVar13 == 0) {
          dVar13 = (pcVar2->cls_0x41c7f0).mbr_0x14;
        }
        *(dword *)(dVar13 + 0x58) = local_240;
      }
    }
    iVar11 = 0;
    local_228 = 10;
    if (0 < (int)local_2c4._0_4_) {
      do {
        if ((-1 < iVar11) &&
           (pvVar1 = *(LPCVOID *)(local_2c4._16_4_ + iVar11 * 4), pvVar1 != (LPCVOID)0x0)) {
          FUN_004830f0(pvVar1);
        }
        meth_0x41cb40((cls_0x45f7c0 *)local_2c4);
        iVar11 = iVar11 + 1;
      } while (iVar11 < (int)local_2c4._0_4_);
    }
    local_2c4._0_4_ = 0;
    local_2c4._4_4_ = 0;
    local_4 = CONCAT31(local_4._1_3_,6);
    FUN_004830f0(local_210);
    if (local_234 == (LPCVOID)0x0) {
      if (local_230 != (FILE **)0x0) {
        FUN_004830f0(local_22c);
        FUN_004a1540(local_230);
      }
    }
    else {
      FUN_004830f0(local_22c);
      FUN_004830f0(local_234);
    }
    local_4 = CONCAT31(local_4._1_3_,1);
    cls_0x478720::meth_0x478730(&local_200);
    iVar11 = 0;
    local_4 = 7;
    if (0 < (int)local_2c4._0_4_) {
      do {
        if ((-1 < iVar11) &&
           (pvVar1 = *(LPCVOID *)(local_2c4._16_4_ + iVar11 * 4), pvVar1 != (LPCVOID)0x0)) {
          FUN_004830f0(pvVar1);
        }
        meth_0x41cb40((cls_0x45f7c0 *)local_2c4);
        iVar11 = iVar11 + 1;
      } while (iVar11 < (int)local_2c4._0_4_);
    }
    local_2c4._0_4_ = 0;
    local_2c4._4_4_ = 0;
    FUN_004830f0((LPCVOID)local_2c4._20_4_);
    local_4 = 0xffffffff;
    FUN_004830f0((LPCVOID)local_2c4._16_4_);
    uVar5 = 1;
  }
  ExceptionList = local_c;
  return uVar5;
}



// Function at 004ac780

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4ac780(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  dword *pdVar9;
  dword *pdVar10;
  bool bVar11;
  LPCVOID local_8;
  uint local_4;
  
  dVar2 = (this->cls_0x41c7f0).mbr_0x0;
  local_4 = 0;
  if (0 < (int)dVar2) {
    do {
      pdVar10 = (dword *)(this->cls_0x41c7f0).mbr_0x10;
      if (((pdVar10 != (dword *)0x0) && (local_4 < dVar2)) && (dVar4 = pdVar10[local_4], dVar4 != 0)
         ) {
        dVar3 = dVar4;
        if (dVar4 == 0) {
          dVar3 = (this->cls_0x41c7f0).mbr_0x14;
        }
        if (*(int *)(dVar3 + 0x20) != 0) {
          if (dVar4 == 0) {
            dVar4 = (this->cls_0x41c7f0).mbr_0x14;
          }
          if (*(int *)(dVar4 + 0x20) == 1) {
            iVar8 = 0;
            do {
              if (iVar8 == 0) {
                local_8 = *(LPCVOID *)(dVar4 + 0x28);
              }
              else if (iVar8 == 1) {
                local_8 = *(LPCVOID *)(dVar4 + 0x2c);
              }
              else if (iVar8 == 2) {
                local_8 = *(LPCVOID *)(dVar4 + 0x30);
              }
              else if (iVar8 == 3) {
                local_8 = *(LPCVOID *)(dVar4 + 0x34);
              }
              uVar7 = (this->cls_0x41c7f0).mbr_0x0;
              uVar6 = 0;
              if (0 < (int)uVar7) {
                pdVar10 = (dword *)(this->cls_0x41c7f0).mbr_0x10;
                pdVar9 = pdVar10;
                do {
                  if (((pdVar10 != (dword *)0x0) && (uVar6 < uVar7)) &&
                     (dVar2 = *pdVar9, dVar2 != 0)) {
                    dVar3 = dVar2;
                    if (dVar2 == 0) {
                      dVar3 = (this->cls_0x41c7f0).mbr_0x14;
                    }
                    if (*(int *)(dVar3 + 0x20) == 1) {
                      if (dVar2 == 0) {
                        dVar2 = (this->cls_0x41c7f0).mbr_0x14;
                      }
                      if (*(LPCVOID *)(dVar2 + 0x28) == local_8) {
                        *(undefined4 *)(dVar2 + 0x28) = 0;
                      }
                      if (*(LPCVOID *)(dVar2 + 0x2c) == local_8) {
                        *(undefined4 *)(dVar2 + 0x2c) = 0;
                      }
                      if (*(LPCVOID *)(dVar2 + 0x30) == local_8) {
                        *(undefined4 *)(dVar2 + 0x30) = 0;
                      }
                      if (*(LPCVOID *)(dVar2 + 0x34) == local_8) {
                        *(undefined4 *)(dVar2 + 0x34) = 0;
                      }
                    }
                  }
                  uVar6 = uVar6 + 1;
                  pdVar9 = pdVar9 + 1;
                } while ((int)uVar6 < (int)uVar7);
              }
              FUN_004830f0(local_8);
              iVar8 = iVar8 + 1;
            } while (iVar8 < 4);
          }
          else if (*(int *)(dVar4 + 0x20) == 2) {
            iVar8 = *(int *)(dVar4 + 0x38);
            uVar7 = 0;
            if (0 < (int)dVar2) {
              bVar11 = dVar2 != 0;
              do {
                if ((bVar11) && (dVar3 = *pdVar10, dVar3 != 0)) {
                  dVar5 = dVar3;
                  if (dVar3 == 0) {
                    dVar5 = (this->cls_0x41c7f0).mbr_0x14;
                  }
                  if (*(int *)(dVar5 + 0x20) == 2) {
                    if (dVar3 == 0) {
                      dVar3 = (this->cls_0x41c7f0).mbr_0x14;
                    }
                    if (*(int *)(dVar3 + 0x38) == iVar8) {
                      *(undefined4 *)(dVar3 + 0x38) = 0;
                    }
                  }
                }
                uVar7 = uVar7 + 1;
                pdVar10 = pdVar10 + 1;
                bVar11 = uVar7 < dVar2;
              } while ((int)uVar7 < (int)dVar2);
            }
            DeleteObject(*(HGDIOBJ *)(dVar4 + 0x38));
            *(undefined4 *)(dVar4 + 0x38) = 0;
          }
        }
      }
      dVar2 = (this->cls_0x41c7f0).mbr_0x0;
      local_4 = local_4 + 1;
    } while ((int)local_4 < (int)dVar2);
  }
  iVar8 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar8) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar8 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 004ac990

void __thiscall
OOAnalyzer::cls_0x45f7c0::meth_0x4ac990
          (cls_0x45f7c0 *this,char *param_1,short *param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_60 [7];
  undefined local_41;
  undefined4 local_40;
  short *local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_24;
  undefined4 local_14;
  int local_10;
  undefined4 local_c;
  undefined4 local_8;
  int local_4;
  
  puVar2 = local_60;
  for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  local_40 = 1;
  _strncpy((char *)local_60,param_1,0x1f);
  local_34 = param_3;
  local_30 = param_4;
  local_2c = param_5;
  local_24 = param_6;
  local_41 = 0;
  local_38 = param_2;
  if (*(int *)(param_2 + 0x204) == 0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    puVar2 = (undefined4 *)(*(int *)(param_2 + 0x204) + 0x408 + (int)param_2);
  }
  local_14 = *puVar2;
  local_8 = 0;
  local_10 = (int)param_2[2];
  local_c = 0;
  local_4 = (int)param_2[0x44 - *param_2];
  meth_0x4acf20(this);
  return;
}



// Function at 004aca60

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4aca60(cls_0x45f7c0 *this)

{
  HDC hdc;
  HGDIOBJ h;
  int iVar1;
  undefined4 *puVar2;
  char *in_stack_00000004;
  HGDIOBJ in_stack_00000008;
  undefined4 auStack_98 [7];
  undefined uStack_79;
  undefined4 uStack_78;
  LONG LStack_4c;
  LONG LStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  LONG LStack_3c;
  tagTEXTMETRICA tStack_38;
  
  hdc = GetWindowDC((HWND)0x0);
  h = SelectObject(hdc,in_stack_00000008);
  GetTextMetricsA(hdc,&tStack_38);
  puVar2 = auStack_98;
  for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  uStack_78 = 2;
  _strncpy((char *)auStack_98,in_stack_00000004,0x1f);
  LStack_4c = tStack_38.tmAveCharWidth;
  uStack_79 = 0;
  LStack_48 = tStack_38.tmHeight;
  LStack_3c = tStack_38.tmAscent;
  uStack_40 = 0;
  uStack_44 = 0;
  SelectObject(hdc,h);
  ReleaseDC((HWND)0x0,hdc);
  meth_0x4acf20(this);
  return;
}



// Function at 004acb30

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4acb30(cls_0x45f7c0 *this)

{
  byte *pbVar1;
  uint uVar2;
  cls_0x45f7c0 *pcVar3;
  cls_0x45f7c0 *extraout_ECX;
  int iVar4;
  byte *in_stack_00000004;
  
  iVar4 = 0;
  pcVar3 = this;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      pbVar1 = *(byte **)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4);
      if (pbVar1 == (byte *)0x0) {
        pbVar1 = (byte *)(this->cls_0x41c7f0).mbr_0x14;
      }
      uVar2 = FUN_0059a530_stricmp((uint)pcVar3,pbVar1,in_stack_00000004);
      if (uVar2 == 0) {
        return iVar4;
      }
      iVar4 = iVar4 + 1;
      pcVar3 = extraout_ECX;
    } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return 0;
}



// Function at 004acf20

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4acf20(cls_0x45f7c0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *in_stack_00000004;
  
  puVar1 = FUN_00482fb0(0x60);
  for (iVar2 = 0x18; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    puVar1 = puVar1 + 1;
  }
  meth_0x41c840(this);
  return;
}



// Function at 004acf50

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4acf50(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dfd8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 004ad030

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4ad030(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 004ad080

void __thiscall
OOAnalyzer::cls_0x45f7c0::meth_0x4ad080
          (cls_0x45f7c0 *this,int param_1,undefined4 param_2,undefined4 param_3)

{
  dword dVar1;
  dword dVar2;
  undefined4 in_stack_00000010;
  
  dVar1 = *(dword *)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4);
  dVar2 = dVar1;
  if (dVar1 == 0) {
    dVar2 = (this->cls_0x41c7f0).mbr_0x14;
  }
  *(undefined4 *)(dVar2 + 0x40) = param_2;
  dVar2 = dVar1;
  if (dVar1 == 0) {
    dVar2 = (this->cls_0x41c7f0).mbr_0x14;
  }
  *(undefined4 *)(dVar2 + 0x44) = param_3;
  if (dVar1 != 0) {
    *(undefined4 *)(dVar1 + 0x48) = in_stack_00000010;
    return;
  }
  *(undefined4 *)((this->cls_0x41c7f0).mbr_0x14 + 0x48) = in_stack_00000010;
  return;
}



// Function at 004d6d00

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x4d6d00(cls_0x45f7c0 *this)

{
  undefined4 *puVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ea28;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (puVar1 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4),
         puVar1 != (undefined4 *)0x0)) {
        (**(code **)*puVar1)(1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0051ed10

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51ed10(cls_0x45f7c0 *this)

{
  dword *this_00;
  undefined4 *puVar1;
  LPCVOID pvVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (puVar1 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4),
         puVar1 != (undefined4 *)0x0)) {
        (**(code **)*puVar1)(1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  this_00 = &(this->cls_0x41c7f0).mbr_0x14;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  iVar3 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x24 + iVar3 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
      }
      meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x28 = 0xffffffff;
  DAT_00667fcc = 0;
  return;
}



// Function at 0051eda0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51eda0_TPlayerManager_Clear(cls_0x45f7c0 *this)

{
  dword dVar1;
  cls_0x5b4f30_TPlayer *this_00;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  
  uVar4 = 0;
  uVar3 = (this->cls_0x41c7f0).mbr_0x0;
  if (0 < (int)uVar3) {
    do {
      dVar1 = (this->cls_0x41c7f0).mbr_0x10;
      if ((((dVar1 != 0) && (uVar4 < uVar3)) &&
          (this_00 = *(cls_0x5b4f30_TPlayer **)(dVar1 + uVar4 * 4), this_00 != (cls_0x5b4f30_TPlayer *)0x0)) &&
         (this_00 != (cls_0x5b4f30_TPlayer *)0x0)) {
        TPlayScreen::meth_0x4c18a0_TCharacter_ClearChar(&this_00->TPlayScreen);
        cls_0x5b4f30_TPlayer::meth_0x518750_TPlayer_ClearPlayer(this_00);
      }
      uVar3 = (this->cls_0x41c7f0).mbr_0x0;
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)uVar3);
  }
  meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
  iVar5 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar5) &&
         (puVar2 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4),
         puVar2 != (undefined4 *)0x0)) {
        (**(code **)*puVar2)(1);
      }
      meth_0x41cb40(this);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  return;
}



// Function at 0051ee30

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51ee30(cls_0x45f7c0 *this)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
  puVar3 = (undefined4 *)(this->cls_0x41c7f0).mbr_0x10;
  if (puVar3 != (undefined4 *)0x0) {
    for (uVar1 = (this->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
    (this->cls_0x41c7f0).mbr_0x4 = 0;
    (this->cls_0x41c7f0).mbr_0x0 = 0;
    return;
  }
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  return;
}



// Function at 0051ee70

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers(cls_0x45f7c0 *this)

{
  dword dVar1;
  int *piVar2;
  dword dVar3;
  int in_stack_00000004;
  
  if (in_stack_00000004 == 0) {
    return (this->cls_0x41c7f0).mbr_0x0;
  }
  dVar3 = (this->cls_0x41c7f0).mbr_0x0;
  dVar1 = 0;
  if (0 < (int)dVar3) {
    piVar2 = (int *)(this->cls_0x41c7f0).mbr_0x10;
    do {
      if (*piVar2 != 0) {
        dVar1 = dVar1 + 1;
      }
      piVar2 = piVar2 + 1;
      dVar3 = dVar3 - 1;
    } while (dVar3 != 0);
  }
  return dVar1;
}



// Function at 0051eea0

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer(cls_0x45f7c0 *this,uint param_1)

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 0) {
    if (param_1 < (this->cls_0x41c7f0).mbr_0x0) {
      return *(int *)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4);
    }
  }
  else {
    dVar1 = (this->cls_0x41c7f0).mbr_0x0;
    uVar4 = 0;
    iVar3 = 0;
    if (0 < (int)dVar1) {
      piVar2 = (int *)(this->cls_0x41c7f0).mbr_0x10;
      do {
        if (*piVar2 != 0) {
          if (uVar4 == param_1) {
            return *piVar2;
          }
          uVar4 = uVar4 + 1;
        }
        iVar3 = iVar3 + 1;
        piVar2 = piVar2 + 1;
      } while (iVar3 < (int)dVar1);
    }
  }
  return 0;
}



// Function at 0051eef0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51eef0_TPlayerManager_SetMainPlayerNum(cls_0x45f7c0 *this)

{
  dword *pdVar1;
  TPlayScreen *pcVar2;
  uint in_stack_00000004;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  this->mbr_0x28 = in_stack_00000004;
  if ((int)in_stack_00000004 < 0) {
    DAT_00667fcc = (TPlayScreen *)0x0;
    if (DAT_00667fd0 == &DAT_0065caf0) {
      local_c = DAT_00666988;
      local_8 = DAT_0066698c;
      local_4 = DAT_00666990;
      TCharacter::meth_0x453940((TCharacter *)&DAT_006668d8,&local_c,_DAT_00666970);
      cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
      if (DAT_0065b088 != (TPlayScreen *)0x0) {
        DAT_0065b088 = (TPlayScreen *)0x0;
        (**(code **)(DAT_0065b028 + 0x28))();
      }
      cls_0x52c3d0::meth_0x52c3d0((cls_0x52c3d0 *)&UNK_0065b48c.field_0x64);
      return;
    }
  }
  else {
    if (in_stack_00000004 < (this->cls_0x41c7f0).mbr_0x0) {
      pcVar2 = *(TPlayScreen **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
    }
    else {
      pcVar2 = (TPlayScreen *)0x0;
    }
    DAT_00667fcc = pcVar2;
    if ((((pcVar2 != (TPlayScreen *)0x0) && (DAT_00676828 != 0)) && (DAT_0067682c == 0)) &&
       (DAT_00668128 != 0)) {
      pdVar1 = &(pcVar2->TScreen).mbr_0x110;
      *pdVar1 = *pdVar1 | 0x100000;
    }
    if (DAT_00667fd0 == &DAT_0065caf0) {
      TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,pcVar2);
      TPlayScreen::meth_0x519230(DAT_00667fcc);
      cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
      if (DAT_0065b088 != DAT_00667fcc) {
        DAT_0065b088 = DAT_00667fcc;
        (**(code **)(DAT_0065b028 + 0x28))();
      }
      cls_0x52c3d0::meth_0x52c3d0((cls_0x52c3d0 *)&UNK_0065b48c.field_0x64);
      cls_0x544fb0::meth_0x546830((cls_0x544fb0 *)&DAT_0065a9d8);
    }
  }
  return;
}



// Function at 0051f060

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f060_TPlayerManager_SetMainPlayer(cls_0x45f7c0 *this)

{
  dword dVar1;
  int iVar2;
  int *piVar3;
  int in_stack_00000004;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  iVar2 = 0;
  if (0 < (int)dVar1) {
    piVar3 = (int *)(this->cls_0x41c7f0).mbr_0x10;
    while (*piVar3 != in_stack_00000004) {
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
      if ((int)dVar1 <= iVar2) {
        meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
        return;
      }
    }
  }
  meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
  return;
}



// Function at 0051f0a0

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f0a0_TPlayerManager_AddPlayer(cls_0x45f7c0 *this,int param_1)

{
  bool bVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  char *_Source;
  undefined4 *puVar5;
  int *piVar6;
  dword dVar7;
  bool bVar8;
  dword in_stack_00000008;
  undefined4 local_40 [16];
  
  dVar2 = 0xffffffff;
  bVar1 = false;
  dVar7 = 0;
  dVar3 = dVar2;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      iVar4 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + dVar7 * 4);
      dVar2 = dVar7;
      if ((iVar4 != 0) && (dVar2 = dVar3, iVar4 == param_1)) {
        if (((int)in_stack_00000008 < 0) || (in_stack_00000008 == dVar7)) {
          return dVar7;
        }
        if (dVar7 == this->mbr_0x28) {
          bVar1 = true;
        }
        meth_0x51f200(this,dVar7);
        dVar2 = dVar7;
      }
      dVar7 = dVar7 + 1;
      dVar3 = dVar2;
    } while ((int)dVar7 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  if (-1 < (int)in_stack_00000008) {
    if (((int)in_stack_00000008 < (int)(this->cls_0x41c7f0).mbr_0x0) &&
       (*(int *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000008 * 4) != 0)) {
      return 0xffffffff;
    }
    dVar2 = in_stack_00000008;
    if (in_stack_00000008 == this->mbr_0x28) {
      bVar1 = true;
    }
  }
  if ((int)dVar2 < 0) {
    dVar2 = meth_0x41c840(this);
  }
  else {
    dVar2 = meth_0x41c910(this,param_1);
  }
  bVar8 = DAT_00667fcc == 0;
  *(dword *)(param_1 + 0x664) = dVar2;
  if ((bVar8) || (bVar1)) {
    dVar3 = (this->cls_0x41c7f0).mbr_0x0;
    iVar4 = 0;
    if (0 < (int)dVar3) {
      piVar6 = (int *)(this->cls_0x41c7f0).mbr_0x10;
      do {
        if (*piVar6 == param_1) break;
        iVar4 = iVar4 + 1;
        piVar6 = piVar6 + 1;
      } while (iVar4 < (int)dVar3);
    }
    meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
  }
  if (((-1 < DAT_0065a784) && (iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar4 != 0)) &&
     (_Source = (char *)(iVar4 + 0x58), _Source != (char *)0x0)) {
    _strncpy((char *)(param_1 + 0x4f0),_Source,0x4f);
    *(undefined *)(param_1 + 0x53f) = 0;
  }
  meth_0x51f370(this);
  if (DAT_006682bc != 0) {
    puVar5 = FUN_0044b1e0(local_40);
    FUN_0044aff0((LPCSTR)puVar5);
    if (DAT_006582e8 != 0) {
      FUN_0044b2d0();
    }
  }
  return dVar2;
}



// Function at 0051f200

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f200(cls_0x45f7c0 *this,uint param_1)

{
  dword dVar1;
  undefined4 *puVar2;
  int *piVar3;
  int in_stack_00000008;
  undefined4 local_40 [16];
  
  if (param_1 < (this->cls_0x41c7f0).mbr_0x0) {
    if (in_stack_00000008 == 0) {
      meth_0x41cb40(this);
    }
    else {
      meth_0x41cb80(this);
    }
    if (param_1 == this->mbr_0x28) {
      meth_0x51eef0_TPlayerManager_SetMainPlayerNum(this);
    }
    if ((in_stack_00000008 != 0) &&
       (dVar1 = (this->cls_0x41c7f0).mbr_0x0, (int)param_1 < (int)dVar1)) {
      piVar3 = (int *)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4);
      do {
        if (*piVar3 != 0) {
          *(uint *)(*piVar3 + 0x664) = param_1;
        }
        param_1 = param_1 + 1;
        piVar3 = piVar3 + 1;
      } while ((int)param_1 < (int)dVar1);
    }
    meth_0x51f370(this);
    if (DAT_006682bc != 0) {
      puVar2 = FUN_0044b1e0(local_40);
      FUN_0044aff0((LPCSTR)puVar2);
      if (DAT_006582e8 != 0) {
        FUN_0044b2d0();
      }
    }
  }
  return;
}



// Function at 0051f2a0

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f2a0(cls_0x45f7c0 *this,int param_1)

{
  dword dVar1;
  int iVar2;
  int *piVar3;
  
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  iVar2 = 0;
  if (0 < (int)dVar1) {
    piVar3 = (int *)(this->cls_0x41c7f0).mbr_0x10;
    do {
      if (*piVar3 == param_1) {
        meth_0x51f200(this,iVar2);
        return;
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < (int)dVar1);
  }
  meth_0x51f370(this);
  return;
}



// Function at 0051f2e0

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f2e0(cls_0x45f7c0 *this)

{
  int iVar1;
  uint uVar2;
  cls_0x45f7c0 *extraout_ECX;
  cls_0x45f7c0 *pcVar3;
  int iVar4;
  byte *in_stack_00000004;
  
  iVar4 = 0;
  pcVar3 = this;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4);
      if ((iVar1 != 0) &&
         (uVar2 = FUN_0059a530_stricmp((uint)pcVar3,*(byte **)(iVar1 + 0x38),in_stack_00000004),
         pcVar3 = extraout_ECX, uVar2 == 0)) {
        return iVar4;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return -1;
}



// Function at 0051f370

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f370(cls_0x45f7c0 *this)

{
  dword *this_00;
  byte bVar1;
  int *piVar2;
  uint uVar3;
  undefined4 *puVar4;
  int *piVar5;
  byte *pbVar6;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  dword dVar7;
  int iVar8;
  undefined4 *puVar9;
  byte *pbVar10;
  int iVar11;
  int *piVar12;
  bool bVar13;
  int local_8;
  int local_4;
  
  iVar8 = 0;
  this_00 = &(this->cls_0x41c7f0).mbr_0x14;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      meth_0x520040((cls_0x45f7c0 *)this_00);
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)*this_00);
  }
  *this_00 = 0;
  dVar7 = (this->cls_0x41c7f0).mbr_0x0;
  this->mbr_0x18 = 0;
  local_4 = 0;
  if (0 < (int)dVar7) {
    do {
      dVar7 = (this->cls_0x41c7f0).mbr_0x10;
      iVar8 = *(int *)(dVar7 + local_4 * 4);
      if (iVar8 != 0) {
        if (*(char *)(iVar8 + 0x494) != '\0') {
          iVar11 = 0;
          if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
            do {
              uVar3 = FUN_0059a530_stricmp(dVar7,(byte *)(iVar8 + 0x494),
                                   (byte *)(*(int *)(this->mbr_0x24 + iVar11 * 4) + 4));
              if (uVar3 == 0) break;
              iVar11 = iVar11 + 1;
              dVar7 = extraout_ECX;
            } while (iVar11 < (int)(this->cls_0x41c7f0).mbr_0x14);
          }
          if ((int)(this->cls_0x41c7f0).mbr_0x14 <= iVar11) {
            iVar11 = *(int *)(iVar8 + 0x4dc);
            if ((iVar11 < 0) || ((int)(this->cls_0x41c7f0).mbr_0x0 <= iVar11)) {
LAB_0051f44d:
              *(int *)(iVar8 + 0x4dc) = local_4;
            }
            else {
              uVar3 = (this->cls_0x41c7f0).mbr_0x10;
              iVar11 = *(int *)(uVar3 + iVar11 * 4);
              if ((iVar11 == 0) ||
                 ((uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(iVar8 + 0x494),(byte *)(iVar11 + 0x494)),
                  uVar3 != 0 ||
                  (uVar3 = FUN_0059a530_stricmp(iVar8 + 0x4c6U,(byte *)(iVar8 + 0x4c6U),
                                        (byte *)(*(int *)((this->cls_0x41c7f0).mbr_0x10 +
                                                         *(int *)(iVar8 + 0x4dc) * 4) + 0x4c6)),
                  uVar3 != 0)))) goto LAB_0051f44d;
            }
            puVar4 = FUN_00482fb0(0x60);
            puVar9 = (undefined4 *)(iVar8 + 0x490);
            for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
              *puVar4 = *puVar9;
              puVar9 = puVar9 + 1;
              puVar4 = puVar4 + 1;
            }
            meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x41c7f0).mbr_0x14);
          }
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  local_4 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      piVar2 = *(int **)(this->mbr_0x24 + local_4 * 4);
      if (piVar2 != (int *)0x0) {
        dVar7 = (this->cls_0x41c7f0).mbr_0x0;
        local_8 = 0;
        if (0 < (int)dVar7) {
          do {
            iVar8 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + local_8 * 4);
            if (iVar8 != 0) {
              piVar5 = piVar2 + 1;
              uVar3 = FUN_0059a530_stricmp(dVar7,(byte *)piVar5,(byte *)(iVar8 + 0x494));
              dVar7 = extraout_ECX_00;
              if (uVar3 == 0) {
                pbVar6 = (byte *)((int)piVar2 + 0x36);
                uVar3 = FUN_0059a530_stricmp(extraout_ECX_00,pbVar6,
                                     (byte *)(*(int *)((this->cls_0x41c7f0).mbr_0x10 + local_8 * 4)
                                             + 0x4c6));
                dVar7 = extraout_ECX_01;
                if (uVar3 == 0) {
                  iVar8 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + local_8 * 4);
                  if (piVar2 != (int *)0x0) {
                    pbVar10 = (byte *)(iVar8 + 0x494);
                    do {
                      bVar1 = *(byte *)piVar5;
                      bVar13 = bVar1 < *pbVar10;
                      if (bVar1 != *pbVar10) {
LAB_0051f53e:
                        iVar11 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                        goto LAB_0051f543;
                      }
                      if (bVar1 == 0) break;
                      bVar1 = *(byte *)((int)piVar5 + 1);
                      bVar13 = bVar1 < pbVar10[1];
                      if (bVar1 != pbVar10[1]) goto LAB_0051f53e;
                      piVar5 = (int *)((int)piVar5 + 2);
                      pbVar10 = pbVar10 + 2;
                    } while (bVar1 != 0);
                    iVar11 = 0;
LAB_0051f543:
                    if (iVar11 == 0) {
                      pbVar10 = (byte *)(iVar8 + 0x4c6);
                      do {
                        bVar1 = *pbVar6;
                        bVar13 = bVar1 < *pbVar10;
                        if (bVar1 != *pbVar10) {
LAB_0051f577:
                          iVar11 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                          goto LAB_0051f57c;
                        }
                        if (bVar1 == 0) break;
                        bVar1 = pbVar6[1];
                        bVar13 = bVar1 < pbVar10[1];
                        if (bVar1 != pbVar10[1]) goto LAB_0051f577;
                        pbVar6 = pbVar6 + 2;
                        pbVar10 = pbVar10 + 2;
                      } while (bVar1 != 0);
                      iVar11 = 0;
LAB_0051f57c:
                      if ((((iVar11 == 0) && (*piVar2 == *(int *)(iVar8 + 0x490))) &&
                          (piVar2[0x12] == *(int *)(iVar8 + 0x4d8))) &&
                         (piVar2[0x13] == *(int *)(iVar8 + 0x4dc))) goto LAB_0051f5c8;
                    }
                    piVar5 = piVar2;
                    piVar12 = (int *)(iVar8 + 0x490);
                    for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
                      *piVar12 = *piVar5;
                      piVar5 = piVar5 + 1;
                      piVar12 = piVar12 + 1;
                    }
                    cls_0x57d9d0::meth_0x584960((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,iVar8);
                  }
LAB_0051f5c8:
                  piVar5 = (int *)(**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 +
                                                         local_8 * 4) + 0xa8))(s_Swag_Bag_005e2b64);
                  if (piVar5 == (int *)0x0) {
                    dVar7 = extraout_ECX_02;
                    if ((DAT_0067682c == 0) ||
                       (piVar5 = (int *)FUN_00474e20_NewObjectByName((byte *)s_Swag_Bag_005e2b70,(undefined4 *)0x0,
                                                     0xffffffff), dVar7 = extraout_ECX_03,
                       piVar5 == (int *)0x0)) goto LAB_0051f663;
                    (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + local_8 * 4) + 0x58))
                              (piVar5,0xffffffff);
                  }
                  if (local_8 == piVar2[0x13]) {
                    iVar11 = *piVar5;
                    iVar8 = 0;
                  }
                  else {
                    iVar8 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + piVar2[0x13] * 4
                                                   ) + 0xa8))(s_Swag_Bag_005e2b7c);
                    dVar7 = extraout_ECX_04;
                    if (iVar8 == 0) goto LAB_0051f663;
                    iVar11 = *piVar5;
                  }
                  (**(code **)(iVar11 + 0x174))(iVar8);
                  dVar7 = extraout_ECX_05;
                }
              }
            }
LAB_0051f663:
            local_8 = local_8 + 1;
          } while (local_8 < (int)(this->cls_0x41c7f0).mbr_0x0);
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < (int)(this->cls_0x41c7f0).mbr_0x14);
  }
  cls_0x51f6b0::meth_0x51f6b0((cls_0x51f6b0 *)this);
  cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
  return;
}



// Function at 0051f840

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51f840(cls_0x45f7c0 *this)

{
  int iVar1;
  uint uVar2;
  cls_0x45f7c0 *extraout_ECX;
  cls_0x45f7c0 *pcVar3;
  int iVar4;
  byte *in_stack_00000004;
  
  iVar4 = 0;
  pcVar3 = this;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      iVar1 = *(int *)(this->mbr_0x24 + iVar4 * 4);
      if ((iVar1 != 0) &&
         (uVar2 = FUN_0059a530_stricmp((uint)pcVar3,(byte *)(iVar1 + 4),in_stack_00000004),
         pcVar3 = extraout_ECX, uVar2 == 0)) {
        return iVar4;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x14);
  }
  return -1;
}



// Function at 0051fd50

int __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x51fd50(cls_0x45f7c0 *this)

{
  byte *pbVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  cls_0x45f7c0 *extraout_ECX;
  cls_0x45f7c0 *pcVar5;
  int iVar6;
  dword in_stack_00000004;
  
  dVar3 = in_stack_00000004;
  iVar6 = 0;
  if (in_stack_00000004 == 0) {
    return 0;
  }
  pbVar1 = (byte *)(in_stack_00000004 + 0x494);
  if (*(char *)(in_stack_00000004 + 0x494) == '\0') {
    return 0;
  }
  in_stack_00000004 = 0;
  pcVar5 = this;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      iVar2 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar6 * 4);
      if (iVar2 != 0) {
        uVar4 = FUN_0059a530_stricmp((uint)pcVar5,(byte *)(iVar2 + 0x494),pbVar1);
        pcVar5 = extraout_ECX;
        if ((uVar4 == 0) &&
           (pcVar5 = (cls_0x45f7c0 *)(this->cls_0x41c7f0).mbr_0x10,
           (&(pcVar5->cls_0x41c7f0).mbr_0x0)[iVar6] != dVar3)) {
          in_stack_00000004 = in_stack_00000004 + 1;
        }
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return in_stack_00000004;
}



// Function at 0051fdd0

undefined4 __thiscall
OOAnalyzer::cls_0x45f7c0::meth_0x51fdd0(cls_0x45f7c0 *this,undefined param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined3 in_stack_00000005;
  int in_stack_0000000c;
  
  if ((_param_1 != 0) && (*(char *)(_param_1 + 0x494) != '\0')) {
    iVar3 = 0;
    iVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4);
        if (iVar1 != 0) {
          uVar2 = FUN_0059a530_stricmp(_param_1 + 0x494U,(byte *)(iVar1 + 0x494),(byte *)(_param_1 + 0x494U)
                              );
          if ((uVar2 == 0) && (*(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4) != _param_1)) {
            if (iVar3 == in_stack_0000000c) {
              return *(undefined4 *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4);
            }
            iVar3 = iVar3 + 1;
          }
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
  }
  return 0;
}



// Function at 00520040

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x520040(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     pvVar1 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 0052c720

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x52c720(cls_0x45f7c0 *this)

{
  undefined4 *puVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (puVar1 = *(undefined4 **)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     puVar1 != (undefined4 *)0x0)) {
    *puVar1 = 0xffffffff;
    if ((LPCVOID)puVar1[2] != (LPCVOID)0x0) {
      FUN_00482f80((LPCVOID)puVar1[2]);
    }
    puVar1[2] = 0;
    puVar1[1] = 0;
    FUN_004830f0(puVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 00536320

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x536320(cls_0x45f7c0 *this)

{
  cls_0x4367d0 **ppcVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (ppcVar1 = *(cls_0x4367d0 ***)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4),
     ppcVar1 != (cls_0x4367d0 **)0x0)) {
    FUN_005343e0(ppcVar1);
    FUN_004830f0(ppcVar1);
  }
  meth_0x41cb40(this);
  return;
}



// Function at 0053da50

cls_0x45f7c0 * __thiscall OOAnalyzer::cls_0x45f7c0::cls_0x45f7c0(cls_0x45f7c0 *this)

{
  dword dVar1;
  LPCVOID pvVar2;
  undefined4 *puVar3;
  int iVar4;
  dword *pdVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1d60;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  local_4 = 0;
  puVar3 = FUN_00482fb0(0x78);
  (this->cls_0x41c7f0).mbr_0x14 = (dword)puVar3;
  for (iVar4 = 0x1e; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  this->mbr_0x18 = 0;
  local_4 = 1;
  pdVar5 = &this->mbr_0x1c;
  for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pdVar5 = 0;
    pdVar5 = pdVar5 + 1;
  }
  pdVar5 = &this->mbr_0x3c;
  for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pdVar5 = 0;
    pdVar5 = pdVar5 + 1;
  }
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  pdVar5 = &this->mbr_0x84;
  for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pdVar5 = 0;
    pdVar5 = pdVar5 + 1;
  }
  dVar1 = (this->cls_0x41c7f0).mbr_0x0;
  iVar4 = 0;
  this->mbr_0xa4 = 0;
  if (0 < (int)dVar1) {
    do {
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
      }
      meth_0x41cb40(this);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  this->mbr_0xbc = 0x41;
  this->mbr_0xb8 = 0x41;
  this->mbr_0xb4 = 0x41;
  this->mbr_0xc0 = 5;
  this->mbr_0xc4 = 5;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  *(undefined *)&this->mbr_0xa8 = 0;
  *(undefined *)((int)&this->mbr_0xa8 + 1) = 0;
  *(undefined *)((int)&this->mbr_0xa8 + 2) = 0;
  this->mbr_0xac = 0;
  this->mbr_0xb0 = 0;
  this->mbr_0xc8 = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 0053db70

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x53db70(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1d7a;
  local_c = ExceptionList;
  iVar5 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    dVar4 = (this->cls_0x41c7f0).mbr_0x10;
    ExceptionList = &local_c;
    do {
      dVar2 = *(dword *)(dVar4 + iVar5 * 4);
      dVar3 = dVar2;
      if (dVar2 == 0) {
        dVar3 = (this->cls_0x41c7f0).mbr_0x14;
      }
      if (*(int *)(dVar3 + 0x70) != 0) {
        dVar4 = dVar2;
        if (dVar2 == 0) {
          dVar4 = (this->cls_0x41c7f0).mbr_0x14;
        }
        if (*(int *)(*(int *)(dVar4 + 0x70) + 0x70) != 0) {
          if (dVar2 == 0) {
            dVar2 = (this->cls_0x41c7f0).mbr_0x14;
          }
          FUN_004830f0(*(LPCVOID *)(*(int *)(dVar2 + 0x70) + 0x70));
          dVar2 = *(dword *)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4);
          dVar4 = dVar2;
          if (dVar2 == 0) {
            dVar4 = (this->cls_0x41c7f0).mbr_0x14;
          }
          *(undefined4 *)(*(int *)(dVar4 + 0x70) + 0x70) = 0;
        }
        if (dVar2 == 0) {
          dVar2 = (this->cls_0x41c7f0).mbr_0x14;
        }
        FUN_004830f0(*(LPCVOID *)(dVar2 + 0x70));
        dVar4 = (this->cls_0x41c7f0).mbr_0x10;
        dVar3 = *(dword *)(dVar4 + iVar5 * 4);
        if (dVar3 == 0) {
          dVar3 = (this->cls_0x41c7f0).mbr_0x14;
        }
        *(undefined4 *)(dVar3 + 0x70) = 0;
      }
      dVar3 = *(dword *)(dVar4 + iVar5 * 4);
      dVar2 = dVar3;
      if (dVar3 == 0) {
        dVar2 = (this->cls_0x41c7f0).mbr_0x14;
      }
      if (*(int *)(dVar2 + 0x74) != 0) {
        if (dVar3 == 0) {
          dVar3 = (this->cls_0x41c7f0).mbr_0x14;
        }
        FUN_004830f0(*(LPCVOID *)(dVar3 + 0x74));
        dVar4 = (this->cls_0x41c7f0).mbr_0x10;
        dVar3 = *(dword *)(dVar4 + iVar5 * 4);
        if (dVar3 == 0) {
          dVar3 = (this->cls_0x41c7f0).mbr_0x14;
        }
        *(undefined4 *)(dVar3 + 0x74) = 0;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  iVar5 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar5) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  pvVar1 = (LPCVOID)this->mbr_0x7c;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  if (pvVar1 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar1);
  }
  this->mbr_0x7c = 0;
  iVar5 = 0;
  local_4 = 1;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar5) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 0053e4e0

undefined4 __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x53e4e0(cls_0x45f7c0 *this,char *param_1)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar3;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined4 *puVar4;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  undefined3 extraout_var_08;
  undefined3 extraout_var_09;
  undefined3 extraout_var_10;
  undefined4 *puVar5;
  undefined3 extraout_var_11;
  undefined3 extraout_var_12;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  uint *in_stack_00000008;
  char *pcVar9;
  undefined4 local_578 [9];
  undefined4 local_554;
  ushort local_550;
  undefined4 *local_508;
  char local_500 [256];
  char local_400 [1024];
  
  _strncpy((char *)&this->mbr_0x1c,param_1,0x1f);
  this->mbr_0x3b = 0;
  *(undefined *)&this->mbr_0x3c = 0;
  this->mbr_0x5c = 0;
  *(undefined *)&this->mbr_0x84 = 0;
  FUN_00479680((byte *)in_stack_00000008);
  bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_BEGIN_005e4ad8,0);
  if (CONCAT31(extraout_var,bVar2) == 0) {
    FUN_00479950(in_stack_00000008,(byte *)s_Spell_def_BEGIN_expected_005e4ae0);
  }
  FUN_004795a0((byte *)in_stack_00000008);
  uVar6 = in_stack_00000008[4];
  do {
    if ((uVar6 == 10) ||
       (bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)&PTR_virt_meth_0x444e45_005e4afc,0),
       CONCAT31(extraout_var_00,bVar2) != 0)) {
      bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)&PTR_virt_meth_0x444e45_005e4df4,0);
      if (CONCAT31(extraout_var_12,bVar2) == 0) {
        FUN_00479950(in_stack_00000008,(byte *)s_Spell_def_END_expected_005e4df8);
      }
      FUN_00479580((byte *)in_stack_00000008);
      return 1;
    }
    if (in_stack_00000008[4] != 4) {
      FUN_00479950(in_stack_00000008,(byte *)s_Spell_def_keyword_expected_005e4b00);
    }
    bVar2 = FUN_00479700((uint)in_stack_00000008,&DAT_005e4b1c,0);
    if (CONCAT31(extraout_var_01,bVar2) == 0) {
      bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_POISONCHANCE_005e4b48,0);
      if (CONCAT31(extraout_var_02,bVar2) == 0) {
        bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_ICONNAME_005e4b8c,0);
        if (CONCAT31(extraout_var_03,bVar2) == 0) {
          bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_DESCRIPTION_005e4bc0,0);
          if (CONCAT31(extraout_var_04,bVar2) == 0) {
            bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_DAMAGETYPE_005e4bfc,0);
            if (CONCAT31(extraout_var_05,bVar2) == 0) {
              bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_FLAGS_005e4c38,0);
              if (CONCAT31(extraout_var_06,bVar2) == 0) {
                bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_ANIMATION_005e4c64,0);
                if (CONCAT31(extraout_var_07,bVar2) == 0) {
                  bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_DELAY_005e4c9c,0);
                  if (CONCAT31(extraout_var_08,bVar2) == 0) {
                    bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_VARIANT_005e4cc8,0);
                    if (CONCAT31(extraout_var_09,bVar2) == 0) {
                      bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_LIGHT_005e4d60,0);
                      if (CONCAT31(extraout_var_11,bVar2) == 0) {
                        pcVar9 = s_Invalid_spell_tag__s_005e4ddc;
                      }
                      else {
                        iVar3 = FUN_0047a410(in_stack_00000008,
                                             (float **)s_LIGHT_<COLOR__b,_b,_b>_<INT__i>_<_005e4d68)
                        ;
                        if (iVar3 != 0) goto LAB_0053e9b8;
                        pcVar9 = s_Error_parsing_spell_LIGHT_tag_005e4dbc;
                      }
LAB_0053e9b1:
                      FUN_00479950(in_stack_00000008,(byte *)pcVar9);
                    }
                    else {
                      puVar4 = local_578;
                      for (iVar3 = 0x1e; iVar3 != 0; iVar3 = iVar3 + -1) {
                        *puVar4 = 0;
                        puVar4 = puVar4 + 1;
                      }
                      iVar3 = FUN_0047a410(in_stack_00000008,
                                           (float **)s_VARIANT__s___i___s___s___i___i____005e4cd0);
                      if (iVar3 == 0) {
                        FUN_00479950(in_stack_00000008,(byte *)s_Error_parsing_VARIANT_tag_005e4d08)
                        ;
                      }
                      local_554 = 0;
                      local_550 = 0;
                      local_508 = (undefined4 *)0x0;
                      _strncpy((char *)&local_554,local_500,5);
                      local_550 = local_550 & 0xff;
                      FUN_00479680((byte *)in_stack_00000008);
                      bVar2 = FUN_00479700((uint)in_stack_00000008,(byte *)s_CONTROLDATA_005e4d24,0)
                      ;
                      if (CONCAT31(extraout_var_10,bVar2) != 0) {
                        local_508 = FUN_00482fb0(0xc4);
                        puVar4 = local_508;
                        for (iVar3 = 0x31; iVar3 != 0; iVar3 = iVar3 + -1) {
                          *puVar4 = 0;
                          puVar4 = puVar4 + 1;
                        }
                        local_508[10] = 1;
                        local_508[0xb] = 0xffffffff;
                        local_508[0x10] = 0xffffffff;
                        local_508[0x25] = 0xffffffff;
                        iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_CONTROLDATA__s_005e4d30);
                        if (iVar3 == 0) {
                          FUN_00479950(in_stack_00000008,
                                       (byte *)s_Error_parsing_CONTROLDATA_tag_005e4d40);
                        }
                        FUN_00479680((byte *)in_stack_00000008);
                        if ((in_stack_00000008[4] == 3) && (in_stack_00000008[6] == 0)) {
                          FUN_00479790((byte *)in_stack_00000008);
                          do {
                            do {
                              FUN_0053dd10(in_stack_00000008,(int)local_578);
                              FUN_00479680((byte *)in_stack_00000008);
                            } while (in_stack_00000008[4] != 3);
                          } while (in_stack_00000008[6] != 1);
                          FUN_00479870((byte *)in_stack_00000008);
                        }
                      }
                      puVar5 = FUN_00482fb0(0x78);
                      puVar4 = local_578;
                      for (iVar3 = 0x1e; iVar3 != 0; iVar3 = iVar3 + -1) {
                        *puVar5 = *puVar4;
                        puVar4 = puVar4 + 1;
                        puVar5 = puVar5 + 1;
                      }
                      meth_0x41c840(this);
                    }
                  }
                  else {
                    iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_DELAY__i_005e4ca4);
                    if (iVar3 == 0) {
                      pcVar9 = s_Error_parsing_DELAY_tag_005e4cb0;
                      goto LAB_0053e9b1;
                    }
                  }
                }
                else {
                  iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_ANIMATION__30s_005e4c70);
                  if (iVar3 == 0) {
                    pcVar9 = s_Error_parsing_ANIMATION_tag_005e4c80;
                    goto LAB_0053e9b1;
                  }
                }
              }
              else {
                iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_FLAGS__i_005e4c40);
                if (iVar3 == 0) {
                  pcVar9 = s_Error_parsing_FLAGS_tag_005e4c4c;
                  goto LAB_0053e9b1;
                }
              }
            }
            else {
              iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_DAMAGETYPE__i_005e4c08);
              if (iVar3 == 0) {
                pcVar9 = s_Error_parsing_DAMAGETYPE_tag_005e4c18;
                goto LAB_0053e9b1;
              }
            }
          }
          else {
            iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_DESCRIPTION__s_005e4bcc);
            if (iVar3 == 0) {
              FUN_00479950(in_stack_00000008,(byte *)s_Error_parsing_DESCRIPTION_tag_005e4bdc);
            }
            uVar6 = 0xffffffff;
            pcVar9 = local_400;
            do {
              if (uVar6 == 0) break;
              uVar6 = uVar6 - 1;
              cVar1 = *pcVar9;
              pcVar9 = pcVar9 + 1;
            } while (cVar1 != '\0');
            puVar4 = FUN_00482fb0(~uVar6);
            uVar6 = 0xffffffff;
            this->mbr_0x7c = (dword)puVar4;
            pcVar9 = local_400;
            do {
              pcVar8 = pcVar9;
              if (uVar6 == 0) break;
              uVar6 = uVar6 - 1;
              pcVar8 = pcVar9 + 1;
              cVar1 = *pcVar9;
              pcVar9 = pcVar8;
            } while (cVar1 != '\0');
            uVar6 = ~uVar6;
            puVar5 = (undefined4 *)(pcVar8 + -uVar6);
            for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
              *puVar4 = *puVar5;
              puVar5 = puVar5 + 1;
              puVar4 = puVar4 + 1;
            }
            for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
              *(undefined *)puVar4 = *(undefined *)puVar5;
              puVar5 = (undefined4 *)((int)puVar5 + 1);
              puVar4 = (undefined4 *)((int)puVar4 + 1);
            }
          }
        }
        else {
          iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_ICONNAME__30s_005e4b98);
          if (iVar3 == 0) {
            pcVar9 = s_Error_parsing_NAME_tag_005e4ba8;
            goto LAB_0053e9b1;
          }
        }
      }
      else {
        iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_POISONCHANCE__i_005e4b58);
        if (iVar3 == 0) {
          pcVar9 = s_Error_parsing_POISONCHANCE_tag_005e4b6c;
          goto LAB_0053e9b1;
        }
      }
    }
    else {
      iVar3 = FUN_0047a410(in_stack_00000008,(float **)s_NAME__30s_005e4b24);
      if (iVar3 == 0) {
        pcVar9 = s_Error_parsing_NAME_tag_005e4b30;
        goto LAB_0053e9b1;
      }
    }
LAB_0053e9b8:
    uVar6 = in_stack_00000008[4];
  } while( true );
}



// Function at 00540a40

void __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x540a40(cls_0x45f7c0 *this)

{
  LPCVOID pvVar1;
  void **ppvVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1de8;
  local_c = ExceptionList;
  iVar3 = 0;
  local_4 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar1 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar1);
      }
      meth_0x41cb40(this);
      iVar3 = iVar3 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  ExceptionList = ppvVar2;
  (this->cls_0x41c7f0).mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x14);
  local_4 = 0xffffffff;
  FUN_004830f0((LPCVOID)(this->cls_0x41c7f0).mbr_0x10);
  ExceptionList = local_c;
  return;
}



// Function at 00587e20

dword __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x587e20(cls_0x45f7c0 *this)

{
  return (this->cls_0x41c7f0).mbr_0x0;
}



// Function at 00587f30

cls_0x45f7c0 * __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x587f30(cls_0x45f7c0 *this)

{
  byte in_stack_00000004;
  
  meth_0x45f6e0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00588010

undefined4 __thiscall OOAnalyzer::cls_0x45f7c0::meth_0x588010(cls_0x45f7c0 *this)

{
  int in_stack_00000004;
  
  return *(undefined4 *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4);
}



