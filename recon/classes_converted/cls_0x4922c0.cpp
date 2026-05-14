#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4922c0



// Function at 004922c0

void cls_0x4922c0::meth_0x4922c0()

{
  if (((DAT_0066829c != 0) && (this->mbr_0xb4 != 0)) && (this->mbr_0xc != 0)) {
    cls_0x57d9d0::meth_0x586dd0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40));
  }
  if (-1 < (int)this->mbr_0x14) {
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_00665f08);
  }
  if (this->mbr_0xc != 0) {
    *(undefined4 *)(this->mbr_0xc + 0x84) = 0;
  }
  if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xb8);
  }
  if ((LPCVOID)this->mbr_0xe0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xe0);
  }
  this->mbr_0xe0 = 0;
  if ((LPCVOID)this->mbr_0xd8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xd8);
  }
  this->mbr_0xd8 = 0;
  if ((LPCVOID)this->mbr_0xdc != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xdc);
  }
  this->mbr_0xdc = 0;
  this->mbr_0xc0 = 0;
  return;
}



// Function at 004923c0

void cls_0x4922c0::meth_0x4923c0()

{
  char *in_stack_00000004;
  
  if (this->mbr_0x48 != 0) {
    meth_0x493e40_TScript_End(this);
  }
  this->mbr_0x48 = 0;
  this->mbr_0x8 = this->mbr_0x4;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0xb4 = 0;
  this->mbr_0xb5 = 0;
  this->mbr_0xc0 = 0;
  if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  FUN_00494ab0((char **)this->mbr_0x4,in_stack_00000004,-1);
  UNK_00665f1c._20_4_ = 1;
  return;
}



// Function at 00492440

void cls_0x4922c0::meth_0x492440(dword param_1)

{
  int iVar1;
  int in_stack_00000008;
  
  if (this->mbr_0x48 != 0) {
    meth_0x493e40_TScript_End(this);
  }
  if (param_1 == 0) {
    param_1 = this->mbr_0x4;
  }
  this->mbr_0x8 = param_1;
  if ((-1 < in_stack_00000008) && (in_stack_00000008 < *(int *)(param_1 + 0x40))) {
    iVar1 = *(int *)(param_1 + 4);
    this->mbr_0xa4 = 0;
    this->mbr_0xc0 = 0;
    this->mbr_0x48 = iVar1 + in_stack_00000008;
  }
  return;
}



// Function at 00492490

void cls_0x4922c0::meth_0x492490()

{
  if (this->mbr_0x48 != 0) {
    meth_0x493e40_TScript_End(this);
  }
  this->mbr_0x48 = 0;
  this->mbr_0x8 = this->mbr_0x4;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0xb4 = 0;
  this->mbr_0xb5 = 0;
  this->mbr_0xc0 = 0;
  if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  return;
}



// Function at 004924f0

void cls_0x4922c0::meth_0x4924f0()

{
  if (this->mbr_0x48 != 0) {
    meth_0x493e40_TScript_End(this);
  }
  this->mbr_0x48 = 0;
  this->mbr_0x8 = this->mbr_0x4;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0xb4 = 0;
  this->mbr_0xb5 = 0;
  this->mbr_0xc0 = 0;
  if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  return;
}



// Function at 004927b0

undefined4 cls_0x4922c0::meth_0x4927b0(undefined4 *param_1, dword *param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  uint extraout_ECX;
  int in_stack_0000000c;
  dword *pdVar7;
  undefined4 local_4;
  
  local_4 = 0;
  if (((undefined4 *)this->mbr_0xa8 == param_1) || ((int)param_1[0xe] < (int)param_2)) {
    return 0;
  }
  switch(*param_1) {
  case 1:
    break;
  case 2:
    if (this->mbr_0x18 != 2) {
      return 0;
    }
    param_2 = &this->mbr_0x20;
    goto LAB_00492801;
  case 3:
    if (this->mbr_0x18 != 3) {
      return 0;
    }
    break;
  case 4:
    if (this->mbr_0x18 != 4) {
      if (DAT_00667fcc == 0) {
        return 0;
      }
      uVar1 = FUN_0059a530_stricmp((uint)param_2,(byte *)(param_1 + 2),*(byte **)(DAT_00667fcc + 0x38));
      if (uVar1 != 0) {
        return 0;
      }
      iVar5 = param_1[0xd];
      iVar6 = *(int *)(in_stack_0000000c + 0x14) - *(int *)(DAT_00667fcc + 0x14);
      iVar2 = iVar6;
      if (iVar6 < 1) {
        iVar2 = *(int *)(DAT_00667fcc + 0x14) - *(int *)(in_stack_0000000c + 0x14);
      }
      if (iVar5 < iVar2) {
        return 0;
      }
      iVar3 = *(int *)(in_stack_0000000c + 0x10) - *(int *)(DAT_00667fcc + 0x10);
      iVar2 = iVar3;
      if (iVar3 < 1) {
        iVar2 = *(int *)(DAT_00667fcc + 0x10) - *(int *)(in_stack_0000000c + 0x10);
      }
      if (iVar5 < iVar2) {
        return 0;
      }
      if (iVar5 * iVar5 * 2 < iVar3 * iVar3 + iVar6 * iVar6) {
        return 0;
      }
      this->mbr_0xc4 = DAT_00667fcc;
      this->mbr_0xcc = (dword)&DAT_005da114;
    }
    break;
  case 5:
    if (this->mbr_0x18 != 5) {
      if (DAT_00667fcc != 0) {
        uVar1 = FUN_0059a530_stricmp((uint)param_2,(byte *)(param_1 + 2),(byte *)s_player_005da11c);
        if ((uVar1 == 0) ||
           (uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)(param_1 + 2),*(byte **)(DAT_00667fcc + 0x38))
           , uVar1 == 0)) {
          if (*(int *)(DAT_00667fcc + 0x10) < (int)param_1[7]) {
            return 0;
          }
          if (*(int *)(DAT_00667fcc + 0x14) < (int)param_1[8]) {
            return 0;
          }
          if (*(int *)(DAT_00667fcc + 0x18) < (int)param_1[9]) {
            return 0;
          }
          if ((int)param_1[10] < *(int *)(DAT_00667fcc + 0x10)) {
            return 0;
          }
          if ((int)param_1[0xb] < *(int *)(DAT_00667fcc + 0x14)) {
            return 0;
          }
          if ((int)param_1[0xc] < *(int *)(DAT_00667fcc + 0x18)) {
            return 0;
          }
          this->mbr_0xc4 = DAT_00667fcc;
          this->mbr_0xcc = (dword)&DAT_005da124;
          break;
        }
      }
      dVar4 = FUN_00452480(param_1 + 7,(uint)*(ushort *)(in_stack_0000000c + 0xe),1);
      if (dVar4 == 0) {
        return 0;
      }
      if ((*(short *)(dVar4 + 4) != 0xc) && (*(short *)(dVar4 + 4) != 0xb)) {
        return 0;
      }
      if ((*(char *)(param_1 + 2) != '\0') &&
         (uVar1 = FUN_0059a530_stricmp(*(uint *)(dVar4 + 0x38),(byte *)(param_1 + 2),
                               (byte *)*(uint *)(dVar4 + 0x38)), uVar1 == 0)) {
        return 0;
      }
      this->mbr_0xc4 = dVar4;
      this->mbr_0xcc = (dword)&DAT_005da12c;
    }
    break;
  case 6:
    if (this->mbr_0x18 != 6) {
      return 0;
    }
    break;
  case 7:
    if (this->mbr_0x18 != 7) {
      return 0;
    }
    uVar1 = FUN_0059a530_stricmp((uint)param_2,(byte *)(param_1 + 2),(byte *)&this->mbr_0x20);
    if (uVar1 == 0) break;
    param_2 = &this->mbr_0x34;
    pdVar7 = param_2;
    goto LAB_00492804;
  case 8:
    if (this->mbr_0x18 != 8) {
      return 0;
    }
    goto LAB_00492801;
  case 9:
    if (this->mbr_0x18 != 9) {
      return 0;
    }
