// Decompiled methods and structure for class: TScreen

/*
/OOAnalyzer/TScreen
pack(disabled)
Structure TScreen {
   0   TScreen::vftable_5a50e8 *   4   vftptr_0x0   "pointer to TScreen::vftable_5a50e8"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   word   2   mbr_0xc   "Unsigned Word (dw, 2-bytes)"
   14   word   2   mbr_0xe   "Unsigned Word (dw, 2-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   byte   1   mbr_0x34   "Unsigned Byte (db)"
   53   byte   1   mbr_0x35   "Unsigned Byte (db)"
   54   byte   1   mbr_0x36   "Unsigned Byte (db)"
   55   byte   1   mbr_0x37   "Unsigned Byte (db)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   word   2   mbr_0x5c   "Unsigned Word (dw, 2-bytes)"
   94   word   2   mbr_0x5e   "Unsigned Word (dw, 2-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   138   word   2   mbr_0x8a   "Unsigned Word (dw, 2-bytes)"
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
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 276 Alignment: 1

*/

// Function at 0046e0f0

void __thiscall OOAnalyzer::TScreen::meth_0x46e0f0(TScreen *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  
  this->mbr_0x58 = 0;
  this->mbr_0x9c = 0xffffffff;
  this->mbr_0xa0 = 0xffffffff;
  this->mbr_0x8a = 0xffff;
  (*this->vftptr_0x0->FUN_00471b50_344)(0);
  *(undefined2 *)((int)&this->mbr_0x4 + 2) = 0xffff;
  *(undefined2 *)&this->mbr_0x4 = 0xffff;
  this->mbr_0x84 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x3c = 0;
  this->mbr_0x44 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x37 = 0;
  this->mbr_0x5c = 0;
  this->mbr_0x5e = 1;
  this->mbr_0xe = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x10 = 0;
  this->mbr_0x24 = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x30 = 0;
  this->mbr_0x2c = 0;
  this->mbr_0x28 = 0;
  this->mbr_0x36 = 0;
  this->mbr_0x35 = 0;
  this->mbr_0x34 = 0;
  *(undefined2 *)&this->field_0x7c = 0xffff;
  *(undefined2 *)&this->field_0x7e = 0xffff;
  this->mbr_0x50 = 0xffffffff;
  this->mbr_0x64 = 0;
  this->mbr_0xb4 = 0;
  this->mbr_0xb8 = 0;
  this->mbr_0xb0 = 0;
  this->mbr_0xbc = 0;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  dVar1 = this->mbr_0x10;
  dVar2 = this->mbr_0x14;
  dVar3 = this->mbr_0x18;
  this->mbr_0xac = 0;
  *(undefined2 *)&this->mbr_0xa8 = 0;
  *(undefined2 *)((int)&this->mbr_0xa8 + 2) = 0;
  this->mbr_0xc0 = dVar1 + 1;
  this->mbr_0xc4 = dVar2 + 1;
  this->mbr_0xc8 = dVar3 + 1;
  this->mbr_0xd4 = 0;
  this->mbr_0xd0 = 0;
  this->mbr_0xcc = 0;
  return;
}



// Function at 0046e1f0

TScreen * __thiscall OOAnalyzer::TScreen::TScreen(TScreen *this)

