#include "C3DCamera.h"

// Decompiled methods and structure for class: C3DCamera



// Function at 00574e70

C3DCamera::C3DCamera *this,dword *param_1,undefined4 *param_2,undefined4 *param_3,
          undefined4 *param_4)

{
  dword *this_00;
  dword dVar1;
  char cVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  uint in_stack_00000014;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a24bc;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5ba014__vftable_5ba014 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  this_00 = &this->m_originx;
  local_4 = 0;
  *(undefined *)this_00 = param_1._0_1_;
  cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)this_00);
  local_4._0_1_ = 1;
  *(undefined *)&this->pm_videoorigin = param_1._0_1_;
  cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)&this->pm_videoorigin);
  this->m_endy = *param_1;
  local_4 = CONCAT31(local_4._1_3_,2);
  dVar1 = param_1[1];
  this->vftptr_0x0 = (cls_0x5ba014__vftable_5ba014 *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
  this->m_screenx = dVar1;
  this->m_centerx = param_1[2];
  this->m_centery = param_1[3];
  uVar4 = 0xffffffff;
  puVar3 = param_2;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *(char *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  } while (cVar2 != '\0');
  uVar4 = ~uVar4 - 1;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)this_00,(char)uVar4);
  if (cVar2 != '\0') {
    puVar3 = (undefined4 *)this->m_originy;
    puVar7 = puVar3;
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar7 = *param_2;
      param_2 = param_2 + 1;
      puVar7 = puVar7 + 1;
    }
    for (uVar5 = uVar4 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined *)puVar7 = *(undefined *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    this->m_clippx = uVar4;
    *(undefined *)((int)puVar3 + uVar4) = 0;
  }
  uVar5 = 0xffffffff;
  puVar3 = param_3;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar2 = *(char *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  } while (cVar2 != '\0');
  uVar5 = ~uVar5 - 1;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->pm_videoorigin,(char)uVar5);
  if (cVar2 != '\0') {
    puVar3 = (undefined4 *)this->pm_zbufferorigin;
    puVar7 = puVar3;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *puVar7 = *param_3;
      param_3 = param_3 + 1;
      puVar7 = puVar7 + 1;
    }
    this->m_startx = uVar5;
    for (uVar6 = uVar5 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined *)puVar7 = *(undefined *)param_3;
      param_3 = (undefined4 *)((int)param_3 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    *(undefined *)((int)puVar3 + uVar5) = 0;
  }
  this->m_starty = 2;
  this->pm_video = 0;
  *(char *)&this->pm_addlighttable = (char)uVar4;
  this->pm_divtable = 0;
  this->pm_alphatable = 0;
  this->m_alphacount = 0;
  local_4 = 4;
  this->vftptr_0x0 = &cls_0x5ba014__vftable_5ba014_005ba014;
  if ((param_4 == (undefined4 *)0x0) || ((int)in_stack_00000014 < 1)) {
    this->pm_zbuffer = 0;
    this->pm_savecarrytable = 0;
  }
  else {
    puVar3 = FUN_00482fb0(in_stack_00000014);
    this->pm_zbuffer = (dword)puVar3;
    for (uVar4 = in_stack_00000014 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar3 = *param_4;
      param_4 = param_4 + 1;
      puVar3 = puVar3 + 1;
    }
    for (uVar4 = in_stack_00000014 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar3 = *(undefined *)param_4;
      param_4 = (undefined4 *)((int)param_4 + 1);
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
    this->pm_savecarrytable = 0;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 00575000

C3DCamera * __thiscall C3DCamera::~C3DCamera(C3DCamera *this)

{
  char cVar1;
  dword dVar2;
  C3DCamera *pcVar3;
  
  this->vftptr_0x0 = &cls_0x5ba014__vftable_5ba014_005ba014;
  if ((LPCVOID)this->pm_zbuffer != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->pm_zbuffer);
  }
  dVar2 = this->pm_divtable;
  if (dVar2 != 0) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->pm_divtable = 0;
  this->pm_alphatable = 0;
  this->m_alphacount = 0;
  this->vftptr_0x0 = (cls_0x5ba014__vftable_5ba014 *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
  if ((HGLOBAL)this->pm_video != (HGLOBAL)0x0) {
    GlobalFree((HGLOBAL)this->pm_video);
    this->pm_video = 0;
  }
  dVar2 = this->pm_zbufferorigin;
  if (dVar2 != 0) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->pm_zbufferorigin = 0;
  this->m_startx = 0;
  pcVar3 = (C3DCamera *)this->m_originy;
  this->m_endx = 0;
  if (pcVar3 != (C3DCamera *)0x0) {
    cVar1 = *(char *)((int)&pcVar3[-1].mbr_0x50 + 3);
    pcVar3 = (C3DCamera *)((int)&pcVar3[-1].mbr_0x50 + 3);
    if ((cVar1 != '\0') && (cVar1 != -1)) {
      this->m_originy = 0;
      this->m_clippx = 0;
      this->m_clippy = 0;
      this->vftptr_0x0 = (cls_0x5ba014__vftable_5ba014 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
      *(char *)&pcVar3->vftptr_0x0 = cVar1 + -1;
      return pcVar3;
    }
    pcVar3 = (C3DCamera *)FUN_004830f0(pcVar3);
  }
  this->m_originy = 0;
  this->m_clippx = 0;
  this->m_clippy = 0;
  this->vftptr_0x0 = (cls_0x5ba014__vftable_5ba014 *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  return pcVar3;
}



// Function at 005750d0

dword C3DCamera::virt_meth_0x5750d0()

{
  int iVar1;
  char *pcVar2;
  bool bVar3;
  char *in_stack_00000004;
  
  iVar1 = 6;
  bVar3 = true;
  pcVar2 = s_modem_005e5b94;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    bVar3 = *in_stack_00000004 == *pcVar2;
    in_stack_00000004 = in_stack_00000004 + 1;
    pcVar2 = pcVar2 + 1;
  } while (bVar3);
  if (bVar3) {
    return this->pm_zbuffer;
  }
  return 0;
}



// Function at 00575100

undefined4 C3DCamera::virt_meth_0x575100(byte *param_1)

{
  byte bVar1;
  char cVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  byte *pbVar9;
  undefined4 *puVar10;
  bool bVar11;
  bool bVar12;
  undefined4 *in_stack_00000008;
  
  iVar4 = 6;
  bVar11 = true;
  pbVar9 = param_1;
  pbVar3 = (byte *)s_modem_005e5b9c;
  do {
    if (iVar4 == 0) break;
    iVar4 = iVar4 + -1;
    bVar11 = *pbVar9 == *pbVar3;
    pbVar9 = pbVar9 + 1;
    pbVar3 = pbVar3 + 1;
  } while (bVar11);
  if (bVar11) {
    pbVar9 = (byte *)this->pm_zbuffer;
    param_1 = (byte *)0x0;
    if (in_stack_00000008 != (undefined4 *)0x0) {
      bVar1 = *pbVar9;
      while (pbVar3 = pbVar9, puVar8 = in_stack_00000008, bVar1 != 0) {
        do {
          bVar1 = *pbVar3;
          bVar11 = bVar1 < *(byte *)puVar8;
          if (bVar1 != *(byte *)puVar8) {
LAB_00575164:
            iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
            goto LAB_00575169;
          }
          if (bVar1 == 0) break;
          bVar1 = pbVar3[1];
          bVar11 = bVar1 < *(byte *)((int)puVar8 + 1);
          if (bVar1 != *(byte *)((int)puVar8 + 1)) goto LAB_00575164;
          pbVar3 = pbVar3 + 2;
          puVar8 = (undefined4 *)((int)puVar8 + 2);
        } while (bVar1 != 0);
        iVar4 = 0;
LAB_00575169:
        if (iVar4 == 0) {
          if ((int)param_1 < 0) {
            return 0;
          }
          if (param_1 != (byte *)this->pm_savecarrytable) {
            if ((HGLOBAL)this->pm_video != (HGLOBAL)0x0) {
              GlobalFree((HGLOBAL)this->pm_video);
              this->pm_video = 0;
            }
            this->pm_savecarrytable = (dword)param_1;
          }
          return 1;
        }
        uVar5 = 0xffffffff;
        pbVar3 = pbVar9;
        do {
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          bVar1 = *pbVar3;
          pbVar3 = pbVar3 + 1;
        } while (bVar1 != 0);
        pbVar3 = pbVar9 + ~uVar5;
        pbVar9 = pbVar9 + ~uVar5;
        param_1 = (byte *)((int)param_1 + 1);
        bVar1 = *pbVar3;
      }
    }
  }
  else {
    iVar4 = 6;
    bVar11 = true;
    pbVar9 = param_1;
    pbVar3 = (byte *)s_phone_005e5ba4;
    do {
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      bVar11 = *pbVar9 == *pbVar3;
      pbVar9 = pbVar9 + 1;
      pbVar3 = pbVar3 + 1;
    } while (bVar11);
    if (bVar11) {
      uVar5 = 0xffffffff;
      pbVar9 = param_1;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        bVar1 = *pbVar9;
        pbVar9 = pbVar9 + 1;
      } while (bVar1 != 0);
      uVar7 = this->pm_alphatable;
      uVar6 = ~uVar5 - 1;
      uVar5 = uVar7;
      if (uVar6 <= uVar7) {
        uVar5 = uVar6;
      }
      bVar11 = false;
      iVar4 = 0;
      bVar12 = true;
      pbVar9 = (byte *)this->pm_divtable;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        bVar11 = *pbVar9 < *param_1;
        bVar12 = *pbVar9 == *param_1;
        pbVar9 = pbVar9 + 1;
        param_1 = param_1 + 1;
      } while (bVar12);
      if (!bVar12) {
        iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
      }
      if (((iVar4 != 0) || (uVar7 < uVar6)) || (uVar7 != uVar6)) {
        uVar5 = 0xffffffff;
        puVar8 = in_stack_00000008;
        do {
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          cVar2 = *(char *)puVar8;
          puVar8 = (undefined4 *)((int)puVar8 + 1);
        } while (cVar2 != '\0');
        uVar5 = ~uVar5 - 1;
        cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->pm_addlighttable,(char)uVar5);
        if (cVar2 != '\0') {
          puVar8 = (undefined4 *)this->pm_divtable;
          puVar10 = puVar8;
          for (uVar7 = uVar5 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
            *puVar10 = *in_stack_00000008;
            in_stack_00000008 = in_stack_00000008 + 1;
            puVar10 = puVar10 + 1;
          }
          this->pm_alphatable = uVar5;
          for (uVar7 = uVar5 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *(undefined *)puVar10 = *(undefined *)in_stack_00000008;
            in_stack_00000008 = (undefined4 *)((int)in_stack_00000008 + 1);
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
          *(undefined *)((int)puVar8 + uVar5) = 0;
        }
        if ((HGLOBAL)this->pm_video != (HGLOBAL)0x0) {
          GlobalFree((HGLOBAL)this->pm_video);
          this->pm_video = 0;
        }
      }
      return 1;
    }
  }
  return 0;
}



// Function at 0057bad0

C3DCamera * C3DCamera::virt_meth_0x57bad0()

{
  byte in_stack_00000004;
  
  ~C3DCamera(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



