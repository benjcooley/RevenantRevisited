// Decompiled methods and structure for class: cls_0x5a3e7c

/*
/OOAnalyzer/cls_0x5a3e7c
pack(disabled)
Structure cls_0x5a3e7c {
   0   cls_0x5a68d8   144   cls_0x5a68d8   "Component (member) class."
}
Length: 144 Alignment: 1

*/

// Function at 004384e0

uint __thiscall OOAnalyzer::cls_0x5a3e7c::meth_0x4384e0(cls_0x5a3e7c *this)

{
  uint uVar1;
  uint in_stack_00000004;
  
  uVar1 = (this->cls_0x5a68d8).mbr_0x38;
  if ((uVar1 & 2) == 0) {
    if ((uVar1 & 4) != 0) {
      return ((in_stack_00000004 >> 0x10 & 0xf8) << 5 | in_stack_00000004 >> 8 & 0xfc) << 3 |
             in_stack_00000004 >> 3 & 0x1f;
    }
    if ((uVar1 & 0x10000) != 0) {
      return ((in_stack_00000004 >> 0x18 & 0xf0) << 4 | in_stack_00000004 >> 0x10 & 0xf0) << 4 |
             in_stack_00000004 >> 8 & 0xf0 | in_stack_00000004 >> 4 & 0xf;
    }
    if ((uVar1 & 0x20000) == 0) {
      return 0;
    }
    uVar1 = (in_stack_00000004 >> 0x18 & 0xff80) << 1 | in_stack_00000004 >> 0x10 & 0xf8;
  }
  else {
    uVar1 = in_stack_00000004 >> 0x10 & 0xf8;
  }
  return (uVar1 << 5 | in_stack_00000004 >> 8 & 0xf8) << 2 | in_stack_00000004 >> 3 & 0x1f;
}



// Function at 00438c70

void __thiscall OOAnalyzer::cls_0x5a3e7c::virt_meth_0x438c70(cls_0x5a3e7c *this)

{
  dword dVar1;
  
  dVar1 = (**(code **)(**(int **)(this->cls_0x5a68d8).mbr_0x70 + 0x2c))();
  (this->cls_0x5a68d8).mbr_0x34 = dVar1;
  return;
}



// Function at 00438cc0

