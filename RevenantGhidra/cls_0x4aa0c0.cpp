// Decompiled methods and structure for class: cls_0x4aa0c0

/*
/OOAnalyzer/cls_0x4aa0c0
pack(disabled)
Structure cls_0x4aa0c0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 144 Alignment: 1

*/

// Function at 004aa0c0

undefined4 __thiscall OOAnalyzer::cls_0x4aa0c0::meth_0x4aa0c0(cls_0x4aa0c0 *this)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  undefined4 unaff_EBX;
  undefined4 unaff_EBP;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  undefined2 uStack_2;
  
  dVar1 = this->mbr_0x1c;
  dVar2 = this->mbr_0x14;
  dVar3 = this->mbr_0x18;
  (**(code **)(this->mbr_0x0 + 0x24))();
  uStack_50 = 0;
  uStack_4c = 0;
  uStack_48 = 0;
  uStack_2 = 0;
  uStack_4 = 0;
  uStack_40 = 0;
  uStack_44 = 0;
  uStack_30 = 0;
  uStack_34 = 0;
  uStack_38 = 0;
  uStack_3c = 0;
  uStack_c = 0;
  uStack_54 = 0x80000000;
  uStack_8 = 0x1f;
  (**(code **)(*(int *)this->mbr_0x80 + 0x5c))(&uStack_54,this->mbr_0x8c,0,0);
  (**(code **)(this->mbr_0x0 + 0x48))(dVar1);
  (**(code **)(this->mbr_0x0 + 0x40))(dVar2,dVar3);
  (**(code **)(this->mbr_0x0 + 0x44))(unaff_EDI,unaff_ESI,unaff_EBP,unaff_EBX);
  return 1;
}



// Function at 004aa930

undefined4 __thiscall OOAnalyzer::cls_0x4aa0c0::meth_0x4aa930(cls_0x4aa0c0 *this)

{
  dword dVar1;
  int iVar2;
  undefined4 *puVar3;
  
  puVar3 = &DAT_00669af0;
  do {
    iVar2 = 0;
    if (puVar3[9] == 0) {
      return 1;
    }
    if (0 < (int)puVar3[this->mbr_0x78 + 0xd]) {
      do {
        meth_0x4aa9d0(this);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)puVar3[this->mbr_0x78 + 0xd]);
    }
    dVar1 = this->mbr_0x78;
    puVar3[dVar1 + 0xd] = 0;
    puVar3[dVar1 + 0xf] = 0;
    puVar3[dVar1 + 0x11] = 0;
    puVar3 = puVar3 + 0x13;
  } while ((int)puVar3 < 0x669de8);
  return 1;
}



// Function at 004aa9d0

undefined4 __thiscall OOAnalyzer::cls_0x4aa0c0::meth_0x4aa9d0(cls_0x4aa0c0 *this)

{
  int iVar1;
  undefined4 unaff_EBP;
  uint unaff_retaddr;
  int in_stack_00000004;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  dword local_6c;
  dword local_68;
  dword local_64;
  dword local_60;
  dword local_5c;
  dword local_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  int iStack_44;
  int iStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  int iStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined2 uStack_1c;
  undefined2 uStack_1a;
  int iStack_10;
  int iStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (((-1 < in_stack_00000004) && (in_stack_00000004 < 10)) &&
     ((&DAT_00669b14)[in_stack_00000004 * 0x13] != 0)) {
    local_6c = this->mbr_0x14;
    local_5c = this->mbr_0x20;
    local_60 = this->mbr_0x24;
    local_64 = this->mbr_0x28;
    local_68 = this->mbr_0x2c;
    local_58 = this->mbr_0x1c;
    (**(code **)(this->mbr_0x0 + 0x24))();
    (**(code **)(this->mbr_0x0 + 0x40))(0,0);
    uVar4 = (&DAT_00669af8)[in_stack_00000004 * 0x13];
    uVar3 = (&DAT_00669af4)[in_stack_00000004 * 0x13];
    (**(code **)(this->mbr_0x0 + 0x44))
              (uVar3,uVar4,(&DAT_00669b0c)[in_stack_00000004 * 0x13],
               (&DAT_00669b10)[in_stack_00000004 * 0x13]);
    if (((&DAT_00669af0)[in_stack_00000004 * 0x13] & 1) != 0) {
      unaff_retaddr = unaff_retaddr | 0x400000;
    }
    if (((&DAT_00669af0)[in_stack_00000004 * 0x13] & 2) != 0) {
      unaff_retaddr = unaff_retaddr & 0xfffffbff;
    }
    local_6c = unaff_retaddr | 0x24;
    iStack_44 = ((&DAT_00669af4)[in_stack_00000004 * 0x13] -
                (&DAT_00669afc)[in_stack_00000004 * 0x13]) + iStack_10;
    uVar2 = 0;
    iVar1 = (&DAT_00669b14)[in_stack_00000004 * 0x13];
    iStack_40 = (iStack_c - (&DAT_00669b00)[in_stack_00000004 * 0x13]) +
                (&DAT_00669af8)[in_stack_00000004 * 0x13];
    uStack_38 = uStack_4;
    iStack_34 = iStack_10 % *(int *)(iVar1 + 4);
    local_68 = 0;
    local_64 = 0;
    local_60 = 0;
    uStack_3c = uStack_8;
    uStack_2c = uStack_8;
    uStack_1a = 0;
    uStack_1c = 0;
    local_58 = 0;
    local_5c = 0;
    uStack_48 = 0;
    uStack_4c = 0;
    uStack_50 = 0;
    uStack_54 = 0;
    uStack_20 = 0x1f;
    uStack_24 = 0;
    iStack_30 = iStack_c % *(int *)(iVar1 + 8);
    uStack_28 = uStack_4;
    (**(code **)(this->mbr_0x0 + 0x5c))(&local_6c,iVar1,0,0);
    (**(code **)(this->mbr_0x0 + 0x40))(unaff_EBP,uStack_24);
    (**(code **)(this->mbr_0x0 + 0x44))(uVar4,uVar3,unaff_EBP,uVar2);
    (**(code **)(this->mbr_0x0 + 0x48))(uVar2);
    return 1;
  }
  return 0;
}