{
  dword *this_00;
  int iVar1;
  short sVar2;
  TScreen__vftable_5a50e8 *pcVar3;
  dword dVar4;
  dword *pdVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  void *unaff_EBX;
  dword *pdVar9;
  bool bVar10;
  dword *in_stack_00000004;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d269;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x68,0);
  this_00 = &this->mbr_0xa8;
  *(undefined2 *)this_00 = 0;
  *(undefined2 *)((int)&this->mbr_0xa8 + 2) = 0;
  this->mbr_0xac = 0;
  local_4 = 1;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  meth_0x46e0f0(this);
  pcVar3 = this->vftptr_0x0;
  pdVar5 = in_stack_00000004;
  pdVar9 = &this->mbr_0x4;
  for (iVar8 = 0xd; iVar8 != 0; iVar8 = iVar8 + -1) {
    *pdVar9 = *pdVar5;
    pdVar5 = pdVar5 + 1;
    pdVar9 = pdVar9 + 1;
  }
  DAT_006669fc = DAT_006669fc | 8;
  (*pcVar3->FUN_00472e90_64)(this->mbr_0x8 | 0x40000);
  bVar10 = (uint)(int)*(short *)&this->mbr_0x4 < DAT_0065a258_TObjectClass_numclasses;
  this->mbr_0x54 = (dword)in_stack_00000004;
  if (bVar10) {
    dVar4 = (&DAT_0065a148_TObjectClass_classes)[*(short *)&this->mbr_0x4];
  }
  else {
    dVar4 = 0;
  }
  this->mbr_0x48 = dVar4;
  if (dVar4 == 0) {
    FUN_00481c10((byte *)s_Bad_object_class__005d47fc);
  }
  dVar4 = this->mbr_0x48;
  sVar2 = *(short *)((int)&this->mbr_0x4 + 2);
  pdVar5 = (dword *)cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)(dVar4 + 0x24));
  if ((pdVar5 != (dword *)0x0) &&
     (pdVar5 = *(dword **)(*(int *)(dVar4 + 0x34) + sVar2 * 4), pdVar5 == (dword *)0x0)) {
    pdVar5 = *(dword **)(dVar4 + 0x38);
  }
  dVar4 = *pdVar5;
  this->mbr_0x4c = (dword)pdVar5;
  this->mbr_0x38 = dVar4;
  sVar2 = *(short *)(this->mbr_0x48 + 0x1c);
  if (0 < sVar2) {
    cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)this_00);
    dVar4 = this->mbr_0x48;
    *(short *)this_00 = sVar2;
    iVar8 = 0;
    if (0 < *(short *)(dVar4 + 0x1c)) {
      do {
        iVar1 = iVar8 * 4;
        iVar6 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)(dVar4 + 0x24));
        iVar8 = iVar8 + 1;
        dVar4 = this->mbr_0x48;
        *(undefined4 *)(iVar1 + this->mbr_0xac) = *(undefined4 *)(*(int *)(iVar6 + 0x18) + iVar1);
      } while (iVar8 < *(short *)(dVar4 + 0x1c));
    }
  }
  sVar2 = *(short *)&this->mbr_0x4;
  switch(sVar2) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 6:
  case 7:
  case 8:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x1a:
    uVar7 = this->mbr_0x8 | 0x4000c000;
    break;
  default:
    goto switchD_0046e343_caseD_5;
  case 0x19:
    uVar7 = this->mbr_0x8 | 0xc000;
  }
  this->mbr_0x8 = uVar7;
switchD_0046e343_caseD_5:
  if ((sVar2 == 0xc) || (sVar2 == 0xb)) {
    (*this->vftptr_0x0->FUN_00472e90_64)(this->mbr_0x8 | 0x4000000);
  }
  iVar8 = (**(code **)(*(int *)this->mbr_0x54 + 0x38))(this);
  if (iVar8 != 0) {
    this->mbr_0x8 = this->mbr_0x8 | 0xc000;
  }
  if (*(short *)&this->mbr_0x4 != 9) {
    this->mbr_0x8 = this->mbr_0x8 | 0x8000;
  }
  if (((this->mbr_0x8 & 0x40000000) == 0) &&
     (((*(short *)&this->mbr_0x4 != 9 || (this->mbr_0x38 != *(dword *)this->mbr_0x4c)) &&
      ((this->mbr_0x8 & 0x8000000) == 0)))) {
    cls_0x497370::meth_0x497370_TScriptManager_ObjectScript((cls_0x497370 *)&DAT_0065def0);
    meth_0x471150(this);
  }
  ExceptionList = unaff_EBX;
  return this;
}



// Function at 0046e420

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  local_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  local_4 = local_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  local_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00471150

void __thiscall OOAnalyzer::TScreen::meth_0x471150(TScreen *this)

