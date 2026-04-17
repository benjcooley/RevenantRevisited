// Decompiled methods and structure for class: cls_0x5b9f34

/*
/OOAnalyzer/cls_0x5b9f34
pack(disabled)
Structure cls_0x5b9f34 {
   0   cls_0x5b9f0c   68   cls_0x5b9f0c   "Component (member) class."
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 80 Alignment: 1

*/

// Function at 00572e20

cls_0x5b9f34 * __thiscall
OOAnalyzer::cls_0x5b9f34::cls_0x5b9f34
          (cls_0x5b9f34 *this,undefined4 param_1,char *param_2,undefined4 param_3)

{
  char cVar1;
  HANDLE pvVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  undefined4 *puVar7;
  CHAR local_10c;
  undefined4 local_10b [63];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2381;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  cls_0x5b9f0c::cls_0x5b9f0c(&this->cls_0x5b9f0c,param_1,0,param_2);
  local_4 = 0;
  this->mbr_0x44 = 0;
  (this->cls_0x5b9f0c).mbr_0x40 = 0;
  this->mbr_0x48 = 0;
  (this->cls_0x5b9f0c).vftptr_0x0 =
       (cls_0x5b9f0c__vftable_5b9f0c *)&cls_0x5b9f34__vftable_5b9f34_005b9f34;
  this->mbr_0x48 = (dword)this;
  (this->cls_0x5b9f0c).mbr_0x8 = DAT_006766f4;
  DAT_006766f4 = DAT_006766f4 + 1;
  uVar3 = 0xffffffff;
  do {
    pcVar6 = param_2;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = param_2 + 1;
    cVar1 = *param_2;
    param_2 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  local_10c = '_';
  puVar5 = (undefined4 *)(pcVar6 + -uVar3);
  puVar7 = local_10b;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar7 = puVar7 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar7 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  }
  local_4 = CONCAT31(local_4._1_3_,1);
  pvVar2 = CreateFileA(&local_10c,0x40000000,2,(LPSECURITY_ATTRIBUTES)0x0,4,0,(HANDLE)0x0);
  if (pvVar2 != (HANDLE)0xffffffff) {
    cls_0x5b9f28::meth_0x5726f0((cls_0x5b9f28 *)this,pvVar2,1);
  }
  (this->cls_0x5b9f0c).mbr_0x28 = 0;
  (this->cls_0x5b9f0c).mbr_0x2c = 5;
  this->mbr_0x4c = 1;
  ExceptionList = pvStack_c;
  return this;
}



// Function at 00572f20

void __thiscall OOAnalyzer::cls_0x5b9f34::meth_0x572f20(cls_0x5b9f34 *this)

{
  undefined2 uVar1;
  bool bVar2;
  undefined *puVar3;
  undefined uVar4;
  undefined *puVar5;
  undefined *puVar6;
  dword dVar7;
  byte *pbVar8;
  byte local_9;
  undefined4 local_8;
  int local_4;
  
  DAT_006769cc = DAT_006769c0 + 1;
  DAT_006769d0._0_4_ = 0;
  puVar5 = DAT_006769c0;
  puVar6 = DAT_006769cc;
  if (DAT_006769c4 < DAT_006769cc) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
    puVar6 = DAT_006769cc;
  }
  puVar3 = DAT_006769c4;
  *puVar5 = 4;
  DAT_006769cc = puVar6 + 1;
  puVar5 = DAT_006769cc;
  if (puVar3 < DAT_006769cc) {
    puVar6 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
    puVar5 = DAT_006769cc;
  }
  puVar3 = DAT_006769c4;
  *puVar6 = 3;
  dVar7 = (this->cls_0x5b9f0c).mbr_0x8;
  DAT_006769cc = puVar5 + 4;
  puVar6 = DAT_006769cc;
  if (puVar3 < DAT_006769cc) {
    puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
    puVar6 = DAT_006769cc;
  }
  local_8._2_1_ = (undefined)(dVar7 >> 0x10);
  *puVar5 = (char)dVar7;
  pbVar8 = (byte *)(this->cls_0x5b9f0c).mbr_0x38;
  local_9 = 1;
  puVar5[1] = (char)(dVar7 >> 8);
  puVar5[2] = local_8._2_1_;
  local_8._3_1_ = (undefined)(dVar7 >> 0x18);
  uVar4 = local_8._3_1_;
  local_8 = 0;
  dVar7 = (this->cls_0x5b9f0c).mbr_0x1c;
  puVar5[3] = uVar4;
  bVar2 = false;
  if (0 < (int)dVar7) {
    do {
      if ((local_9 & *pbVar8) == 0) {
        if (bVar2) {
          DAT_006769cc = puVar6 + 2;
          puVar5 = DAT_006769cc;
          if (DAT_006769c4 < DAT_006769cc) {
            puVar6 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
            puVar5 = DAT_006769cc;
          }
          *puVar6 = (char)local_4;
          puVar6[1] = (char)((uint)local_4 >> 8);
          DAT_006769cc = puVar5 + 2;
          if (DAT_006769c4 < DAT_006769cc) {
            puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
          }
          *puVar5 = (char)local_8;
          puVar5[1] = (char)((uint)local_8 >> 8);
          puVar6 = DAT_006769cc;
        }
        if (((local_9 == 1) && (*pbVar8 == 0xff)) &&
           (local_8 < (int)((this->cls_0x5b9f0c).mbr_0x1c - 8))) {
          local_8 = local_8 + 8;
          pbVar8 = pbVar8 + 1;
        }
        else {
          local_9 = local_9 << 1;
          if (local_9 == 0) {
            pbVar8 = pbVar8 + 1;
            local_9 = 1;
          }
          local_8 = local_8 + 1;
        }
        bVar2 = false;
      }
      else {
        if (!bVar2) {
          local_4 = local_8;
        }
        if (((local_9 == 1) && (*pbVar8 == 0xff)) && (local_8 < (int)(dVar7 - 8))) {
          local_8 = local_8 + 8;
          pbVar8 = pbVar8 + 1;
          bVar2 = true;
        }
        else {
          local_9 = local_9 << 1;
          if (local_9 == 0) {
            pbVar8 = pbVar8 + 1;
            local_9 = 1;
          }
          local_8 = local_8 + 1;
          bVar2 = true;
        }
      }
      dVar7 = (this->cls_0x5b9f0c).mbr_0x1c;
    } while (local_8 < (int)dVar7);
    if (bVar2) {
      DAT_006769cc = puVar6 + 2;
      puVar5 = DAT_006769cc;
      if (DAT_006769c4 < DAT_006769cc) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
        puVar5 = DAT_006769cc;
      }
      uVar1 = *(undefined2 *)&(this->cls_0x5b9f0c).mbr_0x1c;
      *puVar6 = (char)local_4;
      puVar6[1] = (char)((uint)local_4 >> 8);
      DAT_006769cc = puVar5 + 2;
      if (DAT_006769c4 < DAT_006769cc) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
      }
      *puVar5 = (char)uVar1;
      puVar5[1] = (char)((ushort)uVar1 >> 8);
      puVar6 = DAT_006769cc;
    }
  }
  DAT_006769cc = puVar6 + 2;
  if (DAT_006769c4 < DAT_006769cc) {
    puVar6 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&DAT_006769c0);
  }
  *puVar6 = 0xff;
  puVar6[1] = 0xff;
  cls_0x5756d0::meth_0x578ec0((cls_0x5756d0 *)&DAT_00676738,(this->cls_0x5b9f0c).mbr_0x4);
  return;
}



