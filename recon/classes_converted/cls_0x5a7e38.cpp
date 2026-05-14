#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a7e38



// Function at 0040dd60

void cls_0x5a7e38::virt_meth_0x40dd60()

{
  int *piVar1;
  dword dVar2;
  
  piVar1 = (int *)(this->cls_0x5a47f0).mbr_0x4;
  this->mbr_0x78 = 0x47c35000;
  this->mbr_0x7c = 0x47c35000;
  this->mbr_0x80 = 0x47c35000;
  this->mbr_0x84 = 0;
  this->mbr_0x88 = 0;
  this->mbr_0x8c = 0;
  this->mbr_0x90 = 0x3f800000;
  this->mbr_0x94 = 0x3f800000;
  this->mbr_0x98 = 0x3f800000;
  this->mbr_0x70 = 1;
  this->mbr_0x74 = 0;
  this->mbr_0x3c = 0x11;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 8);
  }
  (*(this->cls_0x5a47f0).vftptr_0x0[0x17].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
  *(undefined4 *)&this->field_0x6c = 0xffffffff;
  (*(this->cls_0x5a47f0).vftptr_0x0[0x14].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
  dVar2 = cls_0x4161f0::meth_0x4161f0((cls_0x4161f0 *)((int)&DAT_0065a578 + 4));
  this->mbr_0x40 = dVar2;
  this->mbr_0xf0 = 0;
  this->mbr_0xf4 = 0;
  this->mbr_0xf8 = 0;
  return;
}



// Function at 0040de10

void cls_0x5a7e38::virt_meth_0x40de10()

{
  dword *this_00;
  uint *puVar1;
  LPCVOID pvVar2;
  cls_0x41c7f0 *this_01;
  int iVar3;
  
  this_00 = &(this->cls_0x41c7f0).mbr_0x14;
  iVar3 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      cls_0x41c7f0::meth_0x411300((cls_0x41c7f0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*this_00);
  }
  *this_00 = 0;
  *(undefined4 *)&this->field_0x5c = 0;
  cls_0x411eb0::meth_0x4162c0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  this_01 = &this->cls_0x41c7f0;
  iVar3 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      puVar1 = *(uint **)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4);
      if (puVar1[0x29] != 0) {
        FUN_0040a260(puVar1);
      }
      puVar1 = *(uint **)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4);
      if ((puVar1[0x2b] != 0) && ((*puVar1 & 0x40000) != 0)) {
        if ((LPCVOID)puVar1[0x2b] != (LPCVOID)0x0) {
          FUN_004830f0((LPCVOID)puVar1[0x2b]);
        }
        puVar1[0x2b] = 0;
        puVar1[0x2a] = 0;
        *puVar1 = *puVar1 & 0xfff8ffff;
      }
      if (((-1 < iVar3) && (iVar3 < 0x100)) &&
         (*(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4) != 0)) {
        if ((-1 < iVar3) &&
           (pvVar2 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar2 != (LPCVOID)0x0
           )) {
          if (*(LPCVOID *)((int)pvVar2 + 0xa4) != (LPCVOID)0x0) {
            FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0xa4));
          }
          if (*(LPCVOID *)((int)pvVar2 + 0xac) != (LPCVOID)0x0) {
            FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0xac));
          }
          FUN_004830f0(pvVar2);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_01);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this_01->mbr_0x0);
  }
  iVar3 = 0;
  if (0 < (int)this_01->mbr_0x0) {
    do {
      if ((-1 < iVar3) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x41c7f0).mbr_0x10 + iVar3 * 4), pvVar2 != (LPCVOID)0x0))
      {
        if (*(LPCVOID *)((int)pvVar2 + 0xa4) != (LPCVOID)0x0) {
          FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0xa4));
        }
        if (*(LPCVOID *)((int)pvVar2 + 0xac) != (LPCVOID)0x0) {
          FUN_004830f0(*(LPCVOID *)((int)pvVar2 + 0xac));
        }
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_01);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this_01->mbr_0x0);
  }
  this_01->mbr_0x0 = 0;
  (this->cls_0x41c7f0).mbr_0x4 = 0;
  meth_0x4459c0(this);
  return;
}



// Function at 0040e210

void cls_0x5a7e38::meth_0x40e210()

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  piVar1 = (int *)(this->cls_0x5a47f0).mbr_0x4;
  if (piVar1 == (int *)0x0) {
    return;
  }
  uVar2 = this->mbr_0x3c;
  uVar5 = uVar2 & 8;
  if (uVar5 == 0) {
    uVar4 = *(byte *)((int)piVar1 + 0x36) + 0x40;
  }
  else {
    uVar4 = *(byte *)((int)piVar1 + 0x36) + 0x100;
  }
  *(char *)((int)piVar1 + 0x36) = (char)uVar4;
  piVar1[0x2c] = uVar4 & 0xff;
  if ((uVar4 != 0) && (uVar4 < 0x100)) goto LAB_0040e2ba;
  if ((uVar2 & 4) == 0) {
LAB_0040e28b:
    iVar3 = *(int *)(this->cls_0x5a47f0).mbr_0x8;
    this->mbr_0x3c = this->mbr_0x3c & 0xfffffff1;
    (**(code **)(iVar3 + 0x50))(1);
    DAT_006682c4 = 0;
    DAT_006680b0 = 0;
    DAT_006682a8 = 0;
  }
  else {
    if (uVar5 == 0) {
      *(undefined *)((int)piVar1 + 0x36) = 0x20;
      piVar1[0x2c] = 0x20;
    }
    else {
      *(undefined *)((int)piVar1 + 0x36) = 0;
      piVar1[0x2c] = 0;
    }
    iVar3 = (**(code **)(*piVar1 + 0x18))((this->cls_0x5a47f0).mbr_0xc + 1);
    if (iVar3 == 0) goto LAB_0040e28b;
  }
  (**(code **)(*(int *)(this->cls_0x5a47f0).mbr_0x8 + 0x50))(1);
LAB_0040e2ba:
  (**(code **)(*(int *)(this->cls_0x5a47f0).mbr_0x4 + 0x158))(0);
  (**(code **)(*(int *)(this->cls_0x5a47f0).mbr_0x4 + 0x140))();
  return;
}



// Function at 0040e2e0

/* WARNING: Removing unreachable block (ram,0x0040e36a) */
/* WARNING: Removing unreachable block (ram,0x0040e379) */
/* WARNING: Removing unreachable block (ram,0x0040e3ae) */
/* WARNING: Removing unreachable block (ram,0x0040e3c0) */
/* WARNING: Removing unreachable block (ram,0x0040e407) */
/* WARNING: Removing unreachable block (ram,0x0040e414) */

void cls_0x5a7e38::virt_meth_0x40e2e0()

