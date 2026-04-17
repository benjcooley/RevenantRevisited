// Decompiled methods and structure for class: cls_0x588410

/*
/OOAnalyzer/cls_0x588410
pack(disabled)
Structure cls_0x588410 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   byte   1   mbr_0x14   "Unsigned Byte (db)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   49   byte   1   mbr_0x31   "Unsigned Byte (db)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 56 Alignment: 1

*/

// Function at 005707b0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588480(cls_0x588410 *this)

{
  if ((this->mbr_0x0 != 0) && (0 < (int)this->mbr_0x8)) {
    FUN_004830f0(this->mbr_0x0);
  }
  return;
}



// Function at 005707c0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588480(cls_0x588410 *this)

{
  if ((this->mbr_0x0 != 0) && (0 < (int)this->mbr_0x8)) {
    FUN_004830f0(this->mbr_0x0);
  }
  return;
}



// Function at 005725f0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5725f0(cls_0x588410 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  uVar1 = in_stack_00000004 + this->mbr_0xc;
  this->mbr_0xc = uVar1;
  if (this->mbr_0x4 < uVar1) {
    meth_0x5884a0(this);
  }
  return;
}



// Function at 0057bbb0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x57bbb0(cls_0x588410 *this)

{
  undefined *puVar1;
  undefined4 in_stack_00000004;
  
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 4);
  if ((undefined *)this->mbr_0x4 < puVar1 + 4) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  puVar1[1] = (char)((uint)in_stack_00000004 >> 8);
  puVar1[2] = in_stack_00000004._2_1_;
  puVar1[3] = in_stack_00000004._3_1_;
  return;
}



// Function at 0057bc30

void __thiscall OOAnalyzer::cls_0x588410::meth_0x57bc30(cls_0x588410 *this)

{
  undefined *puVar1;
  undefined2 in_stack_00000004;
  
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 2);
  if ((undefined *)this->mbr_0x4 < puVar1 + 2) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  puVar1[1] = (char)((ushort)in_stack_00000004 >> 8);
  return;
}



// Function at 0057c960

void __thiscall OOAnalyzer::cls_0x588410::meth_0x57c960(cls_0x588410 *this)

{
  uint uVar1;
  undefined *puVar2;
  
  puVar2 = (undefined *)this->mbr_0x0;
  this->mbr_0x10 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x20 = 0;
  this->mbr_0xc = (dword)(puVar2 + 1);
  if ((undefined *)this->mbr_0x4 < puVar2 + 1) {
    puVar2 = (undefined *)meth_0x5884a0(this);
  }
  *puVar2 = 7;
  meth_0x588570(this);
  uVar1 = this->mbr_0xc + 2;
  this->mbr_0xc = uVar1;
  if (this->mbr_0x4 < uVar1) {
    meth_0x5884a0(this);
  }
  this->mbr_0x31 = 2;
  return;
}



// Function at 0057c9c0

undefined __thiscall OOAnalyzer::cls_0x588410::meth_0x57c9c0(cls_0x588410 *this,int param_1)

{
  dword dVar1;
  char cVar2;
  dword in_stack_00000008;
  
  if (param_1 != 0) {
    dVar1 = *(dword *)(param_1 + 0x40);
    if (dVar1 == 0) {
      return 0;
    }
    if (dVar1 == this->mbr_0x1c) {
      if ((int)in_stack_00000008 <= (int)this->mbr_0x24) {
        return 1;
      }
      if (this->mbr_0x20 == this->mbr_0xc - this->mbr_0x0) {
        meth_0x5885b0(this);
      }
    }
    cVar2 = meth_0x57d390(this);
    if (cVar2 == '\0') {
      this->mbr_0x20 = 0xffffffff;
      return 0;
    }
    this->mbr_0x1c = dVar1;
    this->mbr_0x24 = in_stack_00000008;
    this->mbr_0x20 = this->mbr_0xc - this->mbr_0x0;
  }
  return 1;
}



