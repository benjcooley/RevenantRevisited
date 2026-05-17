// Decompiled methods and structure for class: cls_0x5a5ba0

/*
/OOAnalyzer/cls_0x5a5ba0
pack(disabled)
Structure cls_0x5a5ba0 {
   0   cls_0x5a5ba0::vftable_5a5ba0 *   4   vftptr_0x0   "pointer to cls_0x5a5ba0::vftable_5a5ba0"
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
}
Length: 436 Alignment: 1

*/

// Function at 00488910

cls_0x5a5ba0 * __thiscall OOAnalyzer::cls_0x5a5ba0::cls_0x5a5ba0(cls_0x5a5ba0 *this)

{
  this->mbr_0xc = 0xbc;
  this->mbr_0x1c = 0xbc;
  this->mbr_0x4 = 0x1c4;
  this->mbr_0x14 = 0x1c4;
  this->mbr_0x10 = 0x132;
  this->mbr_0x20 = 0x132;
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5ba0__vftable_5a5ba0_005a5ba0;
  return this;
}



// Function at 00488970

cls_0x5a5ba0 * __thiscall OOAnalyzer::cls_0x5a5ba0::~cls_0x5a5ba0(cls_0x5a5ba0 *this)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  cls_0x5a5ba0 *pcVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d6f6;
  local_c = ExceptionList;
  dVar1 = this->mbr_0x19c;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a5ba0__vftable_5a5ba0_005a5ba0;
  if (dVar1 != 0) {
    iVar2 = FUN_0058cff3(dVar1);
    DAT_0065ba08 = DAT_0065ba08 - iVar2;
  }
  FUN_0058cfab(dVar1);
  this->mbr_0x19c = 0;
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  iVar2 = *(int *)&this->field_0x98;
  if (iVar2 != 0) {
    iVar3 = FUN_0058cff3(iVar2);
    DAT_0065ba08 = DAT_0065ba08 - iVar3;
  }
  pcVar4 = (cls_0x5a5ba0 *)FUN_0058cfab(iVar2);
  ExceptionList = local_c;
  return pcVar4;
}



// Function at 00488ae0

cls_0x5a5ba0 * __thiscall OOAnalyzer::cls_0x5a5ba0::virt_meth_0x488ae0(cls_0x5a5ba0 *this)

{
  int iVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5ba0(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5ba0 *)0x0) {
      iVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - iVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00546b50

undefined4 __thiscall OOAnalyzer::cls_0x5a5ba0::virt_meth_0x546b50(cls_0x5a5ba0 *this)

{
  cls_0x5a68d8__vftable_5a68d8 *pcVar1;
  dword dVar2;
  cls_0x5a68d8 *pcVar3;
  int iVar4;
  dword dVar5;
  dword local_114 [64];
  void *pvStack_14;
  void *pvStack_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a1fb2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5a4494_TPane::virt_meth_0x434e40((cls_0x5a4494_TPane *)this);
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 0;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    local_114[0] = this->mbr_0xc;
    dVar5 = this->mbr_0x10;
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar3->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(pcVar3,local_114[0],dVar5);
    pcVar3->mbr_0x70 = 1;
  }
  dVar5 = this->mbr_0x8;
  pcVar1 = pcVar3->vftptr_0x0;
  local_4 = 0xffffffff;
  this->mbr_0x18c = (dword)pcVar3;
  (*pcVar1->virt_meth_0x419e60_64)(pcVar3,this->mbr_0x4);
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c,dVar5);
  uStack_8 = (undefined *)0x2;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,3);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a3a40(pcVar3);
  }
  uStack_8 = (undefined *)0xffffffff;
  this->mbr_0x188 = (dword)pcVar3;
  FUN_0058b100(local_114,&DAT_005e53b4,&DAT_0065dde8,s_statpane_def_005e53a4);
  iVar4 = FUN_004a13f0(local_114,&DAT_005e53bc,0);
  if (iVar4 == 0) {
    FUN_00481c10(s_Unable_to_load_STATPANE_DEF_005e53c0,0);
  }
  dVar5 = FUN_004a17b0(iVar4);
  this->mbr_0x1a4 = dVar5;
  if ((int)dVar5 < 1) {
    this->mbr_0x1a0 = 0;
    this->mbr_0x1a4 = 0;
  }
  else {
    dVar5 = FUN_00482fb0(dVar5 + 1);
    this->mbr_0x1a0 = dVar5;
    FUN_004a15a0(dVar5,this->mbr_0x1a4,1,iVar4);
    dVar5 = this->mbr_0x1a4;
    dVar2 = this->mbr_0x1a0;
    *(undefined *)(dVar2 + dVar5) = 0;
    iVar4 = FUN_004834e0(dVar2,dVar5);
    if (iVar4 != 0) {
      FUN_00483540(this->mbr_0x1a0,this->mbr_0x1a4);
    }
  }
  this->mbr_0x1a8 = 0;
  this->mbr_0x17c = 0;
  this->mbr_0x184 = 0;
  this->mbr_0x190 = 0;
  this->mbr_0x194 = 0;
  this->mbr_0x1ac = 0;
  this->mbr_0x1b0 = 0;
  this->mbr_0x19c = 0;
  (*this->vftptr_0x0->virt_meth_0x5496a0_184)(this);
  ExceptionList = pvStack_14;
  return 1;
}



