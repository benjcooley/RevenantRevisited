#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5790b0



// Function at 005790b0

bool cls_0x5790b0::meth_0x5790b0(undefined param_1, undefined4 param_2)

{
  dword **ppdVar1;
  int iVar2;
  dword *pdVar3;
  dword **ppdVar4;
  int **ppiVar5;
  int iVar6;
  undefined3 in_stack_00000005;
  int *in_stack_0000000c;
  int in_stack_00000010;
  int iVar7;
  
  iVar6 = *in_stack_0000000c;
  iVar2 = in_stack_0000000c[3];
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  iVar7 = 0;
  iVar6 = (**(code **)(*(int *)this->mbr_0x0 + 0xc4))
                    ((int *)this->mbr_0x0,this->mbr_0x14c,param_2,in_stack_00000010 != 0 | 0x200,
                     iVar6,iVar2 - iVar6,0,0,_param_1,&stack0x0000000c);
  if ((iVar6 == 0) || (iVar6 == -0x7ffffff6)) {
    (**(code **)(*_param_1 + 4))();
    ppdVar1 = (dword **)(_param_1 + 2);
    _param_1[5] = iVar7;
    _param_1[6] = 1;
    pdVar3 = *ppdVar1;
    if (pdVar3 != (dword *)0x0) {
      ppdVar4 = (dword **)_param_1[3];
      pdVar3[1] = (dword)ppdVar4;
      *ppdVar4 = pdVar3;
      *ppdVar1 = (dword *)0x0;
    }
    ppiVar5 = (int **)this->mbr_0x158;
    *ppdVar1 = &this->mbr_0x154;
    _param_1[3] = (int)ppiVar5;
    this->mbr_0x158 = (dword)ppdVar1;
    *ppiVar5 = (int *)ppdVar1;
    iVar6 = 0;
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  return iVar6 == 0;
}



