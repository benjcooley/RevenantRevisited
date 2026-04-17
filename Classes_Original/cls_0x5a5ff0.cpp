// Decompiled methods and structure for class: cls_0x5a5ff0

/*
/OOAnalyzer/cls_0x5a5ff0
pack(disabled)
Structure cls_0x5a5ff0 {
   0   cls_0x5a5ff0::vftable_5a5ff0 *   4   vftptr_0x0   "pointer to cls_0x5a5ff0::vftable_5a5ff0"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 20 Alignment: 1

*/

// Function at 004779d0

void __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x4779d0(cls_0x5a5ff0 *this)

{
  int in_stack_00000004;
  
  if ((int)((this->mbr_0xc - this->mbr_0x8) + this->mbr_0x4) < in_stack_00000004) {
    meth_0x49cc70(this);
  }
  return;
}



// Function at 00477c90

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x477c90(cls_0x5a5ff0 *this)

{
  undefined2 *puVar1;
  undefined2 in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 2) {
    meth_0x49cc70(this);
  }
  puVar1 = (undefined2 *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



// Function at 00477cd0

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x477cd0(cls_0x5a5ff0 *this)

{
  undefined *puVar1;
  undefined in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 1) {
    meth_0x49cc70(this);
  }
  puVar1 = (undefined *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



// Function at 0049cc20

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::cls_0x5a5ff0(cls_0x5a5ff0 *this,dword param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  dVar1 = FUN_00482ef0(param_1);
  this->mbr_0x4 = dVar1;
  this->mbr_0xc = param_1;
  this->mbr_0x8 = dVar1;
  this->mbr_0x10 = in_stack_00000008;
  return this;
}



// Function at 0049cc50

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::~cls_0x5a5ff0(cls_0x5a5ff0 *this)

{
  cls_0x5a5ff0 *in_EAX;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  if (this->mbr_0x4 != 0) {
    in_EAX = (cls_0x5a5ff0 *)FUN_004830f0(this->mbr_0x4);
  }
  return in_EAX;
}



// Function at 0049cc70

void __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x49cc70(cls_0x5a5ff0 *this)

{
  dword dVar1;
  int iVar2;
  int in_stack_00000004;
  
  iVar2 = this->mbr_0x8 - this->mbr_0x4;
  if ((int)(this->mbr_0xc - iVar2) < in_stack_00000004) {
    dVar1 = FUN_00482f40(this->mbr_0x4,this->mbr_0xc + this->mbr_0x10);
    this->mbr_0x4 = dVar1;
    this->mbr_0x8 = dVar1 + iVar2;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  return;
}



// Function at 0049ccc0

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x49ccc0(cls_0x5a5ff0 *this)

{
  char cVar1;
  dword dVar2;
  undefined *puVar3;
  dword dVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  char *in_stack_00000004;
  
  uVar5 = 0xffffffff;
  dVar4 = this->mbr_0x4;
  pcVar7 = in_stack_00000004;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  dVar2 = this->mbr_0xc;
  if (((int)((dVar4 - this->mbr_0x8) + dVar2) < (int)(uVar5 + 1)) &&
     (iVar6 = this->mbr_0x8 - dVar4, (int)(dVar2 - iVar6) < (int)(uVar5 + 1))) {
    dVar4 = FUN_00482f40(dVar4,dVar2 + this->mbr_0x10);
    this->mbr_0x4 = dVar4;
    this->mbr_0x8 = dVar4 + iVar6;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  puVar3 = (undefined *)this->mbr_0x8;
  *puVar3 = (char)(uVar5 - 1);
  _strncpy(puVar3 + 1,in_stack_00000004,uVar5 - 1 & 0xff);
  this->mbr_0x8 = this->mbr_0x8 + uVar5;
  return this;
}



// Function at 0049cd50

uint __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x49cd50(cls_0x5a5ff0 *this,undefined4 *param_1)

{
  dword dVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  undefined4 *puVar5;
  uint in_stack_00000008;
  
  dVar3 = this->mbr_0x4;
  dVar1 = this->mbr_0xc;
  if (((int)((dVar3 - this->mbr_0x8) + dVar1) < (int)in_stack_00000008) &&
     (iVar2 = this->mbr_0x8 - dVar3, (int)(dVar1 - iVar2) < (int)in_stack_00000008)) {
    dVar3 = FUN_00482f40(dVar3,this->mbr_0x10 + dVar1);
    this->mbr_0x4 = dVar3;
    this->mbr_0x8 = dVar3 + iVar2;
    this->mbr_0xc = this->mbr_0xc + this->mbr_0x10;
  }
  puVar5 = (undefined4 *)this->mbr_0x8;
  for (uVar4 = in_stack_00000008 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar5 = *param_1;
    param_1 = param_1 + 1;
    puVar5 = puVar5 + 1;
  }
  for (uVar4 = in_stack_00000008 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined *)puVar5 = *(undefined *)param_1;
    param_1 = (undefined4 *)((int)param_1 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + 1);
  }
  this->mbr_0x8 = this->mbr_0x8 + in_stack_00000008;
  return in_stack_00000008;
}



// Function at 0049cdd0

dword __thiscall OOAnalyzer::cls_0x5a5ff0::meth_0x49cdd0(cls_0x5a5ff0 *this)

{
  dword dVar1;
  
  dVar1 = this->mbr_0x4;
  if ((int)this->mbr_0xc < (int)(this->mbr_0x8 - dVar1)) {
    FUN_0054d190(&DAT_0065c5d0,1,s_Output_Stream_Overrun_005daa58);
    dVar1 = 0;
  }
  return dVar1;
}



// Function at 0049ce80

cls_0x5a5ff0 * __thiscall OOAnalyzer::cls_0x5a5ff0::~cls_0x5a5ff0(cls_0x5a5ff0 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a5ff0__vftable_5a5ff0_005a5ff0;
  if (this->mbr_0x4 != 0) {
    FUN_004830f0(this->mbr_0x4);
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



