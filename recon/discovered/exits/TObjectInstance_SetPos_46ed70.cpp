// TObjectInstance_SetPos @ 0x0046ed70 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x8
// FUN_0046ed70 @ 0046ed70 size=667

/* WARNING: Removing unreachable block (ram,0x0046ef71) */
/* WARNING: Removing unreachable block (ram,0x0046ef75) */

int __thiscall FUN_0046ed70(int *param_1,int *param_2,uint param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iStack_80;
  int iStack_7c;
  int iStack_78;
  int iStack_74;
  int iStack_68;
  undefined1 auStack_64 [12];
  undefined1 auStack_58 [8];
  undefined1 auStack_50 [8];
  undefined1 auStack_48 [72];
  
  if (DAT_00666918 == 0) {
    if ((int)param_3 < 0) {
      param_3 = 0;
    }
  }
  else if ((int)param_3 < 0) {
    param_3 = (uint)*(ushort *)((int)param_1 + 0xe);
  }
  if ((((*param_2 != param_1[4]) || (param_2[1] != param_1[5])) || (param_2[2] != param_1[6])) ||
     (param_3 != *(ushort *)((int)param_1 + 0xe))) {
    if (((param_4 == 0) && (DAT_00666918 != 0)) && ((param_1[0x11] != 0 && (-1 < param_1[0x10])))) {
      if ((-1 < param_1[0x14]) &&
         (piVar3 = (int *)FUN_00452690(param_1[0x14],0), piVar3 != (int *)0x0)) {
        iStack_68 = param_2[2] - param_1[6];
        iStack_74 = piVar3[6] + iStack_68;
        iStack_7c = piVar3[4] + (*param_2 - param_1[4]);
        iStack_78 = piVar3[5] + (param_2[1] - param_1[5]);
        (**(code **)(*piVar3 + 8))(&iStack_7c,0xffffffff,0);
      }
      iVar4 = FUN_00459f50(param_1,param_2,param_3);
      (**(code **)(*param_1 + 0xf4))(auStack_50);
      if ((param_1[2] & 0x400000U) == 0) {
        FUN_00452750(param_1,3,0);
      }
      iVar5 = *param_2;
      iVar1 = param_2[1];
      param_1[6] = param_2[2];
      param_1[4] = iVar5;
      param_1[5] = iVar1;
      if ((param_1[2] & 0x80000U) != 0) {
        *(short *)((int)param_1 + 0xe) = (short)param_3;
      }
      if ((param_1[2] & 0x400000U) == 0) {
        FUN_00452750(param_1,0,0);
      }
      if (param_1[0x27] != -1) {
        iStack_78 = param_1[6] + param_1[0x26];
        iStack_7c = param_1[5] + param_1[0x25];
        iStack_80 = param_1[4] + param_1[0x24];
        FUN_00415ad0(param_1[0x27],&iStack_80);
      }
      (**(code **)(*param_1 + 0xf4))(auStack_64);
      iVar5 = (**(code **)(*param_1 + 0xfc))();
      if (iVar5 != 4) {
        FUN_004ad6a0(auStack_58,&iStack_68,auStack_48,&stack0xffffff78);
        uVar2 = param_1[2];
        if (((uVar2 & 8) == 0) &&
           ((((uVar2 & 0x400) == 0 || ((uVar2 & 4) != 0)) && ((short)param_1[0x1f] < 0)))) {
          (**(code **)(*param_1 + 0xf4))(&iStack_78);
          FUN_004548a0(&iStack_7c,(param_1[2] & 0xffU) >> 2 & 1);
        }
      }
      return iVar4;
    }
    iVar4 = param_2[2];
    param_1[4] = *param_2;
    iVar5 = param_2[1];
    param_1[6] = iVar4;
    param_1[5] = iVar5;
    *(short *)((int)param_1 + 0xe) = (short)param_3;
  }
  return param_1[0x10];
}