LAB_00492801:
    pdVar7 = &this->mbr_0x20;
LAB_00492804:
    uVar1 = FUN_0059a530_stricmp((uint)param_2,(byte *)(param_1 + 2),(byte *)pdVar7);
LAB_0049280f:
    if (uVar1 != 0) {
      return 0;
    }
    break;
  case 10:
    uVar1 = this->mbr_0x18 - 10;
    goto LAB_0049280f;
  case 0xb:
    if (this->mbr_0x18 != 0xb) {
      return 0;
    }
    break;
  default:
    goto switchD_004927ec_caseD_b;
  }
  local_4 = 1;
  if (this->mbr_0x10 != 0xffffffff) {
    iVar5 = FUN_00452690_TMapPane_GetInstance(this->mbr_0x10,0);
    if (iVar5 != 0) {
      return 0;
    }
    this->mbr_0x10 = 0xffffffff;
  }
switchD_004927ec_caseD_b:
  return local_4;
}



// Function at 00492b00

void cls_0x4922c0::meth_0x492b00(uint param_1)

{
  uint uVar1;
  byte bVar2;
  dword dVar3;
  dword in_stack_00000008;
  
  if (this->mbr_0xb4 != 0) {
    return;
  }
  if (((((byte *)this->mbr_0xcc == (byte *)0x0) ||
       (uVar1 = FUN_0059a530_stricmp((uint)this,(byte *)this->mbr_0xcc,&DAT_005da134), uVar1 != 0)) ||
      (dVar3 = this->mbr_0xc4, dVar3 == 0)) || (*(short *)(dVar3 + 4) != 0xb)) {
    dVar3 = DAT_00667fcc;
  }
  if ((((param_1 == 2) || (param_1 == 5)) || (param_1 == 10)) && (dVar3 == DAT_00667fcc)) {
    if (DAT_00667e58 != 0) {
      return;
    }
    DAT_00667eb0._0_4_ = (uint)(param_1 == 10);
    DAT_00667e5c._0_4_ = dVar3;
    cls_0x535e90::meth_0x535e90((cls_0x535e90 *)&DAT_00667cc8);
  }
  bVar2 = (byte)param_1;
  this->mbr_0xb4 = bVar2;
  if (((bVar2 == 6) || (bVar2 == 7)) || ((bVar2 == 2 || ((bVar2 == 5 || (bVar2 == 10)))))) {
    this->mbr_0xbc = dVar3;
  }
  else {
    this->mbr_0xbc = in_stack_00000008;
  }
  if (((((((bVar2 != 3) && (bVar2 != 8)) && (bVar2 != 9)) && ((bVar2 != 6 && (bVar2 != 7)))) &&
       ((bVar2 != 2 && ((bVar2 != 5 && (bVar2 != 10)))))) || (this->mbr_0xbc != 0)) &&
     ((((DAT_0066829c != 0 && (DAT_0067682c != 0)) && (dVar3 != 0)) && (dVar3 != DAT_00667fcc)))) {
    cls_0x57d9d0::meth_0x586cc0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,dVar3,this->mbr_0xc,param_1 & 0xff);
    this->mbr_0xb5 = 0x78;
  }
  return;
}



// Function at 00492d70

int cls_0x4922c0::meth_0x492d70_TScript_Triggered()

