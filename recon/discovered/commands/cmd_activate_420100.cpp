// FUN_00420100 @ 00420100 size=1

undefined4 FUN_00420100(int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  iVar2 = DAT_00667fcc;
  if (((param_4 != 0) && (iVar1 = *(int *)(param_4 + 0xc4), iVar1 != 0)) &&
     (*(short *)(iVar1 + 4) == 0xb)) {
    iVar2 = iVar1;
  }
  (**(code **)(*param_1 + 0x248))(iVar2,1);
  return 0;
}


