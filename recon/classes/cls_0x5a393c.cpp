// Decompiled methods and structure for class: cls_0x5a393c

/*
/OOAnalyzer/cls_0x5a393c
pack(disabled)
Structure cls_0x5a393c {
   0   cls_0x5a393c::vftable_5a393c *   4   vftptr_0x0   "pointer to cls_0x5a393c::vftable_5a393c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   52   byte   1   mbr_0x34   "Unsigned Byte (db)"
   53   byte   1   mbr_0x35   "Unsigned Byte (db)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 240 Alignment: 1

*/

// Function at 0040f5b0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x40f5b0(cls_0x5a393c *this)

{
  byte bVar1;
  cls_0x5a486c *pcVar2;
  int *piVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int iVar9;
  float *pfVar10;
  cls_0x45f7c0 *pcVar11;
  float *pfVar12;
  undefined4 uVar13;
  undefined4 *puVar14;
  undefined4 *puVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  dword dVar19;
  int iVar20;
  uint *puVar21;
  uint uVar22;
  int local_108;
  dword local_104;
  uint local_100;
  int local_fc;
  int local_f8;
  int local_f0;
  undefined4 local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  float local_c0 [13];
  float local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 auStack_64 [13];
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  
  iVar20 = 0;
  bVar1 = this->mbr_0x34;
  dVar19 = this->mbr_0x4;
  local_e0 = 2;
  local_dc = 0;
  local_c8 = 0x461c3c00;
  iVar16 = *(int *)(dVar19 + 0x10);
  iVar17 = *(int *)(dVar19 + 0x14);
  iVar18 = *(int *)(dVar19 + 0x18);
  local_d0 = 0x461c3c00;
  local_d8 = 0x461c3c00;
  local_c4 = 0xc61c3c00;
  local_cc = 0xc61c3c00;
  local_d4 = 0xc61c3c00;
  FUN_00417000(&local_e0);
  this->mbr_0xec = 0;
  if ((this->mbr_0x38 == 0) ||
     (iVar9 = FUN_0059a530_stricmp(*(undefined4 *)(*(int *)(this->mbr_0x4 + 0x48) + 4),s_Armor_005c6174),
     iVar9 == 0)) {
    while( true ) {
      pcVar2 = (cls_0x5a486c *)this->mbr_0x8;
      if (pcVar2->mbr_0xc == 0) {
        if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
          cls_0x5a486c::meth_0x447ac0(pcVar2);
        }
        cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
      }
      if ((int)pcVar2->mbr_0x64 <= iVar20) break;
      if (((iVar20 < 0) || (0xff < iVar20)) ||
         (puVar21 = *(uint **)(this->mbr_0x54 + iVar20 * 4), puVar21 == (uint *)0x0)) {
        puVar21 = (uint *)0x0;
      }
      puVar21[10] = 0x3fc90fdb;
      *puVar21 = *puVar21 | 0x6c;
      puVar21[0xb] = 0x3fc90fdb;
      puVar21[0xc] = 0x3fc90fdb;
      puVar21[0x10] = 0x3f800000;
      puVar21[0x11] = 0x3f800000;
      puVar21[0x12] = 0x3f800000;
      dVar19 = this->mbr_0x4;
      puVar21[4] = 0;
      iVar16 = *(int *)(dVar19 + 0x48);
      puVar21[5] = 0;
      puVar21[6] = 0;
      iVar16 = FUN_0059a530_stricmp(*(undefined4 *)(iVar16 + 4),s_Armor_005c617c);
      if (iVar16 == 0) {
        puVar21[0xd2] = 2;
        puVar21[10] = 0;
        puVar21[0xb] = 0;
        puVar21[0xc] = 0xbf490fdb;
        puVar21[0x10] = 0x3fc00000;
        puVar21[0x11] = 0x3fc00000;
        puVar21[0x12] = 0x3fc00000;
      }
      cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
      iVar20 = iVar20 + 1;
    }
  }
  else {
    local_fc = 0x60;
    local_108 = 1;
    iVar20 = DAT_005e8518;
    do {
      local_f0 = 0;
      while( true ) {
        pcVar2 = (cls_0x5a486c *)this->mbr_0x8;
        if (pcVar2->mbr_0xc == 0) {
          if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
            cls_0x5a486c::meth_0x447ac0(pcVar2);
          }
          cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
          iVar20 = DAT_005e8518;
        }
        if ((int)pcVar2->mbr_0x64 <= local_f0) break;
        if (((local_f0 < 0) || (0xff < local_f0)) ||
           (puVar21 = *(uint **)(this->mbr_0x54 + local_f0 * 4), puVar21 == (uint *)0x0)) {
          puVar21 = (uint *)0x0;
        }
        dVar19 = this->mbr_0x38;
        puVar21[4] = (uint)((float)*(double *)(local_fc + 0x30 + dVar19) - (float)iVar16);
        *puVar21 = *puVar21 | 0x100006c;
        puVar21[5] = (uint)((float)*(double *)(local_fc + 0x38 + dVar19) - (float)iVar17);
        puVar21[6] = (uint)((float)*(double *)(local_fc + 0x40 + dVar19) - (float)iVar18);
        puVar21[10] = (uint)(float)*(double *)(local_fc + 0xa8 + dVar19);
        puVar21[0xb] = (uint)(float)*(double *)(local_fc + 0xb0 + dVar19);
        puVar21[0xc] = (uint)(float)*(double *)(local_fc + 0xb8 + dVar19);
        puVar21[0x10] = 0x3f800000;
        puVar21[0x11] = 0x3f800000;
        puVar21[0x12] = 0x3f800000;
        if (local_108 != 5) {
          local_104 = this->mbr_0x8;
          uVar22 = 0;
          if (0 < *(int *)(local_104 + 0x34)) {
            local_f8 = 0;
            fVar4 = (float)local_108 * _DAT_005a36d0;
            do {
              pfVar10 = local_c0;
              for (iVar9 = 0x17; iVar9 != 0; iVar9 = iVar9 + -1) {
                *pfVar10 = 0.0;
                pfVar10 = pfVar10 + 1;
              }
              if (((-1 < local_f8) &&
                  (pcVar11 = (cls_0x45f7c0 *)(local_104 + 0x34),
                  (int)uVar22 < *(int *)(local_104 + 0x34))) &&
                 ((*(int *)(local_104 + 0x44) != 0 &&
                  ((uVar22 < (pcVar11->cls_0x41c7f0).mbr_0x0 &&
                   (*(int *)(*(int *)(local_104 + 0x44) + uVar22 * 4) != 0)))))) {
                if (&stack0x00000000 != (undefined *)0xc0) {
                  pfVar10 = (float *)cls_0x45f7c0::meth_0x410f60(pcVar11);
                  pfVar12 = local_c0;
                  for (iVar20 = 0x17; iVar20 != 0; iVar20 = iVar20 + -1) {
                    *pfVar12 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar12 = pfVar12 + 1;
                  }
                }
                cls_0x45f7c0::meth_0x410f60(pcVar11);
                iVar20 = DAT_005e8518;
              }
              uVar7 = local_84;
              uVar13 = local_88;
              *(float *)(local_f8 + iVar20) = local_c0[1];
              *(float *)(local_f8 + 4 + iVar20) = local_c0[2];
              dVar19 = this->mbr_0x8;
              *(float *)(local_f8 + 8 + iVar20) = local_c0[3];
              local_c0[1] = 0.0;
              local_c0[2] = 0.0;
              *(float *)(local_f8 + 0xc + iVar20) = local_8c;
              local_8c = 0.0;
              local_88 = 0;
              *(undefined4 *)(local_f8 + 0x10 + iVar20) = uVar13;
              local_84 = 0;
              *(undefined4 *)(local_f8 + 0x14 + iVar20) = uVar7;
              local_c0[3] = fVar4;
              if (-1 < local_f8) {
                pcVar11 = (cls_0x45f7c0 *)(dVar19 + 0x34);
                if (((((int)uVar22 < (int)*(uint *)(dVar19 + 0x34)) &&
                     (*(int *)(dVar19 + 0x44) != 0)) && (uVar22 < *(uint *)(dVar19 + 0x34))) &&
                   (*(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4) != 0)) {
                  pfVar12 = (float *)cls_0x45f7c0::meth_0x410f60(pcVar11);
                  pfVar10 = local_c0;
                  for (iVar20 = 0x14; iVar20 != 0; iVar20 = iVar20 + -1) {
                    *pfVar12 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar12 = pfVar12 + 1;
                  }
                  iVar20 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                  *(undefined4 *)(iVar20 + 0x48) = 0;
                  iVar20 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                  piVar3 = *(int **)(iVar20 + 0x50);
                  iVar20 = *piVar3;
                  uVar13 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                  (**(code **)(iVar20 + 0xc))(piVar3,uVar13);
                  iVar20 = DAT_005e8518;
                }
              }
              uVar22 = uVar22 + 1;
              local_f8 = local_f8 + 0x18;
              local_104 = this->mbr_0x8;
            } while ((int)uVar22 < *(int *)(local_104 + 0x34));
          }
          local_100 = puVar21[0xd2];
          *puVar21 = *puVar21 | 0x1000000;
          puVar21[0xd2] = 0x10;
        }
        cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
        if (local_108 == 5) {
LAB_0040fab2:
          local_f0 = local_f0 + 1;
          iVar20 = DAT_005e8518;
        }
        else {
          puVar21[0xd2] = local_100;
          *puVar21 = *puVar21 & 0xfeffffff;
          dVar19 = this->mbr_0x8;
          uVar22 = 0;
          if (*(int *)(dVar19 + 0x34) < 1) goto LAB_0040fab2;
          local_f8 = 0;
          iVar20 = DAT_005e8518;
          do {
            puVar14 = auStack_64;
            for (iVar9 = 0x17; iVar9 != 0; iVar9 = iVar9 + -1) {
              *puVar14 = 0;
              puVar14 = puVar14 + 1;
            }
            if (-1 < local_f8) {
              if ((((int)uVar22 < (int)*(uint *)(dVar19 + 0x34)) && (*(int *)(dVar19 + 0x44) != 0))
                 && ((uVar22 < *(uint *)(dVar19 + 0x34) &&
                     (*(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4) != 0)))) {
                if (&stack0x00000000 != (undefined *)0x64) {
                  puVar14 = (undefined4 *)
                            cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)(dVar19 + 0x34));
                  puVar15 = auStack_64;
                  for (iVar20 = 0x17; iVar20 != 0; iVar20 = iVar20 + -1) {
                    *puVar15 = *puVar14;
                    puVar14 = puVar14 + 1;
                    puVar15 = puVar15 + 1;
                  }
                }
                cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)(dVar19 + 0x34));
                iVar20 = DAT_005e8518;
              }
            }
            auStack_64[1] = *(undefined4 *)(local_f8 + iVar20);
            auStack_64[2] = *(undefined4 *)(local_f8 + 4 + iVar20);
            dVar19 = this->mbr_0x8;
            auStack_64[3] = *(undefined4 *)(local_f8 + 8 + iVar20);
            uStack_30 = *(undefined4 *)(local_f8 + 0xc + iVar20);
            uStack_2c = *(undefined4 *)(local_f8 + 0x10 + iVar20);
            uStack_28 = *(undefined4 *)(local_f8 + 0x14 + iVar20);
            if (-1 < local_f8) {
              pcVar11 = (cls_0x45f7c0 *)(dVar19 + 0x34);
              if ((((int)uVar22 < (int)*(uint *)(dVar19 + 0x34)) && (*(int *)(dVar19 + 0x44) != 0))
                 && ((uVar22 < *(uint *)(dVar19 + 0x34) &&
                     (*(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4) != 0)))) {
                puVar15 = (undefined4 *)cls_0x45f7c0::meth_0x410f60(pcVar11);
                puVar14 = auStack_64;
                for (iVar20 = 0x14; iVar20 != 0; iVar20 = iVar20 + -1) {
                  *puVar15 = *puVar14;
                  puVar14 = puVar14 + 1;
                  puVar15 = puVar15 + 1;
                }
                iVar20 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                *(undefined4 *)(iVar20 + 0x48) = 0;
                iVar20 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                piVar3 = *(int **)(iVar20 + 0x50);
                iVar20 = *piVar3;
                uVar13 = cls_0x45f7c0::meth_0x410f60(pcVar11);
                (**(code **)(iVar20 + 0xc))(piVar3,uVar13);
                iVar20 = DAT_005e8518;
              }
            }
            uVar22 = uVar22 + 1;
            local_f8 = local_f8 + 0x18;
            dVar19 = this->mbr_0x8;
          } while ((int)uVar22 < *(int *)(dVar19 + 0x34));
          local_f0 = local_f0 + 1;
        }
      }
      local_fc = local_fc + -0x18;
      local_108 = local_108 + 1;
    } while (local_108 < 6);
  }
  if ((('\0' < (char)bVar1) && (this->mbr_0x38 == 0)) &&
     ((DAT_00668154 == 0 &&
      (iVar16 = FUN_0059a530_stricmp(*(undefined4 *)(*(int *)(this->mbr_0x4 + 0x48) + 4),s_Armor_005c6184),
      iVar16 != 0)))) {
    dVar19 = this->mbr_0x8;
    uVar22 = 0;
    fVar4 = (float)(int)(char)bVar1 * _DAT_005a37e4;
    if (0 < *(int *)(dVar19 + 0x34)) {
      local_fc = 0;
      do {
        pfVar10 = local_c0;
        for (iVar16 = 0x17; iVar16 != 0; iVar16 = iVar16 + -1) {
          *pfVar10 = 0.0;
          pfVar10 = pfVar10 + 1;
        }
        if (-1 < local_fc) {
          if ((((int)uVar22 < (int)*(uint *)(dVar19 + 0x34)) &&
              (iVar16 = *(int *)(dVar19 + 0x44), iVar16 != 0)) &&
             ((uVar22 < *(uint *)(dVar19 + 0x34) && (*(int *)(iVar16 + uVar22 * 4) != 0)))) {
            if (&stack0x00000000 != (undefined *)0xc0) {
              pfVar10 = *(float **)(iVar16 + uVar22 * 4);
              if (pfVar10 == (float *)0x0) {
                pfVar10 = *(float **)(dVar19 + 0x48);
              }
              pfVar12 = local_c0;
              for (iVar16 = 0x17; iVar16 != 0; iVar16 = iVar16 + -1) {
                *pfVar12 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                pfVar12 = pfVar12 + 1;
              }
            }
            cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)(dVar19 + 0x34));
          }
        }
        uVar8 = local_84;
        uVar7 = local_88;
        fVar6 = local_8c;
        fVar5 = local_c0[3];
        uVar13 = local_c0[2];
        iVar16 = DAT_005e8518;
        *(float *)(local_fc + DAT_005e8518) = local_c0[1];
        local_c0[1] = 1.0;
        local_c0[2] = 0.0;
        *(undefined4 *)(local_fc + 4 + iVar16) = uVar13;
        local_c0[3] = 0.0;
        *(float *)(local_fc + 8 + iVar16) = fVar5;
        local_8c = fVar4;
        *(float *)(local_fc + 0xc + iVar16) = fVar6;
        local_88 = 0;
        *(undefined4 *)(local_fc + 0x10 + iVar16) = uVar7;
        local_84 = 0;
        *(undefined4 *)(local_fc + 0x14 + iVar16) = uVar8;
        dVar19 = this->mbr_0x8;
        if ((((-1 < local_fc) &&
             (pcVar11 = (cls_0x45f7c0 *)(dVar19 + 0x34), (int)uVar22 < *(int *)(dVar19 + 0x34))) &&
            (*(int *)(dVar19 + 0x44) != 0)) &&
           ((uVar22 < (pcVar11->cls_0x41c7f0).mbr_0x0 &&
            (*(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4) != 0)))) {
          pfVar10 = *(float **)(*(int *)(dVar19 + 0x44) + uVar22 * 4);
          if (pfVar10 == (float *)0x0) {
            pfVar10 = *(float **)(dVar19 + 0x48);
          }
          pfVar12 = local_c0;
          for (iVar16 = 0x14; iVar16 != 0; iVar16 = iVar16 + -1) {
            *pfVar10 = *pfVar12;
            pfVar12 = pfVar12 + 1;
            pfVar10 = pfVar10 + 1;
          }
          iVar16 = cls_0x45f7c0::meth_0x410f60(pcVar11);
          *(undefined4 *)(iVar16 + 0x48) = 0;
          iVar16 = cls_0x45f7c0::meth_0x410f60(pcVar11);
          piVar3 = *(int **)(iVar16 + 0x50);
          iVar16 = *piVar3;
          uVar13 = cls_0x45f7c0::meth_0x410f60(pcVar11);
          (**(code **)(iVar16 + 0xc))(piVar3,uVar13);
        }
        uVar22 = uVar22 + 1;
        local_fc = local_fc + 0x18;
        dVar19 = this->mbr_0x8;
      } while ((int)uVar22 < *(int *)(dVar19 + 0x34));
    }
    local_108 = 0;
    while( true ) {
      pcVar2 = (cls_0x5a486c *)this->mbr_0x8;
      if (pcVar2->mbr_0xc == 0) {
        if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
          cls_0x5a486c::meth_0x447ac0(pcVar2);
        }
        cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
      }
      if ((int)pcVar2->mbr_0x64 <= local_108) break;
      if (((local_108 < 0) || (0xff < local_108)) ||
         (puVar21 = *(uint **)(this->mbr_0x54 + local_108 * 4), puVar21 == (uint *)0x0)) {
        puVar21 = (uint *)0x0;
      }
      pcVar2 = (cls_0x5a486c *)this->mbr_0x8;
      *puVar21 = *puVar21 | 0x1000000;
      if (pcVar2->mbr_0xc == 0) {
        if (*(int *)(pcVar2->mbr_0x4 + 0x60) == 0) {
          cls_0x5a486c::meth_0x447ac0(pcVar2);
        }
        cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)pcVar2);
      }
      uVar22 = puVar21[0xd2];
      puVar21[10] = 0x3fc90fdb;
      puVar21[0xb] = 0x3fc90fdb;
      puVar21[0xc] = 0x3fc90fdb;
      *puVar21 = *puVar21 | 0x6c;
      puVar21[0x10] = 0x3f800000;
      puVar21[0x11] = 0x3f800000;
      puVar21[0x12] = 0x3f800000;
      pcVar2 = (cls_0x5a486c *)this->mbr_0x8;
      puVar21[0xd2] = 0x10;
      puVar21[4] = 0;
      puVar21[5] = 0;
      puVar21[6] = 0;
      cls_0x5a486c::meth_0x40a8f0(pcVar2);
      local_108 = local_108 + 1;
      puVar21[0xd2] = uVar22;
      *puVar21 = *puVar21 & 0xfefeffff;
    }
    dVar19 = this->mbr_0x8;
    uVar22 = 0;
    if (0 < *(int *)(dVar19 + 0x34)) {
      iVar16 = 0;
      do {
        puVar14 = auStack_64;
        for (iVar17 = 0x17; iVar17 != 0; iVar17 = iVar17 + -1) {
          *puVar14 = 0;
          puVar14 = puVar14 + 1;
        }
        if (((-1 < iVar16) && ((int)uVar22 < (int)*(uint *)(dVar19 + 0x34))) &&
           ((iVar17 = *(int *)(dVar19 + 0x44), iVar17 != 0 &&
            (((uVar22 < *(uint *)(dVar19 + 0x34) && (*(int *)(iVar17 + uVar22 * 4) != 0)) &&
             (&stack0x00000000 != (undefined *)0x64)))))) {
          puVar14 = *(undefined4 **)(iVar17 + uVar22 * 4);
          if (puVar14 == (undefined4 *)0x0) {
            puVar14 = *(undefined4 **)(dVar19 + 0x48);
          }
          puVar15 = auStack_64;
          for (iVar17 = 0x17; iVar17 != 0; iVar17 = iVar17 + -1) {
            *puVar15 = *puVar14;
            puVar14 = puVar14 + 1;
            puVar15 = puVar15 + 1;
          }
        }
        auStack_64[1] = *(undefined4 *)(DAT_005e8518 + iVar16);
        auStack_64[2] = *(undefined4 *)(DAT_005e8518 + 4 + iVar16);
        auStack_64[3] = *(undefined4 *)(DAT_005e8518 + 8 + iVar16);
        uStack_30 = *(undefined4 *)(DAT_005e8518 + 0xc + iVar16);
        uStack_2c = *(undefined4 *)(DAT_005e8518 + 0x10 + iVar16);
        uStack_28 = *(undefined4 *)(DAT_005e8518 + 0x14 + iVar16);
        if (((-1 < iVar16) && ((int)uVar22 < (int)*(uint *)(dVar19 + 0x34))) &&
           ((iVar17 = *(int *)(dVar19 + 0x44), iVar17 != 0 &&
            ((uVar22 < *(uint *)(dVar19 + 0x34) && (*(int *)(iVar17 + uVar22 * 4) != 0)))))) {
          puVar14 = *(undefined4 **)(iVar17 + uVar22 * 4);
          if (puVar14 == (undefined4 *)0x0) {
            puVar14 = *(undefined4 **)(dVar19 + 0x48);
          }
          iVar17 = *(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4);
          puVar15 = auStack_64;
          for (iVar18 = 0x14; iVar18 != 0; iVar18 = iVar18 + -1) {
            *puVar14 = *puVar15;
            puVar15 = puVar15 + 1;
            puVar14 = puVar14 + 1;
          }
          if (iVar17 == 0) {
            iVar17 = *(int *)(dVar19 + 0x48);
          }
          iVar18 = *(int *)(dVar19 + 0x44);
          *(undefined4 *)(iVar17 + 0x48) = 0;
          iVar17 = *(int *)(iVar18 + uVar22 * 4);
          if (iVar17 == 0) {
            iVar17 = *(int *)(dVar19 + 0x48);
          }
          iVar18 = *(int *)(*(int *)(dVar19 + 0x44) + uVar22 * 4);
          if (iVar18 == 0) {
            iVar18 = *(int *)(dVar19 + 0x48);
          }
          (**(code **)(**(int **)(iVar18 + 0x50) + 0xc))(*(int **)(iVar18 + 0x50),iVar17);
        }
        uVar22 = uVar22 + 1;
        iVar16 = iVar16 + 0x18;
        dVar19 = this->mbr_0x8;
      } while ((int)uVar22 < *(int *)(dVar19 + 0x34));
    }
  }
  local_e0 = 2;
  FUN_00417030(&local_e0);
  uVar13 = __ftol();
  *(undefined4 *)&this->field_0x9c = uVar13;
  uVar13 = __ftol();
  *(undefined4 *)&this->field_0xa0 = uVar13;
  uVar13 = __ftol();
  *(undefined4 *)&this->field_0xa4 = uVar13;
  uVar13 = __ftol();
  *(undefined4 *)&this->field_0xa8 = uVar13;
  if ((*(byte *)&this->mbr_0x3c & 1) != 0) {
    FUN_0040ca20(&this->field_0x9c,1);
  }
  this->mbr_0xec = 1;
  if ((*(byte *)&this->mbr_0x3c & 2) != 0) {
    cls_0x5a486c::meth_0x40ca90((cls_0x5a486c *)this->mbr_0x8,this->mbr_0x4,this->mbr_0xc);
  }
  this->mbr_0xec = 1;
  return 1;
}



