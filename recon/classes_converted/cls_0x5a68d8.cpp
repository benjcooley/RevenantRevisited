#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a68d8



// Function at 00419e10

dword cls_0x5a68d8::virt_meth_0x419e10()

{
  return this->mbr_0xc;
}



// Function at 00419e20

void cls_0x5a68d8::virt_meth_0x419e20()

{
  dword in_stack_00000004;
  
  this->mbr_0x30 = in_stack_00000004;
  return;
}



// Function at 00419e60

void cls_0x5a68d8::virt_meth_0x419e60(dword param_1)

{
  dword in_stack_00000008;
  
  this->mbr_0x14 = param_1;
  this->mbr_0x18 = in_stack_00000008;
  return;
}



// Function at 00419e80

void cls_0x5a68d8::virt_meth_0x419e80(dword param_1, dword param_2, dword param_3)

{
  dword in_stack_00000010;
  
  this->mbr_0x20 = param_1;
  this->mbr_0x24 = param_2;
  this->mbr_0x28 = param_3;
  this->mbr_0x2c = in_stack_00000010;
  return;
}



// Function at 00419ea0

void cls_0x5a68d8::virt_meth_0x419ea0()

{
  dword in_stack_00000004;
  
  this->mbr_0x1c = in_stack_00000004;
  return;
}



// Function at 00438df0

void cls_0x5a68d8::meth_0x438df0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8, undefined4 param_9)

{
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  local_2c = param_1;
  local_28 = param_2;
  local_24 = param_6;
  local_14 = param_6;
  local_54 = param_8;
  local_50 = 0;
  local_4c = 0;
  local_48 = 0;
  local_1c = param_4;
  local_2 = 0;
  local_4 = 0;
  local_40 = 0;
  local_44 = 0;
  local_30 = 0;
  local_34 = 0;
  local_38 = 0;
  local_3c = 0;
  local_c = 0;
  local_20 = param_7;
  local_18 = param_5;
  local_10 = param_7;
  local_8 = 0x1f;
  (*this->vftptr_0x0->virt_meth_0x4bd490_92)(this,&local_54);
  return;
}



// Function at 00438ed0

void cls_0x5a68d8::meth_0x438ed0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined1 param_4, undefined4 param_5, uint param_6)

{
  cls_0x419dd0 cStack_10;
  uint uStack_c;
  
  cStack_10.mbr_0x0 = param_6 | 0x80;
  uStack_c = cStack_10.mbr_0x0;
  cls_0x419dd0::cls_0x419dd0(&cStack_10);
  meth_0x4be2b0_CompositeBuffer(this,(undefined)param_1,(undefined)param_2,0x10,10000,(undefined)param_3,0);
  return;
}



// Function at 004a39a0

void cls_0x5a68d8::meth_0x4a39a0(int param_1, int param_2)

{
  dword dVar1;
  dword dVar2;
  uint uVar3;
  dword *pdVar4;
  uint in_stack_0000000c;
  
  pdVar4 = (dword *)FUN_004a1ec0(param_1,param_2,in_stack_0000000c,0);
  dVar1 = *pdVar4;
  dVar2 = pdVar4[1];
  this->mbr_0x64 = (dword)pdVar4;
  uVar3 = pdVar4[4];
  this->mbr_0x4 = dVar1;
  this->mbr_0x10 = dVar1;
  this->mbr_0x8 = dVar2;
  this->mbr_0x38 = in_stack_0000000c;
  if ((uVar3 & 1) != 0) {
    this->mbr_0xc = 8;
    this->mbr_0x68 = 1;
    return;
  }
  if ((uVar3 & 2) != 0) {
    this->mbr_0xc = 0xf;
    this->mbr_0x68 = 1;
    return;
  }
  if ((uVar3 & 4) != 0) {
    this->mbr_0xc = 0x10;
    this->mbr_0x68 = 1;
    return;
  }
  if ((uVar3 & 8) != 0) {
    this->mbr_0xc = 0x18;
    this->mbr_0x68 = 1;
    return;
  }
  if ((uVar3 & 0x10) != 0) {
    this->mbr_0xc = 0x20;
  }
  this->mbr_0x68 = 1;
  return;
}



// Function at 004a3a40

void cls_0x5a68d8::meth_0x4a3a40()

{
  dword dVar1;
  dword dVar2;
  uint uVar3;
  dword *in_stack_00000004;
  
  this->mbr_0x64 = (dword)in_stack_00000004;
  this->mbr_0x68 = 0;
  dVar1 = *in_stack_00000004;
  dVar2 = in_stack_00000004[1];
  uVar3 = in_stack_00000004[4];
  this->mbr_0x8 = dVar2;
  this->mbr_0x2c = dVar2;
  this->mbr_0x4 = dVar1;
  this->mbr_0x28 = dVar1;
  this->mbr_0x38 = uVar3;
  this->mbr_0x10 = dVar1;
  if ((uVar3 & 1) != 0) {
    this->mbr_0xc = 8;
    return;
  }
  if ((uVar3 & 2) != 0) {
    this->mbr_0xc = 0xf;
    return;
  }
  if ((uVar3 & 4) != 0) {
    this->mbr_0xc = 0x10;
    return;
  }
  if ((uVar3 & 8) != 0) {
    this->mbr_0xc = 0x18;
    return;
  }
  if ((uVar3 & 0x10) != 0) {
    this->mbr_0xc = 0x20;
  }
  return;
}



// Function at 004a5740

void cls_0x5a68d8::meth_0x4a5740(uint param_1, int param_2)

