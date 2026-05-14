// Decompiled methods and structure for class: cls_0x4922c0

/*
/OOAnalyzer/cls_0x4922c0
pack(disabled)
Structure cls_0x4922c0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   byte   1   mbr_0xb4   "Unsigned Byte (db)"
   181   byte   1   mbr_0xb5   "Unsigned Byte (db)"
   182   byte   1   mbr_0xb6   "Unsigned Byte (db)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   word   2   mbr_0xc0   "Unsigned Word (dw, 2-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   204   dword   4   mbr_0xcc   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 232 Alignment: 1

*/

// Function at 004922c0

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x4922c0(cls_0x4922c0 *this)

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
  if (this->mbr_0xb8 != 0) {
    FUN_004830f0(this->mbr_0xb8);
  }
  if (this->mbr_0xe0 != 0) {
    FUN_004830f0(this->mbr_0xe0);
  }
  this->mbr_0xe0 = 0;
  if (this->mbr_0xd8 != 0) {
    FUN_004830f0(this->mbr_0xd8);
  }
  this->mbr_0xd8 = 0;
  if (this->mbr_0xdc != 0) {
    FUN_004830f0(this->mbr_0xdc);
  }
  this->mbr_0xdc = 0;
  this->mbr_0xc0 = 0;
  return;
}



// Function at 004923c0

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x4923c0(cls_0x4922c0 *this)