// Function at 00410df0

cls_0x5a393c * __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x410df0(cls_0x5a393c *this)

{
  byte in_stack_00000004;
  
  cls_0x410e10::~cls_0x410e10((cls_0x410e10 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00418830

byte __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x418830(cls_0x5a393c *this)

{
  ushort uVar1;
  undefined uVar2;
  byte bVar3;
  undefined3 extraout_var;
  int iVar4;
  int *in_stack_00000004;
  
  uVar1 = *(ushort *)(in_stack_00000004 + 3);
  uVar2 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
  if (CONCAT31(extraout_var,uVar2) <= (int)(uint)uVar1) {
    return 0;
  }
  iVar4 = (**(code **)(*in_stack_00000004 + 300))(0xffffffff);
  if (iVar4 == 0) {
    return 0;
  }
  if ((*(byte *)(iVar4 + 0x10) & 0x20) != 0) {
    return 0;
  }
  bVar3 = (*this->vftptr_0x0[3].virt_meth_0x4459e0_32)((cls_0x5a7e38 *)this);
  return bVar3 & 1;
}



// Function at 00418890

void __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x418890(cls_0x5a393c *this)

{
  undefined uVar1;
  int3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;
  
  iVar2 = 0;
  uVar1 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
  if (CONCAT31(extraout_var,uVar1) != 0 && -1 < extraout_var) {
    do {
      uVar1 = (*this->vftptr_0x0[3].virt_meth_0x41a020_0)(this);
      if (CONCAT31(extraout_var_00,uVar1) != 0) {
        FUN_004a21c0();
      }
      iVar2 = iVar2 + 1;
      uVar1 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
    } while (iVar2 < CONCAT31(extraout_var_01,uVar1));
  }
  return;
}



// Function at 00418c60

void __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x418c60(cls_0x5a393c *this)

{
  ushort uVar1;
  undefined uVar2;
  byte bVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar4;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  int iVar5;
  int unaff_EBX;
  int unaff_EBP;
  int unaff_ESI;
  uint unaff_EDI;
  uint uVar6;
  cls_0x46ec50 *unaff_retaddr;
  int *in_stack_00000004;
  uint uVar7;
  int iStack_c;
  cls_0x5a68d8 *pcStack_8;
  cls_0x5a68d8 *pcStack_4;
  
  uVar1 = *(ushort *)(in_stack_00000004 + 3);
  uVar2 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
  uVar7 = 0;
  uVar6 = (uint)uVar1;
  if (CONCAT31(extraout_var,uVar2) != 0 && -1 < extraout_var) {
    do {
      unaff_EDI = uVar7;
      bVar3 = (*this->vftptr_0x0[3].virt_meth_0x4459e0_32)((cls_0x5a7e38 *)this);
      uVar6 = unaff_EDI;
      if ((bVar3 & 4) != 0) break;
      uVar2 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
      uVar7 = unaff_EDI + 1;
      uVar6 = (uint)uVar1;
    } while ((int)(unaff_EDI + 1) < CONCAT31(extraout_var_00,uVar2));
  }
  uVar2 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
  if (((int)uVar6 < CONCAT31(extraout_var_01,uVar2)) &&
     (iVar4 = (**(code **)(*in_stack_00000004 + 300))(uVar6), iVar4 != 0)) {
    cls_0x46ec50::meth_0x46ec50(unaff_retaddr);
    if ((*(uint *)&unaff_retaddr->field_0x8 & 0x400) == 0) {
      (*this->vftptr_0x0[3].virt_meth_0x4459e0_32)((cls_0x5a7e38 *)this);
    }
    else if (DAT_005d79f8 != 0) {
      uVar7 = uVar6;
      uVar2 = (*this->vftptr_0x0[1].virt_meth_0x410af0_36)(this);
      iVar4 = unaff_EBX - CONCAT31(extraout_var_05,uVar2);
      uVar2 = (*this->vftptr_0x0[1].virt_meth_0x4459e0_32)((cls_0x5a7e38 *)this);
      cls_0x5a68d8::meth_0x4bd680
                (pcStack_4,unaff_ESI - CONCAT31(extraout_var_06,uVar2),uVar6,iVar4,uVar7);
      return;
    }
    uVar2 = (*this->vftptr_0x0[1].virt_meth_0x410b00_40)(this);
    iVar4 = iStack_c - CONCAT31(extraout_var_02,uVar2);
    uVar7 = uVar6;
    uVar2 = (*this->vftptr_0x0[1].virt_meth_0x410af0_36)(this);
    iVar5 = unaff_EBP - CONCAT31(extraout_var_03,uVar2);
    uVar2 = (*this->vftptr_0x0[1].virt_meth_0x4459e0_32)((cls_0x5a7e38 *)this);
    cls_0x5a68d8::meth_0x4bdc50
              (pcStack_8,unaff_EDI - CONCAT31(extraout_var_04,uVar2),(short)uVar6,iVar5,uVar7,iVar4)
    ;
  }
  return;
}



// Function at 00418fc0

undefined4 __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x418fc0(cls_0x5a393c *this)

{
  ushort uVar1;
  undefined uVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int *in_stack_00000004;
  
  uVar1 = *(ushort *)(in_stack_00000004 + 3);
  uVar2 = (*this->vftptr_0x0->virt_meth_0x419b80_60)(this);
  if ((int)(uint)uVar1 < CONCAT31(extraout_var,uVar2)) {
    iVar3 = (**(code **)(*in_stack_00000004 + 0x134))();
    if (iVar3 != 0) {
      return 1;
    }
    uVar2 = (*this->vftptr_0x0[2].virt_meth_0x410ab0_8)(this);
    if (1 < CONCAT31(extraout_var_00,uVar2)) {
      return 1;
    }
    if (*(int *)(this->mbr_0x4 + 0x60) != 0) {
      uVar2 = (*this->vftptr_0x0[3].virt_meth_0x410aa0_4)(this);
      if (CONCAT31(extraout_var_01,uVar2) != 0) {
        return 1;
      }
    }
  }
  return 0;
}



// Function at 00419040

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x419040(cls_0x5a393c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c82d;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x44);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a393c__vftable_5a393c_005a393c;
    this_00[1].mbr_0x4 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00419590

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x419590(cls_0x5a393c *this)

{
  short sVar1;
  dword dVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int *piStack_60;
  float fStack_54;
  float afStack_50 [2];
  float fStack_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  int local_38;
  int local_34;
  int local_30;
  int iStack_2c;
  int iStack_28;
  int aiStack_24 [2];
  int iStack_1c;
  int iStack_18;
  
  if ((this->mbr_0x38 == 0) || (*(int *)(this->mbr_0x38 + 0x144) == 0)) {
    iVar4 = cls_0x5a7b98_TCharacter::meth_0x46e8a0((cls_0x5a7b98_TCharacter *)this->mbr_0x4);
    if (iVar4 == 0) {
      return 0;
    }
    dVar2 = this->mbr_0x4;
    local_38 = *(int *)(dVar2 + 0x10);
    local_34 = *(int *)(dVar2 + 0x14);
    local_30 = *(int *)(dVar2 + 0x18);
    uVar7 = (uint)*(ushort *)(dVar2 + 0xc);
    iVar4 = *(int *)(*(int *)(iVar4 + 0x10) + uVar7 * 4);
    if (iVar4 == 0) {
      return 0;
    }
    if (*(short *)(dVar2 + 0x5c) < 1) {
      iVar6 = 0;
    }
    else {
      iVar6 = (int)*(short *)(dVar2 + 0x5c);
    }
    iVar5 = (**(code **)(*(int *)this->mbr_0x8 + 0x90))(uVar7);
    if (iVar5 < 2) {
      iVar5 = 1;
    }
    else {
      iVar5 = (**(code **)(*(int *)this->mbr_0x8 + 0x90))(uVar7);
    }
    if (iVar6 < iVar5 + -1) {
      sVar1 = *(short *)(this->mbr_0x4 + 0x5c);
      if (sVar1 < 1) {
        iVar6 = 0;
      }
      else {
        iVar6 = (int)sVar1;
      }
    }
    else {
      iVar6 = (**(code **)(*(int *)this->mbr_0x8 + 0x90))(uVar7);
      if (iVar6 < 2) {
        iVar6 = 1;
      }
      else {
        iVar6 = (**(code **)(*(int *)this->mbr_0x8 + 0x90))(uVar7);
      }
      iVar6 = iVar6 + -1;
    }
    iVar4 = *(int *)(*(int *)(iVar4 + 8) + iVar6 * 4);
    FUN_0046d7a0(&uStack_3c,&local_30,&iStack_2c);
    local_30 = local_30 - DAT_00666904;
    iStack_2c = iStack_2c - DAT_00666908;
    FUN_0046da70(&uStack_3c,&iStack_28);
    afStack_50[0] = 5.605194e-45;
    iVar6 = (**(code **)(*(int *)this->mbr_0x8 + 100))(uVar7);
    iStack_1c = (**(code **)(*(int *)this->mbr_0x8 + 0x68))(uVar7);
    iStack_18 = 0xf;
    iVar5 = (**(code **)(*(int *)this->mbr_0x4 + 0x134))();
    if (iVar5 == 0) {
      iVar6 = (**(code **)(*(int *)this->mbr_0x8 + 0x74))(uVar7);
      iVar6 = -iVar6;
      aiStack_24[0] = (**(code **)(*(int *)this->mbr_0x8 + 0x78))(uVar7);
      aiStack_24[0] = -aiStack_24[0];
      iStack_18 = (**(code **)(*(int *)this->mbr_0x8 + 0x7c))(uVar7);
    }
    else {
      fStack_48 = (float)DAT_006671a4;
      cls_0x411eb0::meth_0x415b40
                ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),uStack_44,uStack_40,uStack_3c,&iStack_2c,
                 &iStack_28);
      piStack_60 = &iStack_2c;
      iVar5 = 0;
      do {
        if ((DAT_005c61b4 <= iVar5) || (*piStack_60 < 0)) break;
        cls_0x411eb0::meth_0x4160c0
                  ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),*piStack_60,uStack_44,uStack_40,
                   uStack_3c,&fStack_54,afStack_50);
        iVar5 = iVar5 + 1;
        piStack_60 = piStack_60 + 1;
      } while (iVar5 < 2);
      __ftol();
      __ftol();
      __ftol();
    }
    cVar3 = (char)iVar6;
    if (this->mbr_0x38 == 0) {
      cls_0x411eb0::meth_0x414d70
                ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)local_38 - cVar3,
                 (char)local_34 - (char)iStack_1c,local_30 - iStack_18,iVar4,0);
    }
    else {
      iVar5 = 4;
      do {
        dVar2 = this->mbr_0x4;
        uStack_44 = *(undefined4 *)(dVar2 + 0x10);
        uStack_40 = *(undefined4 *)(dVar2 + 0x14);
        uStack_3c = *(undefined4 *)(dVar2 + 0x18);
        if (this->mbr_0x38 != 0) {
          uStack_44 = __ftol();
          fStack_48 = (float)(this->mbr_0x38 + iVar5 * 0x18);
          uStack_40 = __ftol();
          uStack_3c = __ftol();
        }
        FUN_0046d7a0(&uStack_44,&iStack_2c,&iStack_28);
        iStack_2c = iStack_2c - DAT_00666904;
        iStack_28 = iStack_28 - DAT_00666908;
        FUN_0046da70(&uStack_44,aiStack_24);
        if (iVar5 == 0) {
          cls_0x411eb0::meth_0x414d70
                    ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)iStack_2c - cVar3,
                     (char)iStack_28 - (char)iStack_1c,aiStack_24[0] - iStack_18,iVar4,0);
        }
        else {
          __ftol(0,0,*(undefined4 *)(iVar4 + 4),*(undefined4 *)(iVar4 + 8),0,0x90);
          cls_0x411eb0::meth_0x414d70
                    ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)iStack_2c - cVar3,
                     (char)iStack_28 - (char)iStack_1c,aiStack_24[0] - iStack_18,iVar4,0);
        }
        iVar5 = iVar5 + -1;
      } while (-1 < iVar5);
    }
    if ((('\0' < (char)this->mbr_0x34) && (this->mbr_0x38 == 0)) && (DAT_00668154 == 0)) {
      fStack_48 = (float)(int)(char)this->mbr_0x34;
      __ftol(0,0,*(undefined4 *)(iVar4 + 4),*(undefined4 *)(iVar4 + 8),0,0x90);
      cls_0x411eb0::meth_0x414d70
                ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)local_38 - cVar3,
                 (char)local_34 - (char)iStack_1c,local_30 - iStack_18,iVar4,0);
    }
    FUN_004aacb0(local_38 - iVar6,local_34 - iStack_1c,*(int *)(iVar4 + 4) + 1,
                 *(int *)(iVar4 + 8) + 1,1);
  }
  return 1;
}



