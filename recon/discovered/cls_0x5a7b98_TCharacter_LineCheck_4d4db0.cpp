// FUN_004d4db0_FindCharInLine @ 004d4db0 size=253

int * FUN_004d4db0_FindCharInLine(int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int *local_3c;
  
  FUN_0044ceb0(param_2,0x80,0xe0,2,0,*(undefined2 *)((int)param_1 + 0xe));
  do {
    if (local_3c == (int *)0x0) {
      return (int *)0x0;
    }
    if ((((local_3c != (int *)0x0) && (local_3c != param_1)) &&
        (iVar1 = (**(code **)(*local_3c + 0x1c0))(), 0 < iVar1)) &&
       ((*(int *)(local_3c[0x36] + 0x48) == 0 ||
        ((*(uint *)(*(int *)(local_3c[0x36] + 0x48) + 0x24) & 0x2000000) == 0)))) {
      if ((int *)local_3c[0x15] == (int *)0x0) {
        uVar2 = 0;
      }
      else {
        uVar2 = (**(code **)(*(int *)local_3c[0x15] + 0x8c))((short)local_3c[3]);
      }
      if ((((uVar2 & 0x800) == 0) && ((*(byte *)(local_3c + 2) & 0x80) == 0)) &&
         (local_3c[0x3b] == 0)) {
        iVar1 = FUN_0046de60_Distance2D(param_2,local_3c + 4);
        iVar3 = (**(code **)(*local_3c + 600))();
        if ((iVar1 - param_4 == iVar3 || (iVar1 - param_4) - iVar3 < 0) &&
           (((short)local_3c[1] != 0xb || ((*(byte *)(local_3c + 0xdb) & 2) == 0)))) {
          return local_3c;
        }
      }
    }
    FUN_0044d080_TMapIterator_NextItem();
  } while( true );
}


