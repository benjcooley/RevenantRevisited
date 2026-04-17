#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a6858



// Function at 004a5fe0

void cls_0x5a6858::meth_0x4a5fe0()

{
  int *piVar1;
  int iVar2;
  
  if (((this->cls_0x5a68d8).mbr_0x70 != 0) && ((this->cls_0x5a68d8).mbr_0x68 != 0)) {
    if ((this->cls_0x5a68d8).mbr_0x6c != 0) {
      FUN_004172b0(0,0,(int *)0x0);
      FUN_004172b0(1,0,(int *)0x0);
      piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
      iVar2 = (**(code **)(*piVar1 + 4))(piVar1);
      while (iVar2 = iVar2 + -1, iVar2 != 0) {
        piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
        (**(code **)(*piVar1 + 8))(piVar1);
      }
      (this->cls_0x5a68d8).mbr_0x6c = 0;
    }
    piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
    for (iVar2 = (**(code **)(*piVar1 + 4))(piVar1); iVar2 != 0; iVar2 = iVar2 + -1) {
      piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
      (**(code **)(*piVar1 + 8))(piVar1);
    }
    (this->cls_0x5a68d8).mbr_0x68 = 0;
  }
  return;
}



// Function at 004abaa0

void cls_0x5a6858::virt_meth_0x4abaa0()

{
  dword dVar1;
  
  dVar1 = (**(code **)(*(int *)(this->cls_0x5a68d8).mbr_0x64 + 0x2c))();
  (this->cls_0x5a68d8).mbr_0x34 = dVar1;
  return;
}



// Function at 004abae0

