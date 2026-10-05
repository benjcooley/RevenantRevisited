// FUN_0054cbb0 @ 0054cbb0 size=400

void __fastcall FUN_0054cbb0(int *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined *puVar5;
  int iVar6;
  int iVar7;
  undefined4 uStack_70;
  undefined1 auStack_54 [84];
  
  if (param_1[0x10] != 0) {
    iVar6 = (**(code **)(*param_1 + 0x3c))();
    if ((iVar6 == 0) && (param_1[0x24] != 0)) {
      if (param_1[0x25] != 0) {
        (**(code **)(*DAT_00667fd0 + 0x48))();
        param_1[0x25] = 0;
        return;
      }
      uVar1 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x14);
      uVar2 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x18);
      uVar3 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x20);
      uVar4 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x1c);
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      (**(code **)(*param_1 + 0x4c))();
      iVar7 = *(int *)(PTR_DAT_005d79e0 + 8) - param_1[0x1e];
      iVar6 = FUN_0048eaf0();
      puVar5 = PTR_DAT_005d79e0;
      if (iVar6 != 0) {
        iVar7 = *(int *)(PTR_DAT_005d79e0 + 8) - param_1[0x1e];
      }
      if (DAT_006680c8 == 0) {
        FUN_00414d70(0,iVar7,1,param_1[0x23],0,param_1[3],param_1[0x1e],0xffffffff,0,param_1[0x1f],
                     param_1[3],param_1[0x1e],0,2);
      }
      else {
        iVar6 = param_1[0x23];
        FUN_00438d80(auStack_54,0,iVar7,0,param_1[0x1f],param_1[3],param_1[0x1e],0x120);
        (**(code **)(*(int *)puVar5 + 0x5c))(auStack_54,iVar6,0,0);
      }
      FUN_004aa0c0(0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8));
      puVar5 = PTR_DAT_005d79e0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x48))(uVar4);
      (**(code **)(*(int *)puVar5 + 0x40))(uStack_70,uVar1);
      (**(code **)(*(int *)puVar5 + 0x44))(uStack_70,uVar1,uVar2,uVar3);
    }
  }
  return;
}


