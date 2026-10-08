// FUN_00492b00 @ 00492b00 size=310

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00492b00(int param_1,uint param_2,int param_3)

{
  int iVar1;
  char cVar2;
  
  if (*(char *)(param_1 + 0xb4) != '\0') {
    return;
  }
  if ((((*(int *)(param_1 + 0xcc) == 0) ||
       (iVar1 = FUN_0059a530(*(int *)(param_1 + 0xcc),&DAT_005da134), iVar1 != 0)) ||
      (iVar1 = *(int *)(param_1 + 0xc4), iVar1 == 0)) || (*(short *)(iVar1 + 4) != 0xb)) {
    iVar1 = DAT_00667fcc;
  }
  if ((((param_2 == 2) || (param_2 == 5)) || (param_2 == 10)) && (iVar1 == DAT_00667fcc)) {
    if (DAT_00667e58 != 0) {
      return;
    }
    _DAT_00667eb0 = (uint)(param_2 == 10);
    _DAT_00667e5c = iVar1;
    FUN_00535e90();
  }
  cVar2 = (char)param_2;
  *(char *)(param_1 + 0xb4) = cVar2;
  if (((cVar2 == '\x06') || (cVar2 == '\a')) ||
     ((cVar2 == '\x02' || ((cVar2 == '\x05' || (cVar2 == '\n')))))) {
    *(int *)(param_1 + 0xbc) = iVar1;
    param_3 = iVar1;
  }
  else {
    *(int *)(param_1 + 0xbc) = param_3;
  }
  if (((((((cVar2 != '\x03') && (cVar2 != '\b')) && (cVar2 != '\t')) &&
        ((cVar2 != '\x06' && (cVar2 != '\a')))) &&
       ((cVar2 != '\x02' && ((cVar2 != '\x05' && (cVar2 != '\n')))))) ||
      (*(int *)(param_1 + 0xbc) != 0)) &&
     ((((DAT_0066829c != 0 && (DAT_0067682c != 0)) && (iVar1 != 0)) && (iVar1 != DAT_00667fcc)))) {
    FUN_00586cc0(iVar1,*(undefined4 *)(param_1 + 0xc),param_2 & 0xff,param_3);
    *(undefined1 *)(param_1 + 0xb5) = 0x78;
  }
  return;
}


