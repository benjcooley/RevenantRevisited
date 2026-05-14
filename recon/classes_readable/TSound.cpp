#include "TSound.h"

// Decompiled methods and structure for class: TSound



// Function at 004609f0

undefined4 TSound::meth_0x4609f0()

{
  char cVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  undefined unaff_BL;
  undefined4 unaff_EBP;
  undefined4 unaff_ESI;
  dword in_stack_00000004;
  char *pcVar7;
  TSound *pcVar8;
  int in_stack_fffffefc;
  undefined4 in_stack_ffffff00;
  undefined in_stack_ffffff04;
  undefined4 in_stack_ffffff08;
  undefined in_stack_ffffff0c;
  undefined in_stack_ffffff10;
  undefined in_stack_ffffff14;
  undefined in_stack_ffffff18;
  undefined in_stack_ffffff1c;
  char acStack_2 [2];
  
  if ((((int)in_stack_00000004 < 0) || ((int)this->sound_volume <= (int)in_stack_00000004)) ||
     (this->sound_pos == 0)) {
    return 0;
  }
  if (in_stack_00000004 == this->format) {
    return 1;
  }
  pbVar3 = *(byte **)(this->size + in_stack_00000004 * 4);
  if (pbVar3 == (byte *)0x0) {
    return 0;
  }
  pcVar8 = this;
  if (-1 < (int)this->format) {
    if (DAT_0065b488 != 0) {
      cls_0x49ead0::meth_0x49eff0((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c);
    }
    DAT_0065b8f4 = 0;
    TExecuteBuf::meth_0x49d650((TExecuteBuf *)&UNK_0065d1c0.field_0x310);
    cls_0x41c7d0::meth_0x49b400((cls_0x41c7d0 *)&DAT_00667548);
    this->format = 0xffffffff;
  }
  bVar2 = *pbVar3;
  this->format = in_stack_00000004;
  if ((bVar2 & 1) != 0) {
    return 0;
  }
  FUN_00483120((char *)&DAT_0065d6a4,&stack0xfffffefc,0x104);
  uVar5 = 0xffffffff;
  pcVar7 = &stack0xfffffefc;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar4 = -(~uVar5 - 1);
  _strncpy(&stack0xfffffefb + ~uVar5,(char *)(pbVar3 + 0x58),iVar4 + 0x103);
  (&stack0xfffffefb + ~uVar5)[iVar4 + 0x103] = '\0';
  uVar5 = 0xffffffff;
  pcVar7 = &stack0xfffffefc;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  uVar6 = 0xffffffff;
  (&stack0xfffffefb)[~uVar5] = 0;
  pcVar7 = &stack0xfffffefc;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar4 = -(~uVar6 - 1);
  _strncpy(&stack0xfffffefb + ~uVar6,&DAT_005d0dcc,iVar4 + 0x103);
  (&stack0xfffffefb + ~uVar6)[iVar4 + 0x103] = '\0';
  cls_0x49ead0::meth_0x49ee20((cls_0x49ead0 *)&UNK_0065b2dc.field_0x1c,&stack0xfffffefc);
  if (DAT_0065b2fc == 2) {
    iVar4 = 0;
LAB_00460b33:
    if ((UNK_0065c171._599_1_ & 8) != 0) goto LAB_00460b4c;
    if (iVar4 == 0) goto LAB_00460b5e;
    pcVar7 = s_Error_in_module_file__d_005d0e00;
  }
  else {
    iVar4 = DAT_0065b2fc;
    if (DAT_0065b2fc != 10) goto LAB_00460b33;
LAB_00460b4c:
    pcVar7 = s_Compressed_files_found_in_module_005d0dd4;
  }
  FUN_00481c10((byte *)pcVar7);
LAB_00460b5e:
  uVar6 = 0xffffffff;
  (&stack0xfffffefb)[~uVar5] = 0;
  pcVar7 = &stack0xffffff00;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar4 = -(~uVar6 - 1);
  _strncpy(&stack0xfffffeff + ~uVar6,&DAT_005d0e18,iVar4 + 0x103);
  (&stack0xfffffeff + ~uVar6)[iVar4 + 0x103] = '\0';
  _strncpy(&DAT_0065b8f4,&stack0xffffff00,0x103);
  UNK_0065b8f5._258_1_ = 0;
  iVar4 = in_stack_fffffefc;
  _strncpy((char *)(in_stack_fffffefc + 0x20),&stack0xffffff00,0x103);
  *(undefined *)(in_stack_fffffefc + 0x123) = 0;
  TExecuteBuf::meth_0x49d2a0
            ((TExecuteBuf *)&UNK_0065d1c0.field_0x310,unaff_ESI,unaff_EBP,unaff_BL,pcVar8,iVar4,
             in_stack_ffffff00,in_stack_ffffff04,in_stack_ffffff08,in_stack_ffffff0c,
             in_stack_ffffff10,in_stack_ffffff14,in_stack_ffffff18,in_stack_ffffff1c);
  cls_0x41c7d0::meth_0x49b220((cls_0x41c7d0 *)&DAT_00667548);
  cls_0x41bf00::meth_0x41c000_TAreaMgr_Load((cls_0x41bf00 *)&DAT_0065b8b0);
  return 1;
}



// Function at 00460c60

dword TSound::meth_0x460c60()

{
  dword dVar1;
  uint **ppuVar2;
  dword dVar3;
  uint in_stack_00000004;
  
  if (this->sound_pos == 0) {
    return 0;
  }
  if (in_stack_00000004 != 0) {
    dVar3 = this->sound_volume;
    dVar1 = 0;
    if (0 < (int)dVar3) {
      ppuVar2 = (uint **)this->size;
      do {
        if ((**ppuVar2 & in_stack_00000004) == in_stack_00000004) {
          dVar1 = dVar1 + 1;
        }
        ppuVar2 = ppuVar2 + 1;
        dVar3 = dVar3 - 1;
      } while (dVar3 != 0);
    }
    return dVar1;
  }
  return this->sound_volume;
}



// Function at 00460ca0

uint * TSound::meth_0x460ca0(int param_1)

{
  int iVar1;
  uint **ppuVar2;
  int iVar3;
  uint in_stack_00000008;
  
  if (this->sound_pos != 0) {
    if (in_stack_00000008 == 0) {
      return *(uint **)(this->size + param_1 * 4);
    }
    iVar3 = 0;
    iVar1 = 0;
    if (0 < (int)this->sound_volume) {
      ppuVar2 = (uint **)this->size;
      do {
        if ((**ppuVar2 & in_stack_00000008) == in_stack_00000008) {
          if (iVar3 == param_1) {
            return ((uint **)this->size)[iVar1];
          }
          iVar3 = iVar3 + 1;
        }
        iVar1 = iVar1 + 1;
        ppuVar2 = ppuVar2 + 1;
      } while (iVar1 < (int)this->sound_volume);
    }
  }
  return (uint *)0x0;
}



// Function at 00460d00

undefined4 TSound::meth_0x460d00(byte *param_1)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  uint in_stack_00000008;
  
  if ((this->sound_pos != 0) && (iVar3 = 0, 0 < (int)this->sound_volume)) {
    do {
      puVar1 = *(uint **)(this->size + iVar3 * 4);
      uVar2 = *puVar1 & in_stack_00000008;
      if ((uVar2 == in_stack_00000008) &&
         (uVar2 = FUN_0059a530_stricmp(uVar2,(byte *)(puVar1 + 0x16),param_1), uVar2 == 0)) {
        return *(undefined4 *)(this->size + iVar3 * 4);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this->sound_volume);
  }
  return 0;
}



// Function at 00460d60

int TSound::meth_0x460d60(byte *param_1)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint in_stack_00000008;
  
  if (this->sound_pos != 0) {
    iVar3 = 0;
    iVar4 = 0;
    if (0 < (int)this->sound_volume) {
      do {
        puVar1 = *(uint **)(this->size + iVar4 * 4);
        uVar2 = in_stack_00000008 & *puVar1;
        if (uVar2 == in_stack_00000008) {
          uVar2 = FUN_0059a530_stricmp(uVar2,(byte *)(puVar1 + 0x16),param_1);
          if (uVar2 == 0) {
            return iVar3;
          }
          iVar3 = iVar3 + 1;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)this->sound_volume);
    }
  }
  return -1;
}



