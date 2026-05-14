#include "classes_all.h"

// Decompiled methods and structure for class: SoftwareRenderer



// Function at 0046e8b0

bool SoftwareRenderer::virt_meth_0x46e8b0_CreateAnimator()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int *piVar2;
  
  if (this->mbr_0x58 == 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477dd0_416)(this);
    if (CONCAT31(extraout_var,uVar1) != 0) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477dd0_416)(this);
      if (DAT_005d79e4 < CONCAT31(extraout_var_00,uVar1)) {
        return false;
      }
    }
    piVar2 = (int *)(**(code **)(*(int *)this->mbr_0x54 + 0x34))(this);
    this->mbr_0x58 = (dword)piVar2;
    if (piVar2 != (int *)0x0) {
      (**(code **)(*piVar2 + 0x18))();
    }
  }
  return this->mbr_0x58 != 0;
}



// Function at 0046e900

void SoftwareRenderer::virt_meth_0x46e900_FreeAnimator()

{
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x1c))();
    if ((undefined4 *)this->mbr_0x58 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)this->mbr_0x58)(1);
    }
    this->mbr_0x58 = 0;
  }
  return;
}



// Function at 0046e930

int SoftwareRenderer::virt_meth_0x46e930_NeedsAnimator()

{
  int iVar1;
  
  if ((this->mbr_0x58 == 0) && ((int *)this->mbr_0x54 != (int *)0x0)) {
    iVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0x38))(this);
    if ((iVar1 != 0) && (((this->mbr_0x8 & 0x4000) == 0 || ((this->mbr_0x8 & 0x8000) == 0)))) {
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
    return iVar1;
  }
  return 0;
}



// Function at 0046e970

void SoftwareRenderer::virt_meth_0x46e970_Animate(int param_1)

{
  SoftwareRenderer__vftable_5b8e94 *pcVar1;
  undefined uVar2;
  int iVar3;
  undefined3 extraout_var;
  int unaff_retaddr;
  int in_stack_00000008;
  
  if (((this->mbr_0x8 & 0x2000000) != 0) && (in_stack_00000008 != 0xd)) {
    (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
  }
  if (((this->mbr_0x8 & 0x800000) != 0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
    iVar3 = FUN_00483300_RandomRange(0,3);
    if (iVar3 == 0) {
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
  }
  uVar2 = (*this->vftptr_0x0->virt_meth_0x477e50_448)(this);
  if (unaff_retaddr < CONCAT31(extraout_var,uVar2)) {
    pcVar1 = this->vftptr_0x0;
    (*pcVar1->virt_meth_0x477e50_448)(this);
    (*pcVar1->virt_meth_0x477e60_452)(this);
    return;
  }
  (*this->vftptr_0x0->virt_meth_0x477e60_452)(this);
  return;
}



// Function at 0046ea20

int SoftwareRenderer::virt_meth_0x46ea20_Distance()

{
  int iVar1;
  byte bVar2;
  int iVar3;
  int in_stack_00000004;
  
  iVar3 = *(int *)(in_stack_00000004 + 0x10) - this->mbr_0x10;
  if (iVar3 < 0) {
    iVar3 = this->mbr_0x10 - *(int *)(in_stack_00000004 + 0x10);
  }
  iVar1 = *(int *)(in_stack_00000004 + 0x14) - this->mbr_0x14;
  if (iVar1 < 0) {
    iVar1 = this->mbr_0x14 - *(int *)(in_stack_00000004 + 0x14);
  }
  bVar2 = 0;
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  for (; (0xff < iVar3 || (0xff < iVar1)); iVar1 = iVar1 >> 1) {
    iVar3 = iVar3 >> 1;
    bVar2 = bVar2 + 1;
  }
  return (uint)(byte)(&DAT_005e9200)[iVar1 + iVar3 * 0x100] << (bVar2 & 0x1f);
}



// Function at 0046f160

undefined4 SoftwareRenderer::virt_meth_0x46f160()

{
  undefined4 uVar1;
  uint in_stack_00000004;
  
  if ((int *)this->mbr_0x54 != (int *)0x0) {
    if ((int)in_stack_00000004 < 0) {
      in_stack_00000004 = (uint)this->mbr_0xc;
    }
    uVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0xcc))(in_stack_00000004,0);
    return uVar1;
  }
  return 0;
}



// Function at 0046f190

int SoftwareRenderer::virt_meth_0x46f190()

{
  int iVar1;
  
  if ((int *)this->mbr_0x54 != (int *)0x0) {
    iVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0xd4))(this->mbr_0xc,0);
    if (iVar1 != 0) {
      return iVar1;
    }
    iVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0xd8))(this->mbr_0xc);
    if ((iVar1 != 0) && (*(int *)(iVar1 + 0x38) != 0)) {
      return *(int *)(iVar1 + 0x38) + 0x38 + iVar1;
    }
  }
  return 0;
}



// Function at 0046f1e0

void SoftwareRenderer::virt_meth_0x46f1e0()

{
  short sVar1;
  uint uVar2;
  
  if ((int *)this->mbr_0x54 == (int *)0x0) {
    uVar2 = 0;
  }
  else {
    uVar2 = (**(code **)(*(int *)this->mbr_0x54 + 0x8c))(this->mbr_0xc);
  }
  if ((int *)this->mbr_0x54 == (int *)0x0) {
    sVar1 = 0;
  }
  else {
    sVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0x90))(this->mbr_0xc);
  }
  if ((uVar2 & 0x100) == 0) {
    this->mbr_0x5c = 0;
    this->mbr_0x5e = 1;
  }
  else {
    this->mbr_0x5e = 0xffff;
    this->mbr_0x5c = sVar1 - 1;
  }
  if ((int *)this->mbr_0x58 != (int *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0046f246. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(int *)this->mbr_0x58 + 0x20))();
    return;
  }
  return;
}



// Function at 0046f250

undefined4 SoftwareRenderer::virt_meth_0x46f250()

{
  uint uVar1;
  int iVar2;
  uint in_stack_00000004;
  
  if ((int *)this->mbr_0x54 != (int *)0x0) {
    uVar1 = (**(code **)(*(int *)this->mbr_0x54 + 0x3c))();
    if (uVar1 <= in_stack_00000004) {
      (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
      return 0;
    }
  }
  uVar1 = this->mbr_0x8;
  if (((uVar1 & 8) == 0) &&
     ((((uVar1 & 0x400) == 0 || ((uVar1 & 4) != 0)) && ((short)this->mbr_0x7c < 0)))) {
    (*this->vftptr_0x0->virt_meth_0x471020_244)(this);
    TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,&stack0xffffffec);
  }
  TCharacter::meth_0x452750((TCharacter *)&DAT_006668d8,this);
  FUN_00456790(s_d__revenant_Object_cpp_005d4810,0x413);
  this->mbr_0x62 = this->mbr_0x5c;
  this->mbr_0x60 = this->mbr_0xc;
  this->mbr_0xc = (word)in_stack_00000004;
  (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
  (*this->vftptr_0x0->virt_meth_0x46f1e0_320)(this);
  FUN_004567c0();
  TCharacter::meth_0x452750((TCharacter *)&DAT_006668d8,this);
  iVar2 = (**(code **)(*(int *)this->mbr_0x54 + 0x38))(this);
  if (iVar2 == 0) {
    (*this->vftptr_0x0->virt_meth_0x46e900_44)(this);
    uVar1 = this->mbr_0x8 & 0xffffbfff;
  }
  else {
    uVar1 = this->mbr_0x8 | 0x4000;
  }
  this->mbr_0x8 = uVar1;
  if ((((uVar1 & 8) == 0) && (((uVar1 & 0x400) == 0 || ((uVar1 & 4) != 0)))) &&
     ((short)this->mbr_0x7c < 0)) {
    (*this->vftptr_0x0->virt_meth_0x471020_244)(this);
    TCharacter::meth_0x4548a0((TCharacter *)&DAT_006668d8,&stack0xffffffe4);
  }
  return 1;
}



// Function at 0046f3a0

bool SoftwareRenderer::virt_meth_0x46f3a0()

{
  int iVar1;
  
  iVar1 = TCharacter::meth_0x451090((TCharacter *)&DAT_006668d8);
  return -1 < iVar1;
}



// Function at 0046f3d0

undefined4 SoftwareRenderer::virt_meth_0x46f3d0()

{
  int *piVar1;
  SoftwareRenderer *pcVar2;
  SoftwareRenderer *pcVar3;
  undefined uVar4;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  cls_0x5b4f30_TPlayer *pcVar8;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  undefined3 extraout_var_08;
  uint extraout_ECX;
  uint extraout_ECX_00;
  SoftwareRenderer *pcVar9;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  SoftwareRenderer__vftable_5b8e94 *pcVar10;
  int unaff_EBX;
  SoftwareRenderer *unaff_retaddr;
  cls_0x5b4f30_TPlayer *in_stack_00000004;
  uint in_stack_00000008;
  cls_0x477870 cStack_1c;
  
  uVar4 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
  if (CONCAT31(extraout_var,uVar4) != 0) {
    uVar4 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
    uVar5 = (**(code **)(*(int *)CONCAT31(extraout_var_00,uVar4) + 0x58))
                      (in_stack_00000004,in_stack_00000008);
    return uVar5;
  }
  if ((int)in_stack_00000008 < 0) {
    if (this->mbr_0x4 == 0xb) {
      cStack_1c.mbr_0x0 = 1;
      cStack_1c.mbr_0x10 = 0;
      cStack_1c.mbr_0xc = 0;
      cStack_1c.mbr_0x14 = 0;
      cStack_1c.mbr_0x18 = 0;
      cStack_1c.mbr_0x4 = (dword)this;
      cStack_1c.mbr_0x8 = (dword)this;
      cls_0x477870::meth_0x46dfb0(&cStack_1c);
      uVar6 = extraout_ECX;
      piVar1 = (int *)cStack_1c.mbr_0x18;
      while (piVar1 != (int *)0x0) {
        cStack_1c.mbr_0x18 = (dword)piVar1;
        uVar6 = FUN_0059a530_stricmp(uVar6,(byte *)piVar1[0xe],(byte *)s_Pouch_005d4828);
        if (((uVar6 == 0) && (iVar7 = (**(code **)(*piVar1 + 0x7c))(0), iVar7 != 0)) &&
           (*(short *)(iVar7 + 4) ==
            *(short *)&(in_stack_00000004->TPlayScreen).TScreen.mbr_0x4)) {
          uVar5 = (**(code **)(*piVar1 + 0x58))(in_stack_00000004,in_stack_00000008);
          return uVar5;
        }
        cls_0x477870::meth_0x46dfb0(&cStack_1c);
        uVar6 = extraout_ECX_00;
        piVar1 = (int *)cStack_1c.mbr_0x18;
      }
      cStack_1c.mbr_0x18 = 0;
    }
    uVar4 = (*this->vftptr_0x0->virt_meth_0x46fef0_148)(this);
    in_stack_00000008 = CONCAT31(extraout_var_01,uVar4);
    if (0xfe < in_stack_00000008) {
      return 0;
    }
  }
  if (0x115 < (int)in_stack_00000008) {
    return 0;
  }
  uVar4 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
  if (CONCAT31(extraout_var_02,uVar4) == 0) {
    cls_0x477870::cls_0x477870(&cStack_1c,this);
    while ((cls_0x5b4f30_TPlayer *)cStack_1c.mbr_0x18 != (cls_0x5b4f30_TPlayer *)0x0) {
      pcVar8 = (cls_0x5b4f30_TPlayer *)cStack_1c.mbr_0x18;
      if ((int)*(short *)&(((TPlayScreen *)cStack_1c.mbr_0x18)->TScreen).field_0x7c ==
          in_stack_00000008) goto LAB_0046f532;
      cls_0x477870::meth_0x46dfb0(&cStack_1c);
    }
    pcVar8 = (cls_0x5b4f30_TPlayer *)0x0;
  }
  else {
    uVar4 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
    pcVar8 = (cls_0x5b4f30_TPlayer *)
             cls_0x5b4f30_TPlayer::meth_0x4701f0((cls_0x5b4f30_TPlayer *)CONCAT31(extraout_var_03,uVar4));
  }
LAB_0046f532:
  if (pcVar8 != in_stack_00000004) {
    (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->FUN_00470f00_296)();
    pcVar3 = (SoftwareRenderer *)this->mbr_0x64;
    pcVar9 = this;
    while (pcVar2 = pcVar3, pcVar2 != (SoftwareRenderer *)0x0) {
      pcVar9 = pcVar2;
      pcVar3 = (SoftwareRenderer *)pcVar2->mbr_0x64;
    }
    pcVar9 = (SoftwareRenderer *)(-(uint)(pcVar9 != this) & (uint)pcVar9);
    if (pcVar9 == (SoftwareRenderer *)0x0) {
      pcVar9 = this;
    }
    if ((in_stack_00000004->TPlayScreen).TScreen.mbr_0x64 != 0) {
      uVar5 = *(undefined4 *)&(in_stack_00000004->TPlayScreen).TScreen.field_0x40;
      uVar4 = (*pcVar9->vftptr_0x0->virt_meth_0x470330_172)(pcVar9);
      if (CONCAT31(extraout_var_04,uVar4) != 0) {
        unaff_retaddr = (SoftwareRenderer *)(in_stack_00000004->TPlayScreen).TScreen.mbr_0x64;
        unaff_EBX = 1;
      }
      if ((in_stack_00000004->TPlayScreen).TScreen.mbr_0x64 != 0) {
        DAT_00676e5d._0_1_ = 1;
        (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046faf0_96)(uVar5);
        DAT_00676e5d._0_1_ = 0;
      }
    }
    uVar4 = (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046fee0_152)(this);
    if (CONCAT31(extraout_var_05,uVar4) != 0) {
      if (unaff_EBX == 0) {
        cls_0x57d9d0::meth_0x585880((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
      }
      else {
        cls_0x57d9d0::meth_0x585ab0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,in_stack_00000004);
      }
      if (in_stack_00000004 != (cls_0x5b4f30_TPlayer *)0x0) {
        cls_0x5b4f30_TPlayer::meth_0x46e630(in_stack_00000004);
        (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477840_0)
                  ((TScreen *)in_stack_00000004);
      }
      UNK_0065d53c._12_4_ = 1;
      (**(code **)(DAT_0065d4f8 + 0x90))();
      DAT_0065b02c._76_4_ = 1;
      return 1;
    }
    if (pcVar8 != (cls_0x5b4f30_TPlayer *)0x0) {
      DAT_00676e5d._0_1_ = 1;
      (*((pcVar8->TPlayScreen).TScreen.vftptr_0x0)->FUN_0046faf0_96)();
      DAT_00676e5d._0_1_ = 0;
    }
    iVar7 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x68);
    if (iVar7 < 0) {
      return 0;
    }
    uVar6 = extraout_ECX_01;
    if (*(int *)&(in_stack_00000004->TPlayScreen).TScreen.field_0x40 < 1) {
      uVar6 = FUN_0044ce30();
      *(uint *)&(in_stack_00000004->TPlayScreen).TScreen.field_0x40 = uVar6;
      uVar6 = extraout_ECX_02;
    }
    *(short *)&(in_stack_00000004->TPlayScreen).TScreen.field_0x7e = (short)iVar7;
    *(undefined2 *)&(in_stack_00000004->TPlayScreen).TScreen.field_0x7c = 0;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0x64 = (dword)this;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0x18 = 0;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0x14 = 0;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0x10 = 0;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0xe = 0;
    (in_stack_00000004->TPlayScreen).TScreen.mbr_0x44 = 0;
    if (pcVar8 != (cls_0x5b4f30_TPlayer *)0x0) {
      DAT_00676e5d._0_1_ = 1;
      if (unaff_retaddr == (SoftwareRenderer *)0x0) {
        pcVar10 = this->vftptr_0x0;
        pcVar9 = this;
      }
      else {
        pcVar10 = unaff_retaddr->vftptr_0x0;
        pcVar9 = unaff_retaddr;
      }
      (*pcVar10->virt_meth_0x46f3d0_88)(pcVar9);
      DAT_00676e5d._0_1_ = 0;
      uVar6 = extraout_ECX_03;
    }
    uVar6 = FUN_0059a530_stricmp(uVar6,(byte *)(in_stack_00000004->TPlayScreen).TScreen.mbr_0x38,
                         (byte *)s_Swag_Bag_005d4830);
    if (((((uVar6 == 0) && (this->mbr_0x4 == 0xb)) &&
         ((this->mbr_0x494 != 0 &&
          ((iVar7 = cls_0x45f7c0::meth_0x51f840((cls_0x45f7c0 *)&DAT_0065a890), -1 < iVar7 &&
           (iVar7 = *(int *)(DAT_0065a8b4 + iVar7 * 4), iVar7 != 0)))))) &&
        (pcVar9 = (SoftwareRenderer *)
                  cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer
                            ((cls_0x45f7c0 *)&DAT_0065a890,*(undefined4 *)(iVar7 + 0x4c)),
        pcVar9 != (SoftwareRenderer *)0x0)) &&
       ((pcVar9 != this &&
        (uVar4 = (*pcVar9->vftptr_0x0->virt_meth_0x470280_168)(pcVar9),
        CONCAT31(extraout_var_06,uVar4) != 0)))) {
      (*((in_stack_00000004->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x477d60_372)
                (in_stack_00000004);
    }
    if (unaff_EBX == 0) {
      cls_0x57d9d0::meth_0x585880((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
    }
    else {
      cls_0x57d9d0::meth_0x585ab0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,in_stack_00000004);
    }
    if ((DAT_0065d674 != (SoftwareRenderer *)0x0) &&
       ((((this == DAT_0065d674 ||
          (uVar4 = (*DAT_0065d674->vftptr_0x0->virt_meth_0x477d50_368)(DAT_0065d674),
          this == (SoftwareRenderer *)CONCAT31(extraout_var_07,uVar4))) ||
         (unaff_retaddr == DAT_0065d674)) ||
        (uVar4 = (*DAT_0065d674->vftptr_0x0->virt_meth_0x477d50_368)(DAT_0065d674),
        unaff_retaddr == (SoftwareRenderer *)CONCAT31(extraout_var_08,uVar4))))) {
      UNK_0065d53c._12_4_ = 1;
      (**(code **)(DAT_0065d4f8 + 0x90))();
    }
    if ((this == DAT_0065b088) || (unaff_retaddr == DAT_0065b088)) {
      DAT_0065b02c._76_4_ = 1;
    }
  }
  return 1;
}



// Function at 0046faf0

void SoftwareRenderer::virt_meth_0x46faf0_RemoveFromInventory()

{
  int iVar1;
  int iVar2;
  undefined uVar3;
  undefined3 extraout_var;
  int iVar4;
  dword dVar5;
  int *piVar6;
  
  uVar3 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
  if (CONCAT31(extraout_var,uVar3) != 0) {
    (*this->vftptr_0x0->virt_meth_0x477d60_372)(this);
  }
  if (this->mbr_0x64 != 0) {
    cls_0x57d9d0::meth_0x5859c0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
    if ((((this->mbr_0x64 != 0) && (*(short *)(this->mbr_0x64 + 4) == 0xb)) &&
        (0xff < (short)this->mbr_0x7c)) && ((short)this->mbr_0x7c < 0x10b)) {
      (*this->vftptr_0x0->virt_meth_0x477d70_392)(this);
      TPlayScreen::meth_0x5199b0((TPlayScreen *)this->mbr_0x64,0);
    }
    cls_0x45f7c0::meth_0x41cb80((cls_0x45f7c0 *)(this->mbr_0x64 + 0x68));
    iVar4 = 0;
    iVar1 = *(int *)(this->mbr_0x64 + 0x68);
    if (0 < iVar1) {
      piVar6 = *(int **)(this->mbr_0x64 + 0x78);
      do {
        iVar2 = *piVar6;
        piVar6 = piVar6 + 1;
        *(short *)(iVar2 + 0x7e) = (short)iVar4;
        iVar4 = iVar4 + 1;
      } while (iVar4 < iVar1);
    }
    piVar6 = DAT_0065d674;
    this->mbr_0x64 = 0;
    if (piVar6 != (int *)0x0) {
      dVar5 = (**(code **)(*piVar6 + 0x170))();
      if (this->mbr_0x64 == dVar5) {
        UNK_0065d53c._12_4_ = 1;
        (**(code **)(DAT_0065d4f8 + 0x90))();
      }
    }
    if (this->mbr_0x64 == DAT_0065b088) {
      DAT_0065b02c._76_4_ = 1;
    }
  }
  this->mbr_0x7e = 0xffff;
  this->mbr_0x7c = 0xffff;
  return;
}



// Function at 0046fc40

int SoftwareRenderer::virt_meth_0x46fc40(int *param_1, int *param_2)

{
  int iVar1;
  int *unaff_retaddr;
  int in_stack_0000000c;
  
  if (in_stack_0000000c < 1) {
    return 0;
  }
  if (param_2 == (int *)0x0) {
    return 0;
  }
  iVar1 = (**(code **)(*param_2 + 0x198))();
  if (iVar1 < 2) {
    iVar1 = 1;
  }
  else {
    iVar1 = (**(code **)(*param_2 + 0x198))();
  }
  if (iVar1 <= in_stack_0000000c) {
    (**(code **)(*param_2 + 0x60))();
    if (param_1 != (int *)0x0) {
      (**(code **)(*param_1 + 0x58))(param_2,0xffffffff);
      return iVar1;
    }
    (**(code **)*param_2)(1);
    return iVar1;
  }
  (**(code **)(*param_2 + 0x19c))(iVar1 - in_stack_0000000c);
  if (unaff_retaddr != (int *)0x0) {
    iVar1 = (**(code **)(*unaff_retaddr + 0x54))(this->mbr_0x38);
    if (iVar1 == 0) {
      return 0;
    }
  }
  if (param_2[0x19] == DAT_0065d674) {
    UNK_0065d53c._12_4_ = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
  }
  if (param_2[0x19] == DAT_0065b088) {
    DAT_0065b02c._76_4_ = 1;
  }
  return in_stack_0000000c;
}



// Function at 0046fd30

int SoftwareRenderer::virt_meth_0x46fd30(undefined4 param_1, undefined4 param_2)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar3;
  undefined4 unaff_retaddr;
  int in_stack_0000000c;
  int iVar4;
  int iVar2;
  
  iVar3 = 0;
  if (in_stack_0000000c < 1) {
    return 0;
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x470280_168)(this);
  iVar2 = CONCAT31(extraout_var,uVar1);
  if (iVar2 != 0) {
    while( true ) {
      iVar4 = in_stack_0000000c;
      uVar1 = (*this->vftptr_0x0->virt_meth_0x46fc40_108)(this,unaff_retaddr,iVar2);
      in_stack_0000000c = in_stack_0000000c - CONCAT31(extraout_var_00,uVar1);
      iVar3 = iVar3 + CONCAT31(extraout_var_00,uVar1);
      if (in_stack_0000000c < 1) break;
      param_2 = unaff_retaddr;
      uVar1 = (*this->vftptr_0x0->virt_meth_0x470280_168)(this);
      iVar2 = CONCAT31(extraout_var_01,uVar1);
      if (iVar2 == 0) {
        return iVar3;
      }
    }
    if (iVar2 != 0) {
      if (*(int *)(iVar2 + 100) == DAT_0065d674) {
        UNK_0065d53c._12_4_ = 1;
        (**(code **)(DAT_0065d4f8 + 0x90))(iVar4,param_2);
      }
      if (*(int *)(iVar2 + 100) == DAT_0065b088) {
        DAT_0065b02c._76_4_ = 1;
      }
    }
  }
  return iVar3;
}



// Function at 0046fea0

uint SoftwareRenderer::virt_meth_0x46fea0()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar2;
  undefined3 extraout_var_01;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
    uVar2 = (**(code **)(*(int *)CONCAT31(extraout_var_00,uVar1) + 0x88))();
    return uVar2;
  }
  uVar1 = (*this->vftptr_0x0->virt_meth_0x46fef0_148)(this);
  return (uint)(CONCAT31(extraout_var_01,uVar1) < 0xff);
}



// Function at 0046fee0

undefined4 SoftwareRenderer::virt_meth_0x46fee0()

{
  return 0;
}



// Function at 004705f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 SoftwareRenderer::virt_meth_0x4705f0(cls_0x5b4f30_TPlayer *param_1)

{
  dword dVar1;
  int *piVar2;
  char cVar3;
  undefined uVar4;
  int *piVar5;
  undefined3 extraout_var;
  int iVar6;
  uint uVar7;
  uint uVar8;
  undefined4 *puVar9;
  int in_stack_00000008;
  undefined4 uVar10;
  undefined4 uStack_28;
  
  if (DAT_0066829c != 0) {
    if (((*(short *)&(param_1->TPlayScreen).TScreen.mbr_0x4 == 0xb) &&
        (((param_1->TPlayScreen).mbr_0x36c & 2) != 0)) && (this->mbr_0x4 != 0x11)) {
      cls_0x5b4f30_TPlayer::meth_0x51d680_SetPlayerState(param_1);
    }
    if ((this->mbr_0x4 == 5) || (this->mbr_0x4 == 0x11)) {
      if (DAT_00676e5c != '\0') goto LAB_00470682;
    }
    else if (DAT_0067682c == 0) {
      if ((DAT_00676e5c == '\0') &&
         (cVar3 = cls_0x57d9d0::meth_0x5847c0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,param_1),
         cVar3 != '\0')) {
        return 1;
      }
      goto LAB_00470682;
    }
    cls_0x57d9d0::meth_0x584710((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,param_1);
  }
LAB_00470682:
  piVar5 = (int *)FUN_00452690_TMapPane_GetInstance(in_stack_00000008,0);
  dVar1 = this->mbr_0x84;
  if (piVar5 == (int *)0x0) {
    if (dVar1 != 0) {
      FUN_00492640(dVar1,7,(byte *)this->mbr_0x38,*(byte **)this->mbr_0x4c,(int *)param_1,
                   &DAT_005d48cc,0,0);
    }
    if ((param_1 != (cls_0x5b4f30_TPlayer *)0x0) &&
       (dVar1 = (param_1->TPlayScreen).TScreen.mbr_0x84, dVar1 != 0)) {
      FUN_00492640(dVar1,7,(byte *)this->mbr_0x38,(byte *)0x0,(int *)this,&DAT_005d48d4,0,0);
    }
  }
  else {
    if (dVar1 != 0) {
      FUN_00492640(dVar1,7,(byte *)piVar5[0xe],(byte *)0x0,(int *)param_1,&DAT_005d48c4,piVar5,
                   &DAT_005d48bc);
    }
    if (((((piVar5[2] & 0x40000000U) != 0) && (piVar5[0x19] == this->mbr_0x64)) &&
        (*(word *)(piVar5 + 1) == this->mbr_0x4)) &&
       ((uVar4 = (*this->vftptr_0x0->virt_meth_0x473600_212)(this),
        CONCAT31(extraout_var,uVar4) != 0 && (iVar6 = (**(code **)(*piVar5 + 0xd4))(), iVar6 != 0)))
       ) {
      iVar6 = (int)(short)this->mbr_0x6 - (int)*(short *)((int)piVar5 + 6);
      if (iVar6 < 1) {
        iVar6 = (int)*(short *)((int)piVar5 + 6) - (int)(short)this->mbr_0x6;
      }
      if (iVar6 == 1) {
        piVar2 = (int *)this->mbr_0x64;
        if (piVar2 != (int *)0x0) {
          (*this->vftptr_0x0->virt_meth_0x46faf0_96)(this);
          (**(code **)(*piVar5 + 0x60))();
        }
        uVar10 = _DAT_00666970;
        (*this->vftptr_0x0->virt_meth_0x46ed70_8)(this);
        uVar7 = this->mbr_0x8 | 0x8000;
        (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
        (*this->vftptr_0x0->virt_meth_0x46f3a0_140)(this);
        uVar8 = this->mbr_0x8 | 0x1000;
        (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
        (**(code **)(*piVar5 + 8))(&stack0xffffffa4,uVar10,0,uVar8,uVar7);
        (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x8000);
        (**(code **)(*piVar5 + 0x8c))();
        (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x1000);
        puVar9 = (undefined4 *)&stack0xffffff9c;
        for (iVar6 = 0xd; iVar6 != 0; iVar6 = iVar6 + -1) {
          *puVar9 = 0;
          puVar9 = puVar9 + 1;
        }
        uVar10 = 0;
        iVar6 = TCharacter::meth_0x450e40((TCharacter *)&DAT_006668d8,&stack0xffffff9c);
        piVar5 = (int *)FUN_00452690_TMapPane_GetInstance(iVar6,uVar10);
        if ((piVar5 != (int *)0x0) && (piVar2 != (int *)0x0)) {
          (**(code **)(*piVar2 + 0x50))(*(undefined4 *)(piVar5[0x13] + 0x1c),uStack_28,0xffffffff);
          (**(code **)(*piVar5 + 0x40))(piVar5[2] | 0x1000);
        }
        return 1;
      }
    }
  }
  return 0;
}



// Function at 004708e0

void SoftwareRenderer::virt_meth_0x4708e0_OnScreen()

{
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x2c))();
  }
  if ((this->mbr_0x84 != 0) && ((this->mbr_0x8 & 0x200000) == 0)) {
    (*this->vftptr_0x0->virt_meth_0x477d30_340)(this);
    cls_0x4922c0::meth_0x4933d0_TScript_Continue((cls_0x4922c0 *)this->mbr_0x84);
  }
  return;
}



