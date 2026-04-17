// Decompiled methods and structure for class: cls_0x473e40

/*
/OOAnalyzer/cls_0x473e40
pack(disabled)
Structure cls_0x473e40 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00473e40

cls_0x473e40 * __thiscall
OOAnalyzer::cls_0x473e40::cls_0x473e40
          (cls_0x473e40 *this,int param_1,char *param_2,char *param_3,dword param_4,
          undefined4 param_5,undefined4 param_6,undefined4 param_7)

{
  int unaff_ESI;
  int in_stack_00000020;
  undefined4 local_58;
  undefined local_54;
  undefined4 local_50 [15];
  undefined local_11;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  this->mbr_0x0 = param_4;
  _strncpy((char *)local_50,param_2,0x3f);
  local_11 = 0;
  local_54 = 0;
  local_58 = 0;
  _strncpy((char *)&local_58,param_3,4);
  local_10 = param_5;
  local_4 = local_58;
  local_54 = 0;
  local_c = param_6;
  local_8 = param_7;
  if (in_stack_00000020 != 0) {
    FUN_00475ca0(param_1,local_50,(undefined4 *)this->mbr_0x0,unaff_ESI);
    return this;
  }
  FUN_004755d0(param_1,local_50,this->mbr_0x0);
  return this;
}