{
  int iVar1;
  int *unaff_ESI;
  uint *puVar2;
  undefined4 *puVar3;
  uint in_stack_0000000c;
  dword in_stack_00000014;
  undefined local_88 [4];
  uint local_84 [5];
  int iStack_70;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_30;
  uint uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  uint uStack_1c;
  uint uStack_18;
  cls_0x5a68d8 *pcStack_10;
  undefined4 uStack_c;
  cls_0x5a68d8 *local_8;
  
  if (this->mbr_0x68 != 0) {
    return;
  }
  if ((DAT_006680e0 != 0) && ((in_stack_0000000c & 0x400) != 0)) {
    in_stack_0000000c = in_stack_0000000c & 0xffffbfff;
  }
  if (DAT_00669ad8 == 0) {
    in_stack_0000000c = in_stack_0000000c & 0xffffc3ff | 0x200;
  }
  if (((in_stack_0000000c & 0x3000) != 0) && (DAT_006680fc == 0)) {
    in_stack_0000000c = in_stack_0000000c & 0xffffcfff;
  }
  if ((in_stack_0000000c & 0x400) != 0) {
    if ((in_stack_0000000c & 0x80) == 0) {
      in_stack_0000000c = in_stack_0000000c & 0xffffefff | 0x2000;
    }
    if ((((in_stack_0000000c & 0x400) != 0) && ((in_stack_0000000c & 0x1000) == 0)) &&
       (local_8 = this, iVar1 = FUN_004a8170(), iVar1 + (param_1 & 0xfffe) * param_2 * -2 < 0x80000)
       ) {
      if ((in_stack_0000000c & 0x800) != 0) {
        FUN_00481d10((byte *)s_Not_enough_video_memory_for_text_005dae64);
      }
      in_stack_0000000c = in_stack_0000000c & 0xfffffbff | 0x200;
    }
  }
  puVar2 = local_84;
  for (iVar1 = 0x1f; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  local_84[0] = 0x7c;
  local_84[1] = 7;
  if ((in_stack_0000000c & 0x80) == 0) {
    uStack_1c = 0;
  }
  else {
    uStack_1c = 0x1000;
    if ((in_stack_0000000c & 0x100) != 0) {
      uStack_1c = 0x4001000;
    }
    if ((((in_stack_0000000c & 0x18000) != 0) && ((in_stack_0000000c & 0x200) == 0)) &&
       (DAT_006699cc != 0)) {
      local_84[1] = 0x100007;
      if ((in_stack_0000000c & 0x8000) == 0) {
        if ((in_stack_0000000c & 0x10000) != 0) {
          uStack_c = 1;
        }
      }
      else {
        uStack_c = 0;
      }
    }
    if ((in_stack_0000000c & 0x20000) != 0) {
      uStack_18 = uStack_18 | 4;
    }
    if ((in_stack_0000000c & 0x40000) != 0) {
      uStack_18 = uStack_18 | 8;
    }
    if ((in_stack_0000000c & 0x80000) != 0) {
      uStack_18 = uStack_18 | 0x80;
    }
    if ((in_stack_0000000c & 0x100000) != 0) {
      uStack_18 = uStack_18 | 0x10;
    }
  }
  local_8 = (cls_0x5a68d8 *)(in_stack_0000000c & 0x400);
  if (local_8 == (cls_0x5a68d8 *)0x0) {
    if ((in_stack_0000000c & 0x200) == 0) {
      FUN_00481d10((byte *)s_Invalid_TDDSurface_initialize_fl_005dae8c);
    }
    else {
      uStack_1c = uStack_1c | 0x800;
    }
  }
  else {
    uStack_1c = uStack_1c | 0x4000;
  }
  if ((in_stack_0000000c & 0x2000) == 0) {
    if ((in_stack_0000000c & 0x1000) != 0) {
      uStack_1c = uStack_1c | 0x20000000;
    }
  }
  else {
    uStack_1c = uStack_1c | 0x10000000;
  }
  if ((in_stack_0000000c & 0xff) != 0) {
    local_84[1] = local_84[1] | 0x1000;
    uStack_3c = 0x20;
    if ((in_stack_0000000c & 0x7f) == 0) {
      uStack_38 = 0x40;
      uStack_30 = 0x10;
      iVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
      if (iVar1 == 0xf) {
LAB_004a59c7:
        uStack_2c = 0x7c00;
        uStack_28 = 0x3e0;
      }
      else {
LAB_004a59ea:
        uStack_2c = 0xf800;
        uStack_28 = 0x7e0;
      }
      uStack_24 = 0x1f;
      uStack_20 = 0;
    }
    else if ((in_stack_0000000c & 1) == 0) {
      if ((in_stack_0000000c & 2) != 0) {
        uStack_38 = 0x40;
        uStack_30 = 0x10;
        goto LAB_004a59c7;
      }
      if ((in_stack_0000000c & 4) != 0) {
        uStack_38 = 0x40;
        uStack_30 = 0x10;
        goto LAB_004a59ea;
      }
      if ((in_stack_0000000c & 8) == 0) {
        if ((in_stack_0000000c & 0x10) == 0) {
          if ((in_stack_0000000c & 0x20) == 0) {
            if ((in_stack_0000000c & 0x40) != 0) {
              uStack_38 = 0x41;
              uStack_30 = 0x20;
              uStack_2c = 0xff0000;
              uStack_28 = 0xff00;
              uStack_24 = 0xff;
              uStack_20 = 0xff000000;
            }
          }
          else {
            uStack_38 = 0x40;
            uStack_30 = 0x18;
            uStack_2c = 0xff0000;
            uStack_28 = 0xff00;
            uStack_24 = 0xff;
            uStack_20 = 0;
          }
        }
        else {
          uStack_38 = 0x41;
          uStack_30 = 0x10;
          uStack_2c = 0x7c00;
          uStack_28 = 0x3e0;
          uStack_24 = 0x1f;
          uStack_20 = 0x8000;
        }
      }
      else {
        uStack_38 = 0x41;
        uStack_30 = 0x10;
        uStack_2c = 0xf00;
        uStack_28 = 0xf0;
        uStack_24 = 0xf;
        uStack_20 = 0xf000;
      }
    }
    else {
      uStack_38 = 0x30;
      uStack_30 = 8;
      uStack_2c = 0;
      uStack_28 = 0;
      uStack_24 = 0;
      uStack_20 = 0;
    }
  }
  if ((in_stack_0000000c & 0x4000) == 0) {
    if ((in_stack_0000000c & 0xff) == 0) {
      uStack_1c = uStack_1c | 0x40;
    }
  }
  else {
    local_84[1] = local_84[1] | 0x1000;
    uStack_1c = uStack_1c | 0x20000;
    uStack_3c = 0x20;
    uStack_38 = 0x400;
    uStack_30 = DAT_00669214;
    uStack_28 = 0xffff;
  }
  if ((in_stack_0000000c & 0x400000) != 0) {
    uStack_1c = uStack_1c | 0x40000;
  }
  local_84[3] = param_2;
  if (in_stack_00000014 != 0) {
    local_84[1] = local_84[1] | 8;
  }
  iVar1 = (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,local_84,local_88,0);
  if (iVar1 != 0) {
    if (((pcStack_10 == (cls_0x5a68d8 *)0x0) && ((in_stack_0000000c & 0x200) == 0)) &&
       ((uStack_18 == 0 || ((in_stack_0000000c & 0x800) == 0)))) {
      uStack_2c = uStack_2c & 0xcfffbfff | 0x800;
      iVar1 = (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,&stack0xffffff6c,&stack0xffffff68,0);
    }
    if (iVar1 != 0) {
      if ((in_stack_0000000c & 0x800000) != 0) {
        return;
      }
      FUN_00481d10((byte *)s_Unable_to_allocate_surface_005daeb0);
    }
  }
  if (unaff_ESI != (int *)0x0) {
    if ((param_2 != 0) && ((in_stack_0000000c & 0x200000) != 0)) {
      puVar3 = (undefined4 *)&stack0xffffff6c;
      for (iVar1 = 0x1f; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      iStack_70 = param_2;
      iVar1 = (**(code **)(*unaff_ESI + 0x9c))(unaff_ESI,&stack0xffffff6c,0);
      if (iVar1 != 0) {
        FUN_004a90d0(iVar1);
      }
    }
    if (unaff_ESI != (int *)0x0) goto LAB_004a5c6a;
  }
  FUN_00481c10((byte *)s_Couldn_t_allocate_video_surface_005daee8);
LAB_004a5c6a:
  meth_0x4a5dc0(pcStack_10);
  if (in_stack_00000014 != 0) {
    pcStack_10->mbr_0x10 = in_stack_00000014;
  }
  pcStack_10->mbr_0x70 = 1;
  return;
}



// Function at 004a5ca0

void cls_0x5a68d8::meth_0x4a5ca0(int *param_1)

{
  uint uVar1;
  int *piVar2;
  int *in_stack_00000008;
  
  if (this->mbr_0x68 == 0) {
    uVar1 = param_1[4];
    if (((uint)in_stack_00000008 & 0x200000) != 0) {
      if ((uVar1 & 1) == 0) {
        if ((uVar1 & 0x30006) == 0) {
          if ((uVar1 & 8) == 0) {
            piVar2 = (int *)0x4;
            if ((uVar1 & 0x10) == 0) {
              piVar2 = in_stack_00000008;
            }
          }
          else {
            piVar2 = (int *)0x3;
          }
        }
        else {
          piVar2 = (int *)0x2;
        }
      }
      else {
        piVar2 = (int *)0x1;
      }
      if (((*param_1 * (int)piVar2 & 7U) != 0) || (((uint)param_1 & 7) != 0)) {
        FUN_00481c10((byte *)s_Can_t_create_DDSurface_with_non_p_005daf08);
        param_1 = in_stack_00000008;
      }
    }
    meth_0x4a5740(this,*param_1,param_1[1]);
    this->mbr_0x38 = in_stack_00000008[4];
    if (((uint)in_stack_00000008 & 0x200000) == 0) {
      meth_0x4bd680(this,0,0,in_stack_00000008,in_stack_00000008[5] | 0x20);
    }
  }
  return;
}



// Function at 004a5dc0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5a68d8::meth_0x4a5dc0()

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  dword *pdVar4;
  int *in_stack_00000004;
  dword local_7c [17];
  byte bStack_37;
  dword dStack_30;
  int iStack_2c;
  int iStack_20;
  uint uStack_1c;
  uint uStack_18;
  
  pdVar4 = local_7c;
  for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar4 = 0;
    pdVar4 = pdVar4 + 1;
  }
  local_7c[0] = 0x7c;
  (**(code **)(*in_stack_00000004 + 0x58))(in_stack_00000004,local_7c);
  this->mbr_0x8 = local_7c[0];
  this->mbr_0x4 = local_7c[1];
  this->mbr_0x30 = _DAT_006668d0;
  piVar1 = DAT_0066a2ec;
  this->mbr_0xc = dStack_30;
  this->mbr_0x14 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x24 = 0;
  this->mbr_0x28 = local_7c[1];
  this->mbr_0x10 = (int)local_7c[2] / ((int)(dStack_30 + ((int)dStack_30 >> 0x1f & 7U)) >> 3);
  this->mbr_0x2c = local_7c[0];
  if ((in_stack_00000004 == DAT_0066a8f8) || (in_stack_00000004 == piVar1)) {
    iStack_20 = 0;
  }
  if ((((uStack_1c & 0x40) == 0) || (in_stack_00000004 == DAT_0066a8f8)) ||
     (in_stack_00000004 == piVar1)) {
    if ((bStack_37 & 0x24) == 0) {
      if (dStack_30 == 8) {
        this->mbr_0x38 = 1;
      }
      else if (dStack_30 == 0xf) {
LAB_004a5ec0:
        this->mbr_0x38 = 2;
      }
      else if (dStack_30 == 0x10) {
        if (iStack_20 == 0xf000) {
          this->mbr_0x38 = 0x10000;
        }
        else if (((iStack_20 == 0x8000) && (in_stack_00000004 != piVar1)) &&
                (in_stack_00000004 != DAT_0066a8f8)) {
          this->mbr_0x38 = 0x20000;
        }
        else {
          if (iStack_2c != 0xf800) goto LAB_004a5ec0;
          this->mbr_0x38 = 4;
        }
      }
      else if (dStack_30 == 0x18) {
        this->mbr_0x38 = 8;
      }
      else if (dStack_30 == 0x20) {
        this->mbr_0x38 = 0x10;
      }
    }
    else if (dStack_30 == 0x10) {
      this->mbr_0x38 = 0x80000;
    }
    else {
      FUN_00481c10((byte *)s_Non_16_bit_z_buffers_not_support_005daf3c);
      uStack_1c = uStack_18;
    }
  }
  else {
    this->mbr_0x38 = *(dword *)(*(int *)(PTR_DAT_005d79e0 + 0x80) + 0x38);
  }
  this->mbr_0x64 = 0;
  this->mbr_0x68 = (dword)in_stack_00000004;
  this->mbr_0x70 = 0;
  if ((uStack_1c & 0x800) == 0) {
    if ((uStack_1c & 0x4000) == 0) goto LAB_004a5f4d;
    uVar2 = this->mbr_0x74 | 0x400;
  }
  else {
    uVar2 = this->mbr_0x74 | 0x200;
  }
  this->mbr_0x74 = uVar2;
LAB_004a5f4d:
  if ((uStack_1c & 0x1000) != 0) {
    iVar3 = (**(code **)*in_stack_00000004)(in_stack_00000004,&DAT_005a61f8,&this->mbr_0x6c);
    if (iVar3 != 0) {
      FUN_004a90d0(iVar3);
    }
    if (this->mbr_0x6c != 0) {
      this->mbr_0x74 = this->mbr_0x74 | 0x80;
    }
  }
  return;
}



