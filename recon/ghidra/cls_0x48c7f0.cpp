// Decompiled methods and structure for class: cls_0x48c7f0

/*
/OOAnalyzer/cls_0x48c7f0
pack(disabled)
Structure cls_0x48c7f0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 84 Alignment: 1

*/

// Function at 0048c7f0

void __thiscall OOAnalyzer::cls_0x48c7f0::meth_0x48c7f0(cls_0x48c7f0 *this)

{
  cls_0x4746d0 *this_00;
  int iVar1;
  dword dVar2;
  uint uVar3;
  cls_0x4746d0 **ppcVar4;
  uint in_stack_00000004;
  
  uVar3 = 0;
  ppcVar4 = (cls_0x4746d0 **)&DAT_0065a148_TObjectClass_classes;
  while (((DAT_0065a258_TObjectClass_numclasses <= uVar3 || (this_00 = *ppcVar4, this_00 == (cls_0x4746d0 *)0x0)) ||
         (iVar1 = cls_0x4746d0::meth_0x475210(this_00,in_stack_00000004), iVar1 < 0))) {
    ppcVar4 = ppcVar4 + 1;
    uVar3 = uVar3 + 1;
    dVar2 = in_stack_00000004;
    if (0x65a247 < (int)ppcVar4) {
LAB_0048c836:
      switch(dVar2) {
      case 1:
        iVar1 = 0;
        if (0 < (int)this->mbr_0x2c) {
          do {
            uVar3 = FUN_0059a530_stricmp(in_stack_00000004,*(byte **)(this->mbr_0x3c + iVar1 * 4),
                                 (byte *)in_stack_00000004);
            if (uVar3 == 0) {
              cls_0x48af30::meth_0x48af30(*(cls_0x48af30 **)(this->mbr_0x3c + iVar1 * 4));
              return;
            }
            iVar1 = iVar1 + 1;
          } while (iVar1 < (int)this->mbr_0x2c);
          return;
        }
        break;
      case 2:
        iVar1 = 0;
        if (0 < (int)this->mbr_0x40) {
          while (uVar3 = FUN_0059a530_stricmp(in_stack_00000004,*(byte **)(this->mbr_0x50 + iVar1 * 4),
                                      (byte *)in_stack_00000004), uVar3 != 0) {
            iVar1 = iVar1 + 1;
            if ((int)this->mbr_0x40 <= iVar1) {
              return;
            }
          }
          cls_0x48b4a0::meth_0x48b4a0(*(cls_0x48b4a0 **)(this->mbr_0x50 + iVar1 * 4));
        }
        break;
      case 0xb:
      case 0xc:
        iVar1 = 0;
        if (0 < (int)this->mbr_0x18) {
          while (uVar3 = FUN_0059a530_stricmp(in_stack_00000004,*(byte **)(this->mbr_0x28 + iVar1 * 4),
                                      (byte *)in_stack_00000004), uVar3 != 0) {
            iVar1 = iVar1 + 1;
            if ((int)this->mbr_0x18 <= iVar1) {
              return;
            }
          }
        }
      }
      return;
    }
  }
  dVar2 = this_00->mbr_0x8;
  goto LAB_0048c836;
}



// Function at 0048c930

undefined4 __thiscall OOAnalyzer::cls_0x48c7f0::meth_0x48c930(cls_0x48c7f0 *this,uint param_1)

{
  cls_0x4746d0 *this_00;
  int iVar1;
  dword dVar2;
  cls_0x4746d0 **ppcVar3;
  uint uVar4;
  undefined4 in_stack_00000008;
  dword local_4;
  
  local_4 = 0xffffffff;
  uVar4 = 0;
  ppcVar3 = (cls_0x4746d0 **)&DAT_0065a148_TObjectClass_classes;
  do {
    if (((uVar4 < DAT_0065a258_TObjectClass_numclasses) && (this_00 = *ppcVar3, this_00 != (cls_0x4746d0 *)0x0)) &&
       (iVar1 = cls_0x4746d0::meth_0x475210(this_00,param_1), -1 < iVar1)) {
      local_4 = this_00->mbr_0x8;
      break;
    }
    ppcVar3 = ppcVar3 + 1;
    uVar4 = uVar4 + 1;
  } while ((int)ppcVar3 < 0x65a248);
  switch(local_4) {
  case 1:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x2c) {
      do {
        uVar4 = FUN_0059a530_stricmp(param_1,*(byte **)(this->mbr_0x3c + iVar1 * 4),(byte *)param_1);
        if (uVar4 == 0) {
          iVar1 = *(int *)(this->mbr_0x3c + iVar1 * 4);
          *(undefined4 *)(iVar1 + 0xc4) = in_stack_00000008;
          *(dword *)(iVar1 + 200) = local_4;
          return 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < (int)this->mbr_0x2c);
      return 0;
    }
    break;
  case 2:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x40) {
      while (uVar4 = FUN_0059a530_stricmp(param_1,*(byte **)(this->mbr_0x50 + iVar1 * 4),(byte *)param_1),
            uVar4 != 0) {
        iVar1 = iVar1 + 1;
        if ((int)this->mbr_0x40 <= iVar1) {
          return 0;
        }
      }
      dVar2 = this->mbr_0x50;
LAB_0048ca61:
      iVar1 = *(int *)(dVar2 + iVar1 * 4);
      *(undefined4 *)(iVar1 + 0xc0) = in_stack_00000008;
      *(dword *)(iVar1 + 0xc4) = local_4;
      return 1;
    }
    break;
  case 0xb:
  case 0xc:
    iVar1 = 0;
    if (0 < (int)this->mbr_0x18) {
      do {
        uVar4 = FUN_0059a530_stricmp(param_1,*(byte **)(this->mbr_0x28 + iVar1 * 4),(byte *)param_1);
        if (uVar4 == 0) {
          dVar2 = this->mbr_0x28;
          goto LAB_0048ca61;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < (int)this->mbr_0x18);
    }
  }
  return 0;
}



