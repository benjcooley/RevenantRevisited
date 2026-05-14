// FUN_00499d60_TSector_FindPreloadSector @ 00499d60 size=79

int FUN_00499d60_TSector_FindPreloadSector(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = 0;
  piVar3 = DAT_006687a0;
  if (0 < DAT_00668790) {
    do {
      iVar1 = *piVar3;
      if (((*(int *)(iVar1 + 4) == param_1) && (*(int *)(iVar1 + 8) == param_2)) &&
         (*(int *)(iVar1 + 0xc) == param_3)) {
        return DAT_006687a0[iVar2];
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < DAT_00668790);
  }
  return 0;
}


