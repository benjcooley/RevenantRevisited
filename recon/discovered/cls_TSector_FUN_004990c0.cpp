// FUN_004990c0_TSector_SetObject @ 004990c0 size=218

undefined4 __thiscall FUN_004990c0_TSector_SetObject(int param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 *puVar4;
  int iVar5;
  
  uVar2 = FUN_0041c910(param_2,param_3);
  if (param_2 == 0) {
    return uVar2;
  }
  uVar1 = *(undefined4 *)(param_1 + 4);
  *(int *)(param_2 + 0x44) = param_1;
  *(short *)(param_2 + 0xe) = (short)uVar1;
  iVar5 = 1;
  do {
    switch(iVar5) {
    case 0:
      goto switchD_00499102_caseD_0;
    case 1:
      uVar3 = *(uint *)(param_2 + 8) & 8;
      break;
    case 2:
      if ((*(short *)(param_2 + 4) == 0xc) || (*(short *)(param_2 + 4) == 0xb))
      goto switchD_00499102_caseD_0;
      goto switchD_00499102_default;
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
      goto switchD_00499102_default;
    }
    if (uVar3 != 0) {
switchD_00499102_caseD_0:
      uVar1 = *(undefined4 *)(param_1 + 0xb4 + iVar5 * 0x18);
      puVar4 = (undefined4 *)FUN_00482fb0(4);
      *puVar4 = uVar2;
      FUN_0041c910(puVar4,uVar1);
    }
switchD_00499102_default:
    iVar5 = iVar5 + 1;
    if (6 < iVar5) {
      return uVar2;
    }
  } while( true );
}


