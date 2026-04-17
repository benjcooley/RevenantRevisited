// Decompiled methods and structure for class: cls_0x51f6b0

/*
/OOAnalyzer/cls_0x51f6b0
pack(disabled)
Structure cls_0x51f6b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 40 Alignment: 1

*/

// Function at 0051f6b0

void __thiscall OOAnalyzer::cls_0x51f6b0::meth_0x51f6b0(cls_0x51f6b0 *this)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  dword dVar5;
  int iVar6;
  int iVar7;
  dword dVar8;
  int *local_10;
  dword local_c;
  
  dVar8 = this->mbr_0x0;
  if (0 < (int)dVar8) {
    piVar4 = (int *)this->mbr_0x10;
    dVar5 = dVar8;
    do {
      iVar6 = *piVar4;
      if (iVar6 != 0) {
        *(undefined4 *)(iVar6 + 0x4ec) = 0;
        *(undefined4 *)(iVar6 + 0x4e4) = 0;
        *(undefined4 *)(iVar6 + 0x4e8) = 0;
        *(undefined4 *)(iVar6 + 0x4e0) = 0;
      }
      piVar4 = piVar4 + 1;
      dVar5 = dVar5 - 1;
    } while (dVar5 != 0);
  }
  local_c = this->mbr_0x14;
  if (0 < (int)local_c) {
    local_10 = (int *)this->mbr_0x24;
    iVar6 = 0;
    do {
      iVar2 = *local_10;
      iVar7 = iVar6;
      if (iVar2 != 0) {
        *(undefined4 *)(iVar2 + 0x5c) = 0;
        *(undefined4 *)(iVar2 + 0x54) = 0;
        *(undefined4 *)(iVar2 + 0x58) = 0;
        *(undefined4 *)(iVar2 + 0x50) = 0;
        iVar7 = iVar2;
        if (0 < (int)dVar8) {
          piVar4 = (int *)this->mbr_0x10;
          dVar5 = dVar8;
          do {
            iVar3 = *piVar4;
            if (iVar3 != 0) {
              cVar1 = *(char *)(iVar3 + 0x494);
              if ((cVar1 != '\0') && (*(int *)(iVar3 + 0x4dc) == *(int *)(iVar2 + 0x4c))) {
                *(int *)(iVar2 + 0x50) = *(int *)(iVar2 + 0x50) + *(int *)(iVar3 + 0x650);
                *(int *)(iVar2 + 0x58) = *(int *)(iVar2 + 0x58) + *(int *)(iVar3 + 0x658);
                *(int *)(iVar2 + 0x54) = *(int *)(iVar2 + 0x54) + *(int *)(iVar3 + 0x654);
                *(int *)(iVar2 + 0x5c) = *(int *)(iVar2 + 0x5c) + *(int *)(iVar3 + 0x65c);
              }
              if (((iVar6 != 0) && (cVar1 != '\0')) &&
                 (*(int *)(iVar3 + 0x4dc) == *(int *)(iVar6 + 0x4c))) {
                *(undefined4 *)(iVar3 + 0x4e0) = *(undefined4 *)(iVar6 + 0x50);
                *(undefined4 *)(iVar3 + 0x4e8) = *(undefined4 *)(iVar6 + 0x58);
                *(undefined4 *)(iVar3 + 0x4e4) = *(undefined4 *)(iVar6 + 0x54);
                *(undefined4 *)(iVar3 + 0x4ec) = *(undefined4 *)(iVar6 + 0x5c);
              }
            }
            piVar4 = piVar4 + 1;
            dVar5 = dVar5 - 1;
          } while (dVar5 != 0);
        }
      }
      local_10 = local_10 + 1;
      local_c = local_c - 1;
      iVar6 = iVar7;
    } while (local_c != 0);
    if ((iVar7 != 0) && (0 < (int)dVar8)) {
      piVar4 = (int *)this->mbr_0x10;
      do {
        iVar6 = *piVar4;
        if (((iVar6 != 0) && (*(char *)(iVar6 + 0x494) != '\0')) &&
           (*(int *)(iVar6 + 0x4dc) == *(int *)(iVar7 + 0x4c))) {
          *(undefined4 *)(iVar6 + 0x4e0) = *(undefined4 *)(iVar7 + 0x50);
          *(undefined4 *)(iVar6 + 0x4e8) = *(undefined4 *)(iVar7 + 0x58);
          *(undefined4 *)(iVar6 + 0x4e4) = *(undefined4 *)(iVar7 + 0x54);
          *(undefined4 *)(iVar6 + 0x4ec) = *(undefined4 *)(iVar7 + 0x5c);
        }
        piVar4 = piVar4 + 1;
        dVar8 = dVar8 - 1;
      } while (dVar8 != 0);
    }
  }
  cls_0x5756d0::meth_0x57b1f0((cls_0x5756d0 *)&DAT_00676738);
  return;
}



