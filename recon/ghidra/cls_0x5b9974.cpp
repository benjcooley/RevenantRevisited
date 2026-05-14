// Decompiled methods and structure for class: cls_0x5b9974

/*
/OOAnalyzer/cls_0x5b9974
pack(disabled)
Structure cls_0x5b9974 {
   0   TObjectInstance   112   TObjectInstance   "Component (member) class."
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   204   dword   4   mbr_0xcc   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 212 Alignment: 1

*/

// Function at 0053c680

cls_0x5b9974 * __thiscall
OOAnalyzer::cls_0x5b9974::cls_0x5b9974
          (cls_0x5b9974 *this,char *param_1,char *param_2,dword param_3,dword param_4)

{
  char cVar1;
  uint uVar2;
  char *pcVar3;
  dword in_stack_00000014;
  
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = param_3;
  this->mbr_0x74 = param_4;
  uVar2 = 0xffffffff;
  pcVar3 = param_2;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (cVar1 != '\0');
  this->mbr_0xc0 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0xc8 = 0;
  this->mbr_0xcc = 0;
  this->mbr_0xd0 = 0;
  this->mbr_0xc4 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5b9974__vftable_5b9974_005b9974;
  _strncpy((char *)&this->mbr_0x7c,param_2,~uVar2 - 1);
  *(undefined *)((~uVar2 - 1) + (int)&this->mbr_0x7c) = 0;
  uVar2 = 0xffffffff;
  pcVar3 = param_1;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (cVar1 != '\0');
  _strncpy((char *)&this->mbr_0x9c,param_1,~uVar2 - 1);
  *(undefined *)((~uVar2 - 1) + (int)&this->mbr_0x9c) = 0;
  this->mbr_0xbc = in_stack_00000014;
  return this;
}



// Function at 0053c7a0

void __thiscall OOAnalyzer::cls_0x5b9974::virt_meth_0x53c7a0(cls_0x5b9974 *this)

{
  FUN_00482f80((LPCVOID)this->mbr_0x78);
  this->mbr_0x78 = 0;
  return;
}



// Function at 0053c7c0

void __thiscall OOAnalyzer::cls_0x5b9974::virt_meth_0x53c7c0(cls_0x5b9974 *this)

{
  dword dVar1;
  
  TObjectInstance::virt_meth_0x48fda0_TScreen_DrawBackground(&this->TObjectInstance);
  dVar1 = this->mbr_0x70 - 1;
  this->mbr_0x70 = dVar1;
  if (dVar1 == 0) {
    (this->TObjectInstance).cls_0x41c7f0.mbr_0x14 = this->mbr_0xbc;
    TCharacter::meth_0x48ea40((TCharacter *)this);
  }
  return;
}



// Function at 0053c7f0

void __thiscall OOAnalyzer::cls_0x5b9974::virt_meth_0x53c7f0(cls_0x5b9974 *this)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  if ((this->TObjectInstance).mbr_0x50 != 0) {
    uVar2 = 0x10;
    uVar1 = cls_0x46d6b0::meth_0x46d710((cls_0x46d6b0 *)this->mbr_0x78);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)PTR_DAT_005d79e0,0,0,uVar1,uVar2);
  }
  TObjectInstance::virt_meth_0x48ff00_TScreen_Animate(&this->TObjectInstance);
  (this->TObjectInstance).mbr_0x50 = 0;
  return;
}



// Function at 0053c8a0

cls_0x5b9974 * __thiscall OOAnalyzer::cls_0x5b9974::virt_meth_0x53c8a0(cls_0x5b9974 *this)

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



