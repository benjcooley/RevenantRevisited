// FUN_0052bdb0 @ 0052bdb0 size=329

void __fastcall FUN_0052bdb0(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  uVar2 = DAT_0065b500;
  uVar1 = DAT_0065b4fc;
  iVar4 = DAT_0065b4f8;
  iVar3 = DAT_0065b4f4;
  FUN_00412250(&local_10,&local_c,&local_8,&local_4);
  local_8 = local_8 + -1 + local_10;
  local_4 = local_4 + -1 + local_c;
  FUN_00412150(iVar3,iVar4,uVar1,uVar2);
  FUN_00414d70(iVar3,iVar4,0,*(undefined4 *)(param_1 + 0x7c),0,uVar1,uVar2,0xffffffff,0,0,uVar1,
               uVar2,0,0);
  if (*(int *)(param_1 + 0x9c) != 0) {
    uVar1 = (*(undefined4 **)(param_1 + 0x94))[1];
    uVar2 = **(undefined4 **)(param_1 + 0x94);
    FUN_00414d70(iVar3 + 8,iVar4 + 3,0,*(undefined4 *)(param_1 + 0x84),0,uVar2,uVar1,0xffffffff,0,0,
                 uVar2,uVar1,0,0);
  }
  if (*(int *)(param_1 + 0xa0) != 0) {
    uVar1 = (*(undefined4 **)(param_1 + 0x98))[1];
    uVar2 = **(undefined4 **)(param_1 + 0x98);
    FUN_00414d70(iVar3 + 0x90,iVar4 + 3,0,*(undefined4 *)(param_1 + 0x88),0,uVar2,uVar1,0xffffffff,0
                 ,0,uVar2,uVar1,0,0);
  }
  FUN_00412150(local_10,local_c,(local_8 - local_10) + 1,(local_4 - local_c) + 1);
  return;
}


