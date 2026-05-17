// Decompiled methods and structure for class: cls_0x5a3d44

/*
/OOAnalyzer/cls_0x5a3d44
pack(disabled)
Structure cls_0x5a3d44 {
   0   cls_0x5a3ab8   144   cls_0x5a3ab8   "Component (member) class."
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   204   dword   4   mbr_0xcc   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 336 Alignment: 1

*/

// Function at 0042fe80

cls_0x5a3d44 * __thiscall
OOAnalyzer::cls_0x5a3d44::cls_0x5a3d44
          (cls_0x5a3d44 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined4 param_7,
          cls_0x5a3cd8 *param_8,undefined4 param_9,dword *param_10,dword param_11)

{
  dword dVar1;
  uint uVar2;
  cls_0x5a3cd8 *pcVar3;
  int iVar4;
  dword *pdVar5;
  dword *pdVar6;
  dword in_stack_00000030;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cac3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,param_1,6,param_6,param_7,param_8,0,param_2,param_3,param_4,param_5
             ,(short)param_9,0,0xffffffff,0,0,0,0xffffffff);
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3d44__vftable_5a3d44_005a3d44;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  this->mbr_0xa0 = 0xffffffff;
  pdVar5 = param_10;
  if (param_10 == (dword *)0x0) {
    uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
    if (((uVar2 & 0x10000) == 0) || ((uVar2 & 0x40000) != 0)) {
      if (((uVar2 & 0x20000) == 0) || ((uVar2 & 0x40000) != 0)) {
        pdVar5 = &DAT_006559c0;
      }
      else {
        pdVar5 = &DAT_00655c40;
      }
    }
    else {
      pdVar5 = &DAT_00655b48;
    }
  }
  pdVar6 = &this->mbr_0xd8;
  for (iVar4 = 0x3e; iVar4 != 0; iVar4 = iVar4 + -1) {
    *pdVar6 = *pdVar5;
    pdVar5 = pdVar5 + 1;
    pdVar6 = pdVar6 + 1;
  }
  pdVar5 = (dword *)this->mbr_0xd8;
  if (pdVar5 != (dword *)0x0) {
    if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
      (this->cls_0x5a3ab8).mbr_0x60 = -pdVar5[2];
      (this->cls_0x5a3ab8).mbr_0x64 = -pdVar5[3];
    }
    if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
      dVar1 = pdVar5[1];
      (this->cls_0x5a3ab8).mbr_0x68 = *pdVar5;
      (this->cls_0x5a3ab8).mbr_0x6c = dVar1;
    }
  }
  this->mbr_0x90 = param_11;
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  this->mbr_0x94 = in_stack_00000030;
  if (param_11 == 0) {
    uVar2 = uVar2 & 0xfff7ffff;
  }
  else {
    uVar2 = uVar2 | 0x80000;
  }
  (this->cls_0x5a3ab8).mbr_0x14 = uVar2;
  if (this->mbr_0x134 != 0) {
    meth_0x4304a0(this,&param_2,&param_3,&param_4,&param_5);
    param_8 = (cls_0x5a3cd8 *)FUN_00482fb0(0x168);
    local_4._0_1_ = 1;
    if (param_8 == (cls_0x5a3cd8 *)0x0) {
      pcVar3 = (cls_0x5a3cd8 *)0x0;
    }
    else {
      pcVar3 = cls_0x5a3cd8::cls_0x5a3cd8
                         (param_8,param_1,param_2,param_3,param_4,param_5,0,0,0xffffffff,param_9);
    }
    local_4 = (uint)local_4._1_3_ << 8;
    this->mbr_0xd4 = (dword)pcVar3;
    (pcVar3->cls_0x5a3ab8).mbr_0x78 = (dword)this;
  }
  (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
  meth_0x4308b0(this);
  ExceptionList = local_c;
  return this;
}



// Function at 00430090

cls_0x5a3d44 * __thiscall
OOAnalyzer::cls_0x5a3d44::cls_0x5a3d44(cls_0x5a3d44 *this,int *param_1,cls_0x41c7f0 *param_2)

