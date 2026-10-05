// TMapPane_AddObjectUpdateRect @ 0x00454920 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// inferred name (queues the object's screen rect)
// FUN_00454920 @ 00454920 size=258

void __thiscall FUN_00454920(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 unaff_EBX;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  int *local_3c;
  
  if (-1 < param_2) {
    FUN_0044cf80(0,0x80,0,0,0xffffffff);
    while (local_3c != (int *)0x0) {
      if (local_3c[0x10] == param_2) goto LAB_00454975;
      FUN_0044d080();
    }
    local_3c = (int *)FUN_0051f330(param_2);
LAB_00454975:
    if ((((local_3c != (int *)0x0) && (iVar3 = (**(code **)(*local_3c + 0xfc))(), iVar3 != 4)) &&
        ((**(code **)(*local_3c + 0xf4))(&uStack_58), *(int *)(param_1 + 0x50) == 0)) && (iVar3 < 4)
       ) {
      iVar2 = *(int *)(param_1 + 0x130);
      if (0x3f < iVar2) {
        FUN_004546a0();
        *(undefined4 *)(param_1 + 0x130) = 0;
        return;
      }
      puVar1 = (undefined4 *)(param_1 + (iVar2 + 0xb) * 0x1c);
      *puVar1 = unaff_EBX;
      puVar1[1] = uStack_58;
      puVar1[2] = uStack_54;
      puVar1[3] = uStack_50;
      *(int *)(param_1 + 0x144 + iVar2 * 0x1c) = iVar3;
      *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
    }
  }
  return;
}


