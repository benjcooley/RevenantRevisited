// FUN_004acb80 @ 004acb80 size=806

int __thiscall
FUN_004acb80(int param_1,int param_2,char *param_3,int param_4,uint param_5,int param_6,uint param_7
            ,int *param_8)

{
  char cVar1;
  HGDIOBJ h;
  HDC hdc;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  LPCSTR lpString;
  char *pcVar7;
  int *piVar8;
  int iVar9;
  int local_108;
  int local_104;
  int local_100;
  tagSIZE local_fc;
  int local_f4;
  HGDIOBJ local_f0;
  int local_ec;
  uint local_e8 [2];
  char *local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  int local_d4;
  undefined4 local_d0;
  int local_cc;
  uint local_c8;
  int local_c4;
  int *local_c0;
  uint local_bc;
  int local_b8;
  undefined1 local_ac [4];
  code *local_a8;
  uint *local_a0;
  undefined4 local_84;
  undefined4 local_80;
  undefined1 local_58 [88];
  
  uVar2 = 0xffffffff;
  pcVar7 = param_3;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar3 = ~uVar2 - 1;
  if ((param_8 != (int *)0x0) && (uVar2 = param_7, piVar8 = param_8, 0 < (int)param_7)) {
    for (; uVar2 != 0; uVar2 = uVar2 - 1) {
      *piVar8 = iVar3;
      piVar8 = piVar8 + 1;
    }
  }
  iVar5 = *(int *)(*(int *)(param_1 + 0x10) + param_2 * 4);
  if (iVar5 == 0) {
    iVar5 = *(int *)(param_1 + 0x14);
  }
  if (*(int *)(iVar5 + 0x20) != 2) {
    if (*(int *)(iVar5 + 0x20) != 1) {
      return 0;
    }
    local_dc = *(undefined4 *)(iVar5 + 0x28);
    local_a0 = local_e8;
    local_80 = 0;
    local_84 = 0;
    local_d4 = *(int *)(iVar5 + 0x50);
    local_a8 = FUN_004b9ca0;
    if ((param_5 & 0x80) == 0) {
      local_d4 = local_d4 + *(int *)(iVar5 + 0x54);
    }
    local_e8[0] = param_5 | 0x100;
    local_d0 = *(undefined4 *)(iVar5 + 0x58);
    local_c0 = param_8;
    local_e0 = param_3;
    local_d8 = *(undefined4 *)(iVar5 + 0x5c);
    local_cc = param_6;
    local_bc = -(uint)(param_8 != (int *)0x0) & param_7;
    local_c8 = param_7;
    local_c4 = param_4;
    local_b8 = 0;
    FUN_004b9ca0(local_58,local_ac);
    return local_b8;
  }
  h = *(HGDIOBJ *)(iVar5 + 0x38);
  hdc = GetWindowDC((HWND)0x0);
  local_f0 = SelectObject(hdc,h);
  SetTextCharacterExtra(hdc,*(int *)(iVar5 + 0x58));
  local_100 = 0;
  local_104 = 0;
  if (param_8 != (int *)0x0) {
    *param_8 = 0;
  }
  iVar4 = 0;
  iVar5 = param_6 + param_7;
  local_ec = iVar5;
  if (iVar5 < 1) {
LAB_004acdb1:
    SelectObject(hdc,local_f0);
    ReleaseDC((HWND)0x0,hdc);
    return local_104;
  }
  local_108 = 0;
LAB_004acc3c:
  if (((param_8 != (int *)0x0) && (param_6 <= local_100)) && (local_100 - param_6 < (int)param_7)) {
    *(int *)(local_108 + param_6 * -4 + (int)param_8) = iVar4;
  }
  if (iVar4 < iVar3) {
    iVar9 = 0;
    iVar5 = iVar4;
    do {
      cVar1 = param_3[iVar5];
      lpString = param_3 + iVar5;
      iVar4 = iVar5;
      if (cVar1 < '\0') {
        GetTextExtentPoint32A(hdc,lpString,2,&local_fc);
        iVar9 = iVar9 + local_fc.cx;
        if (param_4 < iVar9) goto LAB_004acd81;
        iVar4 = iVar5 + 2;
      }
      else if ((cVar1 == '\n') || (cVar1 == ' ')) {
        GetTextExtentPoint32A(hdc,lpString,1,&local_fc);
        if (*lpString != '\n') {
          iVar9 = iVar9 + local_fc.cx;
        }
        if (param_4 < iVar9) goto LAB_004acd81;
        iVar4 = iVar5 + 1;
      }
      else {
        local_f4 = 0;
        iVar6 = iVar5;
        if (iVar9 == 0) {
          local_f4 = 1;
        }
        do {
          iVar4 = iVar6;
          GetTextExtentPoint32A(hdc,lpString,1,&local_fc);
          if (*lpString != '\n') {
            iVar9 = iVar9 + local_fc.cx;
          }
          iVar6 = iVar4 + 1;
          lpString = lpString + 1;
        } while (((iVar9 <= param_4) && (iVar6 < iVar3)) &&
                ((cVar1 = *lpString, cVar1 != '\n' && ((cVar1 != ' ' && (-1 < cVar1))))));
        if (local_f4 == 0) {
          iVar4 = iVar5;
          if (param_4 < iVar9) goto LAB_004acd81;
        }
        else if (param_4 < iVar9) goto LAB_004acd58;
        iVar4 = iVar6;
      }
LAB_004acd58:
      if (((param_4 < iVar9) || (iVar3 <= iVar4)) || (iVar5 = iVar4, param_3[iVar4] == '\n'))
      goto LAB_004acd81;
    } while( true );
  }
  goto LAB_004acd99;
LAB_004acd81:
  local_104 = local_104 + 1;
  iVar5 = local_ec;
LAB_004acd99:
  local_100 = local_100 + 1;
  local_108 = local_108 + 4;
  if (iVar5 <= local_100) goto LAB_004acdb1;
  goto LAB_004acc3c;
}


