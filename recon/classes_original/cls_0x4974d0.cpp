// Decompiled methods and structure for class: cls_0x4974d0

/*
/OOAnalyzer/cls_0x4974d0
pack(disabled)
Structure cls_0x4974d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   16388   dword   4   mbr_0x4004   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16392 Alignment: 1

*/

// Function at 004974d0

undefined4 __thiscall OOAnalyzer::cls_0x4974d0::meth_0x4974d0(cls_0x4974d0 *this)

{
  char cVar1;
  dword dVar2;
  char *pcVar3;
  char **ppcVar4;
  cls_0x5a5ff0 *this_00;
  uint uVar5;
  int iVar6;
  dword *pdVar7;
  char *pcVar8;
  cls_0x5a5ff0 *in_stack_00000004;
  byte local_34 [52];
  
  this_00 = in_stack_00000004;
  dVar2 = this->mbr_0x0;
  if ((int)((in_stack_00000004->mbr_0xc - in_stack_00000004->mbr_0x8) + in_stack_00000004->mbr_0x4)
      < 4) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
  }
  pdVar7 = (dword *)in_stack_00000004->mbr_0x8;
  in_stack_00000004 = (cls_0x5a5ff0 *)0x0;
  *pdVar7 = dVar2;
  this_00->mbr_0x8 = (dword)(pdVar7 + 1);
  if (0 < (int)this->mbr_0x0) {
    pdVar7 = &this->mbr_0x4004;
    do {
      pcVar3 = (char *)*pdVar7;
      uVar5 = 0xffffffff;
      iVar6 = 0;
      pcVar8 = pcVar3;
      do {
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar8 + 1;
      } while (cVar1 != '\0');
      if (0 < (int)(~uVar5 - 1)) {
        do {
          local_34[iVar6] = pcVar3[iVar6] | 0x80;
          uVar5 = 0xffffffff;
          iVar6 = iVar6 + 1;
          pcVar8 = pcVar3;
          do {
            if (uVar5 == 0) break;
            uVar5 = uVar5 - 1;
            cVar1 = *pcVar8;
            pcVar8 = pcVar8 + 1;
          } while (cVar1 != '\0');
        } while (iVar6 < (int)(~uVar5 - 1));
      }
      local_34[iVar6] = 0;
      cls_0x5a5ff0::meth_0x49ccc0(this_00);
      pcVar3 = (char *)pdVar7[-0x1000];
      if ((int)((this_00->mbr_0xc - this_00->mbr_0x8) + this_00->mbr_0x4) < 4) {
        cls_0x5a5ff0::meth_0x49cc70(this_00);
      }
      ppcVar4 = (char **)this_00->mbr_0x8;
      pdVar7 = pdVar7 + 1;
      dVar2 = this->mbr_0x0;
      *ppcVar4 = pcVar3;
      this_00->mbr_0x8 = (dword)(ppcVar4 + 1);
      in_stack_00000004 = (cls_0x5a5ff0 *)((int)in_stack_00000004 + 1);
    } while ((int)in_stack_00000004 < (int)dVar2);
  }
  return 1;
}



