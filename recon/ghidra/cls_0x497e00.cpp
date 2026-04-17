// Decompiled methods and structure for class: cls_0x497e00

/*
/OOAnalyzer/cls_0x497e00
pack(disabled)
Structure cls_0x497e00 {
   32792   dword   4   mbr_0x8018   "Unsigned Double-Word (ddw, 4-bytes)"
   32808   dword   4   mbr_0x8028   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32812 Alignment: 1

*/

// Function at 00497e00

void __thiscall OOAnalyzer::cls_0x497e00::meth_0x497e00(cls_0x497e00 *this,int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int in_stack_00000008;
  
  uVar3 = 0;
  if (0 < (int)this->mbr_0x8018) {
    do {
      if (((this->mbr_0x8028 != 0) && (uVar3 < this->mbr_0x8018)) &&
         (*(int *)(this->mbr_0x8028 + uVar3 * 4) != 0)) {
        iVar1 = *(int *)(this->mbr_0x8028 + uVar3 * 4);
        if (param_1 == 1) {
          if (*(int *)(iVar1 + 0xd4) == in_stack_00000008) {
LAB_00497e70:
            *(undefined4 *)(iVar1 + 0xd4) = 0;
LAB_00497e76:
            if (param_1 != 1) {
              if (param_1 != 2) goto LAB_00497ea5;
              goto LAB_00497e8a;
            }
          }
          if (*(int *)(iVar1 + 0xe4) == in_stack_00000008) {
LAB_00497e9f:
            *(undefined4 *)(iVar1 + 0xe4) = 0;
          }
        }
        else {
          if (param_1 != 2) goto LAB_00497e76;
          iVar2 = *(int *)(iVar1 + 0xd4);
          if (((iVar2 != 0) && (*(int *)(iVar2 + 0x44) == in_stack_00000008)) &&
             ((*(byte *)(iVar2 + 10) & 8) == 0)) goto LAB_00497e70;
LAB_00497e8a:
          iVar2 = *(int *)(iVar1 + 0xe4);
          if (((iVar2 != 0) && (*(int *)(iVar2 + 0x44) == in_stack_00000008)) &&
             ((*(byte *)(iVar2 + 10) & 8) == 0)) goto LAB_00497e9f;
        }
      }
LAB_00497ea5:
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)this->mbr_0x8018);
  }
  return;
}



