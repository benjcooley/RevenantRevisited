// Decompiled methods and structure for class: cls_0x5aa46c

/*
/OOAnalyzer/cls_0x5aa46c
pack(disabled)
Structure cls_0x5aa46c {
   0   cls_0x5aa46c::vftable_5aa46c *   4   vftptr_0x0   "pointer to cls_0x5aa46c::vftable_5aa46c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   612   dword   4   mbr_0x264   "Unsigned Double-Word (ddw, 4-bytes)"
   616   dword   4   mbr_0x268   "Unsigned Double-Word (ddw, 4-bytes)"
   620   dword   4   mbr_0x26c   "Unsigned Double-Word (ddw, 4-bytes)"
   972   dword   4   mbr_0x3cc   "Unsigned Double-Word (ddw, 4-bytes)"
   1092   dword   4   mbr_0x444   "Unsigned Double-Word (ddw, 4-bytes)"
   1212   dword   4   mbr_0x4bc   "Unsigned Double-Word (ddw, 4-bytes)"
   1216   dword   4   mbr_0x4c0   "Unsigned Double-Word (ddw, 4-bytes)"
   1220   dword   4   mbr_0x4c4   "Unsigned Double-Word (ddw, 4-bytes)"
   1224   dword   4   mbr_0x4c8   "Unsigned Double-Word (ddw, 4-bytes)"
   1228   dword   4   mbr_0x4cc   "Unsigned Double-Word (ddw, 4-bytes)"
   1232   dword   4   mbr_0x4d0   "Unsigned Double-Word (ddw, 4-bytes)"
   1236   dword   4   mbr_0x4d4   "Unsigned Double-Word (ddw, 4-bytes)"
   1240   dword   4   mbr_0x4d8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1244 Alignment: 1

*/

// Function at 004e7530

void __thiscall OOAnalyzer::cls_0x5aa46c::virt_meth_0x4e7530(cls_0x5aa46c *this)

{
  cls_0x5a7e38::virt_meth_0x40dd60((cls_0x5a7e38 *)this);
  (**(code **)(*(int *)this->mbr_0x4 + 0x40))(((int *)this->mbr_0x4)[2] | 0x100);
  return;
}



// Function at 004f64c0

cls_0x5aa46c * __thiscall OOAnalyzer::cls_0x5aa46c::virt_meth_0x4f64c0(cls_0x5aa46c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5aa46c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f64e0

cls_0x5aa46c * __thiscall OOAnalyzer::cls_0x5aa46c::~cls_0x5aa46c(cls_0x5aa46c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f2f8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa46c__vftable_5aa46c_005aa46c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa46c *)pcVar1;
}



// Function at 004f6530

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5aa46c::virt_meth_0x4f6530(cls_0x5aa46c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f32e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4dc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa46c__vftable_5aa46c_005aa46c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