cls_0x5a3e7c * __thiscall OOAnalyzer::cls_0x5a3e7c::virt_meth_0x438cc0(cls_0x5a3e7c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3e7c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004bacc0

cls_0x5a3e7c * __thiscall OOAnalyzer::cls_0x5a3e7c::cls_0x5a3e7c(cls_0x5a3e7c *this)

{
  cls_0x5a68d8::cls_0x5a68d8(&this->cls_0x5a68d8);
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
  (this->cls_0x5a68d8).mbr_0x64 = 0;
  (this->cls_0x5a68d8).mbr_0x68 = 0;
  (this->cls_0x5a68d8).mbr_0x6c = 0;
  (this->cls_0x5a68d8).mbr_0x70 = 0;
  return this;
}



// Function at 004bace0

cls_0x5a3e7c * __thiscall OOAnalyzer::cls_0x5a3e7c::~cls_0x5a3e7c(cls_0x5a3e7c *this)

{
  cls_0x5a3e7c *pcVar1;
  int iVar2;
  int *piVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dff8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3e7c__vftable_5a3e7c_005a3e7c;
  piVar3 = (int *)(this->cls_0x5a68d8).mbr_0x70;
  local_4 = 0;
  if (piVar3 != (int *)0x0) {
    iVar2 = 0;
    if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
      do {
        if ((undefined4 *)*piVar3 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)*piVar3)(1);
        }
        iVar2 = iVar2 + 1;
        piVar3 = piVar3 + 1;
      } while (iVar2 < (int)(this->cls_0x5a68d8).mbr_0x68);
    }
    FUN_004830f0((LPCVOID)(this->cls_0x5a68d8).mbr_0x6c);
    (this->cls_0x5a68d8).mbr_0x6c = 0;
    FUN_004830f0((LPCVOID)(this->cls_0x5a68d8).mbr_0x70);
    (this->cls_0x5a68d8).mbr_0x70 = 0;
  }
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a3e7c *)cls_0x5a6858::meth_0x4bcb50((cls_0x5a6858 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004bbc70

void __thiscall OOAnalyzer::cls_0x5a3e7c::virt_meth_0x4bbc70(cls_0x5a3e7c *this,dword param_1)

{
  int *piVar1;
  int **ppiVar2;
  int iVar3;
  dword in_stack_00000008;
  int local_4;
  
  ppiVar2 = (int **)(this->cls_0x5a68d8).mbr_0x70;
  (this->cls_0x5a68d8).mbr_0x14 = param_1;
  iVar3 = 0;
  (this->cls_0x5a68d8).mbr_0x18 = in_stack_00000008;
  if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
    local_4 = 0;
    do {
      piVar1 = (int *)(local_4 + (this->cls_0x5a68d8).mbr_0x6c);
      (**(code **)(**ppiVar2 + 0x40))(param_1 - *piVar1,in_stack_00000008 - piVar1[1]);
      iVar3 = iVar3 + 1;
      local_4 = local_4 + 0x10;
      ppiVar2 = ppiVar2 + 1;
    } while (iVar3 < (int)(this->cls_0x5a68d8).mbr_0x68);
  }
  return;
}



// Function at 004bbcd0

void __thiscall
OOAnalyzer::cls_0x5a3e7c::virt_meth_0x4bbcd0
          (cls_0x5a3e7c *this,dword param_1,dword param_2,dword param_3)

{
  int **ppiVar1;
  int iVar2;
  dword in_stack_00000010;
  
  ppiVar1 = (int **)(this->cls_0x5a68d8).mbr_0x70;
  (this->cls_0x5a68d8).mbr_0x20 = param_1;
  (this->cls_0x5a68d8).mbr_0x2c = in_stack_00000010;
  iVar2 = 0;
  (this->cls_0x5a68d8).mbr_0x24 = param_2;
  (this->cls_0x5a68d8).mbr_0x28 = param_3;
  if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
    do {
      (**(code **)(**ppiVar1 + 0x44))(0,0,(this->cls_0x5a68d8).mbr_0x4,(this->cls_0x5a68d8).mbr_0x8)
      ;
      iVar2 = iVar2 + 1;
      ppiVar1 = ppiVar1 + 1;
    } while (iVar2 < (int)(this->cls_0x5a68d8).mbr_0x68);
  }
  return;
}



// Function at 004bbd30

void __thiscall OOAnalyzer::cls_0x5a3e7c::virt_meth_0x4bbd30(cls_0x5a3e7c *this)

{
  int iVar1;
  int **ppiVar2;
  dword in_stack_00000004;
  
  iVar1 = 0;
  ppiVar2 = (int **)(this->cls_0x5a68d8).mbr_0x70;
  (this->cls_0x5a68d8).mbr_0x1c = in_stack_00000004;
  if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
    do {
      (**(code **)(**ppiVar2 + 0x48))(0);
      iVar1 = iVar1 + 1;
      ppiVar2 = ppiVar2 + 1;
    } while (iVar1 < (int)(this->cls_0x5a68d8).mbr_0x68);
  }
  return;
}



// Function at 004bbd70

void __thiscall OOAnalyzer::cls_0x5a3e7c::virt_meth_0x4bbd70(cls_0x5a3e7c *this)

{
  int iVar1;
  int **ppiVar2;
  
  iVar1 = 0;
  ppiVar2 = (int **)(this->cls_0x5a68d8).mbr_0x70;
  if (0 < (int)(this->cls_0x5a68d8).mbr_0x68) {
    do {
      (**(code **)(**ppiVar2 + 0x1c))();
      iVar1 = iVar1 + 1;
      ppiVar2 = ppiVar2 + 1;
    } while (iVar1 < (int)(this->cls_0x5a68d8).mbr_0x68);
  }
  return;
}



// Function at 004bbed0

uint __thiscall
OOAnalyzer::cls_0x5a3e7c::virt_meth_0x4bbed0
          (cls_0x5a3e7c *this,uint **param_1,int *param_2,undefined4 param_3)

