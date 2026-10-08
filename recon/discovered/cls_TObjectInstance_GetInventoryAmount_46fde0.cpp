// FUN_0046fde0 @ 0046fde0 size=192

/* WARNING: Removing unreachable block (ram,0x0046fe49) */
/* WARNING: Removing unreachable block (ram,0x0046fe4d) */
/* WARNING: Removing unreachable block (ram,0x0046fe5e) */
/* WARNING: Removing unreachable block (ram,0x0046fe7d) */
/* WARNING: Removing unreachable block (ram,0x0046fe6f) */
/* WARNING: Removing unreachable block (ram,0x0046fe82) */
/* WARNING: Removing unreachable block (ram,0x0046fe84) */

undefined4 __thiscall FUN_0046fde0(int *param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x170))();
  if (iVar1 == 0) {
    FUN_0046dfb0();
    return 0;
  }
  piVar2 = (int *)(**(code **)(*param_1 + 0x170))();
  uVar3 = (**(code **)(*piVar2 + 0x84))(param_2);
  return uVar3;
}


