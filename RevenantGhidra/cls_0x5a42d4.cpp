// Decompiled methods and structure for class: cls_0x5a42d4

/*
/OOAnalyzer/cls_0x5a42d4
pack(disabled)
Structure cls_0x5a42d4 {
   0   cls_0x5a42d4::vftable_5a42d4 *   4   vftptr_0x0   "pointer to cls_0x5a42d4::vftable_5a42d4"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 160 Alignment: 1

*/

// Function at 0043da00

undefined4 __thiscall OOAnalyzer::cls_0x5a42d4::virt_meth_0x43da00(cls_0x5a42d4 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  
  cls_0x5a4494::virt_meth_0x491900((cls_0x5a4494 *)this);
  iVar3 = *(int *)(DAT_0065b020 + DAT_0065bc40 * 4);
  this->mbr_0x98 = 0x3c;
  this->mbr_0x84 = 0x3c;
  this->mbr_0x6c = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x90 = 0;
  this->mbr_0x8c = 0;
  this->mbr_0x78 = 0;
  this->mbr_0x74 = 0;
  if (iVar3 == 0) {
    iVar3 = DAT_0065b024;
  }
  iVar3 = *(int *)(iVar3 + 0x50);
  this->mbr_0x9c = 0xffffffff;
  dVar1 = (int)this->mbr_0x10 / (iVar3 + 2);
  this->mbr_0x94 = dVar1;
  this->mbr_0x88 = dVar1;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  this->mbr_0x70 = 0;
  this->mbr_0x68 = 0;
  puVar2 = FUN_00482fb0(10000);
  this->mbr_0x60 = (dword)puVar2;
  *(undefined *)puVar2 = 0x7b;
  *(undefined *)((int)puVar2 + 1) = 0;
  for (iVar3 = 0x9c4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  puVar2 = FUN_00482fb0(10000);
  *(undefined *)puVar2 = 0x7b;
  this->mbr_0x64 = (dword)puVar2;
  *(undefined *)((int)puVar2 + 1) = 0;
  for (iVar3 = 0x9c4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return 1;
}



// Function at 0043dad0

void __thiscall OOAnalyzer::cls_0x5a42d4::virt_meth_0x43dad0(cls_0x5a42d4 *this)

{
  FUN_004830f0((LPCVOID)this->mbr_0x60);
  this->mbr_0x60 = 0;
  FUN_004830f0((LPCVOID)this->mbr_0x64);
  this->mbr_0x64 = 0;
  TCharacter::virt_meth_0x491970((TCharacter *)this);
  return;
}



// Function at 0043db00

void __thiscall OOAnalyzer::cls_0x5a42d4::virt_meth_0x43db00(cls_0x5a42d4 *this)

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x43db30_84)(this,PTR_DAT_005d79e0);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 0043db30

void __thiscall
OOAnalyzer::cls_0x5a42d4::virt_meth_0x43db30(cls_0x5a42d4 *this,cls_0x5a68d8 *param_1)

{
  (*param_1->vftptr_0x0->virt_meth_0x4bde60_100)
            (param_1,0,0,(char)this->mbr_0xc,(char)this->mbr_0x10,0,0xff,0);
  cls_0x5a68d8::meth_0x4be110
            (param_1,this->mbr_0x68 + this->mbr_0x60,0,3,this->mbr_0x94,DAT_0065bc40,0,0x80000000,
             this->mbr_0x9c,0,1,0xffffffff);
  return;
}



// Function at 0043dfa0

void __thiscall OOAnalyzer::cls_0x5a42d4::virt_meth_0x43dfa0(cls_0x5a42d4 *this,undefined4 param_1)

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
    cls_0x5a4358::meth_0x43dca0((cls_0x5a4358 *)this,1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  default:
    return;
  case 0x21:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,this->mbr_0x74,this->mbr_0x78 - this->mbr_0x94)
    ;
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x22:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,this->mbr_0x74,this->mbr_0x94 + this->mbr_0x78)
    ;
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x23:
    pcVar5 = (char *)(this->mbr_0x70 + this->mbr_0x60);
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
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,this->mbr_0x74 + iVar6,this->mbr_0x78);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x24:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,0,this->mbr_0x78);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x25:
    if (DAT_0065c9e0 == 0) {
      iVar6 = this->mbr_0x74 - 1;
    }
    else {
      iVar6 = this->mbr_0x74 - 6;
    }
    break;
  case 0x26:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,this->mbr_0x74,this->mbr_0x78 - 1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x27:
    if (DAT_0065c9e0 == 0) {
      iVar6 = this->mbr_0x74 + 1;
    }
    else {
      iVar6 = this->mbr_0x74 + 6;
    }
    break;
  case 0x28:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,this->mbr_0x74,this->mbr_0x78 + 1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x2e:
    pcVar3 = this->vftptr_0x0;
    cls_0x5a4358::meth_0x43dca0((cls_0x5a4358 *)this,1);
    (*pcVar3->virt_meth_0x444fc0_44)(this);
    return;
  case 0x72:
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,0,this->mbr_0x78);
    pcVar5 = (char *)(this->mbr_0x70 + this->mbr_0x60);
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
    cls_0x5a4358::meth_0x43dca0((cls_0x5a4358 *)this,iVar6 + 1);
    cls_0x5a4358::meth_0x43dbc0((cls_0x5a4358 *)this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  case 0x59:
  case 0x79:
    if (DAT_0065c9e0 == 0) {
      return;
    }
    dVar4 = this->mbr_0x74;
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,0,this->mbr_0x78);
    pcVar5 = (char *)(this->mbr_0x70 + this->mbr_0x60);
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
    cls_0x5a4358::meth_0x43dca0((cls_0x5a4358 *)this,iVar6 + 1);
    cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,dVar4,this->mbr_0x78);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return;
  }
  pcVar3 = this->vftptr_0x0;
  cls_0x5a4358::meth_0x43de60((cls_0x5a4358 *)this,iVar6,this->mbr_0x78);
  (*pcVar3->virt_meth_0x444fc0_44)(this);
  return;
}



// Function at 0043e2b0

void __thiscall OOAnalyzer::cls_0x5a42d4::virt_meth_0x43e2b0(cls_0x5a42d4 *this,int param_1)

{
  undefined *puVar1;
  cls_0x5a42d4__vftable_5a42d4 *pcVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    if (param_1 == 0xd) {
      dVar4 = this->mbr_0x6c;
      param_1 = 10;
      iVar3 = (dVar4 - this->mbr_0x74) + 1;
      if (iVar3 < (int)dVar4) {
        do {
          puVar1 = (undefined *)(this->mbr_0x60 + iVar3);
          iVar5 = (this->mbr_0x74 - dVar4) + iVar3;
          iVar3 = iVar3 + 1;
          *(undefined *)((this->mbr_0x64 - 1) + iVar5) = *puVar1;
          dVar4 = this->mbr_0x6c;
        } while (iVar3 < (int)dVar4);
      }
      iVar5 = this->mbr_0x74 - this->mbr_0x6c;
      if (iVar5 + -2 + iVar3 < 1) {
        *(undefined *)this->mbr_0x64 = 0;
      }
      else {
        *(undefined *)(this->mbr_0x64 + iVar5 + -2 + iVar3) = 0;
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



