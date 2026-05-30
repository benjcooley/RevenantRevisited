// FUN_004aacb0_BlitEffect_Iterate @ 004aacb0 size=275

void FUN_004aacb0_BlitEffect_Iterate(int param_1,int param_2,int param_3,int param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int local_10;
  int *local_c;
  
  if (DAT_005e91bc == 0) {
    local_10 = 0;
    local_c = &DAT_00669af8;
    do {
      if (local_c[7] != 0) {
        iVar2 = local_c[-1];
        iVar4 = local_c[5] + iVar2;
        if (param_1 < iVar4) {
          iVar3 = *local_c;
          iVar1 = local_c[6] + iVar3;
          if (((param_2 < iVar1) && (iVar2 < param_1 + param_3)) && (iVar3 < param_4 + param_2)) {
            iVar6 = param_3;
            iVar8 = param_1;
            if (param_1 < iVar2) {
              iVar6 = (param_1 - iVar2) + param_3;
              iVar8 = iVar2;
            }
            iVar5 = param_4;
            iVar7 = param_2;
            if (param_2 < iVar3) {
              iVar5 = (param_2 - iVar3) + param_4;
              iVar7 = iVar3;
            }
            if (iVar4 < iVar6 + iVar8) {
              iVar6 = iVar4 - iVar8;
            }
            if (iVar1 < iVar5 + iVar7) {
              iVar5 = iVar1 - iVar7;
            }
            FUN_004aaeb0_BlitEffect_Apply(local_10,(local_c[1] - iVar2) + iVar8,(local_c[2] - *local_c) + iVar7,iVar6
                         ,iVar5,param_5);
          }
        }
      }
      local_c = local_c + 0x13;
      local_10 = local_10 + 1;
    } while (local_10 < 10);
  }
  return;
}


