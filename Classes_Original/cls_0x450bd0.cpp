// Decompiled methods and structure for class: cls_0x450bd0

/*
/OOAnalyzer/cls_0x450bd0
pack(disabled)
Structure cls_0x450bd0 {
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 128 Alignment: 1

*/

// Function at 00450bd0

void __thiscall OOAnalyzer::cls_0x450bd0::meth_0x450bd0(cls_0x450bd0 *this,int param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int in_stack_0000000c;
  uint local_54;
  uint local_50;
  int local_4c;
  cls_0x44ceb0 local_48;
  
  if (-1 < param_1) {
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0x80,0,0);
    while ((int *)local_48.mbr_0xc != (int *)0x0) {
      if (*(int *)(local_48.mbr_0xc + 0x40) == param_1) goto LAB_00450c25;
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
    local_48.mbr_0xc = cls_0x51f330::meth_0x51f330((cls_0x51f330 *)&DAT_0065a890);
LAB_00450c25:
    if ((int *)local_48.mbr_0xc != (int *)0x0) {
      uVar1 = *(uint *)(local_48.mbr_0xc + 0x10);
      local_4c = *(int *)(local_48.mbr_0xc + 0x18);
      uVar2 = *(uint *)(local_48.mbr_0xc + 0x14);
      local_54 = uVar1;
      local_50 = uVar2;
      FUN_0046dad0(this->mbr_0x78 + param_2,this->mbr_0x7c + in_stack_0000000c,&local_54,local_4c);
      if (0 < (int)DAT_00656f00) {
        if (DAT_00656f00 == 0) {
          iVar3 = 0;
        }
        else {
          iVar3 = *DAT_00656f10;
        }
        if ((*(uint *)(iVar3 + 0x14) >> 0x10 & 1) != 0) {
          local_54 = local_54 & 0xfffffff0;
          local_50 = local_50 & 0xfffffff0;
        }
      }
      if (0 < (int)DAT_00656f00) {
        if (DAT_00656f00 < 2) {
          iVar3 = 0;
        }
        else {
          iVar3 = DAT_00656f10[1];
        }
        if ((~*(uint *)(iVar3 + 0x14) >> 0x10 & 1) != 0) {
          local_54 = uVar1;
        }
        if (0 < (int)DAT_00656f00) {
          if (DAT_00656f00 < 3) {
            iVar3 = 0;
          }
          else {
            iVar3 = DAT_00656f10[2];
          }
          if ((~*(uint *)(iVar3 + 0x14) >> 0x10 & 1) != 0) {
            local_50 = uVar2;
          }
        }
      }
      (**(code **)(*(int *)local_48.mbr_0xc + 8))(&local_54,0xffffffff,0);
      UNK_00656e7c._76_4_ = 1;
    }
  }
  return;
}



