// FUN_0046f3d0 @ 0046f3d0 size=1035

/* WARNING: Removing unreachable block (ram,0x0046f454) */
/* WARNING: Removing unreachable block (ram,0x0046f469) */
/* WARNING: Removing unreachable block (ram,0x0046f476) */
/* WARNING: Removing unreachable block (ram,0x0046f4b4) */
/* WARNING: Removing unreachable block (ram,0x0046f480) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_0046f3d0(int *param_1,int *param_2,uint param_3)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  int *piVar7;
  int unaff_EBX;
  int *unaff_retaddr;
  uint uStack_24;
  int *piStack_4;
  
  piVar4 = param_2;
  iVar3 = (**(code **)(*param_1 + 0x170))();
  if (iVar3 != 0) {
    piVar4 = (int *)(**(code **)(*param_1 + 0x170))();
    uVar5 = (**(code **)(*piVar4 + 0x58))(param_2,param_3);
    return uVar5;
  }
  if ((int)param_3 < 0) {
    if ((short)param_1[1] == 0xb) {
      piStack_4 = (int *)0x0;
      FUN_0046dfb0();
    }
    param_3 = (**(code **)(*param_1 + 0x94))();
    if (0xfe < param_3) {
      return 0;
    }
  }
  if (0x115 < (int)param_3) {
    return 0;
  }
  iVar3 = (**(code **)(*param_1 + 0x170))();
  if (iVar3 == 0) {
    FUN_00477870(param_1,0);
    while (piStack_4 != (int *)0x0) {
      if ((int)(short)piStack_4[0x1f] == param_3) goto LAB_0046f532;
      FUN_0046dfb0();
    }
    piStack_4 = (int *)0x0;
  }
  else {
    (**(code **)(*param_1 + 0x170))(param_3);
    piStack_4 = (int *)FUN_004701f0(param_3);
  }
LAB_0046f532:
  if (piStack_4 != param_2) {
    (**(code **)(*param_2 + 0x128))();
    uStack_24 = 0;
    param_2 = (int *)0x0;
    piVar2 = (int *)param_1[0x19];
    piVar7 = param_1;
    while (piVar1 = piVar2, piVar1 != (int *)0x0) {
      piVar7 = piVar1;
      piVar2 = (int *)piVar1[0x19];
    }
    piVar7 = (int *)(-(uint)(piVar7 != param_1) & (uint)piVar7);
    if (piVar7 == (int *)0x0) {
      piVar7 = param_1;
    }
    if (piVar4[0x19] != 0) {
      iVar3 = (**(code **)(*piVar7 + 0xac))(piVar4[0x10]);
      if (iVar3 != 0) {
        param_2 = (int *)piVar4[0x19];
      }
      uStack_24 = (uint)(iVar3 != 0);
      if (piVar4[0x19] != 0) {
        DAT_00676e5d = 1;
        (**(code **)(*piVar4 + 0x60))();
        DAT_00676e5d = 0;
      }
    }
    iVar3 = (**(code **)(*piVar4 + 0x98))(param_1);
    if (iVar3 != 0) {
      if (unaff_EBX == 0) {
        FUN_00585880(param_1,piVar4,param_2);
      }
      else {
        FUN_00585ab0(param_1,piVar4,param_2);
      }
      if (piVar4 != (int *)0x0) {
        FUN_0046e630();
        (**(code **)*piVar4)(1);
      }
      _DAT_0065d548 = 1;
      (**(code **)(DAT_0065d4f8 + 0x90))();
      _DAT_0065b078 = 1;
      return 1;
    }
    if (piStack_4 != (int *)0x0) {
      DAT_00676e5d = 1;
      (**(code **)(*piStack_4 + 0x60))();
      DAT_00676e5d = 0;
    }
    iVar3 = FUN_0041c840(piVar4);
    if (iVar3 < 0) {
      return 0;
    }
    if (piVar4[0x10] < 1) {
      iVar6 = FUN_0044ce30();
      piVar4[0x10] = iVar6;
    }
    *(short *)((int)piVar4 + 0x7e) = (short)iVar3;
    *(short *)(piVar4 + 0x1f) = (short)param_2;
    piVar4[0x19] = (int)param_1;
    piVar4[6] = 0;
    piVar4[5] = 0;
    piVar4[4] = 0;
    *(undefined2 *)((int)piVar4 + 0xe) = 0;
    piVar4[0x11] = 0;
    if (piStack_4 != (int *)0x0) {
      DAT_00676e5d = 1;
      if (unaff_retaddr == (int *)0x0) {
        iVar3 = *param_1;
        uStack_24 = 0xffffffff;
      }
      else {
        iVar3 = *unaff_retaddr;
      }
      (**(code **)(iVar3 + 0x58))(piStack_4,uStack_24);
      DAT_00676e5d = 0;
    }
    iVar3 = FUN_0059a530(piVar4[0xe],s_Swag_Bag_005d4830);
    if ((((iVar3 == 0) && ((short)param_1[1] == 0xb)) && ((char)param_1[0x125] != '\0')) &&
       (((iVar3 = FUN_0051f840(param_1 + 0x125), -1 < iVar3 &&
         (iVar3 = *(int *)(DAT_0065a8b4 + iVar3 * 4), iVar3 != 0)) &&
        ((piVar7 = (int *)FUN_0051eea0(*(undefined4 *)(iVar3 + 0x4c),0), piVar7 != (int *)0x0 &&
         ((piVar7 != param_1 &&
          (iVar3 = (**(code **)(*piVar7 + 0xa8))(s_Swag_Bag_005d483c), iVar3 != 0)))))))) {
      (**(code **)(*piVar4 + 0x174))(iVar3);
    }
    if (unaff_EBX == 0) {
      FUN_00585880(param_1,piVar4,param_2);
    }
    else {
      FUN_00585ab0(param_1,piVar4,param_2);
    }
    if ((DAT_0065d674 != (int *)0x0) &&
       ((((param_1 == DAT_0065d674 ||
          (piVar4 = (int *)(**(code **)(*DAT_0065d674 + 0x170))(), param_1 == piVar4)) ||
         (unaff_retaddr == DAT_0065d674)) ||
        (piVar4 = (int *)(**(code **)(*DAT_0065d674 + 0x170))(), unaff_retaddr == piVar4)))) {
      _DAT_0065d548 = 1;
      (**(code **)(DAT_0065d4f8 + 0x90))();
    }
    if ((param_1 == DAT_0065b088) || (unaff_retaddr == DAT_0065b088)) {
      _DAT_0065b078 = 1;
    }
  }
  return 1;
}


