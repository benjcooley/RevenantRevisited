#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b4f30_TPlayer



// Function at 0046e630

void cls_0x5b4f30_TPlayer::meth_0x46e630()

{
  dword dVar1;
  cls_0x4922c0 *pcVar2;
  uint uVar3;
  
  dVar1 = (this->TPlayScreen).TScreen.mbr_0x44;
  if ((dVar1 == 0) || (DAT_006687a4 != dVar1)) {
    FUN_00499ff0_TSector_DispatchNotifyToAll(1,this);
  }
  else {
    (this->TPlayScreen).TScreen.mbr_0x44 = 0;
  }
  if ((this->TPlayScreen).TScreen.mbr_0x64 != 0) {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046faf0_96)();
  }
  if ((this->TPlayScreen).TScreen.mbr_0x44 != 0) {
    TCharacter::meth_0x451610((TCharacter *)&DAT_006668d8);
  }
  pcVar2 = (cls_0x4922c0 *)(this->TPlayScreen).TScreen.mbr_0x84;
  if (pcVar2 != (cls_0x4922c0 *)0x0) {
    cls_0x4922c0::meth_0x492490(pcVar2);
    pcVar2 = (cls_0x4922c0 *)(this->TPlayScreen).TScreen.mbr_0x84;
    pcVar2->mbr_0xc = 0;
    if (pcVar2 != (cls_0x4922c0 *)0x0) {
      cls_0x4922c0::meth_0x4922c0(pcVar2);
      FUN_004830f0(pcVar2);
    }
    uVar3 = (this->TPlayScreen).TScreen.mbr_0x8;
    (this->TPlayScreen).TScreen.mbr_0x84 = 0;
    (this->TPlayScreen).TScreen.mbr_0x8 = uVar3 | 0x80000000;
    return;
  }
  (this->TPlayScreen).TScreen.mbr_0x8 = (this->TPlayScreen).TScreen.mbr_0x8 | 0x80000000
  ;
  return;
}



// Function at 0046e6f0

void cls_0x5b4f30_TPlayer::meth_0x46e6f0()

{
  char cVar1;
  LPCVOID pvVar2;
  cls_0x4922c0 *this_00;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined4 *puVar8;
  char *in_stack_00000004;
  
  pvVar2 = (LPCVOID)(this->TPlayScreen).TScreen.mbr_0x38;
  if ((pvVar2 != (LPCVOID)0x0) && (pvVar2 != *(LPCVOID *)(this->TPlayScreen).TScreen.mbr_0x4c)
     ) {
    FUN_00482f80(pvVar2);
    (this->TPlayScreen).TScreen.mbr_0x38 = 0;
  }
  if ((in_stack_00000004 == (char *)0x0) || (*in_stack_00000004 == '\0')) {
    (this->TPlayScreen).TScreen.mbr_0x38 =
         *(dword *)(this->TPlayScreen).TScreen.mbr_0x4c;
  }
  else {
    uVar4 = 0xffffffff;
    pcVar7 = in_stack_00000004;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    puVar3 = FUN_00482ef0(~uVar4);
    uVar4 = 0xffffffff;
    do {
      pcVar7 = in_stack_00000004;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      pcVar7 = in_stack_00000004 + 1;
      cVar1 = *in_stack_00000004;
      in_stack_00000004 = pcVar7;
    } while (cVar1 != '\0');
    uVar4 = ~uVar4;
    puVar6 = (undefined4 *)(pcVar7 + -uVar4);
    puVar8 = puVar3;
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar8 = *puVar6;
      puVar6 = puVar6 + 1;
      puVar8 = puVar8 + 1;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar8 = *(undefined *)puVar6;
      puVar6 = (undefined4 *)((int)puVar6 + 1);
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    (this->TPlayScreen).TScreen.mbr_0x38 = (dword)puVar3;
  }
  this_00 = (cls_0x4922c0 *)(this->TPlayScreen).TScreen.mbr_0x84;
  if (this_00 != (cls_0x4922c0 *)0x0) {
    this_00->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_00);
    FUN_004830f0(this_00);
    (this->TPlayScreen).TScreen.mbr_0x84 = 0;
  }
  if ((((this->TPlayScreen).TScreen.mbr_0x8 & 0x40000000) == 0) &&
     ((*(short *)&(this->TPlayScreen).TScreen.mbr_0x4 != 9 ||
      ((this->TPlayScreen).TScreen.mbr_0x38 !=
       *(dword *)(this->TPlayScreen).TScreen.mbr_0x4c)))) {
    cls_0x497370::meth_0x497370_TScriptManager_ObjectScript((cls_0x497370 *)&DAT_0065def0);
    TScreen::meth_0x471150((TScreen *)this);
  }
  return;
}



// Function at 00470090

int cls_0x5b4f30_TPlayer::meth_0x470090()

{
  undefined uVar1;
  undefined3 extraout_var;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var_01;
  int iVar2;
  cls_0x477870 cStack_1c;
  undefined3 extraout_var_00;
  
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this);
  iVar2 = CONCAT31(extraout_var,uVar1);
  cStack_1c.mbr_0x4 = (dword)this;
  while (iVar2 != 0) {
    uVar1 = (*((((TPlayScreen *)cStack_1c.mbr_0x4)->TScreen).vftptr_0x0)->
              virt_meth_0x477d50_368)((void *)cStack_1c.mbr_0x4);
    this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var_00,uVar1);
    uVar1 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this_00);
    cStack_1c.mbr_0x4 = (dword)this_00;
    iVar2 = CONCAT31(extraout_var_01,uVar1);
  }
  iVar2 = 0;
  cStack_1c.mbr_0x0 = 3;
  cStack_1c.mbr_0x10 = 0;
  cStack_1c.mbr_0xc = 0;
  cStack_1c.mbr_0x14 = 0;
  cStack_1c.mbr_0x18 = 0;
  cStack_1c.mbr_0x8 = cStack_1c.mbr_0x4;
  cls_0x477870::meth_0x46dfb0(&cStack_1c);
  while (cStack_1c.mbr_0x18 != 0) {
    iVar2 = iVar2 + 1;
    cls_0x477870::meth_0x46dfb0(&cStack_1c);
  }
  return iVar2;
}



// Function at 00470160

int cls_0x5b4f30_TPlayer::meth_0x470160()

{
  undefined uVar1;
  undefined3 extraout_var;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var_01;
  int iVar2;
  int in_stack_00000004;
  cls_0x477870 cStack_1c;
  undefined3 extraout_var_00;
  
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this);
  iVar2 = CONCAT31(extraout_var,uVar1);
  cStack_1c.mbr_0x4 = (dword)this;
  while (iVar2 != 0) {
    uVar1 = (*((((TPlayScreen *)cStack_1c.mbr_0x4)->TScreen).vftptr_0x0)->
              virt_meth_0x477d50_368)((void *)cStack_1c.mbr_0x4);
    this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var_00,uVar1);
    uVar1 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this_00);
    cStack_1c.mbr_0x4 = (dword)this_00;
    iVar2 = CONCAT31(extraout_var_01,uVar1);
  }
  iVar2 = 0;
  cStack_1c.mbr_0x0 = 3;
  cStack_1c.mbr_0x10 = 0;
  cStack_1c.mbr_0xc = 0;
  cStack_1c.mbr_0x14 = 0;
  cStack_1c.mbr_0x18 = 0;
  cStack_1c.mbr_0x8 = cStack_1c.mbr_0x4;
  cls_0x477870::meth_0x46dfb0(&cStack_1c);
  while( true ) {
    if (cStack_1c.mbr_0x18 == 0) {
      return 0;
    }
    if (iVar2 == in_stack_00000004) break;
    iVar2 = iVar2 + 1;
    cls_0x477870::meth_0x46dfb0(&cStack_1c);
  }
  return cStack_1c.mbr_0x18;
}



// Function at 004701f0

int cls_0x5b4f30_TPlayer::meth_0x4701f0()

{
  undefined uVar1;
  undefined3 extraout_var;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var_01;
  int in_stack_00000004;
  cls_0x477870 cStack_1c;
  int iVar2;
  undefined3 extraout_var_00;
  
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this);
  iVar2 = CONCAT31(extraout_var,uVar1);
  cStack_1c.mbr_0x4 = (dword)this;
  while (iVar2 != 0) {
    uVar1 = (*((((TPlayScreen *)cStack_1c.mbr_0x4)->TScreen).vftptr_0x0)->
              virt_meth_0x477d50_368)((void *)cStack_1c.mbr_0x4);
    this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var_00,uVar1);
    uVar1 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d50_368)(this_00);
    cStack_1c.mbr_0x4 = (dword)this_00;
    iVar2 = CONCAT31(extraout_var_01,uVar1);
  }
  cStack_1c.mbr_0x0 = 0;
  cStack_1c.mbr_0x10 = 0;
  cStack_1c.mbr_0xc = 0;
  cStack_1c.mbr_0x14 = 0;
  cStack_1c.mbr_0x18 = 0;
  cStack_1c.mbr_0x8 = cStack_1c.mbr_0x4;
  cls_0x477870::meth_0x46dfb0(&cStack_1c);
  while( true ) {
    if (cStack_1c.mbr_0x18 == 0) {
      return 0;
    }
    if (*(short *)(cStack_1c.mbr_0x18 + 0x7c) == in_stack_00000004) break;
    cls_0x477870::meth_0x46dfb0(&cStack_1c);
  }
  return cStack_1c.mbr_0x18;
}



// Function at 004713c0

void cls_0x5b4f30_TPlayer::meth_0x4713c0()

{
  uint uVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  int in_stack_00000004;
  
  uVar1 = (this->TPlayScreen).TScreen.mbr_0x8;
  pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
  if (in_stack_00000004 != 0) {
    (*pcVar2->FUN_00472e90_64)(uVar1 | 4);
    return;
  }
  (*pcVar2->FUN_00472e90_64)(uVar1 & 0xfffffffb);
  return;
}



// Function at 00471430

void cls_0x5b4f30_TPlayer::meth_0x471430()

{
  uint uVar1;
  int *in_stack_00000004;
  int local_18;
  int local_14;
  int local_c;
  int local_8;
  int local_4;
  
  local_4 = (this->TPlayScreen).TScreen.mbr_0x18 +
            *(int *)&(this->TPlayScreen).TScreen.field_0x98;
  local_8 = (this->TPlayScreen).TScreen.mbr_0x14 +
            *(int *)&(this->TPlayScreen).TScreen.field_0x94;
  local_c = (this->TPlayScreen).TScreen.mbr_0x10 +
            *(int *)&(this->TPlayScreen).TScreen.field_0x90;
  FUN_0046d830(0x5d4740,&local_c,&local_18);
  if ((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) == 0) {
    *in_stack_00000004 = local_18;
    in_stack_00000004[1] = local_14;
    in_stack_00000004[2] = local_18;
    in_stack_00000004[3] = local_14;
    return;
  }
  uVar1 = (uint)(byte)(this->TPlayScreen).TScreen.field_0x89;
  *in_stack_00000004 = local_18 - uVar1;
  in_stack_00000004[1] = local_14 - uVar1;
  in_stack_00000004[2] = (local_18 - uVar1) + uVar1 * 2;
  in_stack_00000004[3] = (local_14 - uVar1) + uVar1 * 2;
  return;
}



// Function at 004714e0

void cls_0x5b4f30_TPlayer::meth_0x4714e0()

