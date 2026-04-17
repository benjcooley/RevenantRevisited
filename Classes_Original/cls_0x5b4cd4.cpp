// Decompiled methods and structure for class: cls_0x5b4cd4

/*
/OOAnalyzer/cls_0x5b4cd4
pack(disabled)
Structure cls_0x5b4cd4 {
   0   cls_0x5b4cd4::vftable_5b4cd4 *   4   vftptr_0x0   "pointer to cls_0x5b4cd4::vftable_5b4cd4"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   220   dword   4   mbr_0xdc   "Unsigned Double-Word (ddw, 4-bytes)"
   224   dword   4   mbr_0xe0   "Unsigned Double-Word (ddw, 4-bytes)"
   228   dword   4   mbr_0xe4   "Unsigned Double-Word (ddw, 4-bytes)"
   232   dword   4   mbr_0xe8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 252 Alignment: 1

*/

// Function at 0040e8d0

undefined4 __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x40e8d0(cls_0x5b4cd4 *this)

{
  uint uVar1;
  cls_0x5a486c *this_00;
  dword dVar2;
  uint *puVar3;
  uint **ppuVar4;
  uint uVar5;
  int iVar6;
  dword dVar7;
  uint uVar8;
  uint uStack_118;
  int iStack_114;
  int iStack_110;
  uint *apuStack_100 [64];
  
  iVar6 = 0;
  if (0 < (int)this->mbr_0x58) {
    do {
      (**(code **)(**(int **)(this->mbr_0x68 + iVar6 * 4) + 0x18))();
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x58);
  }
  uVar8 = this->mbr_0x44;
  if (0 < (int)uVar8) {
    ppuVar4 = (uint **)this->mbr_0x54;
    uStack_118 = uVar8;
    do {
      puVar3 = *ppuVar4;
      if ((*puVar3 & 0x800000) == 0) {
        iVar6 = *(int *)(*(int *)(this->mbr_0x8 + 0x74) + puVar3[1] * 4);
        if (iVar6 == 0) {
          iVar6 = *(int *)(this->mbr_0x8 + 0x78);
        }
        uVar5 = *(uint *)(*(int *)(iVar6 + 0x20) + this->mbr_0xc * 4);
        if ((uVar5 < uVar8) &&
           (uVar1 = *(uint *)(this->mbr_0x54 + uVar5 * 4), *(uint *)(uVar1 + 4) == uVar5)) {
          puVar3[2] = uVar1;
        }
        else {
          puVar3[2] = 0;
        }
      }
      ppuVar4 = ppuVar4 + 1;
      uStack_118 = uStack_118 - 1;
    } while (uStack_118 != 0);
  }
  uStack_118 = 0;
  if (0 < *(int *)(this->mbr_0x8 + 0x4c) + 1) {
    do {
      if (0 < (int)uVar8) {
        ppuVar4 = (uint **)this->mbr_0x54;
        uVar5 = uVar8;
        do {
          puVar3 = *ppuVar4;
          ppuVar4 = ppuVar4 + 1;
          uVar5 = uVar5 - 1;
          *puVar3 = *puVar3 & 0x7fffffff;
        } while (uVar5 != 0);
      }
      iStack_110 = 0;
      if (0 < (int)uVar8) {
        do {
          apuStack_100[0] = *(uint **)(this->mbr_0x54 + iStack_110 * 4);
          if ((apuStack_100[0] != (uint *)0x0) && ((*apuStack_100[0] & 0x80000000) == 0)) {
            puVar3 = (uint *)apuStack_100[0][2];
            iStack_114 = 1;
            if (puVar3 != (uint *)0x0) {
              ppuVar4 = apuStack_100;
              do {
                ppuVar4 = ppuVar4 + 1;
                if (((*puVar3 & 0x80000000) != 0) || (0x3f < iStack_114)) break;
                *ppuVar4 = puVar3;
                puVar3 = (uint *)puVar3[2];
                iStack_114 = iStack_114 + 1;
              } while (puVar3 != (uint *)0x0);
              if (iStack_114 == 0) goto LAB_0040eac9;
            }
            ppuVar4 = apuStack_100 + iStack_114;
            do {
              puVar3 = ppuVar4[-1];
              this_00 = (cls_0x5a486c *)this->mbr_0x8;
              ppuVar4 = ppuVar4 + -1;
              uVar8 = puVar3[1];
              dVar2 = this->mbr_0xc;
              if (this_00->mbr_0xc == 0) {
                if (*(int *)(this_00->mbr_0x4 + 0x60) == 0) {
                  cls_0x5a486c::meth_0x447ac0(this_00);
                }
                cls_0x5a35ac::meth_0x407510((cls_0x5a35ac *)this_00);
              }
              if (((((int)uVar8 < 0) || ((int)this_00->mbr_0x64 < (int)uVar8)) ||
                  (this_00->mbr_0x74 == 0)) ||
                 ((this_00->mbr_0x64 <= uVar8 || (*(int *)(this_00->mbr_0x74 + uVar8 * 4) == 0)))) {
LAB_0040ea80:
                *puVar3 = *puVar3 | 1;
              }
              else {
                dVar7 = *(dword *)(this_00->mbr_0x74 + uVar8 * 4);
                if (dVar7 == 0) {
                  dVar7 = this_00->mbr_0x78;
                }
                if (*(int *)(*(int *)(dVar7 + 0x44) + dVar2 * 4) == 0) goto LAB_0040ea80;
              }
              cls_0x5a486c::meth_0x40a8f0((cls_0x5a486c *)this->mbr_0x8);
              iStack_114 = iStack_114 + -1;
              *puVar3 = *puVar3 | 0x80000000;
            } while (iStack_114 != 0);
          }
LAB_0040eac9:
          uVar8 = this->mbr_0x44;
          iStack_110 = iStack_110 + 1;
        } while (iStack_110 < (int)uVar8);
      }
      uStack_118 = uStack_118 + 1;
    } while ((int)uStack_118 < *(int *)(this->mbr_0x8 + 0x4c) + 1);
  }
  return 1;
}



