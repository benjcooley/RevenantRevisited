#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5a30



// Function at 00488580

cls_0x5a5a30::cls_0x5a5a30()

{
  this->mbr_0x8 = 0x1a4;
  this->mbr_0x18 = 0x1a4;
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x3c;
  this->mbr_0x20 = 0x3c;
  this->vftptr_0x0 = (cls_0x5a5a30__vftable_5a5a30 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5a30__vftable_5a5a30_005a5a30;
  return this;
}



// Function at 004885e0

cls_0x5a5a30 * cls_0x5a5a30::virt_meth_0x4885e0()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5a30(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5a30 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00544000

cls_0x5a5a30 * __thiscall cls_0x5a5a30::~cls_0x5a5a30(cls_0x5a5a30 *this)

{
  cls_0x5a5a30 *pcVar1;
  int iVar2;
  dword *pdVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1ebf;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a5a30__vftable_5a5a30_005a5a30;
  local_4 = 0;
  pdVar3 = &this->mbr_0x17c;
  iVar2 = 4;
  do {
    if ((undefined4 *)*pdVar3 != (undefined4 *)0x0) {
      (***(code ***)(undefined4 *)*pdVar3)(1);
    }
    pdVar3 = pdVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  this->vftptr_0x0 = (cls_0x5a5a30__vftable_5a5a30 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 1;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5a5a30__vftable_5a5a30 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 2;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a5a30 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00544160

undefined4 cls_0x5a5a30::virt_meth_0x544160()

{
  cls_0x46d6b0 *pcVar1;
  int iVar2;
  cls_0x5b9bf0 *pcVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  cls_0x5a3c68 *pcVar6;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a1ef5;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->virt_meth_0x434e40();
  iVar2 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  iVar2 = (-(uint)(iVar2 != 0x10) & 0xffff8400) + 0xf81f;
  pcVar3 = (cls_0x5b9bf0 *)FUN_00482fb0(0x150);
  pcVar1 = DAT_0065bc3c;
  uStack_4 = 0;
  if (pcVar3 != (cls_0x5b9bf0 *)0x0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x46d6b0::meth_0x46d710(pcVar1);
    cls_0x5b9bf0::cls_0x5b9bf0
              (pcVar3,&DAT_005e5208,DAT_005e4fe8,DAT_005e4ff8,0x20,0x20,0,&LAB_005440a0,uVar5,uVar4,
               0,0,1,1,5,iVar2,0);
    (pcVar3->cls_0x5a3c68).cls_0x5a3ab8.vftptr_0x0 =
         (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5b9c54__vftable_5b9c54_005b9c54;
  }
  uStack_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar3 = (cls_0x5b9bf0 *)FUN_00482fb0(0x150);
  pcVar1 = DAT_0065bc3c;
  uStack_4 = 1;
  if (pcVar3 != (cls_0x5b9bf0 *)0x0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x46d6b0::meth_0x46d710(pcVar1);
    cls_0x5b9bf0::cls_0x5b9bf0
              (pcVar3,&DAT_005e5224,DAT_005e4fec,DAT_005e4ffc,0x20,0x20,0,&LAB_005440d0,uVar5,uVar4,
               0,0,1,2,5,iVar2,0);
    (pcVar3->cls_0x5a3c68).cls_0x5a3ab8.vftptr_0x0 =
         (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5b9c54__vftable_5b9c54_005b9c54;
  }
  uStack_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar6 = (cls_0x5a3c68 *)FUN_00482fb0(0x150);
  pcVar1 = DAT_0065bc3c;
  uStack_4 = 2;
  if (pcVar6 != (cls_0x5a3c68 *)0x0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x46d6b0::meth_0x46d710(pcVar1);
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar6,&DAT_005e5240,DAT_005e4ff0,DAT_005e5000,0x20,0x20,0,&LAB_00544100,uVar5,uVar4,
               0,0,1,0xffffffff,0,iVar2);
    pcVar6[1].cls_0x5a3ab8.vftptr_0x0 = (cls_0x5a3ab8__vftable_5a3ab8 *)0x3;
    *(undefined4 *)&pcVar6[1].cls_0x5a3ab8.field_0x4 = 5;
    (pcVar6->cls_0x5a3ab8).vftptr_0x0 =
         (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5b9c54__vftable_5b9c54_005b9c54;
  }
  uStack_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar6 = (cls_0x5a3c68 *)FUN_00482fb0(0x150);
  pcVar1 = DAT_0065bc3c;
  uStack_4 = 3;
  if (pcVar6 != (cls_0x5a3c68 *)0x0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    uVar5 = cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x46d6b0::meth_0x46d710(pcVar1);
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar6,&DAT_005e525c,DAT_005e4ff4,DAT_005e5004,0x20,0x20,0,&LAB_00544130,uVar5,uVar4,
               0,0,1,0xffffffff,0,iVar2);
    pcVar6[1].cls_0x5a3ab8.vftptr_0x0 = (cls_0x5a3ab8__vftable_5a3ab8 *)0x4;
    *(undefined4 *)&pcVar6[1].cls_0x5a3ab8.field_0x4 = 5;
    (pcVar6->cls_0x5a3ab8).vftptr_0x0 =
         (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5b9c54__vftable_5b9c54_005b9c54;
  }
  uStack_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  ExceptionList = pvStack_c;
  return 1;
}



// Function at 005444c0

void cls_0x5a5a30::virt_meth_0x5444c0()

{
  char cVar1;
  int *piVar2;
  int iVar3;
  char *pcVar4;
  int iVar5;
  undefined *in_stack_00000004;
  
  if (((in_stack_00000004 != PTR_DAT_005d79e0) || (this->mbr_0x50 != 0)) || (DAT_0065b688 != 0)) {
    iVar5 = 0;
    if (0 < *(int *)&this->field_0x88) {
      do {
        piVar2 = (int *)cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
        if ((piVar2 != (int *)0x0) && (piVar2[4] == 4)) {
          if ((DAT_00667fcc != (TPlayScreen *)0x0) &&
             (iVar3 = TPlayScreen::meth_0x51b560(DAT_00667fcc), iVar3 != 0)) {
            if (DAT_00667fcc == (TPlayScreen *)0x0) {
              pcVar4 = &UNK_00670228.field_0x414;
            }
            else {
              pcVar4 = (char *)TPlayScreen::meth_0x51b560(DAT_00667fcc);
            }
            iVar3 = -1;
            do {
              if (iVar3 == 0) break;
              iVar3 = iVar3 + -1;
              cVar1 = *pcVar4;
              pcVar4 = pcVar4 + 1;
            } while (cVar1 != '\0');
            if (iVar3 != -2) {
              if (DAT_00667fcc != (TPlayScreen *)0x0) {
                TPlayScreen::meth_0x51b560(DAT_00667fcc);
              }
              iVar3 = cls_0x53ed70::meth_0x53ed70((cls_0x53ed70 *)((int)&UNK_00667c34 + 4));
              if ((iVar3 != 0) && (DAT_00667fcc != (TPlayScreen *)0x0)) {
                TPlayScreen::meth_0x51b560(DAT_00667fcc);
                iVar3 = TPlayScreen::meth_0x51b7c0(DAT_00667fcc);
                if (iVar3 != 0) {
                  (**(code **)(*piVar2 + 0x1c))(piVar2[5] & 0xfffffffb);
                  goto LAB_005445d3;
                }
              }
            }
          }
          (**(code **)(*piVar2 + 0x1c))(piVar2[5] | 4);
        }
LAB_005445d3:
        iVar5 = iVar5 + 1;
      } while (iVar5 < *(int *)&this->field_0x88);
    }
    (*this->vftptr_0x0->FUN_004361b0_144)();
  }
  if (in_stack_00000004 == PTR_DAT_005d79e0) {
    this->virt_meth_0x435de0();
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return;
  }
  this->virt_meth_0x435cb0();
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return;
}