{
  uint uVar1;
  dword *pdVar2;
  dword dVar3;
  int *piVar4;
  undefined *puVar5;
  undefined4 *puVar6;
  int iVar7;
  cls_0x41c7f0 *this_00;
  cls_0x5a3cd8 *pcVar8;
  undefined4 *in_stack_0000000c;
  undefined4 local_1c;
  undefined4 local_18;
  cls_0x5a3d44 *local_14;
  cls_0x5a3cd8 *local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puVar5 = (undefined *)param_2;
  piVar4 = param_1;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059caeb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  local_14 = this;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,6);
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3d44__vftable_5a3d44_005a3d44;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  this->mbr_0xa0 = 0xffffffff;
  if ((uVar1 & 1) == 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 1;
    puVar6 = in_stack_0000000c;
    if (in_stack_0000000c == (undefined4 *)0x0) {
      if (((uVar1 & 0x10000) == 0) || ((uVar1 & 0x40000) != 0)) {
        if (((uVar1 & 0x20000) == 0) || (puVar6 = &DAT_00655c40, (uVar1 & 0x40000) != 0)) {
          puVar6 = &DAT_006559c0;
        }
      }
      else {
        puVar6 = &DAT_00655b48;
      }
    }
    iVar7 = cls_0x42f9f0::meth_0x42f9f0_DefWidget_LISTBOX_attr_parser((cls_0x42f9f0 *)&this->mbr_0xd8,puVar6,piVar4);
    if (iVar7 != 0) {
      pdVar2 = (dword *)this->mbr_0xd8;
      if (pdVar2 != (dword *)0x0) {
        if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
          (this->cls_0x5a3ab8).mbr_0x60 = -pdVar2[2];
          (this->cls_0x5a3ab8).mbr_0x64 = -pdVar2[3];
        }
        if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
          dVar3 = pdVar2[1];
          (this->cls_0x5a3ab8).mbr_0x68 = *pdVar2;
          (this->cls_0x5a3ab8).mbr_0x6c = dVar3;
        }
      }
      if (((this->cls_0x5a3ab8).mbr_0x14 & 0x80000) == 0) {
        this->mbr_0x90 = 0;
        this->mbr_0x94 = 0;
      }
      else {
        if (*(int *)(puVar5 + 0x10) != 9) {
          (**(code **)(*piVar4 + 0x98))();
          ExceptionList = local_c;
          return this;
        }
        FUN_004795a0();
        this_00 = (cls_0x41c7f0 *)FUN_00482fb0();
        local_4._0_1_ = 1;
        if (this_00 == (cls_0x41c7f0 *)0x0) {
          this_00 = (cls_0x41c7f0 *)0x0;
        }
        else {
          param_2 = this_00;
          cls_0x41c7f0::cls_0x41c7f0(this_00,0x40);
        }
        param_2 = (cls_0x41c7f0 *)&stack0xffffffc8;
        local_4 = (uint)local_4._1_3_ << 8;
        this->mbr_0x90 = (dword)this_00;
        this->mbr_0x94 = 1;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffc8);
        iVar7 = FUN_0042f700(this->mbr_0x90,piVar4,puVar5,this->mbr_0x124);
        if (iVar7 == 0) {
          ExceptionList = local_c;
          return this;
        }
      }
      if (this->mbr_0x134 != 0) {
        meth_0x4304a0(this,&local_18,&local_1c,&stack0x0000000c,&param_1);
        local_10 = (cls_0x5a3cd8 *)FUN_00482fb0();
        local_4._0_1_ = 2;
        if (local_10 == (cls_0x5a3cd8 *)0x0) {
          pcVar8 = (cls_0x5a3cd8 *)0x0;
        }
        else {
          pcVar8 = cls_0x5a3cd8::cls_0x5a3cd8
                             (local_10,piVar4,local_18,local_1c,in_stack_0000000c,param_1,0,0,
                              0xffffffff,param_2);
        }
        local_4 = (uint)local_4._1_3_ << 8;
        this->mbr_0xd4 = (dword)pcVar8;
        (pcVar8->cls_0x5a3ab8).mbr_0x78 = (dword)this;
      }
      (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
      meth_0x4308b0(this);
    }
  }
  ExceptionList = local_c;
  return this;
}



// Function at 004302e0

cls_0x5a3d44 * __thiscall OOAnalyzer::cls_0x5a3d44::~cls_0x5a3d44(cls_0x5a3d44 *this)

