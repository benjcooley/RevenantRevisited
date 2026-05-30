// FUN_004aaeb0_BlitEffect_Apply @ 004aaeb0 size=2737

undefined4 __thiscall
FUN_004aaeb0_BlitEffect_Apply(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,uint param_7)

{
  int *piVar1;
  int iVar2;
  byte bVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int unaff_EDI;
  int iVar10;
  undefined4 unaff_retaddr;
  int iStack_94;
  uint local_90;
  int local_8c;
  int iStack_88;
  int local_84;
  int local_7c;
  int local_64;
  int iStack_60;
  int iStack_5c;
  int iStack_58;
  
  if ((((-1 < param_2) && (param_2 < 10)) && ((&DAT_00669b14)[param_2 * 0x13] != 0)) &&
     (DAT_005e91bc == 0)) {
    iVar9 = (&DAT_00669afc)[param_2 * 0x13];
    iVar10 = (&DAT_00669b00)[param_2 * 0x13];
    if ((param_7 & 4) == 0) {
      iVar7 = *(int *)((&DAT_00669b14)[param_2 * 0x13] + 4);
      iVar4 = *(int *)((&DAT_00669b14)[param_2 * 0x13] + 8);
    }
    else {
      iVar7 = (&DAT_00669b0c)[param_2 * 0x13];
      iVar4 = (&DAT_00669b10)[param_2 * 0x13];
    }
    if (((param_3 < iVar7 + iVar9) && (iVar9 < param_5 + param_3)) &&
       ((param_4 < iVar4 + iVar10 && (iVar10 < param_4 + param_6)))) {
      FUN_00482130();
      if (param_3 < iVar9) {
        param_5 = param_5 + (param_3 - iVar9);
        param_3 = iVar9;
      }
      if (param_4 < iVar10) {
        param_6 = param_6 + (param_4 - iVar10);
        param_4 = iVar10;
      }
      if (iVar7 + iVar9 < param_5 + param_3) {
        param_5 = (iVar7 - param_3) + iVar9;
      }
      if (iVar4 + iVar10 < param_4 + param_6) {
        param_6 = (iVar4 - param_4) + iVar10;
      }
      if ((param_7 & 4) != 0) {
        piVar1 = (int *)(&DAT_00669b14)[param_2 * 0x13];
        iVar9 = piVar1[8];
        iVar10 = piVar1[10];
        local_64 = piVar1[0xb] + -1 + piVar1[9];
        (**(code **)(*piVar1 + 0x44))
                  ((&DAT_00669afc)[param_2 * 0x13],(&DAT_00669b00)[param_2 * 0x13],
                   (&DAT_00669b0c)[param_2 * 0x13],(&DAT_00669b10)[param_2 * 0x13]);
        uVar5 = 0x22;
        if (((&DAT_00669af0)[param_2 * 0x13] & 1) != 0) {
          uVar5 = 0x400022;
        }
        if (((&DAT_00669af0)[param_2 * 0x13] & 2) != 0) {
          uVar5 = CONCAT22((short)((uint)uVar5 >> 0x10),(ushort)(byte)uVar5);
        }
        iVar7 = *(int *)(unaff_EDI + 0x8c);
        piVar1 = (int *)(&DAT_00669b14)[param_2 * 0x13];
        FUN_00438d80_BlitEffect_StructInit(&local_64,param_3,param_4,
                     ((&DAT_00669af4)[param_2 * 0x13] - (&DAT_00669afc)[param_2 * 0x13]) + param_3,
                     ((&DAT_00669af8)[param_2 * 0x13] - (&DAT_00669b00)[param_2 * 0x13]) + param_4,
                     unaff_retaddr,param_2,uVar5);
        (**(code **)(*piVar1 + 0x5c))(&local_64,iVar7,0,0);
        (**(code **)(*(int *)(&DAT_00669b14)[param_2 * 0x13] + 0x44))
                  (iVar7,piVar1,(iVar10 + iVar9) - iVar7,(iVar4 - (int)piVar1) + 1);
      }
      if ((((param_7 & 8) != 0) &&
          ((&DAT_00669afc)[param_2 * 0x13] == (&DAT_00669b04)[param_2 * 0x13])) &&
         ((&DAT_00669b00)[param_2 * 0x13] == (&DAT_00669b08)[param_2 * 0x13])) {
        piVar1 = *(int **)(param_1 + 0x8c);
        iVar9 = piVar1[8];
        iVar10 = piVar1[10];
        local_64 = piVar1[0xb] + -1 + piVar1[9];
        iVar7 = (&DAT_00669af4)[param_2 * 0x13];
        (**(code **)(*piVar1 + 0x44))
                  (iVar7,(&DAT_00669af8)[param_2 * 0x13],(&DAT_00669b0c)[param_2 * 0x13],
                   (&DAT_00669b10)[param_2 * 0x13]);
        uVar5 = 0x24;
        if (((&DAT_00669af0)[param_2 * 0x13] & 1) != 0) {
          uVar5 = 0x400024;
        }
        if (((&DAT_00669af0)[param_2 * 0x13] & 2) != 0) {
          uVar5 = CONCAT22((short)((uint)uVar5 >> 0x10),(ushort)(byte)uVar5);
        }
        iVar2 = (&DAT_00669b14)[param_2 * 0x13];
        piVar1 = *(int **)(unaff_EDI + 0x8c);
        FUN_00438d80_BlitEffect_StructInit(&local_64,
                     ((&DAT_00669af4)[param_2 * 0x13] - (&DAT_00669afc)[param_2 * 0x13]) + param_3,
                     (param_4 - (&DAT_00669b00)[param_2 * 0x13]) + (&DAT_00669af8)[param_2 * 0x13],
                     param_3,param_4,unaff_retaddr,param_2,uVar5);
        (**(code **)(*piVar1 + 0x5c))(&local_64,iVar2,0,0);
        (**(code **)(**(int **)(iVar7 + 0x8c) + 0x44))
                  (iVar2,piVar1,(iVar10 + iVar9) - iVar2,(iVar4 - (int)piVar1) + 1);
      }
      uVar8 = param_7 & 0xfffffff3;
      local_7c = 0;
      do {
        if (local_7c == 0) {
          uVar6 = *(uint *)(param_1 + 0x78);
          if ((param_7 & 1) != 0) {
            bVar3 = (byte)uVar8 & 0x20 | 1;
LAB_004ab2ca:
            if ((int)(&DAT_00669b24)[param_2 * 0x13 + uVar6] < 0x7c) {
              if ((param_7 & 0x10) == 0) {
                iVar9 = param_5 + -1 + param_3;
                iVar10 = param_4 + -1 + param_6;
                local_90 = (&DAT_00669b2c)[param_2 * 0x13 + uVar6];
                iStack_58 = (&DAT_00669b34)[param_2 * 0x13 + uVar6];
                if (local_90 != iStack_58) {
                  iStack_60 = (&DAT_00669b1c)[param_2 * 0x13 + uVar6];
                  do {
                    piVar1 = (int *)(iStack_60 + local_90 * 0x14);
                    if (((piVar1[4] & 0x10U) == 0) && (((piVar1[4] ^ uVar8) & 0x20) == 0)) {
                      local_84 = *piVar1;
                      local_8c = piVar1[2] + -1 + local_84;
                      iStack_94 = piVar1[1];
                      iVar7 = piVar1[3];
                      iStack_88 = iVar7 + -1 + iStack_94;
                      if (0x100 < iVar7) {
                        iVar7 = 0x100;
                      }
                      iStack_5c = 0x10 - (iVar7 >> 4);
                      iVar7 = piVar1[2];
                      if (0x100 < iVar7) {
                        iVar7 = 0x100;
                      }
                      iVar7 = 0x10 - (iVar7 >> 4);
                      if ((((local_8c < param_3) || (iStack_88 < param_4)) || (iVar9 < local_84)) ||
                         (iVar10 < iStack_94)) goto LAB_004ab4ab;
                      local_90 = 0;
                      if (local_84 - iStack_5c <= param_3) {
                        if (param_3 <= local_84) {
                          local_84 = param_3;
                        }
                        local_90 = 1;
                      }
                      if (iVar9 <= iStack_5c + local_8c) {
                        if (local_8c <= iVar9) {
                          local_8c = iVar9;
                        }
                        local_90 = local_90 | 2;
                      }
                      if (iStack_94 - iVar7 <= param_4) {
                        if (param_4 <= iStack_94) {
                          iStack_94 = param_4;
                        }
                        local_90 = local_90 | 4;
                      }
                      if (iVar10 <= iVar7 + iStack_88) {
                        if (iStack_88 <= iVar10) {
                          iStack_88 = iVar10;
                        }
                        local_90 = local_90 | 8;
                      }
                      piVar1[1] = iStack_94;
                      *piVar1 = local_84;
                      piVar1[2] = (local_8c - local_84) + 1;
                      iVar7 = (iStack_88 - iStack_94) + 1;
                      piVar1[3] = iVar7;
                      switch(local_90) {
                      case 0:
                        *piVar1 = param_3;
                        piVar1[1] = param_4;
                        piVar1[2] = (iVar9 - param_3) + 1;
                        piVar1[3] = (iVar10 - param_4) + 1;
                        goto switchD_004ab571_default;
                      case 1:
                        iVar4 = (iVar9 - param_3) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,iVar4,iStack_94 - param_4,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c + 1,iStack_94,(iVar9 - (local_8c + 1)) + 1,
                                     iVar7,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_88 + 1,iVar4,
                                     (iVar10 - (iStack_88 + 1)) + 1,bVar3);
                        goto switchD_004ab571_default;
                      case 2:
                        iVar9 = (iVar9 - param_3) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,iVar9,iStack_94 - param_4,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_94,local_84 - param_3,iVar7,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_88 + 1,iVar9,
                                     (iVar10 - (iStack_88 + 1)) + 1,bVar3);
                        goto switchD_004ab571_default;
                      case 3:
                        iVar9 = (iVar9 - param_3) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,iVar9,iStack_94 - param_4,bVar3);
                        goto LAB_004ab6b7;
                      case 4:
                        iVar7 = (iStack_88 - param_4) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,local_84 - param_3,iVar7,bVar3);
                        local_8c = local_8c + 1;
                        local_84 = (iVar9 - local_8c) + 1;
                        break;
                      case 5:
                        local_8c = local_8c + 1;
                        iVar7 = (iStack_88 - param_4) + 1;
                        local_84 = (iVar9 - local_8c) + 1;
                        break;
                      case 6:
                        iVar7 = (iStack_88 - param_4) + 1;
                        local_84 = local_84 - param_3;
                        local_8c = param_3;
                        break;
                      case 7:
                        goto switchD_004ab571_caseD_7;
                      case 8:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,(iVar9 - param_3) + 1,
                                     iStack_94 - param_4,bVar3);
                        iVar10 = (iVar10 - iStack_94) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_94,local_84 - param_3,iVar10,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c + 1,iStack_94,(iVar9 - (local_8c + 1)) + 1,
                                     iVar10,bVar3);
                        goto switchD_004ab571_default;
                      case 9:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,(iVar9 - param_3) + 1,
                                     iStack_94 - param_4,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c + 1,iStack_94,(iVar9 - (local_8c + 1)) + 1,
                                     (iVar10 - iStack_94) + 1,bVar3);
                        goto switchD_004ab571_default;
                      case 10:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,(iVar9 - param_3) + 1,
                                     iStack_94 - param_4,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_94,local_84 - param_3,
                                     (iVar10 - iStack_94) + 1,bVar3);
                        goto switchD_004ab571_default;
                      case 0xb:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,(iVar9 - param_3) + 1,
                                     iStack_94 - param_4,bVar3);
                        goto switchD_004ab571_default;
                      case 0xc:
                        iVar10 = (iVar10 - param_4) + 1;
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,local_84 - param_3,iVar10,bVar3);
                        FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c + 1,param_4,(iVar9 - (local_8c + 1)) + 1,
                                     iVar10,bVar3);
                        goto switchD_004ab571_default;
                      case 0xd:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c + 1,param_4,(iVar9 - (local_8c + 1)) + 1,
                                     (iVar10 - param_4) + 1,bVar3);
                        goto switchD_004ab571_default;
                      case 0xe:
                        FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,param_4,local_84 - param_3,
                                     (iVar10 - param_4) + 1,bVar3);
                      default:
                        goto switchD_004ab571_default;
                      }
                      FUN_004aaeb0_BlitEffect_Apply(param_2,local_8c,param_4,local_84,iVar7,bVar3);
