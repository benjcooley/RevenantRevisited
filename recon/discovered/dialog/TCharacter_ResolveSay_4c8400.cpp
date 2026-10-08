// FUN_004c8400 @ 004c8400 size=105

undefined4 __thiscall FUN_004c8400(int *param_1,int param_2)

{
  byte bVar1;
  
  if ((0 < *(int *)(param_2 + 0x28)) &&
     ((param_1[0x36] == 0 || ((*(uint *)(param_1[0x36] + 0x60) & 0x100) == 0)))) {
    return 2;
  }
  bVar1 = *(byte *)(param_1 + 0x44);
  *(undefined4 *)(param_2 + 0x28) = 0;
  if ((bVar1 & 2) != 0) {
    (**(code **)(*param_1 + 0x218))(param_1[0x38],0,1);
    return 0;
  }
  (**(code **)(*param_1 + 0x218))(param_1[0x38],0,0);
  return 0;
}


