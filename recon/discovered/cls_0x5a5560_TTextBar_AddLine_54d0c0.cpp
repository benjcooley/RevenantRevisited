// FUN_0054d0c0 @ 0054d0c0 size=176

void __thiscall FUN_0054d0c0(int param_1,undefined4 param_2,undefined4 param_3,char *param_4)

{
  undefined4 *puVar1;
  char *_Dest;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  if (((param_4 != (char *)0x0) && (*param_4 != '\0')) && (*param_4 != ' ')) {
    uVar3 = (uint)(*(int *)(param_1 + 0xa0) != 0);
    iVar4 = uVar3 * 0x5c;
    FUN_0058b790(*(int *)(param_1 + 0x6c) + (uVar3 + 1) * 0x5c,*(int *)(param_1 + 0x6c) + iVar4,
                 uVar3 * -0x5c + 0x3f4);
    if (*(int *)(param_1 + 0x60) < *(int *)(param_1 + 100)) {
      *(int *)(param_1 + 0x60) = *(int *)(param_1 + 0x60) + 1;
    }
    _Dest = (char *)(*(int *)(param_1 + 0x6c) + 0xc + iVar4);
    _strncpy(_Dest,param_4,0x4f);
    iVar2 = *(int *)(param_1 + 0x6c);
    _Dest[0x4f] = '\0';
    puVar1 = (undefined4 *)(iVar2 + iVar4);
    *puVar1 = param_2;
    puVar1[1] = param_3;
    puVar1[2] = 0x78;
    FUN_0054cd40(0xffffffff);
  }
  return;
}


