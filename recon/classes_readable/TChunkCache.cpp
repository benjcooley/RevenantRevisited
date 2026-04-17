#include "TChunkCache.h"

// Decompiled methods and structure for class: TChunkCache



// Function at 0048e630

undefined4 TChunkCache::meth_0x48e630(int param_1)

{
  int *piVar1;
  int iVar2;
  int in_stack_00000008;
  
  piVar1 = (int *)this->currentcycle16;
  if (piVar1 != (int *)0x0) {
    iVar2 = 0;
    if (0 < (int)this->mbr_0x20) {
      do {
        if ((*piVar1 == param_1) && (piVar1[1] == in_stack_00000008)) {
          return 1;
        }
        iVar2 = iVar2 + 1;
        piVar1 = piVar1 + 2;
      } while (iVar2 < (int)this->mbr_0x20);
    }
  }
  return 0;
}



// Function at 0048e670

void TChunkCache::meth_0x48e670(undefined4 param_1)

{
  dword dVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 in_stack_00000008;
  
  if ((0 < (int)this->currentcycle) && (this->currentcycle16 != 0)) {
    if ((int)this->mbr_0x20 < (int)(this->currentcycle + 1)) {
      dVar1 = this->mbr_0x20 + 0x20;
      this->mbr_0x20 = dVar1;
      puVar2 = FUN_00482fb0(dVar1 * 8);
      puVar3 = (undefined4 *)this->currentcycle16;
      puVar6 = puVar3;
      puVar7 = puVar2;
      for (uVar4 = (this->currentcycle << 3) >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar7 = *puVar6;
        puVar6 = puVar6 + 1;
        puVar7 = puVar7 + 1;
      }
      for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined *)puVar7 = *(undefined *)puVar6;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
        puVar7 = (undefined4 *)((int)puVar7 + 1);
      }
      FUN_004830f0(puVar3);
      puVar3 = FUN_00482fb0(this->mbr_0x20 << 3);
      this->currentcycle16 = (dword)puVar3;
      for (uVar4 = (this->currentcycle << 3) >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar3 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar3 = puVar3 + 1;
      }
      for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined *)puVar3 = *(undefined *)puVar2;
        puVar2 = (undefined4 *)((int)puVar2 + 1);
        puVar3 = (undefined4 *)((int)puVar3 + 1);
      }
    }
    *(undefined4 *)(this->currentcycle16 + this->currentcycle * 8) = param_1;
    *(undefined4 *)(this->currentcycle16 + 4 + this->currentcycle * 8) = in_stack_00000008;
    this->currentcycle = this->currentcycle + 1;
    return;
  }
  if ((LPCVOID)this->currentcycle16 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->currentcycle16);
  }
  puVar3 = FUN_00482fb0(this->mbr_0x20 << 3);
  this->currentcycle16 = (dword)puVar3;
  *puVar3 = param_1;
  this->currentcycle = 1;
  puVar3[1] = in_stack_00000008;
  return;
}



