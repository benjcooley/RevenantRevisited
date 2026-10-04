// FUN_00422cc0 @ 00422cc0 size=114

undefined4 FUN_00422cc0(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_50 [80];
  
  if ((*(uint *)(param_1 + 8) & 0x20000) == 0) {
    return 0;
  }
  iVar1 = FUN_0047a410(param_2,&DAT_005cb5b8,auStack_50);
  if (iVar1 == 0) {
    return 4;
  }
  FUN_00429990(auStack_50);
  if (DAT_0067682c != 0) {
    FUN_00584060(param_1,0x4f,auStack_50,0,9);
  }
  return 1;
}


