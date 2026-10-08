// FUN_00422d70 @ 00422d70 size=96

undefined4 FUN_00422d70(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cb5bc,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  if ((param_1 != 0) && ((*(short *)(param_1 + 4) == 0xc || (*(short *)(param_1 + 4) == 0xb)))) {
    FUN_004d4790(0);
  }
  DAT_00666970 = param_2;
  if (param_2 != DAT_00666974) {
    FUN_004546a0();
  }
  return 0;
}


