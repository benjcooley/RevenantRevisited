// FUN_0054ae10 @ 0054ae10 size=262

void FUN_0054ae10(char *param_1,int *param_2,char *param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  
  if (DAT_0066829c != 0) {
    if ((short)param_2[1] != 0xb) goto LAB_0054aeee;
    if ((DAT_00676828 != 0) && (((byte)DAT_006767fc & 0x80) != 0)) {
      iVar6 = param_2[0x194];
      iVar2 = param_2[0x196];
      iVar3 = FUN_0049d6d0(s_STATBARDMFMT_005e57ac);
      if (iVar3 < 0) {
        pcVar4 = s__s_Lv__d_P__d_M__d_005e57bc;
      }
      else {
        pcVar4 = (char *)FUN_0049d780(iVar3);
      }
      uVar5 = (**(code **)(*param_2 + 0x354))();
      FUN_0058b100(param_1,pcVar4,param_3,uVar5,iVar6,iVar2);
      return;
    }
  }
  if ((short)param_2[1] == 0xb) {
    iVar6 = FUN_0049d6d0(s_STATBARFMT_005e57d0);
    if (iVar6 < 0) {
      pcVar4 = s__s_Level__d_005e57dc;
    }
    else {
      pcVar4 = (char *)FUN_0049d780(iVar6);
    }
    uVar5 = (**(code **)(*param_2 + 0x354))();
    FUN_0058b100(param_1,pcVar4,param_3,uVar5);
    return;
  }
LAB_0054aeee:
  uVar7 = 0xffffffff;
  do {
    pcVar4 = param_3;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar4 = param_3 + 1;
    cVar1 = *param_3;
    param_3 = pcVar4;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar4 = pcVar4 + -uVar7;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined4 *)param_1 = *(undefined4 *)pcVar4;
    pcVar4 = pcVar4 + 4;
    param_1 = param_1 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *param_1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
    param_1 = param_1 + 1;
  }
  return;
}


