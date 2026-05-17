// FUN_00436ec0_DefWidget_DispatchControl @ 00436ec0 size=309

undefined4 __thiscall FUN_00436ec0_DefWidget_DispatchControl(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  
  if (*(int *)(param_2 + 0x10) == 4) {
    uVar3 = *(undefined4 *)(param_2 + 0x28);
    iVar5 = 0;
    if (0 < DAT_0065617c) {
      piVar4 = &DAT_00655510;
      do {
        iVar2 = FUN_0059a530_stricmp(uVar3,*(undefined4 *)(*piVar4 + 4));
        if (iVar2 == 0) {
          if (((-1 < iVar5) && (iVar5 < DAT_0065617c)) &&
             (puVar1 = (undefined4 *)(&DAT_00655510)[iVar5], puVar1 != (undefined4 *)0x0)) {
            FUN_00479580();
            iVar5 = FUN_00479700(s_STYLE_005cd9f8,0);
            if (iVar5 != 0) {
              uVar3 = (**(code **)(*param_1 + 0x98))(s_Control_STYLE_definitions_not_al_005cda00,0);
              return uVar3;
            }
            piVar4 = (int *)(**(code **)*puVar1)(param_1,param_2);
            if ((*(byte *)(piVar4 + 5) & 1) != 0) {
              if (piVar4 != (int *)0x0) {
                (**(code **)*piVar4)(1);
              }
              return 0;
            }
            if (*(int *)(param_2 + 0x10) != 9) {
              if (piVar4 != (int *)0x0) {
                (**(code **)*piVar4)(1);
              }
              uVar3 = (**(code **)(*param_1 + 0x98))
                                (s_Unrecognized_tag_in__s_control_005cda38,puVar1[1]);
              return uVar3;
            }
            FUN_004795a0();
            (**(code **)(*piVar4 + 4))(param_1);
            iVar5 = FUN_0041c840(piVar4);
            piVar4[3] = iVar5;
            (**(code **)(*piVar4 + 0x1c))(piVar4[5] | 0x20);
            return 1;
          }
          break;
        }
        iVar5 = iVar5 + 1;
        piVar4 = piVar4 + 1;
      } while (iVar5 < DAT_0065617c);
    }
  }
  uVar3 = (**(code **)(*param_1 + 0x98))
                    (s_Unrecognized_layout_command__s_005cda58,*(undefined4 *)(param_2 + 0x28));
  return uVar3;
}


