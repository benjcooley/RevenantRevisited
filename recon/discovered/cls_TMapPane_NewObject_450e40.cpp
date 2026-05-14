// FUN_00450e40_TMapPane_NewObject @ 00450e40 size=582

int __thiscall FUN_00450e40_TMapPane_NewObject(int param_1,short *param_2,undefined4 param_3)

{
  int iVar1;
  int unaff_EBX;
  int iVar2;
  int *local_84;
  int local_3c;
  
  iVar2 = -1;
  if ((DAT_0065a258_TObjectClass_numclasses <= (uint)(int)*param_2) || ((&DAT_0065a148_TObjectClass_classes)[*param_2] == 0)) {
    return -1;
  }
  iVar1 = FUN_00474bb0_NewObject(param_2,param_3,1);
  if (iVar1 == 0) {
    return -1;
  }
  if ((*param_2 == 0xc) || (*param_2 == 0xb)) {
    iVar2 = FUN_00451cb0_TMapPane_AddShadow(iVar1);
  }
  *(undefined4 *)(iVar1 + 0x40) = 0xffffffff;
  iVar1 = FUN_00451090_TMapPane_AddObject(iVar1);
  if ((iVar1 < 0) && (-1 < iVar2)) {
    FUN_0044cf80_TMapIterator_Initialize(0,0x80,0,0,0xffffffff);
    while (local_84 != (int *)0x0) {
      if (local_84[0x10] == iVar2) goto LAB_00450f25;
      FUN_0044d080_TMapIterator_NextItem();
    }
    local_84 = (int *)FUN_0051f330(iVar2);
LAB_00450f25:
    if (local_84 != (int *)0x0) {
      if ((((short)local_84[1] == 0xc) || ((short)local_84[1] == 0xb)) && (local_84[0xa1] != 0)) {
        FUN_004830f0(local_84[0xa1]);
        local_84[0xa1] = 0;
      }
      if (((local_84 == *(int **)(param_1 + 0xdc)) &&
          (*(undefined4 *)(param_1 + 0xdc) = 0, (*(byte *)(param_1 + 0xd8) & 1) != 0)) &&
         (DAT_00667fcc != 0)) {
        *(int *)(param_1 + 0xdc) = DAT_00667fcc;
        *(undefined4 *)(param_1 + 0xd8) = 9;
      }
      iVar2 = local_84[0x14];
      if (-1 < iVar2) {
        FUN_0045f800_TMapIterator_Ctor(0,0x80,0,0,0xffffffff);
        while (local_3c != 0) {
          if (*(int *)(local_3c + 0x40) == iVar2) goto LAB_00450fd5;
          FUN_0044d080_TMapIterator_NextItem();
        }
        local_3c = FUN_0051f330(iVar2);
LAB_00450fd5:
        if (local_3c != 0) {
          FUN_00451610_TMapPane_RemoveObject(local_3c);
        }
      }
      iVar2 = (**(code **)(*local_84 + 0x20))();
      if (iVar2 != 0) {
        (**(code **)(*local_84 + 0x2c))();
      }
      iVar2 = (**(code **)(*local_84 + 0xb4))();
      if ((iVar2 == 0) && (local_84[0x19] == 0)) {
        FUN_00584270(local_84,0);
        if (local_84[0x11] != 0) {
          FUN_00452750(local_84,3,0);
          FUN_00456710(s_d__revenant_MapPane_cpp_005d0468,0x800);
          FUN_00454920(local_84[0x10]);
          FUN_00499250_TSector_RemoveObject(local_84);
          FUN_00456740();
        }
      }
      else {
        (**(code **)(*local_84 + 0x60))();
      }
      FUN_0046e6d0(local_84);
      (**(code **)*local_84)(1);
      iVar1 = unaff_EBX;
    }
  }
  return iVar1;
}


