// Decompiled methods and structure for class: cls_0x498eb0

/*
/OOAnalyzer/cls_0x498eb0
pack(disabled)
Structure cls_0x498eb0 {
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 00498eb0

void __thiscall OOAnalyzer::cls_0x498eb0::meth_0x498eb0_TSector_GetMaxScreenRect(cls_0x498eb0 *this)

{
  int *piVar1;
  int *piVar2;
  int *in_stack_00000004;
  int local_18;
  int local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  undefined4 local_4;
  
  if ((this->mbr_0x8 < 0x20) && (this->mbr_0xc < 0x20)) {
    local_18 = this->mbr_0x8 * 0x400;
    local_14 = this->mbr_0xc * 0x400;
    local_c = local_18 + 0x3ff;
    local_8 = local_14 + 0x3ff;
  }
  else {
    local_8 = 0;
    local_c = 0;
    local_14 = 0;
    local_18 = 0;
  }
  local_10 = 0;
  local_4 = 0;
  FUN_0046d7a0(&local_18);
  piVar1 = in_stack_00000004 + 3;
  piVar2 = in_stack_00000004 + 2;
  FUN_0046d7a0(&local_c,piVar2,piVar1);
  *in_stack_00000004 = *in_stack_00000004 + -0x600;
  in_stack_00000004[1] = in_stack_00000004[1] + -0x400;
  *piVar2 = *piVar2 + 0x600;
  *piVar1 = *piVar1 + 0x400;
  return;
}



// Function at 00498f60

void __thiscall OOAnalyzer::cls_0x498eb0::meth_0x498f60_TSector_GetMaxMapRect(cls_0x498eb0 *this)

{
  int iVar1;
  int iVar2;
  int *in_stack_00000004;
  
  if ((this->mbr_0x8 < 0x20) && (this->mbr_0xc < 0x20)) {
    iVar1 = this->mbr_0x8 * 0x400;
    iVar2 = this->mbr_0xc * 0x400;
    *in_stack_00000004 = iVar1;
    in_stack_00000004[1] = iVar2;
    in_stack_00000004[2] = iVar1 + 0x3ff;
    in_stack_00000004[3] = iVar2 + 0x3ff;
    return;
  }
  in_stack_00000004[3] = 0;
  in_stack_00000004[2] = 0;
  in_stack_00000004[1] = 0;
  *in_stack_00000004 = 0;
  return;
}



