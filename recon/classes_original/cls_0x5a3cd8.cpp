// Decompiled methods and structure for class: cls_0x5a3cd8

/*
/OOAnalyzer/cls_0x5a3cd8
pack(disabled)
Structure cls_0x5a3cd8 {
   0   cls_0x5a3ab8   144   cls_0x5a3ab8   "Component (member) class."
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
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
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   340   dword   4   mbr_0x154   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   348   dword   4   mbr_0x15c   "Unsigned Double-Word (ddw, 4-bytes)"
   352   dword   4   mbr_0x160   "Unsigned Double-Word (ddw, 4-bytes)"
   356   dword   4   mbr_0x164   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 360 Alignment: 1

*/

// Function at 0042de00

cls_0x5a3cd8 * __thiscall
OOAnalyzer::cls_0x5a3cd8::cls_0x5a3cd8
          (cls_0x5a3cd8 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined4 param_7,
          undefined4 param_8,undefined4 param_9)

{
  uint uVar1;
  int iVar2;
  dword *pdVar3;
  dword *in_stack_00000028;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca94;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8
            (&this->cls_0x5a3ab8,param_1,5,param_6,param_7,param_8,0,param_2,param_3,param_4,param_5
             ,(undefined2)param_9,0,0xffffffff,0,0,0,0xffffffff);
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3cd8__vftable_5a3cd8_005a3cd8;
  this->mbr_0x90 = 0;
  this->mbr_0x94 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 1000;
  this->mbr_0xa0 = 100;
  if ((uVar1 & 1) == 0) {
    if ((in_stack_00000028 == (dword *)0x0) &&
       (in_stack_00000028 = &DAT_00655d38, (uVar1 & 0x20000) == 0)) {
      in_stack_00000028 = &DAT_006560f8;
    }
    pdVar3 = &this->mbr_0xa4;
    for (iVar2 = 0x21; iVar2 != 0; iVar2 = iVar2 + -1) {
      *pdVar3 = *in_stack_00000028;
      in_stack_00000028 = in_stack_00000028 + 1;
      pdVar3 = pdVar3 + 1;
    }
    meth_0x42e240(this);
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0042df00

cls_0x5a3cd8 * __thiscall
OOAnalyzer::cls_0x5a3cd8::cls_0x5a3cd8(cls_0x5a3cd8 *this,undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  dword *pdVar3;
  dword *in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059caa6;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,5);
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3cd8__vftable_5a3cd8_005a3cd8;
  this->mbr_0x90 = 0;
  this->mbr_0x94 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 1000;
  this->mbr_0xa0 = 100;
  if ((uVar1 & 1) == 0) {
    if (in_stack_0000000c == (dword *)0x0) {
      pdVar3 = &this->mbr_0xa4;
      iVar2 = 0x21;
      in_stack_0000000c = &DAT_00655d38;
      if ((uVar1 & 0x20000) == 0) {
        in_stack_0000000c = &DAT_006560f8;
      }
    }
    else {
      iVar2 = 0x21;
      pdVar3 = &this->mbr_0xa4;
    }
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      *pdVar3 = *in_stack_0000000c;
      in_stack_0000000c = in_stack_0000000c + 1;
      pdVar3 = pdVar3 + 1;
    }
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 1;
    iVar2 = meth_0x42dfe0(this,param_1,param_2);
    if (iVar2 != 0) {
      (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
      meth_0x42e240(this);
    }
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0042dfe0

undefined4 __thiscall
OOAnalyzer::cls_0x5a3cd8::meth_0x42dfe0(cls_0x5a3cd8 *this,undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  dword *pdVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  undefined4 *in_stack_0000000c;
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 1;
  if ((in_stack_0000000c == (undefined4 *)0x0) &&
     (in_stack_0000000c = &DAT_00655d38, (uVar1 & 0x20000) == 0)) {
    in_stack_0000000c = &DAT_006560f8;
  }
  iVar5 = cls_0x42d730::meth_0x42d730_DefWidget_FRAME_attr_parser((cls_0x42d730 *)&this->mbr_0xa4,in_stack_0000000c,param_1);
  if (iVar5 == 0) {
    return 0;
  }
  pdVar2 = (dword *)this->mbr_0xa4;
  if (pdVar2 != (dword *)0x0) {
    if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
      (this->cls_0x5a3ab8).mbr_0x60 = -pdVar2[2];
      (this->cls_0x5a3ab8).mbr_0x64 = -pdVar2[3];
    }
    if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
      dVar3 = pdVar2[1];
      (this->cls_0x5a3ab8).mbr_0x68 = *pdVar2;
      (this->cls_0x5a3ab8).mbr_0x6c = dVar3;
      return 1;
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x10000) == 0) {
      dVar3 = this->mbr_0xa8;
      dVar4 = pdVar2[1];
      (this->cls_0x5a3ab8).mbr_0x6c = dVar4;
      if (dVar3 != 0) {
        (this->cls_0x5a3ab8).mbr_0x6c = (dVar4 - this->mbr_0xcc) - this->mbr_0xc4;
      }
    }
    else {
      dVar3 = this->mbr_0xa8;
      dVar4 = *pdVar2;
      (this->cls_0x5a3ab8).mbr_0x68 = dVar4;
      if (dVar3 != 0) {
        (this->cls_0x5a3ab8).mbr_0x68 = (dVar4 - this->mbr_0xc8) - this->mbr_0xc0;
        return 1;
      }
    }
  }
  return 1;
}