{
  int *piVar1;
  dword dVar2;
  byte bVar3;
  cls_0x4922c0 *pcVar4;
  cls_0x4922c0 *pcVar5;
  int iVar6;
  char *_Source;
  uint uVar7;
  cls_0x4922c0 *extraout_ECX;
  cls_0x4922c0 *extraout_ECX_00;
  cls_0x4922c0 *extraout_ECX_01;
  cls_0x4922c0 *extraout_ECX_02;
  cls_0x4922c0 *extraout_ECX_03;
  cls_0x4922c0 *extraout_ECX_04;
  cls_0x4922c0 *extraout_ECX_05;
  cls_0x4922c0 *extraout_ECX_06;
  cls_0x4922c0 *extraout_ECX_07;
  cls_0x4922c0 *extraout_ECX_08;
  uint uVar8;
  cls_0x4922c0 *extraout_ECX_09;
  cls_0x4922c0 *extraout_ECX_10;
  cls_0x4922c0 *extraout_ECX_11;
  cls_0x4922c0 *extraout_ECX_12;
  int iVar9;
  bool bVar10;
  int in_stack_00000004;
  char local_20 [31];
  undefined local_1;
  
  bVar3 = this->mbr_0xb4;
  pcVar4 = this;
  if ((bVar3 == 4) && (pcVar4 = (cls_0x4922c0 *)this->mbr_0xbc, 0 < (int)pcVar4)) {
    pcVar4 = (cls_0x4922c0 *)((int)&pcVar4[-1].mbr_0xe4 + 3);
    this->mbr_0xbc = (dword)pcVar4;
  }
  iVar6 = DAT_0067682c;
  iVar9 = 0;
  if ((bVar3 == 0) && (in_stack_00000004 != 0)) {
    iVar9 = 1;
    goto LAB_0049314f;
  }
  if ((bVar3 == 4) && ((int)this->mbr_0xbc < 1)) {
LAB_00492dba:
    iVar9 = 1;
    if (DAT_0066829c == (cls_0x4922c0 *)0x0) goto LAB_00493253;
    cls_0x57d9d0::meth_0x586dd0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40));
    pcVar4 = extraout_ECX;
  }
  else if ((bVar3 == 2) || ((bVar3 == 5 || (bVar3 == 10)))) {
    local_20[0] = '\0';
    pcVar4 = DAT_0066829c;
    if ((DAT_0066829c == (cls_0x4922c0 *)0x0) ||
       ((DAT_0067682c == 0 || ((cls_0x4922c0 *)this->mbr_0xbc == DAT_00667fcc)))) {
      if (DAT_00667eac != 0) {
        bVar10 = DAT_0066829c == (cls_0x4922c0 *)0x0;
        this->mbr_0x0 = this->mbr_0x0 & 0xfffffffb;
        if (((bVar10) || (DAT_00676828 == 0)) || (iVar6 != 0)) {
          if (DAT_00667ea4 < 0) {
            _Source = (char *)0x0;
          }
          else {
            _Source = *(char **)(&DAT_00667e5c.field_0x24 + DAT_00667ea4 * 4);
          }
          _strncpy(local_20,_Source,0x1f);
          local_1 = 0;
          pcVar4 = extraout_ECX_11;
        }
        else {
          cls_0x57d9d0::meth_0x586dd0
                    ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,
                     (char)*(undefined4 *)(this->mbr_0xc + 0x40));
          pcVar4 = extraout_ECX_10;
        }
        goto LAB_00493139;
      }
    }
    else if ((this->mbr_0xb5 == 0) && (dVar2 = this->mbr_0xb8, dVar2 != 0)) {
      uVar8 = *(int *)(dVar2 + 4) - 1;
      uVar7 = (uint)this->mbr_0xb6;
      if ((int)uVar8 <= (int)(uint)this->mbr_0xb6) {
        uVar7 = uVar8;
      }
      _strncpy(local_20,(char *)(uVar7 * 0x20 + 8 + dVar2),0x1f);
      local_1 = 0;
      FUN_004830f0((LPCVOID)this->mbr_0xb8);
      this->mbr_0xb8 = 0;
      pcVar4 = extraout_ECX_09;
LAB_00493139:
      iVar9 = 1;
      if (local_20[0] != '\0') {
        meth_0x493fa0_TScript_Jump(this,this->mbr_0xc);
        pcVar4 = extraout_ECX_12;
      }
    }
  }
  else if (bVar3 == 3) {
    piVar1 = (int *)this->mbr_0xbc;
    if (piVar1 == (int *)0x0) {
joined_r0x00492ea2:
      if (in_stack_00000004 != 0) goto LAB_00492dba;
    }
    else if ((piVar1[2] & 0x20000U) == 0) {
      if (piVar1 != (int *)0x0) {
        uVar7 = (**(code **)(*piVar1 + 0x154))();
        pcVar4 = extraout_ECX_02;
joined_r0x00492f4e:
        if (uVar7 != 0) goto LAB_00492dba;
      }
    }
    else {
      if (piVar1[0x36] == piVar1[0x38]) goto LAB_00492dba;
      if ((int *)piVar1[0x15] == (int *)0x0) {
        uVar7 = 0;
        pcVar4 = (cls_0x4922c0 *)0x0;
      }
      else {
        uVar7 = (**(code **)(*(int *)piVar1[0x15] + 0x8c))(*(undefined2 *)(piVar1 + 3));
        pcVar4 = extraout_ECX_00;
      }
      if ((uVar7 & 1) != 0) {
        uVar7 = (**(code **)(*(int *)this->mbr_0xbc + 0x154))();
        pcVar4 = extraout_ECX_01;
        goto joined_r0x00492f4e;
      }
    }
  }
  else if (bVar3 == 8) {
    pcVar4 = (cls_0x4922c0 *)this->mbr_0xbc;
    if (pcVar4 == (cls_0x4922c0 *)0x0) goto joined_r0x00492ea2;
    if ((pcVar4->mbr_0x8 & 0x20000) == 0) {
      if ((pcVar4 != (cls_0x4922c0 *)0x0) &&
         (iVar6 = (**(code **)(pcVar4->mbr_0x0 + 0x154))(), pcVar4 = extraout_ECX_03, iVar6 != 0)) {
        piVar1 = *(int **)(this->mbr_0xbc + 0x54);
        if (piVar1 == (int *)0x0) {
          uVar7 = 0;
          pcVar4 = (cls_0x4922c0 *)0x0;
        }
        else {
          uVar7 = (**(code **)(*piVar1 + 0x8c))(*(undefined2 *)(this->mbr_0xbc + 0xc));
          pcVar4 = extraout_ECX_04;
        }
        uVar7 = uVar7 & 1;
        goto joined_r0x00492f4e;
      }
    }
    else if (pcVar4->mbr_0xd8 == pcVar4->mbr_0xe0) {
      piVar1 = (int *)this->mbr_0xbc;
      pcVar4 = (cls_0x4922c0 *)(**(code **)(*piVar1 + 0x204))();
      pcVar5 = (cls_0x4922c0 *)(**(code **)(*piVar1 + 500))();
      if ((pcVar4 == pcVar5) && ((*(byte *)(*(int *)(this->mbr_0xbc + 0xd8) + 0x60) & 2) == 0))
      goto LAB_00492dba;
    }
  }
  else if (bVar3 == 9) {
    pcVar4 = (cls_0x4922c0 *)this->mbr_0xbc;
    if ((pcVar4 != (cls_0x4922c0 *)0x0) &&
       (iVar6 = (**(code **)(pcVar4->mbr_0x0 + 0x1c0))(), pcVar4 = extraout_ECX_05, iVar6 < 1)) {
      iVar9 = 1;
      pcVar4 = extraout_ECX_05;
    }
  }
  else if (bVar3 == 6) {
    if ((((DAT_0066829c != (cls_0x4922c0 *)0x0) && ((cls_0x4922c0 *)this->mbr_0xbc != DAT_00667fcc))
        && ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) ||
       (iVar6 = TCharacter::meth_0x48eb00((TCharacter *)&DAT_0065caf0), pcVar4 = extraout_ECX_06
       , iVar6 == 0)) {
      iVar9 = 1;
      if (DAT_0066829c == (cls_0x4922c0 *)0x0) goto LAB_00493253;
      cls_0x57d9d0::meth_0x586dd0
                ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40)
                );
      pcVar4 = extraout_ECX_07;
    }
  }
  else if ((bVar3 == 7) &&
          ((((DAT_0066829c != (cls_0x4922c0 *)0x0 &&
             (pcVar4 = (cls_0x4922c0 *)this->mbr_0xbc, pcVar4 != DAT_00667fcc)) &&
            ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) || (DAT_0065a568 == 0)))) {
    iVar9 = 1;
    if (DAT_0066829c == (cls_0x4922c0 *)0x0) goto LAB_00493253;
    cls_0x57d9d0::meth_0x586dd0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40));
    pcVar4 = extraout_ECX_08;
  }
