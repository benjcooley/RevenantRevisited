// FUN_0049ccc0 @ 0049ccc0 size=141

int __thiscall FUN_0049ccc0(int param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  undefined1 *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  
  uVar5 = 0xffffffff;
  iVar4 = *(int *)(param_1 + 4);
  pcVar7 = param_2;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  iVar2 = *(int *)(param_1 + 0xc);
  if (((iVar4 - *(int *)(param_1 + 8)) + iVar2 < (int)(uVar5 + 1)) &&
     (iVar6 = *(int *)(param_1 + 8) - iVar4, iVar2 - iVar6 < (int)(uVar5 + 1))) {
    iVar4 = FUN_00482f40(iVar4,iVar2 + *(int *)(param_1 + 0x10));
    *(int *)(param_1 + 4) = iVar4;
    *(int *)(param_1 + 8) = iVar4 + iVar6;
    *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) + *(int *)(param_1 + 0x10);
  }
  puVar3 = *(undefined1 **)(param_1 + 8);
  *puVar3 = (char)(uVar5 - 1);
  _strncpy(puVar3 + 1,param_2,uVar5 - 1 & 0xff);
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + uVar5;
  return param_1;
}


