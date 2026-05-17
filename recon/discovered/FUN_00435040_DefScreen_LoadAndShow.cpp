// FUN_00435040_DefScreen_LoadAndShow @ 00435040 size=264

undefined4 __thiscall FUN_00435040_DefScreen_LoadAndShow(int *param_1)

{
  char cVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  int in_stack_00000010;
  int in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  int in_stack_00000020;
  int in_stack_00000024;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  char acStack_12 [6];
  char *pcStack_c;
  char *pcStack_8;
  
  param_1[0x2c] = in_stack_00000020;
  param_1[6] = in_stack_00000014;
  param_1[0x2d] = in_stack_00000024;
  param_1[5] = in_stack_00000010;
  uStack_5c = in_stack_0000001c;
  uStack_60 = in_stack_00000018;
  (**(code **)(*param_1 + 0x20))();
  uVar2 = (**(code **)(*param_1 + 0xa0))(s_background_005cd8d4,0xffffffff);
  FUN_00435660(uVar2);
  FUN_00435380();
  if (pcStack_8 != (char *)0x0) {
    _strncpy((char *)(param_1 + 0x56),pcStack_8,0x1f);
    *(undefined1 *)((int)param_1 + 0x177) = 0;
  }
  if (pcStack_c != (char *)0x0) {
    _strncpy((char *)&uStack_60,pcStack_c,0x4f);
    acStack_12[1] = 0;
    puVar3 = (undefined1 *)FUN_0058ade0(&uStack_60,0x2e);
    if (puVar3 != (undefined1 *)0x0) {
      *puVar3 = 0;
    }
    uVar5 = 0xffffffff;
    pcVar6 = (char *)&uStack_60;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar5 - 1);
    _strncpy(&stack0xffffff9f + ~uVar5,&DAT_005cd8e0,iVar4 + 0x4f);
    (&stack0xffffff9f + ~uVar5)[iVar4 + 0x4f] = '\0';
    iVar4 = FUN_004377c0_DefWidget_LoadFile(&uStack_60);
    if (iVar4 == 0) {
      return 0;
    }
  }
  (**(code **)(*param_1 + 0x2c))(1);
  return 1;
}


