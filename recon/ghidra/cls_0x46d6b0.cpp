// Decompiled methods and structure for class: cls_0x46d6b0

/*
/OOAnalyzer/cls_0x46d6b0
pack(disabled)
Structure cls_0x46d6b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   1028   dword   4   mbr_0x404   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1032 Alignment: 1

*/

// Function at 0046d6b0

int __thiscall OOAnalyzer::cls_0x46d6b0::meth_0x46d6b0(cls_0x46d6b0 *this)

{
  byte *pbVar1;
  uint uVar2;
  cls_0x46d6b0 *extraout_ECX;
  cls_0x46d6b0 *pcVar3;
  dword *pdVar4;
  int iVar5;
  byte *in_stack_00000004;
  
  iVar5 = 0;
  if (0 < (int)this->mbr_0x0) {
    pdVar4 = &this->mbr_0x4;
    pcVar3 = this;
    do {
      if (((*pdVar4 != 0) && (pbVar1 = (byte *)(*pdVar4 + (int)pdVar4), pbVar1 != (byte *)0x0)) &&
         (uVar2 = FUN_0059a530((uint)pcVar3,pbVar1,in_stack_00000004), pcVar3 = extraout_ECX,
         uVar2 == 0)) {
        if ((&this->mbr_0x404)[iVar5] == 0) {
          return 0;
        }
        return (int)&this->mbr_0x404 + (&this->mbr_0x404)[iVar5] + iVar5 * 4;
      }
      iVar5 = iVar5 + 1;
      pdVar4 = pdVar4 + 1;
    } while (iVar5 < (int)this->mbr_0x0);
  }
  return 0;
}



// Function at 0046d710

int __thiscall OOAnalyzer::cls_0x46d6b0::meth_0x46d710(cls_0x46d6b0 *this)

{
  byte *pbVar1;
  uint uVar2;
  cls_0x46d6b0 *extraout_ECX;
  cls_0x46d6b0 *pcVar3;
  dword *pdVar4;
  int iVar5;
  byte *in_stack_00000004;
  byte local_50 [80];
  
  iVar5 = 0;
  if (0 < (int)this->mbr_0x0) {
    pdVar4 = &this->mbr_0x4;
    pcVar3 = this;
    do {
      if (((*pdVar4 != 0) && (pbVar1 = (byte *)(*pdVar4 + (int)pdVar4), pbVar1 != (byte *)0x0)) &&
         (uVar2 = FUN_0059a530((uint)pcVar3,pbVar1,in_stack_00000004), pcVar3 = extraout_ECX,
         uVar2 == 0)) {
        if ((&this->mbr_0x404)[iVar5] != 0) {
          iVar5 = (int)&this->mbr_0x404 + (&this->mbr_0x404)[iVar5] + iVar5 * 4;
          if (iVar5 != 0) {
            return iVar5;
          }
          goto LAB_0046d769;
        }
        break;
      }
      iVar5 = iVar5 + 1;
      pdVar4 = pdVar4 + 1;
    } while (iVar5 < (int)this->mbr_0x0);
  }
  iVar5 = 0;
LAB_0046d769:
  FUN_0058b100((char *)local_50,(byte *)s_Unable_to_find___s__in_multireso_005d4718);
  FUN_00481c10(local_50);
  return iVar5;
}



