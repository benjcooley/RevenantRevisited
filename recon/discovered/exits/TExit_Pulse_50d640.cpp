// TExit_Pulse @ 0x0050d640 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x110; disassembly of the key parts appended
// FUN_0050d640 @ 0050d640 size=827

void __fastcall FUN_0050d640(int *param_1)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  byte *pbVar9;
  char *pcVar10;
  byte *pbVar11;
  byte *pbVar12;
  bool bVar13;
  int iStack_1c;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  undefined1 auStack_8 [4];
  undefined1 auStack_4 [4];
  
  FUN_004708e0();
  if (DAT_00668154 == 0) {
    iVar4 = (**(code **)(*param_1 + 0x154))();
    if ((((iVar4 != 0) && (iVar4 = (**(code **)(*param_1 + 0x1f0))(), iVar4 != 0)) &&
        (iVar4 = (**(code **)(*param_1 + 0x24))(), iVar4 != 0)) &&
       (iVar4 = (**(code **)(*param_1 + 0x24))(), *(int *)(iVar4 + 8) != 0)) {
      iVar4 = (**(code **)(*param_1 + 0x24))();
      iVar4 = (**(code **)(**(int **)(iVar4 + 8) + 0x90))((short)param_1[3]);
      if (1 < iVar4) {
        iVar4 = (**(code **)(*param_1 + 0x24))();
        uVar5 = (**(code **)(**(int **)(iVar4 + 8) + 0x88))((short)param_1[3]);
        iVar4 = FUN_0059a530(uVar5,s_CLOSING_005e1860);
        if (iVar4 == 0) {
          FUN_0050d530(3);
        }
        iVar4 = FUN_0059a530(uVar5,s_OPENING_005e1868);
        if (iVar4 == 0) {
          FUN_0050d530(1);
        }
      }
    }
    if (((DAT_00668154 == 0) && (iVar4 = FUN_0046e8a0(), iVar4 != 0)) &&
       ((DAT_0067682c != 0 || (DAT_0066829c == 0)))) {
      (**(code **)(*param_1 + 0x27c))
                (&iStack_18,&iStack_14,auStack_4,&iStack_10,&iStack_c,auStack_8);
      param_1[0x39] = param_1[0x39] & 0xfffffffa;
      iStack_1c = 0;
      iVar4 = FUN_0051ee70(0);
      if (0 < iVar4) {
        do {
          iVar4 = FUN_0051eea0(iStack_1c,0);
          if (iVar4 != 0) {
            iVar6 = (iStack_18 * 0x10 - param_1[4]) + *(int *)(iVar4 + 0x10);
            iVar7 = iStack_14 * 0x10 + (*(int *)(iVar4 + 0x14) - param_1[5]);
            iVar6 = (int)(iVar6 + (iVar6 >> 0x1f & 0xfU)) >> 4;
            iVar7 = (int)(iVar7 + (iVar7 >> 0x1f & 0xfU)) >> 4;
            piVar8 = (int *)FUN_0046e8a0();
            (**(code **)(*piVar8 + 0xc4))((short)param_1[3]);
            if (((-1 < iVar6) && (-1 < iVar7)) && ((iVar6 < iStack_10 && (iVar7 < iStack_c)))) {
              pbVar9 = &DAT_005e1870;
              pbVar12 = *(byte **)param_1[0x13];
              pbVar11 = pbVar12;
              do {
                bVar1 = *pbVar9;
                bVar13 = bVar1 < *pbVar11;
                if (bVar1 != *pbVar11) {
LAB_0050d84f:
                  iVar6 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                  goto LAB_0050d854;
                }
                if (bVar1 == 0) break;
                bVar1 = pbVar9[1];
                bVar13 = bVar1 < pbVar11[1];
                if (bVar1 != pbVar11[1]) goto LAB_0050d84f;
                pbVar9 = pbVar9 + 2;
                pbVar11 = pbVar11 + 2;
              } while (bVar1 != 0);
              iVar6 = 0;
LAB_0050d854:
              if (iVar6 != 0) {
                pbVar9 = &DAT_005e1878;
                pbVar11 = pbVar12;
                do {
                  bVar1 = *pbVar9;
                  bVar13 = bVar1 < *pbVar11;
                  if (bVar1 != *pbVar11) {
LAB_0050d887:
                    iVar6 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                    goto LAB_0050d88c;
                  }
                  if (bVar1 == 0) break;
                  bVar1 = pbVar9[1];
                  bVar13 = bVar1 < pbVar11[1];
                  if (bVar1 != pbVar11[1]) goto LAB_0050d887;
                  pbVar9 = pbVar9 + 2;
                  pbVar11 = pbVar11 + 2;
                } while (bVar1 != 0);
                iVar6 = 0;
LAB_0050d88c:
                if (iVar6 != 0) {
                  pcVar10 = s_PortEW_005e1880;
                  pbVar11 = pbVar12;
                  do {
                    bVar1 = *pcVar10;
                    bVar13 = bVar1 < *pbVar11;
                    if (bVar1 != *pbVar11) {
LAB_0050d8bf:
                      iVar6 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                      goto LAB_0050d8c4;
                    }
                    if (bVar1 == 0) break;
                    bVar1 = pcVar10[1];
                    bVar13 = bVar1 < pbVar11[1];
                    if (bVar1 != pbVar11[1]) goto LAB_0050d8bf;
                    pcVar10 = pcVar10 + 2;
                    pbVar11 = pbVar11 + 2;
                  } while (bVar1 != 0);
                  iVar6 = 0;
LAB_0050d8c4:
                  if (iVar6 != 0) {
                    pbVar11 = &DAT_005e1888;
                    do {
                      bVar1 = *pbVar11;
                      bVar13 = bVar1 < *pbVar12;
                      if (bVar1 != *pbVar12) {
LAB_0050d8f3:
                        iVar6 = (1 - (uint)bVar13) - (uint)(bVar13 != 0);
                        goto LAB_0050d8f8;
                      }
                      if (bVar1 == 0) break;
                      bVar1 = pbVar11[1];
                      bVar13 = bVar1 < pbVar12[1];
                      if (bVar1 != pbVar12[1]) goto LAB_0050d8f3;
                      pbVar11 = pbVar11 + 2;
                      pbVar12 = pbVar12 + 2;
                    } while (bVar1 != 0);
                    iVar6 = 0;
LAB_0050d8f8:
                    if (iVar6 != 0) {
                      uVar2 = param_1[0x39];
                      uVar3 = *(uint *)(iVar4 + 8);
                      param_1[0x39] = uVar2 | 1;
                      if ((uVar3 & 0x100000) == 0) {
                        (**(code **)(*param_1 + 0x248))(iVar4,0);
                      }
                      else {
                        param_1[0x39] = uVar2 | 5;
                      }
                      FUN_004cdf30();
                      *(int **)(iVar4 + 0xe4) = param_1;
                    }
                  }
                }
              }
            }
          }
          iStack_1c = iStack_1c + 1;
          iVar4 = FUN_0051ee70(0);
        } while (iStack_1c < iVar4);
      }
    }
  }
  iVar4 = (**(code **)(*param_1 + 0x24))();
  if (iVar4 == 0) {
    (**(code **)(*param_1 + 0x158))(1);
  }
  return;
}



