// Decompiled methods and structure for class: cls_0x439000

/*
/OOAnalyzer/cls_0x439000
pack(disabled)
Structure cls_0x439000 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 00439000

void __thiscall OOAnalyzer::cls_0x439000::meth_0x439000(cls_0x439000 *this)

{
  dword *this_00;
  int iVar1;
  int iVar2;
  
  if (this->mbr_0x0 != 0) {
    this_00 = &this->mbr_0x4;
    iVar2 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if ((-1 < iVar2) && (iVar1 = *(int *)(this->mbr_0x14 + iVar2 * 4), iVar1 != 0)) {
          FUN_004830f0(iVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)*this_00);
    }
    *this_00 = 0;
    this->mbr_0x8 = 0;
    this->mbr_0x0 = 0;
  }
  return;
}



// Function at 00439060

undefined4 __thiscall OOAnalyzer::cls_0x439000::meth_0x439060(cls_0x439000 *this,uint param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *in_stack_00000008;
  
  if (this->mbr_0x0 == 0) {
    return 0;
  }
  if (this->mbr_0x4 <= param_1) {
    return 0;
  }
  puVar1 = *(undefined4 **)(this->mbr_0x14 + param_1 * 4);
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)this->mbr_0x18;
  }
  for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
    *in_stack_00000008 = *puVar1;
    puVar1 = puVar1 + 1;
    in_stack_00000008 = in_stack_00000008 + 1;
  }
  return 1;
}



// Function at 00439110

void __thiscall OOAnalyzer::cls_0x439000::meth_0x439110(cls_0x439000 *this,uint param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *in_stack_00000008;
  
  if ((this->mbr_0x0 != 0) && (param_1 < this->mbr_0x4)) {
    puVar1 = *(undefined4 **)(this->mbr_0x14 + param_1 * 4);
    if (puVar1 == (undefined4 *)0x0) {
      puVar1 = (undefined4 *)this->mbr_0x18;
    }
    for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar1 = *in_stack_00000008;
      in_stack_00000008 = in_stack_00000008 + 1;
      puVar1 = puVar1 + 1;
    }
  }
  return;
}



// Function at 00439dc0

void __thiscall OOAnalyzer::cls_0x439000::meth_0x439dc0(cls_0x439000 *this)

{
  char cVar1;
  int iVar2;
  dword dVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  int iVar9;
  undefined *puVar10;
  char *pcVar11;
  uint local_90;
  int local_88;
  undefined4 local_84;
  char local_80 [63];
  char acStack_41 [63];
  char acStack_2 [2];
  
  local_84 = this;
  FUN_00482160(s_Controls_005ce634);
  local_90 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if (((this->mbr_0x14 != 0) && (local_90 < this->mbr_0x4)) &&
         (*(int *)(this->mbr_0x14 + local_90 * 4) != 0)) {
        dVar3 = *(dword *)(this->mbr_0x14 + local_90 * 4);
        if (dVar3 == 0) {
          dVar3 = this->mbr_0x18;
        }
        acStack_41[1] = 0;
        piVar7 = (int *)(dVar3 + 0xc);
        local_88 = 0;
        do {
          if (0 < *piVar7) {
            if (0 < local_88) {
              pcVar11 = acStack_41;
              uVar5 = 0xffffffff;
              do {
                pcVar11 = pcVar11 + 1;
                if (uVar5 == 0) break;
                uVar5 = uVar5 - 1;
              } while (*pcVar11 != '\0');
              iVar9 = -(~uVar5 - 1);
              _strncpy(acStack_41 + ~uVar5,&DAT_005ce624,iVar9 + 0x3f);
              (acStack_41 + ~uVar5)[iVar9 + 0x3f] = '\0';
            }
            local_80[0] = '\0';
            if (0 < *piVar7) {
              iVar9 = 0;
              piVar8 = piVar7;
              while (iVar2 = *piVar8, 0 < iVar2) {
                if (((iVar2 < 0x41) || (0x5a < iVar2)) && ((iVar2 < 0x30 || (0x39 < iVar2)))) {
                  iVar6 = 0;
                  piVar4 = &DAT_005cdc98;
                  while (*piVar4 != iVar2) {
                    piVar4 = piVar4 + 2;
                    iVar6 = iVar6 + 1;
                    if ((int *)0x5ce187 < piVar4) goto LAB_00439f43;
                  }
                  puVar10 = (&PTR_DAT_005cdc9c)[iVar6 * 2];
                }
                else {
                  UNK_00656391._3_1_ = (undefined)iVar2;
                  UNK_00656391._4_1_ = 0;
                  puVar10 = (undefined *)0x656394;
                }
                if (puVar10 == (undefined *)0x0) break;
                if (0 < iVar9) {
                  uVar5 = 0xffffffff;
                  pcVar11 = local_80;
                  do {
                    if (uVar5 == 0) break;
                    uVar5 = uVar5 - 1;
                    cVar1 = *pcVar11;
                    pcVar11 = pcVar11 + 1;
                  } while (cVar1 != '\0');
                  FUN_00439fd0(local_80 + (~uVar5 - 1),&DAT_005ce620,0x40 - (~uVar5 - 1));
                }
                uVar5 = 0xffffffff;
                pcVar11 = local_80;
                do {
                  if (uVar5 == 0) break;
                  uVar5 = uVar5 - 1;
                  cVar1 = *pcVar11;
                  pcVar11 = pcVar11 + 1;
                } while (cVar1 != '\0');
                FUN_00439fd0(local_80 + (~uVar5 - 1),puVar10,0x40 - (~uVar5 - 1));
                iVar9 = iVar9 + 1;
                piVar8 = piVar8 + 1;
                if (2 < iVar9) break;
              }
            }
LAB_00439f43:
            pcVar11 = acStack_41;
            uVar5 = 0xffffffff;
            do {
              pcVar11 = pcVar11 + 1;
              if (uVar5 == 0) break;
              uVar5 = uVar5 - 1;
            } while (*pcVar11 != '\0');
            iVar9 = -(~uVar5 - 1);
            _strncpy(acStack_41 + ~uVar5,local_80,iVar9 + 0x3f);
            (acStack_41 + ~uVar5)[iVar9 + 0x3f] = '\0';
            this = local_84;
          }
          local_88 = local_88 + 1;
          piVar7 = piVar7 + 4;
        } while (local_88 < 3);
        FUN_00482600(*(undefined4 *)(dVar3 + 4),acStack_41 + 1);
      }
      local_90 = local_90 + 1;
    } while ((int)local_90 < (int)this->mbr_0x4);
  }
  return;
}



