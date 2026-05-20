// Decompiled methods and structure for class: cls_0x5a3e1c

/*
/OOAnalyzer/cls_0x5a3e1c
pack(disabled)
Structure cls_0x5a3e1c {
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
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
   232   dword   4   mbr_0xe8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
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
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 416 Alignment: 1

*/

// Function at 00433640

cls_0x5a3e1c * __thiscall
OOAnalyzer::cls_0x5a3e1c::cls_0x5a3e1c(cls_0x5a3e1c *this,int *param_1,int param_2)

{
  uint uVar1;
  dword *pdVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  dword dVar8;
  int iVar9;
  cls_0x41c7f0 *this_00;
  cls_0x5a3d44 *pcVar10;
  int in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cb4c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,8);
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e1c__vftable_5a3e1c_005a3e1c;
  this->mbr_0x90 = 0;
  this->mbr_0x94 = 0;
  this->mbr_0xbc = 0;
  this->mbr_0xc0 = 0xffffffff;
  if ((uVar1 & 1) == 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 1;
    if ((in_stack_0000000c == 0) && (in_stack_0000000c = 0x655dd0, (uVar1 & 0x10000) == 0)) {
      in_stack_0000000c = 0x655f50;
    }
    iVar9 = cls_0x433170::meth_0x433170_DefWidget_DROPLIST_attr_parser((cls_0x433170 *)&this->mbr_0xc8,in_stack_0000000c,param_1);
    if (iVar9 != 0) {
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
      if (((this->cls_0x5a3ab8).mbr_0x14 & 0x40000) == 0) {
        this->mbr_0x9c = 0;
        this->mbr_0xa0 = 0;
      }
      else {
        if (*(int *)(param_2 + 0x10) != 9) {
          (**(code **)(*param_1 + 0x98))();
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
          cls_0x41c7f0::cls_0x41c7f0(this_00,0x40);
        }
        local_4 = (uint)local_4._1_3_ << 8;
        this->mbr_0x9c = (dword)this_00;
        this->mbr_0xa0 = 1;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffd0);
        iVar9 = FUN_0042f700(this->mbr_0x9c,param_1,param_2,this->mbr_0x190);
        if (iVar9 == 0) {
          ExceptionList = local_c;
          return this;
        }
      }
      dVar3 = this->mbr_0x130;
      dVar4 = (this->cls_0x5a3ab8).mbr_0x60;
      uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
      if ((uVar1 & 0x80000) == 0) {
        iVar9 = this->mbr_0x13c + (this->cls_0x5a3ab8).mbr_0x6c + (this->cls_0x5a3ab8).mbr_0x64;
      }
      else {
        iVar9 = ((this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0x140) - this->mbr_0x138;
      }
      dVar5 = (this->cls_0x5a3ab8).mbr_0x68;
      dVar6 = this->mbr_0x134;
      dVar7 = this->mbr_0x9c;
      dVar8 = this->mbr_0x140;
      pcVar10 = (cls_0x5a3d44 *)FUN_00482fb0();
      local_4._0_1_ = 2;
      if (pcVar10 == (cls_0x5a3d44 *)0x0) {
        pcVar10 = (cls_0x5a3d44 *)0x0;
      }
      else {
        pcVar10 = cls_0x5a3d44::cls_0x5a3d44
                            (pcVar10,param_1,dVar4 + dVar3,iVar9,(dVar5 - dVar6) - dVar3,dVar8,0,
                             &(this->cls_0x5a3ab8).mbr_0x38,(this->cls_0x5a3ab8).mbr_0x58,
                             (uVar1 & 0x10000) << 2 | -(uint)(dVar7 != 0) & 0x80000 | 0x10002,
                             &this->mbr_0x144,this->mbr_0x9c);
      }
      this->mbr_0xc4 = (dword)pcVar10;
      (pcVar10->cls_0x5a3ab8).mbr_0x78 = (dword)this;
      local_4 = (uint)local_4._1_3_ << 8;
      (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
      meth_0x433ef0(this);
    }
  }
  ExceptionList = local_c;
  return this;
}



// Function at 004338d0

