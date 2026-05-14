// FUN_0041b690 @ 0041b690 size=223

void __fastcall FUN_0041b690(int param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_1 + 0x44);
  if ((uVar1 & 0x100) != 0) {
    if (((uVar1 & 0x8000) == 0) || (DAT_006671f0 == 1)) {
      if (((uVar1 & 0x10000) == 0) || (DAT_006671f0 == 2)) {
        if (((uVar1 & 0x20000) == 0) || (DAT_006671f0 == 3)) {
          if (((uVar1 & 0x40000) != 0) && (DAT_006671f0 != 4)) {
            (**(code **)(DAT_006668d8 + 0x2c))(1);
            DAT_006671f0 = 4;
          }
        }
        else {
          (**(code **)(DAT_006668d8 + 0x2c))(1);
          DAT_006671f0 = 3;
        }
      }
      else {
        (**(code **)(DAT_006668d8 + 0x2c))(1);
        DAT_006671f0 = 2;
      }
    }
    else {
      (**(code **)(DAT_006668d8 + 0x2c))(1);
      DAT_006671f0 = 1;
    }
  }
  if (((*(uint *)(param_1 + 0x44) & 0x100) == 0) && (DAT_006671f0 != 0)) {
    (**(code **)(DAT_006668d8 + 0x2c))(1);
    DAT_006671f0 = 0;
  }
  return;
}


