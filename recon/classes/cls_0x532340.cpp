// Decompiled methods and structure for class: cls_0x532340

/*
/OOAnalyzer/cls_0x532340
pack(disabled)
Structure cls_0x532340 {
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   404   word   2   mbr_0x194   "Unsigned Word (dw, 2-bytes)"
   406   word   2   mbr_0x196   "Unsigned Word (dw, 2-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 412 Alignment: 1

*/

// Function at 00532340

void __thiscall OOAnalyzer::cls_0x532340::meth_0x532340(cls_0x532340 *this,uint param_1,int param_2)

{
  word *this_00;
  word wVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  dword dVar5;
  int iVar6;
  cls_0x4746d0 *pcVar7;
  undefined4 *puVar8;
  dword *pdVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  int in_stack_0000000c;
  uint local_c;
  int local_4;
  
  uVar3 = this->mbr_0x17c;
  if ((uVar3 & 8) == 0) {
    if ((uVar3 & 4) == 0) {
      if ((uVar3 & 0x10) != 0) {
        param_1 = 0;
      }
    }
    else {
      param_1 = 2;
    }
  }
  else {
    param_1 = 1;
  }
  if (((uVar3 & 8) == 0) && ((uVar3 & 4) == 0)) {
    pcVar7 = (cls_0x4746d0 *)(-(uint)(1 < DAT_0065a258) & DAT_0065a14c);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(local_4 + 4 + this->mbr_0x198));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          pdVar9 = &pcVar7->mbr_0x24;
          if ((uVar3 < *pdVar9) &&
             (iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)pdVar9),
             uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) {
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)pdVar9);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            if (local_c < (uint)(int)(short)this->mbr_0x194) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = this->mbr_0x194 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                cls_0x533250::meth_0x533250((cls_0x533250 *)&this->mbr_0x194);
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(2 < DAT_0065a258) & DAT_0065a150);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if (iVar2 != -1) {
      this_00 = &this->mbr_0x194;
      local_c = 0;
      if (0 < (short)*this_00) {
        local_4 = 0;
        do {
          uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(local_4 + 4 + this->mbr_0x198))
          ;
          if (uVar3 != 0xffffffff) {
            uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
            if (uVar3 < pcVar7->mbr_0x24) {
              dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
              if (dVar5 == 0) {
                dVar5 = pcVar7->mbr_0x38;
              }
              if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_0053259e;
              iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&pcVar7->mbr_0x24);
              iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
            }
            else {
LAB_0053259e:
              iVar2 = 0;
            }
            if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
              FUN_0052f310();
              if (local_c < (uint)(int)(short)*this_00) {
                iVar2 = (short)this->mbr_0x196 + -1;
                if ((int)local_c < iVar2) {
                  iVar2 = iVar2 - local_c;
                  puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                  do {
                    iVar2 = iVar2 + -1;
                    puVar8 = puVar10 + 0x12;
                    puVar11 = puVar10;
                    for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                      *puVar11 = *puVar8;
                      puVar8 = puVar8 + 1;
                      puVar11 = puVar11 + 1;
                    }
                    puVar10 = puVar10 + 0x12;
                  } while (iVar2 != 0);
                }
                wVar1 = *this_00;
                *this_00 = wVar1 - 1;
                if ((short)(wVar1 - 1) < 1) {
                  cls_0x533250::meth_0x533250((cls_0x533250 *)this_00);
                }
              }
              local_c = local_c - 1;
              local_4 = local_4 + -0x48;
            }
          }
          local_c = local_c + 1;
          local_4 = local_4 + 0x48;
        } while ((int)local_c < (int)(short)*this_00);
      }
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(4 < DAT_0065a258) & DAT_0065a158);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(local_4 + 4 + this->mbr_0x198));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          if (uVar3 < pcVar7->mbr_0x24) {
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_005326e9;
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&pcVar7->mbr_0x24);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
LAB_005326e9:
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            wVar1 = this->mbr_0x194;
            if (local_c < (uint)(int)(short)wVar1) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = wVar1 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                if (this->mbr_0x198 != 0) {
                  FUN_004830f0(this->mbr_0x198);
                }
                this->mbr_0x198 = 0;
                this->mbr_0x194 = 0;
                this->mbr_0x196 = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(0x12 < DAT_0065a258) & DAT_0065a190);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(local_4 + 4 + this->mbr_0x198));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          if (uVar3 < pcVar7->mbr_0x24) {
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_00532872;
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&pcVar7->mbr_0x24);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
LAB_00532872:
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            wVar1 = this->mbr_0x194;
            if (local_c < (uint)(int)(short)wVar1) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = wVar1 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                if (this->mbr_0x198 != 0) {
                  FUN_004830f0(this->mbr_0x198);
                }
                this->mbr_0x198 = 0;
                this->mbr_0x194 = 0;
                this->mbr_0x196 = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(0x15 < DAT_0065a258) & DAT_0065a19c);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(this->mbr_0x198 + 4 + local_4));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          if (uVar3 < pcVar7->mbr_0x24) {
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_005329fb;
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&pcVar7->mbr_0x24);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
LAB_005329fb:
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            wVar1 = this->mbr_0x194;
            if (local_c < (uint)(int)(short)wVar1) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = wVar1 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                if (this->mbr_0x198 != 0) {
                  FUN_004830f0(this->mbr_0x198);
                }
                this->mbr_0x198 = 0;
                this->mbr_0x194 = 0;
                this->mbr_0x196 = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(5 < DAT_0065a258) & DAT_0065a15c);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(this->mbr_0x198 + 4 + local_4));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          if (uVar3 < pcVar7->mbr_0x24) {
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_00532b84;
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&pcVar7->mbr_0x24);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
LAB_00532b84:
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            wVar1 = this->mbr_0x194;
            if (local_c < (uint)(int)(short)wVar1) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = wVar1 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                if (this->mbr_0x198 != 0) {
                  FUN_004830f0(this->mbr_0x198);
                }
                this->mbr_0x198 = 0;
                this->mbr_0x194 = 0;
                this->mbr_0x196 = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
    pcVar7 = (cls_0x4746d0 *)(-(uint)(0x11 < DAT_0065a258) & DAT_0065a18c);
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(this->mbr_0x198 + 4 + local_4));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          if (uVar3 < pcVar7->mbr_0x24) {
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            if ((uint)(int)*(short *)(dVar5 + 0xc) <= uVar4) goto LAB_00532d02;
            dVar5 = *(dword *)(pcVar7->mbr_0x34 + uVar3 * 4);
            if (dVar5 == 0) {
              dVar5 = pcVar7->mbr_0x38;
            }
            iVar2 = *(int *)(*(int *)(dVar5 + 0x10) + uVar4 * 4);
          }
          else {
LAB_00532d02:
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            wVar1 = this->mbr_0x194;
            if (local_c < (uint)(int)(short)wVar1) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = wVar1 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                if (this->mbr_0x198 != 0) {
                  FUN_004830f0(this->mbr_0x198);
                }
                this->mbr_0x198 = 0;
                this->mbr_0x194 = 0;
                this->mbr_0x196 = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
        if ((int)(short)this->mbr_0x194 <= (int)local_c) {
          return;
        }
      } while( true );
    }
  }
  else {
    if (param_1 < DAT_0065a258) {
      pcVar7 = (cls_0x4746d0 *)(&DAT_0065a148)[param_1];
    }
    else {
      pcVar7 = (cls_0x4746d0 *)0x0;
    }
    iVar2 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
    if ((iVar2 != -1) && (local_c = 0, 0 < (short)this->mbr_0x194)) {
      local_4 = 0;
      do {
        uVar3 = cls_0x4746d0::meth_0x475210(pcVar7,*(undefined4 *)(this->mbr_0x198 + 4 + local_4));
        if (uVar3 != 0xffffffff) {
          uVar4 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&pcVar7->mbr_0x14);
          pdVar9 = &pcVar7->mbr_0x24;
          if ((uVar3 < *pdVar9) &&
             (iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)pdVar9),
             uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) {
            iVar2 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)pdVar9);
            iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
            iVar2 = 0;
          }
          if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
            FUN_0052f310();
            if (local_c < (uint)(int)(short)this->mbr_0x194) {
              iVar2 = (short)this->mbr_0x196 + -1;
              if ((int)local_c < iVar2) {
                iVar2 = iVar2 - local_c;
                puVar10 = (undefined4 *)(this->mbr_0x198 + local_4);
                do {
                  iVar2 = iVar2 + -1;
                  puVar8 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar8;
                    puVar8 = puVar8 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar2 != 0);
              }
              wVar1 = this->mbr_0x194 - 1;
              this->mbr_0x194 = wVar1;
              if ((short)wVar1 < 1) {
                cls_0x533250::meth_0x533250((cls_0x533250 *)&this->mbr_0x194);
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)(short)this->mbr_0x194);
    }
  }
  return;
}



