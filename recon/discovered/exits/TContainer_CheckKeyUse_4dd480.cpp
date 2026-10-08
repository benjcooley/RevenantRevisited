// TContainer_CheckKeyUse @ 0x004dd480 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// FUN_004dd480 @ 004dd480 size=1231

undefined4 __thiscall FUN_004dd480(int *param_1,int *param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  if (param_3 != (int *)0x0) {
    iVar1 = (**(code **)(*param_1 + 0x208))();
    if (iVar1 != 0) {
      if ((short)param_3[1] == 0x10) {
        iVar1 = (**(code **)(*param_3 + 0x1f0))();
        iVar2 = (**(code **)(*param_1 + 0x210))();
        if (iVar1 == iVar2) {
          if (param_2 == DAT_00667fcc) {
            iVar1 = FUN_0049c430(s_unlock_succeed_005e0c8c);
            if (-1 < iVar1) {
              iVar2 = FUN_0049b650(iVar1);
              if (iVar2 != 0) {
                FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
              }
            }
            uVar3 = FUN_0049d800(s_CONTUNLOCKED_005e0c9c);
            FUN_0054d170(&DAT_0065c5d0,&DAT_005e0cac,uVar3);
          }
          (**(code **)(*param_1 + 0x20c))(0);
          return 1;
        }
        if (param_2 == DAT_00667fcc) {
          iVar1 = FUN_0049c430(s_unlock_failed_005e0cb0);
          if (-1 < iVar1) {
            iVar2 = FUN_0049b650(iVar1);
            if (iVar2 != 0) {
              FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
            }
          }
          uVar3 = FUN_0049d800(s_CONTWRONGKEY_005e0cc0);
          FUN_0054d170(&DAT_0065c5d0,&DAT_005e0cd0,uVar3);
        }
        return 1;
      }
      if ((short)param_3[1] == 7) {
        iVar1 = (**(code **)(*param_3 + 0x1f0))();
        if (0 < iVar1) {
          if ((short)param_2[1] == 0xb) {
            iVar2 = (**(code **)(*param_2 + 0xdc))(0x32);
            iVar4 = (**(code **)(*param_2 + 0x3ec))();
            iVar1 = iVar1 + iVar4 + iVar2;
          }
          iVar2 = (**(code **)(*param_1 + 0x218))();
          if (iVar2 == 0) {
            if (param_2 == DAT_00667fcc) {
              iVar1 = FUN_0049c430(s_unlock_fail_005e0cd4);
              if (-1 < iVar1) {
                iVar2 = FUN_0049b650(iVar1);
                if (iVar2 != 0) {
                  FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                }
              }
              uVar3 = FUN_0049d800(s_CONTPICKFAIL_005e0ce0);
              FUN_0054d170(&DAT_0065c5d0,&DAT_005e0cf0,uVar3);
              iVar1 = FUN_0049c430(&DAT_005e0cf4);
              if (-1 < iVar1) {
                iVar2 = FUN_0049b650(iVar1);
                if (iVar2 != 0) {
                  FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                  return 1;
                }
              }
            }
          }
          else {
            iVar2 = (**(code **)(*param_1 + 0x218))();
            if (iVar1 < iVar2) {
              if (param_2 == DAT_00667fcc) {
                iVar1 = FUN_0049c430(s_unlock_fail_005e0cfc);
                if (-1 < iVar1) {
                  iVar2 = FUN_0049b650(iVar1);
                  if (iVar2 != 0) {
                    FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                  }
                }
                uVar3 = FUN_0049d800(s_CONTPICKTOUGH_005e0d08);
                FUN_0054d170(&DAT_0065c5d0,&DAT_005e0d18,uVar3);
                iVar1 = FUN_0049c430(&DAT_005e0d1c);
                if (-1 < iVar1) {
                  iVar2 = FUN_0049b650(iVar1);
                  if (iVar2 != 0) {
                    FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                    return 1;
                  }
                }
              }
            }
            else {
              iVar1 = FUN_00483300(0,iVar1);
              iVar2 = (**(code **)(*param_1 + 0x218))();
              if (iVar1 < iVar2) {
                if (param_2 == DAT_00667fcc) {
                  iVar1 = FUN_0049c430(s_unlock_fail_005e0d24);
                  if (-1 < iVar1) {
                    iVar2 = FUN_0049b650(iVar1);
                    if (iVar2 != 0) {
                      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                    }
                  }
                  uVar3 = FUN_0049d800(s_CONTPICKFAIL_005e0d30);
                  FUN_0054d170(&DAT_0065c5d0,&DAT_005e0d40,uVar3);
                  iVar1 = FUN_0049c430(&DAT_005e0d44);
                  if (-1 < iVar1) {
                    iVar2 = FUN_0049b650(iVar1);
                    if (iVar2 != 0) {
                      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                      return 1;
                    }
                  }
                }
              }
              else {
                if (param_2 == DAT_00667fcc) {
                  iVar1 = FUN_0049c430(s_unlock_succeed_005e0d4c);
                  if (-1 < iVar1) {
                    iVar2 = FUN_0049b650(iVar1);
                    if (iVar2 != 0) {
                      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                    }
                  }
                  uVar3 = FUN_0049d800(s_CONTUNLOCK_005e0d5c);
                  FUN_0054d170(&DAT_0065c5d0,&DAT_005e0d68,uVar3);
                  iVar1 = FUN_0049c430(&DAT_005e0d6c);
                  if (-1 < iVar1) {
                    iVar2 = FUN_0049b650(iVar1);
                    if (iVar2 != 0) {
                      FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
                    }
                  }
                }
                (**(code **)(*param_1 + 0x20c))(0);
                if ((short)param_2[1] == 0xb) {
                  (**(code **)(*param_2 + 0x418))(10,0x32);
                }
              }
            }
          }
          return 1;
        }
      }
    }
  }
  return 0;
}


