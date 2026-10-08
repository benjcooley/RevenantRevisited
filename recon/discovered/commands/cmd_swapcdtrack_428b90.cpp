// FUN_00428b90 @ 00428b90 size=159

undefined4 FUN_00428b90(undefined4 param_1,int param_2)

{
  int iVar1;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005ccbdc,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  if (param_2 == -1) {
    if (-1 < DAT_005c8388) {
      FUN_0049a300(DAT_005c8388);
      DAT_0065abec = 0;
      DAT_005c8388 = param_2;
      return 0;
    }
    return 0xe;
  }
  DAT_005c8388 = DAT_0065abe0 + -1;
  FUN_0049a300(param_2);
  DAT_0065abec = 1;
  DAT_0065abf0 = GetTickCount();
  DAT_0065abf4 = FUN_0049a480(param_2);
  return 0;
}


