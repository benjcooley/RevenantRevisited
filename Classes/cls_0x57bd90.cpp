// Decompiled methods and structure for class: cls_0x57bd90

/*
/OOAnalyzer/cls_0x57bd90
pack(disabled)
Structure cls_0x57bd90 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0057bd90

void __thiscall OOAnalyzer::cls_0x57bd90::meth_0x57bd90(cls_0x57bd90 *this)

{
  char cVar1;
  dword dVar2;
  char in_stack_00000004;
  
  if ((in_stack_00000004 != '\0') && (dVar2 = this->mbr_0x4, dVar2 != 0)) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->mbr_0x4 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  return;
}



// Function at 0057bec0

cls_0x57bd90 * __thiscall
OOAnalyzer::cls_0x57bd90::meth_0x57bec0(cls_0x57bd90 *this,undefined4 *param_1)

{
  char cVar1;
  dword dVar2;
  undefined4 *puVar3;
  uint uVar4;
  undefined4 *puVar5;
  uint in_stack_00000008;
  
  if (0xfffffffd < in_stack_00000008) {
    FUN_0059b6f0();
  }
  dVar2 = this->mbr_0x4;
  if (((dVar2 == 0) || (cVar1 = *(char *)(dVar2 - 1), cVar1 == '\0')) || (cVar1 == -1)) {
    if (in_stack_00000008 == 0) {
      meth_0x57bd90(this);
      return this;
    }
    if ((this->mbr_0xc < 0x20) && (in_stack_00000008 <= this->mbr_0xc)) goto LAB_0057bf31;
    meth_0x57bd90(this);
  }
  else if (in_stack_00000008 == 0) {
    this->mbr_0x4 = 0;
    *(char *)(dVar2 - 1) = cVar1 + -1;
    this->mbr_0x8 = 0;
    this->mbr_0xc = 0;
    return this;
  }
  meth_0x57c0a0(this);
LAB_0057bf31:
  puVar3 = (undefined4 *)this->mbr_0x4;
  this->mbr_0x8 = in_stack_00000008;
  puVar5 = puVar3;
  for (uVar4 = in_stack_00000008 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar5 = *param_1;
    param_1 = param_1 + 1;
    puVar5 = puVar5 + 1;
  }
  for (uVar4 = in_stack_00000008 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar5 = *(undefined *)param_1;
    param_1 = (undefined4 *)((int)param_1 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  }
  *(undefined *)((int)puVar3 + in_stack_00000008) = 0;
  return this;
}



// Function at 0057bf60

void __thiscall OOAnalyzer::cls_0x57bd90::meth_0x57bf60(cls_0x57bd90 *this)

{
  dword in_stack_00000004;
  
  this->mbr_0x8 = in_stack_00000004;
  *(undefined *)(this->mbr_0x4 + in_stack_00000004) = 0;
  return;
}



// Function at 0057bf80

undefined4 __thiscall OOAnalyzer::cls_0x57bd90::meth_0x57bf80(cls_0x57bd90 *this,undefined param_1)

{
  char cVar1;
  undefined *puVar2;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  
  if (0xfffffffd < _param_1) {
    FUN_0059b6f0();
  }
  puVar2 = (undefined *)this->mbr_0x4;
  if (((puVar2 == (undefined *)0x0) || (cVar1 = puVar2[-1], cVar1 == '\0')) || (cVar1 == -1)) {
    if (_param_1 == 0) {
      if (in_stack_00000008 == '\0') {
        if (puVar2 != (undefined *)0x0) {
          this->mbr_0x8 = 0;
          *puVar2 = 0;
        }
        return 0;
      }
      if (puVar2 != (undefined *)0x0) {
        cVar1 = puVar2[-1];
        if ((cVar1 != '\0') && (cVar1 != -1)) {
          this->mbr_0x4 = 0;
          this->mbr_0x8 = 0;
          this->mbr_0xc = 0;
          puVar2[-1] = cVar1 + -1;
          return 0;
        }
        FUN_004830f0(puVar2 + -1);
      }
      this->mbr_0x4 = 0;
      this->mbr_0x8 = 0;
      this->mbr_0xc = 0;
      return 0;
    }
    if (in_stack_00000008 != '\0') {
      if ((this->mbr_0xc < 0x20) && (_param_1 <= this->mbr_0xc)) {
        return 1;
      }
      if (puVar2 != (undefined *)0x0) {
        cVar1 = puVar2[-1];
        if ((cVar1 != '\0') && (cVar1 != -1)) {
          puVar2[-1] = cVar1 + -1;
          this->mbr_0x4 = 0;
          this->mbr_0x8 = 0;
          this->mbr_0xc = 0;
          meth_0x57c0a0(this);
          return 1;
        }
        FUN_004830f0(puVar2 + -1);
      }
      this->mbr_0x4 = 0;
      this->mbr_0x8 = 0;
      this->mbr_0xc = 0;
      meth_0x57c0a0(this);
      return 1;
    }
    if (_param_1 <= this->mbr_0xc) {
      return 1;
    }
  }
  else if (_param_1 == 0) {
    this->mbr_0x4 = 0;
    this->mbr_0x8 = 0;
    this->mbr_0xc = 0;
    puVar2[-1] = cVar1 + -1;
    return 0;
  }
  meth_0x57c0a0(this);
  return 1;
}



// Function at 0057c0a0

void __thiscall OOAnalyzer::cls_0x57bd90::meth_0x57c0a0(cls_0x57bd90 *this)

{
  int iVar1;
  uint uVar2;
  uint in_stack_00000004;
  void *pvStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_005a2840;
  pvStack_10 = ExceptionList;
  uVar2 = in_stack_00000004 | 0x1f;
  if (0xfffffffd < (in_stack_00000004 | 0x1f)) {
    uVar2 = in_stack_00000004;
  }
  iVar1 = uVar2 + 2;
  local_8 = 0;
  if (iVar1 < 0) {
    iVar1 = 0;
  }
  ExceptionList = &pvStack_10;
  FUN_00482fb0(iVar1);
  FUN_0057c11a();
  return;
}



// Function at 0059ba04

cls_0x57bd90 * __thiscall
OOAnalyzer::cls_0x57bd90::meth_0x59ba04(cls_0x57bd90 *this,cls_0x57bd90 *param_1,uint param_2)

{
  char cVar1;
  Alignment *pAVar2;
  uint uVar3;
  uint in_stack_0000000c;
  
  if (param_1->mbr_0x8 < param_2) {
    FUN_0059b91c();
  }
  uVar3 = param_1->mbr_0x8 - param_2;
  if (in_stack_0000000c < uVar3) {
    uVar3 = in_stack_0000000c;
  }
  if (this == param_1) {
    meth_0x59bad2(this,uVar3 + param_2);
    meth_0x59bad2(this,0);
  }
  else {
    if ((uVar3 != 0) && (uVar3 == param_1->mbr_0x8)) {
      pAVar2 = (Alignment *)param_1->mbr_0x4;
      if (pAVar2 == (Alignment *)0x0) {
        pAVar2 = &DAT_005b9fc8;
      }
      if (*(byte *)((int)pAVar2 + -1) < 0xfe) {
        meth_0x57bd90(this);
        pAVar2 = (Alignment *)param_1->mbr_0x4;
        if (pAVar2 == (Alignment *)0x0) {
          pAVar2 = &DAT_005b9fc8;
        }
        this->mbr_0x4 = (dword)pAVar2;
        this->mbr_0x8 = param_1->mbr_0x8;
        this->mbr_0xc = param_1->mbr_0xc;
        *(char *)((int)pAVar2 + -1) = *(char *)((int)pAVar2 + -1) + '\x01';
        return this;
      }
    }
    cVar1 = meth_0x57bf80(this,(char)uVar3);
    if (cVar1 != '\0') {
      pAVar2 = (Alignment *)param_1->mbr_0x4;
      if (pAVar2 == (Alignment *)0x0) {
        pAVar2 = &DAT_005b9fc8;
      }
      FUN_00592220(this->mbr_0x4,(int)pAVar2 + param_2,uVar3);
      this->mbr_0x8 = uVar3;
      *(undefined *)(this->mbr_0x4 + uVar3) = 0;
    }
  }
  return this;
}



// Function at 0059bad2

cls_0x57bd90 * __thiscall OOAnalyzer::cls_0x57bd90::meth_0x59bad2(cls_0x57bd90 *this,uint param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  dword dVar4;
  uint in_stack_00000008;
  
  if (this->mbr_0x8 < param_1) {
    FUN_0059b91c();
  }
  meth_0x59bb39(this);
  uVar2 = this->mbr_0x8 - param_1;
  if (uVar2 < in_stack_00000008) {
    in_stack_00000008 = uVar2;
  }
  if (in_stack_00000008 != 0) {
    iVar3 = this->mbr_0x4 + param_1;
    FUN_0058b790(iVar3,iVar3 + in_stack_00000008,uVar2 - in_stack_00000008);
    dVar4 = this->mbr_0x8 - in_stack_00000008;
    cVar1 = meth_0x57bf80(this,(char)dVar4);
    if (cVar1 != '\0') {
      this->mbr_0x8 = dVar4;
      *(undefined *)(dVar4 + this->mbr_0x4) = 0;
    }
  }
  return this;
}



// Function at 0059bb39

void __thiscall OOAnalyzer::cls_0x57bd90::meth_0x59bb39(cls_0x57bd90 *this)

{
  char *_Str;
  
  _Str = (char *)this->mbr_0x4;
  if (((_Str != (char *)0x0) && (_Str[-1] != '\0')) && (_Str[-1] != -1)) {
    meth_0x57bd90(this);
    _strlen(_Str);
    meth_0x57bec0(this,_Str);
  }
  return;
}



