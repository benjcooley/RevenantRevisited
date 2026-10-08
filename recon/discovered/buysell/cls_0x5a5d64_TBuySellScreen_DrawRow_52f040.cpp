// FUN_0052f040 @ 0052f040 size=719

void __thiscall
FUN_0052f040(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5,
            int param_6)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined1 local_26c;
  undefined1 local_26b;
  undefined1 local_26a;
  undefined1 local_269;
  int local_268;
  undefined1 local_264;
  undefined1 local_263;
  undefined1 local_200;
  undefined4 local_1ff;
  
  FUN_0058b100(&local_264,&DAT_005e3d08,param_1[1]);
  if (param_6 == 0) {
    local_26a = 0x82;
    local_26b = 0xd;
    local_26c = 0xc5;
    local_269 = 0xff;
  }
  else {
    if (param_6 == 1) {
      local_26a = 0xe6;
      local_26b = 0x96;
    }
    else {
      if (param_6 != 2) goto LAB_0052f0ad;
      local_26a = 200;
      local_26b = 0x53;
    }
    local_26c = 0xff;
    local_269 = 0xff;
  }
LAB_0052f0ad:
  param_5 = param_5 % 3;
  uVar6 = 0x401;
  iVar2 = param_5 + 1;
  iVar3 = iVar2 * 0x2a;
  uVar1 = param_4;
  FUN_00419dd0(&local_26c);
  FUN_004be2b0(0x3c,iVar3,0x154,0x14,&local_264,0,DAT_00667540,iVar2,uVar6,uVar1);
  uVar1 = FUN_0049d800(&DAT_005e3d0c);
  FUN_0058b100(&local_264,&DAT_005e3d14,param_1[0xe],uVar1);
  uVar7 = 0x404;
  uVar1 = extraout_ECX;
  uVar6 = param_4;
  FUN_00419dd0(&local_26c);
  FUN_004be2b0(0x164,iVar3,0x44,0x14,&local_264,0,DAT_00667540,uVar1,uVar7,uVar6);
  local_200 = DAT_0066f370;
  local_264 = 0x20;
  puVar4 = &local_1ff;
  for (iVar2 = 0x7f; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  *(undefined2 *)puVar4 = 0;
  *(undefined1 *)((int)puVar4 + 2) = 0;
  local_263 = 0;
  local_26a = 0xa2;
  local_26b = 0xa2;
  local_26c = 0xa2;
  local_269 = 0xa2;
  local_268 = 0;
  if (0 < param_1[0xd]) {
    piVar5 = param_1 + 4;
    do {
      if (*piVar5 != 0) {
        FUN_0058b100(&local_200,s__s_s____d_005e3d1c,&local_200,piVar5[-1],*piVar5);
      }
      local_268 = local_268 + 1;
      piVar5 = piVar5 + 2;
    } while (local_268 < param_1[0xd]);
  }
  uVar7 = 0x401;
  uVar1 = param_4;
  uVar6 = param_4;
  FUN_00419dd0(&local_26c);
  FUN_004be2b0(0x40,param_5 * 0x2a + 0x3e,400,0xc,&local_200,0,DAT_0065abc4,uVar1,uVar7,uVar6);
  if (*param_1 != 0) {
    FUN_0058b100(&local_200,&DAT_005e3d28,*param_1);
    uVar6 = 0x401;
    uVar1 = param_4;
    FUN_00419dd0(&local_26c);
    FUN_004be2b0(0x40,param_5 * 0x2a + 0x4a,400,0xc,&local_200,0,DAT_0065abc4,param_4,uVar6,uVar1);
  }
  if ((int *)param_1[0x10] != (int *)0x0) {
    iVar2 = (**(code **)(*(int *)param_1[0x10] + 0xd4))(0,0);
    if (iVar2 == 0) {
      iVar2 = (**(code **)(*(int *)param_1[0x10] + 0xd8))(0);
      if (iVar2 == 0) {
        return;
      }
      uVar7 = 0;
      uVar6 = 0x110;
      uVar1 = FUN_004186d0(0);
    }
    else {
      uVar7 = 0;
      uVar6 = 0;
      uVar1 = (**(code **)(*(int *)param_1[0x10] + 0xd4))(0,0,0x110,0);
    }
    FUN_004bd680(5,param_5 * 0x2b + 0x2c,uVar1,uVar6,uVar7);
  }
  return;
}