{
  cls_0x45f7c0 *this_00;
  int *piVar1;
  int iVar2;
  dword dVar3;
  cls_0x5a3d44 *pcVar4;
  int iVar5;
  int iVar6;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059cafd;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3d44__vftable_5a3d44_005a3d44;
  this_00 = (cls_0x45f7c0 *)this->mbr_0x90;
  iVar5 = 0;
  local_4 = 0;
  if ((this_00 != (cls_0x45f7c0 *)0x0) && (this->mbr_0x94 != 0)) {
    if (0 < (int)(this_00->cls_0x41c7f0).mbr_0x0) {
      do {
        if ((-1 < iVar5) &&
           (piVar1 = *(int **)((this_00->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), piVar1 != (int *)0x0))
        {
          iVar2 = *piVar1;
          if ((((iVar2 == 2) || (iVar2 == 1)) && (iVar6 = piVar1[9], iVar6 != 0)) ||
             ((iVar2 == 0 && (iVar6 = piVar1[3], iVar6 != 0)))) {
            FUN_00482f80(iVar6);
          }
          FUN_004830f0(piVar1);
        }
        cls_0x45f7c0::meth_0x41cb40(this_00);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
    }
    dVar3 = this->mbr_0x90;
    (this_00->cls_0x41c7f0).mbr_0x0 = 0;
    (this_00->cls_0x41c7f0).mbr_0x4 = 0;
    if (dVar3 != 0) {
      FUN_004830f0(*(undefined4 *)(dVar3 + 0x10));
      FUN_004830f0(dVar3);
    }
  }
  if ((undefined4 *)this->mbr_0xd4 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0xd4)(1);
  }
  pcVar4 = (cls_0x5a3d44 *)(this->cls_0x5a3ab8).mbr_0x5c;
  local_4 = 0xffffffff;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar4 != (cls_0x5a3d44 *)0x0) {
    pcVar4 = (cls_0x5a3d44 *)FUN_00482f80(pcVar4);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  ExceptionList = local_c;
  return pcVar4;
}



// Function at 004304a0

undefined4 __thiscall
OOAnalyzer::cls_0x5a3d44::meth_0x4304a0
          (cls_0x5a3d44 *this,int *param_1,int *param_2,int *param_3,int *param_4)

{
  uint uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  dword dVar6;
  uint *in_stack_00000014;
  
  *param_4 = 0;
  dVar3 = this->mbr_0x134;
  *param_3 = 0;
  *param_2 = 0;
  *param_1 = 0;
  *in_stack_00000014 = 0;
  if (dVar3 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if ((uVar1 & 0x10000) == 0) {
      dVar3 = this->mbr_0x138;
      dVar2 = this->mbr_0x148;
      *param_1 = (this->cls_0x5a3ab8).mbr_0x60 + dVar3;
      dVar4 = (this->cls_0x5a3ab8).mbr_0x64;
      if (dVar2 == 0) {
        dVar6 = this->mbr_0x13c;
      }
      else {
        dVar6 = (this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x13c;
      }
      iVar5 = dVar6 + dVar4;
      *param_2 = iVar5;
      *param_3 = ((this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x140) - dVar3;
      if (dVar2 == 0) {
        dVar3 = this->mbr_0x144;
      }
      else {
        dVar3 = (this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x144;
      }
      *param_4 = (dVar3 - iVar5) + dVar4;
      *in_stack_00000014 = 0x20000;
    }
    else {
      dVar3 = this->mbr_0x148;
      dVar2 = (this->cls_0x5a3ab8).mbr_0x60;
      if (dVar3 == 0) {
        dVar4 = this->mbr_0x138;
      }
      else {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x138;
      }
      dVar6 = (this->cls_0x5a3ab8).mbr_0x64;
      iVar5 = dVar4 + dVar2;
      *param_1 = iVar5;
      dVar4 = this->mbr_0x13c;
      *param_2 = dVar6 + dVar4;
      if (dVar3 == 0) {
        dVar3 = this->mbr_0x140;
      }
      else {
        dVar3 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x140;
      }
      *param_3 = (dVar3 - iVar5) + dVar2;
      *param_4 = ((this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x144) - dVar4;
      *in_stack_00000014 = 0x10000;
    }
    *in_stack_00000014 = *in_stack_00000014 | (uint)CONCAT11(4,(byte)uVar1 & 6);
    return 1;
  }
  return 1;
}



// Function at 004308b0

void __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x4308b0(cls_0x5a3d44 *this)

{
  int *piVar1;
  cls_0x5a3cd8 *this_00;
  dword dVar2;
  uint uVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8 [2];
  
  this->mbr_0xb8 = (this->cls_0x5a3ab8).mbr_0x60 + this->mbr_0x110;
  this->mbr_0xbc = (this->cls_0x5a3ab8).mbr_0x64 + this->mbr_0x114;
  dVar2 = ((this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x118) - this->mbr_0x110;
  dVar5 = (this->cls_0x5a3ab8).mbr_0x6c;
  this->mbr_0xc0 = dVar2;
  dVar4 = (dVar5 - this->mbr_0x11c) - this->mbr_0x114;
  this->mbr_0xc4 = dVar4;
  dVar5 = dVar2;
  if ((int)this->mbr_0x108 <= (int)dVar2) {
    dVar5 = this->mbr_0x108;
  }
  this->mbr_0xb0 = dVar5;
  dVar6 = this->mbr_0x10c;
  if ((int)dVar4 < (int)this->mbr_0x10c) {
    dVar6 = dVar4;
  }
  dVar2 = (int)dVar2 / (int)dVar5;
  this->mbr_0xb4 = dVar6;
  if ((int)dVar2 < 2) {
    dVar2 = 1;
  }
  this->mbr_0xac = dVar2;
  dVar4 = (int)dVar4 / (int)dVar6;
  if ((int)dVar4 < 2) {
    dVar4 = 1;
  }
  this->mbr_0xa8 = dVar4;
  this->mbr_0xc4 = dVar4 * dVar6;
  this->mbr_0xc0 = dVar2 * dVar5;
  this->mbr_0xa4 = dVar4 * dVar2;
  dVar5 = this->mbr_0x98 - dVar4 * dVar2;
  if ((int)dVar5 < (int)this->mbr_0x9c) {
    this->mbr_0x9c = dVar5;
  }
  if ((int)this->mbr_0x9c < 0) {
    this->mbr_0x9c = 0;
  }
  if ((int)this->mbr_0x98 < (int)this->mbr_0xa0) {
    this->mbr_0xa0 = 0xffffffff;
  }
  if (this->mbr_0xd4 != 0) {
    meth_0x4304a0(this,local_8,&local_c,&local_10,&local_14);
    (**(code **)(*(int *)this->mbr_0xd4 + 0x28))(local_8[0],local_c,local_10,local_14);
    (**(code **)(*(int *)this->mbr_0xd4 + 0x1c))(local_14);
    uVar3 = this->mbr_0x98 - this->mbr_0xa4;
    uVar3 = uVar3 & ((int)uVar3 < 1) - 1;
    piVar1 = (int *)this->mbr_0xd4;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    if (((piVar1[0x26] != 0) || (uVar3 != piVar1[0x27])) || ((*(byte *)(piVar1 + 5) & 0x20) != 0)) {
      piVar1[0x26] = 0;
      piVar1[0x27] = uVar3;
      if (piVar1[0x25] < 0) {
        piVar1[0x25] = 0;
      }
      if ((int)uVar3 < piVar1[0x25]) {
        piVar1[0x25] = uVar3;
      }
      (**(code **)(*piVar1 + 0x40))(*(undefined4 *)(piVar1[2] + 100));
    }
    this_00 = (cls_0x5a3cd8 *)this->mbr_0xd4;
    this_00->mbr_0xa0 = this->mbr_0xa4;
    cls_0x5a3cd8::meth_0x42e440(this_00);
  }
  (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  return;
}



// Function at 00430ab0

void __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x430ab0(cls_0x5a3d44 *this)

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  dword dVar5;
  dword in_stack_00000004;
  
  dVar1 = this->mbr_0x9c;
  if ((in_stack_00000004 != dVar1) || ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
    uVar4 = (this->cls_0x5a3ab8).mbr_0x14 & 0x10000;
    if (uVar4 == 0) {
      dVar5 = this->mbr_0xa8;
      iVar2 = (this->mbr_0x98 - this->mbr_0xa4) + -1 + dVar5;
    }
    else {
      dVar5 = this->mbr_0xac;
      iVar2 = (this->mbr_0x98 - this->mbr_0xa4) + -1 + dVar5;
    }
    uVar3 = (int)in_stack_00000004 / (int)dVar5;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    if (iVar2 / (int)dVar5 < (int)uVar3) {
      uVar3 = iVar2 / (int)dVar5;
    }
    if (uVar4 == 0) {
      dVar5 = this->mbr_0xa8 & uVar3;
    }
    else {
      dVar5 = this->mbr_0xac * uVar3;
    }
    this->mbr_0x9c = dVar5;
    if ((cls_0x5a3cd8 *)this->mbr_0xd4 != (cls_0x5a3cd8 *)0x0) {
      cls_0x5a3cd8::meth_0x42e440((cls_0x5a3cd8 *)this->mbr_0xd4);
    }
    meth_0x431000(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    if (dVar1 != dVar5) {
      cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
    }
  }
  return;
}



// Function at 00430b80

void __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x430b80(cls_0x5a3d44 *this)

{
  dword dVar1;
  dword dVar2;
  dword in_stack_00000004;
  
  dVar1 = this->mbr_0xa0;
  if (((in_stack_00000004 != dVar1) || ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) &&
     (((this->cls_0x5a3ab8).mbr_0x14 & 0x200000) == 0)) {
    this->mbr_0xa0 = in_stack_00000004;
    if (((int)this->mbr_0x98 < (int)in_stack_00000004) || ((int)in_stack_00000004 < 0)) {
      this->mbr_0xa0 = 0xffffffff;
    }
    dVar2 = this->mbr_0xa0;
    if ((dVar2 != 0xffffffff) &&
       (((int)dVar2 < (int)this->mbr_0x9c || ((int)(this->mbr_0xa4 + this->mbr_0x9c) <= (int)dVar2))
       )) {
      meth_0x430ab0(this);
    }
    meth_0x430db0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100),dVar1);
    meth_0x430db0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100),this->mbr_0xa0);
    if (dVar1 != this->mbr_0xa0) {
      cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
    }
  }
  return;
}



// Function at 00430c50

void __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x430c50(cls_0x5a3d44 *this)

{
  dword dVar1;
  int *piVar2;
  uint uVar3;
  dword in_stack_00000004;
  
  if ((in_stack_00000004 == this->mbr_0x98) &&
     ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) == 0)) {
    return;
  }
  this->mbr_0x98 = in_stack_00000004;
  if ((int)in_stack_00000004 <= (int)this->mbr_0xa0) {
    this->mbr_0xa0 = 0xffffffff;
  }
  dVar1 = this->mbr_0xa4;
  if ((int)(in_stack_00000004 - dVar1) < (int)this->mbr_0x9c) {
    this->mbr_0x9c = in_stack_00000004 - dVar1;
  }
  if ((int)this->mbr_0x9c < 0) {
    this->mbr_0x9c = 0;
  }
  piVar2 = (int *)this->mbr_0xd4;
  if (piVar2 == (int *)0x0) goto LAB_00430d97;
  if (((this->cls_0x5a3ab8).mbr_0x14 & 0x10000) == 0) {
    uVar3 = (int)((this->mbr_0xa8 - dVar1) + -1 + in_stack_00000004) / (int)this->mbr_0xa8;
    uVar3 = uVar3 & ((int)uVar3 < 0) - 1;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    if (((piVar2[0x26] == 0) && (uVar3 == piVar2[0x27])) && ((*(byte *)(piVar2 + 5) & 0x20) == 0))
    goto LAB_00430d97;
    piVar2[0x26] = 0;
    piVar2[0x27] = uVar3;
    if (piVar2[0x25] < 0) {
      piVar2[0x25] = 0;
    }
    if ((int)uVar3 < piVar2[0x25]) goto LAB_00430d85;
  }
  else {
    uVar3 = (int)((this->mbr_0xac - dVar1) + -1 + in_stack_00000004) / (int)this->mbr_0xac;
    uVar3 = uVar3 & ((int)uVar3 < 0) - 1;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    if (((piVar2[0x26] == 0) && (uVar3 == piVar2[0x27])) && ((*(byte *)(piVar2 + 5) & 0x20) == 0))
    goto LAB_00430d97;
    piVar2[0x26] = 0;
    piVar2[0x27] = uVar3;
    if (piVar2[0x25] < 0) {
      piVar2[0x25] = 0;
    }
    if ((int)uVar3 < piVar2[0x25]) {
LAB_00430d85:
      piVar2[0x25] = uVar3;
    }
  }
  (**(code **)(*piVar2 + 0x40))(*(undefined4 *)(piVar2[2] + 100));
LAB_00430d97:
  meth_0x431000(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
  return;
}



// Function at 00430db0

void __thiscall
OOAnalyzer::cls_0x5a3d44::meth_0x430db0(cls_0x5a3d44 *this,cls_0x5a68d8 *param_1,dword param_2)

{
  uint uVar1;
  dword dVar2;
  cls_0x5a68d8__vftable_5a68d8 *pcVar3;
  undefined uVar4;
  int iVar5;
  undefined4 unaff_EBX;
  dword dVar6;
  int iVar7;
  cls_0x5a68d8 *unaff_retaddr;
  int in_stack_0000000c;
  undefined uVar8;
  undefined uVar9;
  undefined4 uVar10;
  dword local_8c;
  undefined auStack_84 [132];
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if (((((uVar1 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) &&
      (dVar2 = this->mbr_0x9c, (int)dVar2 <= (int)param_2)) &&
     (((int)param_2 < (int)(this->mbr_0xa4 + dVar2) && ((int)param_2 < (int)this->mbr_0x98)))) {
    if ((uVar1 & 0x10000) == 0) {
      dVar6 = this->mbr_0xb0;
      iVar7 = ((int)(param_2 - dVar2) / (int)this->mbr_0xa8) * dVar6 + this->mbr_0xb8;
      local_8c = this->mbr_0xb4;
      iVar5 = (int)(param_2 - dVar2) % (int)this->mbr_0xa8;
    }
    else {
      dVar6 = this->mbr_0xb0;
      local_8c = this->mbr_0xb4;
      iVar7 = ((int)(param_2 - dVar2) % (int)this->mbr_0xac) * dVar6 + this->mbr_0xb8;
      iVar5 = (int)(param_2 - dVar2) / (int)this->mbr_0xac;
    }
    iVar5 = iVar5 * local_8c + this->mbr_0xbc;
    uVar8 = (undefined)iVar5;
    if ((param_2 == this->mbr_0xa0) && (((this->cls_0x5a3ab8).mbr_0x14 & 0x200000) == 0)) {
      uVar10 = 0;
      uVar9 = 0;
      pcVar3 = param_1->vftptr_0x0;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff54);
      uVar4 = cls_0x5a3e7c::meth_0x4384e0((cls_0x5a3e7c *)param_1);
      (*pcVar3->virt_meth_0x4bde60_100)
                (param_1,(char)iVar7,uVar8,(char)dVar6,(char)local_8c,uVar4,uVar9,uVar10);
    }
    else if (in_stack_0000000c == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)(param_1);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x80000) == 0) {
      (**(code **)(*(int *)(this->cls_0x5a3ab8).mbr_0x8 + 0xa4))
                (&(this->cls_0x5a3ab8).mbr_0x38,auStack_84);
      if (*(char *)&(this->cls_0x5a3ab8).mbr_0x38 != '\0') {
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff54);
        cls_0x5a68d8::meth_0x4be2b0
                  (unaff_retaddr,(char)iVar7,uVar8,(char)dVar6,unaff_EBX,(char)auStack_84,0);
      }
    }
    else {
      FUN_0042f570(this->mbr_0x90,(this->cls_0x5a3ab8).mbr_0x8,unaff_retaddr,iVar7,iVar5,dVar6);
    }
    if (param_2 == 0) {
      (*unaff_retaddr->vftptr_0x0->virt_meth_0x4bcc60_32)(unaff_retaddr);
    }
  }
  return;
}



