// Decompiled methods and structure for class: cls_0x5756d0

/*
/OOAnalyzer/cls_0x5756d0
pack(disabled)
Structure cls_0x5756d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   196   byte   1   mbr_0xc4   "Unsigned Byte (db)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   byte   1   mbr_0xfc   "Unsigned Byte (db)"
   253   byte   1   mbr_0xfd   "Unsigned Byte (db)"
   254   byte   1   mbr_0xfe   "Unsigned Byte (db)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   291   byte   1   mbr_0x123   "Unsigned Byte (db)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
   376   dword   4   mbr_0x178   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   byte   1   mbr_0x19c   "Unsigned Byte (db)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   464   dword   4   mbr_0x1d0   "Unsigned Double-Word (ddw, 4-bytes)"
   504   dword   4   mbr_0x1f8   "Unsigned Double-Word (ddw, 4-bytes)"
   544   dword   4   mbr_0x220   "Unsigned Double-Word (ddw, 4-bytes)"
   584   dword   4   mbr_0x248   "Unsigned Double-Word (ddw, 4-bytes)"
   624   dword   4   mbr_0x270   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 628 Alignment: 1

*/

// Function at 005756d0

cls_0x5756d0 * __thiscall OOAnalyzer::cls_0x5756d0::~cls_0x5756d0(cls_0x5756d0 *this)

{
  LPCVOID *ppvVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  cls_0x5756d0 *pcVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a25f8;
  local_c = ExceptionList;
  local_4 = 9;
  ExceptionList = &local_c;
  meth_0x5759f0(this);
  ppvVar1 = (LPCVOID *)this->mbr_0x20;
  if (ppvVar1 != (LPCVOID *)0x0) {
    FUN_004830f0(*ppvVar1);
    FUN_004830f0(ppvVar1);
  }
  if ((undefined4 *)this->mbr_0x178 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x178)(1);
  }
  local_4._0_1_ = 8;
  this->mbr_0x248 = (dword)&cls_0x5ba038__vftable_5ba038_005ba038;
  if ((HANDLE)this->mbr_0x270 != (HANDLE)0x0) {
    CloseHandle((HANDLE)this->mbr_0x270);
  }
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)&this->mbr_0x248);
  local_4._0_1_ = 7;
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)&this->mbr_0x220);
  local_4._0_1_ = 6;
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)&this->mbr_0x1f8);
  local_4._0_1_ = 5;
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)&this->mbr_0x1d0);
  local_4._0_1_ = 4;
  cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)&this->mbr_0x1a8);
  FUN_004830f0((LPCVOID)this->mbr_0x18c);
  FUN_004830f0((LPCVOID)this->mbr_0x17c);
  local_4 = CONCAT31(local_4._1_3_,1);
  cls_0x588410::meth_0x588480((cls_0x588410 *)&this->mbr_0x15c);
  piVar2 = (int *)this->mbr_0x150;
  iVar4 = *piVar2;
  while (iVar4 != 0) {
    iVar4 = *piVar2;
    if (iVar4 != 0) {
      piVar3 = (int *)piVar2[1];
      *piVar2 = 0;
      *(int **)(iVar4 + 4) = piVar3;
      *piVar3 = iVar4;
    }
    iVar4 = *piVar2;
  }
  pcVar5 = (cls_0x5756d0 *)FUN_004830f0((LPCVOID)this->mbr_0x8);
  ExceptionList = local_c;
  return pcVar5;
}



// Function at 00575890

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x575890(cls_0x5756d0 *this)

{
  dword dVar1;
  undefined4 uVar2;
  HANDLE pvVar3;
  HRESULT HVar4;
  uint uVar5;
  int *piVar6;
  byte *pbVar7;
  int iVar8;
  uint extraout_ECX;
  int iVar9;
  int iVar10;
  int iVar11;
  cls_0x5756d0 *pcStack_4;
  
  pcStack_4 = this;
  meth_0x5759f0(this);
  iVar11 = 0;
  pvVar3 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
  this->mbr_0x12c = (dword)pvVar3;
  if (pvVar3 == (HANDLE)0x0) {
    return 0;
  }
  pvVar3 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCSTR)0x0);
  this->mbr_0x124 = (dword)pvVar3;
  if (pvVar3 == (HANDLE)0x0) {
    return 0;
  }
  pvVar3 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCSTR)0x0);
  this->mbr_0x128 = (dword)pvVar3;
  if (pvVar3 == (HANDLE)0x0) {
    return 0;
  }
  pcStack_4 = (cls_0x5756d0 *)0x0;
  HVar4 = CoCreateInstance((IID *)&DAT_005b9df8,(LPUNKNOWN)0x0,1,(IID *)&DAT_005b9de8,&pcStack_4);
  uVar5 = ~-(uint)(HVar4 != 0) & (uint)pcStack_4;
  this->mbr_0x4 = uVar5;
  if (uVar5 == 0) {
    return 0;
  }
  pcStack_4 = (cls_0x5756d0 *)0x0;
  HVar4 = CoCreateInstance((IID *)&DAT_005b9d38,(LPUNKNOWN)0x0,1,(IID *)&DAT_005b9d28,&pcStack_4);
  piVar6 = (int *)(~-(uint)(HVar4 != 0) & (uint)pcStack_4);
  this->mbr_0x0 = (dword)piVar6;
  if (piVar6 == (int *)0x0) {
    return 0;
  }
  (**(code **)(*piVar6 + 0x8c))(piVar6,0,FUN_00575af0,this,1);
  iVar10 = 0;
  iVar9 = (int)(this->mbr_0xc - this->mbr_0x8) >> 2;
  if (0 < iVar9) {
    do {
      pbVar7 = (byte *)(**(code **)(**(int **)(this->mbr_0x8 + iVar11 * 4) + 8))();
      uVar5 = FUN_0059a530(extraout_ECX,(byte *)s_TCP_IP_005e5bbc,pbVar7);
      if (uVar5 == 0) {
        dVar1 = this->mbr_0x8;
        iVar10 = iVar10 + 1;
        uVar2 = *(undefined4 *)((dVar1 - 4) + iVar10 * 4);
        *(undefined4 *)((dVar1 - 4) + iVar10 * 4) = *(undefined4 *)(dVar1 + iVar11 * 4);
        *(undefined4 *)(dVar1 + iVar11 * 4) = uVar2;
      }
      iVar11 = iVar11 + 1;
    } while (iVar11 < iVar9);
  }
  iVar11 = 0;
  if (0 < iVar9) {
    while( true ) {
      iVar8 = (**(code **)(**(int **)(this->mbr_0x8 + iVar11 * 4) + 0xc))();
      if (iVar8 == 0) break;
      iVar11 = iVar11 + 1;
      if (iVar9 <= iVar11) {
        return 1;
      }
    }
    dVar1 = this->mbr_0x8;
    uVar2 = *(undefined4 *)(dVar1 + iVar10 * 4);
    *(undefined4 *)(dVar1 + iVar10 * 4) = *(undefined4 *)(dVar1 + iVar11 * 4);
    *(undefined4 *)(dVar1 + iVar11 * 4) = uVar2;
  }
  return 1;
}



// Function at 005759f0

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5759f0(cls_0x5756d0 *this)

{
  undefined4 *puVar1;
  int *piVar2;
  
  meth_0x578210(this);
  piVar2 = (int *)this->mbr_0x8;
  this->mbr_0x18 = 0xffffffff;
  if (piVar2 != (int *)this->mbr_0xc) {
    do {
      if ((undefined4 *)*piVar2 != (undefined4 *)0x0) {
        (***(code ***)(undefined4 *)*piVar2)(1);
      }
      piVar2 = piVar2 + 1;
    } while (piVar2 != (int *)this->mbr_0xc);
  }
  puVar1 = (undefined4 *)this->mbr_0x20;
  this->mbr_0xc = this->mbr_0x8;
  this->mbr_0xf0 = 0;
  this->mbr_0x28 = 0xffffffff;
  if (puVar1 != (undefined4 *)0x0) {
    puVar1[1] = *puVar1;
  }
  piVar2 = (int *)this->mbr_0x0;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 8))(piVar2);
    this->mbr_0x0 = 0;
  }
  piVar2 = (int *)this->mbr_0x4;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 8))(piVar2);
    this->mbr_0x4 = 0;
  }
  if ((HANDLE)this->mbr_0x128 != (HANDLE)0x0) {
    CloseHandle((HANDLE)this->mbr_0x128);
    this->mbr_0x128 = 0;
  }
  if ((HANDLE)this->mbr_0x124 != (HANDLE)0x0) {
    CloseHandle((HANDLE)this->mbr_0x124);
    this->mbr_0x124 = 0;
  }
  if ((HANDLE)this->mbr_0x12c != (HANDLE)0x0) {
    CloseHandle((HANDLE)this->mbr_0x12c);
    this->mbr_0x12c = 0;
  }
  return;
}



// Function at 00575e90

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x575e90(cls_0x5756d0 *this)

{
  return (int)(this->mbr_0xc - this->mbr_0x8) >> 2;
}



// Function at 00575ea0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x575ea0(cls_0x5756d0 *this)

{
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) && (in_stack_00000004 < (int)(this->mbr_0xc - this->mbr_0x8) >> 2)) {
    return *(undefined4 *)(this->mbr_0x8 + in_stack_00000004 * 4);
  }
  return 0;
}



// Function at 00575ed0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x575ed0(cls_0x5756d0 *this)

{
  undefined4 *puVar1;
  int *piVar2;
  HRESULT HVar3;
  uint uVar4;
  int iVar5;
  LPVOID in_stack_00000004;
  
  if (this->mbr_0xf0 != 0) {
    return 0;
  }
  if (((int)in_stack_00000004 < 0) &&
     ((int)(this->mbr_0xc - this->mbr_0x8) >> 2 <= (int)in_stack_00000004)) {
    in_stack_00000004 = (LPVOID)0xffffffff;
  }
  if ((LPVOID)this->mbr_0x18 != in_stack_00000004) {
    this->mbr_0x18 = (dword)in_stack_00000004;
    puVar1 = (undefined4 *)this->mbr_0x20;
    this->mbr_0x24 = 0;
    this->mbr_0xf0 = 0;
    this->mbr_0x28 = 0xffffffff;
    if (puVar1 != (undefined4 *)0x0) {
      puVar1[1] = *puVar1;
    }
    piVar2 = (int *)this->mbr_0x0;
    if (piVar2 != (int *)0x0) {
      (**(code **)(*piVar2 + 8))(piVar2);
      this->mbr_0x0 = 0;
    }
    in_stack_00000004 = (LPVOID)0x0;
    HVar3 = CoCreateInstance((IID *)&DAT_005b9d38,(LPUNKNOWN)0x0,1,(IID *)&DAT_005b9d28,
                             &stack0x00000004);
    uVar4 = ~-(uint)(HVar3 != 0) & (uint)in_stack_00000004;
    this->mbr_0x0 = uVar4;
    if (uVar4 == 0) goto LAB_00575fa3;
    if (-1 < (int)this->mbr_0x18) {
      iVar5 = (**(code **)(**(int **)(this->mbr_0x8 + this->mbr_0x18 * 4) + 0x20))(this->mbr_0x4);
      if (iVar5 == 0) {
LAB_00575fa3:
        this->mbr_0x18 = 0xffffffff;
        return 0;
      }
      iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x98))((int *)this->mbr_0x0,iVar5,0);
      if (iVar5 != 0) goto LAB_00575fa3;
    }
    ((undefined4 *)this->mbr_0x20)[1] = *(undefined4 *)this->mbr_0x20;
  }
  return 1;
}



