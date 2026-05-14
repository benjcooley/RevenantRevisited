// FUN_00498190_TSector_Dtor @ 00498190 size=277

void __fastcall FUN_00498190_TSector_Dtor(int *param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  void *local_c;
  undefined1 *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059db36;
  local_c = ExceptionList;
  local_4 = 1;
  ExceptionList = &local_c;
  FUN_00499ff0_TSector_DispatchNotifyToAll(2,param_1);
  DAT_006687a4 = param_1;
  if (*param_1 != 0) {
    FUN_004830f0(*param_1);
    *param_1 = 0;
  }
  piVar2 = param_1 + 0x33;
  iVar1 = 6;
  do {
    iVar3 = 0;
    if (0 < *piVar2) {
      do {
        FUN_0049a1a0(iVar3);
        iVar3 = iVar3 + 1;
      } while (iVar3 < *piVar2);
    }
    *piVar2 = 0;
    piVar2[1] = 0;
    piVar2 = piVar2 + 6;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  piVar2 = (int *)param_1[0x32];
  uVar4 = 0;
LAB_00498221:
  do {
    if (((uint *)(param_1 + 0x2e) == (uint *)0x0) || ((uint)param_1[0x2e] <= uVar4)) {
      DAT_006687a4 = (int *)0x0;
      local_4 = local_4 & 0xffffff00;
      FUN_0058e745(param_1 + 0x33,0x18,6,FUN_0049a100);
      local_4 = 0xffffffff;
      FUN_004830f0(param_1[0x32]);
      ExceptionList = local_c;
      return;
    }
    iVar1 = *piVar2;
    if (iVar1 != 0) {
      if ((*(uint *)(iVar1 + 8) & 0x80000) != 0) {
        piVar2 = piVar2 + 1;
        *(undefined4 *)(iVar1 + 0x44) = 0;
        uVar4 = uVar4 + 1;
        goto LAB_00498221;
      }
      FUN_0046e6d0(iVar1);
      FUN_0041cb40(uVar4);
    }
    piVar2 = piVar2 + 1;
    uVar4 = uVar4 + 1;
  } while( true );
}


