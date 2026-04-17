// Decompiled methods and structure for class: cls_0x5701f0

/*
/OOAnalyzer/cls_0x5701f0
pack(disabled)
Structure cls_0x5701f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 005701f0

undefined4 __thiscall OOAnalyzer::cls_0x5701f0::meth_0x5701f0(cls_0x5701f0 *this,undefined4 param_1)

{
  dword *this_00;
  undefined *puVar1;
  undefined *puVar2;
  char *in_stack_00000008;
  undefined local_28 [20];
  void *pvStack_14;
  dword local_10;
  void *local_c;
  undefined *puStack_8;
  dword local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a22b2;
  local_c = ExceptionList;
  if ((in_stack_00000008 != (char *)0x0) && (*in_stack_00000008 != '\0')) {
    puVar2 = (undefined *)this->mbr_0x28;
    this_00 = &this->mbr_0x28;
    ExceptionList = &local_c;
    this->mbr_0x34 = (dword)puVar2;
    this->mbr_0x38 = 0;
    this->mbr_0x34 = (dword)(puVar2 + 1);
    if ((undefined *)this->mbr_0x2c < puVar2 + 1) {
      puVar2 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)this_00);
    }
    puVar1 = (undefined *)this->mbr_0x2c;
    *puVar2 = 3;
    puVar2 = (undefined *)this->mbr_0x34;
    this->mbr_0x34 = (dword)(puVar2 + 2);
    if (puVar1 < puVar2 + 2) {
      puVar2 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)this_00);
    }
    *puVar2 = (char)param_1;
    puVar2[1] = (char)((uint)param_1 >> 8);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this_00);
    if (DAT_0067682c != 0) {
      cls_0x588410::cls_0x588410((cls_0x588410 *)local_28,*this_00 + 1);
      local_10 = 0;
      local_4 = 0;
      (**(code **)(this->mbr_0x0 + 4))(DAT_00676884,local_28);
      local_c = (void *)0xffffffff;
      cls_0x588410::meth_0x588480((cls_0x588410 *)&stack0xffffffd0);
      ExceptionList = pvStack_14;
      return 1;
    }
    cls_0x5756d0::meth_0x578ec0((cls_0x5756d0 *)&DAT_00676738,1);
  }
  ExceptionList = local_c;
  return 0;
}



