#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b401c



// Function at 005101b0

void cls_0x5b401c::virt_meth_0x5101b0()

{
  (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
  this->mbr_0xe0 = 0;
  this->mbr_0x184 = 0x8000;
  this->mbr_0x188 = 0x100000;
  this->mbr_0x18c = 1;
  return;
}



// Function at 00510220

void cls_0x5b401c::virt_meth_0x510220()

{
  short sVar1;
  byte bVar2;
  undefined uVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  int iVar4;
  TPlayScreen *this_00;
  dword dVar5;
  dword *pdVar6;
  int iVar7;
  cls_0x44ceb0 cStack_48;
  
  bVar2 = (*this->vftptr_0x0[2].virt_meth_0x40e570_84)(this);
  sVar1 = *(short *)&this->mbr_0xc;
  if (sVar1 == 0) {
    if (this->mbr_0x18c != 0) {
      dVar5 = this->mbr_0x188;
      if (dVar5 != 0) {
        pdVar6 = &this->mbr_0x1c;
        iVar7 = 0;
        iVar4 = SoftwareRenderer::meth_0x4df070((SoftwareRenderer *)this);
        FUN_0046db20(iVar4,dVar5,(int *)pdVar6,iVar7);
        dVar5 = this->mbr_0x188;
        this->mbr_0x8 = this->mbr_0x8 & 0xfffffffe | 0x10008;
        this->mbr_0x24 = -((int)(dVar5 + ((int)dVar5 >> 0x1f & 0xfU)) >> 4);
        this->mbr_0x184 =
             (dword)(0x1e0 / (longlong)((int)(dVar5 + ((int)dVar5 >> 0x1f & 0xffffU)) >> 0x10));
      }
      (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
    }
  }
  else {
    if (sVar1 == 1) {
      dVar5 = this->mbr_0x184 - 1;
      this->mbr_0x184 = dVar5;
      if ((bVar2 & 2) == 0) {
        if (this->mbr_0xd8 == 0) {
          this_00 = (TPlayScreen *)0x0;
        }
        else {
          this_00 = *(TPlayScreen **)(this->mbr_0xd8 + 4);
        }
        cls_0x44ceb0::cls_0x44ceb0(&cStack_48,&this->mbr_0x10,0x100,0xe0,2,0);
        while ((TPlayScreen *)cStack_48.mbr_0xc != (TPlayScreen *)0x0) {
          if (((((TPlayScreen *)cStack_48.mbr_0xc != this_00) &&
               (uVar3 = (*((TScreen *)cStack_48.mbr_0xc)->vftptr_0x0->virt_meth_0x477e50_448)
                                  ((void *)cStack_48.mbr_0xc),
               CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var)) &&
              (uVar3 = (*this->vftptr_0x0->virt_meth_0x410aa0_4)(this),
              CONCAT31(extraout_var_00,uVar3) < 0x21)) &&
             ((this_00 == (TPlayScreen *)0x0 ||
              (iVar4 = TPlayScreen::meth_0x4c89c0(this_00), iVar4 != 0)))) goto LAB_00510310;
          cls_0x44ceb0::meth_0x44d080((cls_0x44ceb0 *)&stack0xffffffb4);
          cStack_48.mbr_0xc = cStack_48.mbr_0x8;
        }
        if (0 < (int)dVar5) goto LAB_005103ac;
      }
LAB_00510310:
      this->mbr_0x8 = this->mbr_0x8 & 0xfffefff7 | 1;
      (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
      this->virt_meth_0x4de800();
      return;
    }
    if ((sVar1 == 2) && (this->mbr_0x58 == 0)) {
      SoftwareRenderer::meth_0x4defe0((SoftwareRenderer *)this);
      this->virt_meth_0x4de800();
      return;
    }
  }
LAB_005103ac:
  this->virt_meth_0x4de800();
  return;
}



// Function at 00514a50

cls_0x5a47f0 * cls_0x5b401c::virt_meth_0x514a50()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0cfe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x508);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b401c__vftable_5b401c_005b401c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005152e0

cls_0x5b401c * cls_0x5b401c::virt_meth_0x5152e0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005153a0

cls_0x5b401c * cls_0x5b401c::virt_meth_0x5153a0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b401c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005153c0

cls_0x5b401c * __thiscall cls_0x5b401c::~cls_0x5b401c(cls_0x5b401c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0ef8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b401c__vftable_5b401c_005b401c;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b401c *)pcVar1;
}



