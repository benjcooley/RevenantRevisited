// Decompiled methods and structure for class: cls_0x5a4d6c

/*
/OOAnalyzer/cls_0x5a4d6c
pack(disabled)
Structure cls_0x5a4d6c {
   0   cls_0x5a4d6c::vftable_5a4d6c *   4   vftptr_0x0   "pointer to cls_0x5a4d6c::vftable_5a4d6c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   452   dword   4   mbr_0x1c4   "Unsigned Double-Word (ddw, 4-bytes)"
   456   dword   4   mbr_0x1c8   "Unsigned Double-Word (ddw, 4-bytes)"
   460   dword   4   mbr_0x1cc   "Unsigned Double-Word (ddw, 4-bytes)"
   464   dword   4   mbr_0x1d0   "Unsigned Double-Word (ddw, 4-bytes)"
   468   dword   4   mbr_0x1d4   "Unsigned Double-Word (ddw, 4-bytes)"
   472   dword   4   mbr_0x1d8   "Unsigned Double-Word (ddw, 4-bytes)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   480   dword   4   mbr_0x1e0   "Unsigned Double-Word (ddw, 4-bytes)"
   484   dword   4   mbr_0x1e4   "Unsigned Double-Word (ddw, 4-bytes)"
   488   dword   4   mbr_0x1e8   "Unsigned Double-Word (ddw, 4-bytes)"
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
   496   dword   4   mbr_0x1f0   "Unsigned Double-Word (ddw, 4-bytes)"
   500   dword   4   mbr_0x1f4   "Unsigned Double-Word (ddw, 4-bytes)"
   504   dword   4   mbr_0x1f8   "Unsigned Double-Word (ddw, 4-bytes)"
   508   byte   1   mbr_0x1fc   "Unsigned Byte (db)"
   636   dword   4   mbr_0x27c   "Unsigned Double-Word (ddw, 4-bytes)"
   648   dword   4   mbr_0x288   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 652 Alignment: 1

*/

// Function at 00465a50

undefined4 __thiscall OOAnalyzer::cls_0x5a4d6c::virt_meth_0x465a50(cls_0x5a4d6c *this)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  undefined4 *puVar5;
  dword dVar6;
  undefined4 uVar7;
  cls_0x5b4f30 *this_00;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  bool bVar14;
  
  piVar3 = FUN_004a1ec0(100,100,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  uVar8 = DAT_0065a258;
  this->mbr_0x27c = (dword)piVar3;
  iVar9 = 0;
  uVar4 = 0;
  if (0 < (int)uVar8) {
    piVar3 = &DAT_0065a148;
    bVar14 = uVar8 != 0;
    do {
      if (((bVar14) && (*piVar3 != 0)) && (iVar2 = *(int *)(*piVar3 + 0x24), iVar9 < iVar2)) {
        iVar9 = iVar2;
      }
      uVar4 = uVar4 + 1;
      piVar3 = piVar3 + 1;
      bVar14 = uVar4 < uVar8;
    } while ((int)uVar4 < (int)uVar8);
  }
  puVar5 = FUN_00482fb0(iVar9 * 4);
  this->mbr_0x288 = (dword)puVar5;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1b8 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1bc = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1c0 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1d4 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1d8 = dVar6;
  dVar6 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  this->mbr_0x1dc = dVar6;
  if ((this->mbr_0x190 == 1) || (this->mbr_0x190 == 2)) {
    uVar7 = 0x11;
  }
  else {
    uVar7 = 0;
  }
  if (DAT_0065c890 == 0) {
    iVar9 = cls_0x5b93c4::virt_meth_0x435150
                      ((cls_0x5b93c4 *)this,s_createchar_005d3300,0x659c80,uVar7,0,0,0x280,0x1e0,
                       0x1c2,0xa0,s_widgets_005d32f8);
  }
  else {
    iVar9 = cls_0x5b93c4::virt_meth_0x435230
                      ((cls_0x5b93c4 *)this,s_createchar_005d32e0,0x659c7c,uVar7,0,0,0x280,0x1e0,
                       0x1c2,0xa0,DAT_0066733c);
  }
  if (iVar9 == 0) {
    return 0;
  }
  this->mbr_0x1f4 = 1;
  this->mbr_0x1f0 = 1;
  if (this->mbr_0x194 == 2) {
    this_00 = (cls_0x5b4f30 *)FUN_00474e20((byte *)s_Locke_005d330c,(undefined4 *)0x0,0xffffffff);
    this->mbr_0x1f8 = (dword)this_00;
    cls_0x5b4f30::meth_0x46e6f0(this_00);
  }
  (*this->vftptr_0x0->FUN_00435de0_80)();
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  if (DAT_00667fd0 != &DAT_0065caf0) {
    this->mbr_0x19c = 0;
    this->mbr_0x198 = 0;
    return 1;
  }
  uVar8 = 0xffffffff;
  pcVar11 = *(char **)(this->mbr_0x1f8 + 0x38);
  pcVar12 = pcVar11;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar12;
    pcVar12 = pcVar12 + 1;
  } while (cVar1 != '\0');
  puVar5 = FUN_00482ef0(~uVar8);
  uVar8 = 0xffffffff;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  puVar10 = (undefined4 *)(pcVar12 + -uVar8);
  puVar13 = puVar5;
  for (uVar4 = uVar8 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  uVar8 = 0xffffffff;
  this->mbr_0x198 = (dword)puVar5;
  pcVar11 = (char *)(this->mbr_0x1f8 + 0x378);
  pcVar12 = pcVar11;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar12;
    pcVar12 = pcVar12 + 1;
  } while (cVar1 != '\0');
  puVar5 = FUN_00482ef0(~uVar8);
  uVar8 = 0xffffffff;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  puVar10 = (undefined4 *)(pcVar12 + -uVar8);
  puVar13 = puVar5;
  for (uVar4 = uVar8 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  this->mbr_0x19c = (dword)puVar5;
  return 1;
}



