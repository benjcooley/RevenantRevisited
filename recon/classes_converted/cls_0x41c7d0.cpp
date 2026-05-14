#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x41c7d0



// Function at 0041c7d0

void cls_0x41c7d0::meth_0x41c7d0(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = meth_0x49c430(this);
  meth_0x49b8e0(this,uVar1);
  return;
}



// Function at 0049a650

void cls_0x41c7d0::meth_0x49a650()

{
  dword *this_00;
  LPCVOID *ppvVar1;
  undefined4 uVar2;
  void **ppvVar3;
  char cVar4;
  uint uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  uint uVar8;
  int iVar9;
  uint *puVar10;
  undefined4 *puVar11;
  uint *puVar12;
  undefined4 *puVar13;
  uint *local_28;
  dword local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  undefined4 local_18;
  cls_0x41c7d0 *local_14;
  undefined4 *puStack_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059dbcb;
  pvStack_c = ExceptionList;
  iVar9 = 0;
  this_00 = &this->mbr_0x8;
  local_18 = 0;
  local_24 = 0;
  ExceptionList = &pvStack_c;
  ppvVar3 = &pvStack_c;
  local_14 = this;
  if (0 < (int)this->mbr_0x8) {
    do {
      if ((-1 < iVar9) &&
         (ppvVar1 = *(LPCVOID **)(this->mbr_0x18 + iVar9 * 4), ppvVar1 != (LPCVOID *)0x0)) {
        if (*ppvVar1 != (LPCVOID)0x0) {
          FUN_004830f0(*ppvVar1);
        }
        FUN_004830f0(ppvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar9 = iVar9 + 1;
      ppvVar3 = (void **)ExceptionList;
    } while (iVar9 < (int)*this_00);
  }
  ExceptionList = ppvVar3;
  *this_00 = 0;
  this->mbr_0xc = 0;
  iVar9 = _AIL_enumerate_3D_providers_12(&local_18,&local_1c,&local_28);
  do {
    if (iVar9 == 0) {
      ExceptionList = pvStack_c;
      return;
    }
    cVar4 = *(char *)local_28;
    puVar10 = local_28;
    while (cVar4 != '\0') {
      puVar10 = (uint *)((int)puVar10 + 1);
      uVar5 = FUN_0058b266((int)*(char *)puVar10);
      cVar4 = (char)uVar5;
      *(char *)puVar10 = cVar4;
    }
    puVar10 = FUN_0058ad30(local_28,(char *)&cls_0x5da790__vftable_5da790_005da790);
    if (puVar10 == (uint *)0x0) {
      puVar10 = FUN_0058ad30(local_28,s_HARDWARE_005da794);
      if (puVar10 == (uint *)0x0) {
        if ((int)this->mbr_0x4 < 0) {
          uStack_20 = 0;
        }
      }
      else {
        uStack_20 = 0;
        if ((int)this->mbr_0x4 < 0) {
          this->mbr_0x4 = local_24;
        }
      }
    }
    else if (DAT_00668118 == 0) {
      uStack_20 = 1;
      this->mbr_0x4 = local_24;
    }
    puVar6 = FUN_00482fb0(0xc);
    uVar2 = local_1c;
    puVar10 = local_28;
    uStack_4 = 0;
    puStack_10 = puVar6;
    if (puVar6 != (undefined4 *)0x0) {
      uVar5 = 0xffffffff;
      puVar12 = local_28;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        cVar4 = *(char *)puVar12;
        puVar12 = (uint *)((int)puVar12 + 1);
      } while (cVar4 != '\0');
      puVar7 = FUN_00482ef0(~uVar5);
      uVar5 = 0xffffffff;
      do {
        puVar12 = puVar10;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        puVar12 = (uint *)((int)puVar10 + 1);
        cVar4 = *(char *)puVar10;
        puVar10 = puVar12;
      } while (cVar4 != '\0');
      uVar5 = ~uVar5;
      puVar11 = (undefined4 *)((int)puVar12 - uVar5);
      puVar13 = puVar7;
      for (uVar8 = uVar5 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *puVar13 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar11;
        puVar11 = (undefined4 *)((int)puVar11 + 1);
        puVar13 = (undefined4 *)((int)puVar13 + 1);
      }
      puVar6[1] = uVar2;
      *puVar6 = puVar7;
      puVar6[2] = uStack_20;
      this = local_14;
    }
    uStack_4 = 0xffffffff;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x8);
    local_24 = local_24 + 1;
    iVar9 = _AIL_enumerate_3D_providers_12(&local_18,&local_1c,&local_28);
  } while( true );
}



// Function at 0049a830

undefined4 cls_0x41c7d0::meth_0x49a830()