// Function at 00575fd0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x575fd0(cls_0x5756d0 *this)

{
  int *piVar1;
  undefined4 *puVar2;
  byte *pbVar3;
  uint uVar4;
  HRESULT HVar5;
  uint extraout_ECX;
  int iVar6;
  dword dVar7;
  byte *in_stack_00000004;
  byte *pbVar8;
  
  dVar7 = 0;
  iVar6 = (int)(this->mbr_0xc - this->mbr_0x8) >> 2;
  if (0 < iVar6) {
    do {
      if (((-1 < (int)dVar7) && ((int)dVar7 < (int)(this->mbr_0xc - this->mbr_0x8) >> 2)) &&
         (piVar1 = *(int **)(this->mbr_0x8 + dVar7 * 4), piVar1 != (int *)0x0)) {
        pbVar8 = in_stack_00000004;
        pbVar3 = (byte *)(**(code **)(*piVar1 + 8))();
        uVar4 = FUN_0059a530(extraout_ECX,pbVar3,pbVar8);
        if (uVar4 == 0) {
          if (this->mbr_0xf0 != 0) {
            return 0;
          }
          if (((int)dVar7 < 0) && ((int)(this->mbr_0xc - this->mbr_0x8) >> 2 <= (int)dVar7)) {
            dVar7 = 0xffffffff;
          }
          if (this->mbr_0x18 != dVar7) {
            this->mbr_0x18 = dVar7;
            puVar2 = (undefined4 *)this->mbr_0x20;
            this->mbr_0x24 = 0;
            this->mbr_0xf0 = 0;
            this->mbr_0x28 = 0xffffffff;
            if (puVar2 != (undefined4 *)0x0) {
              puVar2[1] = *puVar2;
            }
            piVar1 = (int *)this->mbr_0x0;
            if (piVar1 != (int *)0x0) {
              (**(code **)(*piVar1 + 8))(piVar1);
              this->mbr_0x0 = 0;
            }
            in_stack_00000004 = (byte *)0x0;
            HVar5 = CoCreateInstance((IID *)&DAT_005b9d38,(LPUNKNOWN)0x0,1,(IID *)&DAT_005b9d28,
                                     &stack0x00000004);
            uVar4 = ~-(uint)(HVar5 != 0) & (uint)in_stack_00000004;
            this->mbr_0x0 = uVar4;
            if ((uVar4 == 0) ||
               ((-1 < (int)this->mbr_0x18 &&
                ((iVar6 = (**(code **)(**(int **)(this->mbr_0x8 + this->mbr_0x18 * 4) + 0x20))
                                    (this->mbr_0x4), iVar6 == 0 ||
                 (iVar6 = (**(code **)(*(int *)this->mbr_0x0 + 0x98))((int *)this->mbr_0x0,iVar6,0),
                 iVar6 != 0)))))) {
              this->mbr_0x18 = 0xffffffff;
              return 0;
            }
            ((undefined4 *)this->mbr_0x20)[1] = *(undefined4 *)this->mbr_0x20;
          }
          return 1;
        }
      }
      dVar7 = dVar7 + 1;
    } while ((int)dVar7 < iVar6);
  }
  return 0;
}



// Function at 005761c0

dword __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5761c0(cls_0x5756d0 *this)

{
  return this->mbr_0x18;
}



// Function at 005761d0

uint __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5761d0(cls_0x5756d0 *this)

{
  LPCVOID *ppvVar1;
  cls_0x5756d0 *pcVar2;
  undefined4 *puVar3;
  int *piVar4;
  int iVar5;
  cls_0x5756d0 *pcVar6;
  dword dVar7;
  int *piVar8;
  int *piVar9;
  bool bVar10;
  int iVar11;
  undefined4 local_5c [6];
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  void *pvStack_24;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2692;
  local_c = ExceptionList;
  if (this->mbr_0x0 == 0) {
    return 0;
  }
  if (this->mbr_0xf0 != 0) {
    return 1;
  }
  puVar3 = local_5c;
  for (iVar5 = 0x14; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_44 = DAT_005b9f68;
  local_5c[0] = 0x50;
  local_40 = DAT_005b9f6c;
  local_3c = DAT_005b9f70;
  local_38 = DAT_005b9f74;
  ExceptionList = &local_c;
  pcVar2 = (cls_0x5756d0 *)FUN_00482fb0(0x10);
  local_4 = 0;
  pcVar6 = (cls_0x5756d0 *)0x0;
  if (pcVar2 != (cls_0x5756d0 *)0x0) {
    pcVar2->mbr_0xc = 8;
    puVar3 = FUN_00482fb0(0x620);
    pcVar2->mbr_0x0 = (dword)puVar3;
    pcVar2->mbr_0x4 = (dword)puVar3;
    pcVar2->mbr_0x8 = (dword)(puVar3 + pcVar2->mbr_0xc * 0x31);
    pcVar6 = pcVar2;
  }
  local_4 = 0xffffffff;
  pcVar2 = pcVar6;
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x34))
                    ((int *)this->mbr_0x0,local_5c,0,&LAB_005763e0,pcVar6,0x92);
  if (iVar5 == -0x7788fea2) {
    ExceptionList = pvStack_24;
    return 1;
  }
  iVar11 = iVar5;
  if (iVar5 == -0x7fffbffb) {
    (**(code **)(*(int *)this->mbr_0x0 + 0x10))((int *)this->mbr_0x0);
  }
  ppvVar1 = (LPCVOID *)this->mbr_0x20;
  if (ppvVar1 != (LPCVOID *)0x0) {
    FUN_004830f0(*ppvVar1);
    FUN_004830f0(ppvVar1);
  }
  this->mbr_0x20 = (dword)pcVar6;
  if (-1 < (int)this->mbr_0x28) {
    piVar4 = (int *)pcVar6->mbr_0x0;
    dVar7 = 0;
    if (piVar4 != (int *)pcVar6->mbr_0x4) {
      do {
        iVar5 = 4;
        bVar10 = true;
        piVar8 = piVar4;
        piVar9 = (int *)&stack0xffffff7c;
        do {
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          bVar10 = *piVar8 == *piVar9;
          piVar8 = piVar8 + 1;
          piVar9 = piVar9 + 1;
        } while (bVar10);
        iVar5 = iVar11;
        if (bVar10) break;
        piVar4 = piVar4 + 0x31;
        dVar7 = dVar7 + 1;
      } while (piVar4 != (int *)pcVar6->mbr_0x4);
    }
    if ((int)(pcVar6->mbr_0x4 - pcVar6->mbr_0x0) / 0xc4 <= (int)dVar7) {
      dVar7 = 0xffffffff;
    }
    pcVar2->mbr_0x28 = dVar7;
    this = pcVar2;
  }
  *(uint *)&this->field_0x1c = (uint)(iVar5 == 0);
  ExceptionList = pvStack_24;
  return (uint)(iVar5 == 0);
}



// Function at 00576880

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x576880(cls_0x5756d0 *this)

{
  int **ppiVar1;
  dword dVar2;
  int iVar3;
  DWORD DVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  int *piVar9;
  bool bVar10;
  undefined4 local_10;
  int iStack_c;
  
  if (this->mbr_0x0 == 0) {
    return 0;
  }
  if (this->mbr_0xf0 != 0) {
    return 1;
  }
  local_10 = 0;
  DVar4 = GetTickCount();
  if (this->mbr_0x24 < DVar4) {
    ppiVar1 = (int **)this->mbr_0x20;
    dVar2 = this->mbr_0x28;
    this->mbr_0x20 = 0;
    meth_0x5761d0(this);
    if (dVar2 == this->mbr_0x28) {
      piVar5 = *ppiVar1;
      piVar8 = (int *)this->mbr_0x20;
      iVar3 = ((int)ppiVar1[1] - (int)piVar5) / 0xc4;
      if (iVar3 == (piVar8[1] - *piVar8) / 0xc4) {
        iStack_c = 0;
        if (0 < iVar3) {
          iVar7 = *piVar8 - (int)piVar5;
          do {
            iVar6 = 0x31;
            bVar10 = true;
            piVar8 = piVar5;
            piVar9 = (int *)(iVar7 + (int)piVar5);
            do {
              if (iVar6 == 0) break;
              iVar6 = iVar6 + -1;
              bVar10 = *piVar8 == *piVar9;
              piVar8 = piVar8 + 1;
              piVar9 = piVar9 + 1;
            } while (bVar10);
            if (!bVar10) goto LAB_00576963;
            piVar5 = piVar5 + 0x31;
            iStack_c = iStack_c + 1;
          } while (iStack_c < iVar3);
        }
      }
      else {
LAB_00576963:
        local_10 = 1;
      }
    }
    else {
      local_10 = 1;
    }
    this->mbr_0x24 = DVar4 + 1000;
    if (ppiVar1 != (int **)0x0) {
      FUN_004830f0(*ppiVar1);
      FUN_004830f0(ppiVar1);
    }
  }
  return local_10;
}



// Function at 005769a0

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5769a0(cls_0x5756d0 *this)

{
  return (((int *)this->mbr_0x20)[1] - *(int *)this->mbr_0x20) / 0xc4;
}



// Function at 005769c0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5769c0(cls_0x5756d0 *this,int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if (-1 < param_1) {
    piVar1 = (int *)this->mbr_0x20;
    if (param_1 < (piVar1[1] - *piVar1) / 0xc4) {
      if (in_stack_00000008 != (undefined4 *)0x0) {
        puVar3 = (undefined4 *)(*piVar1 + param_1 * 0xc4);
        for (iVar2 = 0x31; iVar2 != 0; iVar2 = iVar2 + -1) {
          *in_stack_00000008 = *puVar3;
          puVar3 = puVar3 + 1;
          in_stack_00000008 = in_stack_00000008 + 1;
        }
      }
      return 1;
    }
  }
  return 0;
}



// Function at 00576de0

int __thiscall
OOAnalyzer::cls_0x5756d0::meth_0x576de0(cls_0x5756d0 *this,undefined param_1,undefined4 param_2)