// Function at 0042e180

int __thiscall OOAnalyzer::cls_0x5a3cd8::virt_meth_0x42e180(cls_0x5a3cd8 *this)

{
  int *piVar1;
  int iVar2;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  dword dStack_18;
  undefined local_c [12];
  
  if ((*(char *)&(this->cls_0x5a3ab8).mbr_0x38 != '\0') &&
     (piVar1 = (int *)(this->cls_0x5a3ab8).mbr_0x8, piVar1 != (int *)0x0)) {
    dStack_18 = (this->cls_0x5a3ab8).mbr_0x58;
    uStack_1c = 0;
    uStack_20 = 10;
    iVar2 = (**(code **)(*piVar1 + 0xa4))(&(this->cls_0x5a3ab8).mbr_0x38,local_c);
    if (iVar2 != 0) {
      FUN_0058b42c(&uStack_20);
      meth_0x42e440(this);
    }
    return iVar2;
  }
  return 0;
}



// Function at 0042e240

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e240(cls_0x5a3cd8 *this)

{
  dword dVar1;
  uint uVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  dVar5 = (this->cls_0x5a3ab8).mbr_0x60;
  if (this->mbr_0xe4 == 0) {
    dVar3 = this->mbr_0xdc;
  }
  else {
    dVar3 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0xdc;
  }
  dVar1 = (this->cls_0x5a3ab8).mbr_0x64;
  this->mbr_0x140 = dVar3 + dVar5;
  if (this->mbr_0xe8 == 0) {
    dVar3 = this->mbr_0xe0;
  }
  else {
    dVar3 = (this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0xe0;
  }
  this->mbr_0x144 = dVar3 + dVar1;
  if (this->mbr_0x100 == 0) {
    dVar3 = this->mbr_0xf8;
  }
  else {
    dVar3 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0xf8;
  }
  this->mbr_0x148 = dVar3 + dVar5;
  if (this->mbr_0x104 == 0) {
    dVar3 = this->mbr_0xfc;
  }
  else {
    dVar3 = (this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0xfc;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  this->mbr_0x14c = dVar3 + dVar1;
  if ((uVar2 & 0x10000) == 0) {
    dVar3 = (this->cls_0x5a3ab8).mbr_0x68;
    dVar4 = this->mbr_0x114 + dVar5;
    this->mbr_0x150 = dVar4;
    this->mbr_0x158 = dVar4;
    this->mbr_0x154 = (dVar3 - this->mbr_0x118) + dVar5;
    if (this->mbr_0x120 == 0) {
      dVar5 = this->mbr_0x11c;
    }
    else {
      dVar5 = (this->cls_0x5a3ab8).mbr_0x6c - this->mbr_0x11c;
    }
    this->mbr_0x15c = dVar5 + dVar1;
  }
  else {
    dVar3 = this->mbr_0x114 + dVar1;
    dVar4 = (this->cls_0x5a3ab8).mbr_0x6c;
    this->mbr_0x150 = dVar3;
    this->mbr_0x154 = (dVar4 - this->mbr_0x118) + dVar1;
    if (this->mbr_0x120 == 0) {
      this->mbr_0x15c = dVar3;
      this->mbr_0x158 = this->mbr_0x11c + dVar5;
    }
    else {
      dVar1 = (this->cls_0x5a3ab8).mbr_0x68;
      this->mbr_0x15c = dVar3;
      this->mbr_0x158 = (dVar1 - this->mbr_0x11c) + dVar5;
    }
  }
  if ((int)this->mbr_0x9c < (int)this->mbr_0x94) {
    this->mbr_0x94 = this->mbr_0x9c;
  }
  if ((int)this->mbr_0x94 < (int)this->mbr_0x98) {
    this->mbr_0x94 = this->mbr_0x98;
  }
  (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  return;
}



// Function at 0042e3d0

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e3d0(cls_0x5a3cd8 *this,dword param_1)

{
  dword in_stack_00000008;
  
  if ((int)in_stack_00000008 < (int)param_1) {
    in_stack_00000008 = param_1;
  }
  if (((param_1 != this->mbr_0x98) || (in_stack_00000008 != this->mbr_0x9c)) ||
     ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
    this->mbr_0x98 = param_1;
    this->mbr_0x9c = in_stack_00000008;
    if ((int)this->mbr_0x94 < (int)param_1) {
      this->mbr_0x94 = param_1;
    }
    if ((int)in_stack_00000008 < (int)this->mbr_0x94) {
      this->mbr_0x94 = in_stack_00000008;
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x4388c0_64)(this);
  }
  return;
}



// Function at 0042e440

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e440(cls_0x5a3cd8 *this)

{
  dword dVar1;
  dword dVar2;
  int *piVar3;
  undefined4 *puVar4;
  dword dVar5;
  dword in_stack_00000004;
  
  dVar5 = this->mbr_0x9c;
  if ((int)dVar5 < (int)in_stack_00000004) {
    in_stack_00000004 = dVar5;
  }
  dVar1 = this->mbr_0x98;
  if ((int)in_stack_00000004 < (int)dVar1) {
    in_stack_00000004 = dVar1;
  }
  dVar2 = this->mbr_0x94;
  if ((in_stack_00000004 != dVar2) || ((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 0x20) != 0)) {
    this->mbr_0x94 = in_stack_00000004;
    if ((int)(dVar5 - dVar1) < 1) {
      dVar5 = this->mbr_0x150;
    }
    else {
      dVar5 = (int)((this->mbr_0x154 - this->mbr_0x150) * in_stack_00000004) / (int)(dVar5 - dVar1)
              + this->mbr_0x150;
    }
    dVar1 = (this->cls_0x5a3ab8).mbr_0x8;
    piVar3 = *(int **)(dVar1 + 100);
    if ((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && (dVar1 != 0)) &&
       (puVar4 = (undefined4 *)this->mbr_0x10c, puVar4 != (undefined4 *)0x0)) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (piVar3,this->mbr_0x158,this->mbr_0x15c,*puVar4,puVar4[1]);
      (**(code **)(*piVar3 + 0x20))
                (this->mbr_0x158,this->mbr_0x15c,*(undefined4 *)this->mbr_0x10c,
                 ((undefined4 *)this->mbr_0x10c)[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x10000) == 0) {
      this->mbr_0x158 = dVar5;
    }
    else {
      this->mbr_0x15c = dVar5;
    }
    meth_0x42e620(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    if (dVar2 != in_stack_00000004) {
      cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
    }
  }
  return;
}



// Function at 0042e580

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e580(cls_0x5a3cd8 *this)

{
  meth_0x42e440(this);
  return;
}



// Function at 0042e620

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e620(cls_0x5a3cd8 *this,cls_0x5a68d8 *param_1)

{
  undefined4 *puVar1;
  int in_stack_00000008;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  if (((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0))
      && (0 < (int)(this->mbr_0x9c - this->mbr_0x98))) &&
     (puVar1 = (undefined4 *)this->mbr_0x10c, puVar1 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0x158,this->mbr_0x15c,*puVar1,puVar1[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x400000) == 0) {
      dVar5 = this->mbr_0x124;
      dVar4 = this->mbr_0x10c;
      dVar3 = this->mbr_0x15c;
      dVar2 = this->mbr_0x158;
    }
    else {
      dVar5 = this->mbr_0x124;
      dVar4 = this->mbr_0x110;
      dVar3 = this->mbr_0x15c;
      dVar2 = this->mbr_0x158;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar2,dVar3,dVar4,dVar5);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 0042e770

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e770(cls_0x5a3cd8 *this)

{
  uint uVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0x10c != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != (uVar1 >> 0x16 & 1)) {
      if (in_stack_00000004 != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x400000;
        meth_0x42e620(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
        return;
      }
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xffbfffff;
      meth_0x42e620(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    }
  }
  return;
}



// Function at 0042e7d0

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e7d0(cls_0x5a3cd8 *this,cls_0x5a68d8 *param_1)

{
  undefined4 *puVar1;
  int in_stack_00000008;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  if ((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0))
     && (puVar1 = (undefined4 *)this->mbr_0xd4, puVar1 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0x140,this->mbr_0x144,*puVar1,puVar1[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x80000) == 0) {
      dVar5 = this->mbr_0xec;
      dVar4 = this->mbr_0xd4;
      dVar3 = this->mbr_0x144;
      dVar2 = this->mbr_0x140;
    }
    else {
      dVar5 = this->mbr_0xec;
      dVar4 = this->mbr_0xd8;
      dVar3 = this->mbr_0x144;
      dVar2 = this->mbr_0x140;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar2,dVar3,dVar4,dVar5);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 0042e8a0

undefined4 __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e8a0(cls_0x5a3cd8 *this,int param_1)

{
  int *piVar1;
  int in_stack_00000008;
  
  piVar1 = (int *)this->mbr_0xd4;
  if (piVar1 != (int *)0x0) {
    if ((int)this->mbr_0x140 <= param_1) {
      if ((((int)this->mbr_0x144 <= in_stack_00000008) &&
          (param_1 < (int)(*piVar1 + this->mbr_0x140))) &&
         (in_stack_00000008 < (int)(piVar1[1] + this->mbr_0x144))) {
        return 1;
      }
    }
  }
  return 0;
}



// Function at 0042e8f0

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e8f0(cls_0x5a3cd8 *this)

{
  uint uVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0xd4 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != (uVar1 >> 0x13 & 1)) {
      if (in_stack_00000004 != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x80000;
        meth_0x42e7d0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
        return;
      }
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xfff7ffff;
      meth_0x42e7d0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    }
  }
  return;
}



// Function at 0042e950

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e950(cls_0x5a3cd8 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  if (this->mbr_0xd4 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != 0) {
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x40000;
      return;
    }
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xfffbffff;
  }
  return;
}



// Function at 0042e980

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42e980(cls_0x5a3cd8 *this,cls_0x5a68d8 *param_1)

{
  undefined4 *puVar1;
  int in_stack_00000008;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  if ((((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0))
     && (puVar1 = (undefined4 *)this->mbr_0xf0, puVar1 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0x148,this->mbr_0x14c,*puVar1,puVar1[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x100000) == 0) {
      dVar5 = this->mbr_0x108;
      dVar4 = this->mbr_0xf0;
      dVar3 = this->mbr_0x14c;
      dVar2 = this->mbr_0x148;
    }
    else {
      dVar5 = this->mbr_0x108;
      dVar4 = this->mbr_0xf4;
      dVar3 = this->mbr_0x14c;
      dVar2 = this->mbr_0x148;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar2,dVar3,dVar4,dVar5);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 0042ea50

undefined4 __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42ea50(cls_0x5a3cd8 *this,int param_1)

{
  int *piVar1;
  int in_stack_00000008;
  
  piVar1 = (int *)this->mbr_0xf0;
  if (piVar1 != (int *)0x0) {
    if ((int)this->mbr_0x148 <= param_1) {
      if ((((int)this->mbr_0x14c <= in_stack_00000008) &&
          (param_1 < (int)(*piVar1 + this->mbr_0x148))) &&
         (in_stack_00000008 < (int)(piVar1[1] + this->mbr_0x14c))) {
        return 1;
      }
    }
  }
  return 0;
}



// Function at 0042eaa0

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42eaa0(cls_0x5a3cd8 *this)

{
  uint uVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0xf0 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != (uVar1 >> 0x14 & 1)) {
      if (in_stack_00000004 != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x100000;
        meth_0x42e980(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
        return;
      }
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xffefffff;
      meth_0x42e980(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    }
  }
  return;
}



// Function at 0042eb00

void __thiscall OOAnalyzer::cls_0x5a3cd8::meth_0x42eb00(cls_0x5a3cd8 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  if (this->mbr_0xf0 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != 0) {
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x200000;
      return;
    }
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xffdfffff;
  }
  return;
}



// Function at 0042ede0

void __thiscall OOAnalyzer::cls_0x5a3cd8::virt_meth_0x42ede0(cls_0x5a3cd8 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int *in_stack_00000004;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)();
    meth_0x42e7d0(this,in_stack_00000004);
    meth_0x42e980(this,in_stack_00000004);
    meth_0x42e620(this,in_stack_00000004);
    if (((this->cls_0x5a3ab8).mbr_0x78 == 0) || (((this->cls_0x5a3ab8).mbr_0x14 & 0x400) == 0)) {
      if (this->mbr_0xa4 == 0) {
        dVar1 = 10000;
        dVar2 = 10000;
        dVar3 = 10000;
        dVar4 = 10000;
      }
      else if (this->mbr_0xa8 == 0) {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
        dVar2 = (this->cls_0x5a3ab8).mbr_0x68;
        dVar1 = (this->cls_0x5a3ab8).mbr_0x6c;
      }
      else {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0xc0;
        dVar2 = this->mbr_0xc8 + (this->cls_0x5a3ab8).mbr_0x68 + this->mbr_0xc0;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0xc4;
        dVar1 = this->mbr_0xcc + (this->cls_0x5a3ab8).mbr_0x6c + this->mbr_0xc4;
      }
      (**(code **)(*in_stack_00000004 + 0x20))(dVar4,dVar3,dVar2,dVar1);
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 0042f2e0

void __thiscall
OOAnalyzer::cls_0x5a3cd8::virt_meth_0x42f2e0(cls_0x5a3cd8 *this,undefined4 param_1,int param_2)

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
    if ((uVar2 & 0x40000) == 0) {
      if ((uVar2 & 0x400000) != 0) {
        if (0 < (int)(this->mbr_0x154 - this->mbr_0x150)) {
          meth_0x42e440(this);
          return;
        }
        meth_0x42e440(this);
        return;
      }
      dVar4 = (this->cls_0x5a3ab8).mbr_0x8;
      dVar5 = this->mbr_0xd4;
      this->mbr_0x90 = 0;
      *(undefined4 *)(dVar4 + 0xa4) = 0;
      if ((dVar5 != 0) && ((uVar2 & 0x80000) != 0)) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xfff7ffff;
        meth_0x42e7d0(this,*(undefined4 *)(dVar4 + 100));
      }
      if (this->mbr_0xd4 != 0) {
        pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
        *pdVar1 = *pdVar1 & 0xfffbffff;
      }
      if (this->mbr_0xf0 == 0) {
        return;
      }
      uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
      if ((uVar2 & 0x100000) != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xffefffff;
        meth_0x42e980(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
      }
      if (this->mbr_0xf0 == 0) {
        return;
      }
      pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
      *pdVar1 = *pdVar1 & 0xffdfffff;
      return;
    }
    piVar3 = (int *)this->mbr_0xd4;
    if ((((piVar3 == (int *)0x0) || (param_2 < (int)this->mbr_0x140)) ||
        (in_stack_0000000c < (int)this->mbr_0x144)) ||
       (((int)(*piVar3 + this->mbr_0x140) <= param_2 ||
        ((int)(piVar3[1] + this->mbr_0x144) <= in_stack_0000000c)))) {
      if ((piVar3 == (int *)0x0) || ((uVar2 & 0x80000) == 0)) goto LAB_0042f426;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xfff7ffff;
      uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
    }
    else {
      if ((uVar2 & 0x80000) == 0x80000) goto LAB_0042f426;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x80000;
      uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
    }
    meth_0x42e7d0(this,uVar6);
LAB_0042f426:
    meth_0x42e7d0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    return;
  }
  piVar3 = (int *)this->mbr_0xf0;
  if (((piVar3 == (int *)0x0) || (param_2 < (int)this->mbr_0x148)) ||
     ((in_stack_0000000c < (int)this->mbr_0x14c ||
      (((int)(*piVar3 + this->mbr_0x148) <= param_2 ||
       ((int)(piVar3[1] + this->mbr_0x14c) <= in_stack_0000000c)))))) {
    if ((piVar3 == (int *)0x0) || ((uVar2 & 0x100000) == 0)) goto LAB_0042f383;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xffefffff;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  else {
    if ((uVar2 & 0x100000) == 0x100000) goto LAB_0042f383;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x100000;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  meth_0x42e980(this,uVar6);
LAB_0042f383:
  meth_0x42e7d0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
  return;
}



// Function at 00438330

cls_0x5a3cd8 * __thiscall OOAnalyzer::cls_0x5a3cd8::virt_meth_0x438330(cls_0x5a3cd8 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3cd8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00438350

cls_0x5a3cd8 * __thiscall OOAnalyzer::cls_0x5a3cd8::~cls_0x5a3cd8(cls_0x5a3cd8 *this)

{
  cls_0x5a3cd8 *pcVar1;
  
  pcVar1 = (cls_0x5a3cd8 *)(this->cls_0x5a3ab8).mbr_0x5c;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar1 != (cls_0x5a3cd8 *)0x0) {
    pcVar1 = (cls_0x5a3cd8 *)FUN_00482f80(pcVar1);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  return pcVar1;
}



// Function at 00438380

cls_0x5a3d44 * __thiscall
OOAnalyzer::cls_0x5a3cd8::virt_meth_0x438380(cls_0x5a3cd8 *this,undefined4 param_1)

{
  cls_0x5a3d44 *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ccdb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3d44 *)FUN_00482fb0(0x1d0,this);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3d44 *)0x0) {
    pcVar1 = cls_0x5a3d44::cls_0x5a3d44(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3d44 *)0x0;
}



