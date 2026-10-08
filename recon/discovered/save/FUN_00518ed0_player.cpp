// FUN_00518ed0 @ 00518ed0 size=189

void __thiscall FUN_00518ed0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = DAT_0067682c;
  iVar1 = DAT_0066829c;
  if ((param_2 != 0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
    if (*(short *)(param_2 + 4) == 0xb) {
      iVar4 = *(int *)(param_1 + 0x658);
      iVar3 = *(int *)(param_1 + 0x650) + 1;
      if (*(int *)(param_1 + 0x650) == iVar3) {
        return;
      }
      *(int *)(param_1 + 0x650) = iVar3;
      *(int *)(param_1 + 0x658) = iVar4;
    }
    else {
      iVar3 = *(int *)(param_1 + 0x650);
      iVar4 = *(int *)(param_1 + 0x658) + 1;
      if (*(int *)(param_1 + 0x658) == iVar4) {
        return;
      }
      *(int *)(param_1 + 0x650) = iVar3;
      *(int *)(param_1 + 0x658) = iVar4;
    }
    if (iVar1 != 0) {
      if (iVar2 != 0) {
        FUN_00587350(param_1,iVar3,iVar4,param_2);
      }
      FUN_0051f6b0();
      FUN_0057b1f0();
    }
  }
  return;
}


