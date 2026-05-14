// FUN_00499750_TSector_SetWalkmap @ 00499750 size=46

void __thiscall FUN_00499750_TSector_SetWalkmap(int *param_1,int param_2,int param_3,ushort param_4)

{
  ushort uVar1;
  int iVar2;
  
  iVar2 = *param_1;
  if (iVar2 != 0) {
    param_2 = param_3 * 0x40 + param_2;
    uVar1 = *(ushort *)(iVar2 + param_2 * 2);
    *(ushort *)(iVar2 + param_2 * 2) = (param_4 ^ uVar1) & 0x3ff ^ uVar1;
  }
  return;
}