{
  byte bVar1;
  dword dVar2;
  byte in_stack_00000004;
  undefined local_10 [16];
  
  if (((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
     ((this->TPlayScreen).TScreen.field_0x89 != in_stack_00000004)) {
    FUN_00456790(s_d__revenant_Object_cpp_005d48f4,0x90d);
    bVar1 = (this->TPlayScreen).TScreen.field_0x89;
    if ((((in_stack_00000004 <= bVar1) &&
         ((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0)) && (bVar1 != 0)) &&
       (DAT_005d7a18 == 0)) {
      meth_0x471430(this);
      TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,local_10);
    }
    dVar2 = (this->TPlayScreen).TScreen.mbr_0x9c;
    (this->TPlayScreen).TScreen.field_0x89 = in_stack_00000004;
    if (dVar2 != 0xffffffff) {
      cls_0x4159c0::meth_0x4159c0((cls_0x4159c0 *)((int)&DAT_0065a578 + 4),dVar2);
    }
    if (((in_stack_00000004 > bVar1) &&
        ((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0)) &&
       (((this->TPlayScreen).TScreen.field_0x89 != '\0' && (DAT_005d7a18 == 0)))) {
      meth_0x471430(this);
      TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,local_10);
    }
    FUN_004567c0();
    return;
  }
  (this->TPlayScreen).TScreen.field_0x89 = in_stack_00000004;
  return;
}



// Function at 004715e0

void cls_0x5b4f30_TPlayer::meth_0x4715e0()

{
  dword dVar1;
  uint uVar2;
  int in_stack_00000004;
  undefined local_10 [16];
  
  if (((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
     ((short)(this->TPlayScreen).TScreen.mbr_0x8a != in_stack_00000004)) {
    (this->TPlayScreen).TScreen.mbr_0x8a = (word)in_stack_00000004;
    FUN_00456790(s_d__revenant_Object_cpp_005d490c,0x929);
    dVar1 = (this->TPlayScreen).TScreen.mbr_0x9c;
    if (dVar1 != 0xffffffff) {
      cls_0x415a00::meth_0x415a00((cls_0x415a00 *)((int)&DAT_0065a578 + 4),dVar1);
    }
    dVar1 = (this->TPlayScreen).TScreen.mbr_0xa0;
    if (dVar1 != 0xffffffff) {
      FUN_0041d9e0(dVar1);
      uVar2 = FUN_0041d890((undefined4 *)&(this->TPlayScreen).TScreen.field_0x8c,
                           (int)(short)(this->TPlayScreen).TScreen.mbr_0x8a);
      (this->TPlayScreen).TScreen.mbr_0xa0 = uVar2;
    }
    if ((((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
        ((this->TPlayScreen).TScreen.field_0x89 != '\0')) && (DAT_005d7a18 == 0)) {
      meth_0x471430(this);
      TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,local_10);
    }
    FUN_004567c0();
    return;
  }
  (this->TPlayScreen).TScreen.mbr_0x8a = (word)in_stack_00000004;
  return;
}



// Function at 004716c0

void cls_0x5b4f30_TPlayer::meth_0x4716c0()

{
  int iVar1;
  int iVar2;
  dword dVar3;
  int *in_stack_00000004;
  int local_20;
  int local_1c;
  int local_18;
  undefined local_10 [16];
  
  if (((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
     (((*(int *)&(this->TPlayScreen).TScreen.field_0x90 != *in_stack_00000004 ||
       (*(int *)&(this->TPlayScreen).TScreen.field_0x94 != in_stack_00000004[1])) ||
      (*(int *)&(this->TPlayScreen).TScreen.field_0x98 != in_stack_00000004[2])))) {
    FUN_00456790(s_d__revenant_Object_cpp_005d4924,0x941);
    if ((((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
        ((this->TPlayScreen).TScreen.field_0x89 != '\0')) && (DAT_005d7a18 == 0)) {
      meth_0x471430(this);
      TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,&local_20);
    }
    iVar1 = in_stack_00000004[1];
    iVar2 = in_stack_00000004[2];
    dVar3 = (this->TPlayScreen).TScreen.mbr_0x9c;
    *(int *)&(this->TPlayScreen).TScreen.field_0x90 = *in_stack_00000004;
    *(int *)&(this->TPlayScreen).TScreen.field_0x94 = iVar1;
    *(int *)&(this->TPlayScreen).TScreen.field_0x98 = iVar2;
    if (dVar3 != 0xffffffff) {
      local_18 = (this->TPlayScreen).TScreen.mbr_0x18 +
                 *(int *)&(this->TPlayScreen).TScreen.field_0x98;
      local_1c = (this->TPlayScreen).TScreen.mbr_0x14 +
                 *(int *)&(this->TPlayScreen).TScreen.field_0x94;
      local_20 = (this->TPlayScreen).TScreen.mbr_0x10 +
                 *(int *)&(this->TPlayScreen).TScreen.field_0x90;
      cls_0x415ad0::meth_0x415ad0((cls_0x415ad0 *)((int)&DAT_0065a578 + 4),dVar3);
      if ((((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
          ((this->TPlayScreen).TScreen.field_0x89 != '\0')) && (DAT_005d7a18 == 0)) {
        meth_0x471430(this);
        TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,local_10);
      }
    }
    FUN_004567c0();
    return;
  }
  iVar1 = in_stack_00000004[1];
  iVar2 = in_stack_00000004[2];
  *(int *)&(this->TPlayScreen).TScreen.field_0x90 = *in_stack_00000004;
  *(int *)&(this->TPlayScreen).TScreen.field_0x94 = iVar1;
  *(int *)&(this->TPlayScreen).TScreen.field_0x98 = iVar2;
  return;
}



// Function at 00471820

void cls_0x5b4f30_TPlayer::meth_0x471820()

{
  dword dVar1;
  uint uVar2;
  int in_stack_00000004;
  undefined local_10 [16];
  
  if (((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
     (*(int *)&(this->TPlayScreen).TScreen.field_0x8c != in_stack_00000004)) {
    FUN_00456790(s_d__revenant_Object_cpp_005d493c,0x95a);
    dVar1 = (this->TPlayScreen).TScreen.mbr_0x9c;
    *(int *)&(this->TPlayScreen).TScreen.field_0x8c = in_stack_00000004;
    if (dVar1 != 0xffffffff) {
      cls_0x415a40::meth_0x415a40((cls_0x415a40 *)((int)&DAT_0065a578 + 4),dVar1);
    }
    dVar1 = (this->TPlayScreen).TScreen.mbr_0xa0;
    if (dVar1 != 0xffffffff) {
      FUN_0041d9e0(dVar1);
      uVar2 = FUN_0041d890(&stack0x00000004,(int)(short)(this->TPlayScreen).TScreen.mbr_0x8a);
      (this->TPlayScreen).TScreen.mbr_0xa0 = uVar2;
    }
    if ((((*(byte *)&(this->TPlayScreen).TScreen.mbr_0x8 & 4) != 0) &&
        ((this->TPlayScreen).TScreen.field_0x89 != '\0')) && (DAT_005d7a18 == 0)) {
      meth_0x471430(this);
      TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,local_10);
    }
    FUN_004567c0();
    return;
  }
  *(int *)&(this->TPlayScreen).TScreen.field_0x8c = in_stack_00000004;
  return;
}



// Function at 00472db0

void cls_0x5b4f30_TPlayer::meth_0x472db0(byte *param_1)

{
  TScreen__vftable_5a50e8 *pcVar1;
  uint uVar2;
  cls_0x5b4f30_TPlayer *pcVar3;
  cls_0x5b4f30_TPlayer *extraout_ECX;
  undefined **ppuVar4;
  int iVar5;
  int in_stack_00000008;
  
  iVar5 = 0;
  ppuVar4 = &PTR_s_IMMOBILE_005d4770;
  pcVar3 = this;
  do {
    uVar2 = FUN_0059a530_stricmp((uint)pcVar3,*ppuVar4,param_1);
    if (uVar2 == 0) {
      if (-1 < iVar5) {
        pcVar1 = (this->TPlayScreen).TScreen.vftptr_0x0;
        if (in_stack_00000008 != 0) {
          (*pcVar1->FUN_00472e90_64)
                    (1 << ((byte)iVar5 & 0x1f) | (this->TPlayScreen).TScreen.mbr_0x8);
          return;
        }
        (*pcVar1->FUN_00472e90_64)
                  (~(1 << ((byte)iVar5 & 0x1f)) & (this->TPlayScreen).TScreen.mbr_0x8);
      }
      return;
    }
    ppuVar4 = ppuVar4 + 1;
    iVar5 = iVar5 + 1;
    pcVar3 = extraout_ECX;
  } while (ppuVar4 < &PTR_s_talis1_005d47f0);
  return;
}



// Function at 00472e30

bool cls_0x5b4f30_TPlayer::meth_0x472e30()

{
  uint uVar1;
  cls_0x5b4f30_TPlayer *pcVar2;
  cls_0x5b4f30_TPlayer *extraout_ECX;
  undefined **ppuVar3;
  int iVar4;
  byte *in_stack_00000004;
  
  iVar4 = 0;
  ppuVar3 = &PTR_s_IMMOBILE_005d4770;
  pcVar2 = this;
  do {
    uVar1 = FUN_0059a530_stricmp((uint)pcVar2,*ppuVar3,in_stack_00000004);
    if (uVar1 == 0) {
      if (iVar4 < 0) {
        return false;
      }
      return (1 << ((byte)iVar4 & 0x1f) & (this->TPlayScreen).TScreen.mbr_0x8) != 0;
    }
    ppuVar3 = ppuVar3 + 1;
    iVar4 = iVar4 + 1;
    pcVar2 = extraout_ECX;
  } while (ppuVar3 < &PTR_s_talis1_005d47f0);
  return false;
}



// Function at 00477670

undefined4 cls_0x5b4f30_TPlayer::meth_0x477670()

{
  short sVar1;
  dword dVar2;
  dword *pdVar3;
  uint in_stack_00000004;
  
  if (((this->TPlayScreen).TScreen.mbr_0x44 == 0) &&
     ((this->TPlayScreen).TScreen.mbr_0x58 == 0)) {
    dVar2 = (this->TPlayScreen).TScreen.mbr_0x48;
    if (((int)in_stack_00000004 < *(int *)(dVar2 + 0x24)) &&
       ((((*(int *)(dVar2 + 0x34) != 0 && (in_stack_00000004 < *(uint *)(dVar2 + 0x24))) &&
         (*(int *)(*(int *)(dVar2 + 0x34) + in_stack_00000004 * 4) != 0)) &&
        ((pdVar3 = *(dword **)(*(int *)(dVar2 + 0x34) + in_stack_00000004 * 4),
         pdVar3 != (dword *)0x0 || (pdVar3 = *(dword **)(dVar2 + 0x38), pdVar3 != (dword *)0x0))))))
    {
      dVar2 = FUN_00446b10(pdVar3[2],1);
      if (dVar2 != 0) {
        FUN_00446ba0((cls_0x5a486c *)(this->TPlayScreen).TScreen.mbr_0x54);
        if ((this->TPlayScreen).TScreen.mbr_0x38 ==
            *(dword *)(this->TPlayScreen).TScreen.mbr_0x4c) {
          (this->TPlayScreen).TScreen.mbr_0x38 = *pdVar3;
        }
        sVar1 = *(short *)&(this->TPlayScreen).TScreen.mbr_0x4;
        (this->TPlayScreen).TScreen.mbr_0x4c = (dword)pdVar3;
        (this->TPlayScreen).TScreen.mbr_0x54 = dVar2;
        *(short *)((int)&(this->TPlayScreen).TScreen.mbr_0x4 + 2) = (short)in_stack_00000004;
        if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
          meth_0x4d6550(this);
        }
        return 1;
      }
    }
  }
  return 0;
}



// Function at 004ced20

int cls_0x5b4f30_TPlayer::meth_0x4ced20()

{
  int iVar1;
  undefined4 in_stack_00000004;
  
  iVar1 = TPlayScreen::meth_0x4ce350_Walk(&this->TPlayScreen);
  if (iVar1 != 0) {
    cls_0x57d9d0::meth_0x583e80
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x1c,in_stack_00000004,1);
  }
  return iVar1;
}



// Function at 004cf2e0

/* WARNING: Type propagation algorithm not settling */

undefined4 cls_0x5b4f30_TPlayer::meth_0x4cf2e0()

{
  dword dVar1;
  int *piVar2;
  undefined uVar3;
  int3 extraout_var;
  int iVar4;
  cls_0x4daae0 *pcVar5;
  undefined3 extraout_var_00;
  cls_0x4daae0 *this_00;
  char *pcVar6;
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e823;
  local_c = ExceptionList;
  if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
    dVar1 = (this->TPlayScreen).TScreen.mbr_0xd8;
    iVar4 = *(int *)(dVar1 + 0x48);
    if ((iVar4 != 0) && ((*(uint *)(iVar4 + 0x24) & 0x2000000) != 0)) {
      return 0;
    }
    iVar4 = *(int *)(dVar1 + 0x4c);
    if ((iVar4 != 0) && ((*(byte *)(iVar4 + 0x24) & 0x80) != 0)) {
      return 0;
    }
  }
  ExceptionList = &local_c;
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar3) == 0 || extraout_var < 0) {
    ExceptionList = local_c;
    return 0;
  }
  dVar1 = (this->TPlayScreen).TScreen.mbr_0xe0;
  if ((dVar1 != 0) && (iVar4 = FUN_004dab80_TActionBlock_Is(dVar1,s_sneak_005c618c), iVar4 != 0)) {
    ExceptionList = local_c;
    return 1;
  }
  iVar4 = cls_0x57d9d0::meth_0x57d9d0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)this,0x27,1);
  if (iVar4 != 0) {
    cls_0x57d9d0::meth_0x57dc70((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
  }
  pcVar5 = (cls_0x4daae0 *)FUN_00482fb0(100);
  uStack_4 = 0;
  if (pcVar5 == (cls_0x4daae0 *)0x0) {
    pcVar5 = (cls_0x4daae0 *)0x0;
  }
  else {
    pcVar5 = cls_0x4daae0::cls_0x4daae0
                       (pcVar5,(this->TPlayScreen).TScreen.mbr_0xe0,s_sneak_005e00fc);
  }
  uStack_4 = 0xffffffff;
  this_00 = pcVar5;
  (*(this->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x46ed70_8)(this);
  piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xd8;
  if ((piVar2 != (int *)0x0) && (*piVar2 == 2)) {
    uVar3 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477840_0)
                      ((TScreen *)this);
    pcVar6 = s_sneakr_005e010c;
    if (CONCAT31(extraout_var_00,uVar3) == 0) {
      pcVar6 = s_sneakf_005e0114;
    }
    this_00 = (cls_0x4daae0 *)FUN_00482fb0(100);
    if (this_00 == (cls_0x4daae0 *)0x0) {
      pcVar5 = (cls_0x4daae0 *)0x0;
    }
    else {
      pcVar5 = cls_0x4daae0::cls_0x4daae0(this_00,(this->TPlayScreen).TScreen.mbr_0xd8,pcVar6)
      ;
    }
  }
  pcVar5[0xc].mbr_0x0 = pcVar5[0xc].mbr_0x0 | 0x20;
  (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_0046f250_24)(pcVar5);
  ExceptionList = this_00;
  return 1;
}



// Function at 004d01e0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d01e0()

{
  short sVar1;
  undefined uVar2;
  int3 extraout_var;
  int iVar3;
  cls_0x5b4f30_TPlayer *pcVar4;
  undefined3 extraout_var_00;
  undefined4 uVar5;
  int *piVar6;
  cls_0x5b4f30_TPlayer *this_00;
  char *pcVar7;
  dword dStack_ac;
  dword dStack_a8;
  dword dStack_a4;
  int iStack_a0;
  int iStack_9c;
  int iStack_98;
  int iStack_94;
  int iStack_90;
  char acStack_8c [100];
  int aiStack_28 [10];
  
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar2) == 0 || extraout_var < 0) {
    return 0;
  }
  dStack_ac = (this->TPlayScreen).TScreen.mbr_0x10;
  dStack_a8 = (this->TPlayScreen).TScreen.mbr_0x14;
  dStack_a4 = (this->TPlayScreen).TScreen.mbr_0x18;
  FUN_0046db20_ConvertToVector((uint)(this->TPlayScreen).TScreen.mbr_0x36,0x3c,&iStack_98,0);
  dStack_a4 = dStack_a4 + iStack_90;
  dStack_ac = dStack_ac + iStack_98;
  dStack_a8 = dStack_a8 + iStack_94;
  iVar3 = TCharacter::meth_0x452060
                    ((TCharacter *)&DAT_006668d8,&dStack_ac,
                     (char)(this->TPlayScreen).TScreen.mbr_0xe,aiStack_28,0x3c,0,0xffffffff,10
                    );
  this_00 = (cls_0x5b4f30_TPlayer *)0x0;
  uVar5 = iStack_a0;
  if (iVar3 < 1) {
LAB_004d0309:
    iStack_a0 = uVar5;
    if (this == DAT_00667fcc) {
      uVar5 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar5);
    }
    return 0;
  }
  piVar6 = aiStack_28;
  iStack_a0 = iVar3;
  do {
    pcVar4 = (cls_0x5b4f30_TPlayer *)FUN_00452690_TMapPane_GetInstance(*piVar6,0);
    if (((pcVar4 != (cls_0x5b4f30_TPlayer *)0x0) &&
        ((*(byte *)&(pcVar4->TPlayScreen).TScreen.mbr_0x8 & 0x80) == 0)) &&
       (((sVar1 = *(short *)&(pcVar4->TPlayScreen).TScreen.mbr_0x4, sVar1 == 5 ||
         (sVar1 == 10)) ||
        ((((sVar1 == 0xb || (sVar1 == 0xc)) && (pcVar4 != this)) &&
         (iVar3 = TPlayScreen::meth_0x4c89c0_IsEnemy(&this->TPlayScreen), iVar3 == 0)))))) {
      uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046ea20_4)(pcVar4);
      if ((this_00 == (cls_0x5b4f30_TPlayer *)0x0) || (CONCAT31(extraout_var_00,uVar2) <= iStack_9c)) {
        this_00 = pcVar4;
        iStack_9c = CONCAT31(extraout_var_00,uVar2);
      }
    }
    piVar6 = piVar6 + 1;
    iStack_a0 = iStack_a0 + -1;
  } while (iStack_a0 != 0);
  uVar5 = 0;
  if (this_00 == (cls_0x5b4f30_TPlayer *)0x0) goto LAB_004d0309;
  if (this != DAT_00667fcc) goto LAB_004d0411;
  sVar1 = *(short *)&(this_00->TPlayScreen).TScreen.mbr_0x4;
  if ((sVar1 == 0xb) || (sVar1 == 0xc)) {
    FUN_0046e7f0(acStack_8c,100,*(char **)(this_00->TPlayScreen).TScreen.mbr_0x4c);
    iVar3 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (iVar3 < 0) {
      cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      pcVar7 = s__s__s_005e0290;
      goto LAB_004d0404;
    }
  }
  else {
    TPlayScreen::meth_0x46e7d0(&this_00->TPlayScreen,acStack_8c);
    iVar3 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (iVar3 < 0) {
      cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      pcVar7 = s__s__s_005e02b0;
LAB_004d0404:
      FUN_0054d170((TCharacter *)&DAT_0065c5d0,pcVar7);
      goto LAB_004d0411;
    }
  }
  uVar5 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar5);
LAB_004d0411:
  (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_004705f0_188)(this,0xffffffff);
  return 1;
}



// Function at 004d0430

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d0430()

{
  undefined uVar1;
  int3 extraout_var;
  int iVar2;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined4 uVar3;
  int *piVar4;
  cls_0x5b4f30_TPlayer *this_01;
  dword dStack_ac;
  dword dStack_a8;
  dword dStack_a4;
  int iStack_a0;
  int iStack_98;
  int iStack_94;
  int iStack_90;
  int aiStack_8c [10];
  undefined auStack_64 [100];
  
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar1) == 0 || extraout_var < 0) {
    return 0;
  }
  dStack_ac = (this->TPlayScreen).TScreen.mbr_0x10;
  dStack_a8 = (this->TPlayScreen).TScreen.mbr_0x14;
  dStack_a4 = (this->TPlayScreen).TScreen.mbr_0x18;
  FUN_0046db20_ConvertToVector((uint)(this->TPlayScreen).TScreen.mbr_0x36,0x28,&iStack_98,0);
  dStack_a4 = dStack_a4 + iStack_90;
  dStack_ac = dStack_ac + iStack_98;
  dStack_a8 = dStack_a8 + iStack_94;
  iVar2 = TCharacter::meth_0x452060
                    ((TCharacter *)&DAT_006668d8,&dStack_ac,
                     (char)(this->TPlayScreen).TScreen.mbr_0xe,aiStack_8c,0x50,0,0xffffffff,10
                    );
  this_01 = (cls_0x5b4f30_TPlayer *)0x0;
  uVar3 = iStack_a0;
  if (0 < iVar2) {
    piVar4 = aiStack_8c;
    iStack_a0 = iVar2;
    do {
      this_00 = (cls_0x5b4f30_TPlayer *)FUN_00452690_TMapPane_GetInstance(*piVar4,0);
      if ((this_00 != (cls_0x5b4f30_TPlayer *)0x0) && (this_00 != this)) {
        uVar1 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477a20_192)
                          (this_00);
        if (CONCAT31(extraout_var_00,uVar1) == -1) {
          uVar1 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477c00_308)
                            (this_00);
          if (CONCAT31(extraout_var_01,uVar1) == 0) goto LAB_004d0532;
        }
        if ((*(byte *)&(this_00->TPlayScreen).TScreen.mbr_0x8 & 0x80) == 0) {
          uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046ea20_4)(this_00);
          if ((this_01 == (cls_0x5b4f30_TPlayer *)0x0) || (CONCAT31(extraout_var_02,uVar1) <= iStack_a0)) {
            this_01 = this_00;
            iStack_a0 = CONCAT31(extraout_var_02,uVar1);
          }
        }
      }
LAB_004d0532:
      piVar4 = piVar4 + 1;
      iStack_a0 = iStack_a0 + -1;
    } while (iStack_a0 != 0);
    uVar3 = 0;
    if (this_01 != (cls_0x5b4f30_TPlayer *)0x0) {
      if (this == DAT_00667fcc) {
        TPlayScreen::meth_0x46e7d0(&this_01->TPlayScreen,auStack_64);
        iVar2 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        if (iVar2 < 0) {
          cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
          FUN_0054d170((TCharacter *)&DAT_0065c5d0,s__s__s_005e02dc);
        }
        else {
          uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
          FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar3);
        }
      }
      TPlayScreen::meth_0x4cfef0(&this->TPlayScreen);
      return 1;
    }
  }
  iStack_a0 = uVar3;
  if (this == DAT_00667fcc) {
    uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar3);
  }
  return 0;
}



