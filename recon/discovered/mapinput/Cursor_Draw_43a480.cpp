// FUN_0043a480 @ 0043a480 size=283

void FUN_0043a480(void)

{
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  if (DAT_00668510 < 0x280) {
    if (DAT_00668510 < 1) {
      DAT_00668510 = 1;
    }
  }
  else {
    DAT_00668510 = 0x280;
  }
  if (DAT_00668514 < 0x1e0) {
    if (DAT_00668514 < 1) {
      DAT_00668514 = 1;
    }
  }
  else {
    DAT_00668514 = 0x1e0;
  }
  if (DAT_006563c8 != 0) {
    FUN_004bd680(DAT_00668510 - DAT_006563a8,DAT_00668514 - DAT_006563ac,DAT_006563c8,0x20100,0);
  }
  if (DAT_006563b4 == 0) {
LAB_0043a542:
    if (DAT_006563c4 == 0) goto LAB_0043a560;
  }
  else if (DAT_006563c4 == 0) {
    FUN_004bd680(DAT_00668510,DAT_00668514,DAT_006563b4,0x21100,0);
    goto LAB_0043a542;
  }
  FUN_004bd680(DAT_00668510,DAT_00668514,DAT_006563c4,0x21100,0);
LAB_0043a560:
  if (DAT_006563d0 != 0) {
    FUN_0043a240(0,0,0);
    DAT_006563c8 = 0;
    DAT_006563a8 = 0;
    DAT_006563ac = 0;
    DAT_006563d0 = 0;
  }
  DAT_006563c4 = 0;
  DAT_006563b0 = 0;
  return;
}


