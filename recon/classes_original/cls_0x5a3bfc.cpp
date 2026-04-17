// Decompiled methods and structure for class: cls_0x5a3bfc

/*
/OOAnalyzer/cls_0x5a3bfc
pack(disabled)
Structure cls_0x5a3bfc {
   0   cls_0x5a3ab8   144   cls_0x5a3ab8   "Component (member) class."
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   204   dword   4   mbr_0xcc   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 220 Alignment: 1

*/

// Function at 0042b5d0

cls_0x5a3bfc * __thiscall
OOAnalyzer::cls_0x5a3bfc::cls_0x5a3bfc(cls_0x5a3bfc *this,undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  dword *pdVar2;
  dword dVar3;
  int iVar4;
  int in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca4c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a3ab8::cls_0x5a3ab8(&this->cls_0x5a3ab8,param_1,3);
  local_4 = 0;
  (this->cls_0x5a3ab8).vftptr_0x0 =
       (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5a3bfc__vftable_5a3bfc_005a3bfc;
  (this->cls_0x5a3ab8).mbr_0x14 = (this->cls_0x5a3ab8).mbr_0x14 | 1;
  if (in_stack_0000000c == 0) {
    in_stack_0000000c = 0x6554b8;
  }
  iVar4 = cls_0x42b340::meth_0x42b340((cls_0x42b340 *)&this->mbr_0x90,in_stack_0000000c,param_1);
  if (iVar4 != 0) {
    uVar1 = (this->cls_0x5a3ab8).mbr_0x14;
    if ((uVar1 & 0x40000) == 0) {
      if ((uVar1 & 0x80000) != 0) {
        this->mbr_0xd0 = this->mbr_0xd0 & 0xfffffffc | 4;
      }
    }
    else {
      this->mbr_0xd0 = this->mbr_0xd0 & 0xfffffffa | 2;
    }
    if ((uVar1 & 0x100000) == 0) {
      if ((uVar1 & 0x200000) != 0) {
        this->mbr_0xd0 = this->mbr_0xd0 & 0xffffffaf | 0x20;
      }
    }
    else {
      this->mbr_0xd0 = this->mbr_0xd0 & 0xffffff9f | 0x10;
    }
    pdVar2 = (dword *)this->mbr_0x90;
    if (pdVar2 != (dword *)0x0) {
      if ((this->cls_0x5a3ab8).mbr_0x60 == 0xffffd8f0) {
        (this->cls_0x5a3ab8).mbr_0x60 = -pdVar2[2];
        (this->cls_0x5a3ab8).mbr_0x64 = -pdVar2[3];
      }
      if ((this->cls_0x5a3ab8).mbr_0x68 == 0xffffd8f0) {
        dVar3 = pdVar2[1];
        (this->cls_0x5a3ab8).mbr_0x68 = *pdVar2;
        (this->cls_0x5a3ab8).mbr_0x6c = dVar3;
      }
    }
    (this->cls_0x5a3ab8).mbr_0x14 = uVar1 & 0xfffffffe;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 00438070

cls_0x5a3bfc * __thiscall OOAnalyzer::cls_0x5a3bfc::virt_meth_0x438070(cls_0x5a3bfc *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3bfc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00438090

cls_0x5a3bfc * __thiscall OOAnalyzer::cls_0x5a3bfc::~cls_0x5a3bfc(cls_0x5a3bfc *this)

{
  cls_0x5a3bfc *pcVar1;
  
  pcVar1 = (cls_0x5a3bfc *)(this->cls_0x5a3ab8).mbr_0x5c;
  (this->cls_0x5a3ab8).vftptr_0x0 = &cls_0x5a3ab8__vftable_5a3ab8_005a3ab8;
  if (pcVar1 != (cls_0x5a3bfc *)0x0) {
    pcVar1 = (cls_0x5a3bfc *)FUN_00482f80(pcVar1);
  }
  (this->cls_0x5a3ab8).mbr_0x5c = 0;
  return pcVar1;
}



// Function at 004380c0

cls_0x5a3c68 * __thiscall
OOAnalyzer::cls_0x5a3bfc::virt_meth_0x4380c0(cls_0x5a3bfc *this,undefined4 param_1)

{
  cls_0x5a3c68 *pcVar1;
  undefined4 in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cc9b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5a3c68 *)FUN_00482fb0(0x148,this);
  local_4 = 0;
  if (pcVar1 != (cls_0x5a3c68 *)0x0) {
    pcVar1 = cls_0x5a3c68::cls_0x5a3c68(pcVar1,param_1,in_stack_00000008);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5a3c68 *)0x0;
}