// Function at 00431000

void __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x431000(cls_0x5a3d44 *this,int *param_1)

{
  dword dVar1;
  dword dVar2;
  int in_stack_00000008;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0xb8,this->mbr_0xbc,this->mbr_0xc0,this->mbr_0xc4);
    }
    dVar2 = this->mbr_0x9c;
    while( true ) {
      dVar1 = this->mbr_0xa4 + this->mbr_0x9c;
      if ((int)this->mbr_0x98 <= (int)dVar1) {
        dVar1 = this->mbr_0x98;
      }
      if ((int)dVar1 <= (int)dVar2) break;
      meth_0x430db0(this,param_1,dVar2);
      dVar2 = dVar2 + 1;
    }
    if (in_stack_00000008 == 0) {
      (**(code **)(*param_1 + 0x20))(this->mbr_0xb8,this->mbr_0xbc,this->mbr_0xc0,this->mbr_0xc4);
    }
  }
  return;
}



// Function at 004310b0

int __thiscall OOAnalyzer::cls_0x5a3d44::meth_0x4310b0(cls_0x5a3d44 *this,int param_1)

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int in_stack_00000008;
  
  dVar1 = this->mbr_0xb8;
  if (((int)dVar1 <= param_1) && (param_1 < (int)(this->mbr_0xc0 + dVar1))) {
    dVar2 = this->mbr_0xbc;
    if (((int)dVar2 <= in_stack_00000008) && (in_stack_00000008 < (int)(this->mbr_0xc4 + dVar2))) {
      if (((this->cls_0x5a3ab8).mbr_0x14 & 0x10000) == 0) {
        iVar4 = ((int)(param_1 - dVar1) / (int)this->mbr_0xb0) * this->mbr_0xa8;
        iVar3 = (int)(in_stack_00000008 - dVar2) / (int)this->mbr_0xb4;
      }
      else {
        iVar4 = ((int)(in_stack_00000008 - dVar2) / (int)this->mbr_0xb4) * this->mbr_0xac;
        iVar3 = (int)(param_1 - dVar1) / (int)this->mbr_0xb0;
      }
      iVar3 = iVar4 + iVar3 + this->mbr_0x9c;
      if (iVar3 < (int)this->mbr_0x98) {
        return iVar3;
      }
    }
  }
  return -1;
}



