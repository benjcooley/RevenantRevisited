// FUN_00420d60 @ 00420d60 size=82

undefined4 FUN_00420d60(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar1 = FUN_0047a410(param_2,s__d__d__d_005caf50,&uStack_4,&uStack_8,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  (**(code **)(*param_1 + 0x200))(uStack_4,uStack_8,param_2);
  return 0;
}


