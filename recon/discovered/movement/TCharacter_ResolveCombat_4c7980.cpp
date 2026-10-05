// TCharacter_ResolveCombat @ 0x004c7980 (vtable slot 0x328; TPlayer's 0x00519210 forwards here) -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/COMMAND_SYSTEM.md §6.5)
// Clears ab->target unless a pick-up is pending (+0x288); flag 0x1000 keeps a goto walk from
// turning to face the opponent.
// FUN_004c7980 @ 004c7980 size=1529

undefined4 __thiscall FUN_004c7980(int *param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  bool bVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint local_5c;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined1 auStack_4c [32];
  undefined1 auStack_2c [32];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  piVar4 = param_2;
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e448;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  if (param_1[0xa2] == 0) {
    ExceptionList = &pvStack_c;
    param_2[0x10] = 0;
    param_2[0xf] = 0;
    param_2[0xe] = 0;
  }
  uVar10 = param_2[0x11];
  param_2 = (int *)0x0;
  if ((((short)param_1[1] == 0xb) &&
      (uVar9 = param_1[0x10], uVar5 = FUN_0047e920(), ((uVar5 ^ uVar9) & 7) == 0)) ||
     ((DAT_00668110 == 0 &&
      (uVar9 = param_1[0x10], uVar5 = FUN_0047e920(), ((uVar5 ^ uVar9) & 0x1f) == 0)))) {
    param_2 = (int *)0x1;
  }
  uVar9 = 0;
  if ((uVar10 == 0) || (iVar6 = FUN_004cd990(uVar10), iVar6 == 0)) {
    if (param_2 == (int *)0x0) {
LAB_004c7a40:
      uVar5 = 0;
    }
    else {
      iVar6 = FUN_004cd690(&local_5c,1,0xffffffff,0xffffffff,0x20,7);
      uVar9 = (iVar6 < 1) - 1 & local_5c;
      uVar5 = uVar9;
      if (uVar9 == 0) goto LAB_004c7a40;
    }
    FUN_004d4790(uVar5);
  }
  if (param_1[0x8d] == 0) {
    if (param_2 != (int *)0x0) {
      iVar6 = FUN_004cd690(&param_2,1,0xffffffff,piVar4[0xc],0x20,7);
      uVar9 = (iVar6 < 1) - 1 & (uint)param_2;
    }
    if ((uVar9 != 0) && (uVar9 != uVar10)) {
      local_5c = (**(code **)(*param_1 + 4))(uVar9);
      if (uVar10 != 0) {
        param_2 = (int *)(**(code **)(*param_1 + 4))(uVar10);
        iVar6 = piVar4[0xc];
        uVar7 = FUN_0046ea90(uVar10);
        iVar6 = FUN_0046ded0(uVar7,iVar6);
        if (iVar6 < 0) {
          iVar6 = piVar4[0xc];
          uVar7 = FUN_0046ea90(uVar10);
          iVar6 = FUN_0046ded0(uVar7,iVar6);
          iVar6 = -iVar6;
        }
        else {
          iVar6 = piVar4[0xc];
          uVar7 = FUN_0046ea90(uVar10);
          iVar6 = FUN_0046ded0(uVar7,iVar6);
        }
        if ((((int)param_2 < 0x21) || (iVar6 < 0x21)) && ((int)param_2 <= (int)local_5c))
        goto LAB_004c7b13;
      }
      FUN_004d4790(uVar9);
    }
  }
LAB_004c7b13:
  piVar1 = (int *)param_1[0x38];
  if ((piVar1 == (int *)0x0) || ((*piVar1 != 3 && ((piVar1 == (int *)0x0 || (*piVar1 != 0x19)))))) {
    param_2 = (int *)0x0;
  }
  else {
    param_2 = (int *)piVar1[0x11];
  }
  piVar4[0x11] = (int)param_2;
  if ((piVar4[0x18] & 0x200U) == 0) {
    local_5c = piVar4[0xb];
    bVar3 = false;
    uVar10 = local_5c & 0xff;
    if (((short)param_1[1] == 0xc) ||
       (((short)param_1[1] == 0xb &&
        (((DAT_005d7a64 != 0 || ((int *)param_1[0x36] == (int *)0x0)) ||
         ((iVar6 = *(int *)param_1[0x36], iVar6 != 2 && ((iVar6 != 4 && (iVar6 != 0x1a)))))))))) {
      bVar3 = true;
    }
    if ((piVar4[0x18] & 0x1000U) != 0) {
      bVar3 = false;
    }
    if (param_2 == (int *)0x0) {
LAB_004c7d01:
      iVar6 = param_1[0x8d];
      if (param_1[0x8d] != 0) goto LAB_004c7d0b;
    }
    else {
      iVar6 = FUN_004cd540(param_2,0xffffffff);
      if ((iVar6 == 0) && (param_1[0x8d] != 0)) {
        bVar3 = false;
      }
      if ((!bVar3) || (iVar6 = (int)param_2, param_1[0x95] != 0)) goto LAB_004c7d01;
LAB_004c7d0b:
      uStack_58 = *(undefined4 *)(iVar6 + 0x10);
      uStack_54 = *(undefined4 *)(iVar6 + 0x14);
      uStack_50 = *(undefined4 *)(iVar6 + 0x18);
      uVar10 = FUN_0046dc60(param_1 + 4,&uStack_58);
      local_5c = uVar10;
    }
    if ((short)param_1[1] == 0xb) {
      if (((int *)param_1[0x36] == (int *)0x0) ||
         (((iVar6 = *(int *)param_1[0x36], iVar6 != 2 && (iVar6 != 4)) && (iVar6 != 0x1a)))) {
        if (piVar4[0xb] != uVar10) {
          iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar10);
          if (iVar6 < 0) {
            iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar10);
            iVar6 = -iVar6;
          }
          else {
            iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar10);
          }
          piVar4[0xc] = uVar10;
          uVar9 = iVar6 - 0x20U & ((int)(iVar6 - 0x20U) < 0) - 1;
          piVar4[0xb] = uVar10;
          piVar4[0xd] = ((int)(uVar9 + ((int)uVar9 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
        }
      }
      else {
        FUN_004d39f0(piVar4[0xc],local_5c,param_1[0x38] + 4,auStack_2c,0x20);
        iVar6 = FUN_0059a530(piVar4 + 1,auStack_2c);
        if (iVar6 != 0) {
          param_2 = (int *)FUN_00482fb0(100);
          uStack_4 = 1;
          if (param_2 == (int *)0x0) {
            iVar6 = 0;
          }
          else {
            iVar6 = FUN_004da9f0(auStack_2c,*(undefined4 *)param_1[0x36]);
          }
          iVar8 = piVar4[0xc];
          uVar10 = uVar10 + 0xf & 0xe0;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar6 + 0x2c) = uVar10;
          *(int *)(iVar6 + 0x30) = iVar8;
          iVar8 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar10);
          if (iVar8 < 0) {
            iVar8 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),*(undefined4 *)(iVar6 + 0x2c))
            ;
            iVar8 = -iVar8;
          }
          else {
            iVar8 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),*(undefined4 *)(iVar6 + 0x2c))
            ;
          }
          uVar10 = iVar8 - 0x20U & ((int)(iVar8 - 0x20U) < 0) - 1;
          *(int *)(iVar6 + 0x44) = piVar4[0x11];
          *(int *)(iVar6 + 0x34) = ((int)(uVar10 + ((int)uVar10 >> 0x1f & 0x1fU)) >> 5) * 8 + 0x10;
          param_1[0x2c] = *(int *)(iVar6 + 0x30);
          (**(code **)(*param_1 + 0x218))(iVar6,0,0);
          ExceptionList = pvStack_c;
          return 0;
        }
      }
    }
    else if (piVar4[0xb] != uVar10) {
      uVar9 = uVar10 & 0xff;
      iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar9);
      if (iVar6 < 0) {
        iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar9);
        iVar6 = -iVar6;
      }
      else {
        iVar6 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar9);
      }
      piVar4[0xb] = uVar10;
      uVar9 = iVar6 - 0x20U & ((int)(iVar6 - 0x20U) < 0) - 1;
      piVar4[0xd] = ((int)(uVar9 + ((int)uVar9 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
      if (*piVar4 == 4) {
        piVar4[0xc] = uVar10;
      }
      else {
        piVar4[0xc] = param_1[0x2c];
      }
    }
    iVar6 = piVar4[0xd];
    iVar8 = piVar4[0xc];
    uVar10 = piVar4[0xb];
    param_1[0x2c] = iVar8;
LAB_004c7f5b:
    FUN_004c5ad0(uVar10,iVar8,iVar6);
  }
  else {
    param_1[0x2d] = 0;
    iVar6 = FUN_0059a530(piVar4 + 1,piVar1 + 1);
    if (iVar6 == 0) {
      uVar10 = piVar4[0xb];
      if (*(byte *)((int)param_1 + 0x36) != uVar10) {
        iVar6 = piVar4[0xd];
        iVar8 = piVar4[0xc];
        goto LAB_004c7f5b;
      }
    }
    else if (param_1[0x20] == 0) {
      ExceptionList = pvStack_c;
      return 0;
    }
    FUN_004d39f0(piVar4[0xc],piVar4[0xb],param_1[0x38] + 4,auStack_4c,0x20);
    param_2 = (int *)FUN_00482fb0(100);
    uStack_4 = 0;
    if (param_2 == (int *)0x0) {
      iVar6 = 0;
    }
    else {
      iVar6 = FUN_004da9f0(auStack_4c,*(undefined4 *)param_1[0x36]);
    }
    uStack_4 = 0xffffffff;
    iVar8 = (**(code **)(*param_1 + 0x1f0))(auStack_4c,0);
    if (iVar8 != 0) {
      iVar8 = piVar4[0x11];
      iVar2 = piVar4[0xb];
      *(int *)(iVar6 + 0x30) = piVar4[0xc];
      *(int *)(iVar6 + 0x44) = iVar8;
      iVar8 = param_1[0x36];
      *(int *)(iVar6 + 0x2c) = iVar2;
      *(uint *)(iVar6 + 0x60) = *(uint *)(iVar6 + 0x60) & 0xfffffdff;
      *(undefined4 *)(iVar6 + 0x34) = 8;
      if (((iVar8 == 0) || ((*(byte *)(iVar8 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar8)) {
        (**(code **)(*param_1 + 0x218))(iVar6,0,0);
      }
      else if (iVar6 != 0) {
        if (*(int *)(iVar6 + 0x5c) != 0) {
          FUN_00482f80(*(int *)(iVar6 + 0x5c));
        }
        FUN_004830f0(iVar6);
      }
    }
  }
  ExceptionList = pvStack_c;
  return 0;
}


