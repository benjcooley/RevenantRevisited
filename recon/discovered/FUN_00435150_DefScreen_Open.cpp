// FUN_00435150_DefScreen_Open @ 00435150 size=213

void __thiscall
FUN_00435150_DefScreen_Open(int param_1,undefined4 param_2,undefined4 param_3,uint param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
            undefined4 param_10,int param_11,int param_12)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  FUN_00434e40_TButtonPane_Initialize();
  *(uint *)(param_1 + 0x60) = param_4;
  if (param_11 == 0) goto LAB_004351de;
  iVar2 = FUN_0059a530_stricmp(param_11,s_widgets_005cd8e8);
  if (iVar2 != 0) {
    FUN_00435990(param_11);
    goto LAB_004351de;
  }
  if ((param_4 & 1) == 0) {
    iVar2 = *(int *)(param_1 + 0x74);
    if (DAT_0065bb10 == iVar2) {
      *(undefined4 *)(param_1 + 0x78) = 0;
      goto LAB_004351de;
    }
    iVar3 = DAT_0065bb10;
    if (iVar2 != 0) {
      iVar1 = *(int *)(param_1 + 0x78);
      goto joined_r0x004351d2;
    }
  }
  else {
    iVar2 = *(int *)(param_1 + 0x74);
    if (DAT_0066733c == iVar2) {
      *(undefined4 *)(param_1 + 0x78) = 0;
      goto LAB_004351de;
    }
    iVar3 = DAT_0066733c;
    if (iVar2 != 0) {
      iVar1 = *(int *)(param_1 + 0x78);
joined_r0x004351d2:
      if (iVar1 != 0) {
        FUN_00482f80(iVar2);
      }
    }
  }
  *(undefined4 *)(param_1 + 0x74) = 0;
  *(undefined4 *)(param_1 + 0x78) = 0;
  if (iVar3 != 0) {
    *(int *)(param_1 + 0x74) = iVar3;
    *(undefined4 *)(param_1 + 0x78) = 0;
  }
LAB_004351de:
  if (param_12 != 0) {
    FUN_00435b20(param_12);
  }
  FUN_00435040_DefScreen_LoadAndShow(param_2,param_3,param_4,param_5,param_6,param_7,param_8,param_9,param_10);
  return;
}


