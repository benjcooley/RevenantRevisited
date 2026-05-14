// Decompiled methods and structure for class: cls_0x496330

/*
/OOAnalyzer/cls_0x496330
pack(disabled)
Structure cls_0x496330 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16388   dword   4   mbr_0x4004   "Unsigned Double-Word (ddw, 4-bytes)"
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32776   dword   4   mbr_0x8008   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
   32792   dword   4   mbr_0x8018   "Unsigned Double-Word (ddw, 4-bytes)"
   32796   dword   4   mbr_0x801c   "Unsigned Double-Word (ddw, 4-bytes)"
   32800   dword   4   mbr_0x8020   "Unsigned Double-Word (ddw, 4-bytes)"
   32808   dword   4   mbr_0x8028   "Unsigned Double-Word (ddw, 4-bytes)"
   32812   dword   4   mbr_0x802c   "Unsigned Double-Word (ddw, 4-bytes)"
   32816   dword   4   mbr_0x8030   "Unsigned Double-Word (ddw, 4-bytes)"
   32828   dword   4   mbr_0x803c   "Unsigned Double-Word (ddw, 4-bytes)"
   32832   dword   4   mbr_0x8040   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32836 Alignment: 1

*/

// Function at 00496330

void __thiscall OOAnalyzer::cls_0x496330::meth_0x496330_TScriptManager_Close(cls_0x496330 *this)

{
  cls_0x4922c0 *this_00;
  LPCVOID *ppvVar1;
  LPCVOID pvVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  dword *pdVar6;
  char local_104 [260];
  
  if (DAT_00668154 != 0) {
    meth_0x496690_TScriptManager_Save(this,s_master_s_005da534);
    FUN_0058b100(local_104,&DAT_005da4d4);
  }
  iVar4 = 0;
  if (0 < (int)this->mbr_0x8018) {
    do {
      this_00 = *(cls_0x4922c0 **)(this->mbr_0x8028 + iVar4 * 4);
      if (this_00 != (cls_0x4922c0 *)0x0) {
        this_00->mbr_0xc = 0;
        cls_0x4922c0::meth_0x4922c0(this_00);
        FUN_004830f0(this_00);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)this->mbr_0x8018);
  }
  if ((undefined4 *)this->mbr_0x8028 != (undefined4 *)0x0) {
    puVar5 = (undefined4 *)this->mbr_0x8028;
    for (uVar3 = this->mbr_0x8020 & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar5 = 0;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
  }
  pdVar6 = &this->mbr_0x8004;
  this->mbr_0x8018 = 0;
  iVar4 = 0;
  this->mbr_0x801c = 0;
  if (0 < (int)*pdVar6) {
    do {
      if ((-1 < iVar4) &&
         (ppvVar1 = *(LPCVOID **)(this->mbr_0x8014 + iVar4 * 4), ppvVar1 != (LPCVOID *)0x0)) {
        FUN_004948e0_TScriptProto_Dtor(ppvVar1);
        FUN_004830f0(ppvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar6);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar6);
  }
  *pdVar6 = 0;
  this->mbr_0x8008 = 0;
  pdVar6 = &this->mbr_0x802c;
  iVar4 = 0;
  if (0 < (int)this->mbr_0x802c) {
    do {
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x803c + iVar4 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar6);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar6);
  }
  *pdVar6 = 0;
  iVar4 = 0;
  this->mbr_0x8030 = 0;
  if (0 < (int)this->mbr_0x0) {
    pdVar6 = &this->mbr_0x4004;
    do {
      if ((LPCVOID)*pdVar6 != (LPCVOID)0x0) {
        FUN_004830f0((LPCVOID)*pdVar6);
      }
      *pdVar6 = 0;
      iVar4 = iVar4 + 1;
      pdVar6 = pdVar6 + 1;
    } while (iVar4 < (int)this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  return;
}



// Function at 00496690

undefined4 __thiscall OOAnalyzer::cls_0x496330::meth_0x496690_TScriptManager_Save(cls_0x496330 *this,undefined4 param_1)

{
  FILE *pFVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  int in_stack_00000008;
  char local_104 [260];
  
  if (this->mbr_0x8040 == 0) {
    return 1;
  }
  FUN_0058b100(local_104,&DAT_005da588);
  pFVar1 = (FILE *)FUN_004457c0(local_104,&DAT_005da590);
  if (pFVar1 == (FILE *)0x0) {
    return 0;
  }
  FUN_0058b56e((char **)pFVar1,(byte *)s___________________Revenant_Scrip_005da594);
  uVar4 = 0;
  uVar3 = 1;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((this->mbr_0x8014 != 0) && (uVar4 < this->mbr_0x8004)) &&
         (*(int *)(this->mbr_0x8014 + uVar4 * 4) != 0)) {
        iVar2 = *(int *)(this->mbr_0x8014 + uVar4 * 4);
        if (*(int *)(iVar2 + 0x38) == in_stack_00000008) {
          iVar2 = FUN_0058e6f7(*(undefined4 **)(iVar2 + 4),(char **)pFVar1);
          if (iVar2 != -1) {
            iVar2 = FUN_0058e6f7((undefined4 *)&DAT_005da44c,(char **)pFVar1);
            if (iVar2 != -1) goto LAB_00496772;
          }
          uVar3 = 0;
        }
      }
LAB_00496772:
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)this->mbr_0x8004);
  }
  FUN_0058b4f1(pFVar1);
  this->mbr_0x8040 = 0;
  return uVar3;
}



