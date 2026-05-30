// Decompiled methods and structure for class: cls_0x5a3db0

/*
/OOAnalyzer/cls_0x5a3db0
pack(disabled)
Structure cls_0x5a3db0 {
   0   cls_0x5a3ab8   144   cls_0x5a3ab8   "Component (member) class."
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
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
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
   324   dword   4   mbr_0x144   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 328 Alignment: 1

*/

// Function at 00431ca0

cls_0x5a3db0 * __thiscall
OOAnalyzer::cls_0x5a3db0::cls_0x5a3db0(cls_0x5a3db0 *this,int *param_1,uint *param_2)

{
  uint uVar1;
  dword dVar2;
  dword dVar3;
  char *_Source;
  size_t _Count;
  LPCVOID pvVar4;
  dword *pdVar5;
  bool bVar6;
  int iVar7;
  undefined3 extraout_var;
  dword dVar8;
  undefined4 *_Dest;
  Alignment *in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cb24;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,7);
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3db0__vftable_5a3db0_005a3db0;
  this->mbr_0x94 = 0;
  this->mbr_0x90 = 0;
  if ((uVar1 & 1) == 0) {
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 1;
    if ((in_stack_0000000c == (Alignment *)0x0) &&
       (in_stack_0000000c = (Alignment *)0x655ab8, (uVar1 & 0x40000) == 0)) {
      in_stack_0000000c = &DAT_00656180;
    }
    iVar7 = cls_0x431750::meth_0x431750_DefWidget_EDIT_attr_parser((cls_0x431750 *)&this->mbr_0xb8,in_stack_0000000c,param_1);
    if (iVar7 != 0) {
      bVar6 = FUN_00479700((uint)param_2,(byte *)s_MAXLEN_005cd76c,0);
      if (CONCAT31(extraout_var,bVar6) == 0) {
        this->mbr_0x98 = 0x14;
      }
      else {
        FUN_00479580((byte *)param_2);
        iVar7 = FUN_0047a410(param_2,(float **)&DAT_005cd774);
        if (iVar7 == 0) {
          (**(code **)(*param_1 + 0x9c))(s_MAXLEN_005cd778);
          ExceptionList = local_c;
          return this;
        }
      }
      this->mbr_0xa0 = this->mbr_0x110;
      dVar2 = (this->cls_0x5a3ab8).mbr_0x60;
      if (this->mbr_0x12c == 0) {
        dVar8 = this->mbr_0x124;
      }
      else {
        dVar8 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x124;
      }
      dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
      this->mbr_0xb0 = dVar8 + dVar2;
      this->mbr_0xb4 = this->mbr_0x128 + dVar3;
      if (this->mbr_0x140 == 0) {
        dVar8 = this->mbr_0x138;
      }
      else {
        dVar8 = (this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x138;
      }
      this->mbr_0xa8 = dVar8 + dVar2;
      this->mbr_0xac = this->mbr_0x13c + dVar3;
      _Dest = FUN_00482ef0(this->mbr_0x98 + 1);
      _Source = (char *)(this->cls_0x5a3ab8).mbr_0x5c;
      if (_Source == (char *)0x0) {
        *(undefined *)_Dest = 0;
      }
      else {
        _Count = this->mbr_0x98;
        _strncpy((char *)_Dest,_Source,_Count);
        pvVar4 = (LPCVOID)(this->cls_0x5a3ab8).mbr_0x5c;
        *(undefined *)(_Count + (int)_Dest) = 0;
        FUN_00482f80(pvVar4);
      }
      pdVar5 = (dword *)this->mbr_0xb8;
      (this->cls_0x5a3ab8).mbr_0x5c = (dword)_Dest;
      if (pdVar5 != (dword *)0x0) {
        if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
          (this->cls_0x5a3ab8).mbr_0x60 = -pdVar5[2];
          (this->cls_0x5a3ab8).mbr_0x64 = -pdVar5[3];
        }
        if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
          dVar2 = pdVar5[1];
          (this->cls_0x5a3ab8).mbr_0x68 = *pdVar5;
          (this->cls_0x5a3ab8).mbr_0x6c = dVar2;
        }
      }
      (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 & 0xfffffffe;
    }
  }
  ExceptionList = local_c;
  return this;
}



// Function at 00432030

