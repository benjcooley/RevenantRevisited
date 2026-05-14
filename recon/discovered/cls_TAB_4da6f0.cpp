// FUN_004da6f0 @ 004da6f0 size=81

void __fastcall FUN_004da6f0(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  piVar2 = (int *)(param_1 + 0x104);
  iVar3 = 8;
  do {
    if ((*piVar2 != 0) && (0 < piVar2[1])) {
      iVar1 = piVar2[1] + -1;
      piVar2[1] = iVar1;
      if (iVar1 < 1) {
        if ((undefined4 *)*piVar2 != (undefined4 *)0x0) {
          (*(code *)**(undefined4 **)*piVar2)(1);
        }
        *piVar2 = 0;
        piVar2[1] = 0;
        piVar2[2] = 0;
        piVar2[3] = 0;
        piVar2[4] = 0;
      }
      else {
        piVar2[4] = piVar2[4] + 400;
      }
    }
    piVar2 = piVar2 + 5;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}


