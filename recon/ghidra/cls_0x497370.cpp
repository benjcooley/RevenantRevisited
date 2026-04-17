// Decompiled methods and structure for class: cls_0x497370

/*
/OOAnalyzer/cls_0x497370
pack(disabled)
Structure cls_0x497370 {
   32772   dword   4   mbr_0x8004   "Unsigned Double-Word (ddw, 4-bytes)"
   32788   dword   4   mbr_0x8014   "Unsigned Double-Word (ddw, 4-bytes)"
   32792   dword   4   mbr_0x8018   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32796 Alignment: 1

*/

// Function at 00497370

cls_0x492170 * __thiscall OOAnalyzer::cls_0x497370::meth_0x497370(cls_0x497370 *this)

{
  byte *pbVar1;
  uint uVar2;
  cls_0x492170 *pcVar3;
  dword dVar4;
  uint uVar5;
  int in_stack_00000004;
  
  uVar5 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if ((((this->mbr_0x8014 != 0) && (uVar5 < this->mbr_0x8004)) &&
          (*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0)) &&
         (((pbVar1 = **(byte ***)(this->mbr_0x8014 + uVar5 * 4), pbVar1 != (byte *)0x0 &&
           (*pbVar1 != 0)) &&
          (uVar2 = FUN_0059a530(*(uint *)(in_stack_00000004 + 0x38),pbVar1,
                                (byte *)*(uint *)(in_stack_00000004 + 0x38)), uVar2 == 0)))) {
        pcVar3 = (cls_0x492170 *)FUN_00482fb0(0xe8);
        if (pcVar3 == (cls_0x492170 *)0x0) goto LAB_00497498;
        pcVar3 = cls_0x492170::cls_0x492170(pcVar3,in_stack_00000004);
        goto LAB_0049749a;
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)this->mbr_0x8004);
  }
  uVar5 = 0;
  if (0 < (int)this->mbr_0x8004) {
    do {
      if (((this->mbr_0x8014 != 0) && (uVar5 < this->mbr_0x8004)) &&
         ((*(int *)(this->mbr_0x8014 + uVar5 * 4) != 0 &&
          (((pbVar1 = **(byte ***)(this->mbr_0x8014 + uVar5 * 4), pbVar1 != (byte *)0x0 &&
            (*pbVar1 != 0)) &&
           (uVar2 = FUN_0059a530(**(uint **)(in_stack_00000004 + 0x4c),pbVar1,
                                 (byte *)**(uint **)(in_stack_00000004 + 0x4c)), uVar2 == 0)))))) {
        pcVar3 = (cls_0x492170 *)FUN_00482fb0(0xe8);
        if (pcVar3 == (cls_0x492170 *)0x0) {
LAB_00497498:
          pcVar3 = (cls_0x492170 *)0x0;
        }
        else {
          pcVar3 = cls_0x492170::cls_0x492170(pcVar3,in_stack_00000004);
        }
LAB_0049749a:
        dVar4 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x8018);
        pcVar3->mbr_0x14 = dVar4;
        return pcVar3;
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)this->mbr_0x8004);
  }
  return (cls_0x492170 *)0x0;
}



