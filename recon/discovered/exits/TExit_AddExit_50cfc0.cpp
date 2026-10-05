// TExit_AddExit @ 0x0050cfc0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// static; editor `exit` command
// FUN_0050cfc0 @ 0050cfc0 size=405

undefined4 FUN_0050cfc0(char *param_1,int *param_2,int param_3)

{
  int iVar1;
  char *pcVar2;
  int *piVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  int local_10;
  int local_c;
  undefined1 local_8 [4];
  undefined1 local_4 [4];
  
  piVar3 = param_2;
  pcVar2 = param_1;
  if (((param_1 == (char *)0x0) || (*param_1 == '\0')) ||
     (puVar5 = DAT_0066d1c4, param_2 == (int *)0x0)) {
    return 0;
  }
  do {
    if (puVar5 == (undefined4 *)0x0) {
LAB_0050d00e:
      puVar5 = (undefined4 *)FUN_00482fb0(0x24);
      uVar6 = FUN_0059b6bc(pcVar2);
      *puVar5 = uVar6;
      puVar5[8] = DAT_0066d1c4;
      DAT_0066d1c4 = puVar5;
LAB_0050d031:
      if ((short)piVar3[1] == 10) {
        iVar4 = FUN_0046e8a0();
        if (iVar4 == 0) {
          puVar5[1] = 0;
          puVar5[2] = 0;
          puVar5[3] = 0;
        }
        else {
          (**(code **)(*piVar3 + 0x27c))(&param_1,&param_2,local_4,&local_10,&local_c,local_8);
          puVar5[3] = 0;
          puVar5[1] = (local_10 * 0x10 + (int)param_1 * -0x20) / 2;
          puVar5[2] = (local_c * 0x10 + (int)param_2 * -0x20) / 2;
        }
        iVar4 = piVar3[5];
        iVar1 = piVar3[4];
        puVar5[3] = puVar5[3] + piVar3[6];
        puVar5[2] = puVar5[2] + iVar4;
        puVar5[1] = puVar5[1] + iVar1;
        puVar5[5] = piVar3[0x10];
        if (param_3 == 0) {
          puVar5[6] = 0xffffffff;
          *(undefined1 *)(puVar5 + 7) = 0xff;
          *(undefined1 *)((int)puVar5 + 0x1d) = 0xff;
          *(undefined1 *)((int)puVar5 + 0x1e) = 0xff;
        }
        else {
          puVar5[6] = DAT_006671a4;
          FUN_0041d7d0(puVar5 + 7);
        }
      }
      else {
        iVar4 = piVar3[5];
        iVar1 = piVar3[6];
        puVar5[1] = piVar3[4];
        puVar5[2] = iVar4;
        puVar5[3] = iVar1;
        puVar5[5] = 0xffffffff;
      }
      puVar5[4] = DAT_00666970;
      DAT_0066d24c = 1;
      return 1;
    }
    iVar4 = FUN_0059a530(*puVar5,pcVar2);
    if (iVar4 == 0) {
      if (puVar5 != (undefined4 *)0x0) goto LAB_0050d031;
      goto LAB_0050d00e;
    }
    puVar5 = (undefined4 *)puVar5[8];
  } while( true );
}


