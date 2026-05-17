#include "SStormParams.h"

// Decompiled methods and structure for class: SStormParams



// Function at 0043da00

undefined4 SStormParams::virt_meth_0x43da00_TTextPane_Initialize()

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  
  this->virt_meth_0x491900();
  iVar3 = *(int *)(DAT_0065b020 + DAT_0065bc40 * 4);
  this->particle_scale = 0x3c;
  this->velocity = 0x3c;
  this->particle_end = 0;
  this->impact_end = 0;
  this->gravity = 0;
  this->impact_frame_inc = 0;
  this->pos_spread = 0;
  this->impact_begin = 0;
  this->impact_v = 0;
  if (iVar3 == 0) {
    iVar3 = DAT_0065b024;
  }
  iVar3 = *(int *)(iVar3 + 0x50);
  this->impact_scale = 0xffffffff;
  dVar1 = (int)this->tex_u / (iVar3 + 2);
  this->particle_frame_inc = dVar1;
  this->pos = dVar1;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  this->impact_u = 0;
  this->particle_begin = 0;
  puVar2 = FUN_00482fb0(10000);
  this->particle_u = (dword)puVar2;
  *(undefined *)puVar2 = 0x7b;
  *(undefined *)((int)puVar2 + 1) = 0;
  for (iVar3 = 0x9c4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  puVar2 = FUN_00482fb0(10000);
  *(undefined *)puVar2 = 0x7b;
  this->particle_v = (dword)puVar2;
  *(undefined *)((int)puVar2 + 1) = 0;
  for (iVar3 = 0x9c4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return 1;
}



// Function at 0043dad0

void SStormParams::virt_meth_0x43dad0()

{
  FUN_004830f0((LPCVOID)this->particle_u);
  this->particle_u = 0;
  FUN_004830f0((LPCVOID)this->particle_v);
  this->particle_v = 0;
  this->virt_meth_0x491970();
  return;
}



// Function at 0043db00

void SStormParams::virt_meth_0x43db00()

{
  if (this->tex_v != 0) {
    (*this->vftptr_0x0->virt_meth_0x43db30_84)(this,PTR_DAT_005d79e0);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 0043db30

void SStormParams::virt_meth_0x43db30(cls_0x5a68d8 *param_1)

{
  (*param_1->vftptr_0x0->virt_meth_0x4bde60_100)
            (param_1,0,0,(char)this->particles,(char)this->tex_u,0,0xff,0);
  cls_0x5a68d8::meth_0x4be110
            (param_1,this->particle_begin + this->particle_u,0,3,this->particle_frame_inc,DAT_0065bc40,0,0x80000000,
             this->impact_scale,0,1,0xffffffff);
  return;
}



// Function at 0043dfa0

void SStormParams::virt_meth_0x43dfa0(undefined4 param_1)

{
  char *pcVar1;
  char cVar2;
  cls_0x5a42d4__vftable_5a42d4 *pcVar3;
  dword dVar4;
  char *pcVar5;
  int iVar6;
  int in_stack_00000008;
  
  if (in_stack_00000008 == 0) {
    return;
  }
  switch(param_1) {
  case 8:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43dca0((cls_0x5a4358_TConsolePane *)this,1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  default:
    return;
  case 0x21:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,this->impact_v,this->impact_begin - this->particle_frame_inc)
    ;
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x22:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,this->impact_v,this->particle_frame_inc + this->impact_begin)
    ;
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x23:
    pcVar5 = (char *)(this->impact_u + this->particle_u);
    iVar6 = 0;
    cVar2 = *pcVar5;
    while ((cVar2 != '\0' && (cVar2 != '\n'))) {
      if (*pcVar5 != '{') {
        iVar6 = iVar6 + 1;
      }
      pcVar1 = pcVar5 + 1;
      pcVar5 = pcVar5 + 1;
      cVar2 = *pcVar1;
    }
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,this->impact_v + iVar6,this->impact_begin);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x24:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,0,this->impact_begin);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x25:
    if (DAT_0065c9e0 == 0) {
      iVar6 = this->impact_v - 1;
    }
    else {
      iVar6 = this->impact_v - 6;
    }
    break;
  case 0x26:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,this->impact_v,this->impact_begin - 1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x27:
    if (DAT_0065c9e0 == 0) {
      iVar6 = this->impact_v + 1;
    }
    else {
      iVar6 = this->impact_v + 6;
    }
    break;
  case 0x28:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,this->impact_v,this->impact_begin + 1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x2e:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358_TConsolePane::meth_0x43dca0((cls_0x5a4358_TConsolePane *)this,1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x72:
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,0,this->impact_begin);
    pcVar5 = (char *)(this->impact_u + this->particle_u);
    iVar6 = 0;
    cVar2 = *pcVar5;
    while ((cVar2 != '\0' && (cVar2 != '\n'))) {
      if (*pcVar5 != '{') {
        iVar6 = iVar6 + 1;
      }
      pcVar1 = pcVar5 + 1;
      pcVar5 = pcVar5 + 1;
      cVar2 = *pcVar1;
    }
    cls_0x5a4358_TConsolePane::meth_0x43dca0((cls_0x5a4358_TConsolePane *)this,iVar6 + 1);
    cls_0x5a4358_TConsolePane::meth_0x43dbc0((cls_0x5a4358_TConsolePane *)this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  case 0x59:
  case 0x79:
    if (DAT_0065c9e0 == 0) {
      return;
    }
    dVar4 = this->impact_v;
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,0,this->impact_begin);
    pcVar5 = (char *)(this->impact_u + this->particle_u);
    iVar6 = 0;
    cVar2 = *pcVar5;
    while ((cVar2 != '\0' && (cVar2 != '\n'))) {
      if (*pcVar5 != '{') {
        iVar6 = iVar6 + 1;
      }
      pcVar1 = pcVar5 + 1;
      pcVar5 = pcVar5 + 1;
      cVar2 = *pcVar1;
    }
    cls_0x5a4358_TConsolePane::meth_0x43dca0((cls_0x5a4358_TConsolePane *)this,iVar6 + 1);
    cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,dVar4,this->impact_begin);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return;
  }
  pcVar3 = this->vftptr_0x0;
  cls_0x5a4358_TConsolePane::meth_0x43de60((cls_0x5a4358_TConsolePane *)this,iVar6,this->impact_begin);
  (*pcVar3->virt_meth_0x444fc0_44)(this);
  return;
}