{
  LPCVOID *ppvVar1;
  dword dVar2;
  cls_0x41c7d0 *pcVar3;
  dword *pdVar4;
  uint uVar5;
  int iVar6;
  dword dVar7;
  undefined4 *puVar8;
  cls_0x41c7d0 *pcStack_4;
  
  if (DAT_00668114 == 0) {
    *(undefined *)&this->mbr_0x5dc = 0;
    *(undefined *)&this->mbr_0x4d8 = 0;
    *(undefined *)&this->mbr_0x3d4 = 0;
    *(undefined *)&this->mbr_0x2d0 = 0;
    *(undefined *)&this->mbr_0x1cc = 0;
    *(undefined *)&this->mbr_0xc8 = 0;
    UNK_006687ac._4_2_ = 1;
    UNK_006687ac._6_2_ = 1;
    UNK_006687ac._8_4_ = 0x5622;
    UNK_006687ac._12_4_ = 0xac44;
    UNK_006687ac._16_2_ = 2;
    UNK_006687ac._18_2_ = 0x10;
    pcStack_4 = this;
    _AIL_set_preference_8(0x21,1);
    _AIL_set_preference_8(0x24,0);
    _AIL_set_preference_8(0x10,0x10000);
    if (DAT_006680f8 != 0) {
      _AIL_set_preference_8(0x1b,1);
    }
    this->mbr_0x0 = 0;
    if ((undefined4 *)this->mbr_0x2c != (undefined4 *)0x0) {
      puVar8 = (undefined4 *)this->mbr_0x2c;
      for (uVar5 = this->mbr_0x24 & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar8 = 0;
        puVar8 = puVar8 + 1;
      }
      for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
        *(undefined *)puVar8 = 0;
        puVar8 = (undefined4 *)((int)puVar8 + 1);
      }
    }
    pdVar4 = &this->mbr_0x8;
    this->mbr_0x1c = 0;
    iVar6 = 0;
    this->mbr_0x20 = 0;
    this->mbr_0xc4 = 0;
    if (0 < (int)*pdVar4) {
      do {
        if ((-1 < iVar6) &&
           (ppvVar1 = *(LPCVOID **)(this->mbr_0x18 + iVar6 * 4), ppvVar1 != (LPCVOID *)0x0)) {
          FUN_0049c950(ppvVar1);
          FUN_004830f0(ppvVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar4);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar4);
    }
    this->mbr_0xc = 0;
    *pdVar4 = 0;
    this->mbr_0x4 = 0xffffffff;
    this->mbr_0x34 = 0;
    this->mbr_0x30 = 0;
    _AIL_startup_0();
    iVar6 = _AIL_waveOutOpen_16(this,0,0,0x6687b0);
    if (iVar6 != 0) {
      DAT_00668114 = 1;
      return 0;
    }
    meth_0x49a650(this);
    if (0 < (int)*pdVar4) {
      if ((-1 < (int)this->mbr_0x4) &&
         (iVar6 = _AIL_open_3D_provider_4
                            (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4)),
         iVar6 != 0)) {
        this->mbr_0x4 = 0xffffffff;
      }
      if (((int)this->mbr_0x4 < 0) && (dVar7 = 0, 0 < (int)*pdVar4)) {
        do {
          iVar6 = _AIL_open_3D_provider_4(*(undefined4 *)(*(int *)(this->mbr_0x18 + dVar7 * 4) + 4))
          ;
          if (iVar6 == 0) {
            this->mbr_0x4 = dVar7;
            break;
          }
          dVar7 = dVar7 + 1;
        } while ((int)dVar7 < (int)*pdVar4);
      }
      dVar7 = this->mbr_0x4;
      if (-1 < (int)dVar7) {
        dVar2 = *(dword *)(*(int *)(this->mbr_0x18 + dVar7 * 4) + 8);
        this->mbr_0x34 = dVar2;
        if (dVar2 != 0) {
          if (DAT_00668114 == 0) {
            pcStack_4 = (cls_0x41c7d0 *)0x0;
            pcVar3 = pcStack_4;
            if (this->mbr_0x34 != 0) {
              _AIL_3D_provider_attribute_12
                        (*(undefined4 *)(*(int *)(this->mbr_0x18 + dVar7 * 4) + 4),
                         s_EAX_environment_selection_005da918,&pcStack_4);
              pcVar3 = pcStack_4;
            }
          }
          else {
            pcVar3 = (cls_0x41c7d0 *)0x0;
          }
          this->mbr_0xc0 = (dword)pcVar3;
          meth_0x49bef0(this);
        }
        dVar7 = _AIL_open_3D_listener_4
                          (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4));
        this->mbr_0x30 = dVar7;
        pdVar4 = &this->mbr_0x7c;
        iVar6 = 0x10;
        do {
          dVar7 = _AIL_allocate_3D_sample_handle_4
                            (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4));
          *pdVar4 = dVar7;
          pdVar4 = pdVar4 + 1;
          iVar6 = iVar6 + -1;
        } while (iVar6 != 0);
        iVar6 = 0xf;
        pdVar4 = &this->mbr_0xb8;
        do {
          if (*pdVar4 != 0) {
            _AIL_release_3D_sample_handle_4((&this->mbr_0x7c)[iVar6]);
            (&this->mbr_0x7c)[iVar6] = 0;
            break;
          }
          iVar6 = iVar6 + -1;
          pdVar4 = pdVar4 + -1;
        } while (-1 < iVar6);
      }
    }
    pdVar4 = &this->mbr_0x3c;
    iVar6 = 0x10;
    do {
      dVar7 = _AIL_allocate_sample_handle_4(this->mbr_0x0);
      *pdVar4 = dVar7;
      pdVar4 = pdVar4 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    iVar6 = 0xf;
    pdVar4 = &this->mbr_0x78;
    do {
      if (*pdVar4 != 0) {
        _AIL_release_sample_handle_4((&this->mbr_0x3c)[iVar6]);
        (&this->mbr_0x3c)[iVar6] = 0;
        break;
      }
      iVar6 = iVar6 + -1;
      pdVar4 = pdVar4 + -1;
    } while (-1 < iVar6);
    meth_0x49afd0(this);
    FUN_0058c9ff((int *)this->mbr_0x2c,(undefined4 *)this->mbr_0x1c,4,&LAB_0049ab00);
  }
  return 1;
}



// Function at 0049ad20

undefined4 cls_0x41c7d0::meth_0x49ad20()

