// FUN_00425c10 @ 00425c10 size=530

undefined4 FUN_00425c10(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined1 auStack_c [4];
  undefined1 auStack_8 [4];
  undefined1 auStack_4 [4];
  
  iVar2 = param_2;
  iVar1 = FUN_00479700(&DAT_005cbee8,0);
  if (iVar1 != 0) {
    if ((param_1[2] & 4U) == 0) {
      (**(code **)(*param_1 + 0x40))(param_1[2] | 4);
      FUN_00454920(param_1[0x10]);
    }
    FUN_00479580();
    return 0;
  }
  iVar1 = FUN_00479700(&PTR_DAT_005cbeec,0);
  if (iVar1 != 0) {
    if ((param_1[2] & 4U) != 0) {
      (**(code **)(*param_1 + 0x40))(param_1[2] & 0xfffffffb);
      FUN_00454920(param_1[0x10]);
    }
    FUN_00479580();
    return 0;
  }
  if (*(int *)(iVar2 + 0x10) == 8) {
    FUN_00479580();
    iVar2 = FUN_0047a410(iVar2,&DAT_005cbef0,&param_2);
    if (iVar2 != 0) {
      FUN_004714e0(param_2);
      return 0;
    }
  }
  else {
    iVar1 = FUN_00479700(s_intensity_005cbef4,1);
    if (iVar1 == 0) {
      iVar1 = FUN_00479700(s_color_005cbf04,1);
      if (iVar1 == 0) {
        iVar1 = FUN_00479700(s_position_005cbf18,1);
        if (iVar1 == 0) {
          iVar1 = FUN_00479700(s_multiplier_005cbf30,1);
          if (iVar1 != 0) {
            FUN_00479580();
            iVar2 = FUN_0047a410(iVar2,&DAT_005cbf3c,&param_2);
            if (iVar2 != 0) {
              FUN_004715e0(param_2);
              return 0;
            }
          }
        }
        else {
          FUN_00479580();
          iVar2 = FUN_0047a410(iVar2,s__d__d__d_005cbf24,auStack_c,auStack_8,auStack_4);
          if (iVar2 != 0) {
            FUN_004716c0(auStack_c);
            return 0;
          }
        }
      }
      else {
        FUN_00479580();
        iVar2 = FUN_0047a410(iVar2,s__b__b__b_005cbf0c,(int)&param_2 + 2,(int)&param_2 + 1,&param_2)
        ;
        if (iVar2 != 0) {
          FUN_00471820(param_2);
          return 0;
        }
      }
    }
    else {
      FUN_00479580();
      iVar2 = FUN_0047a410(iVar2,&DAT_005cbf00,&param_2);
      if (iVar2 != 0) {
        FUN_004714e0(param_2);
        return 0;
      }
    }
  }
  return 4;
}