// Function at 00470920

uint SoftwareRenderer::virt_meth_0x470920()

{
  dword *pdVar1;
  word wVar2;
  undefined uVar3;
  undefined3 extraout_var;
  int iVar4;
  dword dVar5;
  dword dVar6;
  uint uVar7;
  dword dVar8;
  dword dStack_18;
  dword dStack_14;
  dword dStack_10;
  int iStack_c;
  int iStack_8;
  dword dStack_4;
  
  if (((((*(byte *)&this->mbr_0x8 & 1) != 0) ||
       (uVar3 = (*this->vftptr_0x0->virt_meth_0x477a10_180)(this), CONCAT31(extraout_var,uVar3) != 0
       )) || (wVar2 = this->mbr_0x4, wVar2 == 9)) || ((wVar2 == 10 || (wVar2 == 0xe)))) {
    return 0;
  }
  dStack_18 = this->mbr_0x10;
  pdVar1 = &this->mbr_0x10;
  dStack_14 = this->mbr_0x14;
  dStack_10 = this->mbr_0x18;
  uVar7 = 0;
  iVar4 = FUN_00452e10_GetWalkHeight((int *)pdVar1,(uint)this->mbr_0xe,(int *)0x0);
  dVar5 = this->mbr_0x18;
  dVar8 = iVar4 + 1;
  if ((((int)dVar5 < (int)dVar8) || (dVar8 == 1)) && ((this->mbr_0x8 & 0x1000000) == 0)) {
    return 2;
  }
  if ((((int)dVar8 < (int)dVar5) && (-0x320000 < (int)this->mbr_0x24)) &&
     ((this->mbr_0x8 & 0x10000) == 0)) {
    dVar6 = this->mbr_0x24 - 0x60000;
    this->mbr_0x24 = dVar6;
    uVar7 = 4;
    if ((int)(dVar6 + dVar5) < iVar4 + 2) {
      this->mbr_0x24 = (dVar8 - dVar5) + 1;
    }
  }
  FUN_0046db20_ConvertToVector(this->mbr_0xb0,this->mbr_0xb4,&iStack_c,0);
  dStack_4 = this->mbr_0xb8;
  this->mbr_0x30 = this->mbr_0x30 + dStack_4;
  this->mbr_0x2c = this->mbr_0x2c + iStack_8;
  this->mbr_0x28 = this->mbr_0x28 + iStack_c;
  this->mbr_0x30 = this->mbr_0x30 + this->mbr_0x24;
  this->mbr_0x2c = this->mbr_0x2c + this->mbr_0x20;
  dVar6 = this->mbr_0x28 + this->mbr_0x1c;
  this->mbr_0x28 = dVar6;
  dVar5 = dVar6;
  if ((int)dVar6 < 1) {
    dVar5 = -dVar6;
  }
  if (0xffff < (int)dVar5) {
    iVar4 = (int)(dVar6 + ((int)dVar6 >> 0x1f & 0xffffU)) >> 0x10;
    dStack_18 = dStack_18 + iVar4;
    this->mbr_0x28 = dVar6 + iVar4 * -0x10000;
  }
  dVar5 = this->mbr_0x2c;
  dVar6 = dVar5;
  if ((int)dVar5 < 1) {
    dVar6 = -dVar5;
  }
  if (0xffff < (int)dVar6) {
    iVar4 = (int)(dVar5 + ((int)dVar5 >> 0x1f & 0xffffU)) >> 0x10;
    dStack_14 = dStack_14 + iVar4;
    this->mbr_0x2c = dVar5 + iVar4 * -0x10000;
  }
  dVar5 = this->mbr_0x30;
  dVar6 = dVar5;
  if ((int)dVar5 < 1) {
    dVar6 = -dVar5;
  }
  if (0xffff < (int)dVar6) {
    iVar4 = (int)(dVar5 + ((int)dVar5 >> 0x1f & 0xffffU)) >> 0x10;
    dStack_10 = dStack_10 + iVar4;
    this->mbr_0x30 = dVar5 + iVar4 * -0x10000;
  }
  if ((int)dStack_10 < (int)dVar8) {
    dVar5 = this->mbr_0x24;
    dVar6 = dVar5;
    if ((int)dVar5 < 1) {
      dVar6 = -dVar5;
    }
    if ((int)dVar6 < 0x60001) {
      this->mbr_0x24 = 0;
    }
    else {
      this->mbr_0x24 = -((int)(dVar5 + ((int)dVar5 >> 0x1f & 3U)) >> 2);
    }
    this->mbr_0x30 = 0;
    dStack_10 = dVar8;
  }
  if ((this->mbr_0x4 == 0xc) && ((int)dStack_10 < (int)dVar8)) {
    dStack_10 = dVar8;
  }
  if (dStack_18 == *pdVar1) {
    if ((((dStack_14 == this->mbr_0x14) && (dStack_10 == this->mbr_0x18)) && (this->mbr_0x1c == 0))
       && ((this->mbr_0x20 == 0 && (this->mbr_0x24 == 0)))) {
      return 0;
    }
    if (((dStack_18 == *pdVar1) && (dStack_14 == this->mbr_0x14)) && (dStack_10 == this->mbr_0x18))
    {
      return uVar7 | 1;
    }
  }
  (*this->vftptr_0x0->virt_meth_0x46ed70_8)(this);
  return uVar7 | 1;
}



// Function at 00470ca0

void SoftwareRenderer::virt_meth_0x470ca0()

{
  if ((int *)this->mbr_0x58 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x58 + 0x30))();
  }
  return;
}



// Function at 00470cc0

void SoftwareRenderer::virt_meth_0x470cc0()

{
  short sVar1;
  undefined uVar2;
  undefined3 extraout_var;
  int iVar3;
  uint uVar4;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined4 uVar5;
  
  if (((this->mbr_0x8 & 0x800000) == 0) && (this->mbr_0x54 != 0)) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x477a10_180)(this);
    if (CONCAT31(extraout_var,uVar2) == 0) {
      iVar3 = (**(code **)(*(int *)this->mbr_0x54 + 0x90))(this->mbr_0xc);
      uVar4 = (**(code **)(*(int *)this->mbr_0x54 + 0x8c))(this->mbr_0xc);
    }
    else {
      iVar3 = (**(code **)(*(int *)this->mbr_0x54 + 0x98))(this->mbr_0xc);
      uVar4 = (**(code **)(*(int *)this->mbr_0x54 + 0x94))(this->mbr_0xc);
    }
    this->mbr_0x5c = this->mbr_0x5c + this->mbr_0x5e;
    sVar1 = (short)iVar3;
    if ((short)this->mbr_0x5e < 0) {
      if ((short)this->mbr_0x5c < 0) {
        uVar5 = 1;
        (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
        if ((short)this->mbr_0x5c < 0) {
          if ((uVar4 & 1) == 0) {
            if ((uVar4 & 0x80) == 0) {
              if ((int *)this->mbr_0x58 != (int *)0x0) {
                (**(code **)(*(int *)this->mbr_0x58 + 0x24))(1,uVar5);
              }
              this->mbr_0x5c = 0;
            }
            else {
              this->mbr_0x5c = 1;
              this->mbr_0x5e = 1;
            }
          }
          else {
            this->mbr_0x5c = sVar1 - 1;
          }
        }
      }
      else {
        uVar2 = (*this->vftptr_0x0->virt_meth_0x477d30_340)(this);
        if (CONCAT31(extraout_var_00,uVar2) == 1) {
          (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
        }
      }
    }
    else if (iVar3 + -1 < (int)(short)this->mbr_0x5c) {
      uVar5 = 1;
      (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
      if (iVar3 <= (short)this->mbr_0x5c) {
        if ((uVar4 & 1) == 0) {
          if ((uVar4 & 0x80) == 0) {
            if ((int *)this->mbr_0x58 != (int *)0x0) {
              (**(code **)(*(int *)this->mbr_0x58 + 0x24))(1,uVar5);
            }
            this->mbr_0x5c = sVar1 - 1;
          }
          else {
            this->mbr_0x5e = 0xffff;
            this->mbr_0x5c = sVar1 - 1;
          }
        }
        else {
          this->mbr_0x5c = 0;
        }
      }
    }
    else {
      uVar2 = (*this->vftptr_0x0->virt_meth_0x477d30_340)(this);
      if (CONCAT31(extraout_var_01,uVar2) == 1) {
        (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
      }
    }
    if ((int *)this->mbr_0x58 != (int *)0x0) {
      (**(code **)(*(int *)this->mbr_0x58 + 0x28))(0);
    }
  }
  return;
}



// Function at 00470e40

void SoftwareRenderer::virt_meth_0x470e40()

{
  undefined uVar1;
  undefined3 extraout_var;
  dword dVar2;
  cls_0x419dd0 cVar3;
  uint uVar4;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x46e930_48)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    (*this->vftptr_0x0->virt_meth_0x46e8b0_40)(this);
  }
  if (((*(byte *)&this->mbr_0x8 & 4) != 0) && (this->mbr_0x9c == 0xffffffff)) {
    iStack_4 = this->mbr_0x18 + this->mbr_0x98;
    iStack_8 = this->mbr_0x14 + this->mbr_0x94;
    iStack_c = this->mbr_0x10 + this->mbr_0x90;
    cVar3.mbr_0x0 = (dword)this->mbr_0x89;
    uVar4 = cVar3.mbr_0x0;
    cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffe0);
    dVar2 = cls_0x411eb0::meth_0x415790
                      ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)&iStack_c,cVar3.mbr_0x0,uVar4)
    ;
    this->mbr_0x9c = dVar2;
  }
  if ((this->mbr_0x4 == 0xb) || (this->mbr_0x4 == 0xc)) {
    if (this == (SoftwareRenderer *)0x0) {
      dVar2 = 0;
    }
    else {
      dVar2 = this->mbr_0x40;
    }
    cls_0x57d9d0::meth_0x583f60((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,0,0x4c,dVar2,3);
  }
  return;
}



// Function at 00470f00

void SoftwareRenderer::virt_meth_0x470f00()

{
  if (this->mbr_0x58 != 0) {
    (*this->vftptr_0x0->virt_meth_0x46e900_44)(this);
  }
  if (this->mbr_0x9c != 0xffffffff) {
    cls_0x411eb0::meth_0x415920((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
    this->mbr_0x9c = 0xffffffff;
  }
  if (this->mbr_0xa0 != 0xffffffff) {
    FUN_0041d9e0(this->mbr_0xa0);
    this->mbr_0xa0 = 0xffffffff;
  }
  return;
}



// Function at 00471260

void SoftwareRenderer::meth_0x471260()

{
  if ((this->mbr_0x84 != 0) && ((this->mbr_0x8 & 0x200000) == 0)) {
    (*this->vftptr_0x0->virt_meth_0x477d30_340)(this);
    cls_0x4922c0::meth_0x4933d0_TScript_Continue((cls_0x4922c0 *)this->mbr_0x84);
  }
  return;
}



// Function at 00471900

void SoftwareRenderer::virt_meth_0x471900(undefined4 param_1, undefined4 param_2)

{
  undefined *in_stack_0000000c;
  
  if ((int *)this->mbr_0x54 != (int *)0x0) {
    if (in_stack_0000000c == (undefined *)0x0) {
      in_stack_0000000c = PTR_DAT_005d79e0;
    }
    (**(code **)(*(int *)this->mbr_0x54 + 0x24))(this,param_1,param_2,in_stack_0000000c);
  }
  return;
}



// Function at 00471b50

void SoftwareRenderer::virt_meth_0x471b50_SetNotify()

{
  dword in_stack_00000004;
  
  this->mbr_0x80 = in_stack_00000004;
  return;
}



// Function at 00471ba0

void SoftwareRenderer::virt_meth_0x471ba0_Notify(int param_1)

{
  cls_0x4922c0 *pcVar1;
  dword dVar2;
  dword in_stack_00000008;
  
  if (param_1 == 5) {
    pcVar1 = (cls_0x4922c0 *)this->mbr_0x84;
    if ((pcVar1 != (cls_0x4922c0 *)0x0) && (pcVar1->mbr_0x8 == in_stack_00000008)) {
      pcVar1->mbr_0xc = 0;
      if (pcVar1 != (cls_0x4922c0 *)0x0) {
        cls_0x4922c0::meth_0x4922c0(pcVar1);
        FUN_004830f0(pcVar1);
      }
      DAT_006669fc = DAT_006669fc | 8;
      this->mbr_0x84 = 0;
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
  }
  else if ((((param_1 == 8) && (this->mbr_0x84 == 0)) && ((this->mbr_0x8 & 0x40000000) == 0)) &&
          ((this->mbr_0x4 != 9 || (this->mbr_0x38 != *(dword *)this->mbr_0x4c)))) {
    cls_0x497370::meth_0x497370_TScriptManager_ObjectScript((cls_0x497370 *)&DAT_0065def0);
    TScreen::meth_0x471150((TScreen *)this);
  }
  pcVar1 = (cls_0x4922c0 *)this->mbr_0x84;
  if (pcVar1 == (cls_0x4922c0 *)0x0) {
    return;
  }
  dVar2 = pcVar1->mbr_0xc4;
  if (dVar2 != 0) {
    if (param_1 == 1) {
      if (dVar2 != in_stack_00000008) goto LAB_00471c8d;
    }
    else if (((param_1 != 2) || (*(dword *)(dVar2 + 0x44) != in_stack_00000008)) ||
            ((*(uint *)(dVar2 + 8) & 0x80000) != 0)) goto LAB_00471c8d;
    pcVar1->mbr_0xc4 = 0;
    pcVar1->mbr_0xcc = 0;
    cls_0x4922c0::meth_0x492490(pcVar1);
  }
LAB_00471c8d:
  pcVar1 = (cls_0x4922c0 *)this->mbr_0x84;
  dVar2 = pcVar1->mbr_0xc8;
  if (dVar2 != 0) {
    if (param_1 == 1) {
      if (dVar2 != in_stack_00000008) {
        return;
      }
    }
    else {
      if (param_1 != 2) {
        return;
      }
      if (*(dword *)(dVar2 + 0x44) != in_stack_00000008) {
        return;
      }
      if ((*(uint *)(dVar2 + 8) & 0x80000) != 0) {
        return;
      }
    }
    pcVar1->mbr_0xc8 = 0;
    *(undefined4 *)&pcVar1->field_0xd0 = 0;
    cls_0x4922c0::meth_0x492490(pcVar1);
  }
  return;
}



// Function at 00472310

void SoftwareRenderer::virt_meth_0x472310(int *param_1, int param_2)

{
  int iVar1;
  word wVar2;
  SoftwareRenderer *pcVar3;
  SoftwareRenderer *extraout_ECX;
  SoftwareRenderer *pcVar4;
  undefined4 in_EDX;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  int in_stack_0000000c;
  
  iVar1 = param_2;
  if (2 < param_2) {
    param_2 = *(int *)param_1[1];
    pcVar4 = (SoftwareRenderer *)((int *)param_1[1] + 1);
    param_1[1] = (int)pcVar4;
    if ((param_2 < 0x801) && (0 < param_2)) {
      do {
        pcVar3 = FUN_00471ce0_CreateObjectFromStream(pcVar4,in_EDX,param_1,iVar1,in_stack_0000000c);
        pcVar4 = extraout_ECX;
        in_EDX = extraout_EDX;
        if (pcVar3 != (SoftwareRenderer *)0x0) {
          wVar2 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x68);
          pcVar3->mbr_0x7e = wVar2;
          pcVar3->mbr_0x64 = (dword)this;
          pcVar4 = this;
          in_EDX = extraout_EDX_00;
        }
        param_2 = param_2 + -1;
      } while (param_2 != 0);
    }
  }
  return;
}



// Function at 00472380

void SoftwareRenderer::virt_meth_0x472380()

{
  dword *pdVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  dword dVar3;
  cls_0x5a5ff0 *in_stack_00000004;
  cls_0x477870 cStack_1c;
  
  uVar2 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
  if (CONCAT31(extraout_var,uVar2) == 0) {
    dVar3 = this->mbr_0x68;
  }
  else {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x477d50_368)(this);
    dVar3 = TPlayScreen::meth_0x470040((TPlayScreen *)CONCAT31(extraout_var_00,uVar2));
  }
  if ((int)((in_stack_00000004->mbr_0xc + in_stack_00000004->mbr_0x4) - in_stack_00000004->mbr_0x8)
      < 4) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
  }
  pdVar1 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar1 = dVar3;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar1 + 1);
  if (0 < (int)dVar3) {
    cStack_1c.mbr_0x0 = 0;
    cStack_1c.mbr_0x10 = 0;
    cStack_1c.mbr_0xc = 0;
    cStack_1c.mbr_0x14 = 0;
    cStack_1c.mbr_0x18 = 0;
    cStack_1c.mbr_0x4 = (dword)this;
    cStack_1c.mbr_0x8 = (dword)this;
    cls_0x477870::meth_0x46dfb0(&cStack_1c);
    while ((int *)cStack_1c.mbr_0x18 != (int *)0x0) {
      FUN_00472110((int *)cStack_1c.mbr_0x18,in_stack_00000004);
      cls_0x477870::meth_0x46dfb0(&cStack_1c);
    }
  }
  return;
}



// Function at 00472430

void SoftwareRenderer::virt_meth_0x472430(int param_1)

