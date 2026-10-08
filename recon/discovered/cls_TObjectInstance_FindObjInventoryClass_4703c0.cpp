// FUN_004703c0 @ 004703c0 size=190

/* WARNING: Removing unreachable block (ram,0x0047042e) */
/* WARNING: Removing unreachable block (ram,0x00470436) */
/* WARNING: Removing unreachable block (ram,0x0047043e) */
/* WARNING: Removing unreachable block (ram,0x00470442) */
/* WARNING: Removing unreachable block (ram,0x00470471) */
/* WARNING: Removing unreachable block (ram,0x00470455) */

undefined4 __thiscall FUN_004703c0(int *param_1,undefined4 param_2,undefined4 param_3)

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
  uVar3 = (**(code **)(*piVar2 + 0xa4))(param_2,param_3);
  return uVar3;
}


