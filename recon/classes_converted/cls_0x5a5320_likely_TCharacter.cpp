#include "classes_all.h"

// Decompiled methods and structure for class: TCharacter



// Function at 0044d5c0

undefined4 TCharacter::virt_meth_0x44d5c0()

{
  char cVar1;
  uint uVar2;
  cls_0x5a68d8 *pcVar3;
  int iVar4;
  undefined4 *puVar5;
  bool bVar6;
  undefined local_10;
  undefined local_f;
  undefined local_e;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cf96;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  meth_0x45aec0(this);
  this->virt_meth_0x491900();
  puVar5 = DAT_00668588;
  DAT_006663d8 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
  DAT_006663d4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
  DAT_00667c30 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
  DAT_0065c5c4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
  bVar6 = DAT_00668588 != (undefined4 *)0x0;
  this->mbr_0x8c = 0x80000000;
  this->mbr_0x94 = 0x80000000;
  this->mbr_0x88 = 0x80000000;
  this->mbr_0x90 = 0x80000000;
  (this->TObjectInstance).mbr_0x6c = 0x80000000;
  (this->TObjectInstance).mbr_0x68 = 0x80000000;
  (this->TObjectInstance).mbr_0x64 = 0x80000000;
  (this->TObjectInstance).mbr_0x60 = 0x80000000;
  this->mbr_0x7c = 0x80000000;
  this->mbr_0x84 = 0x80000000;
  this->mbr_0x78 = 0x80000000;
  this->mbr_0x80 = 0x80000000;
  this->mbr_0x9c = 0;
  this->mbr_0xa0 = 0;
  this->mbr_0xf4 = 1;
  iVar4 = DAT_00668580;
  if (bVar6) {
    for (; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
  }
  DAT_0066857c = 0;
  DAT_00668578 = 0;
  local_e = 0xff;
  local_f = 0xff;
  local_10 = 0xff;
  FUN_0041da10(0,(undefined4 *)&local_10,0x23);
  iVar4 = DAT_005d7a48;
  this->mbr_0x128 = 0xffffffff;
  this->mbr_0x8f0 = 0;
  this->mbr_0x8e8 = 0;
  uVar2 = iVar4 * 10 - 10;
  uVar2 = uVar2 & ((int)uVar2 < 0) - 1;
  if (this->mbr_0x8cc != uVar2) {
    this->mbr_0x8cc = uVar2;
    this->mbr_0x8d4 = 1;
  }
  cVar1 = *(char *)((int)&this->mbr_0x8d0 + 2);
  this->mbr_0x8f0 = 0;
  this->mbr_0x8e8 = 0;
  if (((cVar1 != -1) || (*(char *)((int)&this->mbr_0x8d0 + 1) != -1)) ||
     (*(char *)&this->mbr_0x8d0 != -1)) {
    this->mbr_0x8d4 = 1;
    this->mbr_0x8d0 = 0xffffff;
  }
  this->mbr_0x86d = 0;
  this->mbr_0x880 = 0xffffffff;
  this->mbr_0x87c = 0;
  this->mbr_0x878 = 0;
  this->mbr_0x874 = 0;
  FUN_00515830();
  FUN_0050c880();
  this->mbr_0x88c = 0;
  this->mbr_0x890 = 0;
  this->mbr_0x8bc = 0;
  this->mbr_0x8c0 = 0;
  this->mbr_0x8c4 = 0x40;
  this->mbr_0x8c8 = 0x40;
  this->mbr_0x894 = 0;
  this->mbr_0x120 = 0;
  this->mbr_0x11c = 0;
  this->mbr_0x12c = 0xffffffff;
  DAT_00658430 = FUN_004a1ec0(0x200,0x200,0x40004,0);
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
  local_4 = 0;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a3a40(pcVar3);
  }
  local_4 = 0xffffffff;
  DAT_00658428 = pcVar3;
  DAT_00658434 = FUN_004a1ec0(0x40,0x40,0x40004,0);
  pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
  local_4 = 2;
  if (pcVar3 == (cls_0x5a68d8 *)0x0) {
    pcVar3 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x5a68d8::cls_0x5a68d8(pcVar3);
    local_4 = CONCAT31(local_4._1_3_,3);
    pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a3a40(pcVar3);
  }
  DAT_00658444 = pcVar3;
  this->mbr_0x934 = 0;
  this->mbr_0x8f8 = 0;
  this->mbr_0x8fc = 0;
  this->mbr_0xc4 = 0;
  this->mbr_0x938 = 0;
  this->mbr_0xc0 = 0;
  this->mbr_0xbc = 0;
  this->mbr_0xac = 0;
  this->mbr_0x93c = 0;
  bVar6 = DAT_0065844c == 0;
  this->mbr_0xa8 = 0;
  this->mbr_0xa4 = 0;
  this->mbr_0xb8 = 0;
  this->mbr_0xb4 = 0;
  this->mbr_0xb0 = 0;
  this->mbr_0xa0 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  this->mbr_0x918 = 0;
  this->mbr_0x924 = 0;
  this->mbr_0x920 = 0;
  this->mbr_0x91c = 0;
  this->mbr_0x930 = 0;
  this->mbr_0x92c = 0;
  this->mbr_0x928 = 0;
  this->mbr_0x940 = 0xc7c35000;
  this->mbr_0x944 = 0x2d;
  this->mbr_0x978 = 0;
  this->mbr_0x97c = 0;
  this->mbr_0x980 = 0;
  this->mbr_0x984 = 0;
  if (bVar6) {
    DAT_00658460 = 0;
    DAT_0065843c = 0;
    DAT_00658458 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
    DAT_006584a8 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
    DAT_006584b4 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_00658310 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_006584b8 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_006584bc = 0;
    DAT_00658324 = 0;
    DAT_006584c0 = 0;
    DAT_00658464 = FUN_0058bd47((LPSECURITY_ATTRIBUTES)0x0,0,0x4563b0,0,1,&DAT_00658454);
    DAT_0065844c = 1;
  }
  ExceptionList = local_c;
  return 1;
}



// Function at 0044d9c0

void TCharacter::virt_meth_0x44d9c0()

{
  LPCVOID *ppvVar1;
  HANDLE pvVar2;
  int iVar3;
  uint *puVar4;
  undefined4 *puVar5;
  code *pcVar6;
  cls_0x45f7a0 cStack_c;
  
  pcVar6 = ReleaseMutex_exref;
  if (DAT_0065844c != 0) {
    iVar3 = ReleaseMutex(DAT_006584b4);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(DAT_006584b4);
    }
    iVar3 = ReleaseMutex(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(DAT_00658310);
    }
    iVar3 = ReleaseMutex(DAT_006584b8);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(DAT_006584b8);
    }
    if (DAT_0065844c != 0) {
      if (DAT_00658324 == 0) {
        DAT_006584c0 = 0;
      }
      else {
        FUN_00482130();
        DAT_00658324 = 0;
        DAT_006584c0 = 1;
        FUN_00482140();
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          this->mbr_0x848 = this->mbr_0x838;
          this->mbr_0x84c = this->mbr_0x83c;
          this->mbr_0x850 = this->mbr_0x840;
          this->mbr_0x854 = this->mbr_0x844;
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
        }
      }
    }
    DAT_006584bc = 1;
    PulseEvent(DAT_00658458);
    WaitForSingleObject(DAT_00658464,0xffffffff);
    DAT_0065844c = 0;
  }
  meth_0x45b000(this);
  FUN_0050c8a0();
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,1);
  iVar3 = DAT_00668580;
  puVar5 = DAT_00668588;
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d0698;
    DAT_00658474 = 0x12ee;
    iVar3 = DAT_00668580;
    puVar5 = DAT_00668588;
  }
  while (DAT_00668580 = iVar3, DAT_00668588 = puVar5, DAT_00668578 != 0) {
    ppvVar1 = (LPCVOID *)*puVar5;
    if (ppvVar1 != (LPCVOID *)0x0) {
      cls_0x45f7a0::meth_0x45f7e0(&cStack_c);
      puVar4 = (uint *)cStack_c.mbr_0x0;
      for (; (puVar4 != (uint *)0x0 && (cStack_c.mbr_0x8 < *puVar4));
          cStack_c.mbr_0x8 = cStack_c.mbr_0x8 + 1) {
        if (*(int **)cStack_c.mbr_0x4 != (int *)0x0) {
          (**(code **)(**(int **)cStack_c.mbr_0x4 + 0x128))();
          puVar4 = (uint *)cStack_c.mbr_0x0;
        }
        cStack_c.mbr_0x4 = cStack_c.mbr_0x4 + 4;
      }
      FUN_00498460_TSector_CloseSector(ppvVar1,1);
      iVar3 = DAT_00668580;
      puVar5 = DAT_00668588;
    }
  }
  if (puVar5 != (undefined4 *)0x0) {
    for (; pcVar6 = ReleaseMutex_exref, iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
  }
  DAT_0066857c = 0;
  DAT_00668578 = 0;
  if (DAT_0065844c != 0) {
    (*pcVar6)(DAT_00658310);
    pvVar2 = DAT_00658310;
    iVar3 = (*pcVar6)(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = (*pcVar6)(pvVar2);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  virt_meth_0x491970(this);
  if (DAT_00658428 != (undefined4 *)0x0) {
    (**(code **)*DAT_00658428)(1);
    DAT_00658428 = (undefined4 *)0x0;
  }
  if (DAT_00658430 != (LPCVOID)0x0) {
    FUN_004830f0(DAT_00658430);
    DAT_00658430 = (LPCVOID)0x0;
  }
  if (DAT_00658444 != (undefined4 *)0x0) {
    (**(code **)*DAT_00658444)(1);
    DAT_00658444 = (undefined4 *)0x0;
  }
  if (DAT_00658434 != (LPCVOID)0x0) {
    FUN_004830f0(DAT_00658434);
    DAT_00658434 = (LPCVOID)0x0;
  }
  return;
}



// Function at 0044dca0

void TCharacter::virt_meth_0x44dca0()

{
  DAT_00667c30 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
  DAT_0065c5c4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
  DAT_006663d4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
  DAT_006663d8 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
  DAT_0065ba00 = DAT_00667c30 + 0x80;
  DAT_00667c50 = DAT_0065c5c4 + 0x80;
  cls_0x411eb0::meth_0x412150
            ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),DAT_006663d8,DAT_006663d4,DAT_00667c30);
  this->mbr_0x85c = 0;
  this->mbr_0x864 = 0;
  this->mbr_0x858 = 0;
  this->mbr_0x860 = 0;
  meth_0x45b080(this);
  if (DAT_0065844c == 0) {
    DAT_00658460 = 0;
    DAT_0065843c = 0;
    DAT_00658458 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
    DAT_006584a8 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
    DAT_006584b4 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_00658310 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_006584b8 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
    DAT_006584bc = 0;
    DAT_00658324 = 0;
    DAT_006584c0 = 0;
    DAT_00658464 = FUN_0058bd47((LPSECURITY_ATTRIBUTES)0x0,0,0x4563b0,0,1,&DAT_00658454);
    DAT_0065844c = 1;
  }
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x4903b0_40)(&this->TObjectInstance);
  return;
}



// Function at 0044e050

void TCharacter::meth_0x44e050()

{
  byte bVar1;
  HANDLE hMutex;
  int iVar2;
  uint uVar3;
  byte *pbVar4;
  byte *pbVar5;
  char *in_stack_00000004;
  byte local_30c [258];
  byte abStack_20a [260];
  byte abStack_106 [262];
  
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,1);
  FUN_00456710(s_d__revenant_MapPane_cpp_005d0698,0x12ee);
  while (DAT_00668578 != 0) {
    if ((LPCVOID *)*DAT_00668588 != (LPCVOID *)0x0) {
      FUN_00451ac0((LPCVOID *)*DAT_00668588);
    }
  }
  cls_0x45f7c0::meth_0x45f680((cls_0x45f7c0 *)&DAT_00668578);
  FUN_00456740();
  (this->TObjectInstance).mbr_0x6c = (this->TObjectInstance).mbr_0x6c + 10000000;
  (this->TObjectInstance).mbr_0x68 = (this->TObjectInstance).mbr_0x68 + 10000000;
  meth_0x4546a0(this);
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d029c;
    DAT_00658474 = 0x2d2;
  }
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,0);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar2 = ReleaseMutex(DAT_00658310);
    while (iVar2 != 0) {
      iVar2 = ReleaseMutex(hMutex);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  FUN_00483120(s_curmap_005d02b4,(char *)(abStack_20a + 2),0x104);
  uVar3 = 0xffffffff;
  pbVar5 = abStack_20a + 2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    bVar1 = *pbVar5;
    pbVar5 = pbVar5 + 1;
  } while (bVar1 != 0);
  iVar2 = -(~uVar3 - 1);
  _strncpy((char *)(abStack_20a + ~uVar3 + 1),s____DAT_005d02bc,iVar2 + 0x103);
  (abStack_20a + ~uVar3 + 1)[iVar2 + 0x103] = 0;
  FUN_004818b0(abStack_20a + 2);
  if (in_stack_00000004 != (char *)0x0) {
    FUN_00483120(in_stack_00000004,(char *)(abStack_106 + 2),0x104);
    iVar2 = -1;
    pbVar5 = abStack_106 + 2;
    do {
      pbVar4 = pbVar5;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pbVar4 = pbVar5 + 1;
      bVar1 = *pbVar5;
      pbVar5 = pbVar4;
    } while (bVar1 != 0);
    *(undefined4 *)(pbVar4 + -1) = DAT_005d0258;
    *(undefined2 *)(pbVar4 + 3) = DAT_005d025c;
    pbVar4[5] = DAT_005d025e;
    FUN_00483120(s_curmap_005d0260,(char *)local_30c,0x104);
    uVar3 = 0xffffffff;
    pbVar5 = local_30c;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      bVar1 = *pbVar5;
      pbVar5 = pbVar5 + 1;
    } while (bVar1 != 0);
    iVar2 = -(~uVar3 - 1);
    _strncpy((char *)(local_30c + (~uVar3 - 1)),&DAT_005d0268,iVar2 + 0x103);
    (local_30c + (~uVar3 - 1))[iVar2 + 0x103] = 0;
    FUN_004814d0(abStack_106 + 2,local_30c);
  }
  return;
}



// Function at 0044e460

void TCharacter::meth_0x44e460()

{
  byte bVar1;
  HANDLE pvVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  byte local_104 [258];
  byte abStack_2 [2];
  
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,1);
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d0698;
    DAT_00658474 = 0x12ee;
  }
  while (DAT_00668578 != 0) {
    if ((LPCVOID *)*DAT_00668588 != (LPCVOID *)0x0) {
      FUN_00451ac0((LPCVOID *)*DAT_00668588);
    }
  }
  cls_0x45f7c0::meth_0x45f680((cls_0x45f7c0 *)&DAT_00668578);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    pvVar2 = DAT_00658310;
    iVar3 = ReleaseMutex(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(pvVar2);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  (this->TObjectInstance).mbr_0x6c = (this->TObjectInstance).mbr_0x6c + 10000000;
  (this->TObjectInstance).mbr_0x68 = (this->TObjectInstance).mbr_0x68 + 10000000;
  meth_0x4546a0(this);
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d029c;
    DAT_00658474 = 0x2d2;
  }
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,0);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    pvVar2 = DAT_00658310;
    iVar3 = ReleaseMutex(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(pvVar2);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  FUN_00483120(s_curmap_005d02b4,(char *)local_104,0x104);
  uVar4 = 0xffffffff;
  pbVar5 = local_104;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    bVar1 = *pbVar5;
    pbVar5 = pbVar5 + 1;
  } while (bVar1 != 0);
  iVar3 = -(~uVar4 - 1);
  _strncpy((char *)(local_104 + (~uVar4 - 1)),s____DAT_005d02bc,iVar3 + 0x103);
  (local_104 + (~uVar4 - 1))[iVar3 + 0x103] = 0;
  FUN_004818b0(local_104);
  return;
}



// Function at 0044e5f0

void TCharacter::meth_0x44e5f0()

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int in_stack_00000004;
  int local_54;
  int local_50;
  int local_4c;
  cls_0x44ceb0 local_48;
  
  if (-1 < in_stack_00000004) {
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
    while ((TPlayScreen *)local_48.mbr_0xc != (TPlayScreen *)0x0) {
      if (*(int *)&((TScreen *)local_48.mbr_0xc)->field_0x40 == in_stack_00000004)
      goto LAB_0044e645;
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
    local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_0044e645:
    if (((TPlayScreen *)local_48.mbr_0xc != (TPlayScreen *)0x0) &&
       (iVar1 = TPlayScreen::meth_0x46e8a0((TPlayScreen *)local_48.mbr_0xc), iVar1 != 0)) {
      uVar3 = (int)((TScreen *)local_48.mbr_0xc)->mbr_0x10 >> 4 & 0x8000003f;
      if ((int)uVar3 < 0) {
        uVar3 = (uVar3 - 1 | 0xffffffc0) + 1;
      }
      uVar2 = (int)((TScreen *)local_48.mbr_0xc)->mbr_0x14 >> 4 & 0x8000003f;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xffffffc0) + 1;
      }
      TPlayScreen::meth_0x470f50((TPlayScreen *)local_48.mbr_0xc);
      this->mbr_0x8bc = uVar3 - in_stack_00000004;
      this->mbr_0x8c0 = uVar2 - local_54;
      this->mbr_0x8c4 = (uVar3 - in_stack_00000004) + local_50;
      this->mbr_0x8c8 = (uVar2 - local_54) + local_4c;
      meth_0x4546a0(this);
    }
  }
  return;
}



// Function at 0044e930

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x44e930(int param_1, int param_2, int param_3)

{
  int iVar1;
  dword dVar2;
  dword dVar3;
  bool bVar4;
  cls_0x499720 *this_00;
  int iVar5;
  uint uVar6;
  uint *in_stack_00000010;
  uint local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  dVar2 = this->mbr_0x7c;
  dVar3 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
  FUN_0046dad0(this->mbr_0x78 + param_1,dVar2 + param_2,(int *)&local_24,0);
  FUN_0046dad0(this->mbr_0x78 + param_1,dVar3 + 0xdc + dVar2,(int *)&local_30,0);
  local_1c = local_1c & 0xfffffff0;
  local_20 = local_20 & 0xfffffff0;
  local_24 = local_24 & 0xfffffff0;
  local_28 = local_28 & 0xfffffff0;
  local_2c = local_2c & 0xfffffff0;
  local_30 = local_30 & 0xfffffff0;
  bVar4 = false;
  if ((int)local_20 <= (int)local_2c) {
    do {
      if (bVar4) goto LAB_0044eb3d;
      iVar5 = 0;
      do {
        local_14 = local_2c;
        local_18 = local_30;
        if (iVar5 == 1) {
          local_2c = local_2c - 0x10;
        }
        else if (iVar5 == 2) {
          local_30 = local_30 - 0x10;
        }
        iVar1 = local_30 + 8;
        local_10 = local_28;
        this_00 = (cls_0x499720 *)FUN_00499e10_TSector_FindLoadedSector(_DAT_00666970,iVar1 >> 10,(int)(local_2c + 8) >> 10);
        if (this_00 == (cls_0x499720 *)0x0) {
          uVar6 = 0;
        }
        else {
          uVar6 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(this_00,iVar1 >> 4 & 0x3f);
        }
        FUN_0046dad0(this->mbr_0x78 + param_1,(int)(uVar6 * 0x362) / 1000 + param_2 + this->mbr_0x7c
                     ,(int *)&local_c,0);
        local_c = local_c + 8;
        local_8 = local_8 + 8;
        if ((((local_c ^ local_30) & 0xfffffff0) == 0) && (((local_8 ^ local_2c) & 0xfffffff0) == 0)
           ) {
          bVar4 = true;
          local_28 = uVar6;
          break;
        }
        local_30 = local_18;
        local_2c = local_14;
        local_28 = local_10;
        iVar5 = iVar5 + 1;
      } while (iVar5 < 3);
      local_2c = local_2c - 0x10;
      local_30 = local_30 - 0x10;
    } while ((int)local_20 <= (int)local_2c);
    if (bVar4) goto LAB_0044eb3d;
  }
  FUN_0046dad0(this->mbr_0x78 + param_1,param_2 + this->mbr_0x7c,(int *)&local_30,
               *(int *)(param_3 + 8));
LAB_0044eb3d:
  in_stack_00000010[2] = local_28;
  *in_stack_00000010 = local_30;
  in_stack_00000010[1] = local_2c;
  return;
}



// Function at 0044ed50

void TCharacter::meth_0x44ed50(int *param_1)

{
  int in_stack_00000008;
  
  FUN_0046dad0((this->mbr_0x78 - (this->TObjectInstance).cls_0x41c7f0.mbr_0x0) + DAT_00668510,
               (this->mbr_0x7c - (this->TObjectInstance).cls_0x41c7f0.mbr_0x4) + DAT_00668514,param_1,
               *(int *)(DAT_00667fcc + 0x18) + in_stack_00000008);
  return;
}



// Function at 0044ee00

void TCharacter::meth_0x44ee00()

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  char local_20 [32];
  
  if (DAT_00667fcc != 0) {
    local_2c = *(int *)(DAT_00667fcc + 0x10);
    local_28 = *(int *)(DAT_00667fcc + 0x14);
    local_24 = *(int *)(DAT_00667fcc + 0x18);
    FUN_0046dad0((this->mbr_0x78 - (this->TObjectInstance).cls_0x41c7f0.mbr_0x0) + DAT_00668510,
                 (this->mbr_0x7c - (this->TObjectInstance).cls_0x41c7f0.mbr_0x4) + DAT_00668514,
                 &local_38,*(int *)(DAT_00667fcc + 0x18) + 0x32);
    iVar2 = FUN_0046dc60_AngleToPP(&local_2c,&local_38);
    local_30 = local_30 - local_24;
    local_34 = local_34 - local_28;
    local_38 = local_38 - local_2c;
    iVar4 = local_38;
    if (local_38 < 1) {
      iVar4 = -local_38;
    }
    if (iVar4 < 0x10) {
      iVar4 = local_34;
      if (local_34 < 1) {
        iVar4 = -local_34;
      }
      if (iVar4 < 0x10) {
        iVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
        FUN_0043a020(iVar4);
        this->mbr_0x11c = 1;
        return;
      }
    }
    FUN_0058b100(local_20,(byte *)s_wedge__s_005d02f0);
    iVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
    FUN_0043a020(iVar4);
    iVar4 = -1;
    pcVar5 = local_20;
    do {
      pcVar6 = pcVar5;
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      pcVar6 = pcVar5 + 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar6;
    } while (cVar1 != '\0');
    uVar8 = 0x2b;
    uVar7 = 0;
    *(undefined4 *)(pcVar6 + -1) = s_shadow_005d02fc._0_4_;
    *(undefined2 *)(pcVar6 + 3) = s_shadow_005d02fc._4_2_;
    pcVar6[5] = s_shadow_005d02fc[6];
    uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
    FUN_0043a090(uVar3,uVar7,uVar8);
    FUN_0043a0b0(0,1);
    if (this->mbr_0x128 != 0xffffffff) {
      (**(code **)(*DAT_00667fd0 + 0x30))(this->mbr_0x128,0);
    }
    this->mbr_0x128 = 0xffffffff;
    switch(iVar2 + 0x10U & 0xe0) {
    case 0:
      this->mbr_0x128 = 0x21;
      break;
    case 0x20:
      this->mbr_0x128 = 0x27;
      break;
    case 0x40:
      this->mbr_0x128 = 0x22;
      break;
    case 0x60:
      this->mbr_0x128 = 0x28;
      break;
    case 0x80:
      this->mbr_0x128 = 0x23;
      break;
    case 0xa0:
      this->mbr_0x128 = 0x25;
      break;
    case 0xc0:
      this->mbr_0x128 = 0x24;
      break;
    case 0xe0:
      this->mbr_0x128 = 0x26;
    }
    if (this->mbr_0x128 != 0xffffffff) {
      (**(code **)(*DAT_00667fd0 + 0x30))(this->mbr_0x128,1);
    }
    this->mbr_0x11c = 1;
  }
  return;
}



// Function at 0044f140

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::virt_meth_0x44f140(int param_1, int param_2)

{
  dword *pdVar1;
  dword *pdVar2;
  short sVar3;
  undefined uVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  cls_0x499720 *pcVar8;
  int *piVar9;
  int3 extraout_var;
  undefined4 uVar10;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  char *pcVar11;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  uint uVar12;
  int iVar13;
  dword dVar14;
  float fVar15;
  float fVar16;
  TPlayScreen *this_00;
  float10 fVar17;
  unkbyte10 Var18;
  longlong lVar19;
  longlong lVar20;
  int in_stack_0000000c;
  float *pfVar21;
  cls_0x499720 *local_d4;
  float local_d0;
  float local_cc;
  dword local_c8;
  float local_c4;
  float local_c0;
  float fStack_bc;
  float fStack_b8;
  cls_0x499720 *pcStack_b4;
  float local_b0;
  float fStack_ac;
  float fStack_a8;
  cls_0x499720 *pcStack_a4;
  cls_0x499720 *local_a0;
  dword local_9c;
  dword local_98;
  cls_0x499720 *local_94;
  cls_0x41c750 local_90;
  dword local_84;
  cls_0x44ceb0 cStack_80;
  
  iVar5 = cls_0x5b93c4::virt_meth_0x4364d0
                    ((cls_0x5b93c4 *)&DAT_0065be50,
                     ((this->TObjectInstance).cls_0x41c7f0.mbr_0x0 - DAT_0065be54) + param_2);
  if (iVar5 != 0) {
    return;
  }
  if (DAT_00668154 != 0) {
    if (param_1 == 1) {
      if (param_2 < 0) {
        return;
      }
      if (in_stack_0000000c < 0) {
        return;
      }
      if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x8 <= param_2) {
        return;
      }
      if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0xc <= in_stack_0000000c) {
        return;
      }
      if (this->mbr_0x894 == 0) {
        if (0 < (int)DAT_00656f00) {
          if (DAT_00656f00 < 0xb) {
            iVar5 = 0;
          }
          else {
            iVar5 = DAT_00656f10[10];
          }
          local_d4 = (cls_0x499720 *)(*(uint *)(iVar5 + 0x14) >> 0x10 & 1);
          if (local_d4 != (cls_0x499720 *)0x0) {
            dVar6 = (this->TObjectInstance).mbr_0x6c;
            local_98 = dVar6 + 1;
            if ((int)(dVar6 - 1) <= (int)local_98) {
              local_a0 = (cls_0x499720 *)(local_98 * 0x40);
              do {
                dVar6 = (this->TObjectInstance).mbr_0x68;
                fVar16 = (float)(dVar6 + 1);
                if ((int)(dVar6 - 1) <= (int)fVar16) {
                  local_94 = (cls_0x499720 *)((int)fVar16 * 0x40);
                  do {
                    local_b0 = fVar16;
                    local_d4 = (cls_0x499720 *)FUN_00499e10_TSector_FindLoadedSector(this->mbr_0x9c,(int)fVar16,local_98);
                    if (local_d4 != (cls_0x499720 *)0x0) {
                      local_9c = this->mbr_0x8c8;
                      if (0x3f < (int)local_9c) {
                        local_9c = 0x40;
                      }
                      local_9c = local_9c - 1;
                      if ((int)this->mbr_0x8c0 <= (int)local_9c) {
                        do {
                          dVar6 = this->mbr_0x8c4;
                          if (0x3f < (int)dVar6) {
                            dVar6 = 0x40;
                          }
                          dVar6 = dVar6 - 1;
                          if ((int)this->mbr_0x8bc <= (int)dVar6) {
                            iVar5 = (int)&local_a0->mbr_0x0 + local_9c;
                            fVar15 = (float)(((int)local_94 + dVar6) * 0x10);
                            do {
                              local_d0 = fVar15;
                              local_cc = (float)(iVar5 * 0x10);
                              local_c8 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(local_d4,dVar6);
                              FUN_0046d7a0(&local_d0,&local_c0,&local_c4);
                              iVar7 = (int)(((int)local_c0 - param_2) - this->mbr_0x78) / 2;
                              iVar13 = ((int)local_c4 - this->mbr_0x7c) - in_stack_0000000c;
                              if (iVar7 < 1) {
                                iVar7 = -iVar7;
                              }
                              if (iVar13 < 1) {
                                iVar13 = -iVar13;
                              }
                              if (iVar13 + iVar7 < 8) {
                                this->mbr_0x104 = local_98;
                                this->mbr_0x10c = local_9c;
                                this->mbr_0x100 = (dword)local_b0;
                                this->mbr_0x110 = this->mbr_0x7c + in_stack_0000000c;
                                this->mbr_0xfc = local_c8;
                                this->mbr_0x108 = dVar6;
                                local_84 = (int)local_c4 + 0x14;
                                if ((this->TObjectInstance).mbr_0x50 != 0) {
                                  return;
                                }
                                dVar6 = this->mbr_0x130;
                                if (0x3f < (int)dVar6) {
                                  meth_0x4546a0(this);
                                  this->mbr_0x130 = 0;
                                  return;
                                }
                                pdVar1 = &this->mbr_0x134 + dVar6 * 7;
                                *pdVar1 = (int)local_c0 - 0x14;
                                pdVar1[1] = (int)local_c4 - 0x14;
                                pdVar1[2] = (int)local_c0 + 0x14;
                                pdVar1[3] = local_84;
                                (&this->mbr_0x144)[dVar6 * 7] = 3;
                                this->mbr_0x130 = this->mbr_0x130 + 1;
                                return;
                              }
                              dVar6 = dVar6 - 1;
                              fVar15 = (float)((int)fVar15 + -0x10);
                              fVar16 = local_b0;
                            } while ((int)this->mbr_0x8bc <= (int)dVar6);
                          }
                          local_9c = local_9c - 1;
                        } while ((int)this->mbr_0x8c0 <= (int)local_9c);
                      }
                    }
                    fVar16 = (float)((int)fVar16 - 1);
                    local_94 = local_94 + -0x10;
                  } while ((int)((this->TObjectInstance).mbr_0x68 - 1) <= (int)fVar16);
                }
                local_98 = local_98 - 1;
                local_a0 = local_a0 + -0x10;
              } while ((int)((this->TObjectInstance).mbr_0x6c - 1) <= (int)local_98);
            }
            goto LAB_0044f78e;
          }
        }
        iVar5 = meth_0x452520(this,param_2);
        if (iVar5 == 0) {
          return;
        }
        local_d0 = *(float *)(iVar5 + 0x10);
        local_cc = *(float *)(iVar5 + 0x14);
        local_c8 = *(dword *)(iVar5 + 0x18);
        pdVar1 = &this->mbr_0x10c;
        pdVar2 = &this->mbr_0x108;
        FUN_0046d7a0(&local_d0,pdVar2,pdVar1);
        dVar6 = this->mbr_0x78 - *pdVar2;
        this->mbr_0x100 = dVar6;
        dVar14 = this->mbr_0x7c - *pdVar1;
        *pdVar2 = dVar6 + param_2;
        this->mbr_0x104 = dVar14;
        uVar10 = *(undefined4 *)(iVar5 + 0x40);
        *pdVar1 = dVar14 + in_stack_0000000c;
        this->mbr_0x110 = local_c8;
        cls_0x4405d0::meth_0x4405d0((cls_0x4405d0 *)&DAT_00656e78,uVar10);
        return;
      }
    }
    else {
      if (param_1 == 3) {
        if (param_2 < 0) {
          return;
        }
        if (in_stack_0000000c < 0) {
          return;
        }
        if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x8 <= param_2) {
          return;
        }
        if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0xc <= in_stack_0000000c) {
          return;
        }
        iVar5 = meth_0x452520(this,param_2);
        if (iVar5 != 0) {
          cls_0x4405d0::meth_0x4405d0((cls_0x4405d0 *)&DAT_00656e78,*(undefined4 *)(iVar5 + 0x40));
        }
        cls_0x5a4358::virt_meth_0x43f350((cls_0x5a4358 *)&DAT_00656ce8);
        return;
      }
      if (param_1 != 4) {
        if (param_1 != 2) {
          return;
        }
        if (this->mbr_0x88c != 0) {
          if ((code *)this->mbr_0x890 != (code *)0x0) {
            (*(code *)this->mbr_0x890)();
          }
          iVar5 = DAT_0065a28c;
          this->mbr_0x88c = 0;
          FUN_0043a020(iVar5);
          this->mbr_0x894 = 0;
          return;
        }
        if (DAT_00668518 == 1) {
          cls_0x4405d0::meth_0x440d20((cls_0x4405d0 *)&DAT_00656e78);
        }
        iVar5 = cls_0x4405d0::meth_0x4406e0((cls_0x4405d0 *)&DAT_00656e78);
        while (iVar5 != 0) {
          iVar5 = cls_0x4405d0::meth_0x4406e0((cls_0x4405d0 *)&DAT_00656e78);
        }
        this->mbr_0x894 = 0;
        return;
      }
      if (this->mbr_0x88c != 0) {
        FUN_0046dad0((this->mbr_0x78 - this->mbr_0x108) + param_2,
                     (this->mbr_0x7c - this->mbr_0x10c) + in_stack_0000000c,(int *)&local_d0,
                     this->mbr_0xb8);
        if (0 < (int)DAT_00656f00) {
          if (DAT_00656f00 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = *DAT_00656f10;
          }
          local_d4 = (cls_0x499720 *)(*(uint *)(iVar5 + 0x14) >> 0x10 & 1);
          if (local_d4 != (cls_0x499720 *)0x0) {
            local_d0 = (float)((uint)local_d0 & 0xfffffff0);
            local_cc = (float)((uint)local_cc & 0xfffffff0);
          }
        }
        (*(code *)this->mbr_0x88c)(local_d0,local_cc,local_c8);
        iVar5 = DAT_0065a28c;
        if (this->mbr_0x898 != 2) {
          return;
        }
        this->mbr_0x894 = 0;
        this->mbr_0x88c = 0;
        FUN_0043a020(iVar5);
        return;
      }
      if (0 < (int)DAT_00656f00) {
        if (DAT_00656f00 < 0xb) {
          iVar5 = 0;
        }
        else {
          iVar5 = DAT_00656f10[10];
        }
        local_d4 = (cls_0x499720 *)(*(uint *)(iVar5 + 0x14) >> 0x10 & 1);
        if (local_d4 != (cls_0x499720 *)0x0) {
          dVar6 = this->mbr_0x108;
          if ((int)dVar6 < 0) {
            return;
          }
          if ((int)this->mbr_0x10c < 0) {
            return;
          }
          local_d0 = (float)((this->mbr_0x100 * 0x40 + dVar6) * 0x10);
          local_cc = (float)((this->mbr_0x104 * 0x40 + this->mbr_0x10c) * 0x10);
          pcVar8 = (cls_0x499720 *)FUN_00499e10_TSector_FindLoadedSector(_DAT_00666970,this->mbr_0x100,this->mbr_0x104);
          if (pcVar8 == (cls_0x499720 *)0x0) {
            local_c8 = 0;
          }
          else {
            local_c8 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(pcVar8,dVar6);
          }
          FUN_0046d7a0(&local_d0,&local_d4,&local_c4);
          local_84 = (int)local_c4 + 0x14;
          if ((this->TObjectInstance).mbr_0x50 == 0) {
            dVar6 = this->mbr_0x130;
            if ((int)dVar6 < 0x40) {
              pdVar1 = &this->mbr_0x134 + dVar6 * 7;
              *pdVar1 = (dword)(local_d4 + -5);
              pdVar1[1] = (int)local_c4 - 0x14;
              pdVar1[2] = (dword)(local_d4 + 5);
              pdVar1[3] = local_84;
              (&this->mbr_0x144)[dVar6 * 7] = 0;
              this->mbr_0x130 = this->mbr_0x130 + 1;
            }
            else {
              meth_0x4546a0(this);
              this->mbr_0x130 = 0;
            }
          }
LAB_0044f78e:
          this->mbr_0xfc = 0xffffffff;
          this->mbr_0xf8 = 0xffffffff;
          this->mbr_0x110 = 0xffffffff;
          this->mbr_0x10c = 0xffffffff;
          this->mbr_0x108 = 0xffffffff;
          this->mbr_0x104 = 0xffffffff;
          this->mbr_0x100 = 0xffffffff;
          return;
        }
      }
    }
    cls_0x4405d0::meth_0x440c60((cls_0x4405d0 *)&DAT_00656e78);
    return;
  }
  FUN_00499ff0_TSector_DispatchNotifyToAll(4,DAT_00667fcc);
  if (param_1 == 2) {
    FUN_00499ff0_TSector_DispatchNotifyToAll(4,DAT_00667fcc);
    if (DAT_0065c9e0 == 0) {
      if (DAT_00667fcc == (cls_0x5b4f30_TPlayer *)0x0) {
        return;
      }
      meth_0x44ee00(this);
      return;
    }
    iVar5 = meth_0x452520(this,param_2);
    if (iVar5 == 0) {
      return;
    }
    sVar3 = *(short *)(iVar5 + 4);
    if (sVar3 == 9) {
      return;
    }
    if (sVar3 == 0x19) {
      return;
    }
    if (sVar3 == 10) {
      return;
    }
    if (sVar3 == 0xf) {
      return;
    }
    if (sVar3 == 0xe) {
      return;
    }
    meth_0x47eff0((TCharacter *)&DAT_0065caf0);
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
    return;
  }
  if (param_1 == 5) {
    if (DAT_00667fcc == (cls_0x5b4f30_TPlayer *)0x0) {
      return;
    }
    if (this->mbr_0x11c == 0) {
      return;
    }
    iVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
    FUN_0043a020(iVar5);
    (**(code **)(*DAT_00667fd0 + 0x30))(this->mbr_0x128,0);
    this->mbr_0x128 = 0xffffffff;
    this->mbr_0x11c = 0;
    return;
  }
  if (param_1 == 1) {
    if (DAT_00667fcc == (cls_0x5b4f30_TPlayer *)0x0) {
      return;
    }
    if (param_2 < 0) {
      return;
    }
    if (in_stack_0000000c < 0) {
      return;
    }
    if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x8 <= param_2) {
      return;
    }
    if ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0xc <= in_stack_0000000c) {
      return;
    }
    piVar9 = (int *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe0;
    if (piVar9 != (int *)0x0) {
      if ((*piVar9 == 3) && ((this->mbr_0x12c == 0xffffffff || (this->mbr_0x12c == 6)))) {
        iVar5 = meth_0x452520(this,param_2);
        if ((iVar5 != 0) && (*(short *)(iVar5 + 4) == 0xc)) {
          piVar9 = (int *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe0;
          if ((piVar9 == (int *)0x0) ||
             ((*piVar9 != 3 && ((piVar9 == (int *)0x0 || (*piVar9 != 0x19)))))) {
            iVar7 = 0;
          }
          else {
            iVar7 = piVar9[0x11];
          }
          if (iVar7 != iVar5) {
            iVar5 = TPlayScreen::meth_0x4c89c0_IsEnemy(&DAT_00667fcc->TPlayScreen);
            if (iVar5 == 0) {
              return;
            }
            TPlayScreen::meth_0x4d4790_SetFighting(&DAT_00667fcc->TPlayScreen);
            return;
          }
        }
        FUN_00483300_RandomRange(1,3);
        cls_0x5b4f30_TPlayer::meth_0x4d2480_ButtonAttack(DAT_00667fcc);
        return;
      }
      if (((piVar9 != (int *)0x0) && (*piVar9 == 0x19)) &&
         ((this->mbr_0x12c == 0xffffffff || (this->mbr_0x12c == 6)))) {
        iVar5 = cls_0x5b4f30_TPlayer::meth_0x4d1050(DAT_00667fcc);
        if (iVar5 == 0) {
          cls_0x5b4f30_TPlayer::meth_0x4d0aa0(DAT_00667fcc);
          local_d0 = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x10;
          local_cc = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x14;
          local_c8 = (DAT_00667fcc->TPlayScreen).TScreen.mbr_0x18;
          meth_0x44ed50(this,&local_90);
          FUN_0046dc60_AngleToPP((int *)&local_d0,(int *)&local_90);
          cls_0x5b4f30_TPlayer::meth_0x4d0c70(DAT_00667fcc);
        }
        this->mbr_0x120 = 1;
        return;
      }
    }
    piVar9 = (int *)meth_0x452520(this,param_2);
    if (piVar9 == (int *)0x0) {
      this->mbr_0xf8 = 0xffffffff;
    }
    else {
      this->mbr_0xf8 = piVar9[0x10];
    }
    this->mbr_0x120 = 1;
    if (piVar9 == (int *)0x0) {
      return;
    }
    iVar5 = (**(code **)(*piVar9 + 0x134))();
    if (iVar5 == 0) {
      return;
    }
    cls_0x5a5ba0::virt_meth_0x5496a0((cls_0x5a5ba0 *)&DAT_0065b140);
    return;
  }
  if (param_1 != 4) {
    return;
  }
  if (((((DAT_00667fcc == (cls_0x5b4f30_TPlayer *)0x0) ||
        (uVar4 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e50_448)
                           (DAT_00667fcc), CONCAT31(extraout_var,uVar4) == 0 || extraout_var < 0))
       || (param_2 < 0)) ||
      ((in_stack_0000000c < 0 || ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x8 <= param_2)))) ||
     ((int)(this->TObjectInstance).cls_0x41c7f0.mbr_0xc <= in_stack_0000000c)) goto LAB_0045046f;
  piVar9 = (int *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe0;
  if (piVar9 != (int *)0x0) {
    if (((*piVar9 == 3) && ((this->mbr_0x12c == 0xffffffff || (this->mbr_0x12c == 6)))) &&
       (this->mbr_0x120 != 0)) goto LAB_0045046f;
    if ((((piVar9 != (int *)0x0) && (*piVar9 == 0x19)) &&
        (iVar5 = cls_0x5b4f30_TPlayer::meth_0x4d1050(DAT_00667fcc), iVar5 != 0)) && (this->mbr_0x120 != 0))
    {
      local_d0 = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x10;
      iVar7 = DAT_00668510 - (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
      local_cc = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x14;
      local_c8 = (DAT_00667fcc->TPlayScreen).TScreen.mbr_0x18;
      iVar5 = DAT_00668514 - (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
      cls_0x41c750::meth_0x41c750(&local_90);
      FUN_0046dad0(this->mbr_0x78 + iVar7,this->mbr_0x7c + iVar5,(int *)&fStack_bc,
                   (int)((int)&((cls_0x499720 *)(local_90.mbr_0x8 + 0x30))->mbr_0x0 + 2));
      FUN_0046dc60_AngleToPP((int *)&local_d0,(int *)&fStack_bc);
      cls_0x5b4f30_TPlayer::meth_0x4d0fd0(DAT_00667fcc);
      goto LAB_0045046f;
    }
  }
  this_00 = (TPlayScreen *)0x0;
  if ((((DAT_0065d674 == (cls_0x5b4f30_TPlayer *)0x0) ||
       (this_00 = (TPlayScreen *)cls_0x5b4f30_TPlayer::meth_0x4701f0(DAT_0065d674),
       this_00 == (TPlayScreen *)0x0)) &&
      ((DAT_0065b088 == (cls_0x5b4f30_TPlayer *)0x0 ||
       ((DAT_0065b090 < 0 ||
        (this_00 = (TPlayScreen *)cls_0x5b4f30_TPlayer::meth_0x4701f0(DAT_0065b088),
        this_00 == (TPlayScreen *)0x0)))))) && (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0)) {
    this_00 = (TPlayScreen *)(&(DAT_00667fcc->TPlayScreen).mbr_0x2a0)[DAT_0065b878];
  }
  dVar6 = 0xffffffff;
  piVar9 = (int *)meth_0x452520(this,param_2);
  if (piVar9 != (int *)0x0) {
    dVar6 = piVar9[0x10];
  }
  if (this->mbr_0x120 != 0) {
    if (((piVar9 == (int *)0x0) || (iVar5 = (**(code **)(*piVar9 + 4))(DAT_00667fcc), iVar5 < 0x61))
       || (*(short *)(piVar9 + 1) == 0xc)) {
      if ((this->mbr_0xf8 == dVar6) && (piVar9 != (int *)0x0)) {
        iVar5 = (**(code **)(*piVar9 + 0x134))();
        if (iVar5 == 0) {
          (**(code **)(*piVar9 + 0xbc))(DAT_00667fcc,0xffffffff);
        }
        else {
          if ((int)dVar6 < 0) {
            cStack_80.mbr_0xc = 0;
          }
          else {
            cls_0x44ceb0::meth_0x44cf80(&cStack_80,0,0x80,0,0);
            while ((int *)cStack_80.mbr_0xc != (int *)0x0) {
              if (*(dword *)(cStack_80.mbr_0xc + 0x40) == dVar6) goto LAB_0044ffc8;
              cls_0x44ceb0::meth_0x44d080(&cStack_80);
            }
            cStack_80.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
          }
LAB_0044ffc8:
          DAT_00668570._0_4_ = (int *)cStack_80.mbr_0xc;
          if ((*(short *)(cStack_80.mbr_0xc + 4) != 0xb) &&
             (*(short *)(cStack_80.mbr_0xc + 4) != 0xc)) {
            TPlayScreen::meth_0x4cfef0(&DAT_00667fcc->TPlayScreen);
          }
        }
      }
    }
    else {
      fStack_bc = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x10;
      fStack_b8 = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x14;
      pcStack_b4 = (cls_0x499720 *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x18;
      meth_0x44e930(this,param_2,in_stack_0000000c,&fStack_bc);
      local_d4 = (cls_0x499720 *)((int)fStack_ac - (int)fStack_bc);
      Var18 = fpatan((float10)((int)fStack_a8 - (int)fStack_b8),(float10)(int)local_d4);
      fVar17 = (float10)fcos(Var18);
      local_c4 = (float)(fVar17 * (float10)_DAT_005a4968);
      fVar17 = (float10)fsin(Var18);
      local_c0 = (float)(fVar17 * (float10)_DAT_005a4968);
      local_d0 = (float)(int)fStack_bc;
      local_cc = (float)(int)fStack_b8;
      lVar19 = __ftol();
      fStack_ac = (float)((int)fStack_ac + (int)lVar19);
      lVar19 = __ftol();
      fStack_a8 = (float)((int)fStack_a8 + (int)lVar19);
      iVar7 = 0;
      iVar5 = (int)(iVar5 + -0x20 + (iVar5 + -0x20 >> 0x1f & 0x1fU)) >> 5;
      if (0 < iVar5) {
        lVar19 = __ftol();
        local_d4 = (cls_0x499720 *)lVar19;
        do {
          lVar19 = __ftol();
          local_90.mbr_0x0 = (dword)lVar19;
          lVar19 = __ftol();
          local_90.mbr_0x4 = (dword)lVar19;
          local_90.mbr_0x8 = (dword)local_d4;
          FUN_004530a0_GetWalkHeightRadius((int *)&local_90,(uint)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe,0x20
                       ,(int *)&local_b0,&local_94,&local_a0);
          fVar16 = local_b0;
          if ((int)local_b0 < 0) {
            fVar16 = (float)-(int)local_b0;
          }
          if ((0x20 < (int)fVar16) || (local_a0 != (cls_0x499720 *)0x0)) break;
          local_d0 = local_d0 + local_c4;
          iVar7 = iVar7 + 1;
          local_cc = local_cc + local_c0;
        } while (iVar7 < iVar5);
      }
      if (iVar7 == iVar5) {
        TPlayScreen::meth_0x4cedb0(&DAT_00667fcc->TPlayScreen,fStack_ac,fStack_a8);
      }
      else {
        uVar10 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        FUN_0054d190((TCharacter *)&DAT_0065c5d0,0x10,uVar10);
      }
    }
    goto LAB_0045046f;
  }
  if (this_00 == (TPlayScreen *)0x0) goto LAB_0045046f;
  if (-1 < (int)dVar6) {
    cls_0x44ceb0::meth_0x44cf80(&cStack_80,0,0x80,0,0);
    while ((int *)cStack_80.mbr_0xc != (int *)0x0) {
      if (*(dword *)(cStack_80.mbr_0xc + 0x40) == dVar6) goto LAB_00450047;
      cls_0x44ceb0::meth_0x44d080(&cStack_80);
    }
    cStack_80.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00450047:
    if ((int *)cStack_80.mbr_0xc != (int *)0x0) {
      sVar3 = *(short *)(cStack_80.mbr_0xc + 0x7c);
      iVar5 = (**(code **)(*(int *)cStack_80.mbr_0xc + 0xbc))
                        (DAT_00667fcc,*(undefined4 *)&(this_00->TScreen).field_0x40);
      if (iVar5 != 0) {
        if (sVar3 < 0x100) {
          UNK_0065d53c._12_4_ = 1;
          (**(code **)(DAT_0065d4f8 + 0x90))();
        }
        else if (0x10a < sVar3) {
          DAT_0065b02c._76_4_ = 1;
        }
        goto LAB_0045046f;
      }
    }
  }
  uVar4 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x46fad0_92)(this_00);
  if ((CONCAT31(extraout_var_00,uVar4) == 0) || (*(short *)&(this_00->TScreen).mbr_0x4 == 0x15)
     ) goto LAB_0045046f;
  sVar3 = *(short *)&(this_00->TScreen).field_0x7c;
  if ((0xff < sVar3) && (sVar3 < 0x10b)) {
    TPlayScreen::meth_0x5199b0(&DAT_00667fcc->TPlayScreen,0);
  }
  local_d0 = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x10;
  local_cc = (float)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x14;
  local_c8 = (DAT_00667fcc->TPlayScreen).TScreen.mbr_0x18;
  meth_0x44e930(this,param_2,in_stack_0000000c,&local_d0);
  if (*(short *)&(this_00->TScreen).mbr_0x4 != 8) {
    fStack_ac = (float)((int)fStack_ac + 0x40);
    fStack_a8 = (float)((int)fStack_a8 + 0x40);
  }
  local_c0 = (float)((int)fStack_ac - (int)local_d0);
  local_c4 = (float)((int)fStack_a8 - (int)local_cc);
  local_d4 = (cls_0x499720 *)((int)local_c0 * (int)local_c0 + (int)local_c4 * (int)local_c4);
  lVar19 = __ftol();
  if (0x60 < (int)lVar19) {
    iVar5 = 0;
    Var18 = fpatan((float10)(int)local_c4,(float10)(int)local_c0);
    fVar17 = (float10)fcos(Var18);
    local_c0 = (float)(fVar17 * (float10)_DAT_005a4968);
    fVar17 = (float10)fsin(Var18);
    local_b0 = (float)(fVar17 * (float10)_DAT_005a4968);
    fStack_bc = (float)(int)local_d0;
    fStack_b8 = (float)(int)local_cc;
    lVar19 = __ftol();
    do {
      lVar20 = __ftol();
      local_90.mbr_0x0 = (dword)lVar20;
      lVar20 = __ftol();
      local_90.mbr_0x4 = (dword)lVar20;
      local_90.mbr_0x8 = (dword)(cls_0x499720 *)lVar19;
      FUN_004530a0_GetWalkHeightRadius((int *)&local_90,(uint)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe,0x20,
                   (int *)&local_d4,&local_a0,&local_c4);
      pcVar8 = local_d4;
      if ((int)local_d4 < 0) {
        pcVar8 = (cls_0x499720 *)-(int)local_d4;
      }
      if ((0x20 < (int)pcVar8) || (local_c4 != 0.0)) break;
      fStack_bc = fStack_bc + local_c0;
      iVar5 = iVar5 + 1;
      fStack_b8 = fStack_b8 + local_b0;
    } while (iVar5 < 3);
    lVar20 = __ftol();
    fStack_ac = (float)lVar20;
    lVar20 = __ftol();
    fStack_a8 = (float)lVar20;
    pcStack_a4 = (cls_0x499720 *)lVar19;
  }
  (*((this_00->TScreen).vftptr_0x0)->FUN_0046faf0_96)();
  uVar10 = 0;
  uVar12 = (uint)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xe;
  pfVar21 = &fStack_ac;
  (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x46ed70_8)(this_00);
  (*((this_00->TScreen).vftptr_0x0)->FUN_0046f3a0_140)(pfVar21,uVar12,uVar10);
  TPlayScreen::meth_0x46e7d0(this_00,&local_90.mbr_0x4);
  uVar4 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477db0_408)(this_00);
  if (CONCAT31(extraout_var_01,uVar4) < 2) {
    iVar5 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (iVar5 < 0) {
      cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      pcVar11 = s__s__s_005d03b4;
      goto LAB_004503bc;
    }
    uVar10 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar10);
  }
  else {
    iVar5 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    if (iVar5 < 0) {
      iVar5 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if (iVar5 < 0) {
        cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477db0_408)(this_00);
        FUN_0054d170((TCharacter *)&DAT_0065c5d0,s__d__s_s__s_005d0378);
        goto LAB_004503c9;
      }
      (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477db0_408)(this_00);
      pcVar11 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    }
    else {
      (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477db0_408)(this_00);
      pcVar11 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    }
LAB_004503bc:
    FUN_0054d170((TCharacter *)&DAT_0065c5d0,pcVar11);
  }
LAB_004503c9:
  DAT_00668570._4_4_ = this_00;
  uVar4 = (*((this_00->TScreen).vftptr_0x0)->FUN_0046e930_48)();
  if (CONCAT31(extraout_var_02,uVar4) != 0) {
    (*((this_00->TScreen).vftptr_0x0)->FUN_0046e8b0_40)();
  }
  uVar4 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477900_36)(this_00);
  if (CONCAT31(extraout_var_03,uVar4) != 0) {
    fStack_bc = fStack_ac;
    uVar12 = (this_00->TScreen).mbr_0x48;
    fStack_b8 = fStack_a8;
    pcStack_b4 = pcStack_a4;
    uVar12 = FUN_0059a530_stricmp(uVar12,*(byte **)(uVar12 + 4),(byte *)s_Armor_005d03bc);
    if ((uVar12 != 0) && (*(short *)&(this_00->TScreen).mbr_0x4 != 8)) {
      pcStack_b4 = pcStack_b4 + 0x10;
      FUN_00445ef0(CONCAT31(extraout_var_03,uVar4),(int *)&fStack_bc,&fStack_ac,0,0xffffffff,1);
    }
  }
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    cls_0x57d9d0::meth_0x585bc0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,DAT_00667fcc,this_00);
  }
LAB_0045046f:
  this->mbr_0x120 = 0;
  if ((((-1 < param_2) && (-1 < in_stack_0000000c)) &&
      (param_2 < (int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x8)) &&
     (in_stack_0000000c < (int)(this->TObjectInstance).cls_0x41c7f0.mbr_0xc)) {
    FUN_0043a100(0,0,0);
    FUN_0043a170((char *)&DAT_00658d98);
    FUN_0043a140((int *)0x0);
  }
  return;
}



// Function at 00450d20

void TCharacter::meth_0x450d20()

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  dword *pdVar4;
  dword *in_stack_00000004;
  TCharacter *local_4;
  
  pdVar4 = in_stack_00000004;
  local_4 = this;
  FUN_0046d7a0(in_stack_00000004,&stack0x00000004,&local_4);
  iVar3 = DAT_0065c5c4;
  *(int *)&(this->TObjectInstance).field_0x34 = (int)in_stack_00000004 - DAT_00667c30 / 2;
  dVar1 = *pdVar4;
  *(int *)&(this->TObjectInstance).field_0x38 = (int)local_4 - iVar3 / 2;
  if (((dVar1 != this->mbr_0xb0) || (pdVar4[1] != this->mbr_0xb4)) || (pdVar4[2] != this->mbr_0xb8))
  {
    cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,dVar1,pdVar4[1]);
  }
  dVar1 = pdVar4[1];
  dVar2 = pdVar4[2];
  this->mbr_0xa4 = *pdVar4;
  this->mbr_0xa8 = dVar1;
  this->mbr_0xac = dVar2;
  return;
}



// Function at 00450dc0

void TCharacter::virt_meth_0x450dc0()

{
  dword dVar1;
  
  DAT_0065c5c4 = *(dword *)&(this->TObjectInstance).field_0x20;
  DAT_00667c30 = (this->TObjectInstance).mbr_0x1c;
  dVar1 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x14;
  DAT_006663d8 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x4 = dVar1;
  DAT_006663d4 = dVar1;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x0 = DAT_006663d8;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x8 = DAT_00667c30;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0xc = DAT_0065c5c4;
  cls_0x411eb0::meth_0x412150
            ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),DAT_006663d8,dVar1,DAT_00667c30);
  FUN_004aa7c0((this->TObjectInstance).mbr_0x58,DAT_006663d8,DAT_006663d4,DAT_00667c30,DAT_0065c5c4);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  DAT_00658d94 = 1;
  return;
}



// Function at 00450e40

int TCharacter::meth_0x450e40(short *param_1)

{
  short sVar1;
  LPCVOID pvVar2;
  dword dVar3;
  undefined uVar4;
  int iVar5;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int unaff_EBX;
  int iVar6;
  undefined4 in_stack_00000008;
  cls_0x44ceb0 local_90;
  cls_0x44ceb0 local_48;
  
  iVar6 = -1;
  if ((DAT_0065a258_TObjectClass_numclasses <= (uint)(int)*param_1) ||
     ((cls_0x4746d0 *)(&DAT_0065a148_TObjectClass_classes)[*param_1] == (cls_0x4746d0 *)0x0)) {
    return -1;
  }
  iVar5 = cls_0x4746d0::meth_0x474bb0_NewObject
                    ((cls_0x4746d0 *)(&DAT_0065a148_TObjectClass_classes)[*param_1],param_1,in_stack_00000008);
  if (iVar5 == 0) {
    return -1;
  }
  if ((*param_1 == 0xc) || (*param_1 == 0xb)) {
    iVar6 = meth_0x451cb0(this);
  }
  *(undefined4 *)(iVar5 + 0x40) = 0xffffffff;
  iVar5 = meth_0x451090(this);
  if ((iVar5 < 0) && (-1 < iVar6)) {
    cls_0x44ceb0::meth_0x44cf80(&local_90,0,0x80,0,0);
    this_00 = (cls_0x5b4f30_TPlayer *)local_90.mbr_0xc;
    while (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      if (*(int *)&(this_00->TPlayScreen).TScreen.field_0x40 == iVar6) goto LAB_00450f25;
      local_90.mbr_0xc = (dword)this_00;
      cls_0x44ceb0::meth_0x44d080(&local_90);
      this_00 = (cls_0x5b4f30_TPlayer *)local_90.mbr_0xc;
    }
    this_00 = (cls_0x5b4f30_TPlayer *)cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00450f25:
    if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      sVar1 = *(short *)&(this_00->TPlayScreen).TScreen.mbr_0x4;
      if (((sVar1 == 0xc) || (sVar1 == 0xb)) &&
         (pvVar2 = (LPCVOID)(this_00->TPlayScreen).mbr_0x284, pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
        (this_00->TPlayScreen).mbr_0x284 = 0;
      }
      if (((this_00 == (cls_0x5b4f30_TPlayer *)this->mbr_0xdc) &&
          (this->mbr_0xdc = 0, (*(byte *)&this->mbr_0xd8 & 1) != 0)) && (DAT_00667fcc != 0)) {
        this->mbr_0xdc = DAT_00667fcc;
        this->mbr_0xd8 = 9;
      }
      dVar3 = (this_00->TPlayScreen).TScreen.mbr_0x50;
      if (-1 < (int)dVar3) {
        cls_0x44ceb0::cls_0x44ceb0(&local_48,0,0x80,0,0);
        while (local_48.mbr_0xc != 0) {
          if (*(dword *)(local_48.mbr_0xc + 0x40) == dVar3) goto LAB_00450fd5;
          cls_0x44ceb0::meth_0x44d080(&local_48);
        }
        local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00450fd5:
        if (local_48.mbr_0xc != 0) {
          meth_0x451610(this);
        }
      }
      uVar4 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x4778f0_32)(this_00);
      if (CONCAT31(extraout_var,uVar4) != 0) {
        (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046e900_44)();
      }
      uVar4 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477a10_180)(this_00);
      if ((CONCAT31(extraout_var_00,uVar4) == 0) &&
         ((this_00->TPlayScreen).TScreen.mbr_0x64 == 0)) {
        cls_0x57d9d0::meth_0x584270((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this_00);
        dVar3 = (this_00->TPlayScreen).TScreen.mbr_0x44;
        if (dVar3 != 0) {
          meth_0x452750(this,this_00);
          FUN_00456710(s_d__revenant_MapPane_cpp_005d0468,0x800);
          meth_0x454920(this);
          FUN_00499250_TSector_RemoveObject(dVar3,(int)this_00);
          FUN_00456740();
        }
      }
      else {
        (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046faf0_96)();
      }
      FUN_0046e6d0(this_00);
      (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                ((TScreen *)this_00);
      iVar5 = unaff_EBX;
    }
  }
  return iVar5;
}



// Function at 00451090

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 TCharacter::meth_0x451090()

{
  HANDLE hMutex;
  int iVar1;
  int iVar2;
  int iVar3;
  DWORD DVar4;
  uint uVar5;
  dword dVar6;
  int in_stack_00000004;
  undefined auStack_58 [16];
  cls_0x44ceb0 cStack_48;
  
  iVar1 = in_stack_00000004;
  if (*(int *)(in_stack_00000004 + 0x44) != 0) {
    return 0xffffffff;
  }
  iVar2 = FUN_00499e10_TSector_FindLoadedSector((uint)*(ushort *)(in_stack_00000004 + 0xe),
                       *(int *)(in_stack_00000004 + 0x10) >> 10,
                       *(int *)(in_stack_00000004 + 0x14) >> 10);
  if (iVar2 == 0) {
    return 0xffffffff;
  }
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d03c4;
    DAT_00658474 = 0x727;
  }
  FUN_00498fb0_TSector_AddObject(iVar2,iVar1,-1);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar2 = ReleaseMutex(DAT_00658310);
    while (iVar2 != 0) {
      iVar2 = ReleaseMutex(hMutex);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  if (*(int *)(iVar1 + 0x40) < 0) {
    FUN_0058c5a4(&stack0x00000004);
    iVar2 = in_stack_00000004 + -0x34d6574c;
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    iVar3 = _rand();
    DVar4 = GetTickCount();
    FUN_0058c575(iVar3 + DVar4);
    uVar5 = _rand();
    if (_DAT_00666970 == DAT_00658d8c) {
      DAT_00658d8c = DAT_00658d8c + 1;
    }
    if (0x1f < (int)DAT_00658d8c) {
      DAT_00658d8c = 0;
    }
    *(uint *)(iVar1 + 0x40) = (DAT_00658d8c & 0x1f) << 0x1a | (uVar5 | iVar2 << 0xf) & 0x7fffffff;
  }
  if (*(short *)(iVar1 + 0x7c) < 0) {
    meth_0x452750(this,iVar1);
    iVar2 = *(int *)(iVar1 + 0x40);
    if (-1 < iVar2) {
      cls_0x44ceb0::cls_0x44ceb0(&cStack_48,0,0x80,0,0);
      while ((int *)cStack_48.mbr_0xc != (int *)0x0) {
        if (*(int *)(cStack_48.mbr_0xc + 0x40) == iVar2) goto LAB_0045122a;
        cls_0x44ceb0::meth_0x44d080(&cStack_48);
      }
      cStack_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_0045122a:
      if (((((int *)cStack_48.mbr_0xc != (int *)0x0) &&
           (dVar6 = (**(code **)(*(int *)cStack_48.mbr_0xc + 0xfc))(), dVar6 != 4)) &&
          ((**(code **)(*(int *)cStack_48.mbr_0xc + 0xf4))(auStack_58),
          (this->TObjectInstance).mbr_0x50 == 0)) && ((int)dVar6 < 4)) {
        if ((int)this->mbr_0x130 < 0x40) {
          cls_0x41c700::meth_0x41c700((cls_0x41c700 *)(&this->mbr_0x134 + this->mbr_0x130 * 7));
          (&this->mbr_0x144)[this->mbr_0x130 * 7] = dVar6;
          this->mbr_0x130 = this->mbr_0x130 + 1;
        }
        else {
          meth_0x4546a0(this);
          this->mbr_0x130 = 0;
        }
      }
    }
  }
  cls_0x57d9d0::meth_0x584220((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,iVar1);
  return *(undefined4 *)(iVar1 + 0x40);
}



// Function at 00451610

int * TCharacter::meth_0x451610()

{
  int iVar1;
  HANDLE hMutex;
  int *piVar2;
  int iVar3;
  dword dVar4;
  int *in_stack_00000004;
  undefined auStack_58 [16];
  cls_0x44ceb0 local_48;
  
  if (in_stack_00000004 == (int *)0x0) {
    return (int *)0x0;
  }
  iVar3 = in_stack_00000004[0x14];
  if (-1 < iVar3) {
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
    piVar2 = (int *)local_48.mbr_0xc;
    while (local_48.mbr_0xc = (dword)piVar2, piVar2 != (int *)0x0) {
      if (piVar2[0x10] == iVar3) goto LAB_00451670;
      cls_0x44ceb0::meth_0x44d080(&local_48);
      piVar2 = (int *)local_48.mbr_0xc;
    }
    piVar2 = (int *)cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00451670:
    if (piVar2 != (int *)0x0) {
      meth_0x451610(this);
    }
  }
  iVar3 = (**(code **)(*in_stack_00000004 + 0x20))();
  if (iVar3 != 0) {
    (**(code **)(*in_stack_00000004 + 0x2c))();
  }
  iVar3 = (**(code **)(*in_stack_00000004 + 0xb4))();
  if ((iVar3 == 0) && (in_stack_00000004[0x19] == 0)) {
    cls_0x57d9d0::meth_0x584270((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,in_stack_00000004);
    iVar3 = in_stack_00000004[0x11];
    if (iVar3 != 0) {
      meth_0x452750(this,in_stack_00000004);
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
        DAT_00658474 = 0x800;
      }
      iVar1 = in_stack_00000004[0x10];
      if (-1 < iVar1) {
        cls_0x44ceb0::cls_0x44ceb0(&local_48,0,0x80,0,0);
        while ((int *)local_48.mbr_0xc != (int *)0x0) {
          if (*(int *)(local_48.mbr_0xc + 0x40) == iVar1) goto LAB_0045174f;
          cls_0x44ceb0::meth_0x44d080(&local_48);
        }
        local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_0045174f:
        if (((((int *)local_48.mbr_0xc != (int *)0x0) &&
             (dVar4 = (**(code **)(*(int *)local_48.mbr_0xc + 0xfc))(), dVar4 != 4)) &&
            ((**(code **)(*(int *)local_48.mbr_0xc + 0xf4))(auStack_58),
            (this->TObjectInstance).mbr_0x50 == 0)) && ((int)dVar4 < 4)) {
          if ((int)this->mbr_0x130 < 0x40) {
            cls_0x41c700::meth_0x41c700((cls_0x41c700 *)(&this->mbr_0x134 + this->mbr_0x130 * 7));
            (&this->mbr_0x144)[this->mbr_0x130 * 7] = dVar4;
            this->mbr_0x130 = this->mbr_0x130 + 1;
          }
          else {
            meth_0x4546a0(this);
            this->mbr_0x130 = 0;
          }
        }
      }
      FUN_00499250_TSector_RemoveObject(iVar3,(int)in_stack_00000004);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar3 = ReleaseMutex(DAT_00658310);
        while (iVar3 != 0) {
          iVar3 = ReleaseMutex(hMutex);
        }
        DAT_0065845c = (char *)0x0;
        DAT_00658474 = 0;
        return in_stack_00000004;
      }
    }
  }
  else {
    (**(code **)(*in_stack_00000004 + 0x60))();
  }
  return in_stack_00000004;
}



// Function at 00451840

void TCharacter::meth_0x451840()

{
  short sVar1;
  LPCVOID pvVar2;
  dword dVar3;
  HANDLE hMutex;
  undefined uVar4;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int *piVar5;
  int iVar6;
  cls_0x5b4f30_TPlayer *in_stack_00000004;
  undefined auStack_54 [12];
  cls_0x44ceb0 local_48;
  
  if ((in_stack_00000004 != (cls_0x5b4f30_TPlayer *)0x0) &&
     (((sVar1 = *(short *)&(in_stack_00000004->TPlayScreen).TScreen.mbr_0x4, sVar1 == 0xc ||
       (sVar1 == 0xb)) &&
      (pvVar2 = (LPCVOID)(in_stack_00000004->TPlayScreen).mbr_0x284, pvVar2 != (LPCVOID)0x0)))) {
    FUN_004830f0(pvVar2);
    (in_stack_00000004->TPlayScreen).mbr_0x284 = 0;
  }
  if (((in_stack_00000004 == (cls_0x5b4f30_TPlayer *)this->mbr_0xdc) &&
      (this->mbr_0xdc = 0, (*(byte *)&this->mbr_0xd8 & 1) != 0)) && (DAT_00667fcc != 0)) {
    this->mbr_0xdc = DAT_00667fcc;
    this->mbr_0xd8 = 9;
  }
  if (in_stack_00000004 == (cls_0x5b4f30_TPlayer *)0x0) {
    FUN_00481c10((byte *)s_Tried_to_delete_an_object_not_in_005d03f4);
  }
  else {
    dVar3 = (in_stack_00000004->TPlayScreen).TScreen.mbr_0x50;
    if (-1 < (int)dVar3) {
      cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
      while (local_48.mbr_0xc != 0) {
        if (*(dword *)(local_48.mbr_0xc + 0x40) == dVar3) goto LAB_004518f8;
        cls_0x44ceb0::meth_0x44d080(&local_48);
      }
      local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_004518f8:
      if (local_48.mbr_0xc != 0) {
        meth_0x451610(this);
      }
    }
    uVar4 = (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x4778f0_32)
                      (in_stack_00000004);
    if (CONCAT31(extraout_var,uVar4) != 0) {
      (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046e900_44)();
    }
    uVar4 = (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477a10_180)
                      (in_stack_00000004);
    if ((CONCAT31(extraout_var_00,uVar4) != 0) ||
       ((in_stack_00000004->TPlayScreen).TScreen.mbr_0x64 != 0)) {
      (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046faf0_96)();
      FUN_0046e6d0(in_stack_00000004);
      return;
    }
    cls_0x57d9d0::meth_0x584270((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,in_stack_00000004);
    dVar3 = (in_stack_00000004->TPlayScreen).TScreen.mbr_0x44;
    if (dVar3 != 0) {
      meth_0x452750(this,in_stack_00000004);
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
        DAT_00658474 = 0x800;
      }
      piVar5 = (int *)FUN_00452690_TMapPane_GetInstance(*(int *)&(in_stack_00000004->TPlayScreen).TScreen.
                                            field_0x40,0);
      if ((piVar5 != (int *)0x0) && (iVar6 = (**(code **)(*piVar5 + 0xfc))(), iVar6 != 4)) {
        (**(code **)(*piVar5 + 0xf4))(auStack_54);
        meth_0x4548a0(this,auStack_54);
      }
      FUN_00499250_TSector_RemoveObject(dVar3,(int)in_stack_00000004);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar6 = ReleaseMutex(DAT_00658310);
        while (iVar6 != 0) {
          iVar6 = ReleaseMutex(hMutex);
        }
        DAT_0065845c = (char *)0x0;
        DAT_00658474 = 0;
        FUN_0046e6d0(in_stack_00000004);
        return;
      }
    }
  }
  FUN_0046e6d0(in_stack_00000004);
  return;
}



// Function at 00451b10

int TCharacter::meth_0x451b10()

{
  dword *pdVar1;
  int iVar2;
  dword dVar3;
  HANDLE hMutex;
  dword dVar4;
  int iVar5;
  int in_stack_00000004;
  dword dStack_58;
  dword dStack_54;
  dword dStack_50;
  dword dStack_4c;
  cls_0x44ceb0 local_48;
  
  cls_0x57d9d0::meth_0x584270((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,in_stack_00000004);
  iVar5 = *(int *)(in_stack_00000004 + 0x44);
  if (iVar5 == 0) {
    return in_stack_00000004;
  }
  meth_0x452750(this,in_stack_00000004);
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
    DAT_00658474 = 0x800;
  }
  iVar2 = *(int *)(in_stack_00000004 + 0x40);
  if (-1 < iVar2) {
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
    while ((int *)local_48.mbr_0xc != (int *)0x0) {
      if (*(int *)(local_48.mbr_0xc + 0x40) == iVar2) goto LAB_00451bc2;
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
    local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00451bc2:
    if (((((int *)local_48.mbr_0xc != (int *)0x0) &&
         (dVar4 = (**(code **)(*(int *)local_48.mbr_0xc + 0xfc))(), dVar4 != 4)) &&
        ((**(code **)(*(int *)local_48.mbr_0xc + 0xf4))(&dStack_58),
        (this->TObjectInstance).mbr_0x50 == 0)) && ((int)dVar4 < 4)) {
      dVar3 = this->mbr_0x130;
      if ((int)dVar3 < 0x40) {
        pdVar1 = &this->mbr_0x134 + dVar3 * 7;
        *pdVar1 = dStack_58;
        pdVar1[1] = dStack_54;
        pdVar1[2] = dStack_50;
        pdVar1[3] = dStack_4c;
        (&this->mbr_0x144)[dVar3 * 7] = dVar4;
        this->mbr_0x130 = this->mbr_0x130 + 1;
      }
      else {
        meth_0x4546a0(this);
        this->mbr_0x130 = 0;
      }
    }
  }
  FUN_00499250_TSector_RemoveObject(iVar5,in_stack_00000004);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar5 = ReleaseMutex(DAT_00658310);
    while (iVar5 != 0) {
      iVar5 = ReleaseMutex(hMutex);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  return in_stack_00000004;
}



// Function at 00451cb0

int TCharacter::meth_0x451cb0()

{
  int iVar1;
  undefined4 *puVar2;
  int in_stack_00000004;
  undefined4 local_84;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  char local_50 [80];
  
  if (*(short *)(in_stack_00000004 + 4) == 0xe) {
    *(undefined4 *)(in_stack_00000004 + 0x50) = 0xffffffff;
    return -1;
  }
  puVar2 = &local_84;
  for (iVar1 = 0xd; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  FUN_0058b100(local_50,(byte *)s_shadow__s_005d0480);
  local_84._0_2_ = 0xe;
  iVar1 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066de40.field_0x10,local_50);
  if (iVar1 < 0) {
    return -1;
  }
  local_78 = *(undefined4 *)(in_stack_00000004 + 0x10);
  local_74 = *(undefined4 *)(in_stack_00000004 + 0x14);
  local_84._2_2_ = (undefined2)iVar1;
  local_70 = *(undefined4 *)(in_stack_00000004 + 0x18);
  iVar1 = meth_0x450e40(this,&local_84);
  if (-1 < iVar1) {
    *(int *)(in_stack_00000004 + 0x50) = iVar1;
  }
  return iVar1;
}



// Function at 00451fe0

void TCharacter::meth_0x451fe0(byte *param_1, int param_2, int param_3)

{
  uint uVar1;
  undefined4 in_stack_00000010;
  dword in_stack_ffffffd8;
  dword in_stack_ffffffdc;
  dword in_stack_ffffffe0;
  
  if (param_2 == 0) {
    uVar1 = this->mbr_0x9c;
  }
  else {
    uVar1 = (uint)*(ushort *)(param_2 + 0xe);
  }
  cls_0x45f6c0::cls_0x45f6c0((cls_0x45f6c0 *)&stack0xffffffd8);
  FUN_00451de0(param_1,in_stack_ffffffd8,in_stack_ffffffdc,in_stack_ffffffe0,uVar1,param_3,
               in_stack_00000010);
  return;
}



// Function at 00452060

int TCharacter::meth_0x452060(int *param_1, undefined param_2, int param_3, int param_4, undefined param_5, int param_6, int param_7)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  longlong lVar9;
  undefined3 in_stack_00000015;
  undefined4 in_stack_00000020;
  int local_180;
  int *local_17c;
  int iStack_178;
  int iStack_170;
  int local_168;
  int local_164;
  int local_160;
  int local_15c;
  int local_158;
  int iStack_14c;
  cls_0x44ceb0 local_148;
  int local_100 [64];
  
  if (param_7 < 1) {
    return 0;
  }
  if (param_7 < 0x41) {
    local_17c = local_100;
  }
  else {
    local_17c = FUN_00482fb0(param_7 << 2);
  }
  local_158 = param_4 * param_4;
  local_180 = 0;
  if (_param_5 < 1) {
    local_168 = *param_1 - param_4;
    local_164 = param_1[1] - param_4;
    local_160 = local_168 + -1 + param_4 * 2;
    local_15c = local_164 + -1 + param_4 * 2;
  }
  else {
    local_168 = *param_1 - (param_4 >> 1);
    local_164 = param_1[1] - (_param_5 >> 1);
    local_160 = local_168 + -1 + param_4;
    local_15c = local_164 + -1 + _param_5;
  }
  cls_0x44ceb0::meth_0x44cf80(&local_148,&local_168,0x6e0,in_stack_00000020,0);
  piVar1 = (int *)local_148.mbr_0xc;
  do {
    if (piVar1 == (int *)0x0) {
      if (param_7 >= 0x41) {
        FUN_004830f0(local_17c);
      }
      return local_180;
    }
    local_148.mbr_0xc = (dword)piVar1;
    iVar2 = (**(code **)(*piVar1 + 0xb4))();
    if ((iVar2 == 0) && ((param_6 == -1 || (*(short *)(piVar1 + 1) == param_6)))) {
      iStack_14c = piVar1[6];
      iVar2 = piVar1[4];
      iVar8 = piVar1[5];
      if (_param_5 < 1) {
        if ((param_6 == 0xc) || (param_6 == 0xb)) {
          (**(code **)(*piVar1 + 600))();
          lVar9 = __ftol();
          iStack_178 = (int)lVar9;
        }
        else {
          iStack_178 = 0;
        }
        iVar4 = *param_1;
        iVar5 = iVar4 - iVar2;
        iStack_170 = iVar5;
        if (iVar5 < 1) {
          iVar5 = iVar2 - iVar4;
          iStack_170 = iVar2 - iVar4;
        }
        iVar2 = param_1[1];
        iVar7 = iVar2 - iVar8;
        iVar4 = iVar7;
        if (iVar7 < 1) {
          iVar7 = iVar8 - iVar2;
          iVar4 = iVar8 - iVar2;
        }
        iVar2 = param_1[2];
        iVar6 = iVar2 - iStack_14c;
        iVar8 = iVar6;
        if (iVar6 < 1) {
          iVar6 = iStack_14c - iVar2;
          iVar8 = iStack_14c - iVar2;
        }
        iStack_178 = (iVar6 * iVar8 + iVar7 * iVar4 + iVar5 * iStack_170) - iStack_178;
        if (iStack_178 < local_158) {
          iVar2 = 0;
          piVar3 = local_17c;
          if (0 < local_180) {
            do {
              if (iStack_178 <= *piVar3) break;
              iVar2 = iVar2 + 1;
              piVar3 = piVar3 + 1;
            } while (iVar2 < local_180);
          }
          if (iVar2 < param_7) {
            if (iVar2 != local_180) {
              iVar8 = param_7 + -1;
              iVar4 = local_180;
              if (iVar8 < local_180) {
                iVar4 = iVar8;
              }
              FUN_0058b790(local_17c + iVar2 + 1,local_17c + iVar2,(iVar2 * 0x3fffffff + iVar4) * 4)
              ;
              iVar4 = local_180;
              if (iVar8 < local_180) {
                iVar4 = iVar8;
              }
              FUN_0058b790((undefined4 *)(param_3 + 4 + iVar2 * 4),
                           (undefined4 *)(param_3 + iVar2 * 4),(iVar2 * 0x3fffffff + iVar4) * 4);
            }
            *(int *)(param_3 + iVar2 * 4) = piVar1[0x10];
            local_17c[iVar2] = iStack_178;
joined_r0x0045242e:
            if (local_180 < param_7) {
              local_180 = local_180 + 1;
            }
          }
        }
      }
      else {
        iVar4 = *param_1 - iVar2;
        if (iVar4 < 1) {
          iVar4 = iVar2 - *param_1;
        }
        iVar2 = param_1[1] - iVar8;
        if (iVar2 < 1) {
          iVar2 = iVar8 - param_1[1];
        }
        if ((iVar4 <= param_4 >> 1) && (iVar2 <= _param_5 >> 1)) {
          iVar2 = iVar2 * iVar2 + iVar4 * iVar4;
          iVar8 = 0;
          piVar3 = local_17c;
          if (0 < local_180) {
            do {
              if (iVar2 <= *piVar3) break;
              iVar8 = iVar8 + 1;
              piVar3 = piVar3 + 1;
            } while (iVar8 < local_180);
          }
          if (iVar8 < param_7) {
            if (iVar8 != local_180) {
              iVar4 = param_7 + -1;
              iVar5 = local_180;
              if (iVar4 < local_180) {
                iVar5 = iVar4;
              }
              FUN_0058b790(local_17c + iVar8 + 1,local_17c + iVar8,(iVar5 + iVar8 * 0x3fffffff) * 4)
              ;
              iVar5 = local_180;
              if (iVar4 < local_180) {
                iVar5 = iVar4;
              }
              FUN_0058b790((undefined4 *)(param_3 + 4 + iVar8 * 4),
                           (undefined4 *)(param_3 + iVar8 * 4),(iVar8 * 0x3fffffff + iVar5) * 4);
            }
            iVar4 = piVar1[0x10];
            local_17c[iVar8] = iVar2;
            *(int *)(param_3 + iVar8 * 4) = iVar4;
            goto joined_r0x0045242e;
          }
        }
      }
    }
    cls_0x44ceb0::meth_0x44d080(&local_148);
    piVar1 = (int *)local_148.mbr_0xc;
  } while( true );
}



// Function at 00452520

TPlayScreen * TCharacter::meth_0x452520(int param_1)

{
  int *piVar1;
  undefined uVar2;
  TPlayScreen *pcVar3;
  undefined3 extraout_var;
  int iVar4;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined *puVar5;
  TPlayScreen *this_00;
  int in_stack_00000008;
  int iVar6;
  undefined *puStack_4;
  
  if ((param_1 != DAT_005e91cc) || (pcVar3 = DAT_005e91d4, in_stack_00000008 != DAT_005e91d0)) {
    pcVar3 = (TPlayScreen *)
             FUN_004142d0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),param_1,in_stack_00000008);
  }
  if (pcVar3 == (TPlayScreen *)0x0) {
    puVar5 = (undefined *)(param_1 + this->mbr_0x78);
    this_00 = (TPlayScreen *)(in_stack_00000008 + this->mbr_0x7c);
    iVar6 = 1;
    (**(code **)(*(int *)this->mbr_0x864 + 0x44))(puVar5,this_00,1);
    cls_0x44ceb0::meth_0x44cf80((cls_0x44ceb0 *)&stack0xffffffa8,0,0x20,0,0);
    while (this_00 != (TPlayScreen *)0x0) {
      puVar5 = &stack0xffffffa0;
      uVar2 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477b30_240)(this_00);
      if ((CONCAT31(extraout_var,uVar2) != 0) &&
         (((DAT_00668154 != 0 || (*(short *)&(this_00->TScreen).mbr_0x4 != 9)) &&
          (iVar4 = TPlayScreen::meth_0x46e8a0(this_00), **(int **)(*(int *)(iVar4 + 4) + 0x54) != 1
          )))) {
        if ((pcVar3 == (TPlayScreen *)0x0) ||
           (uVar2 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477be0_268)(this_00),
           CONCAT31(extraout_var_00,uVar2) != 0)) {
LAB_00452617:
          if (((DAT_00668154 != 0) ||
              ((iVar4 = FUN_0043a160(), iVar4 == 0 &&
               (uVar2 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477c00_308)(this_00),
               CONCAT31(extraout_var_02,uVar2) != 0)))) ||
             (puVar5 = puStack_4,
             uVar2 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477a20_192)(this_00),
             CONCAT31(extraout_var_03,uVar2) != -1)) {
            (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477be0_268)(this_00);
            pcVar3 = this_00;
          }
        }
        else if (iVar6 == 0) {
          puVar5 = (undefined *)this->mbr_0x864;
          uVar2 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477bc0_260)(this_00);
          if (CONCAT31(extraout_var_01,uVar2) != 0) goto LAB_00452617;
        }
      }
      cls_0x44ceb0::meth_0x44d080((cls_0x44ceb0 *)&stack0xffffffa8);
    }
    piVar1 = (int *)this->mbr_0x864;
    (**(code **)(*piVar1 + 0x44))(0,0,piVar1[1],piVar1[2],puVar5);
  }
  return pcVar3;
}



// Function at 00452750

void TCharacter::meth_0x452750(TPlayScreen *param_1)

{
  byte bVar1;
  short sVar2;
  cls_0x499560 *this_00;
  undefined uVar3;
  undefined3 extraout_var;
  int *piVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  uint *puVar11;
  int iVar12;
  undefined4 *unaff_EBX;
  int unaff_EBP;
  int iVar13;
  int **unaff_ESI;
  uint *unaff_EDI;
  int in_stack_00000008;
  int **ppiStack_58;
  undefined4 *puVar14;
  uint uStack_40;
  undefined4 uStack_3c;
  int iStack_34;
  int *piStack_30;
  int iStack_2c;
  dword local_20;
  TCharacter *local_1c;
  undefined4 uStack_18;
  int *piStack_14;
  int iStack_10;
  
  if (((param_1 != (TPlayScreen *)0x0) && (-1 < *(int *)&(param_1->TScreen).field_0x40)) &&
     ((in_stack_00000008 != 0 || (((param_1->TScreen).mbr_0x8 & 0x400000) == 0)))) {
    local_20 = 0;
    local_1c = this;
    if (*(short *)&(param_1->TScreen).mbr_0x4 == 9) {
      uVar3 = (*(param_1->TScreen).vftptr_0x0[1].virt_meth_0x477920_56)(param_1);
      local_20 = CONCAT31(extraout_var,uVar3);
    }
    piVar4 = (int *)TPlayScreen::meth_0x46e8a0(param_1);
    if ((piVar4 != (int *)0x0) &&
       (piStack_14 = piVar4, iVar5 = (**(code **)(*piVar4 + 0x60))((param_1->TScreen).mbr_0xc),
       iVar5 != 0)) {
      iStack_2c = iVar5;
      (**(code **)(*piVar4 + 0xb4))((param_1->TScreen).mbr_0xc,&stack0xffffffbc);
      uVar6 = (**(code **)(*piVar4 + 0xbc))((param_1->TScreen).mbr_0xc);
      iVar7 = (**(code **)(*piVar4 + 0xc0))((param_1->TScreen).mbr_0xc);
      puVar14 = (undefined4 *)0x0;
      puVar11 = &uStack_40;
      ppiStack_58 = &piStack_14;
      if ((param_1->TScreen).mbr_0x36 != 0) {
        puVar14 = FUN_00482ef0((int)&uStack_40 * (int)&piStack_14);
        TPlayScreen::meth_0x470f50(param_1);
        bVar1 = (param_1->TScreen).mbr_0x36;
        iVar7 = unaff_EBP;
        puVar11 = unaff_EDI;
        uVar6 = uStack_40;
        ppiStack_58 = unaff_ESI;
        if (bVar1 < 0x80) {
          iVar12 = 0;
          if (0 < (int)unaff_ESI) {
            do {
              iVar8 = 0;
              if (0 < (int)unaff_EDI) {
                do {
                  iVar9 = 3 - iVar8;
                  iVar13 = iVar12 * (int)unaff_EDI + iVar8;
                  iVar8 = iVar8 + 1;
                  *(undefined *)((int)puVar14 + iVar13) =
                       *(undefined *)(((int)&uStack_18 + iVar9) * (int)&uStack_40 + iVar12 + iVar5);
                } while (iVar8 < (int)unaff_EDI);
              }
              iVar12 = iVar12 + 1;
            } while (iVar12 < (int)unaff_ESI);
          }
        }
        else if (bVar1 < 0xc0) {
          iVar12 = 0;
          if (0 < (int)unaff_ESI) {
            do {
              iVar8 = 0;
              if (0 < (int)unaff_EDI) {
                do {
                  iVar13 = (iVar12 + 1) * (int)&uStack_40 - iVar8;
                  iVar9 = iVar12 * (int)unaff_EDI + iVar8;
                  iVar8 = iVar8 + 1;
                  *(undefined *)((int)puVar14 + iVar9) = *(undefined *)(iVar13 + -1 + iVar5);
                  param_1 = uStack_18;
                } while (iVar8 < (int)unaff_EDI);
              }
              iVar12 = iVar12 + 1;
            } while (iVar12 < (int)unaff_ESI);
          }
        }
        else {
          iVar12 = 0;
          if (0 < (int)unaff_ESI) {
            do {
              iVar8 = 0;
              if (0 < (int)unaff_EDI) {
                do {
                  iVar13 = ((int)&piStack_14 - iVar8) * (int)&uStack_40 - iVar8;
                  iVar9 = iVar12 * (int)unaff_EDI + iVar8;
                  iVar8 = iVar8 + 1;
                  *(undefined *)((int)puVar14 + iVar9) = *(undefined *)(iVar13 + -1 + iVar5);
                } while (iVar8 < (int)unaff_EDI);
              }
              iVar12 = iVar12 + 1;
            } while (iVar12 < (int)unaff_ESI);
          }
        }
      }
      local_20 = (param_1->TScreen).mbr_0x18;
      sVar2 = *(short *)&(param_1->TScreen).mbr_0x4;
      uStack_40 = (uint)(sVar2 == 10);
      if (sVar2 == 10) {
        uStack_40 = 1;
      }
      iVar13 = ((int)(param_1->TScreen).mbr_0x10 >> 4) - uVar6;
      iVar7 = ((int)(param_1->TScreen).mbr_0x14 >> 4) - iVar7;
      iVar12 = iVar13 * 0x10 >> 10;
      iVar8 = iVar7 * 0x10 >> 10;
      iVar9 = ((int)ppiStack_58 + iVar7) * 0x10 + -0x10 >> 10;
      iVar5 = (iVar13 + (int)puVar11) * 0x10 + -0x10 >> 10;
      iStack_34 = iVar5;
      if ((iStack_10 == 0) ||
         ((((*(uint *)(iStack_10 + 4) == (uint)(uStack_18->TScreen).mbr_0xe &&
            (iVar12 <= *(int *)(iStack_10 + 8))) && (*(int *)(iStack_10 + 8) <= iVar5)) &&
          ((iVar8 <= *(int *)(iStack_10 + 0xc) && (*(int *)(iStack_10 + 0xc) <= iVar9)))))) {
        iStack_10 = 0;
        if (0 < DAT_00668578) {
          do {
            this_00 = *(cls_0x499560 **)(DAT_00668588 + iStack_10 * 4);
            if (((this_00 != (cls_0x499560 *)0x0) &&
                (*(uint *)&this_00->field_0x4 == (uint)(uStack_18->TScreen).mbr_0xe)) &&
               ((iVar12 <= (int)this_00->mbr_0x8 &&
                ((((int)this_00->mbr_0x8 <= iVar5 && (iVar8 <= (int)this_00->mbr_0xc)) &&
                 ((int)this_00->mbr_0xc <= iVar9)))))) {
              if (piStack_14 == (int *)0x3) {
                puVar10 = puVar14;
                if (puVar14 == (undefined4 *)0x0) {
                  puVar10 = unaff_EBX;
                }
                cls_0x499560::meth_0x499560_TSector_WalkmapHandler
                          (this_00,2,puVar10,0,iVar13,iVar7,puVar11,ppiStack_58,puVar11,0);
                FUN_00452b60(uStack_18,iVar13,iVar7,(int)puVar11,(int)ppiStack_58,this_00);
              }
              else {
                puVar10 = puVar14;
                if (puVar14 == (undefined4 *)0x0) {
                  puVar10 = unaff_EBX;
                }
                cls_0x499560::meth_0x499560_TSector_WalkmapHandler
                          (this_00,piStack_14,puVar10,local_20,iVar13,iVar7,puVar11,ppiStack_58,
                           puVar11,uStack_3c);
                iVar5 = iStack_34;
              }
            }
            iStack_10 = iStack_10 + 1;
          } while (iStack_10 < DAT_00668578);
        }
        if (piStack_14 == (int *)0x1) {
          (**(code **)(*piStack_30 + 0x50))(1);
        }
        if (puVar14 != (undefined4 *)0x0) {
          FUN_00482f80(puVar14);
        }
      }
      else if (puVar14 != (undefined4 *)0x0) {
        FUN_00482f80(puVar14);
        return;
      }
    }
  }
  return;
}



// Function at 00453320

void TCharacter::meth_0x453320()

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  if (0 < DAT_00668578) {
    do {
      iVar1 = *(int *)(DAT_00668588 + iVar2 * 4);
      if ((iVar1 != 0) && (*(int *)(iVar1 + 0x98) != 0)) {
        iVar3 = 0;
        if (0 < *(int *)(iVar1 + 0xb8)) {
          do {
            meth_0x452750(this,*(undefined4 *)(*(int *)(iVar1 + 200) + iVar3 * 4));
            iVar3 = iVar3 + 1;
          } while (iVar3 < *(int *)(iVar1 + 0xb8));
        }
        *(undefined4 *)(iVar1 + 0xa8) = 0;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < DAT_00668578);
  }
  return;
}



// Function at 00453640

void TCharacter::meth_0x453640(int param_1)

{
  uint uVar1;
  int in_stack_00000008;
  
  uVar1 = param_1 + (DAT_005d7a48 * 5 + -10) * 2;
  uVar1 = uVar1 & ((int)uVar1 < 0) - 1;
  if (in_stack_00000008 != 0) {
    this->mbr_0x8f0 = 0;
    this->mbr_0x8e8 = 0;
  }
  if (0 < (int)this->mbr_0x8e8) {
    this->mbr_0x8dc = uVar1;
    return;
  }
  if (this->mbr_0x8cc != uVar1) {
    this->mbr_0x8cc = uVar1;
    this->mbr_0x8d4 = 1;
  }
  return;
}



// Function at 004536b0

void TCharacter::meth_0x4536b0(dword *param_1)

{
  dword dVar1;
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    this->mbr_0x8f0 = 0;
    this->mbr_0x8e8 = 0;
  }
  if (0 < (int)this->mbr_0x8e8) {
    this->mbr_0x8e4 = *param_1;
    return;
  }
  if (((*(char *)((int)&this->mbr_0x8d0 + 2) != *(char *)((int)param_1 + 2)) ||
      (*(char *)((int)&this->mbr_0x8d0 + 1) != *(char *)((int)param_1 + 1))) ||
     (*(char *)&this->mbr_0x8d0 != *(char *)param_1)) {
    dVar1 = *param_1;
    this->mbr_0x8d4 = 1;
    this->mbr_0x8d0 = dVar1;
  }
  return;
}



// Function at 00453720

void TCharacter::meth_0x453720(dword param_1, dword *param_2, dword param_3)

{
  dword dVar1;
  dword dVar2;
  dword in_stack_00000010;
  
  dVar2 = param_3;
  if (DAT_005d79e4 != 5) {
    dVar2 = in_stack_00000010;
  }
  if ((((param_1 != this->mbr_0x8cc) ||
       (*(char *)((int)param_2 + 2) != *(char *)((int)&this->mbr_0x8d0 + 2))) ||
      (*(char *)((int)param_2 + 1) != *(char *)((int)&this->mbr_0x8d0 + 1))) ||
     (*(char *)param_2 != *(char *)&this->mbr_0x8d0)) {
    dVar1 = *param_2;
    this->mbr_0x8d8 = this->mbr_0x8cc;
    this->mbr_0x8e4 = dVar1;
    this->mbr_0x8e0 = this->mbr_0x8d0;
    this->mbr_0x8dc = param_1;
    this->mbr_0x8e8 = param_3;
    this->mbr_0x8f0 = dVar2;
    this->mbr_0x8ec = 0;
    this->mbr_0x8f4 = 0;
  }
  return;
}



// Function at 004537b0

void TCharacter::meth_0x4537b0()

{
  byte bVar1;
  byte bVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  
  if ((0 < (int)this->mbr_0x8e8) && (dVar3 = this->mbr_0x8f0, 0 < (int)dVar3)) {
    dVar4 = this->mbr_0x8f4;
    if (this->mbr_0x8ec != (int)(dVar4 * this->mbr_0x8e8) / (int)dVar3) {
      this->mbr_0x8ec = this->mbr_0x8ec + 1;
      return;
    }
    iVar5 = dVar3 - dVar4;
    this->mbr_0x8d4 = 1;
    bVar1 = *(byte *)((int)&this->mbr_0x8e0 + 2);
    this->mbr_0x8cc =
         (int)(this->mbr_0x8d8 * iVar5) / (int)dVar3 + (int)(this->mbr_0x8dc * dVar4) / (int)dVar3;
    bVar2 = *(byte *)((int)&this->mbr_0x8e0 + 1);
    *(char *)((int)&this->mbr_0x8d0 + 2) =
         (char)((int)((uint)bVar1 * iVar5) / (int)dVar3) +
         (char)((int)(*(byte *)((int)&this->mbr_0x8e4 + 2) * dVar4) / (int)dVar3);
    bVar1 = *(byte *)&this->mbr_0x8e0;
    *(char *)((int)&this->mbr_0x8d0 + 1) =
         (char)((int)((uint)bVar2 * iVar5) / (int)dVar3) +
         (char)((int)(*(byte *)((int)&this->mbr_0x8e4 + 1) * dVar4) / (int)dVar3);
    *(char *)&this->mbr_0x8d0 =
         (char)((int)((uint)bVar1 * iVar5) / (int)dVar3) +
         (char)((int)(*(byte *)&this->mbr_0x8e4 * dVar4) / (int)dVar3);
    this->mbr_0x8ec = this->mbr_0x8ec + 1;
    this->mbr_0x8f4 = dVar4 + 1;
    if ((int)dVar3 < (int)(dVar4 + 1)) {
      this->mbr_0x8f0 = 0;
      this->mbr_0x8e8 = 0;
    }
  }
  return;
}



// Function at 004538d0

void TCharacter::meth_0x4538d0(dword param_1)

{
  int iVar1;
  uint in_stack_00000008;
  
  if ((((*(byte *)&this->mbr_0xd8 & 1) != 0) && (this->mbr_0xdc == DAT_00667fcc)) &&
     (param_1 != DAT_00667fcc)) {
    cls_0x535a10::meth_0x535d80((cls_0x535a10 *)&DAT_00667cc8);
    iVar1 = meth_0x47ed20((TCharacter *)&DAT_0065caf0);
    if (iVar1 == 3) {
      meth_0x47ecc0((TCharacter *)&DAT_0065caf0);
    }
  }
  this->mbr_0xdc = param_1;
  this->mbr_0xd8 = in_stack_00000008 & 0xfffffffd | 1;
  return;
}



// Function at 00453940

void TCharacter::meth_0x453940(dword *param_1, dword param_2)

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  uint in_stack_0000000c;
  
  if (((*(byte *)&this->mbr_0xd8 & 1) != 0) && (this->mbr_0xdc == DAT_00667fcc)) {
    cls_0x535a10::meth_0x535d80((cls_0x535a10 *)&DAT_00667cc8);
    iVar3 = meth_0x47ed20((TCharacter *)&DAT_0065caf0);
    if (iVar3 == 3) {
      meth_0x47ecc0((TCharacter *)&DAT_0065caf0);
    }
  }
  dVar1 = *param_1;
  dVar2 = param_1[2];
  this->mbr_0xe4 = param_1[1];
  this->mbr_0xe0 = dVar1;
  this->mbr_0xe8 = dVar2;
  this->mbr_0xec = param_2;
  this->mbr_0xd8 = in_stack_0000000c & 0xfffffffe | 2;
  return;
}



// Function at 004539d0

void TCharacter::meth_0x4539d0()

{
  dword *this_00;
  dword dVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  dword dVar6;
  int local_34;
  int local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  dword local_18;
  dword local_14;
  dword local_10;
  dword local_c;
  dword local_8;
  dword local_4;
  
  if (DAT_00668154 != 0) {
    if (((this->mbr_0x114 != 0) || (this->mbr_0x118 != 0)) && (DAT_006671d0 == 0)) {
      FUN_0046dad0(this->mbr_0x114,this->mbr_0x118,(int *)&local_c,0);
      local_4 = local_4 + this->mbr_0xac;
      local_8 = local_8 + this->mbr_0xa8;
      local_c = local_c + this->mbr_0xa4;
      FUN_0046d7a0(&local_c,&local_24,&local_28);
      iVar2 = DAT_0065c5c4 / 2;
      *(int *)&(this->TObjectInstance).field_0x34 = local_24 - DAT_00667c30 / 2;
      dVar6 = this->mbr_0xb0;
      *(int *)&(this->TObjectInstance).field_0x38 = local_28 - iVar2;
      if (((local_c != dVar6) || (local_8 != this->mbr_0xb4)) || (local_4 != this->mbr_0xb8)) {
        cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_c,local_8);
      }
      cls_0x41c750::meth_0x41c750((cls_0x41c750 *)&this->mbr_0xa4);
      UNK_00656e7c._76_4_ = 1;
    }
    this_00 = &this->mbr_0xa4;
    FUN_0046d7a0(this_00,&local_24,&local_28);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)&(this->TObjectInstance).field_0x34 = local_24 - DAT_00667c30 / 2;
    dVar6 = this->mbr_0xb0;
    dVar1 = *this_00;
    *(int *)&(this->TObjectInstance).field_0x38 = local_28 - iVar2;
    if (((dVar1 != dVar6) || (this->mbr_0xa8 != this->mbr_0xb4)) ||
       (this->mbr_0xac != this->mbr_0xb8)) {
      cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,dVar1,this->mbr_0xa8);
    }
    cls_0x41c750::meth_0x41c750((cls_0x41c750 *)this_00);
    if (DAT_006671d0 == 0) {
      return;
    }
    meth_0x4546a0(this);
    UNK_00656e7c._76_4_ = 1;
    return;
  }
  local_18 = this->mbr_0xb0;
  local_14 = this->mbr_0xb4;
  local_10 = this->mbr_0xb8;
  dVar6 = this->mbr_0x98;
  local_2c = dVar6;
  if ((DAT_00658440 & 1) == 0) {
    DAT_00658440 = DAT_00658440 | 1;
    DAT_00658468 = 0;
    DAT_0065846c = 0;
    DAT_00658470 = 0;
    FUN_0058b66c(0x4541b0);
  }
  if ((DAT_00658440 & 2) == 0) {
    DAT_00658440 = DAT_00658440 | 2;
    FUN_0058b66c(0x4541a0);
  }
  if (((this->mbr_0xd8 & 1) == 0) || (this->mbr_0x918 == 1)) {
    if (((this->mbr_0xd8 & 2) == 0) || (this->mbr_0x918 == 1)) {
      dVar6 = this->mbr_0x98;
      local_c = local_18;
      local_8 = local_14;
      local_4 = local_10;
      local_2c = dVar6;
    }
    else {
      local_c = this->mbr_0xe0;
      local_8 = this->mbr_0xe4;
      local_4 = this->mbr_0xe8;
      dVar6 = this->mbr_0xec;
      local_2c = dVar6;
    }
  }
  else {
    dVar1 = this->mbr_0xdc;
    if (dVar1 != 0) {
      local_c = *(dword *)(dVar1 + 0x10);
      local_8 = *(dword *)(dVar1 + 0x14);
      local_4 = *(dword *)(dVar1 + 0x18);
      dVar6 = (dword)*(ushort *)(dVar1 + 0xe);
      local_2c = dVar6;
    }
  }
  iVar2 = DAT_00658320 - local_4;
  if (iVar2 < 0) {
    iVar2 = local_4 - DAT_00658320;
  }
  if (iVar2 < 9) {
    local_4 = DAT_00658320;
  }
  if (((DAT_005d7a04 == 0) && (DAT_0065d0d0 != 0)) || (dVar6 != this->mbr_0x98)) {
LAB_00453e5a:
    DAT_00658470 = 0;
    DAT_0065846c = 0;
    DAT_00658468 = 0;
    local_14 = local_8;
    local_18 = local_c;
    local_10 = local_4;
  }
  else {
    dVar6 = this->mbr_0xa4;
    iVar2 = dVar6 - local_c;
    if (iVar2 < 0) {
      iVar2 = local_c - dVar6;
    }
    iVar3 = this->mbr_0xa8 - local_8;
    if (iVar3 < 0) {
      iVar3 = local_8 - this->mbr_0xa8;
    }
    iVar4 = iVar2;
    if (iVar3 <= iVar2) {
      iVar4 = iVar3;
    }
    if (((0x3ff < (iVar3 - (iVar4 >> 1)) + iVar2) || ((this->mbr_0xd8 & 4) == 0)) ||
       ((this->mbr_0xd8 & 8) != 0)) goto LAB_00453e5a;
    iVar2 = 10;
    if (DAT_005d7a08 != 0) {
      iVar2 = 6;
    }
    iVar3 = (int)(local_c - dVar6) / iVar2;
    iVar4 = (int)(local_8 - this->mbr_0xa8) / iVar2;
    iVar2 = (int)(local_4 - this->mbr_0xac) / iVar2;
    if (iVar3 + iVar4 < 0x100) {
      if (DAT_00658468 < iVar3) {
        DAT_00658468 = DAT_00658468 + 1;
      }
      else if (iVar3 < DAT_00658468) {
        DAT_00658468 = DAT_00658468 + -1;
      }
      if (DAT_0065846c < iVar4) {
        DAT_0065846c = DAT_0065846c + 1;
      }
      else if (iVar4 < DAT_0065846c) {
        DAT_0065846c = DAT_0065846c + -1;
      }
      if (DAT_00658470 < iVar2) {
        DAT_00658470 = DAT_00658470 + 1;
      }
      else if (iVar2 < DAT_00658470) {
        DAT_00658470 = DAT_00658470 + -1;
      }
      if (DAT_00658468 < 0xd) {
        if (DAT_00658468 < -0xc) {
          DAT_00658468 = -0xc;
        }
      }
      else {
        DAT_00658468 = 0xc;
      }
      if (DAT_0065846c < 0xd) {
        if (DAT_0065846c < -0xc) {
          DAT_0065846c = -0xc;
        }
      }
      else {
        DAT_0065846c = 0xc;
      }
      if (DAT_00658470 < 0xd) {
        if (DAT_00658470 < -0xc) {
          DAT_00658470 = -0xc;
        }
      }
      else {
        DAT_00658470 = 0xc;
      }
      local_10 = local_10 + DAT_00658470;
      local_14 = local_14 + DAT_0065846c;
      local_18 = local_18 + DAT_00658468;
    }
    else {
      local_18 = local_c;
      DAT_00658470 = 0;
      DAT_0065846c = 0;
      DAT_00658468 = 0;
      local_14 = local_8;
      local_10 = local_4;
    }
  }
  DAT_00658320 = local_4;
  if ((DAT_005d7a04 == 0) && (DAT_0065d0d0 != 0)) {
    FUN_0046d7a0(&local_c,&local_34,&local_30);
    iVar2 = local_34;
    if (local_34 < 1) {
      iVar2 = -local_34;
    }
    iVar3 = (DAT_00667c30 + -0x40) / 2;
    if (iVar2 < iVar3) {
      local_34 = 0;
    }
    else {
      if (local_34 < 0) {
        iVar3 = -iVar3;
      }
      local_34 = (local_34 + iVar3) / (DAT_00667c30 + -0x40);
    }
    local_30 = local_30 + -0x20;
    iVar2 = local_30;
    if (local_30 < 1) {
      iVar2 = -local_30;
    }
    iVar3 = (DAT_0065c5c4 + -0x40) / 2;
    if (iVar2 < iVar3) {
      local_30 = 0;
    }
    else {
      if (local_30 < 0) {
        iVar3 = -iVar3;
      }
      local_30 = (local_30 + iVar3) / (DAT_0065c5c4 + -0x40);
    }
    FUN_0046d7a0(&this->mbr_0xa4,&local_28,&local_24);
    iVar2 = DAT_00667c30 + -0x40;
    iVar3 = local_28;
    if (local_28 < 1) {
      iVar3 = -local_28;
    }
    iVar4 = iVar2 / 2;
    if (iVar3 < iVar4) {
      local_28 = 0;
    }
    else {
      if (local_28 < 0) {
        iVar4 = -iVar4;
      }
      local_28 = (local_28 + iVar4) / iVar2;
    }
    iVar3 = DAT_0065c5c4 + -0x40;
    iVar4 = local_24;
    if (local_24 < 1) {
      iVar4 = -local_24;
    }
    iVar5 = iVar3 / 2;
    if (iVar4 < iVar5) {
      local_24 = 0;
    }
    else {
      if (local_24 < 0) {
        iVar5 = -iVar5;
      }
      local_24 = (local_24 + iVar5) / iVar3;
    }
    local_30 = iVar3 * local_30;
    local_34 = iVar2 * local_34;
    FUN_0046dad0(local_34,local_30,(int *)&local_18,0);
    iVar2 = local_18 - this->mbr_0xa4;
    if (iVar2 < 1) {
      iVar2 = this->mbr_0xa4 - local_18;
    }
    if (iVar2 < 5) {
      iVar2 = local_14 - this->mbr_0xa8;
      if (iVar2 < 1) {
        iVar2 = this->mbr_0xa8 - local_14;
      }
      if (iVar2 < 5) goto LAB_00454138;
    }
    FUN_0046d7a0(&local_18,&local_20,&local_1c);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)&(this->TObjectInstance).field_0x34 = local_20 - DAT_00667c30 / 2;
    dVar6 = this->mbr_0xb0;
    *(int *)&(this->TObjectInstance).field_0x38 = local_1c - iVar2;
    if (((local_18 != dVar6) || (local_14 != this->mbr_0xb4)) || (local_10 != this->mbr_0xb8)) {
      cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_18,local_14);
    }
    this->mbr_0xac = local_10;
    this->mbr_0xa4 = local_18;
    this->mbr_0xa8 = local_14;
    meth_0x4546a0((TCharacter *)&DAT_006668d8);
  }
  else {
    FUN_0046d7a0(&local_18,&local_1c,&local_20);
    iVar2 = DAT_0065c5c4 / 2;
    *(int *)&(this->TObjectInstance).field_0x34 = local_1c - DAT_00667c30 / 2;
    dVar6 = this->mbr_0xb0;
    *(int *)&(this->TObjectInstance).field_0x38 = local_20 - iVar2;
    if ((local_18 != dVar6) || ((local_14 != this->mbr_0xb4 || (local_10 != this->mbr_0xb8)))) {
      cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_18,local_14);
    }
    this->mbr_0xa4 = local_18;
    this->mbr_0xa8 = local_14;
    this->mbr_0xac = local_10;
  }
LAB_00454138:
  this->mbr_0x98 = local_2c;
  if (local_2c != this->mbr_0x9c) {
    meth_0x4546a0(this);
  }
  UNK_00658314._4_4_ = local_c;
  DAT_006584ac._0_4_ = this->mbr_0x9c;
  UNK_00658314._8_4_ = local_8;
  DAT_00658320 = local_4;
  this->mbr_0xd8 = this->mbr_0xd8 & 0xfffffff7;
  return;
}



// Function at 00454390

void TCharacter::virt_meth_0x454390()

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  int local_8;
  int local_4;
  
  if (this->mbr_0x9a8 != DAT_005d7a18) {
    meth_0x45b080(this);
  }
  if (this->mbr_0x8f8 != 0) {
    meth_0x45f1e0(this);
  }
  meth_0x4537b0(this);
  FUN_004580f0();
  FUN_00458390();
  meth_0x4539d0(this);
  FUN_0046d7a0(&this->mbr_0xa4,&local_8,&local_4);
  iVar3 = DAT_0065c5c4 / 2;
  *(int *)&(this->TObjectInstance).field_0x34 = local_8 - DAT_00667c30 / 2;
  *(int *)&(this->TObjectInstance).field_0x38 = local_4 - iVar3;
  cls_0x41c410::meth_0x41c410_TAreaMgr_Pulse((cls_0x41c410 *)&DAT_0065b8b0);
  dVar1 = *(dword *)&(this->TObjectInstance).field_0x2c;
  this->mbr_0x80 = this->mbr_0x78;
  dVar2 = *(dword *)&(this->TObjectInstance).field_0x30;
  this->mbr_0x78 = dVar1;
  this->mbr_0x84 = this->mbr_0x7c;
  this->mbr_0x7c = dVar2;
  meth_0x459220(this);
  return;
}



// Function at 004546a0

void TCharacter::meth_0x4546a0()

{
  TObjectInstance__vftable_5a5ed4 *pcVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int unaff_ESI;
  int iStack_28;
  int iStack_24;
  int iStack_20;
  dword dStack_1c;
  dword dStack_18;
  dword dStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  
  if (this->mbr_0xf4 == 0) {
    pcVar1 = (this->TObjectInstance).vftptr_0x0;
    this->mbr_0xf4 = 1;
    (*pcVar1->virt_meth_0x490530_44)(this);
    FUN_0041d9f0();
    DAT_00658d94 = 1;
    if ((((DAT_00668154 == 0) && (DAT_00667fcc != 0)) && (DAT_005d7a04 == 0)) && (DAT_0065d0d0 != 0)
       ) {
      uStack_10 = *(undefined4 *)(DAT_00667fcc + 0x10);
      uStack_c = *(undefined4 *)(DAT_00667fcc + 0x14);
      uStack_8 = *(undefined4 *)(DAT_00667fcc + 0x18);
      FUN_0046d7a0(&uStack_10,(undefined4 *)&stack0xffffffd4,&iStack_28);
      iVar5 = DAT_00667c30 + -0x40;
      iVar6 = unaff_ESI;
      if (unaff_ESI < 1) {
        iVar6 = -unaff_ESI;
      }
      iVar3 = iVar5 / 2;
      if (iVar6 < iVar3) {
        iVar6 = 0;
      }
      else {
        if (unaff_ESI < 0) {
          iVar3 = -iVar3;
        }
        iVar6 = (unaff_ESI + iVar3) / iVar5;
      }
      iStack_28 = iStack_28 + -0x20;
      iVar3 = DAT_0065c5c4 + -0x40;
      iVar7 = iStack_28;
      if (iStack_28 < 1) {
        iVar7 = -iStack_28;
      }
      iVar4 = iVar3 / 2;
      if (iVar7 < iVar4) {
        iStack_28 = 0;
      }
      else {
        if (iStack_28 < 0) {
          iVar4 = -iVar4;
        }
        iStack_28 = (iStack_28 + iVar4) / iVar3;
      }
      iStack_28 = iVar3 * iStack_28;
      FUN_0046dad0(iVar5 * iVar6,iStack_28,(int *)&dStack_1c,0);
      iVar5 = dStack_1c - this->mbr_0xb0;
      if (iVar5 < 1) {
        iVar5 = this->mbr_0xb0 - dStack_1c;
      }
      if (iVar5 < 5) {
        iVar5 = dStack_18 - this->mbr_0xb4;
        if (iVar5 < 1) {
          iVar5 = this->mbr_0xb4 - dStack_18;
        }
        if (iVar5 < 5) {
          return;
        }
      }
      FUN_0046d7a0(&dStack_1c,&iStack_24,&iStack_20);
      iVar5 = DAT_0065c5c4 / 2;
      *(int *)&(this->TObjectInstance).field_0x34 = iStack_24 - DAT_00667c30 / 2;
      dVar2 = this->mbr_0xb0;
      *(int *)&(this->TObjectInstance).field_0x38 = iStack_20 - iVar5;
      if (((dStack_1c != dVar2) || (dStack_18 != this->mbr_0xb4)) || (dStack_14 != this->mbr_0xb8))
      {
        cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,dStack_1c,dStack_18);
      }
      this->mbr_0xa4 = dStack_1c;
      this->mbr_0xa8 = dStack_18;
      this->mbr_0xac = dStack_14;
    }
  }
  return;
}



// Function at 004548a0

void TCharacter::meth_0x4548a0(dword *param_1)

{
  dword *pdVar1;
  dword dVar2;
  dword dVar3;
  dword in_stack_00000008;
  
  if (((this->TObjectInstance).mbr_0x50 == 0) && ((int)in_stack_00000008 < 4)) {
    dVar2 = this->mbr_0x130;
    if (0x3f < (int)dVar2) {
      meth_0x4546a0(this);
      this->mbr_0x130 = 0;
      return;
    }
    pdVar1 = &this->mbr_0x134 + dVar2 * 7;
    *pdVar1 = *param_1;
    pdVar1[1] = param_1[1];
    dVar3 = param_1[3];
    pdVar1[2] = param_1[2];
    pdVar1[3] = dVar3;
    (&this->mbr_0x144)[dVar2 * 7] = in_stack_00000008;
    this->mbr_0x130 = this->mbr_0x130 + 1;
  }
  return;
}



// Function at 00454920

void TCharacter::meth_0x454920()

{
  dword *pdVar1;
  dword dVar2;
  dword dVar3;
  dword unaff_EBX;
  int in_stack_00000004;
  dword dStack_58;
  dword dStack_54;
  dword dStack_50;
  cls_0x44ceb0 local_48;
  
  if (-1 < in_stack_00000004) {
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
    while ((int *)local_48.mbr_0xc != (int *)0x0) {
      if (*(int *)(local_48.mbr_0xc + 0x40) == in_stack_00000004) goto LAB_00454975;
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
    local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00454975:
    if (((((int *)local_48.mbr_0xc != (int *)0x0) &&
         (dVar3 = (**(code **)(*(int *)local_48.mbr_0xc + 0xfc))(), dVar3 != 4)) &&
        ((**(code **)(*(int *)local_48.mbr_0xc + 0xf4))(&dStack_58),
        (this->TObjectInstance).mbr_0x50 == 0)) && ((int)dVar3 < 4)) {
      dVar2 = this->mbr_0x130;
      if (0x3f < (int)dVar2) {
        meth_0x4546a0(this);
        this->mbr_0x130 = 0;
        return;
      }
      pdVar1 = &this->mbr_0x134 + dVar2 * 7;
      *pdVar1 = unaff_EBX;
      pdVar1[1] = dStack_58;
      pdVar1[2] = dStack_54;
      pdVar1[3] = dStack_50;
      (&this->mbr_0x144)[dVar2 * 7] = dVar3;
      this->mbr_0x130 = this->mbr_0x130 + 1;
    }
  }
  return;
}



// Function at 00454a30

void TCharacter::meth_0x454a30()

{
  FUN_00446740();
  meth_0x4546a0(this);
  return;
}



// Function at 00454dd0

void TCharacter::meth_0x454dd0()

{
  int iVar1;
  HANDLE hMutex;
  HANDLE pvVar2;
  int iVar3;
  DWORD DVar4;
  DWORD DVar5;
  undefined4 *puVar6;
  
  if (DAT_0065844c != 0) {
    iVar3 = DAT_00658460;
    if (this->mbr_0x834 != 0) {
      if (DAT_00658324 == 0) {
        this->mbr_0x848 = this->mbr_0x838;
        this->mbr_0x84c = this->mbr_0x83c;
        this->mbr_0x850 = this->mbr_0x840;
        this->mbr_0x854 = this->mbr_0x844;
        this->mbr_0x834 = 0;
        meth_0x4562a0(this);
        iVar3 = DAT_00658460;
        if (DAT_0065844c != 0) {
          if ((DAT_00658324 != 0) && ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
            GetTickCount();
            pvVar2 = DAT_006584b4;
            iVar3 = ReleaseMutex(DAT_006584b4);
            hMutex = DAT_00658310;
            while (DAT_00658310 = hMutex, iVar3 != 0) {
              iVar3 = ReleaseMutex(pvVar2);
              hMutex = DAT_00658310;
            }
            iVar3 = ReleaseMutex(hMutex);
            pvVar2 = DAT_006584b8;
            while (DAT_006584b8 = pvVar2, iVar3 != 0) {
              iVar3 = ReleaseMutex(hMutex);
              pvVar2 = DAT_006584b8;
            }
            iVar3 = ReleaseMutex(pvVar2);
            while (iVar3 != 0) {
              iVar3 = ReleaseMutex(pvVar2);
            }
            if (DAT_00658478 == 0) {
              while (DAT_00658324 != 0) {
                SetEvent(DAT_00658458);
                WaitForSingleObject(DAT_006584a8,0xffffffff);
                ResetEvent(DAT_006584a8);
              }
            }
            else {
              ResetEvent(DAT_00658458);
              if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
                iVar3 = 0;
                if (0 < DAT_00658438) {
                  puVar6 = &DAT_006584c8;
                  do {
                    meth_0x456810(this,puVar6);
                    iVar3 = iVar3 + 1;
                    puVar6 = puVar6 + 7;
                  } while (iVar3 < DAT_00658438);
                }
                DAT_00658324 = 0;
                DAT_006584c0 = 0;
                SetEvent(DAT_006584a8);
              }
            }
            DAT_006584c0 = 0;
            GetTickCount();
          }
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
          iVar3 = DAT_00658460;
        }
      }
      else if (0 < (int)DAT_0065843c) {
        DVar4 = GetTickCount();
        SetEvent(DAT_00658458);
        WaitForSingleObject(DAT_006584a8,DAT_0065843c);
        ResetEvent(DAT_006584a8);
        DVar5 = GetTickCount();
        iVar3 = DAT_00658460;
        (&DAT_006582fc)[DAT_00658460] = (&DAT_006582fc)[DAT_00658460] + (DVar5 - DVar4);
      }
    }
    iVar1 = (&DAT_00658494)[iVar3];
    (&DAT_00658494)[iVar3] = iVar1 + 1;
    if (0x18 < iVar1 + 1) {
      FUN_00455000(0);
    }
  }
  return;
}



// Function at 004550f0

void TCharacter::meth_0x4550f0()

{
  HANDLE hMutex;
  HANDLE pvVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if ((((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) && (DAT_0065844c != 0)) &&
     ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
    if (DAT_00658324 == 0) {
      DAT_006584c0 = 0;
    }
    else {
      GetTickCount();
      pvVar1 = DAT_006584b4;
      iVar2 = ReleaseMutex(DAT_006584b4);
      hMutex = DAT_00658310;
      while (DAT_00658310 = hMutex, iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
        hMutex = DAT_00658310;
      }
      iVar2 = ReleaseMutex(hMutex);
      pvVar1 = DAT_006584b8;
      while (DAT_006584b8 = pvVar1, iVar2 != 0) {
        iVar2 = ReleaseMutex(hMutex);
        pvVar1 = DAT_006584b8;
      }
      iVar2 = ReleaseMutex(pvVar1);
      while (iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
      }
      if (DAT_00658478 == 0) {
        while (DAT_00658324 != 0) {
          SetEvent(DAT_00658458);
          WaitForSingleObject(DAT_006584a8,0xffffffff);
          ResetEvent(DAT_006584a8);
        }
      }
      else {
        ResetEvent(DAT_00658458);
        if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
          iVar2 = 0;
          if (0 < DAT_00658438) {
            puVar3 = &DAT_006584c8;
            do {
              meth_0x456810(this,puVar3);
              iVar2 = iVar2 + 1;
              puVar3 = puVar3 + 7;
            } while (iVar2 < DAT_00658438);
          }
          DAT_00658324 = 0;
          DAT_006584c0 = 0;
          SetEvent(DAT_006584a8);
        }
      }
      DAT_006584c0 = 0;
      GetTickCount();
    }
  }
  if (this->mbr_0x834 != 0) {
    this->mbr_0x848 = this->mbr_0x838;
    this->mbr_0x84c = this->mbr_0x83c;
    this->mbr_0x850 = this->mbr_0x840;
    this->mbr_0x854 = this->mbr_0x844;
    this->mbr_0x834 = 0;
  }
  meth_0x4562a0(this);
  if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
    FUN_00482130();
    DAT_00658438 = 0;
    FUN_00482140();
  }
  return;
}



// Function at 004552b0

void TCharacter::meth_0x4552b0(dword param_1)

{
  dword dVar1;
  int iVar2;
  undefined3 extraout_var;
  int iVar3;
  dword *pdVar4;
  bool bVar5;
  dword in_stack_00000008;
  int local_74;
  dword local_70;
  dword local_6c;
  dword local_68;
  dword local_64;
  cls_0x45f6f0 local_60;
  dword local_50;
  dword local_4c;
  dword local_48;
  dword local_44;
  int local_40 [16];
  
  dVar1 = this->mbr_0x8c;
  this->mbr_0x90 = this->mbr_0x88;
  this->mbr_0x8c = in_stack_00000008;
  bVar5 = DAT_0065844c != 0;
  this->mbr_0x94 = dVar1;
  this->mbr_0x88 = param_1;
  if (((bVar5) && (this->mbr_0x918 == 0)) && (DAT_00658324 == 0)) {
    if (((this->mbr_0x9a8 == 0) && (DAT_005d7a54 != 0)) || (DAT_00658478 != 0)) {
      if (((this->mbr_0x9a8 == 0) && (DAT_005d7a54 != 0)) && (DAT_00658478 == 1)) {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          this->mbr_0x848 = this->mbr_0x838;
          this->mbr_0x84c = this->mbr_0x83c;
          this->mbr_0x850 = this->mbr_0x840;
          this->mbr_0x854 = this->mbr_0x844;
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if (DAT_00658324 == 0) {
          meth_0x455f90(this);
        }
        DAT_00658478 = 0;
      }
    }
    else {
      if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
        meth_0x455e40(this);
      }
      if (this->mbr_0x834 != 0) {
        this->mbr_0x848 = this->mbr_0x838;
        this->mbr_0x84c = this->mbr_0x83c;
        this->mbr_0x850 = this->mbr_0x840;
        this->mbr_0x854 = this->mbr_0x844;
        this->mbr_0x834 = 0;
      }
      meth_0x4562a0(this);
      if (DAT_00658324 == 0) {
        meth_0x455f90(this);
      }
      DAT_00658478 = 1;
    }
    if ((this->mbr_0x834 != 0) && (DAT_00658324 == 0)) {
      if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
        meth_0x455e40(this);
      }
      if (this->mbr_0x834 != 0) {
        this->mbr_0x848 = this->mbr_0x838;
        this->mbr_0x84c = this->mbr_0x83c;
        this->mbr_0x850 = this->mbr_0x840;
        this->mbr_0x854 = this->mbr_0x844;
        this->mbr_0x834 = 0;
      }
      meth_0x4562a0(this);
      if (DAT_00658324 == 0) {
        meth_0x455f90(this);
      }
    }
    if (this->mbr_0x9c != this->mbr_0xa0) {
      this->mbr_0xf4 = 1;
    }
    local_70 = this->mbr_0x88;
    if (((local_70 == this->mbr_0x90) && (this->mbr_0x8c == this->mbr_0x94)) &&
       (this->mbr_0xf4 == 0)) {
      if (0 < (int)this->mbr_0x130) {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          cls_0x41c700::meth_0x41c700((cls_0x41c700 *)&this->mbr_0x848);
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
        }
        local_74 = 0;
        if (0 < (int)this->mbr_0x130) {
          pdVar4 = &this->mbr_0x144;
          do {
            iVar2 = FUN_004ad610((int *)&this->mbr_0x848,(int *)(pdVar4 + -4),(int *)&local_50);
            if ((iVar2 != 0) && (dVar1 = *pdVar4, DAT_0065844c != 0)) {
              if (DAT_00658324 != 0) {
                meth_0x4550f0(this);
              }
              if (DAT_00658438 < 0x50) {
                FUN_00482130();
                cls_0x41c700::meth_0x41c700((cls_0x41c700 *)(&DAT_006584c8 + DAT_00658438 * 7));
                (&DAT_006584d8)[DAT_00658438 * 7] = dVar1;
                (&DAT_006584dc)[DAT_00658438 * 7] = 0;
                (&DAT_006584e0)[DAT_00658438 * 7] = 0;
                DAT_00658438 = DAT_00658438 + 1;
                FUN_00482140();
              }
            }
            local_74 = local_74 + 1;
            pdVar4 = pdVar4 + 7;
          } while (local_74 < (int)this->mbr_0x130);
        }
        iVar2 = DAT_0065844c;
        this->mbr_0x130 = 0;
        if (iVar2 != 0) {
          FUN_00482130();
          DAT_00658324 = 1;
          DAT_006584c0 = 0;
          if (DAT_00658478 == 0) {
            SetEvent(DAT_00658458);
          }
          FUN_00482140();
          if (DAT_00658478 != 0) {
            meth_0x456330(this);
          }
        }
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          cls_0x41c700::meth_0x41c700((cls_0x41c700 *)&this->mbr_0x848);
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
          return;
        }
      }
    }
    else {
      local_6c = this->mbr_0x8c;
      local_68 = (local_70 - 0x41) + DAT_0065ba00;
      local_64 = (local_6c - 0x41) + DAT_00667c50;
      local_60.mbr_0x0 = local_70;
      local_60.mbr_0x4 = local_6c;
      local_60.mbr_0x8 = local_68;
      local_60.mbr_0xc = local_64;
      if ((this->mbr_0xf4 == 0) &&
         (((((pdVar4 = &this->mbr_0x848, (int)local_70 <= (int)this->mbr_0x850 &&
             ((int)*pdVar4 <= (int)local_68)) && ((int)local_6c <= (int)this->mbr_0x854)) &&
           ((int)this->mbr_0x84c <= (int)local_64)) ||
          ((((int)local_70 <= (int)this->mbr_0x840 && ((int)this->mbr_0x838 <= (int)local_68)) &&
           (((int)local_6c <= (int)this->mbr_0x844 && ((int)this->mbr_0x83c <= (int)local_64))))))))
      {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          cls_0x41c700::meth_0x41c700((cls_0x41c700 *)pdVar4);
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if (DAT_00658324 == 0) {
          meth_0x455f90(this);
        }
        if ((int)this->mbr_0x90 < (int)this->mbr_0x88) {
          local_68 = local_68 + 0x40;
        }
        else if ((int)this->mbr_0x88 < (int)this->mbr_0x90) {
          local_70 = local_70 - 0x40;
        }
        else {
          local_70 = *pdVar4;
          local_68 = this->mbr_0x850;
        }
        if ((int)this->mbr_0x94 < (int)this->mbr_0x8c) {
          local_64 = local_64 + 0x40;
        }
        else if ((int)this->mbr_0x8c < (int)this->mbr_0x94) {
          local_6c = local_6c - 0x40;
        }
        else {
          local_6c = this->mbr_0x84c;
          local_64 = this->mbr_0x854;
        }
        bVar5 = FUN_004ad6a0((int *)&local_70,(int *)pdVar4,local_40,&local_74);
        if ((CONCAT31(extraout_var,bVar5) != 0) && (iVar2 = 0, 0 < local_74)) {
          do {
            if (DAT_0065844c != 0) {
              if (DAT_00658324 != 0) {
                meth_0x4550f0(this);
              }
              if (DAT_00658438 < 0x50) {
                FUN_00482130();
                cls_0x41c700::meth_0x41c700((cls_0x41c700 *)(&DAT_006584c8 + DAT_00658438 * 7));
                (&DAT_006584d8)[DAT_00658438 * 7] = 0;
                (&DAT_006584dc)[DAT_00658438 * 7] = 0;
                (&DAT_006584e0)[DAT_00658438 * 7] = 0;
                DAT_00658438 = DAT_00658438 + 1;
                FUN_00482140();
              }
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < local_74);
        }
        FUN_004ad610((int *)&this->mbr_0x848,(int *)&local_70,(int *)&this->mbr_0x848);
        iVar2 = 0;
        if (0 < (int)this->mbr_0x130) {
          pdVar4 = &this->mbr_0x144;
          do {
            local_50 = pdVar4[-4];
            local_4c = pdVar4[-3];
            local_48 = pdVar4[-2];
            local_44 = pdVar4[-1];
            iVar3 = FUN_004ad610((int *)&local_50,(int *)&this->mbr_0x848,(int *)&local_50);
            if (iVar3 != 0) {
              meth_0x4560e0(this);
            }
            iVar2 = iVar2 + 1;
            pdVar4 = pdVar4 + 7;
          } while (iVar2 < (int)this->mbr_0x130);
        }
        this->mbr_0x838 = local_70;
        this->mbr_0x83c = local_6c;
        this->mbr_0x844 = local_64;
        this->mbr_0x840 = local_68;
        this->mbr_0x834 = 1;
        meth_0x455b80(this);
        if ((0 < (int)this->mbr_0x130) ||
           (iVar2 = cls_0x45f6f0::meth_0x45f6f0(&local_60), iVar2 == 0)) {
          meth_0x4550f0(this);
          this->mbr_0x130 = 0;
          return;
        }
      }
      else {
        if (DAT_0065844c != 0) {
          if (DAT_00658324 == 0) {
            DAT_006584c0 = 0;
          }
          else {
            FUN_00482130();
            DAT_00658324 = 0;
            DAT_006584c0 = 1;
            FUN_00482140();
            meth_0x4550f0(this);
          }
        }
        local_70 = this->mbr_0x88;
        local_6c = this->mbr_0x8c;
        local_68 = (local_70 - 1) + DAT_0065ba00;
        local_64 = (local_6c - 1) + DAT_00667c50;
        local_60.mbr_0x0 = local_70;
        local_60.mbr_0x4 = local_6c;
        local_60.mbr_0x8 = local_68;
        local_60.mbr_0xc = local_64;
        if (DAT_0065844c != 0) {
          if (DAT_00658324 != 0) {
            meth_0x4550f0(this);
          }
          if (DAT_00658438 < 0x50) {
            FUN_00482130();
            cls_0x41c700::meth_0x41c700((cls_0x41c700 *)(&DAT_006584c8 + DAT_00658438 * 7));
            (&DAT_006584d8)[DAT_00658438 * 7] = 0;
            (&DAT_006584dc)[DAT_00658438 * 7] = 0;
            (&DAT_006584e0)[DAT_00658438 * 7] = 0;
            DAT_00658438 = DAT_00658438 + 1;
            FUN_00482140();
          }
        }
        this->mbr_0x83c = local_6c;
        iVar2 = DAT_0065844c;
        this->mbr_0x838 = local_70;
        this->mbr_0x840 = local_68;
        this->mbr_0x844 = local_64;
        this->mbr_0x834 = 1;
        if (iVar2 != 0) {
          FUN_00482130();
          DAT_00658324 = 1;
          DAT_006584c0 = 0;
          if (DAT_00658478 == 0) {
            SetEvent(DAT_00658458);
          }
          FUN_00482140();
          if (DAT_00658478 != 0) {
            meth_0x456330(this);
          }
        }
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          meth_0x455e40(this);
        }
        if (this->mbr_0x834 != 0) {
          cls_0x41c700::meth_0x41c700((cls_0x41c700 *)&this->mbr_0x848);
          this->mbr_0x834 = 0;
        }
        meth_0x4562a0(this);
        if (DAT_00658324 == 0) {
          meth_0x455f90(this);
        }
      }
      this->mbr_0x130 = 0;
    }
  }
  return;
}



// Function at 00455b80

void TCharacter::meth_0x455b80()

{
  undefined4 *puVar1;
  int iVar2;
  
  if (DAT_0065844c != 0) {
    FUN_00482130();
    DAT_00658324 = 1;
    DAT_006584c0 = 0;
    if (DAT_00658478 == 0) {
      SetEvent(DAT_00658458);
    }
    FUN_00482140();
    if (((DAT_00658478 != 0) && (ResetEvent(DAT_00658458), DAT_00658324 != 0)) &&
       (DAT_006682bc == 0)) {
      iVar2 = 0;
      if (0 < DAT_00658438) {
        puVar1 = &DAT_006584c8;
        do {
          meth_0x456810(this,puVar1);
          iVar2 = iVar2 + 1;
          puVar1 = puVar1 + 7;
        } while (iVar2 < DAT_00658438);
      }
      DAT_00658324 = 0;
      DAT_006584c0 = 0;
      SetEvent(DAT_006584a8);
    }
  }
  return;
}



// Function at 00455e40

int TCharacter::meth_0x455e40()

{
  HANDLE hMutex;
  HANDLE pvVar1;
  DWORD DVar2;
  int iVar3;
  DWORD DVar4;
  undefined4 *puVar5;
  
  if ((DAT_0065844c != 0) && ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
    if (DAT_00658324 != 0) {
      DVar2 = GetTickCount();
      pvVar1 = DAT_006584b4;
      iVar3 = ReleaseMutex(DAT_006584b4);
      hMutex = DAT_00658310;
      while (DAT_00658310 = hMutex, iVar3 != 0) {
        iVar3 = ReleaseMutex(pvVar1);
        hMutex = DAT_00658310;
      }
      iVar3 = ReleaseMutex(hMutex);
      pvVar1 = DAT_006584b8;
      while (DAT_006584b8 = pvVar1, iVar3 != 0) {
        iVar3 = ReleaseMutex(hMutex);
        pvVar1 = DAT_006584b8;
      }
      iVar3 = ReleaseMutex(pvVar1);
      while (iVar3 != 0) {
        iVar3 = ReleaseMutex(pvVar1);
      }
      if (DAT_00658478 == 0) {
        while (DAT_00658324 != 0) {
          SetEvent(DAT_00658458);
          WaitForSingleObject(DAT_006584a8,0xffffffff);
          ResetEvent(DAT_006584a8);
        }
      }
      else {
        ResetEvent(DAT_00658458);
        if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
          iVar3 = 0;
          if (0 < DAT_00658438) {
            puVar5 = &DAT_006584c8;
            do {
              meth_0x456810(this,puVar5);
              iVar3 = iVar3 + 1;
              puVar5 = puVar5 + 7;
            } while (iVar3 < DAT_00658438);
          }
          DAT_00658324 = 0;
          DAT_006584c0 = 0;
          SetEvent(DAT_006584a8);
        }
      }
      DAT_006584c0 = 0;
      DVar4 = GetTickCount();
      return DVar4 - DVar2;
    }
    DAT_006584c0 = 0;
  }
  return 0;
}



// Function at 00455f90

void TCharacter::meth_0x455f90()

{
  HANDLE hMutex;
  HANDLE pvVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if (DAT_0065844c != 0) {
    if ((DAT_00658324 != 0) && ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
      GetTickCount();
      pvVar1 = DAT_006584b4;
      iVar2 = ReleaseMutex(DAT_006584b4);
      hMutex = DAT_00658310;
      while (DAT_00658310 = hMutex, iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
        hMutex = DAT_00658310;
      }
      iVar2 = ReleaseMutex(hMutex);
      pvVar1 = DAT_006584b8;
      while (DAT_006584b8 = pvVar1, iVar2 != 0) {
        iVar2 = ReleaseMutex(hMutex);
        pvVar1 = DAT_006584b8;
      }
      iVar2 = ReleaseMutex(pvVar1);
      while (iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
      }
      if (DAT_00658478 == 0) {
        while (DAT_00658324 != 0) {
          SetEvent(DAT_00658458);
          WaitForSingleObject(DAT_006584a8,0xffffffff);
          ResetEvent(DAT_006584a8);
        }
      }
      else {
        ResetEvent(DAT_00658458);
        if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
          iVar2 = 0;
          if (0 < DAT_00658438) {
            puVar3 = &DAT_006584c8;
            do {
              meth_0x456810(this,puVar3);
              iVar2 = iVar2 + 1;
              puVar3 = puVar3 + 7;
            } while (iVar2 < DAT_00658438);
          }
          DAT_00658324 = 0;
          DAT_006584c0 = 0;
          SetEvent(DAT_006584a8);
        }
      }
      DAT_006584c0 = 0;
      GetTickCount();
    }
    FUN_00482130();
    DAT_00658438 = 0;
    FUN_00482140();
  }
  return;
}



// Function at 004560e0

void TCharacter::meth_0x4560e0()

{
  undefined4 uVar1;
  HANDLE hMutex;
  HANDLE pvVar2;
  int iVar3;
  undefined4 *in_stack_00000004;
  undefined4 in_stack_00000008;
  
  if (DAT_0065844c != 0) {
    if (DAT_00658324 != 0) {
      if (((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) &&
         ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
        GetTickCount();
        pvVar2 = DAT_006584b4;
        iVar3 = ReleaseMutex(DAT_006584b4);
        hMutex = DAT_00658310;
        while (DAT_00658310 = hMutex, iVar3 != 0) {
          iVar3 = ReleaseMutex(pvVar2);
          hMutex = DAT_00658310;
        }
        iVar3 = ReleaseMutex(hMutex);
        pvVar2 = DAT_006584b8;
        while (DAT_006584b8 = pvVar2, iVar3 != 0) {
          iVar3 = ReleaseMutex(hMutex);
          pvVar2 = DAT_006584b8;
        }
        iVar3 = ReleaseMutex(pvVar2);
        while (iVar3 != 0) {
          iVar3 = ReleaseMutex(pvVar2);
        }
        if (DAT_00658478 == 0) {
          while (DAT_00658324 != 0) {
            SetEvent(DAT_00658458);
            WaitForSingleObject(DAT_006584a8,0xffffffff);
            ResetEvent(DAT_006584a8);
          }
        }
        else {
          meth_0x456330(this);
        }
        DAT_006584c0 = 0;
        GetTickCount();
      }
      if (this->mbr_0x834 != 0) {
        this->mbr_0x848 = this->mbr_0x838;
        this->mbr_0x84c = this->mbr_0x83c;
        this->mbr_0x850 = this->mbr_0x840;
        this->mbr_0x854 = this->mbr_0x844;
        this->mbr_0x834 = 0;
      }
      meth_0x4562a0(this);
      if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
        FUN_00482130();
        DAT_00658438 = 0;
        FUN_00482140();
      }
    }
    if (DAT_00658438 < 0x50) {
      FUN_00482130();
      (&DAT_006584c8)[DAT_00658438 * 7] = *in_stack_00000004;
      (&DAT_006584cc)[DAT_00658438 * 7] = in_stack_00000004[1];
      uVar1 = in_stack_00000004[3];
      (&DAT_006584d0)[DAT_00658438 * 7] = in_stack_00000004[2];
      (&DAT_006584d4)[DAT_00658438 * 7] = uVar1;
      (&DAT_006584d8)[DAT_00658438 * 7] = in_stack_00000008;
      (&DAT_006584dc)[DAT_00658438 * 7] = 0;
      (&DAT_006584e0)[DAT_00658438 * 7] = 0;
      DAT_00658438 = DAT_00658438 + 1;
      FUN_00482140();
    }
  }
  return;
}



// Function at 004562a0

void TCharacter::meth_0x4562a0()

{
  int *piVar1;
  int iVar2;
  
  if ((((DAT_0065844c != 0) && (this->mbr_0x9a8 == 0)) && (DAT_006682bc == 0)) &&
     (iVar2 = 0, 0 < DAT_00658438)) {
    piVar1 = &DAT_006584cc;
    do {
      if ((piVar1[4] != 0) && (piVar1[5] == 0)) {
        cls_0x4aaeb0::meth_0x4aaeb0
                  ((cls_0x4aaeb0 *)PTR_DAT_005d79e0,(this->TObjectInstance).mbr_0x58,piVar1[-1],*piVar1
                   ,(piVar1[1] - piVar1[-1]) + 1,(piVar1[2] - *piVar1) + 1);
        piVar1[5] = 1;
      }
      iVar2 = iVar2 + 1;
      piVar1 = piVar1 + 7;
    } while (iVar2 < DAT_00658438);
  }
  return;
}



// Function at 00456330

void TCharacter::meth_0x456330()

{
  undefined4 *puVar1;
  int iVar2;
  
  ResetEvent(DAT_00658458);
  if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
    iVar2 = 0;
    if (0 < DAT_00658438) {
      puVar1 = &DAT_006584c8;
      do {
        meth_0x456810(this,puVar1);
        iVar2 = iVar2 + 1;
        puVar1 = puVar1 + 7;
      } while (iVar2 < DAT_00658438);
    }
    DAT_00658324 = 0;
    DAT_006584c0 = 0;
    SetEvent(DAT_006584a8);
  }
  return;
}



// Function at 00456450

void TCharacter::meth_0x456450()

{
  bool bVar1;
  HANDLE pvVar2;
  HANDLE pvVar3;
  int iVar4;
  undefined4 *puVar5;
  int iStack_24;
  int iStack_20;
  HANDLE local_18;
  HANDLE local_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  local_18 = DAT_006584b4;
  local_14 = DAT_00658310;
  while( true ) {
    do {
      while( true ) {
        if (DAT_00658324 == 0) {
          WaitForSingleObject(DAT_00658458,0xffffffff);
        }
        ResetEvent(DAT_00658458);
        pvVar2 = DAT_006584b4;
        if (DAT_006584bc != 0) {
          return;
        }
        if (DAT_00658324 != 0) break;
        iVar4 = ReleaseMutex(DAT_006584b4);
        pvVar3 = DAT_00658310;
        while (DAT_00658310 = pvVar3, iVar4 != 0) {
          iVar4 = ReleaseMutex(pvVar2);
          pvVar3 = DAT_00658310;
        }
        iVar4 = ReleaseMutex(pvVar3);
        pvVar2 = DAT_006584b8;
        while (DAT_006584b8 = pvVar2, iVar4 != 0) {
          iVar4 = ReleaseMutex(pvVar3);
          pvVar2 = DAT_006584b8;
        }
        iVar4 = ReleaseMutex(pvVar2);
        while (iVar4 != 0) {
          iVar4 = ReleaseMutex(pvVar2);
        }
        SetEvent(DAT_006584a8);
      }
      FUN_00482130();
      bVar1 = 0 < DAT_00658438;
      FUN_00482140();
    } while (DAT_00658478 != 0);
    if (bVar1) break;
LAB_0045663c:
    FUN_00482130();
    if (DAT_006584c0 == 0) {
      DAT_00658324 = 0;
      DAT_006584c0 = 0;
      SetEvent(DAT_006584a8);
    }
    else {
      DAT_006584c0 = 0;
    }
    FUN_00482140();
    if (DAT_006584bc != 0) {
      return;
    }
  }
  iStack_24 = 0;
  puVar5 = &DAT_006584d0;
  do {
    FUN_00482130();
    bVar1 = DAT_00658438 <= iStack_24;
    if (!bVar1) {
      uStack_10 = puVar5[-2];
      uStack_c = puVar5[-1];
      uStack_8 = *puVar5;
      uStack_4 = puVar5[1];
      iStack_20 = puVar5[3];
    }
    FUN_00482140();
    if (iStack_20 == 0) {
      if ((bVar1) || (DAT_006584c0 != 0)) goto LAB_0045663c;
      WaitForMultipleObjects(2,&local_18,1,0xffffffff);
      meth_0x456810(this,&uStack_10);
      FUN_00482130();
      puVar5[3] = 1;
      FUN_00482140();
      ReleaseMutex(DAT_006584b4);
      ReleaseMutex(DAT_00658310);
      ReleaseMutex(DAT_006584b8);
      pvVar3 = DAT_006584b4;
      iVar4 = ReleaseMutex(DAT_006584b4);
      pvVar2 = DAT_00658310;
      while (DAT_00658310 = pvVar2, iVar4 != 0) {
        iVar4 = ReleaseMutex(pvVar3);
        pvVar2 = DAT_00658310;
      }
      iVar4 = ReleaseMutex(pvVar2);
      pvVar3 = DAT_006584b8;
      while (DAT_006584b8 = pvVar3, iVar4 != 0) {
        iVar4 = ReleaseMutex(pvVar2);
        pvVar3 = DAT_006584b8;
      }
      iVar4 = ReleaseMutex(pvVar3);
      while (iVar4 != 0) {
        iVar4 = ReleaseMutex(pvVar3);
      }
    }
    iStack_24 = iStack_24 + 1;
    puVar5 = puVar5 + 7;
  } while( true );
}



// Function at 00456810

void TCharacter::meth_0x456810(int **param_1)

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  dword dVar4;
  int unaff_EBX;
  dword dVar5;
  int unaff_ESI;
  int unaff_EDI;
  int **ppiVar6;
  int *piVar7;
  int iStack_74;
  int iStack_70;
  int iStack_6c;
  undefined local_54 [42];
  undefined2 uStack_2a;
  dword dStack_20;
  
  cls_0x45f7c0::meth_0x45f680((cls_0x45f7c0 *)&DAT_00658480);
  cls_0x44ceb0::cls_0x44ceb0((cls_0x44ceb0 *)local_54,param_1,0x3f,0,0);
  while ((local_54._12_4_ != 0 &&
         (cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_00658480), DAT_006584c0 == 0))) {
    cls_0x44ceb0::meth_0x44d080((cls_0x44ceb0 *)local_54);
  }
  piVar7 = *param_1;
  piVar2 = param_1[1];
  dVar4 = (int)param_1[3] + (1 - (int)piVar2);
  dVar1 = (int)param_1[2] + (1 - (int)piVar7);
  (**(code **)(*(int *)this->mbr_0x858 + 0x44))();
  ppiVar6 = (int **)0x1;
  (**(code **)(*(int *)this->mbr_0x858 + 0x48))();
  (**(code **)(*(int *)this->mbr_0x85c + 0x44))
            (unaff_EBX,iStack_74,(iStack_70 - unaff_EBX) + 1,(iStack_6c - iStack_74) + 1);
  (**(code **)(*(int *)this->mbr_0x85c + 0x48))(1);
  if ((int *)this->mbr_0x860 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x860 + 0x44))
              (dVar1,dVar4,(unaff_EDI - dVar1) + 1,(unaff_ESI - dVar4) + 1);
    (**(code **)(*(int *)this->mbr_0x860 + 0x48))(1);
  }
  dVar5 = dStack_20;
  if (((int)dStack_20 < 1) || ((this->mbr_0x9a8 != 0 && ((int)dStack_20 < 4)))) {
    meth_0x456cc0(this);
  }
  if ((DAT_006584bc == 0) && (DAT_006584c0 == 0)) {
    if ((this->mbr_0x9a8 == 0) && ((int)dVar5 < 2)) {
      if (0 < (int)dVar5) {
        FUN_0043c7c0((int *)this->mbr_0x858,(int *)&stack0xffffff74);
      }
      if ((DAT_006682bc == 0) && (this->mbr_0x9a8 == 0)) {
        cls_0x45f7a0::meth_0x45f7e0((cls_0x45f7a0 *)&stack0xffffff68);
        for (; (ppiVar6 != (int **)0x0 && (piVar2 < *ppiVar6)); piVar2 = (int *)((int)piVar2 + 1)) {
          FUN_00481e80(DAT_006584b8);
          FUN_00471930(*piVar7,(int *)this->mbr_0x858,(this->TObjectInstance).mbr_0x50);
          ReleaseMutex(DAT_006584b8);
          if (DAT_006584c0 != 0) break;
          piVar7 = piVar7 + 1;
        }
      }
      if (DAT_006584bc != 0) {
        return;
      }
      if (DAT_006584c0 != 0) {
        return;
      }
    }
    if (((int)dVar5 < 3) && (DAT_006682bc == 0)) {
      if (this->mbr_0x9a8 == 0) {
        FUN_0043d1f0((int *)this->mbr_0x85c,this->mbr_0x858,(int *)&stack0xffffff74);
      }
      if (this->mbr_0x868 == 0) {
        dVar5 = this->mbr_0x858;
        piVar7 = (int *)this->mbr_0x85c;
        FUN_00438d80((undefined4 *)&stack0xffffff84,dVar1,dVar4,dVar1,dVar4,(unaff_EDI - dVar1) + 1,
                     (unaff_ESI - dVar4) + 1,&DAT_00400406);
        (**(code **)(*piVar7 + 0x5c))(&stack0xffffff84,dVar5,0,0);
        dVar5 = dStack_20;
      }
      if (DAT_006584bc != 0) {
        return;
      }
    }
    if (DAT_006584c0 == 0) {
      if ((DAT_00668154 != 0) && (0 < (int)DAT_00656f00)) {
        if (DAT_00656f00 < 0xb) {
          iVar3 = 0;
        }
        else {
          iVar3 = *(int *)(DAT_00656f10 + 0x28);
        }
        if ((*(uint *)(iVar3 + 0x14) >> 0x10 & 1) != 0) {
          meth_0x4576d0(this);
          if (DAT_006584bc != 0) {
            return;
          }
          if (DAT_006584c0 != 0) {
            return;
          }
        }
      }
      if ((((3 < (int)dVar5) || ((meth_0x457300(this), DAT_006584bc == 0 && (DAT_006584c0 == 0))))
          && ((DAT_00668154 == 0 ||
              ((meth_0x457970(this), DAT_006584bc == 0 && (DAT_006584c0 == 0)))))) &&
         ((this->mbr_0x9a8 != 0 && (DAT_006682bc == 0)))) {
        local_54._12_4_ = (unaff_ESI - dVar4) + 1;
        local_54._8_4_ = (unaff_EDI - dVar1) + 1;
        uStack_2a = 0;
        local_54._40_2_ = 0;
        local_54._36_4_ = 0x1f;
        local_54._32_4_ = 0;
        local_54._0_4_ = dVar1;
        local_54._4_4_ = dVar4;
        local_54._16_4_ = dVar1;
        local_54._20_4_ = dVar4;
        local_54._24_4_ = local_54._8_4_;
        local_54._28_4_ = local_54._12_4_;
        (**(code **)(*(int *)this->mbr_0x860 + 0x5c))(&stack0xffffff84,this->mbr_0x858,0,0);
      }
    }
  }
  return;
}



// Function at 00456cc0

void TCharacter::meth_0x456cc0()

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  int iVar5;
  int **ppiVar6;
  int *in_stack_00000004;
  undefined *puVar7;
  uint local_2c;
  cls_0x45f7a0 local_28;
  undefined *local_1c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a4988;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_1c = &stack0xffffff18;
  if (DAT_006682bc != 0) {
    return;
  }
  if ((DAT_005d79f4 == 0) && (this->mbr_0x9c == 0)) {
    iVar2 = in_stack_00000004[1];
    iVar3 = *in_stack_00000004;
    iVar5 = *(int *)this->mbr_0x858;
    puVar7 = &DAT_00400c02;
  }
  else {
    iVar2 = in_stack_00000004[1];
    iVar3 = *in_stack_00000004;
    iVar5 = *(int *)this->mbr_0x858;
    puVar7 = (undefined *)0xc02;
  }
  ExceptionList = &local_14;
  (**(code **)(iVar5 + 100))
            (iVar3,iVar2,(in_stack_00000004[2] - iVar3) + 1,(in_stack_00000004[3] - iVar2) + 1,0,
             0xffff,0,puVar7);
  cls_0x45f7a0::meth_0x45f7e0(&local_28);
  puVar4 = (uint *)local_28.mbr_0x0;
  while( true ) {
    if ((puVar4 == (uint *)0x0) || (*puVar4 <= local_28.mbr_0x8)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (!bVar1) break;
    if (((((*(int **)local_28.mbr_0x4)[2] & 0x100U) == 0) ||
        ((DAT_00668154 != 0 && (this->mbr_0x8f8 == 0)))) &&
       ((this->mbr_0x9a8 == 0 ||
        (iVar2 = (**(code **)(**(int **)local_28.mbr_0x4 + 0x44))(),
        puVar4 = (uint *)local_28.mbr_0x0, iVar2 == 0)))) {
      FUN_00481e80(DAT_006584b8);
      local_8 = 0;
      (**(code **)(**(int **)local_28.mbr_0x4 + 0x100))(this->mbr_0x858);
      local_8 = 0xffffffff;
      ReleaseMutex(DAT_006584b8);
      puVar4 = (uint *)local_28.mbr_0x0;
      if (DAT_006584c0 != 0) {
        ExceptionList = local_14;
        return;
      }
    }
    local_28.mbr_0x4 = local_28.mbr_0x4 + 4;
    local_28.mbr_0x8 = local_28.mbr_0x8 + 1;
  }
  if (this->mbr_0x9a8 != 0) {
    meth_0x45b7d0(this,0);
  }
  local_2c = 0;
  ppiVar6 = DAT_00658490;
  do {
    if (DAT_00658480 <= local_2c) {
      ExceptionList = local_14;
      return;
    }
    if (((((*ppiVar6)[2] & 0x100U) == 0) || ((DAT_00668154 != 0 && (this->mbr_0x8f8 == 0)))) &&
       ((this->mbr_0x9a8 == 0 || (iVar2 = (**(code **)(**ppiVar6 + 0x44))(), iVar2 != 0)))) {
      FUN_00481e80(DAT_006584b8);
      local_8 = 2;
      (**(code **)(**ppiVar6 + 0x100))(this->mbr_0x858);
      local_8 = 0xffffffff;
      ReleaseMutex(DAT_006584b8);
      if (DAT_006584c0 != 0) {
        ExceptionList = local_14;
        return;
      }
    }
    ppiVar6 = ppiVar6 + 1;
    local_2c = local_2c + 1;
  } while( true );
}



// Function at 00457300

void TCharacter::meth_0x457300()

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  undefined4 local_134;
  undefined4 local_130;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_124;
  undefined4 local_120;
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  undefined4 local_104;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined2 local_e4;
  undefined2 local_e2;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  undefined2 local_7c;
  undefined2 local_7a;
  dword local_40;
  int local_3c;
  cls_0x45f7a0 local_28;
  undefined *local_1c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a49b8;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_1c = &stack0xfffffeb0;
  if (DAT_006682bc == 0) {
    ExceptionList = &local_14;
    cls_0x45f7a0::meth_0x45f7e0(&local_28);
    puVar3 = (uint *)local_28.mbr_0x0;
    while( true ) {
      if ((puVar3 == (uint *)0x0) || (*puVar3 <= local_28.mbr_0x8)) {
        bVar1 = false;
      }
      else {
        bVar1 = true;
      }
      if (!bVar1) break;
      if (((*(uint *)(*(int *)local_28.mbr_0x4 + 8) & 0x100) == 0) ||
         ((DAT_00668154 != 0 && (this->mbr_0x8f8 == 0)))) {
        FUN_00481e80(DAT_006584b8);
        local_8 = 0;
        local_3c = *(int *)local_28.mbr_0x4;
        local_40 = this->mbr_0x85c;
        if (*(int **)(local_3c + 0x54) != (int *)0x0) {
          (**(code **)(**(int **)(local_3c + 0x54) + 0x14))(local_3c,local_40);
        }
        local_8 = 0xffffffff;
        ReleaseMutex(DAT_006584b8);
        puVar3 = (uint *)local_28.mbr_0x0;
        if (DAT_006584c0 != 0) {
          ExceptionList = local_14;
          return;
        }
      }
      local_28.mbr_0x4 = local_28.mbr_0x4 + 4;
      local_28.mbr_0x8 = local_28.mbr_0x8 + 1;
    }
    if (this->mbr_0x9a8 != 0) {
      meth_0x45b7d0(this,1);
    }
    if (DAT_0066816c != 0) {
      iVar2 = (**(code **)(*(int *)this->mbr_0x85c + 0x38))();
      local_98 = *(undefined4 *)(iVar2 + 8);
      local_9c = *(undefined4 *)(iVar2 + 4);
      local_cc = 0x80000000;
      local_c8 = 0;
      local_c4 = 0;
      local_c0 = 0;
      local_a4 = 0;
      local_a0 = 0;
      local_94 = 0;
      local_90 = 0;
      local_7a = 0;
      local_7c = 0;
      local_b8 = 0;
      local_bc = 0;
      local_a8 = 0;
      local_ac = 0;
      local_b0 = 0;
      local_b4 = 0;
      local_80 = 0x1f;
      local_84 = 0;
      local_8c = local_9c;
      local_88 = local_98;
      (**(code **)(*(int *)this->mbr_0x85c + 0x5c))(&local_cc,iVar2,0,0);
    }
    if (DAT_00668170 != 0) {
      iVar2 = (**(code **)(*(int *)this->mbr_0x85c + 0x3c))();
      local_100 = *(undefined4 *)(iVar2 + 8);
      local_104 = *(undefined4 *)(iVar2 + 4);
      local_134 = 0x80000000;
      local_130 = 0;
      local_12c = 0;
      local_128 = 0;
      local_10c = 0;
      local_108 = 0;
      local_fc = 0;
      local_f8 = 0;
      local_e2 = 0;
      local_e4 = 0;
      local_120 = 0;
      local_124 = 0;
      local_110 = 0;
      local_114 = 0;
      local_118 = 0;
      local_11c = 0;
      local_e8 = 0x1f;
      local_ec = 0;
      local_f4 = local_104;
      local_f0 = local_100;
      (**(code **)(*(int *)this->mbr_0x85c + 0x5c))(&local_134,iVar2,0,0);
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 004576d0

void TCharacter::meth_0x4576d0()

{
  cls_0x498eb0 *this_00;
  undefined4 uVar1;
  dword dVar2;
  dword dVar3;
  int *in_stack_00000004;
  undefined4 uVar4;
  int local_2c;
  undefined4 local_28;
  int local_24;
  int local_20;
  dword local_1c;
  dword local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((DAT_006682bc == 0) && (local_2c = 0, 0 < DAT_00668578)) {
    do {
      this_00 = *(cls_0x498eb0 **)(DAT_00668588 + local_2c * 4);
      if ((this_00 != (cls_0x498eb0 *)0x0) &&
         ((((this_00[9].mbr_0x8 != 0 &&
            (cls_0x498eb0::meth_0x498eb0_TSector_GetMaxScreenRect(this_00), *in_stack_00000004 <= local_8)) &&
           (local_10 <= in_stack_00000004[2])) &&
          ((in_stack_00000004[1] <= local_4 && (local_c <= in_stack_00000004[3])))))) {
        for (dVar3 = this->mbr_0x8c0; ((int)dVar3 < 0x40 && ((int)dVar3 < (int)this->mbr_0x8c8));
            dVar3 = dVar3 + 1) {
          for (dVar2 = this->mbr_0x8bc; ((int)dVar2 < 0x40 && ((int)dVar2 < (int)this->mbr_0x8c4));
              dVar2 = dVar2 + 1) {
            local_1c = dVar2;
            local_18 = dVar3;
            local_14 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap((cls_0x499720 *)this_00,dVar2);
            local_1c = (this_00->mbr_0x8 * 0x40 + dVar2) * 0x10;
            local_18 = (this_00->mbr_0xc * 0x40 + dVar3) * 0x10;
            FUN_0046d810(&local_1c,&local_28);
            local_24 = local_24 + 8;
            local_20 = local_20 + -0x14e;
            if (0 < local_14) {
              uVar4 = 0x20512;
              uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
              cls_0x5a68d8::meth_0x4bdc50
                        ((cls_0x5a68d8 *)this->mbr_0x85c,local_28,(short)local_24,local_20,uVar1,
                         uVar4);
            }
            uVar4 = 0xa0112;
            uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
            cls_0x5a68d8::meth_0x4bd680
                      ((cls_0x5a68d8 *)this->mbr_0x85c,local_28,local_24,uVar1,uVar4);
          }
        }
      }
      local_2c = local_2c + 1;
    } while (local_2c < DAT_00668578);
  }
  return;
}



// Function at 00457970

void TCharacter::meth_0x457970()

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  dword dVar4;
  cls_0x44ceb0 local_48;
  
  if (DAT_006682bc == 0) {
    DAT_006573fc = 0;
    iVar3 = DAT_00656ff4;
    if (DAT_006573f4 < 1) {
      iVar3 = -1;
    }
    if (-1 < iVar3) {
      do {
        cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
        piVar2 = (int *)local_48.mbr_0xc;
        while (local_48.mbr_0xc = (dword)piVar2, piVar2 != (int *)0x0) {
          if (piVar2[0x10] == iVar3) goto LAB_004579ec;
          cls_0x44ceb0::meth_0x44d080(&local_48);
          piVar2 = (int *)local_48.mbr_0xc;
        }
        piVar2 = (int *)cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_004579ec:
        if (((piVar2 != (int *)0x0) && (iVar3 = (**(code **)(*piVar2 + 0xb4))(), iVar3 == 0)) &&
           ((piVar2[2] & 0x400U) == 0)) {
          if (DAT_006584c0 != 0) {
            return;
          }
          piVar1 = (int *)piVar2[0x15];
          if (DAT_005d7a18 == 0) {
            if (piVar1 != (int *)0x0) {
              iVar3 = *piVar1;
              dVar4 = this->mbr_0x85c;
              goto LAB_00457a35;
            }
          }
          else if (piVar1 != (int *)0x0) {
            iVar3 = *piVar1;
            dVar4 = this->mbr_0x858;
LAB_00457a35:
            (**(code **)(iVar3 + 0x1c))(piVar2,dVar4);
          }
        }
        DAT_006573fc = DAT_006573fc + 1;
        if (DAT_006573f4 <= DAT_006573fc) {
          return;
        }
        iVar3 = (&DAT_00656ff4)[DAT_006573fc];
        if (iVar3 < 0) {
          return;
        }
      } while( true );
    }
  }
  return;
}



// Function at 00457b30

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x457b30()

{
  cls_0x499720 *this_00;
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iStack_ac;
  int iStack_a8;
  int iStack_a4;
  dword dStack_a0;
  dword dStack_9c;
  undefined4 uStack_98;
  cls_0x44ceb0 cStack_94;
  cls_0x44ceb0 cStack_4c;
  
  uVar5 = 0;
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x4902c0_36)(this);
  if (0 < (int)DAT_00656f00) {
    if (DAT_00656f00 < 0xb) {
      iVar3 = 0;
    }
    else {
      iVar3 = *(int *)(DAT_00656f10 + 0x28);
    }
    if (((((*(uint *)(iVar3 + 0x14) >> 0x10 & 1) != 0) && (DAT_006682bc == 0)) &&
        (-1 < (int)this->mbr_0x108)) && (-1 < (int)this->mbr_0x10c)) {
      dStack_a0 = this->mbr_0x108;
      dStack_9c = this->mbr_0x10c;
      this_00 = (cls_0x499720 *)FUN_00499e10_TSector_FindLoadedSector(_DAT_00666970,this->mbr_0x100,this->mbr_0x104);
      if (this_00 == (cls_0x499720 *)0x0) {
        uStack_98 = 0;
      }
      else {
        uStack_98 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(this_00,dStack_a0);
      }
      dStack_a0 = (this->mbr_0x100 * 0x40 + this->mbr_0x108) * 0x10;
      dStack_9c = (this->mbr_0x104 * 0x40 + this->mbr_0x10c) * 0x10;
      FUN_0046d810(&dStack_a0,&iStack_ac);
      iStack_ac = iStack_ac + (DAT_006663d8 - this->mbr_0x78);
      iStack_a8 = iStack_a8 + 8 + (DAT_006663d4 - this->mbr_0x7c);
      iStack_a4 = iStack_a4 + -0x14e;
      uVar4 = 0x20502;
      uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
      cls_0x5a68d8::meth_0x4bdc50
                ((cls_0x5a68d8 *)PTR_DAT_005d79e0,iStack_ac,(short)iStack_a8,iStack_a4,uVar1,uVar4);
      uVar4 = 0xa0102;
      uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
      cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)PTR_DAT_005d79e0,iStack_ac,iStack_a8,uVar1,uVar4);
    }
  }
  DAT_006573fc = 0;
  iVar3 = DAT_00656ff4;
  if (DAT_006573f4 < 1) {
    iVar3 = -1;
  }
  while (-1 < iVar3) {
    cls_0x44ceb0::meth_0x44cf80(&cStack_94,0,0x80,0,0);
    piVar2 = (int *)cStack_94.mbr_0xc;
    while (cStack_94.mbr_0xc = (dword)piVar2, piVar2 != (int *)0x0) {
      if (piVar2[0x10] == iVar3) goto LAB_00457df1;
      cls_0x44ceb0::meth_0x44d080(&cStack_94);
      piVar2 = (int *)cStack_94.mbr_0xc;
    }
    piVar2 = (int *)cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00457df1:
    if (((piVar2 != (int *)0x0) && (iVar3 = (**(code **)(*piVar2 + 0xb4))(), iVar3 == 0)) &&
       ((piVar2[2] & 0x400U) != 0)) {
      if ((int *)piVar2[0x15] != (int *)0x0) {
        (**(code **)(*(int *)piVar2[0x15] + 0x1c))(piVar2,PTR_DAT_005d79e0);
      }
      iVar3 = piVar2[0x14];
      if (-1 < iVar3) {
        cls_0x44ceb0::meth_0x44cf80(&cStack_4c,0,0x80,0,0);
        piVar2 = (int *)cStack_4c.mbr_0xc;
        while (cStack_4c.mbr_0xc = (dword)piVar2, piVar2 != (int *)0x0) {
          if (piVar2[0x10] == iVar3) goto LAB_00457e71;
          cls_0x44ceb0::meth_0x44d080(&cStack_4c);
          piVar2 = (int *)cStack_4c.mbr_0xc;
        }
        piVar2 = (int *)cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00457e71:
        if (((piVar2 != (int *)0x0) && (iVar3 = (**(code **)(*piVar2 + 0xb4))(), iVar3 == 0)) &&
           (((piVar2[2] & 0x400U) != 0 && ((int *)piVar2[0x15] != (int *)0x0)))) {
          (**(code **)(*(int *)piVar2[0x15] + 0x1c))(piVar2,PTR_DAT_005d79e0);
        }
      }
    }
    DAT_006573fc = DAT_006573fc + 1;
    if (DAT_006573f4 <= DAT_006573fc) break;
    iVar3 = (&DAT_00656ff4)[DAT_006573fc];
  }
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x44))
            (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),uVar5);
  return;
}



// Function at 00457ef0

void TCharacter::virt_meth_0x457ef0()

{
  int *piVar1;
  int iVar2;
  cls_0x44ceb0 local_64;
  undefined *local_1c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a49d0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_1c = &stack0xffffff54;
  if (DAT_006680d8 == 0) {
    ExceptionList = &local_14;
    cls_0x44ceb0::meth_0x44cf80(&local_64,0,0xa0,5,0);
    while (piVar1 = (int *)local_64.mbr_0xc, (int *)local_64.mbr_0xc != (int *)0x0) {
      local_8 = 0;
      iVar2 = (**(code **)(*(int *)local_64.mbr_0xc + 0xb4))();
      if ((iVar2 == 0) || (piVar1[0x19] == DAT_00667fcc)) {
        (**(code **)(*piVar1 + 0x120))();
        local_8 = 0xffffffff;
        cls_0x44ceb0::meth_0x44d080(&local_64);
      }
      else {
        local_8 = 0xffffffff;
        cls_0x44ceb0::meth_0x44d080(&local_64);
      }
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 00458750

void TCharacter::meth_0x458750()

{
  dword dVar1;
  dword dVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint extraout_ECX;
  uint uVar7;
  uint extraout_ECX_00;
  int *piVar8;
  undefined4 in_stack_00000004;
  float local_15c;
  float local_158;
  float local_150;
  float local_14c;
  float local_148;
  float local_144;
  int local_140;
  int local_13c;
  int local_138;
  int *local_134;
  float local_130 [16];
  cls_0x44ceb0 local_f0;
  dword local_a8;
  dword local_a4;
  dword local_a0;
  int local_9c;
  int local_98;
  int local_94;
  int local_90;
  int local_8c;
  int local_88;
  int local_84;
  int local_80;
  int local_7c;
  cls_0x44ceb0 local_74;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  undefined *local_1c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a4a30;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_1c = &stack0xfffffdb0;
  if (DAT_006680d8 == 0) {
    if (this->mbr_0x918 == 0) {
      local_28 = this->mbr_0x8c - 0xc0;
      local_2c = this->mbr_0x88 - 0xc0;
      dVar1 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
      local_20 = this->mbr_0x8c + 0xbf + dVar1;
      dVar2 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
      local_24 = this->mbr_0x88 + 0xbf + dVar2;
      ExceptionList = &local_14;
      local_1c = &stack0xfffffdb0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x44))(DAT_006663d8,DAT_006663d4,dVar2,dVar1);
      cls_0x44ceb0::meth_0x44cf80(&local_74,&local_2c,0x29,5,0);
      while (piVar8 = (int *)local_74.mbr_0xc, (int *)local_74.mbr_0xc != (int *)0x0) {
        local_8 = 0;
        (**(code **)(*(int *)local_74.mbr_0xc + 0xf4))(&local_9c);
        (**(code **)(*piVar8 + 0xf8))(&local_8c);
        if ((((local_94 < local_2c) || (local_24 < local_9c)) || (local_90 < local_28)) ||
           (local_20 < local_98)) {
          bVar3 = false;
        }
        else {
          bVar3 = true;
        }
        if (bVar3) {
LAB_004588a1:
          iVar4 = 1;
        }
        else {
          if (((local_84 < local_2c) || (local_24 < local_8c)) ||
             ((local_80 < local_28 || (local_20 < local_88)))) {
            iVar4 = 0;
          }
          else {
            iVar4 = 1;
          }
          if (iVar4 != 0) goto LAB_004588a1;
        }
        local_7c = iVar4;
        if (iVar4 == 0) {
LAB_004588f5:
          (**(code **)(*piVar8 + 0x128))();
LAB_004588ff:
          (**(code **)(*piVar8 + 0x11c))(in_stack_00000004);
          local_8 = 0xffffffff;
          cls_0x44ceb0::meth_0x44d080(&local_74);
        }
        else {
          iVar5 = (**(code **)(*piVar8 + 0x20))();
          if ((iVar5 != 0) || (-1 < piVar8[0x27])) {
            if (iVar4 == 0) goto LAB_004588f5;
            goto LAB_004588ff;
          }
          (**(code **)(*piVar8 + 0x124))();
          (**(code **)(*piVar8 + 0x11c))(in_stack_00000004);
          local_8 = 0xffffffff;
          cls_0x44ceb0::meth_0x44d080(&local_74);
        }
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x44))
                (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8));
    }
    else {
      dVar1 = this->mbr_0xdc;
      if (dVar1 == 0) {
        local_a8 = this->mbr_0xe0;
        local_a4 = this->mbr_0xe4;
        local_a0 = this->mbr_0xe8;
      }
      else {
        local_a8 = *(dword *)(dVar1 + 0x10);
        local_a4 = *(dword *)(dVar1 + 0x14);
        local_a0 = *(dword *)(dVar1 + 0x18);
      }
      ExceptionList = &local_14;
      FUN_00417470(2,local_130);
      local_28 = this->mbr_0x8c - 0x400;
      local_2c = this->mbr_0x88 - 0x400;
      local_20 = this->mbr_0x8c + 0x3ff + (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
      local_24 = this->mbr_0x88 + 0x3ff + (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
      cls_0x44ceb0::meth_0x44cf80(&local_f0,&local_2c,0x29,5,0);
      uVar7 = extraout_ECX;
      while ((int *)local_f0.mbr_0xc != (int *)0x0) {
        local_134 = (int *)local_f0.mbr_0xc;
        FUN_0059a530_stricmp(uVar7,*(byte **)(local_f0.mbr_0xc + 0x38),(byte *)s_Sardok_005d0644);
        local_8 = 2;
        iVar4 = local_134[4];
        iVar5 = local_134[5];
        local_138 = local_134[6];
        local_150 = (float)iVar4;
        local_14c = (float)iVar5;
        local_148 = (float)local_138;
        local_140 = iVar4;
        local_13c = iVar5;
        FUN_0043ade0(local_130,&local_150,&local_15c,&local_144);
        dVar1 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
        if ((((local_15c <= (float)(-0x32 - dVar1)) || ((float)(dVar1 + 0x32) <= local_15c)) ||
            (dVar1 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc, local_158 <= (float)(-0x32 - dVar1))
            ) || ((float)(dVar1 + 0x32) <= local_158)) {
LAB_00458d65:
          piVar8 = local_134;
          (**(code **)(*local_134 + 0x128))();
        }
        else {
          iVar6 = local_a8 - iVar4;
          if (iVar6 < 0) {
            iVar6 = iVar4 - local_a8;
          }
          iVar4 = local_a4 - iVar5;
          if (iVar4 < 0) {
            iVar4 = iVar5 - local_a4;
          }
          iVar5 = iVar6;
          if (iVar4 <= iVar6) {
            iVar5 = iVar4;
          }
          iVar4 = FUN_0045f730(0,local_a0,(iVar4 - (iVar5 >> 1)) + iVar6,local_138);
          piVar8 = local_134;
          if ((float)(this->mbr_0x8cc << 1) + (float)this->mbr_0x940 <= (float)iVar4)
          goto LAB_00458d65;
          (**(code **)(*local_134 + 0x124))();
        }
        (**(code **)(*piVar8 + 0x11c))(in_stack_00000004);
        local_8 = 0xffffffff;
        cls_0x44ceb0::meth_0x44d080(&local_f0);
        uVar7 = extraout_ECX_00;
      }
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 004590e0

void TCharacter::meth_0x4590e0()

{
  LPCVOID *ppvVar1;
  HANDLE hMutex;
  uint *puVar2;
  int iVar3;
  undefined4 *puVar4;
  cls_0x45f7a0 local_c;
  
  FUN_00499be0_TSector_ClearPreloadSectors((LPCVOID)0xffffffff,0,0,1);
  iVar3 = DAT_00668580;
  puVar4 = DAT_00668588;
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d0698;
    DAT_00658474 = 0x12ee;
    iVar3 = DAT_00668580;
    puVar4 = DAT_00668588;
  }
  while (DAT_00668580 = iVar3, DAT_00668588 = puVar4, DAT_00668578 != 0) {
    ppvVar1 = (LPCVOID *)*puVar4;
    if (ppvVar1 != (LPCVOID *)0x0) {
      cls_0x45f7a0::meth_0x45f7e0(&local_c);
      puVar2 = (uint *)local_c.mbr_0x0;
      for (; (puVar2 != (uint *)0x0 && (local_c.mbr_0x8 < *puVar2));
          local_c.mbr_0x8 = local_c.mbr_0x8 + 1) {
        if (*(int **)local_c.mbr_0x4 != (int *)0x0) {
          (**(code **)(**(int **)local_c.mbr_0x4 + 0x128))();
          puVar2 = (uint *)local_c.mbr_0x0;
        }
        local_c.mbr_0x4 = local_c.mbr_0x4 + 4;
      }
      FUN_00498460_TSector_CloseSector(ppvVar1,1);
      iVar3 = DAT_00668580;
      puVar4 = DAT_00668588;
    }
  }
  if (puVar4 != (undefined4 *)0x0) {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
  }
  DAT_0066857c = 0;
  DAT_00668578 = 0;
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar3 = ReleaseMutex(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(hMutex);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  (this->TObjectInstance).mbr_0x6c = (this->TObjectInstance).mbr_0x6c + 10000000;
  (this->TObjectInstance).mbr_0x68 = (this->TObjectInstance).mbr_0x68 + 10000000;
  meth_0x4546a0(this);
  return;
}



// Function at 00459220

void TCharacter::meth_0x459220()

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  this->mbr_0xbc = this->mbr_0xb0;
  this->mbr_0xc0 = this->mbr_0xb4;
  this->mbr_0xc4 = this->mbr_0xb8;
  this->mbr_0xb0 = this->mbr_0xa4;
  this->mbr_0xb8 = this->mbr_0xac;
  dVar1 = (this->TObjectInstance).mbr_0x6c;
  this->mbr_0xb4 = this->mbr_0xa8;
  dVar2 = this->mbr_0xb4;
  (this->TObjectInstance).mbr_0x64 = dVar1;
  dVar1 = this->mbr_0xb0;
  dVar3 = this->mbr_0x9c;
  (this->TObjectInstance).mbr_0x60 = (this->TObjectInstance).mbr_0x68;
  (this->TObjectInstance).mbr_0x68 = (int)dVar1 >> 10;
  this->mbr_0xa0 = dVar3;
  dVar3 = this->mbr_0x98;
  (this->TObjectInstance).mbr_0x6c = (int)dVar2 >> 10;
  this->mbr_0x9c = dVar3;
  if (this->mbr_0xa0 == dVar3) {
    iVar6 = dVar1 - this->mbr_0xbc;
    if (iVar6 < 0) {
      iVar6 = this->mbr_0xbc - dVar1;
    }
    iVar4 = dVar2 - this->mbr_0xc0;
    if (iVar4 < 0) {
      iVar4 = this->mbr_0xc0 - dVar2;
    }
    iVar5 = iVar6;
    if (iVar4 <= iVar6) {
      iVar5 = iVar4;
    }
    if ((iVar4 - (iVar5 >> 1)) + iVar6 < 0x401) goto LAB_004592fe;
  }
  *(undefined4 *)(DAT_00667fd0 + 0x6c) = 1;
LAB_004592fe:
  meth_0x459490(this);
  meth_0x4597b0(this);
  meth_0x459a70(this);
  meth_0x459b80(this);
  if (DAT_00668154 == 0) {
    return;
  }
  FUN_004409d0((cls_0x4405d0 *)&DAT_00656e78);
  return;
}



// Function at 00459490

void TCharacter::meth_0x459490()

{
  undefined4 uVar1;
  LPCVOID *ppvVar2;
  HANDLE pvVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  cls_0x45f7a0 local_c;
  
  iVar9 = 0;
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    DAT_0065845c = s_d__revenant_MapPane_cpp_005d06b0;
    DAT_00658474 = 0x136d;
  }
  piVar5 = DAT_00668588;
  iVar7 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar4 = *piVar5;
      iVar7 = iVar7 + -1;
      uVar1 = *(undefined4 *)(iVar4 + 0x98);
      *(undefined4 *)(iVar4 + 0x98) = 0;
      *(undefined4 *)(iVar4 + 0x9c) = uVar1;
      *(undefined4 *)(iVar4 + 0xa4) = 0;
      *(undefined4 *)(iVar4 + 0xa0) = 0;
      piVar5 = piVar5 + 1;
    } while (iVar7 != 0);
  }
  FUN_00459330(this->mbr_0x9c,(int)this->mbr_0xb0 >> 10,(int)this->mbr_0xb4 >> 10,0);
  iVar10 = 0;
  iVar4 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
  iVar7 = DAT_00668578;
  if (0 < iVar4) {
    do {
      iVar7 = cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,iVar10);
      if (((((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) ||
          (iVar7 == DAT_00667fcc)) && ((iVar7 != 0 && ((*(byte *)(iVar7 + 0x36c) & 1) != 0)))) {
        uVar6 = *(int *)(iVar7 + 0x10) >> 10;
        uVar8 = *(int *)(iVar7 + 0x14) >> 10;
        if ((uVar6 < 0x20) && (uVar8 < 0x20)) {
          FUN_00459330((uint)*(ushort *)(iVar7 + 0xe),uVar6,uVar8,iVar7);
        }
      }
      iVar10 = iVar10 + 1;
      iVar4 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
      iVar7 = DAT_00668578;
    } while (iVar10 < iVar4);
  }
  while (iVar7 = iVar7 + -1, -1 < iVar7) {
    ppvVar2 = (LPCVOID *)DAT_00668588[iVar7];
    if (((ppvVar2 != (LPCVOID *)0x0) && (ppvVar2[0x26] == (LPCVOID)0x0)) &&
       (ppvVar2[4] == (LPCVOID)0x0)) {
      cls_0x45f7a0::meth_0x45f7e0(&local_c);
      iVar4 = cls_0x45f7a0::meth_0x45f7a0(&local_c);
      while (iVar4 != 0) {
        if (*(int **)local_c.mbr_0x4 != (int *)0x0) {
          (**(code **)(**(int **)local_c.mbr_0x4 + 0x128))();
        }
        local_c.mbr_0x4 = local_c.mbr_0x4 + 4;
        local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
        iVar4 = cls_0x45f7a0::meth_0x45f7a0(&local_c);
      }
      FUN_00498460_TSector_CloseSector(ppvVar2,0);
    }
  }
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    pvVar3 = DAT_00658310;
    iVar7 = ReleaseMutex(DAT_00658310);
    while (iVar7 != 0) {
      iVar7 = ReleaseMutex(pvVar3);
    }
    DAT_0065845c = (char *)0x0;
    DAT_00658474 = 0;
  }
  iVar7 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar4 = DAT_00668588[iVar9];
      if (((*(int *)(iVar4 + 0xa4) == 1) && (*(int *)(iVar4 + 0xa0) == 0)) &&
         (iVar10 = 0, 0 < *(int *)(iVar4 + 0xb8))) {
        do {
          piVar5 = *(int **)(*(int *)(iVar4 + 200) + iVar10 * 4);
          if ((piVar5 != (int *)0x0) && ((piVar5[2] & 0x4000U) != 0)) {
            (**(code **)(*piVar5 + 0x128))();
          }
          iVar10 = iVar10 + 1;
          iVar7 = DAT_00668578;
        } while (iVar10 < *(int *)(iVar4 + 0xb8));
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < iVar7);
  }
  iVar9 = 0;
  if (((DAT_0066829c != 0) && (DAT_00676828 != 0)) &&
     ((DAT_0067682c == 0 && ((DAT_006768d4 != '\0' && (0 < iVar7)))))) {
    do {
      if ((*(int *)(DAT_00668588[iVar9] + 0x9c) == 0) && (*(int *)(DAT_00668588[iVar9] + 0x98) == 1)
         ) {
        if (DAT_0065844c != 0) {
          FUN_00481e80(DAT_00658310);
          DAT_0065845c = s_d__revenant_MapPane_cpp_005d06c8;
          DAT_00658474 = 0x13bc;
          if (DAT_0065844c != 0) {
            ReleaseMutex(DAT_00658310);
            pvVar3 = DAT_00658310;
            iVar7 = ReleaseMutex(DAT_00658310);
            while (iVar7 != 0) {
              iVar7 = ReleaseMutex(pvVar3);
            }
            DAT_0065845c = (char *)0x0;
            DAT_00658474 = 0;
          }
        }
        cls_0x57d9d0::meth_0x586170((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,DAT_00667fcc);
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < DAT_00668578);
  }
  return;
}



// Function at 004597b0

void TCharacter::meth_0x4597b0()

{
  int *piVar1;
  bool bVar2;
  cls_0x5b4f30_TPlayer *pcVar3;
  cls_0x5b4f30_TPlayer *pcVar4;
  int iVar5;
  
  pcVar3 = DAT_00667fcc;
  bVar2 = false;
  if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
    iVar5 = TPlayScreen::meth_0x45f770(&DAT_00667fcc->TPlayScreen);
    pcVar4 = DAT_00667fcc;
    if ((iVar5 != 0) ||
       ((piVar1 = (int *)(pcVar3->TPlayScreen).TScreen.mbr_0xe0, piVar1 != (int *)0x0 &&
        (*piVar1 == 0x19)))) {
      iVar5 = TPlayScreen::meth_0x45f770(&DAT_00667fcc->TPlayScreen);
      if (((iVar5 != 0) ||
          ((piVar1 = (int *)(pcVar4->TPlayScreen).TScreen.mbr_0xe0, piVar1 != (int *)0x0 &&
           (*piVar1 == 0x19)))) &&
         (*(int *)((pcVar4->TPlayScreen).TScreen.mbr_0xe0 + 0x44) != 0)) {
        bVar2 = true;
      }
    }
  }
  if (((DAT_005d7a30 != 0) &&
      (((((this->TObjectInstance).mbr_0x68 != (this->TObjectInstance).mbr_0x60 ||
         ((this->TObjectInstance).mbr_0x6c != (this->TObjectInstance).mbr_0x64)) ||
        (this->mbr_0x9c != this->mbr_0xa0)) || ((this->TObjectInstance).mbr_0x50 != 0)))) && (!bVar2))
  {
    iVar5 = FUN_00499db0_TSector_InPreloadArea(&this->mbr_0xb0,this->mbr_0x9c);
    if (iVar5 == 0) {
      if ((DAT_0065c610 != 0) && (DAT_0065c618 == 0)) {
        iVar5 = (**(code **)(DAT_0065c5d0 + 0x40))();
        if ((iVar5 != 0) && (0 < *(int *)(DAT_00667fd0 + 0x48))) {
          cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
          meth_0x54ca20((TCharacter *)&DAT_0065c5d0);
          meth_0x54cbb0((TCharacter *)&DAT_0065c5d0);
          meth_0x491990((TCharacter *)&DAT_0065c5d0);
        }
      }
      if ((DAT_0066829c != 0) && (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0)) {
        iVar5 = cls_0x57d9d0::meth_0x57d9d0
                          ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)DAT_00667fcc,0x1d,1);
        if (iVar5 != 0) {
          cls_0x57d9d0::meth_0x57dc70((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
        }
        cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(DAT_00667fcc);
      }
      FUN_004997d0((int *)&this->mbr_0xb0,(LPCVOID)this->mbr_0x9c,&LAB_00459a00);
      if ((((DAT_0066829c != 0) && (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0)) &&
          (piVar1 = (int *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0xd8, piVar1 != (int *)0x0)
          ) && (((iVar5 = *piVar1, iVar5 == 2 || (iVar5 == 4)) || (iVar5 == 0x1a)))) {
        cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(DAT_00667fcc);
        cls_0x57d9d0::meth_0x583e80
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,DAT_00667fcc,0x1c,
                   (DAT_00667fcc->TPlayScreen).TScreen.mbr_0xb0,1);
      }
      if ((DAT_0065c610 != 0) && (DAT_0065c618 == 0)) {
        iVar5 = (**(code **)(DAT_0065c5d0 + 0x40))();
        if ((iVar5 != 0) && (0 < *(int *)(DAT_00667fd0 + 0x48))) {
          meth_0x54cad0((TCharacter *)&DAT_0065c5d0);
          meth_0x54cbb0((TCharacter *)&DAT_0065c5d0);
          meth_0x491990((TCharacter *)&DAT_0065c5d0);
        }
      }
      meth_0x4546a0(this);
    }
  }
  return;
}



// Function at 00459a70

void TCharacter::meth_0x459a70()

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int local_14;
  
  local_14 = 0;
  iVar5 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar1 = *(int *)(DAT_00668588 + local_14 * 4);
      if (((iVar1 != 0) && (*(int *)(iVar1 + 0xa8) != 0)) && (*(int *)(iVar1 + 0x98) != 0)) {
        iVar2 = *(int *)(iVar1 + 8);
        iVar3 = *(int *)(iVar1 + 0xc);
        iVar4 = *(int *)(iVar1 + 4);
        iVar6 = 0;
        if (0 < iVar5) {
          do {
            iVar5 = *(int *)(DAT_00668588 + iVar6 * 4);
            if (((((iVar5 != 0) && (*(int *)(iVar5 + 4) == iVar4)) &&
                 ((iVar2 + -1 <= *(int *)(iVar5 + 8) &&
                  ((*(int *)(iVar5 + 8) <= iVar2 + 1 && (iVar3 + -1 <= *(int *)(iVar5 + 0xc)))))))
                && (*(int *)(iVar5 + 0xc) <= iVar3 + 1)) && (iVar7 = 0, 0 < *(int *)(iVar5 + 0xb8)))
            {
              do {
                meth_0x452750(this,*(undefined4 *)(*(int *)(iVar5 + 200) + iVar7 * 4));
                iVar7 = iVar7 + 1;
              } while (iVar7 < *(int *)(iVar5 + 0xb8));
            }
            iVar6 = iVar6 + 1;
            iVar5 = DAT_00668578;
          } while (iVar6 < DAT_00668578);
        }
        *(undefined4 *)(iVar1 + 0xa8) = 0;
      }
      local_14 = local_14 + 1;
    } while (local_14 < iVar5);
  }
  return;
}



// Function at 00459b80

void TCharacter::meth_0x459b80()

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar3 = 0;
  iVar1 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
  if (0 < iVar1) {
    do {
      piVar2 = (int *)cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,iVar3);
      if (piVar2 != (int *)0x0) {
        if (((piVar2[0xdb] & 1U) == 0) || (piVar2[0x11] != 0)) {
          if ((piVar2[0xdb] & 1U) != 0) goto LAB_00459c2b;
          if (piVar2[0x11] != 0) {
            if ((-1 < piVar2[0x14]) && (iVar1 = FUN_00452690_TMapPane_GetInstance(piVar2[0x14],0), iVar1 != 0)) {
              meth_0x451610(this);
            }
            iVar1 = (**(code **)(*piVar2 + 0x20))();
            if (iVar1 != 0) {
              (**(code **)(*piVar2 + 0x2c))();
            }
            iVar1 = (**(code **)(*piVar2 + 0xb4))();
            if ((iVar1 == 0) && (piVar2[0x19] == 0)) {
              meth_0x451b10(this);
            }
            else {
              (**(code **)(*piVar2 + 0x60))();
            }
            goto LAB_00459c2b;
          }
LAB_00459c30:
          iVar1 = (**(code **)(*piVar2 + 0x24))();
          if (iVar1 != 0) {
            (**(code **)(*piVar2 + 0x128))();
          }
        }
        else {
          meth_0x451090(this);
LAB_00459c2b:
          if (piVar2[0x11] == 0) goto LAB_00459c30;
        }
        if (piVar2[0x198] != 0) {
          cls_0x57d9d0::meth_0x586630((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,piVar2);
          iStack_8 = piVar2[5];
          iStack_4 = piVar2[6];
          iStack_c = piVar2[4];
          uStack_18 = 0;
          uStack_14 = 0;
          uStack_10 = 0;
          iVar1 = FUN_00475040((byte *)piVar2[0x198],*(undefined2 *)((int)piVar2 + 0xe),&iStack_c,0,
                               0,&uStack_18,0xffffffff);
          if (iVar1 != 0) {
            meth_0x451090(this);
          }
          piVar2[0x198] = 0;
        }
      }
      iVar3 = iVar3 + 1;
      iVar1 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
    } while (iVar3 < iVar1);
  }
  return;
}



// Function at 00459f50

int TCharacter::meth_0x459f50()

{
  HANDLE hMutex;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *in_stack_00000004;
  int *in_stack_00000008;
  uint in_stack_0000000c;
  
  iVar1 = (**(code **)(*in_stack_00000004 + 0xb4))();
  if (iVar1 != 0) {
    return in_stack_00000004[0x10];
  }
  if ((in_stack_0000000c == 0xffffffff) || ((in_stack_00000004[2] & 0x80000U) == 0)) {
    in_stack_0000000c = (uint)*(ushort *)((int)in_stack_00000004 + 0xe);
  }
  if (*in_stack_00000008 < 0) {
    *in_stack_00000008 = 0;
  }
  else if (0x7fff < *in_stack_00000008) {
    *in_stack_00000008 = 0x8000;
  }
  if (in_stack_00000008[1] < 0) {
    in_stack_00000008[1] = 0;
  }
  else if (0x7fff < in_stack_00000008[1]) {
    in_stack_00000008[1] = 0x8000;
  }
  iVar1 = in_stack_00000008[1] >> 10;
  iVar5 = *in_stack_00000008 >> 10;
  if ((in_stack_00000004[2] & 0x80000U) == 0) {
    iVar2 = FUN_00499e10_TSector_FindLoadedSector(in_stack_0000000c,iVar5,iVar1);
    if (iVar2 == 0) {
      iVar3 = in_stack_00000004[4] >> 10;
      iVar2 = *in_stack_00000008;
      iVar5 = iVar3 * 0x400;
      iVar6 = (in_stack_00000004[5] >> 10) * 0x400;
      iVar4 = (iVar3 + 1) * 0x400;
      iVar1 = ((in_stack_00000004[5] >> 10) + 1) * 0x400;
      if (iVar2 == iVar5 || SBORROW4(iVar2,iVar5) != iVar2 + iVar3 * -0x400 < 0) {
        iVar2 = iVar5;
      }
      if ((iVar4 <= iVar2) ||
         (iVar4 = *in_stack_00000008,
         iVar4 != iVar5 && SBORROW4(iVar4,iVar5) == iVar4 + iVar3 * -0x400 < 0)) {
        iVar5 = iVar4;
      }
      iVar2 = in_stack_00000008[1];
      *in_stack_00000008 = iVar5;
      iVar4 = DAT_0066829c;
      iVar3 = iVar2;
      if (iVar2 <= iVar6) {
        iVar3 = iVar6;
      }
      if ((iVar3 < iVar1) && (iVar1 = iVar2, iVar2 <= iVar6)) {
        iVar1 = iVar6;
      }
      iVar5 = iVar5 >> 10;
      in_stack_00000008[1] = iVar1;
      iVar1 = iVar1 >> 10;
      if (((iVar4 != 0) && (DAT_00676828 != 0)) && (DAT_0067682c == 0)) {
        (**(code **)(*in_stack_00000004 + 0x40))(in_stack_00000004[2] | 0x1000);
      }
    }
  }
  else {
    iVar2 = FUN_00499e10_TSector_FindLoadedSector(in_stack_0000000c,iVar5,iVar1);
    if (iVar2 == 0) {
      if (in_stack_00000004 != (int *)0x0) {
        if ((-1 < in_stack_00000004[0x14]) &&
           (iVar1 = FUN_00452690_TMapPane_GetInstance(in_stack_00000004[0x14],0), iVar1 != 0)) {
          meth_0x451610(this);
        }
        iVar1 = (**(code **)(*in_stack_00000004 + 0x20))();
        if (iVar1 != 0) {
          (**(code **)(*in_stack_00000004 + 0x2c))();
        }
        iVar1 = (**(code **)(*in_stack_00000004 + 0xb4))();
        if ((iVar1 == 0) && (in_stack_00000004[0x19] == 0)) {
          meth_0x451b10(this);
          return 0;
        }
        (**(code **)(*in_stack_00000004 + 0x60))();
      }
      return 0;
    }
  }
  if (((in_stack_0000000c == *(ushort *)((int)in_stack_00000004 + 0xe)) &&
      (iVar5 == in_stack_00000004[4] >> 10)) && (iVar1 == in_stack_00000004[5] >> 10)) {
    return in_stack_00000004[0x10];
  }
  if (in_stack_00000004 != (int *)0x0) {
    iVar2 = in_stack_00000004[0x11];
    iVar1 = FUN_00499e10_TSector_FindLoadedSector(in_stack_0000000c,iVar5,iVar1);
    if ((iVar2 != 0) && (iVar1 != 0)) {
      if (iVar2 == iVar1) {
        return in_stack_00000004[0x10];
      }
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        DAT_0065845c = s_d__revenant_MapPane_cpp_005d06ec;
        DAT_00658474 = 0x1512;
      }
      FUN_00499250_TSector_RemoveObject(iVar2,(int)in_stack_00000004);
      FUN_00498fb0_TSector_AddObject(iVar1,(int)in_stack_00000004,-1);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar1 = ReleaseMutex(DAT_00658310);
        while (iVar1 != 0) {
          iVar1 = ReleaseMutex(hMutex);
        }
        DAT_0065845c = (char *)0x0;
        DAT_00658474 = 0;
      }
      return in_stack_00000004[0x10];
    }
  }
  return -1;
}



// Function at 0045a490

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x45a490(int param_1, int param_2)

{
  cls_0x499720 *this_00;
  dword dVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int in_stack_0000000c;
  
  this_00 = (cls_0x499720 *)
            FUN_00499e10_TSector_FindLoadedSector(_DAT_00666970,(this->TObjectInstance).mbr_0x68,(this->TObjectInstance).mbr_0x6c);
  if ((this_00 != (cls_0x499720 *)0x0) && (param_1 != 0)) {
    dVar1 = this->mbr_0x8c8;
    if (0x3f < (int)dVar1) {
      dVar1 = 0x40;
    }
    iVar4 = dVar1 - 1;
    if ((int)this->mbr_0x8c0 <= iVar4) {
      do {
        dVar1 = this->mbr_0x8c4;
        if (0x3f < (int)dVar1) {
          dVar1 = 0x40;
        }
        iVar3 = dVar1 - 1;
        if ((int)this->mbr_0x8bc <= iVar3) {
          do {
            iVar2 = cls_0x499720::meth_0x499720_TSector_ReturnWalkmap(this_00,iVar3);
            if ((in_stack_0000000c == 0) || (iVar2 != 0)) {
              cls_0x499720::meth_0x499750_TSector_SetWalkmap(this_00,iVar3,iVar4);
            }
            iVar3 = iVar3 + -1;
          } while ((int)this->mbr_0x8bc <= iVar3);
        }
        iVar4 = iVar4 + -1;
      } while ((int)this->mbr_0x8c0 <= iVar4);
    }
    meth_0x4546a0(this);
  }
  return;
}



// Function at 0045a570

void TCharacter::meth_0x45a570(dword param_1, dword param_2)

{
  uint uVar1;
  int iVar2;
  bool bVar3;
  dword in_stack_0000000c;
  undefined local_58 [16];
  cls_0x44ceb0 local_48;
  
  if (((int)in_stack_0000000c < 3) && (-1 < (int)in_stack_0000000c)) {
    this->mbr_0x88c = param_1;
    this->mbr_0x890 = param_2;
    this->mbr_0x898 = in_stack_0000000c;
    uVar1 = DAT_006573f8;
    if (((in_stack_0000000c == 1) || (in_stack_0000000c == 2)) &&
       ((bVar3 = DAT_006573f8 < DAT_006573f4, this->mbr_0x894 = 1, bVar3 &&
        (iVar2 = (&DAT_00656ff4)[uVar1], -1 < iVar2)))) {
      cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
      while ((int *)local_48.mbr_0xc != (int *)0x0) {
        if (*(int *)(local_48.mbr_0xc + 0x40) == iVar2) goto LAB_0045a617;
        cls_0x44ceb0::meth_0x44d080(&local_48);
      }
      local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_0045a617:
      if ((int *)local_48.mbr_0xc != (int *)0x0) {
        (**(code **)(*(int *)local_48.mbr_0xc + 0xf4))(local_58);
        this->mbr_0x108 = 0;
        this->mbr_0x10c = 0;
      }
    }
    if (this->mbr_0x898 == 1) {
      FUN_0043a020(0);
      return;
    }
    iVar2 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
    FUN_0043a020(iVar2);
  }
  return;
}



// Function at 0045a990

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x45a990(float param_1, float param_2)

{
  float10 fVar1;
  float10 fVar2;
  float10 fVar3;
  float in_stack_0000000c;
  
  fVar1 = (float10)fsin((float10)in_stack_0000000c);
  this->mbr_0x934 = this->mbr_0x928;
  this->mbr_0x938 = this->mbr_0x92c;
  this->mbr_0x93c = this->mbr_0x930;
  this->mbr_0x940 = (dword)param_1;
  this->mbr_0x930 = (dword)param_2;
  this->mbr_0x92c = (dword)in_stack_0000000c;
  this->mbr_0x924 = (dword)(float)(fVar1 * (float10)param_1);
  fVar2 = (float10)fcos((float10)in_stack_0000000c);
  fVar1 = (float10)_DAT_005a36c8;
  fVar3 = (float10)fcos((float10)param_2 + fVar1);
  this->mbr_0x91c = (dword)(float)(fVar3 * fVar2 * (float10)param_1);
  fVar1 = (float10)fsin((float10)param_2 + fVar1);
  this->mbr_0x920 = (dword)(float)(fVar1 * fVar2 * (float10)param_1);
  return;
}



// Function at 0045aa10

void TCharacter::meth_0x45aa10(dword *param_1, dword *param_2)

{
  dword dVar1;
  dword dVar2;
  dword *in_stack_0000000c;
  
  if (this->mbr_0x940 == 0xc7c35000) {
    this->mbr_0x940 =
         (dword)SQRT((float)this->mbr_0x91c * (float)this->mbr_0x91c +
                     SQRT((float)this->mbr_0x924 * (float)this->mbr_0x924 +
                          (float)this->mbr_0x920 * (float)this->mbr_0x920));
  }
  dVar1 = this->mbr_0x930;
  dVar2 = this->mbr_0x92c;
  *param_1 = this->mbr_0x940;
  *param_2 = dVar1;
  *in_stack_0000000c = dVar2;
  return;
}



// Function at 0045aa80

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x45aa80()

{
  dword in_stack_00000004;
  
  this->mbr_0x944 = in_stack_00000004;
  this->mbr_0x974 = 0;
  FUN_00412bc0((float)in_stack_00000004 * _DAT_005a4a60);
  return;
}



// Function at 0045abb0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x45abb0()

{
  dword dVar1;
  float fVar2;
  int local_2c;
  int local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c [3];
  
  dVar1 = this->mbr_0x918;
  if (((dVar1 == 1) || (dVar1 == 2)) || (dVar1 == 3)) {
    if (this->mbr_0x978 != 0) {
      this->mbr_0x978 = this->mbr_0x978 - 1;
      this->mbr_0x91c = (dword)(*(float *)&this->field_0x948 + (float)this->mbr_0x91c);
      this->mbr_0x920 = (dword)(*(float *)&this->field_0x94c + (float)this->mbr_0x920);
      this->mbr_0x924 = (dword)(*(float *)&this->field_0x950 + (float)this->mbr_0x924);
    }
    if (this->mbr_0x97c != 0) {
      this->mbr_0x97c = this->mbr_0x97c - 1;
      this->mbr_0x928 = (dword)(*(float *)&this->field_0x948 + (float)this->mbr_0x928);
      this->mbr_0x92c = (dword)(*(float *)&this->field_0x94c + (float)this->mbr_0x92c);
      this->mbr_0x930 = (dword)(*(float *)&this->field_0x950 + (float)this->mbr_0x930);
    }
    if (this->mbr_0x980 != 0) {
      this->mbr_0x980 = this->mbr_0x980 - 1;
      this->mbr_0x940 = (dword)(*(float *)&this->field_0x960 + (float)this->mbr_0x940);
    }
    if (this->mbr_0x984 != 0) {
      this->mbr_0x944 = this->mbr_0x944 + this->mbr_0x964;
      this->mbr_0x984 = this->mbr_0x984 - 1;
    }
  }
  if (dVar1 == 0) {
    local_24 = (float)this->mbr_0xa4;
    local_20 = (float)this->mbr_0xa8;
    local_1c = (float)this->mbr_0xac;
    if (0 < (int)local_1c) {
      FUN_0046d7a0(&local_24,&local_28,&local_2c);
      FUN_0046dad0(local_28,local_2c,(int *)&local_24,0);
    }
    FUN_0046d810(&local_24,&local_18);
    DAT_005e91b8 = 0;
    if (DAT_006682bc == 0) {
      FUN_00412b90();
      FUN_00412670(0);
      __ftol();
      cls_0x412870::meth_0x412870((cls_0x412870 *)((int)&DAT_0065a578 + 4),&local_24);
      return;
    }
  }
  else {
    if (dVar1 == 1) {
      DAT_005e91b8 = dVar1;
      FUN_00412bc0((float)this->mbr_0x944 * _DAT_005a4a60);
      FUN_00412670(1);
      FUN_00412ae0((float *)&this->mbr_0x91c,(float *)&this->mbr_0x928);
      return;
    }
    if (((dVar1 == 2) || (dVar1 == 3)) && (DAT_00667fcc != 0)) {
      local_24 = (float)this->mbr_0xb0;
      local_20 = (float)this->mbr_0xb4;
      DAT_005e91b8 = 1;
      fVar2 = (float)this->mbr_0xb8 + _DAT_005a4a64;
      local_1c = (fVar2 / (_DAT_005a3520 - fVar2 * _DAT_005a3528 * _DAT_005a3524)) * _DAT_005a351c;
      local_18 = local_24 + (float)this->mbr_0x91c;
      local_14 = local_20 + (float)this->mbr_0x920;
      local_10 = local_1c + (float)this->mbr_0x924;
      FUN_00412bc0((float)this->mbr_0x944 * _DAT_005a4a60);
      FUN_00412670(1);
      FUN_004129e0(&local_18,&local_24,(float)this->mbr_0x928,local_c);
      FUN_00412ae0(&local_18,(float *)&this->mbr_0x928);
    }
  }
  return;
}



// Function at 0045aec0

void TCharacter::meth_0x45aec0()

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  dword *pdVar4;
  
  this->mbr_0x9a8 = 0;
  this->mbr_0x9ac = 0x19;
  this->mbr_0x9b0 = 0x19;
  this->mbr_0x9b4 = 0x271;
  this->mbr_0x9b8 = 0x9c4;
  pdVar4 = &this->mbr_0x9e0;
  iVar3 = 2;
  do {
    puVar2 = FUN_00482fb0(this->mbr_0x9b4 << 5);
    dVar1 = this->mbr_0x9b4;
    pdVar4[-2] = (dword)puVar2;
    puVar2 = FUN_00482fb0(dVar1 << 5);
    dVar1 = this->mbr_0x9b4;
    pdVar4[-1] = (dword)puVar2;
    puVar2 = FUN_00482fb0(dVar1 * 0xc);
    if (puVar2 == (undefined4 *)0x0) {
      puVar2 = (undefined4 *)0x0;
    }
    *pdVar4 = (dword)puVar2;
    puVar2 = FUN_00482fb0(this->mbr_0x9b4 << 3);
    dVar1 = this->mbr_0x9b4;
    pdVar4[1] = (dword)puVar2;
    puVar2 = FUN_00482fb0(dVar1 << 3);
    dVar1 = this->mbr_0x9b4;
    pdVar4[2] = (dword)puVar2;
    puVar2 = FUN_00482fb0(dVar1 << 3);
    pdVar4[3] = (dword)puVar2;
    puVar2 = FUN_00482fb0(this->mbr_0x9b8 << 1);
    dVar1 = this->mbr_0x9b8;
    pdVar4[4] = (dword)puVar2;
    puVar2 = FUN_00482fb0(dVar1 << 1);
    pdVar4[5] = (dword)puVar2;
    pdVar4 = pdVar4 + 8;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  this->mbr_0x9c4 = 1;
  this->mbr_0x9c8 = 0;
  this->mbr_0x9cc = 0;
  this->mbr_0x9cd = 0;
  this->mbr_0x9ce = 0;
  this->mbr_0x9d0 = 0;
  this->mbr_0x9d4 = 0;
  this->mbr_0x9bc = 0xfffe7960;
  this->mbr_0x9c0 = 0xfffe7960;
  return;
}



// Function at 0045b000

void TCharacter::meth_0x45b000()

{
  dword *pdVar1;
  int iVar2;
  
  pdVar1 = &this->mbr_0x9e0;
  iVar2 = 2;
  do {
    FUN_004830f0((LPCVOID)pdVar1[-2]);
    FUN_004830f0((LPCVOID)pdVar1[-1]);
    FUN_004830f0((LPCVOID)*pdVar1);
    FUN_004830f0((LPCVOID)pdVar1[1]);
    FUN_004830f0((LPCVOID)pdVar1[2]);
    FUN_004830f0((LPCVOID)pdVar1[3]);
    FUN_004830f0((LPCVOID)pdVar1[4]);
    FUN_004830f0((LPCVOID)pdVar1[5]);
    pdVar1 = pdVar1 + 8;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



// Function at 0045b080

void TCharacter::meth_0x45b080()

{
  cls_0x5a68d8__vftable_5a68d8 *pcVar1;
  dword dVar2;
  cls_0x499500 *this_00;
  HANDLE hMutex;
  cls_0x5a68d8 *pcVar3;
  int iVar4;
  uint uVar5;
  void *unaff_ESI;
  bool bVar6;
  dword in_stack_00000004;
  cls_0x5a68d8 *local_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  uStack_8 = (cls_0x5a68d8 *)&LAB_0059d04b;
  local_c = (cls_0x5a68d8 *)ExceptionList;
  if (DAT_0066818c != 0) {
    in_stack_00000004 = 0;
  }
  if ((in_stack_00000004 != this->mbr_0x9a8) || (this->mbr_0x85c == 0)) {
    ExceptionList = &local_c;
    if (DAT_0065844c != 0) {
      ExceptionList = &local_c;
      FUN_00481e80(DAT_006584b4);
      DAT_00658448 = s_d__revenant_MapPane_cpp_005d0728;
      UNK_006584fc._2188_4_ = 0x16e1;
    }
    if (in_stack_00000004 == 0) {
      if ((((this->mbr_0x860 != 0) || (this->mbr_0x858 != 0)) || (this->mbr_0x85c != 0)) ||
         (this->mbr_0x864 != 0)) {
        FUN_004aa490((this->TObjectInstance).mbr_0x58);
        (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490030_24)(&this->TObjectInstance);
        if ((undefined4 *)this->mbr_0x864 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x864)(1);
        }
        this->mbr_0x864 = 0;
        if ((undefined4 *)this->mbr_0x85c != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x85c)(1);
        }
        this->mbr_0x85c = 0;
        if ((undefined4 *)this->mbr_0x858 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x858)(1);
        }
        this->mbr_0x858 = 0;
        if ((undefined4 *)this->mbr_0x860 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x860)(1);
        }
        this->mbr_0x860 = 0;
      }
      iVar4 = DAT_0066818c;
      this->mbr_0x860 = 0;
      if (iVar4 == 0) {
        pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
        uStack_4 = 0xc;
        if (pcVar3 == (cls_0x5a68d8 *)0x0) {
          pcVar3 = (cls_0x5a68d8 *)0x0;
        }
        else {
          cls_0x5a68d8::cls_0x5a68d8(pcVar3);
          uStack_4 = CONCAT31(uStack_4._1_3_,0xd);
          pcVar3->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
          iVar4 = cls_0x5a6858::meth_0x4bb440((cls_0x5a6858 *)pcVar3,0x300,0x300,0x180,0x300,2,1);
          if (iVar4 == 0) {
            FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
          }
        }
        pcVar1 = pcVar3->vftptr_0x0;
        uStack_4 = 0xffffffff;
        this->mbr_0x85c = (dword)pcVar3;
        (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
        pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
        uStack_8 = (cls_0x5a68d8 *)0xe;
        if (pcVar3 == (cls_0x5a68d8 *)0x0) {
          pcVar3 = (cls_0x5a68d8 *)0x0;
        }
        else {
          dVar2 = this->mbr_0x85c;
          cls_0x5a68d8::cls_0x5a68d8(pcVar3);
          uStack_8 = (cls_0x5a68d8 *)CONCAT31(uStack_8._1_3_,0xf);
          pcVar3->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
          iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
          if (iVar4 == 0) {
            FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
          }
        }
        pcVar1 = pcVar3->vftptr_0x0;
        bVar6 = DAT_005d7a34 != 0;
        uStack_4 = 0xffffffff;
        this->mbr_0x858 = (dword)pcVar3;
        this->mbr_0x868 = (uint)bVar6;
        (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      }
      else {
        pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
        uStack_4 = 8;
        if (pcVar3 == (cls_0x5a68d8 *)0x0) {
          pcVar3 = (cls_0x5a68d8 *)0x0;
        }
        else {
          cls_0x5a68d8::cls_0x5a68d8(pcVar3);
          uStack_4 = CONCAT31(uStack_4._1_3_,9);
          pcVar3->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
          iVar4 = cls_0x5a6858::meth_0x4bb440((cls_0x5a6858 *)pcVar3,0x300,0x300,0x180,0x300,2,1);
          if (iVar4 == 0) {
            FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
          }
        }
        pcVar1 = pcVar3->vftptr_0x0;
        uStack_4 = 0xffffffff;
        this->mbr_0x85c = (dword)pcVar3;
        (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
        pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
        uStack_8 = (cls_0x5a68d8 *)0xa;
        if (pcVar3 == (cls_0x5a68d8 *)0x0) {
          pcVar3 = (cls_0x5a68d8 *)0x0;
        }
        else {
          dVar2 = this->mbr_0x85c;
          cls_0x5a68d8::cls_0x5a68d8(pcVar3);
          uStack_8 = (cls_0x5a68d8 *)CONCAT31(uStack_8._1_3_,0xb);
          pcVar3->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
          iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
          if (iVar4 == 0) {
            FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
          }
        }
        pcVar1 = pcVar3->vftptr_0x0;
        uStack_4 = 0xffffffff;
        this->mbr_0x858 = (dword)pcVar3;
        this->mbr_0x868 = 1;
        (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      }
      pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
      uStack_8 = (cls_0x5a68d8 *)0x10;
      if (pcVar3 == (cls_0x5a68d8 *)0x0) {
        pcVar3 = (cls_0x5a68d8 *)0x0;
      }
      else {
        dVar2 = this->mbr_0x85c;
        cls_0x5a68d8::cls_0x5a68d8(pcVar3);
        uStack_8 = (cls_0x5a68d8 *)CONCAT31(uStack_8._1_3_,0x11);
        pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
        iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
        if (iVar4 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
        }
      }
      pcVar1 = pcVar3->vftptr_0x0;
      uStack_8 = (cls_0x5a68d8 *)0xffffffff;
      this->mbr_0x864 = (dword)pcVar3;
      (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      FUN_004aa850((this->TObjectInstance).cls_0x41c7f0.mbr_0x0,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x4,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x8,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0xc,this->mbr_0x864);
      (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48ff00_20)(&this->TObjectInstance);
      dVar2 = (this->TObjectInstance).mbr_0x58;
      uVar5 = FUN_004aa530(dVar2);
      FUN_004aa500(dVar2,uVar5 & 0xfffffffe);
      if (DAT_00668154 == 0) {
        iVar4 = 0;
        if (0 < DAT_00668578) {
          do {
            this_00 = *(cls_0x499500 **)(DAT_00668588 + iVar4 * 4);
            if (this_00 != (cls_0x499500 *)0x0) {
              cls_0x499500::meth_0x499500_TSector_InitLights(this_00);
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 < DAT_00668578);
        }
        meth_0x4546a0(this);
      }
    }
    else {
      if (((this->mbr_0x85c != 0) || (this->mbr_0x858 != 0)) || (this->mbr_0x864 != 0)) {
        FUN_004aa490((this->TObjectInstance).mbr_0x58);
        (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490030_24)(&this->TObjectInstance);
        if ((undefined4 *)this->mbr_0x864 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x864)(1);
        }
        this->mbr_0x864 = 0;
        if ((undefined4 *)this->mbr_0x858 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x858)(1);
        }
        this->mbr_0x858 = 0;
        if ((undefined4 *)this->mbr_0x85c != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x85c)(1);
        }
        this->mbr_0x85c = 0;
      }
      pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
      uStack_8 = pcVar3;
      if (pcVar3 == (cls_0x5a68d8 *)0x0) {
        pcVar3 = (cls_0x5a68d8 *)0x0;
      }
      else {
        cls_0x5a68d8::cls_0x5a68d8(pcVar3);
        pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
        iVar4 = cls_0x5a68d8::meth_0x4bb5c0(pcVar3,0x300,0x300);
        if (iVar4 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
        }
      }
      pcVar1 = pcVar3->vftptr_0x0;
      this->mbr_0x860 = (dword)pcVar3;
      (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
      local_c = pcVar3;
      if (pcVar3 == (cls_0x5a68d8 *)0x0) {
        pcVar3 = (cls_0x5a68d8 *)0x0;
      }
      else {
        dVar2 = this->mbr_0x860;
        cls_0x5a68d8::cls_0x5a68d8(pcVar3);
        pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
        iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
        if (iVar4 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
        }
      }
      pcVar1 = pcVar3->vftptr_0x0;
      this->mbr_0x85c = (dword)pcVar3;
      (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
      uStack_4 = 4;
      if (pcVar3 == (cls_0x5a68d8 *)0x0) {
        pcVar3 = (cls_0x5a68d8 *)0x0;
      }
      else {
        dVar2 = this->mbr_0x85c;
        cls_0x5a68d8::cls_0x5a68d8(pcVar3);
        uStack_4 = CONCAT31(uStack_4._1_3_,5);
        pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
        iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
        if (iVar4 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
        }
      }
      pcVar1 = pcVar3->vftptr_0x0;
      bVar6 = DAT_005d7a34 != 0;
      uStack_4 = 0xffffffff;
      this->mbr_0x858 = (dword)pcVar3;
      this->mbr_0x868 = (uint)bVar6;
      (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
      uStack_8 = (cls_0x5a68d8 *)0x6;
      if (pcVar3 == (cls_0x5a68d8 *)0x0) {
        pcVar3 = (cls_0x5a68d8 *)0x0;
      }
      else {
        dVar2 = this->mbr_0x85c;
        cls_0x5a68d8::cls_0x5a68d8(pcVar3);
        uStack_8 = (cls_0x5a68d8 *)CONCAT31(uStack_8._1_3_,7);
        pcVar3->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
        iVar4 = cls_0x5a68d8::meth_0x4bb950(pcVar3,dVar2);
        if (iVar4 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
        }
      }
      pcVar1 = pcVar3->vftptr_0x0;
      uStack_8 = (cls_0x5a68d8 *)0xffffffff;
      this->mbr_0x864 = (dword)pcVar3;
      (*pcVar1->virt_meth_0x419ea0_72)(pcVar3);
      FUN_004aa850((this->TObjectInstance).cls_0x41c7f0.mbr_0x0,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x4,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x8,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0xc,this->mbr_0x864);
      (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48ff00_20)(&this->TObjectInstance);
      dVar2 = (this->TObjectInstance).mbr_0x58;
      uVar5 = FUN_004aa530(dVar2);
      FUN_004aa500(dVar2,uVar5 | 1);
    }
    if (DAT_0065844c != 0) {
      ReleaseMutex(DAT_006584b4);
      hMutex = DAT_006584b4;
      iVar4 = ReleaseMutex(DAT_006584b4);
      while (iVar4 != 0) {
        iVar4 = ReleaseMutex(hMutex);
      }
      DAT_00658448 = (char *)0x0;
      UNK_006584fc._2188_4_ = 0;
    }
    meth_0x4546a0(this);
    this->mbr_0x9bc = 0xfffe7960;
    this->mbr_0x9c0 = 0xfffe7960;
    this->mbr_0x9a8 = (dword)uStack_8;
  }
  ExceptionList = unaff_ESI;
  return;
}



// Function at 0045b7d0

void TCharacter::meth_0x45b7d0(int param_1)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  ushort *puVar9;
  int iVar10;
  ushort *puVar11;
  int iVar12;
  int *piVar13;
  int *in_stack_00000008;
  int *local_a0;
  int local_9c;
  int local_94;
  int local_90;
  int local_8c;
  int local_88;
  uint local_84;
  int local_80;
  int iStack_7c;
  int local_78;
  int iStack_74;
  uint uStack_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  int local_5c;
  int *piStack_58;
  int iStack_54;
  int iStack_50;
  uint uStack_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int iStack_38;
  dword *local_34;
  int iStack_30;
  int iStack_2c;
  int iStack_28;
  int iStack_24;
  int local_20;
  int local_1c;
  int local_10;
  undefined4 local_c;
  int local_8;
  int local_4;
  
  local_34 = &this->mbr_0x9d8 + param_1 * 8;
  iVar5 = *(int *)(this->mbr_0x860 + 4);
  local_78 = *in_stack_00000008 % iVar5;
  if (local_78 < 0) {
    local_78 = local_78 + iVar5;
  }
  iVar5 = *(int *)(this->mbr_0x860 + 8);
  local_78 = -local_78;
  iVar6 = in_stack_00000008[1] % iVar5;
  if (iVar6 < 0) {
    iVar6 = iVar6 + iVar5;
  }
  local_10 = 0;
  local_8 = in_stack_00000008[2] - *in_stack_00000008;
  local_c = 0;
  dVar1 = this->mbr_0x858;
  local_4 = in_stack_00000008[3] - in_stack_00000008[1];
  iVar7 = -iVar6;
  iVar5 = *(int *)(dVar1 + 8) + iVar7;
  local_3c = iVar7;
  if (-iVar5 == iVar6 || iVar7 < iVar5) {
    do {
      iVar5 = local_78;
      iVar6 = local_78;
      if (local_78 <= *(int *)(dVar1 + 4) + local_78) {
        do {
          local_40 = *in_stack_00000008 + iVar5;
          local_48 = in_stack_00000008[1] + iVar7;
          local_6c = 0;
          if (0 < *(int *)(dVar1 + 0x68)) {
            local_68 = 0;
            do {
              piVar8 = (int *)(*(int *)(dVar1 + 0x6c) + local_68);
              local_20 = *piVar8;
              local_1c = piVar8[1];
              iVar6 = piVar8[2];
              iVar3 = piVar8[3];
              local_94 = local_20 + iVar5;
              local_90 = local_1c + iVar7;
              local_8c = iVar6 + iVar5;
              local_88 = iVar3 + iVar7;
              iVar2 = FUN_004ad610(&local_94,&local_10,&local_94);
              if (iVar2 != 0) {
                local_8c = (local_8c - iVar5 & 0xffffffe0U) + iVar5;
                local_94 = ((local_94 - iVar5) + 0x1fU & 0xffffffe0) + iVar5;
                local_88 = (local_88 - iVar7 & 0xffffffe0U) + iVar7;
                local_90 = ((local_90 - iVar7) + 0x1fU & 0xffffffe0) + iVar7;
                local_84 = local_90 - iVar7 >> 5;
                local_5c = local_94 - iVar5 >> 5;
                local_80 = this->mbr_0x9ac * local_84 + local_5c;
                piVar13 = (int *)(local_34[2] + local_80 * 0xc);
                local_60 = iVar3 - local_1c;
                local_a0 = (int *)(this->mbr_0x9e0 + local_80 * 0xc);
                iVar3 = (this->mbr_0x9ac - ((local_8c - local_94) + 1 >> 5)) + -1;
                iVar6 = iVar6 - local_20;
                local_64 = iVar6;
                local_44 = iVar3;
                piVar8 = (int *)(**(code **)(**(int **)(*(int *)(this->mbr_0x858 + 0x70) +
                                                       local_6c * 4) + 0x38))();
                piStack_58 = piVar8;
                iStack_38 = (**(code **)(*piVar8 + 0x2c))();
                if (iStack_38 == 0) {
                  return;
                }
                if (DAT_005d7a54 != 0) {
                  (**(code **)(*piVar8 + 0x30))();
                }
                iStack_30 = piVar8[1] << 3;
                uStack_70 = local_84;
                iStack_7c = local_90;
                iVar2 = local_8c;
                iVar12 = local_94;
                if (local_90 <= local_88) {
                  do {
                    local_9c = local_5c;
                    iVar10 = iVar12;
                    iStack_74 = iVar12;
                    if (iVar12 <= iVar2) {
                      iStack_2c = (iStack_7c - local_1c) - iVar7;
                      iStack_24 = (iStack_7c - iVar7) + local_48;
                      do {
                        iVar3 = (iVar12 - local_20) - iVar5;
                        if (iVar6 <= iVar3) {
                          iVar3 = iVar6;
                        }
                        iVar6 = iStack_2c;
                        if (local_60 <= iStack_2c) {
                          iVar6 = local_60;
                        }
                        iStack_54 = (iVar12 - iVar5) + local_40;
                        iStack_50 = iStack_24;
                        iStack_28 = local_64 - iVar3;
                        uStack_4c = 0x10000000;
                        if (0x17 < iStack_28) {
                          iStack_28 = 0x18;
                        }
                        iStack_28 = iStack_28 + iVar3;
                        iVar2 = local_60 - iVar6;
                        if (0x17 < iVar2) {
                          iVar2 = 0x18;
                        }
                        iStack_74 = iVar12;
                        if (iVar2 + iVar6 < iVar6) {
LAB_0045bbac:
                          FUN_0046dab0(piVar13,&iStack_54);
                        }
                        else {
                          local_84 = ((iVar2 + iVar6) - iVar6) + 8U >> 3;
                          puVar11 = (ushort *)(iStack_38 + (piStack_58[1] * iVar6 + iVar3) * 2);
                          do {
                            if (iVar3 < iStack_28) {
                              uVar4 = (iStack_28 - iVar3) + 7U >> 3;
                              puVar9 = puVar11;
                              do {
                                if (*puVar9 < uStack_4c) {
                                  uStack_4c = (uint)*puVar9;
                                }
                                puVar9 = puVar9 + 8;
                                uVar4 = uVar4 - 1;
                              } while (uVar4 != 0);
                            }
                            puVar11 = puVar11 + iStack_30;
                            local_84 = local_84 - 1;
                          } while (local_84 != 0);
                          if (uStack_4c != 0xffff) goto LAB_0045bbac;
                          FUN_0046dad0(iStack_54,iStack_24,piVar13,0);
                          iVar12 = iStack_74;
                        }
                        if ((((param_1 == 1) && (*piVar13 == *local_a0)) &&
                            (piVar13[1] == local_a0[1])) && (piVar13[2] == local_a0[2])) {
                          piVar13[2] = -10000;
                        }
                        if (local_9c == 0) {
                          iVar6 = this->mbr_0x9ac * 3 + -3;
LAB_0045bc2c:
                          piVar8 = piVar13 + iVar6;
                          *piVar8 = *piVar13;
                          piVar8[1] = piVar13[1];
                          piVar8[2] = piVar13[2];
                        }
                        else if (uStack_70 == 0) {
                          iVar6 = (this->mbr_0x9b0 - 1) * this->mbr_0x9ac * 3;
                          goto LAB_0045bc2c;
                        }
                        local_a0 = local_a0 + 3;
                        iVar12 = iVar12 + 0x20;
                        piVar13 = piVar13 + 3;
                        local_80 = local_80 + 1;
                        local_9c = local_9c + 1;
                        iVar2 = local_8c;
                        iVar10 = local_94;
                        iVar3 = local_44;
                        iVar6 = local_64;
                        iStack_74 = iVar12;
                      } while (iVar12 <= local_8c);
                    }
                    iStack_7c = iStack_7c + 0x20;
                    local_a0 = local_a0 + iVar3 * 3;
                    piVar13 = piVar13 + iVar3 * 3;
                    local_80 = local_80 + iVar3;
                    uStack_70 = uStack_70 + 1;
                    iVar12 = iVar10;
                    piVar8 = piStack_58;
                  } while (iStack_7c <= local_88);
                }
                if (piVar8[0xd] != 0) {
                  (**(code **)(*piVar8 + 0x30))();
                }
              }
              local_6c = local_6c + 1;
              dVar1 = this->mbr_0x858;
              local_68 = local_68 + 0x10;
              iVar6 = local_78;
            } while (local_6c < *(int *)(dVar1 + 0x68));
          }
          dVar1 = this->mbr_0x858;
          iVar5 = iVar5 + *(int *)(dVar1 + 4);
        } while (iVar5 <= *(int *)(dVar1 + 4) + iVar6);
      }
      dVar1 = this->mbr_0x858;
      iVar7 = iVar7 + *(int *)(dVar1 + 8);
    } while (iVar7 <= *(int *)(dVar1 + 8) + local_3c);
  }
  this->mbr_0x9c4 = 1;
  return;
}



// Function at 0045bd50

void TCharacter::meth_0x45bd50(int *param_1)

{
  byte bVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  int iVar5;
  dword dVar6;
  dword dVar7;
  bool bVar8;
  bool bVar9;
  uint uVar10;
  dword *pdVar11;
  int iVar12;
  uint uVar13;
  short *psVar14;
  uint uVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  int *piVar19;
  int iVar20;
  int iVar21;
  short *psVar22;
  short sVar23;
  short sVar24;
  int iVar25;
  short *psVar26;
  int iVar27;
  int iVar28;
  dword dVar29;
  undefined4 *puVar30;
  bool bVar31;
  longlong lVar32;
  longlong lVar33;
  longlong lVar34;
  longlong lVar35;
  uint in_stack_00000008;
  int local_b8;
  int local_b4;
  int local_b0;
  int local_a8;
  int local_a0;
  int local_98;
  short *local_94;
  dword local_90;
  dword local_8c;
  int *local_88;
  dword local_80;
  short local_60;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  cls_0x44ceb0 local_48;
  
  bVar31 = false;
  bVar8 = false;
  bVar9 = false;
  if ((((this->mbr_0x9c8 != this->mbr_0x8cc) ||
       (this->mbr_0x9ce != *(byte *)((int)&this->mbr_0x8d0 + 2))) ||
      (this->mbr_0x9cd != *(byte *)((int)&this->mbr_0x8d0 + 1))) ||
     (this->mbr_0x9cc != *(byte *)&this->mbr_0x8d0)) {
    bVar31 = true;
  }
  bVar1 = *(byte *)((int)&this->mbr_0x8d0 + 1);
  this->mbr_0x9c8 = this->mbr_0x8cc;
  this->mbr_0x9ce = *(byte *)((int)&this->mbr_0x8d0 + 2);
  this->mbr_0x9cd = bVar1;
  this->mbr_0x9cc = *(byte *)&this->mbr_0x8d0;
  lVar32 = __ftol();
  lVar33 = __ftol();
  lVar34 = __ftol();
  uVar10 = in_stack_00000008 & 1;
  if (uVar10 != 0) {
    puVar30 = (undefined4 *)this->mbr_0x9e8;
    for (uVar15 = (this->mbr_0x9b4 << 3) >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
      *puVar30 = 0;
      puVar30 = puVar30 + 1;
    }
    bVar9 = true;
    for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
      *(undefined *)puVar30 = 0;
      puVar30 = (undefined4 *)((int)puVar30 + 1);
    }
    puVar30 = (undefined4 *)this->mbr_0xa08;
    for (uVar15 = (this->mbr_0x9b4 << 3) >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
      *puVar30 = 0;
      puVar30 = puVar30 + 1;
    }
    for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
      *(undefined *)puVar30 = 0;
      puVar30 = (undefined4 *)((int)puVar30 + 1);
    }
  }
  if (0 < (int)this->mbr_0x9d4) {
    puVar30 = (undefined4 *)this->mbr_0x9ec;
    for (uVar15 = (this->mbr_0x9b4 << 3) >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
      *puVar30 = 0;
      puVar30 = puVar30 + 1;
    }
    bVar8 = true;
    for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
      *(undefined *)puVar30 = 0;
      puVar30 = (undefined4 *)((int)puVar30 + 1);
    }
    puVar30 = (undefined4 *)this->mbr_0xa0c;
    for (uVar15 = (this->mbr_0x9b4 << 3) >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
      *puVar30 = 0;
      puVar30 = puVar30 + 1;
    }
    for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
      *(undefined *)puVar30 = 0;
      puVar30 = (undefined4 *)((int)puVar30 + 1);
    }
  }
  local_50 = *(int *)&(this->TObjectInstance).field_0x2c;
  local_4c = *(int *)&(this->TObjectInstance).field_0x30;
  local_58 = local_50 + -0x100;
  local_54 = local_4c + -0x100;
  local_50 = (param_1[2] - *param_1) + 0x100 + local_50;
  local_4c = (param_1[3] - param_1[1]) + 0x100 + local_4c;
  if (uVar10 != 0) {
    this->mbr_0x9d0 = 0;
  }
  uVar15 = in_stack_00000008 & 2;
  if (uVar15 != 0) {
    this->mbr_0x9d4 = 0;
  }
  cls_0x44ceb0::meth_0x44cf80(&local_48,&local_58,3,3,0);
  dVar6 = local_48.mbr_0xc;
  do {
    if (dVar6 == 0) {
      if (((bVar9) || (bVar8)) || (bVar31)) {
        local_b4 = 2;
        dVar6 = this->mbr_0x9b4;
        pdVar11 = &this->mbr_0x9e8;
        do {
          psVar14 = (short *)*pdVar11;
          if (0 < (int)dVar6) {
            psVar22 = (short *)((int)pdVar11[-1] + (4 - (int)psVar14) + (int)psVar14);
            psVar26 = (short *)((int)pdVar11[1] + (4 - (int)psVar14) + (int)psVar14);
            dVar29 = dVar6;
            do {
              sVar2 = psVar26[-2];
              local_b0._0_2_ = (short)lVar32;
              psVar22[1] = 0;
              psVar22[-2] = sVar2 + (short)local_b0 + *psVar14;
              psVar22[-1] = psVar26[-1] + psVar14[1] + (short)lVar33;
              local_60 = (short)lVar34;
              dVar29 = dVar29 - 1;
              *psVar22 = psVar14[2] + local_60 + *psVar26;
              psVar14 = psVar14 + 4;
              psVar22 = psVar22 + 4;
              psVar26 = psVar26 + 4;
            } while (dVar29 != 0);
          }
          pdVar11 = pdVar11 + 8;
          local_b4 = local_b4 + -1;
        } while (local_b4 != 0);
        local_b4 = 0;
        dVar6 = this->mbr_0x9b0;
        do {
          iVar16 = 0;
          pdVar11 = &this->mbr_0x9d8 + local_b4 * 8;
          local_98 = 0;
          local_8c = *pdVar11;
          local_a8 = 0;
          local_90 = pdVar11[1];
          psVar14 = (short *)pdVar11[3];
          dVar29 = pdVar11[2];
          if (0 < (int)dVar6) {
            dVar7 = this->mbr_0x9ac;
            psVar22 = psVar14;
            local_80 = dVar29;
            do {
              if (0 < (int)dVar7) {
                local_88 = (int *)(dVar29 + 0x14 + iVar16 * 0xc);
                local_a0 = 1;
                iVar25 = iVar16 * 8;
                do {
                  local_94 = (short *)(iVar25 + 8);
                  if ((local_b4 == 1) && (*(int *)(local_80 + 8) == -10000)) {
                    iVar16 = 0;
                    uVar10 = 0;
                    local_b8 = 0;
                    local_b0 = 0;
                    if ((-1 < local_a0 + -2) &&
                       (((local_a0 + -2 < (int)dVar7 && (-1 < local_a8 + -1)) &&
                        (local_a8 + -1 < (int)dVar6)))) {
                      iVar27 = (local_98 - dVar7) + -1;
                      bVar31 = *(int *)(dVar29 + 8 + iVar27 * 0xc) != -10000;
                      if (bVar31) {
                        iVar16 = (int)psVar14[iVar27 * 4];
                        local_b0 = (int)psVar14[iVar27 * 4 + 1];
                        local_b8 = (int)psVar14[iVar27 * 4 + 2];
                      }
                      uVar10 = (uint)bVar31;
                    }
                    if (((-1 < local_a0 + -1) && (local_a0 + -1 < (int)dVar7)) &&
                       ((-1 < local_a8 + -1 &&
                        ((local_a8 + -1 < (int)dVar6 &&
                         (iVar27 = local_98 - dVar7, *(int *)(dVar29 + 8 + iVar27 * 0xc) != -10000))
                        )))) {
                      iVar16 = iVar16 + psVar14[iVar27 * 4];
                      local_b0 = local_b0 + psVar14[iVar27 * 4 + 1];
                      local_b8 = local_b8 + psVar14[iVar27 * 4 + 2];
                      uVar10 = uVar10 + 1;
                    }
                    if ((((-1 < local_a0) && (local_a0 < (int)dVar7)) && (-1 < local_a8 + -1)) &&
                       ((local_a8 + -1 < (int)dVar6 &&
                        (iVar27 = (local_98 - dVar7) + 1,
                        *(int *)(dVar29 + 8 + iVar27 * 0xc) != -10000)))) {
                      iVar16 = iVar16 + psVar14[iVar27 * 4];
                      local_b0 = local_b0 + psVar14[iVar27 * 4 + 1];
                      local_b8 = local_b8 + psVar14[iVar27 * 4 + 2];
                      uVar10 = uVar10 + 1;
                    }
                    if (((-1 < local_a0 + -2) && (local_a0 + -2 < (int)dVar7)) &&
                       ((-1 < local_a8 && ((local_a8 < (int)dVar6 && (local_88[-6] != -10000)))))) {
                      iVar16 = iVar16 + *(short *)(iVar25 + -8 + (int)psVar14);
                      local_b0 = local_b0 + *(short *)(iVar25 + -6 + (int)psVar14);
                      local_b8 = local_b8 + *(short *)(iVar25 + -4 + (int)psVar14);
                      uVar10 = uVar10 + 1;
                    }
                    if ((((-1 < local_a0) && (local_a0 < (int)dVar7)) && (-1 < local_a8)) &&
                       ((local_a8 < (int)dVar6 && (*local_88 != -10000)))) {
                      iVar16 = iVar16 + *(short *)((int)local_94 + (int)psVar14);
                      local_b0 = local_b0 + *(short *)(iVar25 + 10 + (int)psVar14);
                      local_b8 = local_b8 + *(short *)(iVar25 + 0xc + (int)psVar14);
                      uVar10 = uVar10 + 1;
                    }
                    if (((-1 < local_a0 + -2) && (local_a0 + -2 < (int)dVar7)) &&
                       ((-1 < local_a8 + 1 &&
                        ((local_a8 + 1 < (int)dVar6 &&
                         (iVar25 = (dVar7 - 1) + local_98,
                         *(int *)(dVar29 + 8 + iVar25 * 0xc) != -10000)))))) {
                      iVar16 = iVar16 + psVar14[iVar25 * 4];
                      local_b0 = local_b0 + psVar14[iVar25 * 4 + 1];
                      local_b8 = local_b8 + psVar14[iVar25 * 4 + 2];
                      uVar10 = uVar10 + 1;
                    }
                    if ((((-1 < local_a0 + -1) && (local_a0 + -1 < (int)dVar7)) &&
                        (-1 < local_a8 + 1)) &&
                       ((local_a8 + 1 < (int)dVar6 &&
                        (iVar25 = local_98 + dVar7, *(int *)(dVar29 + 8 + iVar25 * 0xc) != -10000)))
                       ) {
                      iVar16 = iVar16 + psVar14[iVar25 * 4];
                      local_b0 = local_b0 + psVar14[iVar25 * 4 + 1];
                      local_b8 = local_b8 + psVar14[iVar25 * 4 + 2];
                      uVar10 = uVar10 + 1;
                    }
                    if (((-1 < local_a0) && (local_a0 < (int)dVar7)) &&
                       ((-1 < local_a8 + 1 &&
                        ((local_a8 + 1 < (int)dVar6 &&
                         (iVar25 = dVar7 + 1 + local_98,
                         *(int *)(dVar29 + 8 + iVar25 * 0xc) != -10000)))))) {
                      iVar16 = iVar16 + psVar14[iVar25 * 4];
                      local_b0 = local_b0 + psVar14[iVar25 * 4 + 1];
                      local_b8 = local_b8 + psVar14[iVar25 * 4 + 2];
                      uVar10 = uVar10 + 1;
                    }
                    if (uVar10 != 0) {
                      *psVar22 = (short)(iVar16 / (int)uVar10);
                      psVar22[1] = (short)(local_b0 / (int)uVar10);
                      psVar22[2] = (short)(local_b8 / (int)uVar10);
                      goto LAB_0045c732;
                    }
                  }
                  else {
LAB_0045c732:
                    sVar2 = psVar22[1];
                    sVar3 = psVar22[2];
                    sVar23 = sVar2;
                    if (sVar2 <= sVar3) {
                      sVar23 = sVar3;
                    }
                    sVar4 = *psVar22;
                    sVar24 = sVar4;
                    if ((sVar4 <= sVar23) && (sVar24 = sVar2, sVar2 <= sVar3)) {
                      sVar24 = sVar3;
                    }
                    iVar16 = (int)sVar24;
                    if (0x1ff < iVar16) {
                      *psVar22 = (short)((sVar4 * 0x1ff) / iVar16);
                      psVar22[1] = (short)((sVar2 * 0x1ff) / iVar16);
                      psVar22[2] = (short)((sVar3 * 0x1ff) / iVar16);
                    }
                    sVar2 = *psVar22;
                    if (iVar16 < 0x100) {
                      *(uint *)(local_8c + 0x10) =
                           (((int)sVar2 | 0xffffff00U) << 8 | (int)psVar22[1]) << 8 |
                           (int)psVar22[2];
                      *(undefined4 *)(local_90 + 0x10) = 0;
                      psVar22[3] = 0;
                    }
                    else {
                      psVar22[3] = 1;
                      uVar10 = ((CONCAT31((int3)(char)((ushort)sVar2 >> 8),(char)sVar2) & 0xfffffffe
                                | 0xfffffe00) << 8 | (int)psVar22[1] & 0xfffffffeU) << 7 |
                               (int)(psVar22[2] >> 1);
                      *(uint *)(local_8c + 0x10) = uVar10;
                      *(uint *)(local_90 + 0x10) = uVar10;
                    }
                  }
                  local_88 = local_88 + 3;
                  local_8c = local_8c + 0x20;
                  local_90 = local_90 + 0x20;
                  local_80 = local_80 + 0xc;
                  iVar16 = local_98 + 1;
                  psVar22 = psVar22 + 4;
                  bVar31 = local_a0 < (int)dVar7;
                  local_a0 = local_a0 + 1;
                  local_98 = iVar16;
                  iVar25 = (int)local_94;
                } while (bVar31);
              }
              local_a8 = local_a8 + 1;
            } while (local_a8 < (int)dVar6);
          }
          local_b4 = local_b4 + 1;
        } while (local_b4 < 2);
      }
      return;
    }
    local_48.mbr_0xc = dVar6;
    if ((dVar6 != 0) && ((*(uint *)(dVar6 + 8) & 4) != 0)) {
      uVar17 = *(uint *)(dVar6 + 8) & 0x4000;
      if (uVar17 == 0) {
        if (uVar10 != 0) {
          this->mbr_0x9d0 = this->mbr_0x9d0 + 1;
          goto LAB_0045c035;
        }
      }
      else if (uVar15 != 0) {
        this->mbr_0x9d4 = this->mbr_0x9d4 + 1;
LAB_0045c035:
        if ((!bVar8) && (uVar15 != 0)) {
          puVar30 = (undefined4 *)this->mbr_0x9ec;
          for (uVar18 = (this->mbr_0x9b4 << 3) >> 2; uVar18 != 0; uVar18 = uVar18 - 1) {
            *puVar30 = 0;
            puVar30 = puVar30 + 1;
          }
          bVar8 = true;
          for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
            *(undefined *)puVar30 = 0;
            puVar30 = (undefined4 *)((int)puVar30 + 1);
          }
          puVar30 = (undefined4 *)this->mbr_0xa0c;
          for (uVar18 = (this->mbr_0x9b4 << 3) >> 2; uVar18 != 0; uVar18 = uVar18 - 1) {
            *puVar30 = 0;
            puVar30 = puVar30 + 1;
          }
          for (iVar16 = 0; iVar16 != 0; iVar16 = iVar16 + -1) {
            *(undefined *)puVar30 = 0;
            puVar30 = (undefined4 *)((int)puVar30 + 1);
          }
        }
        iVar27 = *(int *)(dVar6 + 0x90) + *(int *)(dVar6 + 0x10);
        iVar16 = *(int *)(dVar6 + 0x94) + *(int *)(dVar6 + 0x14);
        iVar25 = *(int *)(dVar6 + 0x98) + *(int *)(dVar6 + 0x18);
        uVar18 = (uint)*(byte *)(dVar6 + 0x89);
        local_a0 = 2;
        pdVar11 = &this->mbr_0x9ec;
        do {
          piVar19 = (int *)pdVar11[-3];
          if (uVar17 == 0) {
            local_94 = (short *)pdVar11[-1];
          }
          else {
            local_94 = (short *)*pdVar11;
          }
          local_98 = 0;
          if (0 < (int)this->mbr_0x9b4) {
            do {
              iVar5 = piVar19[2];
              if (iVar5 != -10000) {
                iVar20 = *piVar19;
                iVar21 = piVar19[1];
                if (((((int)(iVar27 - uVar18) <= iVar20) && ((int)(iVar16 - uVar18) <= iVar21)) &&
                    (iVar20 <= (int)(uVar18 + iVar27))) && (iVar21 <= (int)(uVar18 + iVar16))) {
                  iVar28 = iVar20 - iVar27;
                  iVar12 = iVar28;
                  if (iVar28 < 0) {
                    iVar12 = iVar27 - iVar20;
                  }
                  if (iVar12 < 0xff) {
                    if (iVar28 < 0) {
                      iVar28 = iVar27 - iVar20;
                    }
                  }
                  else {
                    iVar28 = 0xff;
                  }
                  iVar12 = iVar21 - iVar16;
                  iVar20 = iVar12;
                  if (iVar12 < 0) {
                    iVar20 = iVar16 - iVar21;
                  }
                  if (iVar20 < 0xff) {
                    if (iVar12 < 0) {
                      iVar12 = iVar16 - iVar21;
                    }
                  }
                  else {
                    iVar12 = 0xff;
                  }
                  iVar21 = iVar5 - iVar25;
                  iVar20 = iVar21;
                  if (iVar21 < 0) {
                    iVar20 = iVar25 - iVar5;
                  }
                  if (iVar20 < 0xff) {
                    if (iVar21 < 0) {
                      iVar21 = iVar25 - iVar5;
                    }
                  }
                  else {
                    iVar21 = 0xff;
                  }
                  uVar13 = (uint)(byte)(&DAT_005e9200)
                                       [iVar28 * 0x100 +
                                        (uint)(byte)(&DAT_005e9200)[iVar21 + iVar12 * 0x100]];
                  if (uVar13 < *(byte *)(dVar6 + 0x89)) {
                    FUN_0041d7f0((((int)uVar13 >> 1) - ((int)uVar13 >> 3)) + uVar13,
                                 (uint)*(byte *)(dVar6 + 0x89),(int)*(short *)(dVar6 + 0x8a));
                  }
                  lVar35 = __ftol();
                  *local_94 = *local_94 + (short)lVar35;
                  lVar35 = __ftol();
                  local_94[1] = local_94[1] + (short)lVar35;
                  lVar35 = __ftol();
                  local_94[2] = local_94[2] + (short)lVar35;
                }
              }
              local_94 = local_94 + 4;
              local_98 = local_98 + 1;
              piVar19 = piVar19 + 3;
            } while (local_98 < (int)this->mbr_0x9b4);
          }
          pdVar11 = pdVar11 + 8;
          local_a0 = local_a0 + -1;
        } while (local_a0 != 0);
      }
    }
    cls_0x44ceb0::meth_0x44d080(&local_48);
    dVar6 = local_48.mbr_0xc;
  } while( true );
}



// Function at 0045c8a0

/* WARNING: Type propagation algorithm not settling */

void TCharacter::meth_0x45c8a0()

{
  int iVar1;
  dword *pdVar2;
  dword dVar3;
  float fVar4;
  float fVar5;
  short sVar6;
  dword dVar7;
  int *piVar8;
  int *piVar9;
  undefined4 uVar10;
  short sVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  TCharacter *pcVar16;
  int *in_stack_00000004;
  int local_84;
  float *local_7c;
  int local_78;
  int local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  int local_5c;
  float *local_58;
  int local_54;
  TCharacter *local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  dword local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  dword *local_8;
  int local_4;
  
  if ((DAT_006695a8 == 0) || (local_34 = 1, DAT_006699cc == 0)) {
    local_34 = 0;
  }
  iVar14 = *(int *)(this->mbr_0x860 + 4);
  iVar12 = *(int *)&(this->TObjectInstance).field_0x2c % iVar14;
  if (iVar12 < 0) {
    iVar12 = iVar12 + iVar14;
  }
  iVar14 = *(int *)(this->mbr_0x860 + 8);
  iVar12 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0 - iVar12;
  iVar13 = *(int *)&(this->TObjectInstance).field_0x30 % iVar14;
  if (iVar13 < 0) {
    iVar13 = iVar13 + iVar14;
  }
  iVar13 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4 - iVar13;
  local_50 = this;
  local_1c = iVar13;
  local_10 = iVar12;
  local_14 = FUN_00416ff0();
  if (local_14 == 0) {
    FUN_00416f70();
  }
  dVar7 = this->mbr_0x860;
  local_44 = iVar13;
  if (iVar13 <= *(int *)(dVar7 + 8) + iVar13) {
    do {
      iVar14 = iVar12;
      local_44 = iVar13;
      local_40 = iVar12;
      if (iVar12 <= *(int *)(dVar7 + 4) + iVar12) {
        do {
          local_3c = 0;
          if (0 < *(int *)(dVar7 + 0x68)) {
            local_48 = 0;
            local_40 = iVar14;
            do {
              piVar8 = (int *)(*(int *)(dVar7 + 0x6c) + local_48);
              local_30 = *piVar8;
              local_2c = piVar8[1];
              local_64 = local_2c + iVar13;
              local_28 = piVar8[2];
              local_60 = local_28 + iVar14;
              local_24 = piVar8[3];
              local_68 = local_30 + iVar14;
              local_5c = local_24 + iVar13;
              iVar12 = FUN_004ad610(&local_68,in_stack_00000004,&local_68);
              if (iVar12 != 0) {
                local_60 = (((local_60 - iVar14) + 0x20U & 0xffffffe0) - 1) + iVar14;
                local_68 = (local_68 - iVar14 & 0xffffffe0U) + iVar14;
                local_64 = (local_64 - iVar13 & 0xffffffe0U) + iVar13;
                local_5c = (((local_5c - iVar13) + 0x20U & 0xffffffe0) - 1) + iVar13;
                local_20 = local_68 - iVar14 >> 5;
                local_c = local_64 - iVar13 >> 5;
                local_4 = (local_60 - local_68) + 1 >> 5;
                local_78 = 0;
                pcVar16 = this;
                do {
                  this = local_50;
                  local_38 = pcVar16->mbr_0x9ac;
                  local_54 = 1;
                  pdVar2 = &pcVar16->mbr_0x9d8 + local_78 * 8;
                  iVar12 = local_38 * local_c + local_20;
                  local_74 = pdVar2[2] + iVar12 * 0xc;
                  local_58 = (float *)(*pdVar2 + iVar12 * 0x20);
                  local_7c = (float *)(iVar12 * 0x20 + pdVar2[1]);
                  iVar13 = (local_38 - local_4) + -1;
                  iVar15 = 0;
                  local_70 = local_64;
                  iVar14 = 0;
                  local_4c = 0;
                  local_8 = pdVar2;
                  if (local_64 <= local_5c + 1) {
                    do {
                      local_6c = local_68;
                      if (local_68 <= local_60 + 1) {
                        fVar5 = (float)((local_70 - local_2c) - local_44) /
                                (float)((local_24 - local_2c) + 1);
                        local_84 = (local_38 + iVar12) * 8;
                        do {
                          local_7c[2] = 1.0;
                          local_58[2] = 1.0;
                          *local_7c = (float)local_6c;
                          *local_58 = (float)local_6c;
                          local_18 = (local_6c - local_30) - local_40;
                          local_7c[1] = (float)local_70;
                          local_58[1] = (float)local_70;
                          local_7c[5] = 0.0;
                          local_58[5] = 0.0;
                          fVar4 = (float)local_18 / (float)((local_28 - local_30) + 1);
                          local_7c[3] = 1.0;
                          local_58[3] = 1.0;
                          local_7c[6] = fVar4;
                          local_58[6] = fVar4;
                          local_7c[7] = fVar5;
                          local_58[7] = fVar5;
                          if ((local_6c < local_60) && (local_70 < local_5c)) {
                            dVar7 = pdVar2[3];
                            if (((((*(short *)(dVar7 + iVar12 * 8) < 0x200) ||
                                  (((*(short *)(dVar7 + 2 + iVar12 * 8) < 0x200 ||
                                    (*(short *)(dVar7 + 4 + iVar12 * 8) < 0x200)) ||
                                   (*(short *)(dVar7 + 8 + iVar12 * 8) < 0x200)))) ||
                                 (((*(short *)(dVar7 + 10 + iVar12 * 8) < 0x200 ||
                                   (*(short *)(dVar7 + 0xc + iVar12 * 8) < 0x200)) ||
                                  (*(short *)(local_84 + dVar7) < 0x200)))) ||
                                ((*(short *)(local_84 + 2 + dVar7) < 0x200 ||
                                 (*(short *)(local_84 + 4 + dVar7) < 0x200)))) ||
                               ((*(short *)(local_84 + 8 + dVar7) < 0x200 ||
                                ((*(short *)(local_84 + 10 + dVar7) < 0x200 ||
                                 (local_54 = 1, *(short *)(local_84 + 0xc + dVar7) < 0x200)))))) {
                              local_54 = 0;
                            }
                            sVar6 = (short)iVar12;
                            if ((((local_78 == 0) || (*(int *)(local_74 + 8) != -10000)) ||
                                (*(int *)(local_74 + 0x14) != -10000)) ||
                               (*(int *)(local_74 + 8 + local_38 * 0xc) != -10000)) {
                              dVar3 = pdVar2[6];
                              *(short *)((dVar3 - 2) + (iVar15 + 1) * 2) = sVar6;
                              *(short *)(dVar3 + (iVar15 + 1) * 2) = sVar6 + 1;
                              sVar11 = *(short *)&local_50->mbr_0x9ac + sVar6;
                              *(short *)(dVar3 + (iVar15 + 2) * 2) = sVar11;
                              iVar15 = iVar15 + 3;
                              if ((local_54 == 0) &&
                                 (((*(short *)(dVar7 + 6 + iVar12 * 8) != 0 ||
                                   (*(short *)(dVar7 + 0xe + iVar12 * 8) != 0)) ||
                                  (*(short *)(local_84 + 6 + dVar7) != 0)))) {
                                dVar3 = pdVar2[7];
                                *(short *)((dVar3 - 2) + (iVar14 + 1) * 2) = sVar6;
                                *(short *)(dVar3 + (iVar14 + 1) * 2) = sVar6 + 1;
                                *(short *)(dVar3 + (iVar14 + 2) * 2) = sVar11;
                                iVar14 = iVar14 + 3;
                                local_4c = iVar14;
                              }
                            }
                            if (((local_78 == 0) || (*(int *)(local_74 + 0x14) != -10000)) ||
                               ((iVar1 = local_74 + local_38 * 0xc, *(int *)(iVar1 + 0x14) != -10000
                                || (*(int *)(iVar1 + 8) != -10000)))) {
                              dVar3 = pdVar2[6];
                              *(short *)(dVar3 + iVar15 * 2) = sVar6 + 1;
                              sVar11 = *(short *)&local_50->mbr_0x9ac + sVar6;
                              iVar15 = iVar15 + 3;
                              *(short *)((dVar3 - 4) + iVar15 * 2) = sVar11 + 1;
                              *(short *)((dVar3 - 2) + iVar15 * 2) = sVar11;
                              iVar14 = local_4c;
                              if ((local_54 == 0) &&
                                 (((*(short *)(dVar7 + 0xe + iVar12 * 8) != 0 ||
                                   (*(short *)(local_84 + 0xe + dVar7) != 0)) ||
                                  (*(short *)(local_84 + 6 + dVar7) != 0)))) {
                                dVar7 = pdVar2[7];
                                *(short *)(dVar7 + local_4c * 2) = sVar6 + 1;
                                *(short *)(dVar7 + (local_4c + 1) * 2) = sVar11 + 1;
                                *(short *)(dVar7 + (local_4c + 2) * 2) = sVar11;
                                local_4c = local_4c + 3;
                                iVar14 = local_4c;
                              }
                            }
                          }
                          local_58 = local_58 + 8;
                          local_6c = local_6c + 0x20;
                          local_7c = local_7c + 8;
                          local_74 = local_74 + 0xc;
                          iVar12 = iVar12 + 1;
                          local_84 = local_84 + 8;
                        } while (local_6c <= local_60 + 1);
                      }
                      local_70 = local_70 + 0x20;
                      local_74 = local_74 + iVar13 * 0xc;
                      local_58 = local_58 + iVar13 * 8;
                      local_7c = local_7c + iVar13 * 8;
                      iVar12 = iVar12 + iVar13;
                    } while (local_70 <= local_5c + 1);
                    if (iVar15 != 0) {
                      piVar8 = *(int **)(*(int *)(local_50->mbr_0x860 + 0x70) + local_3c * 4);
                      if (local_78 == 0) {
                        FUN_00417720(0,2);
                        FUN_00417720(1,2);
                        iVar12 = cls_0x411eb0::meth_0x417060
                                           ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x13);
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                        iVar12 = cls_0x411eb0::meth_0x417060
                                           ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x14);
joined_r0x0045cf75:
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                      }
                      else if (local_78 == 1) {
                        FUN_00417720(0,2);
                        FUN_00417720(1,2);
                        iVar12 = cls_0x411eb0::meth_0x417060
                                           ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x13);
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                        iVar12 = cls_0x411eb0::meth_0x417060
                                           ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x14);
                        goto joined_r0x0045cf75;
                      }
                      piVar9 = (int *)(**(code **)(*piVar8 + 0xc))();
                      uVar10 = (**(code **)(*piVar8 + 0x10))();
                      iVar12 = FUN_004172b0(0,uVar10,piVar9);
                      if (iVar12 != 0) {
                        FUN_004a90d0(iVar12);
                      }
                      if (iVar14 < 1) {
                        iVar14 = FUN_004172b0(1,0,(int *)0x0);
                        if (iVar14 != 0) {
                          FUN_004a90d0(iVar14);
                        }
                        iVar14 = FUN_004174b0(4,0x1c4,*pdVar2,this->mbr_0x9b4,pdVar2[6],iVar15,0);
                        if (iVar14 != 0) {
                          FUN_004a90d0(iVar14);
                        }
                      }
                      else if ((local_34 == 0) || (local_54 == 0)) {
                        iVar12 = FUN_004172b0(1,0,(int *)0x0);
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                        iVar12 = FUN_004174b0(4,0x1c4,*pdVar2,this->mbr_0x9b4,pdVar2[6],iVar15,0);
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                        iVar12 = cls_0x411eb0::meth_0x417060
                                           ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x14);
                        if (iVar12 != 0) {
                          FUN_004a90d0(iVar12);
                        }
                        iVar14 = FUN_004174b0(4,0x1c4,pdVar2[1],this->mbr_0x9b4,pdVar2[7],iVar14,0);
                        if (iVar14 != 0) {
                          FUN_004a90d0(iVar14);
                        }
                      }
                      else {
                        piVar9 = (int *)(**(code **)(*piVar8 + 0xc))();
                        uVar10 = (**(code **)(*piVar8 + 0x10))();
                        iVar14 = FUN_004172b0(1,uVar10,piVar9);
                        if (iVar14 != 0) {
                          FUN_004a90d0(iVar14);
                        }
                        FUN_00417720(0,2);
                        FUN_00417720(1,2);
                        iVar14 = FUN_004174b0(4,0x1c4,*pdVar2,this->mbr_0x9b4,pdVar2[6],iVar15,0);
                        if (iVar14 != 0) {
                          FUN_004a90d0(iVar14);
                        }
                      }
                    }
                  }
                  local_78 = local_78 + 1;
                  iVar13 = local_44;
                  iVar14 = local_40;
                  pcVar16 = this;
                } while (local_78 < 2);
              }
              dVar7 = this->mbr_0x860;
              local_3c = local_3c + 1;
              local_48 = local_48 + 0x10;
              iVar12 = local_10;
            } while (local_3c < *(int *)(dVar7 + 0x68));
          }
          dVar7 = this->mbr_0x860;
          iVar14 = iVar14 + *(int *)(dVar7 + 4);
          local_40 = iVar14;
        } while (iVar14 <= *(int *)(dVar7 + 4) + iVar12);
      }
      dVar7 = this->mbr_0x860;
      iVar13 = iVar13 + *(int *)(dVar7 + 8);
      local_44 = iVar13;
    } while (iVar13 <= *(int *)(dVar7 + 8) + local_1c);
  }
  if (local_14 == 0) {
    FUN_00416fb0();
  }
  return;
}



// Function at 0045d260

void TCharacter::meth_0x45d260()

{
  HANDLE hMutex;
  int iVar1;
  int iVar2;
  dword local_10;
  dword local_c;
  int local_8;
  int local_4;
  
  if ((this->mbr_0x9a8 != 0) && (this->mbr_0x918 == 0)) {
    iVar1 = FUN_00416ff0();
    if (iVar1 == 0) {
      FUN_00416f70();
    }
    cls_0x411eb0::meth_0x412150
              ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),DAT_006663d8,DAT_006663d4,DAT_00667c30);
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),7);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0xe);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),9);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x1a);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x1b);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x1d);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x11);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x12);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x16);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x13);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x14);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    iVar2 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),4);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
    if (DAT_0065844c != 0) {
      FUN_00481e80(DAT_006584b4);
      DAT_00658448 = s_d__revenant_MapPane_cpp_005d0998;
      UNK_006584fc._2188_4_ = 0x1a83;
    }
    local_10 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
    local_c = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
    local_8 = ((this->TObjectInstance).cls_0x41c7f0.mbr_0x8 - 1) + local_10;
    local_4 = ((this->TObjectInstance).cls_0x41c7f0.mbr_0xc - 1) + local_c;
    meth_0x45bd50(this,&local_10);
    this->mbr_0x9c4 = 0;
    meth_0x45c8a0(this);
    if (DAT_0065844c != 0) {
      ReleaseMutex(DAT_006584b4);
      hMutex = DAT_006584b4;
      iVar2 = ReleaseMutex(DAT_006584b4);
      while (iVar2 != 0) {
        iVar2 = ReleaseMutex(hMutex);
      }
      DAT_00658448 = (char *)0x0;
      UNK_006584fc._2188_4_ = 0;
    }
    iVar2 = *(int *)&(this->TObjectInstance).field_0x30;
    this->mbr_0x9bc = *(int *)&(this->TObjectInstance).field_0x2c >> 5;
    this->mbr_0x9c0 = iVar2 >> 5;
    if (iVar1 == 0) {
      FUN_00416fb0();
    }
  }
  return;
}



// Function at 0045f1e0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x45f1e0()

{
  dword dVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  ushort *puVar5;
  ushort *puVar6;
  int iVar7;
  dword dVar8;
  int *piStack_b8;
  dword local_b4;
  dword dStack_b0;
  dword dStack_ac;
  int iStack_a8;
  int aiStack_a4 [20];
  undefined4 auStack_54 [21];
  
  dVar8 = this->mbr_0x88;
  dVar1 = this->mbr_0x8c;
  if ((dVar8 == this->mbr_0x910 << 9) && (dVar1 == this->mbr_0x914 << 9)) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    piVar2 = DAT_00658428;
    piStack_b8 = (int *)this->mbr_0x85c;
    FUN_00438d80(auStack_54,0,0,this->mbr_0x88,this->mbr_0x8c,0x200,0x200,4);
    (**(code **)(*piVar2 + 0x5c))(auStack_54,piStack_b8,0,0);
    puVar5 = (ushort *)(**(code **)(*DAT_00658428 + 0x2c))();
    puVar6 = (ushort *)(**(code **)(*DAT_00658444 + 0x2c))();
    if ((puVar5 != (ushort *)0x0) && (puVar6 != (ushort *)0x0)) {
      FUN_0045d870(puVar5,puVar6);
    }
    (**(code **)(*DAT_00658428 + 0x30))();
    (**(code **)(*DAT_00658444 + 0x30))();
    FUN_0058b100((char *)aiStack_a4,(byte *)s__sautomaps__d__d__d_bmp_005d09b0);
    FUN_004a2960(DAT_00658434,aiStack_a4,1);
    piVar2 = DAT_00658444;
    if ((this->mbr_0x8fc != 0) && (DAT_00658450 != (int *)0x0)) {
      piStack_b8 = DAT_00658450;
      FUN_00438d80(auStack_54,(((int)dVar8 >> 9) - this->mbr_0x900) * 0x40,
                   (((int)dVar1 >> 9) - this->mbr_0x904) * 0x40,0,0,DAT_00658444[1],DAT_00658444[2],
                   0x80000000);
      (**(code **)(*piStack_b8 + 0x5c))(auStack_54,piVar2,0,0);
    }
    iVar7 = DAT_0065c5c4;
    uVar3 = DAT_0065847c;
    if ((int)this->mbr_0x910 < (int)this->mbr_0x908) {
      dVar8 = this->mbr_0x910 + 1;
      this->mbr_0x910 = dVar8;
      FUN_0046dad0(dVar8 * 0x200 + DAT_00667c30 / 2,this->mbr_0x914 * 0x200 + iVar7 / 2,
                   (int *)&local_b4,0);
      FUN_0046d7a0(&local_b4,&iStack_a8,&piStack_b8);
      iVar7 = DAT_0065c5c4 / 2;
      *(int *)&(this->TObjectInstance).field_0x34 = iStack_a8 - DAT_00667c30 / 2;
      dVar8 = this->mbr_0xb0;
      *(int *)&(this->TObjectInstance).field_0x38 = (int)piStack_b8 - iVar7;
      if (((local_b4 != dVar8) || (dStack_b0 != this->mbr_0xb4)) || (dStack_ac != this->mbr_0xb8)) {
        cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_b4,dStack_b0);
      }
    }
    else {
      if ((int)this->mbr_0x90c <= (int)this->mbr_0x914) {
        this->mbr_0x8f8 = 0;
        DAT_006680b0 = uVar3;
        FUN_0058b100((char *)aiStack_a4,(byte *)s__sautomaps_lev_dcomposite_bmp_005d09c8);
        if (DAT_006582f8 != (int *)0x0) {
          FUN_004a2960(DAT_006582f8,aiStack_a4,1);
        }
        if (DAT_00658450 != (int *)0x0) {
          (**(code **)*DAT_00658450)(1);
          DAT_00658450 = (int *)0x0;
        }
        if (DAT_006582f8 == (int *)0x0) {
          return;
        }
        FUN_004830f0(DAT_006582f8);
        DAT_006582f8 = (int *)0x0;
        return;
      }
      dVar8 = this->mbr_0x914 + 1;
      this->mbr_0x914 = dVar8;
      iVar4 = DAT_00667c30;
      this->mbr_0x910 = this->mbr_0x900;
      FUN_0046dad0(this->mbr_0x900 * 0x200 + iVar4 / 2,dVar8 * 0x200 + iVar7 / 2,(int *)&local_b4,0)
      ;
      FUN_0046d7a0(&local_b4,&piStack_b8,&iStack_a8);
      iVar7 = DAT_0065c5c4 / 2;
      *(int *)&(this->TObjectInstance).field_0x34 = (int)piStack_b8 - DAT_00667c30 / 2;
      dVar8 = this->mbr_0xb0;
      *(int *)&(this->TObjectInstance).field_0x38 = iStack_a8 - iVar7;
      if (((local_b4 != dVar8) || (dStack_b0 != this->mbr_0xb4)) || (dStack_ac != this->mbr_0xb8)) {
        cls_0x49beb0::meth_0x49beb0((cls_0x49beb0 *)&DAT_00667548,local_b4,dStack_b0);
      }
    }
    this->mbr_0xa4 = local_b4;
    this->mbr_0xa8 = dStack_b0;
    this->mbr_0xac = dStack_ac;
  }
  else {
    FUN_0046dad0(this->mbr_0x910 * 0x200 + DAT_00667c30 / 2,
                 this->mbr_0x914 * 0x200 + DAT_0065c5c4 / 2,(int *)&local_b4,0);
    meth_0x450d20(this);
  }
  meth_0x4546a0((TCharacter *)&DAT_006668d8);
  UNK_00656e7c._76_4_ = 1;
  return;
}



// Function at 0047a620

TCharacter::TCharacter()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x5bc = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x5c4 = 0;
  this->mbr_0x5c8 = 0;
  this->mbr_0x5cc = 0;
  this->mbr_0x5c0 = 0;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x14 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&TCharacter__vftable_5a5320_005a5320;
  return this;
}



// Function at 0047b4a0

void TCharacter::virt_meth_0x47b4a0_TPlayScreen_Animate()

{
  if ((undefined4 *)this->mbr_0x69c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x69c)();
    this->mbr_0x69c = 0;
  }
  TObjectInstance::virt_meth_0x48ff00_TScreen_Animate(&this->TObjectInstance);
  return;
}



// Function at 0047c2c0

void TCharacter::virt_meth_0x47c2c0_TPlayScreen_MouseClick()

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  dword dVar4;
  longlong lVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  int in_stack_00000004;
  
  if ((this->TObjectInstance).mbr_0x50 == 0) {
    TObjectInstance::virt_meth_0x490030_TScreen_MouseClick(&this->TObjectInstance);
    if (in_stack_00000004 != 0) {
      dVar4 = (this->TObjectInstance).mbr_0x1c;
      if ((int)dVar4 < 1) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(&(this->TObjectInstance).field_0x2c + dVar4 * 4);
      }
      if (((uVar2 & 0xf0) == 0) && (DAT_006682bc == 0)) {
        virt_meth_0x491a80((TCharacter *)&DAT_006668d8);
        FUN_0043a5a0();
      }
    }
    if (((this->mbr_0x5d4 == 0) && (DAT_00668154 == 0)) && (DAT_00666920 == 0)) {
      iVar1 = (**(code **)(DAT_006668d8 + 0x40))();
      if ((iVar1 != 0) && (DAT_00666918 != 0)) {
        dVar4 = (this->TObjectInstance).mbr_0x1c;
        if ((int)dVar4 < 1) {
          uVar2 = 0;
        }
        else {
          uVar2 = *(uint *)(&(this->TObjectInstance).field_0x2c + dVar4 * 4);
        }
        if ((uVar2 & 8) == 0) {
          dVar4 = this->mbr_0x680 + 1;
          uVar2 = (this->mbr_0x688 - this->mbr_0x684) + dVar4;
          this->mbr_0x680 = dVar4;
          lVar5 = __allmul(uVar2,(int)uVar2 >> 0x1f,100,0);
          uVar6 = __alldiv((uint)lVar5,(uint)((ulonglong)lVar5 >> 0x20),0x18,0);
          uVar3 = (uint)uVar6;
          this->mbr_0x68c = uVar3;
          lVar5 = __allmul(uVar3,(int)uVar3 >> 0x1f,0x5a0,0);
          uVar2 = DAT_0065d804;
          uVar7 = __alldiv((uint)lVar5,(uint)((ulonglong)lVar5 >> 0x20),DAT_0065d804,
                           (int)DAT_0065d804 >> 0x1f);
          uVar7 = __allrem((uint)uVar7,(uint)((ulonglong)uVar7 >> 0x20),0x5a0,0);
          this->mbr_0x690 = (dword)uVar7;
          this->mbr_0x694 =
               (dword)((longlong)
                       ((ulonglong)(uint)((int)uVar3 >> 0x1f) << 0x20 | uVar6 & 0xffffffff) /
                      (longlong)(int)uVar2);
        }
      }
    }
  }
  return;
}



// Function at 0047c500

void TCharacter::meth_0x47c500()

{
  dword dVar1;
  dword in_stack_00000004;
  
  dVar1 = this->mbr_0x6a0;
  if (((dVar1 != in_stack_00000004) && (this->mbr_0x6a4 == 0)) && (this->mbr_0x6a8 == 0)) {
    if (in_stack_00000004 == 0) {
      if (dVar1 != 0) {
        this->mbr_0x6a8 = 1;
        return;
      }
    }
    else if (dVar1 == 0) {
      this->mbr_0x6a4 = 1;
    }
  }
  return;
}



// Function at 0047c550

void TCharacter::meth_0x47c550()

{
  dword in_stack_00000004;
  
  this->mbr_0x5d8 = in_stack_00000004;
  if (in_stack_00000004 == 0) {
    meth_0x47c580(this);
  }
  if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
    cls_0x5b4f30_TPlayer::meth_0x472db0(DAT_00667fcc,&DAT_005d7604);
  }
  return;
}



// Function at 0047c580

void TCharacter::meth_0x47c580()

{
  dword in_stack_00000004;
  
  this->mbr_0x5e0 = in_stack_00000004;
  if (in_stack_00000004 != 0) {
    this->mbr_0x5d8 = 0;
    DAT_00666924 = (uint)(in_stack_00000004 == 0);
    return;
  }
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x4903b0_40)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490660_48)(&this->TObjectInstance);
  DAT_00666924 = 1;
  return;
}



// Function at 0047ce80

void TCharacter::virt_meth_0x47ce80_TPlayScreen_Joystick(undefined4 param_1, undefined4 param_2)

{
  cls_0x4922c0 *this_00;
  int *piVar1;
  int iVar2;
  
  if (this->mbr_0x5d4 == 0) {
    if ((((DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) &&
         ((DAT_00667fcc->TPlayScreen).TScreen.mbr_0x84 != 0)) &&
        (iVar2 = cls_0x5b4f30_TPlayer::meth_0x472e30(DAT_00667fcc), iVar2 == 0)) &&
       ((this_00 = (cls_0x4922c0 *)(DAT_00667fcc->TPlayScreen).TScreen.mbr_0x84,
        this_00->mbr_0x1c == 1 && (cls_0x4922c0::meth_0x492490(this_00), this->mbr_0x5d8 != 0)))) {
      this->mbr_0x5d8 = 0;
      meth_0x47c580(this);
      if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
        cls_0x5b4f30_TPlayer::meth_0x472db0(DAT_00667fcc,&DAT_005d7604);
      }
      piVar1 = DAT_0065cb34;
      this->mbr_0x5dc = 1;
      if (piVar1 == (int *)0x0) {
        return;
      }
      (**(code **)(*piVar1 + 0x2c))();
      return;
    }
    TObjectInstance::virt_meth_0x4903b0_TScreen_Joystick(&this->TObjectInstance);
  }
  return;
}



// Function at 0047e340

void TCharacter::meth_0x47e340()

{
  undefined4 uVar1;
  
  if ((DAT_0066829c != 0) && (DAT_0065db48 == 0)) {
    meth_0x47c580(this);
    if (DAT_00667fcc != (TPlayScreen *)0x0) {
      virt_meth_0x44f140((TCharacter *)&DAT_006668d8,5,0);
      TPlayScreen::meth_0x4cee70_Stop(DAT_00667fcc);
    }
    cls_0x5a5e1c::virt_meth_0x463e00((cls_0x5a5e1c *)&DAT_0065da00.field_0x108);
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    uVar1 = meth_0x48ed60(this);
    meth_0x48eea0(this,uVar1);
  }
  return;
}



// Function at 0047e3c0

void TCharacter::meth_0x47e3c0()

{
  int iVar1;
  undefined4 uVar2;
  
  if ((DAT_0066829c != 0) && (DAT_00659800 == 0)) {
    iVar1 = (**(code **)(*DAT_00667fcc + 0x1c0))();
    if ((iVar1 < 1) || ((DAT_00667fcc[0xdb] & 0x100000U) != 0)) {
      meth_0x47c580(this);
      cls_0x5a4e24::virt_meth_0x469370((cls_0x5a4e24 *)&UNK_006596e0.field_0xe0);
      cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
      uVar2 = meth_0x48ed60(this);
      meth_0x48eea0(this,uVar2);
    }
  }
  return;
}



// Function at 0047e500

void TCharacter::meth_0x47e500()

{
  undefined4 uVar1;
  int iVar2;
  
  if (DAT_0066f788 == 0) {
LAB_0047e517:
    do {
      cls_0x5b9480::virt_meth_0x537110((cls_0x5b9480 *)&DAT_0066f748);
      uVar1 = meth_0x48f040(this,&DAT_0066f748);
      cls_0x5b9480::virt_meth_0x537170((cls_0x5b9480 *)&DAT_0066f748);
      switch(uVar1) {
      case 1:
        UNK_0066f8d4._404_4_ = 1;
        cls_0x5b9584::virt_meth_0x539380((cls_0x5b9584 *)&DAT_0066f8d0);
        iVar2 = meth_0x48f040(this,&DAT_0066f8d0);
        cls_0x5b9584::virt_meth_0x539440((cls_0x5b9584 *)&DAT_0066f8d0);
        break;
      case 2:
        cls_0x5b963c::virt_meth_0x5399f0((cls_0x5b963c *)&DAT_0066fb08);
        iVar2 = meth_0x48f040(this,&DAT_0066fb08);
        cls_0x5b963c::virt_meth_0x539ab0((cls_0x5b963c *)&DAT_0066fb08);
        break;
      case 3:
        UNK_0066fcbc._384_4_ = 1;
        cls_0x5b9744::virt_meth_0x53a8b0((cls_0x5b9744 *)&UNK_0066fcbc.field_0x4);
        meth_0x48f040(this,0x66fcc0);
        cls_0x5b9744::virt_meth_0x53aa60((cls_0x5b9744 *)&UNK_0066fcbc.field_0x4);
        UNK_0065cb3c._4_4_ = 1;
        goto LAB_0047e517;
      case 4:
        (this->TObjectInstance).cls_0x41c7f0.mbr_0x14 = 0x65d358;
        meth_0x48ea40(this);
        return;
      case 5:
        PostQuitMessage(0);
      default:
        goto switchD_0047e556_caseD_5;
      }
      UNK_0065cb3c._4_4_ = 1;
    } while (iVar2 == 0);
  }
switchD_0047e556_caseD_5:
  return;
}



// Function at 0047e660

undefined4 TCharacter::meth_0x47e660()

{
  undefined4 uVar1;
  
  UNK_0066f8d4._404_4_ = 1;
  cls_0x5b9584::virt_meth_0x539380((cls_0x5b9584 *)&DAT_0066f8d0);
  uVar1 = meth_0x48f040(this,&DAT_0066f8d0);
  cls_0x5b9584::virt_meth_0x539440((cls_0x5b9584 *)&DAT_0066f8d0);
  UNK_0065cb3c._4_4_ = 1;
  return uVar1;
}



// Function at 0047e6b0

undefined4 TCharacter::meth_0x47e6b0()

{
  undefined4 uVar1;
  
  cls_0x5b963c::virt_meth_0x5399f0((cls_0x5b963c *)&DAT_0066fb08);
  uVar1 = meth_0x48f040(this,&DAT_0066fb08);
  cls_0x5b963c::virt_meth_0x539ab0((cls_0x5b963c *)&DAT_0066fb08);
  UNK_0065cb3c._4_4_ = 1;
  return uVar1;
}



// Function at 0047e700

undefined4 TCharacter::meth_0x47e700()

{
  undefined4 uVar1;
  
  UNK_0066fcbc._384_4_ = 1;
  cls_0x5b9744::virt_meth_0x53a8b0((cls_0x5b9744 *)&UNK_0066fcbc.field_0x4);
  uVar1 = meth_0x48f040(this,0x66fcc0);
  cls_0x5b9744::virt_meth_0x53aa60((cls_0x5b9744 *)&UNK_0066fcbc.field_0x4);
  UNK_0065cb3c._4_4_ = 1;
  return uVar1;
}



// Function at 0047e850

void TCharacter::meth_0x47e850()

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  char local_80 [128];
  
  iVar1 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  if (iVar1 < 0) {
    pcVar2 = s_Quick_Save__d_005d76c4;
  }
  else {
    pcVar2 = (char *)cls_0x45f7c0::meth_0x49d780((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
  }
  cls_0x45f7c0::meth_0x48d260((cls_0x45f7c0 *)&DAT_0065a618);
  iVar1 = 1;
  do {
    FUN_004811b0(local_80,0x80,(byte *)pcVar2);
    iVar3 = cls_0x45f7c0::meth_0x48d6d0((cls_0x45f7c0 *)&DAT_0065a618);
    if (iVar3 < 0) break;
    iVar1 = iVar1 + 1;
  } while (iVar1 < 1000);
  this->mbr_0x5e8 = 1;
  this->mbr_0x5ec = 0xffffffff;
  _strncpy((char *)&this->mbr_0x5f0,local_80,0x7f);
  this->mbr_0x66f = 0;
  return;
}



// Function at 0047e920

int TCharacter::meth_0x47e920()

{
  return (this->mbr_0x688 - this->mbr_0x684) + this->mbr_0x680;
}



// Function at 0047e940

dword TCharacter::meth_0x47e940()

{
  return this->mbr_0x68c;
}



// Function at 0047e950

void TCharacter::meth_0x47e950()

{
  uint uVar1;
  longlong lVar2;
  undefined8 uVar3;
  uint in_stack_00000004;
  
  this->mbr_0x684 = this->mbr_0x680;
  lVar2 = __allmul(in_stack_00000004,(int)in_stack_00000004 >> 0x1f,0x18,0);
  uVar3 = __alldiv((uint)lVar2,(uint)((ulonglong)lVar2 >> 0x20),100,0);
  uVar1 = (uint)uVar3;
  this->mbr_0x688 = uVar1;
  lVar2 = __allmul(uVar1,(int)uVar1 >> 0x1f,100,0);
  uVar3 = __alldiv((uint)lVar2,(uint)((ulonglong)lVar2 >> 0x20),0x18,0);
  lVar2 = __allmul((uint)uVar3,(int)(uint)uVar3 >> 0x1f,0x5a0,0);
  uVar3 = __alldiv((uint)lVar2,(uint)((ulonglong)lVar2 >> 0x20),DAT_0065d804,
                   (int)DAT_0065d804 >> 0x1f);
  uVar3 = __allrem((uint)uVar3,(uint)((ulonglong)uVar3 >> 0x20),0x5a0,0);
  this->mbr_0x690 = (dword)uVar3;
  return;
}



// Function at 0047ebc0

void TCharacter::meth_0x47ebc0()

{
  dword in_stack_00000004;
  
  if (((this->mbr_0x6ac != in_stack_00000004) && (this->mbr_0x6b0 == 0)) && (this->mbr_0x6b4 == 0))
  {
    if (in_stack_00000004 == 0) {
      if (this->mbr_0x6ac != 0) {
        this->mbr_0x6b4 = 1;
        return;
      }
    }
    else {
      this->mbr_0x6c4 = 2;
      this->mbr_0x6b0 = 1;
    }
  }
  return;
}



// Function at 0047ecc0

void TCharacter::meth_0x47ecc0()

{
  if (this->mbr_0x6ac != 0) {
    this->mbr_0x6b4 = 1;
  }
  return;
}



// Function at 0047ece0

void TCharacter::meth_0x47ece0()

{
  if (this->mbr_0x6ac != 0) {
    this->mbr_0x6b4 = 1;
  }
  return;
}



// Function at 0047ed20

dword TCharacter::meth_0x47ed20()

{
  return this->mbr_0x6c0;
}



// Function at 0047ed40

void TCharacter::meth_0x47ed40()

{
  int *piVar1;
  
  meth_0x53cab0((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6c8 = 0;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    UNK_0065b824._4_4_ = 0;
    UNK_0065b824._8_4_ = 0;
    (**(code **)(DAT_0065b7e0 + 0x28))();
  }
  piVar1 = DAT_0065bfe4;
  (**(code **)(*DAT_0065bfe4 + 0x1c))(DAT_0065bfe4[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfe8;
  (**(code **)(*DAT_0065bfe8 + 0x1c))(DAT_0065bfe8[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfec;
  (**(code **)(*DAT_0065bfec + 0x1c))(DAT_0065bfec[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047ee30

void TCharacter::meth_0x47ee30()

{
  int *piVar1;
  
  meth_0x53cb40((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6cc = 1;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  }
  piVar1 = DAT_0065bff4;
  (**(code **)(*DAT_0065bff4 + 0x1c))(DAT_0065bff4[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff8;
  (**(code **)(*DAT_0065bff8 + 0x1c))(DAT_0065bff8[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff0;
  (**(code **)(*DAT_0065bff0 + 0x1c))(DAT_0065bff0[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047ef00

void TCharacter::meth_0x47ef00()

{
  int *piVar1;
  
  meth_0x53cb40((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6cc = 2;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    UNK_006661c4._52_4_ = 0;
    UNK_006661c4._56_4_ = 0;
    (**(code **)(DAT_006661b0 + 0x28))();
  }
  piVar1 = DAT_0065bff4;
  (**(code **)(*DAT_0065bff4 + 0x1c))(DAT_0065bff4[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff8;
  (**(code **)(*DAT_0065bff8 + 0x1c))(DAT_0065bff8[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff0;
  (**(code **)(*DAT_0065bff0 + 0x1c))(DAT_0065bff0[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047eff0

void TCharacter::meth_0x47eff0()

{
  int *piVar1;
  
  meth_0x53cab0((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6c8 = 1;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    DAT_0065b188 = 0;
    DAT_0065b18c._0_4_ = 0;
    (**(code **)(DAT_0065b140 + 0x28))();
  }
  piVar1 = DAT_0065bfe4;
  (**(code **)(*DAT_0065bfe4 + 0x1c))(DAT_0065bfe4[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfe8;
  (**(code **)(*DAT_0065bfe8 + 0x1c))(DAT_0065bfe8[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfec;
  (**(code **)(*DAT_0065bfec + 0x1c))(DAT_0065bfec[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047f0e0

void TCharacter::meth_0x47f0e0()

{
  int *piVar1;
  
  meth_0x53cab0((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6c8 = 2;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    UNK_0065a9dc._68_4_ = 0;
    UNK_0065a9dc._72_4_ = 0;
    (**(code **)(DAT_0065a9d8 + 0x28))();
  }
  piVar1 = DAT_0065bfe4;
  (**(code **)(*DAT_0065bfe4 + 0x1c))(DAT_0065bfe4[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfe8;
  (**(code **)(*DAT_0065bfe8 + 0x1c))(DAT_0065bfe8[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bfec;
  (**(code **)(*DAT_0065bfec + 0x1c))(DAT_0065bfec[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047f1d0

void TCharacter::meth_0x47f1d0()

{
  int *piVar1;
  
  meth_0x53cb40((TCharacter *)&UNK_0066603d.field_0x103);
  this->mbr_0x6cc = 0;
  if (this->mbr_0x6a0 == 0) {
    if ((((DAT_0065d190 != 1) && (DAT_0065d194 == 0)) && (DAT_0065d198 == 0)) && (DAT_0065d190 == 0)
       ) {
      DAT_0065d194 = 1;
    }
  }
  else {
    cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
    UNK_0065d53c._4_4_ = 0;
    UNK_0065d53c._8_4_ = 0;
    (**(code **)(DAT_0065d4f8 + 0x28))();
    (**(code **)(DAT_0065d4f8 + 0x28))();
  }
  piVar1 = DAT_0065bff4;
  (**(code **)(*DAT_0065bff4 + 0x1c))(DAT_0065bff4[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff8;
  (**(code **)(*DAT_0065bff8 + 0x1c))(DAT_0065bff8[5] & 0xfffeffff);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  piVar1 = DAT_0065bff0;
  (**(code **)(*DAT_0065bff0 + 0x1c))(DAT_0065bff0[5] | 0x10000);
  (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
  return;
}



// Function at 0047f4c0

void TCharacter::meth_0x47f4c0(dword param_1, dword param_2, dword param_3)

{
  char *in_stack_00000010;
  
  if (param_1 != 4) {
    this->mbr_0x6d8 = param_1;
    this->mbr_0x6e0 = param_3;
    this->mbr_0x6dc = param_2;
    if (in_stack_00000010 != (char *)0x0) {
      _strncpy((char *)&this->mbr_0x6e4,in_stack_00000010,0x7f);
      this->mbr_0x763 = 0;
      return;
    }
    *(undefined *)&this->mbr_0x6e4 = 0;
  }
  return;
}



// Function at 0047f510

TCharacter * TCharacter::virt_meth_0x47f510_TPlayScreen_Dtor()

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0048ea40

void TCharacter::meth_0x48ea40()

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = 0;
  if (0 < (int)(this->TObjectInstance).mbr_0x1c) {
    piVar3 = (int *)&(this->TObjectInstance).field_0x20;
    do {
      piVar1 = *(int **)((this->TObjectInstance).cls_0x41c7f0.mbr_0x10 + *piVar3 * 4);
      if (piVar1 != (int *)0x0) {
        piVar1[0x17] = 0;
        (**(code **)(*piVar1 + 8))();
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < (int)(this->TObjectInstance).mbr_0x1c);
  }
  piVar3 = (int *)(this->TObjectInstance).mbr_0x44;
  (this->TObjectInstance).mbr_0x54 = 1;
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x2c))();
  }
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490960_64)(&this->TObjectInstance,0x100);
  return;
}



// Function at 0048ead0

undefined4 TCharacter::meth_0x48ead0()

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = (int *)(this->TObjectInstance).mbr_0x40;
  if (piVar1 == (int *)0x0) {
    return 1;
  }
                    /* WARNING: Could not recover jumptable at 0x0048eadf. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  uVar2 = (**(code **)(*piVar1 + 0x20))();
  return uVar2;
}



// Function at 0048eaf0

undefined4 TCharacter::meth_0x48eaf0()

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = (int *)(this->TObjectInstance).mbr_0x44;
  if (piVar1 == (int *)0x0) {
    return 0;
  }
                    /* WARNING: Could not recover jumptable at 0x0048eafc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  uVar2 = (**(code **)(*piVar1 + 0x24))();
  return uVar2;
}



// Function at 0048eb00

undefined4 TCharacter::meth_0x48eb00()

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)(this->TObjectInstance).mbr_0x40;
  if (piVar1 != (int *)0x0) {
    iVar2 = (**(code **)(*piVar1 + 0x18))();
    if (iVar2 != 0) {
      return 1;
    }
  }
  piVar1 = (int *)(this->TObjectInstance).mbr_0x44;
  if (piVar1 != (int *)0x0) {
    iVar2 = (**(code **)(*piVar1 + 0x18))();
    if (iVar2 != 0) {
      return 1;
    }
  }
  return 0;
}



// Function at 0048ed60

uint TCharacter::meth_0x48ed60()

{
  uint uVar1;
  cls_0x41c7f0 *pcVar2;
  int *piVar3;
  int in_stack_00000004;
  
  piVar3 = (int *)(this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
  pcVar2 = &(this->TObjectInstance).cls_0x41c7f0;
  for (uVar1 = 0; (pcVar2 != (cls_0x41c7f0 *)0x0 && (uVar1 < pcVar2->mbr_0x0)); uVar1 = uVar1 + 1) {
    if (in_stack_00000004 == *piVar3) {
      return uVar1;
    }
    piVar3 = piVar3 + 1;
  }
  return 0xffffffff;
}



// Function at 0048eea0

undefined4 TCharacter::meth_0x48eea0(int param_1)

{
  dword dVar1;
  undefined4 uVar2;
  uint in_stack_00000008;
  
  if ((*(int *)((this->TObjectInstance).cls_0x41c7f0.mbr_0x10 + param_1 * 4) == 0) ||
     (dVar1 = (this->TObjectInstance).mbr_0x1c, 3 < (int)dVar1)) {
    uVar2 = 0;
  }
  else {
    *(int *)(&(this->TObjectInstance).field_0x20 + dVar1 * 4) = param_1;
    *(uint *)(&(this->TObjectInstance).field_0x30 + (this->TObjectInstance).mbr_0x1c * 4) =
         in_stack_00000008;
    (this->TObjectInstance).mbr_0x1c = (this->TObjectInstance).mbr_0x1c + 1;
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490960_64)(&this->TObjectInstance,0x101);
    if ((in_stack_00000008 & 0x100) != 0) {
      FUN_004aacb0(0,0,*(int *)(PTR_DAT_005d79e0 + 4),*(int *)(PTR_DAT_005d79e0 + 8));
    }
    uVar2 = 1;
    if ((this->TObjectInstance).mbr_0x1c == 1) {
      DAT_00668504 = (this->TObjectInstance).mbr_0x48;
      return uVar2;
    }
  }
  return uVar2;
}



// Function at 0048efd0

void TCharacter::meth_0x48efd0()

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  uVar3 = 0;
  if (0 < (int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x0) {
    do {
      dVar1 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
      if (((dVar1 != 0) && (uVar3 < (this->TObjectInstance).cls_0x41c7f0.mbr_0x0)) &&
         (*(int *)(dVar1 + uVar3 * 4) != 0)) {
        iVar2 = (**(code **)(**(int **)((this->TObjectInstance).cls_0x41c7f0.mbr_0x10 + uVar3 * 4) +
                            0x3c))();
        if (iVar2 == 0) {
          (**(code **)(**(int **)((this->TObjectInstance).cls_0x41c7f0.mbr_0x10 + uVar3 * 4) + 0x2c))
                    (1);
        }
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)(this->TObjectInstance).cls_0x41c7f0.mbr_0x0);
  }
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  return;
}



// Function at 0048f040

undefined4 TCharacter::meth_0x48f040(int param_1)

{
  int iVar1;
  uint uVar2;
  cls_0x492080 local_c;
  
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  cls_0x492080::meth_0x492080(&local_c);
  for (; ((uint *)local_c.mbr_0x0 != (uint *)0x0 && (local_c.mbr_0x8 < *(uint *)local_c.mbr_0x0));
      local_c.mbr_0x8 = local_c.mbr_0x8 + 1) {
    uVar2 = local_c.mbr_0x8;
    if (param_1 == *(int *)local_c.mbr_0x4) goto LAB_0048f09a;
    local_c.mbr_0x4 = local_c.mbr_0x4 + 4;
  }
  uVar2 = 0xffffffff;
LAB_0048f09a:
  meth_0x48eea0(this,uVar2);
  iVar1 = *(int *)(param_1 + 0x40);
  while (iVar1 != 0) {
    if (DAT_006682b8 != 0) {
      return 0;
    }
    FUN_004911b0(1);
    iVar1 = *(int *)(param_1 + 0x40);
  }
  if (DAT_006682b8 != 0) {
    return 0;
  }
  return *(undefined4 *)(param_1 + 0x5c);
}



// Function at 00491970

void TCharacter::virt_meth_0x491970()

{
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48fda0_16)(&this->TObjectInstance);
  (this->TObjectInstance).mbr_0x40 = 0;
  return;
}



// Function at 00491990

void TCharacter::meth_0x491990()

{
  if (DAT_006682bc == 0) {
    cls_0x4aa0c0::meth_0x4aa0c0((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
  }
  return;
}



// Function at 00491a80

void TCharacter::virt_meth_0x491a80()

{
  int *in_stack_00000004;
  
  if (in_stack_00000004 == (int *)0x0) {
    in_stack_00000004 = (int *)PTR_DAT_005d79e0;
  }
  (**(code **)(*in_stack_00000004 + 0x40))
            ((this->TObjectInstance).cls_0x41c7f0.mbr_0x0 - *(int *)&(this->TObjectInstance).field_0x2c,
             (this->TObjectInstance).cls_0x41c7f0.mbr_0x4 - *(int *)&(this->TObjectInstance).field_0x30);
  (**(code **)(*in_stack_00000004 + 0x44))
            ((this->TObjectInstance).cls_0x41c7f0.mbr_0x0,(this->TObjectInstance).cls_0x41c7f0.mbr_0x4,
             (this->TObjectInstance).cls_0x41c7f0.mbr_0x8,(this->TObjectInstance).cls_0x41c7f0.mbr_0xc);
  (**(code **)(*in_stack_00000004 + 0x48))(0);
  return;
}



// Function at 00491bb0

bool TCharacter::virt_meth_0x491bb0()

{
  return *(int *)&(this->TObjectInstance).field_0x3c == DAT_00667fd0;
}



// Function at 00491bd0

void TCharacter::virt_meth_0x491bd0()

{
  (this->TObjectInstance).mbr_0x50 = 0;
  (this->TObjectInstance).mbr_0x48 = 1;
  (this->TObjectInstance).mbr_0x54 = 1;
  return;
}



// Function at 0052d8a0

undefined4 TCharacter::virt_meth_0x52d8a0()

{
  this->virt_meth_0x491900();
  return 1;
}



// Function at 0052d8b0

void TCharacter::virt_meth_0x491970()

{
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48fda0_16)(&this->TObjectInstance);
  (this->TObjectInstance).mbr_0x40 = 0;
  return;
}



// Function at 0052d8c0

void TCharacter::virt_meth_0x52d8c0()

{
  if (*(char *)&(this->TObjectInstance).mbr_0x64 != '\0') {
    *(undefined *)&(this->TObjectInstance).mbr_0x64 = 0;
  }
  if ((this->TObjectInstance).mbr_0x50 != 0) {
    FUN_004aacb0((this->TObjectInstance).cls_0x41c7f0.mbr_0x0,(this->TObjectInstance).cls_0x41c7f0.mbr_0x4
                 ,(this->TObjectInstance).cls_0x41c7f0.mbr_0x8,
                 (this->TObjectInstance).cls_0x41c7f0.mbr_0xc);
  }
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  return;
}



// Function at 0052d910

void TCharacter::virt_meth_0x52d910()

{
  undefined4 uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  cls_0x5a68d8 *this_00;
  cls_0x5a68d8 *pcVar6;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1903;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 0;
  if (this_00 == (cls_0x5a68d8 *)0x0) {
    pcVar6 = (cls_0x5a68d8 *)0x0;
  }
  else {
    uVar1 = *(undefined4 *)(PTR_DAT_005d79e0 + 4);
    cls_0x5a68d8::cls_0x5a68d8(this_00);
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    this_00->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(this_00,uVar1,0xb2);
    this_00->mbr_0x70 = 1;
    pcVar6 = this_00;
  }
  dVar2 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
  dVar3 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
  dVar4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
  dVar5 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
  local_4 = 0xffffffff;
  (this->TObjectInstance).mbr_0x60 = (dword)pcVar6;
  FUN_004aa850(dVar5,dVar4,dVar3,dVar2,(int)pcVar6);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48ff00_20)(&this->TObjectInstance);
  ExceptionList = this_00;
  return;
}



// Function at 0052d9d0

void TCharacter::virt_meth_0x52d9d0()

{
  undefined4 *puVar1;
  
  FUN_004aa490((this->TObjectInstance).mbr_0x58);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490030_24)(&this->TObjectInstance);
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x60;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  (this->TObjectInstance).mbr_0x60 = 0;
  return;
}



// Function at 0052da00

void TCharacter::virt_meth_0x52da00()

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  int iVar6;
  
  dVar1 = *(dword *)&(this->TObjectInstance).field_0x20;
  dVar2 = (this->TObjectInstance).mbr_0x1c;
  dVar3 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x14;
  dVar4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x0 = dVar4;
  dVar5 = (this->TObjectInstance).mbr_0x58;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x4 = dVar3;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x8 = dVar2;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0xc = dVar1;
  FUN_004aa7c0(dVar5,dVar4,dVar3,dVar2,dVar1);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  iVar6 = meth_0x47ed20((TCharacter *)&DAT_0065caf0);
  if (iVar6 == 2) {
    cls_0x5a5808::virt_meth_0x52c910((cls_0x5a5808 *)&DAT_0065b638);
    return;
  }
  return;
}



// Function at 0052da60

void TCharacter::virt_meth_0x52da60(dword param_1)

{
  int iVar1;
  undefined4 in_stack_00000008;
  
  (this->TObjectInstance).mbr_0x1c = param_1;
  *(undefined4 *)&(this->TObjectInstance).field_0x20 = in_stack_00000008;
  iVar1 = meth_0x47ed20((TCharacter *)&DAT_0065caf0);
  if (iVar1 == 2) {
    cls_0x5a5808::virt_meth_0x52c930((cls_0x5a5808 *)&DAT_0065b638,param_1);
  }
  return;
}



// Function at 00536360

undefined4 TCharacter::virt_meth_0x536360()

{
  TObjectInstance__vftable_5a5ed4 *pcVar1;
  dword dVar2;
  dword *pdVar3;
  undefined **ppuVar4;
  
  this->virt_meth_0x491900();
  dVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065dde4);
  (this->TObjectInstance).mbr_0x64 = dVar2;
  ppuVar4 = &PTR_DAT_005e3fb8;
  pdVar3 = &(this->TObjectInstance).mbr_0x68;
  do {
    dVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065dde4);
    *pdVar3 = dVar2;
    ppuVar4 = ppuVar4 + 1;
    pdVar3 = pdVar3 + 1;
  } while ((int)ppuVar4 < 0x5e3fe4);
  pcVar1 = (this->TObjectInstance).vftptr_0x0;
  this->mbr_0x94 = 0xffffffff;
  this->mbr_0x98 = 0xffffffff;
  (*pcVar1->virt_meth_0x490530_44)(this);
  this->mbr_0xbc = 0xffffffff;
  this->mbr_0xc0 = 0xffffffff;
  this->mbr_0xc4 = 0xffffffff;
  this->mbr_0xc8 = 0xffffffff;
  return 1;
}



// Function at 005366a0

void TCharacter::virt_meth_0x5366a0()

{
  if ((this->TObjectInstance).mbr_0x50 != 0) {
    (*(this->TObjectInstance).vftptr_0x0[1].virt_meth_0x4920b0_8)(this);
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  }
  return;
}



// Function at 005368c0

void TCharacter::virt_meth_0x5368c0()

{
  char cVar1;
  dword dVar2;
  TScreen__vftable_5a50e8 *pcVar3;
  undefined uVar4;
  int *piVar5;
  int iVar6;
  undefined3 extraout_var;
  TPlayScreen *this_00;
  uint uVar7;
  uint uVar8;
  dword *pdVar9;
  char *pcVar10;
  char *pcVar11;
  dword *pdVar12;
  
  this_00 = DAT_00667fcc;
  if (DAT_00667fcc != (TPlayScreen *)0x0) {
    dVar2 = this->mbr_0xc4;
    if (-1 < (int)dVar2) {
      this->mbr_0xc4 = dVar2 + 1;
    }
    if (-1 < (int)this->mbr_0xbc) {
      piVar5 = (int *)TPlayScreen::meth_0x46e8a0(this_00);
      iVar6 = (**(code **)(*piVar5 + 0x90))(this->mbr_0xbc);
      this_00 = DAT_00667fcc;
      if ((int)this->mbr_0xc4 < iVar6) {
        return;
      }
    }
    pcVar3 = (this_00->TScreen).vftptr_0x0;
    this->mbr_0xc0 = this->mbr_0xbc;
    this->mbr_0xc8 = dVar2;
    uVar4 = (*pcVar3->virt_meth_0x477c10_312)(this_00);
    this->mbr_0xbc = CONCAT31(extraout_var,uVar4);
    uVar7 = 0xffffffff;
    pcVar10 = &DAT_005e4060;
    do {
      pcVar11 = pcVar10;
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      pcVar11 = pcVar10 + 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar11;
    } while (cVar1 != '\0');
    uVar7 = ~uVar7;
    this->mbr_0xc4 = 0;
    pdVar9 = (dword *)(pcVar11 + -uVar7);
    pdVar12 = &this->mbr_0x9c;
    for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *pdVar12 = *pdVar9;
      pdVar9 = pdVar9 + 1;
      pdVar12 = pdVar12 + 1;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined *)pdVar12 = *(undefined *)pdVar9;
      pdVar9 = (dword *)((int)pdVar9 + 1);
      pdVar12 = (dword *)((int)pdVar12 + 1);
    }
  }
  return;
}



// Function at 0053c8c0

undefined4 TCharacter::virt_meth_0x53c8c0()

{
  this->virt_meth_0x491900();
  (this->TObjectInstance).mbr_0x60 = 1;
  *(undefined *)((int)&(this->TObjectInstance).mbr_0x6c + 3) = 0;
  *(undefined *)&(this->TObjectInstance).mbr_0x6c = 0;
  (this->TObjectInstance).mbr_0x64 = 0;
  return 1;
}



// Function at 0053c8f0

void TCharacter::virt_meth_0x491970()

{
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48fda0_16)(&this->TObjectInstance);
  (this->TObjectInstance).mbr_0x40 = 0;
  return;
}



// Function at 0053c900

void TCharacter::virt_meth_0x53c900()

{
  if ((((DAT_0065d194 != 0) || (DAT_0065d198 != 0)) ||
      (*(char *)&(this->TObjectInstance).mbr_0x6c != '\0')) ||
     (*(char *)((int)&(this->TObjectInstance).mbr_0x6c + 3) != '\0')) {
    if ((this->TObjectInstance).mbr_0x50 != 0) {
      FUN_004aacb0((this->TObjectInstance).cls_0x41c7f0.mbr_0x0,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x4,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0x8,
                   (this->TObjectInstance).cls_0x41c7f0.mbr_0xc);
    }
    *(undefined *)&(this->TObjectInstance).mbr_0x6c = 0;
    *(undefined *)((int)&(this->TObjectInstance).mbr_0x6c + 3) = 0;
  }
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  return;
}



// Function at 0053c970

void TCharacter::virt_meth_0x53c970()

{
  undefined4 uVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  cls_0x5a68d8 *this_00;
  cls_0x5a68d8 *pcVar6;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1c83;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 0;
  if (this_00 == (cls_0x5a68d8 *)0x0) {
    pcVar6 = (cls_0x5a68d8 *)0x0;
  }
  else {
    uVar1 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
    cls_0x5a68d8::cls_0x5a68d8(this_00);
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    this_00->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(this_00,0xbc,uVar1);
    this_00->mbr_0x70 = 1;
    pcVar6 = this_00;
  }
  dVar2 = (this->TObjectInstance).cls_0x41c7f0.mbr_0xc;
  dVar3 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x8;
  dVar4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x4;
  dVar5 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x0;
  local_4 = 0xffffffff;
  (this->TObjectInstance).mbr_0x68 = (dword)pcVar6;
  FUN_004aa850(dVar5,dVar4,dVar3,dVar2,(int)pcVar6);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x48ff00_20)(&this->TObjectInstance);
  ExceptionList = this_00;
  return;
}



// Function at 0053ca30

void TCharacter::virt_meth_0x53ca30()

{
  undefined4 *puVar1;
  
  FUN_004aa490((this->TObjectInstance).mbr_0x58);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490030_24)(&this->TObjectInstance);
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x68;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  (this->TObjectInstance).mbr_0x68 = 0;
  return;
}



// Function at 0053ca60

void TCharacter::virt_meth_0x53ca60()

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  
  dVar1 = *(dword *)&(this->TObjectInstance).field_0x20;
  dVar2 = (this->TObjectInstance).mbr_0x1c;
  dVar3 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x14;
  dVar4 = (this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x0 = dVar4;
  dVar5 = (this->TObjectInstance).mbr_0x58;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x4 = dVar3;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x8 = dVar2;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0xc = dVar1;
  FUN_004aa7c0(dVar5,dVar4,dVar3,dVar2,dVar1);
  (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  return;
}



// Function at 0053cab0

void TCharacter::meth_0x53cab0()

{
  int iVar1;
  dword dVar2;
  undefined **ppuVar3;
  dword in_stack_00000004;
  
  if (in_stack_00000004 == (this->TObjectInstance).mbr_0x60) {
    iVar1 = (**(code **)(*(int *)(&PTR_DAT_005e46f4)[in_stack_00000004] + 0x3c))();
    if (iVar1 != 0) {
      (**(code **)(*(int *)(&PTR_DAT_005e46f4)[in_stack_00000004] + 0x30))();
      return;
    }
  }
  else {
    dVar2 = 0;
    ppuVar3 = &PTR_DAT_005e46f4;
    do {
      if (dVar2 != in_stack_00000004) {
        (**(code **)(*(int *)*ppuVar3 + 0x34))();
      }
      ppuVar3 = (undefined **)((int **)ppuVar3 + 1);
      dVar2 = dVar2 + 1;
    } while ((int)ppuVar3 < 0x5e4700);
    (**(code **)(*(int *)(&PTR_DAT_005e46f4)[in_stack_00000004] + 0x30))();
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
    cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)&DAT_0065caf0);
    if (in_stack_00000004 != (this->TObjectInstance).mbr_0x60) {
      (this->TObjectInstance).mbr_0x60 = in_stack_00000004;
      *(undefined *)((int)&(this->TObjectInstance).mbr_0x6c + 1) = 1;
    }
  }
  return;
}



// Function at 0053cb40

void TCharacter::meth_0x53cb40()

{
  int iVar1;
  dword dVar2;
  undefined **ppuVar3;
  dword in_stack_00000004;
  
  if (in_stack_00000004 == (this->TObjectInstance).mbr_0x64) {
    iVar1 = (**(code **)(*(int *)(&PTR_DAT_005e4700)[in_stack_00000004] + 0x3c))();
    if (iVar1 != 0) {
      (**(code **)(*(int *)(&PTR_DAT_005e4700)[in_stack_00000004] + 0x30))();
      return;
    }
  }
  else {
    dVar2 = 0;
    ppuVar3 = &PTR_DAT_005e4700;
    do {
      if (dVar2 != in_stack_00000004) {
        (**(code **)(*(int *)*ppuVar3 + 0x34))();
      }
      ppuVar3 = (undefined **)((int **)ppuVar3 + 1);
      dVar2 = dVar2 + 1;
    } while ((int)ppuVar3 < 0x5e470c);
    (**(code **)(*(int *)(&PTR_DAT_005e4700)[in_stack_00000004] + 0x30))();
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
    cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)&DAT_0065caf0);
    if (in_stack_00000004 != (this->TObjectInstance).mbr_0x64) {
      (this->TObjectInstance).mbr_0x64 = in_stack_00000004;
      *(undefined *)((int)&(this->TObjectInstance).mbr_0x6c + 2) = 1;
    }
  }
  return;
}



// Function at 00549740

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 TCharacter::virt_meth_0x549740()

{
  cls_0x5a68d8__vftable_5a68d8 *pcVar1;
  dword dVar2;
  dword dVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  cls_0x46d6b0 *pcVar6;
  cls_0x5a68d8 *pcVar7;
  int iVar8;
  undefined4 *puVar9;
  void *pvStack_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a2174;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->virt_meth_0x491900();
  if (DAT_006680c8 == 0) {
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 0xc;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,0xd);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,0x80,0x80);
      pcVar7->mbr_0x70 = 1;
    }
    uVar4 = _DAT_006668d0;
    pcVar1 = pcVar7->vftptr_0x0;
    dVar2 = pcVar7->mbr_0x8;
    dVar3 = pcVar7->mbr_0x4;
    local_4 = 0xffffffff;
    (this->TObjectInstance).mbr_0x64 = (dword)pcVar7;
    (*pcVar1->virt_meth_0x4bde60_100)(pcVar7,0,0,(char)dVar3,(char)dVar2,(char)uVar4,0xff,0x7f7f);
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    uStack_8 = (undefined *)0xe;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,0xf);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,0x28,0x28);
      pcVar7->mbr_0x70 = 1;
    }
    uStack_8 = (undefined *)0xffffffff;
    (this->TObjectInstance).mbr_0x60 = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    uStack_8 = (undefined *)0x10;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,0x11);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    (this->TObjectInstance).mbr_0x6c = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 0x12;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,0x13);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    this->mbr_0x70 = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 0x14;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,0x15);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    pcVar6 = DAT_0065a9d0;
    local_4 = 0xffffffff;
    this->mbr_0x74 = (dword)pcVar7;
    puVar9 = (undefined4 *)cls_0x46d6b0::meth_0x46d710(pcVar6);
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 0x16;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      uVar4 = puVar9[1];
      uVar5 = *puVar9;
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,0x17);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,uVar5,uVar4);
      pcVar7->mbr_0x70 = 1;
    }
    (this->TObjectInstance).mbr_0x68 = (dword)pcVar7;
  }
  else {
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 0;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,1);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,0x80,0x80);
      pcVar7->mbr_0x70 = 1;
    }
    uVar4 = _DAT_006668d0;
    pcVar1 = pcVar7->vftptr_0x0;
    dVar2 = pcVar7->mbr_0x8;
    dVar3 = pcVar7->mbr_0x4;
    local_4 = 0xffffffff;
    (this->TObjectInstance).mbr_0x64 = (dword)pcVar7;
    (*pcVar1->virt_meth_0x4bde60_100)(pcVar7,0,0,(char)dVar3,(char)dVar2,(char)uVar4,0xff,0x7f7f);
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    uStack_8 = (undefined *)0x2;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,3);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,0x28,0x28);
      pcVar7->mbr_0x70 = 1;
    }
    uStack_8 = (undefined *)0xffffffff;
    (this->TObjectInstance).mbr_0x60 = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    uStack_8 = (undefined *)0x4;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,5);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    (this->TObjectInstance).mbr_0x6c = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 6;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,7);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    this->mbr_0x70 = (dword)pcVar7;
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_4 = 8;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      pcVar7 = (cls_0x5a68d8 *)0x0;
    }
    else {
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,9);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar8 = cls_0x5a68d8::meth_0x4bb5c0(pcVar7,0x80,0x40);
      if (iVar8 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    pcVar6 = DAT_0065a9d0;
    local_4 = 0xffffffff;
    this->mbr_0x74 = (dword)pcVar7;
    puVar9 = (undefined4 *)cls_0x46d6b0::meth_0x46d710(pcVar6);
    pcVar7 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 10;
    if (pcVar7 == (cls_0x5a68d8 *)0x0) {
      (this->TObjectInstance).mbr_0x68 = 0;
    }
    else {
      uVar4 = puVar9[1];
      uVar5 = *puVar9;
      cls_0x5a68d8::cls_0x5a68d8(pcVar7);
      local_4 = CONCAT31(local_4._1_3_,0xb);
      pcVar7->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar7->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar7,uVar5,uVar4);
      pcVar7->mbr_0x70 = 1;
      (this->TObjectInstance).mbr_0x68 = (dword)pcVar7;
    }
  }
  this->mbr_0x94 = 0;
  this->mbr_0xa0 = 0xffffffff;
  this->mbr_0x9c = 0xffffffff;
  this->mbr_0x98 = 0xffffffff;
  this->mbr_0xa8 = 0;
  this->mbr_0xb4 = 0xffffffff;
  this->mbr_0xb0 = 0xffffffff;
  this->mbr_0xac = 0xffffffff;
  this->mbr_0xa4 = 0xffffffff;
  this->mbr_0xc0 = 0;
  this->mbr_0xbc = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0xdc = 0;
  this->mbr_0xd8 = 0;
  this->mbr_0xd4 = 0;
  this->mbr_0xc8 = 0;
  this->mbr_0xc4 = 0;
  this->mbr_0xd0 = 0;
  this->mbr_0xcc = 0;
  ExceptionList = pvStack_c;
  return 1;
}



// Function at 00549d40

void TCharacter::virt_meth_0x549d40()

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x64;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x60;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x6c;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  if ((undefined4 *)this->mbr_0x70 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x70)(1);
  }
  if ((undefined4 *)this->mbr_0x74 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x74)(1);
  }
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x68;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  virt_meth_0x491970(this);
  return;
}



// Function at 0054a0a0

void TCharacter::meth_0x54a0a0()

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  if (DAT_00667fcc != (int *)0x0) {
    piVar3 = (int *)(this->TObjectInstance).mbr_0x6c;
    (**(code **)(*piVar3 + 100))(0,0,piVar3[1],piVar3[2],0,0xffff,0x7f7f,0x80000000);
    iVar1 = (**(code **)(*DAT_00667fcc + 0x130))();
    if (iVar1 != 0) {
      cls_0x5a68d8::meth_0x4bd680
                ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x60,0,0,iVar1,0x80000000);
    }
    uVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    piVar3 = (int *)cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar6 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0,0,uVar2,0x80000000);
    if (iVar1 != 0) {
      (**(code **)(*(int *)(this->TObjectInstance).mbr_0x6c + 0x5c))
                (&stack0xffffff8c,(this->TObjectInstance).mbr_0x60,0,0);
    }
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x1a - (*piVar3 >> 1),
               0x1f - (piVar3[1] >> 1),piVar3,0x2000);
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x2b,0x20,uVar6,0x2000);
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x2b,0x11,uVar5,0x2000);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x2b,3,uVar4,0x2000);
    (**(code **)(*(int *)this->mbr_0x70 + 0x5c))(&stack0xffffff8c,(this->TObjectInstance).mbr_0x6c,0,0)
    ;
  }
  return;
}



// Function at 0054a310

void TCharacter::meth_0x54a310()

{
  int *piVar1;
  cls_0x5a68d8 *this_00;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  if (((((DAT_00667fcc != (int *)0x0) && (piVar4 = (int *)DAT_00667fcc[0x38], piVar4 != (int *)0x0))
       && ((*piVar4 == 3 || ((piVar4 != (int *)0x0 && (*piVar4 == 0x19)))))) &&
      (piVar4 = (int *)piVar4[0x11], piVar4 != (int *)0x0)) && (piVar4 != DAT_00667fcc)) {
    piVar1 = (int *)(this->TObjectInstance).mbr_0x6c;
    (**(code **)(*piVar1 + 100))(0,0,piVar1[1],piVar1[2],0,0xffff,0x7f7f,0x80000000);
    iVar2 = (**(code **)(*piVar4 + 0x130))();
    if ((iVar2 != 0) &&
       (this_00 = (cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x60, this_00 != (cls_0x5a68d8 *)0x0)) {
      cls_0x5a68d8::meth_0x4bd680(this_00,0,0,iVar2,0x80000000);
    }
    uVar3 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    piVar4 = (int *)cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar6 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    uVar7 = cls_0x46d6b0::meth_0x46d710(DAT_0065a9d0);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0,0,uVar3,0x80000000);
    if (iVar2 != 0) {
      (**(code **)(*(int *)(this->TObjectInstance).mbr_0x6c + 0x5c))
                (&stack0xffffff8c,(this->TObjectInstance).mbr_0x60,0,0);
    }
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x62 - (*piVar4 >> 1),
               0x1f - (piVar4[1] >> 1),piVar4,0x2000);
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x3a,0x20,uVar7,0x2000);
    cls_0x5a68d8::meth_0x4bd680
              ((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x3a,0x11,uVar6,0x2000);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)(this->TObjectInstance).mbr_0x6c,0x3a,3,uVar5,0x2000);
    (**(code **)(*(int *)this->mbr_0x74 + 0x5c))(&stack0xffffff8c,(this->TObjectInstance).mbr_0x6c,0,0)
    ;
  }
  return;
}



// Function at 0054a5d0

void TCharacter::meth_0x54a5d0(int param_1, uint param_2, undefined4 param_3, undefined4 param_4, int param_5, undefined param_6, int param_7, int param_8, undefined4 param_9, undefined4 param_10, int param_11, int param_12)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int unaff_EDI;
  undefined3 in_stack_00000019;
  int in_stack_00000034;
  char cVar8;
  undefined uVar9;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  uint local_34;
  undefined4 local_30;
  int local_2c;
  uint local_28;
  uint local_24;
  undefined4 local_20;
  int local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  int local_4;
  
  iVar2 = param_12;
  if ((int)param_2 < 1) {
    param_2 = 1;
  }
  if ((int)param_2 < param_1) {
    param_1 = param_2;
  }
  if (param_1 < 0) {
    param_1 = 0;
  }
  iVar3 = (param_1 * param_11) / (int)param_2;
  if (in_stack_00000034 < 1) {
    iVar5 = param_8;
    if (param_11 - param_12 <= iVar3) {
      iVar5 = _param_6;
    }
    uVar4 = iVar3 - param_12;
    uVar6 = param_11 + param_12 * -2;
    param_2 = uVar6;
    if ((int)(((int)uVar4 < 0) - 1 & uVar4) < (int)uVar6) {
      param_2 = ((int)uVar4 < 0) - 1 & uVar4;
    }
    uVar6 = uVar6 - param_2;
    param_1 = param_8;
    param_12 = iVar5;
  }
  else {
    if (param_12 >> 1 <= iVar3) {
      param_8 = _param_6;
    }
    param_1 = _param_6;
    uVar7 = iVar3 - param_12;
    uVar4 = param_11 + param_12 * -2;
    uVar6 = uVar4;
    if ((int)(((int)uVar7 < 0) - 1 & uVar7) < (int)uVar4) {
      uVar6 = ((int)uVar7 < 0) - 1 & uVar7;
    }
    param_2 = uVar4 - uVar6;
    param_12 = param_8;
  }
  if (DAT_006680c8 != 0) {
    local_2c = param_3;
    local_1c = param_5;
    local_28 = param_4;
    local_18 = param_12;
    local_54 = 0x100;
    local_50 = 0;
    local_4c = 0;
    local_48 = 0;
    local_24 = iVar2;
    local_20 = param_10;
    local_14 = iVar2;
    local_10 = param_10;
    local_4 = 0;
    local_40 = 0;
    local_44 = 0;
    local_30 = 0;
    local_34 = 0;
    local_38 = 0;
    local_3c = 0;
    local_8 = 0x1f;
    local_c = 0;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&local_54,(this->TObjectInstance).mbr_0x68,0,0);
    if (0 < (int)uVar6) {
      local_3c = local_4 + iVar2;
      local_28 = local_c;
      local_2c = param_1 + iVar2;
      local_30 = param_10;
      local_20 = param_10;
      local_14 = 0;
      local_50 = 0;
      local_54 = 0;
      local_40 = 0;
      local_44 = 0;
      local_48 = 0;
      local_4c = 0;
      local_18 = 0x1f;
      local_1c = 0;
      local_34 = uVar6;
      local_24 = uVar6;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
    }
    if (0 < local_8) {
      local_3c = local_4 + uVar6 + iVar2;
      local_28 = param_9;
      local_34 = local_8;
      local_24 = local_8;
      local_2c = uVar6 + param_1 + iVar2;
      local_30 = param_10;
      local_20 = param_10;
      local_14 = 0;
      local_50 = 0;
      local_54 = 0;
      local_40 = 0;
      local_44 = 0;
      local_48 = 0;
      local_4c = 0;
      local_18 = 0x1f;
      local_1c = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
    }
    local_3c = (local_4 - iVar2) + param_7;
    local_2c = (param_1 - iVar2) + param_7;
    local_28 = param_2;
    local_34 = (param_5 - param_7) + iVar2;
    local_30 = param_10;
    local_20 = param_10;
    local_14 = 0;
    local_50 = 0;
    local_54 = 0;
    local_40 = 0;
    local_44 = 0;
    local_48 = 0;
    local_4c = 0;
    local_18 = 0x1f;
    local_1c = 0;
    local_24 = local_34;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
              (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
    return;
  }
  cVar8 = (char)param_3;
  uVar9 = (undefined)param_4;
  cls_0x411eb0::meth_0x414d70
            ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),cVar8,uVar9,1,(this->TObjectInstance).mbr_0x68,0)
  ;
  cVar1 = (char)iVar2;
  if (0 < (int)uVar6) {
    cls_0x411eb0::meth_0x414d70
              ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),cVar8 + cVar1,uVar9,1,
               (this->TObjectInstance).mbr_0x68,0);
  }
  if (0 < (int)param_2) {
    cls_0x411eb0::meth_0x414d70
              ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)uVar6 + cVar8 + cVar1,uVar9,1,
               (this->TObjectInstance).mbr_0x68,0);
  }
  cls_0x411eb0::meth_0x414d70
            ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(cVar8 - cVar1) + (char)param_11,uVar9,1,
             (this->TObjectInstance).mbr_0x68,0);
  return;
}



// Function at 0054bf70

undefined4 TCharacter::virt_meth_0x54bf70()

{
  cls_0x5a68d8__vftable_5a68d8 *pcVar1;
  undefined *puVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  undefined4 *puVar6;
  int iVar7;
  cls_0x5a68d8 *pcVar8;
  dword dVar9;
  dword dVar10;
  bool bVar11;
  cls_0x5a68d8 *local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  dVar10 = DAT_005e5804;
  puVar2 = PTR_DAT_005d79e0;
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a2222;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->TObjectInstance).mbr_0x64 = DAT_005e5800;
  dVar9 = *(dword *)(puVar2 + 4);
  (this->TObjectInstance).mbr_0x68 = dVar10;
  iVar5 = DAT_0065b020;
  this->mbr_0x70 = dVar9;
  iVar5 = *(int *)(iVar5 + DAT_0065abc4 * 4);
  if (iVar5 == 0) {
    iVar5 = DAT_0065b024;
  }
  iVar3 = cls_0x4acb80::meth_0x521c60((cls_0x4acb80 *)&UNK_0065abf8.field_0x418);
  iVar7 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
  this->mbr_0x74 = (*(int *)(iVar3 + 0x54) + *(int *)(iVar5 + 0x50)) * 0xc;
  if (iVar7 == 0) {
    iVar7 = DAT_0065b024;
  }
  iVar3 = cls_0x4acb80::meth_0x521c60((cls_0x4acb80 *)&UNK_0065abf8.field_0x418);
  dVar10 = DAT_006668e4;
  iVar5 = DAT_006668e0;
  dVar9 = DAT_006668dc;
  dVar4 = *(int *)(iVar3 + 0x54) + *(int *)(iVar7 + 0x50);
  iVar3 = DAT_005e5800 * dVar4;
  this->mbr_0x78 = dVar4;
  iVar7 = DAT_006668e8;
  *(int *)&(this->TObjectInstance).field_0x20 = iVar3;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x10 = dVar9;
  (this->TObjectInstance).cls_0x41c7f0.mbr_0x14 = (iVar7 - iVar3) + iVar5;
  (this->TObjectInstance).mbr_0x1c = dVar10;
  iVar5 = this->virt_meth_0x491900();
  if (iVar5 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  puVar6 = FUN_00482fb0(0x450);
  (this->TObjectInstance).mbr_0x6c = (dword)puVar6;
  bVar11 = DAT_006680c8 == 0;
  (this->TObjectInstance).mbr_0x60 = 0;
  if (bVar11) {
    local_10 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 6;
    if (local_10 == (cls_0x5a68d8 *)0x0) {
      pcVar8 = (cls_0x5a68d8 *)0x0;
    }
    else {
      iVar5 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
      if (iVar5 == 0) {
        iVar5 = DAT_0065b024;
      }
      iVar7 = cls_0x4acb80::meth_0x521c60((cls_0x4acb80 *)&UNK_0065abf8.field_0x418);
      iVar7 = *(int *)(iVar7 + 0x54);
      iVar5 = *(int *)(iVar5 + 0x50);
      dVar9 = this->mbr_0x70;
      cls_0x5a68d8::cls_0x5a68d8(local_10);
      local_4 = CONCAT31(local_4._1_3_,7);
      local_10->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      local_10->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(local_10,dVar9,iVar7 + iVar5);
      local_10->mbr_0x70 = 1;
      pcVar8 = local_10;
    }
    dVar9 = pcVar8->mbr_0x8;
    pcVar1 = pcVar8->vftptr_0x0;
    dVar10 = pcVar8->mbr_0x4;
    local_4 = 0xffffffff;
    this->mbr_0x84 = (dword)pcVar8;
    (*pcVar1->virt_meth_0x4bde60_100)(pcVar8,0,0,(char)dVar10,(char)dVar9,0,0xff,0x7f7f);
    pcVar8 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    uStack_8 = (undefined *)0x8;
    if (pcVar8 == (cls_0x5a68d8 *)0x0) {
      pcVar8 = (cls_0x5a68d8 *)0x0;
    }
    else {
      dVar9 = this->mbr_0x74;
      dVar10 = this->mbr_0x70;
      cls_0x5a68d8::cls_0x5a68d8(pcVar8);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,9);
      pcVar8->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar5 = cls_0x5a68d8::meth_0x4bb5c0(pcVar8,dVar10,dVar9);
      if (iVar5 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    dVar9 = pcVar8->mbr_0x8;
    pcVar1 = pcVar8->vftptr_0x0;
    dVar10 = pcVar8->mbr_0x4;
    uStack_8 = (undefined *)0xffffffff;
    this->mbr_0x88 = (dword)pcVar8;
    (*pcVar1->virt_meth_0x4bde60_100)(pcVar8,0,0,(char)dVar10,(char)dVar9,0,0xff,0x7f7f);
    pcVar8 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    local_c = (void *)0xa;
    if (pcVar8 == (cls_0x5a68d8 *)0x0) goto LAB_0054c369;
    dVar9 = this->mbr_0x74;
    dVar10 = this->mbr_0x70;
    cls_0x5a68d8::cls_0x5a68d8(pcVar8);
    local_c = (void *)CONCAT31(local_c._1_3_,0xb);
    pcVar8->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
  }
  else {
    local_10 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 0;
    if (local_10 == (cls_0x5a68d8 *)0x0) {
      pcVar8 = (cls_0x5a68d8 *)0x0;
    }
    else {
      iVar5 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
      if (iVar5 == 0) {
        iVar5 = DAT_0065b024;
      }
      iVar7 = cls_0x4acb80::meth_0x521c60((cls_0x4acb80 *)&UNK_0065abf8.field_0x418);
      iVar7 = *(int *)(iVar7 + 0x54);
      iVar5 = *(int *)(iVar5 + 0x50);
      dVar9 = this->mbr_0x70;
      cls_0x5a68d8::cls_0x5a68d8(local_10);
      local_4 = CONCAT31(local_4._1_3_,1);
      local_10->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      local_10->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(local_10,dVar9,iVar7 + iVar5);
      local_10->mbr_0x70 = 1;
      pcVar8 = local_10;
    }
    dVar9 = pcVar8->mbr_0x8;
    pcVar1 = pcVar8->vftptr_0x0;
    dVar10 = pcVar8->mbr_0x4;
    local_4 = 0xffffffff;
    this->mbr_0x84 = (dword)pcVar8;
    (*pcVar1->virt_meth_0x4bde60_100)(pcVar8,0,0,(char)dVar10,(char)dVar9,0,0xff,0x7f7f);
    pcVar8 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    uStack_8 = (undefined *)0x2;
    if (pcVar8 == (cls_0x5a68d8 *)0x0) {
      pcVar8 = (cls_0x5a68d8 *)0x0;
    }
    else {
      dVar9 = this->mbr_0x74;
      dVar10 = this->mbr_0x70;
      cls_0x5a68d8::cls_0x5a68d8(pcVar8);
      uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,3);
      pcVar8->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
      iVar5 = cls_0x5a68d8::meth_0x4bb5c0(pcVar8,dVar10,dVar9);
      if (iVar5 == 0) {
        FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    uStack_8 = (undefined *)0xffffffff;
    this->mbr_0x88 = (dword)pcVar8;
    pcVar8 = (cls_0x5a68d8 *)FUN_00482fb0(0x74);
    uStack_8 = (undefined *)0x4;
    if (pcVar8 == (cls_0x5a68d8 *)0x0) {
LAB_0054c369:
      pcVar8 = (cls_0x5a68d8 *)0x0;
      goto LAB_0054c36b;
    }
    dVar9 = this->mbr_0x74;
    dVar10 = this->mbr_0x70;
    cls_0x5a68d8::cls_0x5a68d8(pcVar8);
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,5);
    pcVar8->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
  }
  iVar5 = cls_0x5a68d8::meth_0x4bb5c0(pcVar8,dVar10,dVar9);
  if (iVar5 == 0) {
    FUN_00481c10((byte *)s_Couldn_t_initialize_mosaic_surfa_005cdc70);
  }
LAB_0054c36b:
  this->mbr_0x8c = (dword)pcVar8;
  this->mbr_0x120 = 0xffffffff;
  this->mbr_0x7c = pcVar8->mbr_0x8 - this->mbr_0x78;
  this->mbr_0xa0 = 0;
  *(undefined *)&this->mbr_0xd0 = 0;
  this->mbr_0x90 = 0;
  this->mbr_0x94 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  ExceptionList = local_10;
  return 1;
}



// Function at 0054c3d0

void TCharacter::virt_meth_0x54c3d0()

{
  undefined4 *puVar1;
  
  FUN_004830f0((LPCVOID)(this->TObjectInstance).mbr_0x6c);
  puVar1 = (undefined4 *)this->mbr_0x84;
  (this->TObjectInstance).mbr_0x6c = 0;
  if (puVar1 != (undefined4 *)0x0) {
    (**(code **)*puVar1)(1);
  }
  this->mbr_0x84 = 0;
  if ((undefined4 *)this->mbr_0x88 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x88)(1);
  }
  this->mbr_0x88 = 0;
  if ((undefined4 *)this->mbr_0x8c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x8c)(1);
  }
  this->mbr_0x8c = 0;
  virt_meth_0x491970(this);
  return;
}



// Function at 0054c440

void TCharacter::virt_meth_0x54c440()

{
  if ((this->TObjectInstance).mbr_0x50 != 0) {
    meth_0x54cd40(this);
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490530_44)(this);
  }
  return;
}



// Function at 0054c9c0

void TCharacter::virt_meth_0x54c9c0()

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x6c;
  this->mbr_0xa0 = 0;
  *puVar1 = 0x40;
  meth_0x54cd40(this);
  meth_0x54d700(this);
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x6c;
  this->mbr_0x90 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  *puVar1 = 1;
  meth_0x54cd40(this);
  (this->TObjectInstance).mbr_0x48 = 1;
  (this->TObjectInstance).mbr_0x4c = 1;
  return;
}



// Function at 0054ca20

void TCharacter::meth_0x54ca20()

{
  cls_0x419dd0 cVar1;
  
  this->mbr_0x94 = 1;
  this->mbr_0x90 = 1;
  cVar1.mbr_0x0 = (dword)this;
  cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffff4);
  meth_0x54d0c0(this,0x80,cVar1.mbr_0x0);
  return;
}



// Function at 0054ca60

void TCharacter::meth_0x54ca60(dword param_1)

{
  dword in_stack_00000008;
  cls_0x419dd0 cVar1;
  
  if (this->mbr_0x90 == 0) {
    cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    cVar1.mbr_0x0 = 1;
    this->mbr_0x94 = 1;
    this->mbr_0x90 = 1;
    cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffff4);
    meth_0x54d0c0(this,0x80,cVar1.mbr_0x0);
  }
  this->mbr_0x9c = in_stack_00000008;
  this->mbr_0x98 = param_1;
  meth_0x54cd40(this);
  return;
}



// Function at 0054cad0

void TCharacter::meth_0x54cad0()

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)(this->TObjectInstance).mbr_0x6c;
  this->mbr_0x90 = 0;
  this->mbr_0x98 = 0;
  this->mbr_0x9c = 0;
  *puVar1 = 1;
  meth_0x54cd40(this);
  return;
}



// Function at 0054cb00

void TCharacter::meth_0x54cb00()

{
  uint uVar1;
  dword dVar2;
  dword dVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 uVar6;
  
  dVar2 = this->mbr_0x9c;
  dVar3 = this->mbr_0x98;
  iVar5 = dVar2 - dVar3;
  if (iVar5 < 1) {
    iVar5 = dVar3 - dVar2;
  }
  if (iVar5 < 5) {
    this->mbr_0x98 = dVar2;
  }
  else {
    if ((int)dVar3 < (int)dVar2) {
      dVar3 = dVar3 + 4;
    }
    else {
      if ((int)dVar3 <= (int)dVar2) goto LAB_0054cb3d;
      dVar3 = dVar3 - 4;
    }
    this->mbr_0x98 = dVar3;
  }
LAB_0054cb3d:
  uVar1 = this->mbr_0x98 - 0xba;
  uVar6 = 0x100;
  uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0);
  cls_0x4bd8c0::meth_0x4bd8c0
            ((cls_0x4bd8c0 *)this->mbr_0x84,(0 < (int)uVar1) - 1 & uVar1,1,uVar4,uVar6);
  return;
}



// Function at 0054cbb0

void TCharacter::meth_0x54cbb0()

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  dword dVar5;
  undefined *puVar6;
  undefined uVar7;
  undefined3 extraout_var;
  int iVar8;
  int iVar9;
  undefined4 uStack_70;
  undefined4 auStack_54 [21];
  
  if ((this->TObjectInstance).mbr_0x40 != 0) {
    uVar7 = (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x46cf30_60)(this);
    if ((CONCAT31(extraout_var,uVar7) == 0) && (this->mbr_0x90 != 0)) {
      if (this->mbr_0x94 != 0) {
        (*((DAT_00667fd0->TObjectInstance).vftptr_0x0)->virt_meth_0x491870_72)
                  (&DAT_00667fd0->TObjectInstance);
        this->mbr_0x94 = 0;
        return;
      }
      uVar1 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x14);
      uVar2 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x18);
      uVar3 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x20);
      uVar4 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x1c);
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      (*(this->TObjectInstance).vftptr_0x0[1].virt_meth_0x492060_0)(&this->TObjectInstance);
      iVar9 = *(int *)(PTR_DAT_005d79e0 + 8) - this->mbr_0x78;
      iVar8 = meth_0x48eaf0(DAT_00667fd0);
      puVar6 = PTR_DAT_005d79e0;
      if (iVar8 != 0) {
        iVar9 = *(int *)(PTR_DAT_005d79e0 + 8) - this->mbr_0x78;
      }
      if (DAT_006680c8 == 0) {
        cls_0x411eb0::meth_0x414d70
                  ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0,(char)iVar9,1,this->mbr_0x8c,0);
      }
      else {
        dVar5 = this->mbr_0x8c;
        FUN_00438d80(auStack_54,0,iVar9,0,this->mbr_0x7c,(this->TObjectInstance).cls_0x41c7f0.mbr_0x8,
                     this->mbr_0x78,0x120);
        (**(code **)(*(int *)puVar6 + 0x5c))(auStack_54,dVar5,0,0);
      }
      cls_0x4aa0c0::meth_0x4aa0c0((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
      puVar6 = PTR_DAT_005d79e0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x48))(uVar4);
      (**(code **)(*(int *)puVar6 + 0x40))(uStack_70,uVar1);
      (**(code **)(*(int *)puVar6 + 0x44))(uStack_70,uVar1,uVar2,uVar3);
    }
  }
  return;
}



// Function at 0054cd40

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TCharacter::meth_0x54cd40()

{
  undefined4 *puVar1;
  dword extraout_ECX;
  dword extraout_ECX_00;
  cls_0x419dd0 cVar2;
  int iVar3;
  dword dVar4;
  undefined4 uVar5;
  undefined4 uStack_14c;
  int iStack_148;
  int *piStack_144;
  undefined4 uStack_140;
  undefined4 uStack_13c;
  undefined4 uStack_138;
  undefined4 uStack_134;
  undefined4 uStack_130;
  int iStack_12c;
  int iStack_128;
  undefined4 uStack_124;
  dword dStack_120;
  undefined4 auStack_e8 [21];
  undefined4 auStack_94 [22];
  int iStack_3c;
  dword dStack_1c;
  
  dStack_120 = 0x80000000;
  uStack_124 = 0x7f7f;
  iStack_128 = 0xffff;
  iStack_12c = 0x997b;
  uStack_130 = 300;
  uStack_134 = 0x28;
  uStack_138 = 0;
  uStack_13c = 0x28;
  uStack_140 = 0x54cd77;
  (**(code **)(*(int *)this->mbr_0x84 + 100))();
  dVar4 = this->mbr_0x7c;
  dStack_120 = 0;
  if (0 < (int)(this->TObjectInstance).mbr_0x60) {
    iStack_128 = 0;
    do {
      if ((dStack_1c == 0xffffffff) || (dStack_1c == dStack_120)) {
        puVar1 = (undefined4 *)(iStack_128 + (this->TObjectInstance).mbr_0x6c);
        iStack_12c = DAT_0067064c;
        switch(*puVar1) {
        case 2:
          iStack_12c = DAT_00670664;
          break;
        case 4:
          iStack_12c = DAT_00670654;
          break;
        case 8:
          iStack_12c = DAT_00670668;
          break;
        case 0x10:
          iStack_12c = DAT_0067065c;
          break;
        case 0x20:
          iStack_12c = DAT_00670658;
        case 1:
          break;
        case 0x40:
          iStack_12c = puVar1[1];
          if (iStack_12c == 0) {
            iStack_12c = DAT_00670660;
          }
          break;
        case 0x80:
          iStack_12c = DAT_00670650;
        }
        uStack_140 = 0x80000000;
        piStack_144 = (int *)0x7f7f;
        iStack_148 = 0xffff;
        uStack_14c = _DAT_006668d0;
        (**(code **)(*(int *)this->mbr_0x84 + 100))();
        cVar2.mbr_0x0 = extraout_ECX;
        if ((iStack_3c == 0) && (this->mbr_0x90 != 0)) {
          meth_0x54cb00(this);
          cVar2.mbr_0x0 = extraout_ECX_00;
        }
        uVar5 = 0x400;
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffe98);
        cls_0x5a68d8::meth_0x438ed0
                  ((cls_0x5a68d8 *)this->mbr_0x84,4,9,
                   iStack_148 + 0xc + (this->TObjectInstance).mbr_0x6c,(char)DAT_0065abc4,cVar2.mbr_0x0
                   ,uVar5);
        if (DAT_006680c8 == 0) {
          (**(code **)(*(int *)this->mbr_0x88 + 100))
                    (0,dVar4,((int *)this->mbr_0x88)[1],this->mbr_0x78,0);
          piStack_144 = (int *)this->mbr_0x88;
          FUN_00438d80(auStack_94,0,dVar4,0,0,*(undefined4 *)(this->mbr_0x84 + 4),
                       *(undefined4 *)(this->mbr_0x84 + 8),0x100);
          puVar1 = auStack_94;
          iVar3 = *piStack_144;
        }
        else {
          piStack_144 = (int *)this->mbr_0x88;
          FUN_00438d80(auStack_e8,0,dVar4,0,0,*(undefined4 *)(this->mbr_0x84 + 4),
                       *(undefined4 *)(this->mbr_0x84 + 8),0x80000000);
          puVar1 = auStack_e8;
          iVar3 = *piStack_144;
        }
        (**(code **)(iVar3 + 0x5c))(puVar1);
        uStack_14c = 0x80000000;
        iStack_148 = 0;
        piStack_144 = (int *)0x0;
        uStack_140 = 0;
        uStack_124 = 0;
        uStack_138 = 0;
        uStack_13c = 0;
        iStack_128 = 0;
        iStack_12c = 0;
        uStack_130 = 0;
        uStack_134 = 0;
        dStack_120 = dVar4;
        (**(code **)(*(int *)this->mbr_0x8c + 0x5c))(&uStack_14c,this->mbr_0x88,0,0);
      }
      dVar4 = dVar4 - this->mbr_0x78;
      if ((int)dVar4 < 0) {
        dVar4 = dVar4 + *(int *)(this->mbr_0x84 + 8);
      }
      dStack_120 = dStack_120 + 1;
      iStack_128 = iStack_128 + 0x5c;
    } while ((int)dStack_120 < (int)(this->TObjectInstance).mbr_0x60);
  }
  return;
}



// Function at 0054d0c0

void TCharacter::meth_0x54d0c0(undefined4 param_1, undefined4 param_2)

{
  undefined4 *puVar1;
  char *_Dest;
  dword dVar2;
  uint uVar3;
  int iVar4;
  char *in_stack_0000000c;
  
  if (((in_stack_0000000c != (char *)0x0) && (*in_stack_0000000c != '\0')) &&
     (*in_stack_0000000c != ' ')) {
    uVar3 = (uint)(this->mbr_0xa0 != 0);
    dVar2 = (this->TObjectInstance).mbr_0x6c;
    iVar4 = uVar3 * 0x5c;
    FUN_0058b790((undefined4 *)(dVar2 + (uVar3 + 1) * 0x5c),(undefined4 *)(dVar2 + iVar4),
                 uVar3 * -0x5c + 0x3f4);
    dVar2 = (this->TObjectInstance).mbr_0x60;
    if ((int)dVar2 < (int)(this->TObjectInstance).mbr_0x64) {
      (this->TObjectInstance).mbr_0x60 = dVar2 + 1;
    }
    _Dest = (char *)((this->TObjectInstance).mbr_0x6c + 0xc + iVar4);
    _strncpy(_Dest,in_stack_0000000c,0x4f);
    dVar2 = (this->TObjectInstance).mbr_0x6c;
    _Dest[0x4f] = '\0';
    puVar1 = (undefined4 *)(dVar2 + iVar4);
    *puVar1 = param_1;
    puVar1[1] = param_2;
    puVar1[2] = 0x78;
    meth_0x54cd40(this);
  }
  return;
}



// Function at 0054d1b0

void TCharacter::meth_0x54d1b0(undefined4 param_1, byte *param_2)

{
  uint *puVar1;
  FILE *pFVar2;
  uint *puVar3;
  dword extraout_ECX;
  dword extraout_ECX_00;
  undefined4 *in_stack_0000000c;
  cls_0x419dd0 cVar4;
  char local_1c0 [64];
  char local_180 [128];
  uint local_100 [64];
  
  if ((this->TObjectInstance).mbr_0x40 != 0) {
    if (DAT_00668178 != 0) {
      FUN_0058b100(local_180,(byte *)s__sTextDump_txt_005e582c);
      pFVar2 = (FILE *)FUN_0058b5db(local_180,&DAT_005e583c);
      if (pFVar2 != (FILE *)0x0) {
        if (DAT_005e5808 != 0) {
          DAT_005e5808 = 0;
          FUN_0058ec43(local_1c0);
          FUN_0058b56e((char **)pFVar2,(byte *)s_Revenant_Text_Dump_executed_at___005e5840);
        }
        FUN_0058ec08((char **)pFVar2,param_2,in_stack_0000000c);
        FUN_0058b56e((char **)pFVar2,&DAT_005e5864);
        FUN_0058b4f1(pFVar2);
      }
    }
    FUN_0058bced((char *)local_100,param_2,in_stack_0000000c);
    puVar1 = local_100;
    puVar3 = FUN_0058ade0(local_100,'\n');
    cVar4.mbr_0x0 = extraout_ECX;
    while (puVar3 != (uint *)0x0) {
      *(undefined *)puVar3 = 0;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffe28);
      meth_0x54d0c0(this,param_1,cVar4.mbr_0x0);
      puVar1 = (uint *)((int)puVar3 + 1);
      puVar3 = FUN_0058ade0(puVar1,'\n');
      cVar4.mbr_0x0 = extraout_ECX_00;
    }
    if (*(char *)puVar1 != '\0') {
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffe28);
      meth_0x54d0c0(this,param_1,cVar4.mbr_0x0);
    }
  }
  return;
}



// Function at 0054d2f0

void TCharacter::meth_0x54d2f0()

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  cls_0x419dd0 cVar4;
  
  if (this->mbr_0xa0 == 0) {
    DAT_0065a9c8 = DAT_0065a9c8 | DAT_0065a9c4;
    DAT_0065a9c4 = 0;
    if (DAT_00667fcc != (TPlayScreen *)0x0) {
      TPlayScreen::meth_0x4cee70_Stop(DAT_00667fcc);
      TPlayScreen::meth_0x4cf000_SetWalkMode(DAT_00667fcc);
    }
    pcVar2 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    uVar3 = 0xffffffff;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar2 + 1;
    } while (cVar1 != '\0');
    cVar4.mbr_0x0 = ~uVar3 - 1;
    this->mbr_0xa4 = 0x50 - cVar4.mbr_0x0;
    *(undefined *)&this->mbr_0xd0 = 0;
    cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xfffffff0);
    meth_0x54d0c0(this,0x20,cVar4.mbr_0x0);
    this->mbr_0xa0 = 1;
  }
  return;
}



// Function at 0054d390

void TCharacter::meth_0x54d390()

{
  char cVar1;
  undefined4 *puVar2;
  dword dVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  char *pcVar8;
  undefined4 local_4;
  
  iVar4 = DAT_00667fcc;
  if (this->mbr_0xa0 != 0) {
    puVar2 = (undefined4 *)(this->TObjectInstance).mbr_0x6c;
    this->mbr_0xa0 = 0;
    cVar1 = *(char *)(iVar4 + 0x494);
    *puVar2 = 0x40;
    local_4 = DAT_00670660;
    if (cVar1 != '\0') {
      if (cVar1 == '\0') {
        local_4 = 0xffffff;
      }
      else {
        iVar6 = *(int *)(iVar4 + 0x4d8);
        if (0xf < iVar6) {
          iVar6 = 0x10;
        }
        local_4 = *(undefined4 *)(&DAT_005e20b8 + iVar6 * 4);
      }
    }
    puVar2[1] = local_4;
    _strncpy((char *)(puVar2 + 3),*(char **)(iVar4 + 0x38),0x4f);
    dVar3 = (this->TObjectInstance).mbr_0x6c;
    *(undefined *)((int)puVar2 + 0x5b) = 0;
    pcVar7 = (char *)(dVar3 + 0xc);
    uVar5 = 0xffffffff;
    pcVar8 = pcVar7;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar5 - 1);
    pcVar7 = pcVar7 + (~uVar5 - 1);
    _strncpy(pcVar7,&DAT_005e5874,iVar4 + 0x4f);
    dVar3 = (this->TObjectInstance).mbr_0x6c;
    pcVar7[iVar4 + 0x4f] = '\0';
    pcVar7 = (char *)(dVar3 + 0xc);
    uVar5 = 0xffffffff;
    pcVar8 = pcVar7;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar5 - 1);
    pcVar7 = pcVar7 + (~uVar5 - 1);
    _strncpy(pcVar7,(char *)&this->mbr_0xd0,iVar4 + 0x4f);
    pcVar7[iVar4 + 0x4f] = '\0';
    meth_0x54cd40(this);
    meth_0x54d700(this);
  }
  return;
}



// Function at 0054d4a0

void TCharacter::virt_meth_0x54d4a0(int param_1)

{
  char cVar1;
  int *piVar2;
  dword dVar3;
  dword dVar4;
  undefined4 uVar5;
  char *pcVar6;
  uint uVar7;
  cls_0x419dd0 cVar8;
  int iVar9;
  dword *pdVar10;
  char *pcVar11;
  int in_stack_00000008;
  
  iVar9 = param_1;
  if ((DAT_0065d0d0 == 0) ||
     ((((DAT_0066829c == 0 && (DAT_00667fcc != (TPlayScreen *)0x0)) &&
       (piVar2 = (int *)(DAT_00667fcc->TScreen).mbr_0xe0, piVar2 != (int *)0x0)) &&
      ((*piVar2 == 3 || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19)))))))) {
    if (this->mbr_0xa0 == 0) {
      return;
    }
  }
  else {
    if (in_stack_00000008 == 0) {
      return;
    }
    if (this->mbr_0xa0 == 0) {
      if (param_1 != 0xd) {
        return;
      }
      DAT_0065a9c8 = DAT_0065a9c8 | DAT_0065a9c4;
      DAT_0065a9c4 = 0;
      if (DAT_00667fcc != (TPlayScreen *)0x0) {
        TPlayScreen::meth_0x4cee70_Stop(DAT_00667fcc);
        TPlayScreen::meth_0x4cf000_SetWalkMode(DAT_00667fcc);
      }
      pcVar6 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      uVar7 = 0xffffffff;
      do {
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      cVar8.mbr_0x0 = ~uVar7 - 1;
      this->mbr_0xa4 = 0x50 - cVar8.mbr_0x0;
      *(undefined *)&this->mbr_0xd0 = 0;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffe8);
      meth_0x54d0c0(this,0x20,cVar8.mbr_0x0);
      this->mbr_0xa0 = 1;
      return;
    }
    if (param_1 != 0xd) {
      if (param_1 == 8) {
        uVar7 = 0xffffffff;
        pdVar10 = &this->mbr_0xd0;
        do {
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          cVar1 = *(char *)pdVar10;
          pdVar10 = (dword *)((int)pdVar10 + 1);
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        if (((int)(uVar7 - 1) < 2) || ((*(byte *)((int)&this->mbr_0xcc + uVar7 + 1) & 0x80) == 0)) {
          if (0 < (int)(uVar7 - 1)) {
            *(undefined *)((int)&this->mbr_0xcc + uVar7 + 2) = 0;
          }
        }
        else {
          *(undefined *)((int)&this->mbr_0xcc + uVar7 + 2) = 0;
          *(undefined *)((int)&this->mbr_0xcc + uVar7 + 1) = 0;
        }
      }
      else {
        if (param_1 < 0x100) {
          if (param_1 < 0x20) goto LAB_0054d5f1;
          uVar7 = 0xffffffff;
          param_1._0_2_ = (ushort)(byte)param_1;
          pdVar10 = &this->mbr_0xd0;
          do {
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            cVar1 = *(char *)pdVar10;
            pdVar10 = (dword *)((int)pdVar10 + 1);
          } while (cVar1 != '\0');
          iVar9 = this->mbr_0xa4 - (~uVar7 - 1);
          pcVar6 = (char *)((~uVar7 - 1) + (int)&this->mbr_0xd0);
        }
        else {
          param_1 = CONCAT31(CONCAT21(param_1._2_2_,(byte)param_1),(char)((uint)param_1 >> 8));
          uVar5 = param_1;
          uVar7 = 0xffffffff;
          param_1._0_2_ = (ushort)uVar5;
          param_1._0_3_ = (uint3)(ushort)param_1;
          pdVar10 = &this->mbr_0xd0;
          do {
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            cVar1 = *(char *)pdVar10;
            pdVar10 = (dword *)((int)pdVar10 + 1);
          } while (cVar1 != '\0');
          iVar9 = this->mbr_0xa4 - (~uVar7 - 1);
          pcVar6 = (char *)((~uVar7 - 1) + (int)&this->mbr_0xd0);
        }
        _strncpy(pcVar6,(char *)&param_1,iVar9 - 1);
        pcVar6[iVar9 + -1] = '\0';
      }
LAB_0054d5f1:
      pcVar6 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      dVar3 = (this->TObjectInstance).mbr_0x6c;
      _strncpy((char *)(dVar3 + 0xc),pcVar6,0x4f);
      dVar4 = (this->TObjectInstance).mbr_0x6c;
      *(undefined *)(dVar3 + 0x5b) = 0;
      pcVar6 = (char *)(dVar4 + 0xc);
      uVar7 = 0xffffffff;
      pcVar11 = pcVar6;
      do {
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        cVar1 = *pcVar11;
        pcVar11 = pcVar11 + 1;
      } while (cVar1 != '\0');
      iVar9 = -(~uVar7 - 1);
      pcVar6 = pcVar6 + (~uVar7 - 1);
      _strncpy(pcVar6,(char *)&this->mbr_0xd0,iVar9 + 0x4f);
      pcVar6[iVar9 + 0x4f] = '\0';
      meth_0x54cd40(this);
      return;
    }
  }
  meth_0x54d390(this);
  return;
}



// Function at 0054d700

/* WARNING: Type propagation algorithm not settling */

void TCharacter::meth_0x54d700()

{
  byte bVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int *piVar3;
  undefined4 *puVar4;
  undefined3 extraout_var_04;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  undefined4 unaff_EBP;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  byte *pbVar9;
  int iVar10;
  byte *in_stack_00000004;
  cls_0x5a36f8__vftable_5a36f8 *local_60;
  char *local_5c;
  byte *local_58;
  byte *local_54;
  byte *local_50;
  undefined4 local_4c;
  LPCVOID local_48;
  FILE **local_44;
  cls_0x5a36f8__vftable_5a36f8 **local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 *local_24;
  undefined local_20;
  undefined4 local_1c;
  cls_0x478720 local_14;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  cls_0x5b4f30_TPlayer *this_00;
  undefined3 extraout_var_03;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2244;
  local_c = ExceptionList;
  if (DAT_0066829c == 0) {
    if ((*in_stack_00000004 == 0x40) && (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0)) {
      local_58 = in_stack_00000004 + 1;
      uVar8 = 0xffffffff;
      pbVar9 = local_58;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        bVar1 = *pbVar9;
        pbVar9 = pbVar9 + 1;
      } while (bVar1 != 0);
      local_60 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
      local_54 = local_58 + (~uVar8 - 1);
      local_5c = s_String_005e5884;
      ExceptionList = &local_c;
      local_50 = local_58;
      cls_0x478720::cls_0x478720(&local_14);
      local_40 = &local_60;
      local_4 = 0;
      local_4c = 0;
      local_48 = (LPCVOID)0x0;
      local_44 = (FILE **)0x0;
      local_3c = 0;
      local_38 = 0;
      local_34 = 0;
      local_2c = 0;
      local_28 = 0;
      local_20 = 0;
      local_1c = 1;
      local_24 = FUN_00482fb0(0x2000);
      *(undefined *)local_24 = 0;
      local_4 = 1;
      FUN_00478a10((byte *)&local_4c);
      FUN_0041e8e0((int *)DAT_00667fcc,(byte *)&local_4c,1,0);
      local_4 = 2;
      FUN_004830f0(local_24);
      if (local_48 == (LPCVOID)0x0) {
        if (local_44 != (FILE **)0x0) {
          FUN_004830f0(local_40);
          FUN_004a1540(local_44);
        }
      }
      else {
        FUN_004830f0(local_40);
        FUN_004830f0(local_48);
      }
      local_4 = 0xffffffff;
      cls_0x478720::meth_0x478730(&local_14);
    }
    else {
      ExceptionList = &local_c;
      uVar8 = FUN_0059a530_stricmp((uint)this,in_stack_00000004,(byte *)s_alreadydead_005e588c);
      if (uVar8 == 0) {
        DAT_00668104 = (uint)(DAT_00668104 == 0);
      }
      else {
        uVar8 = FUN_0059a530_stricmp(extraout_ECX,in_stack_00000004,(byte *)s_alchemy_005e5898);
        if (uVar8 == 0) {
          if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
            cls_0x5b4f30_TPlayer::meth_0x51e900(DAT_00667fcc);
          }
        }
        else {
          uVar8 = FUN_0059a530_stricmp(extraout_ECX_00,in_stack_00000004,(byte *)s_nahkranoth_005e58a0);
          if (uVar8 == 0) {
            DAT_00668108 = (uint)(DAT_00668108 == 0);
          }
          else {
            uVar8 = FUN_0059a530_stricmp(extraout_ECX_01,in_stack_00000004,(byte *)s_noamnesia_005e58ac);
            if (uVar8 == 0) {
              if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
                (*(DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472310_360)
                          (0x1e,unaff_EDI,unaff_ESI);
                (*(DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0[1].FUN_00472980_356)
                          (unaff_EBP);
                (*(DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477650_384)
                          (DAT_00667fcc);
              }
            }
            else {
              uVar8 = FUN_0059a530_stricmp(extraout_ECX_02,in_stack_00000004,
                                   (byte *)s_lookunderthehood_005e58b8);
              if (uVar8 == 0) {
                DAT_0066812c = (uint)(DAT_0066812c == 0);
              }
              else {
                uVar8 = FUN_0059a530_stricmp(extraout_ECX_03,in_stack_00000004,(byte *)s_dummies_005e58cc);
                if (uVar8 == 0) {
                  DAT_00668110 = (uint)(DAT_00668110 == 0);
                }
                else {
                  uVar8 = FUN_0059a530_stricmp(extraout_ECX_04,in_stack_00000004,
                                       (byte *)s_abracadabra_005e58d4);
                  if (uVar8 == 0) {
                    DAT_0066810c = (uint)(DAT_0066810c == 0);
                    if (DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0) {
                      uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                virt_meth_0x470280_168)(DAT_00667fcc);
                      this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var,uVar2);
                      if (this_00 == (cls_0x5b4f30_TPlayer *)0x0) {
                        (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                          virt_meth_0x46f940_84)(DAT_00667fcc);
                        uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                  virt_meth_0x470280_168)(DAT_00667fcc);
                        this_00 = (cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var_00,uVar2);
                        if (this_00 == (cls_0x5b4f30_TPlayer *)0x0) {
                          this_00 = DAT_00667fcc;
                        }
                      }
                      iVar5 = 0;
                      if (0 < (int)DAT_0066deec) {
                        do {
                          uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                    virt_meth_0x470280_168)(DAT_00667fcc);
                          iVar7 = CONCAT31(extraout_var_01,uVar2);
                          if (iVar7 == 0) {
                            (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->
                              virt_meth_0x46f940_84)(this_00);
                            uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                      virt_meth_0x470280_168)(DAT_00667fcc);
                            iVar7 = CONCAT31(extraout_var_02,uVar2);
                            if (iVar7 != 0) goto LAB_0054da79;
                          }
                          else {
LAB_0054da79:
                            if (*(cls_0x5b4f30_TPlayer **)(iVar7 + 100) != this_00) {
                              (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046f3d0_88)
                                        (iVar7,0xffffffff);
                            }
                          }
                          iVar5 = iVar5 + 1;
                        } while (iVar5 < (int)DAT_0066deec);
                      }
                      iVar5 = DAT_00667c3c;
                      iVar7 = 0;
                      if (0 < DAT_00667c3c) {
                        do {
                          iVar10 = 0;
                          piVar3 = *(int **)(DAT_00667c4c + iVar7 * 4);
                          if (0 < *piVar3) {
                            do {
                              cls_0x544fb0::meth_0x544fb0((cls_0x544fb0 *)&DAT_0065a9d8);
                              iVar10 = iVar10 + 1;
                            } while (iVar10 < *piVar3);
                          }
                          iVar7 = iVar7 + 1;
                        } while (iVar7 < iVar5);
                      }
                    }
                    DAT_0065b02c._76_4_ = 1;
                    UNK_0065d53c._12_4_ = 1;
                    UNK_0065a9dc._76_4_ = 1;
                  }
                  else {
                    uVar8 = FUN_0059a530_stricmp(extraout_ECX_05,in_stack_00000004,
                                         (byte *)s_potionsnlotions_005e5904);
                    if (uVar8 == 0) {
                      uVar8 = 0;
                      if (0 < (int)DAT_0066d28c) {
                        do {
                          if (((DAT_0066d29c == 0) || (DAT_0066d28c <= uVar8)) ||
                             (*(int *)(DAT_0066d29c + uVar8 * 4) == 0)) {
                            puVar4 = (undefined4 *)0x0;
                          }
                          else {
                            puVar4 = *(undefined4 **)(DAT_0066d29c + uVar8 * 4);
                            if (puVar4 == (undefined4 *)0x0) {
                              puVar4 = DAT_0066d2a0;
                            }
                          }
                          uVar6 = *puVar4;
                          uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                    virt_meth_0x470280_168)(DAT_00667fcc);
                          piVar3 = (int *)CONCAT31(extraout_var_03,uVar2);
                          if (piVar3 == (int *)0x0) {
                            (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                              virt_meth_0x46f940_84)(DAT_00667fcc);
                          }
                          else {
                            iVar5 = (**(code **)(*piVar3 + 0x198))(uVar6);
                            if (iVar5 < 5) {
                              uVar6 = 5;
                            }
                            else {
                              uVar6 = (**(code **)(*piVar3 + 0x198))();
                            }
                            (**(code **)(*piVar3 + 0x19c))(uVar6);
                          }
                          uVar8 = uVar8 + 1;
                        } while ((int)uVar8 < (int)DAT_0066d28c);
                      }
                      DAT_0065b02c._76_4_ = 1;
                      UNK_0065d53c._12_4_ = 1;
                    }
                    else {
                      uVar8 = FUN_0059a530_stricmp(extraout_ECX_06,in_stack_00000004,
                                           (byte *)s_gimmesomegrub_005e5914);
                      if (uVar8 == 0) {
                        uVar8 = 0;
                        if (0 < (int)DAT_0066d2cc) {
                          do {
                            if (((DAT_0066d2dc == 0) || (DAT_0066d2cc <= uVar8)) ||
                               (*(int *)(DAT_0066d2dc + uVar8 * 4) == 0)) {
                              puVar4 = (undefined4 *)0x0;
                            }
                            else {
                              puVar4 = *(undefined4 **)(DAT_0066d2dc + uVar8 * 4);
                              if (puVar4 == (undefined4 *)0x0) {
                                puVar4 = DAT_0066d2e0;
                              }
                            }
                            uVar6 = *puVar4;
                            uVar2 = (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                      virt_meth_0x470280_168)(DAT_00667fcc);
                            piVar3 = (int *)CONCAT31(extraout_var_04,uVar2);
                            if (piVar3 == (int *)0x0) {
                              (*((DAT_00667fcc->TPlayScreen).TScreen.vftptr_0x0)->
                                virt_meth_0x46f940_84)(DAT_00667fcc);
                            }
                            else {
                              iVar5 = (**(code **)(*piVar3 + 0x198))(uVar6);
                              if (iVar5 < 5) {
                                uVar6 = 5;
                              }
                              else {
                                uVar6 = (**(code **)(*piVar3 + 0x198))();
                              }
                              (**(code **)(*piVar3 + 0x19c))(uVar6);
                            }
                            uVar8 = uVar8 + 1;
                          } while ((int)uVar8 < (int)DAT_0066d2cc);
                        }
                        DAT_0065b02c._76_4_ = 1;
                        UNK_0065d53c._12_4_ = 1;
                      }
                      else {
                        uVar8 = FUN_0059a530_stricmp(extraout_ECX_07,in_stack_00000004,
                                             (byte *)s_debug_005e5924);
                        if (uVar8 != 0) {
                          ExceptionList = local_c;
                          return;
                        }
                        DAT_0066812c = (uint)(DAT_00668130 == 0);
                        DAT_00668130 = DAT_0066812c;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      iVar5 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if (-1 < iVar5) {
        uVar6 = cls_0x45f7c0::meth_0x49d780((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        FUN_0054d170(this,uVar6);
      }
      iVar5 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      if ((-1 < iVar5) &&
         (iVar7 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar7 != 0)) {
        cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar5,0x7f,1);
      }
    }
  }
  else {
    ExceptionList = &local_c;
    cls_0x5701f0::meth_0x5701f0((cls_0x5701f0 *)&UNK_00676268.field_0x400,DAT_00676874);
  }
  ExceptionList = local_c;
  return;
}



