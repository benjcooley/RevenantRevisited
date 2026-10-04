// FUN_0047c580 @ 0047c580 size=174

void __thiscall FUN_0047c580(int *param_1,int param_2)

{
  param_1[0x178] = param_2;
  if (param_2 != 0) {
    param_1[0x176] = 0;
    DAT_00666924 = (uint)(param_2 == 0);
    return;
  }
  (**(code **)(*param_1 + 0x28))(5,1,1);
  (**(code **)(*param_1 + 0x30))(0x26,0);
  (**(code **)(*param_1 + 0x30))(0x25,0);
  (**(code **)(*param_1 + 0x30))(0x27,0);
  (**(code **)(*param_1 + 0x30))(0x28,0);
  (**(code **)(*param_1 + 0x30))(0x52,0);
  (**(code **)(*param_1 + 0x30))(0x22,0);
  (**(code **)(*param_1 + 0x30))(0x21,0);
  (**(code **)(*param_1 + 0x30))(0x24,0);
  (**(code **)(*param_1 + 0x30))(0x23,0);
  DAT_00666924 = 1;
  return;
}