{
  dword *pdVar1;
  dword dVar2;
  int *piVar3;
  HANDLE hMutex;
  DWORD DVar4;
  uint uVar5;
  int iVar6;
  dword *pdVar7;
  dword *pdVar8;
  undefined4 *puVar9;
  int3 in_stack_00000005;
  char *in_stack_0000000c;
  DWORD local_98;
  undefined local_94 [28];
  undefined auStack_78 [28];
  dword local_5c [4];
  dword local_4c;
  dword local_48;
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a26af;
  local_c = ExceptionList;
  pdVar1 = &this->mbr_0x104;
  if (in_stack_0000000c == (char *)0x0) {
    ExceptionList = &local_c;
    _strncpy((char *)pdVar1,s_Unknown_005e5bf4,0x1f);
    this->mbr_0x123 = 0;
    local_98 = 0x20;
    GetUserNameA((LPSTR)pdVar1,&local_98);
  }
  else {
    ExceptionList = &local_c;
    _strncpy((char *)pdVar1,in_stack_0000000c,0x1f);
    this->mbr_0x123 = 0;
  }
  dVar2 = this->mbr_0xf0;
  if (dVar2 != 0) {
    if (this->mbr_0xf8 != 0) {
      DAT_006766ac = 0;
      DAT_006766b0 = 0xffffffff;
      goto LAB_00576e9e;
    }
    if (dVar2 != 0) {
      if (this->mbr_0xf4 != 0) {
        ExceptionList = local_c;
        return 1;
      }
      ExceptionList = local_c;
      return 0;
    }
  }
  if (this->mbr_0xf4 != 0) {
    ExceptionList = local_c;
    return 1;
  }
  if (dVar2 != 0) {
    ExceptionList = local_c;
    return 1;
  }
LAB_00576e9e:
  if ((in_stack_00000005 < 0) ||
     ((((int *)this->mbr_0x20)[1] - *(int *)this->mbr_0x20) / 0xc4 <= (int)_param_1)) {
    _param_1 = 0xffffffff;
  }
  this->mbr_0x28 = _param_1;
  if ((dVar2 != 0) && (this->mbr_0xf8 != 0)) {
    this->mbr_0x28 = 0;
LAB_0057706f:
    cls_0x588410::cls_0x588410((cls_0x588410 *)local_94);
    uStack_4 = 0;
    cls_0x588410::cls_0x588410((cls_0x588410 *)auStack_78);
    auStack_78._24_4_ = 0;
    uStack_4 = CONCAT31(uStack_4._1_3_,1);
    cls_0x588410::meth_0x588660((cls_0x588410 *)local_94);
    iVar6 = cls_0x5b9f28::meth_0x579ce0
                      ((cls_0x5b9f28 *)&this->mbr_0x248,1,local_94._0_4_,
                       local_94._12_4_ - local_94._0_4_,auStack_78,20000);
    if ((iVar6 != 0) && ((this->mbr_0xf0 == 0 || (this->mbr_0xf8 == 0)))) {
      meth_0x578210(this);
    }
    uStack_4 = uStack_4 & 0xffffff00;
    cls_0x588410::meth_0x588480((cls_0x588410 *)auStack_78);
    uStack_4 = 0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)local_94);
    ExceptionList = local_c;
    return iVar6;
  }
  pdVar1 = &this->mbr_0x2c;
  if ((-1 < (int)_param_1) &&
     (piVar3 = (int *)this->mbr_0x20, (int)_param_1 < (piVar3[1] - *piVar3) / 0xc4)) {
    if (pdVar1 != (dword *)0x0) {
      pdVar7 = (dword *)(*piVar3 + _param_1 * 0xc4);
      pdVar8 = pdVar1;
      for (iVar6 = 0x31; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pdVar8 = *pdVar7;
        pdVar7 = pdVar7 + 1;
        pdVar8 = pdVar8 + 1;
      }
    }
    dVar2 = this->mbr_0x30;
    puVar9 = (undefined4 *)(auStack_78 + 0x1c);
    for (iVar6 = 0x14; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    local_5c[2] = *pdVar1;
    local_4c = this->mbr_0x34;
    local_48 = this->mbr_0x38;
    local_5c[0] = 0x50;
    local_5c[3] = dVar2;
    iVar6 = (**(code **)(*(int *)this->mbr_0x0 + 0x60))((int *)this->mbr_0x0,auStack_78 + 0x1c,0x81)
    ;
    while (iVar6 != 0) {
      Sleep(200);
      if (iVar6 == -0x7788fea2) {
        ExceptionList = local_c;
        return 1;
      }
      iVar6 = (**(code **)(*(int *)this->mbr_0x0 + 0x60))
                        ((int *)this->mbr_0x0,auStack_78 + 0x1c,0x81);
    }
    this->mbr_0xf0 = 1;
    this->mbr_0xf4 = 0;
    this->mbr_0x19c = 0;
    this->mbr_0x100 = 0;
    iVar6 = meth_0x578360(this);
    if (iVar6 != 0) {
      DVar4 = GetTickCount();
      uVar5 = GetTickCount();
      while (uVar5 < DVar4 + 2000) {
        iVar6 = (**(code **)(*(int *)this->mbr_0x0 + 0xb8))((int *)this->mbr_0x0,1,&local_98);
        if (iVar6 == 0) {
          WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
          if (this->mbr_0x180 == this->mbr_0x184) {
            cls_0x57be50::meth_0x57be50((cls_0x57be50 *)&this->mbr_0x17c);
          }
          puVar9 = (undefined4 *)this->mbr_0x180;
          hMutex = (HANDLE)this->mbr_0x12c;
          *puVar9 = 1;
          this->mbr_0x180 = (dword)(puVar9 + 1);
          ReleaseMutex(hMutex);
          meth_0x578b80(this);
          meth_0x577350(this);
          goto LAB_0057706f;
        }
        uVar5 = GetTickCount();
      }
      meth_0x578210(this);
    }
  }
  ExceptionList = local_c;
  return 1;
}



// Function at 00577350

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x577350(cls_0x5756d0 *this)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  
  if ((this->mbr_0xf0 != 0) && (this->mbr_0xf4 == 0)) {
    WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
    uVar4 = 0;
    iVar2 = (**(code **)(*(int *)this->mbr_0x0 + 0x58))((int *)this->mbr_0x0);
    if (iVar2 == -0x7788ffe2) {
      puVar3 = FUN_00482fb0(uVar4);
      piVar1 = (int *)this->mbr_0x0;
      iVar2 = *piVar1;
      *puVar3 = 0x50;
      iVar2 = (**(code **)(iVar2 + 0x58))(piVar1,puVar3,&stack0xfffffff0);
      if (iVar2 == 0) {
        FUN_00576500((int)puVar3,&this->mbr_0x2c);
      }
      FUN_004830f0(puVar3);
    }
    ReleaseMutex((HANDLE)this->mbr_0x12c);
  }
  return;
}



// Function at 005773f0

void __thiscall
OOAnalyzer::cls_0x5756d0::meth_0x5773f0
          (cls_0x5756d0 *this,undefined4 param_1,undefined4 *param_2,char *param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  bool bVar4;
  int in_stack_00000010;
  
  FUN_0058ecbc(param_3,in_stack_00000010,(byte *)s__s_s__s__s__02x_02x_02x_02x_02x__005e5c00);
  puVar3 = param_2;
  for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  bVar4 = DAT_00668124 == 0;
  *param_2 = 0x50;
  if (bVar4) {
    param_2[1] = 0x30c0;
  }
  else {
    param_2[1] = 0x3080;
  }
  uVar1 = DAT_005b9f68;
  param_2[0xc] = param_3;
  param_2[6] = uVar1;
  uVar1 = DAT_005b9f6c;
  param_2[10] = 0x40;
  param_2[7] = uVar1;
  param_2[8] = DAT_005b9f70;
  param_2[9] = DAT_005b9f74;
  return;
}



// Function at 00577500

/* WARNING: Type propagation algorithm not settling */

int __thiscall
OOAnalyzer::cls_0x5756d0::meth_0x577500
          (cls_0x5756d0 *this,undefined param_1,undefined param_2,undefined4 param_3)

{
  byte bVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  undefined uVar3;
  undefined uVar4;
  undefined3 extraout_var;
  int iVar5;
  byte *pbVar6;
  HANDLE pvVar7;
  undefined4 uVar8;
  cls_0x5b4f30 *this_00;
  dword dVar9;
  cls_0x5b4f30 *this_01;
  dword *pdVar10;
  dword *pdVar11;
  undefined4 *puVar12;
  undefined3 in_stack_00000009;
  cls_0x5b4f30 *in_stack_00000010;
  DWORD local_f4 [4];
  undefined4 uStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  undefined4 uStack_d8;
  undefined4 uStack_d4;
  char *pcStack_cc;
  char acStack_ac [31];
  undefined uStack_8d;
  undefined auStack_8c [132];
  dword *pdStack_8;
  char *pcStack_4;
  
  pdVar10 = &this->mbr_0x104;
  if (in_stack_00000010 == (cls_0x5b4f30 *)0x0) {
    _strncpy((char *)pdVar10,s_Unknown_005e5c44,0x1f);
    this->mbr_0x123 = 0;
    local_f4[0] = 0x20;
    GetUserNameA((LPSTR)pdVar10,local_f4);
  }
  else {
    _strncpy((char *)pdVar10,(char *)(in_stack_00000010->TPlayScreen).TScreen.mbr_0x38,0x1f);
    this->mbr_0x123 = 0;
  }
  pcVar2 = (in_stack_00000010->TPlayScreen).TScreen.vftptr_0x0;
  (*pcVar2->virt_meth_0x477eb0_472)(in_stack_00000010);
  (*pcVar2->virt_meth_0x477e60_452)(in_stack_00000010);
  pcVar2 = (in_stack_00000010->TPlayScreen).TScreen.vftptr_0x0;
  (*pcVar2->virt_meth_0x477ed0_480)(in_stack_00000010);
  (*pcVar2->virt_meth_0x477e80_460)(in_stack_00000010);
  pcVar2 = (in_stack_00000010->TPlayScreen).TScreen.vftptr_0x0;
  uVar4 = (*pcVar2->virt_meth_0x477ef0_488)(in_stack_00000010);
  iVar5 = CONCAT31(extraout_var,uVar4);
  (*pcVar2->virt_meth_0x477ea0_468)(in_stack_00000010);
  pdVar10 = pdStack_8;
  this->mbr_0x19c = 0;
  if (this->mbr_0xf0 == 0) {
LAB_005775d0:
    if (this->mbr_0xf4 != 0) {
      return 0;
    }
  }
  else {
    if (this->mbr_0xf8 == 0) {
      if (this->mbr_0xf0 != 0) {
        if (this->mbr_0xf4 != 0) {
          return 0;
        }
        return 1;
      }
      goto LAB_005775d0;
    }
    DAT_006766ac = 0;
    DAT_006766b0 = 0xffffffff;
  }
  iVar5 = FUN_00570c70(&in_stack_00000010->TPlayScreen,pdStack_8 + 0x26,iVar5);
  if (iVar5 != 0) {
    return iVar5;
  }
  pbVar6 = (byte *)cls_0x4609f0::meth_0x460d00
                             ((cls_0x4609f0 *)&UNK_0065a630.field_0x138,pdVar10 + 0x14);
  uVar4 = DAT_005d79dc;
  if (pbVar6 == (byte *)0x0) {
    return 1;
  }
  bVar1 = pbVar6[0x78];
  *(byte *)((int)pdVar10 + 0x91) = pbVar6[0x79];
  *(byte *)(pdVar10 + 0x24) = bVar1;
  uVar3 = DAT_005d79dd;
  *(undefined *)((int)pdVar10 + 0x92) = uVar4;
  *(undefined *)((int)pdVar10 + 0x93) = uVar3;
  _strncpy((char *)(pdVar10 + 0x1c),pcStack_4,0x1f);
  *(undefined *)((int)pdVar10 + 0x8f) = 0;
  if (in_stack_00000010 == (cls_0x5b4f30 *)0x0) {
    return 1;
  }
  dVar9 = this->mbr_0xf0;
  pdVar11 = &this->mbr_0x2c;
  for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
    *pdVar11 = *pdVar10;
    pdVar10 = pdVar10 + 1;
    pdVar11 = pdVar11 + 1;
  }
  if ((dVar9 == 0) || (this->mbr_0xf8 == 0)) {
    _strncpy(acStack_ac,(char *)&this->mbr_0x3c,0x1f);
    uStack_8d = 0;
    puVar12 = (undefined4 *)&stack0xffffff04;
    for (iVar5 = 0x14; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar12 = 0;
      puVar12 = puVar12 + 1;
    }
    uStack_e4 = DAT_005b9f68;
    uStack_e0 = DAT_005b9f6c;
    pcStack_cc = acStack_ac;
    uStack_d8 = DAT_005b9f74;
    uStack_dc = DAT_005b9f70;
    uStack_d4 = 0x40;
    iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x60))((int *)this->mbr_0x0,&stack0xffffff04,2);
    if (iVar5 != 0) {
      return 1;
    }
    this->mbr_0xf0 = 1;
    this->mbr_0xf4 = 1;
    this->mbr_0x100 = 3;
    ResetEvent((HANDLE)this->mbr_0x124);
    pvVar7 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,(LPTHREAD_START_ROUTINE)&LAB_00578490,this,0,
                          &this->mbr_0x130);
    this->mbr_0x134 = (dword)pvVar7;
    if (pvVar7 == (HANDLE)0x0) {
LAB_005777da:
      (**(code **)(*(int *)this->mbr_0x0 + 0x10))((int *)this->mbr_0x0);
      return 1;
    }
    WaitForSingleObject((HANDLE)this->mbr_0x124,0xffffffff);
    WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
    dVar9 = this->mbr_0x148;
    ReleaseMutex((HANDLE)this->mbr_0x12c);
    if (dVar9 == 0) {
      meth_0x578210(this);
      goto LAB_005777da;
    }
    WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
    cls_0x57bd10::meth_0x57bd10((cls_0x57bd10 *)&this->mbr_0x17c);
    ReleaseMutex((HANDLE)this->mbr_0x12c);
    meth_0x578b80(this);
  }
  if ((pdStack_8 != (dword *)0x0) && ((this->mbr_0xf0 == 0 || (this->mbr_0xf4 != 0)))) {
    if (this->mbr_0xf4 != 0) {
      meth_0x5773f0(this,pdStack_8,&stack0xffffff04,auStack_8c);
      iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x7c))((int *)this->mbr_0x0,&stack0xffffff04,0);
      if (iVar5 != 0) goto LAB_00577888;
    }
    pdVar10 = &this->mbr_0x2c;
    for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pdVar10 = *pdStack_8;
      pdStack_8 = pdStack_8 + 1;
      pdVar10 = pdVar10 + 1;
    }
  }