{
  char cVar1;
  HANDLE pvVar2;
  uint *puVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  HANDLE pvVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char *pcVar12;
  uint *puVar13;
  undefined4 *puVar14;
  char *in_stack_00000004;
  uint local_4ac [32];
  uint local_42c [4];
  undefined4 local_41c;
  char local_418 [528];
  undefined4 local_208 [65];
  undefined4 local_104 [65];
  
  uVar7 = 0xffffffff;
  pcVar11 = in_stack_00000004;
  do {
    pcVar12 = pcVar11;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  puVar5 = (undefined4 *)(pcVar12 + -uVar7);
  puVar4 = local_208;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *puVar4 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar4 = puVar4 + 1;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined *)puVar4 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  }
  uVar7 = 0xffffffff;
  pcVar11 = in_stack_00000004;
  do {
    pcVar12 = pcVar11;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar12 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  puVar5 = (undefined4 *)(pcVar12 + -uVar7);
  puVar4 = local_104;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *puVar4 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar4 = puVar4 + 1;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined *)puVar4 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  }
  iVar9 = -1;
  puVar5 = local_208;
  do {
    puVar4 = puVar5;
    if (iVar9 == 0) break;
    iVar9 = iVar9 + -1;
    puVar4 = (undefined4 *)((int)puVar5 + 1);
    cVar1 = *(char *)puVar5;
    puVar5 = puVar4;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar4 + -1) = DAT_005da7d0;
  iVar9 = -1;
  *(undefined2 *)((int)puVar4 + 3) = DAT_005da7d4;
  puVar5 = local_104;
  do {
    puVar4 = puVar5;
    if (iVar9 == 0) break;
    iVar9 = iVar9 + -1;
    puVar4 = (undefined4 *)((int)puVar5 + 1);
    cVar1 = *(char *)puVar5;
    puVar5 = puVar4;
  } while (cVar1 != '\0');
  *(undefined4 *)((int)puVar4 + -1) = DAT_005da7d8;
  *(undefined2 *)((int)puVar4 + 3) = DAT_005da7dc;
  pvVar2 = FUN_004a19d0((LPCSTR)local_104,local_42c);
  pvVar6 = pvVar2;
  while (pvVar6 != (HANDLE)0xffffffff) {
    uVar7 = 0xffffffff;
    pcVar11 = local_418;
    do {
      pcVar12 = pcVar11;
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      pcVar12 = pcVar11 + 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar12;
    } while (cVar1 != '\0');
    uVar7 = ~uVar7;
    puVar3 = (uint *)(pcVar12 + -uVar7);
    puVar13 = local_4ac;
    for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *puVar13 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar13 = puVar13 + 1;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined *)puVar13 = *(undefined *)puVar3;
      puVar3 = (uint *)((int)puVar3 + 1);
      puVar13 = (uint *)((int)puVar13 + 1);
    }
    puVar3 = FUN_0058ade0(local_4ac,'.');
    if (puVar3 != (uint *)0x0) {
      *(undefined *)puVar3 = 0;
    }
    puVar4 = FUN_00482fb0(0x1c);
    puVar5 = puVar4;
    for (iVar9 = 7; iVar9 != 0; iVar9 = iVar9 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    uVar7 = 0xffffffff;
    puVar4[2] = in_stack_00000004;
    puVar3 = local_4ac;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *(char *)puVar3;
      puVar3 = (uint *)((int)puVar3 + 1);
    } while (cVar1 != '\0');
    puVar5 = FUN_00482ef0(~uVar7);
    uVar7 = 0xffffffff;
    puVar3 = local_4ac;
    do {
      puVar13 = puVar3;
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      puVar13 = (uint *)((int)puVar3 + 1);
      cVar1 = *(char *)puVar3;
      puVar3 = puVar13;
    } while (cVar1 != '\0');
    uVar7 = ~uVar7;
    puVar10 = (undefined4 *)((int)puVar13 - uVar7);
    puVar14 = puVar5;
    for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
      *puVar14 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar14 = puVar14 + 1;
    }
    for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined *)puVar14 = *(undefined *)puVar10;
      puVar10 = (undefined4 *)((int)puVar10 + 1);
      puVar14 = (undefined4 *)((int)puVar14 + 1);
    }
    *puVar4 = puVar5;
    puVar4[1] = &DAT_005da7e0;
    puVar4[3] = local_41c;
    puVar4[6] = 0;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x1c);
    pvVar6 = (HANDLE)FUN_004a1b20(pvVar2,local_42c);
  }
  pvVar6 = FUN_004a19d0((LPCSTR)local_208,local_42c);
  if (pvVar6 != (HANDLE)0xffffffff) {
    do {
      uVar7 = 0xffffffff;
      pcVar11 = local_418;
      do {
        pcVar12 = pcVar11;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar12 = pcVar11 + 1;
        cVar1 = *pcVar11;
        pcVar11 = pcVar12;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      puVar3 = (uint *)(pcVar12 + -uVar7);
      puVar13 = local_4ac;
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *puVar13 = *puVar3;
        puVar3 = puVar3 + 1;
        puVar13 = puVar13 + 1;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined *)puVar13 = *(undefined *)puVar3;
        puVar3 = (uint *)((int)puVar3 + 1);
        puVar13 = (uint *)((int)puVar13 + 1);
      }
      puVar3 = FUN_0058ade0(local_4ac,'.');
      if (puVar3 != (uint *)0x0) {
        *(undefined *)puVar3 = 0;
      }
      puVar4 = FUN_00482fb0(0x1c);
      puVar5 = puVar4;
      for (iVar9 = 7; iVar9 != 0; iVar9 = iVar9 + -1) {
        *puVar5 = 0;
        puVar5 = puVar5 + 1;
      }
      uVar7 = 0xffffffff;
      puVar4[2] = in_stack_00000004;
      puVar3 = local_4ac;
      do {
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        cVar1 = *(char *)puVar3;
        puVar3 = (uint *)((int)puVar3 + 1);
      } while (cVar1 != '\0');
      puVar5 = FUN_00482ef0(~uVar7);
      uVar7 = 0xffffffff;
      puVar3 = local_4ac;
      do {
        puVar13 = puVar3;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        puVar13 = (uint *)((int)puVar3 + 1);
        cVar1 = *(char *)puVar3;
        puVar3 = puVar13;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      puVar10 = (undefined4 *)((int)puVar13 - uVar7);
      puVar14 = puVar5;
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *puVar14 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar14 = puVar14 + 1;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined *)puVar14 = *(undefined *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar14 = (undefined4 *)((int)puVar14 + 1);
      }
      *puVar4 = puVar5;
      puVar4[1] = &DAT_005da7e8;
      puVar4[3] = local_41c;
      puVar4[6] = 2;
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x1c);
      iVar9 = FUN_004a1b20(pvVar6,local_42c);
    } while (iVar9 != -1);
  }
  return 1;
}



// Function at 0049afd0

undefined4 cls_0x41c7d0::meth_0x49afd0()

