#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x41c7f0



// Function at 00411300

void cls_0x41c7f0::meth_0x411300()

{
  int *piVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (piVar1 = *(int **)(this->mbr_0x10 + in_stack_00000004 * 4), piVar1 != (int *)0x0)) {
    (**(code **)(*piVar1 + 8))(1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 00411330

void cls_0x41c7f0::meth_0x411330()

{
  LPCVOID pvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (pvVar1 = *(LPCVOID *)(this->mbr_0x10 + in_stack_00000004 * 4), pvVar1 != (LPCVOID)0x0)) {
    if (*(LPCVOID *)((int)pvVar1 + 8) != (LPCVOID)0x0) {
      FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 8));
    }
    if (*(LPCVOID *)((int)pvVar1 + 0xc) != (LPCVOID)0x0) {
      FUN_00482f80(*(LPCVOID *)((int)pvVar1 + 0xc));
    }
    FUN_004830f0(pvVar1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 0041c7f0

cls_0x41c7f0::cls_0x41c7f0 *this,dword param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  dword in_stack_00000008;
  
  this->mbr_0x8 = param_1;
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0xc = in_stack_00000008;
  this->mbr_0x10 = 0;
  puVar1 = FUN_00482fb0(param_1 << 2);
  this->mbr_0x10 = (dword)puVar1;
  for (uVar2 = this->mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  return this;
}



// Function at 00438f20

void cls_0x41c7f0::meth_0x438f20()

{
  undefined4 *puVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (puVar1 = *(undefined4 **)(this->mbr_0x10 + in_stack_00000004 * 4), puVar1 != (undefined4 *)0x0
     )) {
    (**(code **)*puVar1)(1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 00461700

void cls_0x41c7f0::meth_0x461700()

{
  LPCVOID pvVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  return;
}



// Function at 004a1e90

void cls_0x41c7f0::meth_0x4a1e90()

{
  LPCVOID pvVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (pvVar1 = *(LPCVOID *)(this->mbr_0x10 + in_stack_00000004 * 4), pvVar1 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 004acfe0

void cls_0x41c7f0::meth_0x4acfe0()

{
  LPCVOID pvVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x10 + iVar2 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  return;
}



// Function at 0053fe80

/* WARNING: Type propagation algorithm not settling */

undefined4 cls_0x41c7f0::meth_0x53fe80(byte *param_1)

{
  byte *pbVar1;
  SoftwareRenderer *this_00;
  cls_0x53f680 *this_01;
  undefined uVar2;
  uint uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined4 uVar4;
  undefined3 extraout_var_02;
  uint uVar5;
  cls_0x41c7f0 *extraout_ECX;
  cls_0x41c7f0 *extraout_ECX_00;
  cls_0x41c7f0 *pcVar6;
  cls_0x41c7f0 *extraout_ECX_01;
  cls_0x41c7f0 *extraout_ECX_02;
  int unaff_EBX;
  int iVar7;
  int unaff_EBP;
  dword dVar8;
  cls_0x5b4f30_TPlayer *this_02;
  int iVar9;
  undefined4 *puVar10;
  int iVar11;
  int *piVar12;
  cls_0x5b4f30_TPlayer *unaff_retaddr;
  cls_0x5b4f30_TPlayer *in_stack_00000008;
  undefined4 in_stack_0000000c;
  cls_0x45f7c0 *in_stack_00000010;
  cls_0x44ceb0 local_50;
  
  iVar9 = 0;
  iVar7 = DAT_00667c4c;
  local_50.mbr_0x4 = (dword)this;
  if (0 < DAT_00667c3c) {
    do {
      iVar11 = 0;
      if (0 < **(int **)(iVar7 + iVar9 * 4)) {
        do {
          iVar7 = *(int *)(iVar7 + iVar9 * 4);
          uVar3 = *(uint *)(*(int *)(iVar7 + 0x10) + iVar11 * 4);
          uVar5 = uVar3;
          if (uVar3 == 0) {
            uVar5 = *(uint *)(iVar7 + 0x14);
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar5 + 0x24),param_1);
          iVar7 = DAT_00667c4c;
          if (uVar3 == 0) {
            dVar8 = *(dword *)(DAT_00667c4c + iVar9 * 4);
            goto LAB_0053fee8;
          }
          iVar11 = iVar11 + 1;
        } while (iVar11 < **(int **)(DAT_00667c4c + iVar9 * 4));
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < DAT_00667c3c);
  }
  dVar8 = 0;
LAB_0053fee8:
  iVar9 = 0;
  local_50.mbr_0x0 = dVar8;
  if (0 < DAT_00667c3c) {
    do {
      iVar11 = 0;
      if (0 < **(int **)(iVar7 + iVar9 * 4)) {
        do {
          iVar7 = *(int *)(iVar7 + iVar9 * 4);
          uVar3 = *(uint *)(*(int *)(iVar7 + 0x10) + iVar11 * 4);
          uVar5 = uVar3;
          if (uVar3 == 0) {
            uVar5 = *(uint *)(iVar7 + 0x14);
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar5 + 0x24),param_1);
          if (uVar3 == 0) {
            iVar7 = cls_0x540ad0::meth_0x540ad0(*(cls_0x540ad0 **)(DAT_00667c4c + iVar9 * 4));
            goto LAB_0053ff4f;
          }
          iVar11 = iVar11 + 1;
          iVar7 = DAT_00667c4c;
        } while (iVar11 < **(int **)(DAT_00667c4c + iVar9 * 4));
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < DAT_00667c3c);
  }
  iVar7 = 0;
LAB_0053ff4f:
  if (dVar8 == 0) {
    return 0;
  }
  if (iVar7 == 0) {
    return 0;
  }
  if ((*(dword *)(local_50.mbr_0x4 + 0x14) != 0) && (DAT_00668154 == 0)) {
    return 0;
  }
  iVar9 = *(int *)(iVar7 + 0x4c);
  pcVar6 = (cls_0x41c7f0 *)local_50.mbr_0x4;
  if (in_stack_00000008 != (cls_0x5b4f30_TPlayer *)0x0) {
    if (*(short *)&(in_stack_00000008->TPlayScreen).TScreen.mbr_0x4 == 0xb) {
      uVar2 = (*(in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0[1].virt_meth_0x477ea0_468)
                        (in_stack_00000008);
      iVar11 = (int)((ulonglong)((longlong)(CONCAT31(extraout_var,uVar2) * iVar9) * -0x51eb851f) >>
                    0x20);
      iVar9 = iVar9 + ((iVar11 >> 5) - (iVar11 >> 0x1f));
    }
    uVar2 = (*((in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477e90_464)
                      (in_stack_00000008);
    if ((CONCAT31(extraout_var_00,uVar2) < iVar9) && (DAT_0066810c == 0)) {
      if (in_stack_00000008 != DAT_00667fcc) {
        return 0;
      }
joined_r0x0053ffe3:
      if ((DAT_0066829c != 0) && ((DAT_00676828 != 0 && (DAT_0067682c == 0)))) {
        FUN_00587280((int *)in_stack_00000008);
        return 0;
      }
      uVar4 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar4);
      return 0;
    }
    pcVar6 = *(cls_0x41c7f0 **)(iVar7 + 0x5c);
    if ((-1 < (int)pcVar6) && (DAT_0066810c == 0)) {
      iVar9 = FUN_00483300_RandomRange(1,100);
      pcVar6 = extraout_ECX;
      if (*(short *)&(in_stack_00000008->TPlayScreen).TScreen.mbr_0x4 != 0xb)
      goto LAB_005401a0;
      uVar2 = (*((in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x2a)
      ;
      iVar11 = CONCAT31(extraout_var_01,uVar2) - *(int *)(iVar7 + 0x5c);
      if (iVar11 < 0) {
        if (iVar11 == -3) {
          iVar11 = 0x14;
        }
        else if (iVar11 == -2) {
          iVar11 = 0x28;
        }
        else {
          iVar11 = (-(uint)(iVar11 != -1) & 0xffffffb0) + 0x50;
        }
      }
      else {
        iVar11 = 100;
      }
      pcVar6 = extraout_ECX_00;
      if ((iVar11 < iVar9) && (in_stack_00000008 == DAT_00667fcc)) goto joined_r0x0053ffe3;
    }
    if (*(short *)&(in_stack_00000008->TPlayScreen).TScreen.mbr_0x4 == 0xb) {
      (*((in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x35);
      uVar2 = (*((in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0)->FUN_00477aa0_220)(0x2a)
      ;
      pcVar6 = (cls_0x41c7f0 *)(*(int *)(iVar7 + 0x5c) - CONCAT31(extraout_var_02,uVar2));
      if (0 < *(int *)(iVar7 + 0x5c)) {
        (*(in_stack_00000008->TPlayScreen).TScreen.vftptr_0x0[2].virt_meth_0x477920_56)
                  (in_stack_00000008);
        pcVar6 = extraout_ECX_01;
      }
    }
  }
LAB_005401a0:
  pbVar1 = *(byte **)(unaff_EBP + 0x70);
  if (pbVar1 == (byte *)0x0) {
    iVar7 = 0;
    if (0 < DAT_0067021c) {
      piVar12 = &DAT_00670220;
      do {
        uVar3 = FUN_0059a530_stricmp((uint)pcVar6,(byte *)s_spell_005e4f50,*(byte **)(*piVar12 + 4));
        if (uVar3 == 0) goto LAB_0054033b;
        iVar7 = iVar7 + 1;
        piVar12 = piVar12 + 1;
        pcVar6 = extraout_ECX_02;
      } while (iVar7 < DAT_0067021c);
    }
  }
  else {
    iVar7 = 0;
    if (0 < DAT_0067021c) {
      piVar12 = &DAT_00670220;
      do {
        uVar3 = FUN_0059a530_stricmp(*(uint *)(*piVar12 + 4),pbVar1,(byte *)*(uint *)(*piVar12 + 4));
        if (uVar3 == 0) goto LAB_0054033b;
        iVar7 = iVar7 + 1;
        piVar12 = piVar12 + 1;
      } while (iVar7 < DAT_0067021c);
    }
  }
  puVar10 = (undefined4 *)0x0;
LAB_005401e5:
  this_02 = in_stack_00000008;
  if ((*(int *)(unaff_EBP + 0x74) != 0) &&
     ((*(byte *)&(in_stack_00000008->TPlayScreen).TScreen.mbr_0x110 & 0x20) != 0)) {
    cls_0x44ceb0::cls_0x44ceb0(&local_50,in_stack_00000008,0,5,0);
    this_00 = (SoftwareRenderer *)local_50.mbr_0xc;
    while (this_00 != (SoftwareRenderer *)0x0) {
      local_50.mbr_0xc = (dword)this_00;
      if ((((this_00->mbr_0x4 == 0x19) && (this_00 != (SoftwareRenderer *)0x0)) &&
          (dVar8 = this_00->mbr_0xd8, dVar8 != 0)) &&
         ((iVar7 = *(int *)(dVar8 + 0x120), iVar7 != 0 && (*(int *)(iVar7 + 0x74) != 0)))) {
        if ((*(int *)(iVar7 + 0x70) != 0) &&
           ((*(int *)(iVar7 + 0x70) != -0x74 &&
            (iVar7 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548), iVar7 != 0)))) {
          cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
        }
        if (*(cls_0x5b4f30_TPlayer **)(dVar8 + 4) == unaff_retaddr) {
          SoftwareRenderer::meth_0x4defe0(this_00);
        }
      }
      cls_0x44ceb0::meth_0x44d080(&local_50);
      this_00 = (SoftwareRenderer *)local_50.mbr_0xc;
      this_02 = unaff_retaddr;
    }
  }
  local_50.mbr_0x34 =
       (**(code **)*puVar10)
                 (this_02,param_1,in_stack_00000008,in_stack_0000000c,unaff_EBX,unaff_EBP);
  iVar9 = 0;
  iVar7 = DAT_00667c4c;
  if (0 < DAT_00667c3c) {
    do {
      iVar11 = 0;
      if (0 < **(int **)(iVar7 + iVar9 * 4)) {
        do {
          iVar7 = *(int *)(iVar7 + iVar9 * 4);
          uVar3 = *(uint *)(*(int *)(iVar7 + 0x10) + iVar11 * 4);
          uVar5 = uVar3;
          if (uVar3 == 0) {
            uVar5 = *(uint *)(iVar7 + 0x14);
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar5 + 0x24),(byte *)local_50.mbr_0x30);
          iVar7 = DAT_00667c4c;
          if (uVar3 == 0) {
            uVar4 = *(undefined4 *)(DAT_00667c4c + iVar9 * 4);
            goto LAB_00540379;
          }
          iVar11 = iVar11 + 1;
        } while (iVar11 < **(int **)(DAT_00667c4c + iVar9 * 4));
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < DAT_00667c3c);
  }
  uVar4 = 0;
LAB_00540379:
  *(undefined4 *)(local_50.mbr_0x34 + 0x11c) = uVar4;
  iVar9 = 0;
  if (0 < DAT_00667c3c) {
    do {
      iVar11 = 0;
      if (0 < **(int **)(iVar7 + iVar9 * 4)) {
        do {
          iVar7 = *(int *)(iVar7 + iVar9 * 4);
          uVar3 = *(uint *)(*(int *)(iVar7 + 0x10) + iVar11 * 4);
          uVar5 = uVar3;
          if (uVar3 == 0) {
            uVar5 = *(uint *)(iVar7 + 0x14);
          }
          uVar3 = FUN_0059a530_stricmp(uVar3,(byte *)(uVar5 + 0x24),(byte *)local_50.mbr_0x30);
          if (uVar3 == 0) {
            iVar7 = *(int *)(DAT_00667c4c + iVar9 * 4);
            dVar8 = *(dword *)(*(int *)(iVar7 + 0x10) + iVar11 * 4);
            if (dVar8 == 0) {
              dVar8 = *(dword *)(iVar7 + 0x14);
            }
            goto LAB_005403e2;
          }
          iVar11 = iVar11 + 1;
          iVar7 = DAT_00667c4c;
        } while (iVar11 < **(int **)(DAT_00667c4c + iVar9 * 4));
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < DAT_00667c3c);
  }
  dVar8 = 0;
LAB_005403e2:
  this_01 = (cls_0x53f680 *)local_50.mbr_0x34;
  *(dword *)(local_50.mbr_0x34 + 0x120) = dVar8;
  if (this_02 != (cls_0x5b4f30_TPlayer *)0x0) {
    cls_0x5b4f30_TPlayer::meth_0x4d5900(this_02);
  }
  (in_stack_00000010->cls_0x41c7f0).mbr_0x14 = *(dword *)(unaff_EBX + 0x50);
  cls_0x53f680::meth_0x53f680(this_01);
  cls_0x45f7c0::meth_0x41c840(in_stack_00000010);
  return 1;
LAB_0054033b:
  puVar10 = (undefined4 *)(&DAT_00670220)[iVar7];
  goto LAB_005401e5;
}



// Function at 00540750

void cls_0x41c7f0::meth_0x540750()

{
  dword dVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  
  if (0 < (int)this->mbr_0x14) {
    this->mbr_0x14 = this->mbr_0x14 - 1;
  }
  dVar1 = this->mbr_0x0;
  uVar5 = 0;
  if (0 < (int)dVar1) {
    do {
      if (((this->mbr_0x10 != 0) && (uVar5 < this->mbr_0x0)) &&
         (piVar2 = *(int **)(this->mbr_0x10 + uVar5 * 4), piVar2 != (int *)0x0)) {
        (**(code **)(*piVar2 + 8))();
        iVar4 = (**(code **)(**(int **)(this->mbr_0x10 + uVar5 * 4) + 4))();
        if (iVar4 != 0) {
          (**(code **)(**(int **)(this->mbr_0x10 + uVar5 * 4) + 0xc))();
          if ((-1 < (int)uVar5) &&
             (puVar3 = *(undefined4 **)(this->mbr_0x10 + uVar5 * 4), puVar3 != (undefined4 *)0x0)) {
            (**(code **)*puVar3)(1);
          }
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)dVar1);
  }
  return;
}



// Function at 005407d0

int cls_0x41c7f0::meth_0x5407d0()

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar1 = this->mbr_0x0;
  iVar2 = 0;
  uVar3 = 0;
  if (0 < (int)uVar1) {
    piVar4 = (int *)this->mbr_0x10;
    do {
      if (((int *)this->mbr_0x10 != (int *)0x0) && (uVar3 < uVar1)) {
        if (*piVar4 != 0) {
          iVar2 = iVar2 + *(int *)(*piVar4 + 0x134);
        }
      }
      uVar3 = uVar3 + 1;
      piVar4 = piVar4 + 1;
    } while ((int)uVar3 < (int)uVar1);
    return iVar2;
  }
  return 0;
}



// Function at 00540820

int cls_0x41c7f0::meth_0x540820()

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar1 = this->mbr_0x0;
  iVar2 = 0;
  uVar3 = 0;
  if (0 < (int)uVar1) {
    piVar4 = (int *)this->mbr_0x10;
    do {
      if (((int *)this->mbr_0x10 != (int *)0x0) && (uVar3 < uVar1)) {
        if (*piVar4 != 0) {
          iVar2 = iVar2 + *(int *)(*piVar4 + 0x138);
        }
      }
      uVar3 = uVar3 + 1;
      piVar4 = piVar4 + 1;
    } while ((int)uVar3 < (int)uVar1);
    return iVar2;
  }
  return 0;
}



// Function at 005408d0

void cls_0x41c7f0::meth_0x5408d0(undefined4 param_1)

{
  dword dVar1;
  cls_0x53f730 *this_00;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  
  dVar1 = this->mbr_0x0;
  uVar4 = 0;
  if (0 < (int)dVar1) {
    do {
      if ((((this->mbr_0x10 != 0) && (uVar4 < this->mbr_0x0)) &&
          (this_00 = *(cls_0x53f730 **)(this->mbr_0x10 + uVar4 * 4), this_00 != (cls_0x53f730 *)0x0)
          ) && (this_00 != (cls_0x53f730 *)0x0)) {
        cls_0x53f730::meth_0x53f730(this_00,param_1);
        iVar3 = (**(code **)(this_00->mbr_0x0 + 4))();
        if (iVar3 != 0) {
          if ((-1 < (int)uVar4) &&
             (puVar2 = *(undefined4 **)(this->mbr_0x10 + uVar4 * 4), puVar2 != (undefined4 *)0x0)) {
            (**(code **)*puVar2)(1);
          }
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
        }
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)dVar1);
  }
  return;
}



// Function at 00540940

void cls_0x41c7f0::meth_0x540940()

{
  dword dVar1;
  int *piVar2;
  undefined4 *puVar3;
  uint uVar4;
  
  uVar4 = 0;
  dVar1 = this->mbr_0x0;
  if (0 < (int)dVar1) {
    do {
      if (((this->mbr_0x10 != 0) && (uVar4 < this->mbr_0x0)) &&
         (piVar2 = *(int **)(this->mbr_0x10 + uVar4 * 4), piVar2 != (int *)0x0)) {
        (**(code **)(*piVar2 + 0xc))();
        if ((-1 < (int)uVar4) &&
           (puVar3 = *(undefined4 **)(this->mbr_0x10 + uVar4 * 4), puVar3 != (undefined4 *)0x0)) {
          (**(code **)*puVar3)(1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)dVar1);
  }
  return;
}



