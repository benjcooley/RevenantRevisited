#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a502c



// Function at 0046b010

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x5a502c::virt_meth_0x46b010()

{
  int *piVar1;
  cls_0x46d6b0 *this_00;
  int iVar2;
  uint *puVar3;
  
  this->virt_meth_0x434e40();
  iVar2 = DAT_00676828;
  this->mbr_0x1cc = 0;
  if ((iVar2 == 0) || (DAT_00676830 == 0)) {
    cls_0x5756d0::meth_0x578210((cls_0x5756d0 *)&DAT_00676738);
  }
  this->mbr_0x1c4 = 0xffffffff;
  cls_0x5756d0::meth_0x575ed0((cls_0x5756d0 *)&DAT_00676738);
  puVar3 = (uint *)0x14;
  piVar1 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  iVar2 = _DAT_006668d0;
  this->mbr_0x1c8 = (dword)piVar1;
  piVar1[6] = iVar2;
  this_00 = (cls_0x46d6b0 *)
            FUN_0047f670((cls_0x49ead0 *)s_connect_dat_005d409c,(void *)0xffffffff,0,puVar3);
  if (this_00 == (cls_0x46d6b0 *)0x0) {
    return 0;
  }
  cls_0x5b98b8::meth_0x435c60((cls_0x5b98b8 *)this,this_00);
  cls_0x46d6b0::meth_0x46d710(this_00);
  cls_0x5b98b8::meth_0x435660((cls_0x5b98b8 *)this);
  cls_0x5b98b8::meth_0x435ad0((cls_0x5b98b8 *)this,DAT_0065bb10);
  iVar2 = cls_0x5756d0::meth_0x577c10((cls_0x5756d0 *)&DAT_00676738);
  if (iVar2 == 0) {
    this->mbr_0x1b8 = 0;
  }
  else {
    this->mbr_0x1b8 = 1;
  }
  this->mbr_0x1ac = 0;
  cls_0x5b98b8::meth_0x4377c0_DefWidget_LoadFile((cls_0x5b98b8 *)this);
  cls_0x5b98b8::meth_0x437620_DefWidget_ParseBuffer((cls_0x5b98b8 *)this);
  return 1;
}



// Function at 0046b110

void cls_0x5a502c::virt_meth_0x46b110()

{
  if ((LPCVOID)this->mbr_0x1ac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1ac);
  }
  if ((LPCVOID)this->mbr_0x1c8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1c8);
    this->mbr_0x1c8 = 0;
  }
  this->virt_meth_0x434f30();
  return;
}



// Function at 0046caf0

void cls_0x5a502c::virt_meth_0x46caf0()