{
  dword *pdVar1;
  char *pcVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  uint extraout_ECX;
  uint extraout_ECX_00;
  dword *pdVar6;
  dword *pdVar7;
  
  pdVar1 = &this->mbr_0xc8;
  FUN_00483120((char *)&DAT_0065dde8,(char *)pdVar1,0x104);
  uVar5 = 0xffffffff;
  pdVar6 = pdVar1;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar6;
    pdVar6 = (dword *)((int)pdVar6 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar1);
  _strncpy(pcVar2,s_sound_effects__005da7f0,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  FUN_0059bd3e(pdVar1);
  iVar4 = meth_0x49ad20(this);
  if (iVar4 == 0) {
    return 0;
  }
  pdVar1 = &this->mbr_0x1cc;
  FUN_00483120((char *)&DAT_0065dde8,(char *)pdVar1,0x104);
  uVar5 = 0xffffffff;
  pdVar6 = pdVar1;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar6;
    pdVar6 = (dword *)((int)pdVar6 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar1);
  _strncpy(pcVar2,s_sound__005da800,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  uVar5 = 0xffffffff;
  pdVar6 = pdVar1;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar6;
    pdVar6 = (dword *)((int)pdVar6 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar1);
  _strncpy(pcVar2,&DAT_0065bb18.field_0x100,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  uVar5 = 0xffffffff;
  pdVar6 = pdVar1;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar6;
    pdVar6 = (dword *)((int)pdVar6 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar1);
  _strncpy(pcVar2,&DAT_005da808,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  FUN_0059bd3e(pdVar1);
  iVar4 = meth_0x49ad20(this);
  if (iVar4 == 0) {
    return 0;
  }
  pdVar6 = &this->mbr_0x2d0;
  FUN_00483120(&DAT_0065bc44,(char *)pdVar6,0x104);
  uVar5 = 0xffffffff;
  pdVar7 = pdVar6;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar7;
    pdVar7 = (dword *)((int)pdVar7 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar6);
  _strncpy(pcVar2,s_sound_effects__005da80c,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  FUN_0059bd3e(pdVar6);
  uVar5 = FUN_0059a530_stricmp(extraout_ECX,(byte *)&this->mbr_0xc8,(byte *)pdVar6);
  if (uVar5 != 0) {
    meth_0x49ad20(this);
  }
  pdVar6 = &this->mbr_0x3d4;
  FUN_00483120(&DAT_0065bc44,(char *)pdVar6,0x104);
  uVar5 = 0xffffffff;
  pdVar7 = pdVar6;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar7;
    pdVar7 = (dword *)((int)pdVar7 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar6);
  _strncpy(pcVar2,s_sound__005da81c,iVar4 + 0x103);
  uVar5 = 0xffffffff;
  pcVar2[iVar4 + 0x103] = '\0';
  pdVar7 = pdVar6;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar7;
    pdVar7 = (dword *)((int)pdVar7 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar6);
  _strncpy(pcVar2,&DAT_0065bb18.field_0x100,iVar4 + 0x103);
  uVar5 = 0xffffffff;
  pcVar2[iVar4 + 0x103] = '\0';
  pdVar7 = pdVar6;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar3 = *(char *)pdVar7;
    pdVar7 = (dword *)((int)pdVar7 + 1);
  } while (cVar3 != '\0');
  iVar4 = -(~uVar5 - 1);
  pcVar2 = (char *)((~uVar5 - 1) + (int)pdVar6);
  _strncpy(pcVar2,&DAT_005da824,iVar4 + 0x103);
  pcVar2[iVar4 + 0x103] = '\0';
  FUN_0059bd3e(pdVar6);
  uVar5 = FUN_0059a530_stricmp(extraout_ECX_00,(byte *)pdVar1,(byte *)pdVar6);
  if (uVar5 != 0) {
    meth_0x49ad20(this);
  }
  return 1;
}



// Function at 0049b220

undefined4 cls_0x41c7d0::meth_0x49b220()

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  dword *pdVar6;
  dword *pdVar7;
  
  if (DAT_00668114 == 0) {
    if ((DAT_0065a784 < 0) || (*(int *)(DAT_0065a77c + DAT_0065a784 * 4) == 0)) {
      return 0;
    }
    pdVar6 = &this->mbr_0x4d8;
    FUN_00483120((char *)&DAT_0065d6a4,(char *)pdVar6,0x104);
    if (DAT_0065a784 < 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar3 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,(char *)(iVar5 + 0x58),iVar3 + 0x103);
    pcVar1[iVar3 + 0x103] = '\0';
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar5 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,s__sound_effects__005da828,iVar5 + 0x103);
    pcVar1[iVar5 + 0x103] = '\0';
    iVar5 = meth_0x49ad20(this);
    if (iVar5 == 0) {
      return 0;
    }
    pdVar6 = &this->mbr_0x5dc;
    FUN_00483120((char *)&DAT_0065d6a4,(char *)pdVar6,0x104);
    if (DAT_0065a784 < 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar3 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,(char *)(iVar5 + 0x58),iVar3 + 0x103);
    pcVar1[iVar3 + 0x103] = '\0';
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar5 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,s__sound__005da838,iVar5 + 0x103);
    pcVar1[iVar5 + 0x103] = '\0';
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar5 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,&DAT_0065bb18.field_0x100,iVar5 + 0x103);
    pcVar1[iVar5 + 0x103] = '\0';
    uVar4 = 0xffffffff;
    pdVar7 = pdVar6;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar2 = *(char *)pdVar7;
      pdVar7 = (dword *)((int)pdVar7 + 1);
    } while (cVar2 != '\0');
    iVar5 = -(~uVar4 - 1);
    pcVar1 = (char *)((~uVar4 - 1) + (int)pdVar6);
    _strncpy(pcVar1,&DAT_005da840,iVar5 + 0x103);
    pcVar1[iVar5 + 0x103] = '\0';
    iVar5 = meth_0x49ad20(this);
    if (iVar5 == 0) {
      return 0;
    }
    FUN_0058c9ff((int *)this->mbr_0x2c,(undefined4 *)this->mbr_0x1c,4,&LAB_0049ab00);
  }
  return 1;
}



// Function at 0049b400

void cls_0x41c7d0::meth_0x49b400()

{
  dword *this_00;
  LPCVOID *ppvVar1;
  LPCVOID pvVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  dword local_4;
  
  iVar5 = 0;
  if ((DAT_00668114 == 0) && (*(char *)&this->mbr_0x4d8 != '\0')) {
    dVar3 = this->mbr_0x1c;
    this_00 = &this->mbr_0x1c;
    local_4 = 0;
    if (0 < (int)dVar3) {
      do {
        ppvVar1 = *(LPCVOID **)(this->mbr_0x2c + iVar5 * 4);
        if (ppvVar1 != (LPCVOID *)0x0) {
          if (((dword *)ppvVar1[2] == &this->mbr_0x4d8) || ((dword *)ppvVar1[2] == &this->mbr_0x5dc)
             ) {
            if ((-1 < iVar5) &&
               (((iVar5 < (int)dVar3 && (ppvVar1 != (LPCVOID *)0x0)) &&
                (iVar4 = meth_0x49c560(this), iVar4 != 0)))) {
              if (ppvVar1[4] != (LPCVOID)0x0) {
                FUN_004830f0(ppvVar1[4]);
              }
              FUN_00482f80(*ppvVar1);
              if ((-1 < iVar5) &&
                 (pvVar2 = *(LPCVOID *)(this->mbr_0x2c + iVar5 * 4), pvVar2 != (LPCVOID)0x0)) {
                FUN_004830f0(pvVar2);
              }
              cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
            }
          }
          else {
            cls_0x45f7c0::meth_0x41c910((cls_0x45f7c0 *)this_00,ppvVar1);
            local_4 = local_4 + 1;
          }
        }
        dVar3 = *this_00;
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)dVar3);
    }
    *(undefined *)&this->mbr_0x4d8 = 0;
    *this_00 = local_4;
    *(undefined *)&this->mbr_0x5dc = 0;
  }
  return;
}



// Function at 0049b650

undefined4 cls_0x41c7d0::meth_0x49b650()

