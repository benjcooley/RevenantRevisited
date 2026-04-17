// Decompiled methods and structure for class: cls_0x5a4680

/*
/OOAnalyzer/cls_0x5a4680
pack(disabled)
Structure cls_0x5a4680 {
   0   cls_0x5a4680::vftable_5a4680 *   4   vftptr_0x0   "pointer to cls_0x5a4680::vftable_5a4680"
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
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 432 Alignment: 1

*/

// Function at 00441bd0

bool __thiscall OOAnalyzer::cls_0x5a4680::virt_meth_0x441bd0(cls_0x5a4680 *this)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  int iVar5;
  int extraout_EDX;
  int iVar6;
  dword dVar7;
  int iVar8;
  undefined2 *puVar9;
  int local_10;
  int local_c;
  
  cls_0x5a4494::virt_meth_0x434e40((cls_0x5a4494 *)this);
  dVar7 = this->mbr_0xc;
  dVar1 = this->mbr_0x10;
  iVar8 = 0;
  iVar2 = (int)dVar7 / ((int)dVar7 / 0x28);
  local_10 = 0;
  local_c = 0;
  iVar3 = (int)dVar1 / ((int)dVar1 / 0x14);
  iVar5 = (int)dVar1 % ((int)dVar1 / 0x14);
  if (0 < (int)dVar1) {
    do {
      iVar6 = 0;
      if (0 < (int)dVar7) {
        puVar4 = (&PTR_DAT_005ceca0)[iVar8 * 3];
        puVar9 = &DAT_005ceca8 + iVar8 * 6;
        do {
          cls_0x5b5808::meth_0x444e30
                    ((cls_0x5b5808 *)this,puVar4,iVar6,local_c,iVar2,iVar3,
                     CONCAT22((short)((uint)iVar5 >> 0x10),*puVar9),*(undefined4 *)(puVar9 + -2),0,0
                     ,0,0,0,0xffffffff,0,0,0);
          iVar8 = local_10 + 1;
          puVar4 = *(undefined **)(puVar9 + 2);
          if (puVar4 == (undefined *)0x0) {
            return true;
          }
          iVar6 = iVar6 + iVar2;
          dVar7 = this->mbr_0xc;
          iVar5 = extraout_EDX;
          puVar9 = puVar9 + 6;
          local_10 = iVar8;
        } while (iVar6 < (int)dVar7);
      }
      local_c = local_c + iVar3;
      iVar5 = iVar3;
    } while (local_c < (int)this->mbr_0x10);
  }
  return (&PTR_DAT_005ceca0)[iVar8 * 3] == (undefined *)0x0;
}



// Function at 00441ce0

void __thiscall OOAnalyzer::cls_0x5a4680::virt_meth_0x441ce0(cls_0x5a4680 *this)

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x441d10_84)(this,PTR_DAT_005d79e0);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 00441d10

void __thiscall OOAnalyzer::cls_0x5a4680::virt_meth_0x441d10(cls_0x5a4680 *this,int *param_1)

{
  (**(code **)(*param_1 + 100))(0,0,this->mbr_0xc,this->mbr_0x10,0,0xffff,0);
  cls_0x5b93c4::virt_meth_0x435cb0((cls_0x5b93c4 *)this);
  return;
}



// Function at 00441d50

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a4680::virt_meth_0x441d50(cls_0x5a4680 *this,int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int in_stack_00000008;
  
  cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  iVar2 = DAT_0066698c;
  if (((in_stack_00000008 != 0) && (iVar4 = param_1 + -0x70, -1 < iVar4)) && (iVar4 < 5)) {
    piVar1 = (int *)((int)this + iVar4 * 0xc + 0x17c);
    if (DAT_0065c9e0 != 0) {
      *piVar1 = DAT_00666988;
      iVar3 = DAT_00666990;
      piVar1[1] = iVar2;
      iVar2 = _DAT_00666970;
      piVar1[2] = iVar3;
      *(int *)((int)this + iVar4 * 4 + 0x1ac) = iVar2;
      return;
    }
    if ((-1 < *piVar1) || (-1 < *(int *)((int)this + (iVar4 * 3 + 0x60) * 4))) {
      cls_0x5a5320::meth_0x450d20((cls_0x5a5320 *)&DAT_006668d8);
      _DAT_00666970 = *(int *)((int)this + iVar4 * 4 + 0x1ac);
      if (_DAT_00666970 != DAT_00666974) {
        cls_0x5a5320::meth_0x4546a0((cls_0x5a5320 *)&DAT_006668d8);
      }
      UNK_00656e7c._76_4_ = 1;
    }
  }
  return;
}



// Function at 00445420

cls_0x5a4680 * __thiscall OOAnalyzer::cls_0x5a4680::cls_0x5a4680(cls_0x5a4680 *this)

{
  dword *pdVar1;
  dword *pdVar2;
  int iVar3;
  
  this->mbr_0x4 = 0x168;
  this->mbr_0x14 = 0x168;
  this->mbr_0x8 = 0x164;
  this->mbr_0x18 = 0x164;
  this->mbr_0xc = 0x78;
  this->mbr_0x1c = 0x78;
  this->mbr_0x10 = 0x7d;
  this->mbr_0x20 = 0x7d;
  this->vftptr_0x0 = (cls_0x5a4680__vftable_5a4680 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4680__vftable_5a4680_005a4680;
  pdVar1 = &this->mbr_0x180;
  pdVar2 = &this->mbr_0x1ac;
  iVar3 = 4;
  do {
    *pdVar2 = 0xffffffff;
    pdVar1[1] = 0xffffffff;
    *pdVar1 = 0xffffffff;
    pdVar1[-1] = 0xffffffff;
    pdVar2 = pdVar2 + 1;
    pdVar1 = pdVar1 + 3;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return this;
}



// Function at 004454d0

cls_0x5a4680 * __thiscall OOAnalyzer::cls_0x5a4680::virt_meth_0x4454d0(cls_0x5a4680 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4680(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004454f0

cls_0x5a4680 * __thiscall OOAnalyzer::cls_0x5a4680::~cls_0x5a4680(cls_0x5a4680 *this)

{
  cls_0x5a4680 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ceb6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4680__vftable_5a4680 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5a4680__vftable_5a4680 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4680 *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



