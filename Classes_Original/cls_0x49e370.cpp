// Decompiled methods and structure for class: cls_0x49e370

/*
/OOAnalyzer/cls_0x49e370
pack(disabled)
Structure cls_0x49e370 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0049e370

bool __thiscall OOAnalyzer::cls_0x49e370::meth_0x49e370(cls_0x49e370 *this)

{
  MMRESULT MVar1;
  uint uVar2;
  uint uPeriod;
  timecaps_tag tStack_8;
  
  DAT_006687f0 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCSTR)0x0);
  if (this->mbr_0x0 != 0) {
    return true;
  }
  MVar1 = timeGetDevCaps(&tStack_8,8);
  if (MVar1 == 0) {
    uVar2 = tStack_8.wPeriodMin;
    if (tStack_8.wPeriodMin < 0x2a) {
      uVar2 = 0x29;
    }
    uPeriod = tStack_8.wPeriodMax;
    if ((uVar2 < tStack_8.wPeriodMax) && (uPeriod = 0x29, 0x29 < tStack_8.wPeriodMin)) {
      uPeriod = tStack_8.wPeriodMin;
    }
    timeBeginPeriod(uPeriod);
    MVar1 = timeSetEvent(uPeriod,5,&LAB_0049e2f0,0,1);
    this->mbr_0x0 = MVar1;
    return MVar1 != 0;
  }
  return false;
}



// Function at 0049e410

void __thiscall OOAnalyzer::cls_0x49e370::meth_0x49e410(cls_0x49e370 *this)

{
  HANDLE hObject;
  
  if (this->mbr_0x0 != 0) {
    timeEndPeriod(DAT_005dacf4);
    timeKillEvent(this->mbr_0x0);
    hObject = DAT_006687f0;
    this->mbr_0x0 = 0;
    CloseHandle(hObject);
  }
  return;
}



