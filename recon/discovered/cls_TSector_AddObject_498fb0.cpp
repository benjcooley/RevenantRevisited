// FUN_00498fb0_TSector_AddObject @ 00498fb0 size=229

undefined4 __thiscall FUN_00498fb0_TSector_AddObject(int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 *puVar4;
  int iVar5;
  
  if (param_2 == 0) {
    return 0xffffffff;
  }
  uVar2 = *(undefined4 *)(param_1 + 4);
  *(int *)(param_2 + 0x44) = param_1;
  *(short *)(param_2 + 0xe) = (short)uVar2;
  if (param_3 < 0) {
    uVar2 = FUN_0041c840(param_2);
  }
  else {
    uVar2 = FUN_0041c910(param_2,param_3);
  }
  iVar5 = 1;
  do {
    switch(iVar5) {
    case 0:
      goto switchD_00499007_caseD_0;
    case 1:
      uVar3 = *(uint *)(param_2 + 8) & 8;
      break;
    case 2:
      if ((*(short *)(param_2 + 4) == 0xc) || (*(short *)(param_2 + 4) == 0xb))
      goto switchD_00499007_caseD_0;
      goto switchD_00499007_default;
    case 3:
      uVar3 = *(uint *)(param_2 + 8) & 4;
      break;
    case 4:
      uVar3 = *(uint *)(param_2 + 8) & 0x8000;
      break;
    case 5:
      uVar3 = *(uint *)(param_2 + 8) & 0x4000;
      break;
    case 6:
      uVar3 = *(uint *)(param_2 + 8) & 0x40000;
      break;
    default:
      goto switchD_00499007_default;
    }
    if (uVar3 != 0) {
switchD_00499007_caseD_0:
      uVar1 = *(undefined4 *)(param_1 + 0xb4 + iVar5 * 0x18);
      puVar4 = (undefined4 *)FUN_00482fb0(4);
      *puVar4 = uVar2;
      FUN_0041c910(puVar4,uVar1);
    }
switchD_00499007_default:
    iVar5 = iVar5 + 1;
    if (6 < iVar5) {
      return uVar2;
    }
  } while( true );
}