{
  dword dVar1;
  uint uVar2;
  int iVar3;
  
  dVar1 = (this->cls_0x5a47f0).mbr_0x4;
  if (dVar1 == 0) {
    uVar2 = (this->cls_0x5a47f0).mbr_0xc;
  }
  else {
    uVar2 = (uint)*(ushort *)(dVar1 + 0xc);
  }
  (*(this->cls_0x5a47f0).vftptr_0x0[0x14].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
  iVar3 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x14) {
    do {
      (**(code **)(**(int **)(*(int *)&this->field_0x68 + iVar3 * 4) + 0x14))(uVar2);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(this->cls_0x41c7f0).mbr_0x14);
  }
  cls_0x5a486c::meth_0x40b2e0
            ((cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8,(this->cls_0x5a47f0).mbr_0x4,
             (this->cls_0x5a47f0).mbr_0xc);
  cls_0x5a486c::meth_0x40c7d0
            ((cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8,s_effect_005c5eb8,
             (this->cls_0x5a47f0).mbr_0xc,(this->cls_0x5a47f0).mbr_0x14,&stack0xffffffa4,1,0);
  cls_0x5a486c::meth_0x40c7d0
            ((cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8,s_deleteeffect_005c5ec0,
             (this->cls_0x5a47f0).mbr_0xc,(this->cls_0x5a47f0).mbr_0x14,&stack0xffffffa4,1,0);
  FUN_0040b5c0((cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8,
               (TPlayScreen *)(this->cls_0x5a47f0).mbr_0x4,(this->cls_0x5a47f0).mbr_0xc,
               (this->cls_0x5a47f0).mbr_0x14);
  return;
}



// Function at 0040e460

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::virt_meth_0x40e460()

{
  int *piVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  
  meth_0x445a50(this);
  piVar1 = (int *)(this->cls_0x5a47f0).mbr_0x4;
  if (piVar1 != (int *)0x0) {
    iVar6 = piVar1[4];
    iVar2 = piVar1[5];
    iVar3 = piVar1[6];
    this->mbr_0x70 = 0;
    if ((((float)iVar6 != (float)this->mbr_0x78) || ((float)iVar2 != (float)this->mbr_0x7c)) ||
       ((float)iVar3 != (float)this->mbr_0x80)) {
      this->mbr_0x78 = (dword)(float)iVar6;
      this->mbr_0x70 = 1;
      this->mbr_0x7c = (dword)(float)iVar2;
      fVar4 = (float)iVar3;
      this->mbr_0x80 =
           (dword)((fVar4 / (_DAT_005a3520 - fVar4 * _DAT_005a3528 * _DAT_005a3524)) * _DAT_005a351c
                  );
    }
    fVar4 = (float)(uint)*(byte *)((int)piVar1 + 0x36) * _DAT_005a3778;
    fVar5 = (float)this->mbr_0x8c - fVar4;
    if (fVar5 < _DAT_005a3530) {
      fVar5 = -fVar5;
    }
    if ((float)_DAT_005a3770 < fVar5) {
      this->mbr_0x8c = (dword)fVar4;
      this->mbr_0x70 = 1;
    }
  }
  if (((*(byte *)&this->mbr_0x3c & 2) != 0) &&
     (iVar6 = (**(code **)(*piVar1 + 0x154))(), iVar6 != 0)) {
    meth_0x40e210(this);
  }
  return;
}



// Function at 0040e670

void cls_0x5a7e38::meth_0x40e670()

{
  float *in_stack_00000004;
  float local_40 [16];
  
  FUN_0043a9f0(in_stack_00000004);
  FUN_0043b230(in_stack_00000004,(float)this->mbr_0x8c);
  FUN_0043a9f0(local_40);
  FUN_0043b180(local_40,(float)this->mbr_0x84);
  FUN_0043aa90(in_stack_00000004,in_stack_00000004,local_40);
  FUN_0043a9f0(local_40);
  FUN_0043b1d0(local_40,(float)this->mbr_0x88);
  FUN_0043aa90(in_stack_00000004,in_stack_00000004,local_40);
  FUN_0043a9f0(local_40);
  FUN_0043b090(local_40,&this->mbr_0x78);
  FUN_0043aa90(in_stack_00000004,in_stack_00000004,local_40);
  return;
}



// Function at 0040e710

void cls_0x5a7e38::meth_0x40e710()

{
  int iVar1;
  dword *pdVar2;
  dword *in_stack_00000004;
  
  pdVar2 = &this->mbr_0xac;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar2 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    pdVar2 = pdVar2 + 1;
  }
  this->mbr_0x74 = 1;
  return;
}



// Function at 0040e740

void cls_0x5a7e38::meth_0x40e740()

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  undefined4 uVar4;
  longlong lVar5;
  dword *pdVar6;
  dword *pdVar7;
  float local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  if (((this->cls_0x5a47f0).mbr_0x4 != 0) && (DAT_00666644 == &DAT_006668d8)) {
    pdVar7 = &this->mbr_0xf4;
    pdVar6 = &this->mbr_0xf0;
    lVar5 = __ftol();
    iVar3 = (int)lVar5 + 0x32;
    lVar5 = __ftol();
    uVar4 = (undefined4)lVar5;
    lVar5 = __ftol();
    cls_0x411eb0::meth_0x415c70
              ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),DAT_005c61b4,(int)lVar5,uVar4,iVar3,pdVar6,
               pdVar7);
  }
  if (this->mbr_0x74 == 0) {
    pdVar7 = &this->mbr_0xac;
    FUN_0043a9f0(pdVar7);
    FUN_0043b230((float *)pdVar7,(float)this->mbr_0x8c);
    FUN_0043a9f0(&local_40);
    FUN_0043b180(&local_40,(float)this->mbr_0x84);
    FUN_0043aa90((float *)pdVar7,(float *)pdVar7,&local_40);
    FUN_0043a9f0(&local_40);
    FUN_0043b1d0(&local_40,(float)this->mbr_0x88);
    FUN_0043aa90((float *)pdVar7,(float *)pdVar7,&local_40);
    FUN_0043a9f0(&local_40);
    FUN_0043b090(&local_40,&this->mbr_0x78);
    FUN_0043aa90((float *)pdVar7,(float *)pdVar7,&local_40);
  }
  dVar1 = (this->cls_0x5a47f0).mbr_0x4;
  this->mbr_0x74 = 0;
  if (dVar1 != 0) {
    dVar1 = (this->cls_0x5a47f0).mbr_0x8;
    dVar2 = (this->cls_0x5a47f0).mbr_0x18;
    *(dword *)(dVar1 + 0xa4) = (this->cls_0x5a47f0).mbr_0x10;
    *(dword *)(dVar1 + 0xa8) = dVar2;
  }
  FUN_00416f30();
  local_40 = 2.802597e-45;
  local_3c = 0;
  local_28 = 0x461c3c00;
  local_30 = 0x461c3c00;
  local_38 = 0x461c3c00;
  local_24 = 0xc61c3c00;
  local_2c = 0xc61c3c00;
  local_34 = 0xc61c3c00;
  FUN_00417000(&local_40);
  this->mbr_0xec = 0;
  return;
}



// Function at 0040edc0

int cls_0x5a7e38::meth_0x40edc0()

{
  byte bVar1;
  cls_0x5a486c *pcVar2;
  uint uVar3;
  byte *pbVar4;
  int iVar5;
  int iVar6;
  uint *puVar7;
  bool bVar8;
  uint *in_stack_00000004;
  
  FUN_0059bd3e(in_stack_00000004);
  iVar6 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      if (*(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar6 * 4) != 0) {
        pcVar2 = (cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8;
        if (pcVar2->mbr_0xc == 0) {
          if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
            cls_0x5a486c::meth_0x447ac0(pcVar2);
          }
          cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
        }
        pcVar2 = (cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8;
        uVar3 = *(uint *)(*(int *)((this->cls_0x41c7f0).mbr_0x10 + iVar6 * 4) + 4);
        if (pcVar2->mbr_0xc == 0) {
          if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
            cls_0x5a486c::meth_0x447ac0(pcVar2);
          }
          cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
        }
        puVar7 = in_stack_00000004;
        if (((((int)uVar3 < 0) || ((int)pcVar2->mbr_0x64 < (int)uVar3)) || (pcVar2->mbr_0x74 == 0))
           || ((pcVar2->mbr_0x64 <= uVar3 || (*(int *)(pcVar2->mbr_0x74 + uVar3 * 4) == 0)))) {
          pbVar4 = (byte *)0x0;
        }
        else {
          pbVar4 = *(byte **)(pcVar2->mbr_0x74 + uVar3 * 4);
          if (pbVar4 == (byte *)0x0) {
            pbVar4 = (byte *)pcVar2->mbr_0x78;
          }
        }
        do {
          bVar1 = *pbVar4;
          bVar8 = bVar1 < *(byte *)puVar7;
          if (bVar1 != *(byte *)puVar7) {
LAB_0040eeb7:
            iVar5 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
            goto LAB_0040eebc;
          }
          if (bVar1 == 0) break;
          bVar1 = pbVar4[1];
          bVar8 = bVar1 < *(byte *)((int)puVar7 + 1);
          if (bVar1 != *(byte *)((int)puVar7 + 1)) goto LAB_0040eeb7;
          pbVar4 = pbVar4 + 2;
          puVar7 = (uint *)((int)puVar7 + 2);
        } while (bVar1 != 0);
        iVar5 = 0;
LAB_0040eebc:
        if (iVar5 == 0) {
          return iVar6;
        }
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return -1;
}



// Function at 0040eef0

int cls_0x5a7e38::meth_0x40eef0()

{
  int iVar1;
  int in_stack_00000004;
  
  if (((in_stack_00000004 < 0) || (0xff < in_stack_00000004)) ||
     (iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + in_stack_00000004 * 4), iVar1 == 0)) {
    iVar1 = 0;
  }
  return iVar1;
}