cls_0x5a3e1c * __thiscall OOAnalyzer::cls_0x5a3e1c::~cls_0x5a3e1c(cls_0x5a3e1c *this)

{
  dword dVar1;
  cls_0x45f7c0 *this_00;
  int *piVar2;
  int iVar3;
  cls_0x5a3e1c *pcVar4;
  int iVar5;
  int iVar6;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059cb5e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3e1c__vftable_5a3e1c_005a3e1c;
  dVar1 = (this->cls_0x5a3ab8).mbr_0x8;
  iVar5 = 0;
  local_4 = 0;
  if (dVar1 != 0) {
    *(undefined4 *)(dVar1 + 0xa8) = 0;
    *(undefined4 *)(dVar1 + 0xac) = 1;
  }
  if ((undefined4 *)this->mbr_0xc4 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0xc4)(1);
  }
  this_00 = (cls_0x45f7c0 *)this->mbr_0x9c;
  if ((this_00 != (cls_0x45f7c0 *)0x0) && (this->mbr_0xa0 != 0)) {
    if (0 < (int)(this_00->cls_0x41c7f0).mbr_0x0) {
      do {
        if ((-1 < iVar5) &&
           (piVar2 = *(int **)((this_00->cls_0x41c7f0).mbr_0x10 + iVar5 * 4), piVar2 != (int *)0x0))
        {
          iVar3 = *piVar2;
          if ((((iVar3 == 2) || (iVar3 == 1)) && (iVar6 = piVar2[9], iVar6 != 0)) ||
             ((iVar3 == 0 && (iVar6 = piVar2[3], iVar6 != 0)))) {
            FUN_00482f80(iVar6);
          }
          FUN_004830f0(piVar2);
        }
        cls_0x45f7c0::meth_0x41cb40(this_00);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
    }
    dVar1 = this->mbr_0x9c;
    (this_00->cls_0x41c7f0).mbr_0x0 = 0;
    (this_00->cls_0x41c7f0).mbr_0x4 = 0;
    if (dVar1 != 0) {
      FUN_004830f0(*(undefined4 *)(dVar1 + 0x10));
      FUN_004830f0(dVar1);
    }
  }
  pcVar4 = (cls_0x5a3e1c *)(this->cls_0x5a3ab8).mbr_0x5c;
  local_4 = 0xffffffff;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar4 != (cls_0x5a3e1c *)0x0) {
    pcVar4 = (cls_0x5a3e1c *)FUN_00482f80(pcVar4);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  ExceptionList = local_c;
  return pcVar4;
}



// Function at 00433ef0

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x433ef0(cls_0x5a3e1c *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  uint uVar5;
  dword dVar6;
  int iVar7;
  
  if ((int)this->mbr_0xbc < (int)this->mbr_0xc0) {
    this->mbr_0xc0 = 0xffffffff;
  }
  dVar1 = (this->cls_0x5a3ab8).mbr_0x60;
  dVar2 = (this->cls_0x5a3ab8).mbr_0x64;
  this->mbr_0xa4 = dVar1 + this->mbr_0xc8 + this->mbr_0xdc;
  dVar3 = (this->cls_0x5a3ab8).mbr_0x68;
  this->mbr_0xa8 = this->mbr_0xcc + dVar2 + this->mbr_0xe0;
  this->mbr_0xac = (((dVar3 - this->mbr_0xe4) - this->mbr_0xd0) - this->mbr_0xc8) - this->mbr_0xdc;
  dVar4 = (this->cls_0x5a3ab8).mbr_0x6c;
  this->mbr_0xb0 = (((dVar4 - this->mbr_0xe8) - this->mbr_0xd4) - this->mbr_0xcc) - this->mbr_0xe0;
  if (this->mbr_0x128 == 0) {
    dVar6 = this->mbr_0x120;
  }
  else {
    dVar6 = dVar3 - this->mbr_0x120;
  }
  this->mbr_0xb4 = dVar6 + dVar1;
  this->mbr_0xb8 = this->mbr_0x124 + dVar2;
  if ((int *)this->mbr_0xc4 != (int *)0x0) {
    uVar5 = (this->cls_0x5a3ab8).mbr_0x14;
    if ((uVar5 & 0x80000) == 0) {
      iVar7 = this->mbr_0x13c + dVar4 + dVar2;
    }
    else {
      iVar7 = (dVar2 - this->mbr_0x140) - this->mbr_0x138;
    }
    dVar2 = this->mbr_0x9c;
    (**(code **)(*(int *)this->mbr_0xc4 + 0x28))
              (this->mbr_0x130 + dVar1,iVar7,(dVar3 - this->mbr_0x134) - this->mbr_0x130,
               this->mbr_0x140);
    (**(code **)(*(int *)this->mbr_0xc4 + 0x1c))
              (-(uint)(dVar2 != 0) & 0x80000 | (uVar5 & 0x10000) << 2 | 0x10002);
    cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0xc4);
    cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0xc4);
    cls_0x5a3d44::meth_0x430ab0((cls_0x5a3d44 *)this->mbr_0xc4);
  }
  (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  return;
}



