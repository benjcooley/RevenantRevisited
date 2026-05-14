#include "SAutoMapData.h"

// Decompiled methods and structure for class: SAutoMapData



// Function at 00499500

void SAutoMapData::meth_0x499500_TSector_InitLights()

{
  int *piVar1;
  int iVar2;
  
  if ((this->ResID == 0) && (this->Level == 0)) {
    for (iVar2 = 0; iVar2 < (int)this->Top; iVar2 = iVar2 + 1) {
      piVar1 = *(int **)(this->Right + iVar2 * 4);
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)this->Bottom;
      }
      if (*(int *)(this->Left + *piVar1 * 4) != 0) {
        FUN_00471b40();
      }
    }
    this->Level = 1;
  }
  return;
}



