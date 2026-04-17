// Decompiled methods and structure for class: cls_0x49d2a0

/*
/OOAnalyzer/cls_0x49d2a0
pack(disabled)
Structure cls_0x49d2a0 {
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 40 Alignment: 1

*/

// Function at 0049d2a0

/* WARNING: Removing unreachable block (ram,0x0049d5ac) */
/* WARNING: Removing unreachable block (ram,0x0049d5f5) */
/* WARNING: Removing unreachable block (ram,0x0049d5fb) */
/* WARNING: Removing unreachable block (ram,0x0049d5df) */
/* WARNING: Removing unreachable block (ram,0x0049d60f) */
/* WARNING: Removing unreachable block (ram,0x0049d612) */

undefined4 __thiscall
OOAnalyzer::cls_0x49d2a0::meth_0x49d2a0
          (cls_0x49d2a0 *this,undefined4 param_1,undefined4 param_2,undefined param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined1 param_7,
          undefined4 param_8,undefined param_9,undefined1 param_10,undefined param_11,
          undefined param_12,undefined param_13)

{
  cls_0x45f7c0 *this_00;
  undefined *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int extraout_ECX;
  int iVar4;
  void *in_stack_00001174;
  void *local_c;
  undefined *local_8;
  int local_4;
  
  local_4 = 0xffffffff;
  local_8 = &LAB_0059dc42;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058c030();
  this_00 = (cls_0x45f7c0 *)(extraout_ECX + 0x14);
  iVar4 = 0;
  if (0 < *(int *)(extraout_ECX + 0x14)) {
    do {
      cls_0x45f7c0::meth_0x49d920(this_00);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
  }
  iVar4 = DAT_0065a784;
  (this_00->cls_0x41c7f0).mbr_0x0 = 0;
  *(undefined4 *)(extraout_ECX + 0x18) = 0;
  if ((-1 < iVar4) && (iVar4 = *(int *)(DAT_0065a77c + iVar4 * 4), iVar4 != 0)) {
    FUN_0058b100(&stack0x00000074,s__s_s__s_dialog_def_005daac4,&DAT_0065d6a4,iVar4 + 0x58,0x65bc18)
    ;
    iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005daad8,0);
    if (iVar4 == 0) {
      if (DAT_0065a784 < 0) {
        iVar4 = 0;
      }
      else {
        iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
      }
      FUN_0058b100(&stack0x00000074,s__s_s__s_def_005daadc,&DAT_0065d6a4,iVar4 + 0x58,0x65bc18);
      iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005daae8,0);
      if (iVar4 == 0) {
        if (DAT_0065a784 < 0) {
          iVar4 = 0;
        }
        else {
          iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
        }
        FUN_0058b100(&stack0x00000074,s__s_s__s_def_005daaf4,&DAT_0065d6a4,iVar4 + 0x58,
                     s_english_005daaec);
        iVar4 = FUN_004a13f0(&stack0x00000074,&DAT_005dab00,0);
        if (iVar4 == 0) {
          ExceptionList = in_stack_00001174;
          return 0;
        }
      }
    }
    cls_0x478720::cls_0x478720((cls_0x478720 *)&param_11);
    local_c = (void *)0x0;
    local_8 = (undefined *)0x0;
    local_4 = 0;
    puVar1 = (undefined *)FUN_00482fb0(0x2000);
    *puVar1 = 0;
    iVar4 = FUN_004788d0(iVar4,&stack0x00000074);
    if (iVar4 != 0) {
      iVar4 = FUN_00479450();
      if (iVar4 == 0) {
        FUN_00479950(s_Syntax_error_in_header_005dab04,0);
      }
      do {
        iVar4 = FUN_0047a410(&local_c,s__63t__4091s_005dab1c,&param_13,&stack0x00000178);
        if (iVar4 == 0) {
          FUN_00479950(s_tag__line__expected_005dab28,0);
        }
        FUN_00479950(s_RETURN_expected_005dab3c,0);
        FUN_0059be72(&param_13);
        puVar2 = (undefined4 *)FUN_00482fb0(8);
        if (puVar2 == (undefined4 *)0x0) {
          puVar2 = (undefined4 *)0x0;
        }
        else {
          puVar2[1] = 0;
          *puVar2 = 0;
        }
        uVar3 = FUN_0059b6bc(&param_13);
        *puVar2 = uVar3;
        uVar3 = FUN_0059b6bc(&stack0x00000178);
        puVar2[1] = uVar3;
        cls_0x45f7c0::meth_0x41c840(this_00);
        FUN_00479450();
      } while( true );
    }
    FUN_004830f0(puVar1);
    if (local_8 == (undefined *)0x0) {
      if (local_4 != 0) {
        FUN_004830f0(0);
        FUN_004a1540(local_4);
      }
    }
    else {
      FUN_004830f0(0);
      FUN_004830f0(local_8);
    }
    cls_0x478720::meth_0x478730((cls_0x478720 *)&param_11);
  }
  ExceptionList = in_stack_00001174;
  return 0;
}



// Function at 0049d650

void __thiscall OOAnalyzer::cls_0x49d2a0::meth_0x49d650(cls_0x49d2a0 *this)

{
  dword *this_00;
  int *piVar1;
  int iVar2;
  
  this_00 = &this->mbr_0x14;
  iVar2 = 0;
  if ((int)this->mbr_0x14 < 1) {
    *this_00 = 0;
    this->mbr_0x18 = 0;
    return;
  }
  do {
    if ((-1 < iVar2) && (piVar1 = *(int **)(this->mbr_0x24 + iVar2 * 4), piVar1 != (int *)0x0)) {
      if (*piVar1 != 0) {
        FUN_00482f80(*piVar1);
      }
      if (piVar1[1] != 0) {
        FUN_00482f80(piVar1[1]);
      }
      FUN_004830f0(piVar1);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
    iVar2 = iVar2 + 1;
  } while (iVar2 < (int)*this_00);
  *this_00 = 0;
  this->mbr_0x18 = 0;
  return;
}