// ---- Disassembly of the state step, the strip scan set-up and the per-player body ----
// ---- 0050d640
// 0050d640  SUB ESP,0x1c
// 0050d643  PUSH EBP
// 0050d644  PUSH ESI
// 0050d645  MOV EBP,ECX
// 0050d647  CALL 0x004708e0
// 0050d64c  MOV EAX,[0x00668154]
// 0050d651  TEST EAX,EAX
// 0050d653  JNZ 0x0050d95c
// 0050d659  MOV EAX,dword ptr [EBP]
// 0050d65c  MOV ECX,EBP
// 0050d65e  CALL dword ptr [EAX + 0x154]
// 0050d664  TEST EAX,EAX
// 0050d666  JZ 0x0050d70f
// 0050d66c  MOV EDX,dword ptr [EBP]
// 0050d66f  MOV ECX,EBP
// 0050d671  CALL dword ptr [EDX + 0x1f0]
// 0050d677  TEST EAX,EAX
// 0050d679  JZ 0x0050d70f
// 0050d67f  MOV EAX,dword ptr [EBP]
// 0050d682  MOV ECX,EBP
// 0050d684  CALL dword ptr [EAX + 0x24]
// 0050d687  TEST EAX,EAX
// 0050d689  JZ 0x0050d70f
// 0050d68f  MOV EDX,dword ptr [EBP]
// 0050d692  MOV ECX,EBP
// 0050d694  CALL dword ptr [EDX + 0x24]
// 0050d697  MOV ECX,dword ptr [EAX + 0x8]
// 0050d69a  TEST ECX,ECX
// 0050d69c  JZ 0x0050d70f
// 0050d69e  MOV EAX,dword ptr [EBP]
// 0050d6a1  MOV ECX,EBP
// 0050d6a3  CALL dword ptr [EAX + 0x24]
// 0050d6a6  MOV ECX,dword ptr [EAX + 0x8]
// 0050d6a9  XOR EAX,EAX
// 0050d6ab  MOV AX,word ptr [EBP + 0xc]
// 0050d6af  MOV EDX,dword ptr [ECX]
// 0050d6b1  PUSH EAX
// 0050d6b2  CALL dword ptr [EDX + 0x90]
// 0050d6b8  CMP EAX,0x1
// 0050d6bb  JLE 0x0050d70f
// 0050d6bd  MOV EDX,dword ptr [EBP]
// 0050d6c0  MOV ECX,EBP
// 0050d6c2  CALL dword ptr [EDX + 0x24]
// 0050d6c5  MOV ECX,dword ptr [EAX + 0x8]
// 0050d6c8  XOR EDX,EDX
// 0050d6ca  MOV DX,word ptr [EBP + 0xc]
// 0050d6ce  MOV EAX,dword ptr [ECX]
// 0050d6d0  PUSH EDX
// 0050d6d1  CALL dword ptr [EAX + 0x88]
// 0050d6d7  MOV ESI,EAX
// 0050d6d9  PUSH 0x5e1860
// 0050d6de  PUSH ESI
// 0050d6df  CALL 0x0059a530
// 0050d6e4  ADD ESP,0x8
// 0050d6e7  TEST EAX,EAX
// 0050d6e9  JNZ 0x0050d6f4
// 0050d6eb  PUSH 0x3
// 0050d6ed  MOV ECX,EBP
// 0050d6ef  CALL 0x0050d530
// 0050d6f4  PUSH 0x5e1868
// 0050d6f9  PUSH ESI
// 0050d6fa  CALL 0x0059a530
// 0050d6ff  ADD ESP,0x8
// 0050d702  TEST EAX,EAX
// 0050d704  JNZ 0x0050d70f
// 0050d706  PUSH 0x1
// 0050d708  MOV ECX,EBP
// 0050d70a  CALL 0x0050d530
// 0050d70f  MOV EAX,[0x00668154]
// 0050d714  TEST EAX,EAX
// 0050d716  JNZ 0x0050d95c
// 0050d71c  MOV ECX,EBP
// 0050d71e  CALL 0x0046e8a0
// 0050d723  TEST EAX,EAX
// 0050d725  JZ 0x0050d95c
// 0050d72b  MOV EAX,[0x0067682c]
// 0050d730  TEST EAX,EAX
// 0050d732  JNZ 0x0050d741
// 0050d734  MOV EAX,[0x0066829c]
// 0050d739  TEST EAX,EAX
// 0050d73b  JNZ 0x0050d95c
// 0050d741  MOV EAX,dword ptr [EBP]
// 0050d744  LEA ECX,[ESP + 0x1c]
// 0050d748  LEA EDX,[ESP + 0x18]
// 0050d74c  PUSH ECX
// 0050d74d  PUSH EDX
// 0050d74e  LEA ECX,[ESP + 0x1c]
// 0050d752  LEA EDX,[ESP + 0x28]
// 0050d756  PUSH ECX
// 0050d757  PUSH EDX
// 0050d758  LEA ECX,[ESP + 0x20]
// 0050d75c  LEA EDX,[ESP + 0x1c]
// 0050d760  PUSH ECX
// 0050d761  PUSH EDX
// 0050d762  MOV ECX,EBP
// 0050d764  CALL dword ptr [EAX + 0x27c]
// 0050d76a  MOV ESI,dword ptr [EBP + 0xe4]
// 0050d770  MOV ECX,0x65a890
// 0050d775  AND ESI,0xfffffffa
// 0050d778  MOV dword ptr [EBP + 0xe4],ESI
// 0050d77e  XOR ESI,ESI
// ---- 0050d8f8
// 0050d8f8  TEST EAX,EAX
// 0050d8fa  JZ 0x0050d93d
// 0050d8fc  MOV EDX,dword ptr [EBP + 0xe4]
// 0050d902  MOV ECX,dword ptr [EBX + 0x8]
// 0050d905  OR EDX,0x1
// 0050d908  TEST ECX,0x100000
// 0050d90e  MOV dword ptr [EBP + 0xe4],EDX
// 0050d914  MOV EAX,EDX
// 0050d916  JNZ 0x0050d928
// 0050d918  MOV EAX,dword ptr [EBP]
// 0050d91b  PUSH 0x0
// 0050d91d  PUSH EBX
// 0050d91e  MOV ECX,EBP
// 0050d920  CALL dword ptr [EAX + 0x248]
// 0050d926  JMP 0x0050d930
// 0050d928  OR AL,0x4
// 0050d92a  MOV dword ptr [EBP + 0xe4],EAX
// 0050d930  MOV ECX,EBX
// 0050d932  CALL 0x004cdf30
// 0050d937  MOV dword ptr [EBX + 0xe4],EBP
// 0050d93d  MOV ESI,dword ptr [ESP + 0x10]
// 0050d941  PUSH 0x0
// 0050d943  INC ESI
// 0050d944  MOV ECX,0x65a890
// 0050d949  MOV dword ptr [ESP + 0x14],ESI
// 0050d94d  CALL 0x0051ee70
// 0050d952  CMP ESI,EAX
// 0050d954  JL 0x0050d794
// 0050d95a  POP EDI
// 0050d95b  POP EBX
// 0050d95c  MOV EDX,dword ptr [EBP]
// 0050d95f  MOV ECX,EBP
// 0050d961  CALL dword ptr [EDX + 0x24]
// 0050d964  TEST EAX,EAX
// 0050d966  JNZ 0x0050d975
// 0050d968  MOV EAX,dword ptr [EBP]
// 0050d96b  PUSH 0x1
// 0050d96d  MOV ECX,EBP
// 0050d96f  CALL dword ptr [EAX + 0x158]
// 0050d975  POP ESI
// 0050d976  POP EBP
