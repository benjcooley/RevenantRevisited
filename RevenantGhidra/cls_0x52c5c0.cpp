// Decompiled methods and structure for class: cls_0x52c5c0

/*
/OOAnalyzer/cls_0x52c5c0
pack(disabled)
Structure cls_0x52c5c0 {
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 104 Alignment: 1

*/

// Function at 0052c5c0

void __thiscall OOAnalyzer::cls_0x52c5c0::meth_0x52c5c0(cls_0x52c5c0 *this)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  uint *puVar5;
  uint uVar6;
  int **ppiVar7;
  cls_0x5a5ff0 local_20;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18e4;
  local_c = ExceptionList;
  if (this->mbr_0x64 != 0) {
    ExceptionList = &local_c;
    puVar3 = FUN_00482fb0(0xc);
    local_4 = 0;
    if (puVar3 != (undefined4 *)0x0) {
      puVar4 = (undefined4 *)this->mbr_0x64;
      *puVar3 = *puVar4;
      cls_0x5a5ff0::cls_0x5a5ff0(&local_20,0x2080);
      piVar2 = puVar3 + 1;
      local_4._0_1_ = 1;
      FUN_005295e0((short *)(puVar4 + 1),0x1040,piVar2,&local_20);
      puVar4 = FUN_00482ef0(*piVar2 << 1);
      iVar1 = *piVar2;
      puVar3[2] = puVar4;
      puVar4 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_20);
      local_4 = (uint)local_4._1_3_ << 8;
      puVar3 = (undefined4 *)puVar3[2];
      for (uVar6 = (uint)(iVar1 << 1) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *puVar3 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar3 = puVar3 + 1;
      }
      for (uVar6 = iVar1 << 1 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined *)puVar3 = *(undefined *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + 1);
        puVar3 = (undefined4 *)((int)puVar3 + 1);
      }
      cls_0x5a5ff0::~cls_0x5a5ff0(&local_20);
    }
    local_4 = 0xffffffff;
    puVar5 = (uint *)(this->mbr_0x60 + 0x24);
    uVar6 = 0;
    ppiVar7 = *(int ***)(this->mbr_0x60 + 0x34);
    while( true ) {
      if ((puVar5 == (uint *)0x0) || (*puVar5 <= uVar6)) goto LAB_0052c6f2;
      if (**ppiVar7 == *(int *)this->mbr_0x64) break;
      ppiVar7 = ppiVar7 + 1;
      uVar6 = uVar6 + 1;
    }
    piVar2 = *ppiVar7;
    if (piVar2 != (int *)0x0) {
      *piVar2 = -1;
      if ((LPCVOID)piVar2[2] != (LPCVOID)0x0) {
        FUN_00482f80((LPCVOID)piVar2[2]);
      }
      piVar2[2] = 0;
      piVar2[1] = 0;
      FUN_004830f0(piVar2);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
LAB_0052c6f2:
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
  }
  ExceptionList = local_c;
  return;
}



