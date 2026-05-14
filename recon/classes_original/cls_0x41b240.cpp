// Decompiled methods and structure for class: cls_0x41b240

/*
/OOAnalyzer/cls_0x41b240
pack(disabled)
Structure cls_0x41b240 {
   0   byte   1   mbr_0x0   "Unsigned Byte (db)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
   216   byte   1   mbr_0xd8   "Unsigned Byte (db)"
   217   byte   1   mbr_0xd9   "Unsigned Byte (db)"
   218   byte   1   mbr_0xda   "Unsigned Byte (db)"
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
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   444   dword   4   mbr_0x1bc   "Unsigned Double-Word (ddw, 4-bytes)"
   576   dword   4   mbr_0x240   "Unsigned Double-Word (ddw, 4-bytes)"
   580   dword   4   mbr_0x244   "Unsigned Double-Word (ddw, 4-bytes)"
   584   dword   4   mbr_0x248   "Unsigned Double-Word (ddw, 4-bytes)"
   592   dword   4   mbr_0x250   "Unsigned Double-Word (ddw, 4-bytes)"
   596   dword   4   mbr_0x254   "Unsigned Double-Word (ddw, 4-bytes)"
   600   dword   4   mbr_0x258   "Unsigned Double-Word (ddw, 4-bytes)"
   604   dword   4   mbr_0x25c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 608 Alignment: 1

*/

// Function at 0041b240

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41b240_TArea_PlayCDMusic(cls_0x41b240 *this)

{
  int iVar1;
  DWORD DVar2;
  int iVar3;
  dword dVar4;
  
  iVar1 = DAT_0065abf4;
  iVar3 = DAT_0065abf0;
  if (DAT_0065abec == 0) {
    DVar2 = GetTickCount();
    if ((int)this->mbr_0x250 <= (int)(DVar2 - this->mbr_0x248)) {
      if ((DAT_0065abd8 == 0) &&
         (iVar3 = cls_0x49a300::meth_0x49a590((cls_0x49a300 *)&DAT_0065abc8), iVar3 == 0)) {
        if (this->mbr_0x240 == 0) {
          dVar4 = this->mbr_0x1b8 + 1;
          this->mbr_0x1b8 = dVar4;
          if ((int)this->mbr_0x1bc <= (int)dVar4) {
            this->mbr_0x1b8 = 0;
          }
        }
        else {
          do {
            dVar4 = FUN_00483300_RandomRange(0,this->mbr_0x1bc - 1);
            if (dVar4 != this->mbr_0x1b8) break;
          } while (1 < (int)this->mbr_0x1bc);
          this->mbr_0x1b8 = dVar4;
        }
        dVar4 = cls_0x49a300::meth_0x49a480((cls_0x49a300 *)&DAT_0065abc8);
        this->mbr_0x254 = dVar4;
        dVar4 = dVar4 + this->mbr_0x244 * 1000;
        this->mbr_0x250 = dVar4;
        if ((int)dVar4 < 0x3e9) {
          this->mbr_0x250 = 30000;
        }
        dVar4 = this->mbr_0x250;
        if (10000 < (int)dVar4) {
          dVar4 = 10000;
        }
        this->mbr_0x250 = dVar4;
        cls_0x49a300::meth_0x49a300((cls_0x49a300 *)&DAT_0065abc8);
      }
      DVar2 = GetTickCount();
      this->mbr_0x248 = DVar2;
    }
  }
  else {
    DVar2 = GetTickCount();
    if (iVar1 <= (int)(DVar2 - iVar3)) {
      cls_0x49a300::meth_0x49a300((cls_0x49a300 *)&DAT_0065abc8);
      DAT_0065abf4 = cls_0x49a300::meth_0x49a480((cls_0x49a300 *)&DAT_0065abc8);
      DAT_0065abec = 1;
      DAT_0065abf0 = GetTickCount();
      return;
    }
  }
  return;
}