cls_0x5a6858 * cls_0x5a6858::virt_meth_0x4abae0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a6858(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004bad80

undefined4 cls_0x5a6858::meth_0x4bad80(dword param_1, dword param_2, undefined4 *param_3, cls_0x5a68d8 *param_4)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 *puVar8;
  cls_0x5a68d8 *this_00;
  cls_0x5a68d8 *this_01;
  dword dVar9;
  int *piVar10;
  uint uVar11;
  int iVar12;
  dword dVar13;
  undefined4 *puVar14;
  uint in_stack_00000014;
  cls_0x5a68d8 **local_18;
  int local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e074;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a68d8).mbr_0x64 = in_stack_00000014;
  (this->cls_0x5a68d8).mbr_0x68 = (dword)param_4;
  if ((in_stack_00000014 & 0x200000) != 0) {
    (this->cls_0x5a68d8).mbr_0x64 = in_stack_00000014 & 0xffffe19a;
  }
  uVar11 = (this->cls_0x5a68d8).mbr_0x64;
  if ((uVar11 & 0x400000) != 0) {
    (this->cls_0x5a68d8).mbr_0x64 = uVar11 & 0xfff87fff;
  }
  uVar11 = (this->cls_0x5a68d8).mbr_0x64;
  if ((uVar11 & 0x200e00) == 0) {
    uVar6 = 0;
  }
  else if (((uVar11 & 0x8000) == 0) || ((uVar11 & 0x30000) != 0)) {
    (this->cls_0x5a68d8).mbr_0xc = 0x10;
    if ((uVar11 & 1) == 0) {
      if ((uVar11 & 0x1e) == 0) {
        if ((uVar11 & 0x20) == 0) {
          if ((uVar11 & 0x40) != 0) {
            (this->cls_0x5a68d8).mbr_0xc = 0x20;
          }
        }
        else {
          (this->cls_0x5a68d8).mbr_0xc = 0x18;
        }
      }
      else {
        (this->cls_0x5a68d8).mbr_0xc = 0x10;
      }
    }
    else {
      (this->cls_0x5a68d8).mbr_0xc = 8;
    }
    dVar9 = (this->cls_0x5a68d8).mbr_0xc;
    iVar7 = FUN_004a80b0();
    dVar13 = (this->cls_0x5a68d8).mbr_0x68;
    iVar12 = 0;
    if (0 < (int)dVar13) {
      piVar10 = param_3 + 1;
      do {
        piVar1 = piVar10 + 1;
        piVar2 = piVar10 + -1;
        piVar3 = piVar10 + 2;
        iVar4 = *piVar10;
        piVar10 = piVar10 + 4;
        iVar12 = iVar12 + ((*piVar1 - *piVar2) + 1) * ((*piVar3 - iVar4) + 1);
        dVar13 = dVar13 - 1;
      } while (dVar13 != 0);
    }
    uVar11 = (this->cls_0x5a68d8).mbr_0x64;
    if (((uVar11 & 0x800) != 0) && (iVar7 = iVar7 - iVar12 * ((int)dVar9 >> 3), iVar7 < 0x80000)) {
      if ((uVar11 & 0x1000) != 0) {
        ExceptionList = local_c;
        return 0;
      }
      (this->cls_0x5a68d8).mbr_0x64 = uVar11 & 0xfffff7ff | 0x400;
    }
    uVar11 = (this->cls_0x5a68d8).mbr_0x64;
    if ((((uVar11 & 0x8000) != 0) && ((uVar11 & 0x20000) != 0)) && (iVar7 + iVar12 * -2 < 0x80000))
    {
      if ((uVar11 & 0x40000) != 0) {
        ExceptionList = local_c;
        return 0;
      }
      (this->cls_0x5a68d8).mbr_0x64 = uVar11 & 0xfffdffff | 0x10000;
    }
    puVar8 = FUN_00482fb0((int)param_4 << 4);
    if (puVar8 == (undefined4 *)0x0) {
      puVar8 = (undefined4 *)0x0;
    }
    (this->cls_0x5a68d8).mbr_0x6c = (dword)puVar8;
    puVar14 = param_3;
    for (uVar11 = (uint)((int)param_4 << 4) >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar8 = *puVar14;
      puVar14 = puVar14 + 1;
      puVar8 = puVar8 + 1;
    }
    for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined *)puVar8 = *(undefined *)puVar14;
      puVar14 = (undefined4 *)((int)puVar14 + 1);
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    local_18 = (cls_0x5a68d8 **)FUN_00482fb0((this->cls_0x5a68d8).mbr_0x68 << 2);
    (this->cls_0x5a68d8).mbr_0x70 = (dword)local_18;
    local_10 = 0;
    if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
      piVar10 = param_3 + 1;
      do {
        uVar11 = (this->cls_0x5a68d8).mbr_0x64;
        param_4 = (cls_0x5a68d8 *)0x0;
        if ((uVar11 & 0x200000) == 0) {
          if ((uVar11 & 0x200) == 0) {
            param_4 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
            local_4 = 2;
            if (param_4 == (cls_0x5a68d8 *)0x0) {
              param_4 = (cls_0x5a68d8 *)0x0;
            }
            else {
              iVar7 = piVar10[2];
              iVar12 = *piVar10;
              iVar4 = piVar10[1];
              iVar5 = piVar10[-1];
              cls_0x5a68d8::cls_0x5a68d8(param_4);
              local_4 = CONCAT31(local_4._1_3_,3);
              param_4->vftptr_0x0 =
                   (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
              param_4->mbr_0x68 = 0;
              cls_0x5a68d8::meth_0x4a5740(param_4,(iVar4 - iVar5) + 1,(iVar7 - iVar12) + 1);
              param_4->mbr_0x70 = 1;
            }
            local_4 = 0xffffffff;
            if ((param_4->mbr_0x10 != (piVar10[1] - piVar10[-1]) + 1U) && (DAT_006680c4 != 0)) {
              if (param_4 != (cls_0x5a68d8 *)0x0) {
                (*param_4->vftptr_0x0->~cls_0x5a68d8_0)(param_4);
              }
              param_4 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
              local_4 = 4;
              if (param_4 == (cls_0x5a68d8 *)0x0) {
                param_4 = (cls_0x5a68d8 *)0x0;
              }
              else {
                iVar7 = piVar10[2];
                iVar12 = *piVar10;
                iVar4 = piVar10[1];
                iVar5 = piVar10[-1];
                cls_0x5a68d8::cls_0x5a68d8(param_4);
                local_4 = CONCAT31(local_4._1_3_,5);
                param_4->vftptr_0x0 =
                     (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
                param_4->mbr_0x68 = 0;
                cls_0x5a68d8::meth_0x4a5740(param_4,(iVar4 - iVar5) + 1,(iVar7 - iVar12) + 1);
                param_4->mbr_0x70 = 1;
              }
              local_4 = 0xffffffff;
            }
          }
          else {
            param_4 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
            local_4 = 0;
            if (param_4 == (cls_0x5a68d8 *)0x0) {
              param_4 = (cls_0x5a68d8 *)0x0;
            }
            else {
              iVar7 = piVar10[2];
              iVar12 = *piVar10;
              iVar4 = piVar10[-1];
              iVar5 = piVar10[1];
              cls_0x5a68d8::cls_0x5a68d8(param_4);
              local_4 = CONCAT31(local_4._1_3_,1);
              param_4->vftptr_0x0 =
                   (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
              cls_0x5a68d8::meth_0x4a39a0(param_4,(iVar5 - iVar4) + 1,(iVar7 - iVar12) + 1);
            }
            local_4 = 0xffffffff;
          }
        }
        this_00 = (cls_0x5a68d8 *)0x0;
        if (((this->cls_0x5a68d8).mbr_0x64 & 0x8000) == 0) {
LAB_004bb361:
          if (((this->cls_0x5a68d8).mbr_0x64 & 0x800000) != 0) goto LAB_004bb376;
          *local_18 = param_4;
        }
        else {
          this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
          local_4 = 6;
          if (this_00 == (cls_0x5a68d8 *)0x0) {
            this_00 = (cls_0x5a68d8 *)0x0;
          }
          else {
            iVar7 = piVar10[2];
            iVar12 = *piVar10;
            iVar4 = piVar10[-1];
            iVar5 = piVar10[1];
            cls_0x5a68d8::cls_0x5a68d8(this_00);
            local_4 = CONCAT31(local_4._1_3_,7);
            this_00->vftptr_0x0 =
                 (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
            this_00->mbr_0x68 = 0;
            cls_0x5a68d8::meth_0x4a5740(this_00,(iVar5 - iVar4) + 1,(iVar7 - iVar12) + 1);
            this_00->mbr_0x70 = 1;
          }
          local_4 = 0xffffffff;
          if ((this_00->mbr_0x10 != (piVar10[1] - piVar10[-1]) + 1U) && (DAT_006680c4 != 0)) {
            if (this_00 != (cls_0x5a68d8 *)0x0) {
              (*this_00->vftptr_0x0->~cls_0x5a68d8_0)(this_00);
            }
            this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
            local_4 = 8;
            if (this_00 == (cls_0x5a68d8 *)0x0) {
              this_00 = (cls_0x5a68d8 *)0x0;
            }
            else {
              iVar7 = piVar10[2];
              iVar12 = *piVar10;
              iVar4 = piVar10[1];
              iVar5 = piVar10[-1];
              cls_0x5a68d8::cls_0x5a68d8(this_00);
              local_4 = CONCAT31(local_4._1_3_,9);
              this_00->vftptr_0x0 =
                   (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
              this_00->mbr_0x68 = 0;
              cls_0x5a68d8::meth_0x4a5740(this_00,(iVar4 - iVar5) + 1,(iVar7 - iVar12) + 1);
              this_00->mbr_0x70 = 1;
            }
          }
          local_4 = 0xffffffff;
          if (this_00 == (cls_0x5a68d8 *)0x0) goto LAB_004bb361;
LAB_004bb376:
          this_01 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
          local_4 = 10;
          if (this_01 == (cls_0x5a68d8 *)0x0) {
            this_01 = (cls_0x5a68d8 *)0x0;
          }
          else {
            cls_0x5a68d8::cls_0x5a68d8(this_01);
            local_4 = CONCAT31(local_4._1_3_,0xb);
            this_01->vftptr_0x0 =
                 (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a6858__vftable_5a6858_005a6858;
            cls_0x5a68d8::meth_0x4bc7e0(this_01,param_4,this_00);
          }
          local_4 = 0xffffffff;
          *local_18 = this_01;
        }
        local_10 = local_10 + 1;
        piVar10 = piVar10 + 4;
        local_18 = local_18 + 1;
      } while (local_10 < (int)(this->cls_0x5a68d8).mbr_0x68);
    }
    (this->cls_0x5a68d8).mbr_0x4 = param_1;
    (this->cls_0x5a68d8).mbr_0x10 = param_1;
    (this->cls_0x5a68d8).mbr_0x8 = param_2;
    dVar9 = (**(code **)(**(int **)(this->cls_0x5a68d8).mbr_0x70 + 0x18))();
    (this->cls_0x5a68d8).mbr_0xc = dVar9;
    (this->cls_0x5a68d8).mbr_0x38 = *(dword *)(*(int *)(this->cls_0x5a68d8).mbr_0x70 + 0x38);
    (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x4bcd00_36)(&this->cls_0x5a68d8);
    uVar6 = 1;
  }
  else {
    uVar6 = 0;
  }
  ExceptionList = local_c;
  return uVar6;
}



// Function at 004bb440

undefined4 cls_0x5a6858::meth_0x4bb440(undefined4 param_1, undefined4 param_2, int param_3, int param_4, int param_5, int param_6)

{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint in_stack_0000001c;
  int local_124;
  int local_120;
  int local_11c;
  int local_118;
  int local_114;
  int local_10c;
  int local_100 [64];
  
  iVar7 = param_5 * param_6;
  piVar2 = local_100;
  bVar1 = false;
  if (0x10 < iVar7) {
    piVar2 = FUN_00482fb0(iVar7 * 0x10);
    if (piVar2 == (int *)0x0) {
      piVar2 = (int *)0x0;
    }
    bVar1 = true;
  }
  if ((in_stack_0000001c & 0x40000000) == 0) {
    local_118 = 0;
    local_10c = 0;
    local_11c = param_3;
    local_114 = param_4;
  }
  else {
    local_114 = param_4 + -4;
    local_118 = 2;
    local_10c = 2;
    local_11c = param_3 + -4;
  }
  local_124 = 0;
  if (0 < param_6) {
    local_120 = param_6;
    piVar4 = piVar2;
    do {
      iVar6 = 0;
      if (0 < param_5) {
        piVar3 = piVar4;
        iVar8 = param_5;
        do {
          piVar4 = piVar3 + 4;
          *piVar3 = iVar6 - local_118;
          piVar3[1] = local_124 - local_10c;
          piVar3[2] = param_3 + -1 + iVar6;
          piVar3[3] = param_4 + -1 + local_124;
          iVar6 = iVar6 + local_11c;
          iVar8 = iVar8 + -1;
          piVar3 = piVar4;
        } while (iVar8 != 0);
      }
      local_124 = local_124 + local_114;
      local_120 = local_120 + -1;
    } while (local_120 != 0);
  }
  uVar5 = meth_0x4bad80(this,param_1,param_2,piVar2,iVar7);
  if (bVar1) {
    FUN_004830f0(piVar2);
  }
  return uVar5;
}



// Function at 004bc730

cls_0x5a6858::cls_0x5a6858()

{
  cls_0x5a68d8::cls_0x5a68d8(&this->cls_0x5a68d8);
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a6858__vftable_5a6858_005a6858;
  (this->cls_0x5a68d8).mbr_0x6c = 0;
  (this->cls_0x5a68d8).mbr_0x68 = 0;
  (this->cls_0x5a68d8).mbr_0x64 = 0;
  (this->cls_0x5a68d8).mbr_0x70 = 0;
  return this;
}



// Function at 004bc750

cls_0x5a6858 * __thiscall cls_0x5a6858::~cls_0x5a6858(cls_0x5a6858 *this)

{
  undefined4 *puVar1;
  cls_0x5a6858 *pcVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059e118;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a6858__vftable_5a6858_005a6858;
  local_4 = 0;
  if ((this->cls_0x5a68d8).mbr_0x70 != 0) {
    puVar1 = (undefined4 *)(this->cls_0x5a68d8).mbr_0x64;
    if (puVar1 != (undefined4 *)0x0) {
      (**(code **)*puVar1)(1);
    }
    puVar1 = (undefined4 *)(this->cls_0x5a68d8).mbr_0x68;
    if (puVar1 != (undefined4 *)0x0) {
      (**(code **)*puVar1)(1);
    }
    puVar1 = (undefined4 *)(this->cls_0x5a68d8).mbr_0x6c;
    if (puVar1 != (undefined4 *)0x0) {
      (**(code **)*puVar1)(1);
    }
    (this->cls_0x5a68d8).mbr_0x6c = 0;
    (this->cls_0x5a68d8).mbr_0x68 = 0;
    (this->cls_0x5a68d8).mbr_0x64 = 0;
    (this->cls_0x5a68d8).mbr_0x70 = 0;
  }
  local_4 = 0xffffffff;
  pcVar2 = (cls_0x5a6858 *)meth_0x4bcb50(this);
  ExceptionList = local_c;
  return pcVar2;
}



// Function at 004bc8b0

void cls_0x5a6858::virt_meth_0x4bc8b0()

{
  dword dVar1;
  dword dVar2;
  int *piVar3;
  
  dVar1 = (this->cls_0x5a68d8).mbr_0x64;
  if (((dVar1 != 0) && (dVar2 = (this->cls_0x5a68d8).mbr_0x68, dVar2 != 0)) &&
     ((*(int *)(dVar1 + 4) != *(int *)(dVar2 + 4) || (*(int *)(dVar1 + 8) != *(int *)(dVar2 + 8)))))
  {
    FUN_00481c10((byte *)s_Multisurface_sizes_don_t_match_005defac);
  }
  dVar1 = (this->cls_0x5a68d8).mbr_0x64;
  if (((dVar1 != 0) && (dVar2 = (this->cls_0x5a68d8).mbr_0x6c, dVar2 != 0)) &&
     ((*(int *)(dVar1 + 4) != *(int *)(dVar2 + 4) || (*(int *)(dVar1 + 8) != *(int *)(dVar2 + 8)))))
  {
    FUN_00481c10((byte *)s_Multisurface_sizes_don_t_match_005defac);
  }
  cls_0x5a68d8::virt_meth_0x4bcd00(&this->cls_0x5a68d8);
  piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x64;
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x40))(0,0);
    piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x64;
    (**(code **)(*piVar3 + 0x44))(0,0,piVar3[1],piVar3[2]);
    (**(code **)(*(int *)(this->cls_0x5a68d8).mbr_0x64 + 0x48))(0);
  }
  piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x68;
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x40))(0,0);
    piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x68;
    (**(code **)(*piVar3 + 0x44))(0,0,piVar3[1],piVar3[2]);
    (**(code **)(*(int *)(this->cls_0x5a68d8).mbr_0x68 + 0x48))(0);
  }
  piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x40))(0,0);
    piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
    (**(code **)(*piVar3 + 0x44))(0,0,piVar3[1],piVar3[2]);
    (**(code **)(*(int *)(this->cls_0x5a68d8).mbr_0x6c + 0x48))(0);
  }
  return;
}



