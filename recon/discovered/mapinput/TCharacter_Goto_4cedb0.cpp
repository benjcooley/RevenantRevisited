// FUN_004cedb0 @ 004cedb0 size=156

undefined4 __thiscall FUN_004cedb0(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_4 = *(undefined4 *)(param_1 + 0x18);
  local_c = param_2;
  local_8 = param_3;
  uVar1 = FUN_0046dc60(param_1 + 0x10,&local_c);
  iVar2 = FUN_004ce350(uVar1);
  if (iVar2 != 0) {
    iVar2 = *(int *)(param_1 + 0xdc);
    if (iVar2 == *(int *)(param_1 + 0xe0)) {
      iVar2 = *(int *)(param_1 + 0xd8);
    }
    uVar1 = *(undefined4 *)(param_1 + 0x18);
    *(undefined4 *)(iVar2 + 0x38) = param_2;
    *(undefined4 *)(iVar2 + 0x3c) = param_3;
    *(undefined4 *)(iVar2 + 0x40) = uVar1;
    *(uint *)(iVar2 + 0x60) = *(uint *)(iVar2 + 0x60) | 0x1000;
    if (param_4 != 0) {
      *(int *)(param_1 + 0x288) = param_4;
    }
    FUN_00586680(param_1,param_2,param_3,param_4);
    return 1;
  }
  return 0;
}


