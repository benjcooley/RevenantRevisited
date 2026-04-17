// Decompiled methods and structure for class: cls_0x4acee0

/*
/OOAnalyzer/cls_0x4acee0
pack(disabled)
Structure cls_0x4acee0 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   2   word   2   mbr_0x2   "Unsigned Word (dw, 2-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 004acee0

int __thiscall OOAnalyzer::cls_0x4acee0::meth_0x4acee0(cls_0x4acee0 *this)

{
  word wVar1;
  cls_0x4acee0 cVar2;
  uint in_stack_00000004;
  
  wVar1 = this->mbr_0x0;
  if ((short)wVar1 <= (short)((ushort)in_stack_00000004 & 0xff)) {
    if ((int)(in_stack_00000004 & 0xff) <= (int)(short)this->mbr_0x2 + (int)(short)wVar1) {
      cVar2 = this[((in_stack_00000004 & 0xff) - (int)(short)wVar1) + 0x102];
      if (cVar2 != (cls_0x4acee0)0x0) {
        return (int)&this[((in_stack_00000004 & 0xff) - (int)(short)wVar1) + 0x102].mbr_0x0 +
               (int)cVar2;
      }
    }
  }
  return 0;
}



