// Decompiled methods and structure for class: cls_0x5726b0

/*
/OOAnalyzer/cls_0x5726b0
pack(disabled)
Structure cls_0x5726b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 005726b0

cls_0x5726b0 * __thiscall OOAnalyzer::cls_0x5726b0::~cls_0x5726b0(cls_0x5726b0 *this)

{
  cls_0x5726b0 *hObject;
  
  this->mbr_0x0 = (dword)&cls_0x5b9f0c__vftable_5b9f0c_005b9f0c;
  if (this->mbr_0x38 != 0) {
    FUN_004830f0(this->mbr_0x38);
  }
  hObject = (cls_0x5726b0 *)this->mbr_0x10;
  if (hObject != (cls_0x5726b0 *)0x0) {
    hObject = (cls_0x5726b0 *)CloseHandle(hObject);
  }
  if (this->mbr_0x34 != 0) {
    hObject = (cls_0x5726b0 *)FUN_004830f0(this->mbr_0x34);
  }
  return hObject;
}



