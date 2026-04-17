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
  FILE **ppFVar1;
  undefined4 *puVar2;
  uint **ppuVar3;
  uint *puVar4;
  int extraout_ECX;
  int iVar5;
  void *in_stack_00001174;
  void *local_c;
  undefined *local_8;
  FILE **local_4;
  
  local_4 = (FILE **)0xffffffff;
  local_8 = &LAB_0059dc42;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058c030();
  this_00 = (cls_0x45f7c0 *)(extraout_ECX + 0x14);
  iVar5 = 0;
  if (0 < *(int *)(extraout_ECX + 0x14)) {
    do {
      cls_0x45f7c0::meth_0x49d920(this_00);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
  }
  iVar5 = DAT_0065a784;
  (this_00->cls_0x41c7f0).mbr_0x0 = 0;
  *(undefined4 *)(extraout_ECX + 0x18) = 0;
  if ((-1 < iVar5) && (*(int *)(DAT_0065a77c + iVar5 * 4) != 0)) {
    FUN_0058b100(&stack0x00000074,(byte *)s__s_s__s_dialog_def_005daac4);
    ppFVar1 = (FILE **)FUN_004a13f0(&stack0x00000074,(uint *)&DAT_005daad8,0);
    if (ppFVar1 == (FILE **)0x0) {
      FUN_0058b100(&stack0x00000074,(byte *)s__s_s__s_def_005daadc);
      ppFVar1 = (FILE **)FUN_004a13f0(&stack0x00000074,(uint *)&DAT_005daae8,0);
      if (ppFVar1 == (FILE **)0x0) {
        FUN_0058b100(&stack0x00000074,(byte *)s__s_s__s_def_005daaf4);
        ppFVar1 = (FILE **)FUN_004a13f0(&stack0x00000074,(uint *)&DAT_005dab00,0);
        if (ppFVar1 == (FILE **)0x0) {
          ExceptionList = in_stack_00001174;
          return 0;
        }
      }
    }
    cls_0x478720::cls_0x478720((cls_0x478720 *)&param_11);
    local_c = (void *)0x0;
    local_8 = (undefined *)0x0;
    local_4 = (FILE **)0x0;
    puVar2 = FUN_00482fb0(0x2000);
    *(undefined *)puVar2 = 0;
    iVar5 = FUN_004788d0((int)&local_c,ppFVar1,(cls_0x5a36f8__vftable_5a36f8 *)&stack0x00000074);
    if (iVar5 != 0) {
      iVar5 = FUN_00479450((uint *)&local_c);
      if (iVar5 == 0) {
        FUN_00479950(&local_c,(byte *)s_Syntax_error_in_header_005dab04);
      }
      do {
        iVar5 = FUN_0047a410((uint *)&local_c,(float **)s__63t__4091s_005dab1c);
        if (iVar5 == 0) {
          FUN_00479950(&local_c,(byte *)s_tag__line__expected_005dab28);
        }
        FUN_00479950(&local_c,(byte *)s_RETURN_expected_005dab3c);
        FUN_0059be72((uint *)&param_13);
        ppuVar3 = (uint **)FUN_00482fb0(8);
        if (ppuVar3 == (uint **)0x0) {
          ppuVar3 = (uint **)0x0;
        }
        else {
          ppuVar3[1] = (uint *)0x0;
          *ppuVar3 = (uint *)0x0;
        }
        puVar4 = FUN_0059b6bc((uint *)&param_13);
        *ppuVar3 = puVar4;
        puVar4 = FUN_0059b6bc((uint *)&stack0x00000178);
        ppuVar3[1] = puVar4;
        cls_0x45f7c0::meth_0x41c840(this_00);
        FUN_00479450((uint *)&local_c);
      } while( true );
    }
    FUN_004830f0(puVar2);
    if (local_8 == (undefined *)0x0) {
      if (local_4 != (FILE **)0x0) {
        FUN_004830f0((LPCVOID)0x0);
        FUN_004a1540(local_4);
      }
    }
    else {
      FUN_004830f0((LPCVOID)0x0);
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
  LPCVOID *ppvVar1;
  int iVar2;
  
  this_00 = &this->mbr_0x14;
  iVar2 = 0;
  if ((int)this->mbr_0x14 < 1) {
    *this_00 = 0;
    this->mbr_0x18 = 0;
    return;
  }
  do {
    if ((-1 < iVar2) &&
       (ppvVar1 = *(LPCVOID **)(this->mbr_0x24 + iVar2 * 4), ppvVar1 != (LPCVOID *)0x0)) {
      if (*ppvVar1 != (LPCVOID)0x0) {
        FUN_00482f80(*ppvVar1);
      }
      if (ppvVar1[1] != (LPCVOID)0x0) {
        FUN_00482f80(ppvVar1[1]);
      }
      FUN_004830f0(ppvVar1);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
    iVar2 = iVar2 + 1;
  } while (iVar2 < (int)*this_00);
  *this_00 = 0;
  this->mbr_0x18 = 0;
  return;
}