{
  char cVar1;
  char **ppcVar2;
  int iVar3;
  dword dVar4;
  FILE **ppFVar5;
  char *pcVar6;
  undefined4 *puVar7;
  SIZE_T SVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  char *pcVar13;
  undefined4 *puVar14;
  int in_stack_00000004;
  int local_10c;
  int *local_108;
  undefined4 local_104 [65];
  
  if (((((DAT_00668114 != 0) || ((int)this->mbr_0x1c < 1)) || (in_stack_00000004 < 0)) ||
      (((int)this->mbr_0x1c <= in_stack_00000004 || (this->mbr_0x0 == 0)))) ||
     (ppcVar2 = *(char ***)(this->mbr_0x2c + in_stack_00000004 * 4), ppcVar2 == (char **)0x0)) {
    return 0;
  }
  if (ppcVar2[4] != (char *)0x0) {
    return 1;
  }
  uVar9 = 0xffffffff;
  pcVar6 = ppcVar2[2];
  do {
    pcVar13 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar13 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar13;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  puVar7 = (undefined4 *)(pcVar13 + -uVar9);
  puVar14 = local_104;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar14 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar14 = puVar14 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined *)puVar14 = *(undefined *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
    puVar14 = (undefined4 *)((int)puVar14 + 1);
  }
  uVar9 = 0xffffffff;
  pcVar6 = *ppcVar2;
  do {
    pcVar13 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar13 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar13;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  iVar11 = -1;
  puVar7 = local_104;
  do {
    puVar14 = puVar7;
    if (iVar11 == 0) break;
    iVar11 = iVar11 + -1;
    puVar14 = (undefined4 *)((int)puVar7 + 1);
    cVar1 = *(char *)puVar7;
    puVar7 = puVar14;
  } while (cVar1 != '\0');
  puVar7 = (undefined4 *)(pcVar13 + -uVar9);
  puVar14 = (undefined4 *)((int)puVar14 + -1);
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar14 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar14 = puVar14 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined *)puVar14 = *(undefined *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
    puVar14 = (undefined4 *)((int)puVar14 + 1);
  }
  uVar9 = 0xffffffff;
  pcVar6 = ppcVar2[1];
  do {
    pcVar13 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar13 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar13;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  iVar11 = -1;
  puVar7 = local_104;
  do {
    puVar14 = puVar7;
    if (iVar11 == 0) break;
    iVar11 = iVar11 + -1;
    puVar14 = (undefined4 *)((int)puVar7 + 1);
    cVar1 = *(char *)puVar7;
    puVar7 = puVar14;
  } while (cVar1 != '\0');
  puVar7 = (undefined4 *)(pcVar13 + -uVar9);
  puVar14 = (undefined4 *)((int)puVar14 + -1);
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar14 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar14 = puVar14 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined *)puVar14 = *(undefined *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
    puVar14 = (undefined4 *)((int)puVar14 + 1);
  }
  ppFVar5 = (FILE **)FUN_004a1240(local_104,(uint *)&DAT_005da844,0);
  if (ppFVar5 != (FILE **)0x0) {
    pcVar6 = (char *)FUN_004a17b0((int *)ppFVar5);
    ppcVar2[3] = pcVar6;
    puVar7 = FUN_00482fb0((uint)pcVar6);
    ppcVar2[4] = (char *)puVar7;
    uVar9 = FUN_004a15a0(puVar7,(uint)ppcVar2[3],1,ppFVar5);
    if (uVar9 != 1) {
      FUN_004830f0(ppcVar2[4]);
      ppcVar2[4] = (char *)0x0;
      ppcVar2[3] = (char *)0x0;
      return 0;
    }
    FUN_004a1540(ppFVar5);
    local_10c = 100;
    SVar8 = FUN_004835f0();
    if ((char *)(SVar8 / 0x14) < ppcVar2[3] + this->mbr_0xbc) {
      do {
        if (local_10c < 1) break;
        iVar11 = 0;
        uVar9 = 0xffffffff;
        iVar12 = 0;
        if (0 < (int)this->mbr_0x1c) {
          local_108 = (int *)this->mbr_0x2c;
          do {
            iVar3 = *local_108;
            if (((iVar3 != 0) && (*(int *)(iVar3 + 0x10) != 0)) &&
               (((*(byte *)(iVar3 + 0x18) & 8) != 0 && (*(uint *)(iVar3 + 0x14) < uVar9)))) {
              uVar9 = *(uint *)(iVar3 + 0x14);
              iVar11 = iVar12;
            }
            iVar12 = iVar12 + 1;
            local_108 = local_108 + 1;
          } while (iVar12 < (int)this->mbr_0x1c);
        }
        meth_0x49b8e0(this,iVar11);
        local_10c = local_10c + -1;
        SVar8 = FUN_004835f0();
      } while ((char *)(SVar8 / 0x14) < ppcVar2[3] + this->mbr_0xbc);
    }
  }
  dVar4 = this->mbr_0xbc;
  ppcVar2[6] = (char *)((uint)ppcVar2[6] & 0xffffffef | 8);
  this->mbr_0xbc = (dword)(ppcVar2[3] + dVar4);
  return 1;
}



// Function at 0049b880

undefined4 cls_0x41c7d0::meth_0x49b880()

{
  uint uVar1;
  undefined4 uVar2;
  undefined *puStack00000004;
  undefined local_1c [28];
  
  puStack00000004 = local_1c;
  if (DAT_00668114 == 0) {
    uVar1 = FUN_0058e8bb(&stack0x00000004,this->mbr_0x2c,this->mbr_0x1c,4,&LAB_0049ab00);
    if ((uVar1 != 0) && (-1 < (int)(uVar1 - this->mbr_0x2c) >> 2)) {
      uVar2 = meth_0x49b650(this);
      return uVar2;
    }
  }
  return 0;
}



// Function at 0049b8e0

undefined4 cls_0x41c7d0::meth_0x49b8e0(int param_1)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int in_stack_00000008;
  
  if (((DAT_00668114 == 0) && (-1 < param_1)) && (param_1 <= (int)this->mbr_0x1c)) {
    dVar1 = this->mbr_0xc4;
    *(dword *)(*(int *)(this->mbr_0x2c + param_1 * 4) + 0x14) = dVar1;
    this->mbr_0xc4 = dVar1 + 1;
    iVar2 = *(int *)(this->mbr_0x2c + param_1 * 4);
    if (iVar2 != 0) {
      iVar3 = meth_0x49c560(this);
      if (iVar3 != 0) {
        if (*(LPCVOID *)(iVar2 + 0x10) != (LPCVOID)0x0) {
          FUN_004830f0(*(LPCVOID *)(iVar2 + 0x10));
          dVar1 = this->mbr_0x2c;
          *(undefined4 *)(iVar2 + 0x10) = 0;
          this->mbr_0xbc = this->mbr_0xbc - *(int *)(*(int *)(dVar1 + param_1 * 4) + 0xc);
        }
        uVar4 = *(uint *)(iVar2 + 0x18) & 0xfffffff7;
        *(uint *)(iVar2 + 0x18) = uVar4;
        if (in_stack_00000008 != 0) {
          *(uint *)(iVar2 + 0x18) = uVar4 | 0x10;
        }
        return 1;
      }
    }
  }
  return 0;
}



// Function at 0049b990

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x41c7d0::meth_0x49b990(undefined param_1, undefined param_2, int param_3)

