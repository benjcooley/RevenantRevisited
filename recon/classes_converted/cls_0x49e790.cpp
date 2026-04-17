#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49e790



// Function at 0049e790

void cls_0x49e790::meth_0x49e790()

{
  dword dVar1;
  uint uVar2;
  int iVar3;
  int local_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  undefined2 uStack_2;
  
  if ((this->mbr_0xc != 0) && (this->mbr_0x8 != 0)) {
    uVar2 = this->mbr_0x28 + this->mbr_0x2c;
    this->mbr_0x28 = uVar2;
    if (0xffff < (int)uVar2) {
      this->mbr_0x28 = uVar2 & 0xffff;
      if ((int)this->mbr_0x14 <= (int)this->mbr_0x1c) {
        iVar3 = 0;
        if (0 < (int)this->mbr_0x1c) {
          do {
            FUN_0058b100((char *)&local_54,(byte *)s_video_03d_bmp_005dacf8);
            FUN_004a2960(*(int **)(*(int *)(this->mbr_0x10 + iVar3 * 4) + 100),&local_54,1);
            iVar3 = iVar3 + 1;
          } while (iVar3 < (int)this->mbr_0x1c);
        }
        dVar1 = this->mbr_0x1c;
        this->mbr_0x1c = 0;
        this->mbr_0x18 = this->mbr_0x18 + dVar1;
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      uStack_20 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
      uStack_24 = *(undefined4 *)(PTR_DAT_005d79e0 + 4);
      local_54 = -0x80000000;
      uStack_50 = 0;
      uStack_4c = 0;
      uStack_48 = 0;
      uStack_2c = 0;
      uStack_28 = 0;
      uStack_1c = 0;
      uStack_18 = 0;
      uStack_2 = 0;
      uStack_4 = 0;
      uStack_40 = 0;
      uStack_44 = 0;
      uStack_30 = 0;
      uStack_34 = 0;
      uStack_38 = 0;
      uStack_3c = 0;
      uStack_8 = 0x1f;
      uStack_c = 0;
      uStack_14 = uStack_24;
      uStack_10 = uStack_20;
      (**(code **)(**(int **)(this->mbr_0x10 + this->mbr_0x1c * 4) + 0x5c))
                (&local_54,PTR_DAT_005d79e0,0,0);
      this->mbr_0x1c = this->mbr_0x1c + 1;
    }
  }
  return;
}



