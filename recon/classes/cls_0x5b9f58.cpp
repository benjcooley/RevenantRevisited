// Decompiled methods and structure for class: cls_0x5b9f58

/*
/OOAnalyzer/cls_0x5b9f58
pack(disabled)
Structure cls_0x5b9f58 {
   0   cls_0x5b9f58::vftable_5b9f58 *   4   vftptr_0x0   "pointer to cls_0x5b9f58::vftable_5b9f58"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 00573ec0

LONG __thiscall OOAnalyzer::cls_0x5b9f58::virt_meth_0x573ec0(cls_0x5b9f58 *this)

{
  LONG LVar1;
  
  LVar1 = InterlockedDecrement((LONG *)&this->mbr_0x4);
  if (LVar1 < 1) {
    if ((LVar1 == 0) && (this != (cls_0x5b9f58 *)0x0)) {
      (*this->vftptr_0x0->virt_deldtor_0x573f40_0)(this);
    }
    LVar1 = 0;
  }
  return LVar1;
}



// Function at 00573f00

cls_0x5b9f58 * __thiscall OOAnalyzer::cls_0x5b9f58::virt_meth_0x573f00(cls_0x5b9f58 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b9f58(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00573f20

cls_0x5b9f58 * __thiscall OOAnalyzer::cls_0x5b9f58::~cls_0x5b9f58(cls_0x5b9f58 *this)

{
  cls_0x5b9f58 *pcVar1;
  cls_0x5b9f58 **ppcVar2;
  
  pcVar1 = (cls_0x5b9f58 *)this->mbr_0x8;
  if (pcVar1 != (cls_0x5b9f58 *)0x0) {
    ppcVar2 = (cls_0x5b9f58 **)this->mbr_0xc;
    this->mbr_0x8 = 0;
    pcVar1->mbr_0x4 = (dword)ppcVar2;
    *ppcVar2 = pcVar1;
  }
  this->vftptr_0x0 = &cls_0x5b9f58__vftable_5b9f58_005b9f58;
  return pcVar1;
}