switchD_004ab571_caseD_7:
                      iVar9 = (iVar9 - param_3) + 1;
LAB_004ab6b7:
                      FUN_004aaeb0_BlitEffect_Apply(param_2,param_3,iStack_88 + 1,iVar9,
                                   (iVar10 - (iStack_88 + 1)) + 1,bVar3);
                      goto switchD_004ab571_default;
                    }
LAB_004ab4ab:
                    local_90 = local_90 + 1;
                    if (0x7f < (int)local_90) {
                      local_90 = 0;
                    }
                  } while (local_90 != iStack_58);
                }
              }
            }
            else {
              (&DAT_00669b24)[param_2 * 0x13 + uVar6] = 0;
              (&DAT_00669b2c)[param_2 * 0x13 + uVar6] = 0;
              (&DAT_00669b34)[param_2 * 0x13 + uVar6] = 0;
              param_3 = (&DAT_00669afc)[param_2 * 0x13];
              param_4 = (&DAT_00669b00)[param_2 * 0x13];
              param_5 = *(int *)((&DAT_00669b14)[param_2 * 0x13] + 4);
              param_6 = *(int *)((&DAT_00669b14)[param_2 * 0x13] + 8);
            }
            piVar1 = (int *)((&DAT_00669b1c)[param_2 * 0x13 + uVar6] +
                            (&DAT_00669b34)[param_2 * 0x13 + uVar6] * 0x14);
            if (0 < param_5) {
              piVar1[2] = param_5;
              *piVar1 = param_3;
              piVar1[1] = param_4;
              piVar1[3] = param_6;
              piVar1[4] = uVar8;
            }
            (&DAT_00669b24)[param_2 * 0x13 + uVar6] = (&DAT_00669b24)[param_2 * 0x13 + uVar6] + 1;
            iVar9 = (&DAT_00669b34)[param_2 * 0x13 + uVar6];
            (&DAT_00669b34)[param_2 * 0x13 + uVar6] = iVar9 + 1;
            if (0x7f < iVar9 + 1) {
              (&DAT_00669b34)[param_2 * 0x13 + uVar6] = 0;
            }
          }
        }
        else {
          uVar6 = (uint)(*(int *)(param_1 + 0x78) == 0);
          if ((param_7 & 2) != 0) {
            bVar3 = (byte)uVar8 & 0x20 | 2;
            goto LAB_004ab2ca;
          }
        }
switchD_004ab571_default:
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


