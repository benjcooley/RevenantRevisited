// Decompiled methods and structure for class: cls_0x429fd0

/*
/OOAnalyzer/cls_0x429fd0
pack(disabled)
Structure cls_0x429fd0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 00429fd0

undefined4 __thiscall
OOAnalyzer::cls_0x429fd0::meth_0x429fd0(cls_0x429fd0 *this,cls_0x429fd0 *param_1,int *param_2)

{
  int iVar1;
  undefined4 uVar2;
  dword dVar3;
  uint *in_stack_0000000c;
  
  dVar3 = DAT_0065caec;
  if (param_1 == (cls_0x429fd0 *)0x0) {
    this->mbr_0x0 = 0;
    this->mbr_0x4 = 0;
    this->mbr_0x8 = 0;
    this->mbr_0xc = 0;
    this->mbr_0x0 = dVar3;
    this->mbr_0x8 = 1;
    this->mbr_0x4 = 0xffffff;
    this->mbr_0xc = 0x80000000;
  }
  else if (this != param_1) {
    this->mbr_0x0 = param_1->mbr_0x0;
    this->mbr_0x4 = param_1->mbr_0x4;
    dVar3 = param_1->mbr_0xc;
    this->mbr_0x8 = param_1->mbr_0x8;
    this->mbr_0xc = dVar3;
  }
  iVar1 = FUN_0047a410(in_stack_0000000c,(float **)s__20s_<COLOR__b__b__b>_<FLAGS__d>_005ccf70);
  if (iVar1 == 0) {
    uVar2 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_text_data_005ccfa0,0);
    return uVar2;
  }
  dVar3 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
  if (dVar3 != 0) {
    this->mbr_0x0 = dVar3;
  }
  return 1;
}