{
  uint *puVar1;
  dword dVar2;
  uint uVar3;
  int iVar4;
  int *unaff_EDI;
  uint **ppuVar5;
  undefined4 *puVar6;
  int **ppiVar7;
  undefined4 in_stack_00000010;
  uint local_21c;
  int iStack_218;
  int iStack_214;
  int *piStack_210;
  int *piStack_20c;
  int *local_208;
  int iStack_204;
  int iStack_200;
  undefined4 auStack_1fc [7];
  dword dStack_1e0;
  dword dStack_1dc;
  dword dStack_1d8;
  int iStack_1c4;
  int iStack_1c0;
  int iStack_1bc;
  uint *local_1a4 [21];
  uint *apuStack_150 [15];
  int aiStack_114 [69];
  
  ppuVar5 = local_1a4;
  for (iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 + -1) {
    *ppuVar5 = *param_1;
    param_1 = param_1 + 1;
    ppuVar5 = ppuVar5 + 1;
  }
  local_21c = 0;
  local_208 = (int *)0x0;
  if (((param_2 != (int *)0x0) && (iVar4 = (**(code **)(*param_2 + 8))(), iVar4 == 0x10)) &&
     (local_208 = param_2, param_2[0x1a] != (this->cls_0x5a68d8).mbr_0x68)) {
    FUN_00481c10((byte *)s_Attempt_to_blit_between_non_iden_005def24);
  }
  iVar4 = meth_0x4bd200(this,local_1a4);
  if (iVar4 == 0) {
    return 0;
  }
  dVar2 = (this->cls_0x5a68d8).mbr_0x4;
  puVar6 = auStack_1fc;
  for (iVar4 = 0x16; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  dStack_1dc = (this->cls_0x5a68d8).mbr_0x8;
  dStack_1d8 = (this->cls_0x5a68d8).mbr_0x10;
  if (param_2 == (int *)0x0) {
    iStack_1bc = 0;
    iStack_1c0 = 0;
    iStack_1c4 = 0;
  }
  else {
    iStack_1c4 = param_2[1];
    iStack_1c0 = param_2[2];
    iStack_1bc = param_2[4];
  }
  dStack_1e0 = dVar2;
  iVar4 = FUN_004ad7c0(auStack_1fc,local_1a4,apuStack_150,&iStack_218,unaff_EDI);
  if (iVar4 != 0) {
    iStack_204 = param_2[5];
    iStack_200 = param_2[6];
    piStack_20c = param_2;
    iStack_214 = 0;
    if (0 < iStack_218) {
      piStack_210 = aiStack_114;
      do {
        iVar4 = 0;
        puVar1 = (uint *)(piStack_210 + -0xf);
        piStack_210[-1] = piStack_210[-1] - iStack_204;
        dVar2 = (this->cls_0x5a68d8).mbr_0x68;
        *piStack_210 = *piStack_210 - iStack_200;
        ppiVar7 = (int **)(this->cls_0x5a68d8).mbr_0x70;
        *puVar1 = *puVar1 & 0xfffffff9;
        if (0 < (int)dVar2) {
          do {
            if (local_208 != (int *)0x0) {
              piStack_20c = *(int **)(local_208[0x1c] + iVar4 * 4);
            }
            uVar3 = (**(code **)(**ppiVar7 + 0x5c))(puVar1,piStack_20c,param_3,in_stack_00000010);
            ppiVar7 = ppiVar7 + 1;
            local_21c = local_21c | uVar3;
            iVar4 = iVar4 + 1;
          } while (iVar4 < (int)(this->cls_0x5a68d8).mbr_0x68);
        }
        iStack_214 = iStack_214 + 1;
        piStack_210 = piStack_210 + 0x15;
      } while (iStack_214 < iStack_218);
    }
    return local_21c;
  }
  return 0;
}



// Function at 004bcd30

undefined4 __thiscall OOAnalyzer::cls_0x5a3e7c::meth_0x4bcd30(cls_0x5a3e7c *this,uint *param_1)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  cls_0x5a68d8__vftable_5a68d8 *pcVar4;
  uint uVar5;
  uint *puVar6;
  undefined uVar7;
  uint uVar8;
  undefined3 extraout_var;
  int iVar9;
  ushort *puVar10;
  int iVar11;
  int in_stack_00000008;
  
  puVar6 = param_1;
  if (*param_1 == 0x80000000) {
    if (in_stack_00000008 == 0) {
      *param_1 = 0;
    }
    else {
      *param_1 = *(uint *)(in_stack_00000008 + 0x14);
    }
  }
  if (((this->cls_0x5a68d8).mbr_0x1c == 1) && ((*param_1 & 0x4000006) == 0)) {
    *param_1 = *param_1 | 2;
  }
  if ((in_stack_00000008 != 0) && ((*param_1 & 0x20000) != 0)) {
    param_1[10] = param_1[10] - *(int *)(in_stack_00000008 + 8);
    param_1[0xb] = param_1[0xb] - *(int *)(in_stack_00000008 + 0xc);
    *param_1 = *param_1 & 0xfffdffff;
  }
  if ((*param_1 & 0x40000) != 0) {
    cls_0x429950::cls_0x429950((cls_0x429950 *)&param_1,0xff,0);
    uVar8 = meth_0x4384e0(this);
    puVar6[0x12] = uVar8;
  }
  dVar1 = (this->cls_0x5a68d8).mbr_0x14;
  dVar2 = (this->cls_0x5a68d8).mbr_0x20;
  puVar6[5] = (this->cls_0x5a68d8).mbr_0x18;
  dVar3 = (this->cls_0x5a68d8).mbr_0x28;
  puVar6[4] = dVar1;
  dVar1 = (this->cls_0x5a68d8).mbr_0x24;
  puVar6[6] = dVar2;
  dVar2 = (this->cls_0x5a68d8).mbr_0x2c;
  puVar6[8] = dVar3;
  pcVar4 = (this->cls_0x5a68d8).vftptr_0x0;
  puVar6[7] = dVar1;
  puVar6[9] = dVar2;
  uVar7 = (*pcVar4->virt_meth_0x4bcb90_76)(this);
  dVar1 = (this->cls_0x5a68d8).mbr_0x14;
  dVar2 = (this->cls_0x5a68d8).mbr_0x18;
  puVar6[2] = CONCAT31(extraout_var,uVar7);
  iVar11 = puVar6[0xb] + dVar2;
  dVar2 = (this->cls_0x5a68d8).mbr_0x20;
  iVar9 = puVar6[10] + dVar1;
  if (((((int)dVar2 < (int)(puVar6[0xc] + iVar9)) &&
       (dVar1 = (this->cls_0x5a68d8).mbr_0x24, (int)dVar1 < (int)(puVar6[0xd] + iVar11))) &&
      (iVar9 < (int)((this->cls_0x5a68d8).mbr_0x28 + dVar2))) &&
     (iVar11 < (int)((this->cls_0x5a68d8).mbr_0x2c + dVar1))) {
    if ((in_stack_00000008 != 0) && (uVar8 = *(uint *)(in_stack_00000008 + 0x10), (uVar8 & 1) != 0))
    {
      uVar5 = (this->cls_0x5a68d8).mbr_0x38;
      if ((uVar5 & 0x20000) != 0) {
        if ((uVar8 & 0x200000) != 0) {
          FUN_004b9690(in_stack_00000008);
        }
        *(uint *)(in_stack_00000008 + 0x10) =
             *(uint *)(in_stack_00000008 + 0x10) & 0xff4fffff | 0x400000;
        return 1;
      }
      if (((uVar5 & 6) != 0) && ((uVar8 & 0x400000) != 0)) {
        if ((*(int *)(in_stack_00000008 + 0x40) != 0) &&
           (puVar10 = (ushort *)(*(int *)(in_stack_00000008 + 0x40) + 0x40 + in_stack_00000008),
           puVar10 != (ushort *)0x0)) {
          iVar9 = 0x100;
          do {
            *puVar10 = *puVar10 & 0x7fff;
            puVar10 = puVar10 + 1;
            iVar9 = iVar9 + -1;
          } while (iVar9 != 0);
        }
        *(uint *)(in_stack_00000008 + 0x10) = uVar8 & 0xff1fffff | 0x100000;
        if ((uVar5 & 4) != 0) {
          FUN_004b8b30(in_stack_00000008);
        }
      }
    }
    return 1;
  }
  return 0;
}



