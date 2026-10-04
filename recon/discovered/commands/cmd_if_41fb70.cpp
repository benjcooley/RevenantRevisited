// FUN_0041fb70 @ 0041fb70 size=54

int FUN_0041fb70(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_0041f230(param_2,&param_3,param_1,param_3);
  if (iVar1 == 0) {
    return 0x84;
  }
  return (-(uint)(param_3 != 0) & 0xffffffc0) + 0x80;
}


