// FUN_00427090 @ 00427090 size=423

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00427090(int param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  if ((DAT_0066829c != 0) && (DAT_0067682c != 0)) {
    iVar2 = 0;
    if (((param_1 == 0) || (iVar1 = param_1, *(short *)(param_1 + 4) != 0xb)) &&
       ((((param_3 == 0 || (iVar1 = param_3, *(short *)(param_3 + 4) != 0xb)) &&
         (iVar1 = iVar2, param_4 != 0)) &&
        (((*(int *)(param_4 + 0xc4) != 0 &&
          (iVar2 = FUN_0059a530(*(undefined4 *)(param_4 + 0xcc),&DAT_005cabd8), iVar2 == 0)) &&
         (*(short *)(*(int *)(param_4 + 0xc4) + 4) == 0xb)))))) {
      iVar1 = *(int *)(param_4 + 0xc4);
    }
    if (iVar1 != DAT_00667fcc) {
      iVar2 = 0;
      if ((param_1 != 0) && (*(short *)(param_1 + 4) == 0xb)) {
        FUN_00586a60(param_1);
        return 0;
      }
      if ((param_3 != 0) && (*(short *)(param_3 + 4) == 0xb)) {
        FUN_00586a60(param_3);
        return 0;
      }
      if ((((param_4 != 0) && (*(int *)(param_4 + 0xc4) != 0)) &&
          (iVar1 = FUN_0059a530(*(undefined4 *)(param_4 + 0xcc),&DAT_005cabd8), iVar1 == 0)) &&
         (*(short *)(*(int *)(param_4 + 0xc4) + 4) == 0xb)) {
        iVar2 = *(int *)(param_4 + 0xc4);
      }
      FUN_00586a60(iVar2);
      return 0;
    }
  }
  iVar2 = 0;
  _DAT_00667e5c = param_1;
  _DAT_00667eb0 = 0;
  _DAT_0065d1a8 = 1;
  if ((param_1 != 0) && (*(short *)(param_1 + 4) == 0xb)) {
    _DAT_0065a56c = param_1;
    return 0;
  }
  if ((param_3 != 0) && (*(short *)(param_3 + 4) == 0xb)) {
    _DAT_0065a56c = param_3;
    return 0;
  }
  if ((((param_4 != 0) && (*(int *)(param_4 + 0xc4) != 0)) &&
      (iVar1 = FUN_0059a530(*(undefined4 *)(param_4 + 0xcc),&DAT_005cabd8), iVar1 == 0)) &&
     (*(short *)(*(int *)(param_4 + 0xc4) + 4) == 0xb)) {
    iVar2 = *(int *)(param_4 + 0xc4);
  }
  _DAT_0065a56c = iVar2;
  return 0;
}