{
  cls_0x4922c0 *this_00;
  uint uVar1;
  cls_0x4922c0 *in_stack_00000004;
  
  this_00 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_00 != in_stack_00000004) {
    if ((((this->mbr_0x8 & 0x40000000) == 0) &&
        ((*(short *)&this->mbr_0x4 != 9 || (this->mbr_0x38 != *(dword *)this->mbr_0x4c)))) &&
       ((DAT_0066829c == 0 ||
        (((-1 < DAT_0065a784 && (uVar1 = *(uint *)(DAT_0065a77c + DAT_0065a784 * 4), uVar1 != 0)) &&
         ((*(byte *)(uVar1 & (DAT_0065a784 < 0) - 1) & 0x10) != 0)))))) {
      if (this_00 != (cls_0x4922c0 *)0x0) {
        this_00->mbr_0xc = 0;
        cls_0x4922c0::meth_0x4922c0(this_00);
        FUN_004830f0(this_00);
        this->mbr_0x84 = 0;
      }
      this->mbr_0x84 = (dword)in_stack_00000004;
      in_stack_00000004->mbr_0xc = (dword)this;
      if (in_stack_00000004 != (cls_0x4922c0 *)0x0) {
        if ((this->mbr_0x8 & 0x8000) == 0) {
          (*this->vftptr_0x0->FUN_00472e90_64)(this->mbr_0x8 | 0x8000);
        }
        DAT_006669fc = DAT_006669fc | 5;
        (*this->vftptr_0x0->FUN_00472e90_64)(this->mbr_0x8 | 0x40000);
      }
      if ((cls_0x4922c0 *)this->mbr_0x84 != (cls_0x4922c0 *)0x0) {
        cls_0x4922c0::meth_0x4924f0((cls_0x4922c0 *)this->mbr_0x84);
        return;
      }
    }
    else if (in_stack_00000004 != (cls_0x4922c0 *)0x0) {
      in_stack_00000004->mbr_0xc = 0;
    }
  }
  return;
}



// Function at 00477840

TScreen * __thiscall OOAnalyzer::TScreen::virt_meth_0x477840(TScreen *this)

