// Decompiled methods and structure for class: cls_0x497120

/*
/OOAnalyzer/cls_0x497120
pack(disabled)
Structure cls_0x497120 {
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
   32792   dword   4   mbr_0x8018   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32796 Alignment: 1

*/

// Function at 00497120

cls_0x492170 * __thiscall
OOAnalyzer::cls_0x497120::meth_0x497120_TScriptManager_AddScript
          (cls_0x497120 *this,undefined4 param_1,uint param_2,char *param_3)

{
  char cVar1;
  void **ppvVar2;
  uint uVar3;
  char **ppcVar4;
  cls_0x492170 *this_00;
  dword dVar5;
  cls_0x492170 *pcVar6;
  int iVar7;
  char *pcVar8;
  char *in_stack_00000010;
  uint local_e4;
  LPCVOID local_e0;
  FILE **local_dc;
  cls_0x5a36f8__vftable_5a36f8 **local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 *local_bc;
  undefined local_b8;
  undefined4 local_b4;
  cls_0x478720 local_ac;
  cls_0x5a36f8__vftable_5a36f8 *local_a4;
  char *local_a0;
  char *local_9c;
  char *local_98;
  char *local_94;
  undefined4 local_90;
  char local_8c [128];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059dab0;
  local_c = ExceptionList;
  pcVar6 = (cls_0x492170 *)0x0;
  iVar7 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)this->mbr_0x8004) {
    do {
      ExceptionList = ppvVar2;
      uVar3 = FUN_0059a530_stricmp(param_2,**(byte ***)(this->mbr_0x8014 + iVar7 * 4),(byte *)param_2);
      if (uVar3 == 0) {
        if (*(int *)(this->mbr_0x8014 + iVar7 * 4) != 0) goto LAB_00497314;
        break;
      }
      iVar7 = iVar7 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar7 < (int)this->mbr_0x8004);
  }
  FUN_0058b100(local_8c,(byte *)s_OBJECT__s_BEGIN_END_005da6b4);
  uVar3 = 0xffffffff;
  pcVar8 = local_8c;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  local_9c = local_8c;
  local_94 = local_8c;
  local_a4 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_98 = local_8c + (~uVar3 - 1);
  local_a0 = s_String_005da6d0;
  cls_0x478720::cls_0x478720(&local_ac);
  local_d8 = &local_a4;
  local_4 = 0;
  local_e4 = 0;
  local_e0 = (LPCVOID)0x0;
  local_dc = (FILE **)0x0;
  local_d4 = 0;
  local_d0 = 0;
  local_cc = 0;
  local_c4 = 0;
  local_c0 = 0;
  local_b8 = 0;
  local_b4 = 1;
  local_bc = FUN_00482fb0(0x2000);
  *(undefined *)local_bc = 0;
  local_4 = 1;
  FUN_00478a10((byte *)&local_e4);
  local_90 = (char **)FUN_00482fb0(0x4c);
  local_4._0_1_ = 2;
  if (local_90 == (char **)0x0) {
    ppcVar4 = (char **)0x0;
  }
  else {
    ppcVar4 = FUN_004946f0_TScriptProto_Ctor(local_90,(char *)0x0,param_3,in_stack_00000010,(char *)0x0);
  }
  local_4 = CONCAT31(local_4._1_3_,1);
  FUN_00494e20_TScriptProto_ParseScript(ppcVar4,&local_e4);
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x8004);
  local_4 = 3;
  FUN_004830f0(local_bc);
  if (local_e0 == (LPCVOID)0x0) {
    if (local_dc != (FILE **)0x0) {
      FUN_004830f0(local_d8);
      FUN_004a1540(local_dc);
    }
  }
  else {
    FUN_004830f0(local_d8);
    FUN_004830f0(local_e0);
  }
  local_4 = 0xffffffff;
  cls_0x478720::meth_0x478730(&local_ac);
LAB_00497314:
  this_00 = (cls_0x492170 *)FUN_00482fb0(0xe8);
  if (this_00 != (cls_0x492170 *)0x0) {
    pcVar6 = cls_0x492170::cls_0x492170(this_00,param_1);
  }
  dVar5 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x8018);
  pcVar6->mbr_0x14 = dVar5;
  ExceptionList = local_c;
  return pcVar6;
}



