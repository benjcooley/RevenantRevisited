// FUN_00422dd0 @ 00422dd0 size=332

undefined4 FUN_00422dd0(int param_1)

{
  int iVar1;
  int iVar2;
  int iStack_11c;
  undefined4 *puStack_118;
  int iStack_114;
  int iStack_110;
  int iStack_10c;
  int iStack_108;
  int iStack_104;
  undefined4 auStack_100 [64];
  
  iStack_104 = FUN_0049dfa0(**(undefined4 **)(param_1 + 0x4c));
  if (iStack_104 < 0) {
    FUN_0041ee50(s_Max_number_of_templates_reached_005cb5c0);
    return 0;
  }
  iStack_10c = *(int *)(param_1 + 0x14);
  iStack_110 = *(int *)(param_1 + 0x10);
  iStack_108 = *(int *)(param_1 + 0x18);
  iVar1 = FUN_00452060(&iStack_110,*(undefined2 *)(param_1 + 0xe),auStack_100,0x3200,0x3200,9,0x40,0
                      );
  iStack_11c = 0;
  iStack_114 = 0;
  if (0 < iVar1) {
    puStack_118 = auStack_100;
    do {
      iVar2 = FUN_00452690(*puStack_118,0);
      if ((iVar2 != 0) && (iVar2 != param_1)) {
        iVar2 = FUN_0049e040(iStack_104,iVar2,*(int *)(iVar2 + 0x10) - iStack_110,
                             *(int *)(iVar2 + 0x14) - iStack_10c,*(int *)(iVar2 + 0x18) - iStack_108
                            );
        if (iVar2 < 0) {
          FUN_0041ee50(s_Too_many_objects_nearby__the_las_005cb5e4,iVar1 - iStack_114);
          break;
        }
        iStack_11c = iStack_11c + 1;
      }
      iStack_114 = iStack_114 + 1;
      puStack_118 = puStack_118 + 1;
    } while (iStack_114 < iVar1);
  }
  FUN_0041ee50(s__d_objects_added_to_template_for_005cb61c,iStack_11c,
               **(undefined4 **)(param_1 + 0x4c));
  return 0;
}


