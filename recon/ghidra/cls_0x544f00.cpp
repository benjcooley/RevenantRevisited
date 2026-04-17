// Decompiled methods and structure for class: cls_0x544f00

/*
/OOAnalyzer/cls_0x544f00
pack(disabled)
Structure cls_0x544f00 {
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   464   dword   4   mbr_0x1d0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 468 Alignment: 1

*/

// Function at 00544f00

cls_0x544f00 * __thiscall OOAnalyzer::cls_0x544f00::cls_0x544f00(cls_0x544f00 *this)

{
  dword dVar1;
  int iVar2;
  cls_0x544f00 *pcVar3;
  cls_0x544f00 *pcVar4;
  cls_0x544f00 *pcVar5;
  int *piVar6;
  cls_0x544f00 *in_stack_00000004;
  int local_c;
  
  local_c = 0;
  pcVar4 = (cls_0x544f00 *)0x0;
  if (0 < *(int *)(DAT_00667fcc + 0x2ec)) {
    dVar1 = this->mbr_0x17c;
    piVar6 = (int *)this->mbr_0x1a8;
    pcVar5 = (cls_0x544f00 *)0x0;
    while( true ) {
      iVar2 = *piVar6;
      pcVar3 = pcVar5;
      if ((((int)dVar1 <= (int)pcVar5) && ((int)pcVar5 <= (int)(dVar1 + 0xe5))) ||
         (((int)dVar1 <= (int)&pcVar5->field_0x28 &&
          ((int)&pcVar5->field_0x28 <= (int)(dVar1 + 0xe5))))) break;
      local_c = local_c + 1;
      piVar6 = piVar6 + 1;
      pcVar4 = pcVar5;
      pcVar5 = (cls_0x544f00 *)(&pcVar5->field_0x0 + iVar2);
      if (*(int *)(DAT_00667fcc + 0x2ec) <= local_c) {
        return pcVar3;
      }
    }
    if (0 < (int)in_stack_00000004) {
      this->mbr_0x1d0 = (int)(cls_0x544f00 *)(&pcVar5->field_0x0 + iVar2) - dVar1;
      return this;
    }
    this->mbr_0x1d0 = (int)pcVar4 - dVar1;
    pcVar4 = in_stack_00000004;
  }
  return pcVar4;
}



