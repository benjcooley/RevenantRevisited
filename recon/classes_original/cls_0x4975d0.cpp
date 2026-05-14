// Decompiled methods and structure for class: cls_0x4975d0

/*
/OOAnalyzer/cls_0x4975d0
pack(disabled)
Structure cls_0x4975d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16388   dword   4   mbr_0x4004   "Unsigned Double-Word (ddw, 4-bytes)"
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32792 Alignment: 1

*/

// Function at 004975d0

undefined4 __thiscall OOAnalyzer::cls_0x4975d0::meth_0x4975d0_TGameState_FindStateValue(cls_0x4975d0 *this)

{
  undefined **ppuVar1;
  int iVar2;
  undefined **ppuVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = 0;
  if (PTR_DAT_005da0b0 != (undefined *)0x0) {
    ppuVar3 = &PTR_DAT_005da0b0;
    do {
      iVar2 = FUN_0059a530_stricmp();
      if (iVar2 == 0) {
        if (-1 < iVar5) {
          return (&DAT_005da0b4)[iVar5 * 2];
        }
        break;
      }
      ppuVar1 = ppuVar3 + 2;
      ppuVar3 = ppuVar3 + 2;
      iVar5 = iVar5 + 1;
    } while (*ppuVar1 != (undefined *)0x0);
  }
  uVar4 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      iVar5 = FUN_0059a530_stricmp();
      if (iVar5 == 0) goto LAB_00497634;
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)this->mbr_0x0);
  }
  uVar4 = 0xffffffff;
LAB_00497634:
  if ((uVar4 < this->mbr_0x0) && (-1 < (int)uVar4)) {
    return *(undefined4 *)(&this->field_0x4 + uVar4 * 4);
  }
  return 0xfeced300;
}



// Function at 00497700

void __thiscall
OOAnalyzer::cls_0x4975d0::meth_0x497700(cls_0x4975d0 *this,undefined4 param_1,undefined4 param_2)

{
  char *pcVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int in_stack_0000000c;
  
  uVar5 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((((this->mbr_0x8014 != 0) && (uVar5 < this->mbr_0x8004)) &&
           (*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0)) && (in_stack_0000000c != 0)) &&
         ((((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,*(undefined4 *)(in_stack_0000000c + 0x38)), iVar3 == 0)) ||
          (((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,**(undefined4 **)(in_stack_0000000c + 0x4c)), iVar3 == 0))))
         )) {
        iVar6 = 0;
        iVar3 = *(int *)(this->mbr_0x8014 + uVar5 * 4);
        if (0 < *(int *)(iVar3 + 0x24)) {
          do {
            iVar4 = FUN_0059a530_stricmp(*(int *)(*(int *)(iVar3 + 0x34) + iVar6 * 4) + 8,param_1);
            if (iVar4 == 0) {
              piVar2 = *(int **)(*(int *)(iVar3 + 0x34) + iVar6 * 4);
              if (*piVar2 == 0) {
                *(undefined4 *)piVar2[1] = param_2;
              }
              break;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 < *(int *)(iVar3 + 0x24));
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)this->mbr_0x8004);
  }
  return;
}



// Function at 00497800

int __thiscall OOAnalyzer::cls_0x4975d0::meth_0x497800(cls_0x4975d0 *this,undefined4 param_1)

{
  char *pcVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int in_stack_00000008;
  
  uVar5 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((((this->mbr_0x8014 != 0) && (uVar5 < this->mbr_0x8004)) &&
           (*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0)) && (in_stack_00000008 != 0)) &&
         ((((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,*(undefined4 *)(in_stack_00000008 + 0x38)), iVar3 == 0)) ||
          (((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,**(undefined4 **)(in_stack_00000008 + 0x4c)), iVar3 == 0))))
         )) {
        iVar6 = 0;
        iVar3 = *(int *)(this->mbr_0x8014 + uVar5 * 4);
        if (0 < *(int *)(iVar3 + 0x24)) {
          do {
            iVar4 = FUN_0059a530_stricmp(*(int *)(*(int *)(iVar3 + 0x34) + iVar6 * 4) + 8,param_1);
            if (iVar4 == 0) {
              piVar2 = *(int **)(*(int *)(iVar3 + 0x34) + iVar6 * 4);
              if ((*piVar2 == 0) && (iVar3 = *(int *)piVar2[1], iVar3 != -20000000)) {
                return iVar3;
              }
              break;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 < *(int *)(iVar3 + 0x24));
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)this->mbr_0x8004);
  }
  return -20000000;
}



// Function at 00497910

void __thiscall
OOAnalyzer::cls_0x4975d0::meth_0x497910(cls_0x4975d0 *this,undefined4 param_1,char *param_2)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  int iVar9;
  char *pcVar10;
  char *pcVar11;
  undefined4 *puVar12;
  int in_stack_0000000c;
  
  uVar7 = 0;
  if ((int)this->mbr_0x8004 < 1) {
    return;
  }
