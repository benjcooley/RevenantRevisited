// FUN_00420ab0 @ 00420ab0 size=415

undefined4 FUN_00420ab0(uint param_1,int param_2,uint param_3,uint *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_2 + 0x10) != 4) {
    return 4;
  }
  if (DAT_0066829c == 0) {
    if (param_1 == DAT_00667fcc) {
      iVar1 = FUN_00479700(&DAT_005caf24,0);
      if (iVar1 == 0) {
        iVar1 = FUN_00479700(&PTR_DAT_005caf28,0);
        if (iVar1 == 0) {
          return 4;
        }
        FUN_0047c550(1);
      }
      else {
        FUN_0047c550(0);
      }
    }
    else {
      iVar1 = FUN_00479700(&DAT_005caf2c,0);
      if (iVar1 == 0) {
        iVar1 = FUN_00479700(&PTR_DAT_005caf30,0);
        if (iVar1 == 0) {
          return 4;
        }
        uVar2 = 0;
      }
      else {
        uVar2 = 1;
      }
      FUN_0047c580(uVar2);
    }
  }
  else {
    if (((param_1 == 0) || (*(short *)(param_1 + 4) != 0xb)) &&
       ((param_3 == 0 || (param_1 = param_3, *(short *)(param_3 + 4) != 0xb)))) {
      if (param_4 == (uint *)0x0) goto LAB_00420c39;
      if (((param_4[0x31] == 0) || (iVar1 = FUN_0059a530(param_4[0x33],&DAT_005cabd8), iVar1 != 0))
         || (param_1 = param_4[0x31], *(short *)(param_1 + 4) != 0xb)) goto LAB_00420c0c;
    }
    if (param_1 != 0) {
      iVar1 = FUN_00479700(&DAT_005caf1c,0);
      if (iVar1 == 0) {
        iVar1 = FUN_00479700(&PTR_DAT_005caf20,0);
        if (iVar1 != 0) {
          FUN_0051d680(*(uint *)(param_1 + 0x36c) | 4);
        }
      }
      else {
        FUN_0051d680(*(uint *)(param_1 + 0x36c) & 0xfffffffb);
      }
    }
  }
LAB_00420c0c:
  if (param_4 != (uint *)0x0) {
    iVar1 = FUN_00479700(&DAT_005caf34,0);
    if (iVar1 != 0) {
      *param_4 = *param_4 & 0xfffffffe;
      FUN_00479580();
      return 0;
    }
    *param_4 = *param_4 | 1;
  }
LAB_00420c39:
  FUN_00479580();
  return 0;
}