// Function at 004bd200

undefined4 __thiscall OOAnalyzer::cls_0x5a3e7c::meth_0x4bd200(cls_0x5a3e7c *this,uint *param_1)

{
  dword dVar1;
  dword dVar2;
  cls_0x5a68d8__vftable_5a68d8 *pcVar3;
  undefined uVar4;
  int iVar5;
  undefined3 extraout_var;
  int iVar6;
  int in_stack_00000008;
  
  if (*param_1 == 0x80000000) {
    *param_1 = 0;
  }
  if (((this->cls_0x5a68d8).mbr_0x1c == 1) && ((*param_1 & 0x4000006) == 0)) {
    *param_1 = *param_1 | 2;
  }
  if (in_stack_00000008 == 0) {
    iVar5 = 0;
    iVar6 = 0;
  }
  else {
    iVar6 = *(int *)(in_stack_00000008 + 0x14);
    iVar5 = *(int *)(in_stack_00000008 + 0x18);
  }
  dVar1 = (this->cls_0x5a68d8).mbr_0x14;
  param_1[0xf] = param_1[0xf] + iVar5;
  dVar2 = (this->cls_0x5a68d8).mbr_0x18;
  param_1[4] = dVar1;
  dVar1 = (this->cls_0x5a68d8).mbr_0x20;
  param_1[5] = dVar2;
  dVar2 = (this->cls_0x5a68d8).mbr_0x24;
  param_1[6] = dVar1;
  dVar1 = (this->cls_0x5a68d8).mbr_0x28;
  param_1[7] = dVar2;
  dVar2 = (this->cls_0x5a68d8).mbr_0x2c;
  param_1[8] = dVar1;
  pcVar3 = (this->cls_0x5a68d8).vftptr_0x0;
  param_1[0xe] = param_1[0xe] + iVar6;
  param_1[9] = dVar2;
  uVar4 = (*pcVar3->virt_meth_0x4bcb90_76)(this);
  param_1[2] = CONCAT31(extraout_var,uVar4);
  return 1;
}