// Function at 004d0aa0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d0aa0()

{
  int *piVar1;
  dword dVar2;
  TScreen__vftable_5a50e8 *pcVar3;
  undefined uVar4;
  undefined uVar5;
  byte *pbVar6;
  uint uVar7;
  int3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar8;
  cls_0x4da9f0 *this_00;
  cls_0x4da9f0 *pcVar9;
  uint extraout_ECX;
  byte *in_stack_ffffffe4;
  void *pvVar10;
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e8ed;
  local_c = ExceptionList;
  piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if ((piVar1 != (int *)0x0) && (*piVar1 == 0x19)) {
    in_stack_ffffffe4 = (byte *)((this->TPlayScreen).TScreen.mbr_0xd8 + 4);
    ExceptionList = &local_c;
    pbVar6 = (byte *)FUN_004dadd0_StName(piVar1 + 1,&DAT_005e02fc);
    uVar7 = FUN_0059a530_stricmp(extraout_ECX,pbVar6,in_stack_ffffffe4);
    if (uVar7 != 0) {
      in_stack_ffffffe4 = (byte *)0x4d0b17;
      uVar4 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
      if (CONCAT31(extraout_var,uVar4) != 0 && -1 < extraout_var) {
        if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
          dVar2 = (this->TPlayScreen).TScreen.mbr_0xd8;
          iVar8 = *(int *)(dVar2 + 0x48);
          if ((iVar8 != 0) && ((*(uint *)(iVar8 + 0x24) & 0x2000000) != 0)) {
            ExceptionList = in_stack_ffffffe4;
            return 0;
          }
          iVar8 = *(int *)(dVar2 + 0x4c);
          if ((iVar8 != 0) && ((*(byte *)(iVar8 + 0x24) & 0x80) != 0)) {
            ExceptionList = in_stack_ffffffe4;
            return 0;
          }
        }
        if (*(short *)&(this->TPlayScreen).TScreen.mbr_0x4 == 0xb) {
          pvVar10 = (void *)0x1;
          uVar4 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x4703c0_164)(this);
          if ((this->TPlayScreen).mbr_0x2c0 == 0) {
            uVar5 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x4703c0_164)(this);
            TPlayScreen::meth_0x5199b0(&this->TPlayScreen,CONCAT31(extraout_var_01,uVar5));
          }
          if (CONCAT31(extraout_var_00,uVar4) == 0) {
            ExceptionList = pvVar10;
            return 0;
          }
        }
        iVar8 = cls_0x57d9d0::meth_0x57d9d0
                          ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)this,0x23,1);
        if (iVar8 != 0) {
          cls_0x57d9d0::meth_0x57dc70((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
        }
        piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xd8;
        if ((piVar1 != (int *)0x0) &&
           (((iVar8 = *piVar1, iVar8 == 2 || (iVar8 == 4)) || (iVar8 == 0x1a)))) {
          TPlayScreen::meth_0x4cee70_Stop(&this->TPlayScreen);
        }
        this_00 = (cls_0x4da9f0 *)FUN_00482fb0(100);
        if (this_00 == (cls_0x4da9f0 *)0x0) {
          pcVar9 = (cls_0x4da9f0 *)0x0;
        }
        else {
          pcVar9 = cls_0x4da9f0::cls_0x4da9f0(this_00,s_bowaim_005e02e8);
        }
        pcVar3 = (this->TPlayScreen).TScreen.vftptr_0x0;
        pcVar9->mbr_0x60 = pcVar9->mbr_0x60 | 0x20;
        uVar7 = (uint)(this->TPlayScreen).TScreen.mbr_0x36;
        pcVar9->mbr_0x30 = uVar7;
        pcVar9->mbr_0x2c = uVar7;
        pcVar9->mbr_0x24 = 0x14;
        (*pcVar3[1].FUN_0046f250_24)(pcVar9);
        ExceptionList = this_00;
        return 1;
      }
    }
  }
  ExceptionList = in_stack_ffffffe4;
  return 0;
}



// Function at 004d0c70

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d0c70()

{
  byte bVar1;
  int *piVar2;
  dword dVar3;
  undefined uVar4;
  byte *pbVar5;
  uint uVar6;
  int3 extraout_var;
  int iVar7;
  byte *pbVar8;
  uint extraout_ECX;
  uint in_stack_00000004;
  
  piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if ((piVar2 != (int *)0x0) && (*piVar2 == 0x19)) {
    pbVar8 = (byte *)((this->TPlayScreen).TScreen.mbr_0xd8 + 4);
    pbVar5 = (byte *)FUN_004dadd0_StName(piVar2 + 1,&DAT_005e02fc);
    uVar6 = FUN_0059a530_stricmp(extraout_ECX,pbVar5,pbVar8);
    if ((uVar6 == 0) &&
       (uVar4 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this),
       CONCAT31(extraout_var,uVar4) != 0 && -1 < extraout_var)) {
      if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
        dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8;
        iVar7 = *(int *)(dVar3 + 0x48);
        if ((iVar7 != 0) && ((*(uint *)(iVar7 + 0x24) & 0x2000000) != 0)) {
          return 0;
        }
        iVar7 = *(int *)(dVar3 + 0x4c);
        if ((iVar7 != 0) && ((*(byte *)(iVar7 + 0x24) & 0x80) != 0)) {
          return 0;
        }
      }
      if ((*(uint *)((this->TPlayScreen).TScreen.mbr_0xd8 + 0x60) & 0x100) == 0) {
        cls_0x57d9d0::meth_0x583e80
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x24,in_stack_00000004,1);
        dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8;
        bVar1 = (this->TPlayScreen).TScreen.mbr_0x36;
        *(uint *)(dVar3 + 0x30) = in_stack_00000004;
        *(uint *)(dVar3 + 0x2c) = in_stack_00000004;
        iVar7 = FUN_0046ded0((uint)bVar1,in_stack_00000004);
        if (iVar7 < 0) {
          iVar7 = FUN_0046ded0((uint)(this->TPlayScreen).TScreen.mbr_0x36,in_stack_00000004);
          iVar7 = -iVar7;
        }
        else {
          iVar7 = FUN_0046ded0((uint)(this->TPlayScreen).TScreen.mbr_0x36,in_stack_00000004);
        }
        uVar6 = iVar7 - 0x20U & ((int)(iVar7 - 0x20U) < 0) - 1;
        *(int *)((this->TPlayScreen).TScreen.mbr_0xd8 + 0x34) =
             ((int)(uVar6 + ((int)uVar6 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
        return 1;
      }
      return 1;
    }
  }
  return 0;
}



// Function at 004d0dc0

void cls_0x5b4f30_TPlayer::meth_0x4d0dc0()

{
  meth_0x4d0c70(this);
  return;
}



// Function at 004d0de0

void cls_0x5b4f30_TPlayer::meth_0x4d0de0()

{
  meth_0x4d0c70(this);
  return;
}



// Function at 004d0fd0

int cls_0x5b4f30_TPlayer::meth_0x4d0fd0()

{
  int iVar1;
  undefined4 in_stack_00000004;
  
  iVar1 = TPlayScreen::meth_0x4d0e00(&this->TPlayScreen);
  if (iVar1 != 0) {
    cls_0x57d9d0::meth_0x583e80
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x25,in_stack_00000004,1);
  }
  return iVar1;
}



// Function at 004d1000

int cls_0x5b4f30_TPlayer::meth_0x4d1000()

{
  int iVar1;
  int in_stack_00000004;
  
  if (in_stack_00000004 == 0) {
    return 0;
  }
  TPlayScreen::meth_0x46ea90_AngleTo(&this->TPlayScreen);
  iVar1 = TPlayScreen::meth_0x4d0e00(&this->TPlayScreen);
  if (iVar1 != 0) {
    cls_0x57d9d0::meth_0x583f60
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x3e,
               *(undefined4 *)(in_stack_00000004 + 0x40),1);
  }
  return iVar1;
}



// Function at 004d1050

bool cls_0x5b4f30_TPlayer::meth_0x4d1050()

{
  int *piVar1;
  byte *pbVar2;
  uint uVar3;
  byte *pbVar4;
  uint extraout_ECX;
  
  piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if ((piVar1 != (int *)0x0) && (*piVar1 == 0x19)) {
    pbVar4 = (byte *)((this->TPlayScreen).TScreen.mbr_0xd8 + 4);
    pbVar2 = (byte *)FUN_004dadd0_StName(piVar1 + 1,&DAT_005e02fc);
    uVar3 = FUN_0059a530_stricmp(extraout_ECX,pbVar2,pbVar4);
    return uVar3 == 0;
  }
  return false;
}



// Function at 004d1dd0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d1dd0(undefined param_1, undefined param_2, undefined param_3, undefined param_4, undefined param_5, undefined4 param_6, undefined param_7, int param_8)

{
  int iVar1;
  int iVar2;
  undefined3 in_stack_0000000d;
  int in_stack_00000024;
  int local_4;
  
  if (in_stack_00000024 != 0) {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046ea20_4)(in_stack_00000024);
  }
  local_4 = 0;
  do {
    iVar2 = 0;
    if (0 < *(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 0xcc)) {
      do {
        iVar1 = TPlayScreen::meth_0x4d1120_IsValidAttack(&this->TPlayScreen,iVar2);
        if (iVar1 != 0) {
          *_param_3 = iVar2;
          return 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < *(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 0xcc));
    }
    local_4 = local_4 + 1;
  } while (local_4 < 3);
  return 0;
}



// Function at 004d2480

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d2480_ButtonAttack()

{
  int *piVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  undefined uVar3;
  int3 extraout_var;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  dword dVar8;
  int unaff_EBX;
  TPlayScreen *this_00;
  undefined unaff_retaddr;
  dword in_stack_00000004;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 auStack_c [2];
  uint uStack_4;
  
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var) {
    if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
      dVar8 = (this->TPlayScreen).TScreen.mbr_0xd8;
      iVar4 = *(int *)(dVar8 + 0x48);
      if ((iVar4 != 0) && ((*(uint *)(iVar4 + 0x24) & 0x2000000) != 0)) {
        return 0;
      }
      iVar4 = *(int *)(dVar8 + 0x4c);
      if ((iVar4 != 0) && ((*(byte *)(iVar4 + 0x24) & 0x80) != 0)) {
        return 0;
      }
    }
    piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
    iStack_20 = 0;
    if ((piVar1 == (int *)0x0) ||
       (((iVar4 = *piVar1, iVar4 != 3 && ((piVar1 == (int *)0x0 || (iVar4 != 0x19)))) ||
        (this_00 = (TPlayScreen *)piVar1[0x11], this_00 == (TPlayScreen *)0x0)))) {
      iVar4 = TPlayScreen::meth_0x4cd690_FindCharacters
                        (&this->TPlayScreen,&uStack_4,1,0xff,
                         (this->TPlayScreen).TScreen.mbr_0x36,0x20);
      this_00 = (TPlayScreen *)((iVar4 < 1) - 1 & uStack_4);
    }
    else if ((iVar4 != 3) && (iVar4 != 0x19)) {
      this_00 = (TPlayScreen *)0x0;
    }
    dVar8 = (this->TPlayScreen).mbr_0x160;
    if ((((dVar8 != 0) && ((*(uint *)(dVar8 + 0x24) & 0x4000) != 0)) &&
        (iVar4 = TCharacter::meth_0x47e920((TCharacter *)&DAT_0065caf0),
        (int)(iVar4 - (this->TPlayScreen).mbr_0x164) <=
        *(int *)((this->TPlayScreen).mbr_0x160 + 0xc0))) &&
       (dVar8 = (this->TPlayScreen).mbr_0x16c, (int)dVar8 < 3)) {
      (this->TPlayScreen).mbr_0x16c = dVar8 + 1;
      return 1;
    }
    if (((*(short *)&(this->TPlayScreen).TScreen.mbr_0x4 == 0xb) &&
        (this_00 != (TPlayScreen *)0x0)) &&
       ((*(short *)&(this_00->TScreen).mbr_0x4 != 0xb &&
        ((DAT_0066829c == 0 || (DAT_0067682c != 0)))))) {
      if (in_stack_00000004 == (this->TPlayScreen).mbr_0x28c) {
        dVar8 = (this->TPlayScreen).mbr_0x290 + 1;
        (this->TPlayScreen).mbr_0x290 = dVar8;
        if (2 < (int)dVar8) {
          iStack_20 = 1;
        }
      }
      else {
        (this->TPlayScreen).mbr_0x290 = 1;
      }
      (this->TPlayScreen).mbr_0x28c = in_stack_00000004;
      if ((iStack_20 != 0) && (iVar4 = FUN_00483300_RandomRange(0,10), iVar4 == 0)) {
        FUN_00483300_RandomRange(1,0x32);
        pcVar2 = (this_00->TScreen).vftptr_0x0;
        iStack_24 = -1;
        this_00->mbr_0x120 = 0;
        (*pcVar2->virt_meth_0x477ed0_480)(this_00);
        (*pcVar2->virt_meth_0x477e80_460)(this_00);
        iStack_1c = 0;
        do {
          iVar4 = TPlayScreen::meth_0x4d1ff0_FindButtonAttack(this_00);
          if ((iVar4 != 0) || (iVar4 = TPlayScreen::meth_0x4d1eb0_FindPcntAttack(this_00), iVar4 != 0)) {
            iVar5 = *(int *)&(this_00->TScreen).field_0xfc;
            iVar7 = *(int *)(*(int *)(iVar5 + 0xdc) + unaff_EBX * 4);
            if (iVar7 == 0) {
              iVar7 = *(int *)(iVar5 + 0xe0);
            }
            if ((*(uint *)(iVar7 + 0x24) & 0x1800000) == 0) break;
            iVar4 = 0;
          }
          iStack_1c = iStack_1c + 1;
        } while (iStack_1c < 10);
        if ((iVar4 != 0) && (iVar4 = TPlayScreen::meth_0x4d2120_DoAttack(this_00), iVar4 != 0)) {
          return 0;
        }
      }
    }
    iVar4 = FUN_00483300_RandomRange(1,0x32);
    iVar5 = FUN_00483300_RandomRange(1,0x32);
    auStack_c[0] = 0xffffffff;
    uStack_10 = 0xffffffff;
    uStack_14 = 0xffffffff;
    uStack_18 = 0xffffffff;
    iStack_20 = -1;
    if (iStack_24 != 0) {
      iStack_20 = 100;
    }
    iVar4 = meth_0x4d1dd0(this,unaff_retaddr,(char)iVar4 + (char)iVar5,(char)auStack_c,
                          (char)&uStack_10,(char)&uStack_14,&uStack_18,(char)&iStack_20,0);
    if (iVar4 != 0) {
      uVar6 = TPlayScreen::meth_0x4d2120_DoAttack(&this->TPlayScreen);
      return uVar6;
    }
  }
  return 0;
}



// Function at 004d27f0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d27f0()

{
  dword dVar1;
  undefined uVar2;
  int3 extraout_var;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined in_stack_00000004;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined auStack_4 [4];
  
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar2) != 0 && -1 < extraout_var) {
    if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
      dVar1 = (this->TPlayScreen).TScreen.mbr_0xd8;
      iVar3 = *(int *)(dVar1 + 0x48);
      if ((iVar3 != 0) && ((*(uint *)(iVar3 + 0x24) & 0x2000000) != 0)) {
        return 0;
      }
      iVar3 = *(int *)(dVar1 + 0x4c);
      if ((iVar3 != 0) && ((*(byte *)(iVar3 + 0x24) & 0x80) != 0)) {
        return 0;
      }
    }
    TPlayScreen::meth_0x4cd690_FindCharacters
              (&this->TPlayScreen,auStack_4,1,0,(this->TPlayScreen).TScreen.mbr_0x36,0x20);
    iVar3 = FUN_00483300_RandomRange(1,0x32);
    iVar4 = FUN_00483300_RandomRange(1,0x32);
    uStack_8 = 0xffffffff;
    uStack_c = 0xffffffff;
    uStack_10 = 0xffffffff;
    uStack_14 = 0xffffffff;
    uStack_18 = 0xffffffff;
    iVar3 = meth_0x4d1dd0(this,in_stack_00000004,(char)iVar3 + (char)iVar4,(char)&uStack_8,
                          (char)&uStack_c,(char)&uStack_10,&uStack_14,(char)&uStack_18,1);
    if (iVar3 != 0) {
      uVar5 = TPlayScreen::meth_0x4d2120_DoAttack(&this->TPlayScreen);
      return uVar5;
    }
  }
  return 0;
}



// Function at 004d2be0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d2be0()

