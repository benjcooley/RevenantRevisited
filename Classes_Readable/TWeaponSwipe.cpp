#include "TWeaponSwipe.h"

// Decompiled methods and structure for class: TWeaponSwipe



// Function at 0049e610

void TWeaponSwipe::meth_0x49e610()

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  int local_50 [20];
  
  if (this->weaponmat != 0) {
    if (this->obj != 0) {
      iVar3 = 0;
      if (0 < (int)this->charanim) {
        do {
          FUN_0058b100((char *)local_50,(byte *)s_video_03d_bmp_005dacf8);
          FUN_004a2960(*(int **)(*(int *)(this->initialized + iVar3 * 4) + 100),local_50,1);
          iVar3 = iVar3 + 1;
        } while (iVar3 < (int)this->charanim);
      }
      dVar1 = this->charanim;
      this->charanim = 0;
      this->numverts = this->numverts + dVar1;
      this->obj = 0;
    }
    VirtualUnlock((LPVOID)this->weaponmat,this->maxsegs);
    FUN_00482f80((LPCVOID)this->weaponmat);
    iVar3 = 0;
    this->weaponmat = 0;
    if (0 < (int)this->weaponverts) {
      do {
        puVar2 = *(undefined4 **)(this->initialized + iVar3 * 4);
        if (puVar2 != (undefined4 *)0x0) {
          (**(code **)*puVar2)(1);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < (int)this->weaponverts);
    }
    FUN_004830f0((LPCVOID)this->initialized);
    this->initialized = 0;
  }
  return;
}