{
  byte bVar1;
  byte bVar2;
  short sVar3;
  word wVar4;
  byte *pbVar5;
  uint *puVar6;
  uint uVar7;
  word *pwVar8;
  undefined4 uVar9;
  cls_0x4922c0 *this_00;
  int iVar10;
  undefined4 *puVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  dword dVar15;
  dword dVar16;
  int iVar17;
  uint uVar18;
  int in_stack_00000008;
  uint local_10;
  int local_c;
  
  iVar10 = param_1;
  bVar1 = **(byte **)(param_1 + 4);
  *(byte **)(param_1 + 4) = *(byte **)(param_1 + 4) + 1;
  if (bVar1 != 0) {
    puVar11 = FUN_00482ef0(bVar1 + 1);
    iVar17 = 0;
    this->mbr_0x38 = (dword)puVar11;
    if (bVar1 != 0) {
      do {
        pbVar5 = *(byte **)(param_1 + 4);
        bVar2 = *pbVar5;
        *(byte *)((int)puVar11 + iVar17) = bVar2;
        *(byte *)((int)puVar11 + iVar17) = bVar2 & 0x7f;
        iVar17 = iVar17 + 1;
        *(byte **)(param_1 + 4) = pbVar5 + 1;
      } while (iVar17 < (int)(uint)bVar1);
    }
    *(undefined *)((int)puVar11 + iVar17) = 0;
  }
  puVar6 = *(uint **)(param_1 + 4);
  uVar12 = puVar6[2];
  uVar7 = *puVar6;
  this->mbr_0x10 = puVar6[1];
  uVar18 = puVar6[3];
  this->mbr_0x14 = uVar12;
  uVar12 = this->mbr_0x8;
  this->mbr_0x18 = uVar18;
  *(uint **)(param_1 + 4) = puVar6 + 4;
  uVar12 = (uVar12 ^ uVar7) & 0x3ff1ffd7 ^ uVar12;
  this->mbr_0x8 = uVar12;
  if ((in_stack_00000008 < 6) || ((uVar12 & 1) == 0)) {
    this->mbr_0x1c = puVar6[4];
    this->mbr_0x20 = puVar6[5];
    this->mbr_0x24 = puVar6[6];
    *(uint **)(param_1 + 4) = puVar6 + 7;
  }
  pwVar8 = *(word **)(param_1 + 4);
  if (in_stack_00000008 < 9) {
    bVar1 = *(byte *)pwVar8;
    *(byte **)(param_1 + 4) = (byte *)((int)pwVar8 + 1);
    this->mbr_0xc = (ushort)bVar1;
  }
  else {
    this->mbr_0xc = *pwVar8;
    *(word **)(param_1 + 4) = pwVar8 + 1;
  }
  if ((in_stack_00000008 < 6) || ((uVar12 & 0x80000) == 0)) {
    this->mbr_0xe = 0;
  }
  else {
    pwVar8 = *(word **)(param_1 + 4);
    if (in_stack_00000008 < 9) {
      bVar1 = *(byte *)pwVar8;
      *(byte **)(param_1 + 4) = (byte *)((int)pwVar8 + 1);
      this->mbr_0xe = (ushort)bVar1;
    }
    else {
      this->mbr_0xe = *pwVar8;
      *(word **)(param_1 + 4) = pwVar8 + 1;
    }
  }
  if (in_stack_00000008 < 5) {
    *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 1;
  }
  pwVar8 = *(word **)(param_1 + 4);
  if (in_stack_00000008 < 3) {
    this->mbr_0x36 = *(byte *)pwVar8;
    dVar15 = *(dword *)((int)pwVar8 + 7);
    *(word **)(param_1 + 4) = pwVar8 + 6;
    this->mbr_0x50 = dVar15;
    this->mbr_0x7c = 0xffff;
    this->mbr_0x40 = 0xffffffff;
  }
  else {
    this->mbr_0x7c = *pwVar8;
    this->mbr_0x7e = pwVar8[1];
    this->mbr_0x50 = *(dword *)(pwVar8 + 2);
    this->mbr_0x34 = *(byte *)(pwVar8 + 4);
    this->mbr_0x35 = *(byte *)((int)pwVar8 + 9);
    this->mbr_0x36 = *(byte *)(pwVar8 + 5);
    this->mbr_0x40 = *(dword *)((int)pwVar8 + 0xb);
    *(undefined **)(param_1 + 4) = (undefined *)((int)pwVar8 + 0xf);
  }
  this->mbr_0xb0 = (uint)this->mbr_0x36;
  if (in_stack_00000008 < 5) {
    this->mbr_0x5c = 0;
    this->mbr_0x5e = 1;
    this->mbr_0x37 = 0;
    sVar3 = *(short *)(this->mbr_0x48 + 0x1c);
    if (0 < sVar3) {
      cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)&this->mbr_0xa8);
      dVar15 = this->mbr_0x48;
      *(short *)&this->mbr_0xa8 = sVar3;
      iVar17 = 0;
      if (0 < *(short *)(dVar15 + 0x1c)) {
        do {
          iVar14 = iVar17 * 4;
          iVar13 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)(dVar15 + 0x24));
          iVar17 = iVar17 + 1;
          dVar15 = this->mbr_0x48;
          *(undefined4 *)(iVar14 + this->mbr_0xac) =
               *(undefined4 *)(*(int *)(iVar13 + 0x18) + iVar14);
        } while (iVar17 < *(short *)(dVar15 + 0x1c));
      }
    }
    (*this->vftptr_0x0->virt_meth_0x477e60_452)(this);
  }
  else {
    if ((in_stack_00000008 < 6) || ((uVar12 & 0x4000) != 0)) {
      pwVar8 = *(word **)(param_1 + 4);
      wVar4 = pwVar8[1];
      this->mbr_0x5c = *pwVar8;
      this->mbr_0x5e = wVar4;
      *(word **)(param_1 + 4) = pwVar8 + 2;
    }
    dVar15 = this->mbr_0x48;
    bVar1 = **(byte **)(param_1 + 4);
    *(byte **)(param_1 + 4) = *(byte **)(param_1 + 4) + 1;
    sVar3 = *(short *)(dVar15 + 0x1c);
    this->mbr_0x37 = bVar1;
    if (0 < sVar3) {
      cls_0x4785e0::meth_0x4785e0((cls_0x4785e0 *)&this->mbr_0xa8);
      *(short *)&this->mbr_0xa8 = sVar3;
    }
    bVar1 = **(byte **)(param_1 + 4);
    *(byte **)(param_1 + 4) = *(byte **)(param_1 + 4) + 1;
    if (bVar1 != 0) {
      uVar12 = 0;
      local_10 = 0;
      local_c = 0;
      if (bVar1 != 0) {
        param_1 = 0;
        do {
          if ((int)*(short *)&this->mbr_0xa8 <= (int)uVar12) break;
          puVar11 = *(undefined4 **)(iVar10 + 4);
          uVar9 = *puVar11;
          uVar7 = puVar11[1];
          *(undefined4 **)(iVar10 + 4) = puVar11 + 2;
          dVar15 = this->mbr_0x48;
          if (uVar12 < (uint)(int)*(short *)(dVar15 + 0x1c)) {
            uVar18 = *(uint *)(*(int *)(dVar15 + 0x20) + 0x4c + param_1);
          }
          else {
            uVar18 = 0;
          }
          if ((uVar7 & 0x7f7f7f7f) == uVar18) {
            local_10 = uVar12 + 1;
            *(undefined4 *)((this->mbr_0xac - 4) + local_10 * 4) = uVar9;
            param_1 = param_1 + 0x50;
            uVar12 = local_10;
          }
          else {
            uVar18 = 0;
            if (0 < *(short *)(dVar15 + 0x1c)) {
              iVar17 = 0;
              do {
                if (uVar18 < (uint)(int)*(short *)(dVar15 + 0x1c)) {
                  uVar12 = *(uint *)(*(int *)(dVar15 + 0x20) + 0x4c + iVar17);
                }
                else {
                  uVar12 = 0;
                }
                if ((uVar7 & 0x7f7f7f7f) == uVar12) {
                  *(undefined4 *)(this->mbr_0xac + uVar18 * 4) = uVar9;
                }
                uVar18 = uVar18 + 1;
                iVar17 = iVar17 + 0x50;
                uVar12 = local_10;
              } while ((int)uVar18 < (int)*(short *)(dVar15 + 0x1c));
            }
          }
          local_c = local_c + 1;
        } while (local_c < (int)(uint)bVar1);
      }
    }
    if ((this->mbr_0x8 & 0x4000000) != 0) {
      dVar15 = this->mbr_0x48;
      iVar17 = 0;
      if (0 < *(short *)(dVar15 + 0x1c)) {
        do {
          if ((iVar17 < 3) || (5 < iVar17)) {
            iVar14 = cls_0x45f7c0::meth_0x44ce10((cls_0x45f7c0 *)(dVar15 + 0x24));
            *(undefined4 *)(this->mbr_0xac + iVar17 * 4) =
                 *(undefined4 *)(*(int *)(iVar14 + 0x18) + iVar17 * 4);
          }
          dVar15 = this->mbr_0x48;
          iVar17 = iVar17 + 1;
        } while (iVar17 < *(short *)(dVar15 + 0x1c));
      }
    }
  }
  uVar12 = this->mbr_0x8;
  if ((uVar12 & 4) != 0) {
    pbVar5 = *(byte **)(iVar10 + 4);
    this->mbr_0x88 = *pbVar5;
    this->mbr_0x90 = *(dword *)(pbVar5 + 1);
    this->mbr_0x94 = *(dword *)(pbVar5 + 5);
    this->mbr_0x98 = *(dword *)(pbVar5 + 9);
    *(byte *)((int)&this->mbr_0x8c + 2) = pbVar5[0xd];
    *(byte *)((int)&this->mbr_0x8c + 1) = pbVar5[0xe];
    *(byte *)&this->mbr_0x8c = pbVar5[0xf];
    this->mbr_0x89 = pbVar5[0x10];
    this->mbr_0x8a = *(word *)(pbVar5 + 0x11);
    *(byte **)(iVar10 + 4) = pbVar5 + 0x13;
    this->mbr_0x8 = uVar12 | 0x4004;
  }
  if ((uint)(int)(short)this->mbr_0x4 < DAT_0065a258_TObjectClass_numclasses) {
    dVar15 = (&DAT_0065a148_TObjectClass_classes)[(short)this->mbr_0x4];
  }
  else {
    dVar15 = 0;
  }
  this->mbr_0x48 = dVar15;
  if (dVar15 == 0) {
    FUN_00481c10((byte *)s_Bad_object_class__005d4a0c);
  }
  dVar15 = this->mbr_0x48;
  uVar12 = (uint)(short)this->mbr_0x6;
  if (((*(int *)(dVar15 + 0x34) == 0) || (*(uint *)(dVar15 + 0x24) <= uVar12)) ||
     (*(int *)(*(int *)(dVar15 + 0x34) + uVar12 * 4) == 0)) {
    dVar16 = 0;
  }
  else {
    dVar16 = *(dword *)(*(int *)(dVar15 + 0x34) + uVar12 * 4);
    if (dVar16 == 0) {
      dVar16 = *(dword *)(dVar15 + 0x38);
    }
  }
  this_00 = (cls_0x4922c0 *)this->mbr_0x84;
  this->mbr_0x4c = dVar16;
  if (this_00 != (cls_0x4922c0 *)0x0) {
    this_00->mbr_0xc = 0;
    cls_0x4922c0::meth_0x4922c0(this_00);
    FUN_004830f0(this_00);
    this->mbr_0x84 = 0;
  }
  if (this == DAT_0065d674) {
    UNK_0065d53c._12_4_ = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
  }
  if (this == DAT_0065b088) {
    DAT_0065b02c._76_4_ = 1;
  }
  if (in_stack_00000008 < 0xc) {
    this->mbr_0x8 = this->mbr_0x8 & 0x3fffdff;
  }
  uVar12 = this->mbr_0x8;
  this->mbr_0x8 = uVar12 & 0xf7ffffff;
  if ((this->mbr_0x4 == 9) && ((uVar12 & 4) != 0)) {
    this->mbr_0x8 = uVar12 & 0xf7ff7fff;
  }
  if ((this->mbr_0x8 & 0x40000000) != 0) {
    this->mbr_0x8 = this->mbr_0x8 | 0xc000;
  }
  if (((this->mbr_0x8 & 0x40000000) == 0) &&
     ((this->mbr_0x4 != 9 || (this->mbr_0x38 != *(dword *)this->mbr_0x4c)))) {
    cls_0x497370::meth_0x497370_TScriptManager_ObjectScript((cls_0x497370 *)&DAT_0065def0);
    TScreen::meth_0x471150((TScreen *)this);
  }
  return;
}



// Function at 00472980

void SoftwareRenderer::virt_meth_0x472980()

{
  byte bVar1;
  undefined uVar2;
  word wVar3;
  undefined *puVar4;
  dword *pdVar5;
  word *pwVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  uint *puVar9;
  byte *pbVar10;
  int iVar11;
  dword dVar12;
  uint uVar13;
  cls_0x5a5ff0 *pcVar14;
  byte bVar15;
  byte *pbVar16;
  bool bVar17;
  cls_0x5a5ff0 *in_stack_00000004;
  
  if ((this->mbr_0x8 & 4) != 0) {
    this->mbr_0x8 = this->mbr_0x8 | 0xc000;
  }
  pbVar10 = *(byte **)this->mbr_0x4c;
  pbVar16 = (byte *)this->mbr_0x38;
  do {
    bVar15 = *pbVar10;
    bVar17 = bVar15 < *pbVar16;
    if (bVar15 != *pbVar16) {
LAB_004729c6:
      iVar11 = (1 - (uint)bVar17) - (uint)(bVar17 != 0);
      goto LAB_004729cb;
    }
    if (bVar15 == 0) break;
    bVar15 = pbVar10[1];
    bVar17 = bVar15 < pbVar16[1];
    if (bVar15 != pbVar16[1]) goto LAB_004729c6;
    pbVar10 = pbVar10 + 2;
    pbVar16 = pbVar16 + 2;
  } while (bVar15 != 0);
  iVar11 = 0;
LAB_004729cb:
  if (iVar11 == 0) {
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    puVar4 = (undefined *)in_stack_00000004->mbr_0x8;
    *puVar4 = 0;
    in_stack_00000004->mbr_0x8 = (dword)(puVar4 + 1);
  }
  else {
    iVar11 = -1;
    pbVar10 = (byte *)this->mbr_0x38;
    do {
      if (iVar11 == 0) break;
      iVar11 = iVar11 + -1;
      bVar15 = *pbVar10;
      pbVar10 = pbVar10 + 1;
    } while (bVar15 != 0);
    bVar15 = ~(byte)iVar11 - 1;
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
    iVar11 = 0;
    *pbVar10 = bVar15;
    in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
    if (bVar15 != 0) {
      do {
        bVar1 = *(byte *)(this->mbr_0x38 + iVar11);
        cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
        pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
        *pbVar10 = bVar1 | 0x80;
        in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
        iVar11 = iVar11 + 1;
      } while (iVar11 < (int)(uint)bVar15);
    }
  }
  dVar12 = this->mbr_0x8;
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar5 = dVar12;
  dVar12 = this->mbr_0x10;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar5 = dVar12;
  dVar12 = this->mbr_0x14;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar5 = dVar12;
  dVar12 = this->mbr_0x18;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar5 = dVar12;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  if ((*(byte *)&this->mbr_0x8 & 1) == 0) {
    dVar12 = this->mbr_0x1c;
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    dVar12 = this->mbr_0x20;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    dVar12 = this->mbr_0x24;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  }
  wVar3 = this->mbr_0xc;
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pwVar6 = (word *)in_stack_00000004->mbr_0x8;
  *pwVar6 = wVar3;
  in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
  if ((this->mbr_0x8 & 0x80000) != 0) {
    wVar3 = this->mbr_0xe;
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pwVar6 = (word *)in_stack_00000004->mbr_0x8;
    *pwVar6 = wVar3;
    in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
  }
  wVar3 = this->mbr_0x7c;
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pwVar6 = (word *)in_stack_00000004->mbr_0x8;
  *pwVar6 = wVar3;
  wVar3 = this->mbr_0x7e;
  in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pwVar6 = (word *)in_stack_00000004->mbr_0x8;
  *pwVar6 = wVar3;
  dVar12 = this->mbr_0x50;
  in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  bVar15 = this->mbr_0x34;
  *pdVar5 = dVar12;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
  *pbVar10 = bVar15;
  bVar15 = this->mbr_0x35;
  in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
  *pbVar10 = bVar15;
  bVar15 = this->mbr_0x36;
  in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
  dVar12 = this->mbr_0x40;
  *pbVar10 = bVar15;
  in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar5 = dVar12;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
  if ((this->mbr_0x8 & 0x4000) != 0) {
    wVar3 = this->mbr_0x5c;
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pwVar6 = (word *)in_stack_00000004->mbr_0x8;
    *pwVar6 = wVar3;
    wVar3 = this->mbr_0x5e;
    in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pwVar6 = (word *)in_stack_00000004->mbr_0x8;
    *pwVar6 = wVar3;
    in_stack_00000004->mbr_0x8 = (dword)(pwVar6 + 1);
  }
  bVar15 = this->mbr_0x37;
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
  dVar12 = this->mbr_0x48;
  *pbVar10 = bVar15;
  uVar2 = *(undefined *)(dVar12 + 0x1c);
  in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
  cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
  puVar4 = (undefined *)in_stack_00000004->mbr_0x8;
  iVar11 = 0;
  *puVar4 = uVar2;
  in_stack_00000004->mbr_0x8 = (dword)(puVar4 + 1);
  dVar12 = this->mbr_0x48;
  if (0 < *(short *)(dVar12 + 0x1c)) {
    do {
      uVar13 = cls_0x477d10::meth_0x477d10((cls_0x477d10 *)(dVar12 + 0x1c));
      uVar7 = *(undefined4 *)(this->mbr_0xac + iVar11 * 4);
      cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
      puVar8 = (undefined4 *)in_stack_00000004->mbr_0x8;
      *puVar8 = uVar7;
      in_stack_00000004->mbr_0x8 = (dword)(puVar8 + 1);
      cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
      puVar9 = (uint *)in_stack_00000004->mbr_0x8;
      *puVar9 = uVar13 | 0x80808080;
      in_stack_00000004->mbr_0x8 = (dword)(puVar9 + 1);
      dVar12 = this->mbr_0x48;
      iVar11 = iVar11 + 1;
    } while (iVar11 < *(short *)(dVar12 + 0x1c));
  }
  if ((*(byte *)&this->mbr_0x8 & 4) != 0) {
    bVar15 = this->mbr_0x88;
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pbVar10 = (byte *)in_stack_00000004->mbr_0x8;
    dVar12 = this->mbr_0x90;
    *pbVar10 = bVar15;
    in_stack_00000004->mbr_0x8 = (dword)(pbVar10 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    dVar12 = this->mbr_0x94;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    dVar12 = this->mbr_0x98;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
    cls_0x5a5ff0::meth_0x4779d0(in_stack_00000004);
    pdVar5 = (dword *)in_stack_00000004->mbr_0x8;
    *pdVar5 = dVar12;
    in_stack_00000004->mbr_0x8 = (dword)(pdVar5 + 1);
    pcVar14 = (cls_0x5a5ff0 *)cls_0x5a5ff0::meth_0x477cd0(in_stack_00000004);
    pcVar14 = (cls_0x5a5ff0 *)cls_0x5a5ff0::meth_0x477cd0(pcVar14);
    pcVar14 = (cls_0x5a5ff0 *)cls_0x5a5ff0::meth_0x477cd0(pcVar14);
    pcVar14 = (cls_0x5a5ff0 *)cls_0x5a5ff0::meth_0x477cd0(pcVar14);
    cls_0x5a5ff0::meth_0x477c90(pcVar14);
  }
  return;
}



// Function at 00472e90

void SoftwareRenderer::virt_meth_0x472e90_SetFlags()

{
  uint uVar1;
  uint uVar2;
  uint in_stack_00000004;
  
  if (((in_stack_00000004 & 0x20000000) == 0) || ((this->mbr_0x8 & 0x20000000) != 0)) {
    if (((in_stack_00000004 & 0x200) != 0) && ((this->mbr_0x8 & 0x200) == 0)) {
      in_stack_00000004 = in_stack_00000004 & 0xdfffffff;
    }
  }
  else {
    in_stack_00000004 = in_stack_00000004 & 0xfffffdff;
  }
  uVar1 = this->mbr_0x8;
  this->mbr_0x8 = in_stack_00000004;
  if (this->mbr_0x44 != 0) {
    FUN_00451a50((int)this,uVar1,in_stack_00000004);
  }
  if ((this->mbr_0x44 != 0) &&
     ((((uVar1 & 0x400000) == 0 && ((this->mbr_0x8 & 0x400000) != 0)) ||
      (((uVar1 & 0x400000) != 0 && ((this->mbr_0x8 & 0x400000) == 0)))))) {
    TCharacter::meth_0x452750((TCharacter *)&DAT_006668d8,this);
  }
  if (((uVar1 & 4) == 0) && ((*(byte *)&this->mbr_0x8 & 4) != 0)) {
    (*this->vftptr_0x0->virt_meth_0x470e40_292)(this);
  }
  else if (((uVar1 & 4) != 0) && ((*(byte *)&this->mbr_0x8 & 4) == 0)) {
    (*this->vftptr_0x0->virt_meth_0x470f00_296)(this);
  }
  if ((DAT_0066829c != 0) && (uVar2 = this->mbr_0x8 & 0x2800080, (uVar1 & 0x2800080) != uVar2)) {
    cls_0x57d9d0::meth_0x583fe0((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this,0x46,uVar2,0);
  }
  return;
}



// Function at 00472f80

undefined4 SoftwareRenderer::virt_meth_0x472f80_GetProperty(byte *param_1, char *param_2)

{
  char cVar1;
  undefined uVar2;
  uint uVar3;
  char *pcVar4;
  undefined3 extraout_var;
  dword dVar5;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint uVar6;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  SoftwareRenderer *pcVar7;
  SoftwareRenderer *extraout_ECX_05;
  undefined4 unaff_EBX;
  int iVar8;
  int iVar9;
  byte *pbVar10;
  int iVar11;
  undefined4 *puVar12;
  short *psVar13;
  char *pcVar14;
  undefined4 *puVar15;
  int in_stack_0000000c;
  undefined4 local_2b4;
  uint local_2b0;
  SoftwareRenderer *local_2ac;
  undefined local_2a8 [64];
  char acStack_268 [4];
  undefined local_264 [100];
  undefined4 local_200;
  undefined2 local_1fc;
  undefined4 local_1fa [126];
  
  iVar11 = 0;
  local_2b4 = 0;
  *param_2 = '\0';
  local_2ac = this;
  uVar3 = FUN_0059a530_stricmp((uint)this,param_1,&DAT_005d4b5c);
  if (uVar3 == 0) {
    iVar8 = 0;
    pcVar4 = *(char **)this->mbr_0x4c;
    if (pcVar4 != (char *)0x0) {
      uVar3 = 0xffffffff;
      pcVar14 = pcVar4;
      do {
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        cVar1 = *pcVar14;
        pcVar14 = pcVar14 + 1;
      } while (cVar1 != '\0');
      if (0 < (int)(~uVar3 - 1)) {
        do {
          cVar1 = pcVar4[iVar11];
          if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
             (('/' < cVar1 && (cVar1 < ':')))) {
            *(char *)((int)&local_200 + iVar8) = cVar1;
            iVar8 = iVar8 + 1;
          }
          uVar3 = 0xffffffff;
          iVar11 = iVar11 + 1;
          pcVar14 = pcVar4;
          do {
            if (uVar3 == 0) break;
            uVar3 = uVar3 - 1;
            cVar1 = *pcVar14;
            pcVar14 = pcVar14 + 1;
          } while (cVar1 != '\0');
        } while (iVar11 < (int)(~uVar3 - 1));
      }
      *(char *)((int)&local_200 + iVar8) = '\0';
      pcVar4 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if (*pcVar4 == '[') {
        pcVar4 = *(char **)this->mbr_0x4c;
      }
    }
  }
  else {
    uVar3 = FUN_0059a530_stricmp(extraout_ECX,param_1,(byte *)s_objtype_005d4b64);
    if (uVar3 != 0) {
      uVar3 = FUN_0059a530_stricmp(extraout_ECX_00,param_1,(byte *)s_objclass_005d4b6c);
      if (uVar3 != 0) {
        local_2b0 = FUN_0059a530_stricmp(extraout_ECX_01,param_1,(byte *)s_statmod_005d4b80);
        pcVar7 = local_2ac;
        if (local_2b0 == 0) {
          iVar8 = 0;
          local_2a8._0_4_ = local_2b0;
          iVar11 = (**(code **)(DAT_00667fcc->mbr_0x0 + 0xec))
                             ((int)(short)local_2ac->mbr_0x6,(int)(short)local_2ac->mbr_0x4,0,
                              local_264,&local_2b0,local_2a8);
          if (iVar11 != 0) {
            do {
              if (&stack0x00000000 != (undefined *)0x268) {
                uVar3 = 0xffffffff;
                pcVar4 = acStack_268;
                do {
                  pcVar14 = pcVar4;
                  if (uVar3 == 0) break;
                  uVar3 = uVar3 - 1;
                  pcVar14 = pcVar4 + 1;
                  cVar1 = *pcVar4;
                  pcVar4 = pcVar14;
                } while (cVar1 != '\0');
                uVar3 = ~uVar3;
                puVar12 = (undefined4 *)(pcVar14 + -uVar3);
                puVar15 = (undefined4 *)local_2a8;
                for (uVar6 = uVar3 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
                  *puVar15 = *puVar12;
                  puVar12 = puVar12 + 1;
                  puVar15 = puVar15 + 1;
                }
                for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
                  *(undefined *)puVar15 = *(undefined *)puVar12;
                  puVar12 = (undefined4 *)((int)puVar12 + 1);
                  puVar15 = (undefined4 *)((int)puVar15 + 1);
                }
                FUN_0059bd3e((uint *)(local_2a8 + 1));
                if (local_2ac == (SoftwareRenderer *)0x0) {
                  pcVar4 = s__s_s__d_005d4b88;
                }
                else if (local_2ac == (SoftwareRenderer *)0x1) {
                  pcVar4 = s__s_s__d___005d4b94;
                }
                else if (local_2ac == (SoftwareRenderer *)0x2) {
                  pcVar4 = s__s_s___d__005d4ba0;
                }
                else {
                  pcVar4 = s__s_s__d_005d4bac;
                }
                FUN_0058b100((char *)param_1,(byte *)pcVar4);
              }
              iVar8 = iVar8 + 1;
              iVar11 = (**(code **)(DAT_00667fcc->mbr_0x0 + 0xec))
                                 ((int)(short)pcVar7->mbr_0x6,(int)(short)pcVar7->mbr_0x4,iVar8,
                                  acStack_268,&local_2b4,&local_2ac);
            } while (iVar11 != 0);
            if (iVar8 != 0) {
              return 1;
            }
          }
        }
        else {
          uVar3 = FUN_0059a530_stricmp(extraout_ECX_02,param_1,(byte *)s_experience_005d4bb8);
          if (uVar3 == 0) {
            uVar3 = 0;
            if ((this->mbr_0x4 == 0xc) && (DAT_00667fcc != (cls_0x51a5b0 *)0x0)) {
              (*this->vftptr_0x0->virt_meth_0x473600_212)(this);
              uVar3 = cls_0x51a5b0::meth_0x51a5b0(DAT_00667fcc);
            }
            FUN_0058d252(uVar3,param_2,10);
            return 1;
          }
          pbVar10 = *(byte **)(this->mbr_0x48 + 0x20);
          psVar13 = (short *)(this->mbr_0x48 + 0x1c);
          uVar6 = 0;
          uVar3 = extraout_ECX_03;
          while( true ) {
            if ((psVar13 == (short *)0x0) || ((uint)(int)*psVar13 <= uVar6)) goto LAB_004733dc;
            uVar3 = FUN_0059a530_stricmp(uVar3,pbVar10,param_1);
            if (uVar3 == 0) break;
            pbVar10 = pbVar10 + 0x50;
            uVar6 = uVar6 + 1;
            uVar3 = extraout_ECX_04;
          }
          if (-1 < (int)uVar6) {
            if (local_2ac->mbr_0x4 == 0xb) {
              uVar2 = (*local_2ac->vftptr_0x0->virt_meth_0x477aa0_220)(local_2ac);
              uVar3 = CONCAT31(extraout_var,uVar2);
              goto LAB_00473433;
            }
            dVar5 = local_2ac->mbr_0xac;
            goto LAB_00473430;
          }
LAB_004733dc:
          psVar13 = (short *)(local_2ac->mbr_0x48 + 0x14);
          uVar6 = 0;
          pbVar10 = *(byte **)(local_2ac->mbr_0x48 + 0x18);
          pcVar7 = local_2ac;
          while( true ) {
            if (psVar13 == (short *)0x0) {
              return unaff_EBX;
            }
            if ((uint)(int)*psVar13 <= uVar6) {
              return unaff_EBX;
            }
            uVar3 = FUN_0059a530_stricmp((uint)pcVar7,pbVar10,param_1);
            if (uVar3 == 0) break;
            pbVar10 = pbVar10 + 0x50;
            uVar6 = uVar6 + 1;
            pcVar7 = extraout_ECX_05;
          }
          if (-1 < (int)uVar6) {
            iVar11 = *(int *)(*(int *)(local_2ac->mbr_0x48 + 0x34) + (short)local_2ac->mbr_0x6 * 4);
            if (iVar11 == 0) {
              iVar11 = *(int *)(local_2ac->mbr_0x48 + 0x38);
            }
            dVar5 = *(dword *)(iVar11 + 0x10);
LAB_00473430:
            uVar3 = *(uint *)(dVar5 + uVar6 * 4);
LAB_00473433:
            unaff_EBX = 1;
            FUN_0058d252(uVar3,(char *)param_1,10);
          }
        }
        return unaff_EBX;
      }
      local_1fc._0_1_ = s_CLASS_005d4b78[4];
      local_1fc._1_1_ = s_CLASS_005d4b78[5];
      local_200._0_1_ = s_CLASS_005d4b78[0];
      local_200._1_1_ = s_CLASS_005d4b78[1];
      local_200._2_1_ = s_CLASS_005d4b78[2];
      local_200._3_1_ = s_CLASS_005d4b78[3];
      iVar9 = 5;
      puVar12 = local_1fa;
      for (iVar8 = 0x7e; iVar8 != 0; iVar8 = iVar8 + -1) {
        *puVar12 = 0;
        puVar12 = puVar12 + 1;
      }
      dVar5 = this->mbr_0x48;
      *(undefined2 *)puVar12 = 0;
      pcVar4 = *(char **)(dVar5 + 4);
      if (pcVar4 != (char *)0x0) {
        uVar3 = 0xffffffff;
        pcVar14 = pcVar4;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar14 + 1;
        } while (cVar1 != '\0');
        if (0 < (int)(~uVar3 - 1)) {
          do {
            cVar1 = pcVar4[iVar11];
            if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
               (('/' < cVar1 && (cVar1 < ':')))) {
              *(char *)((int)&local_200 + iVar9) = cVar1;
              iVar9 = iVar9 + 1;
            }
            uVar3 = 0xffffffff;
            iVar11 = iVar11 + 1;
            pcVar14 = pcVar4;
            do {
              if (uVar3 == 0) break;
              uVar3 = uVar3 - 1;
              cVar1 = *pcVar14;
              pcVar14 = pcVar14 + 1;
            } while (cVar1 != '\0');
          } while (iVar11 < (int)(~uVar3 - 1));
        }
        *(char *)((int)&local_200 + iVar9) = '\0';
        pcVar4 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        if (*pcVar4 == '[') {
          pcVar4 = *(char **)(this->mbr_0x48 + 4);
        }
      }
      _strncpy(param_2,pcVar4,in_stack_0000000c - 1);
      param_2[in_stack_0000000c + -1] = '\0';
      FUN_0059bd3e((uint *)(param_2 + 1));
      return 1;
    }
    iVar8 = 0;
    pcVar4 = *(char **)this->mbr_0x4c;
    if (pcVar4 != (char *)0x0) {
      uVar3 = 0xffffffff;
      pcVar14 = pcVar4;
      do {
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        cVar1 = *pcVar14;
        pcVar14 = pcVar14 + 1;
      } while (cVar1 != '\0');
      if (0 < (int)(~uVar3 - 1)) {
        do {
          cVar1 = pcVar4[iVar11];
          if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
             (('/' < cVar1 && (cVar1 < ':')))) {
            *(char *)((int)&local_200 + iVar8) = cVar1;
            iVar8 = iVar8 + 1;
          }
          uVar3 = 0xffffffff;
          iVar11 = iVar11 + 1;
          pcVar14 = pcVar4;
          do {
            if (uVar3 == 0) break;
            uVar3 = uVar3 - 1;
            cVar1 = *pcVar14;
            pcVar14 = pcVar14 + 1;
          } while (cVar1 != '\0');
        } while (iVar11 < (int)(~uVar3 - 1));
      }
      *(char *)((int)&local_200 + iVar8) = '\0';
      pcVar4 = (char *)cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if (*pcVar4 == '[') {
        pcVar4 = *(char **)this->mbr_0x4c;
      }
    }
  }
  _strncpy(param_2,pcVar4,in_stack_0000000c - 1);
  param_2[in_stack_0000000c + -1] = '\0';
  return 1;
}