// Function at 004bdcf0

void __thiscall
OOAnalyzer::cls_0x5a3e7c::meth_0x4bdcf0
          (cls_0x5a3e7c *this,undefined4 param_1,undefined2 param_2,undefined4 param_3,
          undefined4 *param_4,undefined4 param_5,int param_6)

{
  int in_stack_0000001c;
  
  if (in_stack_0000001c != 0) {
    meth_0x4384e0(this);
  }
  (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x4bcef0_88)(&this->cls_0x5a68d8);
  return;
}



// Function at 004bdf40

void __thiscall OOAnalyzer::cls_0x5a3e7c::meth_0x4bdf40(cls_0x5a3e7c *this)

{
  undefined uVar1;
  undefined3 extraout_var;
  int in_stack_00000008;
  int in_stack_0000000c;
  int in_stack_00000010;
  int in_stack_00000014;
  undefined auStack_60 [20];
  code *pcStack_4c;
  undefined4 uStack_48;
  undefined *puStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  int iStack_20;
  int iStack_1c;
  int iStack_10;
  int iStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x438d00_20)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    FUN_00481c10((byte *)s_Can_t_draw_line_in_video_surface_005df02c);
  }
  iStack_1c = (in_stack_00000014 - in_stack_0000000c) + 1;
  iStack_20 = (in_stack_00000010 - in_stack_00000008) + 1;
  uStack_48 = 0;
  puStack_44 = (undefined *)0x0;
  uStack_3c = 0;
  uStack_40 = 0;
  uStack_2c = 0;
  uStack_30 = 0;
  uStack_34 = 0;
  uStack_38 = 0;
  uStack_4 = 0x1f;
  uStack_8 = 0;
  pcStack_4c = FUN_004b9700;
  iStack_10 = iStack_20;
  iStack_c = iStack_1c;
  uStack_8 = meth_0x4384e0(this);
  puStack_44 = auStack_60;
  (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x4bcef0_88)(&this->cls_0x5a68d8);
  return;
}



