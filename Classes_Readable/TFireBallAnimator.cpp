#include "TFireBallAnimator.h"

// Decompiled methods and structure for class: TFireBallAnimator



// Function at 0041cdd0

undefined4 TFireBallAnimator::meth_0x41cdd0(int *param_1)

{
  undefined uVar1;
  undefined2 uVar2;
  ushort uVar3;
  undefined2 uVar4;
  dword dVar5;
  char cVar6;
  byte bVar9;
  int *piVar7;
  uint uVar8;
  int iVar10;
  int iVar11;
  uint uVar12;
  int iVar13;
  ushort *puVar14;
  ushort *puVar15;
  undefined4 *puVar16;
  undefined4 *puVar17;
  int iVar18;
  undefined4 *puVar19;
  undefined4 *puVar20;
  int in_stack_00000008;
  int local_c;
  int *local_8;
  
  if ((param_1 == (int *)0x0) || (this->next_state == 0)) {
    return 0;
  }
  FUN_00482130();
  iVar13 = 0;
  local_8 = (int *)this->glow_frame;
  this->ring = this->ring + 1;
  iVar18 = 0;
  piVar7 = (int *)this->frame_count;
  iVar11 = 0x7fffffff;
  local_c = 0;
  if (0 < (int)this->trail) {
    iVar10 = (int)local_8 - (int)piVar7;
    do {
      if (*piVar7 == *param_1) {
        *(dword *)(this->glow_frame + iVar18 * 4) = this->ring;
        FUN_00482140();
        return *(undefined4 *)(this->next_state + iVar18 * 4);
      }
      if (*(int *)(iVar10 + (int)piVar7) < iVar11) {
        iVar11 = *local_8;
        iVar13 = iVar18;
        local_c = iVar18;
      }
      iVar18 = iVar18 + 1;
      local_8 = local_8 + 1;
      piVar7 = piVar7 + 1;
    } while (iVar18 < (int)this->trail);
  }
  dVar5 = this->spark;
  *(dword *)(this->glow_frame + iVar13 * 4) = this->ring;
  *(dword *)(this->next_state + iVar13 * 4) = iVar13 * 0x1000 + dVar5;
  FUN_00482140();
  puVar19 = *(undefined4 **)(this->next_state + iVar13 * 4);
  if (this->next_state == 0) {
    dVar5 = this->next_state;
    *(undefined4 *)(this->frame_count + iVar13 * 4) = 0xffffffff;
    return *(undefined4 *)(dVar5 + iVar13 * 4);
  }
  iVar11 = *param_1;
  iVar13 = 0x400;
  puVar16 = puVar19;
  if (in_stack_00000008 == 1) {
    do {
      *puVar16 = 0;
      puVar16 = puVar16 + 1;
      iVar13 = iVar13 + -1;
    } while (iVar13 != 0);
  }
  else {
    do {
      *puVar16 = 0xffffffff;
      puVar16 = puVar16 + 1;
      iVar13 = iVar13 + -1;
    } while (iVar13 != 0);
  }
  uVar2 = *(undefined2 *)(param_1 + 1);
  iVar13 = 0x40;
  puVar15 = (ushort *)((int)param_1 + 6);
  do {
    while( true ) {
      while( true ) {
        uVar3 = *puVar15;
        cVar6 = (char)uVar3;
        bVar9 = (byte)(uVar3 >> 8);
        if (cVar6 == (char)uVar2) break;
        if (cVar6 == (char)((ushort)uVar2 >> 8)) {
          uVar12 = (uint)bVar9;
          puVar14 = puVar15 + 1;
          puVar15 = puVar15 + 2;
          puVar16 = (undefined4 *)((int)puVar15 + (-4 - (uint)*puVar14));
          if (*puVar14 < 4) {
            do {
              uVar1 = *(undefined *)puVar16;
              puVar16 = (undefined4 *)((int)puVar16 + 1);
              *(undefined *)puVar19 = uVar1;
              puVar19 = (undefined4 *)((int)puVar19 + 1);
              uVar12 = uVar12 - 1;
            } while (uVar12 != 0);
          }
          else {
            uVar12 = (uint)(bVar9 >> 2);
            if (uVar12 != 0) {
              do {
                puVar20 = puVar19;
                puVar17 = puVar16;
                puVar16 = puVar17 + 1;
                *puVar20 = *puVar17;
                puVar19 = puVar20 + 1;
                uVar12 = uVar12 - 1;
              } while (uVar12 != 0);
              if ((bVar9 >> 1 & 1) != 0) {
                uVar4 = *(undefined2 *)puVar16;
                puVar16 = (undefined4 *)((int)puVar17 + 6);
                *(undefined2 *)puVar19 = uVar4;
                puVar19 = (undefined4 *)((int)puVar20 + 6);
              }
            }
            if ((uVar3 & 0x100) != 0) {
              *(undefined *)puVar19 = *(undefined *)puVar16;
              puVar19 = (undefined4 *)((int)puVar19 + 1);
            }
          }
        }
        else {
          *(char *)puVar19 = cVar6;
          puVar19 = (undefined4 *)((int)puVar19 + 1);
          puVar15 = (ushort *)((int)puVar15 + 1);
        }
      }
      puVar14 = puVar15 + 1;
      if (bVar9 == 0) break;
      if (bVar9 < 0x80) {
        uVar8 = bVar9 & 0x7f;
        uVar1 = *(undefined *)puVar14;
        puVar15 = (ushort *)((int)puVar15 + 3);
        for (uVar12 = uVar8 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
          *puVar19 = CONCAT22(CONCAT11(uVar1,uVar1),CONCAT11(uVar1,uVar1));
          puVar19 = puVar19 + 1;
        }
        if ((uVar8 >> 1 & 1) != 0) {
          *(ushort *)puVar19 = CONCAT11(uVar1,uVar1);
          puVar19 = (undefined4 *)((int)puVar19 + 2);
        }
        if ((uVar3 & 0x100) != 0) {
          *(undefined *)puVar19 = uVar1;
          puVar19 = (undefined4 *)((int)puVar19 + 1);
        }
      }
      else {
        puVar19 = (undefined4 *)((int)puVar19 + (bVar9 & 0x7f));
        puVar15 = puVar14;
      }
    }
    iVar13 = iVar13 + -1;
    puVar15 = puVar14;
  } while (iVar13 != 0);
  dVar5 = this->next_state;
  *(int *)(this->frame_count + local_c * 4) = iVar11;
  return *(undefined4 *)(dVar5 + local_c * 4);
}



