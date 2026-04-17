#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9f28



// Function at 005726f0

int cls_0x5b9f28::meth_0x5726f0(HANDLE param_1, undefined param_2)

{
  LPCVOID pvVar1;
  dword dVar2;
  DWORD DVar3;
  uint3 uVar7;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  char cVar8;
  uint uVar9;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  
  pvVar1 = (LPCVOID)(this->cls_0x5b9f0c).mbr_0x38;
  if (pvVar1 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar1);
    (this->cls_0x5b9f0c).mbr_0x38 = 0;
  }
  if (_param_2 == 0) {
    _param_2 = SetFilePointer(param_1,0,(PLONG)0x0,2);
    if (_param_2 == 0xffffffff) {
      return -0x100;
    }
    DVar3 = SetFilePointer(param_1,0,(PLONG)0x0,0);
    uVar7 = (uint3)(DVar3 >> 8);
    if (DVar3 == 0xffffffff) {
      return (uint)uVar7 << 8;
    }
    if (_param_2 == 0) {
      return (uint)uVar7 << 8;
    }
  }
  dVar2 = (this->cls_0x5b9f0c).mbr_0x18;
  (this->cls_0x5b9f0c).mbr_0x10 = (dword)param_1;
  (this->cls_0x5b9f0c).mbr_0x14 = _param_2;
  uVar4 = (int)((dVar2 - 1) + _param_2) / (int)dVar2;
  (this->cls_0x5b9f0c).mbr_0x1c = uVar4;
  (this->cls_0x5b9f0c).mbr_0x20 = ~-(uint)(in_stack_0000000c != '\0') & uVar4;
  uVar9 = (int)(uVar4 + 7 + ((int)(uVar4 + 7) >> 0x1f & 7U)) >> 3;
  puVar5 = FUN_00482fb0(uVar9);
  cVar8 = -(in_stack_0000000c != '\0');
  (this->cls_0x5b9f0c).mbr_0x38 = (dword)puVar5;
  uVar6 = CONCAT22(CONCAT11(cVar8,cVar8),CONCAT11(cVar8,cVar8));
  for (uVar4 = uVar9 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar5 = uVar6;
    puVar5 = puVar5 + 1;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(char *)puVar5 = cVar8;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  }
  return CONCAT31((int3)((uint)uVar6 >> 8),1);
}



// Function at 005729e0

cls_0x5b9f28::cls_0x5b9f28 *this,undefined4 param_1,undefined4 param_2,LPCSTR param_3,
          undefined4 param_4)

{
  undefined4 *puVar1;
  HANDLE pvVar2;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2313;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5b9f0c::cls_0x5b9f0c(&this->cls_0x5b9f0c,param_1,(undefined)param_2,param_3);
  this->mbr_0x44 = 0;
  (this->cls_0x5b9f0c).mbr_0x40 = 0;
  this->mbr_0x48 = 0;
  (this->cls_0x5b9f0c).vftptr_0x0 =
       (cls_0x5b9f0c__vftable_5b9f0c *)&cls_0x5b9f28__vftable_5b9f28_005b9f28;
  local_4 = 1;
  this->mbr_0x48 = (dword)this;
  puVar1 = FUN_00482fb0(0x1c);
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    puVar1[1] = 1;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    *puVar1 = &PTR_virt_meth_0x573f00_005b9f18;
    puVar1[6] = 0;
    puVar1[4] = puVar1;
  }
  this->mbr_0x50 = (dword)puVar1;
  pvVar2 = CreateFileA(param_3,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0,(HANDLE)0x0);
  if (pvVar2 != (HANDLE)0xffffffff) {
    meth_0x5726f0(this,pvVar2,0);
  }
  this->mbr_0x4c = 0;
  ExceptionList = pvStack_c;
  return this;
}



// Function at 00572ab0

cls_0x5b9f28 * __thiscall cls_0x5b9f28::~cls_0x5b9f28(cls_0x5b9f28 *this)

