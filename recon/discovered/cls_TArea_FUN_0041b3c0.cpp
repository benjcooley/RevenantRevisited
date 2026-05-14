// FUN_0041b3c0_TArea_GetCurrentAmbient @ 0041b3c0 size=394

void __thiscall FUN_0041b3c0_TArea_GetCurrentAmbient(int param_1,int *param_2,char *param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if ((*(byte *)(param_1 + 0x44) & 8) != 0) {
    iVar2 = FUN_0047ea00();
    if (iVar2 == 0) {
      uVar1 = *(undefined4 *)(param_1 + 0xdc);
      *param_2 = *(int *)(param_1 + 0xd4);
      *(undefined4 *)param_3 = uVar1;
      return;
    }
    if (iVar2 != 0xff) {
      iVar6 = 0xff - iVar2;
      *param_2 = (*(int *)(param_1 + 0xd0) * iVar2) / 0xff +
                 (*(int *)(param_1 + 0xd4) * iVar6) / 0xff;
      iVar3 = (uint)*(byte *)(param_1 + 0xde) * iVar6;
      iVar4 = (uint)*(byte *)(param_1 + 0xda) * iVar2;
      iVar5 = (uint)*(byte *)(param_1 + 0xdd) * iVar6;
      param_3[2] = (((char)(iVar3 / 0xff) + (char)(iVar3 >> 0x1f)) -
                   (char)((longlong)iVar3 * 0x80808081 >> 0x3f)) +
                   (((char)(iVar4 / 0xff) + (char)(iVar4 >> 0x1f)) -
                   (char)((longlong)iVar4 * 0x80808081 >> 0x3f));
      iVar3 = (uint)*(byte *)(param_1 + 0xd9) * iVar2;
      iVar6 = (uint)*(byte *)(param_1 + 0xdc) * iVar6;
      param_3[1] = (((char)(iVar5 / 0xff) + (char)(iVar5 >> 0x1f)) -
                   (char)((longlong)iVar5 * 0x80808081 >> 0x3f)) +
                   (((char)(iVar3 / 0xff) + (char)(iVar3 >> 0x1f)) -
                   (char)((longlong)iVar3 * 0x80808081 >> 0x3f));
      iVar2 = (uint)*(byte *)(param_1 + 0xd8) * iVar2;
      *param_3 = (((char)(iVar6 / 0xff) + (char)(iVar6 >> 0x1f)) -
                 (char)((longlong)iVar6 * 0x80808081 >> 0x3f)) +
                 (((char)(iVar2 / 0xff) + (char)(iVar2 >> 0x1f)) -
                 (char)((longlong)iVar2 * 0x80808081 >> 0x3f));
      return;
    }
  }
  uVar1 = *(undefined4 *)(param_1 + 0xd8);
  *param_2 = *(int *)(param_1 + 0xd0);
  *(undefined4 *)param_3 = uVar1;
  return;
}


