// Decompiled methods and structure for class: cls_0x497c40

/*
/OOAnalyzer/cls_0x497c40
pack(disabled)
Structure cls_0x497c40 {
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32792 Alignment: 1

*/

// Function at 00497c40

void __thiscall OOAnalyzer::cls_0x497c40::meth_0x497c40(cls_0x497c40 *this)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  dword dVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  byte *pbVar9;
  int iVar10;
  bool bVar11;
  byte *in_stack_00000004;
  int local_4;
  
  iVar8 = 0;
  local_4 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      dVar4 = this->mbr_0x8014;
      iVar10 = 0;
      if (0 < *(int *)(*(int *)(dVar4 + iVar8) + 0xc)) {
        do {
          iVar6 = *(int *)(*(int *)(*(int *)(dVar4 + iVar8) + 0x1c) + iVar10 * 4);
          if (iVar6 == 0) {
            iVar6 = *(int *)(*(int *)(dVar4 + iVar8) + 0x20);
          }
          pbVar5 = (byte *)(iVar6 + 0x3c);
          pbVar9 = in_stack_00000004;
          do {
            bVar1 = *pbVar5;
            bVar11 = bVar1 < *pbVar9;
            if (bVar1 != *pbVar9) {
LAB_00497cb3:
              iVar6 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
              goto LAB_00497cb8;
            }
            if (bVar1 == 0) break;
            bVar1 = pbVar5[1];
            bVar11 = bVar1 < pbVar9[1];
            if (bVar1 != pbVar9[1]) goto LAB_00497cb3;
            pbVar5 = pbVar5 + 2;
            pbVar9 = pbVar9 + 2;
          } while (bVar1 != 0);
          iVar6 = 0;
LAB_00497cb8:
          if ((iVar6 == 0) && (iVar6 = FUN_00451d70(in_stack_00000004,1,0), iVar6 != 0)) {
            iVar2 = *(int *)(this->mbr_0x8014 + iVar8);
            iVar7 = *(int *)(*(int *)(iVar2 + 0x1c) + iVar10 * 4);
            if (iVar7 == 0) {
              iVar7 = *(int *)(iVar2 + 0x20);
            }
            *(undefined4 *)(iVar7 + 0x1c) = *(undefined4 *)(iVar6 + 0x10);
            *(undefined4 *)(iVar7 + 0x20) = *(undefined4 *)(iVar6 + 0x14);
            *(undefined4 *)(iVar7 + 0x24) = *(undefined4 *)(iVar6 + 0x18);
            iVar7 = *(int *)(*(int *)(iVar2 + 0x1c) + iVar10 * 4);
            if (iVar7 == 0) {
              iVar7 = *(int *)(iVar2 + 0x20);
            }
            *(undefined4 *)(iVar7 + 0x28) = *(undefined4 *)(iVar6 + 0xd8);
            uVar3 = *(undefined4 *)(iVar6 + 0xe0);
            *(undefined4 *)(iVar7 + 0x2c) = *(undefined4 *)(iVar6 + 0xdc);
            *(undefined4 *)(iVar7 + 0x30) = uVar3;
          }
          dVar4 = this->mbr_0x8014;
          iVar10 = iVar10 + 1;
        } while (iVar10 < *(int *)(*(int *)(dVar4 + iVar8) + 0xc));
      }
      local_4 = local_4 + 1;
      iVar8 = iVar8 + 4;
    } while (local_4 < (int)this->mbr_0x8004);
  }
  return;
}



