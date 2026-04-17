// Decompiled methods and structure for class: cls_0x51ffa0

/*
/OOAnalyzer/cls_0x51ffa0
pack(disabled)
Structure cls_0x51ffa0 {
   2   word   2   mbr_0x2   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0051ffa0

void __thiscall OOAnalyzer::cls_0x51ffa0::meth_0x51ffa0(cls_0x51ffa0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int in_stack_00000004;
  
  if (in_stack_00000004 < 0) {
    in_stack_00000004 = (short)this->mbr_0x2 + 4;
  }
  else if (in_stack_00000004 < (short)this->mbr_0x2) {
    return;
  }
  puVar1 = FUN_00482fb0(in_stack_00000004 * 8);
  iVar2 = (int)(short)this->mbr_0x2;
  puVar6 = puVar1 + iVar2 * 2;
  for (uVar3 = (uint)((in_stack_00000004 - iVar2) * 8) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
    *(undefined *)puVar6 = 0;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  }
  puVar6 = (undefined4 *)this->mbr_0x4;
  if (puVar6 != (undefined4 *)0x0) {
    puVar5 = puVar6;
    puVar7 = puVar1;
    for (uVar3 = (uint)(iVar2 * 8) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar7 = puVar7 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar7 = *(undefined *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    FUN_004830f0(puVar6);
  }
  this->mbr_0x4 = (dword)puVar1;
  this->mbr_0x2 = (word)in_stack_00000004;
  return;
}



