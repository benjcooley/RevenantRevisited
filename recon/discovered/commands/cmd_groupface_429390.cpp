// FUN_00429390 @ 00429390 size=227

undefined4 FUN_00429390(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  int iVar7;
  undefined4 auStack_40 [16];
  
  iVar5 = 0;
  puVar6 = auStack_40;
  for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  iVar3 = *(int *)(param_2 + 0x10);
  if (iVar3 != 9) {
    puVar6 = auStack_40;
    do {
      if ((iVar3 == 10) || (0xf < iVar5)) break;
      iVar3 = FUN_0047a410(param_2,&DAT_005ccf0c,puVar6);
      if (iVar3 == 0) {
        return 4;
      }
      iVar3 = *(int *)(param_2 + 0x10);
      iVar5 = iVar5 + 1;
      puVar6 = puVar6 + 1;
    } while (iVar3 != 9);
    if (0 < iVar5) {
      iVar4 = 0;
      iVar7 = 0;
      iVar3 = FUN_0051ee70(0);
      if (0 < iVar3) {
        do {
          iVar3 = FUN_0051eea0(iVar4,0);
          if (((iVar3 != 0) && (*(char *)(iVar3 + 0x494) != '\0')) &&
             (iVar2 = FUN_0059a530(iVar3 + 0x494,param_1 + 0x494), iVar2 == 0)) {
            uVar1 = auStack_40[iVar7];
            iVar7 = iVar7 + 1;
            *(char *)(iVar3 + 0x36) = (char)uVar1;
            *(undefined4 *)(iVar3 + 0xb0) = uVar1;
            if (iVar5 <= iVar7) {
              iVar7 = 0;
            }
          }
          iVar4 = iVar4 + 1;
          iVar3 = FUN_0051ee70(0);
        } while (iVar4 < iVar3);
      }
      return 0;
    }
  }
  return 4;
}


