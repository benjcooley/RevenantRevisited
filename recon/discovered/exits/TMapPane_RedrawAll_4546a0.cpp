// TMapPane_RedrawAll @ 0x004546a0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// FUN_004546a0 @ 004546a0 size=507

void __fastcall FUN_004546a0(int *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int unaff_ESI;
  int iStack_28;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  int iStack_18;
  int iStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  
  if (param_1[0x3d] == 0) {
    param_1[0x3d] = 1;
    (**(code **)(*param_1 + 0x2c))(1);
    FUN_0041d9f0();
    DAT_00658d94 = 1;
    if ((((DAT_00668154 == 0) && (DAT_00667fcc != 0)) && (DAT_005d7a04 == 0)) && (DAT_0065d0d0 != 0)
       ) {
      uStack_10 = *(undefined4 *)(DAT_00667fcc + 0x10);
      uStack_c = *(undefined4 *)(DAT_00667fcc + 0x14);
      uStack_8 = *(undefined4 *)(DAT_00667fcc + 0x18);
      FUN_0046d7a0(&uStack_10,&stack0xffffffd4,&iStack_28);
      iVar3 = DAT_00667c30 + -0x40;
      iVar4 = unaff_ESI;
      if (unaff_ESI < 1) {
        iVar4 = -unaff_ESI;
      }
      iVar1 = iVar3 / 2;
      if (iVar4 < iVar1) {
        iVar4 = 0;
      }
      else {
        if (unaff_ESI < 0) {
          iVar1 = -iVar1;
        }
        iVar4 = (unaff_ESI + iVar1) / iVar3;
      }
      iStack_28 = iStack_28 + -0x20;
      iVar1 = DAT_0065c5c4 + -0x40;
      iVar5 = iStack_28;
      if (iStack_28 < 1) {
        iVar5 = -iStack_28;
      }
      iVar2 = iVar1 / 2;
      if (iVar5 < iVar2) {
        iStack_28 = 0;
      }
      else {
        if (iStack_28 < 0) {
          iVar2 = -iVar2;
        }
        iStack_28 = (iStack_28 + iVar2) / iVar1;
      }
      iStack_28 = iVar1 * iStack_28;
      FUN_0046dad0(iVar3 * iVar4,iStack_28,&iStack_1c,0);
      iVar3 = iStack_1c - param_1[0x2c];
      if (iVar3 < 1) {
        iVar3 = param_1[0x2c] - iStack_1c;
      }
      if (iVar3 < 5) {
        iVar3 = iStack_18 - param_1[0x2d];
        if (iVar3 < 1) {
          iVar3 = param_1[0x2d] - iStack_18;
        }
        if (iVar3 < 5) {
          return;
        }
      }
      FUN_0046d7a0(&iStack_1c,&iStack_24,&iStack_20);
      iVar3 = DAT_0065c5c4 / 2;
      param_1[0xd] = iStack_24 - DAT_00667c30 / 2;
      param_1[0xe] = iStack_20 - iVar3;
      if (((iStack_1c != param_1[0x2c]) || (iStack_18 != param_1[0x2d])) ||
         (iStack_14 != param_1[0x2e])) {
        FUN_0049beb0(iStack_1c,iStack_18,iStack_14);
      }
      param_1[0x29] = iStack_1c;
      param_1[0x2a] = iStack_18;
      param_1[0x2b] = iStack_14;
    }
  }
  return;
}


