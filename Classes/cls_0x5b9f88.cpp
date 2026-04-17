// Decompiled methods and structure for class: cls_0x5b9f88

/*
/OOAnalyzer/cls_0x5b9f88
pack(disabled)
Structure cls_0x5b9f88 {
   0   cls_0x5b9f88::vftable_5b9f88 *   4   vftptr_0x0   "pointer to cls_0x5b9f88::vftable_5b9f88"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 00574020

cls_0x5b9f88 * __thiscall
OOAnalyzer::cls_0x5b9f88::cls_0x5b9f88
          (cls_0x5b9f88 *this,dword *param_1,undefined4 *param_2,undefined4 *param_3)

{
  dword *this_00;
  dword dVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  dword in_stack_00000010;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a241e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(undefined *)&this->mbr_0x4 = param_2._0_1_;
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this_00 = &this->mbr_0x14;
  *(undefined *)this_00 = param_2._0_1_;
  this->mbr_0x18 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x20 = 0;
  local_4 = 2;
  this->mbr_0x28 = *param_1;
  this->vftptr_0x0 = &cls_0x5b9f88__vftable_5b9f88_005b9f88;
  this->mbr_0x2c = param_1[1];
  this->mbr_0x30 = param_1[2];
  this->mbr_0x34 = param_1[3];
  uVar3 = 0xffffffff;
  puVar5 = param_2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar2 = *(char *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  } while (cVar2 != '\0');
  uVar3 = ~uVar3 - 1;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->mbr_0x4,(char)uVar3);
  if (cVar2 != '\0') {
    puVar5 = (undefined4 *)this->mbr_0x8;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar5 = *param_2;
      param_2 = param_2 + 1;
      puVar5 = puVar5 + 1;
    }
    this->mbr_0xc = uVar3;
    for (uVar4 = uVar3 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar5 = *(undefined *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
    *(undefined *)(uVar3 + this->mbr_0x8) = 0;
  }
  uVar3 = 0xffffffff;
  puVar5 = param_3;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar2 = *(char *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  } while (cVar2 != '\0');
  uVar3 = ~uVar3 - 1;
  if (0xfffffffd < uVar3) {
    FUN_0059b6f0();
  }
  dVar1 = this->mbr_0x18;
  if (((dVar1 == 0) || (cVar2 = *(char *)(dVar1 - 1), cVar2 == '\0')) || (cVar2 == -1)) {
    if (uVar3 == 0) {
      cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)this_00);
      goto LAB_0057416c;
    }
    if ((0x1f < this->mbr_0x20) || (this->mbr_0x20 < uVar3)) {
      cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)this_00);
      goto LAB_00574140;
    }
  }
  else {
    if (uVar3 == 0) {
      *(char *)(dVar1 - 1) = cVar2 + -1;
      cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)this_00);
      goto LAB_0057416c;
    }
LAB_00574140:
    cls_0x57bd90::meth_0x57c0a0((cls_0x57bd90 *)this_00);
  }
  puVar5 = (undefined4 *)this->mbr_0x18;
  this->mbr_0x1c = uVar3;
  puVar6 = puVar5;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar6 = *param_3;
    param_3 = param_3 + 1;
    puVar6 = puVar6 + 1;
  }
  for (uVar4 = uVar3 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar6 = *(undefined *)param_3;
    param_3 = (undefined4 *)((int)param_3 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  }
  *(undefined *)((int)puVar5 + uVar3) = 0;
LAB_0057416c:
  this->mbr_0x24 = in_stack_00000010;
  this->mbr_0x38 = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 005741a0

cls_0x5b9f88 * __thiscall OOAnalyzer::cls_0x5b9f88::~cls_0x5b9f88(cls_0x5b9f88 *this)

{
  char cVar1;
  dword dVar2;
  cls_0x5b9f88 *pcVar3;
  
  this->vftptr_0x0 = &cls_0x5b9f88__vftable_5b9f88_005b9f88;
  if ((HGLOBAL)this->mbr_0x38 != (HGLOBAL)0x0) {
    GlobalFree((HGLOBAL)this->mbr_0x38);
    this->mbr_0x38 = 0;
  }
  dVar2 = this->mbr_0x18;
  if (dVar2 != 0) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->mbr_0x18 = 0;
  this->mbr_0x1c = 0;
  pcVar3 = (cls_0x5b9f88 *)this->mbr_0x8;
  this->mbr_0x20 = 0;
  if (pcVar3 != (cls_0x5b9f88 *)0x0) {
    cVar1 = *(char *)((int)&pcVar3[-1].mbr_0x38 + 3);
    pcVar3 = (cls_0x5b9f88 *)((int)&pcVar3[-1].mbr_0x38 + 3);
    if ((cVar1 != '\0') && (cVar1 != -1)) {
      this->mbr_0x8 = 0;
      this->mbr_0xc = 0;
      this->mbr_0x10 = 0;
      this->vftptr_0x0 = (cls_0x5b9f88__vftable_5b9f88 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
      *(char *)&pcVar3->vftptr_0x0 = cVar1 + -1;
      return pcVar3;
    }
    pcVar3 = (cls_0x5b9f88 *)FUN_004830f0(pcVar3);
  }
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this->vftptr_0x0 = (cls_0x5b9f88__vftable_5b9f88 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  return pcVar3;
}



// Function at 0057b920

cls_0x5b9f88 * __thiscall OOAnalyzer::cls_0x5b9f88::virt_meth_0x57b920(cls_0x5b9f88 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9f88(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



