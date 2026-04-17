// Decompiled methods and structure for class: cls_0x48e630

/*
/OOAnalyzer/cls_0x48e630
pack(disabled)
Structure cls_0x48e630 {
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 36 Alignment: 1

*/

// Function at 0048e630

undefined4 __thiscall OOAnalyzer::cls_0x48e630::meth_0x48e630(cls_0x48e630 *this,int param_1)

{
  int *piVar1;
  int iVar2;
  int in_stack_00000008;
  
  piVar1 = (int *)this->mbr_0x1c;
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

void __thiscall OOAnalyzer::cls_0x48e630::meth_0x48e670(cls_0x48e630 *this,undefined4 param_1)

{
  dword dVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 in_stack_00000008;
  
  if ((0 < (int)this->mbr_0x18) && (this->mbr_0x1c != 0)) {
    if ((int)this->mbr_0x20 < (int)(this->mbr_0x18 + 1)) {
      dVar1 = this->mbr_0x20 + 0x20;
      this->mbr_0x20 = dVar1;
      puVar2 = (undefined4 *)FUN_00482fb0(dVar1 * 8);
      puVar3 = (undefined4 *)this->mbr_0x1c;
      puVar6 = puVar3;
      puVar7 = puVar2;
      for (uVar4 = (this->mbr_0x18 << 3) >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
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
      puVar3 = (undefined4 *)FUN_00482fb0(this->mbr_0x20 << 3);
      this->mbr_0x1c = (dword)puVar3;
      for (uVar4 = (this->mbr_0x18 << 3) >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
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
    *(undefined4 *)(this->mbr_0x1c + this->mbr_0x18 * 8) = param_1;
    *(undefined4 *)(this->mbr_0x1c + 4 + this->mbr_0x18 * 8) = in_stack_00000008;
    this->mbr_0x18 = this->mbr_0x18 + 1;
    return;
  }
  if (this->mbr_0x1c != 0) {
    FUN_004830f0(this->mbr_0x1c);
  }
  puVar3 = (undefined4 *)FUN_00482fb0(this->mbr_0x20 << 3);
  this->mbr_0x1c = (dword)puVar3;
  *puVar3 = param_1;
  this->mbr_0x18 = 1;
  puVar3[1] = in_stack_00000008;
  return;
}



