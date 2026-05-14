#include "TDialogPane.h"

// Decompiled methods and structure for class: TDialogPane



// Function at 00570900

void TDialogPane::meth_0x570900()

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  TDialogPane *pcVar4;
  TLightningAnimator *this_00;
  dword dVar5;
  cls_0x5b4f30_TPlayer *this_01;
  byte *pbVar6;
  dword *pdVar7;
  TDialogPane *local_90 [32];
  void *pvStack_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a22f0;
  pvStack_c = ExceptionList;
  local_4 = 0;
  this_01 = DAT_00667fcc;
  ExceptionList = &pvStack_c;
  local_90[0] = this;
  if (this->character == 0) {
    ExceptionList = &pvStack_c;
    if ((int)this->grabslot < 0) goto LAB_005709b0;
    ExceptionList = &pvStack_c;
    this_01 = (cls_0x5b4f30_TPlayer *)
              cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,this->grabslot);
  }
  if ((this_01 != (cls_0x5b4f30_TPlayer *)0x0) && (this_01 != DAT_00667fcc)) {
    cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(this_01);
    (*((this_01->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477650_384)(this_01);
    pbVar6 = (byte *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0058b100((char *)local_90,pbVar6);
    cls_0x5701f0::meth_0x5701f0((cls_0x5701f0 *)&UNK_00676268.field_0x400,0xffffffff);
  }
LAB_005709b0:
  while ((*(int *)this->dialogdata != 0 &&
         (piVar1 = (int *)((int *)this->dialogdata)[2], piVar1 != (int *)0x0))) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    FUN_004830f0(piVar1);
  }
  pdVar7 = &DAT_006766d4;
  dVar5 = DAT_006766d4;
  while (dVar5 != 0) {
    pcVar4 = (TDialogPane *)*pdVar7;
    if (pcVar4 == this) {
      *pdVar7 = this->numchoices;
      break;
    }
    if (pcVar4 != (TDialogPane *)0x0) {
      pdVar7 = &pcVar4->mbr_0xc;
    }
    dVar5 = *pdVar7;
  }
  pdVar7 = &DAT_006766ec;
  dVar5 = DAT_006766ec;
  do {
    if (dVar5 == 0) {
LAB_00570a2b:
      this_00 = (TLightningAnimator *)this->freshresponse;
      if (this_00 != (TLightningAnimator *)0x0) {
        TLightningAnimator::meth_0x588480(this_00);
        FUN_004830f0(this_00);
      }
      piVar1 = (int *)this->dialogdata;
      iVar2 = *piVar1;
      while (iVar2 != 0) {
        iVar2 = *piVar1;
        if (iVar2 != 0) {
          piVar3 = (int *)piVar1[1];
          *piVar1 = 0;
          *(int **)(iVar2 + 4) = piVar3;
          *piVar3 = iVar2;
        }
        iVar2 = *piVar1;
      }
      ExceptionList = pvStack_10;
      return;
    }
    pcVar4 = (TDialogPane *)*pdVar7;
    if (pcVar4 == this) {
      *pdVar7 = this->choice;
      goto LAB_00570a2b;
    }
    if (pcVar4 != (TDialogPane *)0x0) {
      pdVar7 = &pcVar4->mbr_0x10;
    }
    dVar5 = *pdVar7;
  } while( true );
}



// Function at 00570c00

undefined4 TDialogPane::meth_0x570c00()

{
  undefined4 uVar1;
  
  if (this->character != 0) {
    return DAT_00667fcc;
  }
  if (-1 < (int)this->grabslot) {
    uVar1 = cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,this->grabslot);
    return uVar1;
  }
  return 0;
}



// Function at 005722e0

void TDialogPane::meth_0x5722e0()

{
  int **ppiVar1;
  dword dVar2;
  DWORD DVar3;
  dword *pdVar4;
  int *piVar5;
  char in_stack_00000004;
  char in_stack_00000008;
  int *in_stack_0000000c;
  int in_stack_00000010;
  
  DVar3 = GetTickCount();
  piVar5 = (int *)(DVar3 + in_stack_00000010);
  if (*(int *)this->dialogdata != 0) {
    for (ppiVar1 = (int **)((int *)this->dialogdata)[2]; ppiVar1 != (int **)0x0;
        ppiVar1 = (int **)(*ppiVar1)[2]) {
      if (((*(char *)(ppiVar1 + 3) == in_stack_00000004) &&
          (*(char *)((int)ppiVar1 + 0xd) == in_stack_00000008)) && (ppiVar1[4] == in_stack_0000000c)
         ) {
        if (piVar5 <= ppiVar1[5]) {
          return;
        }
        ppiVar1[5] = piVar5;
        return;
      }
      if (**ppiVar1 == 0) break;
    }
  }
  pdVar4 = FUN_00482fb0(0x18);
  if (pdVar4 == (dword *)0x0) {
    pdVar4 = (dword *)0x0;
  }
  else {
    pdVar4[1] = 0;
    *pdVar4 = 0;
    pdVar4[2] = (dword)pdVar4;
    *(char *)(pdVar4 + 3) = in_stack_00000004;
    *(char *)((int)pdVar4 + 0xd) = in_stack_00000008;
    pdVar4[4] = (dword)in_stack_0000000c;
    pdVar4[5] = (dword)piVar5;
  }
  dVar2 = *pdVar4;
  if (dVar2 != 0) {
    *(dword *)(dVar2 + 4) = pdVar4[1];
    *(dword *)pdVar4[1] = dVar2;
    *pdVar4 = 0;
  }
  dVar2 = this->dialogdata;
  pdVar4[1] = (dword)this;
  *pdVar4 = dVar2;
  this->dialogdata = (dword)pdVar4;
  *(dword **)(dVar2 + 4) = pdVar4;
  return;
}



