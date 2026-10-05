// TExit_GetExitStrip @ 0x0050ce70 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x27c (Ghidra mangles the out-params; see EXITS.md 1.4)
// FUN_0050ce70 @ 0050ce70 size=328

void __thiscall
FUN_0050ce70(int *param_1,int *param_2,int *param_3,undefined4 param_4,int *param_5,int *param_6,
            undefined4 param_7)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int *unaff_EBP;
  
  piVar1 = param_6;
  FUN_00470f50(param_2,param_3,param_5,param_6);
  piVar2 = (int *)FUN_0046e8a0();
  (**(code **)(*piVar2 + 0xb4))((short)param_1[3],&param_6,&param_6,param_7);
  iVar3 = *param_3;
  if (iVar3 < 1) {
    iVar3 = 1;
  }
  iVar4 = *param_1;
  *param_3 = iVar3;
  iVar3 = (**(code **)(iVar4 + 0x25c))();
  if (iVar3 != 0) {
    iVar4 = (**(code **)(*param_1 + 0x25c))();
    iVar3 = *param_2;
    if (iVar4 == 2) {
      *param_2 = ((int)(iVar3 + (iVar3 >> 0x1f & 3U)) >> 2) * 3;
      *unaff_EBP = (*unaff_EBP + 1) / 2;
      iVar3 = *piVar1;
      *param_5 = ((int)(*param_5 + (*param_5 >> 0x1f & 3U)) >> 2) * 3;
      *piVar1 = (iVar3 + 1) / 2;
      return;
    }
    *param_2 = iVar3 / 2;
    *unaff_EBP = *unaff_EBP / 2;
    *param_5 = *param_5 / 2;
    *piVar1 = *piVar1 / 2;
    return;
  }
  uVar5 = (**(code **)(*param_1 + 0x254))();
  if ((int)uVar5 < 0) {
    uVar5 = (uint)*(byte *)((int)param_1 + 0x36);
  }
  if (((int)uVar5 < 0xe0) && (0x1f < (int)uVar5)) {
    if ((int)uVar5 < 0x60) {
      *param_2 = *param_2 + (1 - *param_5);
    }
    else if ((int)uVar5 < 0xa0) {
      *unaff_EBP = *unaff_EBP + (1 - *piVar1);
      goto LAB_0050cfab;
    }
    *param_5 = 1;
    return;
  }
LAB_0050cfab:
  *piVar1 = 1;
  return;
}