// Function at 00419d80

void __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x419d80(cls_0x5a393c *this)

{
  cls_0x5a35ac *this_00;
  dword dVar1;
  
  if (this->mbr_0x40 != 0) {
    this_00 = (cls_0x5a35ac *)this->mbr_0x8;
    if ((this_00 != (cls_0x5a35ac *)0x0) &&
       (dVar1 = (this_00->cls_0x5a486c).mbr_0xc - 1, (this_00->cls_0x5a486c).mbr_0xc = dVar1,
       (int)dVar1 < 1)) {
      (this_00->cls_0x5a486c).mbr_0xc = 0;
      cls_0x5a35ac::meth_0x4191b0(this_00);
    }
    cls_0x411eb0::meth_0x4162c0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
    cls_0x5a7e38::meth_0x4459c0((cls_0x5a7e38 *)this);
    this->mbr_0x40 = 0;
  }
  return;
}



// Function at 00419fb0

undefined4 __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x419fb0(cls_0x5a393c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar2;
  
  uVar1 = (*this->vftptr_0x0[3].virt_meth_0x410aa0_4)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[3].virt_meth_0x410aa0_4)(this);
    uVar2 = cls_0x4186c0::meth_0x4186c0((cls_0x4186c0 *)CONCAT31(extraout_var_00,uVar1));
    return uVar2;
  }
  return 0;
}