LAB_0049314f:
  if (((DAT_0066829c != (cls_0x4922c0 *)0x0) && (this->mbr_0xb5 != 0)) &&
     (bVar3 = this->mbr_0xb5 - 1, this->mbr_0xb5 = bVar3, bVar3 == 0)) {
    if ((((byte *)this->mbr_0xcc == (byte *)0x0) ||
        (uVar7 = FUN_0059a530_stricmp((uint)pcVar4,(byte *)this->mbr_0xcc,&DAT_005da134), uVar7 != 0)) ||
       ((pcVar4 = (cls_0x4922c0 *)this->mbr_0xc4, pcVar4 == (cls_0x4922c0 *)0x0 ||
        (*(short *)&pcVar4->mbr_0x4 != 0xb)))) {
      pcVar4 = DAT_00667fcc;
    }
    if (pcVar4 == (cls_0x4922c0 *)0x0) {
      if (this->mbr_0x48 != 0) {
        meth_0x493e40_TScript_End(this);
      }
      this->mbr_0x8 = this->mbr_0x4;
    }
    else {
      uVar7._0_1_ = pcVar4[3].mbr_0xb4;
      uVar7._1_1_ = pcVar4[3].mbr_0xb5;
      uVar7._2_1_ = pcVar4[3].mbr_0xb6;
      uVar7._3_1_ = pcVar4[3].field_0xb7;
      if ((uVar7 & 0x10000) != 0) {
        cls_0x57d9d0::meth_0x586cc0
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,pcVar4,this->mbr_0xc,(uint)this->mbr_0xb4
                  );
        this->mbr_0xb5 = 0x78;
        goto LAB_00493253;
      }
      if (this->mbr_0x48 != 0) {
        meth_0x493e40_TScript_End(this);
      }
      this->mbr_0x8 = this->mbr_0x4;
    }
    this->mbr_0x48 = 0;
    this->mbr_0x4c = 0;
    this->mbr_0x50 = 0;
    this->mbr_0x10 = 0xffffffff;
    this->mbr_0xb4 = 0;
    this->mbr_0xb5 = 0;
    this->mbr_0xc0 = 0;
    if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
      FUN_004830f0((LPCVOID)this->mbr_0xb8);
      this->mbr_0xb8 = 0;
    }
    this->mbr_0xa4 = 0;
    return 0;
  }
LAB_00493253:
  pcVar4 = DAT_0066829c;
  if (this->mbr_0xb5 == 0) {
    if (iVar9 != 0) {
      this->mbr_0xb4 = 0;
      this->mbr_0xb5 = 0;
      if ((pcVar4 != (cls_0x4922c0 *)0x0) && (this->mbr_0x1c != 1)) {
        this->mbr_0xc0 = 0xb40;
      }
    }
    return iVar9;
  }
  return 0;
}



// Function at 004933d0

undefined4 cls_0x4922c0::meth_0x4933d0_TScript_Continue()

