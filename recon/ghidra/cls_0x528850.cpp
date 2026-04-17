// Decompiled methods and structure for class: cls_0x528850

/*
/OOAnalyzer/cls_0x528850
pack(disabled)
Structure cls_0x528850 {
   14   word   2   mbr_0xe   "Unsigned Word (dw, 2-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 00528850

void __thiscall OOAnalyzer::cls_0x528850::meth_0x528850(cls_0x528850 *this)

{
  int in_stack_00000004;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  dword local_c;
  dword local_8;
  undefined4 local_4;
  
  local_c = this->mbr_0x10;
  local_8 = this->mbr_0x14;
  local_18 = *(int *)(in_stack_00000004 + 0x10);
  local_14 = *(undefined4 *)(in_stack_00000004 + 0x14);
  local_10 = *(undefined4 *)(in_stack_00000004 + 0x18);
  local_4 = local_10;
  FUN_004533d0((int *)&local_c,&local_18,(uint)this->mbr_0xe,(int *)0x0,(int *)0x0);
  return;
}



