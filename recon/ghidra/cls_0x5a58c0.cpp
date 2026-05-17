// Decompiled methods and structure for class: cls_0x5a58c0

/*
/OOAnalyzer/cls_0x5a58c0
pack(disabled)
Structure cls_0x5a58c0 {
   0   cls_0x5a58c0::vftable_5a58c0 *   4   vftptr_0x0   "pointer to cls_0x5a58c0::vftable_5a58c0"
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
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 428 Alignment: 1

*/

// Function at 00487f50

cls_0x5a58c0 * __thiscall OOAnalyzer::cls_0x5a58c0::cls_0x5a58c0(cls_0x5a58c0 *this)

{
  this->mbr_0x4 = 0x1c4;
  this->mbr_0x14 = 0x1c4;
  this->mbr_0x8 = 0x132;
  this->mbr_0x18 = 0x132;
  this->mbr_0xc = 0xbc;
  this->mbr_0x1c = 0xbc;
  this->vftptr_0x0 = (cls_0x5a58c0__vftable_5a58c0 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x10 = 0xae;
  this->mbr_0x20 = 0xae;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x17c = 0;
  this->vftptr_0x0 = &cls_0x5a58c0__vftable_5a58c0_005a58c0;
  return this;
}



// Function at 00487fc0

void __thiscall OOAnalyzer::cls_0x5a58c0::virt_meth_0x487fc0(cls_0x5a58c0 *this)

{
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  return;
}



// Function at 00487fe0

cls_0x5a58c0 * __thiscall OOAnalyzer::cls_0x5a58c0::virt_meth_0x487fe0(cls_0x5a58c0 *this)

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a58c0(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a58c0 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00537650

undefined4 __thiscall OOAnalyzer::cls_0x5a58c0::virt_meth_0x537650(cls_0x5a58c0 *this)

{
  cls_0x46d6b0 *pcVar1;
  dword dVar2;
  cls_0x5a68d8 *pcVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  cls_0x5a3c68 *pcVar6;
  int iVar7;
  void *unaff_EBP;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1b0c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5a4494_TPane::virt_meth_0x434e40((cls_0x5a4494_TPane *)this);
  pcVar1 = DAT_0065c130;
  this->mbr_0x184 = 0xffffffff;
  this->mbr_0x180 = 0xffffffff;
  this->mbr_0x18c = 0;
  dVar2 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar1 = DAT_0065c130;
  this->mbr_0x198 = dVar2;
  dVar2 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  this->mbr_0x19c = dVar2;
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
  local_4 = 0;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x46d6b0::meth_0x46d710(DAT_0065c130);
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a3a40(pcVar3);
  }
  local_4 = 0xffffffff;
  this->mbr_0x1a0 = (dword)pcVar3;
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
  local_4 = 2;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    local_4 = CONCAT31(local_4._1_3_,3);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a39a0(pcVar3,0x14,0x14);
  }
  pcVar1 = DAT_0065c130;
  local_4 = 0xffffffff;
  this->mbr_0x1a8 = (dword)pcVar3;
  uVar4 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065c130);
  cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar6 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 4;
  if (pcVar6 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar6,s_scrollleft_005e4164,0x8c,0xb,0x18,0x18,0,&LAB_00537500,uVar5,uVar4,0,0,1,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar1 = DAT_0065c130;
  if (*(int *)&this->field_0x88 == 0) {
    iVar7 = 0;
  }
  else {
    iVar7 = **(int **)&this->field_0x98;
  }
  *(uint *)(iVar7 + 0xd4) = *(uint *)(iVar7 + 0xd4) | 0x2000;
  uVar4 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065c130);
  cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar6 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 5;
  if (pcVar6 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar6,s_scrollright_005e4194,0xa1,0xc,0x18,0x18,0,&LAB_00537510,uVar5,uVar4,0,0,1,
               0xffffffff,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  if (*(uint *)&this->field_0x88 < 2) {
    iVar7 = 0;
  }
  else {
    iVar7 = *(int *)(*(int *)&this->field_0x98 + 4);
  }
  *(uint *)(iVar7 + 0xd4) = *(uint *)(iVar7 + 0xd4) | 0x2000;
  (**(code **)(*(int *)this->mbr_0x1a8 + 0x1c))(0);
  (**(code **)(*(int *)this->mbr_0x1a0 + 0x1c))(0);
  cls_0x5b98b8::meth_0x435660((cls_0x5b98b8 *)this);
  this->mbr_0x188 = 0;
  this->mbr_0x1a4 = 0;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  ExceptionList = unaff_EBP;
  return 1;
}