{
  dword dVar1;
  word wVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  uint uVar7;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  dword dVar8;
  int *extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar9;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  dword *pdVar10;
  char *pcVar11;
  bool bVar12;
  int in_stack_00000004;
  char *pcVar13;
  dword local_32c;
  undefined4 local_328;
  LPCVOID local_324;
  FILE **local_320;
  cls_0x5a36f8__vftable_5a36f8 **local_31c;
  int local_318;
  undefined4 local_314;
  int local_310;
  undefined4 local_308;
  undefined4 local_304;
  undefined4 *local_300;
  undefined local_2fc;
  undefined4 local_2f8;
  cls_0x478720 local_2f0;
  int local_2e8;
  int local_2e4;
  int local_2e0;
  cls_0x5a36f8__vftable_5a36f8 *local_2dc;
  char *local_2d8;
  int local_2d4;
  int local_2d0;
  char *local_2cc;
  byte abStack_2c8 [100];
  byte abStack_264 [100];
  byte abStack_200 [100];
  byte abStack_19c [100];
  byte abStack_138 [100];
  byte abStack_d4 [100];
  byte abStack_70 [100];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d957;
  local_c = ExceptionList;
  local_32c = 0;
  local_2e0 = -1;
  local_2e4 = 0;
  local_2e8 = 0;
  if ((((this->mbr_0x4c & 0x10000) == 0) && (DAT_0066856c == 0)) &&
     ((this->mbr_0xc == 0 || ((*(uint *)(this->mbr_0xc + 8) & 0x200000) == 0)))) {
    ExceptionList = &local_c;
    if (this->mbr_0xc0 != 0) {
      wVar2 = this->mbr_0xc0 - 1;
      bVar12 = DAT_0066829c != 0;
      ExceptionList = &local_c;
      this->mbr_0xc0 = wVar2;
      if ((bVar12) && (wVar2 == 0)) {
        meth_0x492490(this);
      }
    }
    if (((in_stack_00000004 == 0) || (this->mbr_0xb4 != 0)) &&
       (iVar3 = meth_0x492d70_TScript_Triggered(this), iVar3 == 0)) {
      ExceptionList = local_c;
      return 0;
    }
    iVar3 = DAT_0066829c;
    this->mbr_0xb4 = 0;
    this->mbr_0xb5 = 0;
    if (((iVar3 != 0) && (DAT_00676828 != 0)) && (DAT_0067682c == 0)) {
      ExceptionList = local_c;
      return 0;
    }
    if (((uint **)this->mbr_0xe4 != (uint **)0x0) && ((byte *)this->mbr_0xd8 != (byte *)0x0)) {
      FUN_004d0950((uint **)this->mbr_0xe4,(byte *)this->mbr_0xd8,0xffffffff,0,this->mbr_0xdc);
      if (this->mbr_0xe4 != 0) {
        meth_0x492b00(this,8);
      }
      this->mbr_0xe4 = 0;
      ExceptionList = local_c;
      return 0;
    }
    local_2d4 = *(int *)(this->mbr_0x8 + 4);
    local_2d0 = *(int *)(this->mbr_0x8 + 0x40) + local_2d4;
    local_2dc = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
    local_2d8 = s_String_005da144;
    local_2cc = (char *)local_2d4;
    cls_0x478720::cls_0x478720(&local_2f0);
    local_31c = &local_2dc;
    local_4 = 0;
    local_328 = 0;
    local_324 = (LPCVOID)0x0;
    local_320 = (FILE **)0x0;
    local_318 = 0;
    local_314 = 0;
    local_310 = 0;
    local_308 = 0;
    local_304 = 0;
    local_2fc = 0;
    local_2f8 = 1;
    local_300 = FUN_00482fb0(0x2000);
    *(undefined *)local_300 = 0;
    local_4 = 1;
    dVar8 = this->mbr_0x4;
    if (dVar8 != 0) {
      do {
        iVar3 = 0;
        if (0 < *(int *)(dVar8 + 0x44)) {
          do {
            iVar4 = *(int *)(*(int *)(dVar8 + 0x1c) + iVar3 * 4);
            if (iVar4 == 0) {
              iVar4 = *(int *)(dVar8 + 0x20);
            }
            iVar5 = meth_0x4927b0(this,iVar4,this->mbr_0x4c);
            dVar1 = local_32c;
            iVar4 = local_2e0;
            if (iVar5 != 0) {
              piVar6 = *(int **)(*(int *)(dVar8 + 0x1c) + iVar3 * 4);
              if (piVar6 == (int *)0x0) {
                piVar6 = *(int **)(dVar8 + 0x20);
              }
              if (((*piVar6 != 1) || (dVar1 = dVar8, iVar4 = iVar3, local_32c != 0)) &&
                 ((this->mbr_0x48 == 0 ||
                  ((this->mbr_0x1c == 1 ||
                   (dVar1 = local_32c, iVar4 = local_2e0, this->mbr_0xac != 0)))))) {
                if ((this->mbr_0x48 != 0) && (this->mbr_0x1c == 1)) {
                  this->mbr_0xac = this->mbr_0x48;
                  this->mbr_0xb0 = this->mbr_0xa4;
                }
                pdVar10 = *(dword **)(*(int *)(dVar8 + 0x1c) + iVar3 * 4);
                if (pdVar10 == (dword *)0x0) {
                  pdVar10 = *(dword **)(dVar8 + 0x20);
                }
                meth_0x492440(this,dVar8);
                this->mbr_0x1c = *pdVar10;
                this->mbr_0x18 = 0;
                *(undefined *)&this->mbr_0x20 = 0;
                local_2e8 = 1;
                this->mbr_0xa4 = 0;
                if ((int *)this->mbr_0xc != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc + 0x148))();
                }
                if ((int *)this->mbr_0xc4 != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc4 + 0x148))();
                }
                piVar6 = (int *)this->mbr_0xc8;
                if (piVar6 != (int *)0x0) {
                  (**(code **)(*piVar6 + 0x148))();
                  piVar6 = extraout_ECX;
                }
                if (this->mbr_0x1c == 1) {
                  this->mbr_0x10 = 0xffffffff;
                }
                else if (((this->mbr_0xc4 != 0) && ((byte *)this->mbr_0xcc != (byte *)0x0)) &&
                        (uVar7 = FUN_0059a530_stricmp((uint)piVar6,(byte *)this->mbr_0xcc,&DAT_005da14c),
                        uVar7 == 0)) {
                  this->mbr_0x10 = *(dword *)(this->mbr_0xc4 + 0x40);
                }
                if (this->mbr_0x1c != 1) {
                  this->mbr_0xc0 = 0xb40;
                }
                goto LAB_004937b4;
              }
            }
            local_2e0 = iVar4;
            local_32c = dVar1;
            iVar3 = iVar3 + 1;
          } while (iVar3 < *(int *)(dVar8 + 0x44));
        }
        dVar8 = *(dword *)(dVar8 + 8);
      } while (dVar8 != 0);
      if (local_32c != 0) {
        if (this->mbr_0x48 != 0) goto LAB_00493827;
        pdVar10 = *(dword **)(*(int *)(local_32c + 0x1c) + local_2e0 * 4);
        if (pdVar10 == (dword *)0x0) {
          pdVar10 = *(dword **)(local_32c + 0x20);
        }
        meth_0x492440(this,local_32c);
        this->mbr_0x1c = *pdVar10;
        this->mbr_0x18 = 0;
        *(undefined *)&this->mbr_0x20 = 0;
        if ((int *)this->mbr_0xc != (int *)0x0) {
          (**(code **)(*(int *)this->mbr_0xc + 0x148))();
        }
        if ((int *)this->mbr_0xc4 != (int *)0x0) {
          (**(code **)(*(int *)this->mbr_0xc4 + 0x148))();
        }
        if ((int *)this->mbr_0xc8 != (int *)0x0) {
          (**(code **)(*(int *)this->mbr_0xc8 + 0x148))();
        }
        local_2e8 = 1;
        if (this->mbr_0xac == 0) {
          this->mbr_0xa4 = 0;
          this->mbr_0xac = this->mbr_0x48;
        }
        else {
          this->mbr_0x48 = this->mbr_0xac;
          this->mbr_0xa4 = this->mbr_0xb0;
          this->mbr_0xac = 0;
          this->mbr_0xb0 = 0;
        }
      }
    }