{
  char cVar1;
  int *piVar2;
  dword dVar3;
  int iVar4;
  TScreen__vftable_5a50e8 *pcVar5;
  undefined uVar6;
  int3 extraout_var;
  undefined3 extraout_var_00;
  char *pcVar7;
  undefined3 extraout_var_01;
  cls_0x4da9f0 *pcVar8;
  uint uVar9;
  uint uVar10;
  undefined4 *puVar11;
  char *pcVar12;
  undefined4 *puVar13;
  char *in_stack_00000004;
  undefined4 auStack_2c [5];
  void *pvStack_18;
  void *pvStack_14;
  void *local_c;
  undefined *puStack_8;
  cls_0x4da9f0 *pcStack_4;
  
  pcStack_4 = (cls_0x4da9f0 *)0xffffffff;
  puStack_8 = &LAB_0059e92c;
  local_c = ExceptionList;
  piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if ((piVar2 != (int *)0x0) &&
     (((*piVar2 == 3 || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19)))) &&
      (ExceptionList = &local_c,
      uVar6 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this),
      CONCAT31(extraout_var,uVar6) != 0 && -1 < extraout_var)))) {
    if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
      dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8;
      iVar4 = *(int *)(dVar3 + 0x48);
      if ((iVar4 != 0) && ((*(uint *)(iVar4 + 0x24) & 0x2000000) != 0)) {
        ExceptionList = pvStack_14;
        return 0;
      }
      iVar4 = *(int *)(dVar3 + 0x4c);
      if ((iVar4 != 0) && ((*(byte *)(iVar4 + 0x24) & 0x80) != 0)) {
        ExceptionList = pvStack_14;
        return 0;
      }
    }
    if ((*(short *)&(this->TPlayScreen).TScreen.mbr_0x4 == 0xb) &&
       (((this->TPlayScreen).mbr_0x36c & 2) != 0)) {
      meth_0x51d680_SetPlayerState(this);
    }
    if (((DAT_0066829c == 0) ||
        (uVar6 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477570_376)(this),
        0 < CONCAT31(extraout_var_00,uVar6))) || (DAT_00676e5c != '\0')) {
      switch((int)((int)in_stack_00000004 -
                   ((this->TPlayScreen).TScreen.mbr_0x36 + 0xf & 0xe0) & 0xff) >> 5) {
      case 0:
        in_stack_00000004 = s_leapf_005e0360;
        break;
      case 1:
        in_stack_00000004 = s_leapfr_005e0368;
        break;
      case 2:
        in_stack_00000004 = s_leapr_005e0370;
        break;
      case 3:
        in_stack_00000004 = s_leapbr_005e0378;
        break;
      case 4:
        in_stack_00000004 = s_leapb_005e0380;
        break;
      case 5:
        in_stack_00000004 = s_leapbl_005e0388;
        break;
      case 6:
        in_stack_00000004 = s_leapl_005e0390;
        break;
      case 7:
        in_stack_00000004 = s_leapfl_005e0398;
      }
      pcVar7 = (char *)FUN_004dadd0_StName((this->TPlayScreen).TScreen.mbr_0xe0 + 4,in_stack_00000004
                                   );
      uVar9 = 0xffffffff;
      do {
        pcVar12 = pcVar7;
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        pcVar12 = pcVar7 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar12;
      } while (cVar1 != '\0');
      uVar9 = ~uVar9;
      pcVar5 = (this->TPlayScreen).TScreen.vftptr_0x0;
      puVar11 = (undefined4 *)(pcVar12 + -uVar9);
      puVar13 = auStack_2c;
      for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *puVar13 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
      uVar6 = (*pcVar5[1].virt_meth_0x477840_0)((TScreen *)this);
      if (CONCAT31(extraout_var_01,uVar6) != 0) {
        pcStack_4 = (cls_0x4da9f0 *)FUN_00482fb0(100);
        local_c = (void *)0x0;
        if (pcStack_4 == (cls_0x4da9f0 *)0x0) {
          pcVar8 = (cls_0x4da9f0 *)0x0;
        }
        else {
          pcVar8 = cls_0x4da9f0::cls_0x4da9f0(pcStack_4,&stack0xffffffcc);
        }
        local_c = (void *)0xffffffff;
        pcVar8->mbr_0x60 = pcVar8->mbr_0x60 | 0x20;
        if ((pcVar8 != (cls_0x4da9f0 *)0x0) &&
           (dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8, dVar3 != 0)) {
          pcVar8->mbr_0x44 = *(dword *)(dVar3 + 0x44);
        }
        (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_0046f250_24)(pcVar8);
        ExceptionList = pvStack_18;
        return 1;
      }
    }
  }
  ExceptionList = pvStack_14;
  return 0;
}



// Function at 004d30f0

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d30f0()

{
  int *piVar1;
  
  piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if (((piVar1 != (int *)0x0) &&
      (((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) &&
       (this != (cls_0x5b4f30_TPlayer *)0x0)))) &&
     ((piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0xd8, piVar1 != (int *)0x0 &&
      (*piVar1 == 8)))) {
    piVar1[10] = 0;
    return 1;
  }
  return 0;
}



// Function at 004d3150

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d3150()

{
  char cVar1;
  int *piVar2;
  dword dVar3;
  TScreen__vftable_5a50e8 *pcVar4;
  undefined uVar5;
  int3 extraout_var;
  undefined3 extraout_var_00;
  int iVar6;
  undefined3 extraout_var_01;
  cls_0x4da9f0 *pcVar7;
  uint uVar8;
  uint uVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  longlong lVar14;
  undefined4 in_stack_00000004;
  undefined uStack_20;
  undefined uStack_1b;
  void *pvStack_18;
  void *pvStack_14;
  void *local_c;
  undefined *puStack_8;
  cls_0x4da9f0 *pcStack_4;
  
  pcStack_4 = (cls_0x4da9f0 *)0xffffffff;
  puStack_8 = &LAB_0059e956;
  local_c = ExceptionList;
  piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xe0;
  if (piVar2 == (int *)0x0) {
    ExceptionList = pvStack_14;
    return 0;
  }
  if (*piVar2 != 3) {
    if (piVar2 == (int *)0x0) {
      ExceptionList = pvStack_14;
      return 0;
    }
    if (*piVar2 != 0x19) {
      ExceptionList = pvStack_14;
      return 0;
    }
  }
  if (this == (cls_0x5b4f30_TPlayer *)0x0) {
    ExceptionList = pvStack_14;
    return 0;
  }
  piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xd8;
  if (piVar2 == (int *)0x0) {
    ExceptionList = pvStack_14;
    return 0;
  }
  if (*piVar2 != 3) {
    ExceptionList = pvStack_14;
    return 0;
  }
  ExceptionList = &local_c;
  uVar5 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar5) == 0 || extraout_var < 0) {
    ExceptionList = pvStack_14;
    return 0;
  }
  if (((this->TPlayScreen).TScreen.mbr_0x110 & 0x80000) == 0) {
    dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8;
    iVar6 = *(int *)(dVar3 + 0x48);
    if ((iVar6 != 0) && ((*(uint *)(iVar6 + 0x24) & 0x2000000) != 0)) {
      ExceptionList = pvStack_14;
      return 0;
    }
    iVar6 = *(int *)(dVar3 + 0x4c);
    if ((iVar6 != 0) && ((*(byte *)(iVar6 + 0x24) & 0x80) != 0)) {
      ExceptionList = pvStack_14;
      return 0;
    }
  }
  if ((*(short *)&(this->TPlayScreen).TScreen.mbr_0x4 == 0xb) &&
     (((this->TPlayScreen).mbr_0x36c & 2) != 0)) {
    meth_0x51d680_SetPlayerState(this);
  }
  if (((DAT_0066829c != 0) &&
      (uVar5 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477570_376)(this),
      CONCAT31(extraout_var_00,uVar5) < 1)) && (DAT_00676e5c == '\0')) {
    ExceptionList = pvStack_14;
    return 0;
  }
  cls_0x57d9d0::meth_0x5874f0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)this);
  uVar8 = 0xffffffff;
  pcVar11 = s_crollb_005e03b0;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  puVar10 = (undefined4 *)(pcVar12 + -uVar8);
  puVar13 = (undefined4 *)&uStack_20;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *puVar13 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  iVar6 = FUN_004dab80_TActionBlock_Is((this->TPlayScreen).TScreen.mbr_0xe0,&DAT_005e064c);
  if (iVar6 != 0) {
    uStack_20 = 0x68;
  }
  uVar8 = (uint)(this->TPlayScreen).TScreen.mbr_0x36;
  if (DAT_006671f0 != 0) {
    if (DAT_006671f0 != 0) {
      lVar14 = __ftol();
      uVar9 = 0x60U - (int)lVar14 & 0xff;
    }
    else {
      uVar9 = 0;
    }
    uVar8 = uVar8 + uVar9 & 0xff;
  }
  if ((0xf < uVar8) && (uVar8 < 0x31)) {
    switch(in_stack_00000004) {
    case 0:
    case 4:
    case 6:
      goto switchD_004d3303_caseD_0;
    case 1:
    case 5:
    case 7:
      goto switchD_004d3303_caseD_1;
    case 2:
      goto switchD_004d3303_caseD_2;
    case 3:
      goto switchD_004d3303_caseD_3;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0x30 < uVar8) && (uVar8 < 0x51)) {
    switch(in_stack_00000004) {
    case 0:
    case 2:
    case 4:
      goto switchD_004d3303_caseD_0;
    case 1:
    case 3:
    case 7:
      goto switchD_004d3303_caseD_1;
    case 5:
      goto switchD_004d3303_caseD_2;
    case 6:
      goto switchD_004d3303_caseD_3;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0x50 < uVar8) && (uVar8 < 0x71)) {
    switch(in_stack_00000004) {
    case 0:
      goto switchD_004d3303_caseD_3;
    case 1:
      goto switchD_004d3303_caseD_2;
    case 2:
    case 4:
    case 5:
      goto switchD_004d3303_caseD_0;
    case 3:
    case 6:
    case 7:
      goto switchD_004d3303_caseD_1;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0x70 < uVar8) && (uVar8 < 0x92)) {
    switch(in_stack_00000004) {
    case 0:
    case 3:
    case 6:
      goto switchD_004d3303_caseD_1;
    case 1:
    case 2:
    case 5:
      goto switchD_004d3303_caseD_0;
    case 4:
      goto switchD_004d3303_caseD_3;
    case 7:
      goto switchD_004d3303_caseD_2;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0x91 < uVar8) && (uVar8 < 0xb3)) {
    switch(in_stack_00000004) {
    case 0:
    case 4:
    case 6:
      goto switchD_004d3303_caseD_1;
    case 1:
    case 5:
    case 7:
      goto switchD_004d3303_caseD_0;
    case 2:
      goto switchD_004d3303_caseD_3;
    case 3:
      goto switchD_004d3303_caseD_2;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0xb2 < uVar8) && (uVar8 < 0xd3)) {
    switch(in_stack_00000004) {
    case 0:
    case 2:
    case 4:
      goto switchD_004d3303_caseD_1;
    case 1:
    case 3:
    case 7:
      goto switchD_004d3303_caseD_0;
    case 5:
      goto switchD_004d3303_caseD_3;
    case 6:
      goto switchD_004d3303_caseD_2;
    default:
      goto switchD_004d3303_caseD_8;
    }
  }
  if ((0xd2 < uVar8) && (uVar8 < 0xf3)) {
    switch(in_stack_00000004) {
    case 0:
switchD_004d3303_caseD_2:
      uStack_1b = 0x6c;
      break;
    case 1:
switchD_004d3303_caseD_3:
      uStack_1b = 0x72;
      break;
    case 2:
    case 4:
    case 5:
switchD_004d3303_caseD_1:
      uStack_1b = 0x66;
      break;
    case 3:
    case 6:
    case 7:
switchD_004d3303_caseD_0:
      uStack_1b = 0x62;
      break;
    default:
      break;
    }
switchD_004d3303_caseD_8:
    uVar5 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477840_0)
                      ((TScreen *)this);
    if (CONCAT31(extraout_var_01,uVar5) == 0) {
      ExceptionList = pvStack_14;
      return 0;
    }
    pcStack_4 = (cls_0x4da9f0 *)FUN_00482fb0(100);
    local_c = (void *)0x0;
    if (pcStack_4 == (cls_0x4da9f0 *)0x0) {
      pcVar7 = (cls_0x4da9f0 *)0x0;
    }
    else {
      pcVar7 = cls_0x4da9f0::cls_0x4da9f0(pcStack_4,&stack0xffffffd8);
    }
    local_c = (void *)0xffffffff;
    pcVar7->mbr_0x44 = *(dword *)((this->TPlayScreen).TScreen.mbr_0xd8 + 0x44);
    pcVar4 = (this->TPlayScreen).TScreen.vftptr_0x0;
    pcVar7->mbr_0x60 = pcVar7->mbr_0x60 & 0xffffffdf | 0x10;
    (*pcVar4[1].FUN_0046f250_24)(pcVar7);
    ExceptionList = pvStack_18;
    return 1;
  }
  switch(in_stack_00000004) {
  case 0:
  case 3:
  case 6:
    goto switchD_004d3303_caseD_0;
  case 1:
  case 2:
  case 5:
    goto switchD_004d3303_caseD_1;
  case 4:
    goto switchD_004d3303_caseD_2;
  case 7:
    goto switchD_004d3303_caseD_3;
  default:
    goto switchD_004d3303_caseD_8;
  }
}



// Function at 004d4610

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d4610()

{
  dword dVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  SoftwareRenderer *this_00;
  undefined uVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  int iVar4;
  cls_0x5b4f30_TPlayer *pcVar5;
  undefined4 uVar6;
  undefined1 unaff_retaddr;
  undefined1 in_stack_00000004;
  undefined4 in_stack_00000008;
  cls_0x44ceb0 cStack_4c;
  
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
  if (CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var) {
    return 0;
  }
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e10_432)(this);
  if (CONCAT31(extraout_var_00,uVar3) != 0) {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e20_436)(this);
  }
  cls_0x44ceb0::cls_0x44ceb0(&cStack_4c,&stack0xffffffa8,0x80,0xe0,4,0);
  this_00 = (SoftwareRenderer *)cStack_4c.mbr_0xc;
  while (this_00 != (SoftwareRenderer *)0x0) {
    cStack_4c.mbr_0xc = (dword)this_00;
    iVar4 = FUN_0046de60_Distance2D((int *)&stack0xffffffa8,(int *)&this_00->mbr_0x10);
    if (((iVar4 < 0x81) && (this_00->mbr_0x4 == 0x19)) && (dVar1 = this_00->mbr_0xd8, dVar1 != 0)) {
      if (*(int *)(dVar1 + 0xc) < 1) {
        pcVar5 = (cls_0x5b4f30_TPlayer *)0x0;
      }
      else {
        pcVar5 = *(cls_0x5b4f30_TPlayer **)(dVar1 + 0x10);
      }
      if ((pcVar5 == this) || ((cls_0x5b4f30_TPlayer *)this_00->mbr_0xe4 == this)) {
        SoftwareRenderer::meth_0x4defe0(this_00);
      }
    }
    cls_0x44ceb0::meth_0x44d080(&cStack_4c);
    this_00 = (SoftwareRenderer *)cStack_4c.mbr_0xc;
  }
  cls_0x57d9d0::meth_0x5865a0
            ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,unaff_retaddr,in_stack_00000004,
             in_stack_00000008);
  pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
  (*pcVar2->virt_meth_0x477eb0_472)(this);
  (*pcVar2->virt_meth_0x477e60_452)(this);
  pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
  (*pcVar2->virt_meth_0x477ef0_488)(this);
  (*pcVar2->virt_meth_0x477ea0_468)(this);
  pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
  (*pcVar2->virt_meth_0x477ed0_480)(this);
  (*pcVar2->virt_meth_0x477e80_460)(this);
  TPlayScreen::meth_0x4d4220(&this->TPlayScreen);
  TCharacter::meth_0x451610((TCharacter *)&DAT_006668d8);
  uVar6 = TPlayScreen::meth_0x4d4460(&this->TPlayScreen);
  TCharacter::meth_0x451090((TCharacter *)&DAT_006668d8);
  return uVar6;
}



// Function at 004d56c0

void cls_0x5b4f30_TPlayer::meth_0x4d56c0()

{
  undefined uVar1;
  int3 extraout_var;
  dword in_stack_00000004;
  
  (this->TPlayScreen).mbr_0x1a0 = in_stack_00000004;
  if (in_stack_00000004 == 1) {
    uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
    if (CONCAT31(extraout_var,uVar1) != 0 && -1 < extraout_var) {
      (this->TPlayScreen).mbr_0x198 = 0xfffffffb;
      (this->TPlayScreen).mbr_0x19c = 100;
      (this->TPlayScreen).mbr_0x1a0 = 1;
      return;
    }
  }
  else {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)(this);
    (this->TPlayScreen).mbr_0x198 = 5;
    (this->TPlayScreen).mbr_0x19c = 0;
    (this->TPlayScreen).mbr_0x1a0 = 0xffffffff;
  }
  return;
}



// Function at 004d5790

dword cls_0x5b4f30_TPlayer::meth_0x4d5790()

{
  return (this->TPlayScreen).mbr_0x194;
}



// Function at 004d5900

undefined4 cls_0x5b4f30_TPlayer::meth_0x4d5900()