// Function at 004a9b50

void cls_0x5a68d8::meth_0x4a9b50()

{
  if (this->mbr_0x88 != 0) {
    FUN_004a7430();
    this->mbr_0x88 = 0;
  }
  return;
}



// Function at 004a9c90

void cls_0x5a68d8::meth_0x4a9c90()

{
  int iVar1;
  int iVar2;
  int unaff_EBX;
  int unaff_EBP;
  uint uVar3;
  uint uVar4;
  tagRECT local_20;
  int iStack_c;
  int iStack_4;
  
  if ((int *)this->mbr_0x80 != (int *)0x0) {
    if ((this->mbr_0x84 & 1) != 0) {
      GetClientRect(DAT_0065b8ec,&local_20);
      ClientToScreen(DAT_0065b8ec,(LPPOINT)&local_20);
      uVar3 = local_20.left - DAT_00668138;
      uVar4 = local_20.top - DAT_0066813c;
      iVar1 = local_20.right + -1 + local_20.left;
      iStack_4 = local_20.bottom + -1 + local_20.top;
      iVar2 = ((int *)this->mbr_0x80)[1];
      (**(code **)(*(int *)this->mbr_0x80 + 0x40))(uVar3,uVar4);
      iVar2 = iVar2 + -1;
      uVar3 = uVar3 & ((int)uVar3 < 1) - 1;
      uVar4 = uVar4 & ((int)uVar4 < 1) - 1;
      if (iVar1 < iVar2) {
        iVar2 = iVar1;
      }
      if (unaff_EBX + -1 <= iStack_c) {
        iStack_c = unaff_EBX + -1;
      }
      (**(code **)(**(int **)(unaff_EBP + 0x80) + 0x44))
                (uVar3,uVar4,(iVar2 - uVar3) + 1,(iStack_c - uVar4) + 1);
      return;
    }
    (**(code **)(*(int *)this->mbr_0x80 + 0x24))();
  }
  return;
}



// Function at 004a9ee0

undefined4 cls_0x5a68d8::meth_0x4a9ee0()

{
  dword dVar1;
  int iVar2;
  int *piVar3;
  int iStack_8;
  
  if ((DAT_005d7a1c != 0) && (this->mbr_0x88 != 0)) {
    if (((int *)this->mbr_0x80 == (int *)0x0) ||
       ((iVar2 = (**(code **)(*(int *)this->mbr_0x80 + 0xc))(), iVar2 == 0 ||
        (iVar2 = (**(code **)(*DAT_0066a8f8 + 0x60))(DAT_0066a8f8), iVar2 == -0x7789fe3e)))) {
      return 0;
    }
    if (((this->mbr_0x84 & 3) == 0) && (DAT_0066a904 == 0)) {
      iVar2 = (**(code **)(*DAT_0066a8f8 + 0x48))(DAT_0066a8f8,2);
      while (iVar2 == -0x7789fde4) {
        iVar2 = (**(code **)(*DAT_0066a8f8 + 0x48))(DAT_0066a8f8,2);
      }
      (**(code **)(*DAT_0066a8f8 + 0x2c))(DAT_0066a8f8,0,iStack_8 != 0);
    }
    else {
      (**(code **)(*(int *)this->mbr_0x80 + 0x5c))(&stack0xffffffa8,this->mbr_0x8c,0,0);
    }
    if (((this->mbr_0x84 & 2) == 0) && (DAT_0066a904 == 0)) {
      this->mbr_0x78 = (uint)(this->mbr_0x78 == 0);
    }
    if (((DAT_006680b8 == 0) &&
        (iVar2 = (**(code **)(*(int *)this->mbr_0x80 + 0xc))(), iVar2 == DAT_0066a2ec)) ||
       ((DAT_006680b8 == 1 &&
        (piVar3 = (int *)(**(code **)(*(int *)this->mbr_0x80 + 0xc))(), piVar3 == DAT_0066a8f8)))) {
      dVar1 = this->mbr_0x80;
      this->mbr_0x80 = this->mbr_0x8c;
      this->mbr_0x8c = dVar1;
    }
  }
  return 1;
}



// Function at 004bb5c0

void cls_0x5a68d8::meth_0x4bb5c0(int param_1, int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  int *piVar8;
  uint in_stack_0000000c;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  int local_4;
  
  if ((in_stack_0000000c & 0x80) != 0) {
    if (DAT_00669324 == 0) {
      if (DAT_0066959c != 0) {
        iVar2 = 1;
        do {
          if (param_1 == iVar2) {
            iVar2 = 1;
            goto LAB_004bb624;
          }
          iVar2 = iVar2 << 1;
        } while (iVar2 != 0);
        goto LAB_004bb66c;
      }
    }
    else if ((DAT_0066959c != 0) || (param_1 != param_2)) {
      if ((DAT_0066959c != 0) && (param_1 == param_2)) {
        iVar2 = 1;
        do {
          if (param_1 == iVar2) {
            iVar2 = 1;
            goto LAB_004bb660;
          }
          iVar2 = iVar2 << 1;
        } while (iVar2 != 0);
      }
      goto LAB_004bb66c;
    }
  }
LAB_004bb727:
  local_10 = 0;
  local_c = 0;
  local_8 = param_1 + -1;
  local_4 = param_2 + -1;
  cls_0x5a6858::meth_0x4bad80((cls_0x5a6858 *)this,param_1,param_2,&local_10,1);
  return;
  while (iVar2 = iVar2 << 1, iVar2 != 0) {
LAB_004bb660:
    if (param_2 == iVar2) goto LAB_004bb727;
  }
  goto LAB_004bb66c;
  while (iVar2 = iVar2 << 1, iVar2 != 0) {
LAB_004bb624:
    if (param_2 == iVar2) goto LAB_004bb727;
  }
LAB_004bb66c:
  if (param_2 < param_1) {
    piVar3 = &param_2;
    piVar4 = &local_20;
    piVar7 = &local_18;
    piVar8 = &param_1;
    piVar6 = &local_1c;
    piVar5 = &local_14;
  }
  else {
    piVar3 = &param_1;
    piVar4 = &local_1c;
    piVar7 = &local_14;
    piVar8 = &param_2;
    piVar6 = &local_20;
    piVar5 = &local_18;
  }
  FUN_004bb760(*piVar3,piVar7,piVar4,this->mbr_0x64);
  if (DAT_00669324 == 0) {
    FUN_004bb760(*piVar8,piVar5,piVar6,this->mbr_0x64);
  }
  else {
    iVar2 = *piVar7;
    iVar1 = *piVar8;
    *piVar5 = iVar2;
    *piVar6 = (iVar2 + -1 + iVar1) / iVar2;
  }
  cls_0x5a6858::meth_0x4bb440
            ((cls_0x5a6858 *)this,param_1,param_2,local_14,local_18,local_1c,local_20);
  return;
}



// Function at 004bb870

undefined4 cls_0x5a68d8::meth_0x4bb870(int *param_1)

{
  int iVar1;
  
  iVar1 = meth_0x4bb5c0(this,*param_1,param_1[1]);
  if (iVar1 == 0) {
    return 0;
  }
  meth_0x4bd680(this,0,0,param_1,0x80000000);
  if ((this->mbr_0x64 & 0x40000000) != 0) {
    meth_0x4bd680(this,-*param_1,0,param_1,0x80000000);
    meth_0x4bd680(this,*param_1,0,param_1,0x80000000);
    meth_0x4bd680(this,0,-param_1[1],param_1,0x80000000);
    meth_0x4bd680(this,0,param_1[1],param_1,0x80000000);
  }
  return 1;
}



// Function at 004bb950

undefined4 cls_0x5a68d8::meth_0x4bb950(int param_1)

