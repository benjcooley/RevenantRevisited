// Decompiled methods and structure for class: cls_0x5b9f28

/*
/OOAnalyzer/cls_0x5b9f28
pack(disabled)
Structure cls_0x5b9f28 {
   0   cls_0x5b9f0c   68   cls_0x5b9f0c   "Component (member) class."
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 84 Alignment: 1

*/

// Function at 005726f0

int __thiscall
OOAnalyzer::cls_0x5b9f28::meth_0x5726f0(cls_0x5b9f28 *this,HANDLE param_1,undefined param_2)

{
  dword dVar1;
  DWORD DVar2;
  uint3 uVar6;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  char cVar7;
  uint uVar8;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  
  dVar1 = (this->cls_0x5b9f0c).mbr_0x38;
  if (dVar1 != 0) {
    FUN_004830f0(dVar1);
    (this->cls_0x5b9f0c).mbr_0x38 = 0;
  }
  if (_param_2 == 0) {
    _param_2 = SetFilePointer(param_1,0,(PLONG)0x0,2);
    if (_param_2 == 0xffffffff) {
      return -0x100;
    }
    DVar2 = SetFilePointer(param_1,0,(PLONG)0x0,0);
    uVar6 = (uint3)(DVar2 >> 8);
    if (DVar2 == 0xffffffff) {
      return (uint)uVar6 << 8;
    }
    if (_param_2 == 0) {
      return (uint)uVar6 << 8;
    }
  }
  dVar1 = (this->cls_0x5b9f0c).mbr_0x18;
  (this->cls_0x5b9f0c).mbr_0x10 = (dword)param_1;
  (this->cls_0x5b9f0c).mbr_0x14 = _param_2;
  uVar3 = (int)((dVar1 - 1) + _param_2) / (int)dVar1;
  (this->cls_0x5b9f0c).mbr_0x1c = uVar3;
  (this->cls_0x5b9f0c).mbr_0x20 = ~-(uint)(in_stack_0000000c != '\0') & uVar3;
  uVar8 = (int)(uVar3 + 7 + ((int)(uVar3 + 7) >> 0x1f & 7U)) >> 3;
  puVar4 = (undefined4 *)FUN_00482fb0(uVar8);
  cVar7 = -(in_stack_0000000c != '\0');
  (this->cls_0x5b9f0c).mbr_0x38 = (dword)puVar4;
  uVar5 = CONCAT22(CONCAT11(cVar7,cVar7),CONCAT11(cVar7,cVar7));
  for (uVar3 = uVar8 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar4 = uVar5;
    puVar4 = puVar4 + 1;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(char *)puVar4 = cVar7;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  }
  return CONCAT31((int3)((uint)uVar5 >> 8),1);
}



// Function at 005729e0

cls_0x5b9f28 * __thiscall
OOAnalyzer::cls_0x5b9f28::cls_0x5b9f28
          (cls_0x5b9f28 *this,undefined4 param_1,undefined4 param_2,LPCSTR param_3,
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
  puVar1 = (undefined4 *)FUN_00482fb0(0x1c);
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

cls_0x5b9f28 * __thiscall OOAnalyzer::cls_0x5b9f28::~cls_0x5b9f28(cls_0x5b9f28 *this)

{
  dword dVar1;
  dword *pdVar2;
  dword *pdVar3;
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
    pdVar3 = (dword *)0x0;
  }
  else {
    pdVar3 = &(this->cls_0x5b9f0c).mbr_0x40;
  }
  dVar1 = *pdVar3;
  if (dVar1 != 0) {
    pdVar2 = (dword *)pdVar3[1];
    *pdVar3 = 0;
    *(dword **)(dVar1 + 4) = pdVar2;
    *pdVar2 = dVar1;
  }
  dVar1 = (this->cls_0x5b9f0c).mbr_0x38;
  local_4 = 0xffffffff;
  (this->cls_0x5b9f0c).vftptr_0x0 = &cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if (dVar1 != 0) {
    FUN_004830f0(dVar1);
  }
  hObject = (cls_0x5b9f28 *)(this->cls_0x5b9f0c).mbr_0x10;
  if (hObject != (cls_0x5b9f28 *)0x0) {
    hObject = (cls_0x5b9f28 *)CloseHandle(hObject);
  }
  dVar1 = (this->cls_0x5b9f0c).mbr_0x34;
  if (dVar1 != 0) {
    hObject = (cls_0x5b9f28 *)FUN_004830f0(dVar1);
  }
  ExceptionList = local_c;
  return hObject;
}



// Function at 00573190

undefined4 __thiscall OOAnalyzer::cls_0x5b9f28::meth_0x573190(cls_0x5b9f28 *this)

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

cls_0x5b9f28 * __thiscall OOAnalyzer::cls_0x5b9f28::virt_meth_0x573d30(cls_0x5b9f28 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9f28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00579ce0

dword __thiscall
OOAnalyzer::cls_0x5b9f28::meth_0x579ce0
          (cls_0x5b9f28 *this,int *param_1,undefined4 param_2,undefined4 param_3,
          cls_0x588410 *param_4,DWORD param_5)

{
  dword *pdVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined *puVar5;
  undefined *puVar6;
  dword dVar7;
  int in_stack_00000018;
  undefined local_60 [28];
  undefined auStack_44 [52];
  undefined4 uStack_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2736;
  local_c = ExceptionList;
  dVar7 = 1;
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
        cls_0x588410::cls_0x588410((cls_0x588410 *)(auStack_44 + 0x1c),param_2);
        uStack_10 = 0;
        local_4 = 0;
        cls_0x588410::cls_0x588410((cls_0x588410 *)(local_60 + 0x1c));
        local_4._0_1_ = 1;
        dVar7 = FUN_00570ce0(param_1,auStack_44 + 0x1c,param_3,local_60 + 0x1c);
        cls_0x588410::meth_0x5891c0(param_4,auStack_44._0_4_);
        local_4 = (uint)local_4._1_3_ << 8;
        cls_0x588410::meth_0x588480((cls_0x588410 *)(local_60 + 0x1c));
        local_4 = 0xffffffff;
        cls_0x588410::meth_0x588480((cls_0x588410 *)(auStack_44 + 0x1c));
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
      puVar6 = (undefined *)(local_60._12_4_ + 1);
      local_4 = 2;
      puVar5 = (undefined *)local_60._12_4_;
      local_60._12_4_ = puVar6;
      if ((uint)local_60._4_4_ < puVar6) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      *puVar5 = 5;
      puVar6 = (undefined *)(local_60._12_4_ + 1);
      puVar5 = (undefined *)local_60._12_4_;
      local_60._12_4_ = puVar6;
      if ((uint)local_60._4_4_ < puVar6) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      uVar2 = *(undefined2 *)&(this->cls_0x5b9f0c).mbr_0x30;
      *puVar5 = param_1._0_1_;
      puVar6 = (undefined *)(local_60._12_4_ + 2);
      puVar5 = (undefined *)local_60._12_4_;
      local_60._12_4_ = puVar6;
      if ((uint)local_60._4_4_ < puVar6) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)local_60);
      }
      *puVar5 = (char)uVar2;
      puVar5[1] = (char)((ushort)uVar2 >> 8);
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
        param_1 = (int *)FUN_00482fb0(0x1c);
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
      dVar7 = (this->cls_0x5b9f0c).mbr_0x38;
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
      dVar7 = 1;
    }
  }
  ExceptionList = local_c;
  return dVar7;
}



