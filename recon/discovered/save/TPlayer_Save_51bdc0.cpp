// FUN_0051bdc0 @ 0051bdc0 size=923

void __thiscall FUN_0051bdc0(int param_1,int param_2)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined *puVar7;
  undefined *puVar8;
  undefined *puVar9;
  undefined *puVar10;
  uint *puStack_c;
  int *piStack_8;
  uint uStack_4;
  
  FUN_004779d0(1);
  puVar2 = *(undefined1 **)(param_2 + 8);
  *puVar2 = 4;
  *(undefined1 **)(param_2 + 8) = puVar2 + 1;
  FUN_004d50d0(param_2);
  FUN_0049ccc0(param_1 + 0x2cc);
  FUN_0049ccc0(param_1 + 0x2d2);
  FUN_0049ccc0(param_1 + 0x2d8);
  FUN_0049ccc0(param_1 + 0x2de);
  FUN_0049ccc0(param_1 + 0x2e4);
  uVar3 = *(undefined4 *)(param_1 + 0x2ec);
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_0051ff80(param_1 + 0x2ec);
  for (; (uVar3 = DAT_0065d190, puStack_c != (uint *)0x0 && (uStack_4 < *puStack_c));
      uStack_4 = uStack_4 + 1) {
    iVar6 = 0;
    do {
      uVar1 = *(undefined1 *)(*piStack_8 + iVar6);
      FUN_004779d0(1);
      puVar2 = *(undefined1 **)(param_2 + 8);
      *puVar2 = uVar1;
      iVar6 = iVar6 + 1;
      *(undefined1 **)(param_2 + 8) = puVar2 + 1;
    } while (iVar6 < 6);
    piStack_8 = piStack_8 + 1;
  }
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 0xc);
  iVar4 = *(int *)(param_2 + 4);
  *puVar5 = uVar3;
  uVar3 = DAT_0065d1b8;
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar6 - (int)puVar5) + iVar4 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 0xc);
  iVar4 = *(int *)(param_2 + 4);
  *puVar5 = uVar3;
  uVar3 = DAT_0065d1bc;
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar6 - (int)puVar5) + iVar4 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 0xc);
  iVar4 = *(int *)(param_2 + 4);
  *puVar5 = uVar3;
  uVar3 = DAT_0065d19c;
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar6 - (int)puVar5) + iVar4 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 0xc);
  iVar4 = *(int *)(param_2 + 4);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x360);
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar6 - (int)puVar5) + iVar4 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x364);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x368);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_0049ccc0(param_1 + 0x494);
  if ((DAT_0065a250 & 0x10) == 0) {
    puVar10 = (undefined *)(param_1 + 0x4c6);
  }
  else {
    puVar10 = &DAT_0066da5c;
  }
  FUN_0049ccc0(puVar10);
  uVar3 = *(undefined4 *)(param_1 + 0x4d8);
  if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x490);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x4dc);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_0049ccc0(param_1 + 0x4f0);
  uVar3 = *(undefined4 *)(param_1 + 0x36c);
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x370);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  if ((DAT_0065a250 & 0x10) == 0) {
    puVar10 = (undefined *)(param_1 + 0x5d0);
    puVar9 = (undefined *)(param_1 + 0x590);
    puVar8 = (undefined *)(param_1 + 0x570);
    puVar7 = (undefined *)(param_1 + 0x378);
  }
  else {
    puVar10 = &DAT_0066da6c;
    puVar9 = &DAT_0066da68;
    puVar8 = &DAT_0066da64;
    puVar7 = &DAT_0066da60;
  }
  FUN_0049ccc0(puVar7);
  FUN_0049ccc0(puVar8);
  FUN_0049ccc0(puVar9);
  FUN_0049ccc0(puVar10);
  uVar3 = *(undefined4 *)(param_1 + 0x650);
  if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x654);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar6 = *(int *)(param_2 + 0xc);
  iVar4 = *(int *)(param_2 + 4);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x658);
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar6 - (int)puVar5) + iVar4 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined4 *)(param_1 + 0x65c);
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  FUN_00529830(param_2);
  return;
}