LAB_00577888:
  if ((_param_2 != 0) &&
     (iVar5 = cls_0x45f7c0::meth_0x48df70((cls_0x45f7c0 *)&DAT_0065a618,_param_2), iVar5 == 0)) {
    cls_0x45f7c0::meth_0x51eda0((cls_0x45f7c0 *)&DAT_0065a890);
  }
  uVar8 = cls_0x45f7c0::meth_0x51f2e0((cls_0x45f7c0 *)&DAT_0065a890);
  this_00 = (cls_0x5b4f30 *)cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,uVar8);
  if (this_00 == (cls_0x5b4f30 *)0x0) {
    dVar9 = cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,in_stack_00000010);
    this->mbr_0x13c = dVar9;
  }
  else {
    if ((*pbVar6 & 8) == 0) {
      cls_0x45f7c0::meth_0x51f200((cls_0x45f7c0 *)&DAT_0065a890,in_stack_00000010->mbr_0x664);
      (*((in_stack_00000010->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                ((TScreen *)in_stack_00000010);
      in_stack_00000010 = this_00;
    }
    else {
      cls_0x45f7c0::meth_0x51f200((cls_0x45f7c0 *)&DAT_0065a890,this->mbr_0x13c);
      (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                ((TScreen *)this_00);
    }
    cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,in_stack_00000010);
    this->mbr_0x13c = in_stack_00000010->mbr_0x664;
  }
  cls_0x45f7c0::meth_0x51eef0((cls_0x45f7c0 *)&DAT_0065a890);
  if ((*pbVar6 & 8) != 0) {
    if (DAT_0065a784 < 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_00460400(iVar5,(int *)in_stack_00000010,-1);
  }
  this_01 = in_stack_00000010;
  if (this_00 != (cls_0x5b4f30 *)0x0) {
    this_01 = this_00;
  }
  cls_0x5b4f30::meth_0x51d680(this_01);
  DAT_00667fcc = in_stack_00000010;
  return 0;
}



// Function at 00577be0

char __thiscall OOAnalyzer::cls_0x5756d0::meth_0x577be0(cls_0x5756d0 *this)

{
  if ((this->mbr_0xf0 == 0) && (this->mbr_0xf4 == 0)) {
    return (-(this->mbr_0x140 != 0) & 2U) + 1;
  }
  return '\0';
}



// Function at 00577c10

bool __thiscall OOAnalyzer::cls_0x5756d0::meth_0x577c10(cls_0x5756d0 *this)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uStack_14;
  undefined4 uStack_10;
  undefined4 *puStack_c;
  undefined4 local_4;
  
  puStack_c = &local_4;
  if (this->mbr_0xfd != 0) {
    return false;
  }
  local_4 = 0;
  uStack_10 = 0;
  uStack_14 = 0;
  iVar2 = (**(code **)(*(int *)this->mbr_0x4 + 0x20))((int *)this->mbr_0x4);
  if ((iVar2 != 0) && (iVar2 != -0x7788ffe2)) {
    return false;
  }
  puVar3 = FUN_00482ef0(uStack_14);
  piVar1 = (int *)this->mbr_0x4;
  iVar2 = *piVar1;
  *puVar3 = 0x28;
  iVar2 = (**(code **)(iVar2 + 0x20))(piVar1,0,puVar3,&uStack_14);
  FUN_00482f80(puVar3);
  return iVar2 == 0;
}



// Function at 00577c90

int * __thiscall OOAnalyzer::cls_0x5756d0::meth_0x577c90(cls_0x5756d0 *this)

{
  char *_Source;
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  dword dVar5;
  undefined4 *unaff_EDI;
  undefined4 *puVar6;
  int *piVar7;
  bool bVar8;
  int *piVar9;
  uint uStack_1c;
  undefined4 *puStack_18;
  undefined4 local_c [2];
  undefined4 local_4;
  
  if (this->mbr_0xfd != 0) {
    return (int *)0x0;
  }
  if (this->mbr_0xfc != 0) {
    return (int *)0x1;
  }
  piVar9 = (int *)this->mbr_0x4;
  puStack_18 = local_c;
  dVar5 = 0;
  local_c[0] = 0;
  uStack_1c = 0;
  local_4 = 0;
  iVar2 = (**(code **)(*piVar9 + 0x20))(piVar9,0);
  if ((iVar2 != 0) && (iVar2 != -0x7788ffe2)) {
    return (int *)0x0;
  }
  puVar3 = FUN_00482ef0(uStack_1c);
  piVar4 = (int *)this->mbr_0x4;
  iVar2 = *piVar4;
  *puVar3 = 0x28;
  puStack_18 = puVar3;
  iVar2 = (**(code **)(iVar2 + 0x20))(piVar4,0,puVar3,&uStack_1c);
  if (iVar2 == 0) {
    puVar6 = (undefined4 *)this->mbr_0x20;
    this->mbr_0xf0 = 0;
    this->mbr_0x28 = 0xffffffff;
    if (puVar6 != (undefined4 *)0x0) {
      puVar6[1] = *puVar6;
    }
    piVar4 = (int *)this->mbr_0x0;
    if (piVar4 != (int *)0x0) {
      (**(code **)(*piVar4 + 8))(piVar4);
      this->mbr_0x0 = 0;
    }
    if ((*(byte *)(puVar3 + 1) & 2) != 0) {
      iVar2 = puVar3[2];
      if (DAT_00668124 == 0) {
        *(undefined4 *)(iVar2 + 4) = 0x30c0;
      }
      else {
        *(undefined4 *)(iVar2 + 4) = 0x3080;
      }
      if (*(uint *)(iVar2 + 0x28) < 2) {
        *(undefined4 *)(iVar2 + 0x28) = 0x10;
      }
    }
    puVar6 = puVar3;
    if (0 < (int)(this->mbr_0xc - this->mbr_0x8 & 0xfffffffc)) {
      do {
        puVar3 = unaff_EDI;
        if (((int)dVar5 < 0) || ((int)(this->mbr_0xc - this->mbr_0x8) >> 2 <= (int)dVar5)) {
          piVar4 = (int *)0x0;
        }
        else {
          piVar4 = *(int **)(this->mbr_0x8 + dVar5 * 4);
        }
        piVar4 = (int *)(**(code **)(*piVar4 + 0x18))();
        iVar2 = 4;
        bVar8 = true;
        piVar7 = puVar6 + 4;
        do {
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          bVar8 = *piVar4 == *piVar7;
          piVar4 = piVar4 + 1;
          piVar7 = piVar7 + 1;
        } while (bVar8);
        if (bVar8) {
          this->mbr_0x18 = dVar5;
          break;
        }
        dVar5 = dVar5 + 1;
        puVar6 = puVar3;
        unaff_EDI = puVar3;
      } while ((int)dVar5 < (int)(this->mbr_0xc - this->mbr_0x8) >> 2);
    }
    if ((puVar3[3] != 0) && (_Source = *(char **)(puVar3[3] + 8), _Source != (char *)0x0)) {
      _strncpy((char *)&this->mbr_0x104,_Source,0x1f);
      this->mbr_0x123 = 0;
    }
    iVar2 = (**(code **)(*(int *)this->mbr_0x4 + 0x30))((int *)this->mbr_0x4,0,0,puVar3);
    if (iVar2 == 0) {
      uVar1 = puVar3[1];
      this->mbr_0xfc = 1;
      this->mbr_0xfe = (byte)(uVar1 >> 1) & 1;
      piVar9 = (int *)0x1;
    }
  }
  FUN_00482f80(puVar3);
  return piVar9;
}



// Function at 00577e40

bool __thiscall OOAnalyzer::cls_0x5756d0::meth_0x577e40(cls_0x5756d0 *this)

{
  undefined *puVar1;
  undefined4 *puVar2;
  cls_0x5756d0 **ppcVar3;
  int *piVar4;
  int iVar5;
  HANDLE pvVar6;
  undefined4 *puVar7;
  DWORD DVar8;
  uint uVar9;
  uint unaff_ESI;
  dword dVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  bool bVar13;
  undefined *puStack_1c;
  cls_0x5756d0 *pcStack_18;
  undefined4 uStack_14;
  
  if (this->mbr_0xf0 != 0) {
    if (this->mbr_0xf8 != 0) {
      return true;
    }
    if (this->mbr_0xf0 != 0) {
      return false;
    }
  }
  uStack_14 = 0x577e79;
  iVar5 = meth_0x577c90(this);
  if (iVar5 != 0) {
    uStack_14 = 0;
    puStack_1c = &DAT_005b9d28;
    pcStack_18 = this;
    iVar5 = (**(code **)(*(int *)this->mbr_0x4 + 0x3c))();
    if (iVar5 != 0) {
      return iVar5 == -0x7788fea2;
    }
    this->mbr_0xfd = 1;
    this->mbr_0xfc = 0;
    if (this->mbr_0xfe == 0) {
      this->mbr_0xf0 = 1;
      this->mbr_0xf4 = 0;
      this->mbr_0x19c = 0;
      this->mbr_0x100 = 0;
      ResetEvent((HANDLE)this->mbr_0x124);
      pvVar6 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,(LPTHREAD_START_ROUTINE)&LAB_00578490,this,
                            0,&this->mbr_0x130);
      this->mbr_0x134 = (dword)pvVar6;
      if (pvVar6 != (HANDLE)0x0) {
        WaitForSingleObject((HANDLE)this->mbr_0x124,0xffffffff);
        WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
        dVar10 = this->mbr_0x148;
        ReleaseMutex((HANDLE)this->mbr_0x12c);
        if (dVar10 != 0) {
          DVar8 = GetTickCount();
          uVar9 = GetTickCount();
          while (uVar9 < DVar8 + 2000) {
            iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0xb8))((int *)this->mbr_0x0,1,&pcStack_18);
            if (iVar5 == 0) {
              WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
              if (this->mbr_0x180 == this->mbr_0x184) {
                cls_0x57be50::meth_0x57be50((cls_0x57be50 *)&this->mbr_0x17c);
              }
              puVar7 = (undefined4 *)this->mbr_0x180;
              *puVar7 = 1;
              this->mbr_0x180 = (dword)(puVar7 + 1);
              ReleaseMutex((HANDLE)this->mbr_0x12c);
              meth_0x578b80(this);
              if ((this->mbr_0xf0 != 0) && (this->mbr_0xf4 == 0)) {
                WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
                puStack_1c = (undefined *)0x0;
                iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x58))
                                  ((int *)this->mbr_0x0,0,&puStack_1c);
                if (iVar5 == -0x7788ffe2) {
                  puVar7 = FUN_00482fb0(unaff_ESI);
                  piVar4 = (int *)this->mbr_0x0;
                  iVar5 = *piVar4;
                  *puVar7 = 0x50;
                  iVar5 = (**(code **)(iVar5 + 0x58))(piVar4,puVar7,&stack0xffffffd8);
                  if (iVar5 == 0) {
                    FUN_00576500((int)puVar7,&this->mbr_0x2c);
                  }
                  FUN_004830f0(puVar7);
                }
                ReleaseMutex((HANDLE)this->mbr_0x12c);
              }
              bVar13 = true;
              goto LAB_005780ed;
            }
            uVar9 = GetTickCount();
          }
        }
        meth_0x578210(this);
      }
      bVar13 = false;
