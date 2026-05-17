// Decompiled methods and structure for class: cls_0x5a5d64

/*
/OOAnalyzer/cls_0x5a5d64
pack(disabled)
Structure cls_0x5a5d64 {
   0   cls_0x5a5d64::vftable_5a5d64 *   4   vftptr_0x0   "pointer to cls_0x5a5d64::vftable_5a5d64"
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
   404   word   2   mbr_0x194   "Unsigned Word (dw, 2-bytes)"
   406   word   2   mbr_0x196   "Unsigned Word (dw, 2-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 440 Alignment: 1

*/

// Function at 00488ef0

cls_0x5a5d64 * __thiscall OOAnalyzer::cls_0x5a5d64::cls_0x5a5d64(cls_0x5a5d64 *this)

{
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d79b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x8 = 0x12e;
  this->mbr_0x18 = 0x12e;
  this->mbr_0xc = 0x1c4;
  this->mbr_0x1c = 0x1c4;
  this->vftptr_0x0 = (cls_0x5a5d64__vftable_5a5d64 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x10 = 0xb2;
  this->mbr_0x20 = 0xb2;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x194 = 0;
  this->mbr_0x196 = 0;
  this->mbr_0x198 = 0;
  this->mbr_0x190 = 0;
  this->mbr_0x1ac = 0;
  this->vftptr_0x0 = &cls_0x5a5d64__vftable_5a5d64_005a5d64;
  ExceptionList = local_c;
  return this;
}



// Function at 00488f90

cls_0x5a5d64 * __thiscall OOAnalyzer::cls_0x5a5d64::~cls_0x5a5d64(cls_0x5a5d64 *this)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  cls_0x5a5d64 *pcVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d7d4;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a5d64__vftable_5a5d64_005a5d64;
  local_4 = 1;
  virt_meth_0x52f6f0(this);
  dVar1 = this->mbr_0x198;
  if (dVar1 != 0) {
    iVar2 = FUN_0058cff3(dVar1);
    DAT_0065ba08 = DAT_0065ba08 - iVar2;
    FUN_0058cfab(dVar1);
  }
  this->vftptr_0x0 = (cls_0x5a5d64__vftable_5a5d64 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 2;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  iVar2 = *(int *)&this->field_0x98;
  if (iVar2 != 0) {
    iVar3 = FUN_0058cff3(iVar2);
    DAT_0065ba08 = DAT_0065ba08 - iVar3;
  }
  pcVar4 = (cls_0x5a5d64 *)FUN_0058cfab(iVar2);
  ExceptionList = local_c;
  return pcVar4;
}



// Function at 00489070

cls_0x5a5d64 * __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x489070(cls_0x5a5d64 *this)

{
  int iVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5d64(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5d64 *)0x0) {
      iVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - iVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 0052f390

undefined4 __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x52f390(cls_0x5a5d64 *this)

{
  dword dVar1;
  cls_0x5a3c68 *pcVar2;
  int *piVar3;
  cls_0x5a68d8 *this_00;
  void *unaff_ESI;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a194f;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x1b0 = 1;
  this->mbr_0x180 = 0;
  this->mbr_0x188 = 0xffffffff;
  this->mbr_0x18c = 0xffffffff;
  if ((this->mbr_0x1ac != 0) ||
     (cls_0x5a4494_TPane::virt_meth_0x434e40((cls_0x5a4494_TPane *)this), this->mbr_0x190 != 0)) {
    ExceptionList = local_c;
    return 1;
  }
  dVar1 = FUN_0047f670(s_buysell_dat_005e3d2c,0xffffffff,0);
  this->mbr_0x190 = dVar1;
  this->mbr_0x184 = 3;
  this->mbr_0x1a0 = 0;
  this->mbr_0x1a4 = 0;
  this->mbr_0x1a8 = 0;
  this->mbr_0x1b4 = 0;
  if (dVar1 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  pcVar2 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 0;
  if (pcVar2 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar2,this->mbr_0x190,s_BuySellUp_005e3d38,0x26,meth_0x52fe90,0,0,0,0x10,0xffffffff)
    ;
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  pcVar2 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 1;
  if (pcVar2 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar2,this->mbr_0x190,s_BuySellDown_005e3d44,0x28,meth_0x52fee0,0,0,0,0x10,
               0xffffffff);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  pcVar2 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 2;
  if (pcVar2 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar2,this->mbr_0x190,s_BuySellActivate_005e3d50,0x41,cls_0x5b93c4::meth_0x52ff40,0,
               0,0,0x10,0xffffffff);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  pcVar2 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 3;
  if (pcVar2 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar2,this->mbr_0x190,s_BuySellExit_005e3d60,0x45,&LAB_00530570,0,0,0,0x10,
               0xffffffff);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  piVar3 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
  piVar3 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
  piVar3 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
  piVar3 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  (**(code **)(*piVar3 + 0x1c))(piVar3[5] & 0xffffffdf);
  cls_0x46d6b0::meth_0x46d710((cls_0x46d6b0 *)this->mbr_0x190);
  this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  if (this_00 == (cls_0x5a68d8 *)0x0) {
    this->mbr_0x1ac = 1;
    this->mbr_0x19c = 0;
    ExceptionList = unaff_ESI;
    return 1;
  }
  cls_0x5a68d8::cls_0x5a68d8(this_00);
  this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
  this_00->mbr_0x68 = 0;
  cls_0x5a68d8::meth_0x4a5740(this_00,400,0x18);
  this_00->mbr_0x70 = 1;
  this->mbr_0x19c = (dword)this_00;
  this->mbr_0x1ac = 1;
  ExceptionList = unaff_ESI;
  return 1;
}



// Function at 0052f6f0

void __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x52f6f0(cls_0x5a5d64 *this)

{
  if (this->mbr_0x190 != 0) {
    FUN_00482f80(this->mbr_0x190);
    this->mbr_0x190 = 0;
  }
  if ((undefined4 *)this->mbr_0x19c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x19c)(1);
    this->mbr_0x19c = 0;
  }
  if (this->mbr_0x1a4 != 0) {
    FUN_004830f0(this->mbr_0x1a4);
    this->mbr_0x1a4 = 0;
  }
  if (this->mbr_0x1a8 != 0) {
    FUN_004830f0(this->mbr_0x1a8);
    this->mbr_0x1a8 = 0;
  }
  cls_0x5a5320_TPlayScreen::meth_0x48ed60((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ef30((cls_0x5a4f28 *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)&DAT_0065caf0);
  UNK_0065cb3c._4_4_ = 1;
  this->mbr_0x1ac = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 0052f7a0

void __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x52f7a0(cls_0x5a5d64 *this)

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x52f7d0_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 0052fa60

void __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x52fa60(cls_0x5a5d64 *this,int param_1)

{
  dword dVar1;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 0) goto code_r0x0052fac5;
  if (param_1 == 0x31) {
    dVar1 = this->mbr_0x180;
LAB_0052fa98:
    this->mbr_0x188 = dVar1;
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  else {
    if (param_1 == 0x32) {
      dVar1 = this->mbr_0x180 + 1;
      goto LAB_0052fa98;
    }
    if (param_1 == 0x33) {
      dVar1 = this->mbr_0x180 + 2;
      goto LAB_0052fa98;
    }
  }
  if ((int)(short)this->mbr_0x194 <= (int)this->mbr_0x188) {
    this->mbr_0x188 = (int)(short)this->mbr_0x194 - 1;
  }
code_r0x0052fac5:
  switch(param_1) {
  case 0x42:
  case 0x56:
  case 0x62:
  case 0x76:
    break;
  default:
    cls_0x5b93c4::virt_meth_0x4361f0((cls_0x5b93c4 *)this,param_1);
  }
  return;
}



// Function at 0052fd50

void __thiscall
OOAnalyzer::cls_0x5a5d64::virt_meth_0x52fd50(cls_0x5a5d64 *this,int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int in_stack_0000000c;
  
  cls_0x5b93c4::virt_meth_0x436530((cls_0x5b93c4 *)this,param_1,param_2);
  if (((param_1 != 4) && (param_1 != 5)) || (199 < param_2)) goto LAB_0052fe7c;
  if ((in_stack_0000000c < 0x2b) || (0x55 < in_stack_0000000c)) {
    if ((in_stack_0000000c < 0x57) || (0x81 < in_stack_0000000c)) {
      if ((0x82 < in_stack_0000000c) && (in_stack_0000000c < 0xae)) {
        this->mbr_0x188 = this->mbr_0x180 + 2;
        iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
        if (-1 < iVar1) {
          iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
          goto joined_r0x0052fe07;
        }
      }
    }
    else {
      this->mbr_0x188 = this->mbr_0x180 + 1;
      iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      if (-1 < iVar1) {
        iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
        goto joined_r0x0052fe07;
      }
    }
  }
  else {
    this->mbr_0x188 = this->mbr_0x180;
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (-1 < iVar1) {
      iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
joined_r0x0052fe07:
      if (iVar2 != 0) {
        cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
      }
    }
  }
  if ((int)(short)this->mbr_0x194 <= (int)this->mbr_0x188) {
    this->mbr_0x188 = (int)(short)this->mbr_0x194 - 1;
  }
LAB_0052fe7c:
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return;
}



// Function at 00530600

void __thiscall OOAnalyzer::cls_0x5a5d64::virt_meth_0x530600(cls_0x5a5d64 *this)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < (short)this->mbr_0x194) {
    do {
      FUN_0052f310();
      iVar1 = iVar1 + 1;
    } while (iVar1 < (short)this->mbr_0x194);
  }
  if (this->mbr_0x198 != 0) {
    FUN_004830f0(this->mbr_0x198);
  }
  this->mbr_0x198 = 0;
  this->mbr_0x194 = 0;
  this->mbr_0x196 = 0;
  this->mbr_0x1b0 = 0;
  this->mbr_0x48 = 1;
  this->mbr_0x4c = 1;
  return;
}



// Function at 00530670

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x530670(cls_0x5a5d64 *this,undefined4 param_1)

{
  word *pwVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  dword dVar6;
  int iVar7;
  int iVar8;
  cls_0x4746d0 *pcVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  undefined4 in_stack_00000008;
  undefined4 local_48 [18];
  
  if ((this->mbr_0x17c & 0x10) != 0) {
    pcVar9 = (cls_0x4746d0 *)(-(uint)(2 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a150);
    uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
    if (uVar2 == 0xffffffff) {
      pcVar9 = (cls_0x4746d0 *)(-(uint)(1 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a14c);
      uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
      if (uVar2 == 0xffffffff) {
        pcVar9 = (cls_0x4746d0 *)(-(uint)(4 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a158);
        uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
        if (uVar2 == 0xffffffff) {
          pcVar9 = (cls_0x4746d0 *)(-(uint)(0x12 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a190);
          uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
          if (uVar2 == 0xffffffff) {
            pcVar9 = (cls_0x4746d0 *)(-(uint)(5 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a15c);
            uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
            if (uVar2 == 0xffffffff) {
              pcVar9 = (cls_0x4746d0 *)(-(uint)(0x11 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a18c);
              uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
              if (uVar2 == 0xffffffff) {
                pcVar9 = (cls_0x4746d0 *)(-(uint)(0x15 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a19c);
                uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
                if (uVar2 == 0xffffffff) {
                  return;
                }
              }
            }
          }
        }
      }
    }
    uVar3 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar9->mbr_0x14);
    if (uVar2 < pcVar9->mbr_0x24) {
      dVar6 = *(dword *)(pcVar9->mbr_0x34 + uVar2 * 4);
      if (dVar6 == 0) {
        dVar6 = pcVar9->mbr_0x38;
      }
      if (uVar3 < (uint)(int)*(short *)(dVar6 + 0xc)) {
        dVar6 = *(dword *)(pcVar9->mbr_0x34 + uVar2 * 4);
        if (dVar6 == 0) {
          dVar6 = pcVar9->mbr_0x38;
        }
        iVar4 = *(int *)(*(int *)(dVar6 + 0x10) + uVar3 * 4);
        if (iVar4 != 0) {
          if (iVar4 != 1) {
            return;
          }
          iVar4 = cls_0x48e630::meth_0x48e630((cls_0x48e630 *)&DAT_0065a618,pcVar9->mbr_0x8);
          if (iVar4 == 0) {
            return;
          }
          iVar4 = FUN_0052da90(param_1,this->mbr_0x17c,in_stack_00000008);
          if (iVar4 == 0) {
            return;
          }
          pwVar1 = &this->mbr_0x194;
          if ((short)this->mbr_0x196 <= (short)this->mbr_0x194) {
            cls_0x533280::meth_0x533280((cls_0x533280 *)pwVar1);
          }
          puVar10 = local_48;
          puVar5 = (undefined4 *)(this->mbr_0x198 + (short)*pwVar1 * 0x48);
          for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
            *puVar5 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar5 = puVar5 + 1;
          }
          *pwVar1 = *pwVar1 + 1;
          return;
        }
      }
    }
    iVar4 = FUN_0052da90(param_1,this->mbr_0x17c,in_stack_00000008);
    if (iVar4 == 0) {
      return;
    }
    pwVar1 = &this->mbr_0x194;
    if ((short)this->mbr_0x196 <= (short)this->mbr_0x194) {
      cls_0x533280::meth_0x533280((cls_0x533280 *)pwVar1);
    }
    puVar10 = local_48;
    puVar5 = (undefined4 *)(this->mbr_0x198 + (short)*pwVar1 * 0x48);
    for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar5 = puVar5 + 1;
    }
    *pwVar1 = *pwVar1 + 1;
    return;
  }
  uVar2 = ((this->mbr_0x17c & 4) != 0) + 1;
  if (uVar2 < DAT_0065a258_TObjectClass_numclasses) {
    pcVar9 = (cls_0x4746d0 *)(&DAT_0065a148_TObjectClass_classes)[uVar2];
  }
  else {
    pcVar9 = (cls_0x4746d0 *)0x0;
  }
  uVar2 = cls_0x4746d0::meth_0x475210(pcVar9,param_1);
  if (uVar2 == 0xffffffff) {
    return;
  }
  uVar3 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar9->mbr_0x14);
  if (uVar2 < pcVar9->mbr_0x24) {
    dVar6 = *(dword *)(pcVar9->mbr_0x34 + uVar2 * 4);
    if (dVar6 == 0) {
      dVar6 = pcVar9->mbr_0x38;
    }
    if ((uint)(int)*(short *)(dVar6 + 0xc) <= uVar3) goto LAB_00530942;
    dVar6 = *(dword *)(pcVar9->mbr_0x34 + uVar2 * 4);
    if (dVar6 == 0) {
      dVar6 = pcVar9->mbr_0x38;
    }
    iVar4 = *(int *)(*(int *)(dVar6 + 0x10) + uVar3 * 4);
    if (iVar4 == 0) goto LAB_00530942;
    if (iVar4 != 1) {
      return;
    }
    iVar4 = cls_0x48e630::meth_0x48e630((cls_0x48e630 *)&DAT_0065a618,pcVar9->mbr_0x8);
    if (iVar4 == 0) {
      return;
    }
    iVar4 = FUN_0052da90(param_1,this->mbr_0x17c,in_stack_00000008);
    if (iVar4 == 0) {
      return;
    }
    if ((short)this->mbr_0x194 < (short)this->mbr_0x196) goto LAB_00530ab4;
    iVar8 = (short)this->mbr_0x196 + 4;
    puVar5 = (undefined4 *)FUN_00482fb0(iVar8 * 0x48);
    iVar4 = (int)(short)this->mbr_0x196;
    puVar10 = puVar5 + iVar4 * 0x12;
    for (uVar2 = (uint)((iVar8 - iVar4) * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined *)puVar10 = 0;
      puVar10 = (undefined4 *)((int)puVar10 + 1);
    }
    puVar10 = (undefined4 *)this->mbr_0x198;
    if (puVar10 != (undefined4 *)0x0) {
      puVar11 = puVar10;
      puVar12 = puVar5;
      for (uVar2 = (uint)(iVar4 * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
        *puVar12 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar12 = puVar12 + 1;
      }
      for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(undefined *)puVar12 = *(undefined *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
        puVar12 = (undefined4 *)((int)puVar12 + 1);
      }
      FUN_004830f0(puVar10);
    }
  }
  else {
LAB_00530942:
    iVar4 = FUN_0052da90(param_1,this->mbr_0x17c,in_stack_00000008);
    if (iVar4 == 0) {
      return;
    }
    if ((short)this->mbr_0x194 < (short)this->mbr_0x196) goto LAB_00530ab4;
    iVar8 = (short)this->mbr_0x196 + 4;
    puVar5 = (undefined4 *)FUN_00482fb0(iVar8 * 0x48);
    iVar4 = (int)(short)this->mbr_0x196;
    puVar10 = puVar5 + iVar4 * 0x12;
    for (uVar2 = (uint)((iVar8 - iVar4) * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined *)puVar10 = 0;
      puVar10 = (undefined4 *)((int)puVar10 + 1);
    }
    puVar10 = (undefined4 *)this->mbr_0x198;
    if (puVar10 != (undefined4 *)0x0) {
      puVar11 = puVar10;
      puVar12 = puVar5;
      for (uVar2 = (uint)(iVar4 * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
        *puVar12 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar12 = puVar12 + 1;
      }
      for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(undefined *)puVar12 = *(undefined *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
        puVar12 = (undefined4 *)((int)puVar12 + 1);
      }
      FUN_004830f0(puVar10);
    }
  }
  this->mbr_0x198 = (dword)puVar5;
  this->mbr_0x196 = (word)iVar8;
LAB_00530ab4:
  puVar10 = local_48;
  puVar5 = (undefined4 *)(this->mbr_0x198 + (short)this->mbr_0x194 * 0x48);
  for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar5 = puVar5 + 1;
  }
  this->mbr_0x194 = this->mbr_0x194 + 1;
  return;
}



// Function at 00531d70

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x531d70(cls_0x5a5d64 *this)

{
  byte bVar1;
  int *piVar2;
  byte *pbVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  byte *pbVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  bool bVar14;
  byte *in_stack_00000004;
  cls_0x477870 local_64;
  undefined4 auStack_48 [18];
  
  local_64.mbr_0x4 = this->mbr_0x1b4;
  if (local_64.mbr_0x4 != 0) {
    local_64.mbr_0x0 = 1;
    local_64.mbr_0x10 = 0;
    local_64.mbr_0xc = 0;
    local_64.mbr_0x14 = 0;
    local_64.mbr_0x18 = 0;
    local_64.mbr_0x8 = local_64.mbr_0x4;
    cls_0x477870::meth_0x46dfb0(&local_64);
    piVar2 = (int *)local_64.mbr_0x18;
    while (piVar2 != (int *)0x0) {
      pbVar10 = (byte *)piVar2[0xe];
      pbVar3 = in_stack_00000004;
      do {
        bVar1 = *pbVar3;
        bVar14 = bVar1 < *pbVar10;
        if (bVar1 != *pbVar10) {
LAB_00531def:
          iVar4 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
          goto LAB_00531df4;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar3[1];
        bVar14 = bVar1 < pbVar10[1];
        if (bVar1 != pbVar10[1]) goto LAB_00531def;
        pbVar3 = pbVar3 + 2;
        pbVar10 = pbVar10 + 2;
      } while (bVar1 != 0);
      iVar4 = 0;
LAB_00531df4:
      local_64.mbr_0x18 = (dword)piVar2;
      if ((((iVar4 == 0) && (*(short *)(piVar2 + 0x1f) < 0x100)) &&
          (iVar4 = (**(code **)(*piVar2 + 0xd4))(s_SaleType_005e3e80), iVar4 != 2)) &&
         (iVar4 = (**(code **)(*piVar2 + 400))(), iVar4 != -1)) {
        switch(*(undefined2 *)(piVar2 + 1)) {
        case 1:
          bVar1 = *(byte *)&this->mbr_0x17c & 8;
          break;
        case 2:
          if (((this->mbr_0x17c & 4) == 0) && ((this->mbr_0x17c & 0x10) == 0))
          goto switchD_00531e4c_caseD_3;
          goto LAB_00531e83;
        default:
          goto switchD_00531e4c_caseD_3;
        case 4:
        case 5:
        case 0x11:
        case 0x12:
        case 0x15:
          bVar1 = *(byte *)&this->mbr_0x17c & 0x10;
        }
        if (bVar1 != 0) {
LAB_00531e83:
          iVar4 = piVar2[0xe];
          uVar5 = (**(code **)(*piVar2 + 0x198))();
          iVar4 = FUN_0052da90(iVar4,this->mbr_0x17c,uVar5);
          if (iVar4 != 0) {
            if ((short)this->mbr_0x196 <= (short)this->mbr_0x194) {
              iVar9 = (short)this->mbr_0x196 + 4;
              puVar6 = (undefined4 *)FUN_00482fb0(iVar9 * 0x48);
              iVar4 = (int)(short)this->mbr_0x196;
              puVar12 = puVar6 + iVar4 * 0x12;
              for (uVar7 = (uint)((iVar9 - iVar4) * 0x48) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
                *puVar12 = 0;
                puVar12 = puVar12 + 1;
              }
              for (iVar8 = 0; iVar8 != 0; iVar8 = iVar8 + -1) {
                *(undefined *)puVar12 = 0;
                puVar12 = (undefined4 *)((int)puVar12 + 1);
              }
              puVar12 = (undefined4 *)this->mbr_0x198;
              if (puVar12 != (undefined4 *)0x0) {
                puVar11 = puVar12;
                puVar13 = puVar6;
                for (uVar7 = (uint)(iVar4 * 0x48) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
                  *puVar13 = *puVar11;
                  puVar11 = puVar11 + 1;
                  puVar13 = puVar13 + 1;
                }
                for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
                  *(undefined *)puVar13 = *(undefined *)puVar11;
                  puVar11 = (undefined4 *)((int)puVar11 + 1);
                  puVar13 = (undefined4 *)((int)puVar13 + 1);
                }
                FUN_004830f0(puVar12);
              }
              this->mbr_0x198 = (dword)puVar6;
              this->mbr_0x196 = (word)iVar9;
            }
            puVar12 = auStack_48;
            puVar6 = (undefined4 *)(this->mbr_0x198 + (short)this->mbr_0x194 * 0x48);
            for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
              *puVar6 = *puVar12;
              puVar12 = puVar12 + 1;
              puVar6 = puVar6 + 1;
            }
            this->mbr_0x194 = this->mbr_0x194 + 1;
          }
        }
      }
switchD_00531e4c_caseD_3:
      cls_0x477870::meth_0x46dfb0(&local_64);
      piVar2 = (int *)local_64.mbr_0x18;
    }
  }
  return;
}



// Function at 00532fb0

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x532fb0(cls_0x5a5d64 *this,undefined4 param_1)

{
  dword dVar1;
  undefined4 in_stack_00000008;
  
  dVar1 = cls_0x5a5320_TPlayScreen::meth_0x451fe0((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,param_1,in_stack_00000008,0);
  this->mbr_0x1a0 = dVar1;
  return;
}



// Function at 00532fe0

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x532fe0(cls_0x5a5d64 *this)

{
  char cVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  undefined4 *puVar7;
  char *in_stack_00000004;
  
  if (this->mbr_0x1a4 != 0) {
    FUN_004830f0(this->mbr_0x1a4);
  }
  uVar3 = 0xffffffff;
  pcVar6 = in_stack_00000004;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  puVar2 = (undefined4 *)FUN_00482ef0(~uVar3);
  uVar3 = 0xffffffff;
  do {
    pcVar6 = in_stack_00000004;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = in_stack_00000004 + 1;
    cVar1 = *in_stack_00000004;
    in_stack_00000004 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  puVar5 = (undefined4 *)(pcVar6 + -uVar3);
  puVar7 = puVar2;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar7 = puVar7 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar7 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  }
  this->mbr_0x1a4 = (dword)puVar2;
  return;
}



// Function at 00533040

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x533040(cls_0x5a5d64 *this)

{
  char cVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  undefined4 *puVar7;
  char *in_stack_00000004;
  
  if (this->mbr_0x1a8 != 0) {
    FUN_004830f0(this->mbr_0x1a8);
  }
  uVar3 = 0xffffffff;
  pcVar6 = in_stack_00000004;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  puVar2 = (undefined4 *)FUN_00482ef0(~uVar3);
  uVar3 = 0xffffffff;
  do {
    pcVar6 = in_stack_00000004;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = in_stack_00000004 + 1;
    cVar1 = *in_stack_00000004;
    in_stack_00000004 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  puVar5 = (undefined4 *)(pcVar6 + -uVar3);
  puVar7 = puVar2;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar7 = puVar7 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar7 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  }
  this->mbr_0x1a8 = (dword)puVar2;
  return;
}



// Function at 005883e0

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x5883e0(cls_0x5a5d64 *this)

{
  dword in_stack_00000004;
  
  this->mbr_0x17c = in_stack_00000004;
  return;
}



// Function at 005883f0

void __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x5883f0(cls_0x5a5d64 *this)

{
  dword in_stack_00000004;
  
  this->mbr_0x1b4 = in_stack_00000004;
  return;
}



// Function at 00588400

uint __thiscall OOAnalyzer::cls_0x5a5d64::meth_0x588400(cls_0x5a5d64 *this)

{
  return this->mbr_0x17c & 2;
}



