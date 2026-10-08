// FUN_004235a0 @ 004235a0 size=102

undefined4 FUN_004235a0(int param_1,int param_2)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0x10) == 8) {
    iVar1 = FUN_0047a410(param_2,&DAT_005cb794,&param_2);
    if (iVar1 == 0) {
      return 4;
    }
    if (param_1 != 0) {
      *(undefined1 *)(param_1 + 0x37) = (undefined1)param_2;
      return 0;
    }
  }
  else if (param_1 != 0) {
    FUN_0058b100(&DAT_00654a88,s_Group___d_005cb798,*(undefined1 *)(param_1 + 0x37));
    FUN_0041ee50(&DAT_00654a88);
  }
  return 0;
}