LAB_005780ed:
      bVar13 = !bVar13;
    }
    else {
      this->mbr_0xf0 = 1;
      this->mbr_0xf4 = 1;
      this->mbr_0x100 = 3;
      ResetEvent((HANDLE)this->mbr_0x124);
      pvVar6 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,(LPTHREAD_START_ROUTINE)&LAB_00578490,this,
                            0,&this->mbr_0x130);
      this->mbr_0x134 = (dword)pvVar6;
      if (pvVar6 == (HANDLE)0x0) {
        bVar13 = true;
      }
      else {
        WaitForSingleObject((HANDLE)this->mbr_0x124,0xffffffff);
        WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
        puVar1 = (undefined *)this->mbr_0x148;
        ReleaseMutex((HANDLE)this->mbr_0x12c);
        if (puVar1 == (undefined *)0x0) {
          meth_0x578210(this);
          puStack_1c = puVar1;
        }
        else {
          puStack_1c = puVar1;
          WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
          pcStack_18 = (cls_0x5756d0 *)this->mbr_0x14c;
          if (this->mbr_0x180 == this->mbr_0x184) {
            dVar10 = this->mbr_0x188 + 8;
            puVar7 = FUN_00482fb0(dVar10 * 4);
            if ((int)this->mbr_0x188 <= (int)dVar10) {
              dVar10 = this->mbr_0x188;
            }
            puVar2 = (undefined4 *)this->mbr_0x17c;
            puVar11 = puVar2;
            puVar12 = puVar7;
            for (; dVar10 != 0; dVar10 = dVar10 - 1) {
              *puVar12 = *puVar11;
              puVar11 = puVar11 + 1;
              puVar12 = puVar12 + 1;
            }
            FUN_004830f0(puVar2);
            dVar10 = this->mbr_0x17c;
            this->mbr_0x17c = (dword)puVar7;
            this->mbr_0x180 = (dword)(puVar7 + ((int)(this->mbr_0x180 - dVar10) >> 2));
            dVar10 = this->mbr_0x188 + 8;
            this->mbr_0x188 = dVar10;
            this->mbr_0x184 = (dword)(puVar7 + dVar10);
          }
          ppcVar3 = (cls_0x5756d0 **)this->mbr_0x180;
          *ppcVar3 = pcStack_18;
          this->mbr_0x180 = (dword)(ppcVar3 + 1);
          ReleaseMutex((HANDLE)this->mbr_0x12c);
          meth_0x578b80(this);
        }
        bVar13 = puStack_1c == (undefined *)0x0;
      }
    }
    if (!bVar13) {
      this->mbr_0xf8 = 1;
      return true;
    }
  }
  return false;
}



// Function at 00578210

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578210(cls_0x5756d0 *this)

{
  dword dVar1;
  int *piVar2;
  undefined4 *puVar3;
  cls_0x570900 *this_00;
  int iVar4;
  DWORD DVar5;
  
  if ((this->mbr_0x148 != 0) && (this->mbr_0x134 != 0)) {
    WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
    this->mbr_0x148 = 0;
    ResetEvent((HANDLE)this->mbr_0x124);
    ReleaseMutex((HANDLE)this->mbr_0x12c);
    dVar1 = this->mbr_0x134;
    while (dVar1 != 0) {
      iVar4 = ReleaseMutex((HANDLE)this->mbr_0x12c);
      while (iVar4 != 0) {
        iVar4 = ReleaseMutex((HANDLE)this->mbr_0x12c);
      }
      SetEvent((HANDLE)this->mbr_0x128);
      DVar5 = WaitForSingleObject((HANDLE)this->mbr_0x124,1000);
      if (DVar5 != 0x102) break;
      dVar1 = this->mbr_0x134;
    }
    this->mbr_0x134 = 0;
    this->mbr_0x130 = 0;
  }
  piVar2 = (int *)this->mbr_0x0;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 0x10))(piVar2);
  }
  puVar3 = (undefined4 *)this->mbr_0x20;
  this->mbr_0x190 = this->mbr_0x18c;
  this->mbr_0x144 = 0;
  this->mbr_0xf0 = 0;
  this->mbr_0xf4 = 0;
  this->mbr_0xf8 = 0;
  this->mbr_0x100 = 3;
  if (puVar3 != (undefined4 *)0x0) {
    puVar3[1] = *puVar3;
  }
  while ((this_00 = DAT_006766d4, *(int *)this->mbr_0x150 != 0 &&
         (piVar2 = (int *)((int *)this->mbr_0x150)[2], piVar2 != (int *)0x0))) {
    (**(code **)(*piVar2 + 8))();
  }
  while (this_00 != (cls_0x570900 *)0x0) {
    DAT_006766d4 = this_00;
    cls_0x570900::meth_0x570900(this_00);
    FUN_004830f0(this_00);
    this_00 = DAT_006766d4;
  }
  DAT_006766d4 = this_00;
  DAT_006766ac = 0;
  DAT_006766b0 = 0xffffffff;
  return;
}



// Function at 00578360

dword __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578360(cls_0x5756d0 *this)

{
  dword dVar1;
  HANDLE pvVar2;
  
  ResetEvent((HANDLE)this->mbr_0x124);
  pvVar2 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,(LPTHREAD_START_ROUTINE)&LAB_00578490,this,0,
                        &this->mbr_0x130);
  this->mbr_0x134 = (dword)pvVar2;
  if (pvVar2 == (HANDLE)0x0) {
    return 0;
  }
  WaitForSingleObject((HANDLE)this->mbr_0x124,0xffffffff);
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  dVar1 = this->mbr_0x148;
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  if (dVar1 == 0) {
    meth_0x578210(this);
  }
  return dVar1;
}



// Function at 005789a0

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5789a0(cls_0x5756d0 *this)

{
  int iVar1;
  int in_stack_00000004;
  
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))((int *)this->mbr_0x0);
  if (iVar1 != 0) {
    iVar1 = FUN_00570ad0(in_stack_00000004);
    return iVar1;
  }
  return in_stack_00000004;
}



// Function at 005789f0

bool __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5789f0(cls_0x5756d0 *this)

{
  int iVar1;
  
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))((int *)this->mbr_0x0);
  return iVar1 == 0;
}



// Function at 00578a30

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578a30(cls_0x5756d0 *this)

{
  int iVar1;
  undefined4 uVar2;
  cls_0x570900 *in_stack_00000004;
  
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))((int *)this->mbr_0x0);
  if (iVar1 != 0) {
    in_stack_00000004 = (cls_0x570900 *)FUN_00570ad0((int)in_stack_00000004);
  }
  if (in_stack_00000004 == (cls_0x570900 *)0x0) {
    return 0;
  }
  uVar2 = cls_0x570900::meth_0x570c00(in_stack_00000004);
  return uVar2;
}



// Function at 00578a90

undefined1 * __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578a90(cls_0x5756d0 *this)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  char *pcVar8;
  char *pcVar9;
  undefined4 *puVar10;
  
  iVar3 = (**(code **)(*(int *)this->mbr_0x0 + 0x54))((int *)this->mbr_0x0);
  if (iVar3 != -0x7788ffe2) {
    uVar5 = 0xffffffff;
    pcVar8 = s_Unknown_005e5c54;
    do {
      pcVar9 = pcVar8;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar9 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar9;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    puVar4 = (undefined4 *)(pcVar9 + -uVar5);
    puVar7 = (undefined4 *)&DAT_006766f8;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *puVar7 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar7 = puVar7 + 1;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined *)puVar7 = *(undefined *)puVar4;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    return &DAT_006766f8;
  }
  puVar4 = FUN_00482fb0((uint)&stack0x00000004);
  piVar2 = (int *)this->mbr_0x0;
  iVar3 = *piVar2;
  *puVar4 = 0x10;
  iVar3 = (**(code **)(iVar3 + 0x54))(piVar2);
  if (iVar3 == 0) {
    _strncpy(&DAT_006766f8,(char *)puVar4[2],0x3f);
    UNK_006766f9._62_1_ = 0;
    FUN_004830f0(puVar4);
    return &DAT_006766f8;
  }
  uVar5 = 0xffffffff;
  pcVar8 = s_Unknown_005e5c4c;
  do {
    pcVar9 = pcVar8;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar9 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar9;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  puVar7 = (undefined4 *)(pcVar9 + -uVar5);
  puVar10 = (undefined4 *)&DAT_006766f8;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *puVar10 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar10 = puVar10 + 1;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *(undefined *)puVar10 = *(undefined *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
    puVar10 = (undefined4 *)((int)puVar10 + 1);
  }
  FUN_004830f0(puVar4);
  return &DAT_006766f8;
}



// Function at 00578b80

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578b80(cls_0x5756d0 *this)

