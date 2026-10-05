// FUN_00518fe0 @ 00518fe0 size=101

void __thiscall FUN_00518fe0(int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = DAT_0066829c;
  if ((*(int *)(param_1 + 0x650) != param_2) || (*(int *)(param_1 + 0x658) != param_3)) {
    *(int *)(param_1 + 0x650) = param_2;
    *(int *)(param_1 + 0x658) = param_3;
    if (iVar1 != 0) {
      if (DAT_0067682c != 0) {
        FUN_00587350(param_1,param_2,param_3,param_4);
      }
      FUN_0051f6b0();
      FUN_0057b1f0();
    }
  }
  return;
}


