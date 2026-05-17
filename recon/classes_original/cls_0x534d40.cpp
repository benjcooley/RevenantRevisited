// Decompiled methods and structure for class: cls_0x534d40

/*
/OOAnalyzer/cls_0x534d40
pack(disabled)
Structure cls_0x534d40 {
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 324 Alignment: 1

*/

// Function at 00534d40

void __thiscall OOAnalyzer::cls_0x534d40::meth_0x534d40(cls_0x534d40 *this)

{
  dword dVar1;
  int iVar2;
  undefined *puVar3;
  dword *pdVar4;
  dword *pdVar5;
  int iStack_60;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  int local_24;
  dword local_20;
  dword local_1c;
  dword local_18;
  int local_14;
  dword local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if (this->mbr_0x20 != 0xffffd8f0) {
    iVar2 = (int)(this->mbr_0x58 * 0xff) / 0xc;
    if (0x7f < iVar2) {
      if (DAT_006680c8 != 0) {
        local_2c = this->mbr_0x18 + this->mbr_0x20;
        local_20 = this->mbr_0x2c;
        local_28 = this->mbr_0x24 + this->mbr_0x1c;
        local_24 = 0x32;
        local_14 = 0x32;
        local_54 = 0x100;
        local_50 = 0;
        local_4c = 0;
        local_48 = 0;
        local_1c = 0;
        local_18 = 0;
        local_2 = 0;
        local_4 = 0;
        local_40 = 0;
        local_44 = 0;
        local_30 = 0;
        local_34 = 0;
        local_38 = 0;
        local_3c = 0;
        local_8 = 0x1f;
        local_c = 0;
        local_10 = local_20;
        (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&local_54,this->mbr_0x48,0,0);
        puVar3 = PTR_DAT_005d79e0;
        dVar1 = this->mbr_0x48;
        FUN_00438d80_BlitEffect_StructInit(&stack0xffffff9c,this->mbr_0x18 + 0x32 + this->mbr_0x20,
                     this->mbr_0x24 + this->mbr_0x1c,0x32,0,this->mbr_0x28 - 0x32,this->mbr_0x2c,
                     0x100);
        (**(code **)(*(int *)puVar3 + 0x5c))(&stack0xffffff9c,dVar1,0,0);
        iStack_60 = 0;
        if (0 < (int)this->mbr_0x5c) {
          pdVar5 = &this->mbr_0x140;
          pdVar4 = &this->mbr_0x80;
          do {
            if ((pdVar5[-0x10] != 0) &&
               (0x7f < (((int)(*pdVar5 * 0xff + ((int)(*pdVar5 * 0xff) >> 0x1f & 7U)) >> 3) * iVar2)
                       / 0xff)) {
              local_20 = (pdVar4[3] - pdVar4[1]) + 1;
              local_2c = *pdVar4 + this->mbr_0x18 + this->mbr_0x20;
              local_24 = (pdVar4[2] - *pdVar4) + 1;
              local_28 = pdVar4[1] + this->mbr_0x24 + this->mbr_0x1c;
              local_1c = *pdVar4;
              local_18 = pdVar4[1];
              local_54 = 0x100;
              local_50 = 0;
              local_4c = 0;
              local_48 = 0;
              local_2 = 0;
              local_4 = 0;
              local_40 = 0;
              local_44 = 0;
              local_30 = 0;
              local_34 = 0;
              local_38 = 0;
              local_3c = 0;
              local_8 = 0x1f;
              local_c = 0;
              local_14 = local_24;
              local_10 = local_20;
              (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&local_54,this->mbr_0x4c,0,0);
            }
            iStack_60 = iStack_60 + 1;
            pdVar5 = pdVar5 + 1;
            pdVar4 = pdVar4 + 4;
          } while (iStack_60 < (int)this->mbr_0x5c);
        }
      }
      if (DAT_005d7a18 == 0) {
        FUN_004aacb0_BlitEffect_Iterate(this->mbr_0x18 + this->mbr_0x20,this->mbr_0x24 + this->mbr_0x1c,this->mbr_0x28,
                     this->mbr_0x2c,1);
      }
    }
  }
  return;
}