// Function at 0043e2b0

void SStormParams::virt_meth_0x43e2b0(int param_1)

{
  undefined *puVar1;
  cls_0x5a42d4__vftable_5a42d4 *pcVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    if (param_1 == 0xd) {
      dVar4 = this->particle_end;
      param_1 = 10;
      iVar3 = (dVar4 - this->impact_v) + 1;
      if (iVar3 < (int)dVar4) {
        do {
          puVar1 = (undefined *)(this->particle_u + iVar3);
          iVar5 = (this->impact_v - dVar4) + iVar3;
          iVar3 = iVar3 + 1;
          *(undefined *)((this->particle_v - 1) + iVar5) = *puVar1;
          dVar4 = this->particle_end;
        } while (iVar3 < (int)dVar4);
      }
      iVar5 = this->impact_v - this->particle_end;
      if (iVar5 + -2 + iVar3 < 1) {
        *(undefined *)this->particle_v = 0;
      }
      else {
        *(undefined *)(this->particle_v + iVar5 + -2 + iVar3) = 0;
      }
    }
    if ((((0x1f < param_1) && (param_1 < 0x7f)) || (param_1 == 10)) &&
       (((param_1 != 0x60 && (DAT_0065c9e0 == 0)) && (DAT_0065c9d8 == 0)))) {
      pcVar2 = this->vftptr_0x0;
      (*pcVar2->virt_meth_0x4450d0_124)(this);
      (*pcVar2->virt_meth_0x444fc0_44)(this);
    }
  }
  return;
}



