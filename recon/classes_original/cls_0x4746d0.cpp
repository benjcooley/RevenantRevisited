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
          (cls_0x4746d0 *this,char *param_1,undefined4 param_2,undefined param_3,undefined param_4,
          undefined4 param_5)

{
  char cVar1;
  word wVar2;
  int iVar3;
  int iVar4;
  DWORD DVar5;
  uint uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  dword dVar10;
  int iVar11;
  uint uVar13;
  int *piVar14;
  int iVar15;
  undefined4 *puVar16;
  char *pcVar17;
  undefined4 *puVar18;
  word *pwVar19;
  undefined3 in_stack_0000000d;
  undefined3 in_stack_00000011;
  undefined4 in_stack_00000018;
  int local_80;
  int iStack_7c;
  undefined auStack_70 [100];
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  uint uVar12;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d33e;
  pvStack_c = ExceptionList;
  iVar15 = 0;
  ExceptionList = &pvStack_c;
  if (0 < DAT_0065a248) {
    piVar14 = &DAT_00659d48;
    ExceptionList = &pvStack_c;
    do {
      iVar4 = FUN_0059a530_stricmp(param_1,*(undefined4 *)(*piVar14 + 4));
      if (iVar4 == 0) {
        local_80 = (&DAT_00659d48)[iVar15];
        if (local_80 != 0) goto LAB_00474779;
        break;
      }
      iVar15 = iVar15 + 1;
      piVar14 = piVar14 + 1;
    } while (iVar15 < DAT_0065a248);
  }
  dVar10 = this->mbr_0x4;
  iVar15 = 0;
  if (0 < DAT_0065a248) {
    piVar14 = &DAT_00659d48;
    do {
      iVar4 = FUN_0059a530_stricmp(dVar10,*(undefined4 *)(*piVar14 + 4));
      if (iVar4 == 0) {
        local_80 = (&DAT_00659d48)[iVar15];
        goto LAB_00474779;
      }
      iVar15 = iVar15 + 1;
      piVar14 = piVar14 + 1;
    } while (iVar15 < DAT_0065a248);
  }
  local_80 = 0;
LAB_00474779:
  if (_param_3 == 0) {
    iVar15 = FUN_00483300_RandomRange(0,0xff);
    DVar5 = GetTickCount();
    _param_3 = (iVar15 << 8 | DVar5 & 0xff) << 0x10 | DAT_00659d44 & 0xffff;
    DAT_00659d44._0_2_ = (short)DAT_00659d44 + 1;
  }
  iVar15 = FUN_00446270(param_2,0,0,0,0,in_stack_00000018);
  if (iVar15 < 0) {
    FUN_0058b100(auStack_70,s_Unable_to_find_imagery_file___s__005d4cd4,param_2,param_1);
    FUN_004820b0(auStack_70);
    iVar15 = -1;
  }
  else {
    if (DAT_0065a260 == 0) {
      iVar4 = 0;
      iStack_7c = DAT_00659d18 - 1;
      uVar13 = DAT_00659d18;
      if (-1 < iStack_7c) {
        do {
          uVar6 = (int)uVar13 / 2;
          if (uVar6 == 0) {
            if ((uVar13 != 0) && (_param_3 == *(uint *)(*(int *)(DAT_00659d28 + iVar4 * 4) + 0x1c)))
            {
              ExceptionList = pvStack_c;
              return -1;
            }
            break;
          }
          uVar12 = uVar6;
          if ((uVar13 & 1) == 0) {
            uVar12 = uVar6 - 1;
          }
          iVar11 = uVar12 + iVar4;
          uVar12 = *(uint *)(*(int *)(DAT_00659d28 + iVar11 * 4) + 0x1c);
          if (_param_3 == uVar12) {
            ExceptionList = pvStack_c;
            return -1;
          }
          if (_param_3 < uVar12) {
            iStack_7c = iVar11 + -1;
            if ((uVar13 & 1) == 0) {
              uVar6 = uVar6 - 1;
            }
          }
          else {
            iVar4 = iVar11 + 1;
          }
          uVar13 = uVar6;
        } while (iVar4 <= iStack_7c);
      }
    }
    puVar7 = (undefined4 *)FUN_00482fb0(0x28);
    uStack_4 = 0;
    if (puVar7 == (undefined4 *)0x0) {
      puVar7 = (undefined4 *)0x0;
    }
    else {
      dVar10 = this->mbr_0x8;
      cls_0x478320::cls_0x478320((cls_0x478320 *)(puVar7 + 3));
      uStack_4._0_1_ = 1;
      cls_0x478320::cls_0x478320((cls_0x478320 *)(puVar7 + 5));
      uVar13 = 0xffffffff;
      uStack_4 = CONCAT31(uStack_4._1_3_,2);
      pcVar17 = param_1;
      do {
        if (uVar13 == 0) break;
        uVar13 = uVar13 - 1;
        cVar1 = *pcVar17;
        pcVar17 = pcVar17 + 1;
      } while (cVar1 != '\0');
      puVar8 = (undefined4 *)FUN_00482ef0(~uVar13);
      uVar13 = 0xffffffff;
      do {
        pcVar17 = param_1;
        if (uVar13 == 0) break;
        uVar13 = uVar13 - 1;
        pcVar17 = param_1 + 1;
        cVar1 = *param_1;
        param_1 = pcVar17;
      } while (cVar1 != '\0');
      uVar13 = ~uVar13;
      puVar16 = (undefined4 *)(pcVar17 + -uVar13);
      puVar18 = puVar8;
      for (uVar6 = uVar13 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *puVar18 = *puVar16;
        puVar16 = puVar16 + 1;
        puVar18 = puVar18 + 1;
      }
      for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
        *(undefined *)puVar18 = *(undefined *)puVar16;
        puVar16 = (undefined4 *)((int)puVar16 + 1);
        puVar18 = (undefined4 *)((int)puVar18 + 1);
      }
      *puVar7 = puVar8;
      puVar7[1] = local_80;
      iStack_7c._0_1_ = (undefined)dVar10;
      puVar7[2] = iVar15;
      puVar7[7] = _param_3;
      *(undefined *)((int)puVar7 + 0x22) = (undefined)iStack_7c;
      *(undefined2 *)(puVar7 + 8) = 0;
      cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar7 + 3));
      cls_0x478320::meth_0x478340((cls_0x478320 *)(puVar7 + 5));
      if (_param_4 != (char *)0x0) {
        uVar13 = 0xffffffff;
        pcVar17 = _param_4;
        do {
          if (uVar13 == 0) break;
          uVar13 = uVar13 - 1;
          cVar1 = *pcVar17;
          pcVar17 = pcVar17 + 1;
        } while (cVar1 != '\0');
        puVar8 = (undefined4 *)FUN_00482ef0(~uVar13);
        uVar13 = 0xffffffff;
        do {
          pcVar17 = _param_4;
          if (uVar13 == 0) break;
          uVar13 = uVar13 - 1;
          pcVar17 = _param_4 + 1;
          cVar1 = *_param_4;
          _param_4 = pcVar17;
        } while (cVar1 != '\0');
        uVar13 = ~uVar13;
        puVar16 = (undefined4 *)(pcVar17 + -uVar13);
        puVar18 = puVar8;
        for (uVar6 = uVar13 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
          *puVar18 = *puVar16;
          puVar16 = puVar16 + 1;
          puVar18 = puVar18 + 1;
        }
        for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
          *(undefined *)puVar18 = *(undefined *)puVar16;
          puVar16 = (undefined4 *)((int)puVar16 + 1);
          puVar18 = (undefined4 *)((int)puVar18 + 1);
        }
        puVar7[9] = puVar8;
      }
      *(undefined *)((int)puVar7 + 0x23) = (undefined)param_5;
    }
    uStack_4 = 0xffffffff;
    iVar15 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x24);
    *(short *)(puVar7 + 8) = (short)iVar15;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_00659d18);
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_00659d30);
    if (DAT_0065a260 == 0) {
      FUN_0058c9ff(DAT_00659d28,DAT_00659d18,4,&LAB_00474b00);
      FUN_0058c9ff(DAT_00659d40,DAT_00659d30,4,&LAB_00474b20);
    }
    wVar2 = this->mbr_0x14;
    pwVar19 = &this->mbr_0x14;
    cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)(puVar7 + 3));
    *(word *)(cls_0x4785e0 *)(puVar7 + 3) = wVar2;
    iVar4 = 0;
    if (0 < (short)*pwVar19) {
      do {
        iVar11 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)&this->mbr_0x24);
        iVar3 = *(int *)(iVar11 + 0x10);
        iVar11 = iVar4 * 4;
        uVar9 = cls_0x429970::meth_0x429970((cls_0x429970 *)pwVar19);
        wVar2 = *pwVar19;
        iVar4 = iVar4 + 1;
        *(undefined4 *)(iVar3 + iVar11) = uVar9;
      } while (iVar4 < (short)wVar2);
    }
    wVar2 = this->mbr_0x1c;
    pwVar19 = &this->mbr_0x1c;
    cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)(puVar7 + 5));
    *(word *)(cls_0x4785e0 *)(puVar7 + 5) = wVar2;
    iVar4 = 0;
    if (0 < (short)*pwVar19) {
      do {
        dVar10 = *(dword *)(this->mbr_0x34 + iVar15 * 4);
        if (dVar10 == 0) {
          dVar10 = this->mbr_0x38;
        }
        iVar3 = *(int *)(dVar10 + 0x18);
        iVar11 = iVar4 * 4;
        uVar9 = cls_0x429970::meth_0x429970((cls_0x429970 *)pwVar19);
        wVar2 = *pwVar19;
        iVar4 = iVar4 + 1;
        *(undefined4 *)(iVar3 + iVar11) = uVar9;
      } while (iVar4 < (short)wVar2);
    }
    DAT_0065a25c = 1;
  }
  ExceptionList = pvStack_c;
  return iVar15;
}



