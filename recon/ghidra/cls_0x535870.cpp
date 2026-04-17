// Decompiled methods and structure for class: cls_0x535870

/*
/OOAnalyzer/cls_0x535870
pack(disabled)
Structure cls_0x535870 {
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   472   dword   4   mbr_0x1d8   "Unsigned Double-Word (ddw, 4-bytes)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   484   dword   4   mbr_0x1e4   "Unsigned Double-Word (ddw, 4-bytes)"
   488   dword   4   mbr_0x1e8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 492 Alignment: 1

*/

// Function at 00535870

void __thiscall OOAnalyzer::cls_0x535870::meth_0x535870(cls_0x535870 *this,char *param_1)

{
  char cVar1;
  cls_0x4367d0 **ppcVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  dword *pdVar7;
  undefined4 *puVar8;
  cls_0x4367d0 **ppcVar9;
  char *pcVar10;
  undefined4 *puVar11;
  char *in_stack_00000008;
  
  if ((this->mbr_0x40 != 0) || (this->mbr_0x190 != 0)) {
    if ((-1 < (int)this->mbr_0x1dc) && (this->mbr_0x40 != 0)) {
      ppcVar2 = (cls_0x4367d0 **)this->mbr_0x190;
      if (ppcVar2 != (cls_0x4367d0 **)0x0) {
        iVar6 = 0;
        ppcVar2[0x14] = (cls_0x4367d0 *)0x1;
        ppcVar2[0x16] = (cls_0x4367d0 *)0x0;
        if (0 < (int)ppcVar2[0x17]) {
          ppcVar9 = ppcVar2 + 0x40;
          do {
            if (*ppcVar9 != (cls_0x4367d0 *)0x0) {
              cls_0x4367d0::meth_0x4367d0(*ppcVar2);
              *ppcVar9 = (cls_0x4367d0 *)0x0;
            }
            iVar6 = iVar6 + 1;
            ppcVar9 = ppcVar9 + 1;
          } while (iVar6 < (int)ppcVar2[0x17]);
        }
        this->mbr_0x190 = 0;
      }
      iVar6 = 0;
      if (0 < (int)this->mbr_0x1d8) {
        pdVar7 = &this->mbr_0x1b8;
        do {
          if ((LPCVOID)pdVar7[-8] != (LPCVOID)0x0) {
            FUN_00482f80((LPCVOID)pdVar7[-8]);
          }
          if ((LPCVOID)*pdVar7 != (LPCVOID)0x0) {
            FUN_00482f80((LPCVOID)*pdVar7);
          }
          iVar6 = iVar6 + 1;
          pdVar7 = pdVar7 + 1;
        } while (iVar6 < (int)this->mbr_0x1d8);
      }
      this->mbr_0x1dc = 0xffffffff;
      this->mbr_0x1d8 = 0;
      this->mbr_0x1e4 = 0;
    }
    if ((int)this->mbr_0x1d8 < 8) {
      if (in_stack_00000008 == (char *)0x0) {
        (&this->mbr_0x198)[this->mbr_0x1d8] = 0;
      }
      else {
        uVar4 = 0xffffffff;
        pcVar10 = in_stack_00000008;
        do {
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          cVar1 = *pcVar10;
          pcVar10 = pcVar10 + 1;
        } while (cVar1 != '\0');
        puVar3 = FUN_00482ef0(~uVar4);
        uVar4 = 0xffffffff;
        do {
          pcVar10 = in_stack_00000008;
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          pcVar10 = in_stack_00000008 + 1;
          cVar1 = *in_stack_00000008;
          in_stack_00000008 = pcVar10;
        } while (cVar1 != '\0');
        uVar4 = ~uVar4;
        puVar8 = (undefined4 *)(pcVar10 + -uVar4);
        puVar11 = puVar3;
        for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar11 = *puVar8;
          puVar8 = puVar8 + 1;
          puVar11 = puVar11 + 1;
        }
        for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined *)puVar11 = *(undefined *)puVar8;
          puVar8 = (undefined4 *)((int)puVar8 + 1);
          puVar11 = (undefined4 *)((int)puVar11 + 1);
        }
        (&this->mbr_0x198)[this->mbr_0x1d8] = (dword)puVar3;
      }
      if (param_1 == (char *)0x0) {
        puVar3 = (undefined4 *)0x0;
      }
      else {
        uVar4 = 0xffffffff;
        pcVar10 = param_1;
        do {
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          cVar1 = *pcVar10;
          pcVar10 = pcVar10 + 1;
        } while (cVar1 != '\0');
        puVar3 = FUN_00482ef0(~uVar4);
        uVar4 = 0xffffffff;
        do {
          pcVar10 = param_1;
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          pcVar10 = param_1 + 1;
          cVar1 = *param_1;
          param_1 = pcVar10;
        } while (cVar1 != '\0');
        uVar4 = ~uVar4;
        puVar8 = (undefined4 *)(pcVar10 + -uVar4);
        puVar11 = puVar3;
        for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar11 = *puVar8;
          puVar8 = puVar8 + 1;
          puVar11 = puVar11 + 1;
        }
        for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined *)puVar11 = *(undefined *)puVar8;
          puVar8 = (undefined4 *)((int)puVar8 + 1);
          puVar11 = (undefined4 *)((int)puVar11 + 1);
        }
      }
      (&this->mbr_0x1b8)[this->mbr_0x1d8] = (dword)puVar3;
      this->mbr_0x1d8 = this->mbr_0x1d8 + 1;
    }
  }
  return;
}



// Function at 005883c0

void __thiscall OOAnalyzer::cls_0x535870::meth_0x5883c0(cls_0x535870 *this,dword param_1)

{
  dword in_stack_00000008;
  
  this->mbr_0x194 = param_1;
  this->mbr_0x1e8 = in_stack_00000008;
  return;
}



