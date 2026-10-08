// TExit_SetExitState @ 0x0050d530 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// non-virtual
// FUN_0050d530 @ 0050d530 size=269

void __thiscall FUN_0050d530(int *param_1,int param_2)

{
  int iVar1;
  
  if (param_2 == 1) {
    iVar1 = (**(code **)(*param_1 + 0x138))(&DAT_005e17e0,0xffffffff);
  }
  else if (param_2 == 3) {
    iVar1 = (**(code **)(*param_1 + 0x138))(s_closed_005e17e8,0xffffffff);
  }
  else if (param_2 == 0) {
    iVar1 = (**(code **)(*param_1 + 0x138))(s_openingout_005e17f0,0xffffffff);
    if (-1 < iVar1) goto LAB_0050d630;
    iVar1 = (**(code **)(*param_1 + 0x138))(s_closed_to_open_005e17fc,0xffffffff);
  }
  else if (param_2 == 2) {
    iVar1 = (**(code **)(*param_1 + 0x138))(s_closingout_005e180c,0xffffffff);
    if (-1 < iVar1) goto LAB_0050d630;
    iVar1 = (**(code **)(*param_1 + 0x138))(s_open_to_closed_005e1818,0xffffffff);
  }
  else if (param_2 == 4) {
    iVar1 = (**(code **)(*param_1 + 0x138))(s_openingin_005e1828,0xffffffff);
    if (-1 < iVar1) goto LAB_0050d630;
    iVar1 = (**(code **)(*param_1 + 0x138))(s_closed_to_open_005e1834,0xffffffff);
  }
  else {
    iVar1 = param_2;
    if (param_2 == 5) {
      iVar1 = (**(code **)(*param_1 + 0x138))(s_closingin_005e1844,0xffffffff);
      if (-1 < iVar1) goto LAB_0050d630;
      iVar1 = (**(code **)(*param_1 + 0x138))(s_open_to_closed_005e1850,0xffffffff);
    }
  }
  if (iVar1 < 0) {
    iVar1 = param_2;
  }
LAB_0050d630:
  (**(code **)(*param_1 + 0x18))(iVar1);
  return;
}