{
  dword dVar1;
  dword *pdVar2;
  LPCVOID pvVar3;
  dword *pdVar4;
  cls_0x5b9f28 *hObject;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a234c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5b9f0c).vftptr_0x0 =
       (cls_0x5b9f0c__vftable_5b9f0c *)&cls_0x5b9f28__vftable_5b9f28_005b9f28;
  local_4 = 1;
  if ((int *)this->mbr_0x50 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x50 + 8))();
  }
  if (this == (cls_0x5b9f28 *)0x0) {
    pdVar4 = (dword *)0x0;
  }
  else {
    pdVar4 = &(this->cls_0x5b9f0c).mbr_0x40;
  }
  dVar1 = *pdVar4;
  if (dVar1 != 0) {
    pdVar2 = (dword *)pdVar4[1];
    *pdVar4 = 0;
    *(dword **)(dVar1 + 4) = pdVar2;
    *pdVar2 = dVar1;
  }
  pvVar3 = (LPCVOID)(this->cls_0x5b9f0c).mbr_0x38;
  local_4 = 0xffffffff;
  (this->cls_0x5b9f0c).vftptr_0x0 = &cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if (pvVar3 != (LPCVOID)0x0) {
    FUN_004830f0(pvVar3);
  }
  hObject = (cls_0x5b9f28 *)(this->cls_0x5b9f0c).mbr_0x10;
  if (hObject != (cls_0x5b9f28 *)0x0) {
    hObject = (cls_0x5b9f28 *)CloseHandle(hObject);
  }
  pvVar3 = (LPCVOID)(this->cls_0x5b9f0c).mbr_0x34;
  if (pvVar3 != (LPCVOID)0x0) {
    hObject = (cls_0x5b9f28 *)FUN_004830f0(pvVar3);
  }
  ExceptionList = local_c;
  return hObject;
}



// Function at 00573190

undefined4 cls_0x5b9f28::meth_0x573190()

{
  DWORD DVar1;
  dword in_stack_00000004;
  
  if (this->mbr_0x4c == 1) {
    DVar1 = SetFilePointer((HANDLE)(this->cls_0x5b9f0c).mbr_0x10,in_stack_00000004,(PLONG)0x0,0);
    if (DVar1 == 0xffffffff) {
      return 0;
    }
    SetEndOfFile((HANDLE)(this->cls_0x5b9f0c).mbr_0x10);
    meth_0x5726f0(this,(this->cls_0x5b9f0c).mbr_0x10,(char)in_stack_00000004);
    this->mbr_0x4c = 2;
    (this->cls_0x5b9f0c).mbr_0x2c = 5;
    (this->cls_0x5b9f0c).mbr_0x28 = 0;
  }
  else if ((this->mbr_0x4c == 2) && ((this->cls_0x5b9f0c).mbr_0x14 == in_stack_00000004)) {
    return 0;
  }
  return 1;
}



// Function at 00573d30

cls_0x5b9f28 * cls_0x5b9f28::virt_meth_0x573d30()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9f28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00579ce0

dword cls_0x5b9f28::meth_0x579ce0(int *param_1, undefined4 param_2, cls_0x588410 *param_3, cls_0x588410 *param_4, DWORD param_5)