// Function at 0057ca40

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x57ca40(cls_0x588410 *this)

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  undefined *puVar4;
  ushort uVar5;
  ushort uVar6;
  cls_0x5b4f30 *in_stack_00000004;
  undefined4 local_4;
  
  uVar5 = 0;
  local_4 = -1;
  if (((in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 != 0) &&
     ((((iVar3 = cls_0x5a7b98::meth_0x45f770(&in_stack_00000004->cls_0x5a7b98), iVar3 != 0 &&
        ((iVar3 = FUN_004dab80(s_combatrun_005e0618), iVar3 != 0 ||
         (iVar3 = FUN_004dab80(s_handrun_005e0610), iVar3 != 0)))) ||
       ((piVar1 = (int *)(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0,
        piVar1 != (int *)0x0 &&
        ((*piVar1 == 0x19 && (iVar3 = FUN_004dab80(s_bowrun_005e0608), iVar3 != 0)))))) ||
      (iVar3 = FUN_004dab80(&DAT_005e0604), iVar3 != 0)))) {
    uVar5 = 1;
  }
  piVar1 = (int *)(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xd8;
  if ((piVar1 != (int *)0x0) && (((iVar3 = *piVar1, iVar3 == 2 || (iVar3 == 4)) || (iVar3 == 0x1a)))
     ) {
    uVar5 = uVar5 | 4;
  }
  piVar1 = (int *)(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0;
  if ((piVar1 != (int *)0x0) && (*piVar1 == 0x19)) {
    uVar5 = uVar5 | 0x20;
  }
  iVar3 = cls_0x5a7b98::meth_0x45f770(&in_stack_00000004->cls_0x5a7b98);
  if ((iVar3 != 0) ||
     ((piVar1 = (int *)(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0, uVar6 = uVar5,
      piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) {
    iVar3 = cls_0x5a7b98::meth_0x45f770(&in_stack_00000004->cls_0x5a7b98);
    if (((iVar3 != 0) ||
        ((piVar1 = (int *)(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0,
         piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) &&
       (iVar3 = *(int *)((in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 + 0x44), iVar3 != 0
       )) {
      local_4 = *(int *)(iVar3 + 0x40);
    }
    uVar6 = uVar5 | 8;
    if (local_4 != -1) {
      uVar6 = uVar5 | 0x18;
    }
  }
  if (DAT_0067682c != 0) {
    if (*(short *)&(in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0x4 == 0xb) {
      if ((*(byte *)&(in_stack_00000004->cls_0x5a7b98).mbr_0x36c & 4) != 0) {
        uVar6 = uVar6 | 0x40;
      }
      if ((*(byte *)&(in_stack_00000004->cls_0x5a7b98).mbr_0x36c & 8) != 0) {
        uVar6 = uVar6 | 0x80;
      }
      if ((*(byte *)&(in_stack_00000004->cls_0x5a7b98).mbr_0x36c & 0x10) == 0) {
        uVar6 = uVar6 | 0x100;
      }
    }
    iVar3 = cls_0x5b4f30::meth_0x4d5790(in_stack_00000004);
    if (iVar3 == 0) {
      uVar6 = uVar6 | 0x200;
    }
  }
  puVar4 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar4 + 2);
  if ((undefined *)this->mbr_0x4 < puVar4 + 2) {
    puVar4 = (undefined *)meth_0x5884a0(this);
  }
  *puVar4 = (char)uVar6;
  puVar4[1] = (char)(uVar6 >> 8);
  if ((uVar6 & 4) != 0) {
    puVar4 = (undefined *)this->mbr_0xc;
    dVar2 = (in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.mbr_0xb0;
    this->mbr_0xc = (dword)(puVar4 + 1);
    if ((undefined *)this->mbr_0x4 < puVar4 + 1) {
      puVar4 = (undefined *)meth_0x5884a0(this);
    }
    *puVar4 = (char)dVar2;
  }
  if (local_4 != -1) {
    puVar4 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar4 + 4);
    if ((undefined *)this->mbr_0x4 < puVar4 + 4) {
      puVar4 = (undefined *)meth_0x5884a0(this);
    }
    *puVar4 = (char)local_4;
    puVar4[1] = (char)((uint)local_4 >> 8);
    puVar4[2] = local_4._2_1_;
    puVar4[3] = local_4._3_1_;
  }
  (*((in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e50_448)
            (in_stack_00000004);
  meth_0x588df0(this);
  (*((in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e70_456)
            (in_stack_00000004);
  meth_0x588df0(this);
  (*((in_stack_00000004->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e90_464)
            (in_stack_00000004);
  meth_0x588df0(this);
  return 1;
}



// Function at 0057d200

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x57d200(cls_0x588410 *this)

{
  uint uVar1;
  undefined4 uVar2;
  cls_0x5a7b98 *this_00;
  undefined uVar3;
  undefined uVar4;
  undefined2 uVar5;
  undefined *puVar6;
  undefined3 extraout_var;
  cls_0x5a7b98 *in_stack_00000004;
  cls_0x477870 local_1c;
  
  if (in_stack_00000004 != (cls_0x5a7b98 *)0x0) {
    puVar6 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar6 + 1);
    if ((undefined *)this->mbr_0x4 < puVar6 + 1) {
      puVar6 = (undefined *)meth_0x5884a0(this);
    }
    *puVar6 = 0x2f;
    meth_0x588570(this);
    uVar1 = this->mbr_0xc + 2;
    this->mbr_0xc = uVar1;
    if (this->mbr_0x4 < uVar1) {
      meth_0x5884a0(this);
    }
    uVar5 = FUN_00585cc0(in_stack_00000004);
    puVar6 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar6 + 2);
    if ((undefined *)this->mbr_0x4 < puVar6 + 2) {
      puVar6 = (undefined *)meth_0x5884a0(this);
    }
    *puVar6 = (char)uVar5;
    puVar6[1] = (char)((ushort)uVar5 >> 8);
    uVar3 = cls_0x5a7b98::meth_0x470040(in_stack_00000004);
    puVar6 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar6 + 1);
    if ((undefined *)this->mbr_0x4 < puVar6 + 1) {
      puVar6 = (undefined *)meth_0x5884a0(this);
    }
    *puVar6 = uVar3;
    local_1c.mbr_0x0 = 1;
    local_1c.mbr_0x10 = 0;
    local_1c.mbr_0xc = 0;
    local_1c.mbr_0x14 = 0;
    local_1c.mbr_0x18 = 0;
    cls_0x477870::meth_0x46dfb0(&local_1c);
    this_00 = (cls_0x5a7b98 *)local_1c.mbr_0x18;
    while (local_1c.mbr_0x18 = (dword)this_00, this_00 != (cls_0x5a7b98 *)0x0) {
      puVar6 = (undefined *)this->mbr_0xc;
      uVar2 = *(undefined4 *)&(this_00->cls_0x5a50e8).field_0x40;
      this->mbr_0xc = (dword)(puVar6 + 4);
      if ((undefined *)this->mbr_0x4 < puVar6 + 4) {
        puVar6 = (undefined *)meth_0x5884a0(this);
      }
      in_stack_00000004._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar6 = (char)uVar2;
      in_stack_00000004._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar6[1] = (char)((uint)uVar2 >> 8);
      puVar6[2] = in_stack_00000004._2_1_;
      puVar6[3] = in_stack_00000004._3_1_;
      FUN_00584fd0(this_00,this);
      uVar3 = cls_0x5a7b98::meth_0x470040(this_00);
      uVar4 = (*((this_00->cls_0x5a50e8).vftptr_0x0)->virt_meth_0x477d50_368)(this_00);
      if (CONCAT31(extraout_var,uVar4) != 0) {
        uVar3 = 0;
      }
      puVar6 = (undefined *)this->mbr_0xc;
      this->mbr_0xc = (dword)(puVar6 + 1);
      if ((undefined *)this->mbr_0x4 < puVar6 + 1) {
        puVar6 = (undefined *)meth_0x5884a0(this);
      }
      *puVar6 = uVar3;
      cls_0x477870::meth_0x46dfb0(&local_1c);
      this_00 = (cls_0x5a7b98 *)local_1c.mbr_0x18;
    }
    meth_0x588a50(this);
    return 1;
  }
  return 0;
}



// Function at 0057d390

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x57d390(cls_0x588410 *this)

{
  short sVar1;
  undefined4 uVar2;
  dword dVar3;
  cls_0x5a7b98 *this_00;
  undefined uVar4;
  undefined2 uVar5;
  undefined *puVar6;
  undefined3 extraout_var;
  char *pcVar7;
  undefined *puVar8;
  int iVar9;
  undefined2 extraout_var_00;
  char cVar10;
  cls_0x5a50e8__vftable_5a50e8 *pcVar11;
  dword *pdVar12;
  undefined uStack00000004;
  undefined uStack00000006;
  undefined uStack00000007;
  int in_stack_00000008;
  undefined4 *puVar13;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  uint uStack_c;
  uint uStack_8;
  uint uStack_4;
  
  this_00 = _uStack00000004;
  if (_uStack00000004 == (cls_0x5a7b98 *)0x0) {
    puVar6 = (undefined *)meth_0x5725f0(this);
    *puVar6 = 5;
    return 1;
  }
  uVar4 = (*((_uStack00000004->cls_0x5a50e8).vftptr_0x0)->virt_meth_0x477a10_180)(_uStack00000004);
  iVar9 = in_stack_00000008;
  if (CONCAT31(extraout_var,uVar4) != 0) {
    return 0;
  }
  sVar1 = *(short *)&(_uStack00000004->cls_0x5a50e8).mbr_0x4;
  _uStack00000004 = (cls_0x5a7b98 *)0xffffffff;
  if (in_stack_00000008 == 2) {
    if (sVar1 == 0xb) {
      pcVar11 = this_00[1].cls_0x5a50e8.vftptr_0x0;
      if ((int)pcVar11 < 0) {
        return 0;
      }
      puVar6 = (undefined *)meth_0x5725f0(this);
      *puVar6 = 0xe;
      puVar6 = (undefined *)meth_0x5725f0(this);
      *puVar6 = (char)pcVar11;
    }
    else {
      pcVar7 = (char *)meth_0x5725f0(this);
      *pcVar7 = ((sVar1 != 0xc) - 1U & 3) + 8;
      uVar5 = FUN_004515c0(this_00);
      puVar8 = (undefined *)meth_0x5725f0(this);
      puVar6 = (undefined *)this->mbr_0x4;
      *puVar8 = (char)uVar5;
      uVar4 = *(undefined *)&(this_00->cls_0x5a50e8).mbr_0xe;
      puVar8[1] = (char)((ushort)uVar5 >> 8);
      puVar8 = (undefined *)this->mbr_0xc;
      this->mbr_0xc = (dword)(puVar8 + 1);
      if (puVar6 < puVar8 + 1) {
        puVar8 = (undefined *)meth_0x5884a0(this);
      }
      *puVar8 = uVar4;
      pcVar11 = (cls_0x5a50e8__vftable_5a50e8 *)_uStack00000004;
    }
    FUN_00584f30(this_00,this,(int)pcVar11 < 0 | 0x16);
    iVar9 = cls_0x5a7b98::meth_0x470040(this_00);
    if (iVar9 < 1) {
      return 1;
    }
    meth_0x57d200(this);
    return 1;
  }
  cVar10 = (char)in_stack_00000008;
  if (sVar1 == 0xc) {
    pcVar7 = (char *)this->mbr_0xc;
    this->mbr_0xc = (dword)(pcVar7 + 1);
    if ((char *)this->mbr_0x4 < pcVar7 + 1) {
      pcVar7 = (char *)meth_0x5884a0(this);
    }
    puVar6 = (undefined *)this->mbr_0x4;
    *pcVar7 = cVar10 + '\t';
    puVar8 = (undefined *)this->mbr_0xc;
    uVar2 = *(undefined4 *)&(this_00->cls_0x5a50e8).field_0x40;
    this->mbr_0xc = (dword)(puVar8 + 4);
    if (puVar6 < puVar8 + 4) {
      puVar8 = (undefined *)meth_0x5884a0(this);
    }
    uStack_18._2_1_ = (undefined)((uint)uVar2 >> 0x10);
    uStack_18._3_1_ = (undefined)((uint)uVar2 >> 0x18);
    *puVar8 = (char)uVar2;
    puVar8[1] = (char)((uint)uVar2 >> 8);
    puVar8[2] = uStack_18._2_1_;
    puVar8[3] = uStack_18._3_1_;
    iVar9 = in_stack_00000008;
  }
  else {
    if (sVar1 != 0xb) {
      pcVar7 = (char *)this->mbr_0xc;
      this->mbr_0xc = (dword)(pcVar7 + 1);
      if ((char *)this->mbr_0x4 < pcVar7 + 1) {
        pcVar7 = (char *)meth_0x5884a0(this);
      }
      puVar6 = (undefined *)this->mbr_0x4;
      *pcVar7 = cVar10 + '\x06';
      puVar8 = (undefined *)this->mbr_0xc;
      pcVar11 = *(cls_0x5a50e8__vftable_5a50e8 **)&(this_00->cls_0x5a50e8).field_0x40;
      this->mbr_0xc = (dword)(puVar8 + 4);
      _uStack00000004 = (cls_0x5a7b98 *)pcVar11;
      if (puVar6 < puVar8 + 4) {
        puVar8 = (undefined *)meth_0x5884a0(this);
      }
      *puVar8 = (char)pcVar11;
      puVar8[1] = (char)((uint)pcVar11 >> 8);
      puVar8[2] = uStack00000006;
      puVar8[3] = uStack00000007;
      if (in_stack_00000008 != 1) {
        return 1;
      }
      goto LAB_0057d513;
    }
    _uStack00000004 = (cls_0x5a7b98 *)this_00[1].cls_0x5a50e8.vftptr_0x0;
    if ((int)_uStack00000004 < 0) {
      return 0;
    }
    pcVar7 = (char *)this->mbr_0xc;
    this->mbr_0xc = (dword)(pcVar7 + 1);
    if ((char *)this->mbr_0x4 < pcVar7 + 1) {
      pcVar7 = (char *)meth_0x5884a0(this);
    }
    *pcVar7 = cVar10 + '\f';
    puVar6 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar6 + 1);
    if ((undefined *)this->mbr_0x4 < puVar6 + 1) {
      puVar6 = (undefined *)meth_0x5884a0(this);
    }
    *puVar6 = uStack00000004;
  }
  if (iVar9 != 1) {
    return 1;
  }
  meth_0x57ca40(this);
  if (-1 < (int)_uStack00000004) {
    cls_0x56fe30::cls_0x56fe30((cls_0x56fe30 *)&stack0x00000004);
    pdVar12 = &this_00->mbr_0x2a0;
    uStack_18 = 0xb;
    do {
      dVar3 = *pdVar12;
      if (dVar3 == 0) {
        puVar13 = &stack0x00000008;
        uVar4 = 1;
        in_stack_00000008 = CONCAT31(in_stack_00000008._1_3_,0xfe);
      }
      else {
        uStack_14 = *(undefined4 *)(*(int *)(dVar3 + 0x4c) + 0x1c);
        cls_0x56fe30::meth_0x56fe90((cls_0x56fe30 *)&stack0x00000004,&uStack_14,4);
        uStack_10 = CONCAT22(extraout_var_00,*(undefined2 *)(dVar3 + 0xc));
        puVar13 = &uStack_10;
        uVar4 = 2;
      }
      cls_0x56fe30::meth_0x56fe90((cls_0x56fe30 *)&stack0x00000004,puVar13,uVar4);
      pcVar11 = (cls_0x5a50e8__vftable_5a50e8 *)_uStack00000004;
      pdVar12 = pdVar12 + 1;
      uStack_18 = uStack_18 + -1;
    } while (uStack_18 != 0);
    puVar6 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar6 + 2);
    if ((undefined *)this->mbr_0x4 < puVar6 + 2) {
      puVar6 = (undefined *)meth_0x5884a0(this);
    }
    *puVar6 = (char)pcVar11;
    puVar6[1] = (char)((uint)pcVar11 >> 8);
  }
LAB_0057d513:
  uStack_c = (this_00->cls_0x5a50e8).mbr_0x14 << 0x10 | (this_00->cls_0x5a50e8).mbr_0x10 & 0xffff;
  uStack_4 = uStack_4 ^ ((this_00->cls_0x5a50e8).mbr_0xb0 ^ uStack_4) & 0xff;
  uStack_8 = ((this_00->cls_0x5a50e8).mbr_0xe & 0xff) << 0x10 |
             (this_00->cls_0x5a50e8).mbr_0x18 & 0xffff |
             (uint)(this_00->cls_0x5a50e8).mbr_0x36 << 0x18;
  meth_0x5886d0(this,&uStack_c);
  return 1;
}



// Function at 00588150

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x588150(cls_0x588410 *this)

{
  int iVar1;
  int *in_stack_00000004;
  
  iVar1 = this->mbr_0xc - this->mbr_0x0;
  return CONCAT31((int3)((uint)iVar1 >> 8),iVar1 < *in_stack_00000004);
}



// Function at 00588170

int __thiscall OOAnalyzer::cls_0x588410::meth_0x588170(cls_0x588410 *this)

{
  return this->mbr_0xc - this->mbr_0x0;
}



// Function at 00588180

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588180(cls_0x588410 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  uVar1 = this->mbr_0x0 + in_stack_00000004;
  this->mbr_0xc = uVar1;
  if (this->mbr_0x4 < uVar1) {
    meth_0x5884a0(this);
  }
  return;
}



// Function at 005881a0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5881a0(cls_0x588410 *this)

{
  undefined *puVar1;
  undefined4 in_stack_00000004;
  
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 4);
  if ((undefined *)this->mbr_0x4 < puVar1 + 4) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  puVar1[1] = (char)((uint)in_stack_00000004 >> 8);
  puVar1[2] = in_stack_00000004._2_1_;
  puVar1[3] = in_stack_00000004._3_1_;
  return;
}



// Function at 005881e0

undefined __thiscall OOAnalyzer::cls_0x588410::meth_0x5881e0(cls_0x588410 *this)

{
  undefined *puVar1;
  
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 00588200

undefined __thiscall OOAnalyzer::cls_0x588410::meth_0x588200(cls_0x588410 *this)

{
  undefined *puVar1;
  
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 00588220

undefined2 __thiscall OOAnalyzer::cls_0x588410::meth_0x588220(cls_0x588410 *this)

{
  undefined2 *puVar1;
  
  puVar1 = (undefined2 *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined2 *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined2 *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 00588250

undefined2 __thiscall OOAnalyzer::cls_0x588410::meth_0x588250(cls_0x588410 *this)

{
  undefined2 *puVar1;
  
  puVar1 = (undefined2 *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined2 *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined2 *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 00588280

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x588280(cls_0x588410 *this)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined4 *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined4 *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 005882c0

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x5882c0(cls_0x588410 *this)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined4 *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined4 *)meth_0x5884a0(this);
  }
  return *puVar1;
}



// Function at 00588410

cls_0x588410 * __thiscall OOAnalyzer::cls_0x588410::cls_0x588410(cls_0x588410 *this)

{
  dword dVar1;
  
  this->mbr_0x8 = 0x100;
  dVar1 = FUN_00482fb0(0x100);
  this->mbr_0x0 = dVar1;
  this->mbr_0xc = dVar1;
  this->mbr_0x4 = dVar1 + this->mbr_0x8;
  this->mbr_0x10 = 0;
  return this;
}



// Function at 00588450

cls_0x588410 * __thiscall OOAnalyzer::cls_0x588410::cls_0x588410(cls_0x588410 *this,dword param_1)

{
  int in_stack_00000008;
  
  this->mbr_0x8 = -in_stack_00000008;
  this->mbr_0x0 = param_1;
  this->mbr_0x4 = in_stack_00000008 + param_1;
  this->mbr_0xc = param_1;
  this->mbr_0x10 = 0;
  return this;
}



// Function at 00588480

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588480(cls_0x588410 *this)

{
  if ((this->mbr_0x0 != 0) && (0 < (int)this->mbr_0x8)) {
    FUN_004830f0(this->mbr_0x0);
  }
  return;
}



// Function at 005884a0

undefined * __thiscall OOAnalyzer::cls_0x588410::meth_0x5884a0(cls_0x588410 *this)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int in_stack_00000004;
  CHAR local_100 [256];
  
  uVar5 = (this->mbr_0xc - this->mbr_0x0) + 0x100 & 0xffffff00;
  if ((int)DAT_00676ecc < (int)uVar5) {
    DAT_00676ecc = uVar5;
    FUN_0058b100(local_100,s_Netpack__Largest_Size____d_005e65d4,uVar5);
    OutputDebugStringA(local_100);
  }
  puVar2 = (undefined4 *)FUN_00482fb0(uVar5);
  puVar1 = (undefined4 *)this->mbr_0x0;
  this->mbr_0xc = (dword)((int)puVar2 + (this->mbr_0xc - (int)puVar1));
  if (this->mbr_0x10 != 0) {
    this->mbr_0x10 = (dword)((this->mbr_0x10 - (int)puVar1) + (int)puVar2);
  }
  uVar4 = this->mbr_0x8;
  puVar6 = puVar1;
  puVar7 = puVar2;
  for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar7 = *puVar6;
    puVar6 = puVar6 + 1;
    puVar7 = puVar7 + 1;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar7 = *(undefined *)puVar6;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  }
  FUN_004830f0(puVar1);
  this->mbr_0x0 = (dword)puVar2;
  this->mbr_0x4 = (dword)((int)puVar2 + uVar5);
  this->mbr_0x8 = uVar5;
  return (undefined *)((int)puVar2 + (in_stack_00000004 - (int)puVar1));
}



// Function at 00588570

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588570(cls_0x588410 *this)

{
  byte bVar1;
  dword dVar2;
  int *in_stack_00000004;
  
  dVar2 = this->mbr_0x0;
  *in_stack_00000004 = this->mbr_0xc - dVar2;
  if (this->mbr_0x10 != 0) {
    bVar1 = this->mbr_0x14;
    in_stack_00000004[1] = this->mbr_0x10 - dVar2;
    *(byte *)(in_stack_00000004 + 2) = bVar1;
    return;
  }
  bVar1 = this->mbr_0x14;
  in_stack_00000004[1] = -1;
  *(byte *)(in_stack_00000004 + 2) = bVar1;
  return;
}



// Function at 005885b0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5885b0(cls_0x588410 *this)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int *in_stack_00000004;
  
  uVar3 = *in_stack_00000004 + this->mbr_0x0;
  this->mbr_0xc = uVar3;
  if (this->mbr_0x4 < uVar3) {
    meth_0x5884a0(this);
  }
  iVar2 = in_stack_00000004[1];
  if (-1 < iVar2) {
    this->mbr_0x14 = *(byte *)(in_stack_00000004 + 2);
    this->mbr_0x10 = iVar2 + this->mbr_0x0;
    return;
  }
  bVar1 = *(byte *)(in_stack_00000004 + 2);
  this->mbr_0x10 = 0;
  this->mbr_0x14 = bVar1;
  return;
}



// Function at 00588600

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588600(cls_0x588410 *this)

{
  byte bVar1;
  undefined *puVar2;
  char in_stack_00000004;
  
  if (this->mbr_0x10 == 0) {
    puVar2 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar2 + 1);
    if ((undefined *)this->mbr_0x4 < puVar2 + 1) {
      puVar2 = (undefined *)meth_0x5884a0(this);
    }
    this->mbr_0x10 = (dword)puVar2;
    *puVar2 = 0;
    this->mbr_0x14 = 0x80;
  }
  if (in_stack_00000004 != '\0') {
    *(byte *)this->mbr_0x10 = *(byte *)this->mbr_0x10 | this->mbr_0x14;
  }
  bVar1 = this->mbr_0x14 >> 1;
  this->mbr_0x14 = bVar1;
  if (bVar1 == 0) {
    this->mbr_0x10 = 0;
  }
  return;
}



// Function at 00588660

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588660(cls_0x588410 *this)

{
  char cVar1;
  undefined *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *in_stack_00000004;
  
  if (in_stack_00000004 == (undefined4 *)0x0) {
    puVar2 = (undefined *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar2 + 1);
    if ((undefined *)this->mbr_0x4 < puVar2 + 1) {
      puVar2 = (undefined *)meth_0x5884a0(this);
    }
    *puVar2 = DAT_00676ed0;
    return;
  }
  uVar4 = 0xffffffff;
  puVar3 = in_stack_00000004;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *(char *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  } while (cVar1 != '\0');
  puVar3 = (undefined4 *)this->mbr_0xc;
  uVar4 = ~uVar4;
  this->mbr_0xc = (dword)(undefined *)((int)puVar3 + uVar4);
  if ((undefined *)this->mbr_0x4 < (undefined *)((int)puVar3 + uVar4)) {
    puVar3 = (undefined4 *)meth_0x5884a0(this);
  }
  for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *puVar3 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    puVar3 = puVar3 + 1;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar3 = *(undefined *)in_stack_00000004;
    in_stack_00000004 = (undefined4 *)((int)in_stack_00000004 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  }
  return;
}



// Function at 005886d0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5886d0(cls_0x588410 *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  uint in_stack_00000008;
  
  if ((param_1 != (undefined4 *)0x0) && (in_stack_00000008 != 0)) {
    puVar1 = (undefined4 *)this->mbr_0xc;
    this->mbr_0xc = (dword)(undefined *)((int)puVar1 + in_stack_00000008);
    if ((undefined *)this->mbr_0x4 < (undefined *)((int)puVar1 + in_stack_00000008)) {
      puVar1 = (undefined4 *)meth_0x5884a0(this);
    }
    for (uVar2 = in_stack_00000008 >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar1 = *param_1;
      param_1 = param_1 + 1;
      puVar1 = puVar1 + 1;
    }
    for (uVar2 = in_stack_00000008 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
      *(undefined *)puVar1 = *(undefined *)param_1;
      param_1 = (undefined4 *)((int)param_1 + 1);
      puVar1 = (undefined4 *)((int)puVar1 + 1);
    }
  }
  return;
}



// Function at 00588740

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588740(cls_0x588410 *this,char *param_1)

{
  undefined uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  bool bVar4;
  char cVar5;
  byte bVar6;
  undefined2 *puVar7;
  undefined *puVar8;
  undefined4 *puVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  undefined4 *puVar14;
  undefined2 uStack00000008;
  undefined uStack0000000a;
  undefined uStack0000000b;
  
  iVar13 = 0;
  cVar5 = *param_1;
  bVar4 = false;
  if (cVar5 != '\0') {
    iVar12 = _uStack00000008 + -4;
    do {
      if ('/' < cVar5 == cVar5 < ':') {
        iVar13 = iVar13 * 0xb + -0x30 + (int)cVar5;
        goto switchD_0058879e_caseD_63;
      }
      switch(cVar5) {
      case 'b':
        iVar13 = *(int *)(iVar12 + 4);
        if (this->mbr_0x10 == 0) {
          puVar8 = (undefined *)meth_0x5725f0(this);
          this->mbr_0x10 = (dword)puVar8;
          *puVar8 = 0;
          this->mbr_0x14 = 0x80;
        }
        if (iVar13 != 0) {
          *(byte *)this->mbr_0x10 = *(byte *)this->mbr_0x10 | this->mbr_0x14;
        }
        bVar6 = this->mbr_0x14 >> 1;
        this->mbr_0x14 = bVar6;
        if (bVar6 == 0) {
          this->mbr_0x10 = 0;
        }
        break;
      default:
        goto switchD_0058879e_caseD_63;
      case 'd':
        if (bVar4) {
          if (iVar13 == 1) {
            uVar1 = *(undefined *)(iVar12 + 4);
            puVar8 = (undefined *)this->mbr_0xc;
            this->mbr_0xc = (dword)(puVar8 + 1);
            if ((undefined *)this->mbr_0x4 < puVar8 + 1) {
              puVar8 = (undefined *)meth_0x5884a0(this);
            }
            *puVar8 = uVar1;
          }
          else if (iVar13 == 2) {
            puVar8 = (undefined *)this->mbr_0xc;
            uVar2 = *(undefined2 *)(iVar12 + 4);
            this->mbr_0xc = (dword)(puVar8 + 2);
            if ((undefined *)this->mbr_0x4 < puVar8 + 2) {
              puVar8 = (undefined *)meth_0x5884a0(this);
            }
            *puVar8 = (char)uVar2;
            puVar8[1] = (char)((ushort)uVar2 >> 8);
          }
          else if (iVar13 == 3) {
            uVar3 = *(undefined4 *)(iVar12 + 4);
            puVar7 = (undefined2 *)this->mbr_0xc;
            this->mbr_0xc = (int)puVar7 + 3U;
            if (this->mbr_0x4 < (int)puVar7 + 3U) {
              puVar7 = (undefined2 *)meth_0x5884a0(this);
            }
            uStack00000008 = (undefined2)uVar3;
            uStack0000000a = (undefined)((uint)uVar3 >> 0x10);
            *puVar7 = uStack00000008;
            *(undefined *)(puVar7 + 1) = uStack0000000a;
          }
          else {
            puVar8 = (undefined *)this->mbr_0xc;
            uVar3 = *(undefined4 *)(iVar12 + 4);
            this->mbr_0xc = (dword)(puVar8 + 4);
            if ((undefined *)this->mbr_0x4 < puVar8 + 4) {
              puVar8 = (undefined *)meth_0x5884a0(this);
            }
            uStack0000000b = (undefined)((uint)uVar3 >> 0x18);
            *puVar8 = (char)uVar3;
            uStack0000000a = (undefined)((uint)uVar3 >> 0x10);
            puVar8[1] = (char)((uint)uVar3 >> 8);
            puVar8[2] = uStack0000000a;
            puVar8[3] = uStack0000000b;
          }
        }
        else if (iVar13 == 1) {
          uVar1 = *(undefined *)(iVar12 + 4);
          puVar8 = (undefined *)this->mbr_0xc;
          this->mbr_0xc = (dword)(puVar8 + 1);
          if ((undefined *)this->mbr_0x4 < puVar8 + 1) {
            puVar8 = (undefined *)meth_0x5884a0(this);
          }
          *puVar8 = uVar1;
        }
        else if (iVar13 == 2) {
          uVar2 = *(undefined2 *)(iVar12 + 4);
          puVar8 = (undefined *)meth_0x5725f0(this);
          *puVar8 = (char)uVar2;
          puVar8[1] = (char)((ushort)uVar2 >> 8);
        }
        else if (iVar13 == 3) {
          uVar3 = *(undefined4 *)(iVar12 + 4);
          puVar7 = (undefined2 *)meth_0x5725f0(this);
          uStack00000008 = (undefined2)uVar3;
          uStack0000000a = (undefined)((uint)uVar3 >> 0x10);
          *puVar7 = uStack00000008;
          *(undefined *)(puVar7 + 1) = uStack0000000a;
        }
        else {
          puVar8 = (undefined *)this->mbr_0xc;
          uVar3 = *(undefined4 *)(iVar12 + 4);
          this->mbr_0xc = (dword)(puVar8 + 4);
          if ((undefined *)this->mbr_0x4 < puVar8 + 4) {
            puVar8 = (undefined *)meth_0x5884a0(this);
          }
          uStack0000000b = (undefined)((uint)uVar3 >> 0x18);
          *puVar8 = (char)uVar3;
          uStack0000000a = (undefined)((uint)uVar3 >> 0x10);
          puVar8[1] = (char)((uint)uVar3 >> 8);
          puVar8[2] = uStack0000000a;
          puVar8[3] = uStack0000000b;
        }
        break;
      case 'l':
        meth_0x5886d0(this,*(undefined4 *)(iVar12 + 4));
        break;
      case 's':
        puVar14 = *(undefined4 **)(iVar12 + 4);
        if (puVar14 == (undefined4 *)0x0) {
          puVar8 = (undefined *)meth_0x5725f0(this);
          *puVar8 = DAT_00676ed0;
        }
        else {
          uVar10 = 0xffffffff;
          puVar9 = puVar14;
          do {
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            cVar5 = *(char *)puVar9;
            puVar9 = (undefined4 *)((int)puVar9 + 1);
          } while (cVar5 != '\0');
          puVar9 = (undefined4 *)meth_0x5725f0(this);
          for (uVar11 = ~uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
            *puVar9 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar9 = puVar9 + 1;
          }
          for (uVar10 = ~uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
            *(undefined *)puVar9 = *(undefined *)puVar14;
            puVar14 = (undefined4 *)((int)puVar14 + 1);
            puVar9 = (undefined4 *)((int)puVar9 + 1);
          }
        }
        break;
      case 'u':
        bVar4 = true;
        goto switchD_0058879e_caseD_63;
      }
      iVar13 = 0;
      iVar12 = iVar12 + 4;
      bVar4 = false;
switchD_0058879e_caseD_63:
      param_1 = param_1 + 1;
      cVar5 = *param_1;
    } while (cVar5 != '\0');
  }
  return;
}



// Function at 00588a50

int __thiscall OOAnalyzer::cls_0x588410::meth_0x588a50(cls_0x588410 *this)

{
  char cVar1;
  int iVar2;
  int *in_stack_00000004;
  
  iVar2 = (this->mbr_0xc - *in_stack_00000004) - this->mbr_0x0;
  if ((int)(uint)*(byte *)((int)in_stack_00000004 + 9) <= iVar2) {
    cVar1 = meth_0x588c00(this,in_stack_00000004);
    if (cVar1 != '\0') {
      return iVar2 - (uint)*(byte *)((int)in_stack_00000004 + 9);
    }
  }
  return -1;
}



// Function at 00588a90

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x588a90(cls_0x588410 *this,int param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  uint in_stack_00000008;
  int local_10;
  int local_c;
  byte local_8;
  undefined4 local_7;
  
  local_10 = this->mbr_0xc - this->mbr_0x0;
  if (this->mbr_0x10 == 0) {
    local_c = -1;
  }
  else {
    local_c = this->mbr_0x10 - this->mbr_0x0;
  }
  local_8 = this->mbr_0x14;
  if (((char)in_stack_00000008 != '\0') &&
     (uVar2 = (in_stack_00000008 & 0xff) + this->mbr_0xc, this->mbr_0xc = uVar2,
     this->mbr_0x4 < uVar2)) {
    meth_0x5884a0(this);
  }
  uVar2 = param_1 + this->mbr_0xc;
  local_7 = CONCAT31(local_7._1_3_,(char)in_stack_00000008);
  this->mbr_0xc = uVar2;
  if (this->mbr_0x4 < uVar2) {
    meth_0x5884a0(this);
  }
  iVar3 = (this->mbr_0xc - this->mbr_0x0) - local_10;
  if (((iVar3 < (int)(local_7 & 0xff)) || (cVar1 = meth_0x588c00(this,&local_10), cVar1 == '\0')) ||
     ((int)(iVar3 - (local_7 & 0xff)) < 0)) {
    uVar2 = this->mbr_0x0 + local_10;
    this->mbr_0xc = uVar2;
    if (this->mbr_0x4 < uVar2) {
      meth_0x5884a0(this);
    }
    if (local_c < 0) {
      this->mbr_0x10 = 0;
      this->mbr_0x14 = local_8;
      return 0;
    }
    this->mbr_0x14 = local_8;
    this->mbr_0x10 = this->mbr_0x0 + local_c;
    return 0;
  }
  local_10 = local_10 + in_stack_00000008;
  uVar2 = local_10 + this->mbr_0x0;
  this->mbr_0xc = uVar2;
  if (this->mbr_0x4 < uVar2) {
    meth_0x5884a0(this);
  }
  if (local_c < 0) {
    this->mbr_0x10 = 0;
    this->mbr_0x14 = local_8;
    return 1;
  }
  this->mbr_0x10 = this->mbr_0x0 + local_c;
  this->mbr_0x14 = local_8;
  return 1;
}



// Function at 00588c00

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x588c00(cls_0x588410 *this,int *param_1)

{
  undefined uVar1;
  dword dVar2;
  undefined2 *puVar3;
  undefined uVar4;
  undefined uVar5;
  undefined2 uStack00000008;
  undefined uStack0000000a;
  undefined uStack0000000b;
  
  dVar2 = this->mbr_0xc;
  puVar3 = (undefined2 *)(*param_1 + this->mbr_0x0);
  uVar1 = *(undefined *)((int)param_1 + 9);
  this->mbr_0xc = (dword)puVar3;
  uVar4 = (undefined)_uStack00000008;
  uVar5 = (undefined)((uint)_uStack00000008 >> 8);
  switch(uVar1) {
  case 1:
    break;
  case 2:
    if (_uStack00000008 < 0x10000) {
      this->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined2 *)this->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined2 *)meth_0x5884a0(this);
      }
      this->mbr_0xc = dVar2;
      *(undefined *)puVar3 = uVar4;
      this->mbr_0x10 = 0;
      *(undefined *)((int)puVar3 + 1) = uVar5;
      return 1;
    }
    this->mbr_0xc = dVar2;
    return 0;
  case 3:
    if (_uStack00000008 < 0x1000000) {
      this->mbr_0xc = (dword)(undefined *)((int)puVar3 + 3U);
      if ((undefined *)this->mbr_0x4 < (undefined *)((int)puVar3 + 3U)) {
        puVar3 = (undefined2 *)meth_0x5884a0(this);
      }
      this->mbr_0xc = dVar2;
      *puVar3 = uStack00000008;
      this->mbr_0x10 = 0;
      *(undefined *)(puVar3 + 1) = uStack0000000a;
      return 1;
    }
  default:
    this->mbr_0xc = dVar2;
    return 0;
  case 4:
    this->mbr_0xc = (dword)(puVar3 + 2);
    if ((undefined2 *)this->mbr_0x4 < puVar3 + 2) {
      puVar3 = (undefined2 *)meth_0x5884a0(this);
    }
    *(undefined *)puVar3 = uVar4;
    this->mbr_0xc = dVar2;
    this->mbr_0x10 = 0;
    *(undefined *)((int)puVar3 + 1) = uVar5;
    *(undefined *)(puVar3 + 1) = uStack0000000a;
    *(undefined *)((int)puVar3 + 3) = uStack0000000b;
    return 1;
  }
  if (_uStack00000008 < 0x100) {
    this->mbr_0xc = (dword)(undefined *)((int)puVar3 + 1U);
    if ((undefined *)this->mbr_0x4 < (undefined *)((int)puVar3 + 1U)) {
      puVar3 = (undefined2 *)meth_0x5884a0(this);
    }
    this->mbr_0xc = dVar2;
    this->mbr_0x10 = 0;
    *(undefined *)puVar3 = uVar4;
    return 1;
  }
  this->mbr_0xc = dVar2;
  return 0;
}



// Function at 00588d70

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588d70(cls_0x588410 *this)

{
  undefined *puVar1;
  int in_stack_00000004;
  
  if (in_stack_00000004 < 0x80) {
    if (in_stack_00000004 < -0x80) {
      in_stack_00000004 = -0x80;
    }
  }
  else {
    in_stack_00000004 = 0x7f;
  }
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  return;
}



// Function at 00588db0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588db0(cls_0x588410 *this)

{
  undefined *puVar1;
  int in_stack_00000004;
  
  if (in_stack_00000004 < 0x100) {
    if (in_stack_00000004 < 0) {
      in_stack_00000004 = 0;
    }
  }
  else {
    in_stack_00000004 = 0xff;
  }
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 1);
  if ((undefined *)this->mbr_0x4 < puVar1 + 1) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  return;
}



// Function at 00588df0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588df0(cls_0x588410 *this)

{
  undefined *puVar1;
  int in_stack_00000004;
  
  if (in_stack_00000004 < 0x8000) {
    if (in_stack_00000004 < -0x8000) {
      in_stack_00000004 = -0x8000;
    }
  }
  else {
    in_stack_00000004 = 0x7fff;
  }
  puVar1 = (undefined *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar1 + 2);
  if ((undefined *)this->mbr_0x4 < puVar1 + 2) {
    puVar1 = (undefined *)meth_0x5884a0(this);
  }
  *puVar1 = (char)in_stack_00000004;
  puVar1[1] = (char)((uint)in_stack_00000004 >> 8);
  return;
}



// Function at 00588e80

void __thiscall OOAnalyzer::cls_0x588410::meth_0x588e80(cls_0x588410 *this)

{
  undefined2 uVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined2 *in_stack_00000004;
  
  puVar3 = (undefined *)this->mbr_0xc;
  uVar1 = *in_stack_00000004;
  this->mbr_0xc = (dword)(puVar3 + 2);
  if ((undefined *)this->mbr_0x4 < puVar3 + 2) {
    puVar3 = (undefined *)meth_0x5884a0(this);
  }
  puVar2 = (undefined *)this->mbr_0x4;
  *puVar3 = (char)uVar1;
  puVar3[1] = (char)((ushort)uVar1 >> 8);
  puVar3 = (undefined *)this->mbr_0xc;
  uVar1 = in_stack_00000004[2];
  this->mbr_0xc = (dword)(puVar3 + 2);
  if (puVar2 < puVar3 + 2) {
    puVar3 = (undefined *)meth_0x5884a0(this);
  }
  puVar2 = (undefined *)this->mbr_0x4;
  *puVar3 = (char)uVar1;
  puVar3[1] = (char)((ushort)uVar1 >> 8);
  puVar3 = (undefined *)this->mbr_0xc;
  uVar1 = in_stack_00000004[4];
  this->mbr_0xc = (dword)(puVar3 + 2);
  if (puVar2 < puVar3 + 2) {
    puVar3 = (undefined *)meth_0x5884a0(this);
  }
  *puVar3 = (char)uVar1;
  puVar3[1] = (char)((ushort)uVar1 >> 8);
  return;
}



// Function at 005891c0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5891c0(cls_0x588410 *this,undefined4 *param_1)

{
  byte bVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  uint uVar6;
  dword dVar7;
  undefined4 *puVar8;
  uint in_stack_00000008;
  int local_8;
  
  dVar2 = this->mbr_0x0;
  dVar7 = this->mbr_0xc - dVar2;
  if ((int)this->mbr_0x18 < (int)dVar7) {
    this->mbr_0x18 = dVar7;
  }
  dVar3 = this->mbr_0x18;
  dVar5 = in_stack_00000008 + dVar3;
  this->mbr_0x18 = dVar5;
  if (this->mbr_0x4 < dVar5 + dVar2) {
    dVar4 = this->mbr_0xc;
    if (this->mbr_0x10 == 0) {
      local_8 = -1;
    }
    else {
      local_8 = this->mbr_0x10 - dVar2;
    }
    bVar1 = this->mbr_0x14;
    uVar6 = (dVar5 - dVar7) + this->mbr_0xc;
    this->mbr_0xc = uVar6;
    if (this->mbr_0x4 < uVar6) {
      meth_0x5884a0(this);
    }
    uVar6 = (dVar4 - dVar2) + this->mbr_0x0;
    this->mbr_0xc = uVar6;
    if (this->mbr_0x4 < uVar6) {
      meth_0x5884a0(this);
    }
    if (local_8 < 0) {
      this->mbr_0x10 = 0;
    }
    else {
      this->mbr_0x10 = this->mbr_0x0 + local_8;
    }
    this->mbr_0x14 = bVar1;
  }
  puVar8 = (undefined4 *)(dVar3 + this->mbr_0xc);
  for (uVar6 = in_stack_00000008 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *puVar8 = *param_1;
    param_1 = param_1 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar6 = in_stack_00000008 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined *)puVar8 = *(undefined *)param_1;
    param_1 = (undefined4 *)((int)param_1 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  return;
}



// Function at 00589290

undefined4 __thiscall OOAnalyzer::cls_0x588410::meth_0x589290(cls_0x588410 *this)

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  dword dVar4;
  byte bVar5;
  
  if (this->mbr_0x10 == 0) {
    dVar4 = this->mbr_0xc;
    this->mbr_0xc = dVar4 + 1;
    if (this->mbr_0x4 < dVar4 + 1) {
      dVar4 = meth_0x5884a0(this);
    }
    this->mbr_0x10 = dVar4;
    this->mbr_0x14 = 0x80;
  }
  pbVar3 = (byte *)this->mbr_0x10;
  bVar1 = this->mbr_0x14;
  bVar2 = *pbVar3;
  bVar5 = bVar1 >> 1;
  this->mbr_0x14 = bVar5;
  if (bVar5 == 0) {
    this->mbr_0x10 = 0;
  }
  return CONCAT31((int3)((uint)pbVar3 >> 8),(bVar2 & bVar1) != 0);
}



// Function at 005892e0

char * __thiscall OOAnalyzer::cls_0x588410::meth_0x5892e0(cls_0x588410 *this)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  char *pcVar4;
  
  uVar3 = 0xffffffff;
  pcVar2 = (char *)this->mbr_0xc;
  pcVar4 = pcVar2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  this->mbr_0xc = (dword)(pcVar2 + ~uVar3);
  if ((char *)this->mbr_0x4 < pcVar2 + ~uVar3) {
    meth_0x5884a0(this);
  }
  return pcVar2;
}



// Function at 00589310

void __thiscall OOAnalyzer::cls_0x588410::meth_0x589310(cls_0x588410 *this)

{
  uint uVar1;
  int in_stack_00000004;
  
  uVar1 = in_stack_00000004 + this->mbr_0xc;
  this->mbr_0xc = uVar1;
  if (this->mbr_0x4 < uVar1) {
    meth_0x5884a0(this);
  }
  return;
}



// Function at 00589350

void __thiscall OOAnalyzer::cls_0x588410::meth_0x589350(cls_0x588410 *this,char *param_1)

{
  undefined uVar1;
  byte bVar2;
  byte bVar3;
  char *pcVar4;
  bool bVar5;
  char cVar6;
  byte bVar7;
  undefined *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  dword dVar11;
  int iVar12;
  int iVar13;
  undefined *puVar14;
  char *pcVar15;
  int in_stack_00000008;
  
  cVar6 = *param_1;
  iVar13 = 0;
  bVar5 = false;
  if (cVar6 != '\0') {
    iVar12 = in_stack_00000008 + -4;
    do {
      if ('/' < cVar6 == cVar6 < ':') {
        iVar13 = iVar13 * 0xb + -0x30 + (int)cVar6;
        goto switchD_005893aa_caseD_63;
      }
      switch(cVar6) {
      case 'b':
        if (this->mbr_0x10 == 0) {
          dVar11 = meth_0x5725f0(this);
          this->mbr_0x10 = dVar11;
          this->mbr_0x14 = 0x80;
        }
        bVar2 = this->mbr_0x14;
        bVar3 = *(byte *)this->mbr_0x10;
        bVar7 = bVar2 >> 1;
        this->mbr_0x14 = bVar7;
        if (bVar7 == 0) {
          this->mbr_0x10 = 0;
        }
        **(uint **)(iVar12 + 4) = (uint)((bVar3 & bVar2) != 0);
        break;
      default:
        goto switchD_005893aa_caseD_63;
      case 'd':
        if (bVar5) {
          if (iVar13 == 1) {
            puVar8 = (undefined *)this->mbr_0xc;
            puVar14 = *(undefined **)(iVar12 + 4);
            this->mbr_0xc = (dword)(puVar8 + 1);
            if (puVar8 + 1 <= (undefined *)this->mbr_0x4) goto LAB_005894e5;
            puVar8 = (undefined *)meth_0x5884a0(this);
            *puVar14 = *puVar8;
          }
          else if (iVar13 == 2) {
LAB_00589436:
            puVar8 = (undefined *)this->mbr_0xc;
            puVar14 = *(undefined **)(iVar12 + 4);
            this->mbr_0xc = (dword)(puVar8 + 2);
            if ((undefined *)this->mbr_0x4 < puVar8 + 2) {
              puVar8 = (undefined *)meth_0x5884a0(this);
            }
            uVar1 = puVar8[1];
            *puVar14 = *puVar8;
            puVar14[1] = uVar1;
          }
          else if (iVar13 == 3) {
            puVar9 = (undefined2 *)this->mbr_0xc;
            puVar10 = *(undefined2 **)(iVar12 + 4);
            this->mbr_0xc = (int)puVar9 + 3U;
            if (this->mbr_0x4 < (int)puVar9 + 3U) {
              puVar9 = (undefined2 *)meth_0x5884a0(this);
            }
            uVar1 = *(undefined *)(puVar9 + 1);
            *puVar10 = *puVar9;
            *(undefined *)(puVar10 + 1) = uVar1;
          }
          else {
            puVar8 = (undefined *)this->mbr_0xc;
            puVar14 = *(undefined **)(iVar12 + 4);
            this->mbr_0xc = (dword)(puVar8 + 4);
            if ((undefined *)this->mbr_0x4 < puVar8 + 4) {
              puVar8 = (undefined *)meth_0x5884a0(this);
            }
LAB_005893eb:
            uVar1 = puVar8[1];
            *puVar14 = *puVar8;
            puVar14[1] = uVar1;
            uVar1 = puVar8[3];
            puVar14[2] = puVar8[2];
            puVar14[3] = uVar1;
          }
        }
        else if (iVar13 == 1) {
          puVar8 = (undefined *)this->mbr_0xc;
          puVar14 = *(undefined **)(iVar12 + 4);
          this->mbr_0xc = (dword)(puVar8 + 1);
          if ((undefined *)this->mbr_0x4 < puVar8 + 1) {
            puVar8 = (undefined *)meth_0x5884a0(this);
          }
LAB_005894e5:
          *puVar14 = *puVar8;
        }
        else {
          if (iVar13 == 2) goto LAB_00589436;
          if (iVar13 != 3) {
            puVar14 = *(undefined **)(iVar12 + 4);
            puVar8 = (undefined *)meth_0x5725f0(this);
            goto LAB_005893eb;
          }
          puVar9 = *(undefined2 **)(iVar12 + 4);
          puVar10 = (undefined2 *)meth_0x5725f0(this);
          uVar1 = *(undefined *)(puVar10 + 1);
          *puVar9 = *puVar10;
          *(undefined *)(puVar9 + 1) = uVar1;
        }
        break;
      case 'l':
        dVar11 = this->mbr_0xc;
        this->mbr_0xc = iVar13 + dVar11;
        if (this->mbr_0x4 < iVar13 + dVar11) {
          dVar11 = meth_0x5884a0(this);
        }
        **(dword **)(iVar12 + 4) = dVar11;
        break;
      case 's':
        pcVar4 = (char *)this->mbr_0xc;
        iVar13 = -1;
        pcVar15 = pcVar4;
        do {
          if (iVar13 == 0) break;
          iVar13 = iVar13 + -1;
          cVar6 = *pcVar15;
          pcVar15 = pcVar15 + 1;
        } while (cVar6 != '\0');
        meth_0x5725f0(this);
        **(char ***)(iVar12 + 4) = pcVar4;
        break;
      case 'u':
        bVar5 = true;
        goto switchD_005893aa_caseD_63;
      }
      iVar13 = 0;
      iVar12 = iVar12 + 4;
      bVar5 = false;
switchD_005893aa_caseD_63:
      param_1 = param_1 + 1;
      cVar6 = *param_1;
    } while (cVar6 != '\0');
  }
  return;
}



// Function at 005895c0

int __thiscall OOAnalyzer::cls_0x588410::meth_0x5895c0(cls_0x588410 *this)

{
  int iVar1;
  byte *pbVar2;
  ushort *puVar3;
  undefined3 *puVar4;
  int *piVar5;
  uint in_stack_00000004;
  
  iVar1 = in_stack_00000004;
  switch(in_stack_00000004) {
  case 1:
    pbVar2 = (byte *)this->mbr_0xc;
    this->mbr_0xc = (dword)(pbVar2 + 1);
    if ((byte *)this->mbr_0x4 < pbVar2 + 1) {
      pbVar2 = (byte *)meth_0x5884a0(this);
    }
    return *pbVar2 - in_stack_00000004;
  case 2:
    puVar3 = (ushort *)this->mbr_0xc;
    this->mbr_0xc = (dword)(puVar3 + 1);
    if ((ushort *)this->mbr_0x4 < puVar3 + 1) {
      puVar3 = (ushort *)meth_0x5884a0(this);
    }
    break;
  case 3:
    puVar4 = (undefined3 *)this->mbr_0xc;
    this->mbr_0xc = (int)puVar4 + 3U;
    if (this->mbr_0x4 < (int)puVar4 + 3U) {
      puVar4 = (undefined3 *)meth_0x5884a0(this);
    }
    in_stack_00000004 = CONCAT13(in_stack_00000004._3_1_,*puVar4);
    return in_stack_00000004 - iVar1;
  case 4:
    piVar5 = (int *)this->mbr_0xc;
    this->mbr_0xc = (dword)(piVar5 + 1);
    if ((int *)this->mbr_0x4 < piVar5 + 1) {
      piVar5 = (int *)meth_0x5884a0(this);
    }
    return *piVar5 - in_stack_00000004;
  default:
    return 0;
  }
  in_stack_00000004 = (uint)*puVar3;
  return in_stack_00000004 - iVar1;
}



// Function at 005896c0

void __thiscall OOAnalyzer::cls_0x588410::meth_0x5896c0(cls_0x588410 *this,int *param_1)

{
  int *piVar1;
  int *piVar2;
  undefined4 in_stack_00000008;
  
  piVar1 = param_1;
  piVar2 = (int *)this->mbr_0xc;
  this->mbr_0x10 = 0;
  *param_1 = (int)piVar2 - this->mbr_0x0;
  *(byte *)(param_1 + 2) = this->mbr_0x14;
  param_1[1] = -1;
  switch(in_stack_00000008) {
  case 1:
    this->mbr_0xc = (dword)(byte *)((int)piVar2 + 1U);
    if ((byte *)this->mbr_0x4 < (byte *)((int)piVar2 + 1U)) {
      piVar2 = (int *)meth_0x5884a0(this);
    }
    *param_1 = *param_1 + (uint)*(byte *)piVar2;
    return;
  case 2:
    this->mbr_0xc = (dword)(ushort *)((int)piVar2 + 2U);
    if ((ushort *)this->mbr_0x4 < (ushort *)((int)piVar2 + 2U)) {
      piVar2 = (int *)meth_0x5884a0(this);
    }
    break;
  case 3:
    this->mbr_0xc = (dword)(byte *)((int)piVar2 + 3U);
    if ((byte *)this->mbr_0x4 < (byte *)((int)piVar2 + 3U)) {
      piVar2 = (int *)meth_0x5884a0(this);
    }
    param_1 = (int *)CONCAT13(param_1._3_1_,*(undefined3 *)piVar2);
    *piVar1 = *piVar1 + (int)param_1;
    return;
  case 4:
    this->mbr_0xc = (dword)(piVar2 + 1);
    if ((int *)this->mbr_0x4 < piVar2 + 1) {
      piVar2 = (int *)meth_0x5884a0(this);
    }
    *param_1 = *param_1 + *piVar2;
    return;
  default:
    *param_1 = *param_1 + (int)param_1;
    return;
  }
  param_1 = (int *)(uint)*(ushort *)piVar2;
  *piVar1 = *piVar1 + (int)param_1;
  return;
}



// Function at 00589810

void __thiscall OOAnalyzer::cls_0x588410::meth_0x589810(cls_0x588410 *this)

{
  ushort *puVar1;
  uint *puVar2;
  ushort *puVar3;
  uint *in_stack_00000004;
  
  puVar2 = in_stack_00000004;
  puVar3 = (ushort *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar3 + 1);
  if ((ushort *)this->mbr_0x4 < puVar3 + 1) {
    puVar3 = (ushort *)meth_0x5884a0(this);
  }
  puVar1 = (ushort *)this->mbr_0x4;
  *in_stack_00000004 = (uint)*puVar3;
  puVar3 = (ushort *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar3 + 1);
  if (puVar1 < puVar3 + 1) {
    puVar3 = (ushort *)meth_0x5884a0(this);
  }
  puVar1 = (ushort *)this->mbr_0x4;
  in_stack_00000004[1] = (uint)*puVar3;
  puVar3 = (ushort *)this->mbr_0xc;
  this->mbr_0xc = (dword)(puVar3 + 1);
  if (puVar1 < puVar3 + 1) {
    puVar3 = (ushort *)meth_0x5884a0(this);
  }
  in_stack_00000004 = (uint *)(uint)*puVar3;
  puVar2[2] = (uint)in_stack_00000004;
  return;
}