// Function at 0040ef80

undefined4 cls_0x5a7e38::meth_0x40ef80(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if (param_1 < 0) {
    return 0;
  }
  if ((0xff < param_1) ||
     (iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4), iVar1 == 0)) {
    iVar1 = 0;
  }
  puVar3 = (undefined4 *)(iVar1 + 0x58);
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *in_stack_00000008 = *puVar3;
    puVar3 = puVar3 + 1;
    in_stack_00000008 = in_stack_00000008 + 1;
  }
  return 1;
}



// Function at 0040efc0

undefined4 cls_0x5a7e38::meth_0x40efc0(int param_1, float *param_2)

{
  int iVar1;
  int iVar2;
  float *pfVar3;
  float *pfVar4;
  float *in_stack_0000000c;
  float local_4c;
  float local_48;
  float local_44;
  float local_40 [16];
  
  if (param_1 < 0) {
    return 0;
  }
  if ((0xff < param_1) ||
     (iVar1 = *(int *)((this->cls_0x41c7f0).mbr_0x10 + param_1 * 4), iVar1 == 0)) {
    iVar1 = 0;
  }
  pfVar3 = (float *)(iVar1 + 0x58);
  pfVar4 = local_40;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pfVar4 = *pfVar3;
    pfVar3 = pfVar3 + 1;
    pfVar4 = pfVar4 + 1;
  }
  pfVar3 = local_40;
  pfVar4 = local_40;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pfVar4 = *pfVar3;
    pfVar3 = pfVar3 + 1;
    pfVar4 = pfVar4 + 1;
  }
  if (in_stack_0000000c == (float *)0x0) {
    local_44 = 0.0;
    local_48 = 0.0;
    local_4c = 0.0;
  }
  else {
    local_4c = *in_stack_0000000c;
    local_48 = in_stack_0000000c[1];
    local_44 = in_stack_0000000c[2];
  }
  FUN_0043ad80(local_40,&local_4c,param_2);
  return 1;
}



// Function at 004459c0

void cls_0x5a7e38::meth_0x4459c0()

{
  LPCVOID pvVar1;
  
  pvVar1 = (LPCVOID)(this->cls_0x5a47f0).mbr_0x38;
  if (pvVar1 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar1);
    (this->cls_0x5a47f0).mbr_0x38 = 0;
  }
  return;
}



// Function at 004459e0

void cls_0x5a7e38::virt_meth_0x4459e0()

{
  if ((this->cls_0x5a47f0).mbr_0x4 != 0) {
    (*(this->cls_0x5a47f0).vftptr_0x0[1].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[2].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[3].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[4].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[5].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
  }
  (this->cls_0x5a47f0).mbr_0x2c = 1;
  (this->cls_0x5a47f0).mbr_0x30 = 0;
  return;
}



// Function at 00445a50

void cls_0x5a7e38::meth_0x445a50()

{
  if ((this->cls_0x5a47f0).mbr_0x4 != 0) {
    (*(this->cls_0x5a47f0).vftptr_0x0[1].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[2].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[3].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[4].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
    (*(this->cls_0x5a47f0).vftptr_0x0[5].~cls_0x5a47f0_0)(&this->cls_0x5a47f0);
  }
  return;
}



// Function at 004d76b0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

cls_0x5a7e38::cls_0x5a7e38()

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  cls_0x40b770 *pcVar4;
  int iVar5;
  dword *pdVar6;
  int iStack00000004;
  undefined4 local_68 [4];
  undefined4 uStack_58;
  undefined4 uStack_48;
  undefined4 uStack_38;
  undefined4 uStack_28;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059eb13;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5a47f0::cls_0x5a47f0(&this->cls_0x5a47f0);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  local_4 = CONCAT31(local_4._1_3_,1);
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0);
  this->mbr_0x1a8 = (dword)&cls_0x5a7e98__vftable_5a7e98_005a7e98;
  this->mbr_0x528 = 0;
  this->mbr_0x4fc = 0;
  this->mbr_0x55c = 0;
  local_4 = 6;
  (this->cls_0x5a47f0).vftptr_0x0 =
       (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a7e38__vftable_5a7e38_005a7e38;
  this->mbr_0xf78 = 0;
  puVar2 = FUN_00482fb0(0x80);
  this->mbr_0x57c = (dword)puVar2;
  puVar2 = FUN_00482fb0(0x80);
  pcVar4 = (cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8;
  this->mbr_0x580 = (dword)puVar2;
  iVar5 = 0;
  dVar1 = pcVar4->mbr_0x34;
  this->mbr_0x584 = 0;
  if (0 < (int)dVar1) {
    do {
      puVar2 = local_68;
      for (iVar3 = 0x17; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar2 = 0;
        puVar2 = puVar2 + 1;
      }
      cls_0x40b770::meth_0x40b770(pcVar4,iVar5);
      dVar1 = this->mbr_0x580;
      *(undefined4 *)(this->mbr_0x57c + iVar5 * 4) = local_68[1];
      pcVar4 = (cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8;
      *(undefined4 *)(dVar1 + iVar5 * 4) = local_68[2];
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)pcVar4->mbr_0x34);
  }
  iStack00000004 = (**(code **)(*(int *)(this->cls_0x5a47f0).mbr_0x4 + 0x2d8))();
  this->mbr_0x588 = (dword)((float)iStack00000004 * _DAT_005a350c);
  meth_0x4d83b0(this);
  meth_0x4d9ea0(this);
  iVar3 = 0;
  pdVar6 = &this->mbr_0x104;
  for (iVar5 = 0x28; iVar5 != 0; iVar5 = iVar5 + -1) {
    *pdVar6 = 0;
    pdVar6 = pdVar6 + 1;
  }
  pcVar4 = (cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8;
  this->mbr_0x58c = 0;
  if (0 < (int)pcVar4->mbr_0x34) {
    do {
      cls_0x40b770::meth_0x40b770(pcVar4,0);
      uStack_28 = 0x3f800000;
      uStack_38 = 0x3f800000;
      uStack_58 = 0x3f800000;
      uStack_48 = 0x3f800000;
      cls_0x40b770::meth_0x40b830(pcVar4,0);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)pcVar4->mbr_0x34);
  }
  this->mbr_0x1a4 = 0;
  ExceptionList = pvStack_c;
  return this;
}



// Function at 004d7860

cls_0x5a7e38 * __thiscall cls_0x5a7e38::~cls_0x5a7e38(cls_0x5a7e38 *this)

{
  cls_0x5b0074 *pcVar1;
  dword *pdVar2;
  int iVar3;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059eb68;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->cls_0x5a47f0).vftptr_0x0 =
       (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a7e38__vftable_5a7e38_005a7e38;
  local_4 = 4;
  meth_0x4d84b0(this);
  meth_0x4d7da0(this);
  if ((undefined4 *)this->mbr_0xfc != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0xfc)(1);
    this->mbr_0xfc = 0;
    FUN_004830f0((LPCVOID)this->mbr_0x100);
    this->mbr_0x100 = 0;
  }
  iVar3 = 8;
  pdVar2 = &this->mbr_0x104;
  do {
    if ((undefined4 *)*pdVar2 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)*pdVar2)(1);
    }
    pdVar2 = pdVar2 + 5;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  pdVar2 = &this->mbr_0x104;
  for (iVar3 = 0x28; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar2 = 0;
    pdVar2 = pdVar2 + 1;
  }
  pdVar2 = &this->mbr_0x1a8;
  cls_0x5a7e98::meth_0x4d92d0((cls_0x5a7e98 *)pdVar2);
  virt_meth_0x40de10(this);
  local_4 = CONCAT31(local_4._1_3_,3);
  if ((LPCVOID)this->mbr_0xcd0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xcd0);
  }
  if ((LPCVOID)this->mbr_0xcd8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xcd8);
  }
  local_4 = CONCAT31(local_4._1_3_,2);
  if ((LPCVOID)this->mbr_0x984 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x984);
  }
  if ((LPCVOID)this->mbr_0x98c != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x98c);
  }
  local_4 = CONCAT31(local_4._1_3_,1);
  if ((LPCVOID)this->mbr_0x638 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x638);
  }
  if ((LPCVOID)this->mbr_0x640 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x640);
  }
  *pdVar2 = (dword)&cls_0x5a7e98__vftable_5a7e98_005a7e98;
  local_4._0_1_ = 5;
  cls_0x5a7e98::meth_0x4d92d0((cls_0x5a7e98 *)pdVar2);
  local_4 = (uint)local_4._1_3_ << 8;
  if ((LPCVOID)this->mbr_0x250 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x250);
  }
  if ((LPCVOID)this->mbr_0x258 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x258);
  }
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = pvStack_c;
  return (cls_0x5a7e38 *)pcVar1;
}