{
  char cVar1;
  TScreen__vftable_5a50e8 *pcVar2;
  dword dVar3;
  char cVar4;
  undefined uVar5;
  uint *puVar6;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  cls_0x4da9f0 *pcVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  undefined4 *puVar11;
  char *pcVar12;
  char *pcVar13;
  undefined4 *puVar14;
  uint *in_stack_00000004;
  undefined4 local_74 [13];
  undefined4 local_40 [9];
  void *pvStack_1c;
  undefined4 uStack_14;
  cls_0x4da9f0 *pcStack_c;
  undefined *puStack_8;
  dword dStack_4;
  
  dStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e9e9;
  pcStack_c = (cls_0x4da9f0 *)ExceptionList;
  ExceptionList = &pcStack_c;
  puVar6 = FUN_0058ad30(in_stack_00000004,&DAT_005e0588);
  if (puVar6 == (uint *)0x0) {
    uVar8 = 0xffffffff;
    do {
      puVar6 = in_stack_00000004;
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      puVar6 = (uint *)((int)in_stack_00000004 + 1);
      cVar4 = *(char *)in_stack_00000004;
      in_stack_00000004 = puVar6;
    } while (cVar4 != '\0');
    uVar8 = ~uVar8;
    puVar11 = (undefined4 *)((int)puVar6 - uVar8);
    puVar14 = local_74;
    for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *puVar14 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar14 = puVar14 + 1;
    }
    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
      *(undefined *)puVar14 = *(undefined *)puVar11;
      puVar11 = (undefined4 *)((int)puVar11 + 1);
      puVar14 = (undefined4 *)((int)puVar14 + 1);
    }
  }
  else {
    uVar8 = 0xffffffff;
    pcVar12 = &DAT_005e058c;
    do {
      pcVar13 = pcVar12;
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      pcVar13 = pcVar12 + 1;
      cVar4 = *pcVar12;
      pcVar12 = pcVar13;
    } while (cVar4 != '\0');
    uVar8 = ~uVar8;
    puVar11 = (undefined4 *)(pcVar13 + -uVar8);
    puVar14 = local_40;
    for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *puVar14 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar14 = puVar14 + 1;
    }
    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
      *(undefined *)puVar14 = *(undefined *)puVar11;
      puVar11 = (undefined4 *)((int)puVar11 + 1);
      puVar14 = (undefined4 *)((int)puVar14 + 1);
    }
    uVar8 = 0xffffffff;
    puVar6 = in_stack_00000004;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar4 = *(char *)puVar6;
      puVar6 = (uint *)((int)puVar6 + 1);
    } while (cVar4 != '\0');
    uVar9 = 0xffffffff;
    pcVar12 = (char *)((~uVar8 - 2) + (int)in_stack_00000004);
    do {
      pcVar13 = pcVar12;
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      pcVar13 = pcVar12 + 1;
      cVar4 = *pcVar12;
      pcVar12 = pcVar13;
    } while (cVar4 != '\0');
    uVar9 = ~uVar9;
    iVar10 = -1;
    puVar11 = local_40;
    do {
      puVar14 = puVar11;
      if (iVar10 == 0) break;
      iVar10 = iVar10 + -1;
      puVar14 = (undefined4 *)((int)puVar11 + 1);
      cVar4 = *(char *)puVar11;
      puVar11 = puVar14;
    } while (cVar4 != '\0');
    puVar11 = (undefined4 *)(pcVar13 + -uVar9);
    puVar14 = (undefined4 *)((int)puVar14 + -1);
    for (uVar8 = uVar9 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *puVar14 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar14 = puVar14 + 1;
    }
    for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
      *(undefined *)puVar14 = *(undefined *)puVar11;
      puVar11 = (undefined4 *)((int)puVar11 + 1);
      puVar14 = (undefined4 *)((int)puVar14 + 1);
    }
    TPlayScreen::meth_0x4ce1b0_BuildActionName(&this->TPlayScreen,local_74);
  }
  cVar4 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477840_0)
                    ((TScreen *)this);
  if (CONCAT31(extraout_var,cVar4) == 0) {
    uVar8 = 0xffffffff;
    pcVar12 = s_invoke_005e0590;
    do {
      pcVar13 = pcVar12;
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      pcVar13 = pcVar12 + 1;
      cVar1 = *pcVar12;
      pcVar12 = pcVar13;
    } while (cVar4 != cVar1);
    uVar8 = ~uVar8;
    pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
    puVar11 = (undefined4 *)(pcVar13 + -uVar8);
    puVar14 = (undefined4 *)&stack0xffffff84;
    for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *puVar14 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar14 = puVar14 + 1;
    }
    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
      *(undefined *)puVar14 = *(undefined *)puVar11;
      puVar11 = (undefined4 *)((int)puVar11 + 1);
      puVar14 = (undefined4 *)((int)puVar14 + 1);
    }
    uVar5 = (*pcVar2[1].virt_meth_0x477840_0)((TScreen *)this);
    if (CONCAT31(extraout_var_00,uVar5) == 0) {
      ExceptionList = pvStack_1c;
      return 0;
    }
  }
  pcStack_c = (cls_0x4da9f0 *)FUN_00482fb0(100);
  uStack_14 = 0;
  if (pcStack_c == (cls_0x4da9f0 *)0x0) {
    pcVar7 = (cls_0x4da9f0 *)0x0;
  }
  else {
    pcVar7 = cls_0x4da9f0::cls_0x4da9f0(pcStack_c,&stack0xffffff7c);
  }
  pcVar7->mbr_0x60 = pcVar7->mbr_0x60 & 0xffffffef;
  (this->TPlayScreen).mbr_0x188 = dStack_4;
  pcVar7->mbr_0x28 = dStack_4;
  pcVar2 = (this->TPlayScreen).TScreen.vftptr_0x0;
  uStack_14 = 0xffffffff;
  pcVar7->mbr_0x44 = (dword)puStack_8;
  (*pcVar2[1].FUN_0046e8b0_40)(pcVar7,0,0);
  dVar3 = (this->TPlayScreen).TScreen.mbr_0xd8;
  if ((((dVar3 != 0) && ((*(byte *)(dVar3 + 0x60) & 0x10) != 0)) &&
      (*(dword *)&(this->TPlayScreen).TScreen.field_0xdc != dVar3)) &&
     (pcVar7 != (cls_0x4da9f0 *)0x0)) {
    if ((LPCVOID)pcVar7->mbr_0x5c != (LPCVOID)0x0) {
      FUN_00482f80((LPCVOID)pcVar7->mbr_0x5c);
    }
    FUN_004830f0(pcVar7);
  }
  ExceptionList = pvStack_1c;
  return 1;
}



// Function at 004d6550

void cls_0x5b4f30_TPlayer::meth_0x4d6550()

{
  undefined4 uVar1;
  
  uVar1 = cls_0x48c780::meth_0x48c780
                    ((cls_0x48c780 *)&UNK_0065d6a6.field_0x102,
                     (int)*(short *)((int)&(this->TPlayScreen).TScreen.mbr_0x4 + 2));
  *(undefined4 *)&(this->TPlayScreen).TScreen.field_0xfc = uVar1;
  return;
}



// Function at 004f6ea0

dword cls_0x5b4f30_TPlayer::meth_0x4f6ea0()

{
  return (this->TPlayScreen).TScreen.mbr_0xd8;
}



// Function at 00518450

cls_0x5b4f30_TPlayer::cls_0x5b4f30_TPlayer *this,undefined4 param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1110;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  TPlayScreen::TPlayScreen(&this->TPlayScreen,param_1);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->TPlayScreen).field_0x2ec,0x40);
  local_4._0_1_ = 1;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->TPlayScreen).field_0x338,0x40);
  *(cls_0x5b5354__vftable_5b5354 **)&(this->TPlayScreen).field_0x314 =
       &cls_0x5b5354__vftable_5b5354_005b5354;
  local_4._0_1_ = 2;
  (this->TPlayScreen).mbr_0x34c = 0;
  *(undefined2 *)&(this->TPlayScreen).field_0x34e = 1;
  (this->TPlayScreen).mbr_0x350 = 0;
  puVar1 = FUN_00482fb0(8);
  (this->TPlayScreen).mbr_0x350 = (dword)puVar1;
  for (uVar2 = (uint)((int)*(short *)&(this->TPlayScreen).field_0x34e << 3) >> 2; uVar2 != 0;
      uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  local_4._0_1_ = 3;
  (this->TPlayScreen).mbr_0x358 = 0;
  (this->TPlayScreen).mbr_0x35a = 4;
  (this->TPlayScreen).mbr_0x35c = 0;
  puVar1 = FUN_00482fb0(0x20);
  (this->TPlayScreen).mbr_0x35c = (dword)puVar1;
  for (uVar2 = (uint)((int)(short)(this->TPlayScreen).mbr_0x35a << 3) >> 2; uVar2 != 0;
      uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  local_4 = CONCAT31(local_4._1_3_,4);
  (this->TPlayScreen).TScreen.vftptr_0x0 =
       (TScreen__vftable_5a50e8 *)&cls_0x5b4f30__vftable_5b4f30_005b4f30;
  meth_0x518750_TPlayer_ClearPlayer(this);
  ExceptionList = local_c;
  return this;
}



// Function at 00518570

cls_0x5b4f30_TPlayer * __thiscall cls_0x5b4f30_TPlayer::~cls_0x5b4f30_TPlayer(cls_0x5b4f30_TPlayer *this)

{
  undefined *puVar1;
  LPCVOID pvVar2;
  undefined4 *puVar3;
  int iVar4;
  cls_0x5b4f30_TPlayer *pcVar5;
  cls_0x5b4f30_TPlayer **ppcVar6;
  int iVar7;
  dword *pdVar8;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1165;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->TPlayScreen).TScreen.vftptr_0x0 =
       (TScreen__vftable_5a50e8 *)&cls_0x5b4f30__vftable_5b4f30_005b4f30;
  local_4 = 4;
  if (((this->TPlayScreen).TScreen.mbr_0x8 & 0x80000000) == 0) {
    meth_0x46e630(this);
  }
  if (this == DAT_00667fcc) {
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
    cls_0x535a10::meth_0x535d80((cls_0x535a10 *)&DAT_00667cc8);
    FUN_00533dc0(0);
    DAT_00667e5c._0_4_ = 0;
    DAT_00667eb0._0_4_ = 1;
    cls_0x5b93c4::meth_0x5391a0((cls_0x5b93c4 *)&DAT_0065d4f8);
  }
  puVar1 = &(this->TPlayScreen).field_0x2ec;
  iVar7 = 0;
  if (0 < *(int *)&(this->TPlayScreen).field_0x2ec) {
    do {
      if ((-1 < iVar7) &&
         (pvVar2 = *(LPCVOID *)(*(int *)&(this->TPlayScreen).field_0x2fc + iVar7 * 4),
         pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)puVar1);
      iVar7 = iVar7 + 1;
    } while (iVar7 < *(int *)puVar1);
  }
  iVar7 = DAT_0065a890;
  iVar4 = 0;
  *(undefined4 *)puVar1 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x2f0 = 0;
  ppcVar6 = DAT_0065a8a0;
  if (0 < iVar7) {
    do {
      if (*ppcVar6 == this) {
        cls_0x45f7c0::meth_0x51f200((cls_0x45f7c0 *)&DAT_0065a890,iVar4);
        goto LAB_0051866b;
      }
      iVar4 = iVar4 + 1;
      ppcVar6 = ppcVar6 + 1;
    } while (iVar4 < iVar7);
  }
  cls_0x45f7c0::meth_0x51f370((cls_0x45f7c0 *)&DAT_0065a890);
LAB_0051866b:
  local_4._0_1_ = 3;
  pdVar8 = &(this->TPlayScreen).mbr_0x2a0;
  for (iVar7 = 0xb; iVar7 != 0; iVar7 = iVar7 + -1) {
    *pdVar8 = 0;
    pdVar8 = pdVar8 + 1;
  }
  pvVar2 = (LPCVOID)(this->TPlayScreen).mbr_0x35c;
  if (pvVar2 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar2);
  }
  pvVar2 = (LPCVOID)(this->TPlayScreen).mbr_0x350;
  local_4._0_1_ = 2;
  if (pvVar2 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar2);
  }
  *(cls_0x5b5354__vftable_5b5354 **)&(this->TPlayScreen).field_0x314 =
       &cls_0x5b5354__vftable_5b5354_005b5354;
  puVar1 = &(this->TPlayScreen).field_0x338;
  iVar7 = 0;
  local_4._0_1_ = 5;
  if (0 < *(int *)puVar1) {
    do {
      if ((-1 < iVar7) &&
         (puVar3 = *(undefined4 **)(*(int *)&(this->TPlayScreen).field_0x348 + iVar7 * 4),
         puVar3 != (undefined4 *)0x0)) {
        FUN_00520070(puVar3,1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)puVar1);
      iVar7 = iVar7 + 1;
    } while (iVar7 < *(int *)puVar1);
  }
  *(undefined4 *)puVar1 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x33c = 0;
  local_4._0_1_ = 1;
  FUN_004830f0(*(LPCVOID *)&(this->TPlayScreen).field_0x348);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0(*(LPCVOID *)&(this->TPlayScreen).field_0x2fc);
  local_4 = 0xffffffff;
  pcVar5 = (cls_0x5b4f30_TPlayer *)cls_0x5a761c::~cls_0x5a761c((cls_0x5a761c *)this);
  ExceptionList = local_c;
  return pcVar5;
}



// Function at 00518750

void cls_0x5b4f30_TPlayer::meth_0x518750_TPlayer_ClearPlayer()

