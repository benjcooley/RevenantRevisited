// Decompiled methods and structure for class: cls_0x59b88e

/*
/OOAnalyzer/cls_0x59b88e
pack(disabled)
Structure cls_0x59b88e {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0059b88e

cls_0x59b88e * __thiscall OOAnalyzer::cls_0x59b88e::~cls_0x59b88e(cls_0x59b88e *this)

{
  cls_0x59b88e *pcVar1;
  exception *this_00;
  int unaff_EBP;
  
  FUN_0059c1c8();
  *(exception **)(unaff_EBP + -0x10) = this_00;
  this_00->vftptr_0x0 = (exception__vftable_5bafd8 *)&logic_error__vftable_5baf9c_005baf9c;
  *(undefined4 *)(unaff_EBP + -4) = 0;
  cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)(this_00 + 1));
  *(undefined4 *)(unaff_EBP + -4) = 0xffffffff;
  pcVar1 = (cls_0x59b88e *)exception::~exception(this_00);
  ExceptionList = *(void **)(unaff_EBP + -0xc);
  return pcVar1;
}