// Function at 004d7a00

void cls_0x5a7e38::virt_meth_0x4d7a00()

{
  if ((*(uint *)((this->cls_0x5a47f0).mbr_0x4 + 8) & 0x2800000) == 0) {
    virt_meth_0x40e2e0(this);
  }
  meth_0x4da6f0(this);
  return;
}



// Function at 004d7a20

void cls_0x5a7e38::virt_meth_0x4d7a20()

{
  if (this->mbr_0x528 == 0) {
    meth_0x4d8f10(this);
  }
  virt_meth_0x40e460(this);
  return;
}



// Function at 004d7da0

void cls_0x5a7e38::meth_0x4d7da0()

{
  int iVar1;
  cls_0x40b770 *this_00;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_5c [23];
  
  iVar2 = 0;
  this_00 = (cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8;
  if (0 < (int)this_00->mbr_0x34) {
    do {
      puVar3 = local_5c;
      for (iVar1 = 0x17; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      cls_0x40b770::meth_0x40b770(this_00,iVar2);
      local_5c[1] = *(undefined4 *)(this->mbr_0x57c + iVar2 * 4);
      local_5c[3] = *(undefined4 *)(this->mbr_0x580 + iVar2 * 4);
      cls_0x40b770::meth_0x40b830((cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8,iVar2);
      this_00 = (cls_0x40b770 *)(this->cls_0x5a47f0).mbr_0x8;
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this_00->mbr_0x34);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x57c);
  FUN_004830f0((LPCVOID)this->mbr_0x580);
  return;
}



// Function at 004d7f20

void cls_0x5a7e38::meth_0x4d7f20()

{
  char cVar1;
  byte bVar2;
  int *piVar3;
  TPlayScreen *this_00;
  int *piVar4;
  cls_0x5a486c *this_01;
  char *pcVar5;
  byte *pbVar6;
  uint *puVar7;
  int iVar8;
  uint uVar9;
  dword *pdVar10;
  dword *pdVar11;
  bool bVar12;
  int in_stack_00000004;
  uint uVar13;
  dword dStack_18;
  int local_14;
  
  if (*(short *)((this->cls_0x5a47f0).mbr_0x4 + 4) == 0xb) {
    pdVar10 = &this->mbr_0x594;
    for (iVar8 = 0xd3; iVar8 != 0; iVar8 = iVar8 + -1) {
      *pdVar10 = 0;
      pdVar10 = pdVar10 + 1;
    }
    piVar3 = (int *)(this->cls_0x5a47f0).mbr_0x4;
    local_14 = 0;
    do {
      this_00 = (TPlayScreen *)piVar3[local_14 + 0xa8];
      if ((this_00 != (TPlayScreen *)0x0) &&
         (iVar8 = TPlayScreen::meth_0x46e8a0(this_00), **(int **)(*(int *)(iVar8 + 4) + 0x54) == 1)
         ) {
        this_01 = (cls_0x5a486c *)TPlayScreen::meth_0x46e8a0(this_00);
        pcVar5 = (char *)(**(code **)(*piVar3 + 0x300))();
        uVar9 = 0xffffffff;
        do {
          if (uVar9 == 0) break;
          uVar9 = uVar9 - 1;
          cVar1 = *pcVar5;
          pcVar5 = pcVar5 + 1;
        } while (cVar1 != '\0');
        dStack_18 = 0;
        iVar8 = cls_0x5a486c::meth_0x408f70(this_01);
        if (0 < iVar8) {
          do {
            pbVar6 = (byte *)cls_0x5a486c::meth_0x409200(this_01);
            bVar12 = true;
            bVar2 = *pbVar6;
            if ((bVar2 == 0x2a) || (bVar2 == 0x40)) {
              bVar12 = bVar2 != 0x40;
              pbVar6 = pbVar6 + 1;
            }
            uVar13 = ~uVar9 - 1;
            pcVar5 = (char *)(**(code **)(*piVar3 + 0x300))();
            FUN_0059a600(pbVar6,pcVar5,uVar13);
            if (local_14 == 4) {
              piVar4 = *(int **)((this->cls_0x5a47f0).mbr_0x4 + 0xe0);
              if ((piVar4 == (int *)0x0) || (*piVar4 != 0x19)) {
                iVar8 = meth_0x40edc0(this);
                if ((iVar8 < 0) && (iVar8 = meth_0x40edc0(this), iVar8 < 0)) goto LAB_004d80a8;
                goto LAB_004d80b8;
              }
            }
            else {
              if (local_14 == 7) {
                piVar4 = *(int **)((this->cls_0x5a47f0).mbr_0x4 + 0xe0);
                if (((piVar4 == (int *)0x0) || (*piVar4 != 0x19)) || (this->mbr_0x1a4 != 0))
                goto LAB_004d8180;
                iVar8 = meth_0x40edc0(this);
                if ((-1 < iVar8) || (iVar8 = meth_0x40edc0(this), -1 < iVar8)) goto LAB_004d80b8;
              }
LAB_004d80a8:
              iVar8 = meth_0x40edc0(this);
              if (-1 < iVar8) {
LAB_004d80b8:
                puVar7 = (uint *)meth_0x40eef0(this);
                if (in_stack_00000004 == 1) {
                  if (bVar12) {
                    *puVar7 = *puVar7 | 1;
                  }
                }
                else if (in_stack_00000004 == 2) {
                  this->mbr_0x598 = dStack_18;
                  puVar7 = puVar7 + 0x16;
                  pdVar10 = &this->mbr_0x5ec;
                  for (iVar8 = 0x10; iVar8 != 0; iVar8 = iVar8 + -1) {
                    *pdVar10 = *puVar7;
                    puVar7 = puVar7 + 1;
                    pdVar10 = pdVar10 + 1;
                  }
                  this->mbr_0x594 = 0x100;
                  meth_0x4d82a0(this);
                  cls_0x5a486c::meth_0x40a8f0(this_01);
                  if (*(short *)&(this_00->TScreen).mbr_0x4 == 6) {
                    pdVar10 = &this->mbr_0x594;
                    pdVar11 = &this->mbr_0x8e0;
                    for (iVar8 = 0xd3; iVar8 != 0; iVar8 = iVar8 + -1) {
                      *pdVar11 = *pdVar10;
                      pdVar10 = pdVar10 + 1;
                      pdVar11 = pdVar11 + 1;
                    }
                  }
                  if (0 < *(int *)((this->cls_0x5a47f0).mbr_0x4 + 0x224)) {
                    cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),8);
                    cls_0x5a486c::meth_0x40a8f0(this_01);
                  }
                }
              }
            }
LAB_004d8180:
            dStack_18 = dStack_18 + 1;
            iVar8 = cls_0x5a486c::meth_0x408f70(this_01);
          } while ((int)dStack_18 < iVar8);
        }
      }
      local_14 = local_14 + 1;
    } while (local_14 < 0xb);
  }
  return;
}



// Function at 004d82a0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::meth_0x4d82a0()

{
  int iVar1;
  cls_0x40b770 *in_stack_00000004;
  undefined4 uVar2;
  float local_60;
  
  local_60 = (float)this->mbr_0x588;
  if (this->mbr_0x1a4 != 0) {
    local_60 = 1.0;
  }
  local_60 = local_60 - _DAT_005a34e4;
  if (local_60 < _DAT_005a3530) {
    local_60 = -local_60;
  }
  if (local_60 <= _DAT_005a7ea8) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0x84;
  }
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),uVar2);
  iVar1 = 0;
  if (0 < (int)in_stack_00000004->mbr_0x34) {
    do {
      cls_0x40b770::meth_0x40b770(in_stack_00000004,iVar1);
      cls_0x40b770::meth_0x40b830(in_stack_00000004,iVar1);
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)in_stack_00000004->mbr_0x34);
  }
  return;
}