{
  undefined *this_00;
  word *this_01;
  short sVar1;
  word wVar2;
  LPCVOID pvVar3;
  TScreen__vftable_5a50e8 *pcVar4;
  dword dVar5;
  undefined4 uVar6;
  dword *pdVar7;
  undefined4 *puVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  uint uVar12;
  
  (this->TPlayScreen).field_0x378 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x398 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x39c = 0;
  this->mbr_0x664 = 0xffffffff;
  puVar8 = (undefined4 *)&(this->TPlayScreen).field_0x490;
  for (iVar9 = 0x18; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  puVar8 = (undefined4 *)&(this->TPlayScreen).field_0x570;
  for (iVar9 = 0x38; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  *(undefined4 *)&(this->TPlayScreen).field_0x65c = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x658 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x654 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x650 = 0;
  pdVar7 = &(this->TPlayScreen).mbr_0x2a0;
  for (iVar9 = 0xb; iVar9 != 0; iVar9 = iVar9 + -1) {
    *pdVar7 = 0;
    pdVar7 = pdVar7 + 1;
  }
  iVar11 = 0;
  (this->TPlayScreen).TScreen.mbr_0x8 = (this->TPlayScreen).TScreen.mbr_0x8 | 0x80000;
  puVar8 = (undefined4 *)&(this->TPlayScreen).field_0x2cc;
  for (iVar9 = 7; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  *(undefined2 *)puVar8 = 0;
  this_00 = &(this->TPlayScreen).field_0x2ec;
  *(undefined4 *)&(this->TPlayScreen).field_0x300 = 0;
  if (0 < *(int *)&(this->TPlayScreen).field_0x2ec) {
    do {
      if ((-1 < iVar11) &&
         (pvVar3 = *(LPCVOID *)(*(int *)&(this->TPlayScreen).field_0x2fc + iVar11 * 4),
         pvVar3 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar3);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar11 = iVar11 + 1;
    } while (iVar11 < *(int *)this_00);
  }
  uVar12 = (this->TPlayScreen).TScreen.mbr_0x8;
  *(undefined4 *)this_00 = 0;
  pcVar4 = (this->TPlayScreen).TScreen.vftptr_0x0;
  *(undefined4 *)&(this->TPlayScreen).field_0x2f0 = 0;
  (*pcVar4->FUN_00472e90_64)(uVar12 & 0xfbffffff);
  iVar9 = *(int *)&(this->TPlayScreen).TScreen.field_0xfc;
  *(undefined4 *)&(this->TPlayScreen).field_0x364 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x360 = 0;
  *(undefined4 *)&(this->TPlayScreen).field_0x368 = 0;
  iVar11 = 6;
  piVar10 = (int *)(*(int *)(iVar9 + 400) + 0x20);
  do {
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  iVar9 = 0x22;
  do {
    if (*piVar10 == 0) {
      (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
    }
    else {
      (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
    }
    piVar10 = piVar10 + 1;
    iVar11 = iVar9 + -0x21;
    iVar9 = iVar9 + 1;
  } while (iVar11 < 6);
  iVar9 = 0;
  do {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
    iVar9 = iVar9 + 1;
  } while (iVar9 < 0xb);
  pvVar3 = (LPCVOID)(this->TPlayScreen).mbr_0x350;
  this_01 = &(this->TPlayScreen).mbr_0x34c;
  (this->TPlayScreen).mbr_0x354 = 0xffffffff;
  if (pvVar3 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar3);
  }
  dVar5 = (this->TPlayScreen).TScreen.mbr_0x48;
  iVar9 = 0;
  (this->TPlayScreen).mbr_0x350 = 0;
  *this_01 = 0;
  sVar1 = *(short *)(dVar5 + 0x1c);
  *(undefined2 *)&(this->TPlayScreen).field_0x34e = 0;
  if (0 < sVar1) {
    do {
      iVar11 = cls_0x51ff30::meth_0x51ff30((cls_0x51ff30 *)(dVar5 + 0x1c));
      if (iVar11 != 0) {
        if (*(short *)&(this->TPlayScreen).field_0x34e <= (short)*this_01) {
          cls_0x51ffa0::meth_0x51ffa0((cls_0x51ffa0 *)this_01);
        }
        wVar2 = *this_01;
        *this_01 = wVar2 + 1;
        piVar10 = (int *)((this->TPlayScreen).mbr_0x350 + (short)wVar2 * 8);
        *piVar10 = iVar9;
        piVar10[1] = 0;
      }
      dVar5 = (this->TPlayScreen).TScreen.mbr_0x48;
      iVar9 = iVar9 + 1;
    } while (iVar9 < *(short *)(dVar5 + 0x1c));
  }
  TPlayScreen::meth_0x51c660_TPlayer_RefreshStats(&this->TPlayScreen);
  (this->TPlayScreen).field_0x4f0 = 0;
  (this->TPlayScreen).mbr_0x36c = 1;
  meth_0x472db0(this,&DAT_005e2714);
  uVar6 = TCharacter::meth_0x47e920((TCharacter *)&DAT_0065caf0);
  *(undefined4 *)&(this->TPlayScreen).field_0x374 = uVar6;
  uVar6 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
  *(undefined4 *)&(this->TPlayScreen).field_0x370 = uVar6;
  (this->TPlayScreen).mbr_0x660 = 0;
  pdVar7 = &(this->TPlayScreen).mbr_0x3a0;
  iVar9 = 10;
  do {
    *pdVar7 = 0;
    pdVar7 = pdVar7 + 6;
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  uVar12 = 0;
  if (0 < (short)(this->TPlayScreen).mbr_0x358) {
    do {
      pvVar3 = *(LPCVOID *)((this->TPlayScreen).mbr_0x35c + uVar12 * 8);
      if (pvVar3 != (LPCVOID)0x0) {
        FUN_004830f0(pvVar3);
      }
      wVar2 = (this->TPlayScreen).mbr_0x358;
      if (uVar12 < (uint)(int)(short)wVar2) {
        iVar9 = (short)(this->TPlayScreen).mbr_0x35a + -1;
        if ((int)uVar12 < iVar9) {
          puVar8 = (undefined4 *)((this->TPlayScreen).mbr_0x35c + uVar12 * 8);
          iVar9 = iVar9 - uVar12;
          do {
            *puVar8 = puVar8[2];
            puVar8[1] = puVar8[3];
            puVar8 = puVar8 + 2;
            iVar9 = iVar9 + -1;
          } while (iVar9 != 0);
        }
        wVar2 = wVar2 - 1;
        (this->TPlayScreen).mbr_0x358 = wVar2;
        if ((short)wVar2 < 1) {
          pvVar3 = (LPCVOID)(this->TPlayScreen).mbr_0x35c;
          if (pvVar3 != (LPCVOID)0x0) {
            FUN_004830f0(pvVar3);
          }
          (this->TPlayScreen).mbr_0x35c = 0;
          (this->TPlayScreen).mbr_0x358 = 0;
          (this->TPlayScreen).mbr_0x35a = 0;
        }
      }
      uVar12 = uVar12 + 1;
    } while ((int)uVar12 < (int)(short)(this->TPlayScreen).mbr_0x358);
  }
  pvVar3 = (LPCVOID)(this->TPlayScreen).mbr_0x35c;
  if (pvVar3 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar3);
  }
  (this->TPlayScreen).mbr_0x35c = 0;
  (this->TPlayScreen).mbr_0x358 = 0;
  (this->TPlayScreen).mbr_0x35a = 0;
  return;
}



// Function at 00518ed0

void cls_0x5b4f30_TPlayer::virt_meth_0x518ed0()

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int in_stack_00000004;
  
  iVar2 = DAT_0067682c;
  iVar1 = DAT_0066829c;
  if ((in_stack_00000004 != 0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
    if (*(short *)(in_stack_00000004 + 4) == 0xb) {
      iVar4 = *(int *)&(this->TPlayScreen).field_0x658;
      iVar3 = *(int *)&(this->TPlayScreen).field_0x650 + 1;
      if (*(int *)&(this->TPlayScreen).field_0x650 == iVar3) {
        return;
      }
      *(int *)&(this->TPlayScreen).field_0x650 = iVar3;
      *(int *)&(this->TPlayScreen).field_0x658 = iVar4;
    }
    else {
      iVar3 = *(int *)&(this->TPlayScreen).field_0x650;
      iVar4 = *(int *)&(this->TPlayScreen).field_0x658 + 1;
      if (*(int *)&(this->TPlayScreen).field_0x658 == iVar4) {
        return;
      }
      *(int *)&(this->TPlayScreen).field_0x650 = iVar3;
      *(int *)&(this->TPlayScreen).field_0x658 = iVar4;
    }
    if (iVar1 != 0) {
      if (iVar2 != 0) {
        cls_0x57d9d0::meth_0x587350((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,iVar3,iVar4);
      }
      cls_0x51f6b0::meth_0x51f6b0((cls_0x51f6b0 *)&DAT_0065a890);
      cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
    }
  }
  return;
}



// Function at 00519050

void cls_0x5b4f30_TPlayer::meth_0x519050(int param_1, int param_2)

{
  int iVar1;
  byte *pbVar2;
  int in_stack_0000000c;
  char local_80 [128];
  
  iVar1 = DAT_0066829c;
  if ((*(int *)&(this->TPlayScreen).field_0x654 != param_1) ||
     (*(int *)&(this->TPlayScreen).field_0x65c != param_2)) {
    *(int *)&(this->TPlayScreen).field_0x654 = param_1;
    *(int *)&(this->TPlayScreen).field_0x65c = param_2;
    if (iVar1 != 0) {
      if (DAT_0067682c != 0) {
        cls_0x57d9d0::meth_0x587420((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,param_1,param_2);
      }
      if ((in_stack_0000000c != 0) && (*(short *)(in_stack_0000000c + 4) == 0xb)) {
        FUN_00483300_RandomRange(0,4);
        iVar1 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        if (iVar1 < 0) {
          FUN_004811b0(local_80,0x80,(byte *)s___s__killed___s__Plyr___d_Monst__005e275c);
        }
        else {
          pbVar2 = (byte *)cls_0x45f7c0::meth_0x49d780((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
          FUN_004811b0(local_80,0x80,pbVar2);
        }
        FUN_0054d190((TCharacter *)&DAT_0065c5d0,1,local_80);
      }
      cls_0x51f6b0::meth_0x51f6b0((cls_0x51f6b0 *)&DAT_0065a890);
      cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
    }
  }
  return;
}



// Function at 00519850

char cls_0x5b4f30_TPlayer::virt_meth_0x519850()

{
  int *piVar1;
  char cVar2;
  char cVar3;
  
  cVar2 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477e80_460)(this);
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2b0;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2ac;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = (int *)(this->TPlayScreen).mbr_0x2a0;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2a8;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2a4;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2b4;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2b8;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = (int *)(this->TPlayScreen).mbr_0x2bc;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2c4;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2c8;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 2)) {
    cVar3 = (**(code **)(*piVar1 + 0x1f0))();
    cVar2 = cVar2 + cVar3;
  }
  return cVar2;
}



// Function at 00519ea0

bool cls_0x5b4f30_TPlayer::meth_0x519ea0()

{
  dword dVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  int *piVar5;
  int *piVar6;
  int *in_stack_00000004;
  cls_0x477870 local_38;
  cls_0x477870 local_1c;
  
  local_38.mbr_0x0 = 0;
  local_38.mbr_0x10 = 0;
  local_38.mbr_0xc = 0;
  local_38.mbr_0x14 = 0;
  local_38.mbr_0x18 = 0;
  local_38.mbr_0x4 = (dword)this;
  local_38.mbr_0x8 = (dword)this;
  cls_0x477870::meth_0x46dfb0(&local_38);
  uVar2 = extraout_ECX;
  piVar6 = in_stack_00000004;
  piVar3 = in_stack_00000004;
  do {
    if ((int *)local_38.mbr_0x18 == (int *)0x0) {
LAB_00519f9f:
      if (piVar6 != (int *)0x0) {
        dVar1 = (&(this->TPlayScreen).mbr_0x2a0)[(int)piVar3];
        if (dVar1 != 0) {
          *(undefined2 *)(dVar1 + 0x7c) = *(undefined2 *)(piVar6 + 0x1f);
        }
        *(short *)(piVar6 + 0x1f) = (short)piVar3 + 0x100;
      }
      UNK_0065d53c._12_4_ = 1;
      iVar4 = TPlayScreen::meth_0x5199b0(&this->TPlayScreen,piVar6);
      return iVar4 != 0;
    }
    uVar2 = FUN_0059a530_stricmp(uVar2,*(byte **)(local_38.mbr_0x18 + 0x38),(byte *)in_stack_00000004);
    piVar5 = (int *)local_38.mbr_0x18;
    if (uVar2 == 0) {
      piVar3 = (int *)(**(code **)(*(int *)local_38.mbr_0x18 + 0x188))();
      piVar6 = piVar5;
      goto LAB_00519f9f;
    }
    local_1c.mbr_0x0 = 0;
    local_1c.mbr_0x8 = local_38.mbr_0x18;
    local_1c.mbr_0x4 = local_38.mbr_0x18;
    local_1c.mbr_0x10 = 0;
    local_1c.mbr_0xc = 0;
    local_1c.mbr_0x14 = 0;
    local_1c.mbr_0x18 = 0;
    cls_0x477870::meth_0x46dfb0(&local_1c);
    uVar2 = extraout_ECX_00;
    while (piVar5 = piVar6, (int *)local_1c.mbr_0x18 != (int *)0x0) {
      uVar2 = FUN_0059a530_stricmp(uVar2,*(byte **)(local_1c.mbr_0x18 + 0x38),(byte *)in_stack_00000004);
      piVar5 = (int *)local_1c.mbr_0x18;
      if (uVar2 == 0) {
        piVar3 = (int *)(**(code **)(*(int *)local_1c.mbr_0x18 + 0x188))();
        break;
      }
      cls_0x477870::meth_0x46dfb0(&local_1c);
      uVar2 = extraout_ECX_01;
    }
    cls_0x477870::meth_0x46dfb0(&local_38);
    uVar2 = extraout_ECX_02;
    piVar6 = piVar5;
  } while( true );
}



// Function at 0051a480

int cls_0x5b4f30_TPlayer::virt_meth_0x51a480()

{
  int iVar1;
  undefined uVar2;
  int iVar3;
  int iVar4;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 unaff_EDI;
  
  iVar1 = *(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 0x1cc);
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x24);
  iVar3 = FUN_0048cc20(0x65d7a8,2);
  iVar4 = cls_0x41c7f0::meth_0x540820(&(this->TPlayScreen).cls_0x41c7f0);
  uVar2 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00477aa0_220)(unaff_EDI);
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)
                    (CONCAT31(extraout_var,uVar2) + 0x2b);
  return CONCAT31(extraout_var_00,uVar2) + iVar1 + iVar3 + iVar4;
}



// Function at 0051a4e0

int cls_0x5b4f30_TPlayer::virt_meth_0x51a4e0()

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = *(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 0x1c8);
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x25);
  iVar2 = FUN_0048cc20(0x65d7a8,3);
  iVar3 = cls_0x41c7f0::meth_0x5407d0(&(this->TPlayScreen).cls_0x41c7f0);
  return iVar3 + iVar1 + iVar2;
}



// Function at 0051a520

int cls_0x5b4f30_TPlayer::virt_meth_0x51a520()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 unaff_EDI;
  int iVar2;
  
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
  iVar2 = DAT_0065d844 * CONCAT31(extraout_var,uVar1);
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[2].FUN_0046e900_44)();
  return CONCAT31(extraout_var_00,uVar1) + iVar2;
}



// Function at 0051a550

int cls_0x5b4f30_TPlayer::virt_meth_0x51a550()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 unaff_EDI;
  int iVar2;
  
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
  iVar2 = DAT_0065d844 * CONCAT31(extraout_var,uVar1);
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[2].FUN_0046e930_48)();
  return CONCAT31(extraout_var_00,uVar1) + iVar2;
}



// Function at 0051b2f0

void cls_0x5b4f30_TPlayer::virt_meth_0x51b2f0(undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2b0;
  if ((piVar1 != (int *)0x0) && (*(short *)(piVar1 + 1) == 1)) {
    iVar2 = (**(code **)(*piVar1 + 0x210))();
    if (iVar2 != -1) {
      (**(code **)(**(int **)&(this->TPlayScreen).field_0x2b0 + 0x210))();
      return;
    }
  }
  TPlayScreen::virt_meth_0x4c47d0(&this->TPlayScreen,(undefined1)param_1);
  return;
}



// Function at 0051b340

char * cls_0x5b4f30_TPlayer::virt_meth_0x51b340_GetCombatRoot()

{
  int iVar1;
  char *pcVar2;
  int *in_stack_00000004;
  
  if ((in_stack_00000004 == (int *)0x0) &&
     (in_stack_00000004 = *(int **)&(this->TPlayScreen).field_0x2b0,
     in_stack_00000004 == (int *)0x0)) {
    return &DAT_005e290c;
  }
  if (*(short *)(in_stack_00000004 + 1) != 1) {
    return s_combat_005e2914;
  }
  iVar1 = (**(code **)(*in_stack_00000004 + 0x1f0))();
  if (iVar1 == 5) {
    return s_cstaff_005e291c;
  }
  iVar1 = (**(code **)(*in_stack_00000004 + 0x1f0))();
  pcVar2 = s_cknife_005e2924;
  if (iVar1 != 1) {
    pcVar2 = s_combat_005e292c;
  }
  return pcVar2;
}



// Function at 0051b420

void cls_0x5b4f30_TPlayer::meth_0x51b420_GetOnYerHog()

{
  int *piVar1;
  undefined4 *puVar2;
  undefined uVar3;
  char *pcVar4;
  uint uVar5;
  int iVar6;
  dword dVar7;
  undefined uVar8;
  undefined4 uVar9;
  
  pcVar4 = s_Locke_005e2950;
  if (*(int *)&(this->TPlayScreen).field_0x300 == 0) {
    pcVar4 = (char *)&PTR_DAT_005e2958;
  }
  uVar5 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066d90c.field_0x4,pcVar4);
  if (-1 < (int)uVar5) {
    piVar1 = (int *)(this->TPlayScreen).TScreen.mbr_0x58;
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x1c))();
      puVar2 = (undefined4 *)(this->TPlayScreen).TScreen.mbr_0x58;
      if (puVar2 != (undefined4 *)0x0) {
        (**(code **)*puVar2)(1);
      }
      (this->TPlayScreen).TScreen.mbr_0x58 = 0;
    }
    FUN_00446ba0((cls_0x5a486c *)(this->TPlayScreen).TScreen.mbr_0x54);
    if (((DAT_0066d944 == 0) || (DAT_0066d934 <= uVar5)) ||
       (*(int *)(DAT_0066d944 + uVar5 * 4) == 0)) {
      iVar6 = 0;
    }
    else {
      iVar6 = *(int *)(DAT_0066d944 + uVar5 * 4);
      if (iVar6 == 0) {
        iVar6 = DAT_0066d948;
      }
    }
    dVar7 = FUN_00446b10(*(uint *)(iVar6 + 8),1);
    (this->TPlayScreen).TScreen.mbr_0x54 = dVar7;
    if (dVar7 != 0) {
      iVar6 = *(int *)&(this->TPlayScreen).field_0x300;
      *(short *)((int)&(this->TPlayScreen).TScreen.mbr_0x4 + 2) = (short)uVar5;
      if (iVar6 == 0) {
        cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
        cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
        uVar9 = 1;
        uVar8 = 0x7f;
        uVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
        cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,uVar3,uVar8,uVar9);
        *(undefined4 *)&(this->TPlayScreen).field_0x300 = 1;
        return;
      }
      uVar9 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      cls_0x41c7d0::meth_0x49b8e0((cls_0x41c7d0 *)&DAT_00667548,uVar9);
      cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
      *(undefined4 *)&(this->TPlayScreen).field_0x300 = 0;
    }
  }
  return;
}



// Function at 0051d680

void cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint in_stack_00000004;
  
  dVar1 = (this->TPlayScreen).mbr_0x36c;
  (this->TPlayScreen).mbr_0x36c = in_stack_00000004;
  if ((((in_stack_00000004 & 2) != 0) &&
      (piVar2 = (int *)(this->TPlayScreen).TScreen.mbr_0xd8, piVar2 != (int *)0x0)) &&
     ((iVar3 = *piVar2, iVar3 == 2 || ((iVar3 == 4 || (iVar3 == 0x1a)))))) {
    TPlayScreen::meth_0x4cee70_Stop(&this->TPlayScreen);
  }
  if (DAT_0066829c != 0) {
    if (this == DAT_00667fcc) {
      if ((((in_stack_00000004 & 4) != 0) && (DAT_0065d0d0 != 0)) ||
         (((in_stack_00000004 & 4) == 0 && (DAT_0065d0d0 == 0)))) {
        TCharacter::meth_0x47c580((TCharacter *)&DAT_0065caf0);
      }
      if (((((in_stack_00000004 & 8) == 0) || (DAT_0065cb34 == (int *)0x0)) ||
          (iVar3 = TCharacter::meth_0x48eaf0((TCharacter *)&DAT_0065caf0), iVar3 != 0)) ||
         (iVar3 = TCharacter::meth_0x48eb00((TCharacter *)&DAT_0065caf0), iVar3 != 0)) {
        if ((((in_stack_00000004 & 8) == 0) && (DAT_0065cb30 != (int *)0x0)) &&
           ((iVar3 = TCharacter::meth_0x48ead0((TCharacter *)&DAT_0065caf0), iVar3 == 0 &&
            ((iVar3 = TCharacter::meth_0x48eb00((TCharacter *)&DAT_0065caf0), iVar3 == 0 &&
             (DAT_0065cb30 != (int *)0x0)))))) {
          (**(code **)(*DAT_0065cb30 + 0x28))();
        }
      }
      else {
        (**(code **)(*DAT_0065cb34 + 0x2c))();
        cls_0x535a10::meth_0x535d80((cls_0x535a10 *)&DAT_00667cc8);
        iVar3 = TCharacter::meth_0x47ed20((TCharacter *)&DAT_0065caf0);
        if (iVar3 == 3) {
          TCharacter::meth_0x47ecc0((TCharacter *)&DAT_0065caf0);
        }
      }
      if (((in_stack_00000004 & 0x10) == 0) &&
         ((cls_0x5b4f30_TPlayer *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) != this)) {
        TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,this);
      }
    }
    if (dVar1 != in_stack_00000004) {
      if (DAT_0067682c != 0) {
        cls_0x57d9d0::meth_0x5878a0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this);
        return;
      }
      cls_0x57d9d0::meth_0x583fe0
                ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x45,in_stack_00000004,0);
    }
  }
  return;
}



// Function at 0051db30

void cls_0x5b4f30_TPlayer::meth_0x51db30(int param_1)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar3;
  int iVar4;
  undefined3 extraout_var_05;
  int iVar5;
  undefined4 unaff_EBP;
  int iVar6;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  int iVar7;
  int in_stack_00000008;
  int in_stack_0000000c;
  int iStack00000010;
  int iStack_8;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  
  if ((param_1 < 0x22) || (0x27 < param_1)) {
    iVar5 = 0;
  }
  else {
    iVar2 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + -0x68 +
                    param_1 * 4);
    if (iVar2 < 0) {
      uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
      iVar6 = CONCAT31(extraout_var,uVar1);
      iVar5 = iVar6 * 2 + 0x52;
      if (0xf < iVar6) {
        iVar5 = iVar5 + (0xf - iVar6);
      }
      iVar5 = (-0x54 - iVar2) + iVar5;
      if (iVar5 < 0x1e) goto LAB_0051db8d;
    }
    iVar5 = 0x1e;
  }
LAB_0051db8d:
  iVar2 = in_stack_0000000c;
  if (iVar5 <= in_stack_0000000c) {
    if ((param_1 < 0x22) || (0x27 < param_1)) {
      iVar2 = 0;
    }
    else {
      iVar5 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + -0x68
                      + param_1 * 4);
      if (iVar5 < 0) {
        uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_ESI);
        iVar6 = CONCAT31(extraout_var_00,uVar1);
        iVar2 = iVar6 * 2 + 0x52;
        if (0xf < iVar6) {
          iVar2 = iVar2 + (0xf - iVar6);
        }
        iVar2 = (-0x54 - iVar5) + iVar2;
        if (iVar2 < 0x1e) goto LAB_0051dbe9;
      }
      iVar2 = 0x1e;
    }
  }
LAB_0051dbe9:
  if ((param_1 < 0x22) || (0x27 < param_1)) {
    iVar5 = 0;
  }
  else {
    iVar5 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + -0x68 +
                    param_1 * 4);
    if (iVar5 < 1) {
      iVar5 = 3;
    }
  }
  if (iVar2 <= iVar5) {
    if ((param_1 < 0x22) || (0x27 < param_1)) {
      iVar2 = 0;
    }
    else {
      iVar2 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + -0x68
                      + param_1 * 4);
      if (iVar2 < 1) {
        iVar2 = 3;
      }
    }
  }
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(param_1);
  iVar5 = 0;
  iVar6 = iVar2 - CONCAT31(extraout_var_01,uVar1);
  iVar7 = 0;
  do {
    uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar7 + 0x22);
    iVar5 = iVar5 + CONCAT31(extraout_var_02,uVar1);
    iVar7 = iVar7 + 1;
  } while (iVar7 < 6);
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_ESI);
  iVar3 = CONCAT31(extraout_var_03,uVar1);
  iVar7 = iVar3 * 2 + 0x52;
  if (0xf < iVar3) {
    iVar7 = iVar7 + (0xf - iVar3);
  }
  if (iVar5 != iVar7) {
    iVar6 = iVar6 + (iVar5 - iVar7);
    iVar2 = iVar6;
  }
  if (iVar6 != 0) {
    iStack00000010 = in_stack_0000000c + -0x21;
    if (5 < iStack00000010) {
      iStack00000010 = 0;
    }
    do {
      if (iStack00000010 == in_stack_0000000c + -0x22) break;
      iVar5 = iStack00000010 + 0x22;
      if ((iVar5 < 0x22) || (0x27 < iVar5)) {
        iVar6 = 0;
      }
      else {
        iVar6 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) +
                         -0x68 + iVar5 * 4);
        if (iVar6 < 1) {
          iVar6 = 3;
        }
      }
      if ((iVar5 < 0x22) || (0x27 < iVar5)) {
        iVar7 = 0;
      }
      else {
        iVar3 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) +
                         -0x68 + iVar5 * 4);
        if (iVar3 < 0) {
          uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EBP);
          iVar4 = CONCAT31(extraout_var_04,uVar1);
          iVar7 = iVar4 * 2 + 0x52;
          if (0xf < iVar4) {
            iVar7 = iVar7 + (0xf - iVar4);
          }
          iVar7 = (-0x54 - iVar3) + iVar7;
          if (iVar7 < 0x1e) goto LAB_0051dd53;
        }
        iVar7 = 0x1e;
      }
LAB_0051dd53:
      uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar5);
      iVar5 = CONCAT31(extraout_var_05,uVar1);
      iVar6 = iVar5 - iVar6;
      if ((iVar6 < 0) && (iVar6 = iVar7 - iVar5, iVar6 < 0)) {
        (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
        iVar6 = iStack_8;
      }
      else {
        if (iVar2 < iVar6) {
          iVar6 = iVar2;
        }
        if (iVar7 < iVar5 - iVar6) {
          iVar6 = iVar5 - iVar7;
        }
        iVar6 = iVar2 - iVar6;
        (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
        in_stack_00000008 = in_stack_00000008 + 1;
        iVar2 = iVar6;
        if (5 < in_stack_00000008) {
          in_stack_00000008 = 0;
        }
      }
    } while (iVar6 != 0);
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
  }
  return;
}



// Function at 0051ddf0

void cls_0x5b4f30_TPlayer::meth_0x51ddf0()

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int unaff_ESI;
  int iVar7;
  undefined4 unaff_EDI;
  int iVar2;
  undefined3 extraout_var_00;
  
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
  iVar2 = CONCAT31(extraout_var,uVar1);
  iVar4 = iVar2 * 2 + 0x52;
  if (0xf < iVar2) {
    iVar4 = iVar4 + (0xf - iVar2);
  }
  iVar7 = 0x20;
  iVar2 = 0x22;
  do {
    if ((iVar2 < 0x22) || (0x27 < iVar2)) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + iVar7
                      );
      if (iVar5 < 1) {
        iVar5 = 3;
      }
    }
    iVar4 = iVar4 - iVar5;
    iVar7 = iVar7 + 4;
    iVar5 = iVar2 + -0x21;
    iVar2 = iVar2 + 1;
  } while (iVar5 < 6);
  iVar4 = (iVar4 * 0x10000) / 6;
  uVar6 = iVar4 + 1;
  iVar2 = 0x22;
  do {
    if ((iVar2 < 0x22) || (0x27 < iVar2)) {
      iVar7 = 0;
    }
    else {
      iVar7 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + 0x20)
      ;
      if (iVar7 < 1) {
        iVar7 = 3;
      }
    }
    if ((iVar2 < 0x22) || (0x27 < iVar2)) {
      iVar5 = 0;
    }
    else {
      if (*(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + 0x20) < 0
         ) {
        uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_ESI);
        iVar3 = CONCAT31(extraout_var_00,uVar1);
        iVar5 = iVar3 * 2 + 0x52;
        if (0xf < iVar3) {
          iVar5 = iVar5 + (0xf - iVar3);
        }
        iVar5 = iVar4 + -0x53 + iVar5;
        if (iVar5 < 0x1e) goto LAB_0051df05;
      }
      iVar5 = 0x1e;
    }
LAB_0051df05:
    iVar5 = (((int)uVar6 >> 0x10) + iVar7) - iVar5;
    if (0 < iVar5) {
      uVar6 = uVar6 + iVar5 * -0x10000;
    }
    unaff_ESI = iVar2;
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
    uVar6 = uVar6 + (0x20 - (uVar6 & 0xffff0000));
    iVar7 = iVar2 + -0x21;
    iVar2 = iVar2 + 1;
    if (5 < iVar7) {
      return;
    }
  } while( true );
}



// Function at 0051e0a0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x5b4f30_TPlayer::meth_0x51e0a0_SavePlayerFile(uint *param_1)

{
  char cVar1;
  int iVar2;
  cls_0x5b4f30_TPlayer *pcVar3;
  uint *puVar4;
  FILE *pFVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  uint uVar8;
  undefined4 in_stack_00000008;
  char *pcVar9;
  uint uVar10;
  FILE *pFVar11;
  cls_0x5a5ff0 local_22c;
  undefined4 local_218;
  char local_214 [258];
  char acStack_112 [261];
  undefined local_d;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a123b;
  local_c = ExceptionList;
  local_218 = this;
  if ((param_1 == (uint *)0x0) || (*(char *)param_1 == '\0')) {
    pcVar9 = s__s_chr_005e2b20;
    ExceptionList = &local_c;
  }
  else {
    ExceptionList = &local_c;
    puVar4 = FUN_0058ade0(param_1,'.');
    if (puVar4 != (uint *)0x0) {
      _strncpy(acStack_112 + 2,(char *)param_1,0x103);
      local_d = 0;
      goto LAB_0051e12b;
    }
    pcVar9 = s__s_chr_005e2b18;
  }
  FUN_0058b100(acStack_112 + 2,(byte *)pcVar9);
LAB_0051e12b:
  FUN_00483120((char *)&DAT_0065da00,local_214,0x104);
  uVar8 = 0xffffffff;
  pcVar9 = local_214;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  if (local_214[~uVar8 - 2] == '\\') {
    local_214[~uVar8 - 2] = '\0';
  }
  CreateDirectoryA(local_214,(LPSECURITY_ATTRIBUTES)0x0);
  uVar8 = 0xffffffff;
  pcVar9 = local_214;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar8 - 1);
  _strncpy(local_214 + (~uVar8 - 1),s__Chars_005e2b28,iVar2 + 0x103);
  (local_214 + (~uVar8 - 1))[iVar2 + 0x103] = '\0';
  CreateDirectoryA(local_214,(LPSECURITY_ATTRIBUTES)0x0);
  uVar8 = 0xffffffff;
  pcVar9 = local_214;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar8 - 1);
  _strncpy(local_214 + (~uVar8 - 1),&DAT_005e2b30,iVar2 + 0x103);
  (local_214 + (~uVar8 - 1))[iVar2 + 0x103] = '\0';
  uVar8 = 0xffffffff;
  pcVar9 = local_214;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  iVar2 = -(~uVar8 - 1);
  _strncpy(local_214 + (~uVar8 - 1),acStack_112 + 2,iVar2 + 0x103);
  (local_214 + (~uVar8 - 1))[iVar2 + 0x103] = '\0';
  cls_0x5a5ff0::cls_0x5a5ff0(&local_22c,0x8000);
  local_4 = 0;
  pFVar5 = (FILE *)FUN_004831c0(local_214,&DAT_005e2b34);
  if (pFVar5 == (FILE *)0x0) {
    local_4 = 0xffffffff;
    cls_0x5a5ff0::~cls_0x5a5ff0(&local_22c);
    uVar6 = 0;
  }
  else {
    if ((int)((local_22c.mbr_0x4 - local_22c.mbr_0x8) + local_22c.mbr_0xc) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(&local_22c);
    }
    *(undefined4 *)local_22c.mbr_0x8 = 0x52594c50;
    local_22c.mbr_0x8 = local_22c.mbr_0x8 + 4;
    if ((int)((local_22c.mbr_0x4 - local_22c.mbr_0x8) + local_22c.mbr_0xc) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(&local_22c);
    }
    pcVar3 = local_218;
    _DAT_0065a250 = in_stack_00000008;
    *(undefined4 *)local_22c.mbr_0x8 = 0xf;
    local_22c.mbr_0x8 = local_22c.mbr_0x8 + 4;
    uVar6 = *(undefined4 *)&(local_218->TPlayScreen).TScreen.field_0x40;
    uVar8 = FUN_0044ce30();
    *(uint *)&(pcVar3->TPlayScreen).TScreen.field_0x40 = uVar8;
    FUN_00472110((int *)pcVar3,&local_22c);
    *(undefined4 *)&(pcVar3->TPlayScreen).TScreen.field_0x40 = uVar6;
    uVar8 = local_22c.mbr_0x8 - local_22c.mbr_0x4;
    uVar10 = 1;
    _DAT_0065a250 = 0;
    pFVar11 = pFVar5;
    puVar7 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_22c);
    uVar8 = FUN_0058beb8(puVar7,uVar8,uVar10,&pFVar11->_ptr);
    if (uVar8 == 0) {
      local_4 = 0xffffffff;
      cls_0x5a5ff0::~cls_0x5a5ff0(&local_22c);
      uVar6 = 0;
    }
    else {
      FUN_0058b4f1(pFVar5);
      local_4 = 0xffffffff;
      cls_0x5a5ff0::~cls_0x5a5ff0(&local_22c);
      uVar6 = 1;
    }
  }
  ExceptionList = local_c;
  return uVar6;
}



// Function at 0051e420

void cls_0x5b4f30_TPlayer::meth_0x51e420()

{
  int *piVar1;
  
  piVar1 = (int *)meth_0x4701f0(this);
  if (piVar1 != (int *)0x0) {
    if ((*(short *)(piVar1 + 1) != 1) && (*(short *)(piVar1 + 1) != 2)) {
      (**(code **)(*piVar1 + 0xbc))(this,0xffffffff);
      DAT_0065b02c._76_4_ = 1;
      return;
    }
    meth_0x519ea0(this);
    DAT_0065b02c._76_4_ = 1;
  }
  return;
}



// Function at 0051e4d0

void cls_0x5b4f30_TPlayer::meth_0x51e4d0()

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  byte *pbVar4;
  byte *pbVar5;
  bool bVar6;
  int *in_stack_00000004;
  
  if (in_stack_00000004 != (int *)0x0) {
    pbVar4 = &(this->TPlayScreen).mbr_0x494;
    piVar2 = in_stack_00000004 + 1;
    do {
      bVar1 = *(byte *)piVar2;
      bVar6 = bVar1 < *pbVar4;
      if (bVar1 != *pbVar4) {
LAB_0051e513:
        iVar3 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
        goto LAB_0051e518;
      }
      if (bVar1 == 0) break;
      bVar1 = *(byte *)((int)piVar2 + 1);
      bVar6 = bVar1 < pbVar4[1];
      if (bVar1 != pbVar4[1]) goto LAB_0051e513;
      piVar2 = (int *)((int)piVar2 + 2);
      pbVar4 = pbVar4 + 2;
    } while (bVar1 != 0);
    iVar3 = 0;
LAB_0051e518:
    if (iVar3 == 0) {
      pbVar5 = &(this->TPlayScreen).field_0x4c6;
      pbVar4 = (byte *)((int)in_stack_00000004 + 0x36);
      do {
        bVar1 = *pbVar4;
        bVar6 = bVar1 < *pbVar5;
        if (bVar1 != *pbVar5) {
LAB_0051e54d:
          iVar3 = (1 - (uint)bVar6) - (uint)(bVar6 != 0);
          goto LAB_0051e552;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar4[1];
        bVar6 = bVar1 < pbVar5[1];
        if (bVar1 != pbVar5[1]) goto LAB_0051e54d;
        pbVar4 = pbVar4 + 2;
        pbVar5 = pbVar5 + 2;
      } while (bVar1 != 0);
      iVar3 = 0;
LAB_0051e552:
      if ((((iVar3 == 0) && (*in_stack_00000004 == *(int *)&(this->TPlayScreen).field_0x490)) &&
          (in_stack_00000004[0x12] == *(int *)&(this->TPlayScreen).field_0x4d8)) &&
         (in_stack_00000004[0x13] == *(int *)&(this->TPlayScreen).field_0x4dc)) {
        return;
      }
    }
    piVar2 = (int *)&(this->TPlayScreen).field_0x490;
    for (iVar3 = 0x18; iVar3 != 0; iVar3 = iVar3 + -1) {
      *piVar2 = *in_stack_00000004;
      in_stack_00000004 = in_stack_00000004 + 1;
      piVar2 = piVar2 + 1;
    }
    cls_0x57d9d0::meth_0x584960((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this);
  }
  return;
}



// Function at 0051e5a0

void cls_0x5b4f30_TPlayer::meth_0x51e5a0()

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)&(this->TPlayScreen).field_0x490;
  for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  cls_0x57d9d0::meth_0x584960((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this);
  return;
}