// Function at 00537940

void __thiscall OOAnalyzer::cls_0x5a58c0::virt_meth_0x537940(cls_0x5a58c0 *this)

{
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  if ((undefined4 *)this->mbr_0x1a8 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x1a8)(1);
    this->mbr_0x1a8 = 0;
  }
  if ((undefined4 *)this->mbr_0x1a0 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x1a0)(1);
    this->mbr_0x1a0 = 0;
  }
  return;
}



// Function at 00537980

cls_0x5a58c0 * __thiscall OOAnalyzer::cls_0x5a58c0::~cls_0x5a58c0(cls_0x5a58c0 *this)

{
  cls_0x5a58c0 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1b34;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &cls_0x5a58c0__vftable_5a58c0_005a58c0;
  local_4 = 0;
  if ((undefined4 *)this->mbr_0x1a8 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x1a8)(1);
    this->mbr_0x1a8 = 0;
  }
  if ((undefined4 *)this->mbr_0x1a0 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x1a0)(1);
    this->mbr_0x1a0 = 0;
  }
  this->vftptr_0x0 = (cls_0x5a58c0__vftable_5a58c0 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->vftptr_0x0 = (cls_0x5a58c0__vftable_5a58c0 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 2;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a58c0 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = pvStack_c;
  return pcVar1;
}



// Function at 00537a30

void __thiscall OOAnalyzer::cls_0x5a58c0::virt_meth_0x537a30(cls_0x5a58c0 *this)

{
  if ((this->mbr_0x17c != 0) && (this->mbr_0x50 != 0)) {
    (*this->vftptr_0x0->virt_meth_0x537a70_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  return;
}



// Function at 00538210

void __thiscall
OOAnalyzer::cls_0x5a58c0::virt_meth_0x538210(cls_0x5a58c0 *this,int param_1,dword param_2)

{
  cls_0x5b4f30_TPlayer *this_00;
  cls_0x5b4f30_TPlayer *pcVar1;
  undefined uVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  undefined3 extraout_var;
  int iVar6;
  undefined4 uVar7;
  dword dVar8;
  cls_0x539230 *this_01;
  cls_0x539230 *this_02;
  int *piVar9;
  int *piVar10;
  dword in_stack_0000000c;
  char *pcVar11;
  
  cls_0x5b93c4::virt_meth_0x436530((cls_0x5b93c4 *)this,param_1,param_2);
  this_00 = (cls_0x5b4f30_TPlayer *)this->mbr_0x17c;
  if (this_00 == (cls_0x5b4f30_TPlayer *)0x0) {
    return;
  }
  if (param_1 == 5) {
    if ((int)param_2 < 0) {
      return;
    }
    if ((int)in_stack_0000000c < 0) {
      return;
    }
    if ((int)this->mbr_0xc <= (int)param_2) {
      return;
    }
    if ((int)this->mbr_0x10 <= (int)in_stack_0000000c) {
      return;
    }
    if (DAT_0065c9e0 != 0) {
      iVar3 = cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00);
      if (iVar3 != 0) {
        TCharacter::meth_0x47eff0((TCharacter *)&DAT_0065caf0);
        cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
        return;
      }
      if (DAT_0065d1b8 != 1) {
        return;
      }
      cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
      return;
    }
    if (-1 < (int)this->mbr_0x180) {
      return;
    }
    if (-1 < DAT_0065b878) {
      return;
    }
    if (-1 < DAT_0065b090) {
      return;
    }
    piVar4 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0((cls_0x5b4f30_TPlayer *)this->mbr_0x17c);
    if (piVar4 != (int *)0x0) {
      uVar5 = (**(code **)(*piVar4 + 0xd4))(s_eqslot_005e41f4);
      if ((*(short *)(piVar4 + 1) == 3) && ((TPlayScreen *)piVar4[0x19] == DAT_00667fcc)) {
        pcVar11 = s_Spell_Pouch_005e41fc;
        uVar2 = (*((DAT_00667fcc->TScreen).vftptr_0x0)->virt_meth_0x470280_168)(DAT_00667fcc);
        if ((int *)CONCAT31(extraout_var,uVar2) != (int *)0x0) {
          (**(code **)(*(int *)CONCAT31(extraout_var,uVar2) + 0x58))(piVar4,0xffffffff,pcVar11);
          iVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          if (iVar3 < 0) {
            return;
          }
          iVar6 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
          if (iVar6 == 0) {
            return;
          }
          cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar3,0x7f,1);
          return;
        }
      }
      else if (uVar5 != 0xffffffff) {
        if (10 < uVar5) {
          return;
        }
        iVar3 = TPlayScreen::meth_0x519300(DAT_00667fcc);
        if (iVar3 == 0) {
          return;
        }
        iVar3 = TPlayScreen::meth_0x5199b0(DAT_00667fcc,piVar4);
        if (iVar3 != 0) {
          FUN_00473a10(piVar4);
        }
        (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
        if (*(short *)(piVar4 + 1) != 6) {
          return;
        }
        TPlayScreen::meth_0x4cf000_SetWalkMode(DAT_00667fcc);
        return;
      }
      uVar7 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar7);
      return;
    }
    if ((TPlayScreen *)this->mbr_0x17c == (TPlayScreen *)0x0) {
      return;
    }
    TPlayScreen::meth_0x46ff80((TPlayScreen *)this->mbr_0x17c);
    (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
    return;
  }
  if (param_1 == 1) {
    if ((int)param_2 < 0) {
      return;
    }
    if ((int)in_stack_0000000c < 0) {
      return;
    }
    if ((int)this->mbr_0xc <= (int)param_2) {
      return;
    }
    if ((int)this->mbr_0x10 <= (int)in_stack_0000000c) {
      return;
    }
    iVar3 = param_2 - 8;
    if ((((iVar3 < 0) || ((int)(in_stack_0000000c - 0x2a) < 0)) || (3 < iVar3 / 0x2d)) ||
       (((iVar6 = (int)(in_stack_0000000c - 0x2a) / 0x2c, 2 < iVar6 || (0x27 < iVar3 % 0x2d)) ||
        (0x27 < (int)(in_stack_0000000c - 0x2a) % 0x2c)))) {
      dVar8 = 0xffffffff;
    }
    else {
      dVar8 = (iVar3 / 0x2d) * 3 + this->mbr_0x188 + iVar6;
    }
    this->mbr_0x190 = param_2;
    this->mbr_0x180 = dVar8;
    this->mbr_0x184 = dVar8;
    this->mbr_0x194 = in_stack_0000000c;
    this->mbr_0x18c = 0;
    return;
  }
  if (param_1 != 4) {
    return;
  }
  if ((((int)param_2 < 10) || (0x1e < (int)param_2)) ||
     (((int)in_stack_0000000c < 10 || (0x1d < (int)in_stack_0000000c)))) {
    if (-1 < (int)this->mbr_0x180) {
      if ((int)param_2 < 0) goto LAB_00538721;
      if ((((int)in_stack_0000000c < 0) || ((int)this->mbr_0xc <= (int)param_2)) ||
         ((int)this->mbr_0x10 <= (int)in_stack_0000000c)) goto LAB_00538900;
      iVar6 = param_2 - 8;
      iVar3 = in_stack_0000000c - 0x2a;
      if ((((iVar6 < 0) || (iVar3 < 0)) ||
          ((3 < iVar6 / 0x2d ||
           (((2 < iVar3 / 0x2c || (0x27 < iVar6 % 0x2d)) || (0x27 < iVar3 % 0x2c)))))) ||
         (dVar8 = (iVar6 / 0x2d) * 3 + this->mbr_0x188 + iVar3 / 0x2c, (int)dVar8 < 0))
      goto LAB_00538728;
      piVar4 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00);
      piVar9 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0((cls_0x5b4f30_TPlayer *)this->mbr_0x17c);
      if (piVar4 == (int *)0x0) goto LAB_00538728;
      if (piVar9 != (int *)0x0) {
        if (this->mbr_0x18c == 0) {
LAB_00538884:
          if (piVar9 != piVar4) goto LAB_005388e0;
          param_1 = -1;
        }
        else {
          if (piVar9 == piVar4) {
            if (this->mbr_0x18c != 0) goto LAB_005388e0;
            goto LAB_00538884;
          }
          param_1 = piVar4[0x10];
        }
        if ((((DAT_0066829c == 0) || (dVar8 == this->mbr_0x180)) ||
            ((*(short *)(piVar9 + 1) == 0x11 ||
             (iVar3 = (**(code **)(*piVar9 + 0xb8))(DAT_00667fcc,param_1), iVar3 != 0)))) &&
           (iVar3 = (**(code **)(*piVar9 + 0xbc))(DAT_00667fcc,param_1), iVar3 != 0))
        goto LAB_00538721;
      }
LAB_005388e0:
      (**(code **)(*(int *)this->mbr_0x17c + 0x58))(piVar4,dVar8);
      goto LAB_005386ea;
    }
LAB_00538900:
    if (((-1 < (int)param_2) && (-1 < (int)in_stack_0000000c)) &&
       (((int)param_2 < (int)this->mbr_0xc && ((int)in_stack_0000000c < (int)this->mbr_0x10)))) {
      iVar3 = param_2 - 8;
      iVar6 = in_stack_0000000c - 0x2a;
      if ((((iVar3 < 0) || (iVar6 < 0)) || (3 < iVar3 / 0x2d)) ||
         (((2 < iVar6 / 0x2c || (0x27 < iVar3 % 0x2d)) || (0x27 < iVar6 % 0x2c)))) {
        dVar8 = 0xffffffff;
      }
      else {
        dVar8 = (iVar3 / 0x2d) * 3 + this->mbr_0x188 + iVar6 / 0x2c;
      }
      if (DAT_0065b878 < 0) {
        if (-1 < DAT_0065b090) {
          DAT_0065b02c._76_4_ = 1;
          piVar9 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0(DAT_0065b088);
          piVar10 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0((cls_0x5b4f30_TPlayer *)this->mbr_0x17c);
          piVar4 = DAT_0065d674;
          if (piVar9 == (int *)0x0) {
            return;
          }
          for (; piVar4 != (int *)0x0; piVar4 = (int *)piVar4[0x19]) {
            if (piVar4 == piVar9) {
              return;
            }
          }
          if (piVar10 != (int *)0x0) {
            if (piVar10 == piVar9) {
              param_1 = -1;
            }
            else {
              param_1 = piVar9[0x10];
            }
            if ((((DAT_0066829c == 0) || (*(short *)(piVar10 + 1) == 0x11)) ||
                (iVar3 = (**(code **)(*piVar10 + 0xb8))(DAT_00667fcc,param_1), iVar3 != 0)) &&
               (iVar3 = (**(code **)(*piVar10 + 0xbc))(DAT_00667fcc,param_1), iVar3 != 0)) {
              (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
              goto LAB_00538728;
            }
          }
          (**(code **)(*(int *)this->mbr_0x17c + 0x58))(piVar9,dVar8);
          iVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          if ((-1 < iVar3) &&
             (iVar6 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar6 != 0)) {
            cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar3,0x7f,1);
          }
          (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
        }
        goto LAB_00538728;
      }
      piVar4 = (int *)(&DAT_00667fcc->mbr_0x2a0)[DAT_0065b878];
      piVar9 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00);
      if ((piVar4 != (int *)0x0) && (-1 < (int)dVar8)) {
        if (piVar9 == (int *)0x0) {
          TPlayScreen::meth_0x5199b0(DAT_00667fcc,0);
          (**(code **)(*(int *)this->mbr_0x17c + 0x58))(piVar4,dVar8);
          iVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
          if ((-1 < iVar3) &&
             (iVar6 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar6 != 0)) {
            cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar3,0x7f,1);
          }
        }
        else {
          iVar3 = TPlayScreen::meth_0x519300(DAT_00667fcc);
          if (iVar3 == 0) {
            if (this->mbr_0x18c == 0) {
LAB_00538a70:
              if (piVar9 != piVar4) goto LAB_00538acf;
              param_1 = -1;
            }
            else {
              if (piVar9 == piVar4) {
                if (this->mbr_0x18c != 0) goto LAB_00538acf;
                goto LAB_00538a70;
              }
              param_1 = piVar4[0x10];
            }
            if (((((DAT_0066829c != 0) && (dVar8 != this->mbr_0x180)) &&
                 (*(short *)(piVar9 + 1) != 0x11)) &&
                (iVar3 = (**(code **)(*piVar9 + 0xb8))(DAT_00667fcc,param_1), iVar3 == 0)) ||
               (iVar3 = (**(code **)(*piVar9 + 0xbc))(DAT_00667fcc,param_1), iVar3 == 0))
            goto LAB_00538acf;
          }
          else {
            TPlayScreen::meth_0x5199b0(DAT_00667fcc,piVar9);
          }
        }
        (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
      }
LAB_00538acf:
      if (*(short *)(piVar4 + 1) == 6) {
        TPlayScreen::meth_0x4cf000_SetWalkMode(DAT_00667fcc);
      }
      goto LAB_00538728;
    }
  }
  else {
    pcVar1 = (cls_0x5b4f30_TPlayer *)(this_00->TPlayScreen).TScreen.mbr_0x64;
    if (pcVar1 == (cls_0x5b4f30_TPlayer *)0x0) goto LAB_00538728;
    if ((int)this->mbr_0x180 < 0) {
      if (this_00 != pcVar1) {
        this->mbr_0x17c = (dword)pcVar1;
        cls_0x5b93c4::meth_0x538e40((cls_0x5b93c4 *)this);
        (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
        this_01 = (cls_0x539230 *)cls_0x5b93c4::meth_0x539260((cls_0x5b93c4 *)this);
        this_02 = (cls_0x539230 *)cls_0x5b93c4::meth_0x539260((cls_0x5b93c4 *)this);
        if ((this_02 != (cls_0x539230 *)0x0) && (this_01 != (cls_0x539230 *)0x0)) {
          this->mbr_0x188 = 0;
          cls_0x539230::meth_0x539230(this_01);
          cls_0x539230::meth_0x539230(this_02);
        }
        if (DAT_0066829c != 0) {
          cls_0x57d9d0::meth_0x584680((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
        }
      }
      goto LAB_00538728;
    }
    iVar3 = cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00);
    if ((iVar3 == 0) ||
       (iVar6 = (**(code **)(**(int **)(this->mbr_0x17c + 100) + 0x94))(), 0xfe < iVar6))
    goto LAB_00538728;
    iVar6 = **(int **)(this->mbr_0x17c + 100);
    uVar7 = (**(code **)(iVar6 + 0x94))();
    (**(code **)(iVar6 + 0x58))(iVar3,uVar7);
LAB_005386ea:
    iVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if ((-1 < iVar3) &&
       (iVar6 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar6 != 0)) {
      cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar3,0x7f,1);
    }
  }
LAB_00538721:
  (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
LAB_00538728:
  this->mbr_0x180 = 0xffffffff;
  this->mbr_0x18c = 0;
  if ((((-1 < (int)param_2) && (-1 < (int)in_stack_0000000c)) && ((int)param_2 < (int)this->mbr_0xc)
      ) && ((int)in_stack_0000000c < (int)this->mbr_0x10)) {
    FUN_0043a100(0,0,0);
    FUN_0043a140((int *)0x0);
    FUN_0043a170(&UNK_0066f794.field_0x130);
    FUN_0043a240((undefined4 *)0x0,0,0);
    (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  }
  return;
}



// Function at 00538c10

void __thiscall
OOAnalyzer::cls_0x5a58c0::virt_meth_0x538c10(cls_0x5a58c0 *this,int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  cls_0x4a3060 *this_00;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int in_stack_0000000c;
  
  cls_0x5b93c4::virt_meth_0x436660((cls_0x5b93c4 *)this,param_1,param_2);
  if ((((this->mbr_0x17c != 0) && (-1 < param_2)) && (-1 < in_stack_0000000c)) &&
     ((param_2 < (int)this->mbr_0xc && (in_stack_0000000c < (int)this->mbr_0x10)))) {
    if (((param_2 < 10) || ((0x1e < param_2 || (in_stack_0000000c < 10)))) ||
       ((0x1d < in_stack_0000000c || ((param_1 != 1 && (param_1 != 2)))))) {
      if (this->mbr_0x1a4 != 0) {
        this->mbr_0x1a4 = 0;
        (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
      }
    }
    else {
      this->mbr_0x1a4 = 1;
      (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
    }
    if ((((this->mbr_0x18c == 0) && (param_2 < (int)this->mbr_0xc)) &&
        (in_stack_0000000c < (int)this->mbr_0x10)) && ((param_1 == 1 || (param_1 == 2)))) {
      iVar1 = this->mbr_0x190 - param_2;
      if (iVar1 < 1) {
        iVar1 = param_2 - this->mbr_0x190;
      }
      if (iVar1 < 2) {
        iVar1 = this->mbr_0x194 - in_stack_0000000c;
        if (iVar1 < 1) {
          iVar1 = in_stack_0000000c - this->mbr_0x194;
        }
        if (iVar1 < 2) {
          return;
        }
      }
      piVar2 = (int *)cls_0x5b4f30_TPlayer::meth_0x4701f0((cls_0x5b4f30_TPlayer *)this->mbr_0x17c);
      if ((piVar2 != (int *)0x0) && (iVar1 = (**(code **)(*piVar2 + 0x130))(), iVar1 != 0)) {
        iVar4 = (int)(this->mbr_0x190 - 8) % 0x2d;
        iVar5 = (int)(this->mbr_0x194 - 0x2a) % 0x2c;
        iVar1 = iVar4;
        this_00 = (cls_0x4a3060 *)(**(code **)(*piVar2 + 0x130))(iVar4,iVar5);
        iVar1 = cls_0x4a3060::meth_0x4a3060(this_00,iVar1);
        if (iVar1 == 0) {
          iVar5 = 0x14;
          iVar4 = 0x14;
        }
        uVar3 = (**(code **)(*piVar2 + 0x130))();
        FUN_0043a100(uVar3,iVar4,iVar5);
        FUN_0043a140(piVar2);
        FUN_0043a170(&UNK_0066f794.field_0x134);
      }
      this->mbr_0x18c = 1;
      (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
    }
  }
  return;
}



