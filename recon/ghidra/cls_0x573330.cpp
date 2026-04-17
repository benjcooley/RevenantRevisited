// Decompiled methods and structure for class: cls_0x573330

/*
/OOAnalyzer/cls_0x573330
pack(disabled)
Structure cls_0x573330 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 00573330

uint __thiscall
OOAnalyzer::cls_0x573330::meth_0x573330(cls_0x573330 *this,uint param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  uint in_EAX;
  DWORD DVar4;
  byte *pbVar5;
  uint uVar6;
  byte bVar7;
  LPCVOID in_stack_0000000c;
  
  iVar3 = param_1;
  if ((-1 < (int)param_1) && (in_EAX = this->mbr_0x1c, (int)param_1 < (int)in_EAX)) {
    uVar2 = this->mbr_0x18;
    uVar6 = uVar2;
    if (param_1 == in_EAX - 1) {
      uVar6 = this->mbr_0x14 - uVar2 * param_1;
    }
    in_EAX = uVar2;
    if (param_2 == uVar6) {
      param_1 = uVar2 * param_1;
      DVar4 = SetFilePointer((HANDLE)this->mbr_0x10,0,(PLONG)0x0,2);
      in_EAX = DVar4;
      if ((DVar4 != 0xffffffff) &&
         (in_EAX = SetFilePointer((HANDLE)this->mbr_0x10,param_1,(PLONG)0x0,0), in_EAX != 0xffffffff
         )) {
        if (DVar4 < param_1) {
          SetEndOfFile((HANDLE)this->mbr_0x10);
        }
        in_EAX = WriteFile((HANDLE)this->mbr_0x10,in_stack_0000000c,param_2,&param_1,
                           (LPOVERLAPPED)0x0);
        if (in_EAX != 0) {
          DVar4 = timeGetTime();
          this->mbr_0x2c = 5;
          this->mbr_0x28 = DVar4 + 1000;
          pbVar5 = (byte *)this->mbr_0x1c;
          if (iVar3 < (int)pbVar5) {
            pbVar5 = (byte *)(((int)(iVar3 + (iVar3 >> 0x1f & 7U)) >> 3) + this->mbr_0x38);
            bVar7 = '\x01' << ((byte)iVar3 & 7);
            bVar1 = *pbVar5;
            if ((bVar7 & bVar1) != 0) {
              this->mbr_0x20 = this->mbr_0x20 - 1;
            }
            *pbVar5 = ~bVar7 & bVar1;
          }
          return CONCAT31((int3)((uint)pbVar5 >> 8),1);
        }
      }
    }
  }
  return in_EAX & 0xffffff00;
}