{
  char cVar1;
  dword dVar2;
  dword *pdVar3;
  int iVar4;
  undefined *puVar5;
  cls_0x5b4f30 *this_00;
  dword *pdVar6;
  int *piVar7;
  cls_0x570900 *pcStack_3c;
  int *piStack_38;
  cls_0x570900 *pcStack_34;
  undefined auStack_30 [36];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a26de;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  pdVar6 = (dword *)this->mbr_0x17c;
  if (pdVar6 != (dword *)this->mbr_0x180) {
    do {
      dVar2 = *pdVar6;
      pcStack_3c = (cls_0x570900 *)FUN_00570ad0(dVar2);
      if (pcStack_3c == (cls_0x570900 *)0x0) {
        pcStack_34 = (cls_0x570900 *)FUN_00482fb0(0x50);
        uStack_4 = 0;
        if (pcStack_34 == (cls_0x570900 *)0x0) {
          pcStack_3c = (cls_0x570900 *)0x0;
        }
        else {
          pcStack_3c = (cls_0x570900 *)
                       FUN_00570820((undefined4 *)pcStack_34,dVar2,this->mbr_0x14c == dVar2);
        }
        uStack_4 = 0xffffffff;
      }
      iVar4 = (**(code **)(*(int *)this->mbr_0x0 + 0x74))
                        ((int *)this->mbr_0x0,dVar2,&pcStack_3c,4,1);
      if (iVar4 == 0) {
        cls_0x588410::cls_0x588410((cls_0x588410 *)(auStack_30 + 8),auStack_30);
        uStack_4 = 1;
        puVar5 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)(auStack_30 + 8));
        *puVar5 = 1;
        GetTickCount();
        cls_0x588410::meth_0x57bbb0((cls_0x588410 *)(auStack_30 + 8));
        iVar4 = meth_0x5793f0(this,dVar2);
        if (iVar4 != 0) {
          WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
          iVar4 = meth_0x5789a0(this);
          if (iVar4 != 0) {
            *(int *)(iVar4 + 0x2c) = *(int *)(iVar4 + 0x2c) + 1;
          }
          ReleaseMutex((HANDLE)this->mbr_0x12c);
        }
        uStack_4 = 0xffffffff;
        cls_0x588410::meth_0x588480((cls_0x588410 *)(auStack_30 + 8));
      }
      else {
        if (this->mbr_0x190 == this->mbr_0x194) {
          cls_0x57be50::meth_0x57be50((cls_0x57be50 *)&this->mbr_0x18c);
        }
        pdVar3 = (dword *)this->mbr_0x190;
        *pdVar3 = dVar2;
        this->mbr_0x190 = (dword)(pdVar3 + 1);
      }
      pdVar6 = pdVar6 + 1;
    } while (pdVar6 != (dword *)this->mbr_0x180);
  }
  piVar7 = (int *)this->mbr_0x18c;
  this->mbr_0x180 = this->mbr_0x17c;
  piStack_38 = piVar7;
  if (piVar7 != (int *)this->mbr_0x190) {
    do {
      iVar4 = *piVar7;
      piStack_38 = piVar7;
      pcStack_3c = (cls_0x570900 *)FUN_00570ad0(iVar4);
      if (pcStack_3c != (cls_0x570900 *)0x0) {
        if (iVar4 == 1) {
          meth_0x578210(this);
          this->mbr_0x140 = 1;
          break;
        }
        if (((this->mbr_0xf0 != 0) && ((this->mbr_0xc4 & 0x80) != 0)) && (this->mbr_0xf4 != 0)) {
          *(undefined *)&pcStack_3c[1].mbr_0x0 = 1;
        }
        cVar1 = *(char *)&pcStack_3c[1].mbr_0x0;
        this_00 = (cls_0x5b4f30 *)cls_0x570900::meth_0x570c00(pcStack_3c);
        if (pcStack_3c != (cls_0x570900 *)0x0) {
          pcStack_34 = pcStack_3c;
          cls_0x570900::meth_0x570900(pcStack_3c);
          FUN_004830f0(pcStack_34);
        }
        if (this_00 != (cls_0x5b4f30 *)0x0) {
          if (cVar1 == '\0') {
            cls_0x5b4f30::meth_0x51d680(this_00);
          }
          else {
            (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_00472e90_64)
                      ((this_00->TPlayScreen).TScreen.mbr_0x8 | 0x1000);
          }
          this->mbr_0x1a0 = 1;
        }
        (**(code **)(*(int *)this->mbr_0x0 + 0x74))((int *)this->mbr_0x0,iVar4,0,0,1);
        (**(code **)(*(int *)this->mbr_0x0 + 0x24))((int *)this->mbr_0x0,iVar4);
        piVar7 = piStack_38;
      }
      piVar7 = piVar7 + 1;
      piStack_38 = piVar7;
    } while (piVar7 != (int *)this->mbr_0x190);
  }
  this->mbr_0x190 = this->mbr_0x18c;
  if (this->mbr_0xf0 != 0) {
    FUN_00570a80();
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  ExceptionList = pvStack_c;
  return;
}



// Function at 00578ec0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x578ec0(cls_0x5756d0 *this,undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int *in_stack_00000008;
  int in_stack_0000000c;
  
  iVar2 = *in_stack_00000008;
  iVar1 = in_stack_00000008[3];
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  iVar2 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,param_1,in_stack_0000000c != 0 | 0x600,
                     iVar2,iVar1 - iVar2,0,1000,0,0);
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  if ((iVar2 != 0) && (iVar2 != -0x7ffffff6)) {
    return 0;
  }
  return 1;
}



// Function at 005793f0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x5793f0(cls_0x5756d0 *this,undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *in_stack_00000008;
  int in_stack_0000000c;
  int iVar4;
  
  iVar2 = *in_stack_00000008;
  iVar3 = in_stack_00000008[3] - iVar2;
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  iVar4 = iVar3;
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,param_1,in_stack_0000000c != 0 | 0x600,
                     iVar2,iVar3,1,0,0,0);
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  if (iVar1 == -0x7fffbfff) {
    WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
    iVar2 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                      ((int *)this->mbr_0x0,this->mbr_0x14c,iVar4,0x600,iVar2,iVar3,0,1000,0,0);
    ReleaseMutex((HANDLE)this->mbr_0x12c);
    if ((iVar2 != 0) && (iVar2 != -0x7ffffff6)) {
      return 0;
    }
  }
  else if ((iVar1 != 0) && (iVar1 != -0x7ffffff6)) {
    return 0;
  }
  return 1;
}



// Function at 0057a370

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57a370(cls_0x5756d0 *this)

{
  undefined4 uVar1;
  int iVar2;
  byte *unaff_EBP;
  int *in_stack_00000004;
  byte *pbStack_44;
  dword dStack_34;
  dword dStack_28;
  void *pvStack_18;
  undefined4 uStack_10;
  void *local_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uStack_4._0_1_ = 0xff;
  uStack_4._1_1_ = 0xff;
  uStack_4._2_2_ = 0xffff;
  uStack_8 = &LAB_005a2762;
  local_c = ExceptionList;
  if (in_stack_00000004 == (int *)0x0) {
    return 1;
  }
  iVar2 = *in_stack_00000004;
  ExceptionList = &local_c;
  uVar1 = (**(code **)(iVar2 + 0x1d8))();
  (**(code **)(iVar2 + 0x1c4))(uVar1);
  iVar2 = *in_stack_00000004;
  uVar1 = (**(code **)(iVar2 + 0x1e0))();
  (**(code **)(iVar2 + 0x1cc))(uVar1);
  iVar2 = *in_stack_00000004;
  uVar1 = (**(code **)(iVar2 + 0x1e8))();
  (**(code **)(iVar2 + 0x1d4))(uVar1);
  cls_0x588410::cls_0x588410((cls_0x588410 *)&dStack_34);
  uStack_10 = 0;
  cls_0x588410::meth_0x588660((cls_0x588410 *)&dStack_34);
  FUN_00584f30(in_stack_00000004,(cls_0x588410 *)&dStack_34,4);
  cls_0x588410::cls_0x588410((cls_0x588410 *)&stack0xffffffb0);
  uStack_10 = CONCAT31(uStack_10._1_3_,1);
  iVar2 = cls_0x5b9f28::meth_0x579ce0
                    ((cls_0x5b9f28 *)&this->mbr_0x248,2,dStack_34,dStack_28 - dStack_34,
                     &stack0xffffffb0,20000);
  if (((iVar2 == 0) || (this->mbr_0xf0 == 0)) || (this->mbr_0xf4 != 0)) {
    if (unaff_EBP < pbStack_44 + 1) {
      pbStack_44 = (byte *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&stack0xffffffb0);
    }
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,*pbStack_44);
    this->mbr_0x13c = (uint)*pbStack_44;
    if (this->mbr_0xf4 != 0) {
      iVar2 = 0;
    }
  }
  uStack_10 = uStack_10 & 0xffffff00;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&stack0xffffffb0);
  uStack_10 = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&dStack_34);
  ExceptionList = pvStack_18;
  return iVar2;
}



// Function at 0057a4f0

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57a4f0(cls_0x5756d0 *this,undefined4 param_1)

{
  int iVar1;
  byte *pbVar2;
  byte *pbVar3;
  cls_0x588410 local_44;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a277c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x588410::cls_0x588410((cls_0x588410 *)&local_44.mbr_0x1c);
  local_4 = 0;
  cls_0x588410::meth_0x588660((cls_0x588410 *)&local_44.mbr_0x1c);
  cls_0x588410::meth_0x588660((cls_0x588410 *)&local_44.mbr_0x1c);
  cls_0x588410::cls_0x588410(&local_44);
  local_44.mbr_0x18 = 0;
  local_4._0_1_ = 1;
  iVar1 = cls_0x5b9f28::meth_0x579ce0
                    ((cls_0x5b9f28 *)&this->mbr_0x248,3,local_44.mbr_0x1c,
                     local_44.mbr_0x28 - local_44.mbr_0x1c,&local_44,20000);
  if (iVar1 != 0) {
    local_4 = (uint)local_4._1_3_ << 8;
    cls_0x588410::meth_0x588480(&local_44);
    local_4 = 0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
    ExceptionList = local_c;
    return iVar1;
  }
  pbVar3 = (byte *)(local_44.mbr_0xc + 1);
  pbVar2 = (byte *)local_44.mbr_0xc;
  local_44.mbr_0xc = (dword)pbVar3;
  if (local_44.mbr_0x4 < pbVar3) {
    pbVar2 = (byte *)cls_0x588410::meth_0x5884a0(&local_44);
  }
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0x13c = (uint)*pbVar2;
  cls_0x588410::meth_0x588480(&local_44);
  local_4 = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
  ExceptionList = local_c;
  return 0;
}



// Function at 0057a620

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57a620(cls_0x5756d0 *this)

