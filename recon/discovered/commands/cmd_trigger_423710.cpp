// FUN_00423710 @ 00423710 size=71

undefined4 FUN_00423710(int param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  if (*(int *)(param_1 + 0x84) != 0) {
    FUN_00492640(2,*(undefined4 *)(param_2 + 0x28),0,0,0,0,0);
  }
  FUN_00479580();
  return 0;
}


