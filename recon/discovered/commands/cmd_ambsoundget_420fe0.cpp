// FUN_00420fe0 @ 00420fe0 size=90

undefined4 FUN_00420fe0(int param_1)

{
  undefined1 auStack_100 [256];
  
  if (param_1 == 0) {
    return 4;
  }
  FUN_0058b100(auStack_100,s_sound____s__vol___d_range____d___005caf90,param_1 + 0x184,
               *(undefined4 *)(param_1 + 0x1a8),*(undefined4 *)(param_1 + 0x1ac),
               *(undefined4 *)(param_1 + 0x1b0));
  FUN_0041ee50(auStack_100);
  return 0;
}


