// FUN_00494620 @ 00494620 size=202

void __thiscall FUN_00494620(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  
  if (param_2 != 0) {
    if (((*(int *)(param_1 + 0xe0) != 0) && (DAT_0066829c != 0)) && (DAT_0067682c != 0)) {
      if (param_2 == DAT_00667fcc) {
        FUN_0054d170(&DAT_0065c5d0);
      }
      else {
        FUN_005871f0(param_2,*(int *)(param_1 + 0xe0));
      }
    }
    if ((*(int *)(param_1 + 0xd8) != 0) && (iVar2 = *(int *)(param_1 + 0xc), iVar2 != 0)) {
      cVar1 = *(char *)(param_1 + 0xb4);
      if (((cVar1 == '\x03') || (cVar1 == '\b')) && (*(int *)(param_1 + 0xbc) == iVar2)) {
        *(int *)(param_1 + 0xe4) = iVar2;
        return;
      }
      if (((cVar1 != '\t') || (*(int *)(param_1 + 0xbc) != iVar2)) &&
         ((DAT_0066829c != 0 && ((*(short *)(iVar2 + 4) == 0xc || (*(short *)(iVar2 + 4) == 0xb)))))
         ) {
        FUN_004d0950(*(int *)(param_1 + 0xd8),0xffffffff,0,*(undefined4 *)(param_1 + 0xdc));
        FUN_00471330(*(undefined4 *)(param_1 + 0xc));
      }
    }
  }
  return;
}