// Function at 004d83b0

void cls_0x5a7e38::meth_0x4d83b0()

{
  short sVar1;
  uint uVar2;
  int iVar3;
  dword dVar4;
  
  sVar1 = cls_0x4746d0::meth_0x475210
                    ((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,s_CharUtility_005e068c);
  uVar2 = (uint)sVar1;
  if ((((DAT_0066cc9c == 0) || (DAT_0066cc8c <= uVar2)) || (*(int *)(DAT_0066cc9c + uVar2 * 4) == 0)
      ) || ((iVar3 = *(int *)(DAT_0066cc9c + uVar2 * 4), iVar3 == 0 &&
            (iVar3 = DAT_0066cca0, DAT_0066cca0 == 0)))) {
    this->mbr_0x564 = 0;
  }
  else {
    dVar4 = FUN_00446b10(*(uint *)(iVar3 + 8),1);
    this->mbr_0x564 = dVar4;
  }
  dVar4 = cls_0x5a486c::meth_0x409ff0((cls_0x5a486c *)this->mbr_0x564,s_shadow_005e0698);
  this->mbr_0x568 = dVar4;
  dVar4 = cls_0x5a486c::meth_0x409ff0((cls_0x5a486c *)this->mbr_0x564,s_comflash_005e06a0);
  this->mbr_0x578 = dVar4;
  dVar4 = cls_0x5a486c::meth_0x409ff0((cls_0x5a486c *)this->mbr_0x564,s_comring_005e06ac);
  this->mbr_0x574 = dVar4;
  dVar4 = cls_0x5a486c::meth_0x409ff0((cls_0x5a486c *)this->mbr_0x564,s_vision_005e06b4);
  this->mbr_0x56c = dVar4;
  dVar4 = cls_0x5a486c::meth_0x409ff0((cls_0x5a486c *)this->mbr_0x564,s_flamef_005e06bc);
  this->mbr_0x570 = dVar4;
  return;
}



// Function at 004d84b0

void cls_0x5a7e38::meth_0x4d84b0()

{
  LPCVOID pvVar1;
  
  if ((cls_0x5a486c *)this->mbr_0x564 != (cls_0x5a486c *)0x0) {
    FUN_00446ba0((cls_0x5a486c *)this->mbr_0x564);
  }
  if ((uint *)this->mbr_0x568 != (uint *)0x0) {
    FUN_0040a260((uint *)this->mbr_0x568);
    FUN_0040a3d0((uint *)this->mbr_0x568);
    pvVar1 = (LPCVOID)this->mbr_0x568;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xa4));
      }
      if (*(LPCVOID *)((int)pvVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xac));
      }
      FUN_004830f0(pvVar1);
    }
    this->mbr_0x568 = 0;
  }
  if ((uint *)this->mbr_0x578 != (uint *)0x0) {
    FUN_0040a260((uint *)this->mbr_0x578);
    FUN_0040a3d0((uint *)this->mbr_0x578);
    pvVar1 = (LPCVOID)this->mbr_0x578;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xa4));
      }
      if (*(LPCVOID *)((int)pvVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xac));
      }
      FUN_004830f0(pvVar1);
    }
    this->mbr_0x578 = 0;
  }
  if ((uint *)this->mbr_0x574 != (uint *)0x0) {
    FUN_0040a260((uint *)this->mbr_0x574);
    FUN_0040a3d0((uint *)this->mbr_0x574);
    pvVar1 = (LPCVOID)this->mbr_0x574;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xa4));
      }
      if (*(LPCVOID *)((int)pvVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xac));
      }
      FUN_004830f0(pvVar1);
    }
    this->mbr_0x574 = 0;
  }
  if ((uint *)this->mbr_0x56c != (uint *)0x0) {
    FUN_0040a260((uint *)this->mbr_0x56c);
    FUN_0040a3d0((uint *)this->mbr_0x56c);
    pvVar1 = (LPCVOID)this->mbr_0x56c;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xa4));
      }
      if (*(LPCVOID *)((int)pvVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xac));
      }
      FUN_004830f0(pvVar1);
    }
    this->mbr_0x56c = 0;
  }
  if ((uint *)this->mbr_0x570 != (uint *)0x0) {
    FUN_0040a260((uint *)this->mbr_0x570);
    FUN_0040a3d0((uint *)this->mbr_0x570);
    pvVar1 = (LPCVOID)this->mbr_0x570;
    if (pvVar1 != (LPCVOID)0x0) {
      if (*(LPCVOID *)((int)pvVar1 + 0xa4) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xa4));
      }
      if (*(LPCVOID *)((int)pvVar1 + 0xac) != (LPCVOID)0x0) {
        FUN_004830f0(*(LPCVOID *)((int)pvVar1 + 0xac));
      }
      FUN_004830f0(pvVar1);
    }
    this->mbr_0x570 = 0;
  }
  return;
}



// Function at 004d86c0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::meth_0x4d86c0()

{
  float *pfVar1;
  uint *puVar2;
  dword dVar3;
  cls_0x5a486c *this_00;
  float fVar4;
  int iVar5;
  int iVar6;
  float local_f0;
  float fStack_e4;
  float fStack_e0;
  float fStack_dc;
  int iStack_d8;
  int iStack_d4;
  int iStack_d0;
  float fStack_cc;
  float fStack_c8;
  float fStack_c4;
  float local_90;
  float local_8c;
  float local_50;
  float local_4c;
  undefined4 local_40 [16];
  
  puVar2 = (uint *)this->mbr_0x568;
  iVar5 = meth_0x40edc0(this);
  iVar6 = meth_0x40edc0(this);
  if (iVar5 < 0) {
    local_f0 = 0.0;
    local_90 = _DAT_005a3530;
  }
  else {
    meth_0x40ef80(this,iVar5);
    local_f0 = local_8c;
  }
  if (-1 < iVar6) {
    meth_0x40ef80(this,iVar6);
    local_90 = local_50 * _DAT_005a7eb0 + local_90;
    local_f0 = local_4c * _DAT_005a7eb0 + local_f0;
  }
  fVar4 = local_90 * _DAT_005a356c;
  local_f0 = local_f0 * _DAT_005a356c;
  FUN_0043a9f0(local_40);
  pfVar1 = (float *)(puVar2 + 0x16);
  FUN_0043a9f0(pfVar1);
  iVar5 = (**(code **)(*(int *)(this->cls_0x5a47f0).mbr_0x4 + 600))();
  fStack_cc = (float)iVar5 * _DAT_005a7eac * (float)this->mbr_0x588;
  fStack_c8 = fStack_cc;
  fStack_c4 = fStack_cc;
  FUN_0043b130(pfVar1,&fStack_cc);
  dVar3 = (this->cls_0x5a47f0).mbr_0x4;
  iStack_d8 = *(int *)(dVar3 + 0x10);
  iStack_d4 = *(int *)(dVar3 + 0x14);
  iStack_d0 = *(int *)(dVar3 + 0x18);
  fStack_dc = 5.0;
  fStack_e4 = fVar4;
  fStack_e0 = local_f0;
  FUN_0043b090(pfVar1,&fStack_e4);
  FUN_0043b230(pfVar1,(float)(uint)*(byte *)((this->cls_0x5a47f0).mbr_0x4 + 0x36) * _DAT_005a4978);
  fStack_e4 = (float)iStack_d8 + fStack_e4 + _DAT_005a353c;
  fStack_e0 = (float)iStack_d4 + fStack_e0 + _DAT_005a353c;
  fStack_dc = ((float)iStack_d0 / (_DAT_005a3520 - (float)iStack_d0 * _DAT_005a3528 * _DAT_005a3524)
              ) * _DAT_005a351c + fStack_dc;
  FUN_0043b090(pfVar1,&fStack_e4);
  *puVar2 = *puVar2 | 0x5a100;
  puVar2[0x4e] = puVar2[0x2a];
  this_00 = (cls_0x5a486c *)this->mbr_0x564;
  puVar2[0x2c] = 0;
  puVar2[0x4d] = 0;
  puVar2[0x2d] = 0;
  cls_0x5a486c::meth_0x40a8f0(this_00);
  return;
}