// Function at 00473460

undefined4 SoftwareRenderer::virt_meth_0x473460(byte *param_1)

{
  undefined *puVar1;
  SoftwareRenderer__vftable_5b8e94 *pcVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined *extraout_ECX;
  int iVar6;
  byte *pbVar7;
  short *psVar8;
  byte *in_stack_00000008;
  cls_0x4782d0 local_c;
  
  uVar3 = FUN_0059a530_stricmp((uint)this,param_1,&DAT_005d4bcc);
  if (uVar3 == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e6f0((cls_0x5b4f30_TPlayer *)this);
    (*this->vftptr_0x0->virt_meth_0x4735c0_208)(this);
    return 1;
  }
  cls_0x4782d0::meth_0x4785c0(&local_c);
  while( true ) {
    if (((short *)local_c.mbr_0x0 == (short *)0x0) ||
       ((uint)(int)*(short *)local_c.mbr_0x0 <= local_c.mbr_0x8)) goto LAB_00473528;
    uVar3 = FUN_0059a530_stricmp((int)*(short *)local_c.mbr_0x0,(byte *)local_c.mbr_0x4,param_1);
    if (uVar3 == 0) break;
    local_c.mbr_0x4 = local_c.mbr_0x4 + 0x50;
    local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
  }
  if (-1 < (int)local_c.mbr_0x8) {
    puVar1 = (undefined *)this->mbr_0xac;
    iVar6 = local_c.mbr_0x8 * 4;
    uVar4 = FUN_0058b42c(puVar1,in_stack_00000008);
    *(undefined4 *)(puVar1 + iVar6) = uVar4;
    (*this->vftptr_0x0->virt_meth_0x4735c0_208)(this);
    return 1;
  }
LAB_00473528:
  psVar8 = (short *)(this->mbr_0x48 + 0x14);
  uVar3 = 0;
  pbVar7 = *(byte **)(this->mbr_0x48 + 0x18);
  while( true ) {
    if (psVar8 == (short *)0x0) {
      return 0;
    }
    uVar5 = (uint)*psVar8;
    if (uVar5 <= uVar3) break;
    uVar5 = FUN_0059a530_stricmp(uVar5,pbVar7,param_1);
    if (uVar5 == 0) {
      if ((int)uVar3 < 0) {
        return 0;
      }
      uVar4 = FUN_0058b42c(extraout_ECX,in_stack_00000008);
      iVar6 = *(int *)(*(int *)(this->mbr_0x48 + 0x34) + (short)this->mbr_0x6 * 4);
      if (iVar6 == 0) {
        iVar6 = *(int *)(this->mbr_0x48 + 0x38);
      }
      pcVar2 = this->vftptr_0x0;
      *(undefined4 *)(*(int *)(iVar6 + 0x10) + uVar3 * 4) = uVar4;
      (*pcVar2->virt_meth_0x4735c0_208)(this);
      return 1;
    }
    pbVar7 = pbVar7 + 0x50;
    uVar3 = uVar3 + 1;
  }
  return 0;
}



// Function at 00473600

uint SoftwareRenderer::virt_meth_0x473600_GetStat()

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  short *psVar5;
  byte *pbVar6;
  byte *in_stack_00000004;
  cls_0x4782d0 local_c;
  
  cls_0x4782d0::meth_0x4785c0(&local_c);
  while( true ) {
    if (((short *)local_c.mbr_0x0 == (short *)0x0) ||
       ((uint)(int)*(short *)local_c.mbr_0x0 <= local_c.mbr_0x8)) goto LAB_00473674;
    uVar3 = FUN_0059a530_stricmp((int)*(short *)local_c.mbr_0x0,(byte *)local_c.mbr_0x4,in_stack_00000004);
    if (uVar3 == 0) break;
    local_c.mbr_0x4 = local_c.mbr_0x4 + 0x50;
    local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
  }
  if (-1 < (int)local_c.mbr_0x8) {
    bVar2 = (*this->vftptr_0x0->virt_meth_0x477aa0_220)(this);
    return (uint)bVar2;
  }
LAB_00473674:
  psVar5 = (short *)(this->mbr_0x48 + 0x14);
  uVar3 = 0;
  pbVar6 = *(byte **)(this->mbr_0x48 + 0x18);
  while( true ) {
    if (psVar5 == (short *)0x0) {
      return 0;
    }
    if ((uint)(int)*psVar5 <= uVar3) {
      return 0;
    }
    uVar4 = FUN_0059a530_stricmp((uint)in_stack_00000004,pbVar6,in_stack_00000004);
    if (uVar4 == 0) break;
    pbVar6 = pbVar6 + 0x50;
    uVar3 = uVar3 + 1;
  }
  if ((int)uVar3 < 0) {
    return 0;
  }
  iVar1 = *(int *)(*(int *)(this->mbr_0x48 + 0x34) + (short)this->mbr_0x6 * 4);
  if (iVar1 == 0) {
    return *(uint *)(*(int *)(*(int *)(this->mbr_0x48 + 0x38) + 0x10) + uVar3 * 4);
  }
  return *(uint *)(*(int *)(iVar1 + 0x10) + uVar3 * 4);
}



// Function at 004736f0

void SoftwareRenderer::virt_meth_0x4736f0(byte *param_1)

{
  dword dVar1;
  undefined uVar2;
  int iVar3;
  uint uVar4;
  undefined3 extraout_var;
  int iVar5;
  int in_stack_00000008;
  cls_0x4782d0 local_c;
  
  cls_0x4782d0::meth_0x4785c0(&local_c);
  iVar3 = cls_0x4782d0::meth_0x478300(&local_c);
  do {
    if (iVar3 == 0) {
LAB_00473751:
      cls_0x4782d0::meth_0x4785c0(&local_c);
      iVar3 = cls_0x4782d0::meth_0x478300(&local_c);
      if (iVar3 != 0) {
        while (uVar4 = FUN_0059a530_stricmp(local_c.mbr_0x4,(byte *)local_c.mbr_0x4,param_1), uVar4 != 0) {
          local_c.mbr_0x4 = local_c.mbr_0x4 + 0x50;
          local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
          iVar3 = cls_0x4782d0::meth_0x478300(&local_c);
          if (iVar3 == 0) {
            return;
          }
        }
        if (-1 < (int)local_c.mbr_0x8) {
          dVar1 = this->mbr_0x48;
          iVar3 = *(int *)(*(int *)(dVar1 + 0x34) + (short)this->mbr_0x6 * 4);
          if (iVar3 == 0) {
            iVar3 = *(int *)(dVar1 + 0x38);
          }
          iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + local_c.mbr_0x8 * 4);
          iVar5 = *(int *)(*(int *)(dVar1 + 0x34) + (short)this->mbr_0x6 * 4);
          if (iVar5 == 0) {
            iVar5 = *(int *)(dVar1 + 0x38);
          }
          *(int *)(*(int *)(iVar5 + 0x10) + local_c.mbr_0x8 * 4) = in_stack_00000008;
          if (iVar3 != in_stack_00000008) {
LAB_00473856:
            (*this->vftptr_0x0->virt_meth_0x4735c0_208)(this);
          }
        }
      }
      return;
    }
    uVar4 = FUN_0059a530_stricmp(local_c.mbr_0x4,(byte *)local_c.mbr_0x4,param_1);
    if (uVar4 == 0) {
      if (-1 < (int)local_c.mbr_0x8) {
        uVar2 = (*this->vftptr_0x0->virt_meth_0x477aa0_220)(this);
        (*this->vftptr_0x0->virt_meth_0x477b00_232)(this);
        if ((byte *)CONCAT31(extraout_var,uVar2) == param_1) {
          return;
        }
        goto LAB_00473856;
      }
      goto LAB_00473751;
    }
    local_c.mbr_0x4 = local_c.mbr_0x4 + 0x50;
    local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
    iVar3 = cls_0x4782d0::meth_0x478300(&local_c);
  } while( true );
}



// Function at 004778c0

void SoftwareRenderer::virt_meth_0x4778c0_SetStateByName()

{
  SoftwareRenderer__vftable_5b8e94 *pcVar1;
  
  pcVar1 = this->vftptr_0x0;
  (*pcVar1->virt_meth_0x477c10_312)(this);
  (*pcVar1->virt_meth_0x46f250_24)(this);
  return;
}



// Function at 00477990

undefined4 SoftwareRenderer::virt_meth_0x477990()

{
  undefined uVar1;
  undefined3 extraout_var;
  int in_stack_00000004;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x4779c0_128)(this);
  if (in_stack_00000004 < CONCAT31(extraout_var,uVar1)) {
    return *(undefined4 *)(this->mbr_0x78 + in_stack_00000004 * 4);
  }
  return 0;
}



// Function at 00477a60

undefined4 SoftwareRenderer::virt_meth_0x477a60()

{
  int iVar1;
  int in_stack_00000004;
  
  iVar1 = *(int *)(*(int *)(this->mbr_0x48 + 0x34) + (short)this->mbr_0x6 * 4);
  if (iVar1 != 0) {
    return *(undefined4 *)(*(int *)(iVar1 + 0x10) + in_stack_00000004 * 4);
  }
  return *(undefined4 *)(*(int *)(*(int *)(this->mbr_0x48 + 0x38) + 0x10) + in_stack_00000004 * 4);
}



// Function at 00477aa0

undefined4 SoftwareRenderer::virt_meth_0x477aa0()

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < (uint)(int)*(short *)&this->mbr_0xa8) {
    return *(undefined4 *)(this->mbr_0xac + in_stack_00000004 * 4);
  }
  return 0;
}



// Function at 004bf730

void SoftwareRenderer::virt_meth_0x4bf730()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  int iVar3;
  int iVar4;
  
  virt_meth_0x46faf0_RemoveFromInventory(this);
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477900_36)(this);
  if ((CONCAT31(extraout_var,uVar1) != 0) && (this->mbr_0x64 == DAT_0065d674)) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    iVar2 = CONCAT31(extraout_var_00,uVar1);
    if ((short)this->mbr_0x6 < 0x10) {
      if (0x1e < iVar2) {
        iVar2 = 0x1f;
      }
      iVar3 = ((short)this->mbr_0x6 * 0x20 + iVar2) * 4;
      if (((*(int *)(&UNK_0066b15c.field_0x1040 + iVar3) != 0) &&
          (iVar4 = *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) + -1,
          *(int *)(&UNK_0066b15c.field_0x1040 + iVar3) = iVar4, iVar4 < 1)) &&
         (iVar2 * 0x80 != -0x66b99c)) {
        FUN_004830f0(*(LPCVOID *)(&UNK_0066b15c.field_0x840 + iVar3));
        *(undefined4 *)(&UNK_0066b15c.field_0x840 + iVar3) = 0;
      }
    }
  }
  return;
}



// Function at 004bfcb0

void SoftwareRenderer::virt_meth_0x4bfcb0()

{
  undefined uVar1;
  uint uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int unaff_ESI;
  int iVar3;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined4 in_stack_0000000c;
  undefined *in_stack_00000010;
  
  uVar2 = FUN_0059a530_stricmp((uint)this,(byte *)this->mbr_0x38,(byte *)s_Arrow_005df380);
  if (uVar2 == 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    if (CONCAT31(extraout_var,uVar1) < 0x20) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      iVar3 = CONCAT31(extraout_var_00,uVar1);
    }
    else {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      uVar2 = CONCAT31(extraout_var_01,uVar1) - 0x10U & 0x8000000f;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffff0) + 1;
      }
      iVar3 = uVar2 + 0x10;
    }
  }
  else {
    iVar3 = 1;
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477900_36)(this);
  if (CONCAT31(extraout_var_02,uVar1) != 0) {
    if (0xf < (short)this->mbr_0x6) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    if ((*(int *)(&UNK_0066b15c.field_0x1040 + ((short)this->mbr_0x6 * 0x20 + iVar3) * 4) < 1) &&
       ((int *)this->mbr_0x54 != (int *)0x0)) {
      FUN_004bf990((int *)this->mbr_0x54,(uint)this->mbr_0xc,(int)(short)this->mbr_0x6,iVar3,
                   unaff_ESI);
    }
    if (in_stack_00000010 == (undefined *)0x0) {
      in_stack_00000010 = PTR_DAT_005d79e0;
    }
    (**(code **)(*(int *)this->mbr_0x54 + 0x24))
              (this,in_stack_00000008,in_stack_0000000c,in_stack_00000010);
    return;
  }
  virt_meth_0x471900(this,in_stack_00000004,in_stack_00000008);
  return;
}



// Function at 004bfda0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int * SoftwareRenderer::virt_meth_0x4bfda0()

