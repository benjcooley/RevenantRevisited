// Decompiled methods and structure for class: cls_0x5b5750_TScrollPane

/*
/OOAnalyzer/cls_0x5b5750_TScrollPane
pack(disabled)
Structure cls_0x5b5750_TScrollPane {
   0   cls_0x5b5750_TScrollPane::vftable_5b5750 *   4   vftptr_0x0   "pointer to cls_0x5b5750_TScrollPane::vftable_5b5750"
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
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 408 Alignment: 1

*/

// Function at 00521360

void __thiscall OOAnalyzer::cls_0x5b5750_TScrollPane::virt_meth_0x521360_TScrollPane_DrawBackground(cls_0x5b5750_TScrollPane *this)

{
  char cVar1;
  char *pcVar2;
  int *piVar3;
  char *pcVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
  if (this->mbr_0x50 != 0) {
    piVar3 = (int *)cls_0x46d6b0::meth_0x46d710((cls_0x46d6b0 *)this->mbr_0x180);
    iVar7 = (int)((0x1e0 - piVar3[1]) + (0x1e0 - piVar3[1] >> 0x1f & 3U)) >> 2;
    iVar6 = (0x280 - *piVar3) / 2;
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)PTR_DAT_005d79e0,iVar6,iVar7,piVar3,0x110);
    pcVar4 = (char *)0x0;
    if ((this->mbr_0x17c != 0) &&
       (pcVar2 = *(char **)(this->mbr_0x17c + 0xd8), pcVar2 != (char *)0x0)) {
      uVar5 = 0xffffffff;
      pcVar4 = pcVar2;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        cVar1 = *pcVar4;
        pcVar4 = pcVar4 + 1;
      } while (cVar1 != '\0');
      pcVar4 = pcVar2;
      if (~uVar5 - 1 < 0x10) {
        pcVar4 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      }
    }
    if ((this->mbr_0x17c != 0) && (pcVar4 != (char *)0x0)) {
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffe4);
      cls_0x5a68d8::meth_0x4be2b0
                ((cls_0x5a68d8 *)PTR_DAT_005d79e0,(char)iVar6 + 'A',(char)iVar7 + 'D',
                 (char)this->mbr_0x190,this->mbr_0x194,(char)pcVar4,this->mbr_0x188);
    }
    if ((int)this->mbr_0x188 < 1) {
      iVar6 = iRam00000000;
      if (1 < *(uint *)&this->field_0x88) {
        iVar6 = **(int **)(*(int *)&this->field_0x98 + 4);
      }
    }
    else {
      if (*(uint *)&this->field_0x88 < 2) {
        piVar3 = (int *)0x0;
      }
      else {
        piVar3 = *(int **)(*(int *)&this->field_0x98 + 4);
      }
      iVar6 = *piVar3;
    }
    (**(code **)(iVar6 + 0x1c))();
    if ((int)((this->mbr_0x188 - 1) + DAT_0066dacc) < (int)this->mbr_0x18c) {
      if (*(int *)&this->field_0x88 == 0) {
        piVar3 = (int *)0x0;
      }
      else {
        piVar3 = **(int ***)&this->field_0x98;
      }
      iVar6 = *piVar3;
    }
    else {
      iVar6 = iRam00000000;
      if (*(int *)&this->field_0x88 != 0) {
        iVar6 = ***(int ***)&this->field_0x98;
      }
    }
    (**(code **)(iVar6 + 0x1c))();
    if (*(uint *)&this->field_0x88 < 3) {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = *(int **)(*(int *)&this->field_0x98 + 8);
    }
    (**(code **)(*piVar3 + 0x1c))();
    (*this->vftptr_0x0->FUN_004361b0_144)();
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  return;
}



// Function at 00521d70

cls_0x5b5750_TScrollPane * __thiscall OOAnalyzer::cls_0x5b5750_TScrollPane::cls_0x5b5750_TScrollPane(cls_0x5b5750_TScrollPane *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5b5750__vftable_5b5750 *)&cls_0x5a4494__vftable_5a4494_005a4494;
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
  this->mbr_0x17c = 0;
  this->vftptr_0x0 = &cls_0x5b5750__vftable_5b5750_005b5750;
  return this;
}



// Function at 00521dd0

cls_0x5b5750_TScrollPane * __thiscall OOAnalyzer::cls_0x5b5750_TScrollPane::virt_meth_0x521dd0(cls_0x5b5750_TScrollPane *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b5750_TScrollPane(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00521df0

cls_0x5b5750_TScrollPane * __thiscall OOAnalyzer::cls_0x5b5750_TScrollPane::~cls_0x5b5750_TScrollPane(cls_0x5b5750_TScrollPane *this)

{
  cls_0x5b5750_TScrollPane *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1306;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b5750__vftable_5b5750 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5b5750__vftable_5b5750 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b5750_TScrollPane *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