// Function at 00474bb0

int __thiscall OOAnalyzer::cls_0x4746d0::meth_0x474bb0_NewObject(cls_0x4746d0 *this,int param_1,int param_2)

{
  void *pvVar1;
  uint uVar2;
  dword dVar3;
  int iVar4;
  undefined4 uVar5;
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
  if (param_2 < 0) {
    dVar3 = *(dword *)(this->mbr_0x34 + uVar2 * 4);
    if (dVar3 == 0) {
      dVar3 = this->mbr_0x38;
    }
    param_2 = *(int *)(dVar3 + 8);
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
    uVar5 = FUN_0044ce30();
    *(undefined4 *)(iVar4 + 0x40) = uVar5;
  }
  ExceptionList = local_14;
  return iVar4;
}



// Function at 00475210

uint __thiscall OOAnalyzer::cls_0x4746d0::meth_0x475210(cls_0x4746d0 *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  int in_stack_00000008;
  
  piVar3 = (int *)this->mbr_0x34;
  uVar4 = 0;
  do {
    while( true ) {
      if ((&this->mbr_0x24 == (dword *)0x0) || (this->mbr_0x24 <= uVar4)) {
        return 0xffffffff;
      }
      puVar1 = (undefined4 *)*piVar3;
      if (puVar1 != (undefined4 *)0x0) break;
LAB_0047525c:
      piVar3 = piVar3 + 1;
      uVar4 = uVar4 + 1;
    }
    if (in_stack_00000008 == 0) {
      iVar2 = FUN_0059a530_stricmp(*puVar1,param_1);
      if (iVar2 == 0) {
        return uVar4;
      }
      goto LAB_0047525c;
    }
    iVar2 = FUN_004796a0(param_1,*puVar1);
    if (0 < iVar2) {
      return uVar4;
    }
    piVar3 = piVar3 + 1;
    uVar4 = uVar4 + 1;
  } while( true );
}



