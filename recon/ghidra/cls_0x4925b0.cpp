// Decompiled methods and structure for class: cls_0x4925b0

/*
/OOAnalyzer/cls_0x4925b0
pack(disabled)
Structure cls_0x4925b0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 004925b0

undefined4 __thiscall
OOAnalyzer::cls_0x4925b0::meth_0x4925b0(cls_0x4925b0 *this,uint param_1,byte *param_2)

{
  dword dVar1;
  uint uVar2;
  uint extraout_ECX;
  int iVar3;
  uint *puVar4;
  byte *in_stack_0000000c;
  
  dVar1 = this->mbr_0x4;
  do {
    if (dVar1 == 0) {
      return 0;
    }
    iVar3 = 0;
    if (0 < *(int *)(dVar1 + 0x44)) {
      do {
        puVar4 = *(uint **)(*(int *)(dVar1 + 0x1c) + iVar3 * 4);
        if (puVar4 == (uint *)0x0) {
          puVar4 = *(uint **)(dVar1 + 0x20);
        }
        if ((puVar4 != (uint *)0x0) && (*puVar4 == param_1)) {
          if (param_2 == (byte *)0x0) {
            uVar2 = param_1;
            if (in_stack_0000000c == (byte *)0x0) {
              return 1;
            }
          }
          else {
            uVar2 = FUN_0059a530_stricmp(param_1,(byte *)(puVar4 + 2),param_2);
            if (uVar2 == 0) {
              return 1;
            }
            uVar2 = extraout_ECX;
            if (in_stack_0000000c == (byte *)0x0) goto LAB_00492619;
          }
          uVar2 = FUN_0059a530_stricmp(uVar2,(byte *)(puVar4 + 2),in_stack_0000000c);
          if (uVar2 == 0) {
            return 1;
          }
        }
LAB_00492619:
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(dVar1 + 0x44));
    }
    dVar1 = *(dword *)(dVar1 + 8);
  } while( true );
}



