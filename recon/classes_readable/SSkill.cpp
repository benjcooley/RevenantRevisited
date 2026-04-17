#include "SSkill.h"

// Decompiled methods and structure for class: SSkill



// Function at 005877a0

undefined4 SSkill::meth_0x5877a0()

{
  int iVar1;
  UINT UVar2;
  cls_0x44ceb0 local_48;
  
  if (DAT_00676828 != 0) {
    if ((DAT_0067682c == 0) && (this->children == 0)) {
      return 0;
    }
    iVar1 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
    if ((int)(this->ancestor + 0x78) < iVar1) {
      this->ancestor = 0;
    }
    iVar1 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
    if (iVar1 < (int)this->ancestor) {
      return 0;
    }
    if (this->difficulty == 0xffffffff) {
      FUN_00482160(s_Multiplayer_005e6550);
      UVar2 = FUN_00482280(s_MaxMonsters_005e655c,8,(byte *)0x0);
      this->difficulty = UVar2;
    }
    cls_0x44ceb0::meth_0x44cf80(&local_48,0,0,2,0);
    iVar1 = 0;
    while (local_48.mbr_0xc != 0) {
      if ((*(short *)(local_48.mbr_0xc + 4) == 0xc) &&
         (iVar1 = iVar1 + 1, (int)this->difficulty < iVar1)) {
        return 0;
      }
      cls_0x44ceb0::meth_0x44d080(&local_48);
    }
  }
  return 1;
}



// Function at 00587880

void SSkill::meth_0x587880()

{
  int iVar1;
  
  iVar1 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
  this->ancestor = iVar1 + 0x60;
  return;
}



