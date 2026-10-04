// FUN_00429480 @ 00429480 size=258

undefined4 FUN_00429480(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  undefined4 auStack_c0 [48];
  
  puVar4 = auStack_c0;
  for (iVar1 = 0x30; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  iVar3 = 0;
  iVar1 = *(int *)(param_2 + 0x10);
  if (iVar1 != 9) {
    puVar4 = auStack_c0;
    do {
      if ((iVar1 == 10) || (0xf < iVar3)) break;
      iVar1 = FUN_0047a410(param_2,s__d__d_005ccf10,puVar4,puVar4 + 1);
      if (iVar1 == 0) {
        return 4;
      }
      iVar1 = *(int *)(param_2 + 0x10);
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 3;
    } while (iVar1 != 9);
    if (0 < iVar3) {
      iVar2 = 0;
      iVar5 = 0;
      iVar1 = FUN_0051ee70(0);
      if (0 < iVar1) {
        do {
          iVar1 = FUN_0051eea0(iVar2,0);
          if (((iVar1 != 0) && (*(char *)(iVar1 + 0x494) != '\0')) &&
             (iVar1 = FUN_0059a530(iVar1 + 0x494,param_1 + 0x494), iVar1 == 0)) {
            FUN_004cedb0(auStack_c0[iVar5 * 3],auStack_c0[iVar5 * 3 + 1],0);
            iVar5 = iVar5 + 1;
            if (iVar3 <= iVar5) {
              iVar5 = 0;
            }
          }
          iVar2 = iVar2 + 1;
          iVar1 = FUN_0051ee70(0);
        } while (iVar2 < iVar1);
      }
      return 0;
    }
  }
  return 4;
}