{
  int iVar1;
  dword dVar2;
  bool bVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  dword *pdVar8;
  longlong lVar9;
  int3 in_stack_00000005;
  undefined3 in_stack_00000009;
  int *in_stack_00000010;
  int in_stack_00000014;
  int in_stack_00000018;
  undefined local_24 [4];
  undefined local_20 [4];
  undefined local_1c [4];
  int iStack_18;
  int iStack_14;
  int iStack_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((((DAT_00668114 == 0) && (-1 < in_stack_00000005)) && (_param_1 <= (int)this->mbr_0x1c)) &&
     ((iVar1 = *(int *)(this->mbr_0x2c + _param_1 * 4), iVar1 != 0 && (this->mbr_0x0 != 0)))) {
    if (this->mbr_0x30 == 0) {
      iVar7 = 0;
      iVar6 = 0;
      local_4 = 0;
      local_c = 0;
      local_8 = 0;
    }
    else {
      _AIL_3D_position_16(this->mbr_0x30,local_24,local_20,local_1c);
      lVar9 = __ftol();
      iVar7 = (int)lVar9;
      local_c = iVar7;
      lVar9 = __ftol();
      iVar6 = (int)lVar9;
      local_8 = iVar6;
      lVar9 = __ftol();
      local_4 = (int)lVar9;
    }
    iStack_18 = iVar7;
    iStack_14 = iVar6;
    iStack_10 = local_4;
    if (in_stack_00000010 != (int *)0x0) {
      iStack_10 = in_stack_00000010[2];
      iStack_18 = *in_stack_00000010;
      iStack_14 = in_stack_00000010[1];
    }
    if ((*(byte *)(iVar1 + 0x18) & 0x10) != 0) {
      meth_0x49b650(this);
    }
    if (*(int *)(iVar1 + 0x10) != 0) {
      iVar7 = -1;
      iVar6 = 0;
      pdVar8 = &this->mbr_0x7c;
      do {
        if ((*pdVar8 != 0) && (iVar4 = _AIL_3D_sample_status_4(*pdVar8), iVar4 == 2)) {
          _AIL_3D_user_data_8((&this->mbr_0x7c)[iVar6],0);
          _AIL_set_3D_user_data_12((&this->mbr_0x7c)[iVar6],0,0xffffffff);
          meth_0x49c560(this);
          iVar7 = iVar6;
          break;
        }
        iVar6 = iVar6 + 1;
        pdVar8 = pdVar8 + 1;
      } while (iVar6 < 0x10);
      bVar3 = false;
      if (((*(byte *)(iVar1 + 0x18) & 2) == 0) &&
         ((((iVar6 = FUN_0046de60_Distance2D(&iStack_18,&local_c), in_stack_00000014 <= iVar6 ||
            (this->mbr_0x34 != 0)) && (-1 < iVar7)) && (DAT_0066811c == 0)))) {
        iVar6 = _AIL_set_3D_sample_file_8((&this->mbr_0x7c)[iVar7],*(undefined4 *)(iVar1 + 0x10));
        bVar3 = true;
      }
      else {
        iVar7 = -1;
        iVar6 = 0;
        pdVar8 = &this->mbr_0x3c;
        do {
          if ((*pdVar8 != 0) && (iVar4 = _AIL_sample_status_4(*pdVar8), iVar4 == 2)) {
            _AIL_sample_user_data_8((&this->mbr_0x3c)[iVar6],0);
            _AIL_set_sample_user_data_12((&this->mbr_0x3c)[iVar6],0,0xffffffff);
            meth_0x49c560(this);
            iVar7 = iVar6;
            break;
          }
          iVar6 = iVar6 + 1;
          pdVar8 = pdVar8 + 1;
        } while (iVar6 < 0x10);
        if (iVar7 < 0) {
          return 0;
        }
        _AIL_init_sample_4((&this->mbr_0x3c)[iVar7]);
        iVar6 = _AIL_set_named_sample_file_20
                          ((&this->mbr_0x3c)[iVar7],*(undefined4 *)(iVar1 + 4),
                           *(undefined4 *)(iVar1 + 0x10),*(undefined4 *)(iVar1 + 0xc),0);
      }
      if (iVar6 != 0) {
        if (bVar3) {
          _AIL_set_3D_position_16
                    ((&this->mbr_0x7c)[iVar7],(float)iStack_18,(float)iStack_14,(float)iStack_10);
          uVar5 = DAT_005d7aa0 + -0x7f + _param_2;
          _AIL_set_3D_sample_volume_8((&this->mbr_0x7c)[iVar7],uVar5 & ((int)uVar5 < 0) - 1);
          _AIL_set_3D_sample_float_distances_20
                    ((&this->mbr_0x7c)[iVar7],(float)in_stack_00000018,(float)in_stack_00000014,
                     (float)in_stack_00000018,(float)in_stack_00000014);
          _AIL_set_3D_sample_loop_count_8((&this->mbr_0x7c)[iVar7],param_3);
          _AIL_set_3D_user_data_12((&this->mbr_0x7c)[iVar7],0,_param_1);
          _AIL_start_3D_sample_4((&this->mbr_0x7c)[iVar7]);
        }
        else {
          uVar5 = DAT_005d7aa0 + -0x7f + _param_2;
          uVar5 = uVar5 & ((int)uVar5 < 0) - 1;
          if (DAT_0066811c != 0) {
            iVar6 = FUN_0046de60_Distance2D(&iStack_18,&local_c);
            if ((float)iVar6 < _DAT_005a3698) {
              FUN_0046de60_Distance2D(&iStack_18,&local_c);
            }
            lVar9 = __ftol();
            uVar5 = (uint)lVar9;
          }
          _AIL_set_sample_volume_8((&this->mbr_0x3c)[iVar7],uVar5);
          _AIL_set_sample_loop_count_8((&this->mbr_0x3c)[iVar7],param_3);
          _AIL_set_sample_user_data_12((&this->mbr_0x3c)[iVar7],0,_param_1);
          _AIL_register_EOS_callback_8((&this->mbr_0x3c)[iVar7],FUN_0049c480);
          _AIL_start_sample_4((&this->mbr_0x3c)[iVar7]);
        }
        if (param_3 == 0) {
          *(undefined4 *)(iVar1 + 0x14) = 0x7fffffff;
          *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 4;
          return 1;
        }
        dVar2 = this->mbr_0xc4;
        *(dword *)(iVar1 + 0x14) = dVar2;
        this->mbr_0xc4 = dVar2 + 1;
        *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 4;
        return 1;
      }
    }
  }
  return 0;
}



// Function at 0049bd90

