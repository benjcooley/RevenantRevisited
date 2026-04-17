// Decompiled methods and structure for class: cls_0x4a4f50

/*
/OOAnalyzer/cls_0x4a4f50
pack(disabled)
Structure cls_0x4a4f50 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 004a4f50

cls_0x4a4f50 * __thiscall
OOAnalyzer::cls_0x4a4f50::cls_0x4a4f50(cls_0x4a4f50 *this,cls_0x4a4f50 *param_1,uint param_2)

{
  dword dVar1;
  dword dVar2;
  cls_0x4a4f50 *pcVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  cls_0x4a4f50 *pcVar7;
  undefined4 *puVar8;
  cls_0x4a4f50 *pcVar9;
  uint uVar10;
  cls_0x4a4f50 *pcVar11;
  cls_0x4a4f50 *pcVar12;
  undefined4 *puVar13;
  uint uVar14;
  undefined4 *in_stack_0000000c;
  
  uVar14 = param_2;
  pcVar7 = (cls_0x4a4f50 *)this->mbr_0x8;
  if (param_2 <= (uint)((int)(this->mbr_0xc - (int)pcVar7) >> 4)) {
    uVar14 = (int)pcVar7 - (int)param_1 >> 4;
    if (param_2 <= uVar14) {
      if (param_2 != 0) {
        pcVar12 = pcVar7 + -param_2;
        pcVar9 = pcVar7;
        for (pcVar11 = pcVar12; pcVar3 = pcVar7, pcVar11 != pcVar7; pcVar11 = pcVar11 + 1) {
          if (pcVar9 != (cls_0x4a4f50 *)0x0) {
            *(undefined4 *)pcVar9 = *(undefined4 *)pcVar11;
            pcVar9->mbr_0x4 = pcVar11->mbr_0x4;
            pcVar9->mbr_0x8 = pcVar11->mbr_0x8;
            pcVar9->mbr_0xc = pcVar11->mbr_0xc;
          }
          pcVar9 = pcVar9 + 1;
        }
        while (param_1 != pcVar12) {
          *(undefined4 *)(pcVar3 + -1) = *(undefined4 *)(pcVar12 + -1);
          pcVar3[-1].mbr_0x4 = pcVar12[-1].mbr_0x4;
          pcVar3[-1].mbr_0x8 = pcVar12[-1].mbr_0x8;
          pcVar3[-1].mbr_0xc = pcVar12[-1].mbr_0xc;
          pcVar3 = pcVar3 + -1;
          pcVar12 = pcVar12 + -1;
        }
        pcVar9 = param_1 + param_2;
        if (param_1 != pcVar9) {
          do {
            pcVar11 = param_1 + 1;
            *(undefined4 *)param_1 = *in_stack_0000000c;
            param_1->mbr_0x4 = in_stack_0000000c[1];
            param_1->mbr_0x8 = in_stack_0000000c[2];
            param_1->mbr_0xc = in_stack_0000000c[3];
            param_1 = pcVar11;
          } while (pcVar11 != pcVar9);
        }
        this->mbr_0x8 = (dword)(pcVar7 + param_2);
      }
      return pcVar7;
    }
    pcVar9 = param_1 + param_2;
    if (param_1 != pcVar7) {
      pcVar11 = pcVar9 + -param_2;
      do {
        if (pcVar9 != (cls_0x4a4f50 *)0x0) {
          *(undefined4 *)pcVar9 = *(undefined4 *)pcVar11;
          pcVar9->mbr_0x4 = pcVar11->mbr_0x4;
          pcVar9->mbr_0x8 = pcVar11->mbr_0x8;
          pcVar9->mbr_0xc = pcVar11->mbr_0xc;
        }
        pcVar11 = pcVar11 + 1;
        pcVar9 = pcVar9 + 1;
      } while (pcVar11 != pcVar7);
    }
    pcVar9 = pcVar7;
    for (iVar4 = param_2 - uVar14; iVar4 != 0; iVar4 = iVar4 + -1) {
      if (pcVar9 != (cls_0x4a4f50 *)0x0) {
        *(undefined4 *)pcVar9 = *in_stack_0000000c;
        pcVar9->mbr_0x4 = in_stack_0000000c[1];
        dVar1 = in_stack_0000000c[3];
        pcVar9->mbr_0x8 = in_stack_0000000c[2];
        pcVar9->mbr_0xc = dVar1;
      }
      pcVar9 = pcVar9 + 1;
    }
    if (param_1 != pcVar7) {
      do {
        pcVar9 = param_1 + 1;
        *(undefined4 *)param_1 = *in_stack_0000000c;
        param_1->mbr_0x4 = in_stack_0000000c[1];
        param_1->mbr_0x8 = in_stack_0000000c[2];
        param_1->mbr_0xc = in_stack_0000000c[3];
        param_1 = pcVar9;
      } while (pcVar9 != pcVar7);
    }
    this->mbr_0x8 = (dword)(pcVar7 + param_2);
    return this;
  }
  dVar1 = this->mbr_0x4;
  if ((dVar1 == 0) || (uVar10 = (int)((int)pcVar7 - dVar1) >> 4, uVar10 <= param_2)) {
    uVar10 = param_2;
  }
  if (dVar1 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (int)((int)pcVar7 - dVar1) >> 4;
  }
  iVar4 = iVar4 + uVar10;
  iVar5 = iVar4;
  if (iVar4 < 0) {
    iVar5 = 0;
  }
  puVar6 = (undefined4 *)FUN_00482fb0(iVar5 << 4);
  puVar13 = puVar6;
  for (pcVar7 = (cls_0x4a4f50 *)this->mbr_0x4; pcVar7 != param_1; pcVar7 = pcVar7 + 1) {
    if (puVar13 != (undefined4 *)0x0) {
      *puVar13 = *(undefined4 *)pcVar7;
      puVar13[1] = pcVar7->mbr_0x4;
      puVar13[2] = pcVar7->mbr_0x8;
      puVar13[3] = pcVar7->mbr_0xc;
    }
    puVar13 = puVar13 + 4;
  }
  puVar8 = puVar13;
  if (param_2 != 0) {
    do {
      if (puVar8 != (undefined4 *)0x0) {
        *puVar8 = *in_stack_0000000c;
        puVar8[1] = in_stack_0000000c[1];
        puVar8[2] = in_stack_0000000c[2];
        puVar8[3] = in_stack_0000000c[3];
      }
      param_2 = param_2 - 1;
      puVar8 = puVar8 + 4;
    } while (param_2 != 0);
  }
  pcVar7 = (cls_0x4a4f50 *)this->mbr_0x8;
  puVar8 = puVar13 + uVar14 * 4;
  if (param_1 != pcVar7) {
    pcVar9 = (cls_0x4a4f50 *)((int)puVar8 + (uVar14 * -0x10 - (int)puVar13) + (int)param_1);
    do {
      if (puVar8 != (undefined4 *)0x0) {
        *puVar8 = *(undefined4 *)pcVar9;
        puVar8[1] = pcVar9->mbr_0x4;
        puVar8[2] = pcVar9->mbr_0x8;
        puVar8[3] = pcVar9->mbr_0xc;
      }
      pcVar9 = pcVar9 + 1;
      puVar8 = puVar8 + 4;
    } while (pcVar9 != pcVar7);
  }
  FUN_004830f0(this->mbr_0x4);
  dVar1 = this->mbr_0x4;
  this->mbr_0xc = (dword)(puVar6 + iVar4 * 4);
  if (dVar1 != 0) {
    dVar2 = this->mbr_0x8;
    this->mbr_0x4 = (dword)puVar6;
    this->mbr_0x8 = (dword)(cls_0x4a4f50 *)(puVar6 + (((int)(dVar2 - dVar1) >> 4) + uVar14) * 4);
    return (cls_0x4a4f50 *)(puVar6 + (((int)(dVar2 - dVar1) >> 4) + uVar14) * 4);
  }
  this->mbr_0x4 = (dword)puVar6;
  this->mbr_0x8 = (dword)(cls_0x4a4f50 *)(puVar6 + uVar14 * 4);
  return (cls_0x4a4f50 *)(puVar6 + uVar14 * 4);
}



// Function at 004a5590

int __thiscall OOAnalyzer::cls_0x4a4f50::meth_0x4a5590(cls_0x4a4f50 *this)

{
  if (this->mbr_0x4 == 0) {
    return 0;
  }
  return (int)(this->mbr_0x8 - this->mbr_0x4) >> 4;
}



