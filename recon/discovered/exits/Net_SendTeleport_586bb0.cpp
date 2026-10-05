// Net_SendTeleport @ 0x00586bb0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// inferred role: multiplayer host broadcast after a teleport
// FUN_00586bb0 @ 00586bb0 size=271

undefined4 __thiscall
FUN_00586bb0(int param_1,undefined4 param_2,undefined4 *param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  undefined1 *puVar3;
  
  if (DAT_00676828 != 0) {
    iVar2 = FUN_0057d9d0(param_2,0x53,0,9,0);
    if (iVar2 != 0) {
      iVar2 = *(int *)(param_1 + 0x60);
      puVar3 = *(undefined1 **)(iVar2 + 0xc);
      uVar1 = *param_3;
      *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
      if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
        puVar3 = (undefined1 *)FUN_005884a0(puVar3);
      }
      param_2._2_1_ = (undefined1)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      param_2._3_1_ = (undefined1)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      uVar1 = param_3[1];
      puVar3[2] = param_2._2_1_;
      iVar2 = *(int *)(param_1 + 0x60);
      puVar3[3] = param_2._3_1_;
      puVar3 = *(undefined1 **)(iVar2 + 0xc);
      *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
      if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
        puVar3 = (undefined1 *)FUN_005884a0(puVar3);
      }
      param_2._2_1_ = (undefined1)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      param_2._3_1_ = (undefined1)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      uVar1 = param_3[2];
      puVar3[2] = param_2._2_1_;
      iVar2 = *(int *)(param_1 + 0x60);
      puVar3[3] = param_2._3_1_;
      puVar3 = *(undefined1 **)(iVar2 + 0xc);
      *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
      if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
        puVar3 = (undefined1 *)FUN_005884a0(puVar3);
      }
      param_2._2_1_ = (undefined1)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      param_2._3_1_ = (undefined1)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      puVar3[2] = param_2._2_1_;
      iVar2 = *(int *)(param_1 + 0x60);
      puVar3[3] = param_2._3_1_;
      puVar3 = *(undefined1 **)(iVar2 + 0xc);
      *(undefined1 **)(iVar2 + 0xc) = puVar3 + 4;
      if (*(undefined1 **)(iVar2 + 4) < puVar3 + 4) {
        puVar3 = (undefined1 *)FUN_005884a0(puVar3);
      }
      *puVar3 = (char)param_4;
      puVar3[1] = (char)((uint)param_4 >> 8);
      puVar3[2] = param_4._2_1_;
      puVar3[3] = param_4._3_1_;
      FUN_0057dc70();
      return 1;
    }
  }
  return 0;
}