// Function at 004d8bd0

int cls_0x5a7e38::meth_0x4d8bd0()

{
  char cVar1;
  int *piVar2;
  TPlayScreen *this_00;
  cls_0x5a486c *this_01;
  uint uVar3;
  byte *pbVar4;
  char *pcVar5;
  int iVar6;
  dword dVar7;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  int iVar8;
  byte *pbVar9;
  undefined4 *puVar10;
  uint local_368;
  int local_364;
  undefined4 local_358 [41];
  LPCVOID local_2b4;
  LPCVOID local_2ac;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059eb7d;
  local_c = ExceptionList;
  piVar2 = (int *)(this->cls_0x5a47f0).mbr_0x4;
  if (*(short *)(piVar2 + 1) != 0xb) {
    return -1;
  }
  this_00 = (TPlayScreen *)piVar2[0xac];
  local_4 = 0;
  puVar10 = local_358;
  for (iVar6 = 0xd3; iVar6 != 0; iVar6 = iVar6 + -1) {
    *puVar10 = 0;
    puVar10 = puVar10 + 1;
  }
  ExceptionList = &local_c;
  if ((this_00 != (TPlayScreen *)0x0) &&
     (ExceptionList = &local_c, iVar6 = TPlayScreen::meth_0x46e8a0(this_00),
     **(int **)(*(int *)(iVar6 + 4) + 0x54) == 1)) {
    this_01 = (cls_0x5a486c *)TPlayScreen::meth_0x46e8a0(this_00);
    local_368 = 0;
    local_364 = 0;
    dVar7 = this_01->mbr_0x4;
    iVar6 = *(int *)(dVar7 + 0x54);
    if (0 < *(int *)(iVar6 + 4)) {
      pbVar9 = (byte *)(iVar6 + 8);
      do {
        uVar3 = FUN_0059a530_stricmp(dVar7,pbVar9,(byte *)s_still_005e06dc);
        if ((uVar3 == 0) || (uVar3 = FUN_0059a530_stricmp(extraout_ECX,pbVar9,&DAT_005e06e4), uVar3 == 0)) {
          local_368 = 0;
          break;
        }
        pbVar4 = (byte *)(**(code **)(*piVar2 + 0x300))();
        uVar3 = FUN_0059a530_stricmp(extraout_ECX_00,pbVar9,pbVar4);
        if (uVar3 == 0) {
          pcVar5 = (char *)(**(code **)(*piVar2 + 0x300))();
          uVar3 = 0xffffffff;
          goto code_r0x004d8ce3;
        }
        dVar7 = *(uint *)(iVar6 + 4);
        local_364 = local_364 + 1;
        pbVar9 = pbVar9 + 0x4c;
      } while (local_364 < (int)dVar7);
    }
    goto LAB_004d8cfa;
  }
  goto joined_r0x004d8d19;
  while( true ) {
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
    if (cVar1 == '\0') break;
code_r0x004d8ce3:
    if (uVar3 == 0) break;
  }
  local_368 = ~uVar3 - 1;
LAB_004d8cfa:
  if (local_364 < *(int *)(iVar6 + 4)) {
    iVar8 = 0;
    iVar6 = cls_0x5a486c::meth_0x408f70(this_01);
    if (0 < iVar6) {
      do {
        iVar6 = cls_0x5a486c::meth_0x4093b0(this_01,iVar8);
        if (iVar6 == 0) {
          pbVar9 = (byte *)cls_0x5a486c::meth_0x409200(this_01);
          if (0 < (int)local_368) {
            uVar3 = local_368;
            pcVar5 = (char *)(**(code **)(*piVar2 + 0x300))();
            uVar3 = FUN_0059a600(pbVar9,pcVar5,uVar3);
            if (uVar3 == 0) {
              pbVar9 = pbVar9 + local_368;
            }
          }
          iVar6 = meth_0x40edc0(this);
          if (-1 < iVar6) {
LAB_004d8e22:
            local_4 = 0xffffffff;
            if (local_2b4 != (LPCVOID)0x0) {
              FUN_004830f0(local_2b4);
            }
            if (local_2ac != (LPCVOID)0x0) {
              FUN_004830f0(local_2ac);
              ExceptionList = local_c;
              return iVar8;
            }
            ExceptionList = local_c;
            return iVar8;
          }
          uVar3 = FUN_0059a530_stricmp(extraout_ECX_01,pbVar9,(byte *)s_weapon_005e06e8);
          if (uVar3 == 0) {
            iVar6 = meth_0x40edc0(this);
          }
          if (-1 < iVar6) goto LAB_004d8e22;
        }
        iVar8 = iVar8 + 1;
        iVar6 = cls_0x5a486c::meth_0x408f70(this_01);
      } while (iVar8 < iVar6);
    }
    local_4 = 0xffffffff;
    if (local_2b4 != (LPCVOID)0x0) {
      FUN_004830f0(local_2b4);
    }
    goto joined_r0x004d8dfb;
  }
joined_r0x004d8d19:
  local_4 = 0xffffffff;
  if (local_2b4 != (LPCVOID)0x0) {
    local_4 = 0xffffffff;
    FUN_004830f0(local_2b4);
  }
joined_r0x004d8dfb:
  if (local_2ac != (LPCVOID)0x0) {
    FUN_004830f0(local_2ac);
  }
  ExceptionList = local_c;
  return -1;
}



// Function at 004d8e60

void cls_0x5a7e38::meth_0x4d8e60(undefined4 *param_1)

