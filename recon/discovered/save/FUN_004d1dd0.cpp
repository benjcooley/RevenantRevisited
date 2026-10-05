// FUN_004d1dd0 @ 004d1dd0 size=219

undefined4 __thiscall
FUN_004d1dd0(int *param_1,undefined4 param_2,undefined4 param_3,int *param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,undefined4 param_8,int param_9,int param_10)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 local_8;
  int local_4;
  
  if (param_10 == 0) {
    local_8 = 10000;
  }
  else {
    local_8 = (**(code **)(*param_1 + 4))(param_10);
  }
  local_4 = 0;
  do {
    if (local_4 == 0) {
      uVar3 = 2;
    }
    else {
      uVar3 = (uint)(local_4 == 1);
    }
    if (param_9 != 0) {
      uVar3 = uVar3 | 0x40000000;
    }
    iVar2 = 0;
    if (0 < *(int *)(param_1[0x3f] + 0xcc)) {
      do {
        iVar1 = FUN_004d1120(iVar2,param_5,param_6,param_7,param_8,local_8,param_2,0,param_3,3,uVar3
                             ,param_10);
        if (iVar1 != 0) {
          *param_4 = iVar2;
          return 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < *(int *)(param_1[0x3f] + 0xcc));
    }
    local_4 = local_4 + 1;
  } while (local_4 < 3);
  return 0;
}


