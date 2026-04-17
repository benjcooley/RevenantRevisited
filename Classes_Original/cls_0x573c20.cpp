// Decompiled methods and structure for class: cls_0x573c20

/*
/OOAnalyzer/cls_0x573c20
pack(disabled)
Structure cls_0x573c20 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 20 Alignment: 1

*/

// Function at 00573c20

void __thiscall OOAnalyzer::cls_0x573c20::meth_0x573c20(cls_0x573c20 *this)

{
  int *piVar1;
  char cVar2;
  DWORD DVar3;
  int *piVar4;
  
  DVar3 = timeGetTime();
  if (*(int *)this->mbr_0x4 != 0) {
    piVar4 = (int *)((int *)this->mbr_0x4)[2];
    while (piVar1 = piVar4, piVar1 != (int *)0x0) {
      if (*(int *)piVar1[0x10] == 0) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar4 = (int *)((int *)piVar1[0x10])[2];
      }
      cVar2 = (**(code **)(*piVar1 + 8))(DVar3);
      if ((cVar2 == '\0') && (piVar1 != (int *)0x0)) {
        (**(code **)*piVar1)(1);
      }
    }
  }
  if (*(int *)this->mbr_0x10 != 0) {
    piVar4 = (int *)((int *)this->mbr_0x10)[2];
    while (piVar1 = piVar4, piVar1 != (int *)0x0) {
      if (*(int *)piVar1[0x10] == 0) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar4 = (int *)((int *)piVar1[0x10])[2];
      }
      cVar2 = (**(code **)(*piVar1 + 8))(DVar3);
      if ((cVar2 == '\0') && (piVar1 != (int *)0x0)) {
        (**(code **)*piVar1)(1);
      }
    }
  }
  return;
}



