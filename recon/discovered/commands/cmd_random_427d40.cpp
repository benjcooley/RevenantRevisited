// FUN_00427d40 @ 00427d40 size=54

undefined4 FUN_00427d40(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(param_2 + 0x10) == 8) {
    uVar1 = *(undefined4 *)(param_2 + 0x14);
  }
  else {
    uVar1 = 100;
  }
  uVar1 = FUN_00483300(1,uVar1);
  FUN_0041ee50(s_Random_Number__d_005cca48,uVar1);
  FUN_00479580();
  return 0;
}