{
  undefined uVar1;
  TPlayScreen *this_00;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  dword dVar5;
  int local_50;
  int local_4c;
  undefined local_48 [12];
  dword dStack_3c;
  dword local_38;
  undefined local_28 [24];
  void *local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2796;
  local_c = ExceptionList;
  if (-1 < (int)this->mbr_0x13c) {
    ExceptionList = &local_c;
    TCharacter::meth_0x44e460((TCharacter *)&DAT_006668d8);
    cls_0x588410::cls_0x588410((cls_0x588410 *)(local_48 + 4));
    local_4 = 0;
    cls_0x588410::cls_0x588410((cls_0x588410 *)local_28);
    local_10 = (void *)0x0;
    local_4._0_1_ = 1;
    local_50 = 1;
    if (DAT_0067682c != 0) {
      this_00 = (TPlayScreen *)
                cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,this->mbr_0x13c);
      local_4c = 0;
      local_48._0_4_ = 4;
      iVar2 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                        ((int *)this->mbr_0x0,1,&local_4c,local_48,1);
      iVar3 = local_4c;
      if (iVar2 != 0) {
        iVar3 = FUN_00570ad0(1);
      }
      if (iVar3 == 0) {
        FUN_00481c10((byte *)s_Network_login__No_server_player_f_005e5c5c);
      }
      *(dword *)(iVar3 + 0x34) = this->mbr_0x13c;
LAB_0057a7e9:
      if (this_00 != (TPlayScreen *)0x0) {
        dVar5 = this->mbr_0x14c;
        (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477650_384)(this_00);
        cls_0x45f7c0::meth_0x51f060((cls_0x45f7c0 *)&DAT_0065a890);
        TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,this_00);
        TPlayScreen::meth_0x4d4220(this_00);
        this->mbr_0x140 = 0;
        this->mbr_0x144 = 0;
        dStack_3c = local_48._0_4_;
        local_38 = 0;
        if ((this->mbr_0xf0 != 0) && (this->mbr_0xf4 == 0)) {
          local_50 = 0;
          local_4c = 4;
          iVar2 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                            ((int *)this->mbr_0x0,1,&local_50,&local_4c,1,dVar5);
          iVar3 = local_4c;
          if (iVar2 != 0) {
            iVar3 = FUN_00570ad0(1);
          }
          if (iVar3 == 0) {
            FUN_00481c10((byte *)s_Network_login__No_server_player_f_005e5c84);
          }
          *(undefined4 *)(iVar3 + 0x34) = 0;
        }
      }
      local_4 = (uint)local_4._1_3_ << 8;
      cls_0x588410::meth_0x588480((cls_0x588410 *)local_28);
      local_4 = 0xffffffff;
      cls_0x588410::meth_0x588480((cls_0x588410 *)(local_48 + 4));
      ExceptionList = local_c;
      return local_50;
    }
    uVar1 = *(undefined *)&this->mbr_0x13c;
    puVar4 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)(local_48 + 4));
    *puVar4 = uVar1;
    iVar3 = cls_0x5b9f28::meth_0x579ce0
                      ((cls_0x5b9f28 *)&this->mbr_0x248,4,local_48._4_4_,local_38 - local_48._4_4_,
                       local_28,20000);
    local_50 = iVar3;
    if (iVar3 != 0) {
      local_4 = (uint)local_4._1_3_ << 8;
      cls_0x588410::meth_0x588480((cls_0x588410 *)local_28);
      local_4 = 0xffffffff;
      cls_0x588410::meth_0x588480((cls_0x588410 *)(local_48 + 4));
      ExceptionList = local_c;
      return iVar3;
    }
    this_00 = (TPlayScreen *)FUN_00584eb0((cls_0x588410 *)local_28);
    if (this_00 != (TPlayScreen *)0x0) {
      if (*(word *)&(this_00->TScreen).mbr_0x4 == 0xb) {
        cls_0x45f7c0::meth_0x51eda0((cls_0x45f7c0 *)&DAT_0065a890);
        cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,this_00);
        goto LAB_0057a7e9;
      }
      (*((SoftwareRenderer__vftable_5b8e94 *)(this_00->TScreen).vftptr_0x0)->virt_meth_0x528b00_0)
                ((SoftwareRenderer *)this_00);
    }
    puStack_8 = (undefined *)((uint)puStack_8 & 0xffffff00);
    cls_0x588410::meth_0x588480((cls_0x588410 *)&stack0xffffffd4);
    puStack_8 = (undefined *)0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)local_48);
  }
  ExceptionList = local_10;
  return 1;
}



// Function at 0057a8f0

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57a8f0(cls_0x5756d0 *this)

{
  int iVar1;
  int in_stack_00000004;
  
  iVar1 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))((int *)this->mbr_0x0);
  if (iVar1 != 0) {
    in_stack_00000004 = FUN_00570ad0(in_stack_00000004);
  }
  if (in_stack_00000004 == 0) {
    return 0;
  }
  return *(undefined4 *)(in_stack_00000004 + 0x14);
}



// Function at 0057ad00

/* WARNING: Type propagation algorithm not settling */

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57ad00(cls_0x5756d0 *this)

{
  char cVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  dword dVar3;
  undefined uVar4;
  cls_0x5b4f30 *this_00;
  undefined3 extraout_var;
  int iVar5;
  undefined3 extraout_var_00;
  uint uVar6;
  uint uVar7;
  int *piVar8;
  undefined4 *puVar9;
  dword *pdVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  dword *pdVar14;
  int iVar15;
  int iStack_314;
  undefined uStack_309;
  int iStack_308;
  undefined auStack_304 [120];
  undefined auStack_28c [128];
  char cStack_20c;
  char acStack_20b [511];
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a27df;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5b4f30 *)
            cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,this->mbr_0x13c);
  if (this_00 == (cls_0x5b4f30 *)0x0) {
    ExceptionList = local_c;
    return 1;
  }
  uVar4 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477660_388)(this_00);
  iVar15 = 1;
  iStack_314 = 0;
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))((int *)this->mbr_0x0);
  if (iVar5 != 0) {
    iVar15 = FUN_00570ad0(CONCAT31(extraout_var,uVar4));
  }
  if (iVar15 != 0) {
    *(undefined *)(iVar15 + 0x33) = 1;
  }
  iVar15 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                     ((int *)this->mbr_0x0,1,&stack0xfffffcd8,&stack0xfffffcdc,1);
  iVar5 = iStack_314;
  if (iVar15 != 0) {
    iVar5 = FUN_00570ad0(1);
  }
  if (iVar5 != 0) {
    *(undefined *)(iVar5 + 0x33) = 1;
  }
  if ((int)this->mbr_0x13c < 0) {
    ExceptionList = local_c;
    return 1;
  }
  cls_0x588410::cls_0x588410((cls_0x588410 *)auStack_304);
  auStack_304._24_4_ = 0;
  uStack_309 = *(undefined *)&this->mbr_0x13c;
  uStack_4 = 0;
  iStack_308 = cls_0x5b9f28::meth_0x579ce0
                         ((cls_0x5b9f28 *)&this->mbr_0x248,5,&uStack_309,1,auStack_304,20000);
  if (iStack_308 != 0) goto LAB_0057b017;
  if (DAT_006682bc != 0) {
    uVar6 = 0xffffffff;
    pcVar11 = &DAT_00667340;
    do {
      pcVar12 = pcVar11;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar12 = pcVar11 + 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar12;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    puVar9 = (undefined4 *)(pcVar12 + -uVar6);
    puVar13 = (undefined4 *)&cStack_20c;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar13 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar13 = puVar13 + 1;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined *)puVar13 = *(undefined *)puVar9;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
      puVar13 = (undefined4 *)((int)puVar13 + 1);
    }
    puVar9 = (undefined4 *)&cStack_20c;
    while (cStack_20c != '\0') {
      cVar1 = *(char *)puVar9;
      while ((cVar1 != '\0' && (cVar1 != '\n'))) {
        pcVar11 = (char *)((int)puVar9 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
        cVar1 = *pcVar11;
      }
      if (*(char *)puVar9 == '\n') {
        *(char *)puVar9 = '\0';
      }
      puVar9 = (undefined4 *)((int)puVar9 + 1);
      cls_0x5701f0::meth_0x5701f0((cls_0x5701f0 *)&UNK_00676268.field_0x400,this_00->mbr_0x664);
      cStack_20c = *(char *)puVar9;
    }
  }
  cls_0x588410::meth_0x589810((cls_0x588410 *)auStack_304);
  cls_0x588410::meth_0x5725f0((cls_0x588410 *)auStack_304);
  pcVar2 = (this_00->TPlayScreen).TScreen.vftptr_0x0;
  (this_00->TPlayScreen).mbr_0x660 = (dword)s_MPAppear_005e5cac;
  (*pcVar2->virt_meth_0x46ed70_8)(this_00);
  cls_0x5b4f30::meth_0x51d680(this_00);
  uVar4 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477660_388)(this_00);
  iVar15 = 1;
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                    ((int *)this->mbr_0x0,CONCAT31(extraout_var_00,uVar4),&stack0xfffffce0,
                     &stack0xfffffce4);
  if (iVar5 != 0) {
    iVar15 = FUN_00570ad0(CONCAT31(extraout_var_00,uVar4));
  }
  if (iVar15 != 0) {
    *(undefined *)(iVar15 + 0x32) = 1;
  }
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x50))
                    ((int *)this->mbr_0x0,1,&stack0xfffffccc,&stack0xfffffcd0,1);
  if (iVar5 != 0) {
    iStack_314 = FUN_00570ad0(1);
  }
  if (iStack_314 != 0) {
    *(undefined *)(iStack_314 + 0x32) = 1;
  }
  dVar3 = this->mbr_0xf4;
  DAT_006669b0 = DAT_006669b0 | 8;
  this->mbr_0x19c = 1;
  if (((dVar3 != 0) && (pdVar10 = &this->mbr_0x2c, pdVar10 != (dword *)0x0)) &&
     ((this->mbr_0xf0 == 0 || (dVar3 != 0)))) {
    if (dVar3 != 0) {
      meth_0x5773f0(this,pdVar10,auStack_304 + 0x28,auStack_28c);
      iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0x7c))();
      if (iVar5 != 0) goto LAB_0057afea;
    }
    pdVar14 = pdVar10;
    for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pdVar14 = *pdVar10;
      pdVar10 = pdVar10 + 1;
      pdVar14 = pdVar14 + 1;
    }
  }
LAB_0057afea:
  piVar8 = DAT_00668588;
  iVar5 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar15 = *piVar8;
      if (iVar15 != 0) {
        *(undefined4 *)(iVar15 + 0x98) = 0;
        *(undefined4 *)(iVar15 + 0xa0) = 0;
      }
      iVar5 = iVar5 + -1;
      piVar8 = piVar8 + 1;
    } while (iVar5 != 0);
  }
LAB_0057b017:
  iVar5 = iStack_308;
  uStack_4 = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)auStack_304);
  ExceptionList = local_c;
  return iVar5;
}



// Function at 0057b050

undefined4 __thiscall
OOAnalyzer::cls_0x5756d0::meth_0x57b050(cls_0x5756d0 *this,undefined4 *param_1)

