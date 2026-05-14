// FUN_00492d70_TScript_Triggered @ 00492d70 size=1325

int __thiscall FUN_00492d70_TScript_Triggered(uint *param_1,uint param_2)

{
  int *piVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  char *_Source;
  int iVar6;
  uint uVar7;
  int iVar8;
  bool bVar9;
  char local_20 [31];
  undefined1 local_1;
  
  cVar2 = (char)param_1[0x2d];
  if ((cVar2 == '\x04') && (0 < (int)param_1[0x2f])) {
    param_1[0x2f] = param_1[0x2f] - 1;
  }
  iVar6 = DAT_0067682c;
  iVar8 = 0;
  if ((cVar2 == '\0') && (param_2 != 0)) {
    iVar8 = 1;
    goto LAB_0049314f;
  }
  if ((cVar2 == '\x04') && ((int)param_1[0x2f] < 1)) {
LAB_00492dba:
    iVar8 = 1;
    if (DAT_0066829c == 0) goto LAB_00493253;
    FUN_00586dd0(*(undefined4 *)(param_1[3] + 0x40),0);
  }
  else if ((cVar2 == '\x02') || ((cVar2 == '\x05' || (cVar2 == '\n')))) {
    local_20[0] = '\0';
    if ((DAT_0066829c == 0) || ((DAT_0067682c == 0 || (param_1[0x2f] == DAT_00667fcc)))) {
      if (DAT_00667eac != 0) {
        bVar9 = DAT_0066829c == 0;
        *param_1 = *param_1 & 0xfffffffb;
        if (((bVar9) || (DAT_00676828 == 0)) || (iVar6 != 0)) {
          if (DAT_00667ea4 < 0) {
            _Source = (char *)0x0;
          }
          else {
            _Source = *(char **)(&DAT_00667e80 + DAT_00667ea4 * 4);
          }
          _strncpy(local_20,_Source,0x1f);
          local_1 = 0;
        }
        else {
          FUN_00586dd0(*(undefined4 *)(param_1[3] + 0x40),DAT_00667ea4);
        }
        goto LAB_00493139;
      }
    }
    else if ((*(char *)((int)param_1 + 0xb5) == '\0') && (uVar3 = param_1[0x2e], uVar3 != 0)) {
      uVar7 = *(int *)(uVar3 + 4) - 1;
      uVar5 = (uint)*(byte *)((int)param_1 + 0xb6);
      if ((int)uVar7 <= (int)(uint)*(byte *)((int)param_1 + 0xb6)) {
        uVar5 = uVar7;
      }
      _strncpy(local_20,(char *)(uVar5 * 0x20 + 8 + uVar3),0x1f);
      local_1 = 0;
      FUN_004830f0(param_1[0x2e]);
      param_1[0x2e] = 0;
LAB_00493139:
      iVar8 = 1;
      if (local_20[0] != '\0') {
        FUN_00493fa0_TScript_Jump(param_1[3],local_20);
      }
    }
  }
  else if (cVar2 == '\x03') {
    piVar1 = (int *)param_1[0x2f];
    if (piVar1 == (int *)0x0) {
joined_r0x00492ea2:
      if (param_2 != 0) goto LAB_00492dba;
    }
    else if ((piVar1[2] & 0x20000U) == 0) {
      if (piVar1 != (int *)0x0) {
        param_2 = (**(code **)(*piVar1 + 0x154))();
        goto joined_r0x00492ea2;
      }
    }
    else {
      if (piVar1[0x36] == piVar1[0x38]) goto LAB_00492dba;
      if ((int *)piVar1[0x15] == (int *)0x0) {
        uVar3 = 0;
      }
      else {
        uVar3 = (**(code **)(*(int *)piVar1[0x15] + 0x8c))((short)piVar1[3]);
      }
      if ((uVar3 & 1) != 0) {
        param_2 = (**(code **)(*(int *)param_1[0x2f] + 0x154))();
        goto joined_r0x00492ea2;
      }
    }
  }
  else if (cVar2 == '\b') {
    piVar1 = (int *)param_1[0x2f];
    if (piVar1 == (int *)0x0) goto joined_r0x00492ea2;
    if ((piVar1[2] & 0x20000U) == 0) {
      if ((piVar1 != (int *)0x0) && (iVar6 = (**(code **)(*piVar1 + 0x154))(), iVar6 != 0)) {
        piVar1 = *(int **)(param_1[0x2f] + 0x54);
        if (piVar1 == (int *)0x0) {
          param_2 = 0;
        }
        else {
          param_2 = (**(code **)(*piVar1 + 0x8c))(*(undefined2 *)(param_1[0x2f] + 0xc));
        }
        param_2 = param_2 & 1;
        goto joined_r0x00492ea2;
      }
    }
    else if (piVar1[0x36] == piVar1[0x38]) {
      piVar1 = (int *)param_1[0x2f];
      iVar6 = (**(code **)(*piVar1 + 0x204))();
      iVar4 = (**(code **)(*piVar1 + 500))();
      if ((iVar6 == iVar4) && ((*(byte *)(*(int *)(param_1[0x2f] + 0xd8) + 0x60) & 2) == 0))
      goto LAB_00492dba;
    }
  }
  else if (cVar2 == '\t') {
    if (((int *)param_1[0x2f] != (int *)0x0) &&
       (iVar6 = (**(code **)(*(int *)param_1[0x2f] + 0x1c0))(), iVar6 < 1)) {
      iVar8 = 1;
    }
  }
  else if (cVar2 == '\x06') {
    if ((((DAT_0066829c != 0) && (param_1[0x2f] != DAT_00667fcc)) &&
        ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) || (iVar6 = FUN_0048eb00(), iVar6 == 0)) {
      iVar8 = 1;
      if (DAT_0066829c == 0) goto LAB_00493253;
      FUN_00586dd0(*(undefined4 *)(param_1[3] + 0x40),0);
    }
  }
  else if ((cVar2 == '\a') &&
          ((((DAT_0066829c != 0 && (param_1[0x2f] != DAT_00667fcc)) &&
            ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) || (DAT_0065a568 == 0)))) {
    iVar8 = 1;
    if (DAT_0066829c == 0) goto LAB_00493253;
    FUN_00586dd0(*(undefined4 *)(param_1[3] + 0x40),0);
  }
LAB_0049314f:
  if (((DAT_0066829c != 0) && (*(char *)((int)param_1 + 0xb5) != '\0')) &&
     (cVar2 = *(char *)((int)param_1 + 0xb5) + -1, *(char *)((int)param_1 + 0xb5) = cVar2,
     cVar2 == '\0')) {
    if (((param_1[0x33] == 0) || (iVar6 = FUN_0059a530_stricmp(param_1[0x33],&DAT_005da134), iVar6 != 0)) ||
       ((uVar3 = param_1[0x31], uVar3 == 0 || (*(short *)(uVar3 + 4) != 0xb)))) {
      uVar3 = DAT_00667fcc;
    }
    if (uVar3 == 0) {
      if (param_1[0x12] != 0) {
        FUN_00493e40_TScript_End();
      }
      param_1[2] = param_1[1];
    }
    else {
      if ((*(uint *)(uVar3 + 0x36c) & 0x10000) != 0) {
        FUN_00586cc0(uVar3,param_1[3],(char)param_1[0x2d],param_1[0x2f]);
        *(undefined1 *)((int)param_1 + 0xb5) = 0x78;
        goto LAB_00493253;
      }
      if (param_1[0x12] != 0) {
        FUN_00493e40_TScript_End();
      }
      param_1[2] = param_1[1];
    }
    param_1[0x12] = 0;
    param_1[0x13] = 0;
    param_1[0x14] = 0;
    param_1[4] = 0xffffffff;
    *(undefined1 *)(param_1 + 0x2d) = 0;
    *(undefined1 *)((int)param_1 + 0xb5) = 0;
    *(undefined2 *)(param_1 + 0x30) = 0;
    if (param_1[0x2e] != 0) {
      FUN_004830f0(param_1[0x2e]);
      param_1[0x2e] = 0;
    }
    param_1[0x29] = 0;
    return 0;
  }
LAB_00493253:
  iVar6 = DAT_0066829c;
  if (*(char *)((int)param_1 + 0xb5) != '\0') {
    return 0;
  }
  if (iVar8 != 0) {
    *(undefined1 *)(param_1 + 0x2d) = 0;
    *(undefined1 *)((int)param_1 + 0xb5) = 0;
    if ((iVar6 != 0) && (param_1[7] != 1)) {
      *(undefined2 *)(param_1 + 0x30) = 0xb40;
    }
  }
  return iVar8;
}


