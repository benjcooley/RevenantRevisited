// Decompiled methods and structure for class: cls_0x56c730

/*
/OOAnalyzer/cls_0x56c730
pack(disabled)
Structure cls_0x56c730 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 0056c730

void __thiscall OOAnalyzer::cls_0x56c730::meth_0x56c730(cls_0x56c730 *this,dword param_1)

{
  short sVar1;
  dword dVar2;
  short *psVar3;
  ushort uVar4;
  short sVar5;
  uint uVar6;
  short sVar7;
  uint uVar8;
  int *piVar9;
  uint uVar10;
  uint uVar11;
  int in_stack_00000008;
  
  this->mbr_0x8 = param_1;
  if (this->mbr_0x0 != 0) {
    FUN_004830f0(this->mbr_0x0);
  }
  if (this->mbr_0x4 != 0) {
    FUN_004830f0(this->mbr_0x4);
  }
  dVar2 = FUN_00482fb0(param_1 * 4);
  this->mbr_0x0 = dVar2;
  if (in_stack_00000008 == 0x20) {
    psVar3 = (short *)FUN_00482fb0(param_1 << 4);
    this->mbr_0x4 = (dword)psVar3;
    if (param_1 == 0x10000) {
      uVar6 = 0;
      piVar9 = (int *)this->mbr_0x0;
      do {
        uVar8 = uVar6 & 0xf800;
        uVar10 = uVar6 & 0x7e0;
        uVar11 = uVar6 & 0x1f;
        uVar6 = uVar6 + 1;
        *piVar9 = uVar11 + (uVar10 + uVar8 * 2) * 2;
        piVar9 = piVar9 + 1;
      } while ((int)uVar6 < 0x10000);
      uVar6 = 0;
      do {
        uVar4 = (ushort)uVar6 & 0x3f;
        uVar8 = uVar6 & 0x1fc0;
        uVar10 = uVar6 & 0x7e000;
        if ((uVar6 & 0x20) != 0) {
          uVar4 = 0x1f;
        }
        if ((uVar6 & 0x1000) != 0) {
          uVar8 = 0xfc0;
        }
        if ((uVar6 & 0x40000) != 0) {
          uVar10 = 0x3e000;
        }
        uVar6 = uVar6 + 1;
        *psVar3 = (short)(uVar8 >> 1) + uVar4 + (short)(uVar10 >> 2);
        psVar3 = psVar3 + 1;
      } while ((int)uVar6 < 0x80000);
      return;
    }
    uVar6 = 0;
    if (0 < (int)param_1) {
      piVar9 = (int *)this->mbr_0x0;
      do {
        uVar8 = uVar6 & 0x7c00;
        uVar10 = uVar6 & 0x3e0;
        uVar11 = uVar6 & 0x1f;
        uVar6 = uVar6 + 1;
        *piVar9 = uVar11 + (uVar10 + uVar8 * 2) * 2;
        piVar9 = piVar9 + 1;
      } while ((int)uVar6 < (int)param_1);
    }
    uVar6 = 0;
    if (0 < (int)(param_1 * 8)) {
      do {
        uVar4 = (ushort)uVar6 & 0x3f;
        uVar8 = uVar6 & 0xfc0;
        uVar10 = uVar6 & 0x3f000;
        if ((uVar6 & 0x20) != 0) {
          uVar4 = 0x1f;
        }
        if ((uVar6 & 0x800) != 0) {
          uVar8 = 0x7c0;
        }
        if ((uVar6 & 0x20000) != 0) {
          uVar10 = 0x1f000;
        }
        uVar6 = uVar6 + 1;
        *psVar3 = (short)(uVar8 >> 1) + uVar4 + (short)(uVar10 >> 2);
        psVar3 = psVar3 + 1;
      } while ((int)uVar6 < (int)(param_1 * 8));
      return;
    }
  }
  else {
    psVar3 = (short *)FUN_00482fb0(param_1 * 2);
    this->mbr_0x4 = (dword)psVar3;
    if (param_1 == 0x10000) {
      uVar6 = 0;
      piVar9 = (int *)this->mbr_0x0;
      do {
        uVar8 = uVar6 >> 1;
        uVar6 = uVar6 + 1;
        *piVar9 = (uVar8 & 0x3e0) + (uVar8 & 0xf) + (uVar8 & 0x7800);
        piVar9 = piVar9 + 1;
      } while ((int)uVar6 < 0x10000);
      uVar6 = 0;
      do {
        if ((uVar6 & 0x10) == 0) {
          sVar1 = (short)((uVar6 & 0x1f) << 1);
        }
        else {
          sVar1 = 0x1f;
        }
        if ((uVar6 & 0x400) == 0) {
          sVar7 = (short)((uVar6 & 0x7e0) << 1);
        }
        else {
          sVar7 = 0x7e0;
        }
        if ((uVar6 & 0x8000) == 0) {
          sVar5 = (short)((uVar6 & 0xf800) << 1);
        }
        else {
          sVar5 = -0x800;
        }
        uVar6 = uVar6 + 1;
        *psVar3 = sVar5 + sVar7 + sVar1;
        psVar3 = psVar3 + 1;
      } while ((int)uVar6 < 0x10000);
      return;
    }
    uVar6 = 0;
    if (0 < (int)param_1) {
      piVar9 = (int *)this->mbr_0x0;
      do {
        uVar8 = uVar6 >> 1;
        uVar6 = uVar6 + 1;
        *piVar9 = (uVar8 & 0x3c00) + (uVar8 & 0x1e0) + (uVar8 & 0xf);
        piVar9 = piVar9 + 1;
      } while ((int)uVar6 < (int)param_1);
    }
    uVar6 = 0;
    if (0 < (int)param_1) {
      do {
        if ((uVar6 & 0x10) == 0) {
          sVar1 = (short)((uVar6 & 0x1f) << 1);
        }
        else {
          sVar1 = 0x1f;
        }
        if ((uVar6 & 0x200) == 0) {
          sVar7 = (short)((uVar6 & 0x3e0) << 1);
        }
        else {
          sVar7 = 0x3e0;
        }
        if ((uVar6 & 0x4000) == 0) {
          sVar5 = (short)((uVar6 & 0x7c00) << 1);
        }
        else {
          sVar5 = 0x7c00;
        }
        uVar6 = uVar6 + 1;
        *psVar3 = sVar5 + sVar7 + sVar1;
        psVar3 = psVar3 + 1;
      } while ((int)uVar6 < (int)param_1);
    }
  }
  return;
}



