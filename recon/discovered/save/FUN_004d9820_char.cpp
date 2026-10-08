// FUN_004d9820 @ 004d9820 size=337

void __fastcall FUN_004d9820(int param_1)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  if ((*(int *)(param_1 + 0x380) != 0) && (*(int *)(param_1 + 0x350) != 0)) {
    fVar1 = *(float *)(param_1 + 0x390) - *(float *)(param_1 + 900);
    fVar2 = *(float *)(param_1 + 0x394) - *(float *)(param_1 + 0x388);
    fVar3 = *(float *)(param_1 + 0x398) - *(float *)(param_1 + 0x38c);
    if ((fVar2 < fVar1) && (fVar3 < fVar1)) {
      if (*(int *)(param_1 + 0x3b8) == 0) {
        *(undefined4 *)(param_1 + 0x364) = 0;
      }
      else {
        *(undefined4 *)(param_1 + 0x364) = *(undefined4 *)(param_1 + 900);
      }
      *(undefined4 *)(param_1 + 0x368) = 0;
      *(undefined4 *)(param_1 + 0x36c) = 0;
      *(undefined4 *)(param_1 + 0x370) = *(undefined4 *)(param_1 + 0x390);
      *(undefined4 *)(param_1 + 0x374) = 0;
      *(undefined4 *)(param_1 + 0x378) = 0;
      return;
    }
    if ((fVar1 < fVar2) && (fVar3 < fVar2)) {
      *(undefined4 *)(param_1 + 0x364) = 0;
      if (*(int *)(param_1 + 0x3b8) == 0) {
        *(undefined4 *)(param_1 + 0x368) = 0;
      }
      else {
        *(undefined4 *)(param_1 + 0x368) = *(undefined4 *)(param_1 + 0x388);
      }
      *(undefined4 *)(param_1 + 0x36c) = 0;
      *(undefined4 *)(param_1 + 0x370) = 0;
      *(undefined4 *)(param_1 + 0x374) = *(undefined4 *)(param_1 + 0x388);
      *(undefined4 *)(param_1 + 0x378) = 0;
      return;
    }
    *(undefined4 *)(param_1 + 0x364) = 0;
    *(undefined4 *)(param_1 + 0x368) = 0;
    if (*(int *)(param_1 + 0x3b8) == 0) {
      *(undefined4 *)(param_1 + 0x36c) = 0;
    }
    else {
      *(undefined4 *)(param_1 + 0x36c) = *(undefined4 *)(param_1 + 0x38c);
    }
    *(undefined4 *)(param_1 + 0x370) = 0;
    *(undefined4 *)(param_1 + 0x374) = 0;
    *(undefined4 *)(param_1 + 0x378) = *(undefined4 *)(param_1 + 0x398);
  }
  return;
}


