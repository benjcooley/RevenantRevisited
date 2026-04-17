#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x578fe0



// Function at 00578fe0

bool cls_0x578fe0::meth_0x578fe0()

{
  dword **ppdVar1;
  dword *pdVar2;
  dword **ppdVar3;
  int **ppiVar4;
  int iVar5;
  uint uVar6;
  int *in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined4 in_stack_0000000c;
  undefined4 in_stack_00000010;
  int in_stack_00000014;
  cls_0x578fe0 *pcStack_4;
  
  pcStack_4 = this;
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  uVar6 = in_stack_00000014 != 0 | 0x200;
  iVar5 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,in_stack_00000008,uVar6,in_stack_0000000c,
                     in_stack_00000010,0,0,in_stack_00000004,&pcStack_4);
  if ((iVar5 == 0) || (iVar5 == -0x7ffffff6)) {
    (**(code **)(*in_stack_00000004 + 4))();
    ppdVar1 = (dword **)(in_stack_00000004 + 2);
    in_stack_00000004[5] = uVar6;
    in_stack_00000004[6] = 1;
    pdVar2 = *ppdVar1;
    if (pdVar2 != (dword *)0x0) {
      ppdVar3 = (dword **)in_stack_00000004[3];
      pdVar2[1] = (dword)ppdVar3;
      *ppdVar3 = pdVar2;
      *ppdVar1 = (dword *)0x0;
    }
    ppiVar4 = (int **)this->mbr_0x158;
    *ppdVar1 = &this->mbr_0x154;
    in_stack_00000004[3] = (int)ppiVar4;
    this->mbr_0x158 = (dword)ppdVar1;
    *ppiVar4 = (int *)ppdVar1;
    iVar5 = 0;
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  return iVar5 == 0;
}