{
  cls_0x4bc850 *pcVar1;
  int iVar2;
  cls_0x5a68d8 *pcVar3;
  int *piVar4;
  cls_0x4bc850 **ppcStack00000008;
  int local_14;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e0dd;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5a6858::meth_0x4bad80
            ((cls_0x5a6858 *)this,*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
             *(undefined4 *)(param_1 + 0x6c),*(undefined4 *)(param_1 + 0x68));
  ppcStack00000008 = (cls_0x4bc850 **)this->mbr_0x70;
  local_14 = 0;
  if (0 < (int)this->mbr_0x68) {
    do {
      pcVar1 = *(cls_0x4bc850 **)(*(int *)(param_1 + 0x70) + local_14 * 4);
      iVar2 = (**(code **)(pcVar1->mbr_0x0 + 8))();
      if (iVar2 == 8) {
        if (((this->mbr_0x64 & 0x200000) != 0) &&
           (iVar2 = (**(code **)(pcVar1->mbr_0x0 + 0x34))(), iVar2 != 0)) {
          if ((*(uint *)(param_1 + 100) & 0x200) == 0) {
            pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
            uStack_4 = 2;
            if (pcVar3 != (cls_0x5a68d8 *)0x0) {
              piVar4 = (int *)(**(code **)(pcVar1->mbr_0x0 + 0x34))();
              (**(code **)(*piVar4 + 0xc))();
              cls_0x5a68d8(pcVar3);
              uStack_4 = CONCAT31(uStack_4._1_3_,3);
              pcVar3->vftptr_0x0 =
                   (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
              pcVar3->mbr_0x68 = 0;
              meth_0x4a5dc0(pcVar3);
              pcVar3->mbr_0x70 = 0;
            }
          }
          else {
            pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
            uStack_4 = 0;
            if (pcVar3 != (cls_0x5a68d8 *)0x0) {
              (**(code **)(pcVar1->mbr_0x0 + 0x34))();
              cls_0x5a68d8(pcVar3);
              uStack_4 = CONCAT31(uStack_4._1_3_,1);
              pcVar3->vftptr_0x0 =
                   (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
              meth_0x4a3a40(pcVar3);
            }
          }
          uStack_4 = 0xffffffff;
          cls_0x4bc850::meth_0x4bc850(*ppcStack00000008);
        }
        if (((this->mbr_0x64 & 0x400000) != 0) &&
           (iVar2 = (**(code **)(pcVar1->mbr_0x0 + 0x38))(), iVar2 != 0)) {
          pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
          uStack_4 = 4;
          if (pcVar3 == (cls_0x5a68d8 *)0x0) {
            pcVar3 = (cls_0x5a68d8 *)0x0;
          }
          else {
            piVar4 = (int *)(**(code **)(pcVar1->mbr_0x0 + 0x38))();
            (**(code **)(*piVar4 + 0xc))();
            cls_0x5a68d8(pcVar3);
            uStack_4 = CONCAT31(uStack_4._1_3_,5);
            pcVar3->vftptr_0x0 =
                 (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
            pcVar3->mbr_0x68 = 0;
            meth_0x4a5dc0(pcVar3);
            pcVar3->mbr_0x70 = 0;
          }
          uStack_4 = 0xffffffff;
          (*ppcStack00000008)[1].mbr_0x0 = (dword)pcVar3;
        }
      }
      else {
        if ((*(uint *)(param_1 + 100) & 0x200) == 0) {
          pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
          uStack_4 = 8;
          if (pcVar3 != (cls_0x5a68d8 *)0x0) {
            (**(code **)(pcVar1->mbr_0x0 + 0xc))();
            cls_0x5a68d8(pcVar3);
            uStack_4 = CONCAT31(uStack_4._1_3_,9);
            pcVar3->vftptr_0x0 =
                 (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
            pcVar3->mbr_0x68 = 0;
            meth_0x4a5dc0(pcVar3);
            pcVar3->mbr_0x70 = 0;
          }
        }
        else {
          pcVar3 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
          uStack_4 = 6;
          if (pcVar3 != (cls_0x5a68d8 *)0x0) {
            cls_0x5a68d8(pcVar3);
            uStack_4 = CONCAT31(uStack_4._1_3_,7);
            pcVar3->vftptr_0x0 =
                 (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
            meth_0x4a3a40(pcVar3);
          }
        }
        uStack_4 = 0xffffffff;
        if (*ppcStack00000008 != (cls_0x4bc850 *)0x0) {
          (**(code **)(*ppcStack00000008)->mbr_0x0)(1);
        }
        *ppcStack00000008 = pcVar1;
      }
      local_14 = local_14 + 1;
      ppcStack00000008 = ppcStack00000008 + 1;
    } while (local_14 < (int)this->mbr_0x68);
  }
  (*this->vftptr_0x0->virt_meth_0x4bcd00_36)(this);
  ExceptionList = pvStack_c;
  return 1;
}



// Function at 004bc7e0

void cls_0x5a68d8::meth_0x4bc7e0(int *param_1, dword param_2)

{
  dword dVar1;
  int iVar2;
  dword dVar3;
  dword in_stack_0000000c;
  dword in_stack_00000010;
  
  this->mbr_0x64 = (dword)param_1;
  if (param_1 == (int *)0x0) {
    this->mbr_0x4 = 0;
    this->mbr_0x8 = 0;
    this->mbr_0xc = 0;
    this->mbr_0x10 = 0;
    this->mbr_0x38 = 0;
  }
  else {
    dVar1 = param_1[1];
    iVar2 = *param_1;
    this->mbr_0x8 = param_1[2];
    this->mbr_0x4 = dVar1;
    dVar3 = (**(code **)(iVar2 + 0x18))();
    dVar1 = param_1[0xe];
    this->mbr_0xc = dVar3;
    dVar3 = param_1[4];
    this->mbr_0x38 = dVar1;
    this->mbr_0x10 = dVar3;
    (*this->vftptr_0x0->virt_meth_0x4bcd00_36)(this);
  }
  this->mbr_0x6c = in_stack_0000000c;
  this->mbr_0x68 = param_2;
  (*this->vftptr_0x0->virt_meth_0x4bcd00_36)(this);
  this->mbr_0x70 = in_stack_00000010;
  return;
}



// Function at 004bcb00

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

cls_0x5a68d8::cls_0x5a68d8()

{
  dword dVar1;
  
  dVar1 = _DAT_006668d0;
  this->vftptr_0x0 = &cls_0x5a68d8__vftable_5a68d8_005a68d8;
  this->mbr_0x34 = 0;
  this->mbr_0x10 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0x24 = 0;
  this->mbr_0x20 = 0;
  this->mbr_0x28 = 0;
  this->mbr_0x2c = 0;
  this->mbr_0x30 = dVar1;
  this->mbr_0x48 = 0;
  this->mbr_0x40 = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x3c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x54 = 0;
  this->mbr_0x58 = 0;
  this->mbr_0x5c = 0;
  this->mbr_0x60 = 0;
  this->mbr_0x1c = 0;
  return this;
}



// Function at 004bcd00

void cls_0x5a68d8::virt_meth_0x4bcd00()

{
  (*this->vftptr_0x0->virt_meth_0x419e60_64)(this,0);
  (*this->vftptr_0x0->virt_meth_0x419e80_68)(this,0,0,this->mbr_0x4);
  (*this->vftptr_0x0->virt_meth_0x419ea0_72)(this);
  return;
}



// Function at 004bcef0

uint cls_0x5a68d8::virt_meth_0x4bcef0()

{
  undefined uVar1;
  cls_0x5a68d8 *pcVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  int iVar3;
  uint uVar4;
  uint **ppuVar5;
  uint **ppuVar6;
  uint **in_stack_00000004;
  int *in_stack_00000008;
  int *piStack_ac;
  cls_0x5a68d8 *pcStack_a8;
  uint uStack_a4;
  dword dStack_a0;
  int iStack_9c;
  int iStack_98;
  undefined4 uStack_94;
  dword dStack_90;
  dword dStack_8c;
  dword dStack_88;
  undefined4 uStack_84;
  int *piStack_80;
  int iStack_7c;
  undefined4 uStack_78;
  int iStack_74;
  int iStack_70;
  int iStack_6c;
  int iStack_68;
  int iStack_64;
  int iStack_60;
  int iStack_5c;
  int iStack_58;
  uint *local_54 [14];
  int iStack_1c;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  
  ppuVar5 = in_stack_00000004;
  ppuVar6 = local_54;
  for (iVar3 = 0x15; iVar3 != 0; iVar3 = iVar3 + -1) {
    *ppuVar6 = *ppuVar5;
    ppuVar5 = ppuVar5 + 1;
    ppuVar6 = ppuVar6 + 1;
  }
  pcVar2 = (cls_0x5a68d8 *)(**(code **)(*(int *)PTR_DAT_005d79e0 + 0x38))();
  if ((this == pcVar2) && (DAT_006680e8 != 0)) {
    return 0;
  }
  iVar3 = cls_0x5a3e7c::meth_0x4bcd30((cls_0x5a3e7c *)this,local_54);
  if (iVar3 == 0) {
    return 0;
  }
  dStack_a0 = this->mbr_0x38;
  if (in_stack_00000008 == (int *)0x0) {
    piStack_ac = (int *)0x0;
    uStack_a4 = 0;
    piStack_80 = (int *)0x0;
    iStack_7c = 0;
    iStack_64 = 0;
    iStack_60 = 0;
    iStack_5c = 0;
    iStack_70 = 0;
    iStack_68 = 0;
    iStack_58 = 0;
    goto LAB_004bd091;
  }
  iStack_58 = in_stack_00000008[6];
  iStack_68 = *in_stack_00000008;
  iStack_70 = in_stack_00000008[1];
  piStack_ac = in_stack_00000008;
  if (iStack_68 <= iStack_1c) {
    return 0;
  }
  if (iStack_70 <= iStack_18) {
    return 0;
  }
  if (iStack_14 + iStack_1c < 0) {
    return 0;
  }
  if (iStack_10 + iStack_18 < 0) {
    return 0;
  }
  uVar4 = in_stack_00000008[4];
  piStack_80 = in_stack_00000008 + 0x12;
  if (in_stack_00000008[0xc] == 0) {
    iStack_7c = 0;
  }
  else {
    iStack_7c = in_stack_00000008[0xc] + 0x30 + (int)in_stack_00000008;
  }
  if (iStack_7c == 0) {
    uVar4 = uVar4 & 0xffffffdf;
  }
  uStack_a4 = uVar4 & 0xffffffbf;
  if (in_stack_00000008[10] == 0) {
    iStack_60 = 0;
LAB_004bd012:
    uStack_a4 = uVar4 & 0xfffffebf;
  }
  else {
    iStack_60 = in_stack_00000008[10] + 0x28 + (int)in_stack_00000008;
    if (iStack_60 == 0) goto LAB_004bd012;
  }
  if (in_stack_00000008[0x10] == 0) {
    iStack_64 = 0;
  }
  else {
    iStack_64 = in_stack_00000008[0x10] + 0x40 + (int)in_stack_00000008;
  }
  if (in_stack_00000008[8] == 0) {
    uStack_a4 = uStack_a4 & 0xffffff7f;
    iStack_5c = 0;
  }
  else {
    iStack_5c = in_stack_00000008[8] + 0x20 + (int)in_stack_00000008;
    if (iStack_5c == 0) {
      uStack_a4 = uStack_a4 & 0xffffff7f;
    }
  }
LAB_004bd091:
  uStack_78 = 0;
  pcStack_a8 = this;
  iStack_74 = iStack_68;
  iStack_6c = iStack_68;
  uVar1 = (*this->vftptr_0x0->virt_meth_0x58bd3e_44)();
  iStack_9c = CONCAT31(extraout_var,uVar1);
  if (iStack_9c == 0) {
    return 0;
  }
  if (DAT_005d7a54 != 0) {
    (*this->vftptr_0x0->virt_meth_0x58bd3e_48)();
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
  if (((CONCAT31(extraout_var_00,uVar1) == 0) || (((uint)*in_stack_00000004 & 0x800400) == 0)) ||
     ((in_stack_00000008 != (int *)0x0 &&
      (((*(byte *)(in_stack_00000008 + 4) & 0x20) == 0 &&
       (((uint)*in_stack_00000004 & 0x800000) == 0)))))) {
    iStack_98 = 0;
    dStack_a0 = dStack_a0 & 0xffffffdf;
    uStack_84 = 0;
  }
  else {
    if (((this == (cls_0x5a68d8 *)PTR_DAT_005d79e0) && (DAT_006680dc != 0)) && (DAT_006680ec == 0))
    {
      iStack_98 = 0;
    }
    else {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
      iStack_98 = (**(code **)(*(int *)CONCAT31(extraout_var_01,uVar1) + 0x2c))();
    }
    if (iStack_98 == iStack_9c) {
      iStack_98 = 0;
    }
    if (DAT_005d7a54 != 0) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
      (**(code **)(*(int *)CONCAT31(extraout_var_02,uVar1) + 0x30))();
    }
    if (iStack_98 == 0) {
      dStack_a0 = dStack_a0 & 0xffffffdf;
      uStack_84 = 0;
    }
    else {
      dStack_a0 = dStack_a0 | 0x20;
      uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
      uStack_84 = *(undefined4 *)(CONCAT31(extraout_var_03,uVar1) + 0x10);
    }
  }
  dStack_90 = this->mbr_0x4;
  dStack_8c = this->mbr_0x8;
  dStack_88 = this->mbr_0x10;
  uStack_94 = 0;
  uVar4 = FUN_004ad0d0(&piStack_ac,local_54);
  if (this->mbr_0x34 != 0) {
    (*this->vftptr_0x0->virt_meth_0x58bd3e_48)();
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
  if ((CONCAT31(extraout_var_04,uVar1) != 0) &&
     (uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this),
     *(int *)(CONCAT31(extraout_var_05,uVar1) + 0x34) != 0)) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
    (**(code **)(*(int *)CONCAT31(extraout_var_06,uVar1) + 0x30))();
  }
  return uVar4;
}



// Function at 004bd290

uint cls_0x5a68d8::virt_meth_0x4bd290(uint **param_1, int *param_2, undefined4 param_3)

{
  undefined uVar1;
  undefined3 extraout_var;
  uint uVar2;
  int iVar3;
  uint **ppuVar4;
  int *local_ac;
  cls_0x5a68d8 *local_a8;
  int local_a4;
  dword local_a0;
  int iStack_9c;
  undefined4 uStack_98;
  undefined4 uStack_94;
  dword local_90;
  dword local_8c;
  dword local_88;
  undefined4 uStack_84;
  int local_80;
  undefined4 uStack_7c;
  undefined4 uStack_78;
  int local_74;
  int local_70;
  int local_6c;
  undefined4 uStack_68;
  undefined4 uStack_64;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  int local_58;
  uint *local_54 [14];
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  ppuVar4 = local_54;
  for (iVar3 = 0x15; iVar3 != 0; iVar3 = iVar3 + -1) {
    *ppuVar4 = *param_1;
    param_1 = param_1 + 1;
    ppuVar4 = ppuVar4 + 1;
  }
  iVar3 = cls_0x5a3e7c::meth_0x4bd200((cls_0x5a3e7c *)this,local_54);
  if (iVar3 == 0) {
    return 0;
  }
  local_90 = this->mbr_0x4;
  local_8c = this->mbr_0x8;
  local_88 = this->mbr_0x10;
  local_a0 = this->mbr_0x38;
  local_a8 = this;
  if (param_2 == (int *)0x0) {
    local_ac = (int *)0x0;
    local_a4 = 0;
    local_6c = 0;
    local_70 = 0;
    local_74 = 0;
    local_80 = 0;
    local_58 = 0;
  }
  else {
    local_58 = param_2[0xc];
    local_a4 = param_2[0xe];
    local_74 = param_2[1];
    local_6c = param_2[4];
    local_70 = param_2[2];
    local_ac = param_2;
    if ((((uint)local_54[0] & 4) == 0) &&
       ((((local_74 <= local_1c || (local_70 <= local_18)) || (local_14 + local_1c < 0)) ||
        (local_10 + local_18 < 0)))) {
      return 0;
    }
    local_80 = (**(code **)(*param_2 + 0x2c))();
    if (local_80 == 0) {
      return 0;
    }
    if (DAT_005d7a54 != 0) {
      (**(code **)(*param_2 + 0x30))();
    }
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x58bd3e_44)();
  iStack_9c = CONCAT31(extraout_var,uVar1);
  if (iStack_9c == 0) {
    return 0;
  }
  if (DAT_005d7a54 != 0) {
    (*this->vftptr_0x0->virt_meth_0x58bd3e_48)();
  }
  if (iStack_9c == 0) {
    if ((param_2 != (int *)0x0) && (param_2[0xd] != 0)) {
      (**(code **)(*param_2 + 0x30))();
    }
    return 0;
  }
  local_54[0] = (uint *)((uint)local_54[0] & 0xffffc3ff);
  uStack_98 = 0;
  uStack_7c = 0;
  uStack_84 = 0;
  uStack_68 = 0;
  uStack_94 = 0;
  uStack_78 = 0;
  uStack_64 = 0;
  uStack_60 = 0;
  uStack_5c = 0;
  uVar2 = FUN_004ad0d0(&local_ac,local_54);
  if (this->mbr_0x34 != 0) {
    (*this->vftptr_0x0->virt_meth_0x58bd3e_48)();
  }
  if ((param_2 != (int *)0x0) && (param_2[0xd] != 0)) {
    (**(code **)(*param_2 + 0x30))();
  }
  return uVar2;
}



// Function at 004bd490

undefined4 cls_0x5a68d8::virt_meth_0x4bd490(uint *param_1)

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 uVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  int *in_stack_00000008;
  undefined4 in_stack_0000000c;
  undefined4 in_stack_00000010;
  
  if (in_stack_00000008 != (int *)0x0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x438d00_20)(this);
    if (((CONCAT31(extraout_var,uVar1) != 0) && (DAT_005db0b8 != 0)) &&
       ((this->mbr_0x4 != in_stack_00000008[1] || (this->mbr_0x8 != in_stack_00000008[2])))) {
      FUN_00481c10((byte *)s_Unable_to_blit_from_different_si_005defcc);
    }
    iVar2 = (**(code **)(*in_stack_00000008 + 0x54))();
    if (iVar2 != 0) {
      uVar3 = (**(code **)(*in_stack_00000008 + 0x60))
                        (param_1,this,in_stack_0000000c,in_stack_00000010);
      return uVar3;
    }
    if (((in_stack_00000008[0xe] ^ this->mbr_0x38) & 0x3001f) != 0) {
      *param_1 = *param_1 | 0x8000000;
    }
  }
  if (((*param_1 & 0x400000) == 0) &&
     (uVar1 = (*this->vftptr_0x0->virt_meth_0x4bd290_80)
                        (this,param_1,in_stack_00000008,in_stack_0000000c),
     CONCAT31(extraout_var_00,uVar1) == 0)) {
    return 0;
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
  if (CONCAT31(extraout_var_01,uVar1) == 0) {
    return 1;
  }
  if (in_stack_00000008 != (int *)0x0) {
    iVar2 = (**(code **)(*in_stack_00000008 + 0x38))();
    if (iVar2 == 0) {
      return 1;
    }
    iVar2 = (**(code **)(*in_stack_00000008 + 0x38))();
    uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
    if (CONCAT31(extraout_var_02,uVar1) == iVar2) {
      return 1;
    }
  }
  if ((*param_1 & 0x400) != 0) {
    uVar3 = 0;
    if (in_stack_00000008 != (int *)0x0) {
      uVar3 = (**(code **)(*in_stack_00000008 + 0x38))();
    }
    uVar1 = (*this->vftptr_0x0->virt_meth_0x419e40_56)(this);
    iVar2 = (**(code **)(*(int *)CONCAT31(extraout_var_03,uVar1) + 0x50))
                      (param_1,uVar3,in_stack_0000000c,in_stack_00000010);
    if (iVar2 == 0) {
      return 0;
    }
  }
  return 1;
}



// Function at 004bd5c0

void cls_0x5a68d8::virt_meth_0x4bd5c0(undefined4 param_1, int *param_2, undefined4 param_3)

{
  (**(code **)(*param_2 + 0x5c))(param_1,this,param_3);
  return;
}



// Function at 004bd5e0

void cls_0x5a68d8::meth_0x4bd5e0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7)

{
  (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
  return;
}



// Function at 004bd680

void cls_0x5a68d8::meth_0x4bd680(undefined4 param_1, undefined4 param_2, undefined4 *param_3, uint param_4)

{
  int in_stack_00000014;
  
  if (param_3 == (undefined4 *)0x0) {
    return;
  }
  if (in_stack_00000014 == 0) {
    cls_0x5a3e7c::meth_0x4384e0((cls_0x5a3e7c *)this);
  }
  else {
    cls_0x5a3e7c::meth_0x4384e0((cls_0x5a3e7c *)this);
  }
  (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
  return;
}



// Function at 004bda20

void cls_0x5a68d8::meth_0x4bda20(undefined4 param_1, undefined4 param_2, undefined4 *param_3, uint param_4)

{
  if (param_3 == (undefined4 *)0x0) {
    return;
  }
  (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
  return;
}



// Function at 004bdc50

void cls_0x5a68d8::meth_0x4bdc50(undefined4 param_1, undefined2 param_2, undefined4 param_3, undefined4 *param_4, undefined4 param_5)

{
  (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
  return;
}



// Function at 004bde60

void cls_0x5a68d8::virt_meth_0x4bde60(undefined param_1, undefined param_2, undefined param_3, undefined param_4, undefined param_5, undefined param_6, undefined4 param_7)

{
  undefined uVar1;
  undefined3 extraout_var;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x438d00_20)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    FUN_00481c10((byte *)s_Can_t_clear_a_video_surface_text_005deffc);
  }
  (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
  return;
}



// Function at 004be110

undefined4 cls_0x5a68d8::meth_0x4be110(char *param_1, undefined4 param_2, undefined4 param_3, int param_4, int param_5, int param_6, uint param_7, dword param_8, undefined4 param_9, uint param_10, int param_11)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 in_stack_00000030;
  cls_0x419dd0 cStack_20;
  uint uStack_1c;
  uint uStack_18;
  
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    iVar5 = *(int *)(DAT_0065b020 + param_5 * 4);
    if (iVar5 == 0) {
      iVar5 = DAT_0065b024;
    }
    if ((int)param_8 < 1) {
      param_8 = this->mbr_0x28;
    }
    iVar1 = *(int *)(iVar5 + 0x50);
    iVar2 = *(int *)(iVar5 + 0x54);
    if ((param_10 & 2) == 0) {
      if ((param_10 & 4) != 0) {
        param_2._0_1_ = (char)param_2 - (char)param_8;
      }
    }
    else {
      param_2._0_1_ = (char)param_2 - (char)((int)param_8 / 2);
    }
    if (param_6 == 0) {
      if (-1 < param_11) {
        param_7 = param_7 | 0x100000;
      }
    }
    else {
      param_7 = param_7 | 0x80000;
    }
    uVar3 = *(undefined4 *)(iVar5 + 0x54);
    uStack_18 = param_7;
    uStack_1c = param_10;
    *(undefined4 *)(iVar5 + 0x54) = in_stack_00000030;
    cStack_20.mbr_0x0 = param_10;
    cls_0x419dd0::cls_0x419dd0(&cStack_20);
    uVar4 = meth_0x4be2b0_CompositeBuffer(this,(char)param_2,(undefined)param_3,(char)param_8,
                          (iVar1 + iVar2) * param_4,(char)param_1,param_9);
    *(undefined4 *)(iVar5 + 0x54) = uVar3;
    return uVar4;
  }
  return 0;
}



// Function at 004be2b0

undefined4 cls_0x5a68d8::meth_0x4be2b0_CompositeBuffer(undefined param_1, undefined param_2, undefined param_3, dword param_4, undefined param_5, uint param_6)

{
  char cVar1;
  int iVar2;
  dword dVar3;
  cls_0x5a3e7c *pcVar4;
  undefined uVar5;
  undefined3 extraout_var;
  HDC pHVar6;
  undefined3 extraout_var_00;
  int iVar7;
  int *piVar8;
  cls_0x5a68d8 *pcVar9;
  undefined3 extraout_var_02;
  undefined4 uVar10;
  uint uVar11;
  LPCSTR lpchText;
  int unaff_EBX;
  int *unaff_ESI;
  char *pcVar12;
  HDC format;
  undefined4 *puVar13;
  undefined4 unaff_retaddr;
  undefined3 in_stack_00000005;
  undefined3 in_stack_00000009;
  undefined3 in_stack_0000000d;
  undefined3 in_stack_00000015;
  byte in_stack_0000001c;
  undefined in_stack_0000001d;
  undefined2 in_stack_0000001e;
  undefined in_stack_00000020;
  undefined in_stack_00000021;
  undefined2 in_stack_00000022;
  uint in_stack_00000024;
  uint in_stack_00000028;
  uint *in_stack_0000002c;
  int *piVar14;
  HDC pHStack_300;
  uint uStack_2fc;
  dword dStack_2f4;
  cls_0x5a68d8 *pcStack_2ec;
  cls_0x5a68d8 *pcStack_2e8;
  tagRECT tStack_2e4;
  cls_0x5a68d8 *pcStack_2d4;
  int iStack_2d0;
  cls_0x5a68d8 *pcStack_2cc;
  int iStack_2c8;
  uint uStack_2c4;
  undefined4 uStack_2c0;
  uint *puStack_2bc;
  code *pcStack_2b8;
  undefined4 uStack_2b4;
  undefined *puStack_2b0;
  undefined4 uStack_2ac;
  undefined4 uStack_2a8;
  undefined4 uStack_2a4;
  undefined4 uStack_2a0;
  undefined4 uStack_29c;
  undefined4 uStack_298;
  dword dStack_294;
  cls_0x5a3e7c *pcStack_290;
  dword dStack_28c;
  char *pcStack_288;
  dword dStack_284;
  cls_0x5a3e7c *pcStack_280;
  dword dStack_27c;
  char *pcStack_278;
  undefined4 uStack_274;
  int iStack_270;
  int *piStack_26c;
  int iStack_268;
  int iStack_264;
  dword dStack_260;
  undefined4 uStack_25c;
  undefined4 uStack_258;
  undefined4 uStack_254;
  undefined4 uStack_250;
  undefined4 uStack_24c;
  undefined4 uStack_248;
  undefined4 uStack_244;
  undefined4 uStack_240;
  undefined4 uStack_23c;
  undefined4 uStack_238;
  undefined4 uStack_234;
  undefined4 uStack_230;
  dword dStack_22c;
  cls_0x5a3e7c *pcStack_228;
  undefined4 uStack_224;
  undefined4 uStack_220;
  dword dStack_21c;
  cls_0x5a3e7c *pcStack_218;
  undefined4 uStack_214;
  undefined4 uStack_210;
  undefined2 uStack_20c;
  undefined2 uStack_20a;
  int *piStack_208;
  uint uStack_204;
  int *piStack_200;
  tagPOINT atStack_1fc [2];
  undefined auStack_1ec [4];
  uint uStack_1e8;
  uint uStack_1e4;
  int iStack_1e0;
  int iStack_1dc;
  dword dStack_1d8;
  int iStack_1d4;
  int iStack_1cc;
  dword dStack_1c8;
  undefined4 uStack_1c4;
  undefined4 uStack_1c0;
  undefined4 uStack_1bc;
  undefined auStack_1b8 [8];
  undefined4 auStack_1b0 [7];
  dword dStack_194;
  dword dStack_190;
  dword dStack_18c;
  undefined4 uStack_178;
  undefined4 uStack_174;
  undefined4 uStack_170;
  uint *apuStack_158 [2];
  int aiStack_150 [80];
  void *pvStack_10;
  void *local_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  undefined3 extraout_var_01;
  
  uStack_4 = 0xffffffff;
  uStack_8 = &LAB_0059e17c;
  local_c = ExceptionList;
  if ((_param_5 == (char *)0x0) || (*_param_5 == '\0')) {
    ExceptionList = pvStack_10;
    return 1;
  }
  iVar7 = CONCAT22(in_stack_0000001e,CONCAT11(in_stack_0000001d,in_stack_0000001c));
  format = *(HDC *)(DAT_0065b020 + iVar7 * 4);
  if (format == (HDC)0x0) {
    format = DAT_0065b024;
  }
  ExceptionList = &local_c;
  uVar5 = (*this->vftptr_0x0->virt_meth_0x438d00_20)(this);
  if (CONCAT31(extraout_var,uVar5) != 0) {
    FUN_00481c10((byte *)s_Can_t_draw_text_in_video_surface_005df09c);
  }
  if (this == (cls_0x5a68d8 *)PTR_DAT_005d79e0) {
    FUN_004aadd0();
  }
  pHVar6 = *(HDC *)(DAT_0065b020 + iVar7 * 4);
  if (pHVar6 == (HDC)0x0) {
    pHVar6 = DAT_0065b024;
  }
  if (pHVar6[8].unused == 2) {
    dStack_2f4 = _param_2;
    iStack_268 = 0;
    iStack_2c8 = 0;
    if (((this->mbr_0x5c != 0) ||
        (uVar5 = (*this->vftptr_0x0->virt_meth_0x438ce0_12)(this),
        CONCAT31(extraout_var_00,uVar5) == 0)) ||
       (pcVar9 = this, pcStack_2e8 = this,
       ((this->mbr_0x38 ^ *(uint *)(PTR_DAT_005d79e0 + 0x38)) & 0x3001f) != 0)) {
      pcVar9 = (cls_0x5a68d8 *)this->mbr_0x5c;
      if (((pcVar9 == (cls_0x5a68d8 *)0x0) || ((int)pcVar9->mbr_0x4 < (int)param_4)) ||
         ((int)pcVar9->mbr_0x8 < (int)_param_5)) {
        pcVar9 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
        if (pcVar9 == (cls_0x5a68d8 *)0x0) {
          pcStack_2e8 = (cls_0x5a68d8 *)0x0;
          pcStack_2cc = pcVar9;
        }
        else {
          pcStack_2cc = pcVar9;
          cls_0x5a68d8(pcVar9);
          pcVar9->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
          pcVar9->mbr_0x68 = 0;
          meth_0x4a5740(pcVar9,param_4,_param_5);
          pcVar9->mbr_0x70 = 1;
          pcStack_2e8 = pcVar9;
        }
        unaff_retaddr = 0xffffffff;
        (*pcStack_2e8->vftptr_0x0->virt_meth_0x4bde60_100)
                  (pcStack_2e8,0,0,(char)pcStack_2e8->mbr_0x4,(char)pcStack_2e8->mbr_0x8,
                   (char)pcStack_2e8->mbr_0x30,0xff,0x7f7f);
        pcStack_2cc = (cls_0x5a68d8 *)0x1;
        dStack_2f4 = 0;
        pcVar9 = pcStack_2ec;
      }
      else {
        (*pcVar9->vftptr_0x0->virt_meth_0x4bde60_100)
                  (pcVar9,0,0,(char)param_4,param_5,(char)pcVar9->mbr_0x30,0xff,0x7f7f);
        dStack_2f4 = 0;
        pcStack_2e8 = pcVar9;
      }
    }
    puStack_2bc = in_stack_0000002c;
    pcStack_290 = _param_3;
    pcStack_2b8 = (code *)0x0;
    uStack_2b4 = 0;
    puStack_2b0 = (undefined *)0x0;
    dStack_294 = dStack_2f4;
    dStack_28c = param_4;
    pcStack_288 = _param_5;
    dStack_284 = 0;
    pcStack_280 = (cls_0x5a3e7c *)0x0;
    dStack_27c = param_4;
    pcStack_278 = _param_5;
    piStack_26c = (int *)0x0;
    uStack_2a8 = 0;
    uStack_2ac = 0;
    uStack_298 = 0;
    uStack_29c = 0;
    uStack_2a0 = 0;
    uStack_2a4 = 0;
    iStack_270 = 0x1f;
    uStack_274 = 0;
    iVar7 = cls_0x5a3e7c::meth_0x4bd200((cls_0x5a3e7c *)pcVar9,&puStack_2bc);
    if (iVar7 == 0) {
      ExceptionList = pvStack_10;
      return 0;
    }
    puVar13 = auStack_1b0;
    for (iVar7 = 0x16; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar13 = 0;
      puVar13 = puVar13 + 1;
    }
    dStack_194 = pcStack_2e8->mbr_0x4;
    dStack_190 = pcStack_2e8->mbr_0x8;
    dStack_18c = pcStack_2e8->mbr_0x10;
    uStack_170 = 0;
    uStack_174 = 0;
    uStack_178 = 0;
    iVar7 = FUN_004ad7c0(auStack_1b0,&puStack_2bc,apuStack_158,&tStack_2e4.top,unaff_ESI);
    if (iVar7 == 0) {
      ExceptionList = pvStack_10;
      return 0;
    }
    uStack_2fc = 0x2810;
    if ((in_stack_00000028 & 1) == 0) {
      if ((in_stack_00000028 & 4) == 0) {
        if ((in_stack_00000028 & 2) != 0) {
          uStack_2fc = 0x2811;
        }
      }
      else {
        uStack_2fc = 0x2812;
      }
    }
    if ((in_stack_00000028 & 0x10) == 0) {
      if ((in_stack_00000028 & 0x20) == 0) {
        if ((in_stack_00000028 & 0x40) != 0) {
          uStack_2fc = uStack_2fc & 0xffffdfef | 0x24;
        }
      }
      else {
        uStack_2fc = uStack_2fc | 0x28;
      }
    }
    else {
      uStack_2fc = uStack_2fc | 0x20;
    }
    uVar11 = in_stack_00000028 & 0x80;
    if (uVar11 != 0) {
      uStack_2fc = uStack_2fc & 0xffffdfef | 0x20;
    }
    if ((in_stack_00000028 & 0x100) != 0) {
      uStack_2fc = uStack_2fc | 0x400;
    }
    dStack_260 = 0;
    if ((uVar11 != 0) && ((in_stack_00000028 & 0x270) == 0)) {
      dStack_260 = pHStack_300[0x17].unused;
    }
    pcStack_2ec = (cls_0x5a68d8 *)param_6;
    if (uVar11 == 0) {
      iVar7 = FUN_004aceb0((cls_0x4acb80 *)&UNK_0065abf8.field_0x418,
                           CONCAT22(in_stack_00000022,CONCAT11(in_stack_00000021,in_stack_00000020))
                           ,param_6,param_4,in_stack_00000028,
                           CONCAT22(in_stack_0000001e,CONCAT11(in_stack_0000001d,in_stack_0000001c))
                          );
      pcStack_2ec = (cls_0x5a68d8 *)(iVar7 + param_6);
    }
    uVar11 = 0xffffffff;
    pcVar12 = (char *)pcStack_2ec;
    do {
      if (uVar11 == 0) break;
      uVar11 = uVar11 - 1;
      cVar1 = *pcVar12;
      pcVar12 = pcVar12 + 1;
    } while (cVar1 != '\0');
    lpchText = (LPCSTR)(~uVar11 - 1);
    if (in_stack_00000024 == 0xffffffff) {
      if ((in_stack_00000028 & 0x800) == 0) {
        if ((in_stack_00000028 & 0x1000) == 0) {
          in_stack_00000024 = pHStack_300[0x10].unused;
        }
        else {
          in_stack_00000024 = pHStack_300[0x12].unused;
        }
      }
      else {
        in_stack_00000024 = pHStack_300[0x11].unused;
      }
    }
    uVar5 = (*pcStack_2e8->vftptr_0x0->virt_meth_0x438ce0_12)(pcStack_2e8);
    piVar8 = (int *)CONCAT31(extraout_var_01,uVar5);
    if (piVar8 == (int *)0x0) {
      ExceptionList = pvStack_10;
      return 0;
    }
    tStack_2e4.left = 1;
    iVar7 = pHStack_300[0x14].unused;
    if (0xe < iVar7) {
      tStack_2e4.left = 2;
    }
    uStack_2c0 = 1;
    if (0x19 < iVar7) {
      tStack_2e4.left = 2;
      uStack_2c0 = 2;
    }
    uStack_2c4 = (uint)(0x19 >= iVar7);
    piVar14 = piVar8;
    piStack_200 = piVar8;
    iVar7 = (**(code **)(*piVar8 + 0x44))(piVar8);
    if (iVar7 != 0) {
      FUN_004a90d0(iVar7);
    }
    pcStack_2d4 = (cls_0x5a68d8 *)SelectObject(pHStack_300,*(HGDIOBJ *)(unaff_EBX + 0x38));
    iStack_264 = 0;
    if (0 < (int)pcStack_2e8) {
      piStack_200 = (int *)(CONCAT11(in_stack_00000021,in_stack_00000020) & 0x400);
      uStack_204 = ((uint)in_stack_0000001c << 8 |
                   CONCAT21(in_stack_0000001e,in_stack_0000001d) & 0xff) << 8 |
                   CONCAT12(in_stack_00000020,in_stack_0000001e) & 0xff;
      piStack_26c = aiStack_150;
      do {
        pcVar4 = pcStack_290;
        dVar3 = dStack_294;
        iVar7 = piStack_26c[1];
        pcVar12 = (char *)(piStack_26c[6] + *piStack_26c);
        iVar2 = piStack_26c[7];
        SetTextColor(pHStack_300,0);
        SetBkMode(pHStack_300,1);
        if (piStack_200 != (int *)0x0) {
          tStack_2e4.left = (LONG)(pcVar12 + (int)pcStack_2ec);
          tStack_2e4.top = iStack_2c8 + ((iVar2 + iVar7) - iStack_268);
          tStack_2e4.right = tStack_2e4.left + dVar3;
          tStack_2e4.bottom = tStack_2e4.top + (int)pcVar4;
          dStack_260 = tStack_2e4.top;
          DrawTextA(pHStack_300,lpchText,uStack_2fc,&tStack_2e4,(UINT)format);
          if (pcStack_2cc != (cls_0x5a68d8 *)0x0) {
            tStack_2e4.left = (LONG)(pcVar12 + 1 + (int)pcStack_2ec);
            tStack_2e4.top = dStack_260;
            tStack_2e4.right = tStack_2e4.left + dVar3;
            tStack_2e4.bottom = (int)&(pcVar4->cls_0x5a68d8).vftptr_0x0 + dStack_260;
            DrawTextA(pHStack_300,lpchText,uStack_2fc,&tStack_2e4,(UINT)format);
            tStack_2e4.left = (int)pcStack_2ec + (int)pcVar12;
            tStack_2e4.right = tStack_2e4.left + dVar3;
            tStack_2e4.top = dStack_260 + 1;
            tStack_2e4.bottom = (int)&(pcVar4->cls_0x5a68d8).vftptr_0x0 + tStack_2e4.top;
            DrawTextA(pHStack_300,lpchText,uStack_2fc,&tStack_2e4,(UINT)format);
          }
        }
        SetTextColor(pHStack_300,uStack_204);
        tStack_2e4.top = (iVar2 + iVar7) - iStack_268;
        tStack_2e4.right = (LONG)(pcVar12 + dVar3);
        tStack_2e4.bottom = tStack_2e4.top + (int)pcVar4;
        tStack_2e4.left = (LONG)pcVar12;
        DrawTextA(pHStack_300,lpchText,uStack_2fc,&tStack_2e4,(UINT)format);
        this->mbr_0x3c = tStack_2e4.left;
        this->mbr_0x40 = tStack_2e4.top;
        this->mbr_0x44 = (dword)(tStack_2e4.right + -1);
        this->mbr_0x48 = tStack_2e4.bottom - 1;
        GetCurrentPositionEx(pHStack_300,atStack_1fc);
        this->mbr_0x4c = atStack_1fc[0].x;
        this->mbr_0x50 = atStack_1fc[0].y;
        iStack_264 = iStack_264 + 1;
        piStack_26c = piStack_26c + 0x15;
        param_4 = _param_2;
        piVar8 = piStack_208;
      } while (iStack_264 < (int)pcStack_2e8);
    }
    SelectObject(pHStack_300,pcStack_2d4);
    iVar7 = (**(code **)(*piVar8 + 0x68))(piVar8,pHStack_300);
    if (iVar7 != 0) {
      FUN_004a90d0(iVar7);
    }
    if (iStack_270 == 0) {
      if (iStack_2d0 != 0) {
        pcVar9 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
        uStack_8 = (undefined *)0x2;
        pcStack_2d4 = pcVar9;
        if (pcVar9 == (cls_0x5a68d8 *)0x0) {
          pcVar9 = (cls_0x5a68d8 *)0x0;
        }
        else {
          cls_0x5a68d8(pcVar9);
          uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,3);
          pcVar9->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
          pcVar9->mbr_0x68 = 0;
          meth_0x4a5740(pcVar9,param_4,_param_3);
          pcVar9->mbr_0x70 = 1;
        }
        pcStack_228 = _param_3;
        pcStack_218 = _param_3;
        uStack_8 = (undefined *)0xffffffff;
        uStack_25c = 0x100;
        uStack_258 = 0;
        uStack_254 = 0;
        uStack_250 = 0;
        uStack_230 = _param_1;
        uStack_224 = 0;
        uStack_220 = 0;
        uStack_20a = 0;
        uStack_20c = 0;
        uStack_248 = 0;
        uStack_24c = 0;
        uStack_238 = 0;
        uStack_23c = 0;
        uStack_240 = 0;
        uStack_244 = 0;
        uStack_210 = 0x1f;
        uStack_214 = 0;
        uStack_234 = unaff_retaddr;
        dStack_22c = param_4;
        dStack_21c = param_4;
        (*this->vftptr_0x0->virt_meth_0x4bd490_92)(this,&uStack_25c);
        if (pcVar9 != (cls_0x5a68d8 *)0x0) {
          piVar14 = (int *)0x1;
          (*pcVar9->vftptr_0x0->~cls_0x5a68d8_0)(pcVar9);
        }
      }
    }
    else {
      uStack_230 = _param_1;
      pcStack_228 = _param_3;
      pcStack_218 = _param_3;
      uStack_25c = 0x100;
      uStack_258 = 0;
      uStack_254 = 0;
      uStack_250 = 0;
      uStack_224 = 0;
      uStack_220 = 0;
      uStack_20a = 0;
      uStack_20c = 0;
      uStack_248 = 0;
      uStack_24c = 0;
      uStack_238 = 0;
      uStack_23c = 0;
      uStack_240 = 0;
      uStack_244 = 0;
      uStack_210 = 0x1f;
      uStack_214 = 0;
      uStack_234 = unaff_retaddr;
      dStack_22c = param_4;
      dStack_21c = param_4;
      (*this->vftptr_0x0->virt_meth_0x4bd490_92)(this,&uStack_25c);
    }
    uVar5 = (*this->vftptr_0x0->virt_meth_0x4bcb90_76)(this);
    if ((code *)CONCAT31(extraout_var_02,uVar5) != (code *)0x0) {
      if ((iStack_270 != 0) || (iStack_2d0 != 0)) {
        uStack_2c4 = in_stack_00000024;
        uStack_298 = _param_1;
        uStack_2c0 = 0;
        puStack_2bc = (uint *)0x0;
        pcStack_2b8 = (code *)0x0;
        pcStack_290 = _param_3;
        dStack_28c = 0;
        pcStack_288 = (char *)0x0;
        pcStack_280 = _param_3;
        uStack_274 = 0;
        puStack_2b0 = (undefined *)0x0;
        uStack_2b4 = 0;
        uStack_2a0 = 0;
        uStack_2a4 = 0;
        uStack_2a8 = 0;
        uStack_2ac = 0;
        pcStack_278 = (char *)0x1f;
        dStack_27c = 0;
        uStack_29c = unaff_retaddr;
        dStack_294 = param_4;
        dStack_284 = param_4;
        cls_0x5a3e7c::meth_0x4bd200(_param_3,&uStack_2c4);
      }
      (*(code *)CONCAT31(extraout_var_02,uVar5))(auStack_1b8,&uStack_2c4,piVar14);
    }
    uVar10 = cls_0x4acb80::meth_0x4acb80
                       ((cls_0x4acb80 *)&UNK_0065abf8.field_0x418,param_6,lpchText,param_4,
                        CONCAT22(in_stack_00000022,CONCAT11(in_stack_00000021,in_stack_00000020)),
                        _param_5,10000);
  }
  else {
    pcStack_278 = (char *)this->mbr_0x8;
    dStack_27c = this->mbr_0x4;
    uStack_2b4 = 0;
    dStack_284 = 0;
    pcStack_280 = (cls_0x5a3e7c *)0x0;
    piStack_26c = (int *)0x0;
    uStack_2a8 = 0;
    uStack_2ac = 0;
    uStack_298 = 0;
    uStack_29c = 0;
    uStack_2a0 = 0;
    uStack_2a4 = 0;
    iStack_270 = 0x1f;
    uStack_274 = 0;
    if (((uint)in_stack_0000002c & 0x80000000) == 0) {
      puStack_2bc = in_stack_0000002c;
    }
    else {
      puStack_2bc = (uint *)(format[0xf].unused | (uint)in_stack_0000002c & 0x7fffffff);
    }
    dStack_294 = _param_2;
    pcStack_290 = _param_3;
    puStack_2b0 = auStack_1ec;
    dStack_28c = param_4;
    pcStack_288 = _param_5;
    pcStack_2b8 = FUN_004b9ca0;
    tStack_2e4.top = format[0x14].unused;
    if ((in_stack_00000028 & 0x80) == 0) {
      tStack_2e4.top = tStack_2e4.top + format[0x15].unused;
    }
    iStack_1dc = format[0x17].unused;
    iStack_1d4 = format[0x16].unused;
    uStack_1e4 = param_6;
    iStack_1cc = (int)_param_5 / tStack_2e4.top;
    uStack_1e8 = (uint)in_stack_0000002c & 1;
    dStack_1c8 = param_4;
    puStack_2bc = (uint *)((uint)puStack_2bc | 1);
    uStack_1c4 = 0;
    uStack_1c0 = 0;
    uStack_1bc = 0;
    if (((((in_stack_00000028 & 0x400) == 0) || (iStack_1e0 = format[0xb].unused, iStack_1e0 == 0))
        && (((in_stack_00000028 & 0x800) == 0 || (iStack_1e0 = format[0xc].unused, iStack_1e0 == 0))
           )) && (((in_stack_00000028 & 0x1000) == 0 ||
                  (iStack_1e0 = format[0xd].unused, iStack_1e0 == 0)))) {
      iStack_1e0 = format[10].unused;
    }
    if (iStack_1e0 == 0) {
      iStack_1e0 = format[10].unused;
    }
    pcStack_2cc = (cls_0x5a68d8 *)&stack0xfffffcec;
    dStack_1d8 = tStack_2e4.top;
    uStack_274 = cls_0x5a3e7c::meth_0x4384e0((cls_0x5a3e7c *)this);
    (*this->vftptr_0x0->virt_meth_0x4bcef0_88)(this);
    uVar10 = uStack_1c4;
  }
  if (this == (cls_0x5a68d8 *)PTR_DAT_005d79e0) {
    FUN_004aade0();
  }
  ExceptionList = pvStack_10;
  return uVar10;
}



// Function at 004bf050

cls_0x5a68d8 * __thiscall cls_0x5a68d8::~cls_0x5a68d8(cls_0x5a68d8 *this)

{
  undefined4 *puVar1;
  byte in_stack_00000004;
  
  puVar1 = (undefined4 *)this->mbr_0x54;
  this->vftptr_0x0 = &cls_0x5a68d8__vftable_5a68d8_005a68d8;
  if (((puVar1 != (undefined4 *)0x0) && (this->mbr_0x58 != 0)) && (puVar1 != (undefined4 *)0x0)) {
    (**(code **)*puVar1)(1);
  }
  puVar1 = (undefined4 *)this->mbr_0x5c;
  if (((puVar1 != (undefined4 *)0x0) && (this->mbr_0x60 != 0)) && (puVar1 != (undefined4 *)0x0)) {
    (**(code **)*puVar1)(1);
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0e30

TScreen * cls_0x5a68d8::virt_meth_0x4c0e30(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e25b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xdc);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0e10_005a6944;
    this_00->mbr_0xd8 = 0xffffffff;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