// Function at 00431160

void __thiscall
OOAnalyzer::cls_0x5a3d44::virt_meth_0x431160
          (cls_0x5a3d44 *this,cls_0x5a68d8 *param_1,dword param_2,dword param_3,dword param_4)

{
  int *piVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  uint unaff_EBP;
  dword dVar5;
  undefined4 unaff_EDI;
  undefined4 unaff_retaddr;
  dword in_stack_00000014;
  dword local_78;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  dword local_2c;
  dword local_28;
  dword local_24;
  dword local_20;
  dword local_1c;
  dword local_18;
  dword local_14;
  dword local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    if (this->mbr_0xdc == 0) {
      dVar2 = (this->cls_0x5a3ab8).mbr_0x60;
      dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
      dVar5 = (this->cls_0x5a3ab8).mbr_0x68;
      dVar4 = (this->cls_0x5a3ab8).mbr_0x6c;
    }
    else {
      dVar2 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0xf4;
      dVar5 = this->mbr_0xfc + this->mbr_0xf4 + (this->cls_0x5a3ab8).mbr_0x68;
      dVar3 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0xf8;
      dVar4 = this->mbr_0x100 + this->mbr_0xf8 + (this->cls_0x5a3ab8).mbr_0x6c;
    }
    if (param_2 == 0xffffd8f0) {
      param_3 = dVar3;
      in_stack_00000014 = dVar4;
      param_4 = dVar5;
      param_2 = dVar2;
    }
    local_78 = this->mbr_0x104;
    if ((this->mbr_0xd8 != 0) && (local_78 == 0x80000000)) {
      local_78 = *(uint *)(this->mbr_0xd8 + 0x14) & 0xfffdffff;
    }
    piVar1 = (int *)(this->cls_0x5a3ab8).mbr_0x78;
    if ((piVar1 == (int *)0x0) || ((*(byte *)((int)&(this->cls_0x5a3ab8).mbr_0x14 + 1) & 4) == 0)) {
      if ((this->mbr_0xd8 == 0) || ((local_78 & 0xb100) != 0)) {
        if (*(int *)((this->cls_0x5a3ab8).mbr_0x8 + 0x84) == 0) {
          (*param_1->vftptr_0x0->virt_meth_0x4bde60_100)
                    (param_1,(char)param_2,(char)param_3,(char)param_4,(char)in_stack_00000014,0,0,0
                    );
        }
        else {
          local_2c = param_2;
          local_1c = param_2;
          local_50 = 0;
          local_4c = 0;
          local_48 = 0;
          local_2 = 0;
          local_4 = 0;
          local_40 = 0;
          local_44 = 0;
          local_30 = 0;
          local_34 = 0;
          local_38 = 0;
          local_3c = 0;
          local_c = 0;
          local_54 = 0x20;
          local_8 = 0x1f;
          local_28 = param_3;
          local_24 = param_4;
          local_20 = in_stack_00000014;
          local_18 = param_3;
          local_14 = param_4;
          local_10 = in_stack_00000014;
          (*param_1->vftptr_0x0->virt_meth_0x4bd490_92)(param_1,&local_54);
        }
      }
    }
    else {
      (**(code **)(*piVar1 + 0x44))(param_1,param_2,param_3,param_4,in_stack_00000014);
    }
    dVar2 = param_1->mbr_0x1c;
    dVar3 = param_1->mbr_0x14;
    (*param_1->vftptr_0x0->virt_meth_0x419ea0_72)(param_1);
    (*param_1->vftptr_0x0->virt_meth_0x419e80_68)(param_1,unaff_retaddr,param_1,param_4);
    dVar5 = this->mbr_0xd8;
    if (dVar5 != 0) {
      if (this->mbr_0xdc == 0) {
        cls_0x5a68d8::meth_0x4bd680
                  (param_1,(this->cls_0x5a3ab8).mbr_0x60,(this->cls_0x5a3ab8).mbr_0x64,dVar5,
                   unaff_EBP | 0x20);
      }
      else {
        cls_0x429ac0::meth_0x429ac0
                  ((cls_0x429ac0 *)&this->mbr_0xe0,param_1,dVar5,(this->cls_0x5a3ab8).mbr_0x60,
                   (this->cls_0x5a3ab8).mbr_0x64,(this->cls_0x5a3ab8).mbr_0x68,
                   (this->cls_0x5a3ab8).mbr_0x6c);
      }
    }
    (*param_1->vftptr_0x0->virt_meth_0x419ea0_72)(param_1);
    (*param_1->vftptr_0x0->virt_meth_0x419e60_64)(param_1,unaff_EDI);
    (*param_1->vftptr_0x0->virt_meth_0x419e80_68)(param_1,local_78,dVar2,dVar3);
  }
  return;
}



