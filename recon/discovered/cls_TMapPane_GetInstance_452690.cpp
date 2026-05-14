// FUN_00452690_TMapPane_GetInstance @ 00452690 size=94

int FUN_00452690_TMapPane_GetInstance(int param_1,undefined4 param_2)

{
  int iVar1;
  int local_3c;
  
  if (param_1 < 0) {
    return 0;
  }
  FUN_0044cf80_TMapIterator_Initialize(0,0x80,param_2,0,0xffffffff);
  while( true ) {
    if (local_3c == 0) {
      iVar1 = FUN_0051f330(param_1);
      return iVar1;
    }
    if (*(int *)(local_3c + 0x40) == param_1) break;
    FUN_0044d080_TMapIterator_NextItem();
  }
  return local_3c;
}