{
  dword dVar1;
  TPlayScreen *this_00;
  int iVar2;
  cls_0x5a486c *this_01;
  undefined4 *in_stack_00000008;
  
  iVar2 = meth_0x4d8bd0(this);
  if (((iVar2 == -1) || (dVar1 = (this->cls_0x5a47f0).mbr_0x4, *(short *)(dVar1 + 4) != 0xb)) ||
     (this_00 = *(TPlayScreen **)(dVar1 + 0x2b0), this_00 == (TPlayScreen *)0x0)) {
LAB_004d8e93:
    this_01 = (cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8;
    iVar2 = meth_0x40edc0(this);
    if (-1 < iVar2) goto LAB_004d8ecc;
    iVar2 = meth_0x40edc0(this);
    if (-1 < iVar2) goto LAB_004d8ecc;
    iVar2 = meth_0x40edc0(this);
  }
  else {
    this_01 = (cls_0x5a486c *)TPlayScreen::meth_0x46e8a0(this_00);
    if (this_01 == (cls_0x5a486c *)0x0) goto LAB_004d8e93;
  }
  if (iVar2 < 0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    *in_stack_00000008 = 0;
    in_stack_00000008[1] = 0;
    in_stack_00000008[2] = 0;
    return;
  }
LAB_004d8ecc:
  cls_0x5a486c::meth_0x408c20(this_01,iVar2,param_1,in_stack_00000008,0);
  return;
}



// Function at 004d8f10

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::meth_0x4d8f10()

{
  char cVar1;
  dword dVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  char *pcVar8;
  char *pcVar9;
  undefined4 *puVar10;
  undefined4 local_38 [6];
  cls_0x5a7e38 *local_20;
  float fStack_1c;
  float fStack_18;
  float fStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 *local_8;
  undefined4 uStack_4;
  
  if ((this->cls_0x5a47f0).mbr_0x4 != 0) {
    puVar7 = local_38;
    for (iVar4 = 0xe; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    meth_0x4d8e60(this,local_38);
    dVar2 = (this->cls_0x5a47f0).mbr_0x4;
    if (*(short *)(dVar2 + 4) == 0xb) {
      piVar3 = *(int **)(dVar2 + 0x2b0);
      if (piVar3 != (int *)0x0) {
        pcVar8 = (char *)piVar3[0xe];
        if (pcVar8 == (char *)0x0) {
          local_8 = (undefined4 *)0x0;
        }
        else {
          uVar5 = 0xffffffff;
          pcVar9 = pcVar8;
          do {
            if (uVar5 == 0) break;
            uVar5 = uVar5 - 1;
            cVar1 = *pcVar9;
            pcVar9 = pcVar9 + 1;
          } while (cVar1 != '\0');
          local_8 = FUN_00482ef0(~uVar5);
          uVar5 = 0xffffffff;
          do {
            pcVar9 = pcVar8;
            if (uVar5 == 0) break;
            uVar5 = uVar5 - 1;
            pcVar9 = pcVar8 + 1;
            cVar1 = *pcVar8;
            pcVar8 = pcVar9;
          } while (cVar1 != '\0');
          uVar5 = ~uVar5;
          puVar7 = (undefined4 *)(pcVar9 + -uVar5);
          puVar10 = local_8;
          for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
            *puVar10 = *puVar7;
            puVar7 = puVar7 + 1;
            puVar10 = puVar10 + 1;
          }
          for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
            *(undefined *)puVar10 = *(undefined *)puVar7;
            puVar7 = (undefined4 *)((int)puVar7 + 1);
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
        }
        uStack_4 = (**(code **)(*piVar3 + 0xd4))(s_SwipeFull_005e0714);
      }
    }
    else {
      local_8 = (undefined4 *)0x0;
    }
    dVar2 = (this->cls_0x5a47f0).mbr_0x4;
    iVar4 = *(int *)(dVar2 + 0xfc);
    if (((*(byte *)(iVar4 + 0x16a) != 0) || (*(char *)(iVar4 + 0x169) != '\0')) ||
       (*(char *)(iVar4 + 0x168) != '\0')) {
      if (((*(byte *)(iVar4 + 0x16a) == 0xffffffff) && (*(byte *)(iVar4 + 0x169) == 0xffffffff)) &&
         (*(byte *)(iVar4 + 0x168) == 0xffffffff)) {
        *(undefined *)(iVar4 + 0x16a) = 0;
        *(undefined *)(iVar4 + 0x169) = 0;
        *(undefined *)(iVar4 + 0x168) = 0;
      }
      fStack_1c = (float)(uint)*(byte *)(iVar4 + 0x16a) * _DAT_005a3830;
      fStack_18 = (float)(uint)*(byte *)(iVar4 + 0x169) * _DAT_005a3830;
      fStack_14 = (float)(uint)*(byte *)(iVar4 + 0x168) * _DAT_005a3830;
      if (*(short *)(dVar2 + 4) != 0xb) {
        uStack_4 = *(undefined4 *)(iVar4 + 0x16c);
      }
      uStack_c = 8;
      uStack_10 = 0x28;
      local_20 = this;
      cls_0x5a7e98::meth_0x4d90b0((cls_0x5a7e98 *)&this->mbr_0x1a8);
    }
  }
  return;
}



// Function at 004d9ea0

void cls_0x5a7e38::meth_0x4d9ea0()

{
  char cVar1;
  dword dVar2;
  undefined uVar3;
  cls_0x5a68d8 *pcVar4;
  cls_0x5a68d8 *this_00;
  undefined3 extraout_var;
  undefined4 *puVar5;
  uint uVar6;
  uint uVar7;
  cls_0x5a68d8 *this_01;
  int unaff_EBP;
  undefined4 *puVar8;
  char *pcVar9;
  char *pcVar10;
  undefined4 *puVar11;
  bool bVar12;
  undefined4 uVar13;
  cls_0x419dd0 cVar14;
  int iVar15;
  int iVar16;
  cls_0x5a68d8 *local_68 [17];
  void *pvStack_24;
  undefined4 local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ebc0;
  local_c = ExceptionList;
  bVar12 = DAT_0066829c != 0;
  ExceptionList = &local_c;
  this->mbr_0xfc = 0;
  this->mbr_0x100 = 0;
  if (((bVar12) && (dVar2 = (this->cls_0x5a47f0).mbr_0x4, dVar2 != DAT_00667fcc)) &&
     (*(short *)(dVar2 + 4) == 0xb)) {
    pcVar4 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 0;
    local_68[0] = pcVar4;
    if (pcVar4 == (cls_0x5a68d8 *)0x0) {
      pcVar4 = (cls_0x5a68d8 *)0x0;
    }
    else {
      bVar12 = DAT_00669324 != 0;
      cls_0x5a68d8::cls_0x5a68d8(pcVar4);
      local_4 = CONCAT31(local_4._1_3_,1);
      pcVar4->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar4->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar4,0x40,(-(uint)bVar12 & 0x20) + 0x20);
      pcVar4->mbr_0x70 = 1;
    }
    local_4 = 0xffffffff;
    this->mbr_0xfc = (dword)pcVar4;
    pcVar4 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_4 = 2;
    local_68[0] = pcVar4;
    if (pcVar4 == (cls_0x5a68d8 *)0x0) {
      pcVar4 = (cls_0x5a68d8 *)0x0;
    }
    else {
      bVar12 = DAT_00669324 != 0;
      cls_0x5a68d8::cls_0x5a68d8(pcVar4);
      local_4 = CONCAT31(local_4._1_3_,3);
      pcVar4->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      pcVar4->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(pcVar4,0x40,(-(uint)bVar12 & 0x20) + 0x20);
      pcVar4->mbr_0x70 = 1;
    }
    iVar15 = 1;
    local_4 = 0xffffffff;
    (*pcVar4->vftptr_0x0->virt_meth_0x419e20_28)(pcVar4);
    (*pcVar4->vftptr_0x0->virt_meth_0x4bde60_100)
              (pcVar4,0,0,(char)pcVar4->mbr_0x4,(char)pcVar4->mbr_0x8,1,0xff,0x7f7f);
    cls_0x429950::cls_0x429950((cls_0x429950 *)&stack0xffffff90,0xff,0xff);
    cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff70);
    uVar13 = 0;
    uVar3 = cls_0x4da8f0::meth_0x4da8f0(*(cls_0x4da8f0 **)(unaff_EBP + 4));
    cls_0x5a68d8::meth_0x4be2b0
              (pcVar4,0,0,0x40,(-(uint)(DAT_00669324 != 0) & 0x20) + 0x20,uVar3,uVar13);
    this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    local_c = (void *)0x4;
    this_01 = (cls_0x5a68d8 *)0x0;
    if (this_00 != (cls_0x5a68d8 *)0x0) {
      bVar12 = DAT_00669324 != 0;
      cls_0x5a68d8::cls_0x5a68d8(this_00);
      local_c = (void *)CONCAT31(local_c._1_3_,5);
      this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      this_00->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(this_00,0x40,(-(uint)bVar12 & 0x20) + 0x20);
      this_00->mbr_0x70 = 1;
      this_01 = this_00;
    }
    local_c = (void *)0xffffffff;
    FUN_00438d80(local_68,0,0,0,0,pcVar4->mbr_0x4,pcVar4->mbr_0x8,0x100);
    cVar14.mbr_0x0 = (dword)pcVar4;
    (*this_01->vftptr_0x0->virt_meth_0x4bd490_92)(this_01,local_68);
    uVar3 = (*this_01->vftptr_0x0->virt_meth_0x438cf0_16)(this_01);
    iVar16 = iVar15;
    FUN_004a5f90(*(int *)(iVar15 + 0xfc),CONCAT31(extraout_var,uVar3),cVar14.mbr_0x0);
    uVar6 = 0xffffffff;
    pcVar9 = *(char **)(*(int *)(iVar15 + 4) + 0x38);
    pcVar10 = pcVar9;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    puVar5 = FUN_00482ef0(~uVar6);
    uVar6 = 0xffffffff;
    do {
      pcVar10 = pcVar9;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar10 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar10;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    puVar8 = (undefined4 *)(pcVar10 + -uVar6);
    puVar11 = puVar5;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar11 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar11 = puVar11 + 1;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined *)puVar11 = *(undefined *)puVar8;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
      puVar11 = (undefined4 *)((int)puVar11 + 1);
    }
    *(undefined4 **)(iVar16 + 0x100) = puVar5;
    if (pcVar4 != (cls_0x5a68d8 *)0x0) {
      (*pcVar4->vftptr_0x0->~cls_0x5a68d8_0)(pcVar4);
    }
    if (this_01 != (cls_0x5a68d8 *)0x0) {
      (*this_01->vftptr_0x0->~cls_0x5a68d8_0)(this_01);
    }
  }
  ExceptionList = pvStack_24;
  return;
}



