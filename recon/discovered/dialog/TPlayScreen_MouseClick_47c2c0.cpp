// FUN_0047c2c0 @ 0047c2c0 size=310

void __thiscall FUN_0047c2c0(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined8 uVar5;
  
  if (*(int *)(param_1 + 0x50) == 0) {
    FUN_00490030(param_2);
    if (param_2 != 0) {
      if (*(int *)(param_1 + 0x1c) < 1) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(param_1 + 0x2c + *(int *)(param_1 + 0x1c) * 4);
      }
      if (((uVar2 & 0xf0) == 0) && (DAT_006682bc == 0)) {
        FUN_00491a80(0);
        FUN_0043a5a0();
      }
    }
    if (((*(int *)(param_1 + 0x5d4) == 0) && (DAT_00668154 == 0)) && (DAT_00666920 == 0)) {
      iVar1 = (**(code **)(DAT_006668d8 + 0x40))();
      if ((iVar1 != 0) && (DAT_00666918 != 0)) {
        if (*(int *)(param_1 + 0x1c) < 1) {
          uVar2 = 0;
        }
        else {
          uVar2 = *(uint *)(param_1 + 0x2c + *(int *)(param_1 + 0x1c) * 4);
        }
        if ((uVar2 & 8) == 0) {
          iVar4 = *(int *)(param_1 + 0x680) + 1;
          iVar1 = (*(int *)(param_1 + 0x688) - *(int *)(param_1 + 0x684)) + iVar4;
          *(int *)(param_1 + 0x680) = iVar4;
          uVar5 = __allmul(iVar1,iVar1 >> 0x1f,100,0);
          iVar4 = __alldiv(uVar5,0x18,0);
          *(int *)(param_1 + 0x68c) = iVar4;
          uVar5 = __allmul(iVar4,iVar4 >> 0x1f,0x5a0,0);
          iVar1 = DAT_0065d804;
          uVar5 = __alldiv(uVar5,DAT_0065d804,DAT_0065d804 >> 0x1f);
          uVar3 = __allrem(uVar5,0x5a0,0);
          *(undefined4 *)(param_1 + 0x690) = uVar3;
          *(int *)(param_1 + 0x694) = iVar4 / iVar1;
        }
      }
    }
  }
  return;
}


