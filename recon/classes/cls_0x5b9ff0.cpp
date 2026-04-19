// REVSYNC: disputed=TObjectInstance reason="yaml 110/100 PERFECT but class holds 2 strings + Win32 HGLOBAL (GlobalFree in dtor); not a game-object state class" rejected=HIGH date=2026-04-17
// Decompiled methods and structure for class: cls_0x5b9ff0

/*
/OOAnalyzer/cls_0x5b9ff0
pack(disabled)
Structure cls_0x5b9ff0 {
   0   cls_0x5b9ff0::vftable_5b9ff0 *   4   vftptr_0x0   "pointer to cls_0x5b9ff0::vftable_5b9ff0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   byte   1   mbr_0x14   "Unsigned Byte (db)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 80 Alignment: 1

*/

// Function at 00574890

cls_0x5b9ff0 * __thiscall
OOAnalyzer::cls_0x5b9ff0::cls_0x5b9ff0(cls_0x5b9ff0 *this,dword *param_1,undefined4 *param_2)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a2481;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(byte *)&this->mbr_0x4 = (byte)param_2;
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x14 = (byte)param_2;
  this->mbr_0x1c = 0;
  this->mbr_0x20 = 0;
  local_4 = 2;
  this->mbr_0x28 = *param_1;
  this->vftptr_0x0 = (cls_0x5b9ff0__vftable_5b9ff0 *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
  this->mbr_0x2c = param_1[1];
  this->mbr_0x30 = param_1[2];
  uVar2 = 0xffffffff;
  this->mbr_0x34 = param_1[3];
  puVar4 = param_2;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *(char *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  } while (cVar1 != '\0');
  uVar2 = ~uVar2 - 1;
  cVar1 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->mbr_0x4,(char)uVar2);
  if (cVar1 != '\0') {
    puVar4 = (undefined4 *)this->mbr_0x8;
    for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar4 = *param_2;
      param_2 = param_2 + 1;
      puVar4 = puVar4 + 1;
    }
    this->mbr_0xc = uVar2;
    for (uVar3 = uVar2 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined *)puVar4 = *(undefined *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
    *(undefined *)(uVar2 + this->mbr_0x8) = 0;
  }
  uVar2 = 0xffffffff;
  puVar4 = in_stack_0000000c;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *(char *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  } while (cVar1 != '\0');
  uVar2 = ~uVar2 - 1;
  cVar1 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->mbr_0x14,(char)uVar2);
  if (cVar1 != '\0') {
    puVar4 = (undefined4 *)this->mbr_0x18;
    this->mbr_0x1c = uVar2;
    puVar5 = puVar4;
    for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = *in_stack_0000000c;
      in_stack_0000000c = in_stack_0000000c + 1;
      puVar5 = puVar5 + 1;
    }
    for (uVar3 = uVar2 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined *)puVar5 = *(undefined *)in_stack_0000000c;
      in_stack_0000000c = (undefined4 *)((int)in_stack_0000000c + 1);
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
    *(undefined *)((int)puVar4 + uVar2) = 0;
  }
  this->mbr_0x38 = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x24 = 3;
  this->vftptr_0x0 = &cls_0x5b9ff0__vftable_5b9ff0_005b9ff0;
  this->mbr_0x3c = 1;
  this->mbr_0x40 = 0x4b00;
  this->mbr_0x4c = 4;
  ExceptionList = local_c;
  return this;
}



// Function at 0057ba20

cls_0x5b9ff0 * __thiscall OOAnalyzer::cls_0x5b9ff0::virt_meth_0x57ba20(cls_0x5b9ff0 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9ff0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0057ba40

cls_0x5b9ff0 * __thiscall OOAnalyzer::cls_0x5b9ff0::~cls_0x5b9ff0(cls_0x5b9ff0 *this)

{
  char cVar1;
  dword dVar2;
  cls_0x5b9ff0 *pcVar3;
  
  this->vftptr_0x0 = (cls_0x5b9ff0__vftable_5b9ff0 *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
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
  pcVar3 = (cls_0x5b9ff0 *)this->mbr_0x8;
  this->mbr_0x20 = 0;
  if (pcVar3 != (cls_0x5b9ff0 *)0x0) {
    cVar1 = *(char *)((int)&pcVar3[-1].mbr_0x4c + 3);
    pcVar3 = (cls_0x5b9ff0 *)((int)&pcVar3[-1].mbr_0x4c + 3);
    if ((cVar1 != '\0') && (cVar1 != -1)) {
      this->mbr_0x8 = 0;
      this->mbr_0xc = 0;
      this->mbr_0x10 = 0;
      this->vftptr_0x0 = (cls_0x5b9ff0__vftable_5b9ff0 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
      *(char *)&pcVar3->vftptr_0x0 = cVar1 + -1;
      return pcVar3;
    }
    pcVar3 = (cls_0x5b9ff0 *)FUN_004830f0(pcVar3);
  }
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this->vftptr_0x0 = (cls_0x5b9ff0__vftable_5b9ff0 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  return pcVar3;
}



