// Decompiled methods and structure for class: cls_0x4e6620

/*
/OOAnalyzer/cls_0x4e6620
pack(disabled)
Structure cls_0x4e6620 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 220 Alignment: 1

*/

// Function at 004e6620

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x4e6620::meth_0x4e6620(cls_0x4e6620 *this)

{
  dword dVar1;
  cls_0x4e6620 *pcVar2;
  int *piVar3;
  int iVar4;
  
  (**(code **)(this->mbr_0x0 + 0x158))(0);
  if ((this->mbr_0xd8 != 0) && (iVar4 = *(int *)(this->mbr_0xd8 + 4), iVar4 != 0)) {
    *(undefined4 *)(iVar4 + 0x1ac) = *(undefined4 *)(iVar4 + 0x10);
    *(undefined4 *)(iVar4 + 0x1a8) = _DAT_00666970;
    *(undefined4 *)(iVar4 + 0x1b0) = *(undefined4 *)(iVar4 + 0x14);
    *(undefined4 *)(iVar4 + 0x1b4) = *(undefined4 *)(iVar4 + 0x18);
    pcVar2 = (cls_0x4e6620 *)FUN_00451d70((byte *)s_SetVortex_005e0f24,1,0);
    if (pcVar2 != (cls_0x4e6620 *)0x0) {
      if (pcVar2 == this) {
        pcVar2 = (cls_0x4e6620 *)FUN_00451d70((byte *)s_SetVortex_005e0f30,2,0);
        if ((pcVar2 != (cls_0x4e6620 *)0x0) && (pcVar2 != this)) {
          dVar1 = pcVar2->mbr_0xd8;
          if ((dVar1 != 0) &&
             ((((*(int *)(dVar1 + 0x11c) != 0 && (*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0)) &&
               (0 < *(int *)(dVar1 + 0xc))) && (*(int *)(dVar1 + 0x10) != 0)))) {
            if (*(int *)(dVar1 + 0xc) < 1) {
              piVar3 = (int *)0x0;
            }
            else {
              piVar3 = *(int **)(dVar1 + 0x10);
            }
            (**(code **)(*piVar3 + 0x1b4))(0);
          }
          (**(code **)(pcVar2->mbr_0x0 + 0x40))(*(uint *)&pcVar2->field_0x8 | 0x1000);
          (**(code **)(pcVar2->mbr_0x0 + 0x40))(*(uint *)&pcVar2->field_0x8 | 0x8000);
          if (pcVar2->mbr_0xd8 != 0) {
            iVar4 = (**(code **)(pcVar2->mbr_0x0 + 0x3c))();
            if (iVar4 != 0) {
              (**(code **)(*(int *)pcVar2->mbr_0xd8 + 0xc))();
            }
          }
        }
      }
      else {
        dVar1 = pcVar2->mbr_0xd8;
        if (((dVar1 != 0) && (*(int *)(dVar1 + 0x11c) != 0)) &&
           ((*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0 &&
            ((0 < *(int *)(dVar1 + 0xc) && (*(int *)(dVar1 + 0x10) != 0)))))) {
          if (*(int *)(dVar1 + 0xc) < 1) {
            piVar3 = (int *)0x0;
          }
          else {
            piVar3 = *(int **)(dVar1 + 0x10);
          }
          (**(code **)(*piVar3 + 0x1b4))(0);
        }
        (**(code **)(pcVar2->mbr_0x0 + 0x40))(*(uint *)&pcVar2->field_0x8 | 0x1000);
        (**(code **)(pcVar2->mbr_0x0 + 0x40))(*(uint *)&pcVar2->field_0x8 | 0x8000);
        if (pcVar2->mbr_0xd8 != 0) {
          iVar4 = (**(code **)(pcVar2->mbr_0x0 + 0x3c))();
          if (iVar4 != 0) {
            (**(code **)(*(int *)pcVar2->mbr_0xd8 + 0xc))();
            return;
          }
        }
      }
    }
  }
  return;
}



