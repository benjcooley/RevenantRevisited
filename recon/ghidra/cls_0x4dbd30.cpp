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
  TPlayScreen *this_00;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  undefined4 *puVar7;
  char *in_stack_0000000c;
  undefined2 local_34;
  short local_32;
  undefined2 local_2a;
  dword local_28;
  dword local_24;
  dword local_20;
  
  puVar5 = (undefined4 *)&local_34;
  for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  local_32 = cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,param_1);
  if (-1 < local_32) {
    local_28 = this->mbr_0xc0;
    local_24 = this->mbr_0xc4;
    local_2a = DAT_00666970;
    local_20 = this->mbr_0xc8;
    local_34 = 0x19;
    iVar2 = TCharacter::meth_0x450e40((TCharacter *)&DAT_006668d8,&local_34);
    this_00 = (TPlayScreen *)FUN_00452690(iVar2,0);
    if (this_00 != (TPlayScreen *)0x0) {
      TPlayScreen::meth_0x4def90(this_00,this,0,0);
      if (in_stack_0000000c != (char *)0x0) {
        uVar3 = 0xffffffff;
        do {
          pcVar6 = in_stack_0000000c;
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          pcVar6 = in_stack_0000000c + 1;
          cVar1 = *in_stack_0000000c;
          in_stack_0000000c = pcVar6;
        } while (cVar1 != '\0');
        uVar3 = ~uVar3;
        puVar5 = (undefined4 *)(pcVar6 + -uVar3);
        puVar7 = (undefined4 *)&(this_00->TScreen).field_0xec;
        for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *puVar7 = *puVar5;
          puVar5 = puVar5 + 1;
          puVar7 = puVar7 + 1;
        }
        for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
          *(undefined *)puVar7 = *(undefined *)puVar5;
          puVar5 = (undefined4 *)((int)puVar5 + 1);
          puVar7 = (undefined4 *)((int)puVar7 + 1);
        }
      }
      *(undefined4 *)&(this_00->TScreen).field_0xe8 = param_2;
    }
  }
  return;
}



// Function at 004dbe10

void __thiscall OOAnalyzer::cls_0x4dbd30::meth_0x4dbe10(cls_0x4dbd30 *this)

{
  SoftwareRenderer *this_00;
  uint uVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  byte *in_stack_00000004;
  cls_0x44ceb0 local_48;
  
  if (in_stack_00000004 != (byte *)0x0) {
    cls_0x44ceb0::cls_0x44ceb0(&local_48,&this->field_0x10,0x400,0x60,0,0);
    uVar1 = extraout_ECX;
    this_00 = (SoftwareRenderer *)local_48.mbr_0xc;
    while (this_00 != (SoftwareRenderer *)0x0) {
      local_48.mbr_0xc = (dword)this_00;
      if (((this_00 != (SoftwareRenderer *)0x0) && (this_00->mbr_0x4 == 0x19)) &&
         (uVar1 = FUN_0059a530(uVar1,*(byte **)this_00->mbr_0x4c,in_stack_00000004), uVar1 == 0)) {
        SoftwareRenderer::meth_0x4defe0(this_00);
      }
      cls_0x44ceb0::meth_0x44d080(&local_48);
      uVar1 = extraout_ECX_00;
      this_00 = (SoftwareRenderer *)local_48.mbr_0xc;
    }
  }
  return;
}



