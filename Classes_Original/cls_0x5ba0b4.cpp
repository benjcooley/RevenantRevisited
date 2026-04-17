// Decompiled methods and structure for class: cls_0x5ba0b4

/*
/OOAnalyzer/cls_0x5ba0b4
pack(disabled)
Structure cls_0x5ba0b4 {
   0   cls_0x5ba0a0   60   cls_0x5ba0a0   "Component (member) class."
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   byte   1   mbr_0x54   "Unsigned Byte (db)"
   85   byte   1   mbr_0x55   "Unsigned Byte (db)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   113   byte   1   mbr_0x71   "Unsigned Byte (db)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   byte   1   mbr_0x7c   "Unsigned Byte (db)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 168 Alignment: 1

*/

// Function at 0057cc90

void __thiscall
OOAnalyzer::cls_0x5ba0b4::meth_0x57cc90(cls_0x5ba0b4 *this,cls_0x5b4f30 *param_1,undefined param_2)

{
  byte bVar1;
  int *piVar2;
  ushort uVar3;
  short sVar4;
  undefined uVar5;
  ushort *puVar6;
  undefined3 *puVar7;
  short *psVar8;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar9;
  dword unaff_EBX;
  int unaff_EBP;
  uint unaff_ESI;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  byte local_10;
  
  puVar6 = (ushort *)cls_0x588410::meth_0x5725f0(_param_2);
  bVar1 = *(byte *)puVar6;
  uVar3 = *puVar6;
  if ((uVar3 & 4) != 0) {
    cls_0x588410::meth_0x5725f0(_param_2);
  }
  if ((bVar1 & 0x10) != 0) {
    puVar7 = (undefined3 *)cls_0x588410::meth_0x5725f0(_param_2);
    meth_0x57e2e0(this,(char)*puVar7);
  }
  psVar8 = (short *)cls_0x588410::meth_0x5725f0(_param_2);
  sVar4 = *psVar8;
  cls_0x588410::meth_0x5725f0(_param_2);
  cls_0x588410::meth_0x5725f0(_param_2);
  if (param_1 != (cls_0x5b4f30 *)0x0) {
    if (in_stack_0000000c == '\0') {
      uVar5 = (*((param_1->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477570_376)(param_1);
      if (((CONCAT31(extraout_var,uVar5) != 2) &&
          ((*((param_1->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e60_452)(param_1),
          0 < sVar4)) &&
         (uVar5 = (*(param_1->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0[1].virt_meth_0x4778a0_12)
                            (param_1), *(int *)CONCAT31(extraout_var_00,uVar5) == 0x13)) {
        cls_0x5a7b98::meth_0x4d4220(&param_1->cls_0x5a7b98);
      }
      (*((param_1->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e80_460)(param_1);
      (*((param_1->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477ea0_468)(param_1);
      local_10 = (byte)unaff_ESI;
      if ((unaff_ESI & 1) == 0) {
        if ((unaff_ESI & 2) == 0) {
          if (((param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 == 0) ||
             ((((iVar9 = cls_0x5a7b98::meth_0x45f770(&param_1->cls_0x5a7b98), iVar9 == 0 ||
                ((iVar9 = FUN_004dab80(s_combat_005e0634), iVar9 == 0 &&
                 (iVar9 = FUN_004dab80(&DAT_005e064c), iVar9 == 0)))) &&
               ((piVar2 = (int *)(param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0, piVar2 == (int *)0x0
                || ((*piVar2 != 0x19 || (iVar9 = FUN_004dab80(&DAT_005e0648), iVar9 == 0)))))) &&
              (iVar9 = FUN_004dab80(&DAT_005e062c), iVar9 == 0)))) {
            cls_0x5a7b98::meth_0x4cf000(&param_1->cls_0x5a7b98);
          }
        }
        else if (((param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 == 0) ||
                (iVar9 = FUN_004dab80(s_sneak_005c618c), iVar9 == 0)) {
          cls_0x5b4f30::meth_0x4cf2e0(param_1);
        }
      }
      else if (((param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 == 0) ||
              (((iVar9 = cls_0x5a7b98::meth_0x45f770(&param_1->cls_0x5a7b98), iVar9 == 0 ||
                ((iVar9 = FUN_004dab80(s_combatrun_005e0618), iVar9 == 0 &&
                 (iVar9 = FUN_004dab80(s_handrun_005e0610), iVar9 == 0)))) &&
               (((piVar2 = (int *)(param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0,
                 piVar2 == (int *)0x0 ||
                 ((*piVar2 != 0x19 || (iVar9 = FUN_004dab80(s_bowrun_005e0608), iVar9 == 0)))) &&
                (iVar9 = FUN_004dab80(&DAT_005e0604), iVar9 == 0)))))) {
        cls_0x5a7b98::meth_0x4cf490(&param_1->cls_0x5a7b98);
      }
      piVar2 = (int *)(param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xd8;
      if (uVar3 == 0) {
        if ((piVar2 != (int *)0x0) &&
           (((iVar9 = *piVar2, iVar9 == 2 || (iVar9 == 4)) || (iVar9 == 0x1a)))) {
          cls_0x5a7b98::meth_0x4cee70(&param_1->cls_0x5a7b98);
        }
      }
      else if ((piVar2 == (int *)0x0) ||
              ((((iVar9 = *piVar2, iVar9 != 2 && (iVar9 != 4)) && (iVar9 != 0x1a)) ||
               ((param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xb0 != unaff_EBX)))) {
        cls_0x5b4f30::meth_0x4ced20(param_1);
      }
      if ((unaff_ESI & 8) == 0) {
        cls_0x5a7b98::meth_0x4d3fd0(&param_1->cls_0x5a7b98);
      }
      else {
        iVar9 = cls_0x5a7b98::meth_0x45f770(&param_1->cls_0x5a7b98);
        if ((iVar9 == 0) &&
           ((piVar2 = (int *)(param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0, piVar2 == (int *)0x0 ||
            (*piVar2 != 0x19)))) {
          cls_0x5a7b98::meth_0x4d3b90(&param_1->cls_0x5a7b98);
        }
        else {
          iVar9 = cls_0x5a7b98::meth_0x45f770(&param_1->cls_0x5a7b98);
          if ((iVar9 == 0) &&
             ((piVar2 = (int *)(param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0, piVar2 == (int *)0x0
              || (*piVar2 != 0x19)))) {
            iVar9 = 0;
          }
          else {
            iVar9 = *(int *)((param_1->cls_0x5a7b98).cls_0x5a50e8.mbr_0xe0 + 0x44);
          }
          if (iVar9 != unaff_EBP) {
            cls_0x5a7b98::meth_0x4d4790(&param_1->cls_0x5a7b98);
          }
        }
      }
    }
    else {
      local_10 = (byte)uVar3;
    }
    if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
      if (param_1 == DAT_00667fcc) {
        if (((local_10 & 0x40) == 0) && (((param_1->cls_0x5a7b98).mbr_0x36c & 4) != 0)) {
          cls_0x5b4f30::meth_0x51d680(param_1);
        }
        if (((local_10 & 0x80) == 0) && (((param_1->cls_0x5a7b98).mbr_0x36c & 8) != 0)) {
          cls_0x5b4f30::meth_0x51d680(param_1);
        }
        if (((unaff_ESI & 0x100) != 0) && (((param_1->cls_0x5a7b98).mbr_0x36c & 0x10) != 0)) {
          cls_0x5b4f30::meth_0x51d680(param_1);
        }
      }
      if (((unaff_ESI & 0x200) == 0) && (iVar9 = cls_0x5b4f30::meth_0x4d5790(param_1), iVar9 == 0))
      {
        cls_0x5b4f30::meth_0x4d56c0(param_1);
      }
    }
  }
  return;
}



// Function at 0057d750

cls_0x5ba0b4 * __thiscall OOAnalyzer::cls_0x5ba0b4::cls_0x5ba0b4(cls_0x5ba0b4 *this)

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a286e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  this->mbr_0x44 = (dword)&this->mbr_0x3c;
  this->mbr_0x3c = (dword)&this->mbr_0x40;
  this->mbr_0x40 = 0;
  this->mbr_0x50 = (dword)&this->mbr_0x48;
  this->mbr_0x48 = (dword)&this->mbr_0x4c;
  this->mbr_0x4c = 0;
  local_4 = 2;
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba0b4__vftable_5ba0b4_005ba0b4;
  *(undefined *)&(this->cls_0x5ba0a0).cls_0x588410.mbr_0x34 = 0;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24 = 0;
  meth_0x57df80(this);
  this->mbr_0x71 = 0;
  this->mbr_0x9c = 0;
  this->mbr_0x55 = 0;
  this->mbr_0x88 = (dword)&this->mbr_0x80;
  this->mbr_0x80 = (dword)&this->mbr_0x84;
  this->mbr_0x84 = 0;
  this->mbr_0x54 = 0;
  this->mbr_0x94 = (dword)&this->mbr_0x8c;
  this->mbr_0x8c = (dword)&this->mbr_0x90;
  this->mbr_0x5c = 0;
  this->mbr_0x90 = 0;
  this->mbr_0x60 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x74 = 0;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0xa0 = 0;
  this->mbr_0xa4 = 0xffffffff;
  ExceptionList = local_c;
  return this;
}



// Function at 0057de10

cls_0x5ba0b4 * __thiscall OOAnalyzer::cls_0x5ba0b4::cls_0x5ba0b4(cls_0x5ba0b4 *this)

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a28f2;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5ba0a0::cls_0x5ba0a0(&this->cls_0x5ba0a0);
  this->mbr_0x44 = (dword)&this->mbr_0x3c;
  this->mbr_0x3c = (dword)&this->mbr_0x40;
  this->mbr_0x40 = 0;
  this->mbr_0x50 = (dword)&this->mbr_0x48;
  this->mbr_0x48 = (dword)&this->mbr_0x4c;
  this->mbr_0x4c = 0;
  local_4 = 2;
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba0b4__vftable_5ba0b4_005ba0b4;
  *(undefined *)&(this->cls_0x5ba0a0).cls_0x588410.mbr_0x34 = 0;
  (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24 = 0;
  meth_0x57df80(this);
  ExceptionList = local_c;
  return this;
}



// Function at 0057de90

cls_0x5ba0b4 * __thiscall OOAnalyzer::cls_0x5ba0b4::~cls_0x5ba0b4(cls_0x5ba0b4 *this)

{
  cls_0x588410 *pcVar1;
  dword dVar2;
  dword *pdVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  cls_0x5ba0b4 *pcVar7;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a291a;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5ba0a0).vftptr_0x0 =
       (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5ba0b4__vftable_5ba0b4_005ba0b4;
  local_4 = 2;
  while ((*(int *)this->mbr_0x48 != 0 &&
         (pcVar1 = (cls_0x588410 *)((int *)this->mbr_0x48)[2], pcVar1 != (cls_0x588410 *)0x0))) {
    dVar2 = pcVar1->mbr_0x1c;
    if (dVar2 != 0) {
      pdVar3 = (dword *)pcVar1->mbr_0x20;
      pcVar1->mbr_0x1c = 0;
      *(dword **)(dVar2 + 4) = pdVar3;
      *pdVar3 = dVar2;
    }
    cls_0x588410::meth_0x588480(pcVar1);
    FUN_004830f0(pcVar1);
  }
  while ((*(int *)this->mbr_0x3c != 0 &&
         (pcVar1 = (cls_0x588410 *)((int *)this->mbr_0x3c)[2], pcVar1 != (cls_0x588410 *)0x0))) {
    dVar2 = pcVar1->mbr_0x1c;
    if (dVar2 != 0) {
      pdVar3 = (dword *)pcVar1->mbr_0x20;
      pcVar1->mbr_0x1c = 0;
      *(dword **)(dVar2 + 4) = pdVar3;
      *pdVar3 = dVar2;
    }
    cls_0x588410::meth_0x588480(pcVar1);
    FUN_004830f0(pcVar1);
  }
  piVar4 = (int *)this->mbr_0x48;
  iVar5 = *piVar4;
  while (iVar5 != 0) {
    iVar5 = *piVar4;
    if (iVar5 != 0) {
      piVar6 = (int *)piVar4[1];
      *piVar4 = 0;
      *(int **)(iVar5 + 4) = piVar6;
      *piVar6 = iVar5;
    }
    iVar5 = *piVar4;
  }
  piVar4 = (int *)this->mbr_0x3c;
  iVar5 = *piVar4;
  while (iVar5 != 0) {
    iVar5 = *piVar4;
    if (iVar5 != 0) {
      piVar6 = (int *)piVar4[1];
      *piVar4 = 0;
      *(int **)(iVar5 + 4) = piVar6;
      *piVar6 = iVar5;
    }
    iVar5 = *piVar4;
  }
  local_4 = 0xffffffff;
  pcVar7 = (cls_0x5ba0b4 *)cls_0x5ba0a0::~cls_0x5ba0a0(&this->cls_0x5ba0a0);
  ExceptionList = local_c;
  return pcVar7;
}



// Function at 0057df80

void __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57df80(cls_0x5ba0b4 *this)

{
  undefined4 *puVar1;
  dword dVar2;
  cls_0x588410 *pcVar3;
  dword *pdVar4;
  int iVar5;
  dword *pdVar6;
  int iVar7;
  bool bVar8;
  int in_stack_00000004;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2937;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5ba038::meth_0x57c8f0((cls_0x5ba038 *)this);
  iVar7 = 0;
  for (puVar1 = *(undefined4 **)this->mbr_0x3c; puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)*puVar1) {
    iVar7 = iVar7 + 1;
  }
  iVar5 = 0;
  for (puVar1 = *(undefined4 **)this->mbr_0x48; puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)*puVar1) {
    iVar5 = iVar5 + 1;
  }
  iVar7 = (iVar5 + iVar7) - in_stack_00000004;
  bVar8 = iVar7 < 0;
  if (iVar7 < 1) goto LAB_0057e065;
  do {
    if ((*(int *)this->mbr_0x3c == 0) ||
       (pcVar3 = (cls_0x588410 *)((int *)this->mbr_0x3c)[2], pcVar3 == (cls_0x588410 *)0x0)) {
      bVar8 = iVar7 < 0;
      if (0 < iVar7) goto LAB_0057e02a;
      goto LAB_0057e065;
    }
    local_4 = 0;
    cls_0x588050::meth_0x588050((cls_0x588050 *)&pcVar3->mbr_0x1c);
    local_4 = 0xffffffff;
    cls_0x588410::meth_0x588480(pcVar3);
    FUN_004830f0(pcVar3);
    iVar7 = iVar7 + -1;
  } while (0 < iVar7);
  goto LAB_0057e063;
  while( true ) {
    dVar2 = pcVar3->mbr_0x1c;
    if (dVar2 != 0) {
      pdVar4 = (dword *)pcVar3->mbr_0x20;
      pcVar3->mbr_0x1c = 0;
      *(dword **)(dVar2 + 4) = pdVar4;
      *pdVar4 = dVar2;
    }
    cls_0x588410::meth_0x588480(pcVar3);
    FUN_004830f0(pcVar3);
    iVar7 = iVar7 + -1;
    if (iVar7 < 1) break;
LAB_0057e02a:
    if ((*(int *)this->mbr_0x48 == 0) ||
       (pcVar3 = (cls_0x588410 *)((int *)this->mbr_0x48)[2], pcVar3 == (cls_0x588410 *)0x0)) break;
  }
LAB_0057e063:
  bVar8 = iVar7 < 0;
LAB_0057e065:
  if (bVar8) {
    iVar7 = -iVar7;
    do {
      pcVar3 = (cls_0x588410 *)FUN_00482fb0(0x28);
      local_4 = 1;
      if (pcVar3 == (cls_0x588410 *)0x0) {
        pcVar3 = (cls_0x588410 *)0x0;
      }
      else {
        cls_0x588410::cls_0x588410(pcVar3);
        pcVar3->mbr_0x18 = 0;
        pcVar3->mbr_0x20 = 0;
        pcVar3->mbr_0x1c = 0;
        pcVar3->mbr_0x24 = 0;
        pcVar3->mbr_0x24 = (dword)pcVar3;
      }
      local_4 = 0xffffffff;
      if (pcVar3 == (cls_0x588410 *)0x0) {
        pdVar4 = (dword *)0x0;
      }
      else {
        pdVar4 = &pcVar3->mbr_0x1c;
      }
      dVar2 = *pdVar4;
      if (dVar2 != 0) {
        *(dword *)(dVar2 + 4) = pdVar4[1];
        *(dword *)pdVar4[1] = dVar2;
        *pdVar4 = 0;
      }
      pdVar6 = &this->mbr_0x3c;
      iVar7 = iVar7 + -1;
      pdVar4[1] = (dword)pdVar6;
      dVar2 = *pdVar6;
      *pdVar6 = (dword)pdVar4;
      *pdVar4 = dVar2;
      *(dword **)(dVar2 + 4) = pdVar4;
    } while (iVar7 != 0);
  }
  cls_0x5ba038::meth_0x57c900((cls_0x5ba038 *)this);
  ExceptionList = local_c;
  return;
}



// Function at 0057e100

void __thiscall
OOAnalyzer::cls_0x5ba0b4::virt_meth_0x57e100
          (cls_0x5ba0b4 *this,undefined4 param_1,undefined4 param_2)

{
  dword *pdVar1;
  dword **ppdVar2;
  dword *pdVar3;
  cls_0x588410 *this_00;
  
  cls_0x5ba038::meth_0x57c8f0((cls_0x5ba038 *)this);
  if ((*(char *)&(this->cls_0x5ba0a0).cls_0x588410.mbr_0x34 != '\0') &&
     (((*(int *)this->mbr_0x3c != 0 &&
       (this_00 = (cls_0x588410 *)((int *)this->mbr_0x3c)[2], this_00 != (cls_0x588410 *)0x0)) ||
      ((*(int *)this->mbr_0x48 != 0 &&
       (this_00 = (cls_0x588410 *)((int *)this->mbr_0x48)[2], this_00 != (cls_0x588410 *)0x0)))))) {
    this_00->mbr_0xc = this_00->mbr_0x0;
    this_00->mbr_0x10 = 0;
    this_00->mbr_0x18 = 0;
    cls_0x588410::meth_0x5891c0(this_00,&param_1);
    cls_0x588410::meth_0x5891c0(this_00,param_2);
    if (this_00 == (cls_0x588410 *)0x0) {
      pdVar3 = (dword *)0x0;
    }
    else {
      pdVar3 = &this_00->mbr_0x1c;
    }
    pdVar1 = (dword *)*pdVar3;
    if (pdVar1 != (dword *)0x0) {
      ppdVar2 = (dword **)pdVar3[1];
      pdVar1[1] = (dword)ppdVar2;
      *ppdVar2 = pdVar1;
      *pdVar3 = 0;
    }
    ppdVar2 = (dword **)this->mbr_0x50;
    *pdVar3 = (dword)&this->mbr_0x4c;
    pdVar3[1] = (dword)ppdVar2;
    this->mbr_0x50 = (dword)pdVar3;
    *ppdVar2 = pdVar3;
  }
  cls_0x5ba038::meth_0x57c900((cls_0x5ba038 *)this);
  return;
}



// Function at 0057e2e0

uint __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57e2e0(cls_0x5ba0b4 *this,undefined param_1)

{
  dword *pdVar1;
  short sVar2;
  cls_0x588410 *pcVar3;
  char cVar4;
  uint uVar5;
  int iVar6;
  undefined *puVar7;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  cls_0x44ceb0 local_48;
  
  if ((_param_1 == 0) || (_param_1 == -1)) {
    return 0;
  }
  cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x2a0,0,0);
  uVar5 = local_48.mbr_0xc;
  do {
    if (uVar5 == 0) {
LAB_0057e341:
      uVar5 = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
      if ((uVar5 == 0) && (DAT_00676828 != 0)) {
        if (DAT_0067682c == 0) {
          if (((DAT_00676828 != 0) &&
              (iVar6 = cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738), iVar6 != 0)) &&
             (cVar4 = cls_0x57d9d0::meth_0x583b60((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,param_1),
             cVar4 == '\0')) {
            pcVar3 = *(cls_0x588410 **)(iVar6 + 0x14);
            puVar7 = (undefined *)pcVar3->mbr_0xc;
            pcVar3->mbr_0xc = (dword)(puVar7 + 1);
            if ((undefined *)pcVar3->mbr_0x4 < puVar7 + 1) {
              puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
            }
            *puVar7 = 1;
            puVar7 = (undefined *)cls_0x588410::meth_0x5725f0(pcVar3);
            *puVar7 = param_1;
            puVar7[1] = (char)in_stack_00000005;
            puVar7[2] = in_stack_00000005._1_1_;
            puVar7[3] = in_stack_00000005._2_1_;
          }
        }
        else if ((DAT_00676828 != 0) &&
                (iVar6 = cls_0x57d9d0::meth_0x57d9d0
                                   ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,0,0xf,3),
                pcVar3 = DAT_00676e68, iVar6 != 0)) {
          puVar7 = (undefined *)DAT_00676e68->mbr_0xc;
          pdVar1 = &DAT_00676e68->mbr_0x4;
          DAT_00676e68->mbr_0xc = (dword)(puVar7 + 4);
          if ((undefined *)*pdVar1 < puVar7 + 4) {
            puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
          }
          *puVar7 = param_1;
          puVar7[1] = (char)in_stack_00000005;
          puVar7[2] = in_stack_00000005._1_1_;
          puVar7[3] = in_stack_00000005._2_1_;
          cls_0x57d9d0::meth_0x57dc70((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
        }
      }
LAB_0057e3cd:
      if (in_stack_00000008 != '\0') {
        (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = uVar5;
        if (uVar5 != 0) {
          sVar2 = *(short *)(uVar5 + 4);
          if ((sVar2 == 0xc) || (sVar2 == 0xb)) {
            *(uint *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = uVar5;
          }
          *(uint *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = (sVar2 != 0xb) - 1 & uVar5;
          return uVar5;
        }
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = 0;
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
      }
      return uVar5;
    }
    if (*(int *)(uVar5 + 0x40) == _param_1) {
      if (uVar5 != 0) goto LAB_0057e3cd;
      goto LAB_0057e341;
    }
    local_48.mbr_0xc = uVar5;
    cls_0x44ceb0::meth_0x44d080(&local_48);
    uVar5 = local_48.mbr_0xc;
  } while( true );
}



// Function at 0057e490

void __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57e490(cls_0x5ba0b4 *this,undefined param_1)

{
  short sVar1;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  cls_0x44ceb0 local_48;
  
  cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x2a0,0,0);
  do {
    if (local_48.mbr_0xc == 0) {
LAB_0057e4d5:
      local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_0057e4e0:
      if (in_stack_00000008 != '\0') {
        (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = local_48.mbr_0xc;
        if (local_48.mbr_0xc != 0) {
          sVar1 = *(short *)(local_48.mbr_0xc + 4);
          if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
            *(dword *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = local_48.mbr_0xc;
          }
          *(uint *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 =
               (sVar1 != 0xb) - 1 & local_48.mbr_0xc;
          return;
        }
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = 0;
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
      }
      return;
    }
    if (*(int *)(local_48.mbr_0xc + 0x40) == _param_1) {
      if (local_48.mbr_0xc != 0) goto LAB_0057e4e0;
      goto LAB_0057e4d5;
    }
    cls_0x44ceb0::meth_0x44d080(&local_48);
  } while( true );
}



// Function at 0057e5a0

void __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57e5a0(cls_0x5ba0b4 *this)

{
  uint uVar1;
  int *piVar2;
  uint uVar3;
  cls_0x588410 *this_00;
  byte bVar4;
  undefined uVar5;
  uint *puVar6;
  int iVar7;
  dword dVar8;
  dword dVar9;
  undefined *puVar10;
  short sVar11;
  cls_0x588410 *in_stack_00000004;
  uint local_18;
  uint local_14;
  uint local_10;
  undefined4 local_8;
  uint local_4;
  
  puVar6 = (uint *)cls_0x588410::meth_0x589310(in_stack_00000004);
  uVar1 = *puVar6;
  piVar2 = (int *)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x28;
  uVar3 = puVar6[1];
  local_4 = puVar6[2];
  if (piVar2 != (int *)0x0) {
    uVar5 = (undefined)(uVar3 >> 0x18);
    sVar11 = (short)uVar3;
    local_8 = uVar3;
    if (DAT_0067682c != 0) {
      iVar7 = (**(code **)(*piVar2 + 0x178))();
      if ((iVar7 != 2) ||
         (dVar8 = (**(code **)(*(int *)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 + 0x184))(),
         (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24 != dVar8)) {
        piVar2 = (int *)(this->cls_0x5ba0a0).cls_0x588410.mbr_0x28;
        dVar8 = (this->cls_0x5ba0a0).cls_0x588410.mbr_0x24;
        if (piVar2 != (int *)0x0) {
          dVar9 = (**(code **)(*piVar2 + 0x184))();
          if (dVar9 == dVar8) {
            (**(code **)(*piVar2 + 0x180))(0);
          }
          if (((DAT_0067682c != 0) &&
              (iVar7 = cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738), iVar7 != 0)) &&
             (this_00 = *(cls_0x588410 **)(iVar7 + 0x14), this_00 != (cls_0x588410 *)0x0)) {
            puVar10 = (undefined *)cls_0x588410::meth_0x5725f0(this_00);
            *puVar10 = 4;
            puVar10 = (undefined *)this_00->mbr_0xc;
            iVar7 = piVar2[0x10];
            this_00->mbr_0xc = (dword)(puVar10 + 4);
            if ((undefined *)this_00->mbr_0x4 < puVar10 + 4) {
              puVar10 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
            }
            in_stack_00000004._2_1_ = (undefined)((uint)iVar7 >> 0x10);
            in_stack_00000004._3_1_ = (undefined)((uint)iVar7 >> 0x18);
            *puVar10 = (char)iVar7;
            puVar10[1] = (char)((uint)iVar7 >> 8);
            puVar10[2] = in_stack_00000004._2_1_;
            puVar10[3] = in_stack_00000004._3_1_;
          }
        }
        (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = 0;
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = 0;
        *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
        return;
      }
      iVar7 = *(int *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30;
      if ((iVar7 != 0) && ((*(byte *)(iVar7 + 0x36c) & 4) != 0)) {
        return;
      }
      dVar8 = (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28;
      if ((local_8 >> 0x10 & 0xff) != (uint)*(ushort *)(dVar8 + 0xe)) {
        DAT_00676ea4 = DAT_00676ea4 | 1;
        if ((DAT_00676828 != 0) &&
           (iVar7 = cls_0x57d9d0::meth_0x57d9d0
                              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)dVar8,0,1), iVar7 != 0)
           ) {
          cls_0x57d9d0::meth_0x57dc70((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
        }
        DAT_00676ea4 = DAT_00676ea4 & 0xfffffffe;
        return;
      }
      local_18 = uVar1 & 0xffff;
      local_14 = uVar1 >> 0x10;
      local_10 = (uint)sVar11;
      if ((DAT_00676828 == 0) || ((DAT_006767fc & 0x80) == 0)) {
        (**(code **)(**(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c + 0xc))(&local_18);
        (**(code **)(**(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c + 0x114))();
      }
      else {
        (**(code **)(**(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c + 8))
                  (&local_18,0xffffffff,0);
      }
      dVar8 = (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28;
      if (uVar3 >> 0x18 != (uint)*(byte *)(dVar8 + 0x36)) {
        *(undefined *)(dVar8 + 0x36) = uVar5;
      }
      if ((local_10 & 0xff) == *(uint *)(dVar8 + 0xb0)) {
        return;
      }
      *(uint *)(dVar8 + 0xb0) = local_10 & 0xff;
      return;
    }
    if (uVar3 >> 0x18 != (uint)*(byte *)((int)piVar2 + 0x36)) {
      *(undefined *)((int)piVar2 + 0x36) = uVar5;
    }
    if ((local_4 & 0xff) != piVar2[0x2c]) {
      piVar2[0x2c] = local_4 & 0xff;
    }
    local_18 = uVar1 & 0xffff;
    local_8._2_1_ = (byte)(uVar3 >> 0x10);
    bVar4 = local_8._2_1_;
    if (((local_18 != piVar2[4]) || (uVar1 >> 0x10 != piVar2[5])) ||
       (((int)sVar11 != piVar2[6] || ((ushort)local_8._2_1_ != *(ushort *)((int)piVar2 + 0xe))))) {
      local_14 = uVar1 >> 0x10;
      local_10 = (uint)sVar11;
      (**(code **)(*piVar2 + 8))(&local_18,bVar4,0);
    }
  }
  return;
}



// Function at 0057e990

int * __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57e990(cls_0x5ba0b4 *this,undefined param_1)

{
  uint uVar1;
  undefined uVar2;
  short sVar3;
  int iVar4;
  undefined *puVar5;
  int *piVar6;
  int iVar7;
  undefined4 uVar8;
  int *piVar9;
  bool bVar10;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  cls_0x587e30 local_c;
  
  uVar1 = _param_1->mbr_0xc + 2;
  _param_1->mbr_0xc = uVar1;
  if (_param_1->mbr_0x4 < uVar1) {
    cls_0x588410::meth_0x5884a0(_param_1);
  }
  puVar5 = (undefined *)_param_1->mbr_0xc;
  _param_1->mbr_0xc = (dword)(puVar5 + 1);
  if ((undefined *)_param_1->mbr_0x4 < puVar5 + 1) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(_param_1);
  }
  local_c.mbr_0x8 = _param_1->mbr_0x8;
  uVar2 = *puVar5;
  local_c.mbr_0x0 = _param_1->mbr_0x0;
  piVar9 = (int *)0x0;
  local_c.mbr_0x4 = local_c.mbr_0x0;
  cls_0x587e30::meth_0x587e30(&local_c);
  DAT_0065a254 = 0;
  if ((DAT_00676828 == 0) || (DAT_0067682c != 0)) {
    FUN_00472090(&local_c,0);
  }
  else {
    piVar9 = (int *)FUN_00471ce0(&local_c,0xf,1);
  }
  cls_0x588410::meth_0x588180(_param_1);
  if (piVar9 != (int *)0x0) {
    piVar6 = (int *)meth_0x57e490(this,(char)piVar9[0x10]);
    if (piVar6 != (int *)0x0) {
      iVar7 = (**(code **)(*piVar6 + 0x134))();
      if (iVar7 != 0) {
        (**(code **)(*piVar6 + 0x60))();
      }
      FUN_0046e6d0(piVar6);
    }
    (**(code **)(*piVar9 + 8))(piVar9 + 4,uVar2,0);
    if (*(short *)(piVar9 + 1) == 0xb) {
      bVar10 = false;
      iVar7 = cls_0x45f7c0::meth_0x51f2e0((cls_0x45f7c0 *)&DAT_0065a890);
      if (-1 < iVar7) {
        piVar6 = (int *)cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,iVar7);
        bVar10 = iVar7 == DAT_0065a8b8;
        if (piVar6 != (int *)0x0) {
          iVar4 = *piVar9;
          uVar8 = (**(code **)(*piVar6 + 0x184))();
          (**(code **)(iVar4 + 0x180))(uVar8);
          cls_0x45f7c0::meth_0x51f200((cls_0x45f7c0 *)&DAT_0065a890,iVar7);
          FUN_0046e6d0(piVar6);
        }
      }
      cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,piVar9);
      if (bVar10) {
        cls_0x45f7c0::meth_0x51eef0((cls_0x45f7c0 *)&DAT_0065a890);
      }
      cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
    }
    iVar7 = cls_0x5a5320::meth_0x451090((cls_0x5a5320 *)&DAT_006668d8);
    if ((iVar7 == 0) || (iVar7 == -1)) {
      FUN_0046e6d0(piVar9);
      piVar9 = (int *)0x0;
    }
  }
  if (in_stack_00000008 != '\0') {
    (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = (dword)piVar9;
    if (piVar9 != (int *)0x0) {
      sVar3 = *(short *)(piVar9 + 1);
      if ((sVar3 == 0xc) || (sVar3 == 0xb)) {
        *(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = piVar9;
      }
      *(uint *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = (sVar3 != 0xb) - 1 & (uint)piVar9;
      return piVar9;
    }
    *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = 0;
    *(undefined4 *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = 0;
  }
  return piVar9;
}



// Function at 0057ebb0

int * __thiscall OOAnalyzer::cls_0x5ba0b4::meth_0x57ebb0(cls_0x5ba0b4 *this,undefined param_1)

{
  int iVar1;
  byte *pbVar2;
  int *piVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int *piVar7;
  bool bVar8;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  dword local_c;
  dword local_8;
  uint local_4;
  
  pbVar2 = (byte *)_param_1->mbr_0xc;
  _param_1->mbr_0xc = (dword)(pbVar2 + 1);
  if ((byte *)_param_1->mbr_0x4 < pbVar2 + 1) {
    pbVar2 = (byte *)cls_0x588410::meth_0x5884a0(_param_1);
  }
  local_4 = _param_1->mbr_0x8;
  uVar6 = (uint)*pbVar2;
  local_c = _param_1->mbr_0x0;
  uVar5 = _param_1->mbr_0xc - local_c;
  piVar7 = (int *)0x0;
  local_8 = local_c;
  if (uVar5 <= local_4) {
    local_8 = uVar5 + local_c;
  }
  DAT_0065a254 = 0;
  if ((DAT_00676828 == 0) || (DAT_0067682c != 0)) {
    FUN_00472090(&local_c,0);
  }
  else {
    piVar7 = (int *)FUN_00471ce0(&local_c,0xf,1);
  }
  cls_0x588410::meth_0x588180(_param_1);
  piVar3 = (int *)0x0;
  if (piVar7 != (int *)0x0) {
    if (*(short *)(piVar7 + 1) != 0xb) {
      FUN_00481c10(s_NetObjectManager__Sent_Player_no_005e5cb8,0);
    }
    piVar3 = (int *)cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,uVar6);
    bVar8 = uVar6 == DAT_0065a8b8;
    if (piVar3 != (int *)0x0) {
      if (piVar3 == DAT_00676e80) {
        DAT_00676e84 = 1;
      }
      iVar1 = *piVar7;
      uVar4 = (**(code **)(*piVar3 + 0x184))();
      (**(code **)(iVar1 + 0x180))(uVar4);
      cls_0x45f7c0::meth_0x51f200((cls_0x45f7c0 *)&DAT_0065a890,uVar6);
      FUN_0046e6d0(piVar3);
    }
    cls_0x45f7c0::meth_0x51f0a0((cls_0x45f7c0 *)&DAT_0065a890,piVar7);
    if (bVar8) {
      cls_0x45f7c0::meth_0x51eef0((cls_0x45f7c0 *)&DAT_0065a890);
    }
    cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
    piVar3 = piVar7;
  }
  if (in_stack_00000008 != '\0') {
    (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = (dword)piVar7;
    *(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = piVar7;
    *(int **)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = piVar7;
  }
  return piVar3;
}



// Function at 0057ed50

dword __thiscall
OOAnalyzer::cls_0x5ba0b4::meth_0x57ed50(cls_0x5ba0b4 *this,cls_0x588410 *param_1,undefined4 param_2)

{
  int iVar1;
  word wVar2;
  undefined *puVar3;
  uint *puVar4;
  byte *pbVar5;
  word *pwVar6;
  undefined2 extraout_var;
  dword dVar7;
  char cStack0000000c;
  undefined3 uStack0000000d;
  undefined4 *puVar8;
  undefined uVar9;
  cls_0x56fe30 local_18 [2];
  int *local_14;
  int local_10;
  uint local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (cStack0000000c == '\0') {
    puVar3 = (undefined *)param_1->mbr_0xc;
    param_1->mbr_0xc = (dword)(puVar3 + 1);
    if ((undefined *)param_1->mbr_0x4 < puVar3 + 1) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(param_1);
    }
    _cStack0000000c = CONCAT31(uStack0000000d,*puVar3);
  }
  uVar9 = SUB41(param_1,0);
  dVar7 = _cStack0000000c;
  switch(_cStack0000000c & 0xff) {
  case 5:
    dVar7 = meth_0x57e2e0(this,0xff);
    break;
  case 6:
    puVar4 = (uint *)param_1->mbr_0xc;
    param_1->mbr_0xc = (dword)(puVar4 + 1);
    if ((uint *)param_1->mbr_0x4 < puVar4 + 1) {
      puVar4 = (uint *)cls_0x588410::meth_0x5884a0(param_1);
    }
    _cStack0000000c = *puVar4;
    dVar7 = meth_0x57e2e0(this,(char)*(undefined3 *)puVar4);
    break;
  case 7:
    puVar4 = (uint *)param_1->mbr_0xc;
    param_1->mbr_0xc = (dword)(puVar4 + 1);
    if ((uint *)param_1->mbr_0x4 < puVar4 + 1) {
      puVar4 = (uint *)cls_0x588410::meth_0x5884a0(param_1);
    }
    _cStack0000000c = *puVar4;
    dVar7 = meth_0x57e2e0(this,(char)*(undefined3 *)puVar4);
    meth_0x57e5a0(this);
    break;
  case 8:
  case 0xb:
    dVar7 = meth_0x57e990(this,uVar9);
    break;
  case 9:
    puVar4 = (uint *)cls_0x588410::meth_0x5725f0(param_1);
    _cStack0000000c = *puVar4;
    dVar7 = meth_0x57e2e0(this,(char)*(undefined3 *)puVar4);
    break;
  case 10:
    puVar4 = (uint *)cls_0x588410::meth_0x5725f0(param_1);
    _cStack0000000c = *puVar4;
    dVar7 = meth_0x57e2e0(this,(char)*(undefined3 *)puVar4);
    meth_0x57cc90(this,dVar7,uVar9);
    meth_0x57e5a0(this);
    break;
  case 0xc:
    pbVar5 = (byte *)param_1->mbr_0xc;
    param_1->mbr_0xc = (dword)(pbVar5 + 1);
    if ((byte *)param_1->mbr_0x4 < pbVar5 + 1) {
      pbVar5 = (byte *)cls_0x588410::meth_0x5884a0(param_1);
    }
    dVar7 = cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,(uint)*pbVar5);
    if (dVar7 == 0) {
      cls_0x57d9d0::meth_0x583cb0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
    }
    if ((char)param_2 == '\0') {
      return dVar7;
    }
    (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = dVar7;
    *(dword *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = dVar7;
    *(dword *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = dVar7;
    break;
  case 0xd:
    pbVar5 = (byte *)cls_0x588410::meth_0x5725f0(param_1);
    local_c = (uint)*pbVar5;
    dVar7 = cls_0x45f7c0::meth_0x51eea0((cls_0x45f7c0 *)&DAT_0065a890,local_c);
    if (dVar7 == 0) {
      cls_0x57d9d0::meth_0x583cb0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
    }
    if ((char)param_2 != '\0') {
      (this->cls_0x5ba0a0).cls_0x588410.mbr_0x28 = dVar7;
      *(dword *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c = dVar7;
      *(dword *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x30 = dVar7;
    }
    meth_0x57cc90(this,dVar7,uVar9);
    pwVar6 = (word *)cls_0x588410::meth_0x5725f0(param_1);
    wVar2 = *pwVar6;
    if (dVar7 != 0) {
      cls_0x56fe30::cls_0x56fe30(local_18);
      local_14 = (int *)(dVar7 + 0x2a0);
      local_10 = 0xb;
      do {
        iVar1 = *local_14;
        if (iVar1 == 0) {
          puVar8 = (undefined4 *)&stack0x0000000c;
          uVar9 = 1;
          _cStack0000000c = CONCAT31(uStack0000000d,0xfe);
        }
        else {
          local_8 = *(undefined4 *)(*(int *)(iVar1 + 0x4c) + 0x1c);
          cls_0x56fe30::meth_0x56fe90(local_18,&local_8,4);
          local_4 = CONCAT22(extraout_var,*(undefined2 *)(iVar1 + 0xc));
          puVar8 = &local_4;
          uVar9 = 2;
        }
        cls_0x56fe30::meth_0x56fe90(local_18,puVar8,uVar9);
        local_14 = local_14 + 1;
        local_10 = local_10 + -1;
      } while (local_10 != 0);
      if (wVar2 != local_18[0].mbr_0x0) {
        FUN_00583d30(local_c);
      }
    }
    meth_0x57e5a0(this);
    break;
  case 0xe:
    dVar7 = meth_0x57ebb0(this,uVar9);
  }
  if (((char)param_2 != '\0') && (*(int *)&(this->cls_0x5ba0a0).cls_0x588410.field_0x2c != 0)) {
    FUN_004d6190();
  }
  return dVar7;
}



// Function at 00587e00

cls_0x5ba0b4 * __thiscall OOAnalyzer::cls_0x5ba0b4::virt_meth_0x587e00(cls_0x5ba0b4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ba0b4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