// Function at 004340a0

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x4340a0(cls_0x5a3e1c *this)

{
  dword dVar1;
  dword in_stack_00000004;
  
  if ((in_stack_00000004 != this->mbr_0xc0) ||
     ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
    if (((int)in_stack_00000004 < 0) || ((int)this->mbr_0xbc <= (int)in_stack_00000004)) {
      in_stack_00000004 = 0;
    }
    dVar1 = this->mbr_0xc0;
    this->mbr_0xc0 = in_stack_00000004;
    if ((cls_0x5a3d44 *)this->mbr_0xc4 != (cls_0x5a3d44 *)0x0) {
      cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0xc4);
    }
    meth_0x4341a0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    if (dVar1 != this->mbr_0xc0) {
      cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
    }
  }
  return;
}



// Function at 00434110

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x434110(cls_0x5a3e1c *this)

{
  dword dVar1;
  dword in_stack_00000004;
  
  if ((in_stack_00000004 != this->mbr_0xbc) ||
     ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
    dVar1 = this->mbr_0xc0;
    this->mbr_0xbc = in_stack_00000004;
    if (((int)in_stack_00000004 <= (int)dVar1) &&
       ((dVar1 != 0 || ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)))) {
      this->mbr_0xc0 = 0;
      if ((cls_0x5a3d44 *)this->mbr_0xc4 != (cls_0x5a3d44 *)0x0) {
        cls_0x5a3d44::meth_0x430b80((cls_0x5a3d44 *)this->mbr_0xc4);
      }
      meth_0x4341a0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
      if (dVar1 != this->mbr_0xc0) {
        cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
      }
    }
    if ((cls_0x5a3d44 *)this->mbr_0xc4 != (cls_0x5a3d44 *)0x0) {
      cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0xc4);
    }
  }
  return;
}



// Function at 004341a0

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x4341a0(cls_0x5a3e1c *this,cls_0x5a68d8 *param_1)

