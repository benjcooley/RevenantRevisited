// FUN_004c18a0_TCharacter_ClearChar @ 004c18a0 size=577

void __fastcall FUN_004c18a0_TCharacter_ClearChar(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  
  param_1[0x99] = 0;
  param_1[0x9a] = 0;
  param_1[0x9b] = 0x7fffffff;
  param_1[2] = param_1[2] | 0xc028;
  FUN_00471b60(param_1[0xf] | 0x13);
  iVar3 = DAT_00667fd0;
  param_1[0x3a] = DAT_005d7a68;
  iVar3 = *(int *)(iVar3 + 0x48);
  param_1[0x42] = 0;
  param_1[0x43] = 0;
  param_1[0x46] = 0;
  param_1[100] = 0;
  param_1[0x45] = iVar3;
  FUN_004d4220(0xffffffff);
  iVar1 = FUN_0048c780((int)*(short *)((int)param_1 + 6),(int)(short)param_1[1]);
  iVar3 = *param_1;
  param_1[0x3f] = iVar1;
  param_1[0x3b] = 0;
  param_1[0x3e] = 0;
  param_1[0x3d] = 0;
  param_1[0x3c] = 0;
  param_1[0x6e] = 0;
  param_1[0x6f] = 0;
  param_1[0x62] = 0;
  param_1[0x89] = 0;
  param_1[0xa3] = -1;
  param_1[0xa4] = 0;
  uVar2 = (**(code **)(iVar3 + 0x1d8))();
  (**(code **)(iVar3 + 0x1c4))(uVar2);
  iVar3 = *param_1;
  uVar2 = (**(code **)(iVar3 + 0x1e0))();
  (**(code **)(iVar3 + 0x1cc))(uVar2);
  iVar3 = *param_1;
  uVar2 = (**(code **)(iVar3 + 0x1e8))();
  (**(code **)(iVar3 + 0x1d4))(uVar2);
  param_1[0x48] = 1;
  param_1[0x49] = 1;
  param_1[0x5b] = 0;
  param_1[0x47] = -1;
  param_1[0x4d] = -1;
  param_1[0x4c] = -1;
  param_1[0x88] = -1;
  param_1[0x8b] = 0;
  piVar4 = param_1 + 0x70;
  for (iVar3 = 0x18; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar4 = 0;
    piVar4 = piVar4 + 1;
  }
  param_1[0x66] = 5;
  param_1[0x65] = 100;
  param_1[0x67] = 100;
  param_1[0x68] = 0;
  param_1[0x69] = 0;
  FUN_00540940();
  (**(code **)(*param_1 + 0x1b4))(0);
  param_1[0x6b] = -1;
  param_1[0x6c] = -1;
  param_1[0x6d] = -1;
  param_1[0x6a] = -1;
  param_1[0x8d] = 0;
  param_1[0x8e] = 0;
  param_1[0x96] = 0;
  param_1[0x95] = 0;
  param_1[0x97] = 0;
  iVar3 = (**(code **)(*param_1 + 0x314))();
  param_1[0x4e] = iVar3;
  iVar3 = (**(code **)(*param_1 + 0x314))();
  param_1[0x4f] = iVar3;
  iVar3 = (**(code **)(*param_1 + 0x314))();
  param_1[0x50] = iVar3;
  iVar3 = (**(code **)(*param_1 + 0x314))();
  param_1[0x51] = iVar3;
  param_1[0x52] = 0;
  param_1[0x53] = 0;
  param_1[0x54] = 0;
  param_1[0x55] = 0;
  FUN_004d63b0();
  param_1[0xa0] = 0;
  param_1[0xa1] = 0;
  param_1[0xa2] = 0;
  param_1[0xa7] = 0;
  param_1[0xa6] = 0;
  param_1[0xa5] = 0;
  return;
}


