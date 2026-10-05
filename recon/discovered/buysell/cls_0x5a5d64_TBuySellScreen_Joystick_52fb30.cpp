// FUN_0052fb30 @ 0052fb30 size=328

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0052fb30(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  if (param_3 != 0) {
    switch(param_2) {
    case 0x401:
      iVar2 = param_1[0x62] + -1;
      param_1[0x62] = iVar2;
      if ((iVar2 < param_1[0x60]) &&
         (iVar1 = param_1[0x60] - param_1[0x61], param_1[0x60] = iVar1, iVar1 < 0)) {
        param_1[0x60] = 0;
      }
      if (iVar2 < 0) {
        param_1[0x62] = 0;
      }
      break;
    case 0x406:
      iVar1 = param_1[0x62] + 1;
      iVar2 = param_1[0x60] + param_1[0x61];
      param_1[0x62] = iVar1;
      if ((iVar2 <= iVar1) && (param_1[0x60] = iVar2, (short)param_1[0x65] + -1 < iVar2)) {
        param_1[0x60] = iVar2 - param_1[0x61];
      }
      if ((short)param_1[0x65] <= iVar1) {
        param_1[0x62] = (short)param_1[0x65] + -1;
      }
      break;
    case 0x408:
      FUN_0052ff40(0,3000);
      break;
    case 0x409:
      iVar2 = 0;
      param_1[0x6c] = 0;
      if (0 < (short)param_1[0x65]) {
        do {
          FUN_0052f310();
          iVar2 = iVar2 + 1;
        } while (iVar2 < (short)param_1[0x65]);
      }
      if (param_1[0x66] != 0) {
        FUN_004830f0(param_1[0x66]);
      }
      param_1[0x66] = 0;
      *(undefined2 *)(param_1 + 0x65) = 0;
      *(undefined2 *)((int)param_1 + 0x196) = 0;
      _DAT_0065d1a8 = 0;
    }
    (**(code **)(*param_1 + 0x2c))(1);
  }
  FUN_00436340(param_2,param_3);
  return;
}


