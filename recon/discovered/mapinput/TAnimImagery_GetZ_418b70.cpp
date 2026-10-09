// FUN_00418b70 @ 00418b70 size=240

undefined4 __thiscall FUN_00418b70(int *param_1,int *param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int unaff_EBX;
  int unaff_ESI;
  uint uVar6;
  int iVar7;
  
  uVar6 = (uint)*(ushort *)(param_2 + 3);
  iVar1 = (**(code **)(*param_1 + 0x3c))();
  if (iVar1 <= (int)uVar6) {
    return 0;
  }
  iVar7 = -1;
  iVar1 = (**(code **)(*param_2 + 300))(0xffffffff);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_0046ec50(&stack0xfffffff0);
  uVar2 = (**(code **)(*param_1 + 0xec))(uVar6);
  if ((uVar2 & 1) == 0) {
    if ((uVar2 & 2) == 0) {
      return DAT_00668154;
    }
  }
  else if ((*(byte *)(iVar1 + 0x10) & 0x20) == 0) {
    return 1;
  }
  if ((*(uint *)(iVar1 + 0x10) & 0x20) == 0) {
    if ((*(uint *)(iVar1 + 0x10) & 0x100) != 0) {
      return DAT_00668154;
    }
    uVar3 = 0x800100;
  }
  else {
    uVar3 = 0x500;
  }
  iVar1 = (**(code **)(*param_1 + 0x6c))(uVar6,iVar1,uVar3);
  iVar1 = unaff_EBX - iVar1;
  uVar2 = uVar6;
  iVar4 = (**(code **)(*param_1 + 0x68))(uVar6,iVar1);
  iVar4 = unaff_ESI - iVar4;
  iVar5 = (**(code **)(*param_1 + 100))(uVar6,iVar4);
  uVar3 = FUN_004bddc0(iVar7 - iVar5,uVar6,iVar4,uVar2,iVar1);
  return uVar3;
}


