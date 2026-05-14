// Decompiled methods and structure for class: cls_0x5b401c

/*
/OOAnalyzer/cls_0x5b401c
pack(disabled)
Structure cls_0x5b401c {
   0   cls_0x5b401c::vftable_5b401c *   4   vftptr_0x0   "pointer to cls_0x5b401c::vftable_5b401c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   448   dword   4   mbr_0x1c0   "Unsigned Double-Word (ddw, 4-bytes)"
   648   dword   4   mbr_0x288   "Unsigned Double-Word (ddw, 4-bytes)"
   652   dword   4   mbr_0x28c   "Unsigned Double-Word (ddw, 4-bytes)"
   656   dword   4   mbr_0x290   "Unsigned Double-Word (ddw, 4-bytes)"
   1020   dword   4   mbr_0x3fc   "Unsigned Double-Word (ddw, 4-bytes)"
   1024   dword   4   mbr_0x400   "Unsigned Double-Word (ddw, 4-bytes)"
   1084   dword   4   mbr_0x43c   "Unsigned Double-Word (ddw, 4-bytes)"
   1148   dword   4   mbr_0x47c   "Unsigned Double-Word (ddw, 4-bytes)"
   1152   dword   4   mbr_0x480   "Unsigned Double-Word (ddw, 4-bytes)"
   1212   dword   4   mbr_0x4bc   "Unsigned Double-Word (ddw, 4-bytes)"
   1276   dword   4   mbr_0x4fc   "Unsigned Double-Word (ddw, 4-bytes)"
   1280   dword   4   mbr_0x500   "Unsigned Double-Word (ddw, 4-bytes)"
   1284   dword   4   mbr_0x504   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1288 Alignment: 1

*/

// Function at 005101b0

void __thiscall OOAnalyzer::cls_0x5b401c::virt_meth_0x5101b0(cls_0x5b401c *this)

{
  (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
  this->mbr_0xe0 = 0;
  this->mbr_0x184 = 0x8000;
  this->mbr_0x188 = 0x100000;
  this->mbr_0x18c = 1;
  return;
}



// Function at 00510220

void __thiscall OOAnalyzer::cls_0x5b401c::virt_meth_0x510220(cls_0x5b401c *this)

{
  short sVar1;
  byte bVar2;
  undefined uVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  int iVar4;
  undefined4 uVar5;
  cls_0x5a7b98_TCharacter *this_00;
  dword dVar6;
  dword *pdVar7;
  undefined4 uVar8;
  cls_0x44ceb0 cStack_48;
  
  bVar2 = (*this->vftptr_0x0[2].virt_meth_0x40e570_84)(this);
  sVar1 = *(short *)&this->mbr_0xc;
  if (sVar1 == 0) {
    if (this->mbr_0x18c != 0) {
      dVar6 = this->mbr_0x188;
      if (dVar6 != 0) {
        pdVar7 = &this->mbr_0x1c;
        uVar8 = 0;
        uVar5 = cls_0x5b8e94_TObjectInstance::meth_0x4df070((cls_0x5b8e94_TObjectInstance *)this);
        FUN_0046db20_ConvertToVector(uVar5,dVar6,pdVar7,uVar8);
        dVar6 = this->mbr_0x188;
        this->mbr_0x8 = this->mbr_0x8 & 0xfffffffe | 0x10008;
        this->mbr_0x24 = -((int)(dVar6 + ((int)dVar6 >> 0x1f & 0xfU)) >> 4);
        this->mbr_0x184 =
             (dword)(0x1e0 / (longlong)((int)(dVar6 + ((int)dVar6 >> 0x1f & 0xffffU)) >> 0x10));
      }
      (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
    }
  }
  else {
    if (sVar1 == 1) {
      dVar6 = this->mbr_0x184 - 1;
      this->mbr_0x184 = dVar6;
      if ((bVar2 & 2) == 0) {
        if (this->mbr_0xd8 == 0) {
          this_00 = (cls_0x5a7b98_TCharacter *)0x0;
        }
        else {
          this_00 = *(cls_0x5a7b98_TCharacter **)(this->mbr_0xd8 + 4);
        }
        cls_0x44ceb0::cls_0x44ceb0(&cStack_48,&this->mbr_0x10,0x100,0xe0,2,0);
        while ((cls_0x5a7b98_TCharacter *)cStack_48.mbr_0xc != (cls_0x5a7b98_TCharacter *)0x0) {
          if (((((cls_0x5a7b98_TCharacter *)cStack_48.mbr_0xc != this_00) &&
               (uVar3 = (*((cls_0x5a50e8 *)cStack_48.mbr_0xc)->vftptr_0x0->virt_meth_0x477e50_448)
                                  ((void *)cStack_48.mbr_0xc),
               CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var)) &&
              (uVar3 = (*this->vftptr_0x0->virt_meth_0x410aa0_4)(this),
              CONCAT31(extraout_var_00,uVar3) < 0x21)) &&
             ((this_00 == (cls_0x5a7b98_TCharacter *)0x0 ||
              (iVar4 = cls_0x5a7b98_TCharacter::meth_0x4c89c0_IsEnemy(this_00), iVar4 != 0)))) goto LAB_00510310;
          cls_0x44ceb0::meth_0x44d080((cls_0x44ceb0 *)&stack0xffffffb4);
          cStack_48.mbr_0xc = cStack_48.mbr_0x8;
        }
        if (0 < (int)dVar6) goto LAB_005103ac;
      }
LAB_00510310:
      this->mbr_0x8 = this->mbr_0x8 & 0xfffefff7 | 1;
      (*this->vftptr_0x0->virt_meth_0x5104c0_24)(this);
      cls_0x5b8e94_TObjectInstance::virt_meth_0x4de800((cls_0x5b8e94_TObjectInstance *)this);
      return;
    }
    if ((sVar1 == 2) && (this->mbr_0x58 == 0)) {
      cls_0x5b8e94_TObjectInstance::meth_0x4defe0((cls_0x5b8e94_TObjectInstance *)this);
      cls_0x5b8e94_TObjectInstance::virt_meth_0x4de800((cls_0x5b8e94_TObjectInstance *)this);
      return;
    }
  }
LAB_005103ac:
  cls_0x5b8e94_TObjectInstance::virt_meth_0x4de800((cls_0x5b8e94_TObjectInstance *)this);
  return;
}



// Function at 00514a50

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b401c::virt_meth_0x514a50(cls_0x5b401c *this)

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

cls_0x5b401c * __thiscall OOAnalyzer::cls_0x5b401c::virt_meth_0x5152e0(cls_0x5b401c *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005153a0

cls_0x5b401c * __thiscall OOAnalyzer::cls_0x5b401c::virt_meth_0x5153a0(cls_0x5b401c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b401c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005153c0

cls_0x5b401c * __thiscall OOAnalyzer::cls_0x5b401c::~cls_0x5b401c(cls_0x5b401c *this)

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
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b401c *)pcVar1;
}



