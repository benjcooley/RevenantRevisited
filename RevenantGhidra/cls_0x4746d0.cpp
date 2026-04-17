// Decompiled methods and structure for class: cls_0x4746d0

/*
/OOAnalyzer/cls_0x4746d0
pack(disabled)
Structure cls_0x4746d0 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   word   2   mbr_0x14   "Unsigned Word (dw, 2-bytes)"
   28   word   2   mbr_0x1c   "Unsigned Word (dw, 2-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 004746d0

int __thiscall
OOAnalyzer::cls_0x4746d0::meth_0x4746d0
          (cls_0x4746d0 *this,byte *param_1,byte *param_2,undefined param_3,undefined param_4,
          undefined4 param_5)

{
  byte bVar1;
  char cVar2;
  word wVar3;
  int iVar4;
  uint uVar5;
  DWORD DVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  int iVar9;
  undefined4 uVar10;
  dword dVar11;
  int iVar12;
  uint uVar14;
  uint uVar15;
  int *piVar16;
  int iVar17;
  byte *pbVar18;
  char *pcVar19;
  undefined4 *puVar20;
  word *pwVar21;
  undefined3 in_stack_0000000d;
  undefined3 in_stack_00000011;
  undefined4 in_stack_00000018;
  int local_80;
  int iStack_7c;
  byte abStack_70 [100];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  undefined4 *puVar13;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d33e;
  pvStack_c = ExceptionList;
  iVar17 = 0;
  ExceptionList = &pvStack_c;
  if (0 < DAT_0065a248) {
    piVar16 = &DAT_00659d48;
    ExceptionList = &pvStack_c;
    do {
      uVar5 = FUN_0059a530(*(uint *)(*piVar16 + 4),param_1,(byte *)*(uint *)(*piVar16 + 4));
      if (uVar5 == 0) {
        local_80 = (&DAT_00659d48)[iVar17];
        if (local_80 != 0) goto LAB_00474779;
        break;
      }
      iVar17 = iVar17 + 1;
      piVar16 = piVar16 + 1;
    } while (iVar17 < DAT_0065a248);
  }
  pbVar18 = (byte *)this->mbr_0x4;
  iVar17 = 0;
  if (0 < DAT_0065a248) {
    piVar16 = &DAT_00659d48;
    do {
      uVar5 = FUN_0059a530(*(uint *)(*piVar16 + 4),pbVar18,(byte *)*(uint *)(*piVar16 + 4));
      if (uVar5 == 0) {
        local_80 = (&DAT_00659d48)[iVar17];
        goto LAB_00474779;
      }
      iVar17 = iVar17 + 1;
      piVar16 = piVar16 + 1;
    } while (iVar17 < DAT_0065a248);
  }
  local_80 = 0;
LAB_00474779:
  if (_param_3 == 0) {
    iVar17 = FUN_00483300(0,0xff);
    DVar6 = GetTickCount();
    _param_3 = (iVar17 << 8 | DVar6 & 0xff) << 0x10 | DAT_00659d44 & 0xffff;
    DAT_00659d44._0_2_ = (short)DAT_00659d44 + 1;
  }
  uVar5 = FUN_00446270(param_2,(undefined4 *)0x0,0,(char *)0x0,0,in_stack_00000018);
  if ((int)uVar5 < 0) {
    FUN_0058b100((char *)abStack_70,(byte *)s_Unable_to_find_imagery_file___s__005d4cd4);
    FUN_004820b0(abStack_70);
    iVar17 = -1;
  }
  else {
    if (DAT_0065a260 == 0) {
      iVar17 = 0;
      iStack_7c = (int)DAT_00659d18 + -1;
      puVar8 = DAT_00659d18;
      if (-1 < iStack_7c) {
        do {
          puVar7 = (undefined4 *)((int)puVar8 / 2);
          if (puVar7 == (undefined4 *)0x0) {
            if ((puVar8 != (undefined4 *)0x0) &&
               (_param_3 == *(uint *)(DAT_00659d28[iVar17] + 0x1c))) {
              ExceptionList = pvStack_c;
              return -1;
            }
            break;
          }
          puVar13 = puVar7;
          if (((uint)puVar8 & 1) == 0) {
            puVar13 = (undefined4 *)((int)puVar7 + -1);
          }
          iVar12 = (int)puVar13 + iVar17;
          if (_param_3 == *(uint *)(DAT_00659d28[iVar12] + 0x1c)) {
            ExceptionList = pvStack_c;
            return -1;
          }
          if (_param_3 < *(uint *)(DAT_00659d28[iVar12] + 0x1c)) {
            iStack_7c = iVar12 + -1;
            if (((uint)puVar8 & 1) == 0) {
              puVar7 = (undefined4 *)((int)puVar7 + -1);
            }
          }
          else {
            iVar17 = iVar12 + 1;
          }
          puVar8 = puVar7;
        } while (iVar17 <= iStack_7c);
      }
    }
    puVar8 = FUN_00482fb0(0x28);
    uStack_4 = 0;
    if (puVar8 == (undefined4 *)0x0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      dVar11 = this->mbr_0x8;
      cls_0x478320::cls_0x478320((cls_0x478320 *)(puVar8 + 3));
      uStack_4._0_1_ = 1;
      cls_0x478320::cls_0x478320((cls_0x478320 *)(puVar8 + 5));
      uVar14 = 0xffffffff;
      uStack_4 = CONCAT31(uStack_4._1_3_,2);
      pbVar18 = param_1;
      do {
        if (uVar14 == 0) break;
        uVar14 = uVar14 - 1;
        bVar1 = *pbVar18;
        pbVar18 = pbVar18 + 1;
      } while (bVar1 != 0);
      puVar7 = FUN_00482ef0(~uVar14);
      uVar14 = 0xffffffff;
      do {
        pbVar18 = param_1;
        if (uVar14 == 0) break;
        uVar14 = uVar14 - 1;
        pbVar18 = param_1 + 1;
        bVar1 = *param_1;
        param_1 = pbVar18;
      } while (bVar1 != 0);
      uVar14 = ~uVar14;
      puVar13 = (undefined4 *)(pbVar18 + -uVar14);
      puVar20 = puVar7;
      for (uVar15 = uVar14 >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
        *puVar20 = *puVar13;
        puVar13 = puVar13 + 1;
        puVar20 = puVar20 + 1;
      }
      for (uVar14 = uVar14 & 3; uVar14 != 0; uVar14 = uVar14 - 1) {
        *(undefined *)puVar20 = *(undefined *)puVar13;
        puVar13 = (undefined4 *)((int)puVar13 + 1);
        puVar20 = (undefined4 *)((int)puVar20 + 1);
      }
      *puVar8 = puVar7;
      puVar8[1] = local_80;
      iStack_7c._0_1_ = (undefined)dVar11;
      puVar8[2] = uVar5;
      puVar8[7] = _param_3;
      *(undefined *)((int)puVar8 + 0x22) = (undefined)iStack_7c;
      *(undefined2 *)(puVar8 + 8) = 0;
      cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar8 + 3));
      cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar8 + 5));
      if (_param_4 != (char *)0x0) {
        uVar5 = 0xffffffff;
        pcVar19 = _param_4;
        do {
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          cVar2 = *pcVar19;
          pcVar19 = pcVar19 + 1;
        } while (cVar2 != '\0');
        puVar7 = FUN_00482ef0(~uVar5);
        uVar5 = 0xffffffff;
        do {
          pcVar19 = _param_4;
          if (uVar5 == 0) break;
          uVar5 = uVar5 - 1;
          pcVar19 = _param_4 + 1;
          cVar2 = *_param_4;
          _param_4 = pcVar19;
        } while (cVar2 != '\0');
        uVar5 = ~uVar5;
        puVar13 = (undefined4 *)(pcVar19 + -uVar5);
        puVar20 = puVar7;
        for (uVar14 = uVar5 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
          *puVar20 = *puVar13;
          puVar13 = puVar13 + 1;
          puVar20 = puVar20 + 1;
        }
        for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
          *(undefined *)puVar20 = *(undefined *)puVar13;
          puVar13 = (undefined4 *)((int)puVar13 + 1);
          puVar20 = (undefined4 *)((int)puVar20 + 1);
        }
        puVar8[9] = puVar7;
      }
      *(undefined *)((int)puVar8 + 0x23) = (undefined)param_5;
    }
    uStack_4 = 0xffffffff;
    iVar17 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x24);
    *(short *)(puVar8 + 8) = (short)iVar17;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_00659d18);
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_00659d30);
    if (DAT_0065a260 == 0) {
      FUN_0058c9ff(DAT_00659d28,DAT_00659d18,4,&LAB_00474b00);
      FUN_0058c9ff(DAT_00659d40,DAT_00659d30,4,&LAB_00474b20);
    }
    wVar3 = this->mbr_0x14;
    pwVar21 = &this->mbr_0x14;
    cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)(puVar8 + 3));
    *(word *)(cls_0x4785e0 *)(puVar8 + 3) = wVar3;
    iVar12 = 0;
    if (0 < (short)*pwVar21) {
      do {
        iVar9 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&this->mbr_0x24);
        iVar4 = *(int *)(iVar9 + 0x10);
        iVar9 = iVar12 * 4;
        uVar10 = cls_0x429970::meth_0x429970((cls_0x429970 *)pwVar21);
        wVar3 = *pwVar21;
        iVar12 = iVar12 + 1;
        *(undefined4 *)(iVar4 + iVar9) = uVar10;
      } while (iVar12 < (short)wVar3);
    }
    wVar3 = this->mbr_0x1c;
    pwVar21 = &this->mbr_0x1c;
    cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)(puVar8 + 5));
    *(word *)(cls_0x4785e0 *)(puVar8 + 5) = wVar3;
    iVar12 = 0;
    if (0 < (short)*pwVar21) {
      do {
        dVar11 = *(dword *)(this->mbr_0x34 + iVar17 * 4);
        if (dVar11 == 0) {
          dVar11 = this->mbr_0x38;
        }
        iVar4 = *(int *)(dVar11 + 0x18);
        iVar9 = iVar12 * 4;
        uVar10 = cls_0x429970::meth_0x429970((cls_0x429970 *)pwVar21);
        wVar3 = *pwVar21;
        iVar12 = iVar12 + 1;
        *(undefined4 *)(iVar4 + iVar9) = uVar10;
      } while (iVar12 < (short)wVar3);
    }
    DAT_0065a25c = 1;
  }
  ExceptionList = pvStack_c;
  return iVar17;
}



// Function at 00474bb0

int __thiscall OOAnalyzer::cls_0x4746d0::meth_0x474bb0(cls_0x4746d0 *this,int param_1,uint param_2)

{
  void *pvVar1;
  uint uVar2;
  dword dVar3;
  int iVar4;
  undefined4 in_stack_0000000c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  pvVar1 = ExceptionList;
  puStack_c = &DAT_005a52e0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  ExceptionList = &local_14;
  if (*(int *)(param_1 + 0xc) < 0) {
    ExceptionList = &local_14;
    *(undefined4 *)(param_1 + 0xc) = 0;
  }
  if (*(int *)(param_1 + 0x10) < 0) {
    *(undefined4 *)(param_1 + 0x10) = 0;
  }
  if (*(int *)(param_1 + 0x14) < 0) {
    *(undefined4 *)(param_1 + 0x14) = 0;
  }
  uVar2 = (uint)*(short *)(param_1 + 2);
  if (this->mbr_0x24 <= uVar2) {
    ExceptionList = pvVar1;
    return 0;
  }
  dVar3 = *(dword *)(this->mbr_0x34 + uVar2 * 4);
  if (dVar3 == 0) {
    dVar3 = this->mbr_0x38;
  }
  if (*(int *)(dVar3 + 4) == 0) {
    ExceptionList = pvVar1;
    return 0;
  }
  if ((int)param_2 < 0) {
    dVar3 = *(dword *)(this->mbr_0x34 + uVar2 * 4);
    if (dVar3 == 0) {
      dVar3 = this->mbr_0x38;
    }
    param_2 = *(uint *)(dVar3 + 8);
  }
  local_8 = 0;
  iVar4 = FUN_00446b10(param_2,in_stack_0000000c);
  if (iVar4 == 0) {
    ExceptionList = local_14;
    return 0;
  }
  local_8 = 1;
  dVar3 = *(dword *)(this->mbr_0x34 + *(short *)(param_1 + 2) * 4);
  if (dVar3 == 0) {
    dVar3 = this->mbr_0x38;
  }
  iVar4 = (**(code **)**(undefined4 **)(dVar3 + 4))(param_1,iVar4);
  if (*(int *)(iVar4 + 0x40) < 1) {
    uVar2 = FUN_0044ce30();
    *(uint *)(iVar4 + 0x40) = uVar2;
  }
  ExceptionList = local_14;
  return iVar4;
}



// Function at 00475210

uint __thiscall OOAnalyzer::cls_0x4746d0::meth_0x475210(cls_0x4746d0 *this,byte *param_1)

{
  char **ppcVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  int in_stack_00000008;
  
  piVar4 = (int *)this->mbr_0x34;
  uVar5 = 0;
  do {
    while( true ) {
      if ((&this->mbr_0x24 == (dword *)0x0) || (this->mbr_0x24 <= uVar5)) {
        return 0xffffffff;
      }
      ppcVar1 = (char **)*piVar4;
      if (ppcVar1 != (char **)0x0) break;
LAB_0047525c:
      piVar4 = piVar4 + 1;
      uVar5 = uVar5 + 1;
    }
    if (in_stack_00000008 == 0) {
      uVar3 = FUN_0059a530((uint)*ppcVar1,(byte *)*ppcVar1,param_1);
      if (uVar3 == 0) {
        return uVar5;
      }
      goto LAB_0047525c;
    }
    iVar2 = FUN_004796a0((char *)param_1,*ppcVar1);
    if (0 < iVar2) {
      return uVar5;
    }
    piVar4 = piVar4 + 1;
    uVar5 = uVar5 + 1;
  } while( true );
}



