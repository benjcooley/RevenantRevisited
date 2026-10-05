// TMapPane_SectorFrame @ 0x00459220 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// per-frame sector update wrapper
// FUN_00459220 @ 00459220 size=272

void __fastcall FUN_00459220(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  *(undefined4 *)(param_1 + 0xbc) = *(undefined4 *)(param_1 + 0xb0);
  *(undefined4 *)(param_1 + 0xc0) = *(undefined4 *)(param_1 + 0xb4);
  *(undefined4 *)(param_1 + 0xc4) = *(undefined4 *)(param_1 + 0xb8);
  *(undefined4 *)(param_1 + 0xb0) = *(undefined4 *)(param_1 + 0xa4);
  *(undefined4 *)(param_1 + 0xb8) = *(undefined4 *)(param_1 + 0xac);
  *(undefined4 *)(param_1 + 0xb4) = *(undefined4 *)(param_1 + 0xa8);
  iVar2 = *(int *)(param_1 + 0xb4);
  *(undefined4 *)(param_1 + 100) = *(undefined4 *)(param_1 + 0x6c);
  iVar1 = *(int *)(param_1 + 0xb0);
  *(undefined4 *)(param_1 + 0x60) = *(undefined4 *)(param_1 + 0x68);
  *(int *)(param_1 + 0x68) = iVar1 >> 10;
  *(undefined4 *)(param_1 + 0xa0) = *(undefined4 *)(param_1 + 0x9c);
  *(int *)(param_1 + 0x6c) = iVar2 >> 10;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x98);
  if (*(int *)(param_1 + 0xa0) == *(int *)(param_1 + 0x98)) {
    iVar3 = iVar1 - *(int *)(param_1 + 0xbc);
    if (iVar3 < 0) {
      iVar3 = *(int *)(param_1 + 0xbc) - iVar1;
    }
    iVar1 = iVar2 - *(int *)(param_1 + 0xc0);
    if (iVar1 < 0) {
      iVar1 = *(int *)(param_1 + 0xc0) - iVar2;
    }
    iVar2 = iVar3;
    if (iVar1 <= iVar3) {
      iVar2 = iVar1;
    }
    if ((iVar1 - (iVar2 >> 1)) + iVar3 < 0x401) goto LAB_004592fe;
  }
  *(undefined4 *)(DAT_00667fd0 + 0x6c) = 1;
LAB_004592fe:
  FUN_00459490();
  FUN_004597b0();
  FUN_00459a70();
  FUN_00459b80();
  if (DAT_00668154 == 0) {
    return;
  }
  FUN_004409d0();
  return;
}


