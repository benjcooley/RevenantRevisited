// Decompiled methods and structure for class: cls_0x529770

/*
/OOAnalyzer/cls_0x529770
pack(disabled)
Structure cls_0x529770 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 40 Alignment: 1

*/

// Function at 00529770

void __thiscall OOAnalyzer::cls_0x529770::meth_0x529770(cls_0x529770 *this)

{
  undefined4 *puVar1;
  undefined2 *puVar2;
  cls_0x49ce00 *pcVar3;
  undefined4 *puVar4;
  undefined2 *puVar5;
  int iVar6;
  cls_0x49ce00 *in_stack_00000004;
  
  pcVar3 = in_stack_00000004;
  cls_0x49ce00::cls_0x49ce00(in_stack_00000004);
  iVar6 = *(int *)in_stack_00000004->mbr_0x4;
  in_stack_00000004->mbr_0x4 = (dword)((int *)in_stack_00000004->mbr_0x4 + 1);
  in_stack_00000004 = (cls_0x49ce00 *)iVar6;
  if (0 < iVar6) {
    do {
      puVar4 = (undefined4 *)FUN_00482fb0(0xc);
      if (puVar4 == (undefined4 *)0x0) {
        puVar4 = (undefined4 *)0x0;
      }
      else {
        *puVar4 = 0xffffffff;
        puVar4[2] = 0;
        puVar4[1] = 0;
      }
      puVar1 = (undefined4 *)pcVar3->mbr_0x4;
      iVar6 = puVar1[1];
      *puVar4 = *puVar1;
      pcVar3->mbr_0x4 = (dword)(puVar1 + 2);
      puVar4[1] = iVar6;
      puVar5 = (undefined2 *)FUN_00482ef0(iVar6 << 1);
      iVar6 = puVar4[1];
      puVar4[2] = puVar5;
      if (0 < iVar6) {
        do {
          puVar2 = (undefined2 *)pcVar3->mbr_0x4;
          *puVar5 = *puVar2;
          iVar6 = iVar6 + -1;
          pcVar3->mbr_0x4 = (dword)(puVar2 + 1);
          puVar5 = puVar5 + 1;
        } while (iVar6 != 0);
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x24);
      in_stack_00000004 = (cls_0x49ce00 *)((int)in_stack_00000004 + -1);
    } while (in_stack_00000004 != (cls_0x49ce00 *)0x0);
  }
  return;
}