LAB_004937b4:
    if (this->mbr_0x48 != 0) {
LAB_00493827:
      (**(code **)(*(int *)this->mbr_0xc + 0x144))();
      pcVar13 = (char *)this->mbr_0x48;
      local_32c = 6000;
      pcVar11 = pcVar13;
      uVar9 = extraout_ECX_00;
      do {
        local_2cc = pcVar11;
        if ((pcVar13 == (char *)0x0) || ((this->mbr_0x4c & 0x10000) != 0)) goto LAB_00493d81;
        if ((pcVar13 < pcVar11) &&
           ((uVar7 = FUN_0058af6f(uVar9,(int)*pcVar11), uVar7 == 0 &&
            (uVar7 = FUN_0058af6f((int)pcVar11[-1],(int)pcVar11[-1]), uVar7 == 0)))) {
          pcVar11 = pcVar11 + -1;
        }
        FUN_00478a10((byte *)&local_328);
        FUN_00479680((byte *)&local_328);
        if (local_2e4 != 0) {
          (*(*local_31c)->virt_meth_0x4113d0_16)(local_31c);
          FUN_00479580((byte *)&local_32c);
        }
        local_2e4 = 0;
        if (local_318 == 7) {
          if (local_310 != 0x3a) goto LAB_004938e7;
          FUN_004795c0((byte *)&local_328);
        }
        else if (((local_318 == 4) || (local_318 == 3)) || (local_318 == 2)) {
          uVar7 = FUN_0041e8e0((int *)this->mbr_0xc,(byte *)&local_328,0,(uint)this);
          if ((uVar7 & 0x20) != 0) {
            local_4 = 3;
            FUN_004830f0(local_300);
            if (local_324 != (LPCVOID)0x0) {
              FUN_004830f0(local_31c);
              FUN_004830f0(local_324);
              goto LAB_00493e1c;
            }
            if (local_320 == (FILE **)0x0) goto LAB_00493e1c;
            FUN_004830f0(local_31c);
            goto LAB_00493e14;
          }
          if ((uVar7 & 0x40) == 0) {
            if ((uVar7 & 0x80) != 0) {
              FUN_004795a0((byte *)&local_328);
              bVar12 = FUN_00479700((uint)&local_328,(byte *)s_BEGIN_005da154,0);
              if (CONCAT31(extraout_var,bVar12) == 0) {
                FUN_004795c0((byte *)&local_328);
              }
              else {
                iVar3 = FUN_004795f0((byte *)&local_328);
                if (iVar3 == 0) {
                  FUN_0058b100((char *)abStack_d4,(byte *)s_Script_error_at_line__d___s_005da0d0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_d4);
                  }
                  else {
                    FUN_0041ee50(abStack_d4);
                  }
                }
              }
              *(undefined4 *)(&this->field_0x58 + this->mbr_0xa4 * 8) = 0;
            }
          }
          else {
            *(undefined4 *)(&this->field_0x58 + this->mbr_0xa4 * 8) = 1;
          }
          if ((uVar7 & 0x100) != 0) {
            if (*(int *)(&this->field_0x58 + this->mbr_0xa4 * 8) == 0xdeaf) {
              FUN_0058b100((char *)abStack_19c,(byte *)s_Script_error_at_line__d___s_005da0d0);
              if (DAT_00668154 == 0) {
                FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_19c);
              }
              else {
                FUN_0041ee50(abStack_19c);
              }
            }
            else if (*(int *)(&this->field_0x58 + this->mbr_0xa4 * 8) == 1) {
              FUN_004795a0((byte *)&local_328);
              bVar12 = FUN_00479700((uint)&local_328,(byte *)s_BEGIN_005da178,0);
              if (CONCAT31(extraout_var_00,bVar12) == 0) {
                FUN_004795c0((byte *)&local_328);
              }
              else {
                iVar3 = FUN_004795f0((byte *)&local_328);
                if (iVar3 == 0) {
                  FUN_0058b100((char *)abStack_264,(byte *)s_Script_error_at_line__d___s_005da0d0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_264);
                  }
                  else {
                    FUN_0041ee50(abStack_264);
                  }
                }
              }
              *(undefined4 *)(&this->field_0x58 + this->mbr_0xa4 * 8) = 0xdeaf;
            }
            bVar12 = FUN_00479700((uint)&local_328,&DAT_005da180,0);
            if (CONCAT31(extraout_var_01,bVar12) != 0) {
              pcVar13 = pcVar11;
              (*(*local_31c)->virt_meth_0x4113d0_16)(local_31c);
              FUN_00478a10((byte *)&local_32c);
              FUN_00479680((byte *)&local_32c);
              local_2e4 = (*(code *)(*local_320)->_flag)(pcVar13);
              FUN_00478a10((byte *)&local_328);
            }
          }
          if (((uVar7 & 0x200) != 0) && (iVar3 = FUN_004795f0((byte *)&local_328), iVar3 == 0)) {
            FUN_0058b100((char *)abStack_200,(byte *)s_Script_error_at_line__d___s_005da0d0);
            if (DAT_00668154 == 0) {
              FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_200);
            }
            else {
              FUN_0041ee50(abStack_200);
            }
          }
          if ((uVar7 & 0x800) != 0) {
            dVar8 = this->mbr_0xa4;
            this->mbr_0xa4 = dVar8 + 1;
            *(undefined4 *)(&this->field_0x5c + dVar8 * 8) = 0;
            *(undefined4 *)(&this->field_0x58 + this->mbr_0xa4 * 8) = 0xdeaf;
          }
          if (((uVar7 & 0x1000) != 0) &&
             (dVar8 = this->mbr_0xa4 - 1, this->mbr_0xa4 = dVar8, (int)dVar8 < 0)) {
            FUN_0058b100((char *)abStack_138,(byte *)s_Script_error_at_line__d___s_005da0d0);
            if (DAT_00668154 == 0) {
              FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_138);
            }
            else {
              FUN_0041ee50(abStack_138);
            }
          }
          pcVar13 = *(char **)(&this->field_0x54 + this->mbr_0xa4 * 8);
          if (pcVar13 != (char *)0x0) {
            *(undefined4 *)(&this->field_0x54 + this->mbr_0xa4 * 8) = 0;
            local_2cc = pcVar13;
          }
          if ((uVar7 & 0x400) != 0) {
            *(char **)(&this->field_0x54 + this->mbr_0xa4 * 8) = pcVar11;
          }
          if (((uVar7 & 1) != 0) || (this->mbr_0xb4 != 0)) {
            this->mbr_0x48 = (dword)local_2cc;
LAB_00493d81:
            if (this->mbr_0x48 == 0) {
              if (local_2e8 != 0) {
                if ((int *)this->mbr_0xc != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc + 0x14c))();
                }
                if ((int *)this->mbr_0xc4 != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc4 + 0x14c))();
                }
                if ((int *)this->mbr_0xc8 != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc8 + 0x14c))();
                }
              }
              this->mbr_0x50 = 0;
              meth_0x493e40_TScript_End(this);
            }
            local_4 = 4;
            FUN_004830f0(local_300);
            if (local_324 == (LPCVOID)0x0) {
              if (local_320 != (FILE **)0x0) {
                FUN_004830f0(local_31c);
LAB_00493e14:
                FUN_004a1540(local_320);
              }
            }
            else {
              FUN_004830f0(local_31c);
              FUN_004830f0(local_324);
            }
