// Decompiled methods and structure for class: cls_0x45a2e0

/*
/OOAnalyzer/cls_0x45a2e0
pack(disabled)
Structure cls_0x45a2e0 {
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   2148   dword   4   mbr_0x864   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 2152 Alignment: 1

*/

// Function at 0045a2e0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x45a2e0::meth_0x45a2e0(cls_0x45a2e0 *this)

{
  cls_0x499720 *this_00;
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_18;
  int local_14;
  int iStack_10;
  int local_c;
  int local_8;
  int local_4;
  
  this_00 = (cls_0x499720 *)FUN_00499e10_TSector_FindLoadedSector(_DAT_00666970,this->mbr_0x68,this->mbr_0x6c);
  if (this_00 != (cls_0x499720 *)0x0) {
    iVar3 = 0;
    do {
      iVar4 = 0;
      do {
        local_c = (this->mbr_0x68 * 0x40 + iVar4) * 0x10;
        local_8 = (this->mbr_0x6c * 0x40 + iVar3) * 0x10;
        local_4 = 0;
        FUN_0046d810(&local_c,&local_18);
        if (((((int)this->mbr_0x78 <= local_18) &&
             (local_18 < (int)(this->mbr_0xc + this->mbr_0x78))) &&
            ((int)this->mbr_0x7c <= local_14)) &&
           (local_14 < (int)(this->mbr_0x10 + this->mbr_0x7c))) {
          local_4 = 0xff;
          do {
            local_c = (this->mbr_0x68 * 0x40 + iVar4) * 0x10;
            local_8 = (this->mbr_0x6c * 0x40 + iVar3) * 0x10;
            FUN_0046d810(&local_c,&local_18);
            if ((((int)this->mbr_0x78 <= local_18) &&
                (local_18 < (int)(this->mbr_0xc + this->mbr_0x78))) &&
               (((int)this->mbr_0x7c <= local_14 &&
                (local_14 < (int)(this->mbr_0x10 + this->mbr_0x7c))))) {
              (**(code **)(*(int *)this->mbr_0x864 + 0x44))(local_18,local_14,1,1);
              local_14 = local_14 + 8;
              iStack_10 = iStack_10 + -0x14e;
              uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
              iVar2 = cls_0x4bddc0::meth_0x4bddc0
                                ((cls_0x4bddc0 *)this->mbr_0x864,local_18,(short)local_14,iStack_10,
                                 uVar1);
              if (iVar2 == 0) {
                local_c = iVar4;
                local_8 = iVar3;
                cls_0x499720::meth_0x499750_TSector_SetWalkmap(this_00,iVar4,iVar3);
                break;
              }
            }
            local_4 = local_4 + -1;
          } while (1 < local_4);
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < 0x40);
      iVar3 = iVar3 + 1;
    } while (iVar3 < 0x40);
    cls_0x5a5320_TPlayScreen::meth_0x4546a0((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
  }
  return;
}