// Function at 00546d70

void __thiscall OOAnalyzer::cls_0x5a5ba0::virt_meth_0x546d70(cls_0x5a5ba0 *this)

{
  FUN_004830f0(this->mbr_0x1a0);
  this->mbr_0x1a0 = 0;
  this->mbr_0x1a4 = 0;
  if ((undefined4 *)this->mbr_0x18c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x18c)(1);
    this->mbr_0x18c = 0;
  }
  if ((undefined4 *)this->mbr_0x188 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x188)(1);
    this->mbr_0x188 = 0;
  }
  if (this->mbr_0x19c != 0) {
    FUN_00482f80(this->mbr_0x19c);
  }
  this->mbr_0x19c = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 00549010

void __thiscall OOAnalyzer::cls_0x5a5ba0::virt_meth_0x549010(cls_0x5a5ba0 *this)

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x5491c0_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 005493b0

void __thiscall
OOAnalyzer::cls_0x5a5ba0::virt_meth_0x5493b0(cls_0x5a5ba0 *this,int param_1,int param_2)

{
  int in_stack_0000000c;
  
  if (param_1 == 2) {
    (*this->vftptr_0x0->virt_meth_0x5496a0_184)(this);
    cls_0x5b93c4::virt_meth_0x436530((cls_0x5b93c4 *)this,2,param_2);
    return;
  }
  if ((((param_1 == 4) && (-1 < param_2)) && (-1 < in_stack_0000000c)) &&
     ((param_2 < (int)this->mbr_0xc && (in_stack_0000000c < (int)this->mbr_0x10)))) {
    FUN_0043a100(0,0,0);
    FUN_0043a140(0);
    FUN_0043a170(0x670648);
    FUN_0043a240(0,0,0);
    (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  }
  cls_0x5b93c4::virt_meth_0x436530((cls_0x5b93c4 *)this,param_1,param_2);
  return;
}



// Function at 005496a0

void __thiscall OOAnalyzer::cls_0x5a5ba0::virt_meth_0x5496a0(cls_0x5a5ba0 *this)

{
  char cVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 *puVar8;
  int *in_stack_00000004;
  undefined4 uVar9;
  
  if ((int *)this->mbr_0x198 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x198 + 0xa0))();
  }
  this->mbr_0x198 = (dword)in_stack_00000004;
  if (this->mbr_0x19c != 0) {
    FUN_00482f80(this->mbr_0x19c);
  }
  uVar3 = 0xffffffff;
  pcVar6 = s_page1_005e5714;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  puVar2 = (undefined4 *)FUN_00482ef0(~uVar3);
  uVar3 = 0xffffffff;
  pcVar6 = s_page1_005e570c;
  do {
    pcVar7 = pcVar6;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar7 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar7;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  uVar9 = 1;
  puVar5 = (undefined4 *)(pcVar7 + -uVar3);
  puVar8 = puVar2;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar8 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar8 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  this->mbr_0x19c = (dword)puVar2;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  if (in_stack_00000004 != (int *)0x0) {
    (**(code **)(*in_stack_00000004 + 0x9c))(uVar9);
  }
  return;
}



