// FUN_00454450 @ 00454450 size=406

void __thiscall FUN_00454450(int *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  if (DAT_006682bc == 0) {
    FUN_0045abb0();
  }
  if ((param_2 != 0) && (DAT_006682bc == 0)) {
    (**(code **)(*param_1 + 0x24))(0);
    FUN_00412cd0();
  }
  if ((((DAT_00668154 != 0) && (param_2 != 0)) && (param_1[0x45] == 0)) &&
     ((param_1[0x46] == 0 && (DAT_006682bc == 0)))) {
    FUN_00457b30();
  }
  FUN_00458750(param_2);
  (**(code **)(*param_1 + 0x24))(0);
  if ((param_2 == 0) || (DAT_006682bc != 0)) goto LAB_004545cc;
  FUN_00412db0();
  iVar3 = DAT_00668154;
  if (DAT_006682bc != 0) goto LAB_004545cc;
  iVar1 = DAT_00668510 - param_1[1];
  iVar5 = DAT_00668514 - param_1[2];
  if ((((iVar1 < 0) || (iVar5 < 0)) || (param_1[3] <= iVar1)) || (param_1[4] <= iVar5))
  goto LAB_004545cc;
  if ((((byte)*(undefined4 *)(DAT_00667fd0 + 0x48) & 7) == 1) && (param_1[0x4b] = -1, iVar3 == 0)) {
    piVar2 = (int *)FUN_00452520(iVar1,iVar5,0);
    if (piVar2 != (int *)0x0) {
      iVar3 = FUN_0043a160();
      if (iVar3 == 0) {
        iVar3 = (**(code **)(*piVar2 + 0x134))();
        if ((iVar3 != 0) && ((*(byte *)(piVar2 + 2) & 0x80) == 0)) {
          iVar3 = *piVar2;
          param_1[0x4b] = 0;
          iVar3 = (**(code **)(iVar3 + 0x20))();
          if (iVar3 != 0) {
            (**(code **)(*piVar2 + 0x24))();
            FUN_004461c0();
          }
          goto LAB_004545bb;
        }
      }
      iVar3 = *piVar2;
      uVar4 = FUN_0043a160();
      iVar3 = (**(code **)(iVar3 + 0xc0))(uVar4);
      param_1[0x4b] = iVar3;
    }
  }
LAB_004545bb:
  FUN_0043a0d0(param_1[0x4b],0);
LAB_004545cc:
  FUN_0041c5f0(param_2);
  FUN_00454dd0(param_2);
  return;
}


