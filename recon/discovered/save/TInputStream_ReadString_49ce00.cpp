// FUN_0049ce00 @ 0049ce00 size=52

int __thiscall FUN_0049ce00(int param_1,char *param_2)

{
  byte *pbVar1;
  uint uVar2;
  
  _strncpy(param_2,(char *)(*(byte **)(param_1 + 4) + 1),(uint)**(byte **)(param_1 + 4));
  pbVar1 = *(byte **)(param_1 + 4);
  uVar2 = (uint)*pbVar1;
  param_2[uVar2] = '\0';
  *(byte **)(param_1 + 4) = pbVar1 + uVar2 + 1;
  return param_1;
}