{
  undefined uVar1;
  uint uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  int *piVar3;
  int *piVar4;
  int iVar5;
  int unaff_ESI;
  int iVar6;
  
  uVar2 = FUN_0059a530_stricmp((uint)this,(byte *)this->mbr_0x38,(byte *)s_Arrow_005df388);
  if (uVar2 == 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    if (CONCAT31(extraout_var,uVar1) < 0x20) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      iVar6 = CONCAT31(extraout_var_00,uVar1);
    }
    else {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      uVar2 = CONCAT31(extraout_var_01,uVar1) - 0x10U & 0x8000000f;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffff0) + 1;
      }
      iVar6 = uVar2 + 0x10;
    }
  }
  else {
    iVar6 = 1;
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477900_36)(this);
  if (CONCAT31(extraout_var_02,uVar1) != 0) {
    if (0xf < (short)this->mbr_0x6) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    if ((*(int *)(&UNK_0066b15c.field_0x1040 + ((short)this->mbr_0x6 * 0x20 + iVar6) * 4) < 1) &&
       ((int *)this->mbr_0x54 != (int *)0x0)) {
      FUN_004bf990((int *)this->mbr_0x54,(uint)this->mbr_0xc,(int)(short)this->mbr_0x6,iVar6,
                   unaff_ESI);
    }
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    if (CONCAT31(extraout_var_03,uVar1) < 0x20) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      iVar6 = CONCAT31(extraout_var_04,uVar1);
    }
    else {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      uVar2 = CONCAT31(extraout_var_05,uVar1) - 0x10U & 0x8000000f;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffff0) + 1;
      }
      iVar6 = uVar2 + 0x10;
    }
    return *(int **)(&UNK_0066b15c.field_0x840 + ((short)this->mbr_0x6 * 0x20 + iVar6) * 4);
  }
  piVar3 = (int *)virt_meth_0x46f190(this);
  uVar2 = _DAT_006668d0;
  piVar4 = piVar3 + 0x12;
  if (*(ushort *)piVar4 != _DAT_006668d0) {
    iVar6 = piVar3[1];
    iVar5 = 0;
    if (0 < *piVar3 * iVar6) {
      do {
        if (*(ushort *)piVar4 == 0) {
          *(ushort *)piVar4 = (ushort)uVar2;
        }
        iVar5 = iVar5 + 1;
        piVar4 = (int *)((int)piVar4 + 2);
      } while (iVar5 < *piVar3 * iVar6);
    }
    piVar3[6] = uVar2;
  }
  return piVar3;
}



// Function at 004bff60

void SoftwareRenderer::virt_meth_0x4bff60()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  uint uVar2;
  int iVar3;
  undefined4 in_stack_00000004;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
  if (CONCAT31(extraout_var,uVar1) < 0x20) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    iVar3 = CONCAT31(extraout_var_00,uVar1);
  }
  else {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    uVar2 = CONCAT31(extraout_var_01,uVar1) - 0x10U & 0x8000000f;
    if ((int)uVar2 < 0) {
      uVar2 = (uVar2 - 1 | 0xfffffff0) + 1;
    }
    iVar3 = uVar2 + 0x10;
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477900_36)(this);
  if (CONCAT31(extraout_var_02,uVar1) != 0) {
    if (0xf < (short)this->mbr_0x6) {
      FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
    }
    if ((*(int *)(&UNK_0066b15c.field_0x40 + ((short)this->mbr_0x6 * 0x20 + iVar3) * 4) < 1) &&
       ((int *)this->mbr_0x54 != (int *)0x0)) {
      FUN_004bfaf0((int *)this->mbr_0x54,(uint)this->mbr_0xc,(int)(short)this->mbr_0x6,iVar3);
    }
    (**(code **)(*(int *)this->mbr_0x54 + 0x10))(this,in_stack_00000004);
    return;
  }
  if ((int *)this->mbr_0x54 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x54 + 0x10))(this,in_stack_00000004);
  }
  return;
}



// Function at 004c0020

undefined4 SoftwareRenderer::virt_meth_0x4c0020()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  
  if (0xf < (short)this->mbr_0x6) {
    FUN_00481c10(PTR_s_Too_many_ammo_object_types_in_am_005df108);
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477900_36)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    if (CONCAT31(extraout_var_00,uVar1) < 0x20) {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      iVar3 = CONCAT31(extraout_var_01,uVar1);
    }
    else {
      uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
      uVar2 = CONCAT31(extraout_var_02,uVar1) - 0x10U & 0x8000000f;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffff0) + 1;
      }
      iVar3 = uVar2 + 0x10;
    }
    return *(undefined4 *)(&UNK_0066a92c.field_0xc + ((short)this->mbr_0x6 * 0x20 + iVar3) * 4);
  }
  uVar4 = virt_meth_0x46f160(this);
  return uVar4;
}



// Function at 004c0880

void SoftwareRenderer::virt_meth_0x4c0880()

{
  if (0 < (int)this->mbr_0xdc) {
    this->mbr_0xdc = this->mbr_0xdc - 1;
  }
  if ((this->mbr_0xdc == 0) ||
     ((((DAT_00668154 == 0 && (this->mbr_0x64 == 0)) && (this->mbr_0x1c == 0)) &&
      ((this->mbr_0x20 == 0 && (this->mbr_0x24 == 0)))))) {
    (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
  }
  virt_meth_0x4708e0_OnScreen(this);
  return;
}



// Function at 004c08e0

TScreen * SoftwareRenderer::virt_meth_0x4c08e0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e1bb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xe0);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&cls_0x5a6b68__vftable_5a6b68_005a6b68;
    this_00->mbr_0xd8 = 0xffffffff;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8008;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004c09e0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4c09e0()