// Function at 00419ff0

cls_0x5a393c * __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x419ff0(cls_0x5a393c *this)

{
  byte in_stack_00000004;
  
  cls_0x418780::~cls_0x418780((cls_0x418780 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0041a020

cls_0x5a393c * __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x41a020(cls_0x5a393c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a393c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0041a040

cls_0x5a393c * __thiscall OOAnalyzer::cls_0x5a393c::~cls_0x5a393c(cls_0x5a393c *this)

{
  cls_0x5a35ac *this_00;
  cls_0x5a393c *pcVar1;
  dword dVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c878;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a393c__vftable_5a393c_005a393c;
  local_4 = 0;
  if (this->mbr_0x40 != 0) {
    this_00 = (cls_0x5a35ac *)this->mbr_0x8;
    if ((this_00 != (cls_0x5a35ac *)0x0) &&
       (dVar2 = (this_00->cls_0x5a486c).mbr_0xc - 1, (this_00->cls_0x5a486c).mbr_0xc = dVar2,
       (int)dVar2 < 1)) {
      (this_00->cls_0x5a486c).mbr_0xc = 0;
      cls_0x5a35ac::meth_0x4191b0(this_00);
    }
    cls_0x411eb0::meth_0x4162c0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
    cls_0x5a7e38::meth_0x4459c0((cls_0x5a7e38 *)this);
    this->mbr_0x40 = 0;
  }
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a393c *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00445b20

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x5a393c::virt_meth_0x445b20(cls_0x5a393c *this)

{
  double dVar1;
  double *pdVar2;
  int *piVar3;
  char cVar4;
  double *pdVar5;
  int iVar6;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  if (this->mbr_0x35 == 0xff) {
    if ((char)this->mbr_0x34 < '\x01') {
      this->mbr_0x34 = 0;
      this->mbr_0x35 = 0;
    }
    else {
      this->mbr_0x34 = this->mbr_0x34 - 1;
    }
  }
  if (this->mbr_0x35 == 1) {
    if ((char)this->mbr_0x34 < '\x06') {
      this->mbr_0x34 = this->mbr_0x34 + 1;
    }
    else {
      this->mbr_0x35 = 0xff;
    }
  }
  pdVar2 = (double *)this->mbr_0x38;
  if (pdVar2 != (double *)0x0) {
    if (*(int *)(pdVar2 + 0x28) < 0) {
      pdVar5 = pdVar2 + 0x12;
      iVar6 = 4;
      do {
        *(undefined4 *)pdVar5 = *(undefined4 *)(pdVar5 + -3);
        *(undefined4 *)((int)pdVar5 + 4) = *(undefined4 *)((int)pdVar5 + -0x14);
        *(undefined4 *)(pdVar5 + 1) = *(undefined4 *)(pdVar5 + -2);
        *(undefined4 *)((int)pdVar5 + 0xc) = *(undefined4 *)((int)pdVar5 + -0xc);
        *(undefined4 *)(pdVar5 + 2) = *(undefined4 *)(pdVar5 + -1);
        *(undefined4 *)((int)pdVar5 + 0x14) = *(undefined4 *)((int)pdVar5 + -4);
        *(undefined4 *)(pdVar5 + 0xf) = *(undefined4 *)(pdVar5 + 0xc);
        *(undefined4 *)((int)pdVar5 + 0x7c) = *(undefined4 *)((int)pdVar5 + 100);
        *(undefined4 *)(pdVar5 + 0x10) = *(undefined4 *)(pdVar5 + 0xd);
        *(undefined4 *)((int)pdVar5 + 0x84) = *(undefined4 *)((int)pdVar5 + 0x6c);
        *(undefined4 *)(pdVar5 + 0x11) = *(undefined4 *)(pdVar5 + 0xe);
        *(undefined4 *)((int)pdVar5 + 0x8c) = *(undefined4 *)((int)pdVar5 + 0x74);
        pdVar5 = pdVar5 + -3;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
      if ((double)*(int *)((int)pdVar2 + 0x134) < pdVar2[8]) {
        iVar6 = 0;
        pdVar2[6] = pdVar2[6] + *pdVar2;
        pdVar2[7] = pdVar2[1] + pdVar2[7];
        pdVar2[8] = pdVar2[8] + pdVar2[2];
        pdVar2[0x15] = pdVar2[3] + pdVar2[0x15];
        pdVar2[0x16] = pdVar2[4] + pdVar2[0x16];
        pdVar2[0x17] = pdVar2[5] + pdVar2[0x17];
        dVar1 = pdVar2[0x15];
        if ((double)_DAT_005a36c0 < dVar1) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 - (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while ((double)_DAT_005a36c0 < dVar1);
        }
        iVar6 = 0;
        if (dVar1 < (double)_DAT_005a3530) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 + (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while (dVar1 < (double)_DAT_005a3530);
        }
        pdVar2[0x15] = dVar1;
        dVar1 = pdVar2[0x16];
        iVar6 = 0;
        if ((double)_DAT_005a36c0 < dVar1) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 - (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while ((double)_DAT_005a36c0 < dVar1);
        }
        iVar6 = 0;
        if (dVar1 < (double)_DAT_005a3530) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 + (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while (dVar1 < (double)_DAT_005a3530);
        }
        pdVar2[0x16] = dVar1;
        dVar1 = pdVar2[0x17];
        iVar6 = 0;
        if ((double)_DAT_005a36c0 < dVar1) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 - (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while ((double)_DAT_005a36c0 < dVar1);
        }
        iVar6 = 0;
        if (dVar1 < (double)_DAT_005a3530) {
          do {
            if (9 < iVar6) break;
            dVar1 = dVar1 + (double)_DAT_005a36c0;
            iVar6 = iVar6 + 1;
          } while (dVar1 < (double)_DAT_005a3530);
        }
        pdVar2[0x17] = dVar1;
        pdVar2[2] = pdVar2[2] - pdVar2[0x27];
      }
      if (pdVar2[8] < (double)*(int *)((int)pdVar2 + 0x134)) {
        pdVar2[8] = (double)((float)*(int *)((int)pdVar2 + 0x134) + _DAT_005a4848);
        pdVar2[2] = pdVar2[2] * _DAT_005a4840;
        pdVar2[5] = pdVar2[5] * _DAT_005a3a00;
        if ((pdVar2[6] < (double)(*(int *)((int)pdVar2 + 300) + 0x10)) &&
           ((double)(*(int *)((int)pdVar2 + 300) + -0x10) < pdVar2[6])) {
          if ((pdVar2[7] < (double)(*(int *)(pdVar2 + 0x26) + 0x10)) &&
             (((double)(*(int *)(pdVar2 + 0x26) + -0x10) < pdVar2[7] &&
              (pdVar2[2] <= pdVar2[0x27] * _DAT_005a3f70)))) {
            *(undefined4 *)pdVar2 = 0;
            *(undefined4 *)((int)pdVar2 + 4) = 0;
            *(undefined4 *)(pdVar2 + 1) = 0;
            *(undefined4 *)((int)pdVar2 + 0xc) = 0;
            *(undefined4 *)(pdVar2 + 2) = 0;
            *(undefined4 *)((int)pdVar2 + 0x14) = 0;
            *(undefined4 *)(pdVar2 + 3) = 0;
            *(undefined4 *)((int)pdVar2 + 0x1c) = 0;
            *(undefined4 *)(pdVar2 + 4) = 0;
            *(undefined4 *)((int)pdVar2 + 0x24) = 0;
            *(undefined4 *)(pdVar2 + 5) = 0;
            *(undefined4 *)((int)pdVar2 + 0x2c) = 0;
            if (*(int *)(pdVar2 + 0x28) == -1) {
              *(undefined4 *)(pdVar2 + 0x28) = 5;
            }
          }
        }
      }
    }
    if ((0 < *(int *)(pdVar2 + 0x28)) &&
       (iVar6 = *(int *)(pdVar2 + 0x28) + -1, *(int *)(pdVar2 + 0x28) = iVar6, iVar6 == 0)) {
      cVar4 = __ftol();
      piVar3 = (int *)this->mbr_0x4;
      *(byte *)((int)piVar3 + 0x36) = -cVar4 - 0x40U;
      piVar3[0x2c] = (uint)(byte)(-cVar4 - 0x40U);
      local_14 = __ftol();
      local_10 = __ftol();
      local_c = __ftol();
      (**(code **)(*piVar3 + 8))(&local_14,0xffffffff,0);
      FUN_004830f0(this->mbr_0x38);
      this->mbr_0x34 = 0;
      this->mbr_0x35 = 1;
      this->mbr_0x38 = 0;
    }
  }
  return;
}



