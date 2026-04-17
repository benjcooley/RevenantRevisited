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
  undefined4 uVar4;
  undefined4 *puVar5;
  uint *puVar6;
  uint uVar7;
  int **ppiVar8;
  cls_0x5a5ff0 local_20;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18e4;
  local_c = ExceptionList;
  if (this->mbr_0x64 != 0) {
    ExceptionList = &local_c;
    puVar3 = (undefined4 *)FUN_00482fb0(0xc);
    local_4 = 0;
    if (puVar3 != (undefined4 *)0x0) {
      puVar5 = (undefined4 *)this->mbr_0x64;
      *puVar3 = *puVar5;
      cls_0x5a5ff0::cls_0x5a5ff0(&local_20,0x2080);
      piVar2 = puVar3 + 1;
      local_4._0_1_ = 1;
      FUN_005295e0(puVar5 + 1,0x1040,piVar2,&local_20);
      uVar4 = FUN_00482ef0(*piVar2 << 1);
      iVar1 = *piVar2;
      puVar3[2] = uVar4;
      puVar5 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_20);
      local_4 = (uint)local_4._1_3_ << 8;
      puVar3 = (undefined4 *)puVar3[2];
      for (uVar7 = (uint)(iVar1 << 1) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        *puVar3 = *puVar5;
        puVar5 = puVar5 + 1;
        puVar3 = puVar3 + 1;
      }
      for (uVar7 = iVar1 << 1 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined *)puVar3 = *(undefined *)puVar5;
        puVar5 = (undefined4 *)((int)puVar5 + 1);
        puVar3 = (undefined4 *)((int)puVar3 + 1);
      }
      cls_0x5a5ff0::~cls_0x5a5ff0(&local_20);
    }
    local_4 = 0xffffffff;
    puVar6 = (uint *)(this->mbr_0x60 + 0x24);
    uVar7 = 0;
    ppiVar8 = *(int ***)(this->mbr_0x60 + 0x34);
    while( true ) {
      if ((puVar6 == (uint *)0x0) || (*puVar6 <= uVar7)) goto LAB_0052c6f2;
      if (**ppiVar8 == *(int *)this->mbr_0x64) break;
      ppiVar8 = ppiVar8 + 1;
      uVar7 = uVar7 + 1;
    }
    piVar2 = *ppiVar8;
    if (piVar2 != (int *)0x0) {
      *piVar2 = -1;
      if (piVar2[2] != 0) {
        FUN_00482f80(piVar2[2]);
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



