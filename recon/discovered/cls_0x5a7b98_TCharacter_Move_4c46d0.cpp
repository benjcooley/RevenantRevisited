// FUN_004c46d0_Move @ 004c46d0 size=249

uint __fastcall FUN_004c46d0_Move(int param_1)

{
  uint *puVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  
  if ((((DAT_0066829c == 0) || (*(short *)(param_1 + 4) != 0xb)) || (DAT_0067682c == 0)) ||
     ((param_1 == DAT_00667fcc || (*(int *)(param_1 + 0xec) == 0)))) {
    bVar2 = false;
  }
  else {
    bVar2 = true;
  }
  iVar4 = 10;
  while( true ) {
    uVar3 = FUN_004c3bc0_MoveStep();
    iVar4 = iVar4 + -1;
    if (*(int *)(param_1 + 0xec) == 0) break;
    if ((((*(int *)(param_1 + 0x10) == *(int *)(param_1 + 0xf0)) &&
         (*(int *)(param_1 + 0x14) == *(int *)(param_1 + 0xf4))) &&
        (*(int *)(param_1 + 0x18) == *(int *)(param_1 + 0xf8))) ||
       (((uVar3 & 1) == 0 || (iVar4 < 1)))) break;
  }
  if (DAT_0066829c != 0) {
    if ((bVar2) &&
       ((*(int *)(param_1 + 0x10) != *(int *)(param_1 + 0xf0) ||
        (*(int *)(param_1 + 0x14) != *(int *)(param_1 + 0xf4))))) {
      FUN_00583de0(param_1,1);
      uVar3 = uVar3 & 0xfffffffe | 2;
    }
    if (((uVar3 & 2) != 0) && ((*(uint *)(*(int *)(param_1 + 0xd8) + 0x60) & 0x2000) == 0)) {
      FUN_00583de0(param_1,0);
      puVar1 = (uint *)(*(int *)(param_1 + 0xd8) + 0x60);
      *puVar1 = *puVar1 | 0x2000;
    }
  }
  *(undefined4 *)(param_1 + 0xec) = 0;
  return uVar3;
}


