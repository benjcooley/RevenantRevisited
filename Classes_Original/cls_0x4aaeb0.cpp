// Decompiled methods and structure for class: cls_0x4aaeb0

/*
/OOAnalyzer/cls_0x4aaeb0
pack(disabled)
Structure cls_0x4aaeb0 {
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 124 Alignment: 1

*/

// Function at 004aaeb0

undefined4 __thiscall
OOAnalyzer::cls_0x4aaeb0::meth_0x4aaeb0
          (cls_0x4aaeb0 *this,int param_1,int param_2,int param_3,int param_4,int param_5)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  dword dVar6;
  int iVar7;
  int iVar8;
  int unaff_EDI;
  int iVar9;
  undefined4 unaff_retaddr;
  uint in_stack_00000018;
  int local_94;
  uint local_90;
  int local_8c;
  int local_88;
  int local_84;
  int local_7c;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  
  if ((((-1 < param_1) && (param_1 < 10)) && ((&DAT_00669b14)[param_1 * 0x13] != 0)) &&
     (DAT_005e91bc == 0)) {
    iVar8 = (&DAT_00669afc)[param_1 * 0x13];
    iVar9 = (&DAT_00669b00)[param_1 * 0x13];
    if ((in_stack_00000018 & 4) == 0) {
      iVar7 = *(int *)((&DAT_00669b14)[param_1 * 0x13] + 4);
      iVar4 = *(int *)((&DAT_00669b14)[param_1 * 0x13] + 8);
    }
    else {
      iVar7 = (&DAT_00669b0c)[param_1 * 0x13];
      iVar4 = (&DAT_00669b10)[param_1 * 0x13];
    }
    if (((param_2 < iVar7 + iVar8) && (iVar8 < param_4 + param_2)) &&
       ((param_3 < iVar4 + iVar9 && (iVar9 < param_3 + param_5)))) {
      FUN_00482130();
      if (param_2 < iVar8) {
        param_4 = param_4 + (param_2 - iVar8);
        param_2 = iVar8;
      }
      if (param_3 < iVar9) {
        param_5 = param_5 + (param_3 - iVar9);
        param_3 = iVar9;
      }
      if (iVar7 + iVar8 < param_4 + param_2) {
        param_4 = (iVar7 - param_2) + iVar8;
      }
      if (iVar4 + iVar9 < param_3 + param_5) {
        param_5 = (iVar4 - param_3) + iVar9;
      }
      if ((in_stack_00000018 & 4) != 0) {
        piVar1 = (int *)(&DAT_00669b14)[param_1 * 0x13];
        iVar8 = piVar1[8];
        iVar9 = piVar1[10];
        local_64 = piVar1[0xb] + -1 + piVar1[9];
        (**(code **)(*piVar1 + 0x44))
                  ((&DAT_00669afc)[param_1 * 0x13],(&DAT_00669b00)[param_1 * 0x13],
                   (&DAT_00669b0c)[param_1 * 0x13],(&DAT_00669b10)[param_1 * 0x13]);
        uVar5 = 0x22;
        if (((&DAT_00669af0)[param_1 * 0x13] & 1) != 0) {
          uVar5 = 0x400022;
        }
        if (((&DAT_00669af0)[param_1 * 0x13] & 2) != 0) {
          uVar5 = CONCAT22((short)((uint)uVar5 >> 0x10),(ushort)(byte)uVar5);
        }
        iVar7 = *(int *)(unaff_EDI + 0x8c);
        piVar1 = (int *)(&DAT_00669b14)[param_1 * 0x13];
        FUN_00438d80(&local_64,param_2,param_3,
                     ((&DAT_00669af4)[param_1 * 0x13] - (&DAT_00669afc)[param_1 * 0x13]) + param_2,
                     ((&DAT_00669af8)[param_1 * 0x13] - (&DAT_00669b00)[param_1 * 0x13]) + param_3,
                     unaff_retaddr,param_1,uVar5);
        (**(code **)(*piVar1 + 0x5c))(&local_64,iVar7,0,0);
        (**(code **)(*(int *)(&DAT_00669b14)[param_1 * 0x13] + 0x44))
                  (iVar7,piVar1,(iVar9 + iVar8) - iVar7,(iVar4 - (int)piVar1) + 1);
      }
      if ((((in_stack_00000018 & 8) != 0) &&
          ((&DAT_00669afc)[param_1 * 0x13] == (&DAT_00669b04)[param_1 * 0x13])) &&
         ((&DAT_00669b00)[param_1 * 0x13] == (&DAT_00669b08)[param_1 * 0x13])) {
        piVar1 = *(int **)&this[1].field_0x10;
        iVar8 = piVar1[8];
        iVar9 = piVar1[10];
        local_64 = piVar1[0xb] + -1 + piVar1[9];
        iVar7 = (&DAT_00669af4)[param_1 * 0x13];
        (**(code **)(*piVar1 + 0x44))
                  (iVar7,(&DAT_00669af8)[param_1 * 0x13],(&DAT_00669b0c)[param_1 * 0x13],
                   (&DAT_00669b10)[param_1 * 0x13]);
        uVar5 = 0x24;
        if (((&DAT_00669af0)[param_1 * 0x13] & 1) != 0) {
          uVar5 = 0x400024;
        }
        if (((&DAT_00669af0)[param_1 * 0x13] & 2) != 0) {
          uVar5 = CONCAT22((short)((uint)uVar5 >> 0x10),(ushort)(byte)uVar5);
        }
        iVar2 = (&DAT_00669b14)[param_1 * 0x13];
        piVar1 = *(int **)(unaff_EDI + 0x8c);
        FUN_00438d80(&local_64,
                     ((&DAT_00669af4)[param_1 * 0x13] - (&DAT_00669afc)[param_1 * 0x13]) + param_2,
                     (param_3 - (&DAT_00669b00)[param_1 * 0x13]) + (&DAT_00669af8)[param_1 * 0x13],
                     param_2,param_3,unaff_retaddr,param_1,uVar5);
        (**(code **)(*piVar1 + 0x5c))(&local_64,iVar2,0,0);
        (**(code **)(**(int **)(iVar7 + 0x8c) + 0x44))
                  (iVar2,piVar1,(iVar9 + iVar8) - iVar2,(iVar4 - (int)piVar1) + 1);
      }
      local_7c = 0;
      do {
        if (local_7c == 0) {
          dVar6 = this->mbr_0x78;
          uVar3 = in_stack_00000018 & 1;
        }
        else {
          dVar6 = (dword)(this->mbr_0x78 == 0);
          uVar3 = in_stack_00000018 & 2;
        }
        if (uVar3 != 0) {
          if ((int)(&DAT_00669b24)[param_1 * 0x13 + dVar6] < 0x7c) {
            if ((in_stack_00000018 & 0x10) == 0) {
              iVar8 = param_4 + -1 + param_2;
              iVar9 = param_3 + -1 + param_5;
              local_90 = (&DAT_00669b2c)[param_1 * 0x13 + dVar6];
              local_58 = (&DAT_00669b34)[param_1 * 0x13 + dVar6];
              if (local_90 != local_58) {
                local_60 = (&DAT_00669b1c)[param_1 * 0x13 + dVar6];
                do {
                  piVar1 = (int *)(local_60 + local_90 * 0x14);
                  if (((piVar1[4] & 0x10U) == 0) &&
                     (((piVar1[4] ^ in_stack_00000018 & 0xfffffff3) & 0x20) == 0)) {
                    local_84 = *piVar1;
                    local_8c = piVar1[2] + -1 + local_84;
                    local_94 = piVar1[1];
                    iVar7 = piVar1[3];
                    local_88 = iVar7 + -1 + local_94;
                    if (0x100 < iVar7) {
                      iVar7 = 0x100;
                    }
                    local_5c = 0x10 - (iVar7 >> 4);
                    iVar7 = piVar1[2];
                    if (0x100 < iVar7) {
                      iVar7 = 0x100;
                    }
                    iVar7 = 0x10 - (iVar7 >> 4);
                    if ((((local_8c < param_2) || (local_88 < param_3)) || (iVar8 < local_84)) ||
                       (iVar9 < local_94)) goto LAB_004ab4ab;
                    local_90 = 0;
                    if (local_84 - local_5c <= param_2) {
                      if (param_2 <= local_84) {
                        local_84 = param_2;
                      }
                      local_90 = 1;
                    }
                    if (iVar8 <= local_5c + local_8c) {
                      if (local_8c <= iVar8) {
                        local_8c = iVar8;
                      }
                      local_90 = local_90 | 2;
                    }
                    if (local_94 - iVar7 <= param_3) {
                      if (param_3 <= local_94) {
                        local_94 = param_3;
                      }
                      local_90 = local_90 | 4;
                    }
                    if (iVar9 <= iVar7 + local_88) {
                      if (local_88 <= iVar9) {
                        local_88 = iVar9;
                      }
                      local_90 = local_90 | 8;
                    }
                    piVar1[1] = local_94;
                    *piVar1 = local_84;
                    piVar1[2] = (local_8c - local_84) + 1;
                    iVar7 = (local_88 - local_94) + 1;
                    piVar1[3] = iVar7;
                    switch(local_90) {
                    case 0:
                      *piVar1 = param_2;
                      piVar1[1] = param_3;
                      piVar1[2] = (iVar8 - param_2) + 1;
                      piVar1[3] = (iVar9 - param_3) + 1;
                      goto switchD_004ab571_caseD_f;
                    case 1:
                      iVar4 = (iVar8 - param_2) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,param_3,iVar4,local_94 - param_3);
                      meth_0x4aaeb0(this,param_1,local_8c + 1,local_94,(iVar8 - (local_8c + 1)) + 1,
                                    iVar7);
                      meth_0x4aaeb0(this,param_1,param_2,local_88 + 1,iVar4,
                                    (iVar9 - (local_88 + 1)) + 1);
                      goto switchD_004ab571_caseD_f;
                    case 2:
                      iVar8 = (iVar8 - param_2) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,param_3,iVar8,local_94 - param_3);
                      meth_0x4aaeb0(this,param_1,param_2,local_94,local_84 - param_2,iVar7);
                      meth_0x4aaeb0(this,param_1,param_2,local_88 + 1,iVar8,
                                    (iVar9 - (local_88 + 1)) + 1);
                      goto switchD_004ab571_caseD_f;
                    case 3:
                      iVar8 = (iVar8 - param_2) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,param_3,iVar8,local_94 - param_3);
                      goto LAB_004ab6b7;
                    case 4:
                      iVar7 = (local_88 - param_3) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,param_3,local_84 - param_2,iVar7);
                      local_8c = local_8c + 1;
                      local_84 = (iVar8 - local_8c) + 1;
                      break;
                    case 5:
                      local_8c = local_8c + 1;
                      iVar7 = (local_88 - param_3) + 1;
                      local_84 = (iVar8 - local_8c) + 1;
                      break;
                    case 6:
                      iVar7 = (local_88 - param_3) + 1;
                      local_84 = local_84 - param_2;
                      local_8c = param_2;
                      break;
                    case 7:
                      goto switchD_004ab571_caseD_7;
                    case 8:
                      meth_0x4aaeb0(this,param_1,param_2,param_3,(iVar8 - param_2) + 1,
                                    local_94 - param_3);
                      iVar9 = (iVar9 - local_94) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,local_94,local_84 - param_2,iVar9);
                      meth_0x4aaeb0(this,param_1,local_8c + 1,local_94,(iVar8 - (local_8c + 1)) + 1,
                                    iVar9);
                      goto switchD_004ab571_caseD_f;
                    case 9:
                      meth_0x4aaeb0(this,param_1,param_2,param_3,(iVar8 - param_2) + 1,
                                    local_94 - param_3);
                      meth_0x4aaeb0(this,param_1,local_8c + 1,local_94,(iVar8 - (local_8c + 1)) + 1,
                                    (iVar9 - local_94) + 1);
                      goto switchD_004ab571_caseD_f;
                    case 10:
                      meth_0x4aaeb0(this,param_1,param_2,param_3,(iVar8 - param_2) + 1,
                                    local_94 - param_3);
                      meth_0x4aaeb0(this,param_1,param_2,local_94,local_84 - param_2,
                                    (iVar9 - local_94) + 1);
                      goto switchD_004ab571_caseD_f;
                    case 0xb:
                      meth_0x4aaeb0(this,param_1,param_2,param_3,(iVar8 - param_2) + 1,
                                    local_94 - param_3);
                      goto switchD_004ab571_caseD_f;
                    case 0xc:
                      iVar9 = (iVar9 - param_3) + 1;
                      meth_0x4aaeb0(this,param_1,param_2,param_3,local_84 - param_2,iVar9);
                      meth_0x4aaeb0(this,param_1,local_8c + 1,param_3,(iVar8 - (local_8c + 1)) + 1,
                                    iVar9);
                      goto switchD_004ab571_caseD_f;
                    case 0xd:
                      meth_0x4aaeb0(this,param_1,local_8c + 1,param_3,(iVar8 - (local_8c + 1)) + 1,
                                    (iVar9 - param_3) + 1);
                      goto switchD_004ab571_caseD_f;
                    case 0xe:
                      meth_0x4aaeb0(this,param_1,param_2,param_3,local_84 - param_2,
                                    (iVar9 - param_3) + 1);
                    default:
                      goto switchD_004ab571_caseD_f;
                    }
                    meth_0x4aaeb0(this,param_1,local_8c,param_3,local_84,iVar7);
switchD_004ab571_caseD_7:
                    iVar8 = (iVar8 - param_2) + 1;
LAB_004ab6b7:
                    meth_0x4aaeb0(this,param_1,param_2,local_88 + 1,iVar8,
                                  (iVar9 - (local_88 + 1)) + 1);
                    goto switchD_004ab571_caseD_f;
                  }
LAB_004ab4ab:
                  local_90 = local_90 + 1;
                  if (0x7f < (int)local_90) {
                    local_90 = 0;
                  }
                } while (local_90 != local_58);
              }
            }
          }
          else {
            (&DAT_00669b24)[param_1 * 0x13 + dVar6] = 0;
            (&DAT_00669b2c)[param_1 * 0x13 + dVar6] = 0;
            (&DAT_00669b34)[param_1 * 0x13 + dVar6] = 0;
            param_2 = (&DAT_00669afc)[param_1 * 0x13];
            param_3 = (&DAT_00669b00)[param_1 * 0x13];
            param_4 = *(int *)((&DAT_00669b14)[param_1 * 0x13] + 4);
            param_5 = *(int *)((&DAT_00669b14)[param_1 * 0x13] + 8);
          }
          piVar1 = (int *)((&DAT_00669b1c)[param_1 * 0x13 + dVar6] +
                          (&DAT_00669b34)[param_1 * 0x13 + dVar6] * 0x14);
          if (0 < param_4) {
            piVar1[2] = param_4;
            *piVar1 = param_2;
            piVar1[1] = param_3;
            piVar1[3] = param_5;
            piVar1[4] = in_stack_00000018 & 0xfffffff3;
          }
          (&DAT_00669b24)[param_1 * 0x13 + dVar6] = (&DAT_00669b24)[param_1 * 0x13 + dVar6] + 1;
          iVar8 = (&DAT_00669b34)[param_1 * 0x13 + dVar6];
          (&DAT_00669b34)[param_1 * 0x13 + dVar6] = iVar8 + 1;
          if (0x7f < iVar8 + 1) {
            (&DAT_00669b34)[param_1 * 0x13 + dVar6] = 0;
          }
        }
switchD_004ab571_caseD_f:
        local_7c = local_7c + 1;
        if (1 < local_7c) {
          FUN_00482140();
          return 1;
        }
      } while( true );
    }
  }
  return 0;
}



