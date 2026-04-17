// Decompiled methods and structure for class: cls_0x531fc0

/*
/OOAnalyzer/cls_0x531fc0
pack(disabled)
Structure cls_0x531fc0 {
   380   byte   1   mbr_0x17c   "Unsigned Byte (db)"
   404   word   2   mbr_0x194   "Unsigned Word (dw, 2-bytes)"
   406   word   2   mbr_0x196   "Unsigned Word (dw, 2-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 440 Alignment: 1

*/

// Function at 00531fc0

void __thiscall OOAnalyzer::cls_0x531fc0::meth_0x531fc0(cls_0x531fc0 *this)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 in_stack_00000004;
  int in_stack_00000008;
  int in_stack_0000000c;
  cls_0x477870 local_64;
  undefined4 auStack_48 [18];
  
  local_64.mbr_0x4 = this->mbr_0x1b4;
  if (local_64.mbr_0x4 != 0) {
    local_64.mbr_0x0 = 1;
    local_64.mbr_0x10 = 0;
    local_64.mbr_0xc = 0;
    local_64.mbr_0x14 = 0;
    local_64.mbr_0x18 = 0;
    local_64.mbr_0x8 = local_64.mbr_0x4;
    cls_0x477870::meth_0x46dfb0(&local_64);
    piVar2 = (int *)local_64.mbr_0x18;
    while (piVar2 != (int *)0x0) {
      local_64.mbr_0x18 = (dword)piVar2;
      if (((*(short *)(piVar2 + 0x1f) < 0x100) &&
          (iVar3 = (**(code **)(*piVar2 + 0xd4))(s_SaleType_005e3e80), iVar3 != 2)) &&
         (iVar3 = (**(code **)(*piVar2 + 400))(), iVar3 != -1)) {
        switch(*(undefined2 *)(piVar2 + 1)) {
        case 1:
          bVar1 = this->mbr_0x17c & 8;
          break;
        case 2:
          if (((*(uint *)&this->mbr_0x17c & 4) == 0) && ((*(uint *)&this->mbr_0x17c & 0x10) == 0))
          goto switchD_0053205b_caseD_3;
          goto LAB_00532092;
        default:
          goto switchD_0053205b_caseD_3;
        case 4:
        case 5:
        case 0x11:
        case 0x12:
        case 0x15:
          bVar1 = this->mbr_0x17c & 0x10;
        }
        if (bVar1 != 0) {
LAB_00532092:
          iVar3 = (**(code **)(*piVar2 + 0xd4))(in_stack_00000004);
          if ((in_stack_00000008 <= iVar3) && (iVar3 <= in_stack_0000000c)) {
            iVar3 = piVar2[0xe];
            uVar4 = (**(code **)(*piVar2 + 0x198))();
            iVar3 = FUN_0052da90(iVar3,*(undefined4 *)&this->mbr_0x17c,uVar4);
            if (iVar3 != 0) {
              if ((short)this->mbr_0x196 <= (short)this->mbr_0x194) {
                iVar8 = (short)this->mbr_0x196 + 4;
                puVar5 = (undefined4 *)FUN_00482fb0(iVar8 * 0x48);
                iVar3 = (int)(short)this->mbr_0x196;
                puVar10 = puVar5 + iVar3 * 0x12;
                for (uVar6 = (uint)((iVar8 - iVar3) * 0x48) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
                  *puVar10 = 0;
                  puVar10 = puVar10 + 1;
                }
                for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
                  *(undefined *)puVar10 = 0;
                  puVar10 = (undefined4 *)((int)puVar10 + 1);
                }
                puVar10 = (undefined4 *)this->mbr_0x198;
                if (puVar10 != (undefined4 *)0x0) {
                  puVar9 = puVar10;
                  puVar11 = puVar5;
                  for (uVar6 = (uint)(iVar3 * 0x48) >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
                    *(undefined *)puVar11 = *(undefined *)puVar9;
                    puVar9 = (undefined4 *)((int)puVar9 + 1);
                    puVar11 = (undefined4 *)((int)puVar11 + 1);
                  }
                  FUN_004830f0(puVar10);
                }
                this->mbr_0x196 = (word)iVar8;
                this->mbr_0x198 = (dword)puVar5;
              }
              puVar10 = auStack_48;
              puVar5 = (undefined4 *)(this->mbr_0x198 + (short)this->mbr_0x194 * 0x48);
              for (iVar3 = 0x12; iVar3 != 0; iVar3 = iVar3 + -1) {
                *puVar5 = *puVar10;
                puVar10 = puVar10 + 1;
                puVar5 = puVar5 + 1;
              }
              this->mbr_0x194 = this->mbr_0x194 + 1;
            }
          }
        }
      }
switchD_0053205b_caseD_3:
      cls_0x477870::meth_0x46dfb0(&local_64);
      piVar2 = (int *)local_64.mbr_0x18;
    }
  }
  return;
}