{
  cls_0x5a3d44 *this_00;
  cls_0x5a502c__vftable_5a502c *pcVar1;
  DWORD DVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  
  this->virt_meth_0x435d70();
  if (this->mbr_0x1b0 == 0) {
    if (this->mbr_0x1b8 == 0) {
      if ((this->mbr_0x1b4 == 0) &&
         (iVar4 = cls_0x5756d0::meth_0x576880((cls_0x5756d0 *)&DAT_00676738), iVar4 != 0)) {
        cls_0x5756d0::meth_0x5769a0((cls_0x5756d0 *)&DAT_00676738);
        cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0x188);
        (**(code **)(*(int *)this->mbr_0x188 + 0x1c))(((int *)this->mbr_0x188)[5] | 0x20);
      }
      if (DAT_00668128 != 0) {
        if ((this->mbr_0x1cc != 0) && (DAT_00659c6c == 0)) {
          pcVar1 = this->vftptr_0x0;
          DAT_00659c6c = 1;
          cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
          (*pcVar1->virt_meth_0x46bd60_148)(this);
        }
        if (DAT_00668128 != 0) {
          if (((this->mbr_0x1d0 != 0) && (iVar4 = *(int *)(this->mbr_0x180 + 0x98), 0 < iVar4)) &&
             (DAT_00659c68 == 0)) {
            DAT_00659c68 = 1;
            FUN_00483300_RandomRange(0,iVar4 + -1);
            cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0x180);
            this->mbr_0x1cc = 1;
          }
          if (((DAT_00668128 != 0) &&
              (iVar4 = cls_0x5756d0::meth_0x5769a0((cls_0x5756d0 *)&DAT_00676738), iVar4 != 0)) &&
             (DAT_00659c64 == 0)) {
            DAT_00659c64 = 1;
            pcVar1 = this->vftptr_0x0;
            cls_0x5b98b8::meth_0x436980((cls_0x5b98b8 *)this);
            (*pcVar1->virt_meth_0x46bd60_148)(this);
            cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0x188);
            (*this->vftptr_0x0->virt_meth_0x46bd60_148)(this);
          }
        }
      }
    }
    else if (this->mbr_0x1b8 == 1) {
      iVar4 = cls_0x5756d0::meth_0x577e40((cls_0x5756d0 *)&DAT_00676738);
      if (iVar4 == 0) {
        this->mbr_0x1b8 = 2;
        cls_0x5b98b8::meth_0x46b910((cls_0x5b98b8 *)this);
        return;
      }
      if ((DAT_00676828 != 0) && (DAT_00676830 != 0)) {
        cls_0x5b98b8::meth_0x46b910((cls_0x5b98b8 *)this);
        this->mbr_0x1b8 = 0;
        return;
      }
    }
  }
  else {
    if ((this->mbr_0x1b4 == 0) && (DVar2 = GetTickCount(), this->mbr_0x1bc < DVar2)) {
      cls_0x5756d0::meth_0x577350((cls_0x5756d0 *)&DAT_00676738);
      bVar6 = DAT_006767b4 == '\0';
      this->mbr_0x1bc = DVar2 + 1000;
      if (bVar6) {
        dVar3 = 0xffffffff;
      }
      else {
        dVar3 = cls_0x4609f0::meth_0x460d60((cls_0x4609f0 *)&UNK_0065a630.field_0x138,&DAT_006767b4)
        ;
      }
      if (this->mbr_0x1c4 != dVar3) {
        this->mbr_0x1c4 = dVar3;
        cls_0x5b98b8::meth_0x46b910((cls_0x5b98b8 *)this);
        cls_0x45f7c0::meth_0x462000((cls_0x45f7c0 *)&DAT_00659bb8,this->mbr_0x1c4);
        cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0x180);
        cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0x180);
        (**(code **)(*(int *)this->mbr_0x180 + 0x1c))(((int *)this->mbr_0x180)[5] | 0x20);
      }
    }
    this_00 = (cls_0x5a3d44 *)this->mbr_0x1a4;
    if ((this_00 != (cls_0x5a3d44 *)0x0) && (this->mbr_0x1c0 != DAT_006766b0)) {
      iVar4 = (this_00->mbr_0xa4 - 1) + this_00->mbr_0x9c;
      iVar5 = this_00->mbr_0x98 - 1;
      if (iVar4 < iVar5) {
        iVar5 = iVar4;
      }
      dVar3 = this_00->mbr_0x98;
      cls_0x5a3d44::meth_0x430c50(this_00);
      if ((int)(dVar3 - 1) <= iVar5) {
        cls_0x5a3d44::meth_0x430ab0((cls_0x5a3d44 *)this->mbr_0x1a4);
      }
      (**(code **)(*(int *)this->mbr_0x1a4 + 0x1c))(((int *)this->mbr_0x1a4)[5] | 0x20);
      this->mbr_0x1c0 = DAT_006766b0;
      return;
    }
  }
  return;
}



// Function at 0046d5d0

cls_0x5a502c::cls_0x5a502c()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5a502c__vftable_5a502c *)&cls_0x5a4494__vftable_5a4494_005a4494;
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
  this->mbr_0x1c8 = 0;
  this->mbr_0x1ac = 0;
  this->vftptr_0x0 = &cls_0x5a502c__vftable_5a502c_005a502c;
  return this;
}



// Function at 0046d630

cls_0x5a502c * cls_0x5a502c::virt_meth_0x46d630()

{
  byte in_stack_00000004;
  
  ~cls_0x5a502c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d650

cls_0x5a502c * __thiscall cls_0x5a502c::~cls_0x5a502c(cls_0x5a502c *this)

{
  cls_0x5a502c *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d23e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a502c__vftable_5a502c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a502c *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



