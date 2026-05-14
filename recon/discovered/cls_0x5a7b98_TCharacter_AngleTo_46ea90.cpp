// FUN_0046ea90_AngleTo @ 0046ea90 size=60

void __thiscall FUN_0046ea90_AngleTo(int param_1,int param_2)

{
  int local_c;
  int local_8;
  undefined4 local_4;
  
  local_4 = 0;
  local_c = *(int *)(param_2 + 0x10) - *(int *)(param_1 + 0x10);
  local_8 = *(int *)(param_2 + 0x14) - *(int *)(param_1 + 0x14);
  FUN_0046dbe0_AngleFromDelta(&local_c);
  return;
}


