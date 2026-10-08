// FUN_00425770 @ 00425770 size=449

undefined4 FUN_00425770(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  bool bVar8;
  
  DAT_00654a88._0_1_ = 0;
  iVar2 = FUN_00479700(s_classes_005cbe04,0);
  if (iVar2 == 0) {
    uVar5 = FUN_004751c0(*(undefined4 *)(param_2 + 0x28));
    if ((uVar5 < DAT_0065a258) && (iVar2 = (&DAT_0065a148)[uVar5], iVar2 != 0)) {
      FUN_00479580();
      if (*(int *)(param_2 + 0x10) != 4) {
        FUN_00425460(iVar2,0);
        FUN_00479580();
        return 0;
      }
      iVar3 = FUN_00475210(*(undefined4 *)(param_2 + 0x28),0);
      if (-1 < iVar3) {
        FUN_004255e0(iVar2,iVar3);
        FUN_00479580();
        return 0;
      }
      iVar3 = FUN_00425460(iVar2,*(undefined4 *)(param_2 + 0x28));
      if (iVar3 == 0) {
        FUN_0058b100(&DAT_00654a88,s__s_contains_no_type_named___s___005cbe34,
                     *(undefined4 *)(iVar2 + 4),*(undefined4 *)(param_2 + 0x28));
        FUN_0041ee50(&DAT_00654a88);
        FUN_00479580();
        return 0;
      }
    }
    else {
      FUN_0041ee50(s_No_class_by_that_name__005cbe58);
    }
  }
  else {
    FUN_0041ee50(s_Object_Classes__005cbe0c);
    uVar5 = 0;
    iVar2 = 0;
    if (0 < (int)DAT_0065a258) {
      piVar4 = &DAT_0065a148;
      bVar8 = DAT_0065a258 != 0;
      do {
        if ((bVar8) && (*piVar4 != 0)) {
          FUN_0058b100(&DAT_00654a88,s__s___19s_005cbe20,&DAT_00654a88,*(undefined4 *)(*piVar4 + 4))
          ;
          iVar2 = iVar2 + 1;
          if (2 < iVar2) {
            iVar2 = -1;
            pcVar6 = (char *)&DAT_00654a88;
            do {
              pcVar7 = pcVar6;
              if (iVar2 == 0) break;
              iVar2 = iVar2 + -1;
              pcVar7 = pcVar6 + 1;
              cVar1 = *pcVar6;
              pcVar6 = pcVar7;
            } while (cVar1 != '\0');
            *(undefined2 *)(pcVar7 + -1) = DAT_005cbe2c;
            FUN_0041ee50(&DAT_00654a88);
            DAT_00654a88._0_1_ = 0;
            iVar2 = 0;
          }
        }
        uVar5 = uVar5 + 1;
        piVar4 = piVar4 + 1;
        bVar8 = uVar5 < DAT_0065a258;
      } while ((int)uVar5 < (int)DAT_0065a258);
      if (iVar2 != 0) {
        iVar2 = -1;
        pcVar6 = (char *)&DAT_00654a88;
        do {
          pcVar7 = pcVar6;
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          pcVar7 = pcVar6 + 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar7;
        } while (cVar1 != '\0');
        *(undefined2 *)(pcVar7 + -1) = DAT_005cbe30;
        FUN_0041ee50(&DAT_00654a88);
        FUN_00479580();
        return 0;
      }
    }
  }
  FUN_00479580();
  return 0;
}