{
  uint uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  cls_0x5a68d8__vftable_5a68d8 *pcVar7;
  undefined uVar8;
  int in_stack_00000008;
  undefined uVar9;
  undefined uVar10;
  undefined uVar11;
  undefined uVar12;
  undefined4 uVar13;
  undefined auStack_84 [132];
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if (((((uVar1 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) &&
      (dVar2 = this->mbr_0xc0, -1 < (int)dVar2)) && ((int)dVar2 < (int)this->mbr_0xbc)) {
    dVar3 = this->mbr_0xb0;
    dVar4 = this->mbr_0xa8;
    dVar5 = this->mbr_0xac;
    dVar6 = this->mbr_0xa4;
    uVar9 = (undefined)dVar6;
    uVar10 = (undefined)dVar4;
    uVar11 = (undefined)dVar3;
    if ((uVar1 & 0x20000) == 0) {
      if (in_stack_00000008 == 0) {
        (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)(param_1);
      }
    }
    else {
      pcVar7 = param_1->vftptr_0x0;
      uVar13 = 0;
      uVar12 = 0;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff4c);
      uVar8 = cls_0x5a3e7c::meth_0x4384e0((cls_0x5a3e7c *)param_1);
      (*pcVar7->virt_meth_0x4bde60_100)(param_1,uVar9,uVar10,(char)dVar5,uVar11,uVar8,uVar12,uVar13)
      ;
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x40000) == 0) {
      if (*(char *)&(this->cls_0x5a3ab8).mbr_0x38 != '\0') {
        (**(code **)(*(int *)(this->cls_0x5a3ab8).mbr_0x8 + 0xa4))
                  (&(this->cls_0x5a3ab8).mbr_0x38,auStack_84);
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff4c);
        cls_0x5a68d8::meth_0x4be2b0_CompositeBuffer(param_1,uVar9,uVar10,uVar11,dVar2,(char)auStack_84,0);
      }
    }
    else {
      FUN_0042f570(this->mbr_0x9c,(this->cls_0x5a3ab8).mbr_0x8,param_1,dVar6,dVar4,dVar3);
    }
    if (param_1 == (cls_0x5a68d8 *)0x0) {
      (**(code **)(iRam00000000 + 0x20))((void *)0x0);
    }
  }
  return;
}



// Function at 00434350

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x434350(cls_0x5a3e1c *this,cls_0x5a68d8 *param_1)

