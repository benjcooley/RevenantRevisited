// FUN_004982b0_TSector_LoadSector @ 004982b0 size=426

int FUN_004982b0_TSector_LoadSector(int param_1,int param_2,int param_3,int param_4)

{
  void *pvVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  pvVar1 = ExceptionList;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db4b;
  local_c = ExceptionList;
  iVar2 = 0;
  piVar3 = DAT_00668588;
  if (0 < DAT_00668578) {
    do {
      iVar5 = *piVar3;
      if ((((iVar5 != 0) && (*(int *)(iVar5 + 4) == param_1)) && (*(int *)(iVar5 + 8) == param_2))
         && (*(int *)(iVar5 + 0xc) == param_3)) {
        if (iVar5 != 0) {
          ExceptionList = &local_c;
          *(int *)(iVar5 + 0x94) = *(int *)(iVar5 + 0x94) + 1;
          if (*(int *)(iVar5 + 0xb0) <= param_4) {
            ExceptionList = pvVar1;
            return iVar5;
          }
          iVar2 = 0;
          if (0 < *(int *)(iVar5 + 0xb8)) {
            do {
              if (((*(int *)(*(int *)(iVar5 + 200) + iVar2 * 4) != 0) &&
                  (iVar4 = FUN_0046e8a0(), iVar4 != 0)) &&
                 (iVar4 = FUN_00446490(*(undefined4 *)(iVar4 + 8)), *(int *)(iVar4 + 0x60) == 0)) {
                FUN_00447ac0(param_4 == 2);
              }
              iVar2 = iVar2 + 1;
            } while (iVar2 < *(int *)(iVar5 + 0xb8));
          }
          *(int *)(iVar5 + 0xb0) = param_4;
          ExceptionList = local_c;
          return iVar5;
        }
        break;
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < DAT_00668578);
  }
  ExceptionList = &local_c;
  iVar2 = FUN_00482fb0(0x15c);
  iVar5 = 0;
  local_4 = 0;
  if (iVar2 != 0) {
    iVar5 = FUN_00498020_TSector_Ctor(param_1,param_2,param_3);
  }
  local_4 = 0xffffffff;
  FUN_004984d0_TSector_Load(param_4);
  FUN_0041c840(iVar5);
  if (((DAT_005d7a18 == 0) && (DAT_00668154 == 0)) &&
     ((*(int *)(iVar5 + 0x98) == 0 && (*(int *)(iVar5 + 0xac) == 0)))) {
    for (iVar2 = 0; iVar2 < *(int *)(iVar5 + 0xfc); iVar2 = iVar2 + 1) {
      piVar3 = *(int **)(*(int *)(iVar5 + 0x10c) + iVar2 * 4);
      if (piVar3 == (int *)0x0) {
        piVar3 = *(int **)(iVar5 + 0x110);
      }
      if (*(int *)(*(int *)(iVar5 + 200) + *piVar3 * 4) != 0) {
        FUN_00471b40(0);
      }
    }
    *(undefined4 *)(iVar5 + 0xac) = 1;
  }
  ExceptionList = local_c;
  return iVar5;
}