void cls_0x41c7d0::meth_0x49bd90()

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  dword *pdVar5;
  dword *pdVar6;
  int in_stack_00000004;
  
  if ((((DAT_00668114 == 0) && (-1 < in_stack_00000004)) &&
      (in_stack_00000004 <= (int)this->mbr_0x1c)) &&
     ((iVar2 = *(int *)(this->mbr_0x2c + in_stack_00000004 * 4), iVar2 != 0 && (this->mbr_0x0 != 0))
     )) {
    pdVar5 = &this->mbr_0x7c;
    iVar4 = 0;
    pdVar6 = pdVar5;
    do {
      if ((*pdVar6 != 0) && (iVar3 = _AIL_3D_user_data_8(*pdVar6,0), in_stack_00000004 == iVar3)) {
        _AIL_end_3D_sample_4((&this->mbr_0x7c)[iVar4]);
        _AIL_set_3D_user_data_12((&this->mbr_0x7c)[iVar4],0,0xffffffff);
        if (iVar4 < 0x10) goto LAB_0049be57;
        break;
      }
      iVar4 = iVar4 + 1;
      pdVar6 = pdVar6 + 1;
    } while (iVar4 < 0x10);
    iVar4 = 0;
    pdVar6 = &this->mbr_0x3c;
    do {
      if ((*pdVar6 != 0) && (iVar3 = _AIL_sample_user_data_8(*pdVar6,0), in_stack_00000004 == iVar3)
         ) {
        _AIL_end_sample_4((&this->mbr_0x3c)[iVar4]);
        _AIL_set_sample_user_data_12((&this->mbr_0x3c)[iVar4],0,0xffffffff);
        break;
      }
      iVar4 = iVar4 + 1;
      pdVar6 = pdVar6 + 1;
    } while (iVar4 < 0x10);
LAB_0049be57:
    iVar4 = 0;
    do {
      if (((pdVar5[-0x10] != 0) &&
          (iVar3 = _AIL_sample_user_data_8(pdVar5[-0x10],0), in_stack_00000004 == iVar3)) ||
         ((*pdVar5 != 0 && (iVar3 = _AIL_3D_user_data_8(*pdVar5,0), in_stack_00000004 == iVar3))))
      break;
      iVar4 = iVar4 + 1;
      pdVar5 = pdVar5 + 1;
    } while (iVar4 < 0x10);
    if (iVar4 == 0x10) {
      puVar1 = (uint *)(iVar2 + 0x18);
      *puVar1 = *puVar1 & 0xfffffffb;
    }
  }
  return;
}



// Function at 0049bef0

void cls_0x41c7d0::meth_0x49bef0()

{
  dword in_stack_00000004;
  dword local_1a4;
  undefined4 local_1a0;
  undefined4 local_19c [4];
  undefined4 local_18c;
  undefined4 local_188;
  undefined4 local_184;
  undefined4 local_180;
  undefined4 local_17c;
  undefined4 local_178;
  undefined4 local_174;
  undefined4 local_170;
  undefined4 local_16c;
  undefined4 local_168;
  undefined4 local_164;
  undefined4 local_160;
  undefined4 local_15c;
  undefined4 local_158;
  undefined4 local_154;
  undefined4 local_150;
  undefined4 local_14c;
  undefined4 local_148;
  undefined4 local_144;
  undefined4 local_140;
  undefined4 local_13c;
  undefined4 local_138;
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
  undefined4 local_e4;
  undefined4 local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
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
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
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
  undefined4 local_4;
  
  if (DAT_00668114 == 0) {
    local_1a0 = 0;
    local_19c[0] = 0;
    local_19c[1] = 0;
    local_19c[2] = 0;
    local_19c[3] = 1;
    local_18c = 0x3e800000;
    local_188 = 0x3dcccccd;
    local_184 = 0;
    local_180 = 2;
    local_17c = 0x3ed58106;
    local_178 = 0x3ecccccd;
    local_174 = 0x3f2a7efa;
    local_170 = 3;
    local_16c = 0x3f272b02;
    local_168 = 0x3fbfdf3b;
    local_164 = 0x3e29fbe7;
    local_160 = 4;
    local_15c = 0x3e54fdf4;
    local_158 = 0x3ef4bc6a;
    local_154 = 0;
    local_150 = 5;
    local_14c = 0x3f000000;
    local_148 = 0x4013c6a8;
    local_144 = 0x3f6353f8;
    local_140 = 6;
    local_13c = 0x3ece5604;
    local_138 = 0x4088ed91;
    local_134 = 0x3f000000;
    local_130 = 7;
    local_12c = 0x3f000000;
    local_128 = 0x407d8106;
    local_124 = 0x3f000000;
    local_120 = 8;
    local_11c = 0x3e99999a;
    local_118 = 0x40333333;
    local_114 = 0x3f333333;
    local_110 = 9;
    local_10c = 0x3eb8d4fe;
    local_108 = 0x40e91687;
    local_104 = 0x3ea9fbe7;
    local_100 = 10;
    local_fc = 0x3e99999a;
    local_f8 = 0x40333333;
    local_f4 = 0x3f333333;
    local_f0 = 0xb;
    local_ec = 0x3e1cac08;
    local_e8 = 0x3e849ba6;
    local_e4 = 0x40000000;
    local_e0 = 0xc;
    local_dc = 0x3eb8d4fe;
    local_d8 = 0x3fbf1aa0;
    local_d4 = 0;
    local_d0 = 0xd;
    local_cc = 0x3ee353f8;
    local_c8 = 0x402c9ba6;
    local_c4 = 0x3f2353f8;
    local_c0 = 0xe;
    local_bc = 0x3e3851ec;
    local_b8 = 0x40400000;
    local_b4 = 0x3f333333;
    local_b0 = 0xf;
    local_ac = 0x3de147ae;
    local_a8 = 0x40200000;
    local_a4 = 0x3ef33333;
    local_a0 = 0x10;
    local_9c = 0x3de353f8;
    local_98 = 0x40311687;
    local_94 = 0x3e656042;
    local_90 = 0x11;
    local_8c = 0x3e428f5c;
    local_88 = 0x40400000;
    local_84 = 0x3f333333;
    local_80 = 0x12;
    local_7c = 0x3f800000;
    local_78 = 0x3fbfdf3b;
    local_74 = 0x3f000000;
    local_70 = 0x13;
    local_6c = 0x3dc6a7f0;
    local_68 = 0x40311687;
    local_64 = 0x3e656042;
    local_60 = 0x14;
    local_5c = 0x3e800000;
    local_58 = 0x3fc00000;
    local_54 = 0x3f333333;
    local_50 = 0x15;
    local_4c = 0x3f26e979;
    local_48 = 0x4038b439;
    local_44 = 0x3e800000;
    local_40 = 0x16;
    local_3c = 0x3f800000;
    local_38 = 0x3fbfdf3b;
    local_34 = 0;
    local_30 = 0x17;
    local_2c = 0x3f600000;
    local_28 = 0x410645a2;
    local_24 = 0x3fb1a9fc;
    local_20 = 0x18;
    local_1c = 0x3e0e5604;
    local_18 = 0x4189df3b;
    local_14 = 0x3f2a7efa;
    local_10 = 0x19;
    local_c = 0x3ef8d4fe;
    local_8 = 0x40f20419;
    local_4 = 0x3f4e5604;
    if (this->mbr_0x34 != 0) {
      local_1a4 = 0;
      this->mbr_0x38 = in_stack_00000004;
      _AIL_3D_provider_attribute_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_environment_selection_005da848,&local_1a4);
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_environment_selection_005da864,&this->mbr_0x38);
      _AIL_3D_provider_attribute_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_environment_selection_005da880,&local_1a4);
      if (local_1a4 != in_stack_00000004) {
        FUN_004820b0((byte *)s_SetEAXEnvironment_goofed____the_n_005da89c);
      }
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_effect_volume_005da8e8,local_19c + in_stack_00000004 * 4);
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_decay_time_005da8fc,local_19c + in_stack_00000004 * 4 + 1);
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_damping_005da90c,local_19c + in_stack_00000004 * 4 + 2);
    }
  }
  return;
}



