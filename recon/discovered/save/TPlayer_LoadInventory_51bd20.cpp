// FUN_0051bd20 @ 0051bd20 size=150

void __thiscall FUN_0051bd20(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  ushort uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  FUN_00472310(param_2,param_3,param_4);
  uVar1 = *(ushort *)(param_1 + 0x2a);
  if (uVar1 < 4) {
    uVar3 = 0;
  }
  else {
    uVar3 = *(undefined4 *)(param_1[0x2b] + 0xc);
  }
  if (uVar1 < 6) {
    uVar2 = 0;
  }
  else {
    uVar2 = *(undefined4 *)(param_1[0x2b] + 0x14);
  }
  if (uVar1 < 5) {
    uVar4 = 0;
  }
  else {
    uVar4 = *(undefined4 *)(param_1[0x2b] + 0x10);
  }
  FUN_0051c660();
  FUN_00519230();
  FUN_0051c660();
  (**(code **)(*param_1 + 0x1c4))(uVar3);
  (**(code **)(*param_1 + 0x1d4))(uVar2);
  (**(code **)(*param_1 + 0x1cc))(uVar4);
  return;
}


