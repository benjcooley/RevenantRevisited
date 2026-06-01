// FUN_00537f90 @ 00537f90 size=636

/* WARNING: Removing unreachable block (ram,0x005380b4) */
/* WARNING: Removing unreachable block (ram,0x005380c0) */
/* WARNING: Removing unreachable block (ram,0x005380d0) */
/* WARNING: Removing unreachable block (ram,0x005380e7) */
/* WARNING: Removing unreachable block (ram,0x005380ef) */
/* WARNING: Removing unreachable block (ram,0x0053810f) */
/* WARNING: Removing unreachable block (ram,0x00538166) */
/* WARNING: Removing unreachable block (ram,0x0053815d) */
/* WARNING: Removing unreachable block (ram,0x0053816f) */
/* WARNING: Removing unreachable block (ram,0x00538194) */
/* WARNING: Removing unreachable block (ram,0x005381ed) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00537f90(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  if ((param_2 != 0) && (*(int *)(param_1 + 0x17c) != 0)) {
    if (DAT_00668518 == 0) {
      iVar2 = (DAT_00668510 - *(int *)(param_1 + 4)) + -8;
      iVar3 = (DAT_00668514 - *(int *)(param_1 + 8)) + -0x2a;
      if ((((iVar2 < 0) || (iVar3 < 0)) || (3 < iVar2 / 0x2d)) ||
         (((2 < iVar3 / 0x2c || (0x27 < iVar2 % 0x2d)) || (0x27 < iVar3 % 0x2c)))) {
        iVar2 = -1;
      }
      else {
        iVar2 = (iVar2 / 0x2d) * 3 + *(int *)(param_1 + 0x188) + iVar3 / 0x2c;
      }
      uVar1 = FUN_004701f0(iVar2);
      FUN_0043a820(uVar1,0);
    }
    if (*(int *)(param_1 + 0x17c) != 0) {
      FUN_0047e920();
      FUN_0046dfb0();
    }
  }
  return;
}


