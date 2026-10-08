// FUN_00477b30 @ 00477b30 size=80

void __fastcall FUN_00477b30(int *param_1)

{
  undefined1 auStack_10 [12];
  undefined4 *puStack_4;
  
  if ((*(byte *)(param_1 + 2) & 4) == 0) {
    (**(code **)(*param_1 + 0xf4))(auStack_10);
  }
  else {
    (**(code **)(*(int *)param_1[0x15] + 0xa8))(param_1,auStack_10);
  }
  FUN_0041c720(*puStack_4,puStack_4[1]);
  return;
}


