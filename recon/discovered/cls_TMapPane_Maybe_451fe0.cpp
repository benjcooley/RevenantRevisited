// FUN_00451fe0 @ 00451fe0 size=124

void __thiscall FUN_00451fe0(int param_1,undefined4 param_2,int param_3)

{
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (param_3 == 0) {
    local_4 = *(undefined4 *)(param_1 + 0xb8);
    local_c = *(undefined4 *)(param_1 + 0xb0);
    local_8 = *(undefined4 *)(param_1 + 0xb4);
  }
  else {
    local_c = *(undefined4 *)(param_3 + 0x10);
    local_8 = *(undefined4 *)(param_3 + 0x14);
    local_4 = *(undefined4 *)(param_3 + 0x18);
  }
  FUN_0045f6c0(&local_c);
  FUN_00451de0(param_2);
  return;
}