LAB_00493e1c:
            local_4 = 0xffffffff;
            cls_0x478720::meth_0x478730(&local_2f0);
            ExceptionList = local_c;
            return 1;
          }
          if ((uVar7 & 0x2000) != 0) {
            local_2cc = (char *)this->mbr_0x48;
          }
        }
        else {
LAB_004938e7:
          FUN_0058b100((char *)abStack_2c8,(byte *)s_Script_error_at_line__d___s_005da0d0);
          if (DAT_00668154 == 0) {
            FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_2c8);
          }
          else {
            FUN_0041ee50(abStack_2c8);
          }
        }
        while ((local_318 != 9 && (local_318 != 10))) {
          FUN_00478a10((byte *)&local_328);
        }
        uVar9 = 1;
        if ((int)this->mbr_0xa4 < 1) {
          if ((int)local_32c < 1) goto LAB_00493cbd;
LAB_00493d0c:
          this->mbr_0x48 = 0;
          this->mbr_0x4c = 0;
        }
        else {
          dVar8 = local_32c - 1;
          bVar12 = (int)local_32c < 1;
          local_32c = dVar8;
          if (bVar12) {
LAB_00493cbd:
            FUN_0058b100((char *)abStack_70,(byte *)s_Script_error_at_line__d___s_005da0d0);
            if (DAT_00668154 == 0) {
              FUN_0054d170((TCharacter *)&DAT_0065c5d0,abStack_70);
              uVar9 = extraout_ECX_02;
            }
            else {
              FUN_0041ee50(abStack_70);
              uVar9 = extraout_ECX_01;
            }
            goto LAB_00493d0c;
          }
        }
        pcVar13 = (char *)this->mbr_0x48;
        pcVar11 = local_2cc;
      } while( true );
    }
    this->mbr_0x10 = 0xffffffff;
    local_4 = 2;
    FUN_004830f0(local_300);
    if (local_324 == (LPCVOID)0x0) {
      if (local_320 != (FILE **)0x0) {
        FUN_004830f0(local_31c);
        FUN_004a1540(local_320);
      }
    }
    else {
      FUN_004830f0(local_31c);
      FUN_004830f0(local_324);
    }
    local_4 = 0xffffffff;
    cls_0x478720::meth_0x478730(&local_2f0);
  }
  ExceptionList = local_c;
  return 0;
}



// Function at 00493e40

void cls_0x4922c0::meth_0x493e40_TScript_End()

{
  uint uVar1;
  cls_0x4922c0 *extraout_ECX;
  cls_0x4922c0 *pcVar2;
  cls_0x5b4f30_TPlayer *this_00;
  cls_0x4922c0 *local_4;
  
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0xb4 = 0;
  this->mbr_0xb5 = 0;
  this->mbr_0xc0 = 0;
  pcVar2 = this;
  if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xb8);
    this->mbr_0xb8 = 0;
    pcVar2 = extraout_ECX;
  }
  if (((((byte *)this->mbr_0xcc == (byte *)0x0) ||
       (uVar1 = FUN_0059a530_stricmp((uint)pcVar2,(byte *)this->mbr_0xcc,&DAT_005da134), uVar1 != 0)) ||
      (this_00 = (cls_0x5b4f30_TPlayer *)this->mbr_0xc4, this_00 == (cls_0x5b4f30_TPlayer *)0x0)) ||
     (*(short *)&(this_00->TPlayScreen).TScreen.mbr_0x4 != 0xb)) {
    this_00 = DAT_00667fcc;
  }
  local_4 = this;
  if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
    local_4 = (cls_0x4922c0 *)(this_00->TPlayScreen).mbr_0x36c;
  }
  if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
    cls_0x535a10::meth_0x536010((cls_0x535a10 *)&DAT_00667cc8);
    this->mbr_0x0 = this->mbr_0x0 & 0xfffffffb;
  }
  pcVar2 = local_4;
  if ((*(byte *)&this->mbr_0x0 & 1) != 0) {
    if (DAT_0066829c == 0) {
      TCharacter::meth_0x47c580((TCharacter *)&DAT_0065caf0);
    }
    else if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      pcVar2 = (cls_0x4922c0 *)((uint)local_4 & 0xfffffffb);
    }
    this->mbr_0x0 = this->mbr_0x0 & 0xfffffffe;
  }
  if ((*(byte *)&this->mbr_0x0 & 2) != 0) {
    if ((DAT_0066829c == 0) && (DAT_0067682c != 0)) {
      if (DAT_0065cb30 != (int *)0x0) {
        (**(code **)(*DAT_0065cb30 + 0x28))();
      }
    }
    else if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      pcVar2 = (cls_0x4922c0 *)((uint)pcVar2 & 0xfffffff7);
    }
    this->mbr_0x0 = this->mbr_0x0 & 0xfffffffd;
  }
  if ((*(byte *)&this->mbr_0x0 & 8) != 0) {
    if (DAT_0066829c == 0) {
      if ((cls_0x5b4f30_TPlayer *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) != DAT_00667fcc) {
        TCharacter::meth_0x4538d0((TCharacter *)&DAT_006668d8,DAT_00667fcc);
      }
    }
    else if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
      pcVar2 = (cls_0x4922c0 *)((uint)pcVar2 & 0xffffffef);
    }
    this->mbr_0x0 = this->mbr_0x0 & 0xfffffff7;
  }
  if (((DAT_0066829c != 0) && (this_00 != (cls_0x5b4f30_TPlayer *)0x0)) && (local_4 != pcVar2)) {
    cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(this_00);
  }
  return;
}



// Function at 00493fa0

