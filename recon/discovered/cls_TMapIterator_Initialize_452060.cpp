// FUN_00452060_TMapPane_FindObjectsInRange @ 00452060 size=1046

int __thiscall
FUN_00452060_TMapPane_FindObjectsInRange(int param_1,int *param_2,int param_3,int param_4,int param_5,int param_6,int param_7,
            int param_8,undefined4 param_9)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int local_180;
  int *local_17c;
  int iStack_178;
  int iStack_170;
  int local_168;
  int local_164;
  int local_160;
  int local_15c;
  int local_158;
  int iStack_14c;
  int *local_13c;
  int local_100 [64];
  
  if (param_8 < 1) {
    return 0;
  }
  if (param_8 < 0x41) {
    local_17c = local_100;
  }
  else {
    local_17c = (int *)FUN_00482fb0(param_8 << 2);
  }
  local_158 = param_5 * param_5;
  local_180 = 0;
  if (param_3 < 0) {
    param_3 = *(int *)(param_1 + 0x98);
  }
  if (param_6 < 1) {
    local_168 = *param_2 - param_5;
    local_164 = param_2[1] - param_5;
    local_160 = local_168 + -1 + param_5 * 2;
    local_15c = local_164 + -1 + param_5 * 2;
  }
  else {
    local_168 = *param_2 - (param_5 >> 1);
    local_164 = param_2[1] - (param_6 >> 1);
    local_160 = local_168 + -1 + param_5;
    local_15c = local_164 + -1 + param_6;
  }
  FUN_0044cf80_TMapIterator_Initialize(&local_168,0x6e0,param_9,0,param_3);
  piVar1 = local_13c;
  do {
    if (piVar1 == (int *)0x0) {
      if (param_8 >= 0x41) {
        FUN_004830f0(local_17c);
      }
      return local_180;
    }
    local_13c = piVar1;
    iVar2 = (**(code **)(*piVar1 + 0xb4))();
    if ((iVar2 == 0) && ((param_7 == -1 || ((short)piVar1[1] == param_7)))) {
      iStack_14c = piVar1[6];
      iVar2 = piVar1[4];
      iVar8 = piVar1[5];
      if (param_6 < 1) {
        if ((param_7 == 0xc) || (param_7 == 0xb)) {
          (**(code **)(*piVar1 + 600))();
          iStack_178 = __ftol();
        }
        else {
          iStack_178 = 0;
        }
        iVar4 = *param_2;
        iVar5 = iVar4 - iVar2;
        iStack_170 = iVar5;
        if (iVar5 < 1) {
          iVar5 = iVar2 - iVar4;
          iStack_170 = iVar2 - iVar4;
        }
        iVar2 = param_2[1];
        iVar7 = iVar2 - iVar8;
        iVar4 = iVar7;
        if (iVar7 < 1) {
          iVar7 = iVar8 - iVar2;
          iVar4 = iVar8 - iVar2;
        }
        iVar2 = param_2[2];
        iVar6 = iVar2 - iStack_14c;
        iVar8 = iVar6;
        if (iVar6 < 1) {
          iVar6 = iStack_14c - iVar2;
          iVar8 = iStack_14c - iVar2;
        }
        iStack_178 = (iVar6 * iVar8 + iVar7 * iVar4 + iVar5 * iStack_170) - iStack_178;
        if (iStack_178 < local_158) {
          iVar2 = 0;
          piVar3 = local_17c;
          if (0 < local_180) {
            do {
              if (iStack_178 <= *piVar3) break;
              iVar2 = iVar2 + 1;
              piVar3 = piVar3 + 1;
            } while (iVar2 < local_180);
          }
          if (iVar2 < param_8) {
            if (iVar2 != local_180) {
              iVar8 = param_8 + -1;
              iVar4 = local_180;
              if (iVar8 < local_180) {
                iVar4 = iVar8;
              }
              FUN_0058b790(local_17c + iVar2 + 1,local_17c + iVar2,(iVar2 * 0x3fffffff + iVar4) * 4)
              ;
              iVar4 = local_180;
              if (iVar8 < local_180) {
                iVar4 = iVar8;
              }
              FUN_0058b790(param_4 + 4 + iVar2 * 4,param_4 + iVar2 * 4,
                           (iVar2 * 0x3fffffff + iVar4) * 4);
            }
            *(int *)(param_4 + iVar2 * 4) = piVar1[0x10];
            local_17c[iVar2] = iStack_178;
joined_r0x0045242e:
            if (local_180 < param_8) {
              local_180 = local_180 + 1;
            }
          }
        }
      }
      else {
        iVar4 = *param_2 - iVar2;
        if (iVar4 < 1) {
          iVar4 = iVar2 - *param_2;
        }
        iVar2 = param_2[1] - iVar8;
        if (iVar2 < 1) {
          iVar2 = iVar8 - param_2[1];
        }
        if ((iVar4 <= param_5 >> 1) && (iVar2 <= param_6 >> 1)) {
          iVar2 = iVar2 * iVar2 + iVar4 * iVar4;
          iVar8 = 0;
          piVar3 = local_17c;
          if (0 < local_180) {
            do {
              if (iVar2 <= *piVar3) break;
              iVar8 = iVar8 + 1;
              piVar3 = piVar3 + 1;
            } while (iVar8 < local_180);
          }
          if (iVar8 < param_8) {
            if (iVar8 != local_180) {
              iVar4 = param_8 + -1;
              iVar5 = local_180;
              if (iVar4 < local_180) {
                iVar5 = iVar4;
              }
              FUN_0058b790(local_17c + iVar8 + 1,local_17c + iVar8,(iVar5 + iVar8 * 0x3fffffff) * 4)
              ;
              iVar5 = local_180;
              if (iVar4 < local_180) {
                iVar5 = iVar4;
              }
              FUN_0058b790(param_4 + 4 + iVar8 * 4,param_4 + iVar8 * 4,
                           (iVar8 * 0x3fffffff + iVar5) * 4);
            }
            iVar4 = piVar1[0x10];
            local_17c[iVar8] = iVar2;
            *(int *)(param_4 + iVar8 * 4) = iVar4;
            goto joined_r0x0045242e;
          }
        }
      }
    }
    FUN_0044d080_TMapIterator_NextItem();
    piVar1 = local_13c;
  } while( true );
}


