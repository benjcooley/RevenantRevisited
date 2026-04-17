// Decompiled methods and structure for class: cls_0x5a6b68

/*
/OOAnalyzer/cls_0x5a6b68
pack(disabled)
Structure cls_0x5a6b68 {
   0   TScreen   276   TScreen   "Component (member) class."
}
Length: 276 Alignment: 1

*/

// Function at 004bf4c0

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  local_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



// Function at 004c0960

TScreen * __thiscall
OOAnalyzer::cls_0x5a6b68::virt_meth_0x4c0960(cls_0x5a6b68 *this,undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e1db;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xe0);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c09e0_005a6d8c;
    this_00->mbr_0xd8 = 0xffffffff;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8008;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004c0a00

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  uStack_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



// Function at 004c0ab0

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  uStack_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



// Function at 004c0b60

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  uStack_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



// Function at 004c0c10

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  uStack_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



// Function at 004c0c20

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::virt_meth_0x4c0c20(cls_0x5a6b68 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a6b68(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0c40

cls_0x5a6b68 * __thiscall OOAnalyzer::cls_0x5a6b68::~cls_0x5a6b68(cls_0x5a6b68 *this)

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  TScreen *pcVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  puStack_8 = &LAB_0059e198;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TScreen).vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944
  ;
  uStack_4 = 0;
  if (((this->TScreen).mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30::meth_0x46e630((cls_0x5b4f30 *)this);
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477aa0_220)(DAT_0066c9b4);
  pcVar1 = (this->TScreen).vftptr_0x0;
  if (CONCAT31(extraout_var,uVar2) < 0x20) {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    iVar7 = CONCAT31(extraout_var_00,uVar2);
  }
  else {
    uVar2 = (*pcVar1->FUN_00477aa0_220)(DAT_0066c9b4);
    uVar8 = CONCAT31(extraout_var_01,uVar2) - 0x10U & 0x8000000f;
    if ((int)uVar8 < 0) {
      uVar8 = (uVar8 - 1 | 0xfffffff0) + 1;
    }
    iVar7 = uVar8 + 0x10;
  }
  uVar2 = (*((this->TScreen).vftptr_0x0)->FUN_00477a60_216)(DAT_0066c9a0);
  if (CONCAT31(extraout_var_02,uVar2) != 0) {
    if (0xf < *(short *)((int)&(this->TScreen).mbr_0x4 + 2)) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if (((*(int *)(&UNK_0066b15c.field_0x1040 + (iVar3 * 0x20 + iVar7) * 4) != 0) &&
        ((this->TScreen).mbr_0x64 == DAT_0065d674)) && (iVar3 < 0x10)) {
      iVar6 = iVar7;
      if (0x1e < iVar7) {
        iVar6 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar6) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar6 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
    iVar3 = (int)*(short *)((int)&(this->TScreen).mbr_0x4 + 2);
    if ((*(int *)(&UNK_0066b15c.field_0x40 + (iVar3 * 0x20 + iVar7) * 4) != 0) && (iVar3 < 0x10)) {
      if (0x1e < iVar7) {
        iVar7 = 0x1f;
      }
      iVar3 = (iVar3 * 0x20 + iVar7) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x40 + iVar3) != 0) &&
          (iVar6 = *(int *)(&UNK_0066b15c.field_0x40 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x40 + iVar3) = iVar6, iVar6 < 1)) &&
         (iVar7 * 0x80 != -0x66a938)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066a92c.field_0xc + iVar3));
        *(undefined4 *)(&UNK_0066a92c.field_0xc + iVar3) = 0;
      }
    }
  }
  pcVar5 = TScreen::~TScreen(&this->TScreen);
  ExceptionList = puStack_8;
  return (cls_0x5a6b68 *)pcVar5;
}



