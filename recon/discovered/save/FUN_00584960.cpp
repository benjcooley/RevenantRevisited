// FUN_00584960 @ 00584960 size=458

undefined4 __thiscall FUN_00584960(int param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined1 *puVar6;
  int iVar7;
  undefined1 local_c [9];
  undefined1 local_3;
  
  iVar4 = param_2;
  if (DAT_00676828 != 0) {
    iVar5 = FUN_0057d9d0(param_2,0x32,0,0xb,param_3);
    if (iVar5 != 0) {
      iVar5 = *(int *)(param_1 + 0x60);
      uVar3 = *(undefined4 *)(param_2 + 0x490);
      puVar6 = *(undefined1 **)(iVar5 + 0xc);
      *(undefined1 **)(iVar5 + 0xc) = puVar6 + 4;
      if (*(undefined1 **)(iVar5 + 4) < puVar6 + 4) {
        puVar6 = (undefined1 *)FUN_005884a0(puVar6);
      }
      param_3._2_1_ = (undefined1)((uint)uVar3 >> 0x10);
      param_3._3_1_ = (undefined1)((uint)uVar3 >> 0x18);
      *puVar6 = (char)uVar3;
      puVar6[1] = (char)((uint)uVar3 >> 8);
      puVar6[2] = param_3._2_1_;
      puVar6[3] = param_3._3_1_;
      FUN_00588660(param_2 + 0x494);
      FUN_00588660(param_2 + 0x4c6);
      iVar5 = *(int *)(param_1 + 0x60);
      uVar2 = *(undefined1 *)(param_2 + 0x4d8);
      puVar6 = *(undefined1 **)(iVar5 + 0xc);
      *(undefined1 **)(iVar5 + 0xc) = puVar6 + 1;
      if (*(undefined1 **)(iVar5 + 4) < puVar6 + 1) {
        puVar6 = (undefined1 *)FUN_005884a0(puVar6);
      }
      iVar5 = *(int *)(param_1 + 0x60);
      *puVar6 = uVar2;
      uVar2 = *(undefined1 *)(param_2 + 0x4dc);
      puVar6 = *(undefined1 **)(iVar5 + 0xc);
      *(undefined1 **)(iVar5 + 0xc) = puVar6 + 1;
      if (*(undefined1 **)(iVar5 + 4) < puVar6 + 1) {
        puVar6 = (undefined1 *)FUN_005884a0(puVar6);
      }
      iVar5 = *(int *)(param_1 + 0x60);
      *puVar6 = uVar2;
      FUN_00588570(local_c);
      iVar7 = *(int *)(iVar5 + 0xc);
      uVar1 = iVar7 + 1;
      *(uint *)(iVar5 + 0xc) = uVar1;
      if (*(uint *)(iVar5 + 4) < uVar1) {
        FUN_005884a0(iVar7);
      }
      local_3 = 1;
      param_2 = 0;
      iVar5 = FUN_0051ee70(0);
      if (0 < iVar5) {
        do {
          iVar5 = FUN_0051eea0(param_2,0);
          if (((iVar5 != 0) && (iVar5 != iVar4)) && (*(char *)(iVar5 + 0x494) != '\0')) {
            iVar7 = FUN_0059a530(iVar5 + 0x494,iVar4 + 0x494);
            if (iVar7 == 0) {
              iVar7 = *(int *)(param_1 + 0x60);
              uVar3 = *(undefined4 *)(iVar5 + 0x40);
              puVar6 = *(undefined1 **)(iVar7 + 0xc);
              *(undefined1 **)(iVar7 + 0xc) = puVar6 + 4;
              if (*(undefined1 **)(iVar7 + 4) < puVar6 + 4) {
                puVar6 = (undefined1 *)FUN_005884a0(puVar6);
              }
              param_3._2_1_ = (undefined1)((uint)uVar3 >> 0x10);
              param_3._3_1_ = (undefined1)((uint)uVar3 >> 0x18);
              *puVar6 = (char)uVar3;
              puVar6[1] = (char)((uint)uVar3 >> 8);
              puVar6[2] = param_3._2_1_;
              puVar6[3] = param_3._3_1_;
            }
          }
          param_2 = param_2 + 1;
          iVar5 = FUN_0051ee70(0);
        } while (param_2 < iVar5);
      }
      FUN_00588a50(local_c);
      FUN_0057dc70();
      return 1;
    }
  }
  return 0;
}


