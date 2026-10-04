// FUN_00426670 @ 00426670 size=373

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00426670(void)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  
  FUN_0041ee50();
  uVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  FUN_0058b100(&DAT_00654a88,s_Display_bits_per_pixel___d_005cc30c,uVar1);
  FUN_0041ee50(&DAT_00654a88);
  uVar2 = FUN_004a80b0();
  FUN_0058b100(&DAT_00654a88,s_Video_Memory_Total____3_1f_K_005cc328,(double)uVar2 * _DAT_005a3aa0);
  FUN_0041ee50(&DAT_00654a88);
  uVar2 = FUN_004a80b0();
  FUN_0058b100(&DAT_00654a88,s_Video_Memory_Available___3_1f_K_005cc34c,
               (double)uVar2 * _DAT_005a3aa0);
  FUN_0041ee50(&DAT_00654a88);
  FUN_0041ee50();
  uVar1 = FUN_00416420();
  FUN_0058b100(&DAT_00654a88,s_Number_of_3D_objects____d_005cc3ac,uVar1);
  FUN_0041ee50(&DAT_00654a88);
  uVar1 = FUN_00415b30();
  FUN_0058b100(&DAT_00654a88,s_Number_of_3D_lights____d_005cc3c8,uVar1);
  FUN_0041ee50(&DAT_00654a88);
  iVar3 = FUN_004a8350();
  if (iVar3 != 2) {
    FUN_004a8350();
  }
  FUN_0041ee50();
  iVar3 = FUN_004a8370();
  if (iVar3 != 0) {
    FUN_0041ee50();
    return 0;
  }
  FUN_0041ee50();
  return 0;
}