{
  undefined4 in_stack_00000004;
  
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
  if (this->mbr_0xb8 != 0) {
    FUN_004830f0(this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  FUN_00494ab0(in_stack_00000004,0xffffffff);
  UNK_00665f1c._20_4_ = 1;
  return;
}



// Function at 00492440

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x492440(cls_0x4922c0 *this,dword param_1)

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

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x492490(cls_0x4922c0 *this)

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
  if (this->mbr_0xb8 != 0) {
    FUN_004830f0(this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  return;
}



// Function at 004924f0

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x4924f0(cls_0x4922c0 *this)

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
  if (this->mbr_0xb8 != 0) {
    FUN_004830f0(this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  this->mbr_0xa4 = 0;
  return;
}



// Function at 004927b0

undefined4 __thiscall
OOAnalyzer::cls_0x4922c0::meth_0x4927b0(cls_0x4922c0 *this,undefined4 *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int in_stack_0000000c;
  dword *pdVar6;
  undefined4 local_4;
  
  local_4 = 0;
  if (((undefined4 *)this->mbr_0xa8 == param_1) || ((int)param_1[0xe] < param_2)) {
    return 0;
  }
  switch(*param_1) {
  case 1:
    break;
  case 2:
    if (this->mbr_0x18 != 2) {
      return 0;
    }
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
      iVar4 = FUN_0059a530_stricmp(param_1 + 2,*(undefined4 *)(DAT_00667fcc + 0x38));
      if (iVar4 != 0) {
        return 0;
      }
      iVar4 = param_1[0xd];
      iVar5 = *(int *)(in_stack_0000000c + 0x14) - *(int *)(DAT_00667fcc + 0x14);
      iVar1 = iVar5;
      if (iVar5 < 1) {
        iVar1 = *(int *)(DAT_00667fcc + 0x14) - *(int *)(in_stack_0000000c + 0x14);
      }
      if (iVar4 < iVar1) {
        return 0;
      }
      iVar2 = *(int *)(in_stack_0000000c + 0x10) - *(int *)(DAT_00667fcc + 0x10);
      iVar1 = iVar2;
      if (iVar2 < 1) {
        iVar1 = *(int *)(DAT_00667fcc + 0x10) - *(int *)(in_stack_0000000c + 0x10);
      }
      if (iVar4 < iVar1) {
        return 0;
      }
      if (iVar4 * iVar4 * 2 < iVar2 * iVar2 + iVar5 * iVar5) {
        return 0;
      }
      this->mbr_0xc4 = DAT_00667fcc;
      this->mbr_0xcc = (dword)&DAT_005da114;
    }
    break;
  case 5:
    if (this->mbr_0x18 != 5) {
      if (DAT_00667fcc != 0) {
        iVar4 = FUN_0059a530_stricmp(param_1 + 2,s_player_005da11c);
        if ((iVar4 == 0) ||
           (iVar4 = FUN_0059a530_stricmp(param_1 + 2,*(undefined4 *)(DAT_00667fcc + 0x38)), iVar4 == 0)) {
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
      dVar3 = FUN_00452480(param_1 + 7,*(undefined2 *)(in_stack_0000000c + 0xe),1);
      if (dVar3 == 0) {
        return 0;
      }
      if ((*(short *)(dVar3 + 4) != 0xc) && (*(short *)(dVar3 + 4) != 0xb)) {
        return 0;
      }
      if ((*(char *)(param_1 + 2) != '\0') &&
         (iVar4 = FUN_0059a530_stricmp(param_1 + 2,*(undefined4 *)(dVar3 + 0x38)), iVar4 == 0)) {
        return 0;
      }
      this->mbr_0xc4 = dVar3;
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
    iVar4 = FUN_0059a530_stricmp(param_1 + 2,&this->mbr_0x20);
    if (iVar4 == 0) break;
    pdVar6 = &this->mbr_0x34;
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
    pdVar6 = &this->mbr_0x20;
LAB_00492804:
    iVar4 = FUN_0059a530_stricmp(param_1 + 2,pdVar6);
LAB_0049280f:
    if (iVar4 != 0) {
      return 0;
    }
    break;
  case 10:
    iVar4 = this->mbr_0x18 - 10;
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
    iVar4 = FUN_00452690_TMapPane_GetInstance(this->mbr_0x10,0);
    if (iVar4 != 0) {
      return 0;
    }
    this->mbr_0x10 = 0xffffffff;
  }
switchD_004927ec_caseD_b:
  return local_4;
}



// Function at 00492b00

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x492b00(cls_0x4922c0 *this,uint param_1)

{
  int iVar1;
  byte bVar2;
  dword dVar3;
  dword in_stack_00000008;
  
  if (this->mbr_0xb4 != 0) {
    return;
  }
  if ((((this->mbr_0xcc == 0) || (iVar1 = FUN_0059a530_stricmp(this->mbr_0xcc,&DAT_005da134), iVar1 != 0))
      || (dVar3 = this->mbr_0xc4, dVar3 == 0)) || (*(short *)(dVar3 + 4) != 0xb)) {
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

int __thiscall OOAnalyzer::cls_0x4922c0::meth_0x492d70_TScript_Triggered(cls_0x4922c0 *this)

{
  int *piVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  char *_Source;
  int iVar5;
  dword dVar6;
  uint uVar7;
  int iVar8;
  bool bVar9;
  int in_stack_00000004;
  char local_20 [31];
  undefined local_1;
  
  bVar2 = this->mbr_0xb4;
  if ((bVar2 == 4) && (0 < (int)this->mbr_0xbc)) {
    this->mbr_0xbc = this->mbr_0xbc - 1;
  }
  iVar5 = DAT_0067682c;
  iVar8 = 0;
  if ((bVar2 == 0) && (in_stack_00000004 != 0)) {
    iVar8 = 1;
    goto LAB_0049314f;
  }
  if ((bVar2 == 4) && ((int)this->mbr_0xbc < 1)) {
LAB_00492dba:
    iVar8 = 1;
    if (DAT_0066829c == 0) goto LAB_00493253;
    cls_0x57d9d0::meth_0x586dd0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40));
  }
  else if ((bVar2 == 2) || ((bVar2 == 5 || (bVar2 == 10)))) {
    local_20[0] = '\0';
    if ((DAT_0066829c == 0) || ((DAT_0067682c == 0 || (this->mbr_0xbc == DAT_00667fcc)))) {
      if (DAT_00667eac != 0) {
        bVar9 = DAT_0066829c == 0;
        this->mbr_0x0 = this->mbr_0x0 & 0xfffffffb;
        if (((bVar9) || (DAT_00676828 == 0)) || (iVar5 != 0)) {
          if (DAT_00667ea4 < 0) {
            _Source = (char *)0x0;
          }
          else {
            _Source = *(char **)(&DAT_00667e5c.field_0x24 + DAT_00667ea4 * 4);
          }
          _strncpy(local_20,_Source,0x1f);
          local_1 = 0;
        }
        else {
          cls_0x57d9d0::meth_0x586dd0
                    ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,
                     (char)*(undefined4 *)(this->mbr_0xc + 0x40));
        }
        goto LAB_00493139;
      }
    }
    else if ((this->mbr_0xb5 == 0) && (dVar6 = this->mbr_0xb8, dVar6 != 0)) {
      uVar7 = *(int *)(dVar6 + 4) - 1;
      uVar3 = (uint)this->mbr_0xb6;
      if ((int)uVar7 <= (int)(uint)this->mbr_0xb6) {
        uVar3 = uVar7;
      }
      _strncpy(local_20,(char *)(uVar3 * 0x20 + 8 + dVar6),0x1f);
      local_1 = 0;
      FUN_004830f0(this->mbr_0xb8);
      this->mbr_0xb8 = 0;
LAB_00493139:
      iVar8 = 1;
      if (local_20[0] != '\0') {
        meth_0x493fa0_TScript_Jump(this,this->mbr_0xc);
      }
    }
  }
  else if (bVar2 == 3) {
    piVar1 = (int *)this->mbr_0xbc;
    if (piVar1 == (int *)0x0) {
joined_r0x00492ea2:
      if (in_stack_00000004 != 0) goto LAB_00492dba;
    }
    else if ((piVar1[2] & 0x20000U) == 0) {
      if (piVar1 != (int *)0x0) {
        uVar3 = (**(code **)(*piVar1 + 0x154))();
joined_r0x00492f4e:
        if (uVar3 != 0) goto LAB_00492dba;
      }
    }
    else {
      if (piVar1[0x36] == piVar1[0x38]) goto LAB_00492dba;
      if ((int *)piVar1[0x15] == (int *)0x0) {
        uVar3 = 0;
      }
      else {
        uVar3 = (**(code **)(*(int *)piVar1[0x15] + 0x8c))(*(undefined2 *)(piVar1 + 3));
      }
      if ((uVar3 & 1) != 0) {
        uVar3 = (**(code **)(*(int *)this->mbr_0xbc + 0x154))();
        goto joined_r0x00492f4e;
      }
    }
  }
  else if (bVar2 == 8) {
    piVar1 = (int *)this->mbr_0xbc;
    if (piVar1 == (int *)0x0) goto joined_r0x00492ea2;
    if ((piVar1[2] & 0x20000U) == 0) {
      if ((piVar1 != (int *)0x0) && (iVar5 = (**(code **)(*piVar1 + 0x154))(), iVar5 != 0)) {
        piVar1 = *(int **)(this->mbr_0xbc + 0x54);
        if (piVar1 == (int *)0x0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (**(code **)(*piVar1 + 0x8c))(*(undefined2 *)(this->mbr_0xbc + 0xc));
        }
        uVar3 = uVar3 & 1;
        goto joined_r0x00492f4e;
      }
    }
    else if (piVar1[0x36] == piVar1[0x38]) {
      piVar1 = (int *)this->mbr_0xbc;
      iVar5 = (**(code **)(*piVar1 + 0x204))();
      iVar4 = (**(code **)(*piVar1 + 500))();
      if ((iVar5 == iVar4) && ((*(byte *)(*(int *)(this->mbr_0xbc + 0xd8) + 0x60) & 2) == 0))
      goto LAB_00492dba;
    }
  }
  else if (bVar2 == 9) {
    if (((int *)this->mbr_0xbc != (int *)0x0) &&
       (iVar5 = (**(code **)(*(int *)this->mbr_0xbc + 0x1c0))(), iVar5 < 1)) {
      iVar8 = 1;
    }
  }
  else if (bVar2 == 6) {
    if ((((DAT_0066829c != 0) && (this->mbr_0xbc != DAT_00667fcc)) &&
        ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) ||
       (iVar5 = cls_0x5a5320_TPlayScreen::meth_0x48eb00((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0), iVar5 == 0)) {
      iVar8 = 1;
      if (DAT_0066829c == 0) goto LAB_00493253;
      cls_0x57d9d0::meth_0x586dd0
                ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40)
                );
    }
  }
  else if ((bVar2 == 7) &&
          ((((DAT_0066829c != 0 && (this->mbr_0xbc != DAT_00667fcc)) &&
            ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) || (DAT_0065a568 == 0)))) {
    iVar8 = 1;
    if (DAT_0066829c == 0) goto LAB_00493253;
    cls_0x57d9d0::meth_0x586dd0
              ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,(char)*(undefined4 *)(this->mbr_0xc + 0x40));
  }
LAB_0049314f:
  if (((DAT_0066829c != 0) && (this->mbr_0xb5 != 0)) &&
     (bVar2 = this->mbr_0xb5 - 1, this->mbr_0xb5 = bVar2, bVar2 == 0)) {
    if (((this->mbr_0xcc == 0) || (iVar5 = FUN_0059a530_stricmp(this->mbr_0xcc,&DAT_005da134), iVar5 != 0))
       || ((dVar6 = this->mbr_0xc4, dVar6 == 0 || (*(short *)(dVar6 + 4) != 0xb)))) {
      dVar6 = DAT_00667fcc;
    }
    if (dVar6 == 0) {
      if (this->mbr_0x48 != 0) {
        meth_0x493e40_TScript_End(this);
      }
      this->mbr_0x8 = this->mbr_0x4;
    }
    else {
      if ((*(uint *)(dVar6 + 0x36c) & 0x10000) != 0) {
        cls_0x57d9d0::meth_0x586cc0
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,dVar6,this->mbr_0xc,(uint)this->mbr_0xb4)
        ;
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
    if (this->mbr_0xb8 != 0) {
      FUN_004830f0(this->mbr_0xb8);
      this->mbr_0xb8 = 0;
    }
    this->mbr_0xa4 = 0;
    return 0;
  }
LAB_00493253:
  iVar5 = DAT_0066829c;
  if (this->mbr_0xb5 == 0) {
    if (iVar8 != 0) {
      this->mbr_0xb4 = 0;
      this->mbr_0xb5 = 0;
      if ((iVar5 != 0) && (this->mbr_0x1c != 1)) {
        this->mbr_0xc0 = 0xb40;
      }
    }
    return iVar8;
  }
  return 0;
}



// Function at 004933d0

undefined4 __thiscall OOAnalyzer::cls_0x4922c0::meth_0x4933d0_TScript_Continue(cls_0x4922c0 *this)

{
  dword dVar1;
  word wVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  uint uVar7;
  dword dVar8;
  dword *pdVar9;
  char *pcVar10;
  bool bVar11;
  int in_stack_00000004;
  char *pcVar12;
  dword local_32c;
  undefined4 local_328;
  int local_324;
  int *local_320;
  cls_0x5a36f8__vftable_5a36f8 **local_31c;
  int local_318;
  undefined4 local_314;
  int local_310;
  undefined4 local_308;
  undefined4 local_304;
  undefined *local_300;
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
  undefined auStack_2c8 [100];
  undefined auStack_264 [100];
  undefined auStack_200 [100];
  undefined auStack_19c [100];
  undefined auStack_138 [100];
  undefined auStack_d4 [100];
  undefined auStack_70 [100];
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
      bVar11 = DAT_0066829c != 0;
      ExceptionList = &local_c;
      this->mbr_0xc0 = wVar2;
      if ((bVar11) && (wVar2 == 0)) {
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
    if ((this->mbr_0xe4 != 0) && (this->mbr_0xd8 != 0)) {
      FUN_004d0950(this->mbr_0xd8,0xffffffff,0,this->mbr_0xdc);
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
    local_324 = 0;
    local_320 = (int *)0x0;
    local_318 = 0;
    local_314 = 0;
    local_310 = 0;
    local_308 = 0;
    local_304 = 0;
    local_2fc = 0;
    local_2f8 = 1;
    local_300 = (undefined *)FUN_00482fb0(0x2000);
    *local_300 = 0;
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
                pdVar9 = *(dword **)(*(int *)(dVar8 + 0x1c) + iVar3 * 4);
                if (pdVar9 == (dword *)0x0) {
                  pdVar9 = *(dword **)(dVar8 + 0x20);
                }
                meth_0x492440(this,dVar8);
                this->mbr_0x1c = *pdVar9;
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
                if ((int *)this->mbr_0xc8 != (int *)0x0) {
                  (**(code **)(*(int *)this->mbr_0xc8 + 0x148))();
                }
                if (this->mbr_0x1c == 1) {
                  this->mbr_0x10 = 0xffffffff;
                }
                else if (((this->mbr_0xc4 != 0) && (this->mbr_0xcc != 0)) &&
                        (iVar3 = FUN_0059a530_stricmp(this->mbr_0xcc,&DAT_005da14c), iVar3 == 0)) {
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
        pdVar9 = *(dword **)(*(int *)(local_32c + 0x1c) + local_2e0 * 4);
        if (pdVar9 == (dword *)0x0) {
          pdVar9 = *(dword **)(local_32c + 0x20);
        }
        meth_0x492440(this,local_32c);
        this->mbr_0x1c = *pdVar9;
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
      pcVar12 = (char *)this->mbr_0x48;
      local_32c = 6000;
      pcVar10 = pcVar12;
      do {
        local_2cc = pcVar10;
        if ((pcVar12 == (char *)0x0) || ((this->mbr_0x4c & 0x10000) != 0)) goto LAB_00493d81;
        if ((pcVar12 < pcVar10) &&
           ((iVar3 = FUN_0058af6f((int)*pcVar10), iVar3 == 0 &&
            (iVar3 = FUN_0058af6f((int)pcVar10[-1]), iVar3 == 0)))) {
          pcVar10 = pcVar10 + -1;
        }
        FUN_00478a10();
        FUN_00479680();
        if (local_2e4 != 0) {
          (*(*local_31c)->virt_meth_0x4113d0_16)(local_31c);
          FUN_00479580();
        }
        local_2e4 = 0;
        if (local_318 == 7) {
          if (local_310 != 0x3a) goto LAB_004938e7;
          FUN_004795c0();
        }
        else if (((local_318 == 4) || (local_318 == 3)) || (local_318 == 2)) {
          uVar7 = FUN_0041e8e0(this->mbr_0xc,&local_328,0,this);
          if ((uVar7 & 0x20) != 0) {
            local_4 = 3;
            FUN_004830f0(local_300);
            if (local_324 != 0) {
              FUN_004830f0(local_31c);
              FUN_004830f0(local_324);
              goto LAB_00493e1c;
            }
            if (local_320 == (int *)0x0) goto LAB_00493e1c;
            FUN_004830f0(local_31c);
            goto LAB_00493e14;
          }
          if ((uVar7 & 0x40) == 0) {
            if ((uVar7 & 0x80) != 0) {
              FUN_004795a0();
              iVar3 = FUN_00479700(s_BEGIN_005da154,0);
              if (iVar3 == 0) {
                FUN_004795c0();
              }
              else {
                iVar3 = FUN_004795f0();
                if (iVar3 == 0) {
                  FUN_0058b100(auStack_d4,s_Script_error_at_line__d___s_005da0d0,0,
                               s_BEGIN_without_matching_END_005da0f0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170(&DAT_0065c5d0,auStack_d4);
                  }
                  else {
                    FUN_0041ee50(auStack_d4);
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
              FUN_0058b100(auStack_19c,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                           s_ELSE_without_matching_IF_005da15c);
              if (DAT_00668154 == 0) {
                FUN_0054d170(&DAT_0065c5d0,auStack_19c);
              }
              else {
                FUN_0041ee50(auStack_19c);
              }
            }
            else if (*(int *)(&this->field_0x58 + this->mbr_0xa4 * 8) == 1) {
              FUN_004795a0();
              iVar3 = FUN_00479700(s_BEGIN_005da178,0);
              if (iVar3 == 0) {
                FUN_004795c0();
              }
              else {
                iVar3 = FUN_004795f0();
                if (iVar3 == 0) {
                  FUN_0058b100(auStack_264,s_Script_error_at_line__d___s_005da0d0,0,
                               s_BEGIN_without_matching_END_005da0f0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170(&DAT_0065c5d0,auStack_264);
                  }
                  else {
                    FUN_0041ee50(auStack_264);
                  }
                }
              }
              *(undefined4 *)(&this->field_0x58 + this->mbr_0xa4 * 8) = 0xdeaf;
            }
            iVar3 = FUN_00479700(&DAT_005da180,0);
            if (iVar3 != 0) {
              pcVar12 = pcVar10;
              (*(*local_31c)->virt_meth_0x4113d0_16)(local_31c);
              FUN_00478a10(pcVar12);
              FUN_00479680();
              local_2e4 = (**(code **)(*local_320 + 0xc))();
              FUN_00478a10();
            }
          }
          if (((uVar7 & 0x200) != 0) && (iVar3 = FUN_004795f0(), iVar3 == 0)) {
            FUN_0058b100(auStack_200,s_Script_error_at_line__d___s_005da0d0,0,
                         s_BEGIN_without_matching_END_005da0f0);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_200);
            }
            else {
              FUN_0041ee50(auStack_200);
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
            FUN_0058b100(auStack_138,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                         s_END_without_matching_BEGIN_005da184);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_138);
            }
            else {
              FUN_0041ee50(auStack_138);
            }
          }
          pcVar12 = *(char **)(&this->field_0x54 + this->mbr_0xa4 * 8);
          if (pcVar12 != (char *)0x0) {
            *(undefined4 *)(&this->field_0x54 + this->mbr_0xa4 * 8) = 0;
            local_2cc = pcVar12;
          }
          if ((uVar7 & 0x400) != 0) {
            *(char **)(&this->field_0x54 + this->mbr_0xa4 * 8) = pcVar10;
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
            if (local_324 == 0) {
              if (local_320 != (int *)0x0) {
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
          FUN_0058b100(auStack_2c8,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                       s_Bad_token_in_trigger_block_005da1a0);
          if (DAT_00668154 == 0) {
            FUN_0054d170(&DAT_0065c5d0,auStack_2c8);
          }
          else {
            FUN_0041ee50(auStack_2c8);
          }
        }
        while ((local_318 != 9 && (local_318 != 10))) {
          FUN_00478a10();
        }
        if ((int)this->mbr_0xa4 < 1) {
          if ((int)local_32c < 1) goto LAB_00493cbd;
LAB_00493d0c:
          this->mbr_0x48 = 0;
          this->mbr_0x4c = 0;
        }
        else {
          iVar3 = local_32c + -1;
          bVar11 = (int)local_32c < 1;
          local_32c = iVar3;
          if (bVar11) {
LAB_00493cbd:
            FUN_0058b100(auStack_70,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                         s_Infinite_loop_detected_005da1bc);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_70);
            }
            else {
              FUN_0041ee50(auStack_70);
            }
            goto LAB_00493d0c;
          }
        }
        pcVar12 = (char *)this->mbr_0x48;
        pcVar10 = local_2cc;
      } while( true );
    }
    this->mbr_0x10 = 0xffffffff;
    local_4 = 2;
    FUN_004830f0(local_300);
    if (local_324 == 0) {
      if (local_320 != (int *)0x0) {
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

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x493e40_TScript_End(cls_0x4922c0 *this)

{
  int iVar1;
  cls_0x4922c0 *pcVar2;
  cls_0x5b4f30_TPlayer *this_00;
  cls_0x4922c0 *local_4;
  
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0xb4 = 0;
  this->mbr_0xb5 = 0;
  this->mbr_0xc0 = 0;
  if (this->mbr_0xb8 != 0) {
    FUN_004830f0(this->mbr_0xb8);
    this->mbr_0xb8 = 0;
  }
  if ((((this->mbr_0xcc == 0) || (iVar1 = FUN_0059a530_stricmp(this->mbr_0xcc,&DAT_005da134), iVar1 != 0))
      || (this_00 = (cls_0x5b4f30_TPlayer *)this->mbr_0xc4, this_00 == (cls_0x5b4f30_TPlayer *)0x0)) ||
     (*(short *)&(this_00->cls_0x5a7b98_TCharacter).cls_0x5a50e8.mbr_0x4 != 0xb)) {
    this_00 = DAT_00667fcc;
  }
  local_4 = this;
  if (this_00 != (cls_0x5b4f30_TPlayer *)0x0) {
    local_4 = (cls_0x4922c0 *)(this_00->cls_0x5a7b98_TCharacter).mbr_0x36c;
  }
  if ((*(byte *)&this->mbr_0x0 & 4) != 0) {
    cls_0x535a10::meth_0x536010((cls_0x535a10 *)&DAT_00667cc8);
    this->mbr_0x0 = this->mbr_0x0 & 0xfffffffb;
  }
  pcVar2 = local_4;
  if ((*(byte *)&this->mbr_0x0 & 1) != 0) {
    if (DAT_0066829c == 0) {
      cls_0x5a5320_TPlayScreen::meth_0x47c580((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
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
        cls_0x5a5320_TPlayScreen::meth_0x4538d0((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,DAT_00667fcc);
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

/* WARNING: Removing unreachable block (ram,0x0049422f) */
/* WARNING: Removing unreachable block (ram,0x00494157) */
/* WARNING: Removing unreachable block (ram,0x0049418b) */
/* WARNING: Removing unreachable block (ram,0x0049417c) */
/* WARNING: Removing unreachable block (ram,0x0049419d) */
/* WARNING: Removing unreachable block (ram,0x004941d3) */
/* WARNING: Removing unreachable block (ram,0x004940ea) */
/* WARNING: Removing unreachable block (ram,0x004940f1) */
/* WARNING: Removing unreachable block (ram,0x00494208) */
/* WARNING: Removing unreachable block (ram,0x00494245) */
/* WARNING: Removing unreachable block (ram,0x0049424b) */
/* WARNING: Removing unreachable block (ram,0x0049425f) */
/* WARNING: Removing unreachable block (ram,0x00494262) */
/* WARNING: Removing unreachable block (ram,0x004941d9) */
/* WARNING: Removing unreachable block (ram,0x004941bd) */
/* WARNING: Removing unreachable block (ram,0x004941ed) */
/* WARNING: Removing unreachable block (ram,0x004941f0) */
/* WARNING: Removing unreachable block (ram,0x0049427b) */

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x493fa0_TScript_Jump(cls_0x4922c0 *this,undefined4 param_1)

{
  char cVar1;
  undefined *puVar2;
  int iVar3;
  dword dVar4;
  uint uVar5;
  char *pcVar6;
  int iVar7;
  cls_0x478720 local_8c;
  cls_0x5a36f8__vftable_5a36f8 *local_84;
  char *local_80;
  char *local_7c;
  char *local_78;
  char *local_74;
  undefined local_70 [100];
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
  local_4 = 0;
  puVar2 = (undefined *)FUN_00482fb0(0x2000);
  *puVar2 = 0;
  local_4 = 1;
  iVar7 = 1000;
  while (iVar7 = iVar7 + -1, (int)this->mbr_0x4c < iVar7) {
    iVar3 = FUN_004795f0();
    if (iVar3 == 0) {
      FUN_0058b100(local_70,s_Script_error_at_line__d___s_005da0d0,0,
                   s_BEGIN_without_matching_END_005da0f0);
      if (DAT_00668154 == 0) {
        FUN_0054d170(&DAT_0065c5d0,local_70);
      }
      else {
        FUN_0041ee50(local_70);
      }
    }
    FUN_00479680();
  }
  this->mbr_0xa4 = 0;
  do {
    FUN_00479680();
    iVar7 = FUN_00479700(s_BEGIN_005da200,0);
    if (iVar7 == 0) {
      iVar7 = FUN_00479700(&DAT_005da208,0);
      if (iVar7 != 0) {
        dVar4 = this->mbr_0xa4 - 1;
        goto LAB_00494143;
      }
    }
    else {
      dVar4 = this->mbr_0xa4 + 1;
LAB_00494143:
      this->mbr_0xa4 = dVar4;
    }
    FUN_004795a0();
  } while( true );
}



// Function at 00494370

void __thiscall OOAnalyzer::cls_0x4922c0::meth_0x494370(cls_0x4922c0 *this)

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
    if (this->mbr_0xb8 != 0) {
      FUN_004830f0(this->mbr_0xb8);
      this->mbr_0xb8 = 0;
    }
    this->mbr_0xa4 = 0;
  }
  return;
}



// Function at 004943f0

undefined4 __thiscall OOAnalyzer::cls_0x4922c0::meth_0x4943f0(cls_0x4922c0 *this)

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