LAB_00497926:
  if (((((this->mbr_0x8014 != 0) && (uVar7 < this->mbr_0x8004)) &&
       (*(int *)(this->mbr_0x8014 + uVar7 * 4) != 0)) && (in_stack_0000000c != 0)) &&
     ((((pcVar10 = **(char ***)(this->mbr_0x8014 + uVar7 * 4), pcVar10 != (char *)0x0 &&
        (*pcVar10 != '\0')) &&
       (iVar3 = FUN_0059a530_stricmp(pcVar10,*(undefined4 *)(in_stack_0000000c + 0x38)), iVar3 == 0)) ||
      (((pcVar10 = **(char ***)(this->mbr_0x8014 + uVar7 * 4), pcVar10 != (char *)0x0 &&
        (*pcVar10 != '\0')) &&
       (iVar3 = FUN_0059a530_stricmp(pcVar10,**(undefined4 **)(in_stack_0000000c + 0x4c)), iVar3 == 0))))))
  {
    iVar9 = 0;
    iVar3 = *(int *)(this->mbr_0x8014 + uVar7 * 4);
    if (0 < *(int *)(iVar3 + 0x24)) {
      do {
        iVar4 = FUN_0059a530_stricmp(*(int *)(*(int *)(iVar3 + 0x34) + iVar9 * 4) + 8,param_1);
        if (iVar4 == 0) {
          piVar2 = *(int **)(*(int *)(iVar3 + 0x34) + iVar9 * 4);
          if (*piVar2 == 1) {
            uVar5 = 0xffffffff;
            pcVar10 = param_2;
            goto code_r0x004979f7;
          }
          break;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < *(int *)(iVar3 + 0x24));
    }
  }
  goto LAB_00497a10;
  while( true ) {
    uVar5 = uVar5 - 1;
    pcVar11 = pcVar10 + 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar11;
    if (cVar1 == '\0') break;
code_r0x004979f7:
    pcVar11 = pcVar10;
    if (uVar5 == 0) break;
  }
  uVar5 = ~uVar5;
  puVar8 = (undefined4 *)(pcVar11 + -uVar5);
  puVar12 = (undefined4 *)piVar2[1];
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *puVar12 = *puVar8;
    puVar8 = puVar8 + 1;
    puVar12 = puVar12 + 1;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *(undefined *)puVar12 = *(undefined *)puVar8;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
    puVar12 = (undefined4 *)((int)puVar12 + 1);
  }
LAB_00497a10:
  uVar7 = uVar7 + 1;
  if ((int)this->mbr_0x8004 <= (int)uVar7) {
    return;
  }
  goto LAB_00497926;
}



// Function at 00497a30

int __thiscall OOAnalyzer::cls_0x4975d0::meth_0x497a30(cls_0x4975d0 *this,undefined4 param_1)

{
  char *pcVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int in_stack_00000008;
  
  uVar5 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((((this->mbr_0x8014 != 0) && (uVar5 < this->mbr_0x8004)) &&
           (*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0)) && (in_stack_00000008 != 0)) &&
         ((((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,*(undefined4 *)(in_stack_00000008 + 0x38)), iVar3 == 0)) ||
          (((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar5 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar3 = FUN_0059a530_stricmp(pcVar1,**(undefined4 **)(in_stack_00000008 + 0x4c)), iVar3 == 0))))
         )) {
        iVar6 = 0;
        iVar3 = *(int *)(this->mbr_0x8014 + uVar5 * 4);
        if (0 < *(int *)(iVar3 + 0x24)) {
          do {
            iVar4 = FUN_0059a530_stricmp(*(int *)(*(int *)(iVar3 + 0x34) + iVar6 * 4) + 8,param_1);
            if (iVar4 == 0) {
              piVar2 = *(int **)(*(int *)(iVar3 + 0x34) + iVar6 * 4);
              if ((*piVar2 == 1) && (iVar3 = piVar2[1], iVar3 != 0)) {
                return iVar3;
              }
              break;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 < *(int *)(iVar3 + 0x24));
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)this->mbr_0x8004);
  }
  return 0;
}



// Function at 00497b40

int __thiscall OOAnalyzer::cls_0x4975d0::meth_0x497b40(cls_0x4975d0 *this,undefined4 param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int in_stack_00000008;
  
  uVar4 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((((this->mbr_0x8014 != 0) && (uVar4 < this->mbr_0x8004)) &&
           (*(int *)(this->mbr_0x8014 + uVar4 * 4) != 0)) && (in_stack_00000008 != 0)) &&
         ((((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar4 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar2 = FUN_0059a530_stricmp(pcVar1,*(undefined4 *)(in_stack_00000008 + 0x38)), iVar2 == 0)) ||
          (((pcVar1 = **(char ***)(this->mbr_0x8014 + uVar4 * 4), pcVar1 != (char *)0x0 &&
            (*pcVar1 != '\0')) &&
           (iVar2 = FUN_0059a530_stricmp(pcVar1,**(undefined4 **)(in_stack_00000008 + 0x4c)), iVar2 == 0))))
         )) {
        iVar5 = 0;
        iVar2 = *(int *)(this->mbr_0x8014 + uVar4 * 4);
        if (0 < *(int *)(iVar2 + 0x24)) {
          do {
            iVar3 = FUN_0059a530_stricmp(*(int *)(*(int *)(iVar2 + 0x34) + iVar5 * 4) + 8,param_1);
            if (iVar3 == 0) {
              iVar2 = **(int **)(*(int *)(iVar2 + 0x34) + iVar5 * 4);
              if (iVar2 != -1) {
                return iVar2;
              }
              break;
            }
            iVar5 = iVar5 + 1;
          } while (iVar5 < *(int *)(iVar2 + 0x24));
        }
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)this->mbr_0x8004);
  }
  return -1;
}



