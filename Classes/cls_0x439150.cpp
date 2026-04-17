// Decompiled methods and structure for class: cls_0x439150

/*
/OOAnalyzer/cls_0x439150
pack(disabled)
Structure cls_0x439150 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 36 Alignment: 1

*/

// Function at 00439150

undefined4 __thiscall
OOAnalyzer::cls_0x439150::meth_0x439150(cls_0x439150 *this,uint param_1,int param_2)

{
  dword dVar1;
  uint uVar2;
  undefined4 uVar3;
  byte bVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  uint *puVar8;
  uint *puVar9;
  dword dVar10;
  int *piVar11;
  uint uVar12;
  uint in_stack_0000000c;
  uint local_14;
  int local_10;
  int local_8;
  
  if (this->mbr_0x0 != 0) {
    if (param_1 == 0x90) {
      uVar6 = this->mbr_0x1c;
      this->mbr_0x1c = 0;
      this->mbr_0x20 = this->mbr_0x20 | uVar6;
    }
    else {
      dVar1 = this->mbr_0x4;
      local_14 = 0;
      if (0 < (int)dVar1) {
        do {
          if (((this->mbr_0x14 != 0) && (local_14 < this->mbr_0x4)) &&
             (*(int *)(this->mbr_0x14 + local_14 * 4) != 0)) {
            dVar10 = *(dword *)(this->mbr_0x14 + local_14 * 4);
            if (dVar10 == 0) {
              dVar10 = this->mbr_0x18;
            }
            if (((in_stack_0000000c & *(uint *)(dVar10 + 8)) == 0) &&
               (uVar6 = *(uint *)(dVar10 + 0x44), uVar6 != 0)) {
              this->mbr_0x20 = this->mbr_0x20 | uVar6;
              this->mbr_0x1c = this->mbr_0x1c & ~uVar6;
            }
            puVar8 = (uint *)(dVar10 + 0x18);
            local_10 = 3;
            do {
              iVar5 = 0;
              puVar9 = puVar8 + -3;
              do {
                if ((param_1 == *puVar9) &&
                   ((*(uint *)(dVar10 + 0x44) & 0xff) == *(uint *)(dVar10 + 0x44))) {
                  if (param_2 == 0) {
                    *puVar8 = *puVar8 & ~(1 << ((byte)iVar5 & 0x1f));
                    *(undefined4 *)(dVar10 + 0x48) = 0;
                  }
                  else {
                    *puVar8 = *puVar8 | 1 << ((byte)iVar5 & 0x1f);
                  }
                  uVar6 = *(uint *)(dVar10 + 0x44);
                  if (((uVar6 & this->mbr_0x1c) == uVar6) && (param_2 == 0)) {
                    this->mbr_0x1c = ~uVar6 & this->mbr_0x1c;
                    this->mbr_0x20 = this->mbr_0x20 | uVar6;
                  }
                }
                iVar5 = iVar5 + 1;
                puVar9 = puVar9 + 1;
              } while (iVar5 < 3);
              puVar8 = puVar8 + 4;
              local_10 = local_10 + -1;
            } while (local_10 != 0);
          }
          local_14 = local_14 + 1;
        } while ((int)local_14 < (int)dVar1);
      }
      local_14 = 0;
      if (0 < (int)dVar1) {
        do {
          if (((this->mbr_0x14 != 0) && (local_14 < this->mbr_0x4)) &&
             (*(int *)(this->mbr_0x14 + local_14 * 4) != 0)) {
            dVar10 = *(dword *)(this->mbr_0x14 + local_14 * 4);
            if (dVar10 == 0) {
              dVar10 = this->mbr_0x18;
            }
            local_8 = 0;
            piVar11 = (int *)(dVar10 + 0xc);
            do {
              if (0 < *piVar11) {
                iVar5 = 2;
                uVar6 = 4;
                uVar12 = 3;
                piVar7 = piVar11 + 2;
                do {
                  if (0 < *piVar7) break;
                  iVar5 = iVar5 + -1;
                  piVar7 = piVar7 + -1;
                  uVar6 = (int)uVar6 >> 1;
                  uVar12 = (int)uVar12 >> 1;
                } while (0 < iVar5);
                if (piVar11[iVar5] == param_1) {
                  if ((*(int *)(dVar10 + 0x48) == 0) || (bVar4 = 1, param_2 != 0)) {
                    bVar4 = 0;
                  }
                  uVar2 = piVar11[3];
                  if ((bool)((uVar2 & uVar12) == uVar12 | bVar4)) {
                    if (param_2 == 0) {
                      piVar11[3] = ~uVar6 & uVar2;
                    }
                    else {
                      piVar11[3] = uVar2 | uVar6;
                    }
                    if ((*(uint *)(dVar10 + 8) & in_stack_0000000c) != 0) {
                      uVar6 = *(uint *)(dVar10 + 0x44);
                      *(int *)(dVar10 + 0x48) = param_2;
                      this->mbr_0x20 = this->mbr_0x20 | uVar6;
                      if (param_2 == 0) {
                        uVar3 = *(undefined4 *)(dVar10 + 0x40);
                        this->mbr_0x1c = this->mbr_0x1c & ~uVar6;
                        return uVar3;
                      }
                      uVar3 = *(undefined4 *)(dVar10 + 0x3c);
                      this->mbr_0x1c = this->mbr_0x1c | uVar6;
                      return uVar3;
                    }
                  }
                }
                if (-1 < iVar5 + -1) {
                  puVar8 = (uint *)(piVar11 + iVar5 + -1);
                  do {
                    uVar6 = (int)uVar6 >> 1;
                    if (*puVar8 == param_1) {
                      if (param_2 == 0) {
                        piVar11[3] = piVar11[3] & ~uVar6;
                      }
                      else {
                        piVar11[3] = piVar11[3] | uVar6;
                      }
                    }
                    puVar8 = puVar8 + -1;
                    iVar5 = iVar5 + -1;
                  } while (iVar5 != 0);
                }
              }
              piVar11 = piVar11 + 4;
              local_8 = local_8 + 1;
            } while (local_8 < 3);
          }
          local_14 = local_14 + 1;
          if ((int)dVar1 <= (int)local_14) {
            return 0;
          }
        } while( true );
      }
    }
  }
  return 0;
}