// Function at 00431410

void __thiscall OOAnalyzer::cls_0x5a3d44::virt_meth_0x431410(cls_0x5a3d44 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int *in_stack_00000004;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)();
    if ((int *)this->mbr_0xd4 != (int *)0x0) {
      (**(code **)(*(int *)this->mbr_0xd4 + 0x40))();
    }
    meth_0x431000(this,in_stack_00000004);
    if (((this->cls_0x5a3ab8).mbr_0x78 == 0) || (((this->cls_0x5a3ab8).mbr_0x14 & 0x400) == 0)) {
      if (this->mbr_0xd8 == 0) {
        dVar1 = 10000;
        dVar2 = 10000;
        dVar3 = 10000;
        dVar4 = 10000;
      }
      else if (this->mbr_0xdc == 0) {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
        dVar2 = (this->cls_0x5a3ab8).mbr_0x68;
        dVar1 = (this->cls_0x5a3ab8).mbr_0x6c;
      }
      else {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0xf4;
        dVar2 = this->mbr_0xfc + (this->cls_0x5a3ab8).mbr_0x68 + this->mbr_0xf4;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0xf8;
        dVar1 = this->mbr_0x100 + (this->cls_0x5a3ab8).mbr_0x6c + this->mbr_0xf8;
      }
      (**(code **)(*in_stack_00000004 + 0x20))(dVar4,dVar3,dVar2,dVar1);
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 004384b0

cls_0x5a3d44 * __thiscall OOAnalyzer::cls_0x5a3d44::virt_meth_0x4384b0(cls_0x5a3d44 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3d44(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004385c0

cls_0x5a3db0 * __thiscall
OOAnalyzer::cls_0x5a3d44::virt_meth_0x4385c0(cls_0x5a3d44 *this,undefined4 param_1)

{
  cls_0x5a3db0 *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ccfb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3db0 *)FUN_00482fb0(0x148,this);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3db0 *)0x0) {
    pcVar1 = cls_0x5a3db0::cls_0x5a3db0(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3db0 *)0x0;
}



