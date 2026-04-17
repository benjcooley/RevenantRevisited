#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4acb80



// Function at 004acb80

int cls_0x4acb80::meth_0x4acb80(int param_1, char *param_2, int param_3, uint param_4, int param_5, uint param_6)

{
  char cVar1;
  HGDIOBJ h;
  HDC hdc;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  dword dVar7;
  LPCSTR lpString;
  char *pcVar8;
  int *piVar9;
  int iVar10;
  int *in_stack_0000001c;
  int iStack_108;
  int iStack_104;
  int iStack_100;
  tagSIZE tStack_fc;
  int iStack_f4;
  HGDIOBJ pvStack_f0;
  int iStack_ec;
  uint local_e8 [2];
  char *local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  int local_d4;
  undefined4 local_d0;
  int local_cc;
  uint local_c8;
  int local_c4;
  uint local_bc;
  int local_b8;
  uint *local_ac;
  code *local_a8;
  uint *local_a0;
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_58 [22];
  
  uVar2 = 0xffffffff;
  pcVar8 = param_2;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  iVar3 = ~uVar2 - 1;
  if ((in_stack_0000001c != (int *)0x0) &&
     (uVar2 = param_6, piVar9 = in_stack_0000001c, 0 < (int)param_6)) {
    for (; uVar2 != 0; uVar2 = uVar2 - 1) {
      *piVar9 = iVar3;
      piVar9 = piVar9 + 1;
    }
  }
  dVar7 = *(dword *)(this->mbr_0x10 + param_1 * 4);
  if (dVar7 == 0) {
    dVar7 = this->mbr_0x14;
  }
  if (*(int *)(dVar7 + 0x20) != 2) {
    if (*(int *)(dVar7 + 0x20) != 1) {
      return 0;
    }
    local_dc = *(undefined4 *)(dVar7 + 0x28);
    local_a0 = local_e8;
    local_80 = 0;
    local_84 = 0;
    local_d4 = *(int *)(dVar7 + 0x50);
    local_a8 = FUN_004b9ca0;
    if ((param_4 & 0x80) == 0) {
      local_d4 = local_d4 + *(int *)(dVar7 + 0x54);
    }
    local_e8[0] = param_4 | 0x100;
    local_d0 = *(undefined4 *)(dVar7 + 0x58);
    local_e0 = param_2;
    local_d8 = *(undefined4 *)(dVar7 + 0x5c);
    local_cc = param_5;
    local_bc = -(uint)(in_stack_0000001c != (int *)0x0) & param_6;
    local_c8 = param_6;
    local_c4 = param_3;
    local_b8 = 0;
    FUN_004b9ca0(local_58,&local_ac);
    return local_b8;
  }
  h = *(HGDIOBJ *)(dVar7 + 0x38);
  hdc = GetWindowDC((HWND)0x0);
  pvStack_f0 = SelectObject(hdc,h);
  SetTextCharacterExtra(hdc,*(int *)(dVar7 + 0x58));
  iStack_100 = 0;
  iStack_104 = 0;
  if (in_stack_0000001c != (int *)0x0) {
    *in_stack_0000001c = 0;
  }
  iVar4 = 0;
  iVar5 = param_5 + param_6;
  iStack_ec = iVar5;
  if (iVar5 < 1) {
LAB_004acdb1:
    SelectObject(hdc,pvStack_f0);
    ReleaseDC((HWND)0x0,hdc);
    return iStack_104;
  }
  iStack_108 = 0;
LAB_004acc3c:
  if (((in_stack_0000001c != (int *)0x0) && (param_5 <= iStack_100)) &&
     (iStack_100 - param_5 < (int)param_6)) {
    *(int *)(iStack_108 + param_5 * -4 + (int)in_stack_0000001c) = iVar4;
  }
  if (iVar4 < iVar3) {
    iVar10 = 0;
    iVar5 = iVar4;
    do {
      cVar1 = param_2[iVar5];
      lpString = param_2 + iVar5;
      iVar4 = iVar5;
      if (cVar1 < '\0') {
        GetTextExtentPoint32A(hdc,lpString,2,&tStack_fc);
        iVar10 = iVar10 + tStack_fc.cx;
        if (param_3 < iVar10) goto LAB_004acd81;
        iVar4 = iVar5 + 2;
      }
      else if ((cVar1 == '\n') || (cVar1 == ' ')) {
        GetTextExtentPoint32A(hdc,lpString,1,&tStack_fc);
        if (*lpString != '\n') {
          iVar10 = iVar10 + tStack_fc.cx;
        }
        if (param_3 < iVar10) goto LAB_004acd81;
        iVar4 = iVar5 + 1;
      }
      else {
        iStack_f4 = 0;
        iVar6 = iVar5;
        if (iVar10 == 0) {
          iStack_f4 = 1;
        }
        do {
          iVar4 = iVar6;
          GetTextExtentPoint32A(hdc,lpString,1,&tStack_fc);
          if (*lpString != '\n') {
            iVar10 = iVar10 + tStack_fc.cx;
          }
          iVar6 = iVar4 + 1;
          lpString = lpString + 1;
        } while (((iVar10 <= param_3) && (iVar6 < iVar3)) &&
                ((cVar1 = *lpString, cVar1 != '\n' && ((cVar1 != ' ' && (-1 < cVar1))))));
        if (iStack_f4 == 0) {
          iVar4 = iVar5;
          if (param_3 < iVar10) goto LAB_004acd81;
        }
        else if (param_3 < iVar10) goto LAB_004acd58;
        iVar4 = iVar6;
      }
LAB_004acd58:
      if (((param_3 < iVar10) || (iVar3 <= iVar4)) || (iVar5 = iVar4, param_2[iVar4] == '\n'))
      goto LAB_004acd81;
    } while( true );
  }
  goto LAB_004acd99;
LAB_004acd81:
  iStack_104 = iStack_104 + 1;
  iVar5 = iStack_ec;
LAB_004acd99:
  iStack_100 = iStack_100 + 1;
  iStack_108 = iStack_108 + 4;
  if (iVar5 <= iStack_100) goto LAB_004acdb1;
  goto LAB_004acc3c;
}



// Function at 00521c60

dword cls_0x4acb80::meth_0x521c60()

{
  dword dVar1;
  int in_stack_00000004;
  
  dVar1 = *(dword *)(this->mbr_0x10 + in_stack_00000004 * 4);
  if (dVar1 == 0) {
    dVar1 = this->mbr_0x14;
  }
  return dVar1;
}