void __thiscall OOAnalyzer::cls_0x5a3db0::meth_0x432030(cls_0x5a3db0 *this,cls_0x5a68d8 *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  int in_stack_00000008;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if (((((uVar1 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) && ((uVar1 & 4) == 0)) &&
     (puVar2 = (undefined4 *)this->mbr_0x11c, puVar2 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0xb0,this->mbr_0xb4,*puVar2,puVar2[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x2000000) == 0) {
      dVar6 = this->mbr_0x144;
      dVar5 = this->mbr_0x11c;
      dVar4 = this->mbr_0xb4;
      dVar3 = this->mbr_0xb0;
    }
    else {
      dVar6 = this->mbr_0x144;
      dVar5 = this->mbr_0x120;
      dVar4 = this->mbr_0xb4;
      dVar3 = this->mbr_0xb0;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar3,dVar4,dVar5,dVar6);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 004321f0

void __thiscall OOAnalyzer::cls_0x5a3db0::meth_0x4321f0(cls_0x5a3db0 *this,cls_0x5a68d8 *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  int in_stack_00000008;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  
  uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
  if (((((uVar1 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) && ((uVar1 & 4) == 0)) &&
     (puVar2 = (undefined4 *)this->mbr_0x130, puVar2 != (undefined4 *)0x0)) {
    if (in_stack_00000008 == 0) {
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)
                (param_1,this->mbr_0xa8,this->mbr_0xac,*puVar2,puVar2[1]);
    }
    if (((this->cls_0x5a3ab8).mbr_0x14 & 0x8000000) == 0) {
      dVar6 = this->mbr_0x144;
      dVar5 = this->mbr_0x130;
      dVar4 = this->mbr_0xac;
      dVar3 = this->mbr_0xa8;
    }
    else {
      dVar6 = this->mbr_0x144;
      dVar5 = this->mbr_0x134;
      dVar4 = this->mbr_0xac;
      dVar3 = this->mbr_0xa8;
    }
    cls_0x5a68d8::meth_0x4bd680(param_1,dVar3,dVar4,dVar5,dVar6);
    if (in_stack_00000008 == 0) {
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 00432320

void __thiscall OOAnalyzer::cls_0x5a3db0::meth_0x432320(cls_0x5a3db0 *this)

{
  uint uVar1;
  uint in_stack_00000004;
  
  if (this->mbr_0x130 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != (uVar1 >> 0x1b & 1)) {
      if (in_stack_00000004 != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x8000000;
        meth_0x4321f0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
        return;
      }
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xf7ffffff;
      meth_0x4321f0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    }
  }
  return;
}



// Function at 00432380

void __thiscall OOAnalyzer::cls_0x5a3db0::meth_0x432380(cls_0x5a3db0 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  if (this->mbr_0x130 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if (in_stack_00000004 != 0) {
      (this->cls_0x5a3ab8).mbr_0x14 = uVar1 | 0x10000000;
      return;
    }
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xefffffff;
  }
  return;
}



// Function at 004326a0

void __thiscall OOAnalyzer::cls_0x5a3db0::virt_meth_0x4326a0(cls_0x5a3db0 *this)

{
  size_t _Count;
  char *_Dest;
  cls_0x5a3ab8__vftable_5a3ab8 *pcVar1;
  char *in_stack_00000004;
  
  _Count = this->mbr_0x98;
  _Dest = (char *)(this->cls_0x5a3ab8).mbr_0x5c;
  _strncpy(_Dest,in_stack_00000004,_Count);
  pcVar1 = (this->cls_0x5a3ab8).vftptr_0x0;
  _Dest[_Count] = '\0';
  (*pcVar1->virt_meth_0x42a770_28)(this);
  return;
}



// Function at 004326e0

void __thiscall OOAnalyzer::cls_0x5a3db0::meth_0x4326e0(cls_0x5a3db0 *this,cls_0x5a68d8 *param_1)

{
  char cVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  dword dVar8;
  dword dVar9;
  bool bVar10;
  undefined4 *puVar11;
  dword dVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  undefined4 *puVar16;
  char *pcVar17;
  char *pcVar18;
  undefined4 *puVar19;
  int in_stack_00000008;
  cls_0x419dd0 cStack_ac;
  uint uStack_a8;
  undefined4 local_80 [32];
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    uVar13 = 0xffffffff;
    pcVar17 = (char *)(this->cls_0x5a3ab8).mbr_0x5c;
    do {
      if (uVar13 == 0) break;
      uVar13 = uVar13 - 1;
      cVar1 = *pcVar17;
      pcVar17 = pcVar17 + 1;
    } while (cVar1 != '\0');
    bVar10 = (int)(~uVar13 - 1) < 0x7d;
    if (bVar10) {
      puVar11 = local_80;
    }
    else {
      uStack_a8 = 0x43271d;
      puVar11 = FUN_00482fb0(~uVar13 + 1);
    }
    uVar13 = 0xffffffff;
    pcVar17 = (char *)(this->cls_0x5a3ab8).mbr_0x5c;
    do {
      pcVar18 = pcVar17;
      if (uVar13 == 0) break;
      uVar13 = uVar13 - 1;
      pcVar18 = pcVar17 + 1;
      cVar1 = *pcVar17;
      pcVar17 = pcVar18;
    } while (cVar1 != '\0');
    uVar13 = ~uVar13;
    puVar16 = (undefined4 *)(pcVar18 + -uVar13);
    puVar19 = puVar11;
    for (uVar14 = uVar13 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
      *puVar19 = *puVar16;
      puVar16 = puVar16 + 1;
      puVar19 = puVar19 + 1;
    }
    dVar2 = this->mbr_0x90;
    for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
      *(undefined *)puVar19 = *(undefined *)puVar16;
      puVar16 = (undefined4 *)((int)puVar16 + 1);
      puVar19 = (undefined4 *)((int)puVar19 + 1);
    }
    if ((dVar2 != 0) && ((int)this->mbr_0x9c < 0x18)) {
      iVar15 = -1;
      puVar16 = puVar11;
      do {
        puVar19 = puVar16;
        if (iVar15 == 0) break;
        iVar15 = iVar15 + -1;
        puVar19 = (undefined4 *)((int)puVar16 + 1);
        cVar1 = *(char *)puVar16;
        puVar16 = puVar19;
      } while (cVar1 != '\0');
      *(undefined2 *)((int)puVar19 + -1) = DAT_005cd790;
    }
    dVar2 = this->mbr_0xbc;
    dVar3 = (this->cls_0x5a3ab8).mbr_0x60;
    dVar4 = this->mbr_0xfc;
    dVar5 = this->mbr_0x100;
    dVar12 = (this->cls_0x5a3ab8).mbr_0x64 + this->mbr_0xc0 + dVar5;
    dVar6 = (this->cls_0x5a3ab8).mbr_0x6c;
    iVar15 = ((((this->cls_0x5a3ab8).mbr_0x68 - this->mbr_0x104) - this->mbr_0xc4) - dVar2) - dVar4;
    dVar7 = this->mbr_0x108;
    dVar8 = this->mbr_0xc8;
    dVar9 = this->mbr_0xc0;
    uStack_a8 = this->mbr_0x10c;
    uVar13 = (this->cls_0x5a3ab8).mbr_0x14;
    if ((uVar13 & 0x200000) == 0) {
      if ((uVar13 & 0x400000) != 0) {
        uStack_a8 = uStack_a8 & 0xfffffffc | 4;
      }
    }
    else {
      uStack_a8 = uStack_a8 & 0xfffffffa | 2;
    }
    if (*(char *)puVar11 != '\0') {
      cStack_ac.mbr_0x0 = uStack_a8;
      cls_0x419dd0::cls_0x419dd0(&cStack_ac);
      cls_0x5a68d8::meth_0x4be2b0_CompositeBuffer
                (param_1,(char)dVar3 + (char)dVar2 + (char)dVar4,(char)dVar12,(char)iVar15,
                 (((dVar6 - dVar7) - dVar8) - dVar9) - dVar5,(char)puVar11,0);
    }
    if (!bVar10) {
      uStack_a8 = 0x432882;
      FUN_004830f0(puVar11);
    }
    if (in_stack_00000008 == 0) {
      cStack_ac.mbr_0x0 = dVar12;
      uStack_a8 = iVar15;
      (*param_1->vftptr_0x0->virt_meth_0x4bcc60_32)(param_1);
    }
  }
  return;
}



// Function at 004328b0

void __thiscall OOAnalyzer::cls_0x5a3db0::virt_meth_0x4328b0(cls_0x5a3db0 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int *in_stack_00000004;
  
  if (((*(byte *)&(this->cls_0x5a3ab8).mbr_0x14 & 2) == 0) && ((this->cls_0x5a3ab8).mbr_0x8 != 0)) {
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a940_68)();
    meth_0x4326e0(this,in_stack_00000004);
    meth_0x432030(this,in_stack_00000004);
    meth_0x4321f0(this,in_stack_00000004);
    if (((this->cls_0x5a3ab8).mbr_0x78 == 0) || (((this->cls_0x5a3ab8).mbr_0x14 & 0x400) == 0)) {
      if (this->mbr_0xb8 == 0) {
        dVar1 = 10000;
        dVar2 = 10000;
        dVar3 = 10000;
        dVar4 = 10000;
      }
      else if (this->mbr_0xcc == 0) {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64;
        dVar2 = (this->cls_0x5a3ab8).mbr_0x68;
        dVar1 = (this->cls_0x5a3ab8).mbr_0x6c;
      }
      else {
        dVar4 = (this->cls_0x5a3ab8).mbr_0x60 - this->mbr_0xe4;
        dVar2 = this->mbr_0xec + (this->cls_0x5a3ab8).mbr_0x68 + this->mbr_0xe4;
        dVar3 = (this->cls_0x5a3ab8).mbr_0x64 - this->mbr_0xe8;
        dVar1 = this->mbr_0xf0 + (this->cls_0x5a3ab8).mbr_0x6c + this->mbr_0xe8;
      }
      (**(code **)(*in_stack_00000004 + 0x20))(dVar4,dVar3,dVar2,dVar1);
    }
    (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x42a770_28)(this);
  }
  return;
}



// Function at 00432d90

void __thiscall
OOAnalyzer::cls_0x5a3db0::virt_meth_0x432d90(cls_0x5a3db0 *this,undefined4 param_1,int param_2)

{
  dword *pdVar1;
  uint uVar2;
  int *piVar3;
  dword dVar4;
  dword dVar5;
  int in_stack_0000000c;
  undefined4 uVar6;
  
  if (this->mbr_0x94 == 0) {
    return;
  }
  uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
  if ((uVar2 & 0x10000000) == 0) {
    if ((uVar2 & 0x4000000) == 0) {
      dVar4 = (this->cls_0x5a3ab8).mbr_0x8;
      dVar5 = this->mbr_0x11c;
      this->mbr_0x94 = 0;
      *(undefined4 *)(dVar4 + 0xa4) = 0;
      if ((dVar5 != 0) && ((uVar2 & 0x2000000) != 0)) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xfdffffff;
        meth_0x432030(this,*(undefined4 *)(dVar4 + 100));
      }
      if (this->mbr_0x11c != 0) {
        pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
        *pdVar1 = *pdVar1 & 0xfbffffff;
      }
      if (this->mbr_0x130 == 0) {
        return;
      }
      uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
      if ((uVar2 & 0x8000000) != 0) {
        (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xf7ffffff;
        meth_0x4321f0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
      }
      if (this->mbr_0x130 == 0) {
        return;
      }
      pdVar1 = &(this->cls_0x5a3ab8).mbr_0x14;
      *pdVar1 = *pdVar1 & 0xefffffff;
      return;
    }
    piVar3 = (int *)this->mbr_0x11c;
    if ((((piVar3 == (int *)0x0) || (param_2 < (int)this->mbr_0xb0)) ||
        (in_stack_0000000c < (int)this->mbr_0xb4)) ||
       (((int)(*piVar3 + this->mbr_0xb0) <= param_2 ||
        ((int)(piVar3[1] + this->mbr_0xb4) <= in_stack_0000000c)))) {
      if ((piVar3 == (int *)0x0) || ((uVar2 & 0x2000000) == 0)) goto LAB_00432ed6;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xfdffffff;
      uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
    }
    else {
      if ((uVar2 & 0x2000000) == 0x2000000) goto LAB_00432ed6;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x2000000;
      uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
    }
    meth_0x432030(this,uVar6);
LAB_00432ed6:
    meth_0x432030(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    return;
  }
  piVar3 = (int *)this->mbr_0x130;
  if (((piVar3 == (int *)0x0) || (param_2 < (int)this->mbr_0xa8)) ||
     ((in_stack_0000000c < (int)this->mbr_0xac ||
      (((int)(*piVar3 + this->mbr_0xa8) <= param_2 ||
       ((int)(piVar3[1] + this->mbr_0xac) <= in_stack_0000000c)))))) {
    if ((piVar3 == (int *)0x0) || ((uVar2 & 0x8000000) == 0)) goto LAB_00432e33;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xf7ffffff;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  else {
    if ((uVar2 & 0x8000000) == 0x8000000) goto LAB_00432e33;
    (this->cls_0x5a3ab8).mbr_0x14 = uVar2 | 0x8000000;
    uVar6 = *(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100);
  }
  meth_0x4321f0(this,uVar6);
LAB_00432e33:
  meth_0x432030(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
  return;
}



// Function at 00432f70

void __thiscall OOAnalyzer::cls_0x5a3db0::virt_meth_0x432f70(cls_0x5a3db0 *this,int param_1)

{
  char *pcVar1;
  uint uVar2;
  dword dVar3;
  char cVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  int in_stack_00000008;
  
  if (this->mbr_0x90 == 0) {
    return;
  }
  if (in_stack_00000008 == 0) {
    return;
  }
  if (DAT_0065c9e0 != 0) {
    return;
  }
  pcVar1 = (char *)(this->cls_0x5a3ab8).mbr_0x5c;
  uVar5 = 0xffffffff;
  pcVar7 = pcVar1;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar4 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar4 != '\0');
  uVar5 = ~uVar5;
  iVar6 = uVar5 - 1;
  if (param_1 == 0xd) {
    uVar2 = (this->cls_0x5a3ab8).mbr_0x14;
    if ((uVar2 & 0x800000) == 0) {
      if ((uVar2 & 0x1000000) != 0) {
        cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
        return;
      }
      dVar3 = (this->cls_0x5a3ab8).mbr_0x8;
      this->mbr_0x90 = 0;
      *(undefined4 *)(dVar3 + 0xa0) = 0;
      *(undefined4 *)(dVar3 + 0xa8) = 0;
      *(undefined4 *)(dVar3 + 0xac) = 1;
      (this->cls_0x5a3ab8).mbr_0x14 = uVar2 & 0xfffffeff;
      cls_0x5a3c68::meth_0x42a820((cls_0x5a3c68 *)this);
      (*((this->cls_0x5a3ab8).vftptr_0x0)->virt_meth_0x4388c0_64)(this);
      return;
    }
  }
  else if (param_1 == 8) {
    if ((1 < iVar6) && ((pcVar1[uVar5 - 3] & 0x80U) != 0)) {
      dVar3 = (this->cls_0x5a3ab8).mbr_0x8;
      pcVar1[uVar5 - 2] = '\0';
      pcVar1[uVar5 - 3] = '\0';
      meth_0x4326e0(this,*(undefined4 *)(dVar3 + 100));
      return;
    }
    if (0 < iVar6) {
      pcVar1[uVar5 - 2] = '\0';
    }
    meth_0x4326e0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
    return;
  }
  if (((((this->cls_0x5a3ab8).mbr_0x14 & 0x100000) != 0) && (param_1 != 0x2d)) && (param_1 != 0x2e))
  {
    if (param_1 < 0x30) {
      return;
    }
    if (0x39 < param_1) {
      return;
    }
  }
  if (param_1 < 0x20) {
    if (param_1 != 0xd) {
      return;
    }
  }
  else if (param_1 != 0xd) goto LAB_004330a1;
  param_1 = 10;
LAB_004330a1:
  cVar4 = (char)((uint)param_1 >> 8);
  if (cVar4 == '\0') {
    if ((int)uVar5 <= (int)this->mbr_0x98) {
      pcVar1[iVar6] = (char)param_1;
      pcVar1[uVar5] = '\0';
    }
  }
  else if ((int)(uVar5 + 1) <= (int)this->mbr_0x98) {
    pcVar1[iVar6] = cVar4;
    pcVar1[uVar5] = (char)param_1;
    pcVar1[uVar5 + 1] = '\0';
  }
  meth_0x4326e0(this,*(undefined4 *)((this->cls_0x5a3ab8).mbr_0x8 + 100));
  return;
}



// Function at 004386d0

cls_0x5a3db0 * __thiscall OOAnalyzer::cls_0x5a3db0::virt_meth_0x4386d0(cls_0x5a3db0 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3db0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004386f0

cls_0x5a3db0 * __thiscall OOAnalyzer::cls_0x5a3db0::~cls_0x5a3db0(cls_0x5a3db0 *this)

{
  cls_0x5a3db0 *pcVar1;
  
  pcVar1 = (cls_0x5a3db0 *)(this->cls_0x5a3ab8).mbr_0x5c;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar1 != (cls_0x5a3db0 *)0x0) {
    pcVar1 = (cls_0x5a3db0 *)FUN_00482f80(pcVar1);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  return pcVar1;
}



// Function at 00438930

cls_0x5a3e1c * __thiscall
OOAnalyzer::cls_0x5a3db0::virt_meth_0x438930(cls_0x5a3db0 *this,undefined4 param_1)

{
  cls_0x5a3e1c *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cd1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3e1c *)FUN_00482fb0(0x240);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3e1c *)0x0) {
    pcVar1 = cls_0x5a3e1c::cls_0x5a3e1c(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3e1c *)0x0;
}



