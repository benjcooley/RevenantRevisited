// FUN_004cfef0 @ 004cfef0 size=735

undefined4 __thiscall FUN_004cfef0(int *param_1,int *param_2,int *param_3)

{
  char cVar1;
  int iVar2;
  undefined1 *puVar3;
  char *pcVar4;
  undefined1 *puVar5;
  undefined4 uVar6;
  undefined1 auStack_100 [256];
  
  if (param_2 == (int *)0x0) {
    return 0;
  }
  iVar2 = (**(code **)(*param_2 + 0x134))();
  if (iVar2 == 0) {
    return 0;
  }
  if (param_2 == param_1) {
    return 0;
  }
  if (param_3 == (int *)0x0) {
    param_3 = param_1;
  }
  if (DAT_0066829c != 0) {
    if (DAT_0067682c == 0) {
      cVar1 = FUN_005845c0(param_1,param_2,param_3);
      if (cVar1 != '\0') {
        return 1;
      }
    }
    else {
      FUN_00584500(param_1,param_2,param_3);
    }
  }
  if (param_1 != DAT_00667fcc) goto LAB_004d017b;
  if ((short)param_2[1] == 8) {
    FUN_0046e7f0(auStack_100,0x100,*(undefined4 *)param_2[0x13]);
    iVar2 = FUN_0049d6d0(s_FULLMONEYPICKUP_005e0164);
    if (iVar2 < 0) {
      iVar2 = FUN_0049d6d0(s_FULLMONEYPICKUPREV_005e0184);
      if (iVar2 < 0) {
        uVar6 = FUN_0049d800(s_CONTPICKUP_005e01ac);
        uVar6 = (**(code **)(*param_2 + 0x198))(auStack_100,uVar6);
        FUN_0054d170(&DAT_0065c5d0,s__i__s__s__005e01b8,uVar6);
        goto LAB_004d0174;
      }
      puVar5 = (undefined1 *)(**(code **)(*param_2 + 0x198))();
      puVar3 = auStack_100;
      pcVar4 = (char *)FUN_0049d800(s_FULLMONEYPICKUPREV_005e0198);
    }
    else {
      puVar5 = auStack_100;
      puVar3 = (undefined1 *)(**(code **)(*param_2 + 0x198))(puVar5);
      pcVar4 = (char *)FUN_0049d800(s_FULLMONEYPICKUP_005e0174);
    }
LAB_004d0167:
    FUN_0054d170(&DAT_0065c5d0,pcVar4,puVar3,puVar5);
  }
  else {
    if ((short)param_2[1] == 0x15) {
      FUN_0046e7f0(auStack_100,0x100,*(undefined4 *)param_2[0x13]);
      iVar2 = FUN_0049d6d0(s_FULLAMMOPICKUP_005e01c4);
      if (iVar2 < 0) {
        iVar2 = FUN_0049d6d0(s_FULLAMMOPICKUPREV_005e01e4);
        if (iVar2 < 0) {
          uVar6 = FUN_0049d800(s_CONTPICKUP_005e020c);
          uVar6 = (**(code **)(*param_2 + 0x198))(auStack_100,uVar6);
          FUN_0054d170(&DAT_0065c5d0,s__i__s__s__005e0218,uVar6);
          goto LAB_004d0174;
        }
        puVar5 = (undefined1 *)(**(code **)(*param_2 + 0x198))();
        puVar3 = auStack_100;
        pcVar4 = (char *)FUN_0049d800(s_FULLAMMOPICKUPREV_005e01f8);
      }
      else {
        puVar5 = auStack_100;
        puVar3 = (undefined1 *)(**(code **)(*param_2 + 0x198))(puVar5);
        pcVar4 = (char *)FUN_0049d800(s_FULLAMMOPICKUP_005e01d4);
      }
      goto LAB_004d0167;
    }
    FUN_0046e7f0(auStack_100,0x100,*(undefined4 *)param_2[0x13]);
    iVar2 = FUN_0049d6d0(s_FULLCONTPICKUP_005e0224);
    if (iVar2 < 0) {
      puVar5 = (undefined1 *)FUN_0049d800(s_CONTPICKUP_005e0244);
      puVar3 = auStack_100;
      pcVar4 = s__s__s__005e0250;
      goto LAB_004d0167;
    }
    puVar5 = auStack_100;
    uVar6 = FUN_0049d800(s_FULLCONTPICKUP_005e0234);
    FUN_0054d170(&DAT_0065c5d0,uVar6,puVar5);
  }
LAB_004d0174:
  FUN_00473a10();
LAB_004d017b:
  (**(code **)(*param_2 + 0x90))();
  iVar2 = (**(code **)(*param_3 + 0x58))(param_2,0xffffffff);
  if (iVar2 == 0) {
    uVar6 = FUN_0049d800(s_CARRYFULL2_005e0258);
    FUN_0054d170(&DAT_0065c5d0,uVar6);
  }
  return 1;
}


