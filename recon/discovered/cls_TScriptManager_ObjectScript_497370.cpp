// FUN_00497370_TScriptManager_ObjectScript @ 00497370 size=329

int __thiscall FUN_00497370_TScriptManager_ObjectScript(int param_1,int param_2)

{
  char *pcVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  uVar4 = 0;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      if ((((*(int *)(param_1 + 0x8014) != 0) && (uVar4 < *(uint *)(param_1 + 0x8004))) &&
          (*(int *)(*(int *)(param_1 + 0x8014) + uVar4 * 4) != 0)) &&
         (((pcVar1 = (char *)**(undefined4 **)(*(int *)(param_1 + 0x8014) + uVar4 * 4),
           pcVar1 != (char *)0x0 && (*pcVar1 != '\0')) &&
          (iVar2 = FUN_0059a530_stricmp(pcVar1,*(undefined4 *)(param_2 + 0x38)), iVar2 == 0)))) {
        iVar2 = FUN_00482fb0(0xe8);
        if (iVar2 == 0) goto LAB_00497498;
        iVar2 = FUN_00492170_TScript_Ctor(param_2,*(undefined4 *)(*(int *)(param_1 + 0x8014) + uVar4 * 4));
        goto LAB_0049749a;
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < *(int *)(param_1 + 0x8004));
  }
  uVar4 = 0;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      if (((*(int *)(param_1 + 0x8014) != 0) && (uVar4 < *(uint *)(param_1 + 0x8004))) &&
         ((*(int *)(*(int *)(param_1 + 0x8014) + uVar4 * 4) != 0 &&
          (((pcVar1 = (char *)**(undefined4 **)(*(int *)(param_1 + 0x8014) + uVar4 * 4),
            pcVar1 != (char *)0x0 && (*pcVar1 != '\0')) &&
           (iVar2 = FUN_0059a530_stricmp(pcVar1,**(undefined4 **)(param_2 + 0x4c)), iVar2 == 0)))))) {
        iVar2 = FUN_00482fb0(0xe8);
        if (iVar2 == 0) {
LAB_00497498:
          iVar2 = 0;
        }
        else {
          iVar2 = FUN_00492170_TScript_Ctor(param_2,*(undefined4 *)(*(int *)(param_1 + 0x8014) + uVar4 * 4));
        }
LAB_0049749a:
        uVar3 = FUN_0041c840(iVar2);
        *(undefined4 *)(iVar2 + 0x14) = uVar3;
        return iVar2;
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < *(int *)(param_1 + 0x8004));
  }
  return 0;
}