{
  byte in_stack_00000004;
  
  ~TScreen(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c12e0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004ddd60

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004de080

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004de140

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f4ee0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f4fd0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f51c0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f53b0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f5c20

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f6130

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f6300

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f6be0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f6fc0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f7110

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f7b90

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f7c80

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f7d40

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f7ea0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f80a0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f81e0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f8370

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f8560

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f87b0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f89a0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f8bd0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f8de0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9030

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9230

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9540

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9730

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9a30

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9c20

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004f9e10

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 004fa060

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500040

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005001e0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500370

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500500

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500680

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500820

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005009b0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500b40

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500ca0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500d30

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500dc0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500e50

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500ee0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00500f70

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00501000

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00501670

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509010

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005091e0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509380

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509570

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509a20

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509bb0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509d40

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509e10

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00509ea0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050a040

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050a210

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050a3b0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050a4d0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050df70

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050e050

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050e420

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050e500

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050eda0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050ee30

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050f8b0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050fa50

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050fbf0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 0050fe10

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005101a0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00515300

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00520ba0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00522030

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005238a0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005238d0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00523bb0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00523ff0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005241d0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00526c90

void __thiscall OOAnalyzer::TScreen::meth_0x526c90(TScreen *this)

{
  uint uVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  
  uVar1 = FUN_0059a530_stricmp((uint)this,(byte *)this->mbr_0x38,(byte *)s_RunFireHead1S_005e3074);
  if (uVar1 != 0) {
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this->mbr_0x38,(byte *)s_RunFireHead2S_005e3084);
    if (uVar1 != 0) {
      uVar1 = FUN_0059a530_stricmp(extraout_ECX_00,(byte *)this->mbr_0x38,(byte *)s_RunFireHead1E_005e3094);
      if (uVar1 != 0) {
        uVar1 = FUN_0059a530_stricmp(extraout_ECX_01,(byte *)this->mbr_0x38,(byte *)s_RunFireHead2E_005e30a4
                            );
        if (uVar1 != 0) {
          uVar1 = FUN_0059a530_stricmp(extraout_ECX_02,(byte *)this->mbr_0x38,
                               (byte *)s_RunFireHeadN_005e30b4);
          if (uVar1 == 0) {
            this->mbr_0x110 = 0;
          }
          else {
            uVar1 = FUN_0059a530_stricmp(extraout_ECX_03,(byte *)this->mbr_0x38,
                                 (byte *)s_RunFireHeadW_005e30c4);
            if (uVar1 == 0) {
              this->mbr_0x110 = 0xc0;
            }
          }
          goto LAB_00526d42;
        }
      }
      this->mbr_0x110 = 0x40;
      goto LAB_00526d42;
    }
  }
  this->mbr_0x110 = 0x80;
LAB_00526d42:
  this->mbr_0xf8 = 0;
  this->mbr_0x104 = 0;
  this->mbr_0xd8 = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0x10c = 0;
  this->mbr_0x108 = 0;
  this->mbr_0x8 = this->mbr_0x8 | 0x40000;
  return;
}



// Function at 00527570

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527680

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527770

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527860

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527940

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527a20

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527b40

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527c50

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527e00

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527f10

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00527ff0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005280c0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00528190

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00528260

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00528330

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00528400

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005285f0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 00529140

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



// Function at 005421f0

TScreen * __thiscall OOAnalyzer::TScreen::~TScreen(TScreen *this)

{
  TPlayScreen *this_00;
  cls_0x5b4f30_TPlayer *this_01;
  LPCVOID pvVar1;
  cls_0x4922c0 *this_02;
  TScreen *pcVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  void *pvStack_c;
  undefined *puStack_8;
  uint uStack_4;
  
  puStack_8 = &LAB_0059d28c;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &TScreen__vftable_5a50e8_005a50e8;
  uStack_4 = 1;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  if (DAT_0065d674 == this) {
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  if ((DAT_0065b088 == this) && (DAT_0065b088 != (TScreen *)0x0)) {
    DAT_0065b088 = (TScreen *)0x0;
    (**(code **)(DAT_0065b028 + 0x28))();
  }
  if (DAT_0065b2d8 == this) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  FUN_00446ba0((cls_0x5a486c *)this->mbr_0x54);
  this->mbr_0x54 = 0;
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
  }
  if (((((this->mbr_0x8 & 0x40000000) != 0) &&
       (this_00 = (TPlayScreen *)this->mbr_0x64, this_00 != (TPlayScreen *)0x0)) &&
      (*(short *)&(this_00->TScreen).mbr_0x4 == 0xb)) &&
     ((0xff < *(short *)&this->field_0x7c && (*(short *)&this->field_0x7c < 0x10b)))) {
    TPlayScreen::meth_0x5199b0(this_00,0);
  }
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x68) {
    do {
      this_01 = *(cls_0x5b4f30_TPlayer **)(*(int *)&this->field_0x78 + iVar4 * 4);
      if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
        (this_01->TPlayScreen).TScreen.mbr_0x64 = 0;
        cls_0x5b4f30_TPlayer::meth_0x46e630(this_01);
        (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)this_01);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x68);
  }
  if (*(undefined4 **)&this->field_0x78 != (undefined4 *)0x0) {
    puVar5 = *(undefined4 **)&this->field_0x78;
    for (uVar3 = *(uint *)&this->field_0x70 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pvVar1 = (LPCVOID)this->mbr_0x38;
  *(undefined4 *)&this->field_0x6c = 0;
  *(undefined4 *)&this->field_0x68 = 0;
  if ((pvVar1 != (LPCVOID)0x0) && (pvVar1 != *(LPCVOID *)this->mbr_0x4c)) {
    FUN_00482f80(pvVar1);
    this->mbr_0x38 = 0;
  }
  if ((TScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) == this) {
    TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,0);
  }
  this_02 = (cls_0x4922c0 *)this->mbr_0x84;
  if (this_02 != (cls_0x4922c0 *)0x0) {
    this_02->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_02);
    FUN_004830f0(this_02);
    this->mbr_0x84 = 0;
  }
  uStack_4 = uStack_4 & 0xffffff00;
  if ((LPCVOID)this->mbr_0xac != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xac);
  }
  uStack_4 = 0xffffffff;
  pcVar2 = (TScreen *)FUN_004830f0(*(LPCVOID *)&this->field_0x78);
  ExceptionList = pvStack_c;
  return pcVar2;
}