// Function at 0049c430

int cls_0x41c7d0::meth_0x49c430()

{
  uint uVar1;
  undefined *puStack00000004;
  undefined local_1c [28];
  
  puStack00000004 = local_1c;
  uVar1 = FUN_0058e8bb(&stack0x00000004,this->mbr_0x2c,this->mbr_0x1c,4,&LAB_0049ab00);
  if (uVar1 == 0) {
    return -1;
  }
  return (int)(uVar1 - this->mbr_0x2c) >> 2;
}



// Function at 0049c560

undefined4 cls_0x41c7d0::meth_0x49c560()

{
  int iVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  dword *pdVar5;
  int in_stack_00000004;
  
  iVar3 = in_stack_00000004;
  if (((-1 < in_stack_00000004) && (in_stack_00000004 <= (int)this->mbr_0x1c)) &&
     (iVar1 = *(int *)(this->mbr_0x2c + in_stack_00000004 * 4), iVar1 != 0)) {
    bVar2 = false;
    pdVar5 = &this->mbr_0x7c;
    in_stack_00000004 = 0x10;
    do {
      if (pdVar5[-0x10] != 0) {
        iVar4 = _AIL_sample_user_data_8(pdVar5[-0x10],0);
        if (iVar3 == iVar4) {
          iVar4 = _AIL_sample_status_4(pdVar5[-0x10]);
          if (iVar4 == 2) {
            _AIL_set_sample_user_data_12(pdVar5[-0x10],0,0xffffffff);
          }
          else {
            bVar2 = true;
          }
        }
      }
      if (*pdVar5 != 0) {
        iVar4 = _AIL_3D_user_data_8(*pdVar5,0);
        if (iVar3 == iVar4) {
          iVar4 = _AIL_3D_sample_status_4(*pdVar5);
          if (iVar4 == 2) {
            _AIL_set_3D_user_data_12(*pdVar5,0,0xffffffff);
          }
          else {
            bVar2 = true;
          }
        }
      }
      pdVar5 = pdVar5 + 1;
      in_stack_00000004 = in_stack_00000004 + -1;
    } while (in_stack_00000004 != 0);
    if (!bVar2) {
      *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) & 0xfffffffb;
      return 1;
    }
  }
  return 0;
}



// Function at 0049c640

undefined4 cls_0x41c7d0::meth_0x49c640()

{
  int iVar1;
  int iVar2;
  dword *pdVar3;
  int in_stack_00000004;
  undefined4 local_8;
  cls_0x41c7d0 *local_4;
  
  iVar2 = 0;
  local_8 = 0;
  pdVar3 = &this->mbr_0x3c;
  local_4 = this;
  do {
    if (*pdVar3 != 0) {
      iVar1 = _AIL_sample_user_data_8(*pdVar3,0);
      if (in_stack_00000004 == iVar1) {
        _AIL_sample_ms_position_12((&local_4->mbr_0x3c)[iVar2],&local_8,0);
        return local_8;
      }
    }
    iVar2 = iVar2 + 1;
    pdVar3 = pdVar3 + 1;
  } while (iVar2 < 0x10);
  return local_8;
}



// Function at 0049c6b0

void cls_0x41c7d0::meth_0x49c6b0(int param_1, int param_2)

{
  int iVar1;
  int iVar2;
  dword *pdVar3;
  int in_stack_0000000c;
  
  iVar1 = param_1;
  if ((((-1 < param_1) && (param_1 < (int)this->mbr_0x1c)) &&
      (*(int *)(this->mbr_0x2c + param_1 * 4) != 0)) && (this->mbr_0x0 != 0)) {
    if (param_2 < 1) {
      param_2 = 1;
    }
    if (in_stack_0000000c < param_2) {
      in_stack_0000000c = param_2;
    }
    pdVar3 = &this->mbr_0x7c;
    param_1 = 0x10;
    do {
      if ((*pdVar3 != 0) && (iVar2 = _AIL_3D_user_data_8(*pdVar3,0), iVar1 == iVar2)) {
        _AIL_set_3D_sample_float_distances_20
                  (*pdVar3,(float)in_stack_0000000c,(float)param_2,(float)in_stack_0000000c,
                   (float)param_2);
      }
      pdVar3 = pdVar3 + 1;
      param_1 = param_1 + -1;
    } while (param_1 != 0);
  }
  return;
}



// Function at 0049c760

void cls_0x41c7d0::meth_0x49c760(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  dword *pdVar4;
  int in_stack_00000008;
  
  iVar2 = param_1;
  if ((((-1 < param_1) && (param_1 < (int)this->mbr_0x1c)) &&
      (*(int *)(this->mbr_0x2c + param_1 * 4) != 0)) && (this->mbr_0x0 != 0)) {
    if (in_stack_00000008 < 0x7f) {
      if (in_stack_00000008 < 0) {
        in_stack_00000008 = 0;
      }
    }
    else {
      in_stack_00000008 = 0x7f;
    }
    pdVar4 = &this->mbr_0x7c;
    param_1 = 0x10;
    do {
      if ((pdVar4[-0x10] != 0) && (iVar3 = _AIL_sample_user_data_8(pdVar4[-0x10],0), iVar2 == iVar3)
         ) {
        uVar1 = DAT_005d7aa0 + -0x7f + in_stack_00000008;
        _AIL_set_sample_volume_8(pdVar4[-0x10],uVar1 & ((int)uVar1 < 0) - 1);
      }
      if ((*pdVar4 != 0) && (iVar3 = _AIL_3D_user_data_8(*pdVar4,0), iVar2 == iVar3)) {
        uVar1 = DAT_005d7aa0 + -0x7f + in_stack_00000008;
        _AIL_set_3D_sample_volume_8(*pdVar4,uVar1 & ((int)uVar1 < 0) - 1);
      }
      pdVar4 = pdVar4 + 1;
      param_1 = param_1 + -1;
    } while (param_1 != 0);
  }
  return;
}



// Function at 0049c8f0

void cls_0x41c7d0::meth_0x49c8f0()

{
  dword *pdVar1;
  int local_4;
  
  local_4 = 0x10;
  pdVar1 = &this->mbr_0x7c;
  do {
    if (pdVar1[-0x10] != 0) {
      _AIL_sample_user_data_8(pdVar1[-0x10],0);
      meth_0x49bd90(this);
    }
    if (*pdVar1 != 0) {
      _AIL_3D_user_data_8(*pdVar1,0);
      meth_0x49bd90(this);
    }
    pdVar1 = pdVar1 + 1;
    local_4 = local_4 + -1;
  } while (local_4 != 0);
  return;
}



