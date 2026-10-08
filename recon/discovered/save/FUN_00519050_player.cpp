// FUN_00519050 @ 00519050 size=387

void __thiscall FUN_00519050(int param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  char local_90 [4];
  char local_8c [4];
  char local_88;
  char cStack_87;
  char local_86;
  undefined4 local_85;
  undefined1 local_81;
  undefined1 local_80 [128];
  
  iVar2 = DAT_0066829c;
  if ((*(int *)(param_1 + 0x654) != param_2) || (*(int *)(param_1 + 0x65c) != param_3)) {
    *(int *)(param_1 + 0x654) = param_2;
    *(int *)(param_1 + 0x65c) = param_3;
    if (iVar2 != 0) {
      if (DAT_0067682c != 0) {
        FUN_00587420(param_1,param_2,param_3,param_4);
      }
      if ((param_4 != 0) && (*(short *)(param_4 + 4) == 0xb)) {
        local_8c[0] = s_MPFRAGMSG1_005e2750[4];
        local_8c[1] = s_MPFRAGMSG1_005e2750[5];
        local_8c[2] = s_MPFRAGMSG1_005e2750[6];
        local_8c[3] = s_MPFRAGMSG1_005e2750[7];
        local_90[0] = s_MPFRAGMSG1_005e2750[0];
        local_90[1] = s_MPFRAGMSG1_005e2750[1];
        local_90[2] = s_MPFRAGMSG1_005e2750[2];
        local_90[3] = s_MPFRAGMSG1_005e2750[3];
        local_85 = 0;
        local_88 = s_MPFRAGMSG1_005e2750[8];
        cStack_87 = s_MPFRAGMSG1_005e2750[9];
        local_86 = s_MPFRAGMSG1_005e2750[10];
        local_81 = 0;
        cVar1 = FUN_00483300(0,4);
        _local_88 = CONCAT11(cVar1 + '1',local_88);
        iVar2 = FUN_0049d6d0(local_90);
        if (iVar2 < 0) {
          FUN_004811b0(local_80,0x80,s___s__killed___s__Plyr___d_Monst__005e275c,
                       *(undefined4 *)(param_4 + 0x38),*(undefined4 *)(param_1 + 0x38),
                       *(undefined4 *)(param_1 + 0x650),*(undefined4 *)(param_1 + 0x658),
                       *(int *)(param_1 + 0x65c) + *(int *)(param_1 + 0x654));
        }
        else {
          uVar6 = *(undefined4 *)(param_1 + 0x38);
          uVar5 = *(undefined4 *)(param_4 + 0x38);
          iVar4 = *(int *)(param_1 + 0x65c) + *(int *)(param_1 + 0x654);
          uVar8 = *(undefined4 *)(param_1 + 0x658);
          uVar7 = *(undefined4 *)(param_1 + 0x650);
          uVar3 = FUN_0049d780(iVar2);
          FUN_004811b0(local_80,0x80,uVar3,uVar5,uVar6,uVar7,uVar8,iVar4);
        }
        FUN_0054d190(&DAT_0065c5d0,1,local_80);
      }
      FUN_0051f6b0();
      FUN_0057b1f0();
    }
  }
  return;
}