{
  dword *pdVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined *puVar5;
  char *pcVar6;
  char *pcVar7;
  dword dVar8;
  int in_stack_00000018;
  undefined local_60 [80];
  undefined4 uStack_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2736;
  local_c = ExceptionList;
  dVar8 = 1;
  if ((DAT_00676828 == 0) || (DAT_0067682c != 0)) {
    if (DAT_0067682c != 0) {
      ExceptionList = &local_c;
      WaitForSingleObject(DAT_00676864,0xffffffff);
      in_stack_00000018 = 0;
      param_5 = 4;
      iVar3 = (**(code **)(*DAT_00676738 + 0x50))(DAT_00676738,1,&stack0x00000018,&param_5,1);
      iVar4 = in_stack_00000018;
      if (iVar3 != 0) {
        iVar4 = FUN_00570ad0(1);
      }
      if (iVar4 != 0) {
        cls_0x588410::cls_0x588410((cls_0x588410 *)(local_60 + 0x38),param_2);
        uStack_10 = 0;
        local_4 = 0;
        cls_0x588410::cls_0x588410((cls_0x588410 *)(local_60 + 0x1c));
        local_4._0_1_ = 1;
        dVar8 = FUN_00570ce0(iVar4,(char)param_1,(cls_0x588410 *)(local_60 + 0x38),param_3,
                             (cls_0x588410 *)(local_60 + 0x1c));
        cls_0x588410::meth_0x5891c0(param_4,local_60._28_4_);
        local_4 = (uint)local_4._1_3_ << 8;
        cls_0x588410::meth_0x588480((cls_0x588410 *)(local_60 + 0x1c));
        local_4 = 0xffffffff;
        cls_0x588410::meth_0x588480((cls_0x588410 *)(local_60 + 0x38));
      }
      ReleaseMutex(DAT_00676864);
    }
  }
  else {
    ExceptionList = &local_c;
    cls_0x5ba038::meth_0x57c8f0((cls_0x5ba038 *)this);
    if ((this->cls_0x5b9f0c).mbr_0x2c == 0) {
      cls_0x588410::cls_0x588410((cls_0x588410 *)local_60);
      pdVar1 = &(this->cls_0x5b9f0c).mbr_0x30;
      *(short *)pdVar1 = *(short *)pdVar1 + 1;
      pcVar6 = (char *)(local_60._12_4_ + 1);
      local_4 = 2;
      puVar5 = (undefined *)local_60._12_4_;
      local_60._12_4_ = pcVar6;
      if ((uint)local_60._4_4_ < pcVar6) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      *puVar5 = 5;
      pcVar7 = (char *)(local_60._12_4_ + 1);
      pcVar6 = (char *)local_60._12_4_;
      local_60._12_4_ = pcVar7;
      if ((uint)local_60._4_4_ < pcVar7) {
        pcVar6 = (char *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      uVar2 = *(undefined2 *)&(this->cls_0x5b9f0c).mbr_0x30;
      *pcVar6 = (char)param_1;
      pcVar7 = (char *)(local_60._12_4_ + 2);
      pcVar6 = (char *)local_60._12_4_;
      local_60._12_4_ = pcVar7;
      if ((uint)local_60._4_4_ < pcVar7) {
        pcVar6 = (char *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      *pcVar6 = (char)uVar2;
      pcVar6[1] = (char)((ushort)uVar2 >> 8);
      cls_0x588410::meth_0x588a90((cls_0x588410 *)local_60,param_3);
      cls_0x588410::meth_0x5886d0((cls_0x588410 *)local_60,param_2);
      if (in_stack_00000018 == 0) {
        iVar4 = local_60._12_4_ - local_60._0_4_;
        WaitForSingleObject(DAT_00676864,0xffffffff);
        (**(code **)(*DAT_00676738 + 0xc4))
                  (DAT_00676738,DAT_00676884,1,0x600,local_60._0_4_,iVar4,0,1000,0,0);
        ReleaseMutex(DAT_00676864);
      }
      else {
        param_1 = FUN_00482fb0(0x1c);
        if (param_1 == (int *)0x0) {
          param_1 = (int *)0x0;
        }
        else {
          param_1[1] = 1;
          param_1[2] = 0;
          param_1[3] = 0;
          param_1[4] = 0;
          *param_1 = (int)&PTR_virt_meth_0x573f00_005b9f18;
          param_1[6] = 0;
          param_1[4] = (int)param_1;
        }
        cls_0x578fe0::meth_0x578fe0((cls_0x578fe0 *)&DAT_00676738);
      }
      (this->cls_0x5b9f0c).mbr_0x34 = 0xffffffff;
      (this->cls_0x5b9f0c).mbr_0x2c = (dword)param_4;
      (this->cls_0x5b9f0c).mbr_0x38 = 2;
      cls_0x5ba038::meth_0x57c900((cls_0x5ba038 *)this);
      WaitForSingleObject((HANDLE)(this->cls_0x5b9f0c).mbr_0x28,param_5);
      cls_0x5ba038::meth_0x57c8f0((cls_0x5ba038 *)this);
      dVar8 = (this->cls_0x5b9f0c).mbr_0x38;
      (this->cls_0x5b9f0c).mbr_0x2c = 0;
      cls_0x5ba038::meth_0x57c900((cls_0x5ba038 *)this);
      if (param_1 != (int *)0x0) {
        WaitForSingleObject(DAT_00676864,0xffffffff);
        if (param_1[6] == 1) {
          (**(code **)(*DAT_00676738 + 0xcc))(DAT_00676738,param_1[5],0);
          param_1[6] = 3;
        }
        ReleaseMutex(DAT_00676864);
        (**(code **)(*param_1 + 8))();
      }
      local_4 = 0xffffffff;
      cls_0x588410::meth_0x588480((cls_0x588410 *)local_60);
    }
    else {
      cls_0x5ba038::meth_0x57c900((cls_0x5ba038 *)this);
      dVar8 = 1;
    }
  }
  ExceptionList = local_c;
  return dVar8;
}



