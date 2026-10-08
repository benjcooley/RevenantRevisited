// FUN_00586a60 @ 00586a60 size=336

undefined4 __thiscall FUN_00586a60(int param_1,undefined4 param_2)

{
  short sVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined4 uVar4;
  int iVar5;
  undefined *puVar6;
  
  if ((DAT_00676828 != 0) &&
     (iVar2 = FUN_0057d9d0(param_2,0x5e,0,9,0), uVar4 = DAT_0065a534, iVar2 != 0)) {
    iVar2 = *(int *)(param_1 + 0x60);
    puVar3 = *(undefined1 **)(iVar2 + 0xc);
    *(undefined1 **)(iVar2 + 0xc) = puVar3 + 2;
    if (*(undefined1 **)(iVar2 + 4) < puVar3 + 2) {
      puVar3 = (undefined1 *)FUN_005884a0(puVar3);
    }
    *puVar3 = (char)uVar4;
    puVar3[1] = (char)((uint)uVar4 >> 8);
    if (DAT_0065a558 == 0) {
      uVar4 = 0;
    }
    else {
      uVar4 = *(undefined4 *)(DAT_0065a558 + 0x40);
    }
    iVar2 = *(int *)(param_1 + 0x60);
    puVar3 = *(undefined1 **)(iVar2 + 0xc);
    *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
    if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
      puVar3 = (undefined1 *)FUN_005884a0(puVar3);
    }
    param_2._2_1_ = (char)((uint)uVar4 >> 0x10);
    param_2._3_1_ = (undefined1)((uint)uVar4 >> 0x18);
    *puVar3 = (char)uVar4;
    puVar3[1] = (char)((uint)uVar4 >> 8);
    puVar3[2] = param_2._2_1_;
    puVar3[3] = param_2._3_1_;
    puVar6 = DAT_0065a55c;
    if (DAT_0065a55c == (undefined *)0x0) {
      puVar6 = &DAT_00676eb4;
    }
    FUN_00588660(puVar6);
    puVar6 = DAT_0065a560;
    if (DAT_0065a560 == (undefined *)0x0) {
      puVar6 = &DAT_00676eb8;
    }
    FUN_00588660(puVar6);
    sVar1 = DAT_0065a54c;
    iVar2 = *(int *)(param_1 + 0x60);
    puVar3 = *(undefined1 **)(iVar2 + 0xc);
    *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
    if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
      puVar3 = (undefined1 *)FUN_005884a0(puVar3);
    }
    param_2._2_1_ = (char)(sVar1 >> 0xf);
    *puVar3 = (char)sVar1;
    iVar2 = 0;
    puVar3[1] = (char)((ushort)sVar1 >> 8);
    puVar3[2] = param_2._2_1_;
    puVar3[3] = param_2._2_1_;
    if (0 < DAT_0065a54c) {
      iVar5 = 0;
      if (DAT_0065a54c < 1) {
        uVar4 = 0;
        goto LAB_00586b79;
      }
      do {
        uVar4 = *(undefined4 *)(iVar5 + 8 + DAT_0065a550);
LAB_00586b79:
        FUN_00588660(uVar4);
        iVar2 = iVar2 + 1;
        iVar5 = iVar5 + 0x48;
      } while (iVar2 < DAT_0065a54c);
    }
    FUN_0057dc70();
    FUN_00532f40();
    return 1;
  }
  return 0;
}


