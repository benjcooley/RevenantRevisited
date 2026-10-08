// FUN_00429590 @ 00429590 size=293

undefined4 FUN_00429590(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 uStack_c8;
  uint uStack_c4;
  undefined4 auStack_c0 [48];
  
  puVar5 = auStack_c0;
  for (iVar2 = 0x30; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  iVar4 = 0;
  iVar2 = FUN_0047a410(param_2,&DAT_005ccf18,&uStack_c8);
  if (iVar2 == 0) {
    return 4;
  }
  iVar2 = *(int *)(param_2 + 0x10);
  if (iVar2 != 9) {
    puVar5 = auStack_c0;
    do {
      if ((iVar2 == 10) || (0xf < iVar4)) break;
      iVar2 = FUN_0047a410(param_2,s__d__d_005ccf1c,puVar5,puVar5 + 1);
      if (iVar2 == 0) {
        return 4;
      }
      iVar2 = *(int *)(param_2 + 0x10);
      iVar4 = iVar4 + 1;
      puVar5 = puVar5 + 3;
    } while (iVar2 != 9);
    if (0 < iVar4) {
      iVar3 = 0;
      iVar6 = 0;
      iVar2 = FUN_0051ee70(0);
      if (0 < iVar2) {
        do {
          piVar1 = (int *)FUN_0051eea0(iVar3,0);
          if ((piVar1 != (int *)0x0) && ((char)piVar1[0x125] != '\0')) {
            iVar2 = FUN_0059a530(piVar1 + 0x125,param_1 + 0x494);
            uStack_c4 = (uint)(iVar2 == 0);
            if (uStack_c4 != 0) {
              (**(code **)(*piVar1 + 8))(auStack_c0 + iVar6 * 3,uStack_c8,0);
              iVar6 = iVar6 + 1;
              if (iVar4 <= iVar6) {
                iVar6 = 0;
              }
            }
          }
          iVar3 = iVar3 + 1;
          iVar2 = FUN_0051ee70(0);
        } while (iVar3 < iVar2);
      }
      return 0;
    }
  }
  return 4;
}