// Function at 00410ca0

cls_0x5b4cd4 * __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x410ca0(cls_0x5b4cd4 *this)

{
  byte in_stack_00000004;
  
  cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00516700

void __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x516700(cls_0x5b4cd4 *this)

{
  int iVar1;
  
  iVar1 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x11);
  if (iVar1 != 0) {
    FUN_004a90d0(iVar1,s_d__revenant_Money_cpp_005e1de4,0x18d);
  }
  iVar1 = cls_0x411eb0::meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x12);
  if (iVar1 != 0) {
    FUN_004a90d0(iVar1,s_d__revenant_Money_cpp_005e1dfc,0x18e);
  }
  virt_meth_0x40e8d0(this);
  return;
}



// Function at 00516800

cls_0x5b4cd4 * __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x516800(cls_0x5b4cd4 *this)

{
  byte in_stack_00000004;
  
  cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00516b40

cls_0x5b4cd4 * __thiscall OOAnalyzer::cls_0x5b4cd4::~cls_0x5b4cd4(cls_0x5b4cd4 *this)

{
  cls_0x5a50e8 *pcVar1;
  int iVar2;
  dword *pdVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1048;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b4cd4__vftable_5b4cd4_005b4cd4;
  local_4 = 0;
  pdVar3 = &this->mbr_0xd8;
  iVar2 = 5;
  do {
    if ((undefined4 *)*pdVar3 != (undefined4 *)0x0) {
      if (this->mbr_0xf8 != 0) {
        FUN_004754e0(*(undefined4 *)*pdVar3);
      }
      FUN_00482f80(*(undefined4 *)*pdVar3);
      FUN_00482f80(*pdVar3);
      *pdVar3 = 0;
    }
    pdVar3 = pdVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  ExceptionList = local_c;
  return (cls_0x5b4cd4 *)pcVar1;
}



// Function at 00517240

void __thiscall OOAnalyzer::cls_0x5b4cd4::meth_0x517240(cls_0x5b4cd4 *this)

{
  dword dVar1;
  dword dVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  cls_0x5a7b98 *this_00;
  int *piVar5;
  int iVar6;
  int iVar7;
  cls_0x4746d0 *this_01;
  undefined4 *puVar8;
  int in_stack_00000004;
  int local_58;
  int local_54;
  undefined local_50 [4];
  int local_4c;
  int local_48;
  int local_44;
  dword local_40;
  dword local_3c;
  undefined2 local_34;
  undefined2 local_32;
  undefined4 local_30;
  undefined2 local_2a;
  int local_28;
  int local_24;
  int local_20;
  
  if ((DAT_00668154 == 0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
    local_40 = this->mbr_0x10;
    local_3c = this->mbr_0x14;
    dVar1 = this->mbr_0x18;
    iVar7 = 0x14;
    do {
      dVar2 = FUN_00483300(0,0xff);
      uVar3 = FUN_00483300(0x32,0x96,&local_4c,0);
      FUN_0046db20(dVar2,uVar3);
      local_48 = local_48 + local_3c;
      local_4c = local_4c + local_40;
      iVar7 = iVar7 + -1;
      local_44 = dVar1 + 10;
      FUN_004530a0(&local_4c,*(undefined2 *)((int)&this->mbr_0xc + 2),0x10,&local_54,local_50,
                   &local_58);
      if (local_54 < 5) {
        if (local_58 == 0) goto LAB_00517328;
      }
      else {
        local_58 = 1;
      }
    } while (iVar7 != 0);
    if (local_58 == 0) {
LAB_00517328:
      this_01 = (cls_0x4746d0 *)(-(uint)(0xc < DAT_0065a258) & DAT_0065a178);
      iVar7 = cls_0x4746d0::meth_0x475210
                        (this_01,*(undefined4 *)(&this->mbr_0xd8)[in_stack_00000004]);
      if (-1 < iVar7) {
        uVar3 = 0;
        puVar8 = (undefined4 *)&local_34;
        for (iVar6 = 0xd; iVar6 != 0; iVar6 = iVar6 + -1) {
          *puVar8 = 0;
          puVar8 = puVar8 + 1;
        }
        local_34 = *(undefined2 *)&this_01->mbr_0x8;
        local_32 = (undefined2)iVar7;
        local_2a = DAT_00666970;
        local_28 = local_4c;
        local_20 = local_44;
        local_30 = 0;
        local_24 = local_48;
        uVar4 = cls_0x5a5320::meth_0x450e40((cls_0x5a5320 *)&DAT_006668d8,&local_34);
        this_00 = (cls_0x5a7b98 *)FUN_00452690(uVar4,uVar3);
        if (this_00 != (cls_0x5a7b98 *)0x0) {
          piVar5 = (int *)cls_0x5a7b98::meth_0x46e8a0(this_00);
          (**(code **)(*piVar5 + 0xc))();
          cls_0x5877a0::meth_0x587880((cls_0x5877a0 *)&UNK_00676e04.field_0x4);
          FUN_005169b0(this_00,*(undefined4 *)((&this->mbr_0xd8)[in_stack_00000004] + 8));
          (this_00->cls_0x5a50e8).mbr_0x36 = (byte)dVar2;
          (this_00->cls_0x5a50e8).mbr_0xb0 = dVar2;
        }
      }
    }
  }
  return;
}



// Function at 00517530

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x517530(cls_0x5b4cd4 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a107e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b4ecc__vftable_5b4ecc_005b4ecc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00517640

void __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x517640(cls_0x5b4cd4 *this)

{
  cls_0x5b8e94::virt_meth_0x470f00((cls_0x5b8e94 *)this);
  this->mbr_0xf0 = 1;
  return;
}



// Function at 00517660

void __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x517660(cls_0x5b4cd4 *this)

{
  cls_0x5b8e94::virt_meth_0x470e40((cls_0x5b8e94 *)this);
  this->mbr_0xf0 = 0;
  return;
}



// Function at 00517680

cls_0x5b4cd4 * __thiscall OOAnalyzer::cls_0x5b4cd4::virt_meth_0x517680(cls_0x5b4cd4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b4cd4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005176a0

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b4cd4::virt_meth_0x5176a0(cls_0x5b4cd4 *this,undefined4 param_1)

{
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a10bb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&cls_0x5b4cd4__vftable_5b4cd4_005b4cd4;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8101;
    this_00->mbr_0xd8 = 0;
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    *(undefined4 *)&this_00->field_0xf0 = 1;
    *(undefined4 *)&this_00->field_0xec = 2;
    *(undefined4 *)&this_00->field_0xf4 = 0;
    this_00->mbr_0xf8 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



