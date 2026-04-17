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
  char *pcVar1;
  byte bVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
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
    piVar3 = (int *)local_64.mbr_0x18;
    while (piVar3 != (int *)0x0) {
      local_64.mbr_0x18 = (dword)piVar3;
      if (((*(short *)(piVar3 + 0x1f) < 0x100) &&
          (iVar4 = (**(code **)(*piVar3 + 0xd4))(s_SaleType_005e3e80), iVar4 != 2)) &&
         (iVar4 = (**(code **)(*piVar3 + 400))(), iVar4 != -1)) {
        switch(*(undefined2 *)(piVar3 + 1)) {
        case 1:
          bVar2 = this->mbr_0x17c & 8;
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
          bVar2 = this->mbr_0x17c & 0x10;
        }
        if (bVar2 != 0) {
LAB_00532092:
          iVar4 = (**(code **)(*piVar3 + 0xd4))(in_stack_00000004);
          if ((in_stack_00000008 <= iVar4) && (iVar4 <= in_stack_0000000c)) {
            pcVar1 = (char *)piVar3[0xe];
            uVar5 = (**(code **)(*piVar3 + 0x198))();
            iVar4 = FUN_0052da90(auStack_48,pcVar1,(byte)*(undefined4 *)&this->mbr_0x17c,uVar5);
            if (iVar4 != 0) {
              if ((short)this->mbr_0x196 <= (short)this->mbr_0x194) {
                iVar9 = (short)this->mbr_0x196 + 4;
                puVar6 = FUN_00482fb0(iVar9 * 0x48);
                iVar4 = (int)(short)this->mbr_0x196;
                puVar11 = puVar6 + iVar4 * 0x12;
                for (uVar7 = (uint)((iVar9 - iVar4) * 0x48) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
                  *puVar11 = 0;
                  puVar11 = puVar11 + 1;
                }
                for (iVar8 = 0; iVar8 != 0; iVar8 = iVar8 + -1) {
                  *(undefined *)puVar11 = 0;
                  puVar11 = (undefined4 *)((int)puVar11 + 1);
                }
                puVar11 = (undefined4 *)this->mbr_0x198;
                if (puVar11 != (undefined4 *)0x0) {
                  puVar10 = puVar11;
                  puVar12 = puVar6;
                  for (uVar7 = (uint)(iVar4 * 0x48) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
                    *puVar12 = *puVar10;
                    puVar10 = puVar10 + 1;
                    puVar12 = puVar12 + 1;
                  }
                  for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
                    *(undefined *)puVar12 = *(undefined *)puVar10;
                    puVar10 = (undefined4 *)((int)puVar10 + 1);
                    puVar12 = (undefined4 *)((int)puVar12 + 1);
                  }
                  FUN_004830f0(puVar11);
                }
                this->mbr_0x196 = (word)iVar9;
                this->mbr_0x198 = (dword)puVar6;
              }
              puVar11 = auStack_48;
              puVar6 = (undefined4 *)(this->mbr_0x198 + (short)this->mbr_0x194 * 0x48);
              for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
                *puVar6 = *puVar11;
                puVar11 = puVar11 + 1;
                puVar6 = puVar6 + 1;
              }
              this->mbr_0x194 = this->mbr_0x194 + 1;
            }
          }
        }
      }
switchD_0053205b_caseD_3:
      cls_0x477870::meth_0x46dfb0(&local_64);
      piVar3 = (int *)local_64.mbr_0x18;
    }
  }
  return;
}