// Function at 0051e5f0

void cls_0x5b4f30_TPlayer::meth_0x51e5f0()

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int3 extraout_var_01;
  undefined3 extraout_var_02;
  int iVar4;
  undefined3 extraout_var_03;
  int iVar5;
  int unaff_ESI;
  undefined4 unaff_EDI;
  int iVar6;
  int in_stack_00000008;
  
  uVar2 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
  iVar5 = 0;
  iVar6 = 0;
  do {
    uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar6 + 0x28);
    iVar5 = iVar5 + CONCAT31(extraout_var_00,uVar3);
    iVar6 = iVar6 + 1;
  } while (iVar6 < 0xb);
  iVar5 = (((CONCAT31(extraout_var,uVar2) * 0x1e) / 0x1e) * 0x2cb) / 100 - iVar5;
  if ((iVar5 < 0) || ((0 < iVar5 && (in_stack_00000008 != 0)))) {
    while (iVar5 != 0) {
      iVar6 = FUN_00483300_RandomRange(0,10);
      iVar6 = iVar6 + 0x28;
      uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar6);
      if (iVar5 < 0) {
        if (CONCAT31(extraout_var_01,uVar2) != 0 && -1 < extraout_var_01) {
          (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
          iVar5 = iVar5 + 1;
          unaff_ESI = iVar6;
        }
      }
      else {
        uVar3 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_ESI);
        iVar4 = (CONCAT31(extraout_var_02,uVar3) * 0x1e) / 0x1e + 5;
        if (0x1d < iVar4) {
          iVar4 = 0x1e;
        }
        if (CONCAT31(extraout_var_01,uVar2) < iVar4) {
          (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
          iVar5 = iVar5 + -1;
          unaff_ESI = iVar6;
        }
      }
    }
  }
  iVar5 = 0x33;
  do {
    uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar5 + -0xb);
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar5);
    iVar6 = cls_0x48cc90::meth_0x48ccb0((cls_0x48cc90 *)&UNK_0065d6a6.field_0x102);
    if (CONCAT31(extraout_var_03,uVar2) != iVar6) {
      pcVar1 = (this->TPlayScreen).TScreen.vftptr_0x0;
      cls_0x48cc90::meth_0x48cc90((cls_0x48cc90 *)&UNK_0065d6a6.field_0x102);
      (*pcVar1->virt_meth_0x477b00_232)(this);
    }
    iVar6 = iVar5 + -0x32;
    iVar5 = iVar5 + 1;
  } while (iVar6 < 0xb);
  return;
}



// Function at 0051e740

void cls_0x5b4f30_TPlayer::meth_0x51e740()

{
  TScreen__vftable_5a50e8 *pcVar1;
  undefined uVar2;
  undefined uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int3 extraout_var_02;
  undefined3 extraout_var_03;
  int iVar4;
  int iVar5;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  int iVar6;
  uint in_stack_00000004;
  int in_stack_00000008;
  int in_stack_0000000c;
  
  if (in_stack_00000004 < 0xb) {
    uVar2 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
    iVar4 = (CONCAT31(extraout_var,uVar2) * 0x1e) / 0x1e + 5;
    if (0x1d < iVar4) {
      iVar4 = 0x1e;
    }
    if (iVar4 < in_stack_0000000c) {
      (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_ESI);
    }
    iVar4 = in_stack_00000004 + 0x28;
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
    uVar2 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(iVar4);
    iVar4 = 0;
    iVar6 = 0;
    do {
      uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar6 + 0x28);
      iVar4 = iVar4 + CONCAT31(extraout_var_01,uVar3);
      iVar6 = iVar6 + 1;
    } while (iVar6 < 0xb);
    iVar4 = (((CONCAT31(extraout_var_00,uVar2) * 0x1e) / 0x1e) * 0x2cb) / 100 - iVar4;
    if (iVar4 < 0) {
      in_stack_00000008 = in_stack_00000008 + 1;
      if (10 < in_stack_00000008) {
        in_stack_00000008 = 0;
      }
      do {
        uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)
                          (in_stack_00000008 + 0x28);
        iVar6 = CONCAT31(extraout_var_02,uVar2);
        if (iVar6 != 0 && -1 < extraout_var_02) {
          iVar5 = -iVar4;
          if (iVar6 <= -iVar4) {
            iVar5 = iVar6;
          }
          (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477b00_232)(this);
          iVar4 = iVar4 + iVar5;
        }
        in_stack_00000008 = in_stack_00000008 + 1;
        if (10 < in_stack_00000008) {
          in_stack_00000008 = 0;
        }
      } while (iVar4 != 0);
    }
    iVar4 = 0x33;
    do {
      uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar4 + -0xb);
      (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(iVar4);
      iVar6 = cls_0x48cc90::meth_0x48ccb0((cls_0x48cc90 *)&UNK_0065d6a6.field_0x102);
      if (CONCAT31(extraout_var_03,uVar2) != iVar6) {
        pcVar1 = (this->TPlayScreen).TScreen.vftptr_0x0;
        cls_0x48cc90::meth_0x48cc90((cls_0x48cc90 *)&UNK_0065d6a6.field_0x102);
        (*pcVar1->virt_meth_0x477b00_232)(this);
      }
      iVar6 = iVar4 + -0x32;
      iVar4 = iVar4 + 1;
    } while (iVar6 < 0xb);
  }
  return;
}



// Function at 0051e8f0

void cls_0x5b4f30_TPlayer::meth_0x51e8f0()

{
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x46fde0_132)(this);
  return;
}



// Function at 0051e900

void cls_0x5b4f30_TPlayer::meth_0x51e900()

{
  undefined uVar1;
  undefined3 extraout_var;
  int in_stack_00000004;
  cls_0x5b4f30_TPlayer *this_00;
  
  cls_0x57d9d0::meth_0x583f60((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x49,in_stack_00000004,0)
  ;
  DAT_00676e5d._0_1_ = 1;
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x470280_168)(this);
  this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var,uVar1);
  if (in_stack_00000004 < 1) {
    if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      FUN_0046e6d0(this_00);
    }
  }
  else if (this_00 == (cls_0x5b4f30_TPlayer *)0x0) {
    (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x46f940_84)(this);
  }
  else {
    (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477dc0_412)(this_00);
  }
  if (this == DAT_00667fcc) {
    UNK_0065d53c._12_4_ = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
    DAT_0065b02c._76_4_ = 1;
  }
  DAT_00676e5d._0_1_ = 0;
  return;
}



// Function at 0051e9a0

int cls_0x5b4f30_TPlayer::meth_0x51e9a0()

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0;
  iVar5 = 0;
  iVar1 = meth_0x470090(this);
  if (iVar1 < 1) {
    return 0;
  }
  do {
    piVar2 = (int *)meth_0x470160(this);
    if (piVar2 != (int *)0x0) {
      if (*(short *)(piVar2 + 1) == 8) {
        iVar3 = (**(code **)(*piVar2 + 0x198))();
        iVar6 = iVar6 + iVar3;
      }
      else {
        iVar3 = (**(code **)(*piVar2 + 0x198))();
        if (iVar3 < 2) {
          iVar3 = 1;
        }
        else {
          iVar3 = (**(code **)(*piVar2 + 0x198))();
        }
        iVar4 = (**(code **)(*piVar2 + 0xd4))(s_value_005e2b54);
        iVar6 = iVar6 + iVar4 * iVar3;
      }
    }
    iVar5 = iVar5 + 1;
  } while (iVar5 < iVar1);
  return iVar6;
}



// Function at 0051ea40

void cls_0x5b4f30_TPlayer::meth_0x51ea40()

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined3 extraout_var_00;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined4 unaff_EDI;
  int in_stack_00000004;
  undefined *puVar8;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  
  DAT_00676e5d._0_1_ = 1;
  uVar1 = (*(this->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)(unaff_EDI);
  iVar2 = CONCAT31(extraout_var,uVar1) + -1;
  if (0x1c < iVar2) {
    iVar2 = 0x1d;
  }
  iVar2 = *(int *)(&UNK_0065d898.field_0xf0 + iVar2 * 4);
  puVar8 = &DAT_005e2b3c;
  uVar1 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x46fde0_132)(this);
  iStack_10 = 0;
  iStack_14 = 0;
  iVar3 = meth_0x470090(this);
  if (0 < iVar3) {
    do {
      piVar4 = (int *)meth_0x470160(this);
      if (piVar4 != (int *)0x0) {
        if (*(short *)(piVar4 + 1) == 8) {
          iVar5 = (**(code **)(*piVar4 + 0x198))();
          iStack_10 = iStack_10 + iVar5;
        }
        else {
          iVar2 = (**(code **)(*piVar4 + 0x198))(puVar8);
          if (iVar2 < 2) {
            iVar2 = 1;
          }
          else {
            iVar2 = (**(code **)(*piVar4 + 0x198))();
          }
          iVar5 = (**(code **)(*piVar4 + 0xd4))(s_value_005e2b54);
          iStack_10 = iStack_10 + iVar5 * iVar2;
          iVar2 = iStack_18;
        }
      }
      iStack_14 = iStack_14 + 1;
    } while (iStack_14 < iVar3);
  }
  iStack_10 = iStack_10 - CONCAT31(extraout_var_00,uVar1);
  if (iVar2 < iStack_10) {
    do {
      iVar3 = 0;
      piVar4 = (int *)0x0;
      iStack_14 = 0;
      iVar5 = meth_0x470090(this);
      iVar2 = iStack_18;
      if (iVar5 < 1) break;
      do {
        piVar6 = (int *)meth_0x470160(this);
        if ((piVar6 != (int *)0x0) && (*(short *)(piVar6 + 1) != 8)) {
          iVar2 = (**(code **)(*piVar6 + 0x198))();
          if (iVar2 < 2) {
            iVar2 = 1;
          }
          else {
            iVar2 = (**(code **)(*piVar6 + 0x198))();
          }
          iVar7 = (**(code **)(*piVar6 + 0xd4))(s_value_005e2b5c);
          if (iVar3 <= iVar7 * iVar2) {
            iVar3 = iVar7 * iVar2;
            piVar4 = piVar6;
          }
        }
        iStack_14 = iStack_14 + 1;
      } while (iStack_14 < iVar5);
      iVar2 = iStack_18;
      if (piVar4 == (int *)0x0) break;
      (**(code **)(*piVar4 + 0x60))();
      (**(code **)*piVar4)(1);
      iStack_10 = iStack_10 - iVar3;
    } while (iStack_18 < iStack_10);
  }
  iStack_10 = CONCAT31(extraout_var_00,uVar1) + iStack_10;
  if ((iVar2 < iStack_10) || ((in_stack_00000004 != 0 && (iStack_10 < iVar2)))) {
    meth_0x51e900(this);
  }
  DAT_00676e5d._0_1_ = 0;
  return;
}



// Function at 0051ec20

void cls_0x5b4f30_TPlayer::virt_meth_0x51ec20_RemoveItem(int param_1)

{
  dword dVar1;
  dword *pdVar2;
  int iVar3;
  dword in_stack_00000008;
  
  TPlayScreen::virt_meth_0x4c36c0(&this->TPlayScreen,param_1);
  pdVar2 = &(this->TPlayScreen).mbr_0x2a0;
  iVar3 = 0xb;
  do {
    if (param_1 == 1) {
      if (*pdVar2 == in_stack_00000008) {
LAB_0051ec64:
        *pdVar2 = 0;
      }
    }
    else if ((((param_1 == 2) && (dVar1 = *pdVar2, dVar1 != 0)) &&
             (*(dword *)(dVar1 + 0x44) == in_stack_00000008)) &&
            ((*(uint *)(dVar1 + 8) & 0x80000) == 0)) goto LAB_0051ec64;
    pdVar2 = pdVar2 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      return;
    }
  } while( true );
}



// Function at 0051ff00

cls_0x5b4f30_TPlayer * cls_0x5b4f30_TPlayer::virt_meth_0x51ff00_dtor()

{
  byte in_stack_00000004;
  
  ~cls_0x5b4f30_TPlayer(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00520630

int cls_0x5b4f30_TPlayer::virt_meth_0x520630_MaxHealth()

{
  int iVar1;
  undefined uVar2;
  undefined uVar3;
  int iVar4;
  int iVar5;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  
  iVar4 = cls_0x4741b0::meth_0x474210
                    ((cls_0x4741b0 *)((this->TPlayScreen).TScreen.mbr_0x48 + 0x1c));
  if (iVar4 < (short)(this->TPlayScreen).mbr_0x34c) {
    iVar4 = *(int *)((this->TPlayScreen).mbr_0x350 + 4 + iVar4 * 8);
  }
  else {
    iVar4 = 0;
  }
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x23);
  iVar5 = FUN_0048cc20(0x65d7a8,1);
  iVar1 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + 100);
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x18);
  iVar4 = DAT_0065d810 * iVar4;
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x1b);
  return ((CONCAT31(extraout_var,uVar2) + 0x4b + iVar4) *
         (CONCAT31(extraout_var_00,uVar3) + 100 + iVar5 + iVar1)) / 100;
}



// Function at 005206d0

int cls_0x5b4f30_TPlayer::virt_meth_0x5206d0_MaxFatigue()

{
  int iVar1;
  undefined uVar2;
  undefined uVar3;
  int iVar4;
  int iVar5;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  
  iVar4 = cls_0x4741b0::meth_0x474210
                    ((cls_0x4741b0 *)((this->TPlayScreen).TScreen.mbr_0x48 + 0x1c));
  if (iVar4 < (short)(this->TPlayScreen).mbr_0x34c) {
    iVar4 = *(int *)((this->TPlayScreen).mbr_0x350 + 4 + iVar4 * 8);
  }
  else {
    iVar4 = 0;
  }
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x23);
  iVar5 = FUN_0048cc20(0x65d7a8,1);
  iVar1 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + 0x68);
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x1a);
  iVar4 = DAT_0065d814 * iVar4;
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x1d);
  return ((CONCAT31(extraout_var,uVar2) + 0x4b + iVar4) *
         (CONCAT31(extraout_var_00,uVar3) + 200 + iVar1 + iVar5 * 2)) / 200;
}



// Function at 00520770

int cls_0x5b4f30_TPlayer::virt_meth_0x520770_MaxMana()

{
  int iVar1;
  undefined uVar2;
  undefined uVar3;
  int iVar4;
  int iVar5;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  
  iVar4 = cls_0x4741b0::meth_0x474210
                    ((cls_0x4741b0 *)((this->TPlayScreen).TScreen.mbr_0x48 + 0x1c));
  if (iVar4 < (short)(this->TPlayScreen).mbr_0x34c) {
    iVar4 = *(int *)((this->TPlayScreen).mbr_0x350 + 4 + iVar4 * 8);
  }
  else {
    iVar4 = 0;
  }
  (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x26);
  iVar5 = FUN_0048cc20(0x65d7a8,4);
  iVar1 = *(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) + 0x6c);
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x19);
  iVar4 = DAT_0065d818 * iVar4;
  uVar3 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x1c);
  return ((CONCAT31(extraout_var,uVar2) + 0x4b + iVar4) *
         (CONCAT31(extraout_var_00,uVar3) + 100 + iVar5 + iVar1)) / 100;
}



// Function at 00520860

int cls_0x5b4f30_TPlayer::virt_meth_0x520860_StealthMod()

{
  int *piVar1;
  undefined uVar2;
  int iVar3;
  int iVar4;
  undefined3 extraout_var;
  
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2a8;
  if (piVar1 == (int *)0x0) {
    iVar3 = 0;
  }
  else {
    iVar3 = (**(code **)(*piVar1 + 0xd4))(s_Stealth_005e2be8);
  }
  piVar1 = *(int **)&(this->TPlayScreen).field_0x2c8;
  if (piVar1 == (int *)0x0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (**(code **)(*piVar1 + 0xd4))(s_Stealth_005e2be8);
  }
  uVar2 = (*((this->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x31);
  return iVar4 + (*(int *)(*(int *)(*(int *)&(this->TPlayScreen).TScreen.field_0xfc + 400) +
                          0x5c) + CONCAT31(extraout_var,uVar2)) * 10 + iVar3;
}



