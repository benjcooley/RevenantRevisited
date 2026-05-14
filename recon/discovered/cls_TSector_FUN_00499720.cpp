// FUN_00499720_TSector_ReturnWalkmap @ 00499720 size=43

ushort __thiscall FUN_00499720_TSector_ReturnWalkmap(int *param_1,int param_2,int param_3)

{
  if (*param_1 != 0) {
    return *(ushort *)(*param_1 + (param_3 * 0x40 + param_2) * 2) & 0x3ff;
  }
  return 0;
}


