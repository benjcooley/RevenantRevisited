#include "TComplexObject.h"

// Decompiled methods and structure for class: TComplexObject



// Function at 00529830

void TComplexObject::meth_0x529830()

{
  dword *pdVar1;
  undefined2 uVar2;
  dword dVar3;
  dword *pdVar4;
  undefined4 uVar5;
  undefined2 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  cls_0x5a5ff0 *in_stack_00000004;
  uint local_4;
  
  if (DAT_0065b550 == this) {
    cls_0x52c5c0::meth_0x52c5c0((cls_0x52c5c0 *)&UNK_0065b48c.field_0x64);
  }
  cls_0x5a5ff0::meth_0x49ccc0(in_stack_00000004);
  pdVar1 = &this->root;
  dVar3 = *pdVar1;
  if ((int)((in_stack_00000004->mbr_0xc - in_stack_00000004->mbr_0x8) + in_stack_00000004->mbr_0x4)
      < 4) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
  }
  pdVar4 = (dword *)in_stack_00000004->mbr_0x8;
  local_4 = 0;
  *pdVar4 = dVar3;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar4 + 1);
  piVar9 = (int *)this->mbr_0x34;
  for (; (pdVar1 != (dword *)0x0 && (local_4 < *pdVar1)); local_4 = local_4 + 1) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
    uVar5 = *(undefined4 *)*piVar9;
    if ((int)((in_stack_00000004->mbr_0xc - in_stack_00000004->mbr_0x8) + in_stack_00000004->mbr_0x4
             ) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
    }
    puVar7 = (undefined4 *)in_stack_00000004->mbr_0x8;
    iVar8 = *piVar9;
    dVar3 = in_stack_00000004->mbr_0xc;
    *puVar7 = uVar5;
    uVar5 = *(undefined4 *)(iVar8 + 4);
    puVar7 = puVar7 + 1;
    in_stack_00000004->mbr_0x8 = (dword)puVar7;
    if ((int)((dVar3 - (int)puVar7) + in_stack_00000004->mbr_0x4) < 4) {
      cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
    }
    puVar7 = (undefined4 *)in_stack_00000004->mbr_0x8;
    *puVar7 = uVar5;
    in_stack_00000004->mbr_0x8 = (dword)(puVar7 + 1);
    iVar8 = *piVar9;
    iVar10 = 0;
    if (0 < *(int *)(iVar8 + 4)) {
      do {
        uVar2 = *(undefined2 *)(*(int *)(iVar8 + 8) + iVar10 * 2);
        if ((int)((in_stack_00000004->mbr_0xc - in_stack_00000004->mbr_0x8) +
                 in_stack_00000004->mbr_0x4) < 2) {
          cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
        }
        puVar6 = (undefined2 *)in_stack_00000004->mbr_0x8;
        *puVar6 = uVar2;
        in_stack_00000004->mbr_0x8 = (dword)(puVar6 + 1);
        iVar8 = *piVar9;
        iVar10 = iVar10 + 1;
      } while (iVar10 < *(int *)(iVar8 + 4));
    }
    piVar9 = piVar9 + 1;
  }
  return;
}



