// FUN_00499560_TSector_WalkmapHandler @ 00499560 size=432

void __thiscall
FUN_00499560_TSector_WalkmapHandler(int *param_1,int param_2,int param_3,int param_4,int param_5,int param_6,int param_7,
            int param_8,int param_9,short param_10,int param_11)

{
  int iVar1;
  ushort *puVar2;
  int iVar3;
  byte *pbVar4;
  
  if ((*param_1 != 0) && (param_3 != 0)) {
    param_5 = param_5 - ((param_1[2] << 10) >> 4);
    iVar3 = 0;
    param_6 = param_6 - ((param_1[3] << 10) >> 4);
    iVar1 = 0;
    if (param_5 < 0) {
      param_7 = param_7 + param_5;
      iVar3 = -param_5;
      param_5 = 0;
    }
    if (param_6 < 0) {
      param_8 = param_8 + param_6;
      iVar1 = -param_6;
      param_6 = 0;
    }
    if (0x40 < param_7 + param_5) {
      param_7 = 0x40 - param_5;
    }
    if (0x40 < param_8 + param_6) {
      param_8 = 0x40 - param_6;
    }
    if ((param_7 != 0) || (param_8 != 0)) {
      puVar2 = (ushort *)(*param_1 + (param_6 * 0x40 + param_5) * 2);
      pbVar4 = (byte *)(param_3 + iVar1 * param_9 + iVar3);
      param_5 = param_8;
      if (0 < param_8) {
        do {
          param_3 = param_7;
          if (0 < param_7) {
            do {
              if (param_2 == 0) {
                if (*pbVar4 != 0) {
                  iVar1 = (uint)*pbVar4 + param_4;
                  if (iVar1 < 1) {
LAB_0049966b:
                    iVar1 = 1;
                  }
                  else if (iVar1 < 0x400) {
                    if (iVar1 < 1) goto LAB_0049966b;
                  }
                  else {
                    iVar1 = 0x3ff;
                  }
                  if (((int)(*puVar2 & 0x3ff) < iVar1) || (param_11 != 0)) {
                    *puVar2 = (ushort)iVar1 & 0x3ff | param_10 << 10;
                  }
                }
              }
              else if (param_2 == 1) {
                if (*puVar2 == 0) {
                  *pbVar4 = 0;
                }
                else {
                  iVar1 = (*puVar2 & 0x3ff) - param_4;
                  if (iVar1 < 1) {
LAB_004996fb:
                    iVar1 = 1;
                  }
                  else {
                    if (0xff < iVar1) {
                      *pbVar4 = 0xff;
                      goto LAB_0049969a;
                    }
                    if (iVar1 < 1) goto LAB_004996fb;
                  }
                  *pbVar4 = (byte)iVar1;
                }
              }
              else if (param_2 == 2) {
                *puVar2 = 0;
              }
LAB_0049969a:
              puVar2 = puVar2 + 1;
              pbVar4 = pbVar4 + 1;
              param_3 = param_3 + -1;
            } while (param_3 != 0);
          }
          puVar2 = puVar2 + (0x40 - param_7);
          pbVar4 = pbVar4 + (param_9 - param_7);
          param_5 = param_5 + -1;
        } while (param_5 != 0);
      }
    }
  }
  return;
}


