// FUN_0047b4d0_TPlayScreen_Pulse @ 0047b4d0 size=2119

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0047b4d0_TPlayScreen_Pulse(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined *puVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  int iStack_4;
  
  if ((*(int *)(param_1 + 0x6b8) == 0) ||
     ((*(int *)(param_1 + 0x6c0) == 3 && (*(int *)(param_1 + 0x6ac) != 0)))) {
    if ((*(int *)(param_1 + 0x6b8) == 0) &&
       ((*(int *)(param_1 + 0x6c0) == 3 && (*(int *)(param_1 + 0x6ac) != 0)))) {
      *(undefined4 *)(param_1 + 0x6b4) = 1;
    }
  }
  else {
    iVar2 = FUN_0052f390();
    if (iVar2 != 0) {
      *(undefined4 *)(param_1 + 0x6c4) = 3;
      *(undefined4 *)(param_1 + 0x6b0) = 1;
    }
  }
  if (*(int *)(param_1 + 0x6bc) != 0) {
    if ((*(int *)(param_1 + 0x6c0) != 1) || (*(int *)(param_1 + 0x6ac) == 0)) {
      iVar2 = FUN_0052f390();
      if (iVar2 != 0) {
        *(undefined4 *)(param_1 + 0x6c4) = 1;
        *(undefined4 *)(param_1 + 0x6b0) = 1;
      }
      goto LAB_0047b58c;
    }
    if (*(int *)(param_1 + 0x6bc) != 0) goto LAB_0047b58c;
  }
  if ((*(int *)(param_1 + 0x6c0) == 1) && (*(int *)(param_1 + 0x6ac) != 0)) {
    *(undefined4 *)(param_1 + 0x6b4) = 1;
  }
LAB_0047b58c:
  if ((((*(int *)(param_1 + 0x6b0) != 0) || (*(int *)(param_1 + 0x6ac) != 0)) &&
      (*(int *)(param_1 + 0x6a0) == 0)) && (*(int *)(param_1 + 0x6c4) == 3)) {
    *(undefined4 *)(param_1 + 0x6a4) = 1;
  }
  iVar2 = *(int *)(param_1 + 0x6a0);
  if ((((iVar2 != 0) || (*(int *)(param_1 + 0x6a4) != 0)) &&
      ((*(int *)(param_1 + 0x6ac) != 0 || (*(int *)(param_1 + 0x6b0) != 0)))) &&
     ((*(int *)(param_1 + 0x6c4) == 1 && (*(undefined4 *)(param_1 + 0x6a4) = 0, iVar2 != 0)))) {
    *(undefined4 *)(param_1 + 0x6a8) = 1;
  }
  if (*(int *)(param_1 + 0x6c4) == 1) {
    iStack_4 = 0x8c;
  }
  else {
    iStack_4 = (-(uint)(*(int *)(param_1 + 0x6c4) != 3) & 0xffffff8a) + 0xb2;
  }
  if ((((iVar2 == 0) && (*(int *)(param_1 + 0x6a4) == 0)) || (DAT_00666180 == 0)) ||
     (DAT_00666154 == *(int *)(PTR_DAT_005d79e0 + 4) + -0xbc)) {
    if (((iVar2 == 0) || (*(int *)(param_1 + 0x6a8) != 0)) &&
       ((DAT_00666180 != 0 && (DAT_00666154 != *(int *)(PTR_DAT_005d79e0 + 4))))) {
      *(undefined4 *)(param_1 + 0x6a0) = 0;
      uVar3 = FUN_0053cbd0();
      FUN_0048ee10(uVar3);
      uVar3 = FUN_0053cbe0();
      FUN_0048ee10(uVar3);
      _DAT_00666158 = 0;
      DAT_0066615c = 0;
      DAT_006661ac = 0;
      DAT_00666154 = *(int *)(PTR_DAT_005d79e0 + 4);
      _DAT_00666160 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
    }
  }
  else {
    _DAT_00666160 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
    uVar6 = 0xffffffff;
    DAT_00666154 = *(int *)(PTR_DAT_005d79e0 + 4) + -0xbc;
    *(undefined4 *)(param_1 + 0x6a0) = 1;
    _DAT_00666158 = 0;
    DAT_0066615c = 0xbc;
    _DAT_00666190 = 1;
    uVar3 = FUN_0053cbd0(0xffffffff);
    FUN_0048ed90(uVar3,uVar6);
    uVar6 = 0xffffffff;
    uVar3 = FUN_0053cbe0(0xffffffff);
    FUN_0048ed90(uVar3,uVar6);
    piVar4 = (int *)FUN_0053cbd0();
    (**(code **)(*piVar4 + 0x30))();
    piVar4 = (int *)FUN_0053cbe0();
    (**(code **)(*piVar4 + 0x30))();
    piVar4 = (int *)FUN_0053cbd0();
    (**(code **)(*piVar4 + 0x2c))(1);
    piVar4 = (int *)FUN_0053cbe0();
    (**(code **)(*piVar4 + 0x2c))(1);
    DAT_006661ac = 1;
  }
  iVar2 = DAT_00667c70;
  puVar5 = PTR_DAT_005d79e0;
  *(undefined4 *)(param_1 + 0x6a8) = 0;
  *(undefined4 *)(param_1 + 0x6a4) = 0;
  if (((*(int *)(param_1 + 0x6ac) == 0) && (*(int *)(param_1 + 0x6b0) == 0)) ||
     ((DAT_00667c98 == 0 ||
      ((iVar2 == *(int *)(puVar5 + 8) - iStack_4 &&
       (*(int *)(param_1 + 0x6c0) == *(int *)(param_1 + 0x6c4))))))) {
    if (((*(int *)(param_1 + 0x6ac) == 0) || (*(int *)(param_1 + 0x6b4) != 0)) &&
       ((DAT_00667c98 != 0 && (iVar2 != *(int *)(puVar5 + 8))))) {
      *(undefined4 *)(param_1 + 0x6ac) = 0;
      *(undefined4 *)(param_1 + 0x6b8) = 0;
      if (*(int *)(param_1 + 0x6c0) == 1) {
        FUN_0043d6c0();
      }
      else {
        if (*(int *)(param_1 + 0x6c0) == 3) {
          FUN_00530600();
          puVar7 = &DAT_0065a3b8;
        }
        else {
          FUN_0052c7e0();
          _DAT_0065b070 = 1;
          _DAT_0065b074 = 1;
          _DAT_0065c740 = 1;
          _DAT_0065c744 = 1;
          FUN_0048ee10(&DAT_0065b638);
          FUN_0048ee10(&DAT_0065b028);
          puVar7 = &DAT_0065c6f8;
        }
        FUN_0048ee10(puVar7);
      }
      _DAT_00667c6c = 0;
      DAT_00667c70 = *(int *)(PTR_DAT_005d79e0 + 8);
      FUN_0052da60(*(int *)(PTR_DAT_005d79e0 + 4) - DAT_0066615c,0);
      if ((*(int *)(param_1 + 0x6c0) == 1) ||
         (puVar5 = PTR_DAT_005d79e0, iVar2 = DAT_00667c70, *(int *)(param_1 + 0x6c0) == 3)) {
        DAT_0065c618 = 0;
        _DAT_0065c61c = 0;
        (**(code **)(DAT_0065c5d0 + 0x28))();
        _DAT_0065be98 = 0;
        _DAT_0065be9c = 0;
        (**(code **)(DAT_0065be50 + 0x28))();
        _DAT_0065a908 = 0;
        _DAT_0065a90c = 0;
        (**(code **)(DAT_0065a8c0 + 0x28))();
        _DAT_00667d10 = 0;
        _DAT_00667d14 = 0;
        (**(code **)(DAT_00667cc8 + 0x28))();
        *(undefined4 *)(param_1 + 0x6c0) = 2;
        puVar5 = PTR_DAT_005d79e0;
        iVar2 = DAT_00667c70;
      }
    }
  }
  else {
    iVar2 = *(int *)(param_1 + 0x6c0);
    if (iVar2 != *(int *)(param_1 + 0x6c4)) {
      if (iVar2 == 1) {
        FUN_0043d6c0();
        puVar5 = PTR_DAT_005d79e0;
      }
      else {
        if (iVar2 == 3) {
          FUN_00530600();
          puVar7 = &DAT_0065a3b8;
        }
        else {
          FUN_0052c7e0();
          _DAT_0065b070 = 1;
          _DAT_0065b074 = 1;
          _DAT_0065c740 = 1;
          _DAT_0065c744 = 1;
          FUN_0048ee10(&DAT_0065b638);
          FUN_0048ee10(&DAT_0065b028);
          puVar7 = &DAT_0065c6f8;
        }
        FUN_0048ee10(puVar7);
        puVar5 = PTR_DAT_005d79e0;
      }
    }
    DAT_00667c70 = *(int *)(puVar5 + 8) - iStack_4;
    iVar2 = *(int *)(puVar5 + 4) - DAT_0066615c;
    *(undefined4 *)(param_1 + 0x6ac) = 1;
    _DAT_00667c6c = 0;
    FUN_0052da60(iVar2,iStack_4);
    _DAT_00667ca8 = 1;
    if (*(int *)(param_1 + 0x6c4) == 1) {
      FUN_0054c9c0();
      _DAT_0065a908 = 1;
      _DAT_0065a90c = 1;
      FUN_00535120();
      uVar3 = DAT_0065a294;
      _DAT_0065be98 = 1;
      _DAT_0065be9c = 1;
      *(undefined4 *)(param_1 + 0x6c0) = 1;
      FUN_0043d410(uVar3);
      DAT_0065a294 = 0;
      DAT_00667cbc = 1;
      puVar5 = PTR_DAT_005d79e0;
      iVar2 = DAT_00667c70;
    }
    else if (*(int *)(param_1 + 0x6c4) == 3) {
      FUN_0054c9c0();
      _DAT_0065a908 = 0;
      _DAT_0065a90c = 0;
      (**(code **)(DAT_0065a8c0 + 0x28))();
      _DAT_00667d10 = 0;
      _DAT_00667d14 = 0;
      (**(code **)(DAT_00667cc8 + 0x28))();
      _DAT_0065be98 = 0;
      _DAT_0065be9c = 0;
      (**(code **)(DAT_0065be50 + 0x28))();
      *(undefined4 *)(param_1 + 0x6c0) = 3;
      FUN_0048ed90(&DAT_0065a3b8,0xffffffff);
      _DAT_0065a400 = 0;
      _DAT_0065a404 = 0;
      (**(code **)(DAT_0065a3b8 + 0x28))();
      _DAT_0065a408 = 1;
      *(undefined4 *)(param_1 + 0x6b8) = 1;
      DAT_00667cbc = 1;
      puVar5 = PTR_DAT_005d79e0;
      iVar2 = DAT_00667c70;
    }
    else {
      DAT_0065c618 = 0;
      _DAT_0065c61c = 0;
      (**(code **)(DAT_0065c5d0 + 0x28))();
      _DAT_0065a908 = 0;
      _DAT_0065a90c = 0;
      (**(code **)(DAT_0065a8c0 + 0x28))();
      _DAT_00667d10 = 0;
      _DAT_00667d14 = 0;
      (**(code **)(DAT_00667cc8 + 0x28))();
      _DAT_0065be98 = 0;
      _DAT_0065be9c = 0;
      (**(code **)(DAT_0065be50 + 0x28))();
      *(undefined4 *)(param_1 + 0x6c0) = 2;
      FUN_0048ed90(&DAT_0065b638,0xffffffff);
      FUN_0048ed90(&DAT_0065b028,0xffffffff);
      FUN_0048ed90(&DAT_0065c6f8,0xffffffff);
      _DAT_0065b680 = 0;
      _DAT_0065b684 = 0;
      (**(code **)(DAT_0065b638 + 0x28))();
      _DAT_0065b070 = 0;
      _DAT_0065b074 = 0;
      (**(code **)(DAT_0065b028 + 0x28))();
      (**(code **)(DAT_0065b028 + 0x28))();
      _DAT_0065c740 = 0;
      _DAT_0065c744 = 0;
      (**(code **)(DAT_0065c6f8 + 0x28))();
      DAT_0065b688 = 1;
      _DAT_0065b078 = 1;
      _DAT_0065c748 = 1;
      DAT_00667cbc = 1;
      puVar5 = PTR_DAT_005d79e0;
      iVar2 = DAT_00667c70;
    }
  }
  iVar1 = DAT_00667c98;
  *(undefined4 *)(param_1 + 0x6b0) = 0;
  *(undefined4 *)(param_1 + 0x6b4) = 0;
  if (iVar1 != 0) {
    FUN_0052da60(*(int *)(puVar5 + 4) - DAT_0066615c,DAT_00667c78);
    puVar5 = PTR_DAT_005d79e0;
    iVar2 = DAT_00667c70;
  }
  if (DAT_0065be90 != 0) {
    _DAT_0065be64 = (*(int *)(puVar5 + 4) - DAT_0065be5c) - DAT_0066615c;
    _DAT_0065be68 = iVar2 - DAT_0065be60;
  }
  if (DAT_0065c610 != 0) {
    _DAT_0065c5e4 = 0;
    _DAT_0065c5e8 = iVar2 - DAT_0065c5e0;
    _DAT_0065c5f0 = DAT_0065c5e0;
    _DAT_0065c5ec = (*(int *)(puVar5 + 4) - _DAT_0065be6c) - DAT_0066615c;
  }
  if (DAT_0065a900 != 0) {
    _DAT_0065a8e0 = 0x70;
    _DAT_0065a8dc = *(int *)(puVar5 + 4) - DAT_0066615c;
  }
  if (DAT_00666918 != 0) {
    DAT_006668f4 = *(int *)(puVar5 + 4) - DAT_0066615c;
    DAT_006668f8 = *(int *)(puVar5 + 8) - DAT_00667c78;
  }
  if (DAT_00667d08 != 0) {
    _DAT_00667ce4 = *(int *)(puVar5 + 4) - DAT_0066615c;
    _DAT_00667ce8 = *(int *)(puVar5 + 8) - DAT_00667c78;
  }
  if ((*(int *)(param_1 + 0x6a0) == 0) && (*(int *)(param_1 + 0x6ac) == 0)) {
    *(undefined4 *)(param_1 + 0x5b8) = 1;
    *(undefined4 *)(param_1 + 0x6d0) = 0;
    return;
  }
  *(undefined4 *)(param_1 + 0x5b8) = 0;
  *(undefined4 *)(param_1 + 0x6d0) = 0;
  return;
}