// Function at 00573210

bool __thiscall OOAnalyzer::cls_0x5b9f34::virt_meth_0x573210(cls_0x5b9f34 *this)

{
  dword *pdVar1;
  code *pcVar2;
  int iVar3;
  uint in_stack_00000004;
  dword dVar4;
  dword dVar5;
  
  if (in_stack_00000004 < (this->cls_0x5b9f0c).mbr_0x28) {
    return true;
  }
  if (this->mbr_0x4c == 1) {
    if (0 < (int)(this->cls_0x5b9f0c).mbr_0x2c) {
      FUN_00578f40(&DAT_00676738,(this->cls_0x5b9f0c).mbr_0x4,s_1d_1d_4d_s_005e5a78,4,1,
                   (this->cls_0x5b9f0c).mbr_0x8,(this->cls_0x5b9f0c).mbr_0x34);
      (this->cls_0x5b9f0c).mbr_0x28 = in_stack_00000004 + 1000;
      (this->cls_0x5b9f0c).mbr_0x2c = (this->cls_0x5b9f0c).mbr_0x2c - 1;
      goto LAB_005732c8;
    }
    if ((this->cls_0x5b9f0c).mbr_0x3c == 0) goto LAB_005732c8;
    dVar5 = (this->cls_0x5b9f0c).mbr_0x8;
    dVar4 = (this->cls_0x5b9f0c).mbr_0x4;
  }
  else {
    if (this->mbr_0x4c != 2) goto LAB_005732c8;
    if (0 < (int)(this->cls_0x5b9f0c).mbr_0x2c) {
      meth_0x572f20(this);
      pdVar1 = &(this->cls_0x5b9f0c).mbr_0x2c;
      *pdVar1 = *pdVar1 - 1;
      goto LAB_005732c8;
    }
    if ((this->cls_0x5b9f0c).mbr_0x3c == 0) goto LAB_005732c8;
    dVar5 = (this->cls_0x5b9f0c).mbr_0x8;
    dVar4 = (this->cls_0x5b9f0c).mbr_0x4;
  }
  FUN_00578f40(&DAT_00676738,dVar4,s_1d_1d_4d_005e5a6c,4,0,dVar5);
  (this->cls_0x5b9f0c).mbr_0x3c = 0;
LAB_005732c8:
  pcVar2 = (code *)(this->cls_0x5b9f0c).mbr_0xc;
  if (pcVar2 != (code *)0x0) {
    dVar5 = (this->cls_0x5b9f0c).mbr_0x1c;
    if ((int)dVar5 < 1) {
      iVar3 = 1000;
    }
    else {
      iVar3 = (int)((this->cls_0x5b9f0c).mbr_0x20 * 1000) / (int)dVar5;
    }
    iVar3 = (*pcVar2)((this->cls_0x5b9f0c).mbr_0x34,(this->cls_0x5b9f0c).mbr_0x3c,iVar3,
                      (this->cls_0x5b9f0c).mbr_0x30);
    if (iVar3 == 0) {
      return iVar3 != 0;
    }
  }
  dVar5 = (this->cls_0x5b9f0c).mbr_0x3c;
  if ((dVar5 != 0) && (dVar5 != 3)) {
    return true;
  }
  return false;
}



// Function at 00573d50

cls_0x5b9f34 * __thiscall OOAnalyzer::cls_0x5b9f34::virt_meth_0x573d50(cls_0x5b9f34 *this)

{
  byte in_stack_00000004;
  
  cls_0x5b9f0c::~cls_0x5b9f0c(&this->cls_0x5b9f0c);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



