// FUN_004da380 @ 004da380 size=47

void __fastcall FUN_004da380(int param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 8;
  piVar2 = (int *)(param_1 + 0x104);
  do {
    if ((undefined4 *)*piVar2 != (undefined4 *)0x0) {
      (*(code *)**(undefined4 **)*piVar2)(1);
    }
    piVar2 = piVar2 + 5;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  piVar2 = (int *)(param_1 + 0x104);
  for (iVar1 = 0x28; iVar1 != 0; iVar1 = iVar1 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  return;
}


