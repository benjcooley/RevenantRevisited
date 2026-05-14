// FUN_00498020_TSector_Ctor @ 00498020 size=353

undefined4 * __thiscall
FUN_00498020_TSector_Ctor(undefined4 *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db06;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0041c7f0(0x1000,0x40);
  local_4 = 0;
  FUN_0058e839(param_1 + 0x33,0x18,6,FUN_0049a0a0,FUN_0049a100);
  param_1[4] = 0;
  param_1[1] = param_2;
  local_4 = CONCAT31(local_4._1_3_,1);
  param_1[2] = param_3;
  param_1[3] = param_4;
  param_1[0x2c] = 0;
  param_1[0x25] = 0;
  param_1[0x2a] = 1;
  param_1[0x27] = 0;
  param_1[0x26] = 0;
  param_1[0x29] = 0;
  param_1[0x28] = 0;
  param_1[0x2b] = 0;
  if ((undefined4 *)param_1[0x32] != (undefined4 *)0x0) {
    puVar2 = (undefined4 *)param_1[0x32];
    for (uVar3 = param_1[0x30] & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined1 *)puVar2 = 0;
      puVar2 = (undefined4 *)((int)puVar2 + 1);
    }
  }
  param_1[0x2f] = 0;
  param_1[0x2e] = 0;
  piVar5 = param_1 + 0x33;
  param_2 = 6;
  do {
    iVar4 = 0;
    if (0 < *piVar5) {
      do {
        if ((-1 < iVar4) && (iVar1 = *(int *)(piVar5[4] + iVar4 * 4), iVar1 != 0)) {
          FUN_004830f0(iVar1);
        }
        FUN_0041cb40(iVar4);
        iVar4 = iVar4 + 1;
      } while (iVar4 < *piVar5);
    }
    *piVar5 = 0;
    piVar5[1] = 0;
    piVar5 = piVar5 + 6;
    param_2 = param_2 + -1;
  } while (param_2 != 0);
  puVar2 = (undefined4 *)FUN_00482fb0(0x2000);
  *param_1 = puVar2;
  for (iVar4 = 0x800; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  FUN_0058b100(param_1 + 5,s__d__d__d_DAT_005da700,param_1[1],param_1[2],param_1[3]);
  ExceptionList = local_c;
  return param_1;
}