{
  undefined4 *puVar1;
  int in_stack_00000008;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  if ((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0))
     && (puVar1 = (undefined4 *)this->mbr_0x118, puVar1 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0xb4,this->mbr_0xb8,*puVar1,puVar1[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x100000) == 0) {
      dVar5 = this->mbr_0x12c;
      dVar4 = this->mbr_0x118;
      dVar3 = this->mbr_0xb8;
      dVar2 = this->mbr_0xb4;
    }
    else {
      dVar5 = this->mbr_0x12c;
      dVar4 = this->mbr_0x11c;
      dVar3 = this->mbr_0xb8;
      dVar2 = this->mbr_0xb4;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar2,dVar3,dVar4,dVar5);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 004347d0

void __thiscall OOAnalyzer::cls_0x5a3e1c::virt_meth_0x4347d0(cls_0x5a3e1c *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int *in_stack_00000004;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)();
    meth_0x4341a0(this,in_stack_00000004);
    meth_0x434350(this,in_stack_00000004);
    if (((this->cls_0x5a3ab8).mbr_0x78 == 0) || (((this->cls_0x5a3ab8).mbr_0x14 & 0x400) == 0)) {
      if (this->mbr_0xd8 == 0) {
        dVar1 = 10000;
        dVar2 = 10000;
        dVar3 = 10000;
        dVar4 = 10000;
      }
      else if (this->mbr_0xec == 0) {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
        dVar2 = (this->cls_0x5a3ab8).mbr_0x68;
        dVar1 = (this->cls_0x5a3ab8).mbr_0x6c;
      }
      else {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0x104;
        dVar2 = this->mbr_0x10c + (this->cls_0x5a3ab8).mbr_0x68 + this->mbr_0x104;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0x108;
        dVar1 = this->mbr_0x110 + (this->cls_0x5a3ab8).mbr_0x6c + this->mbr_0x108;
      }
      (**(code **)(*in_stack_00000004 + 0x20))(dVar4,dVar3,dVar2,dVar1);
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 00434920

void __thiscall OOAnalyzer::cls_0x5a3e1c::meth_0x434920(cls_0x5a3e1c *this)

{
  int *piVar1;
  
  (**(code **)(*(int *)this->mbr_0xc4 + 0x1c))(((int *)this->mbr_0xc4)[5] | 2);
  piVar1 = (int *)(this->cls_0x5a3ab8).mbr_0x8;
  piVar1[0x2a] = 0;
  piVar1[0x2b] = 1;
  (**(code **)(*piVar1 + 0x2c))(1);
  this->mbr_0x94 = 0;
  return;
}



// Function at 00434960

undefined4 __thiscall
OOAnalyzer::cls_0x5a3e1c::virt_meth_0x434960(cls_0x5a3e1c *this,undefined4 param_1)

{
  cls_0x5a3d44 *this_00;
  dword dVar1;
  int *piVar2;
  int iVar3;
  dword dVar4;
  uint uVar5;
  int in_stack_00000008;
  
  this_00 = (cls_0x5a3d44 *)this->mbr_0xc4;
  if (this_00 != (cls_0x5a3d44 *)0x0) {
    if (in_stack_00000008 == 0x138a) {
      dVar4 = this_00->mbr_0xa0;
      if ((dVar4 != this->mbr_0xc0) || ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
        if (((int)dVar4 < 0) || ((int)this->mbr_0xbc <= (int)dVar4)) {
          dVar4 = 0;
        }
        dVar1 = this->mbr_0xc0;
        this->mbr_0xc0 = dVar4;
        cls_0x5a3d44::meth_0x430b80(this_00);
        meth_0x4341a0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
        if (dVar1 != this->mbr_0xc0) {
          cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
        }
      }
      (**(code **)(*(int *)this->mbr_0xc4 + 0x1c))(((int *)this->mbr_0xc4)[5] | 2);
      piVar2 = (int *)(this->cls_0x5a3ab8).mbr_0x8;
      piVar2[0x2a] = 0;
      piVar2[0x2b] = 1;
      (**(code **)(*piVar2 + 0x2c))(1);
      this->mbr_0x94 = 0;
      return 1;
    }
    if (in_stack_00000008 == 0x65) {
      uVar5 = (this_00->cls_0x5a3ab8).mbr_0x14 | 2;
      (*((this_00->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this_00);
      piVar2 = (int *)(this->cls_0x5a3ab8).mbr_0x8;
      piVar2[0x2a] = 0;
      piVar2[0x2b] = 1;
      (**(code **)(*piVar2 + 0x2c))(1,uVar5);
      iVar3 = DAT_00667fd0;
      this->mbr_0x94 = 0;
      this->mbr_0x98 = *(dword *)(iVar3 + 0x48);
    }
  }
  return 1;
}



// Function at 00434d30

void __thiscall
OOAnalyzer::cls_0x5a3e1c::virt_meth_0x434d30(cls_0x5a3e1c *this,undefined4 param_1,int param_2)

{
  dword *pdVar1;
  uint uVar2;
  int *piVar3;
  dword dVar4;
  dword dVar5;
  int in_stack_0000000c;
  undefined4 uVar6;
  
  if (this->mbr_0x90 == 0) {
    return;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar2 & 0x200000) == 0) {
    dVar4 = (this->cls_0x5a3ab8).mbr_0x8;
    dVar5 = this->mbr_0x118;
    this->mbr_0x90 = 0;
    *(undefined4 *)(dVar4 + 0xa4) = 0;
    if ((dVar5 != 0) && ((uVar2 & 0x100000) != 0)) {
      uVar6 = *(undefined4 *)(dVar4 + 100);
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xffefffff;
      meth_0x434350(this,uVar6);
    }
    if (this->mbr_0x118 == 0) {
      return;
    }
    pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
    *pdVar1 = *pdVar1 & 0xffdfffff;
    return;
  }
  piVar3 = (int *)this->mbr_0x118;
  if ((((piVar3 == (int *)0x0) || (param_2 < (int)this->mbr_0xb4)) ||
      (in_stack_0000000c < (int)this->mbr_0xb8)) ||
     (((int)(*piVar3 + this->mbr_0xb4) <= param_2 ||
      ((int)(piVar3[1] + this->mbr_0xb8) <= in_stack_0000000c)))) {
    if ((piVar3 == (int *)0x0) || ((uVar2 & 0x100000) == 0)) goto LAB_00434dd4;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xffefffff;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  else {
    if ((uVar2 & 0x100000) == 0x100000) goto LAB_00434dd4;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x100000;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  meth_0x434350(this,uVar6);
LAB_00434dd4:
  meth_0x434350(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
  return;
}



// Function at 00438aa0

cls_0x5a3e1c * __thiscall OOAnalyzer::cls_0x5a3e1c::virt_meth_0x438aa0(cls_0x5a3e1c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3e1c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



