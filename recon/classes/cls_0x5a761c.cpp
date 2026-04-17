// Decompiled methods and structure for class: cls_0x5a761c

/*
/OOAnalyzer/cls_0x5a761c
pack(disabled)
Structure cls_0x5a761c {
   0   cls_0x5a761c::vftable_5a761c *   4   vftptr_0x0   "pointer to cls_0x5a761c::vftable_5a761c"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   368   dword   4   mbr_0x170   "Unsigned Double-Word (ddw, 4-bytes)"
   372   dword   4   mbr_0x174   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   644   dword   4   mbr_0x284   "Unsigned Double-Word (ddw, 4-bytes)"
   660   dword   4   mbr_0x294   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 664 Alignment: 1

*/

// Function at 004c12c0

cls_0x5a761c * __thiscall OOAnalyzer::cls_0x5a761c::virt_meth_0x4c12c0(cls_0x5a761c *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c1770

cls_0x5a761c * __thiscall OOAnalyzer::cls_0x5a761c::~cls_0x5a761c(cls_0x5a761c *this)

{
  dword *this_00;
  undefined4 *puVar1;
  int *piVar2;
  int iVar3;
  cls_0x5a7b98 *pcVar4;
  int iVar5;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059e2d6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a761c__vftable_5a761c *)&PTR_virt_meth_0x4d6c00_005a7848;
  local_4 = 1;
  if (this->mbr_0x284 != 0) {
    FUN_004830f0(this->mbr_0x284);
    this->mbr_0x284 = 0;
  }
  if (this->mbr_0x294 != 0) {
    iVar5 = 0;
    piVar2 = (int *)(this->mbr_0xfc + 0xcc);
    if (0 < *piVar2) {
      do {
        iVar3 = *(int *)(piVar2[4] + iVar5 * 4);
        if (iVar3 == 0) {
          iVar3 = piVar2[5];
        }
        if (((*(uint *)(iVar3 + 0x24) & 0x800000) != 0) &&
           (iVar3 = cls_0x53ed70::meth_0x53f010((cls_0x53ed70 *)((int)&UNK_00667c34 + 4)),
           iVar3 != 0)) {
          FUN_004754e0(iVar3 + 0x2a);
        }
        iVar5 = iVar5 + 1;
        piVar2 = (int *)(this->mbr_0xfc + 0xcc);
      } while (iVar5 < *piVar2);
    }
    this->mbr_0x294 = 0;
  }
  this_00 = &this->mbr_0x170;
  iVar5 = 0;
  local_4._0_1_ = 2;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar5) &&
         (puVar1 = *(undefined4 **)(this->mbr_0x180 + iVar5 * 4), puVar1 != (undefined4 *)0x0)) {
        (**(code **)*puVar1)(1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x174 = 0;
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0(this->mbr_0x180);
  local_4 = 0xffffffff;
  pcVar4 = cls_0x5a7b98::~cls_0x5a7b98((cls_0x5a7b98 *)this);
  ExceptionList = local_c;
  return (cls_0x5a761c *)pcVar4;
}



// Function at 004d7550

cls_0x5a7b98 * __thiscall
OOAnalyzer::cls_0x5a761c::virt_meth_0x4d7550(cls_0x5a761c *this,undefined4 param_1)

{
  cls_0x5a7b98 *this_00;
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ea69;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a7b98 *)FUN_00482fb0(0x2a0);
  local_4 = 0;
  if (this_00 != (cls_0x5a7b98 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8((cls_0x5a50e8 *)this_00);
    local_4._0_1_ = 1;
    (this_00->cls_0x5a50e8).vftptr_0x0 =
         (cls_0x5a50e8__vftable_5a50e8 *)&cls_0x5a7b98__vftable_5a7b98_005a7b98;
    *(undefined4 *)&(this_00->cls_0x5a50e8).field_0xdc = 0;
    (this_00->cls_0x5a50e8).mbr_0xd8 = 0;
    (this_00->cls_0x5a50e8).mbr_0xe0 = 0;
    cls_0x5a7b98::meth_0x4db000(this_00);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0(&this_00->cls_0x41c7f0,0x20);
    puVar3 = (undefined4 *)(this_00->cls_0x41c7f0).mbr_0x10;
    if (puVar3 != (undefined4 *)0x0) {
      for (uVar1 = (this_00->cls_0x41c7f0).mbr_0x8 & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
        *(undefined *)puVar3 = 0;
        puVar3 = (undefined4 *)((int)puVar3 + 1);
      }
    }
    (this_00->cls_0x41c7f0).mbr_0x4 = 0;
    (this_00->cls_0x41c7f0).mbr_0x0 = 0;
    (this_00->cls_0x41c7f0).mbr_0x14 = 0;
    local_4 = CONCAT31(local_4._1_3_,3);
    (this_00->cls_0x5a50e8).vftptr_0x0 =
         (cls_0x5a50e8__vftable_5a50e8 *)&PTR_virt_meth_0x4d6c00_005a7848;
    cls_0x5a7b98::meth_0x4c18a0(this_00);
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a7b98 *)0x0;
}



