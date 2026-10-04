// FUN_00425390 @ 00425390 size=135

undefined4 FUN_00425390(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iStack_18;
  int aiStack_14 [3];
  int aiStack_8 [2];
  
  iVar1 = FUN_0047a410(param_2,s__d__d__d__d__d_005cbdc4,aiStack_14 + 2,aiStack_8,&iStack_18,
                       aiStack_14,&param_2);
  if (((iVar1 != 0) && (aiStack_14[2] + 0x200 < iStack_18)) &&
     (aiStack_8[0] + 0x200 < aiStack_14[0])) {
    aiStack_14[1] = 0;
    aiStack_8[1] = 0;
    FUN_0045d660(aiStack_14 + 2,&iStack_18,param_2);
    return 0;
  }
  return 4;
}


