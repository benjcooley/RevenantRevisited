// FUN_004bd8c0 @ 004bd8c0 size=162

void __thiscall
FUN_004bd8c0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4,uint param_5,
            undefined4 param_6)

{
  uint local_54 [11];
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if (param_4 == (undefined4 *)0x0) {
    return;
  }
  local_54[1] = 0;
  local_54[2] = 0;
  local_54[3] = 0;
  local_1c = 0;
  local_18 = 0;
  local_2 = 0;
  local_4 = 0;
  local_54[5] = 0;
  local_54[4] = 0;
  local_54[9] = 0;
  local_54[8] = 0;
  local_54[7] = 0;
  local_54[6] = 0;
  local_c = param_6;
  local_20 = param_4[1];
  local_24 = *param_4;
  local_54[0] = param_5 | 0x100000;
  local_54[10] = param_2;
  local_28 = param_3;
  local_8 = 0x1f;
  local_14 = local_24;
  local_10 = local_20;
  (**(code **)(*param_1 + 0x58))(local_54,param_4);
  return;
}


