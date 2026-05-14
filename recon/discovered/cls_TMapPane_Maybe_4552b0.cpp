// FUN_004552b0 @ 004552b0 size=2241

void __thiscall FUN_004552b0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined1 *puVar6;
  bool bVar7;
  int local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined1 local_40 [64];
  
  uVar2 = *(undefined4 *)(param_1 + 0x8c);
  *(undefined4 *)(param_1 + 0x90) = *(undefined4 *)(param_1 + 0x88);
  *(undefined4 *)(param_1 + 0x8c) = param_3;
  bVar7 = DAT_0065844c != 0;
  *(undefined4 *)(param_1 + 0x94) = uVar2;
  *(undefined4 *)(param_1 + 0x88) = param_2;
  if (((bVar7) && (*(int *)(param_1 + 0x918) == 0)) && (DAT_00658324 == 0)) {
    if (((*(int *)(param_1 + 0x9a8) == 0) && (DAT_005d7a54 != 0)) || (DAT_00658478 != 0)) {
      if (((*(int *)(param_1 + 0x9a8) == 0) && (DAT_005d7a54 != 0)) && (DAT_00658478 == 1)) {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          FUN_00455e40();
        }
        if (*(int *)(param_1 + 0x834) != 0) {
          *(undefined4 *)(param_1 + 0x848) = *(undefined4 *)(param_1 + 0x838);
          *(undefined4 *)(param_1 + 0x84c) = *(undefined4 *)(param_1 + 0x83c);
          *(undefined4 *)(param_1 + 0x850) = *(undefined4 *)(param_1 + 0x840);
          *(undefined4 *)(param_1 + 0x854) = *(undefined4 *)(param_1 + 0x844);
          *(undefined4 *)(param_1 + 0x834) = 0;
        }
        FUN_004562a0();
        if (DAT_00658324 == 0) {
          FUN_00455f90();
        }
        DAT_00658478 = 0;
      }
    }
    else {
      if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
        FUN_00455e40();
      }
      if (*(int *)(param_1 + 0x834) != 0) {
        *(undefined4 *)(param_1 + 0x848) = *(undefined4 *)(param_1 + 0x838);
        *(undefined4 *)(param_1 + 0x84c) = *(undefined4 *)(param_1 + 0x83c);
        *(undefined4 *)(param_1 + 0x850) = *(undefined4 *)(param_1 + 0x840);
        *(undefined4 *)(param_1 + 0x854) = *(undefined4 *)(param_1 + 0x844);
        *(undefined4 *)(param_1 + 0x834) = 0;
      }
      FUN_004562a0();
      if (DAT_00658324 == 0) {
        FUN_00455f90();
      }
      DAT_00658478 = 1;
    }
    if ((*(int *)(param_1 + 0x834) != 0) && (DAT_00658324 == 0)) {
      if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
        FUN_00455e40();
      }
      if (*(int *)(param_1 + 0x834) != 0) {
        *(undefined4 *)(param_1 + 0x848) = *(undefined4 *)(param_1 + 0x838);
        *(undefined4 *)(param_1 + 0x84c) = *(undefined4 *)(param_1 + 0x83c);
        *(undefined4 *)(param_1 + 0x850) = *(undefined4 *)(param_1 + 0x840);
        *(undefined4 *)(param_1 + 0x854) = *(undefined4 *)(param_1 + 0x844);
        *(undefined4 *)(param_1 + 0x834) = 0;
      }
      FUN_004562a0();
      if (DAT_00658324 == 0) {
        FUN_00455f90();
      }
    }
    if (*(int *)(param_1 + 0x9c) != *(int *)(param_1 + 0xa0)) {
      *(undefined4 *)(param_1 + 0xf4) = 1;
    }
    local_70 = *(int *)(param_1 + 0x88);
    if (((local_70 == *(int *)(param_1 + 0x90)) &&
        (*(int *)(param_1 + 0x8c) == *(int *)(param_1 + 0x94))) && (*(int *)(param_1 + 0xf4) == 0))
    {
      if (0 < *(int *)(param_1 + 0x130)) {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          FUN_00455e40();
        }
        if (*(int *)(param_1 + 0x834) != 0) {
          FUN_0041c700(param_1 + 0x838);
          *(undefined4 *)(param_1 + 0x834) = 0;
        }
        FUN_004562a0();
        if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
        }
        local_74 = 0;
        if (0 < *(int *)(param_1 + 0x130)) {
          puVar5 = (undefined4 *)(param_1 + 0x144);
          do {
            iVar3 = FUN_004ad610(param_1 + 0x848,puVar5 + -4,&local_50);
            if ((iVar3 != 0) && (uVar2 = *puVar5, DAT_0065844c != 0)) {
              if (DAT_00658324 != 0) {
                FUN_004550f0(0);
              }
              if (DAT_00658438 < 0x50) {
                FUN_00482130();
                FUN_0041c700(&local_50);
                (&DAT_006584d8)[DAT_00658438 * 7] = uVar2;
                (&DAT_006584dc)[DAT_00658438 * 7] = 0;
                (&DAT_006584e0)[DAT_00658438 * 7] = 0;
                DAT_00658438 = DAT_00658438 + 1;
                FUN_00482140();
              }
            }
            local_74 = local_74 + 1;
            puVar5 = puVar5 + 7;
          } while (local_74 < *(int *)(param_1 + 0x130));
        }
        iVar3 = DAT_0065844c;
        *(undefined4 *)(param_1 + 0x130) = 0;
        if (iVar3 != 0) {
          FUN_00482130();
          DAT_00658324 = 1;
          DAT_006584c0 = 0;
          if (DAT_00658478 == 0) {
            SetEvent(DAT_00658458);
          }
          FUN_00482140();
          if (DAT_00658478 != 0) {
            FUN_00456330();
          }
        }
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          FUN_00455e40();
        }
        if (*(int *)(param_1 + 0x834) != 0) {
          FUN_0041c700(param_1 + 0x838);
          *(undefined4 *)(param_1 + 0x834) = 0;
        }
        FUN_004562a0();
        if ((DAT_00658324 == 0) && (DAT_0065844c != 0)) {
          FUN_00482130();
          DAT_00658438 = 0;
          FUN_00482140();
          return;
        }
      }
    }
    else {
      local_6c = *(int *)(param_1 + 0x8c);
      local_68 = local_70 + -0x41 + DAT_0065ba00;
      local_64 = local_6c + -0x41 + DAT_00667c50;
      local_60 = local_70;
      local_5c = local_6c;
      local_58 = local_68;
      local_54 = local_64;
      if ((*(int *)(param_1 + 0xf4) == 0) &&
         (((((piVar1 = (int *)(param_1 + 0x848), local_70 <= *(int *)(param_1 + 0x850) &&
             (*piVar1 <= local_68)) && (local_6c <= *(int *)(param_1 + 0x854))) &&
           (*(int *)(param_1 + 0x84c) <= local_64)) ||
          (((local_70 <= *(int *)(param_1 + 0x840) && (*(int *)(param_1 + 0x838) <= local_68)) &&
           ((local_6c <= *(int *)(param_1 + 0x844) && (*(int *)(param_1 + 0x83c) <= local_64))))))))
      {
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          FUN_00455e40();
        }
        if (*(int *)(param_1 + 0x834) != 0) {
          FUN_0041c700(param_1 + 0x838);
          *(undefined4 *)(param_1 + 0x834) = 0;
        }
        FUN_004562a0();
        if (DAT_00658324 == 0) {
          FUN_00455f90();
        }
        if (*(int *)(param_1 + 0x90) < *(int *)(param_1 + 0x88)) {
          local_68 = local_68 + 0x40;
        }
        else if (*(int *)(param_1 + 0x88) < *(int *)(param_1 + 0x90)) {
          local_70 = local_70 + -0x40;
        }
        else {
          local_70 = *piVar1;
          local_68 = *(int *)(param_1 + 0x850);
        }
        if (*(int *)(param_1 + 0x94) < *(int *)(param_1 + 0x8c)) {
          local_64 = local_64 + 0x40;
        }
        else if (*(int *)(param_1 + 0x8c) < *(int *)(param_1 + 0x94)) {
          local_6c = local_6c + -0x40;
        }
        else {
          local_6c = *(int *)(param_1 + 0x84c);
          local_64 = *(int *)(param_1 + 0x854);
        }
        iVar3 = FUN_004ad6a0(&local_70,piVar1,local_40,&local_74);
        if ((iVar3 != 0) && (iVar3 = 0, 0 < local_74)) {
          puVar6 = local_40;
          do {
            if (DAT_0065844c != 0) {
              if (DAT_00658324 != 0) {
                FUN_004550f0(0);
              }
              if (DAT_00658438 < 0x50) {
                FUN_00482130();
                FUN_0041c700(puVar6);
                (&DAT_006584d8)[DAT_00658438 * 7] = 0;
                (&DAT_006584dc)[DAT_00658438 * 7] = 0;
                (&DAT_006584e0)[DAT_00658438 * 7] = 0;
                DAT_00658438 = DAT_00658438 + 1;
                FUN_00482140();
              }
            }
            iVar3 = iVar3 + 1;
            puVar6 = puVar6 + 0x10;
          } while (iVar3 < local_74);
        }
        FUN_004ad610(param_1 + 0x848,&local_70,param_1 + 0x848);
        iVar3 = 0;
        if (0 < *(int *)(param_1 + 0x130)) {
          puVar5 = (undefined4 *)(param_1 + 0x144);
          do {
            local_50 = puVar5[-4];
            local_4c = puVar5[-3];
            local_48 = puVar5[-2];
            local_44 = puVar5[-1];
            iVar4 = FUN_004ad610(&local_50,param_1 + 0x848,&local_50);
            if (iVar4 != 0) {
              FUN_004560e0(&local_50,*puVar5);
            }
            iVar3 = iVar3 + 1;
            puVar5 = puVar5 + 7;
          } while (iVar3 < *(int *)(param_1 + 0x130));
        }
        *(int *)(param_1 + 0x838) = local_70;
        *(int *)(param_1 + 0x83c) = local_6c;
        *(int *)(param_1 + 0x844) = local_64;
        *(int *)(param_1 + 0x840) = local_68;
        *(undefined4 *)(param_1 + 0x834) = 1;
        FUN_00455b80();
        if ((0 < *(int *)(param_1 + 0x130)) || (iVar3 = FUN_0045f6f0(param_1 + 0x848), iVar3 == 0))
        {
          FUN_004550f0(1);
          *(undefined4 *)(param_1 + 0x130) = 0;
          return;
        }
      }
      else {
        if (DAT_0065844c != 0) {
          if (DAT_00658324 == 0) {
            DAT_006584c0 = 0;
          }
          else {
            FUN_00482130();
            DAT_00658324 = 0;
            DAT_006584c0 = 1;
            FUN_00482140();
            FUN_004550f0(0);
          }
        }
        local_70 = *(int *)(param_1 + 0x88);
        local_6c = *(int *)(param_1 + 0x8c);
        local_68 = local_70 + -1 + DAT_0065ba00;
        local_64 = local_6c + -1 + DAT_00667c50;
        local_60 = local_70;
        local_5c = local_6c;
        local_58 = local_68;
        local_54 = local_64;
        if (DAT_0065844c != 0) {
          if (DAT_00658324 != 0) {
            FUN_004550f0(0);
          }
          if (DAT_00658438 < 0x50) {
            FUN_00482130();
            FUN_0041c700(&local_70);
            (&DAT_006584d8)[DAT_00658438 * 7] = 0;
            (&DAT_006584dc)[DAT_00658438 * 7] = 0;
            (&DAT_006584e0)[DAT_00658438 * 7] = 0;
            DAT_00658438 = DAT_00658438 + 1;
            FUN_00482140();
          }
        }
        *(int *)(param_1 + 0x83c) = local_6c;
        iVar3 = DAT_0065844c;
        *(int *)(param_1 + 0x838) = local_70;
        *(int *)(param_1 + 0x840) = local_68;
        *(int *)(param_1 + 0x844) = local_64;
        *(undefined4 *)(param_1 + 0x834) = 1;
        if (iVar3 != 0) {
          FUN_00482130();
          DAT_00658324 = 1;
          DAT_006584c0 = 0;
          if (DAT_00658478 == 0) {
            SetEvent(DAT_00658458);
          }
          FUN_00482140();
          if (DAT_00658478 != 0) {
            FUN_00456330();
          }
        }
        if ((DAT_00658d90 != 0) || (DAT_00658d94 != 0)) {
          FUN_00455e40();
        }
        if (*(int *)(param_1 + 0x834) != 0) {
          FUN_0041c700((int *)(param_1 + 0x838));
          *(undefined4 *)(param_1 + 0x834) = 0;
        }
        FUN_004562a0();
        if (DAT_00658324 == 0) {
          FUN_00455f90();
        }
      }
      *(undefined4 *)(param_1 + 0x130) = 0;
    }
  }
  return;
}


