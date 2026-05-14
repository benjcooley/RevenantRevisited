// FUN_00499710_TSector_ClearWalkmap @ 00499710 size=16

void __fastcall FUN_00499710_TSector_ClearWalkmap(undefined4 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)*param_1;
  for (iVar1 = 0x800; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return;
}