// Function at 00465ce0

void __thiscall OOAnalyzer::cls_0x5a4d6c::virt_meth_0x465ce0(cls_0x5a4d6c *this)

{
  FUN_004830f0((LPCVOID)this->mbr_0x27c);
  FUN_004830f0((LPCVOID)this->mbr_0x288);
  if ((LPCVOID)this->mbr_0x198 == (LPCVOID)0x0) {
    if (this->mbr_0x19c == 0) goto LAB_00465d46;
  }
  else {
    FUN_00482f80((LPCVOID)this->mbr_0x198);
    this->mbr_0x198 = 0;
  }
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x19c);
    this->mbr_0x19c = 0;
  }
LAB_00465d46:
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 00469340

void __thiscall OOAnalyzer::cls_0x5a4d6c::virt_meth_0x469340(cls_0x5a4d6c *this)

{
  cls_0x5b93c4::virt_meth_0x435d70((cls_0x5b93c4 *)this);
  if (DAT_00676e84 != '\0') {
    cls_0x5b98b8::meth_0x437ca0((cls_0x5b98b8 *)this);
    cls_0x5b98b8::meth_0x465d50((cls_0x5b98b8 *)this);
    cls_0x5b98b8::meth_0x465ea0((cls_0x5b98b8 *)this);
  }
  return;
}



// Function at 0046d160

cls_0x5a4d6c * __thiscall OOAnalyzer::cls_0x5a4d6c::cls_0x5a4d6c(cls_0x5a4d6c *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5a4d6c__vftable_5a4d6c *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4d6c__vftable_5a4d6c_005a4d6c;
  this->mbr_0x190 = 0xffffffff;
  this->mbr_0x194 = 0xffffffff;
  this->mbr_0x1f8 = 0;
  this->mbr_0x1fc = 0;
  return this;
}



// Function at 0046d1d0

cls_0x5a4d6c * __thiscall OOAnalyzer::cls_0x5a4d6c::virt_meth_0x46d1d0(cls_0x5a4d6c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4d6c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d1f0

cls_0x5a4d6c * __thiscall OOAnalyzer::cls_0x5a4d6c::~cls_0x5a4d6c(cls_0x5a4d6c *this)

{
  cls_0x5a4d6c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d19e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4d6c__vftable_5a4d6c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4d6c *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



