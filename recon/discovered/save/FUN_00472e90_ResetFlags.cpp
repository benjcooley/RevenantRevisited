// FUN_00472e90 @ 00472e90 size=238

void __thiscall FUN_00472e90(int *param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  if (((param_2 & 0x20000000) == 0) || ((param_1[2] & 0x20000000U) != 0)) {
    if (((param_2 & 0x200) != 0) && ((param_1[2] & 0x200U) == 0)) {
      param_2 = param_2 & 0xdfffffff;
    }
  }
  else {
    param_2 = param_2 & 0xfffffdff;
  }
  uVar1 = param_1[2];
  param_1[2] = param_2;
  if (param_1[0x11] != 0) {
    FUN_00451a50(param_1,uVar1,param_2);
  }
  if (param_1[0x11] != 0) {
    if (((uVar1 & 0x400000) == 0) && ((param_1[2] & 0x400000U) != 0)) {
      uVar2 = 3;
    }
    else {
      if (((uVar1 & 0x400000) == 0) || ((param_1[2] & 0x400000U) != 0)) goto LAB_00472f0e;
      uVar2 = 0;
    }
    FUN_00452750(param_1,uVar2,0);
  }
LAB_00472f0e:
  if (((uVar1 & 4) == 0) && ((*(byte *)(param_1 + 2) & 4) != 0)) {
    (**(code **)(*param_1 + 0x124))();
  }
  else if (((uVar1 & 4) != 0) && ((*(byte *)(param_1 + 2) & 4) == 0)) {
    (**(code **)(*param_1 + 0x128))();
  }
  if ((DAT_0066829c != 0) && ((uVar1 & 0x2800080) != (param_1[2] & 0x2800080U))) {
    FUN_00583fe0(param_1,0x46,param_1[2] & 0x2800080U,0,
                 (-(uint)(DAT_0067682c != 0) & 0xffffffdb) + 0x30);
  }
  return;
}