// Function at 0041d000

undefined4 TFireBallAnimator::meth_0x41d000()

{
  undefined uVar1;
  byte bVar2;
  undefined2 uVar3;
  ushort uVar4;
  int iVar5;
  dword dVar6;
  uint uVar7;
  char cVar8;
  byte bVar10;
  undefined2 uVar9;
  int iVar11;
  int iVar12;
  ushort *puVar13;
  ushort *puVar14;
  undefined *puVar15;
  int iVar16;
  undefined4 *puVar17;
  undefined4 *puVar18;
  int *in_stack_00000004;
  int local_14;
  int local_10;
  uint local_c;
  
  if ((in_stack_00000004 == (int *)0x0) || (this->next_state == 0)) {
    return 0;
  }
  FUN_00482130();
  iVar12 = 0;
  this->old_state = this->old_state + 1;
  iVar16 = 0;
  iVar11 = 0x7fffffff;
  local_14 = 0;
  if (0 < (int)this->trail) {
    do {
      if (*(int *)(this->explode + iVar16 * 4) == *in_stack_00000004) {
        *(dword *)(this->burst + iVar16 * 4) = this->old_state;
        FUN_00482140();
        return *(undefined4 *)(this->fireball_angle + iVar16 * 4);
      }
      iVar5 = *(int *)(this->burst + iVar16 * 4);
      if (iVar5 < iVar11) {
        iVar11 = iVar5;
        iVar12 = iVar16;
        local_14 = iVar16;
      }
      iVar16 = iVar16 + 1;
    } while (iVar16 < (int)this->trail);
  }
  *(dword *)(this->burst + iVar12 * 4) = this->old_state;
  *(dword *)(this->fireball_angle + iVar12 * 4) = iVar12 * 0x2000 + this->firsttime;
  FUN_00482140();
  dVar6 = this->fireball_angle;
  puVar18 = *(undefined4 **)(dVar6 + iVar12 * 4);
  if (this->next_state == 0) {
    *(undefined4 *)(this->explode + iVar12 * 4) = 0xffffffff;
    return *(undefined4 *)(dVar6 + iVar12 * 4);
  }
  bVar2 = 0;
  iVar11 = *in_stack_00000004;
  iVar12 = 0x800;
  puVar17 = puVar18;
  do {
    *puVar17 = 0x7f7f7f7f;
    puVar17 = puVar17 + 1;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  uVar3 = *(undefined2 *)(in_stack_00000004 + 1);
  local_10 = 0x40;
  puVar14 = (ushort *)((int)in_stack_00000004 + 6);
  do {
    while( true ) {
      while( true ) {
        uVar4 = *puVar14;
        cVar8 = (char)uVar4;
        bVar10 = (byte)(uVar4 >> 8);
        if (cVar8 == (char)uVar3) break;
        if (cVar8 == (char)((ushort)uVar3 >> 8)) {
          if (bVar10 == 0) {
            bVar2 = *(byte *)(puVar14 + 1);
            puVar14 = (ushort *)((int)puVar14 + 3);
          }
          else {
            local_c = (uint)bVar10;
            puVar13 = puVar14 + 1;
            puVar14 = puVar14 + 2;
            puVar15 = (undefined *)((int)puVar14 + (-4 - (uint)*puVar13));
            iVar12 = (uint)bVar2 << 8;
            do {
              iVar12 = CONCAT31((int3)((uint)iVar12 >> 8),*puVar15);
              puVar15 = puVar15 + 1;
              *(short *)puVar18 = (short)iVar12;
              puVar18 = (undefined4 *)((int)puVar18 + 2);
              local_c = local_c - 1;
            } while (local_c != 0);
          }
        }
        else {
          *(ushort *)puVar18 = CONCAT11(bVar2,cVar8);
          puVar18 = (undefined4 *)((int)puVar18 + 2);
          puVar14 = (ushort *)((int)puVar14 + 1);
        }
      }
      puVar13 = puVar14 + 1;
      if (bVar10 == 0) break;
      if (bVar10 < 0x80) {
        uVar1 = *(undefined *)puVar13;
        puVar14 = (ushort *)((int)puVar14 + 3);
        uVar9 = CONCAT11(bVar2,uVar1);
        for (uVar7 = (bVar10 & 0x7f) >> 1; uVar7 != 0; uVar7 = uVar7 - 1) {
          *puVar18 = CONCAT22(CONCAT11(bVar2,uVar1),uVar9);
          puVar18 = puVar18 + 1;
        }
        if ((uVar4 & 0x100) != 0) {
          *(undefined2 *)puVar18 = uVar9;
          puVar18 = (undefined4 *)((int)puVar18 + 2);
        }
      }
      else {
        puVar18 = (undefined4 *)((int)puVar18 + (bVar10 & 0x7f) * 2);
        puVar14 = puVar13;
      }
    }
    local_10 = local_10 + -1;
    puVar14 = puVar13;
  } while (local_10 != 0);
  *(int *)(this->explode + local_14 * 4) = iVar11;
  return *(undefined4 *)(this->fireball_angle + local_14 * 4);
}



// Function at 0041d1e0

undefined4 TFireBallAnimator::meth_0x41d1e0()

{
  int iVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  int *in_stack_00000004;
  
  if ((in_stack_00000004 == (int *)0x0) || (this->next_state == 0)) {
    return 0;
  }
  FUN_00482130();
  iVar1 = *in_stack_00000004;
  iVar5 = 0;
  this->old_state = this->old_state + 1;
  iVar6 = 0;
  iVar4 = 0x7fffffff;
  if (0 < (int)this->trail) {
    do {
      if (*(int *)(this->explode + iVar6 * 4) == iVar1) {
        *(dword *)(this->burst + iVar6 * 4) = this->old_state;
        FUN_00482140();
        return *(undefined4 *)(this->fireball_angle + iVar6 * 4);
      }
      iVar2 = *(int *)(this->burst + iVar6 * 4);
      if (iVar2 < iVar4) {
        iVar4 = iVar2;
        iVar5 = iVar6;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->trail);
  }
  *(dword *)(this->burst + iVar5 * 4) = this->old_state;
  *(dword *)(this->fireball_angle + iVar5 * 4) = iVar5 * 0x2000 + this->firsttime;
  FUN_00482140();
  if (this->next_state != 0) {
    dVar3 = this->fireball_angle;
    iVar1 = *in_stack_00000004;
    piVar7 = *(int **)(dVar3 + iVar5 * 4);
    for (iVar4 = 0x800; in_stack_00000004 = in_stack_00000004 + 1, iVar4 != 0; iVar4 = iVar4 + -1) {
      *piVar7 = *in_stack_00000004;
      piVar7 = piVar7 + 1;
    }
    *(int *)(this->explode + iVar5 * 4) = iVar1;
    return *(undefined4 *)(dVar3 + iVar5 * 4);
  }
  dVar3 = this->fireball_angle;
  *(undefined4 *)(this->explode + iVar5 * 4) = 0xffffffff;
  return *(undefined4 *)(dVar3 + iVar5 * 4);
}



