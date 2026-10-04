// FUN_00423d40 @ 00423d40 size=470

undefined4 FUN_00423d40(int *param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar4 = param_2;
  piVar1 = param_1;
  param_1 = (int *)0xffffffff;
  iStack_c = piVar1[4];
  iStack_8 = piVar1[5];
  iStack_4 = piVar1[6];
  if ((*(int *)(param_2 + 0x10) == 9) || (*(int *)(param_2 + 0x10) == 10)) {
    iStack_c = DAT_00666988;
    iStack_8 = DAT_0066698c;
    iStack_4 = DAT_00666990;
    param_1 = (int *)DAT_00666970;
  }
  else {
    iVar2 = FUN_00479700(&PTR_DAT_005cb8e8,0);
    if (iVar2 != 0) {
      FUN_00479580();
    }
    iVar3 = FUN_0047a410(iVar4,s__i__i_005cb8ec,&iStack_c,&iStack_8);
    if (iVar3 == 0) {
      return 4;
    }
    if (*(int *)(iVar4 + 0x10) == 8) {
      iVar3 = FUN_0047a410(iVar4,&DAT_005cb8f4,&iStack_4);
      if (iVar3 == 0) {
        return 4;
      }
      if (*(int *)(iVar4 + 0x10) == 8) {
        iVar4 = FUN_0047a410(iVar4,&DAT_005cb8f8,&param_1);
        if (iVar4 == 0) {
          return 4;
        }
      }
    }
    if (iVar2 != 0) {
      iStack_4 = iStack_4 + piVar1[6];
      iStack_8 = iStack_8 + piVar1[5];
      iStack_c = iStack_c + piVar1[4];
    }
  }
  FUN_00454920(piVar1[0x10]);
  if (((short)piVar1[1] == 0xc) || ((short)piVar1[1] == 0xb)) {
    FUN_004d4790(0);
  }
  if ((piVar1 == (int *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4)) && (DAT_00668154 == 0)) {
    DAT_006669b0 = DAT_006669b0 | 8;
  }
  (**(code **)(*piVar1 + 8))(&iStack_c,param_1,DAT_00655490);
  FUN_00454920(piVar1[0x10]);
  if ((DAT_0066829c != 0) && (DAT_0067682c != 0)) {
    FUN_00586bb0(piVar1,&stack0xffffffe8,iStack_8);
  }
  return 0;
}


