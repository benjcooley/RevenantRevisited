// Decompiled methods and structure for class: cls_0x532210

/*
/OOAnalyzer/cls_0x532210
pack(disabled)
Structure cls_0x532210 {
   404   word   2   mbr_0x194   "Unsigned Word (dw, 2-bytes)"
   406   word   2   mbr_0x196   "Unsigned Word (dw, 2-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 412 Alignment: 1

*/

// Function at 00532210

void __thiscall OOAnalyzer::cls_0x532210::meth_0x532210(cls_0x532210 *this)

{
  byte bVar1;
  word wVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  undefined4 *puVar7;
  LPCVOID *ppvVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  bool bVar11;
  byte *in_stack_00000004;
  uint local_8;
  int local_4;
  
  local_8 = 0;
  if (0 < (short)this->mbr_0x194) {
    local_4 = 0;
    do {
      ppvVar8 = (LPCVOID *)(local_4 + this->mbr_0x198);
      pbVar3 = (byte *)ppvVar8[1];
      pbVar6 = in_stack_00000004;
      do {
        bVar1 = *pbVar3;
        bVar11 = bVar1 < *pbVar6;
        if (bVar1 != *pbVar6) {
LAB_00532267:
          iVar4 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
          goto LAB_0053226c;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar3[1];
        bVar11 = bVar1 < pbVar6[1];
        if (bVar1 != pbVar6[1]) goto LAB_00532267;
        pbVar3 = pbVar3 + 2;
        pbVar6 = pbVar6 + 2;
      } while (bVar1 != 0);
      iVar4 = 0;
LAB_0053226c:
      if (iVar4 == 0) {
        FUN_0052f310(ppvVar8);
        wVar2 = this->mbr_0x194;
        if (local_8 < (uint)(int)(short)wVar2) {
          iVar4 = (short)this->mbr_0x196 + -1;
          if ((int)local_8 < iVar4) {
            iVar4 = iVar4 - local_8;
            puVar9 = (undefined4 *)(this->mbr_0x198 + local_4);
            do {
              iVar4 = iVar4 + -1;
              puVar7 = puVar9 + 0x12;
              puVar10 = puVar9;
              for (iVar5 = 0x12; iVar5 != 0; iVar5 = iVar5 + -1) {
                *puVar10 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar10 = puVar10 + 1;
              }
              puVar9 = puVar9 + 0x12;
            } while (iVar4 != 0);
          }
          wVar2 = wVar2 - 1;
          this->mbr_0x194 = wVar2;
          if ((short)wVar2 < 1) {
            if ((LPCVOID)this->mbr_0x198 != (LPCVOID)0x0) {
              FUN_004830f0((LPCVOID)this->mbr_0x198);
            }
            this->mbr_0x198 = 0;
            this->mbr_0x194 = 0;
            this->mbr_0x196 = 0;
          }
        }
        local_8 = local_8 - 1;
        local_4 = local_4 + -0x48;
      }
      local_8 = local_8 + 1;
      local_4 = local_4 + 0x48;
    } while ((int)local_8 < (int)(short)this->mbr_0x194);
  }
  return;
}



