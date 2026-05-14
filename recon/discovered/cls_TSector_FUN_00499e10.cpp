// FUN_00499e10_TSector_FindLoadedSector @ 00499e10 size=70

int FUN_00499e10_TSector_FindLoadedSector(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = 0;
  piVar3 = DAT_00668588;
  if (0 < DAT_00668578) {
    do {
      iVar1 = *piVar3;
      if ((((iVar1 != 0) && (*(int *)(iVar1 + 4) == param_1)) && (*(int *)(iVar1 + 8) == param_2))
         && (*(int *)(iVar1 + 0xc) == param_3)) {
        return iVar1;
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < DAT_00668578);
  }
  return 0;
}