{
  byte in_stack_00000004;
  
  cls_0x5a6b68::~cls_0x5a6b68((cls_0x5a6b68 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0a10

TScreen * SoftwareRenderer::virt_meth_0x4c0a10(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e1fb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xe0);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0a90_005a6fb0;
    this_00->mbr_0xd8 = 0xffffffff;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8008;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004c0a90

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4c0a90()

{
  byte in_stack_00000004;
  
  cls_0x5a6b68::~cls_0x5a6b68((cls_0x5a6b68 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0ac0

TScreen * SoftwareRenderer::virt_meth_0x4c0ac0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e21b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xe0);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0b40_005a71d4;
    this_00->mbr_0xd8 = 0xffffffff;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8008;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004c0b40

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4c0b40()

{
  byte in_stack_00000004;
  
  cls_0x5a6b68::~cls_0x5a6b68((cls_0x5a6b68 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0b70

TScreen * SoftwareRenderer::virt_meth_0x4c0b70(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e23b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xe0);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4c0bf0_005a73f8;
    this_00->mbr_0xd8 = 0xffffffff;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8008;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004c0bf0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4c0bf0()

{
  byte in_stack_00000004;
  
  cls_0x5a6b68::~cls_0x5a6b68((cls_0x5a6b68 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c0e10

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4c0e10()

{
  byte in_stack_00000004;
  
  cls_0x5a6b68::~cls_0x5a6b68((cls_0x5a6b68 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004c10b0

TScreen * SoftwareRenderer::virt_meth_0x4c10b0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e27b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&cls_0x5a761c__vftable_5a761c_005a761c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004dd370

undefined4 SoftwareRenderer::virt_meth_0x4dd370()

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 unaff_ESI;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI);
  if ((CONCAT31(extraout_var,uVar1) == 0) &&
     (iVar2 = TPlayScreen::meth_0x470040((TPlayScreen *)this), iVar2 < 1)) {
    return 0xffffffff;
  }
  if ((*(byte *)&this->mbr_0x8 & 0x80) != 0) {
    return 0xffffffff;
  }
  return 0;
}



// Function at 004dd3e0

void SoftwareRenderer::virt_meth_0x4dd3e0(int param_1, int param_2)

{
  SoftwareRenderer__vftable_5b8e94 *pcVar1;
  
  virt_meth_0x472430(this,param_1);
  if ((1 < param_2) && (param_2 < 5)) {
    pcVar1 = this->vftptr_0x0;
    *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 8;
    (*pcVar1->virt_meth_0x4736f0_224)(this,s_Locked_005e0c74);
    (*this->vftptr_0x0->virt_meth_0x4736f0_224)(this,s_PickDifficulty_005e0c7c);
  }
  if ((this->mbr_0x8 & 0x40000000) == 0) {
    (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
    (*this->vftptr_0x0->virt_meth_0x46f250_24)(this);
  }
  return;
}



// Function at 004dd470

void SoftwareRenderer::virt_meth_0x4dd470()

{
  virt_meth_0x472980(this);
  return;
}



// Function at 004ddd40

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4ddd40()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004de060

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4de060()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004de120

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4de120()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004de800

void SoftwareRenderer::virt_meth_0x4de800()

{
  short sVar1;
  bool bVar2;
  undefined uVar3;
  cls_0x4f54d0 *this_00;
  dword dVar4;
  cls_0x5a7e38 *this_01;
  int iVar5;
  int iVar6;
  int *piVar7;
  uint uVar8;
  undefined3 extraout_var;
  int iVar9;
  dword dVar10;
  SoftwareRenderer__vftable_5b8e94 *pcVar11;
  int iVar12;
  int iVar13;
  longlong lVar14;
  float local_b8;
  float local_b4;
  float local_b0;
  uint local_ac;
  int local_a8;
  int local_a4;
  int local_a0;
  dword dStack_9c;
  int local_90;
  float afStack_8c [3];
  float afStack_80 [16];
  float afStack_40 [16];
  
  if (0 < (int)this->mbr_0x138) {
    if ((int)this->mbr_0x138 < 2) {
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
      this->mbr_0x138 = this->mbr_0x138 - 1;
      return;
    }
    (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    this->mbr_0x138 = this->mbr_0x138 - 1;
    return;
  }
  virt_meth_0x4708e0_OnScreen(this);
  if (((this->mbr_0xd8 == 0) || (iVar6 = *(int *)(this->mbr_0xd8 + 0x11c), iVar6 == 0)) ||
     (*(int *)(iVar6 + 0xb0) < 1)) {
    bVar2 = false;
  }
  else {
    bVar2 = true;
  }
  if (DAT_005d7a18 == 0) {
    if ((*(byte *)&this->mbr_0x8 & 4) != 0) {
LAB_004de916:
      cls_0x5b4f30_TPlayer::meth_0x4713c0((cls_0x5b4f30_TPlayer *)this);
    }
LAB_004de91d:
    if (bVar2) {
      if (((int)this->mbr_0x140 < 1) || (*(int *)(*(int *)(this->mbr_0xd8 + 0x11c) + 0xc4) < 1)) {
        dVar4 = this->mbr_0xd8;
        iVar6 = *(int *)(dVar4 + 0x11c);
        if (((int)this->mbr_0x13c < *(int *)(iVar6 + 0xc0)) && (0 < *(int *)(iVar6 + 0xc0))) {
          iVar6 = *(int *)(iVar6 + 0xb0);
          dVar10 = this->mbr_0x13c + 1;
          iVar13 = *(int *)(dVar4 + 0x11c);
          this->mbr_0x13c = dVar10;
          dVar4 = (int)(iVar6 * dVar10) / *(int *)(iVar13 + 0xc0);
        }
        else {
          dVar4 = *(dword *)(*(int *)(dVar4 + 0x11c) + 0xb0);
        }
        this->mbr_0x148 = dVar4;
      }
      else {
        dVar4 = this->mbr_0x140 - 1;
        this->mbr_0x140 = dVar4;
        dVar4 = (int)(this->mbr_0x148 * dVar4) / *(int *)(*(int *)(this->mbr_0xd8 + 0x11c) + 0xc4);
      }
      *(dword *)&this->mbr_0x144 = dVar4;
      cls_0x5b4f30_TPlayer::meth_0x4714e0((cls_0x5b4f30_TPlayer *)this);
    }
  }
  else {
    if ((*(byte *)&this->mbr_0x8 & 4) != 0) goto LAB_004de91d;
    if (bVar2) {
      this_00 = (cls_0x4f54d0 *)cls_0x5b4f30_TPlayer::meth_0x4f6ea0((cls_0x5b4f30_TPlayer *)this);
      cls_0x4f54d0::meth_0x4f54d0(this_00);
      cls_0x5b4f30_TPlayer::meth_0x471820((cls_0x5b4f30_TPlayer *)this);
      cls_0x5b4f30_TPlayer::meth_0x4714e0((cls_0x5b4f30_TPlayer *)this);
      cls_0x5b4f30_TPlayer::meth_0x4715e0((cls_0x5b4f30_TPlayer *)this);
      cls_0x5b4f30_TPlayer::meth_0x4716c0((cls_0x5b4f30_TPlayer *)this);
      goto LAB_004de916;
    }
  }
  piVar7 = (int *)this->mbr_0xe4;
  if (piVar7 != (int *)0x0) {
    iVar6 = piVar7[6];
    iVar13 = piVar7[4];
    iVar12 = piVar7[5];
    local_ac = (uint)*(ushort *)((int)piVar7 + 0xe);
    local_90 = iVar6;
    if (this->mbr_0x120 == 0) {
      if ((*(char *)&this->mbr_0xec != '\0') &&
         (iVar6 = (**(code **)(*piVar7 + 0x24))(), iVar6 != 0)) {
        this_01 = (cls_0x5a7e38 *)(**(code **)(*(int *)this->mbr_0xe4 + 0x24))();
        cls_0x5a7e38::meth_0x40e670(this_01);
        cls_0x5a7e38::meth_0x40edc0(this_01);
        iVar6 = cls_0x5a7e38::meth_0x40eef0(this_01);
        if (iVar6 == 0) {
          dVar4 = this->mbr_0xe4;
          iVar13 = *(int *)(dVar4 + 0x10);
          iVar12 = *(int *)(dVar4 + 0x14);
          local_90 = *(int *)(dVar4 + 0x18);
        }
        else {
          cls_0x5a486c::meth_0x40a420
                    ((cls_0x5a486c *)(this_01->cls_0x5a47f0).mbr_0x8,iVar6,
                     (this_01->cls_0x5a47f0).mbr_0xc,(this_01->cls_0x5a47f0).mbr_0x14,afStack_80);
          FUN_0043aa90(afStack_40,(float *)(iVar6 + 0x58),afStack_80);
          if (this->mbr_0x14c == 0) {
            local_b0 = 0.0;
            local_b4 = 0.0;
            local_b8 = 0.0;
          }
          else {
            local_b8 = (float)*(int *)&this->field_0x150;
            local_b4 = (float)*(int *)&this->field_0x154;
            local_b0 = (float)*(int *)&this->field_0x158;
          }
          FUN_0043ad80(afStack_40,&local_b8,afStack_8c);
          local_b0 = *(float *)(this->mbr_0xe4 + 0x18);
          lVar14 = __ftol();
          iVar13 = (int)lVar14;
          lVar14 = __ftol();
          iVar12 = (int)lVar14;
          lVar14 = __ftol();
          local_90 = (int)lVar14;
        }
      }
    }
    else {
      FUN_0046db20_ConvertToVector(this->mbr_0x124,this->mbr_0x128,(int *)&local_b8,0);
      iVar12 = iVar12 + (int)local_b4;
      iVar13 = iVar13 + (int)local_b8;
      local_90 = iVar6 + (int)local_b0;
    }
    dVar4 = this->mbr_0xd8;
    if (((dVar4 == 0) || (iVar6 = *(int *)(dVar4 + 0x120), iVar6 == 0)) ||
       (*(int *)(iVar6 + 0x70) == 0)) {
      local_a0 = local_90;
      local_a8 = iVar13;
      local_a4 = iVar12;
    }
    else {
      if (iVar6 == 0) {
        iVar6 = 0;
      }
      else {
        iVar6 = *(int *)(iVar6 + 0x70);
      }
      if (*(int *)(dVar4 + 0x120) == 0) {
        iVar9 = 0;
      }
      else {
        iVar9 = *(int *)(*(int *)(dVar4 + 0x120) + 0x70);
      }
      if (*(int *)(dVar4 + 0x120) == 0) {
        iVar5 = 0;
      }
      else {
        iVar5 = *(int *)(*(int *)(dVar4 + 0x120) + 0x70);
      }
      local_a0 = *(int *)(iVar5 + 0xc0) + local_90;
      local_a8 = *(int *)(iVar6 + 0xb8) + iVar13;
      local_a4 = *(int *)(iVar9 + 0xbc) + iVar12;
    }
    (*this->vftptr_0x0->virt_meth_0x46ed70_8)(this);
    if (0 < (int)this->mbr_0xe8) {
      this->mbr_0xe8 = this->mbr_0xe8 - 1;
    }
    if (this->mbr_0xe8 == 0) {
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
    if (this->mbr_0x15c != 0) {
      sVar1 = *(short *)(this->mbr_0xe4 + 4);
      if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
        iVar6 = *(int *)(this->mbr_0xe4 + 0xe0);
        if ((iVar6 == 0) || (iVar6 = FUN_004dab80_TActionBlock_Is(iVar6,(char *)&this->mbr_0x15c), iVar6 == 0)) {
          pcVar11 = this->vftptr_0x0;
        }
        else {
          pcVar11 = this->vftptr_0x0;
        }
        (*pcVar11->virt_meth_0x472e90_64)(this);
      }
    }
  }
  dVar4 = this->mbr_0xd8;
  if (((dVar4 != 0) && (*(int *)(dVar4 + 0x11c) != 0)) &&
     ((*(int *)(*(int *)(dVar4 + 0x11c) + 200) != 0 &&
      ((0 < *(int *)(dVar4 + 0xc) && (*(int *)(dVar4 + 0x10) != 0)))))) {
    if (*(int *)(dVar4 + 0xc) < 1) {
      piVar7 = (int *)0x0;
    }
    else {
      piVar7 = *(int **)(dVar4 + 0x10);
    }
    (**(code **)(*piVar7 + 0x1b4))(1);
  }
  if (this->mbr_0x130 != 0) {
    if (0 < (int)this->mbr_0x12c) {
      this->mbr_0x12c = this->mbr_0x12c - 1;
    }
    dVar4 = this->mbr_0x12c;
    if (dVar4 == 0) {
      dVar4 = this->mbr_0xd8;
      if ((((dVar4 != 0) && (*(int *)(dVar4 + 0x11c) != 0)) &&
          (*(int *)(*(int *)(dVar4 + 0x11c) + 200) != 0)) &&
         ((0 < *(int *)(dVar4 + 0xc) && (*(int *)(dVar4 + 0x10) != 0)))) {
        if (*(int *)(dVar4 + 0xc) < 1) {
          piVar7 = (int *)0x0;
        }
        else {
          piVar7 = *(int **)(dVar4 + 0x10);
        }
        (**(code **)(*piVar7 + 0x1b4))(0);
      }
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
    else {
      dStack_9c = this->mbr_0x134;
      if ((int)dVar4 < (int)dStack_9c) {
        local_ac = dStack_9c - dVar4;
        lVar14 = __ftol();
        switch((int)lVar14) {
        case 0:
          pcVar11 = this->vftptr_0x0;
          break;
        case 1:
          pcVar11 = this->vftptr_0x0;
          break;
        case 2:
          pcVar11 = this->vftptr_0x0;
          break;
        case 3:
          pcVar11 = this->vftptr_0x0;
          break;
        default:
          goto switchD_004deda2_caseD_4;
        }
        (*pcVar11->virt_meth_0x472e90_64)(this);
      }
    }
  }
switchD_004deda2_caseD_4:
  dVar4 = this->mbr_0xd8;
  if (((dVar4 != 0) && (iVar6 = *(int *)(dVar4 + 0x120), iVar6 != 0)) &&
     (*(int *)(iVar6 + 0x70) != 0)) {
    if (iVar6 == 0) {
      iVar6 = 0;
    }
    else {
      iVar6 = *(int *)(iVar6 + 0x70);
    }
    if (*(int *)(dVar4 + 0x120) == 0) {
      iVar13 = 0;
    }
    else {
      iVar13 = *(int *)(*(int *)(dVar4 + 0x120) + 0x70);
    }
    if ((*(int *)(iVar6 + 0x48) != 0) && (*(int *)(iVar13 + 0x4c) == (int)(short)this->mbr_0x5c)) {
      FUN_004de3c0(*(TPlayScreen **)(dVar4 + 4),(int *)&this->mbr_0x10,*(int *)(iVar6 + 0x48),
                   *(int *)(*(int *)(dVar4 + 0x120) + 0x54),*(int *)(*(int *)(dVar4 + 0x120) + 0x58)
                   ,*(undefined4 *)(*(int *)(dVar4 + 0x11c) + 0x80),0);
    }
  }
  if ((this->mbr_0x58 != 0) && (this->mbr_0xd8 == 0)) {
    if ((int *)this->mbr_0x54 == (int *)0x0) {
      uVar8 = 0;
    }
    else {
      uVar8 = (**(code **)(*(int *)this->mbr_0x54 + 0x8c))(this->mbr_0xc);
    }
    if (((uVar8 & 1) == 0) &&
       (uVar3 = (*this->vftptr_0x0->virt_meth_0x477d30_340)(this), CONCAT31(extraout_var,uVar3) != 0
       )) {
      (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    }
  }
  (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
  return;
}



// Function at 004def30

void SoftwareRenderer::virt_meth_0x4def30()

{
  int iVar1;
  uint in_stack_00000004;
  
  if (((((this->mbr_0x8 & 0x1000) == 0) && ((in_stack_00000004 & 0x1000) != 0)) &&
      (this->mbr_0xd8 != 0)) &&
     (((iVar1 = *(int *)(this->mbr_0xd8 + 0x11c), iVar1 != 0 && (0 < *(int *)(iVar1 + 0xb0))) &&
      (0 < *(int *)(iVar1 + 0xc4))))) {
    this->mbr_0x140 = *(dword *)(iVar1 + 0xc4);
  }
  virt_meth_0x472e90_SetFlags(this);
  return;
}



// Function at 004defe0

void SoftwareRenderer::meth_0x4defe0()

{
  dword dVar1;
  undefined uVar2;
  int *piVar3;
  uint uVar4;
  undefined3 extraout_var;
  uint uVar5;
  
  dVar1 = this->mbr_0xd8;
  if ((((dVar1 != 0) && (*(int *)(dVar1 + 0x11c) != 0)) &&
      (*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0)) &&
     ((0 < *(int *)(dVar1 + 0xc) && (*(int *)(dVar1 + 0x10) != 0)))) {
    if (*(int *)(dVar1 + 0xc) < 1) {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = *(int **)(dVar1 + 0x10);
    }
    (**(code **)(*piVar3 + 0x1b4))(0);
  }
  uVar5 = this->mbr_0x8 | 0x1000;
  (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
  uVar4 = this->mbr_0x8 | 0x8000;
  (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
  if (this->mbr_0xd8 != 0) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x477930_60)(this);
    if (CONCAT31(extraout_var,uVar2) != 0) {
      (**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar4,uVar5);
    }
  }
  return;
}



// Function at 004df070

dword SoftwareRenderer::meth_0x4df070()

{
  int iVar1;
  dword dVar2;
  int local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  dVar2 = this->mbr_0xd8;
  if (dVar2 == 0) {
    this->mbr_0xe0 = 0;
    return this->mbr_0xe0;
  }
  if (((0 < *(int *)(dVar2 + 0xc)) && (iVar1 = *(int *)(dVar2 + 0x10), iVar1 != 0)) &&
     (iVar1 != *(int *)(dVar2 + 4))) {
    local_c = *(int *)(iVar1 + 0x10);
    local_8 = *(undefined4 *)(iVar1 + 0x14);
    local_4 = *(undefined4 *)(iVar1 + 0x18);
    dVar2 = FUN_0046dc60_AngleToPP((int *)&this->mbr_0x10,&local_c);
    this->mbr_0xe0 = dVar2;
    return dVar2;
  }
  dVar2 = (dword)*(byte *)(*(int *)(dVar2 + 4) + 0x36);
  this->mbr_0xe0 = dVar2;
  return dVar2;
}



// Function at 004df0f0

void SoftwareRenderer::virt_meth_0x4df0f0(int param_1)

{
  dword dVar1;
  dword dVar2;
  undefined uVar3;
  int *piVar4;
  uint uVar5;
  undefined3 extraout_var;
  uint uVar6;
  dword in_stack_00000008;
  
  virt_meth_0x471ba0_Notify(this,param_1);
  if (param_1 == 1) {
    if (this->mbr_0xe4 != in_stack_00000008) goto LAB_004df14e;
  }
  else if ((((param_1 != 2) || (dVar1 = this->mbr_0xe4, dVar1 == 0)) ||
           (*(dword *)(dVar1 + 0x44) != in_stack_00000008)) ||
          ((*(uint *)(dVar1 + 8) & 0x80000) != 0)) goto LAB_004df14e;
  this->mbr_0x120 = 0;
  this->mbr_0xe4 = 0;
  this->mbr_0x8 = this->mbr_0x8 | 0x9000;
LAB_004df14e:
  dVar1 = this->mbr_0xd8;
  if ((dVar1 != 0) && (dVar2 = *(dword *)(dVar1 + 4), dVar2 != 0)) {
    if (param_1 == 1) {
      if (dVar2 != in_stack_00000008) {
        return;
      }
    }
    else {
      if (param_1 != 2) {
        return;
      }
      if (*(dword *)(dVar2 + 0x44) != in_stack_00000008) {
        return;
      }
      if ((*(uint *)(dVar2 + 8) & 0x80000) != 0) {
        return;
      }
    }
    if (((*(int *)(dVar1 + 0x11c) != 0) && (*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0)) &&
       ((0 < *(int *)(dVar1 + 0xc) && (*(int *)(dVar1 + 0x10) != 0)))) {
      if (*(int *)(dVar1 + 0xc) < 1) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar4 = *(int **)(dVar1 + 0x10);
      }
      (**(code **)(*piVar4 + 0x1b4))(0);
    }
    uVar6 = this->mbr_0x8 | 0x1000;
    (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    uVar5 = this->mbr_0x8 | 0x8000;
    (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
    if ((this->mbr_0xd8 != 0) &&
       (uVar3 = (*this->vftptr_0x0->virt_meth_0x477930_60)(this), CONCAT31(extraout_var,uVar3) != 0)
       ) {
      (**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar5,uVar6);
    }
    this->mbr_0xd8 = 0;
  }
  return;
}



// Function at 004e9cc0

void SoftwareRenderer::virt_meth_0x4e9cc0(int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  virt_meth_0x4df0f0(this,param_1);
  if (param_1 == 1) {
    if (this->mbr_0x184 != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) {
      return;
    }
    dVar1 = this->mbr_0x184;
    if (dVar1 == 0) {
      return;
    }
    if (*(dword *)(dVar1 + 0x44) != in_stack_00000008) {
      return;
    }
    if ((*(uint *)(dVar1 + 8) & 0x80000) != 0) {
      return;
    }
  }
  (*this->vftptr_0x0->virt_meth_0x472e90_64)(this);
  this->mbr_0x184 = 0;
  return;
}



// Function at 004f4a30

TScreen * SoftwareRenderer::virt_meth_0x4f4a30(int param_1)

{
  uint uVar1;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ecdb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x184);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&cls_0x5a85ac__vftable_5a85ac_005a85ac;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | uVar1 | 0x48001;
    this_00[1].mbr_0x38 = 0;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 004f4ec0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4f4ec0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6110

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4f6110()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6140

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x4f6140()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f23e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4f61e0_005aa19c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f6bc0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4f6bc0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f79b0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4f79b0()

{
  byte in_stack_00000004;
  
  cls_0x4f79d0::~cls_0x4f79d0((cls_0x4f79d0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7d20

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4f7d20()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004fa390

SoftwareRenderer * SoftwareRenderer::virt_meth_0x4fa390()

{
  byte in_stack_00000004;
  
  cls_0x4fa3b0::~cls_0x4fa3b0((cls_0x4fa3b0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004fa440

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x4fa440()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a004e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x4fa4e0_005adc40;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004fa540

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

dword SoftwareRenderer::meth_0x4fa540()

{
  TPlayScreen *this_00;
  int *piVar1;
  dword dVar2;
  float fVar3;
  float fVar4;
  TPlayScreen *pcVar5;
  undefined uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int3 extraout_var;
  dword dVar10;
  uint extraout_ECX;
  uint extraout_ECX_00;
  dword dVar11;
  dword *pdVar12;
  cls_0x4cefb0 *this_01;
  dword *pdVar13;
  longlong lVar14;
  longlong lVar15;
  int iStack_78;
  float fStack_68;
  float fStack_60;
  float fStack_5c;
  dword dStack_58;
  dword local_54;
  dword local_50;
  dword local_4c;
  cls_0x44ceb0 cStack_48;
  
  if (this->mbr_0xd8 == 0) {
    this->mbr_0x3b0 = 0;
    this->mbr_0x510 = 0;
    this->mbr_0x50c = 0;
    this->mbr_0x508 = 0;
    goto LAB_004fa989;
  }
  uVar7 = FUN_0059a530_stricmp((uint)this,(byte *)(*(int *)(this->mbr_0xd8 + 0x120) + 4),
                       (byte *)s_Physical_Paralysis_005e122c);
  if (uVar7 == 0) {
    this->mbr_0x404 = 1;
    this->mbr_0x444 = 0xf0;
  }
  else {
    uVar7 = FUN_0059a530_stricmp(extraout_ECX,(byte *)(*(int *)(this->mbr_0xd8 + 0x120) + 4),
                         (byte *)s_Neural_Paralysis_005e1240);
    if (uVar7 == 0) {
      this->mbr_0x404 = 2;
      this->mbr_0x444 = 0x168;
    }
    else {
      uVar7 = FUN_0059a530_stricmp(this->mbr_0xd8,(byte *)(*(int *)(this->mbr_0xd8 + 0x120) + 4),
                           (byte *)s_Full_Paralysis_005e1254);
      if (uVar7 == 0) {
        this->mbr_0x404 = 3;
        this->mbr_0x444 = 0x168;
      }
      else {
        uVar7 = FUN_0059a530_stricmp(extraout_ECX_00,(byte *)(*(int *)(this->mbr_0xd8 + 0x120) + 4),
                             (byte *)s_Paralize4_005e1264);
        if (uVar7 == 0) {
          this->mbr_0x404 = 4;
          this->mbr_0x444 = 200;
        }
      }
    }
  }
  this_00 = *(TPlayScreen **)(this->mbr_0xd8 + 4);
  if (this_00 == (TPlayScreen *)0x0) {
    this->mbr_0x3b0 = 0;
    this->mbr_0x510 = 0;
    this->mbr_0x50c = 0;
    this->mbr_0x508 = 0;
  }
  else {
    dVar11 = (this_00->TScreen).mbr_0x10;
    this->mbr_0x510 = (this_00->TScreen).mbr_0x18;
    dVar10 = (this_00->TScreen).mbr_0x14;
    this->mbr_0x508 = dVar11;
    this->mbr_0x50c = dVar10;
    lVar14 = __ftol();
    this->mbr_0x510 = (dword)lVar14;
    piVar1 = (int *)(this_00->TScreen).mbr_0xe0;
    if ((piVar1 == (int *)0x0) || ((*piVar1 != 3 && ((piVar1 == (int *)0x0 || (*piVar1 != 0x19))))))
    {
      this_01 = (cls_0x4cefb0 *)0x0;
    }
    else {
      this_01 = (cls_0x4cefb0 *)piVar1[0x11];
    }
    this->mbr_0x3b0 = (dword)this_01;
    if ((this_01 != (cls_0x4cefb0 *)0x0) && (*(int *)(*(int *)&this_01[1].field_0x18 + 0x454) == 0))
    {
      dVar11 = *(dword *)&this_01->field_0x10;
      this->mbr_0x510 = *(dword *)&this_01->field_0x18;
      dVar10 = *(dword *)&this_01->field_0x14;
      this->mbr_0x508 = dVar11;
      this->mbr_0x50c = dVar10;
      lVar14 = __ftol();
      this->mbr_0x510 = (dword)lVar14;
      cls_0x4cefb0::meth_0x4cefb0(this_01);
      (**(code **)(*(int *)this->mbr_0x3b0 + 0x40))(((int *)this->mbr_0x3b0)[2] | 0x800000);
      this->mbr_0x544 = this->mbr_0x544 + 1;
    }
    local_4c = (this_00->TScreen).mbr_0x18;
    local_54 = (this_00->TScreen).mbr_0x10;
    local_50 = (this_00->TScreen).mbr_0x14;
    if (this->mbr_0x404 == 1) {
      iVar8 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      if (-1 < iVar8) {
        iVar9 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
joined_r0x004fa7b1:
        if (iVar9 != 0) {
          cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar8,0x7f,1);
        }
      }
    }
    else {
      iVar8 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      if (-1 < iVar8) {
        iVar9 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
        goto joined_r0x004fa7b1;
      }
    }
    if (((this->mbr_0x404 == 3) || (this->mbr_0x544 == 0)) &&
       (cls_0x44ceb0::cls_0x44ceb0(&cStack_48,&local_54,300,0xe0,2,0),
       (TPlayScreen *)cStack_48.mbr_0xc != (TPlayScreen *)0x0)) {
      do {
        pcVar5 = (TPlayScreen *)cStack_48.mbr_0xc;
        if (((TPlayScreen *)cStack_48.mbr_0xc != this_00) &&
           (uVar6 = (*((TScreen *)cStack_48.mbr_0xc)->vftptr_0x0->virt_meth_0x477e50_448)
                              ((void *)cStack_48.mbr_0xc),
           CONCAT31(extraout_var,uVar6) != 0 && -1 < extraout_var)) {
          fStack_60 = (float)(pcVar5->TScreen).mbr_0x10;
          fStack_5c = (float)(pcVar5->TScreen).mbr_0x14;
          dStack_58 = (pcVar5->TScreen).mbr_0x18;
          iVar8 = FUN_0046de60_Distance2D((int *)&local_54,(int *)&fStack_60);
          if ((iVar8 < 0x191) &&
             ((iVar8 = TPlayScreen::meth_0x4c89c0_IsEnemy(this_00), iVar8 != 0 &&
              (*(int *)(*(int *)&(pcVar5->TScreen).field_0xfc + 0x454) == 0)))) {
            (&this->mbr_0x3b0)[this->mbr_0x544] = (dword)pcVar5;
            dVar11 = this->mbr_0x544;
            pdVar12 = &this->mbr_0x508 + dVar11 * 3;
            dVar10 = (&this->mbr_0x3b0)[dVar11];
            *pdVar12 = *(dword *)(dVar10 + 0x10);
            dVar2 = *(dword *)(dVar10 + 0x18);
            pdVar12[1] = *(dword *)(dVar10 + 0x14);
            pdVar12[2] = dVar2;
            lVar14 = __ftol();
            (&this->mbr_0x510)[dVar11 * 3] = (dword)lVar14;
            cls_0x4cefb0::meth_0x4cefb0((cls_0x4cefb0 *)(&this->mbr_0x3b0)[this->mbr_0x544]);
            (**(code **)(*(int *)(&this->mbr_0x3b0)[this->mbr_0x544] + 0x40))
                      (((int *)(&this->mbr_0x3b0)[this->mbr_0x544])[2] | 0x800000);
            dVar11 = this->mbr_0x544 + 1;
            this->mbr_0x544 = dVar11;
            if ((4 < (int)dVar11) || (this->mbr_0x404 != 3)) goto LAB_004fa968;
          }
        }
        cls_0x44ceb0::meth_0x44d080(&cStack_48);
      } while ((TPlayScreen *)cStack_48.mbr_0xc != (TPlayScreen *)0x0);
      this->mbr_0x401 = 1;
      goto LAB_004fa989;
    }
  }
LAB_004fa968:
  this->mbr_0x401 = 1;
LAB_004fa989:
  local_4c = this->mbr_0x18;
  dVar11 = this->mbr_0x10;
  dVar10 = this->mbr_0x14;
  local_54 = dVar11;
  local_50 = dVar10;
  lVar14 = __ftol();
  if ((int)this->mbr_0x544 < 1) {
    this->mbr_0x3a4 = 0x42480000;
    fStack_68 = 80.0;
    fStack_60 = 0.0;
    fStack_5c = 0.0;
  }
  else {
    iVar8 = (**(code **)(*(int *)this->mbr_0x3b0 + 600))();
    this->mbr_0x3a4 = (dword)(float)iVar8;
    fStack_60 = (float)(dVar11 - this->mbr_0x508);
    fStack_5c = (float)(dVar10 - this->mbr_0x50c);
    fStack_68 = SQRT(fStack_60 * fStack_60 + fStack_5c * fStack_5c);
  }
  iStack_78 = 0;
  *(float *)&this->field_0x3a0 = (float)this->mbr_0x3a4 * _DAT_005a8a14;
  fVar3 = (float)this->mbr_0x3a4 * _DAT_005a3810;
  this->mbr_0x3a4 = (dword)fVar3;
  *(float *)&this->field_0x3a8 = fVar3 + _DAT_005a34e4;
  fVar3 = (float)local_54;
  fVar4 = (float)local_50;
  if (this->mbr_0x404 == 4) {
    pdVar12 = &this->mbr_0x364;
    iVar8 = 0;
    pdVar13 = &this->mbr_0x44c;
    do {
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      pdVar13[-1] = (dword)((float)iVar9 * _DAT_005a3538 + fVar3);
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      *pdVar13 = (dword)((float)iVar9 * _DAT_005a3538 + fVar4);
      iVar9 = FUN_00483300_RandomRange(-5,5);
      pdVar13[1] = (dword)((float)iVar9 * _DAT_005a3538 + (float)((int)lVar14 + 0x3c));
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      pdVar13[-0xb2] = (dword)((float)iVar9 * _DAT_005a3538);
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      pdVar13[-0xb1] = (dword)((float)iVar9 * _DAT_005a3538);
      iVar9 = FUN_00483300_RandomRange(-5,0);
      pdVar13[-0xb0] = (dword)((float)iVar9 * _DAT_005a350c);
      lVar15 = __ftol();
      dVar10 = FUN_00483300_RandomRange(0,(int)lVar15 * 100);
      pdVar12[-0x1e] = (dword)((float)dVar10 * _DAT_005a350c + (float)this->mbr_0x3a4);
      *pdVar12 = 0x3f800000;
      pdVar12[-0xf] = 0x3e75c28f;
      pdVar12[0x29] = this->mbr_0x444 - iVar8;
      dVar11 = this->mbr_0x544;
      if (dVar11 == 0) {
        pdVar12[0x18] = 0;
      }
      else {
        dVar10 = iStack_78 / (int)dVar11;
        pdVar12[0x18] = iStack_78 % (int)dVar11;
      }
      iVar8 = iVar8 + 2;
      iStack_78 = iStack_78 + 1;
      pdVar12 = pdVar12 + 1;
      pdVar13 = pdVar13 + 3;
    } while (iVar8 < 0x1e);
  }
  else {
    pdVar12 = &this->mbr_0x364;
    iVar8 = 0;
    pdVar13 = &this->mbr_0x44c;
    do {
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      pdVar13[-1] = (dword)((float)iVar9 * _DAT_005a3538 + fVar3);
      iVar9 = FUN_00483300_RandomRange(-0xf,0xf);
      *pdVar13 = (dword)((float)iVar9 * _DAT_005a3538 + fVar4);
      iVar9 = FUN_00483300_RandomRange(-5,5);
      pdVar13[1] = (dword)((float)iVar9 * _DAT_005a3538 + (float)((int)lVar14 + 0x3c));
      iVar9 = FUN_00483300_RandomRange(-0x37,0x37);
      pdVar13[-0xb2] = (dword)((((float)iVar9 + fStack_60) * _DAT_005a4960) / fStack_68);
      iVar9 = FUN_00483300_RandomRange(-0x37,0x37);
      pdVar13[-0xb1] = (dword)((((float)iVar9 + fStack_5c) * _DAT_005a4960) / fStack_68);
      iVar9 = FUN_00483300_RandomRange(-5,0);
      pdVar13[-0xb0] = (dword)((float)iVar9 * _DAT_005a350c);
      lVar15 = __ftol();
      iVar9 = FUN_00483300_RandomRange(0,(int)lVar15 * 100);
      pdVar12[-0x1e] = (dword)((float)iVar9 * _DAT_005a350c + (float)this->mbr_0x3a4);
      *pdVar12 = 0x3f800000;
      pdVar12[-0xf] = 0x3e75c28f;
      dVar10 = this->mbr_0x444 - iVar8;
      pdVar12[0x29] = dVar10;
      dVar11 = this->mbr_0x544;
      if (dVar11 == 0) {
        pdVar12[0x18] = 0;
      }
      else {
        dVar10 = iStack_78 / (int)dVar11;
        pdVar12[0x18] = iStack_78 % (int)dVar11;
      }
      iVar8 = iVar8 + 2;
      iStack_78 = iStack_78 + 1;
      pdVar12 = pdVar12 + 1;
      pdVar13 = pdVar13 + 3;
    } while (iVar8 < 0x1e);
  }
  this->mbr_0x504 = 0x3e4ccccd;
  this->mbr_0x500 = 0;
  this->mbr_0x4fc = 0x3d23d70a;
  this->mbr_0x3ac = 1;
  this->mbr_0x400 = 1;
  return dVar10;
}



// Function at 004fb560

void SoftwareRenderer::virt_meth_0x4fb560(int param_1)

{
  dword dVar1;
  dword *pdVar2;
  int iVar3;
  dword in_stack_00000008;
  
  virt_meth_0x4df0f0(this,param_1);
  iVar3 = 0;
  if (0 < (int)this->mbr_0x544) {
    pdVar2 = &this->mbr_0x3b0;
    do {
      if (param_1 == 1) {
        if (*pdVar2 == in_stack_00000008) {
LAB_004fb5a7:
          *pdVar2 = 0;
        }
      }
      else if ((((param_1 == 2) && (dVar1 = *pdVar2, dVar1 != 0)) &&
               (*(dword *)(dVar1 + 0x44) == in_stack_00000008)) &&
              ((*(byte *)(dVar1 + 10) & 8) == 0)) goto LAB_004fb5a7;
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 1;
    } while (iVar3 < (int)this->mbr_0x544);
  }
  return;
}



// Function at 004fd8c0

void SoftwareRenderer::virt_meth_0x4fd8c0()

{
  int *piVar1;
  dword dVar2;
  
  virt_meth_0x4de800(this);
  if (((this->mbr_0x184 == 0) && (this->mbr_0xd8 != 0)) &&
     (piVar1 = *(int **)(this->mbr_0xd8 + 4), piVar1 != (int *)0x0)) {
    if (piVar1[0x69] == 0) {
      FUN_004d5880(piVar1,1);
    }
    else {
      this->mbr_0x184 = 0xffffffff;
      meth_0x4defe0(this);
    }
  }
  dVar2 = this->mbr_0x184 + 1;
  this->mbr_0x184 = dVar2;
  if (499 < (int)dVar2) {
    if ((this->mbr_0xd8 != 0) && (piVar1 = *(int **)(this->mbr_0xd8 + 4), piVar1 != (int *)0x0)) {
      FUN_004d5880(piVar1,0);
    }
    meth_0x4defe0(this);
  }
  return;
}



// Function at 004fe800

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SoftwareRenderer::meth_0x4fe800()

{
  int *piVar1;
  int iVar2;
  dword dVar3;
  float fVar4;
  TPlayScreen *this_00;
  undefined uVar5;
  int iVar6;
  dword dVar7;
  int3 extraout_var;
  TPlayScreen *this_01;
  cls_0x44ceb0 local_48;
  
  iVar6 = FUN_00483300_RandomRange(0,1);
  if (iVar6 == 0) {
    dVar7 = FUN_00483300_RandomRange(0x96,200);
    this->mbr_0x1a0 = dVar7;
    iVar6 = FUN_00483300_RandomRange(0x96,200);
    dVar7 = -iVar6;
  }
  else {
    iVar6 = FUN_00483300_RandomRange(0x96,200);
    this->mbr_0x1a0 = -iVar6;
    dVar7 = FUN_00483300_RandomRange(0x96,200);
  }
  this->mbr_0x1a4 = dVar7;
  this_01 = (TPlayScreen *)0x0;
  fVar4 = (float)this->mbr_0x1a0 * _DAT_005a9d80;
  this->mbr_0x1a8 = 0x15e;
  this->mbr_0x19c = 0x418c0000;
  this->mbr_0x188 = 0;
  this->mbr_0x194 = (dword)fVar4;
  this->mbr_0x198 = (dword)((float)this->mbr_0x1a4 * _DAT_005a9d80);
  if (this->mbr_0xd8 != 0) {
    this_01 = *(TPlayScreen **)(this->mbr_0xd8 + 4);
  }
  cls_0x44ceb0::cls_0x44ceb0(&local_48,&this->mbr_0x10,400,0xe0,2,0);
  this_00 = (TPlayScreen *)local_48.mbr_0xc;
  do {
    if (this_00 == (TPlayScreen *)0x0) {
LAB_004fe951:
      dVar7 = this->mbr_0x188;
      if (dVar7 == 0) {
        if (this_01 != (TPlayScreen *)0x0) {
          dVar7 = (this_01->TScreen).mbr_0x14;
          dVar3 = (this_01->TScreen).mbr_0x18;
          this->mbr_0x1a0 = this->mbr_0x1a0 + (this_01->TScreen).mbr_0x10;
          this->mbr_0x1a4 = this->mbr_0x1a4 + dVar7;
          this->mbr_0x1a8 = this->mbr_0x1a8 + dVar3;
        }
      }
      else {
        iVar6 = *(int *)(dVar7 + 0x14);
        iVar2 = *(int *)(dVar7 + 0x18);
        this->mbr_0x1a0 = this->mbr_0x1a0 + *(int *)(dVar7 + 0x10);
        this->mbr_0x1a4 = this->mbr_0x1a4 + iVar6;
        this->mbr_0x1a8 = this->mbr_0x1a8 + iVar2;
      }
      this->mbr_0x184 = 0;
      this->mbr_0x190 = 2;
      return;
    }
    local_48.mbr_0xc = (dword)this_00;
    if (((((this_00 != this_01) &&
          (iVar6 = FUN_0046de60_Distance2D((int *)&this->mbr_0x10,(int *)&(this_00->TScreen).mbr_0x10),
          iVar6 < 0x191)) &&
         (uVar5 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477e50_448)(this_00),
         CONCAT31(extraout_var,uVar5) != 0 && -1 < extraout_var)) &&
        ((this_01 == (TPlayScreen *)0x0 ||
         (iVar6 = TPlayScreen::meth_0x4c89c0_IsEnemy(this_01), iVar6 != 0)))) &&
       ((this_00 == (TPlayScreen *)0x0 ||
        ((piVar1 = (int *)(this_00->TScreen).mbr_0xd8, piVar1 == (int *)0x0 || (*piVar1 != 0xc)
         ))))) {
      this->mbr_0x188 = (dword)this_00;
      goto LAB_004fe951;
    }
    cls_0x44ceb0::meth_0x44d080(&local_48);
    this_00 = (TPlayScreen *)local_48.mbr_0xc;
  } while( true );
}



// Function at 004ffbf0

void SoftwareRenderer::virt_meth_0x4ffbf0()

{
  dword dVar1;
  
  virt_meth_0x4de800(this);
  dVar1 = this->mbr_0x184 + 1;
  this->mbr_0x184 = dVar1;
  if (499 < (int)dVar1) {
    meth_0x4defe0(this);
  }
  return;
}



// Function at 00500020

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500020()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005001c0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5001c0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005001f0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x5001f0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a00ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x304);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x500290_005ae118;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500350

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500350()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500380

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x500380()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a013e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x184);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x500420_005ae394;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005004e0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5004e0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500510

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x500510()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a018e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x3d0);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5005b0_005ae610;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500660

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500660()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500690

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x500690()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a01de;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x500730_005ae87c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500800

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500800()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500830

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x500830()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a022e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x814);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5008d0_005aeae0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500990

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500990()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005009c0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x5009c0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a027e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4cc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x500a60_005aed48;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500b20

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500b20()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500b50

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x500b50()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a02ce;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x13f8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x500bf0_005aefb8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00500c80

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500c80()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500cb0

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500cb0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a030b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500d10_005af21c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500d10

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500d10()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500d40

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500d40(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a032b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500da0_005af41c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500da0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500da0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500dd0

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500dd0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a034b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500e30_005af61c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500e30

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500e30()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500e60

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500e60(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a036b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500ec0_005af81c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500ec0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500ec0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500ef0

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500ef0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a038b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500f50_005afa1c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500f50

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500f50()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00500f80

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x500f80(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  cls_0x5a85ac *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a03ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x184);
  local_4 = 0;
  pcVar1 = (cls_0x5a85ac *)0x0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x500fe0_005afc1c;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00500fe0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x500fe0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501010

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x501010()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a03de;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x134);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5010b0_005afe1c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501650

SoftwareRenderer * SoftwareRenderer::virt_meth_0x501650()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00501680

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x501680()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a054e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x3d8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x501720_005b02d8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00501790

void SoftwareRenderer::virt_meth_0x501790()

{
  int *piVar1;
  TPlayScreen *this_00;
  
  virt_meth_0x4de800(this);
  if (this->mbr_0x184 == 2) {
    piVar1 = *(int **)(((cls_0x5b99c0 *)this->mbr_0xd8)->mbr_0x4 + 0xe0);
    if ((piVar1 != (int *)0x0) &&
       (((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) &&
        (this_00 = (TPlayScreen *)piVar1[0x11], this_00 != (TPlayScreen *)0x0)))) {
      cls_0x5b99c0::meth_0x53f560((cls_0x5b99c0 *)this->mbr_0xd8);
      TPlayScreen::meth_0x4d3750(this_00,&this->mbr_0x10);
    }
  }
  return;
}



// Function at 00502470

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SoftwareRenderer::virt_meth_0x502470()

{
  int iVar1;
  dword dVar2;
  int iVar3;
  dword dVar4;
  
  virt_meth_0x4de800(this);
  if (this->mbr_0xd8 == 0) {
    meth_0x4defe0(this);
  }
  if (this->mbr_0x188 == 0) {
    dVar2 = this->mbr_0x184;
    dVar4 = dVar2 + 1;
    this->mbr_0x184 = dVar4;
    iVar3 = *(int *)(this->mbr_0xd8 + 4);
    switch(dVar2) {
    case 0:
    case 3:
      this->mbr_0x188 = 10;
      break;
    case 1:
      iVar1 = *(int *)(*(int *)(this->mbr_0xd8 + 0x120) + 0x54);
      this->mbr_0x188 = 100;
      *(float *)(iVar3 + 400) = (float)iVar1 * _DAT_005a350c;
      break;
    case 2:
      *(undefined4 *)(iVar3 + 400) = 0;
      this->mbr_0x188 = 0xf;
    }
    if (4 < (int)dVar4) {
      meth_0x4defe0(this);
    }
  }
  this->mbr_0x188 = this->mbr_0x188 - 1;
  return;
}



// Function at 00504920

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SoftwareRenderer::virt_meth_0x504920()

{
  TPlayScreen *this_00;
  int *piVar1;
  dword dVar2;
  float10 fVar3;
  float10 fVar4;
  
  virt_meth_0x4de800(this);
  if (this->mbr_0x190 != 0) {
    this_00 = (TPlayScreen *)this->mbr_0xe4;
    if (this_00 == (TPlayScreen *)0x0) {
      this->mbr_0x194 = 0;
    }
    else if (*(short *)&(this_00->TScreen).mbr_0x4 == 0xc) {
      piVar1 = (int *)(this_00->TScreen).mbr_0xe0;
      if ((piVar1 != (int *)0x0) &&
         (((*piVar1 == 3 || ((piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) && (piVar1[0x11] != 0))))
      {
        piVar1 = (int *)(this_00->TScreen).mbr_0xe0;
        if ((piVar1 == (int *)0x0) ||
           ((*piVar1 != 3 && ((piVar1 == (int *)0x0 || (*piVar1 != 0x19)))))) {
          dVar2 = TPlayScreen::meth_0x46ea90_AngleTo(this_00);
          this->mbr_0x194 = dVar2;
        }
        else {
          dVar2 = TPlayScreen::meth_0x46ea90_AngleTo(this_00);
          this->mbr_0x194 = dVar2;
        }
      }
    }
    else {
      this->mbr_0x194 = (this_00->TScreen).mbr_0x36 - 0x40 & 0xff;
    }
    this->mbr_0x190 = 0;
    fVar3 = (float10)this->mbr_0x194 * (float10)_DAT_005a4978;
    fVar4 = (float10)fcos(fVar3);
    this->mbr_0x184 = (dword)(float)(fVar4 * (float10)ram0x005b0f68);
    fVar3 = (float10)fsin(fVar3);
    this->mbr_0x188 = (dword)(float)(fVar3 * (float10)ram0x005b0f68);
  }
  if (0 < (int)this->mbr_0x198) {
    this->mbr_0x198 = this->mbr_0x198 - 1;
  }
  return;
}



// Function at 00505290

void SoftwareRenderer::virt_meth_0x505290()

{
  dword dVar1;
  dword dVar2;
  
  virt_meth_0x4de800(this);
  dVar2 = DAT_00666990;
  dVar1 = DAT_0066698c;
  this->mbr_0x10 = DAT_00666988;
  this->mbr_0x14 = dVar1;
  this->mbr_0x18 = dVar2;
  this->mbr_0x10 = this->mbr_0x10 - 0x168;
  this->mbr_0x14 = this->mbr_0x14 - 0x168;
  this->mbr_0x18 = this->mbr_0x18 - 0x168;
  return;
}



// Function at 00507330

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SoftwareRenderer::meth_0x507330()

{
  int *piVar1;
  int iVar2;
  dword dVar3;
  float fVar4;
  TPlayScreen *this_00;
  undefined uVar5;
  int iVar6;
  dword dVar7;
  int3 extraout_var;
  TPlayScreen *this_01;
  cls_0x44ceb0 local_48;
  
  iVar6 = FUN_00483300_RandomRange(0,1);
  if (iVar6 == 0) {
    dVar7 = FUN_00483300_RandomRange(0x96,200);
    this->mbr_0x1a0 = dVar7;
    iVar6 = FUN_00483300_RandomRange(0x96,200);
    dVar7 = -iVar6;
  }
  else {
    iVar6 = FUN_00483300_RandomRange(0x96,200);
    this->mbr_0x1a0 = -iVar6;
    dVar7 = FUN_00483300_RandomRange(0x96,200);
  }
  this->mbr_0x1a4 = dVar7;
  this_01 = (TPlayScreen *)0x0;
  fVar4 = (float)this->mbr_0x1a0 * _DAT_005a9d80;
  this->mbr_0x1a8 = 0x15e;
  this->mbr_0x19c = 0x418c0000;
  this->mbr_0x188 = 0;
  this->mbr_0x194 = (dword)fVar4;
  this->mbr_0x198 = (dword)((float)this->mbr_0x1a4 * _DAT_005a9d80);
  if (this->mbr_0xd8 != 0) {
    this_01 = *(TPlayScreen **)(this->mbr_0xd8 + 4);
  }
  cls_0x44ceb0::cls_0x44ceb0(&local_48,&this->mbr_0x10,400,0xe0,2,0);
  this_00 = (TPlayScreen *)local_48.mbr_0xc;
  do {
    if (this_00 == (TPlayScreen *)0x0) {
LAB_00507481:
      dVar7 = this->mbr_0x188;
      if (dVar7 == 0) {
        if (this_01 != (TPlayScreen *)0x0) {
          dVar7 = (this_01->TScreen).mbr_0x14;
          dVar3 = (this_01->TScreen).mbr_0x18;
          this->mbr_0x1a0 = this->mbr_0x1a0 + (this_01->TScreen).mbr_0x10;
          this->mbr_0x1a4 = this->mbr_0x1a4 + dVar7;
          this->mbr_0x1a8 = this->mbr_0x1a8 + dVar3;
        }
      }
      else {
        iVar6 = *(int *)(dVar7 + 0x14);
        iVar2 = *(int *)(dVar7 + 0x18);
        this->mbr_0x1a0 = this->mbr_0x1a0 + *(int *)(dVar7 + 0x10);
        this->mbr_0x1a4 = this->mbr_0x1a4 + iVar6;
        this->mbr_0x1a8 = this->mbr_0x1a8 + iVar2;
      }
      this->mbr_0x184 = 0;
      this->mbr_0x190 = 2;
      return;
    }
    local_48.mbr_0xc = (dword)this_00;
    if (((((this_00 != this_01) &&
          (iVar6 = FUN_0046de60_Distance2D((int *)&this->mbr_0x10,(int *)&(this_00->TScreen).mbr_0x10),
          iVar6 < 0x191)) &&
         (uVar5 = (*((this_00->TScreen).vftptr_0x0)->virt_meth_0x477e50_448)(this_00),
         CONCAT31(extraout_var,uVar5) != 0 && -1 < extraout_var)) &&
        ((this_01 == (TPlayScreen *)0x0 ||
         (iVar6 = TPlayScreen::meth_0x4c89c0_IsEnemy(this_01), iVar6 != 0)))) &&
       ((this_00 == (TPlayScreen *)0x0 ||
        ((piVar1 = (int *)(this_00->TScreen).mbr_0xd8, piVar1 == (int *)0x0 || (*piVar1 != 0xc)
         ))))) {
      this->mbr_0x188 = (dword)this_00;
      goto LAB_00507481;
    }
    cls_0x44ceb0::meth_0x44d080(&local_48);
    this_00 = (TPlayScreen *)local_48.mbr_0xc;
  } while( true );
}



// Function at 00508ff0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x508ff0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509020

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x509020()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a059e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x210);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5090c0_005b053c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005091c0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5091c0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005091f0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x5091f0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a05ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1bc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x509290_005b07b0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509550

SoftwareRenderer * SoftwareRenderer::virt_meth_0x509550()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509580

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x509580()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a06ae;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x124);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x509620_005b0c98;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509a00

SoftwareRenderer * SoftwareRenderer::virt_meth_0x509a00()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509b90

SoftwareRenderer * SoftwareRenderer::virt_meth_0x509b90()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509bc0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x509bc0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a07ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x13bc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x509c60_005b1448;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509d20

SoftwareRenderer * SoftwareRenderer::virt_meth_0x509d20()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509d50

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x509d50(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a082b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x198);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x509df0_005b16ac;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0;
    *(undefined4 *)&this_00[1].TScreen.mbr_0x37 = 1;
    (this_00->TScreen).mbr_0x8 = 0x4c009;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00509df0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x509df0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a1f0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50a1f0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a220

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x50a220()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a095e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x13f8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x50a2c0_005b1f8c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050a390

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50a390()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a3c0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x50a3c0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a09ae;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x304);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x50a460_005b21f0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050a490

void SoftwareRenderer::virt_meth_0x50a490()

{
  meth_0x4defe0(this);
  (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
  return;
}



// Function at 0050a4b0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50a4b0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050d1a0

undefined4 SoftwareRenderer::virt_meth_0x50d1a0()

{
  undefined uVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined3 extraout_var_00;
  undefined4 uVar3;
  undefined4 unaff_EDI;
  int in_stack_00000008;
  int in_stack_0000000c;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_EDI);
  if (CONCAT31(extraout_var,uVar1) == 0) {
    return 0;
  }
  FUN_00452690_TMapPane_GetInstance(in_stack_0000000c,0);
  iVar2 = TPlayScreen::meth_0x4dd480_UseContainer((TPlayScreen *)this,in_stack_00000008);
  if ((iVar2 == 0) &&
     (uVar1 = (*this->vftptr_0x0[1].virt_meth_0x4778c0_20)(this),
     CONCAT31(extraout_var_00,uVar1) != 0)) {
    if (in_stack_00000008 != DAT_00667fcc) {
      return 0;
    }
    uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
    FUN_0054d170((TCharacter *)&DAT_0065c5d0,uVar3);
    return 0;
  }
  virt_meth_0x4705f0(this,in_stack_00000008);
  return 1;
}



// Function at 0050d370

int SoftwareRenderer::virt_meth_0x50d370()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined4 unaff_ESI;
  int in_stack_00000008;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI);
  if ((CONCAT31(extraout_var,uVar1) != 0) && ((*(byte *)&this->mbr_0x8 & 0x80) == 0)) {
    return (-(uint)(in_stack_00000008 != 0) & 0xfffffffd) + 3;
  }
  return -1;
}



// Function at 0050d3a0

undefined4 SoftwareRenderer::virt_meth_0x50d3a0()

{
  byte *pbVar1;
  uint *puVar2;
  undefined uVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint uVar4;
  TPlayScreen *in_stack_00000004;
  int in_stack_00000008;
  
  if ((DAT_0066829c != 0) && (DAT_0067682c == 0)) {
    return 0;
  }
  if (in_stack_00000004 == (TPlayScreen *)0x0) {
    in_stack_00000004 = DAT_00667fcc;
  }
  uVar3 = (*this->vftptr_0x0[1].virt_meth_0x4778c0_20)(this);
  if (CONCAT31(extraout_var,uVar3) != 0) {
    return 0;
  }
  if ((this->mbr_0x84 != 0) && (in_stack_00000008 == 0)) {
    FUN_00492640(this->mbr_0x84,6,(byte *)0x0,(byte *)0x0,(int *)in_stack_00000004,&DAT_005e17d8_str_event_user,0,0
                );
    return 1;
  }
  uVar3 = (*this->vftptr_0x0->virt_meth_0x477aa0_220)(this);
  if ((CONCAT31(extraout_var_00,uVar3) == 0) && (in_stack_00000008 == 0)) {
    return 0;
  }
  pbVar1 = (byte *)this->mbr_0x38;
  puVar2 = DAT_0066d1c4_TExit_exitlist;
  do {
    if (puVar2 == (uint *)0x0) {
LAB_0050d463:
      if (this->mbr_0x84 != 0) {
        return 1;
      }
      return 0;
    }
    uVar4 = FUN_0059a530_stricmp(*puVar2,(byte *)*puVar2,pbVar1);
    if (uVar4 == 0) {
      if (puVar2 != (uint *)0x0) {
        if (in_stack_00000004 == (TPlayScreen *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4))
        {
          DAT_006669b0 = DAT_006669b0 | 8;
        }
        TPlayScreen::meth_0x4d4790_SetFighting(in_stack_00000004);
        (*((in_stack_00000004->TScreen).vftptr_0x0)->virt_meth_0x46ed70_8)(in_stack_00000004);
        if (DAT_0066829c == 0) {
          return 1;
        }
        if (DAT_0067682c == 0) {
          return 1;
        }
        cls_0x57d9d0::meth_0x586bb0
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,in_stack_00000004,&stack0xffffffe4);
        return 1;
      }
      goto LAB_0050d463;
    }
    puVar2 = (uint *)puVar2[8];
  } while( true );
}



// Function at 0050d510

void SoftwareRenderer::virt_meth_0x50d510()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined4 unaff_ESI;
  
  uVar1 = (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    TPlayScreen::meth_0x50d530((TPlayScreen *)this);
  }
  return;
}



// Function at 0050d640

void SoftwareRenderer::virt_meth_0x50d640()

{
  byte bVar1;
  undefined uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  int iVar3;
  undefined3 extraout_var_04;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  cls_0x4cdf30 *this_00;
  int *piVar7;
  byte *pbVar8;
  char *pcVar9;
  undefined3 extraout_var_05;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  undefined4 unaff_ESI;
  int iVar10;
  byte *pbVar11;
  bool bVar12;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  int iStack_8;
  
  virt_meth_0x4708e0_OnScreen(this);
  if (DAT_00668154 == 0) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x477d30_340)(this);
    if ((((CONCAT31(extraout_var,uVar2) != 0) &&
         (uVar2 = (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI),
         CONCAT31(extraout_var_00,uVar2) != 0)) &&
        (uVar2 = (*this->vftptr_0x0->virt_meth_0x477900_36)(this),
        CONCAT31(extraout_var_01,uVar2) != 0)) &&
       (uVar2 = (*this->vftptr_0x0->virt_meth_0x477900_36)(this),
       *(int *)(CONCAT31(extraout_var_02,uVar2) + 8) != 0)) {
      uVar2 = (*this->vftptr_0x0->virt_meth_0x477900_36)(this);
      iVar3 = (**(code **)(**(int **)(CONCAT31(extraout_var_03,uVar2) + 8) + 0x90))(this->mbr_0xc);
      if (1 < iVar3) {
        uVar2 = (*this->vftptr_0x0->virt_meth_0x477900_36)(this);
        pbVar4 = (byte *)(**(code **)(**(int **)(CONCAT31(extraout_var_04,uVar2) + 8) + 0x88))
                                   (this->mbr_0xc);
        uVar5 = FUN_0059a530_stricmp(extraout_ECX,pbVar4,(byte *)s_CLOSING_005e1860);
        uVar6 = extraout_ECX_00;
        if (uVar5 == 0) {
          TPlayScreen::meth_0x50d530((TPlayScreen *)this);
          uVar6 = extraout_ECX_01;
        }
        uVar6 = FUN_0059a530_stricmp(uVar6,pbVar4,(byte *)s_OPENING_005e1868);
        if (uVar6 == 0) {
          TPlayScreen::meth_0x50d530((TPlayScreen *)this);
        }
      }
    }
    if (((DAT_00668154 == 0) &&
        (iVar3 = TPlayScreen::meth_0x46e8a0((TPlayScreen *)this), iVar3 != 0)) &&
       ((DAT_0067682c != 0 || (DAT_0066829c == 0)))) {
      (*this->vftptr_0x0[1].virt_meth_0x46fea0_136)(this);
      this->mbr_0xe4 = this->mbr_0xe4 & 0xfffffffa;
      iVar10 = 0;
      iVar3 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
      if (0 < iVar3) {
        do {
          this_00 = (cls_0x4cdf30 *)
                    cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,iVar10);
          if (this_00 != (cls_0x4cdf30 *)0x0) {
            iVar3 = (iStack_14 * 0x10 - this->mbr_0x10) + *(int *)&this_00->field_0x10;
            iVar10 = iStack_10 * 0x10 + (*(int *)&this_00->field_0x14 - this->mbr_0x14);
            iVar3 = (int)(iVar3 + (iVar3 >> 0x1f & 0xfU)) >> 4;
            iVar10 = (int)(iVar10 + (iVar10 >> 0x1f & 0xfU)) >> 4;
            piVar7 = (int *)TPlayScreen::meth_0x46e8a0((TPlayScreen *)this);
            (**(code **)(*piVar7 + 0xc4))(this->mbr_0xc);
            if (((-1 < iVar3) && (-1 < iVar10)) && ((iVar3 < iStack_c && (iVar10 < iStack_8)))) {
              pbVar8 = &DAT_005e1870;
              pbVar4 = *(byte **)this->mbr_0x4c;
              pbVar11 = pbVar4;
              do {
                bVar1 = *pbVar8;
                bVar12 = bVar1 < *pbVar11;
                if (bVar1 != *pbVar11) {
LAB_0050d84f:
                  iVar3 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
                  goto LAB_0050d854;
                }
                if (bVar1 == 0) break;
                bVar1 = pbVar8[1];
                bVar12 = bVar1 < pbVar11[1];
                if (bVar1 != pbVar11[1]) goto LAB_0050d84f;
                pbVar8 = pbVar8 + 2;
                pbVar11 = pbVar11 + 2;
              } while (bVar1 != 0);
              iVar3 = 0;
LAB_0050d854:
              if (iVar3 != 0) {
                pbVar8 = &DAT_005e1878;
                pbVar11 = pbVar4;
                do {
                  bVar1 = *pbVar8;
                  bVar12 = bVar1 < *pbVar11;
                  if (bVar1 != *pbVar11) {
LAB_0050d887:
                    iVar3 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
                    goto LAB_0050d88c;
                  }
                  if (bVar1 == 0) break;
                  bVar1 = pbVar8[1];
                  bVar12 = bVar1 < pbVar11[1];
                  if (bVar1 != pbVar11[1]) goto LAB_0050d887;
                  pbVar8 = pbVar8 + 2;
                  pbVar11 = pbVar11 + 2;
                } while (bVar1 != 0);
                iVar3 = 0;
LAB_0050d88c:
                if (iVar3 != 0) {
                  pcVar9 = s_PortEW_005e1880;
                  pbVar11 = pbVar4;
                  do {
                    bVar1 = *pcVar9;
                    bVar12 = bVar1 < *pbVar11;
                    if (bVar1 != *pbVar11) {
LAB_0050d8bf:
                      iVar3 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
                      goto LAB_0050d8c4;
                    }
                    if (bVar1 == 0) break;
                    bVar1 = ((byte *)pcVar9)[1];
                    bVar12 = bVar1 < pbVar11[1];
                    if (bVar1 != pbVar11[1]) goto LAB_0050d8bf;
                    pcVar9 = (char *)((byte *)pcVar9 + 2);
                    pbVar11 = pbVar11 + 2;
                  } while (bVar1 != 0);
                  iVar3 = 0;
LAB_0050d8c4:
                  if (iVar3 != 0) {
                    pbVar11 = &DAT_005e1888;
                    do {
                      bVar1 = *pbVar11;
                      bVar12 = bVar1 < *pbVar4;
                      if (bVar1 != *pbVar4) {
LAB_0050d8f3:
                        iVar3 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
                        goto LAB_0050d8f8;
                      }
                      if (bVar1 == 0) break;
                      bVar1 = pbVar11[1];
                      bVar12 = bVar1 < pbVar4[1];
                      if (bVar1 != pbVar4[1]) goto LAB_0050d8f3;
                      pbVar11 = pbVar11 + 2;
                      pbVar4 = pbVar4 + 2;
                    } while (bVar1 != 0);
                    iVar3 = 0;
LAB_0050d8f8:
                    if (iVar3 != 0) {
                      uVar6 = this->mbr_0xe4;
                      uVar5 = this_00->mbr_0x8;
                      this->mbr_0xe4 = uVar6 | 1;
                      if ((uVar5 & 0x100000) == 0) {
                        (*this->vftptr_0x0[1].virt_meth_0x46f940_84)(this);
                      }
                      else {
                        this->mbr_0xe4 = uVar6 | 5;
                      }
                      cls_0x4cdf30::meth_0x4cdf30(this_00);
                      *(SoftwareRenderer **)&this_00->field_0xe4 = this;
                    }
                  }
                }
              }
            }
          }
          iVar10 = iStack_18 + 1;
          iVar3 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
          iStack_18 = iVar10;
        } while (iVar10 < iVar3);
      }
    }
  }
  uVar2 = (*this->vftptr_0x0->virt_meth_0x477900_36)(this);
  if (CONCAT31(extraout_var_05,uVar2) == 0) {
    (*this->vftptr_0x0->virt_meth_0x471b50_344)(this);
  }
  return;
}



// Function at 0050d9c0

void SoftwareRenderer::virt_meth_0x50d9c0()

{
  dword dVar1;
  dword *pdVar2;
  cls_0x5a5ff0 *in_stack_00000004;
  
  virt_meth_0x4dd470(this);
  dVar1 = this->mbr_0xe4;
  if ((int)((in_stack_00000004->mbr_0xc + in_stack_00000004->mbr_0x4) - in_stack_00000004->mbr_0x8)
      < 4) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
  }
  pdVar2 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar2 = dVar1;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar2 + 1);
  return;
}



// Function at 0050df50

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50df50()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050df80

TScreen * SoftwareRenderer::virt_meth_0x50df80(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a3b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50e030_005b24e0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 0050e400

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50e400()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050e4e0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50e4e0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050ea80

undefined4 SoftwareRenderer::virt_meth_0x50ea80()

{
  undefined uVar1;
  int *piVar2;
  int iVar3;
  undefined3 extraout_var;
  int *in_stack_00000004;
  
  virt_meth_0x46fee0(this);
  piVar2 = (int *)(**(code **)(*in_stack_00000004 + 0xa8))(*(undefined4 *)this->mbr_0x4c);
  if (piVar2 != (int *)0x0) {
    iVar3 = (**(code **)(*piVar2 + 0x198))();
    uVar1 = (*this->vftptr_0x0->virt_meth_0x477db0_408)(this);
    (**(code **)(*piVar2 + 0x19c))(iVar3 + CONCAT31(extraout_var,uVar1));
    return 1;
  }
  return 0;
}



// Function at 0050eb80

TScreen * SoftwareRenderer::virt_meth_0x50eb80(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0aeb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50ed80_005b2cd4;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050ed80

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50ed80()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050edb0

TScreen * SoftwareRenderer::virt_meth_0x50edb0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0b0b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50ee10_005b2ee4;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050ee10

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50ee10()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050f640

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x50f640()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0b3e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b30fc__vftable_5b30fc_005b30fc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050f820

TScreen * SoftwareRenderer::virt_meth_0x50f820(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0bbb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50f890_005b322c;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 1;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050f890

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50f890()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050fa30

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50fa30()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050fbd0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50fbd0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050fd30

TScreen * SoftwareRenderer::virt_meth_0x50fd30(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0c7b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50fdf0_005b3800;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050fdf0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x50fdf0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050ffd0

TScreen * SoftwareRenderer::virt_meth_0x50ffd0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0c9b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x510180_005b39fc;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00517050

undefined4 SoftwareRenderer::meth_0x517050(char *param_1, uint param_2)

{
  char cVar1;
  dword *pdVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  undefined4 *puVar9;
  char *pcVar10;
  undefined4 *puVar11;
  undefined4 in_stack_0000000c;
  
  iVar8 = 0;
  pdVar2 = &this->mbr_0xd8;
  while (*pdVar2 != 0) {
    iVar8 = iVar8 + 1;
    pdVar2 = pdVar2 + 1;
    if (4 < iVar8) {
      return 0;
    }
  }
  if (4 < iVar8) {
    return 0;
  }
  puVar3 = FUN_00482ef0(0x10);
  (&this->mbr_0xd8)[iVar8] = (dword)puVar3;
  uVar6 = 0xffffffff;
  pcVar10 = param_1;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar10 + 1;
  } while (cVar1 != '\0');
  puVar4 = FUN_00482ef0(~uVar6);
  uVar6 = 0xffffffff;
  do {
    pcVar10 = param_1;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar10 = param_1 + 1;
    cVar1 = *param_1;
    param_1 = pcVar10;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  puVar9 = (undefined4 *)(pcVar10 + -uVar6);
  puVar11 = puVar4;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *puVar11 = *puVar9;
    puVar9 = puVar9 + 1;
    puVar11 = puVar11 + 1;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined *)puVar11 = *(undefined *)puVar9;
    puVar9 = (undefined4 *)((int)puVar9 + 1);
    puVar11 = (undefined4 *)((int)puVar11 + 1);
  }
  *puVar3 = puVar4;
  if (0x4e1e < param_2) {
    param_2 = 19999;
  }
  puVar3[1] = param_2;
  puVar3[2] = in_stack_0000000c;
  uVar5 = TCharacter::meth_0x47e940((TCharacter *)&DAT_0065caf0);
  *(undefined4 *)((&this->mbr_0xd8)[iVar8] + 0xc) = uVar5;
  return 1;
}



// Function at 00522010

SoftwareRenderer * SoftwareRenderer::virt_meth_0x522010()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00523530

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x523530(undefined4 param_1)

{
  dword dVar1;
  dword dVar2;
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a135b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x1a0);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    dVar1 = (this_00->TScreen).mbr_0x18;
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x523880_005b5ab8;
    dVar2 = (this_00->TScreen).mbr_0x10;
    *(dword *)&this_00[1].TScreen.mbr_0x37 = (this_00->TScreen).mbr_0x14;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x2c + 3) = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0x14;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0xffffffff;
    *(dword *)((int)&this_00[1].TScreen.mbr_0x30 + 3) = dVar2;
    *(dword *)((int)&this_00[1].TScreen.mbr_0x38 + 3) = dVar1;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005235e0

cls_0x5a85ac * SoftwareRenderer::virt_meth_0x5235e0(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  int iVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1383;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x198);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5238b0_005b5cbc;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x30 + 3) = 0;
    iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    if (-1 < iVar1) {
      iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
      if (iVar2 != 0) {
        cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
      }
    }
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005236b0

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x5236b0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a13ae;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x5238e0_005b5ebc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00523880

SoftwareRenderer * SoftwareRenderer::virt_meth_0x523880()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005238b0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5238b0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00523a70

void SoftwareRenderer::virt_meth_0x523a70()

{
  uint uVar1;
  
  if (this->mbr_0x64 != 0) {
    uVar1 = FUN_0059a530_stricmp((uint)this,**(byte ***)(this->mbr_0x64 + 0x4c),
                         (byte *)s_Spell_Pouch_005e2dc4);
    if (uVar1 != 0) {
      uVar1 = FUN_0059a530_stricmp(this->mbr_0x64,**(byte ***)(this->mbr_0x64 + 0x4c),
                           (byte *)s_SpellPouch_005e2dd0);
      if (uVar1 != 0) goto LAB_00523ac5;
    }
    UNK_006661c4._60_4_ = 1;
    (**(code **)(DAT_006661b0 + 0x90))();
  }
LAB_00523ac5:
  virt_meth_0x46faf0_RemoveFromInventory(this);
  return;
}



// Function at 00523b90

SoftwareRenderer * SoftwareRenderer::virt_meth_0x523b90()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00523da0

undefined4 SoftwareRenderer::virt_meth_0x523da0()

{
  byte bVar1;
  
  bVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
  if ((((this->mbr_0x8 & 0x200) == 0) && ((bVar1 & 0x40) == 0)) &&
     ((((this->mbr_0x8 & 0x20000000) != 0 || ((bVar1 & 0x80) != 0)) ||
      (((bVar1 & 0xf) != 3 && ((bVar1 & 0x20) == 0)))))) {
    return 0;
  }
  return 1;
}



// Function at 00523de0

TScreen * SoftwareRenderer::virt_meth_0x523de0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a147b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x523fd0_005b6398;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 1;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00523fd0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x523fd0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005240f0

TScreen * SoftwareRenderer::virt_meth_0x5240f0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a149b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5241b0_005b65cc;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 005241b0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5241b0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00524ce0

void SoftwareRenderer::virt_meth_0x524ce0()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int3 extraout_var_03;
  uint uVar2;
  TPlayScreen *this_00;
  undefined4 unaff_ESI;
  
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477930_60)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
    if (CONCAT31(extraout_var_00,uVar1) == 0) {
      uVar1 = (*this->vftptr_0x0[1].virt_meth_0x528b00_0)(this);
      if (CONCAT31(extraout_var_01,uVar1) != 0) {
        this->mbr_0xdc = 0;
        (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI);
        (*this->vftptr_0x0[1].virt_meth_0x46ed70_8)(this);
      }
    }
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
  if (CONCAT31(extraout_var_02,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x46e900_44)(this);
    if (CONCAT31(extraout_var_03,uVar1) != 0 && -1 < extraout_var_03) {
      uVar2 = TPlayScreen::meth_0x58eb90(this_00);
      if (this->mbr_0xe4 <= uVar2) {
        (*this->vftptr_0x0[1].virt_meth_0x477920_56)(this);
        this->mbr_0xd8 = 0;
        (*this->vftptr_0x0->virt_meth_0x4778c0_20)(this);
      }
    }
  }
  return;
}



// Function at 005267d0

void SoftwareRenderer::virt_meth_0x5267d0()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int3 extraout_var_02;
  uint uVar2;
  TPlayScreen *this_00;
  
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477930_60)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
    if ((CONCAT31(extraout_var_00,uVar1) == 0) && (this->mbr_0xd8 == 0)) {
      (*this->vftptr_0x0[1].virt_meth_0x46ed70_8)(this);
      (*this->vftptr_0x0[1].virt_meth_0x477940_68)(this);
      this->mbr_0xd8 = 1;
    }
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
  if (CONCAT31(extraout_var_01,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x46e900_44)(this);
    if (CONCAT31(extraout_var_02,uVar1) != 0 && -1 < extraout_var_02) {
      uVar2 = TPlayScreen::meth_0x58eb90(this_00);
      if (this->mbr_0xe4 <= uVar2) {
        (*this->vftptr_0x0[1].virt_meth_0x477920_56)(this);
        this->mbr_0xd8 = 0;
      }
    }
  }
  (*this->vftptr_0x0[1].virt_meth_0x528b00_0)(this);
  return;
}



// Function at 00526e20

void SoftwareRenderer::virt_meth_0x526e20()

{
  dword dVar1;
  
  if (this->mbr_0xd8 == 0) {
    dVar1 = this->mbr_0x110;
    this->mbr_0xd8 = 1;
    if (dVar1 == 0x80) {
      dVar1 = meth_0x526f00(this,0xfffffffb,0x23);
      this->mbr_0xfc = dVar1;
      dVar1 = meth_0x526f00(this,0x14,0x23);
      this->mbr_0x100 = dVar1;
      return;
    }
    if (dVar1 == 0x40) {
      dVar1 = meth_0x526f00(this,0x28,0xfffffffb);
      this->mbr_0xfc = dVar1;
      dVar1 = meth_0x526f00(this,0x28,0x14);
      this->mbr_0x100 = dVar1;
      return;
    }
    if (dVar1 == 0) {
      dVar1 = meth_0x526f00(this,0,0xffffffec);
      this->mbr_0xfc = dVar1;
      dVar1 = meth_0x526f00(this,0x1e,0xffffffe2);
      this->mbr_0x100 = dVar1;
      return;
    }
    if (dVar1 == 0xc0) {
      dVar1 = meth_0x526f00(this,0xfffffff1,0xfffffff6);
      this->mbr_0xfc = dVar1;
      dVar1 = meth_0x526f00(this,0xfffffff1,0xf);
      this->mbr_0x100 = dVar1;
    }
  }
  return;
}



// Function at 00526f00

void SoftwareRenderer::meth_0x526f00(int param_1, int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int in_stack_0000000c;
  undefined4 uVar4;
  undefined4 local_34;
  undefined4 local_30;
  undefined2 local_2a;
  int local_28;
  int local_24;
  int local_20;
  
  puVar3 = &local_34;
  for (iVar2 = 0xd; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_34._0_2_ = 0x19;
  iVar2 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  if (-1 < iVar2) {
    iVar1 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548);
    if (iVar1 != 0) {
      cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar2,0x7f,1);
    }
  }
  local_34._2_2_ =
       cls_0x4746d0::meth_0x475210((cls_0x4746d0 *)&UNK_0066cc1c.field_0x4c,s_headfireball_005e30e0)
  ;
  local_24 = this->mbr_0x14 + param_2;
  uVar4 = 0;
  local_20 = this->mbr_0x18 + in_stack_0000000c;
  local_28 = this->mbr_0x10 + param_1;
  local_2a = DAT_00666970;
  local_30 = 8;
  iVar2 = TCharacter::meth_0x450e40((TCharacter *)&DAT_006668d8,&local_34);
  FUN_00452690_TMapPane_GetInstance(iVar2,uVar4);
  return;
}



// Function at 005273d0

void SoftwareRenderer::virt_meth_0x5273d0(int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  virt_meth_0x471ba0_Notify(this,param_1);
  if (param_1 == 1) {
    if (this->mbr_0x100 == in_stack_00000008) {
LAB_00527413:
      this->mbr_0x100 = 0;
LAB_00527419:
      if (param_1 != 1) {
        if (param_1 != 2) {
          return;
        }
        goto LAB_00527431;
      }
    }
    if (this->mbr_0xfc != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) goto LAB_00527419;
    dVar1 = this->mbr_0x100;
    if (((dVar1 != 0) && (*(dword *)(dVar1 + 0x44) == in_stack_00000008)) &&
       ((*(uint *)(dVar1 + 8) & 0x80000) == 0)) goto LAB_00527413;
LAB_00527431:
    dVar1 = this->mbr_0xfc;
    if (dVar1 == 0) {
      return;
    }
    if (*(dword *)(dVar1 + 0x44) != in_stack_00000008) {
      return;
    }
    if ((*(uint *)(dVar1 + 8) & 0x80000) != 0) {
      return;
    }
  }
  this->mbr_0xfc = 0;
  return;
}



// Function at 00527460

TScreen * SoftwareRenderer::virt_meth_0x527460(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  uint extraout_ECX;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a14bb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527550_005b67cc;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_ArrowWallS_005e2f50);
    if (uVar1 == 0) {
      *(undefined4 *)&this_00->field_0xfc = 0x80;
    }
    else {
      *(undefined4 *)&this_00->field_0xfc = 0x40;
    }
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0xd8 = 0;
    *(undefined4 *)&this_00->field_0x100 = 1;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527550

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527550()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527580

TScreen * SoftwareRenderer::virt_meth_0x527580(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  uint extraout_ECX;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a14db;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527550_005b67cc;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_ArrowWallS_005e2f50);
    if (uVar1 == 0) {
      *(undefined4 *)&this_00->field_0xfc = 0x80;
    }
    else {
      *(undefined4 *)&this_00->field_0xfc = 0x40;
    }
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0xd8 = 0;
    *(undefined4 *)&this_00->field_0x100 = 1;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527660_005b6a08;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527660

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527660()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527690

TScreen * SoftwareRenderer::virt_meth_0x527690(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1503;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527750_005b6c44;
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0xf8 = 0;
    *(undefined4 *)&this_00->field_0xfc = 0;
    *(undefined2 *)&this_00->field_0x102 = 0;
    *(undefined2 *)&this_00->field_0x100 = 0;
    (*(code *)PTR_virt_meth_0x4778c0_005b6c58)(s_closed_005e2f78);
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527750

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527750()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527780

TScreen * SoftwareRenderer::virt_meth_0x527780(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1523;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527750_005b6c44;
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0xf8 = 0;
    *(undefined4 *)&this_00->field_0xfc = 0;
    *(undefined2 *)&this_00->field_0x102 = 0;
    *(undefined2 *)&this_00->field_0x100 = 0;
    (*(code *)PTR_virt_meth_0x4778c0_005b6c58)(s_closed_005e2f78);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527840_005b6e80;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527840

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527840()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527870

TScreen * SoftwareRenderer::virt_meth_0x527870(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a153b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527920_005b70bc;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->mbr_0xd8 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527920

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527920()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527950

TScreen * SoftwareRenderer::virt_meth_0x527950(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a155b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527a00_005b72f8;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->mbr_0xd8 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527a00

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527a00()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527a30

TScreen * SoftwareRenderer::virt_meth_0x527a30(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  uint extraout_ECX;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a157b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527b20_005b7534;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_DunFireWallS_005e2fc4);
    if (uVar1 == 0) {
      *(undefined4 *)&this_00->field_0xfc = 0x80;
    }
    else {
      *(undefined4 *)&this_00->field_0xfc = 0x40;
    }
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x40000;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527b20

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527b20()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527b50

TScreen * SoftwareRenderer::virt_meth_0x527b50(undefined4 param_1)

{
  TScreen *this_00;
  uint uVar1;
  uint extraout_ECX;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a159b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527b20_005b7534;
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,(byte *)this_00->mbr_0x38,(byte *)s_DunFireWallS_005e2fc4);
    if (uVar1 == 0) {
      *(undefined4 *)&this_00->field_0xfc = 0x80;
    }
    else {
      *(undefined4 *)&this_00->field_0xfc = 0x40;
    }
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x40000;
    *(undefined4 *)&this_00->field_0xdc = 0xffffffff;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527c30_005b7770;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527ef0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527ef0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00527f20

TScreen * SoftwareRenderer::virt_meth_0x527f20(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1633;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00527fd0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x527fd0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528000

TScreen * SoftwareRenderer::virt_meth_0x528000(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1653;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5280a0_005b80cc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 005280a0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5280a0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005280d0

TScreen * SoftwareRenderer::virt_meth_0x5280d0(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1673;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x528170_005b8308;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00528170

SoftwareRenderer * SoftwareRenderer::virt_meth_0x528170()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005281a0

TScreen * SoftwareRenderer::virt_meth_0x5281a0(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1693;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x528240_005b8544;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00528240

SoftwareRenderer * SoftwareRenderer::virt_meth_0x528240()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528270

TScreen * SoftwareRenderer::virt_meth_0x528270(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a16b3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x528310_005b8780;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 00528310

SoftwareRenderer * SoftwareRenderer::virt_meth_0x528310()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528340

TScreen * SoftwareRenderer::virt_meth_0x528340(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a16d3;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x114);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    local_4 = CONCAT31(local_4._1_3_,1);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x527fd0_005b7e90;
    TScreen::meth_0x526c90(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5283e0_005b89bc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 005283e0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5283e0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528430

void SoftwareRenderer::virt_meth_0x528430()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  TPlayScreen *this_00;
  
  (*this->vftptr_0x0[1].virt_meth_0x477920_56)(this);
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x46e900_44)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x46e900_44)(this);
    iVar2 = TPlayScreen::meth_0x58eb90(this_00);
    this->mbr_0xe4 = CONCAT31(extraout_var_00,uVar1) + iVar2;
  }
  return;
}



// Function at 00528600

TScreen * SoftwareRenderer::virt_meth_0x528600(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a16eb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x528770_005b8bf4;
    this_00->mbr_0xd8 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



// Function at 005286e0

void SoftwareRenderer::virt_meth_0x5286e0()

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int3 extraout_var_03;
  uint uVar2;
  TPlayScreen *this_00;
  undefined4 unaff_ESI;
  
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477930_60)(this);
  if (CONCAT31(extraout_var,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
    if (CONCAT31(extraout_var_00,uVar1) == 0) {
      uVar1 = (*this->vftptr_0x0[1].virt_meth_0x528b00_0)(this);
      if (CONCAT31(extraout_var_01,uVar1) != 0) {
        this->mbr_0xdc = 0;
        (*this->vftptr_0x0->virt_meth_0x528ee0_496)(this,unaff_ESI);
        (*this->vftptr_0x0[1].virt_meth_0x46ed70_8)(this);
      }
    }
  }
  uVar1 = (*this->vftptr_0x0[1].virt_meth_0x477910_52)(this);
  if (CONCAT31(extraout_var_02,uVar1) != 0) {
    uVar1 = (*this->vftptr_0x0[1].virt_meth_0x46e900_44)(this);
    if (CONCAT31(extraout_var_03,uVar1) != 0 && -1 < extraout_var_03) {
      uVar2 = TPlayScreen::meth_0x58eb90(this_00);
      if (this->mbr_0xe4 <= uVar2) {
        (*this->vftptr_0x0[1].virt_meth_0x477920_56)(this);
        this->mbr_0xd8 = 0;
      }
    }
  }
  return;
}



// Function at 00528770

SoftwareRenderer * SoftwareRenderer::virt_meth_0x528770()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528980

cls_0x5a47f0 * SoftwareRenderer::virt_meth_0x528980()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a173e;
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
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b8e34__vftable_5b8e34_005b8e34;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00528a90

TScreen * SoftwareRenderer::virt_meth_0x528a90(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a177b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&SoftwareRenderer__vftable_5b8e94_005b8e94;
    this_00->mbr_0x8 = this_00->mbr_0x8 & 0xffff7fff | 0x101;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00528b00

SoftwareRenderer * SoftwareRenderer::virt_meth_0x528b00_Dtor()

{
  byte in_stack_00000004;
  
  ~SoftwareRenderer(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528b20

SoftwareRenderer * __thiscall SoftwareRenderer::~SoftwareRenderer(SoftwareRenderer *this)

{
  SoftwareRenderer *pcVar1;
  
  this->vftptr_0x0 = &SoftwareRenderer__vftable_5b8e94_005b8e94;
  pcVar1 = (SoftwareRenderer *)TScreen::~TScreen((TScreen *)this);
  return pcVar1;
}



// Function at 00528ea0

void SoftwareRenderer::virt_meth_0x528ea0()

{
  dword dVar1;
  dword *pdVar2;
  cls_0x5a5ff0 *in_stack_00000004;
  
  virt_meth_0x472980(this);
  dVar1 = this->mbr_0xd8;
  if ((int)((in_stack_00000004->mbr_0xc + in_stack_00000004->mbr_0x4) - in_stack_00000004->mbr_0x8)
      < 4) {
    cls_0x5a5ff0::meth_0x49cc70(in_stack_00000004);
  }
  pdVar2 = (dword *)in_stack_00000004->mbr_0x8;
  *pdVar2 = dVar1;
  in_stack_00000004->mbr_0x8 = (dword)(pdVar2 + 1);
  return;
}



// Function at 00528ee0

TScreen * SoftwareRenderer::virt_meth_0x528ee0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a179b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xdc);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x529120_005b9088;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00529120

SoftwareRenderer * SoftwareRenderer::virt_meth_0x529120()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005421d0

SoftwareRenderer * SoftwareRenderer::virt_meth_0x5421d0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00542200

cls_0x5b99c0 * SoftwareRenderer::virt_meth_0x542200(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6)

{
  cls_0x5b99c0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1e2b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5b99c0 *)FUN_00482fb0(0x150);
  local_4 = 0;
  if (pcVar1 != (cls_0x5b99c0 *)0x0) {
    pcVar1 = cls_0x5b99c0::cls_0x5b99c0(pcVar1,param_1,param_2,param_3,param_4,param_5,param_6);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5b99c0 *)0x0;
}



// Function at 00542290

cls_0x5b99c0 * SoftwareRenderer::virt_meth_0x542290(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6)

{
  cls_0x5b99c0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1e4b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5b99c0 *)FUN_00482fb0(0x154);
  local_4 = 0;
  if (this_00 != (cls_0x5b99c0 *)0x0) {
    cls_0x5b99c0::cls_0x5b99c0(this_00,param_1,param_2,param_3,param_4,param_5,param_6);
    this_00->vftptr_0x0 = (cls_0x5b99c0__vftable_5b99c0 *)&cls_0x5b9bdc__vftable_5b9bdc_005b9bdc;
    *(undefined4 *)&this_00->field_0x114 = 0;
    (this_00->cls_0x41c7f0).mbr_0x14 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5b99c0 *)0x0;
}



