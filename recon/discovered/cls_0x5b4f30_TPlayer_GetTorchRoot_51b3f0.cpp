// FUN_0051b3f0_GetTorchRoot @ 0051b3f0 size=38

char * __thiscall FUN_0051b3f0_GetTorchRoot(int param_1,int param_2)

{
  char *pcVar1;
  
  if (((param_2 == 0) && (param_2 = *(int *)(param_1 + 0x2b8), param_2 == 0)) ||
     (pcVar1 = s_torch_005e2940, *(short *)(param_2 + 4) != 6)) {
    pcVar1 = &DAT_005e2948;
  }
  return pcVar1;
}


