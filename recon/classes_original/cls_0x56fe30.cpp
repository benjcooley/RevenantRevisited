// Decompiled methods and structure for class: cls_0x56fe30

/*
/OOAnalyzer/cls_0x56fe30
pack(disabled)
Structure cls_0x56fe30 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
}
Length: 2 Alignment: 1

*/

// Function at 0056fe30

cls_0x56fe30 * __thiscall OOAnalyzer::cls_0x56fe30::cls_0x56fe30(cls_0x56fe30 *this)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  ushort *puVar4;
  
  if (DAT_005e5981 != '\0') {
    uVar2 = 0;
    puVar4 = &DAT_00676064;
    do {
      uVar1 = 0xc0c1;
      uVar3 = 1;
      do {
        if ((uVar2 & uVar3) != 0) {
          *puVar4 = *puVar4 ^ uVar1;
        }
        uVar3 = uVar3 * 2;
        uVar1 = uVar1 * 2 ^ 0x4003;
      } while ((int)uVar3 < 0x100);
      puVar4 = puVar4 + 1;
      uVar2 = uVar2 + 1;
    } while ((int)puVar4 < 0x676264);
    DAT_005e5981 = 0;
    this->mbr_0x0 = 0;
    return this;
  }
  this->mbr_0x0 = 0;
  return this;
}



// Function at 0056fe90

ushort __thiscall
OOAnalyzer::cls_0x56fe30::meth_0x56fe90(cls_0x56fe30 *this,byte *param_1,undefined param_2)

{
  byte *pbVar1;
  ushort uVar2;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  
  if (in_stack_0000000c != '\0') {
    this->mbr_0x0 = 0;
  }
  pbVar1 = param_1 + _param_2;
  if (pbVar1 <= param_1) {
    return this->mbr_0x0;
  }
  do {
    uVar2 = (&DAT_00676064)[this->mbr_0x0 & 0xff ^ (uint)*param_1] ^ this->mbr_0x0 >> 8;
    param_1 = param_1 + 1;
    this->mbr_0x0 = uVar2;
  } while (param_1 < pbVar1);
  return uVar2;
}



// Function at 0056ff50

void __thiscall OOAnalyzer::cls_0x56fe30::meth_0x56ff50(cls_0x56fe30 *this)

{
  this->mbr_0x0 = 0;
  return;
}



