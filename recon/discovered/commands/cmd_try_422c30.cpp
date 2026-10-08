// FUN_00422c30 @ 00422c30 size=141

undefined4 FUN_00422c30(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_50 [80];
  
  if ((*(uint *)(param_1 + 8) & 0x20000) == 0) {
    return 0;
  }
  iVar1 = FUN_0047a410(param_2,&DAT_005cb5b0,auStack_50);
  if (iVar1 == 0) {
    iVar1 = FUN_0047a410(param_2,&DAT_005cb5b4,auStack_50);
    if (iVar1 == 0) {
      return 4;
    }
  }
  FUN_004111a0(auStack_50);
  if (DAT_0067682c != 0) {
    FUN_00584060(param_1,0x4e,auStack_50,0,9);
  }
  return 1;
}


