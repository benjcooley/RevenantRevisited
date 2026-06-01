// FUN_00537a70 @ 00537a70 size=1300

/* WARNING: Removing unreachable block (ram,0x00537be3) */
/* WARNING: Removing unreachable block (ram,0x00537bef) */
/* WARNING: Removing unreachable block (ram,0x00537bff) */
/* WARNING: Removing unreachable block (ram,0x00537c16) */
/* WARNING: Removing unreachable block (ram,0x00537c22) */
/* WARNING: Removing unreachable block (ram,0x00537c2c) */
/* WARNING: Removing unreachable block (ram,0x00537c89) */
/* WARNING: Removing unreachable block (ram,0x00537c62) */
/* WARNING: Removing unreachable block (ram,0x00537cb3) */
/* WARNING: Removing unreachable block (ram,0x00537ccf) */
/* WARNING: Removing unreachable block (ram,0x00537cdb) */
/* WARNING: Removing unreachable block (ram,0x00537d38) */
/* WARNING: Removing unreachable block (ram,0x00537d48) */
/* WARNING: Removing unreachable block (ram,0x00537d5a) */
/* WARNING: Removing unreachable block (ram,0x00537df4) */
/* WARNING: Removing unreachable block (ram,0x00537e03) */
/* WARNING: Removing unreachable block (ram,0x00537e54) */
/* WARNING: Removing unreachable block (ram,0x00537e58) */
/* WARNING: Removing unreachable block (ram,0x00537e6f) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00537a70(int *param_1,int *param_2)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined1 auStack_190 [400];
  
  (**(code **)(*param_1 + 0x90))();
  if (param_2 != (int *)PTR_DAT_005d79e0) {
    (**(code **)(*param_2 + 0x40))(0,0x132);
  }
  (**(code **)(*param_2 + 100))(0,0,0xbc,0xae,0,0xffff,0,0x80000000);
  FUN_004bd680(0,0,param_1[0x66],0x80000000,0);
  if ((int *)param_1[0x5f] == DAT_00667fcc) {
    iVar2 = param_1[0x68];
  }
  else {
    iVar2 = param_1[0x6a];
  }
  FUN_00438df0(10,10,iVar2,0,0,*(undefined4 *)(iVar2 + 4),*(undefined4 *)(iVar2 + 8),0x100,0,0);
  if (DAT_00667fcc != (int *)0x0) {
    uVar1 = (**(code **)(*DAT_00667fcc + 0x84))(&DAT_005e41a0);
    FUN_0058b100(auStack_190,&DAT_005e41a8,uVar1);
    FUN_004be110(auStack_190,0x50,0xd,1,DAT_006663dc,0,0x80000000,0xffffffff,0,1,0xffffffff,0);
    FUN_004bd680(0x28,5,param_1[0x67],0x2000,0);
  }
  iVar2 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  *(undefined4 *)(iVar2 + 0x18) = _DAT_006668d0;
  FUN_0046dfb0();
  FUN_004830f0(iVar2);
  if (param_1[0x60] < 0) {
    param_1[0x61] = -1;
  }
  if (param_1[0x62] < 1) {
    if (param_1[0x22] == 0) {
      (**(code **)(iRam00000000 + 0x1c))(uRam00000014 | 4);
    }
    else {
      (**(code **)(**(int **)param_1[0x26] + 0x1c))((*(int **)param_1[0x26])[5] | 4);
    }
  }
  else {
    if (param_1[0x22] == 0) {
      piVar4 = (int *)0x0;
    }
    else {
      piVar4 = *(int **)param_1[0x26];
    }
    (**(code **)(*piVar4 + 0x1c))(piVar4[5] & 0xfffffffb);
  }
  if (param_1[0x62] < 0xf3) {
    if ((uint)param_1[0x22] < 2) {
      piVar4 = (int *)0x0;
    }
    else {
      piVar4 = *(int **)(param_1[0x26] + 4);
    }
    iVar2 = *piVar4;
    uVar3 = piVar4[5] & 0xfffffffb;
  }
  else if ((uint)param_1[0x22] < 2) {
    uVar3 = uRam00000014 | 4;
    iVar2 = iRam00000000;
  }
  else {
    uVar3 = (*(int **)(param_1[0x26] + 4))[5] | 4;
    iVar2 = **(int **)(param_1[0x26] + 4);
  }
  (**(code **)(iVar2 + 0x1c))(uVar3);
  if (param_2 == (int *)PTR_DAT_005d79e0) {
    FUN_004aacb0(param_1[1],param_1[2],param_1[3],param_1[4],6);
    return;
  }
  FUN_00435cb0(param_2);
  (**(code **)(*param_2 + 0x40))(0,0);
  return;
}


