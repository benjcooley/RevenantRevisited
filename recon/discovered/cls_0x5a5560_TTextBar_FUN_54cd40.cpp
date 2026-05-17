// FUN_0054cd40 @ 0054cd40 size=720

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0054cd40(int param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined1 *puVar3;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uStack_14c;
  int iStack_148;
  int *piStack_144;
  undefined4 uStack_140;
  undefined4 uStack_13c;
  undefined4 uStack_138;
  undefined4 uStack_134;
  undefined4 uStack_130;
  int iStack_12c;
  int iStack_128;
  undefined4 uStack_124;
  int iStack_120;
  undefined1 auStack_e8 [84];
  undefined1 auStack_94 [88];
  int iStack_3c;
  int iStack_1c;
  
  iStack_120 = 0x80000000;
  uStack_124 = 0x7f7f;
  iStack_128 = 0xffff;
  iStack_12c = 0x997b;
  uStack_130 = 300;
  uStack_134 = 0x28;
  uStack_138 = 0;
  uStack_13c = 0x28;
  uStack_140 = 0x54cd77;
  (**(code **)(**(int **)(param_1 + 0x84) + 100))();
  iVar6 = *(int *)(param_1 + 0x7c);
  iStack_120 = 0;
  if (0 < *(int *)(param_1 + 0x60)) {
    iStack_128 = 0;
    do {
      if ((iStack_1c == -1) || (iStack_1c == iStack_120)) {
        puVar2 = (undefined4 *)(iStack_128 + *(int *)(param_1 + 0x6c));
        iStack_12c = DAT_0067064c;
        switch(*puVar2) {
        case 2:
          iStack_12c = DAT_00670664;
          break;
        case 4:
          iStack_12c = DAT_00670654;
          break;
        case 8:
          iStack_12c = DAT_00670668;
          break;
        case 0x10:
          iStack_12c = DAT_0067065c;
          break;
        case 0x20:
          iStack_12c = DAT_00670658;
        case 1:
          break;
        case 0x40:
          iStack_12c = puVar2[1];
          if (iStack_12c == 0) {
            iStack_12c = DAT_00670660;
          }
          break;
        case 0x80:
          iStack_12c = DAT_00670650;
        }
        piVar1 = *(int **)(param_1 + 0x84);
        uStack_140 = 0x80000000;
        piStack_144 = (int *)0x7f7f;
        iStack_148 = 0xffff;
        uStack_14c = _DAT_006668d0;
        (**(code **)(*piVar1 + 100))(0,0,piVar1[1],piVar1[2]);
        uVar4 = extraout_ECX;
        if ((iStack_3c == 0) && (*(int *)(param_1 + 0x90) != 0)) {
          FUN_0054cb00(iVar6);
          uVar4 = extraout_ECX_00;
        }
        uVar9 = 0x80000000;
        uVar8 = 0x400;
        FUN_00419dd0(&uStack_14c);
        FUN_00438ed0(4,9,iStack_148 + 0xc + *(int *)(param_1 + 0x6c),DAT_0065abc4,uVar4,uVar8,uVar9)
        ;
        if (DAT_006680c8 == 0) {
          (**(code **)(**(int **)(param_1 + 0x88) + 100))
                    (0,iVar6,(*(int **)(param_1 + 0x88))[1],*(undefined4 *)(param_1 + 0x78),0,0xffff
                     ,0x7f7f,0x80000000);
          iVar7 = *(int *)(param_1 + 0x84);
          piStack_144 = *(int **)(param_1 + 0x88);
          FUN_00438d80(auStack_94,0,iVar6,0,0,*(undefined4 *)(iVar7 + 4),*(undefined4 *)(iVar7 + 8),
                       0x100);
          puVar3 = auStack_94;
          iVar5 = *piStack_144;
        }
        else {
          iVar7 = *(int *)(param_1 + 0x84);
          piStack_144 = *(int **)(param_1 + 0x88);
          FUN_00438d80(auStack_e8,0,iVar6,0,0,*(undefined4 *)(iVar7 + 4),*(undefined4 *)(iVar7 + 8),
                       0x80000000);
          puVar3 = auStack_e8;
          iVar5 = *piStack_144;
        }
        (**(code **)(iVar5 + 0x5c))(puVar3,iVar7,0,0);
        uStack_14c = 0x80000000;
        iStack_148 = 0;
        piStack_144 = (int *)0x0;
        uStack_140 = 0;
        uStack_124 = 0;
        uStack_138 = 0;
        uStack_13c = 0;
        iStack_128 = 0;
        iStack_12c = 0;
        uStack_130 = 0;
        uStack_134 = 0;
        iStack_120 = iVar6;
        (**(code **)(**(int **)(param_1 + 0x8c) + 0x5c))
                  (&uStack_14c,*(undefined4 *)(param_1 + 0x88),0,0);
      }
      iVar6 = iVar6 - *(int *)(param_1 + 0x78);
      if (iVar6 < 0) {
        iVar6 = iVar6 + *(int *)(*(int *)(param_1 + 0x84) + 8);
      }
      iStack_120 = iStack_120 + 1;
      iStack_128 = iStack_128 + 0x5c;
    } while (iStack_120 < *(int *)(param_1 + 0x60));
  }
  return;
}