// Function at 004bc9b0

void cls_0x5a6858::virt_meth_0x4bc9b0(dword param_1)

{
  int *piVar1;
  dword in_stack_00000008;
  
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x64;
  (this->cls_0x5a68d8).mbr_0x14 = param_1;
  (this->cls_0x5a68d8).mbr_0x18 = in_stack_00000008;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x40))(param_1);
  }
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x40))(param_1);
  }
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x40))(param_1);
  }
  return;
}



// Function at 004bca70

void cls_0x5a6858::virt_meth_0x4bca70()

{
  int *piVar1;
  dword in_stack_00000004;
  
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x64;
  (this->cls_0x5a68d8).mbr_0x1c = in_stack_00000004;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x48))();
  }
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x48))();
  }
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x6c;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x48))();
  }
  return;
}



// Function at 004bcb50

void cls_0x5a6858::meth_0x4bcb50()

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)(this->cls_0x5a68d8).mbr_0x54;
  (this->cls_0x5a68d8).vftptr_0x0 = &cls_0x5a68d8__vftable_5a68d8_005a68d8;
  if (((puVar1 != (undefined4 *)0x0) && ((this->cls_0x5a68d8).mbr_0x58 != 0)) &&
     (puVar1 != (undefined4 *)0x0)) {
    (**(code **)*puVar1)(1);
  }
  puVar1 = (undefined4 *)(this->cls_0x5a68d8).mbr_0x5c;
  if (((puVar1 != (undefined4 *)0x0) && ((this->cls_0x5a68d8).mbr_0x60 != 0)) &&
     (puVar1 != (undefined4 *)0x0)) {
    (**(code **)*puVar1)(1);
  }
  return;
}



