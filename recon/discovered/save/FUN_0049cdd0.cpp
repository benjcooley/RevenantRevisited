// FUN_0049cdd0 @ 0049cdd0 size=40

int __fastcall FUN_0049cdd0(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 4);
  if (*(int *)(param_1 + 0xc) < *(int *)(param_1 + 8) - iVar1) {
    FUN_0054d190(&DAT_0065c5d0,1,s_Output_Stream_Overrun_005daa58);
    iVar1 = 0;
  }
  return iVar1;
}


