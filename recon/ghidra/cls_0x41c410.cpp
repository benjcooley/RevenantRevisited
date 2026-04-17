// Decompiled methods and structure for class: cls_0x41c410

/*
/OOAnalyzer/cls_0x41c410
pack(disabled)
Structure cls_0x41c410 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 0041c410

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x41c410::meth_0x41c410(cls_0x41c410 *this)

{
  int iVar1;
  dword dVar2;
  cls_0x41b240 *this_00;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int local_14;
  cls_0x41c750 local_c;
  
  cls_0x41c750::meth_0x41c750(&local_c);
  uVar3 = _DAT_00666970;
  iVar5 = 0;
  local_14 = -1;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((iVar5 == 0) ||
         (iVar4 = cls_0x41b0b0::meth_0x41b0b0
                            (*(cls_0x41b0b0 **)(this->mbr_0x14 + iVar5 * 4),&local_c), iVar4 != 0))
      {
        local_14 = iVar5;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x4);
    if ((-1 < local_14) && ((*(byte *)(*(int *)(this->mbr_0x14 + local_14 * 4) + 0x44) & 2) == 0)) {
      iVar5 = 0;
      if (0 < (int)this->mbr_0x4) {
        do {
          iVar4 = *(int *)(this->mbr_0x14 + iVar5 * 4);
          if (((*(byte *)(iVar4 + 0x44) & 2) != 0) && ((*(uint *)(iVar4 + 0x44) & 2) != 0)) {
            *(uint *)(iVar4 + 0x44) = *(uint *)(iVar4 + 0x44) & 0xfffffffd;
            if (*(int *)(iVar4 + 0x25c) != 0) {
              SoftwareRenderer::meth_0x4defe0(*(SoftwareRenderer **)(*(int *)(iVar4 + 0x25c) + 0x1c));
              iVar1 = *(int *)(iVar4 + 0x25c);
              *(undefined4 *)(iVar1 + 0x1c) = 0;
              *(undefined4 *)(iVar1 + 0x20) = 0;
            }
            if ((*(byte *)(iVar4 + 0x44) & 0x20) != 0) {
              cls_0x41c7b0::meth_0x41c7b0((cls_0x41c7b0 *)&DAT_0065abc8);
            }
            if ((*(byte *)(iVar4 + 0x44) & 0x40) != 0) {
              cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
              cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
              cls_0x41c7d0::meth_0x41c7d0((cls_0x41c7d0 *)&DAT_00667548,iVar4 + 0x138);
            }
            if ((DAT_0066829c == 0) && ((*(byte *)(iVar4 + 0x44) & 0x10) != 0)) {
              cls_0x495cf0::meth_0x4967a0((cls_0x495cf0 *)&DAT_0065def0);
            }
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)this->mbr_0x4);
      }
      cls_0x41b240::meth_0x41ba00(*(cls_0x41b240 **)(this->mbr_0x14 + local_14 * 4));
      dVar2 = *(dword *)(this->mbr_0x14 + local_14 * 4);
      this->mbr_0x18 = dVar2;
      if ((*(uint *)(dVar2 + 0x44) & 0x4000) != 0) {
        *(undefined4 *)(this + 1) = *(undefined4 *)(dVar2 + 0x108);
        this[1].mbr_0x4 = *(dword *)(dVar2 + 0x10c);
        *(undefined4 *)&this[1].field_0x8 = *(undefined4 *)(dVar2 + 0x110);
        *(undefined4 *)&this[1].field_0xc = *(undefined4 *)(dVar2 + 0x114);
        *(undefined4 *)&this[1].field_0x10 = *(undefined4 *)(dVar2 + 0x118);
        this[1].mbr_0x14 = *(dword *)(dVar2 + 0x11c);
      }
    }
  }
  iVar5 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      this_00 = *(cls_0x41b240 **)(this->mbr_0x14 + iVar5 * 4);
      if ((*(byte *)&this_00->mbr_0x44 & 2) != 0) {
        cls_0x41b240::meth_0x41b770(this_00);
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)this->mbr_0x4);
  }
  DAT_005e91ec = uVar3;
  DAT_005e91f0 = local_c.mbr_0x0;
  DAT_005e91f4 = local_c.mbr_0x4;
  DAT_005e91f8._0_4_ = local_c.mbr_0x8;
  return;
}