// Function at 0041b3c0

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41b3c0_TArea_GetCurrentAmbient(cls_0x41b240 *this,dword *param_1)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  dword *in_stack_00000008;
  
  if ((*(byte *)&this->mbr_0x44 & 8) != 0) {
    iVar2 = cls_0x47ea00::meth_0x47ea00((cls_0x47ea00 *)&DAT_0065caf0);
    if (iVar2 == 0) {
      dVar1 = this->mbr_0xdc;
      *param_1 = this->mbr_0xd4;
      *in_stack_00000008 = dVar1;
      return;
    }
    if (iVar2 != 0xff) {
      iVar6 = 0xff - iVar2;
      *param_1 = (int)(this->mbr_0xd0 * iVar2) / 0xff + (int)(this->mbr_0xd4 * iVar6) / 0xff;
      iVar3 = (uint)*(byte *)((int)&this->mbr_0xdc + 2) * iVar6;
      iVar4 = (uint)this->mbr_0xda * iVar2;
      iVar5 = (uint)*(byte *)((int)&this->mbr_0xdc + 1) * iVar6;
      *(char *)((int)in_stack_00000008 + 2) =
           (((char)(iVar3 / 0xff) + (char)(iVar3 >> 0x1f)) -
           (char)((longlong)iVar3 * 0x80808081 >> 0x3f)) +
           (((char)(iVar4 / 0xff) + (char)(iVar4 >> 0x1f)) -
           (char)((longlong)iVar4 * 0x80808081 >> 0x3f));
      iVar3 = (uint)this->mbr_0xd9 * iVar2;
      iVar6 = (uint)*(byte *)&this->mbr_0xdc * iVar6;
      *(char *)((int)in_stack_00000008 + 1) =
           (((char)(iVar5 / 0xff) + (char)(iVar5 >> 0x1f)) -
           (char)((longlong)iVar5 * 0x80808081 >> 0x3f)) +
           (((char)(iVar3 / 0xff) + (char)(iVar3 >> 0x1f)) -
           (char)((longlong)iVar3 * 0x80808081 >> 0x3f));
      iVar2 = (uint)this->mbr_0xd8 * iVar2;
      *(char *)in_stack_00000008 =
           (((char)(iVar6 / 0xff) + (char)(iVar6 >> 0x1f)) -
           (char)((longlong)iVar6 * 0x80808081 >> 0x3f)) +
           (((char)(iVar2 / 0xff) + (char)(iVar2 >> 0x1f)) -
           (char)((longlong)iVar2 * 0x80808081 >> 0x3f));
      return;
    }
  }
  dVar1 = *(dword *)&this->mbr_0xd8;
  *param_1 = this->mbr_0xd0;
  *in_stack_00000008 = dVar1;
  return;
}



// Function at 0041b550

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41b550(cls_0x41b240 *this)

