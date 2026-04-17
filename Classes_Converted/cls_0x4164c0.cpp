#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4164c0



// Function at 004164c0

void cls_0x4164c0::meth_0x4164c0(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  undefined4 *puVar10;
  int in_stack_00000008;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint *local_8;
  
  uVar4 = DAT_00669604;
  if ((this->mbr_0x0 != 0) && (DAT_005d7a28 == 0)) {
    uVar9 = *(uint *)(param_1 + 0x4c);
    if ((uVar9 & 0x40) == 0) {
      if ((uVar9 & 0x1828) != 0) {
        local_20 = 6;
        local_24 = 5;
        local_1c = 5;
        local_18 = 0;
      }
    }
    else {
      local_24 = 0;
      iVar2 = 0;
      do {
        if ((*(uint *)(param_1 + 0x58) & 1 << ((byte)iVar2 & 0x1f)) != 0) {
          local_24 = local_24 + 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < 0x20);
      local_20 = 0;
      iVar2 = 0;
      do {
        if ((*(uint *)(param_1 + 0x5c) & 1 << ((byte)iVar2 & 0x1f)) != 0) {
          local_20 = local_20 + 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < 0x20);
      local_1c = 0;
      iVar2 = 0;
      do {
        if ((*(uint *)(param_1 + 0x60) & 1 << ((byte)iVar2 & 0x1f)) != 0) {
          local_1c = local_1c + 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < 0x20);
      local_18 = 0;
      iVar2 = 0;
      do {
        if ((*(uint *)(param_1 + 100) & 1 << ((byte)iVar2 & 0x1f)) != 0) {
          local_18 = local_18 + 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < 0x20);
    }
    local_14 = 0;
    local_10 = 10000;
    local_c = 0;
    iVar2 = local_14;
    if (0 < DAT_005e8934) {
      local_8 = &DAT_005e89b4;
      do {
        uVar1 = *local_8;
        if ((((uVar1 & 0x1828) != 0) && (((uVar1 ^ uVar9) & 0x1828) == 0)) &&
           (iVar2 = local_c, local_8[2] == *(uint *)(param_1 + 0x54))) break;
        if ((uVar1 & 0x20) == 0) {
          if ((uVar1 & 8) == 0) {
            if ((uVar1 & 0x1000) == 0) {
              if ((uVar1 & 0x800) == 0) {
                if ((uVar1 & 0x40) == 0) {
                  iVar3 = 10000;
                  iVar2 = 10000;
                  iVar5 = 10000;
                  iVar6 = 10000;
                }
                else {
                  iVar6 = 0;
                  iVar2 = 0;
                  do {
                    if ((local_8[3] & 1 << ((byte)iVar2 & 0x1f)) != 0) {
                      iVar6 = iVar6 + 1;
                    }
                    iVar2 = iVar2 + 1;
                  } while (iVar2 < 0x20);
                  iVar5 = 0;
                  iVar2 = 0;
                  do {
                    if ((local_8[4] & 1 << ((byte)iVar2 & 0x1f)) != 0) {
                      iVar5 = iVar5 + 1;
                    }
                    iVar2 = iVar2 + 1;
                  } while (iVar2 < 0x20);
                  iVar2 = 0;
                  iVar3 = 0;
                  do {
                    if ((local_8[5] & 1 << ((byte)iVar3 & 0x1f)) != 0) {
                      iVar2 = iVar2 + 1;
                    }
                    iVar3 = iVar3 + 1;
                  } while (iVar3 < 0x20);
                  iVar3 = 0;
                  iVar7 = 0;
                  do {
                    if ((local_8[6] & 1 << ((byte)iVar7 & 0x1f)) != 0) {
                      iVar3 = iVar3 + 1;
                    }
                    iVar7 = iVar7 + 1;
                  } while (iVar7 < 0x20);
                }
              }
              else {
                iVar3 = 0;
                iVar2 = 0;
                iVar6 = 1;
                iVar5 = 0;
              }
            }
            else {
              iVar2 = 1;
              iVar3 = 0;
              iVar5 = 1;
              iVar6 = 1;
            }
          }
          else {
            iVar2 = 2;
            iVar3 = 0;
            iVar5 = 2;
            iVar6 = 2;
          }
        }
        else {
          iVar2 = 4;
          iVar3 = 0;
          iVar5 = 4;
          iVar6 = 4;
        }
        iVar7 = local_24 - iVar6;
        if (iVar7 < 0) {
          iVar7 = iVar6 - local_24;
        }
        iVar6 = local_20 - iVar5;
        if (iVar6 < 0) {
          iVar6 = iVar5 - local_20;
        }
        iVar5 = local_1c - iVar2;
        if (iVar5 < 0) {
          iVar5 = iVar2 - local_1c;
        }
        iVar2 = local_18 - iVar3;
        if (iVar2 < 0) {
          iVar2 = iVar3 - local_18;
        }
        iVar7 = iVar5 + iVar2 + iVar6 + iVar7;
        if (iVar7 < local_10) {
          local_14 = local_c;
          local_10 = iVar7;
        }
        local_c = local_c + 1;
        local_8 = local_8 + 8;
        iVar2 = local_14;
      } while (local_c < DAT_005e8934);
    }
    local_14 = iVar2;
    puVar8 = (undefined4 *)(&DAT_005e898c.field_0x24 + local_14 * 0x20);
    puVar10 = (undefined4 *)(in_stack_00000008 + 0x48);
    for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar10 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar10 = puVar10 + 1;
    }
    uVar9 = *(uint *)(in_stack_00000008 + 0xc);
    while (uVar4 < uVar9) {
      uVar9 = *(uint *)(in_stack_00000008 + 0xc) >> 1;
      *(uint *)(in_stack_00000008 + 0xc) = uVar9;
    }
    uVar9 = *(uint *)(in_stack_00000008 + 8);
    while (uVar4 < uVar9) {
      uVar9 = *(uint *)(in_stack_00000008 + 8) >> 1;
      *(uint *)(in_stack_00000008 + 8) = uVar9;
    }
    if (DAT_00669324 != 0) {
      uVar4 = *(uint *)(in_stack_00000008 + 0xc);
      uVar9 = *(uint *)(in_stack_00000008 + 8);
      if (uVar4 != uVar9) {
        if (uVar9 <= uVar4) {
          uVar4 = uVar9;
        }
        *(uint *)(in_stack_00000008 + 0xc) = uVar4;
        if (uVar4 < uVar9) {
          uVar9 = uVar4;
        }
        *(uint *)(in_stack_00000008 + 8) = uVar9;
      }
    }
  }
  return;
}



// Function at 00416790

void cls_0x4164c0::meth_0x416790(int param_1, undefined4 *param_2, uint *param_3, int param_4, undefined4 *param_5)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  char cVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint *puVar9;
  byte bVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  int *piVar14;
  int *piVar15;
  uint *puVar16;
  bool bVar17;
  bool bVar18;
  uint *in_stack_00000018;
  uint local_94;
  uint local_90;
  int local_88;
  uint local_84;
  uint local_80;
  uint local_7c;
  int local_78;
  undefined4 *local_74;
  undefined4 *local_70;
  uint local_6c;
  byte local_5c;
  uint local_54;
  byte local_4c;
  byte local_48;
  int local_40;
  byte local_3c;
  byte local_38;
  byte local_34;
  byte local_30;
  byte local_2c;
  byte local_28;
  byte local_24;
  byte local_1c;
  byte local_18;
  byte local_14;
  byte local_10;
  byte bStack_a;
  byte local_8;
  byte local_4;
  
  if (this->mbr_0x0 != 0) {
    local_40 = 0;
    local_80 = *(uint *)(param_4 + 8);
    uVar7 = *(uint *)(param_4 + 0x10);
    iVar1 = *(int *)(param_4 + 0x54);
    uVar11 = local_80 * uVar7 * iVar1;
    uVar3 = *(uint *)(param_4 + 0x4c) & 0x1838;
    if (uVar3 != 0) {
      local_40 = 1 << ((byte)iVar1 & 0x1f);
    }
    uVar2 = *(uint *)(param_4 + 0xc);
    local_88 = 0;
    if (((*(uint *)(param_1 + 0xc) == uVar2) && (*(uint *)(param_1 + 8) == local_80)) &&
       (*(uint *)(param_1 + 0x10) == uVar7)) {
      iVar6 = 8;
      bVar17 = true;
      piVar14 = (int *)(param_1 + 0x48);
      piVar15 = (int *)(param_4 + 0x48);
      do {
        if (iVar6 == 0) break;
        iVar6 = iVar6 + -1;
        bVar17 = *piVar14 == *piVar15;
        piVar14 = piVar14 + 1;
        piVar15 = piVar15 + 1;
      } while (bVar17);
      if (bVar17) {
        for (uVar7 = uVar11 >> 5; uVar7 != 0; uVar7 = uVar7 - 1) {
          *param_5 = *param_2;
          param_2 = param_2 + 1;
          param_5 = param_5 + 1;
        }
        for (uVar7 = uVar11 >> 3 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined *)param_5 = *(undefined *)param_2;
          param_2 = (undefined4 *)((int)param_2 + 1);
          param_5 = (undefined4 *)((int)param_5 + 1);
        }
        for (; local_40 != 0; local_40 = local_40 + -1) {
          *in_stack_00000018 = *param_3;
          param_3 = param_3 + 1;
          in_stack_00000018 = in_stack_00000018 + 1;
        }
        return;
      }
    }
    iVar6 = *(int *)(param_1 + 0x54);
    uVar11 = *(uint *)(param_1 + 100);
    bVar17 = (*(uint *)(param_1 + 0x4c) & 0x1838) == 0;
    if (bVar17) {
      uVar12 = *(uint *)(param_1 + 0x58);
      if (uVar12 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar12;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_18 = (byte)iVar8;
      if (uVar12 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar12 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_4 = 8 - cVar5;
      uVar12 = *(uint *)(param_1 + 0x5c);
      if (uVar12 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar12;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_30 = (byte)iVar8;
      if (uVar12 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar12 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_38 = 8 - cVar5;
      uVar12 = *(uint *)(param_1 + 0x60);
      if (uVar12 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar12;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_28 = (byte)iVar8;
      if (uVar12 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar12 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_8 = 8 - cVar5;
      if (uVar11 == 0) {
        local_4c = 0;
        local_5c = 0;
      }
      else {
        if (uVar11 == 0) {
          iVar8 = 0;
        }
        else {
          uVar12 = uVar11;
          for (iVar8 = 0; ((uVar12 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
            uVar12 = uVar12 >> 1;
          }
        }
        local_5c = (byte)iVar8;
        if (uVar11 == 0) {
          local_4c = 8;
        }
        else {
          cVar5 = '\0';
          iVar8 = 0;
          do {
            if ((uVar11 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
              cVar5 = cVar5 + '\x01';
            }
            iVar8 = iVar8 + 1;
          } while (iVar8 < 0x20);
          local_4c = 8 - cVar5;
        }
      }
    }
    uVar12 = *(uint *)(param_4 + 100);
    bVar18 = uVar3 == 0;
    if (bVar18) {
      uVar3 = *(uint *)(param_4 + 0x58);
      if (uVar3 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar3;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_14 = (byte)iVar8;
      if (uVar3 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar3 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_1c = 8 - cVar5;
      uVar3 = *(uint *)(param_4 + 0x5c);
      if (uVar3 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar3;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_24 = (byte)iVar8;
      if (uVar3 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar3 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_2c = 8 - cVar5;
      uVar3 = *(uint *)(param_4 + 0x60);
      if (uVar3 == 0) {
        iVar8 = 0;
      }
      else {
        uVar4 = uVar3;
        for (iVar8 = 0; ((uVar4 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
          uVar4 = uVar4 >> 1;
        }
      }
      local_34 = (byte)iVar8;
      if (uVar3 == 0) {
        cVar5 = '\0';
      }
      else {
        cVar5 = '\0';
        iVar8 = 0;
        do {
          if ((uVar3 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
            cVar5 = cVar5 + '\x01';
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < 0x20);
      }
      local_10 = 8 - cVar5;
      if (uVar12 == 0) {
        local_48 = 0;
        local_3c = 8;
      }
      else {
        if (uVar12 == 0) {
          iVar8 = 0;
        }
        else {
          uVar3 = uVar12;
          for (iVar8 = 0; ((uVar3 & 1) == 0 && (iVar8 < 0x20)); iVar8 = iVar8 + 1) {
            uVar3 = uVar3 >> 1;
          }
        }
        local_48 = (byte)iVar8;
        if (uVar12 == 0) {
          local_3c = 8;
        }
        else {
          cVar5 = '\0';
          iVar8 = 0;
          do {
            if ((uVar12 & 1 << ((byte)iVar8 & 0x1f)) != 0) {
              cVar5 = cVar5 + '\x01';
            }
            iVar8 = iVar8 + 1;
          } while (iVar8 < 0x20);
          local_3c = 8 - cVar5;
        }
      }
    }
    iVar8 = iVar6;
    if (*(uint *)(param_1 + 0xc) != uVar2) {
      iVar8 = (*(uint *)(param_1 + 0xc) / uVar2) * iVar6;
    }
    if (*(uint *)(param_1 + 8) == local_80) {
      local_7c = 1;
    }
    else {
      local_7c = *(uint *)(param_1 + 8) / local_80;
    }
    local_70 = param_2;
    local_74 = param_5;
    uVar3 = uVar7;
    uVar12 = local_94;
    if (0 < (int)local_80) {
      do {
        local_78 = 0;
        local_90 = 0;
        local_54 = uVar2;
        if (0 < (int)uVar2) {
          do {
            puVar9 = (uint *)((local_78 >> 3) + (int)local_70);
            puVar16 = (uint *)(((int)local_90 >> 3) + (int)local_74);
            uVar4 = 0;
            if (bVar17) {
              if (iVar6 == 0x20) {
                uVar12 = *puVar9;
              }
              else if (iVar6 == 0x18) {
                local_94 = (uint)*(uint3 *)puVar9;
                uVar12 = local_94;
              }
              else if (iVar6 == 0x10) {
                uVar12 = (uint)*(ushort *)puVar9;
              }
              else if (iVar6 < 9) {
                uVar12 = (uint)*(byte *)puVar9;
              }
              uVar3 = (uVar12 >> (local_18 & 0x1f)) << (local_4 & 0x1f) & 0xff;
              local_94 = (uVar12 >> (local_30 & 0x1f)) << (local_38 & 0x1f) & 0xff;
              local_6c = (uVar12 >> (local_28 & 0x1f)) << (local_8 & 0x1f) & 0xff;
              if (uVar11 == 0) goto LAB_00416d56;
              uVar13 = (uVar12 >> (local_5c & 0x1f)) << (local_4c & 0x1f) & 0xff;
            }
            else {
              uVar4 = (uint)*(byte *)puVar9;
              if (iVar6 < 8) {
                uVar4 = (int)uVar4 >> (('\b' - ((byte)local_78 & 7)) - (char)iVar6 & 0x1f);
              }
              if (bVar18) {
                local_94 = (uint)*(byte *)((int)param_3 + uVar4 * 4 + 1);
                uVar3 = (uint)*(byte *)(param_3 + uVar4);
                local_6c = (uint)*(byte *)((int)param_3 + uVar4 * 4 + 2);
              }
LAB_00416d56:
              uVar13 = 0xff;
            }
            if (bVar18) {
              uVar4 = (uVar13 >> (local_3c & 0x1f)) << (local_48 & 0x1f) |
                      (local_6c >> (local_10 & 0x1f)) << (local_34 & 0x1f) |
                      (local_94 >> (local_2c & 0x1f)) << (local_24 & 0x1f) |
                      (uVar3 >> (local_1c & 0x1f)) << (local_14 & 0x1f);
              if (iVar1 == 0x20) {
                *puVar16 = uVar4;
              }
              else {
                bVar10 = (byte)uVar4;
                if (iVar1 == 0x18) {
                  bStack_a = (byte)(uVar4 >> 0x10);
                  *(byte *)puVar16 = bVar10;
                  *(byte *)((int)puVar16 + 1) = (byte)(uVar4 >> 8);
                  *(byte *)((int)puVar16 + 2) = bStack_a;
                }
                else if (iVar1 == 0x10) {
                  *(short *)puVar16 = (short)uVar4;
                }
                else if (iVar1 == 8) {
                  *(byte *)puVar16 = bVar10;
                }
                else if (iVar1 < 8) {
                  if ((local_90 & 7) == 0) {
                    *(byte *)puVar16 = bVar10;
                  }
                  else {
                    *(byte *)puVar16 = *(byte *)puVar16 | bVar10;
                  }
                }
              }
            }
            else {
              if (bVar17) {
                uVar4 = 0;
                local_84._0_2_ = CONCAT11((undefined)local_94,(char)uVar3);
                local_84 = CONCAT22((short)local_6c,(undefined2)local_84) & 0xffffff;
                puVar9 = in_stack_00000018;
                if (0 < local_88) {
                  do {
                    if (local_84 == *puVar9) {
                      if ((int)uVar4 < local_88) goto LAB_00416e7e;
                      break;
                    }
                    uVar4 = uVar4 + 1;
                    puVar9 = puVar9 + 1;
                  } while ((int)uVar4 < local_88);
                }
                if ((int)uVar4 < local_40) {
                  in_stack_00000018[uVar4] = local_84;
                  local_88 = local_88 + 1;
                }
              }
LAB_00416e7e:
              if (iVar1 < 8) {
                uVar4 = uVar4 << (('\b' - ((byte)local_90 & 7)) - (byte)iVar1 & 0x1f);
              }
              if ((local_90 & 7) == 0) {
                *(byte *)puVar16 = (byte)uVar4;
              }
              else {
                *(byte *)puVar16 = *(byte *)puVar16 | (byte)uVar4;
              }
            }
            local_78 = local_78 + iVar8;
            local_90 = local_90 + iVar1;
            local_54 = local_54 - 1;
          } while (local_54 != 0);
        }
        local_70 = (undefined4 *)
                   ((int)local_70 + ((int)(*(int *)(param_1 + 0x10) * local_7c * iVar6) >> 3));
        local_74 = (undefined4 *)((int)local_74 + ((int)(uVar7 * iVar1) >> 3));
        local_80 = local_80 - 1;
      } while (local_80 != 0);
    }
  }
  return;
}



