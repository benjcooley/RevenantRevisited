// Decompiled methods and structure for class: cls_0x579280

/*
/OOAnalyzer/cls_0x579280
pack(disabled)
Structure cls_0x579280 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 304 Alignment: 1

*/

// Function at 00579280

void __thiscall OOAnalyzer::cls_0x579280::meth_0x579280(cls_0x579280 *this)

{
  int in_stack_00000004;
  
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  if (*(int *)(in_stack_00000004 + 0x18) == 1) {
    (**(code **)(*(int *)this->mbr_0x0 + 0xcc))
              ((int *)this->mbr_0x0,*(undefined4 *)(in_stack_00000004 + 0x14),0);
    *(undefined4 *)(in_stack_00000004 + 0x18) = 3;
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  return;
}



