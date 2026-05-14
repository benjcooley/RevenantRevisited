// FUN_00496330_TScriptManager_Close @ 00496330 size=343

void __fastcall FUN_00496330_TScriptManager_Close(int *param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  undefined1 local_104 [260];
  
  if (DAT_00668154 != 0) {
    FUN_00496690_TScriptManager_Save(s_master_s_005da534,0);
    FUN_0058b100(local_104,&DAT_005da4d4,&DAT_0065bd48,s_state_def_005da540);
  }
  iVar3 = 0;
  if (0 < param_1[0x2006]) {
    do {
      iVar1 = *(int *)(param_1[0x200a] + iVar3 * 4);
      if (iVar1 != 0) {
        *(undefined4 *)(iVar1 + 0xc) = 0;
        FUN_004922c0();
        FUN_004830f0(iVar1);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < param_1[0x2006]);
  }
  if ((undefined4 *)param_1[0x200a] != (undefined4 *)0x0) {
    puVar4 = (undefined4 *)param_1[0x200a];
    for (uVar2 = param_1[0x2008] & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined1 *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  piVar5 = param_1 + 0x2001;
  param_1[0x2006] = 0;
  iVar3 = 0;
  param_1[0x2007] = 0;
  if (0 < *piVar5) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)(param_1[0x2005] + iVar3 * 4), iVar1 != 0)) {
        FUN_004948e0_TScriptProto_Dtor();
        FUN_004830f0(iVar1);
      }
      FUN_0041cb40(iVar3);
      iVar3 = iVar3 + 1;
    } while (iVar3 < *piVar5);
  }
  *piVar5 = 0;
  param_1[0x2002] = 0;
  iVar3 = 0;
  if (0 < param_1[0x200b]) {
    do {
      if ((-1 < iVar3) && (iVar1 = *(int *)(param_1[0x200f] + iVar3 * 4), iVar1 != 0)) {
        FUN_004830f0(iVar1);
      }
      FUN_0041cb40(iVar3);
      iVar3 = iVar3 + 1;
    } while (iVar3 < param_1[0x200b]);
  }
  param_1[0x200b] = 0;
  iVar3 = 0;
  param_1[0x200c] = 0;
  if (0 < *param_1) {
    piVar5 = param_1 + 0x1001;
    do {
      if (*piVar5 != 0) {
        FUN_004830f0(*piVar5);
      }
      *piVar5 = 0;
      iVar3 = iVar3 + 1;
      piVar5 = piVar5 + 1;
    } while (iVar3 < *param_1);
  }
  *param_1 = 0;
  return;
}


