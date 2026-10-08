// FUN_0046e1f0 @ 0046e1f0 size=505

int * __thiscall FUN_0046e1f0(int *param_1,int *param_2)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  int iVar8;
  void *unaff_EBX;
  int *piVar9;
  bool bVar10;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d269;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_0041c7f0(0,4);
  *(short *)(param_1 + 0x2a) = 0;
  *(undefined2 *)((int)param_1 + 0xaa) = 0;
  param_1[0x2b] = 0;
  local_4 = 1;
  *param_1 = (int)&PTR_FUN_005a50e8;
  FUN_0046e0f0();
  iVar3 = *param_1;
  piVar9 = param_2;
  piVar4 = param_1;
  for (iVar8 = 0xd; piVar4 = piVar4 + 1, iVar8 != 0; iVar8 = iVar8 + -1) {
    *piVar4 = *piVar9;
    piVar9 = piVar9 + 1;
  }
  DAT_006669fc = DAT_006669fc | 8;
  (**(code **)(iVar3 + 0x40))(param_1[2] | 0x40000);
  bVar10 = (uint)(int)(short)param_1[1] < DAT_0065a258;
  param_1[0x15] = (int)param_2;
  if (bVar10) {
    iVar3 = (&DAT_0065a148)[(short)param_1[1]];
  }
  else {
    iVar3 = 0;
  }
  param_1[0x12] = iVar3;
  if (iVar3 == 0) {
    FUN_00481c10(s_Bad_object_class__005d47fc,0);
  }
  iVar3 = param_1[0x12];
  sVar1 = *(short *)((int)param_1 + 6);
  iVar8 = FUN_00410160((int)sVar1);
  piVar4 = (int *)0x0;
  if ((iVar8 != 0) && (piVar4 = *(int **)(*(int *)(iVar3 + 0x34) + sVar1 * 4), piVar4 == (int *)0x0)
     ) {
    piVar4 = *(int **)(iVar3 + 0x38);
  }
  iVar3 = *piVar4;
  param_1[0x13] = (int)piVar4;
  param_1[0xe] = iVar3;
  sVar1 = *(short *)(param_1[0x12] + 0x1c);
  if (0 < sVar1) {
    FUN_004785e0((int)sVar1);
    *(short *)(param_1 + 0x2a) = sVar1;
    iVar3 = 0;
    if (0 < *(short *)(param_1[0x12] + 0x1c)) {
      do {
        iVar8 = iVar3 * 4;
        iVar5 = FUN_0044ce10((int)*(short *)((int)param_1 + 6));
        iVar3 = iVar3 + 1;
        iVar2 = param_1[0x12];
        *(undefined4 *)(iVar8 + param_1[0x2b]) = *(undefined4 *)(*(int *)(iVar5 + 0x18) + iVar8);
      } while (iVar3 < *(short *)(iVar2 + 0x1c));
    }
  }
  sVar1 = (short)param_1[1];
  switch(sVar1) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 6:
  case 7:
  case 8:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x1a:
    uVar6 = param_1[2] | 0x4000c000;
    break;
  default:
    goto switchD_0046e343_caseD_5;
  case 0x19:
    uVar6 = param_1[2] | 0xc000;
  }
  param_1[2] = uVar6;
switchD_0046e343_caseD_5:
  if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
    (**(code **)(*param_1 + 0x40))(param_1[2] | 0x4000000);
  }
  iVar3 = (**(code **)(*(int *)param_1[0x15] + 0x38))(param_1);
  if (iVar3 != 0) {
    param_1[2] = param_1[2] | 0xc000;
  }
  if ((short)param_1[1] != 9) {
    param_1[2] = param_1[2] | 0x8000;
  }
  if (((param_1[2] & 0x40000000U) == 0) &&
     ((((short)param_1[1] != 9 || (param_1[0xe] != *(int *)param_1[0x13])) &&
      ((param_1[2] & 0x8000000U) == 0)))) {
    uVar7 = FUN_00497370(param_1);
    FUN_00471150(uVar7);
  }
  ExceptionList = unaff_EBX;
  return param_1;
}


