// Decompiled methods and structure for class: cls_0x4dbd30

/*
/OOAnalyzer/cls_0x4dbd30
pack(disabled)
Structure cls_0x4dbd30 {
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 204 Alignment: 1

*/

// Function at 004dbd30

void __thiscall
OOAnalyzer::cls_0x4dbd30::meth_0x4dbd30(cls_0x4dbd30 *this,undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  cls_0x5a7b98_TCharacter *this_00;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined4 *puVar8;
  char *in_stack_0000000c;
  undefined2 local_34;
  short local_32;
  undefined2 local_2a;
  dword local_28;
  dword local_24;
  dword local_20;
  
  puVar6 = (undefined4 *)&local_34;
  for (iVar3 = 0xd; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  local_32 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,param_1);
  if (-1 < local_32) {
    local_28 = this->mbr_0xc0;
    local_24 = this->mbr_0xc4;
    local_2a = DAT_00666970;
    local_20 = this->mbr_0xc8;
    local_34 = 0x19;
    uVar2 = cls_0x5a5320_TPlayScreen::meth_0x450e40((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,&local_34);
    this_00 = (cls_0x5a7b98_TCharacter *)FUN_00452690_TMapPane_GetInstance(uVar2,0);
    if (this_00 != (cls_0x5a7b98_TCharacter *)0x0) {
      cls_0x5a7b98_TCharacter::meth_0x4def90(this_00,this,0,0);
      if (in_stack_0000000c != (char *)0x0) {
        uVar4 = 0xffffffff;
        do {
          pcVar7 = in_stack_0000000c;
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          pcVar7 = in_stack_0000000c + 1;
          cVar1 = *in_stack_0000000c;
          in_stack_0000000c = pcVar7;
        } while (cVar1 != '\0');
        uVar4 = ~uVar4;
        puVar6 = (undefined4 *)(pcVar7 + -uVar4);
        puVar8 = (undefined4 *)&(this_00->cls_0x5a50e8).field_0xec;
        for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar8 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar8 = puVar8 + 1;
        }
        for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined *)puVar8 = *(undefined *)puVar6;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
          puVar8 = (undefined4 *)((int)puVar8 + 1);
        }
      }
      *(undefined4 *)&(this_00->cls_0x5a50e8).field_0xe8 = param_2;
    }
  }
  return;
}



// Function at 004dbe10

void __thiscall OOAnalyzer::cls_0x4dbd30::meth_0x4dbe10(cls_0x4dbd30 *this)

{
  cls_0x5b8e94_TObjectInstance *this_00;
  int iVar1;
  int in_stack_00000004;
  cls_0x44ceb0 local_48;
  
  if (in_stack_00000004 != 0) {
    cls_0x44ceb0::cls_0x44ceb0(&local_48,&this->field_0x10,0x400,0x60,0,0);
    this_00 = (cls_0x5b8e94_TObjectInstance *)local_48.mbr_0xc;
    while (this_00 != (cls_0x5b8e94_TObjectInstance *)0x0) {
      local_48.mbr_0xc = (dword)this_00;
      if (((this_00 != (cls_0x5b8e94_TObjectInstance *)0x0) && (this_00->mbr_0x4 == 0x19)) &&
         (iVar1 = FUN_0059a530_stricmp(*(undefined4 *)this_00->mbr_0x4c), iVar1 == 0)) {
        cls_0x5b8e94_TObjectInstance::meth_0x4defe0(this_00);
      }
      cls_0x44ceb0::meth_0x44d080(&local_48);
      this_00 = (cls_0x5b8e94_TObjectInstance *)local_48.mbr_0xc;
    }
  }
  return;
}



