// FUN_0046faf0 @ 0046faf0 size=235

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0046faf0(int *param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  
  iVar2 = (**(code **)(*param_1 + 0x170))();
  if (iVar2 != 0) {
    (**(code **)(*param_1 + 0x174))(0);
  }
  if (param_1[0x19] != 0) {
    FUN_005859c0(param_1);
    if ((((param_1[0x19] != 0) && (*(short *)(param_1[0x19] + 4) == 0xb)) &&
        (0xff < (short)param_1[0x1f])) && ((short)param_1[0x1f] < 0x10b)) {
      uVar3 = (**(code **)(*param_1 + 0x188))();
      FUN_005199b0(0,uVar3);
    }
    FUN_0041cb80((int)*(short *)((int)param_1 + 0x7e));
    iVar4 = 0;
    iVar2 = *(int *)(param_1[0x19] + 0x68);
    if (0 < iVar2) {
      piVar5 = *(int **)(param_1[0x19] + 0x78);
      do {
        iVar1 = *piVar5;
        piVar5 = piVar5 + 1;
        *(short *)(iVar1 + 0x7e) = (short)iVar4;
        iVar4 = iVar4 + 1;
      } while (iVar4 < iVar2);
    }
    piVar5 = DAT_0065d674;
    param_1[0x19] = 0;
    if (piVar5 != (int *)0x0) {
      iVar2 = (**(code **)(*piVar5 + 0x170))();
      if (param_1[0x19] == iVar2) {
        _DAT_0065d548 = 1;
        (**(code **)(DAT_0065d4f8 + 0x90))();
      }
    }
    if (param_1[0x19] == DAT_0065b088) {
      _DAT_0065b078 = 1;
    }
  }
  *(undefined2 *)((int)param_1 + 0x7e) = 0xffff;
  *(undefined2 *)(param_1 + 0x1f) = 0xffff;
  return;
}