{
  undefined uVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  undefined4 uVar4;
  undefined *puVar5;
  undefined4 in_stack_00000008;
  undefined local_44 [52];
  dword local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puVar2 = param_1;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a27f9;
  local_c = ExceptionList;
  if ((-1 < (int)this->mbr_0x13c) && (param_1 != (undefined4 *)0x0)) {
    ExceptionList = &local_c;
    cls_0x588410::cls_0x588410((cls_0x588410 *)local_44);
    local_4 = 0;
    cls_0x588410::cls_0x588410((cls_0x588410 *)(local_44 + 0x1c));
    local_10 = 0;
    local_4 = CONCAT31(local_4._1_3_,1);
    uVar4 = *(undefined4 *)(DAT_00667fcc + 0x664);
    puVar5 = (undefined *)(local_44._12_4_ + 1);
    puVar3 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar5;
    if ((uint)local_44._4_4_ < puVar5) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    *puVar3 = (char)uVar4;
    uVar4 = *param_1;
    puVar5 = (undefined *)(local_44._12_4_ + 4);
    puVar3 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar5;
    if ((uint)local_44._4_4_ < puVar5) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
    *puVar3 = (char)uVar4;
    param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
    puVar3[1] = (char)((uint)uVar4 >> 8);
    puVar3[2] = param_1._2_1_;
    puVar3[3] = param_1._3_1_;
    cls_0x588410::meth_0x588660((cls_0x588410 *)local_44);
    cls_0x588410::meth_0x588660((cls_0x588410 *)local_44);
    uVar1 = *(undefined *)(puVar2 + 0x12);
    puVar5 = (undefined *)(local_44._12_4_ + 1);
    puVar3 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar5;
    if ((uint)local_44._4_4_ < puVar5) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    *puVar3 = uVar1;
    uVar1 = *(undefined *)(puVar2 + 0x13);
    puVar5 = (undefined *)(local_44._12_4_ + 1);
    puVar3 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar5;
    if ((uint)local_44._4_4_ < puVar5) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    *puVar3 = uVar1;
    uVar4 = cls_0x5b9f28::meth_0x579ce0
                      ((cls_0x5b9f28 *)&this->mbr_0x248,in_stack_00000008,local_44._0_4_,
                       local_44._12_4_ - local_44._0_4_,local_44 + 0x1c,20000);
    local_4 = local_4 & 0xffffff00;
    cls_0x588410::meth_0x588480((cls_0x588410 *)(local_44 + 0x1c));
    local_4 = 0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)local_44);
    ExceptionList = local_c;
    return uVar4;
  }
  this->mbr_0x1a0 = 1;
  return 1;
}



// Function at 0057b1f0

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57b1f0(cls_0x5756d0 *this)

{
  this->mbr_0x1a0 = 1;
  return;
}



// Function at 0057b220

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57b220(cls_0x5756d0 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int *piVar3;
  int iVar4;
  undefined4 *puVar5;
  cls_0x570900 *this_00;
  dword dVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  int in_stack_00000004;
  
  if ((this->mbr_0xf4 == 0) || (in_stack_00000004 == DAT_00667fcc)) {
    return 1;
  }
  this_00 = DAT_006766d4;
  if (DAT_006766d4 != (cls_0x570900 *)0x0) {
    do {
      iVar4 = cls_0x570900::meth_0x570c00(this_00);
      if (iVar4 == in_stack_00000004) {
        if (this_00 != (cls_0x570900 *)0x0) {
          iVar4 = *(int *)&this_00->field_0x1c;
          if (iVar4 != 1) {
            dVar6 = this->mbr_0x190;
            dVar1 = this->mbr_0x194;
            *(undefined *)&this_00[1].mbr_0x0 = 1;
            if (dVar6 == dVar1) {
              dVar6 = this->mbr_0x198 + 8;
              puVar5 = FUN_00482fb0(dVar6 * 4);
              if ((int)this->mbr_0x198 <= (int)dVar6) {
                dVar6 = this->mbr_0x198;
              }
              puVar2 = (undefined4 *)this->mbr_0x18c;
              puVar7 = puVar2;
              puVar8 = puVar5;
              for (; dVar6 != 0; dVar6 = dVar6 - 1) {
                *puVar8 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar8 = puVar8 + 1;
              }
              FUN_004830f0(puVar2);
              dVar6 = this->mbr_0x18c;
              this->mbr_0x18c = (dword)puVar5;
              this->mbr_0x190 = (dword)(puVar5 + ((int)(this->mbr_0x190 - dVar6) >> 2));
              dVar6 = this->mbr_0x198 + 8;
              this->mbr_0x198 = dVar6;
              this->mbr_0x194 = (dword)(puVar5 + dVar6);
            }
            piVar3 = (int *)this->mbr_0x190;
            *piVar3 = iVar4;
            this->mbr_0x190 = (dword)(piVar3 + 1);
            return 0;
          }
        }
        break;
      }
      this_00 = (cls_0x570900 *)this_00->mbr_0xc;
    } while (this_00 != (cls_0x570900 *)0x0);
  }
  cls_0x45f7c0::meth_0x51f2a0((cls_0x45f7c0 *)&DAT_0065a890,in_stack_00000004);
  this->mbr_0x1a0 = 1;
  return 0;
}



// Function at 0057b350

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57b350(cls_0x5756d0 *this,int param_1)

{
  undefined *puVar1;
  undefined4 uVar2;
  undefined *puVar3;
  undefined4 in_stack_00000008;
  undefined local_44 [52];
  dword local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2813;
  local_c = ExceptionList;
  if (-1 < (int)this->mbr_0x13c) {
    ExceptionList = &local_c;
    cls_0x588410::cls_0x588410((cls_0x588410 *)local_44);
    local_4 = 0;
    cls_0x588410::cls_0x588410((cls_0x588410 *)(local_44 + 0x1c));
    local_10 = 0;
    local_4 = CONCAT31(local_4._1_3_,1);
    uVar2 = *(undefined4 *)(param_1 + 0x664);
    puVar3 = (undefined *)(local_44._12_4_ + 1);
    puVar1 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar3;
    if ((uint)local_44._4_4_ < puVar3) {
      puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    *puVar1 = (char)uVar2;
    puVar3 = (undefined *)(local_44._12_4_ + 4);
    puVar1 = (undefined *)local_44._12_4_;
    local_44._12_4_ = puVar3;
    if ((uint)local_44._4_4_ < puVar3) {
      puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_44);
    }
    *puVar1 = (char)in_stack_00000008;
    puVar1[1] = (char)((uint)in_stack_00000008 >> 8);
    puVar1[2] = in_stack_00000008._2_1_;
    puVar1[3] = in_stack_00000008._3_1_;
    uVar2 = cls_0x5b9f28::meth_0x579ce0
                      ((cls_0x5b9f28 *)&this->mbr_0x248,0xe,local_44._0_4_,
                       local_44._12_4_ - local_44._0_4_,local_44 + 0x1c,10000);
    local_4 = local_4 & 0xffffff00;
    cls_0x588410::meth_0x588480((cls_0x588410 *)(local_44 + 0x1c));
    local_4 = 0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)local_44);
    ExceptionList = local_c;
    return uVar2;
  }
  return 1;
}



// Function at 0057b480

void __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57b480(cls_0x5756d0 *this)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  dword dVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 in_stack_00000004;
  
  if (this->mbr_0x190 == this->mbr_0x194) {
    dVar3 = this->mbr_0x198 + 8;
    puVar2 = FUN_00482fb0(dVar3 * 4);
    if ((int)this->mbr_0x198 <= (int)dVar3) {
      dVar3 = this->mbr_0x198;
    }
    puVar1 = (undefined4 *)this->mbr_0x18c;
    puVar4 = puVar1;
    puVar5 = puVar2;
    for (; dVar3 != 0; dVar3 = dVar3 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    FUN_004830f0(puVar1);
    dVar3 = this->mbr_0x18c;
    this->mbr_0x18c = (dword)puVar2;
    this->mbr_0x190 = (dword)(puVar2 + ((int)(this->mbr_0x190 - dVar3) >> 2));
    dVar3 = this->mbr_0x198 + 8;
    this->mbr_0x198 = dVar3;
    this->mbr_0x194 = (dword)(puVar2 + dVar3);
  }
  puVar2 = (undefined4 *)this->mbr_0x190;
  *puVar2 = in_stack_00000004;
  this->mbr_0x190 = (dword)(puVar2 + 1);
  return;
}



// Function at 0057b530

int __thiscall OOAnalyzer::cls_0x5756d0::meth_0x57b530(cls_0x5756d0 *this)

{
  int iVar1;
  byte *pbVar2;
  SoftwareRenderer *this_00;
  byte *pbVar3;
  int in_stack_00000004;
  cls_0x588410 local_44;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a282d;
  local_c = ExceptionList;
  if ((DAT_0067682c == 0) && (in_stack_00000004 != 0)) {
    ExceptionList = &local_c;
    cls_0x588410::cls_0x588410((cls_0x588410 *)&local_44.mbr_0x1c);
    local_4 = 0;
    cls_0x588410::cls_0x588410(&local_44);
    local_44.mbr_0x18 = 0;
    local_4._0_1_ = 1;
    cls_0x588410::meth_0x57bbb0((cls_0x588410 *)&local_44.mbr_0x1c);
    cls_0x57bc10::meth_0x57bc10((cls_0x57bc10 *)&UNK_0065a630.field_0x138);
    cls_0x588410::meth_0x57bbb0((cls_0x588410 *)&local_44.mbr_0x1c);
    cls_0x57bc10::meth_0x57bc10((cls_0x57bc10 *)&UNK_0065a630.field_0x138);
    cls_0x57bc10::meth_0x57bc10((cls_0x57bc10 *)&UNK_0065a630.field_0x138);
    cls_0x588410::meth_0x57bc30((cls_0x588410 *)&local_44.mbr_0x1c);
    cls_0x588410::meth_0x57bc30((cls_0x588410 *)&local_44.mbr_0x1c);
    cls_0x588410::meth_0x588660((cls_0x588410 *)&local_44.mbr_0x1c);
    iVar1 = cls_0x5b9f28::meth_0x579ce0
                      ((cls_0x5b9f28 *)&this->mbr_0x248,0xf,local_44.mbr_0x1c,
                       local_44.mbr_0x28 - local_44.mbr_0x1c,&local_44,10000);
    if (iVar1 != 0) {
      local_4 = (uint)local_4._1_3_ << 8;
      cls_0x588410::meth_0x588480(&local_44);
      local_4 = 0xffffffff;
      cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
      ExceptionList = local_c;
      return iVar1;
    }
    pbVar3 = (byte *)(local_44.mbr_0xc + 1);
    pbVar2 = (byte *)local_44.mbr_0xc;
    local_44.mbr_0xc = (dword)pbVar3;
    if (local_44.mbr_0x4 < pbVar3) {
      pbVar2 = (byte *)cls_0x588410::meth_0x5884a0(&local_44);
    }
    this->mbr_0x13c = (uint)*pbVar2;
    this_00 = FUN_00584eb0(&local_44);
    if (this_00 != (SoftwareRenderer *)0x0) {
      if (this_00->mbr_0x4 == 0xb) {
        cls_0x45f7c0::meth_0x51eda0((cls_0x45f7c0 *)&DAT_0065a890);
        cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,this_00);
        local_4 = (uint)local_4._1_3_ << 8;
        cls_0x588410::meth_0x588480(&local_44);
        local_4 = 0xffffffff;
        cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
        ExceptionList = local_c;
        return 0;
      }
      (*this_00->vftptr_0x0->virt_meth_0x528b00_0)(this_00);
    }
    local_4 = (uint)local_4._1_3_ << 8;
    cls_0x588410::meth_0x588480(&local_44);
    local_4 = 0xffffffff;
    cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
  }
  ExceptionList = local_c;
  return 1;
}



// Function at 00588300

dword __thiscall OOAnalyzer::cls_0x5756d0::meth_0x588300(cls_0x5756d0 *this)

{
  return this->mbr_0xf4;
}



// Function at 00588310

undefined4 __thiscall OOAnalyzer::cls_0x5756d0::meth_0x588310(cls_0x5756d0 *this)

{
  if ((this->mbr_0xf0 != 0) && (this->mbr_0xf4 == 0)) {
    return 1;
  }
  return 0;
}



