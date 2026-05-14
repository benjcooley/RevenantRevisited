// FUN_004d61b0_DistanceTo @ 004d61b0 size=111

uint __thiscall FUN_004d61b0_DistanceTo(int *param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iStack_c = param_2[4];
  iStack_8 = param_2[5];
  iStack_4 = param_2[6];
  iVar1 = FUN_0046de60_Distance2D(param_1 + 4,&iStack_c);
  iVar2 = (**(code **)(*param_1 + 600))();
  uVar3 = iVar1 - iVar2;
  if (((short)param_2[1] == 0xc) || ((short)param_2[1] == 0xb)) {
    iVar1 = (**(code **)(*param_2 + 600))();
    uVar3 = uVar3 - iVar1;
  }
  return ((int)uVar3 < 0) - 1 & uVar3;
}