{
  uint uVar1;
  
  if ((this->mbr_0x44 & 0x100) != 0) {
    if ((this->mbr_0x44 & 0x800) != 0) {
      cls_0x5a5320_TPlayScreen::meth_0x450d20((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
    }
    uVar1 = this->mbr_0x44;
    if ((uVar1 & 0x400) != 0) {
      DAT_006671f4._0_4_ = (undefined4)(this->mbr_0xe4 - this->mbr_0xf0);
      DAT_006671f4._4_4_ = (undefined4)(this->mbr_0xe8 - this->mbr_0xf4);
      DAT_006671f4._8_4_ = (undefined4)(this->mbr_0xec - this->mbr_0xf8);
      DAT_006671f4._36_4_ = 0xc7c35000;
    }
    if ((uVar1 & 0x1000) != 0) {
      DAT_006671f4._0_4_ = (undefined4)this->mbr_0xe4;
      DAT_006671f4._36_4_ = 0xc7c35000;
      DAT_006671f4._4_4_ = (undefined4)this->mbr_0xe8;
      DAT_006671f4._8_4_ = (undefined4)this->mbr_0xec;
    }
    if ((uVar1 & 0x2000) != 0) {
      cls_0x5a5320_TPlayScreen::meth_0x45a990((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,this->mbr_0xfc,this->mbr_0x100);
    }
    if ((this->mbr_0x44 & 0x200) != 0) {
      cls_0x5a5320_TPlayScreen::meth_0x45aa80((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
    }
  }
  return;
}



// Function at 0041b690

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41b690(cls_0x41b240 *this)

{
  uint uVar1;
  
  uVar1 = this->mbr_0x44;
  if ((uVar1 & 0x100) != 0) {
    if (((uVar1 & 0x8000) == 0) || (DAT_006671f0 == 1)) {
      if (((uVar1 & 0x10000) == 0) || (DAT_006671f0 == 2)) {
        if (((uVar1 & 0x20000) == 0) || (DAT_006671f0 == 3)) {
          if (((uVar1 & 0x40000) != 0) && (DAT_006671f0 != 4)) {
            (**(code **)(DAT_006668d8 + 0x2c))(1);
            DAT_006671f0 = 4;
          }
        }
        else {
          (**(code **)(DAT_006668d8 + 0x2c))(1);
          DAT_006671f0 = 3;
        }
      }
      else {
        (**(code **)(DAT_006668d8 + 0x2c))(1);
        DAT_006671f0 = 2;
      }
    }
    else {
      (**(code **)(DAT_006668d8 + 0x2c))(1);
      DAT_006671f0 = 1;
    }
  }
  if (((this->mbr_0x44 & 0x100) == 0) && (DAT_006671f0 != 0)) {
    (**(code **)(DAT_006668d8 + 0x2c))(1);
    DAT_006671f0 = 0;
  }
  return;
}



// Function at 0041b770

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41b770_TArea_Pulse(cls_0x41b240 *this)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  dword dVar4;
  int *piVar5;
  undefined4 *puVar6;
  undefined4 local_54;
  undefined local_50 [4];
  int local_4c [4];
  undefined4 local_3c;
  undefined4 local_38;
  undefined2 local_34;
  undefined2 local_32;
  undefined2 local_2a;
  int local_28;
  int local_24;
  int local_20;
  
  if (((*(byte *)&this->mbr_0x44 & 0x20) != 0) && (0 < (int)this->mbr_0x1bc)) {
    meth_0x41b240_TArea_PlayCDMusic(this);
  }
  iVar1 = cls_0x47ea00::meth_0x47eab0((cls_0x47ea00 *)&DAT_0065caf0);
  if ((*(byte *)&this->mbr_0x44 & 8) != 0) {
    if (((iVar1 != DAT_005e91e8) && (iVar1 != 2)) && (iVar1 != 5)) {
      local_4c[0] = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      local_4c[1] = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      local_4c[2] = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      local_4c[3] = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      local_3c = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      local_38 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      iVar2 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if (iVar2 < 0) {
        iVar2 = local_4c[iVar1];
        uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        FUN_0054d170(&DAT_0065c5d0,s__s__s_005c6d04,uVar3,iVar2);
      }
      else {
        iVar2 = local_4c[iVar1];
        uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        FUN_0054d170(&DAT_0065c5d0,uVar3,iVar2);
      }
    }
    dVar4 = cls_0x47ea00::meth_0x47ea00((cls_0x47ea00 *)&DAT_0065caf0);
    if (dVar4 != this->mbr_0xe0) {
      meth_0x41b3c0_TArea_GetCurrentAmbient(this,&local_54);
      cls_0x5a5320_TPlayScreen::meth_0x453640((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,local_54);
      cls_0x5a5320_TPlayScreen::meth_0x4536b0((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,local_50);
      this->mbr_0xe0 = dVar4;
    }
  }
  dVar4 = this->mbr_0x25c;
  if (dVar4 != 0) {
    if ((*(int *)(dVar4 + 0x1c) == 0) && (*(int *)(dVar4 + 0x20) != 0)) {
      puVar6 = (undefined4 *)&local_34;
      for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      local_34 = 0x19;
      local_32 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,dVar4);
      local_28 = DAT_00666988;
      local_2a = DAT_00666970;
      local_24 = DAT_0066698c;
      local_20 = DAT_00666990;
      uVar3 = cls_0x5a5320_TPlayScreen::meth_0x450e40((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,&local_34);
      piVar5 = (int *)FUN_00452690_TMapPane_GetInstance(uVar3,0);
      *(int **)(this->mbr_0x25c + 0x1c) = piVar5;
      if (piVar5 != (int *)0x0) {
        (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x80000);
      }
    }
    if (*(int *)(this->mbr_0x25c + 0x1c) != 0) {
      local_4c[0] = DAT_00666988 + -500;
      local_4c[1] = DAT_0066698c + -500;
      local_4c[2] = DAT_00666990 + -600;
      (**(code **)(**(int **)(this->mbr_0x25c + 0x1c) + 8))(local_4c,0xffffffff,0);
    }
  }
  DAT_005e91e8 = iVar1;
  return;
}



// Function at 0041ba00

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x41b240::meth_0x41ba00_TArea_Enter(cls_0x41b240 *this)

{
  byte bVar1;
  byte *pbVar2;
  undefined uVar3;
  DWORD DVar4;
  int iVar5;
  cls_0x41b240 *pcVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  bool bVar12;
  undefined uVar13;
  cls_0x41b240 *pcVar14;
  cls_0x41b240 *pcVar15;
  undefined local_214 [4];
  undefined4 local_210 [2];
  int local_208;
  byte abStack_200 [512];
  
  iVar10 = DAT_0066697c;
  iVar9 = _DAT_00666970;
  local_208 = DAT_00666980;
  uVar11 = this->mbr_0x44;
  if ((uVar11 & 2) != 0) {
    return;
  }
  this->mbr_0x44 = uVar11 | 2;
  if (this->mbr_0x25c != 0) {
    *(undefined4 *)(this->mbr_0x25c + 0x20) = 1;
  }
  if ((uVar11 & 0x80) != 0) {
    cls_0x41c7d0::meth_0x49bef0((cls_0x41c7d0 *)&DAT_00667548);
  }
  if ((*(byte *)&this->mbr_0x44 & 0x20) != 0) {
    this->mbr_0x1b8 = 0xffffffff;
    DVar4 = GetTickCount();
    this->mbr_0x248 = DVar4;
    bVar12 = DAT_0065abd8 == 0;
    this->mbr_0x250 = 100;
    this->mbr_0x254 = 0;
    if (bVar12) {
      DAT_0065abd8 = 8;
    }
    else {
      DAT_0065abdc = 8;
    }
  }
  if ((this->mbr_0x44 & 0x40) != 0) {
    this->mbr_0x44 = this->mbr_0x44 | 0x40;
    iVar5 = cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar5 == 0) {
      this->mbr_0x44 = this->mbr_0x44 & 0xffffffbf;
    }
    else {
      uVar7 = 0;
      uVar13 = 0x7f;
      uVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,uVar3,uVar13,uVar7);
    }
  }
  if ((*(byte *)&this->mbr_0x44 & 4) != 0) {
    meth_0x41b3c0_TArea_GetCurrentAmbient(this,local_210);
    if (iVar9 == DAT_005e91ec) {
      iVar9 = iVar10 - DAT_005e91f0;
      if (iVar9 < 0) {
        iVar9 = DAT_005e91f0 - iVar10;
      }
      iVar10 = local_208 - DAT_005e91f4;
      if (iVar10 < 0) {
        iVar10 = DAT_005e91f4 - local_208;
      }
      iVar5 = iVar9;
      if (iVar10 <= iVar9) {
        iVar5 = iVar10;
      }
      if ((iVar10 - (iVar5 >> 1)) + iVar9 < 0x401) {
        cls_0x5a5320_TPlayScreen::meth_0x453720((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,local_210[0],local_214,0x48);
        goto LAB_0041bb8f;
      }
    }
    cls_0x5a5320_TPlayScreen::meth_0x453640((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,local_210[0]);
    cls_0x5a5320_TPlayScreen::meth_0x4536b0((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,local_214);
  }
LAB_0041bb8f:
  if ((DAT_0066829c == 0) && ((*(byte *)&this->mbr_0x44 & 0x10) != 0)) {
    iVar9 = FUN_0059a530_stricmp(&this->mbr_0x50,s_master_s_005c6d0c);
    if (iVar9 == 0) {
      FUN_00481c10(s_The_MASTER_S_script_file_can_not_005c6d18,0);
    }
    cls_0x495cf0::meth_0x496490_TScriptManager_Load((cls_0x495cf0 *)&DAT_0065def0);
  }
  meth_0x41b690(this);
  if ((this->mbr_0x44 & 0x3400) != 0) {
    meth_0x41b550(this);
  }
  if ((DAT_00667fcc == 0) || (this->mbr_0x0 == 0)) {
    return;
  }
  uVar11 = 0xffffffff;
  iVar10 = 0;
  iVar9 = 0;
  pcVar6 = this;
  do {
    if (uVar11 == 0) break;
    uVar11 = uVar11 - 1;
    pbVar2 = &pcVar6->mbr_0x0;
    pcVar6 = (cls_0x41b240 *)&pcVar6->field_0x1;
  } while (*pbVar2 != 0);
  if (0 < (int)(~uVar11 - 1)) {
    do {
      bVar1 = (&this->mbr_0x0)[iVar9];
      if (((('`' < (char)bVar1) && ((char)bVar1 < '{')) ||
          (('@' < (char)bVar1 && ((char)bVar1 < '[')))) ||
         (('/' < (char)bVar1 && ((char)bVar1 < ':')))) {
        abStack_200[iVar10] = bVar1;
        iVar10 = iVar10 + 1;
      }
      uVar11 = 0xffffffff;
      iVar9 = iVar9 + 1;
      pcVar6 = this;
      do {
        if (uVar11 == 0) break;
        uVar11 = uVar11 - 1;
        pbVar2 = &pcVar6->mbr_0x0;
        pcVar6 = (cls_0x41b240 *)&pcVar6->field_0x1;
      } while (*pbVar2 != 0);
    } while (iVar9 < (int)(~uVar11 - 1));
  }
  abStack_200[iVar10] = 0;
  pcVar6 = (cls_0x41b240 *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  if ((pcVar6 == (cls_0x41b240 *)0x0) || (pcVar6->mbr_0x0 == 0x5b)) {
    pcVar6 = this;
  }
  iVar9 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  if (iVar9 < 0) {
    iVar9 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (iVar9 < 0) {
      uVar7 = *(undefined4 *)(DAT_00667fcc + 0x38);
      uVar8 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      FUN_0054d170(&DAT_0065c5d0,s__s__s__s_005c6dac,uVar7,uVar8,pcVar6);
      return;
    }
    pcVar15 = *(cls_0x41b240 **)(DAT_00667fcc + 0x38);
    pcVar14 = pcVar6;
  }
  else {
    pcVar14 = *(cls_0x41b240 **)(DAT_00667fcc + 0x38);
    pcVar15 = pcVar6;
  }
  uVar7 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  FUN_0054d170(&DAT_0065c5d0,uVar7,pcVar14,pcVar15);
  return;
}



