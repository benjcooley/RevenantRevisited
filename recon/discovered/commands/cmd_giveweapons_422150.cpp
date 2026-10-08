// FUN_00422150 @ 00422150 size=59

undefined4 FUN_00422150(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_28 [40];
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cb310,auStack_28);
  if (iVar1 == 0) {
    return 4;
  }
  (**(code **)(*param_1 + 0x68))(auStack_28);
  return 0;
}