undefined4 cls_0x4922c0::meth_0x493fa0_TScript_Jump(undefined4 param_1)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  dword dVar4;
  undefined3 extraout_var_01;
  uint uVar5;
  char *pcVar6;
  int iVar7;
  byte *in_stack_00000008;
  undefined4 local_c4;
  LPCVOID local_c0;
  FILE **local_bc;
  cls_0x5a36f8__vftable_5a36f8 **local_b8;
  int local_b4;
  undefined4 local_b0;
  int local_ac;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 *local_9c;
  undefined local_98;
  undefined4 local_94;
  cls_0x478720 local_8c;
  cls_0x5a36f8__vftable_5a36f8 *local_84;
  char *local_80;
  char *local_7c;
  char *local_78;
  char *local_74;
  byte local_70 [100];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d98d;
  local_c = ExceptionList;
  uVar5 = 0xffffffff;
  local_7c = *(char **)(this->mbr_0x8 + 4);
  pcVar6 = local_7c;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  local_84 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_78 = local_7c + (~uVar5 - 1);
  local_80 = s_String_005da1d4;
  ExceptionList = &local_c;
  local_74 = local_7c;
  cls_0x478720::cls_0x478720(&local_8c);
  local_b8 = &local_84;
  local_4 = 0;
  local_c4 = 0;
  local_c0 = (LPCVOID)0x0;
  local_bc = (FILE **)0x0;
  local_b4 = 0;
  local_b0 = 0;
  local_ac = 0;
  local_a4 = 0;
  local_a0 = 0;
  local_98 = 0;
  local_94 = 1;
  local_9c = FUN_00482fb0(0x2000);
  *(undefined *)local_9c = 0;
  local_4 = 1;
  iVar7 = 1000;
  while ((local_b4 != 10 && (iVar7 = iVar7 + -1, (int)this->mbr_0x4c < iVar7))) {
    iVar3 = FUN_004795f0((byte *)&local_c4);
    if (iVar3 == 0) {
      FUN_0058b100((char *)local_70,(byte *)s_Script_error_at_line__d___s_005da0d0);
      if (DAT_00668154 == 0) {
        FUN_0054d170((TCharacter *)&DAT_0065c5d0,local_70);
      }
      else {
        FUN_0041ee50(local_70);
      }
    }
    FUN_00479680((byte *)&local_c4);
  }
  this->mbr_0xa4 = 0;
  do {
    FUN_00479680((byte *)&local_c4);
    if (local_b4 == 10) {
      FUN_0058b100((char *)local_70,(byte *)s_Script_error_at_line__d___s_005da0d0);
      if (DAT_00668154 == 0) {
        FUN_0054d170((TCharacter *)&DAT_0065c5d0,local_70);
      }
      else {
        FUN_0041ee50(local_70);
      }
      local_4 = 2;
      FUN_004830f0(local_9c);
      if (local_c0 == (LPCVOID)0x0) {
        if (local_bc != (FILE **)0x0) {
          FUN_004830f0(local_b8);
          FUN_004a1540(local_bc);
        }
      }
      else {
        FUN_004830f0(local_b8);
        FUN_004830f0(local_c0);
      }
      local_4 = 0xffffffff;
      cls_0x478720::meth_0x478730(&local_8c);
      ExceptionList = local_c;
      return 0;
    }
    if ((local_b4 == 7) && (local_ac == 0x3a)) {
      FUN_00478a10((byte *)&local_c4);
      bVar2 = FUN_00479700((uint)&local_c4,in_stack_00000008,0);
      if (CONCAT31(extraout_var,bVar2) != 0) {
        this->mbr_0x48 = (dword)local_74;
        local_4 = 3;
        FUN_004830f0(local_9c);
        if (local_c0 == (LPCVOID)0x0) {
          if (local_bc != (FILE **)0x0) {
            FUN_004830f0(local_b8);
            FUN_004a1540(local_bc);
          }
        }
        else {
          FUN_004830f0(local_b8);
          FUN_004830f0(local_c0);
        }
        local_4 = 0xffffffff;
        cls_0x478720::meth_0x478730(&local_8c);
        ExceptionList = local_c;
        return 1;
      }
    }
    bVar2 = FUN_00479700((uint)&local_c4,(byte *)s_BEGIN_005da200,0);
    if (CONCAT31(extraout_var_00,bVar2) == 0) {
      bVar2 = FUN_00479700((uint)&local_c4,(byte *)&PTR_virt_meth_0x444e45_005da208,0);
      if (CONCAT31(extraout_var_01,bVar2) != 0) {
        dVar4 = this->mbr_0xa4 - 1;
        goto LAB_00494143;
      }
    }
    else {
      dVar4 = this->mbr_0xa4 + 1;
LAB_00494143:
      this->mbr_0xa4 = dVar4;
    }
    FUN_004795a0((byte *)&local_c4);
  } while( true );
}



// Function at 00494370

void cls_0x4922c0::meth_0x494370()

{
  dword dVar1;
  
  dVar1 = this->mbr_0x4;
  if (dVar1 != 0) {
    while ((*(byte *)(dVar1 + 0x48) & 1) == 0) {
      dVar1 = *(dword *)(dVar1 + 8);
      if (dVar1 == 0) {
        return;
      }
    }
    if (this->mbr_0x48 != 0) {
      meth_0x493e40_TScript_End(this);
    }
    this->mbr_0x48 = 0;
    this->mbr_0x8 = this->mbr_0x4;
    this->mbr_0x4c = 0;
    this->mbr_0x50 = 0;
    this->mbr_0x10 = 0xffffffff;
    this->mbr_0xb4 = 0;
    this->mbr_0xb5 = 0;
    this->mbr_0xc0 = 0;
    if ((LPCVOID)this->mbr_0xb8 != (LPCVOID)0x0) {
      FUN_004830f0((LPCVOID)this->mbr_0xb8);
      this->mbr_0xb8 = 0;
    }
    this->mbr_0xa4 = 0;
  }
  return;
}



// Function at 004943f0

undefined4 cls_0x4922c0::meth_0x4943f0()

{
  dword dVar1;
  uint in_stack_00000004;
  
  dVar1 = this->mbr_0x4;
  while( true ) {
    if (dVar1 == 0) {
      return 0;
    }
    if ((*(uint *)(dVar1 + 0x48) & in_stack_00000004) != 0) break;
    dVar1 = *(dword *)(dVar1 + 8);
  }
  return 1;
}



