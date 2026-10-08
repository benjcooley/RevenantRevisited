// FUN_00426b20 @ 00426b20 size=188

undefined4 FUN_00426b20(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar3 = DAT_0066698c;
  uVar2 = DAT_00666988;
  uStack_4 = 2;
  uStack_8 = 2;
  iVar1 = FUN_0047a410(param_2,s__d__d_005cc628,&uStack_4,&uStack_8);
  if (iVar1 == 0) {
    if ((uVar2 & 0x3ff) < 0x100) {
      uVar2 = uVar2 - 0x100;
    }
    else if ((uVar2 & 0x3ff) < 0x300) {
      uStack_4 = 1;
    }
    else {
      uVar2 = uVar2 + 0x100;
    }
    if ((uVar3 & 0x3ff) < 0x100) {
      uVar3 = uVar3 - 0x100;
    }
    else if ((uVar3 & 0x3ff) < 0x300) {
      uStack_8 = 1;
    }
    else {
      uVar3 = uVar3 + 0x100;
    }
  }
  FUN_0047f2d0((int)uVar2 >> 10,(int)uVar3 >> 10,uStack_4,uStack_8);
  return 0;
}