// Function at 004da1c0

void cls_0x5a7e38::meth_0x4da1c0()

{
  byte bVar1;
  undefined4 *puVar2;
  int *piVar3;
  byte *pbVar4;
  int iVar5;
  byte *pbVar6;
  bool bVar7;
  char local_c;
  int local_8;
  
  puVar2 = (undefined4 *)this->mbr_0xfc;
  if (puVar2 != (undefined4 *)0x0) {
    pbVar6 = *(byte **)((this->cls_0x5a47f0).mbr_0x4 + 0x38);
    pbVar4 = (byte *)this->mbr_0x100;
    do {
      bVar1 = *pbVar4;
      bVar7 = bVar1 < *pbVar6;
      if (bVar1 != *pbVar6) {
LAB_004da206:
        iVar5 = (1 - (uint)bVar7) - (uint)(bVar7 != 0);
        goto LAB_004da20b;
      }
      if (bVar1 == 0) break;
      bVar1 = pbVar4[1];
      bVar7 = bVar1 < pbVar6[1];
      if (bVar1 != pbVar6[1]) goto LAB_004da206;
      pbVar4 = pbVar4 + 2;
      pbVar6 = pbVar6 + 2;
    } while (bVar1 != 0);
    iVar5 = 0;
LAB_004da20b:
    if (iVar5 != 0) {
      if (puVar2 != (undefined4 *)0x0) {
        (**(code **)*puVar2)(1);
      }
      this->mbr_0xfc = 0;
      FUN_004830f0((LPCVOID)this->mbr_0x100);
      this->mbr_0x100 = 0;
      meth_0x4d9ea0(this);
      if (this->mbr_0xfc == 0) {
        return;
      }
    }
    FUN_0046d810((this->cls_0x5a47f0).mbr_0x4 + 0x10,(undefined4 *)&local_c);
    piVar3 = (int *)(this->cls_0x5a47f0).mbr_0x4;
    local_8 = local_8 - *(int *)(piVar3[0x3f] + 0x58c);
    (**(code **)(*piVar3 + 0x2d8))();
    cls_0x411eb0::meth_0x414d70
              ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(local_c - (char)DAT_00666904) + -0x20,
               (char)local_8 - (char)DAT_00666908,1,this->mbr_0xfc,0);
  }
  return;
}



// Function at 004da6f0

void cls_0x5a7e38::meth_0x4da6f0()

{
  dword dVar1;
  dword *pdVar2;
  int iVar3;
  
  pdVar2 = &this->mbr_0x104;
  iVar3 = 8;
  do {
    if ((*pdVar2 != 0) && (0 < (int)pdVar2[1])) {
      dVar1 = pdVar2[1] - 1;
      pdVar2[1] = dVar1;
      if ((int)dVar1 < 1) {
        if ((undefined4 *)*pdVar2 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)*pdVar2)(1);
        }
        *pdVar2 = 0;
        pdVar2[1] = 0;
        pdVar2[2] = 0;
        pdVar2[3] = 0;
        pdVar2[4] = 0;
      }
      else {
        pdVar2[4] = pdVar2[4] + 400;
      }
    }
    pdVar2 = pdVar2 + 5;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



// Function at 004da8d0

cls_0x5a7e38 * cls_0x5a7e38::virt_meth_0x4da8d0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a7e38(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004e7c60

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::meth_0x4e7c60()

{
  dword *pdVar1;
  int iVar2;
  longlong lVar3;
  int in_stack_00000004;
  
  pdVar1 = &this->mbr_0x110 + in_stack_00000004 * 10;
  iVar2 = _rand();
  *pdVar1 = (dword)(((float)iVar2 * _DAT_005aa4dc * _DAT_005aa4d8 - _DAT_005aa4d4) +
                   (float)this->mbr_0x100);
  iVar2 = _rand();
  pdVar1[2] = 0;
  pdVar1[8] = 0;
  pdVar1[1] = (dword)(((float)iVar2 * _DAT_005aa4dc * _DAT_005aa4d8 - _DAT_005aa4d4) +
                     (float)this->mbr_0x104);
  iVar2 = _rand();
  pdVar1[4] = (dword)((float)iVar2 * _DAT_005aa4dc - _DAT_005a356c);
  iVar2 = _rand();
  pdVar1[5] = (dword)((float)iVar2 * _DAT_005aa4dc - _DAT_005a356c);
  iVar2 = _rand();
  pdVar1[6] = (dword)((float)iVar2 * _DAT_005aa4dc);
  _rand();
  lVar3 = __ftol();
  pdVar1[9] = (dword)(float)lVar3;
  iVar2 = _rand();
  pdVar1[3] = (dword)((float)iVar2 * _DAT_005aa4dc * _DAT_005aa4d0 + _DAT_005a4848);
  return;
}



// Function at 004f6e20

void cls_0x5a7e38::meth_0x4f6e20()

{
  if ((*(byte *)&this->mbr_0x3c & 2) != 0) {
    cls_0x5a486c::meth_0x40ca90
              ((cls_0x5a486c *)(this->cls_0x5a47f0).mbr_0x8,(this->cls_0x5a47f0).mbr_0x4,
               (this->cls_0x5a47f0).mbr_0xc);
  }
  this->mbr_0xec = 1;
  return;
}



// Function at 00526040

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a7e38::meth_0x526040()

{
  dword *pdVar1;
  int iVar2;
  float fVar3;
  longlong lVar4;
  int in_stack_00000004;
  
  pdVar1 = &this->mbr_0x110 + in_stack_00000004 * 0x13;
  iVar2 = FUN_00483300_RandomRange(-0xf,0xf);
  *pdVar1 = (dword)(float)iVar2;
  iVar2 = FUN_00483300_RandomRange(-0xf,0xf);
  pdVar1[2] = 0x42c80000;
  pdVar1[1] = (dword)(float)iVar2;
  iVar2 = FUN_00483300_RandomRange(0,100);
  if (iVar2 < 0x50) {
    pdVar1[0x10] = 1;
  }
  else {
    pdVar1[0x10] = 0;
  }
  iVar2 = _rand();
  pdVar1[4] = (dword)((float)iVar2 * _DAT_005aa4dc * _DAT_005aa894 - _DAT_005aedb0);
  iVar2 = _rand();
  pdVar1[5] = (dword)((float)iVar2 * _DAT_005aa4dc * _DAT_005aa894 - _DAT_005aedb0);
  iVar2 = _rand();
  pdVar1[6] = (dword)((float)iVar2 * _DAT_005aa4dc * _DAT_005aa894 - _DAT_005aedb0);
  _rand();
  lVar4 = __ftol();
  pdVar1[0xe] = (dword)(float)lVar4;
  _rand();
  lVar4 = __ftol();
  pdVar1[0xf] = (dword)(float)lVar4;
  iVar2 = _rand();
  pdVar1[3] = (dword)((float)iVar2 * _DAT_005aa4dc * _DAT_005aa4d0 + _DAT_005a4848);
  iVar2 = FUN_00483300_RandomRange(0,0x167);
  pdVar1[8] = (dword)((float)iVar2 * _DAT_005a39f0);
  iVar2 = FUN_00483300_RandomRange(0,0x167);
  pdVar1[9] = (dword)((float)iVar2 * _DAT_005a39f0);
  iVar2 = FUN_00483300_RandomRange(0,0x167);
  pdVar1[10] = (dword)((float)iVar2 * _DAT_005a39f0);
  iVar2 = FUN_00483300_RandomRange(0,0x28);
  pdVar1[0xb] = (dword)((float)iVar2 * _DAT_005a3538);
  iVar2 = FUN_00483300_RandomRange(0,0x28);
  pdVar1[0xc] = (dword)((float)iVar2 * _DAT_005a3538);
  iVar2 = FUN_00483300_RandomRange(0,0x28);
  pdVar1[0xd] = (dword)((float)iVar2 * _DAT_005a3538);
  fVar3 = (float)FUN_00483300_RandomRange(0,3);
  pdVar1[0x11] = (dword)fVar3;
  pdVar1[0x12] = 0;
  return;
}



