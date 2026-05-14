// Decompiled methods and structure for class: cls_0x5b5560

/*
/OOAnalyzer/cls_0x5b5560
pack(disabled)
Structure cls_0x5b5560 {
   0   cls_0x5b5560::vftable_5b5560 *   4   vftptr_0x0   "pointer to cls_0x5b5560::vftable_5b5560"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 220 Alignment: 1

*/

// Function at 00520b80

cls_0x5b5560 * __thiscall OOAnalyzer::cls_0x5b5560::virt_meth_0x520b80(cls_0x5b5560 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00520ca0

cls_0x5b5560 * __thiscall OOAnalyzer::cls_0x5b5560::~cls_0x5b5560(cls_0x5b5560 *this)

{
  cls_0x5a50e8 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a12b8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b5560__vftable_5b5560_005b5560;
  local_4 = 0;
  if ((this->mbr_0x8 & 0x80000000) == 0) {
    cls_0x5b4f30_TPlayer::meth_0x46e630((cls_0x5b4f30_TPlayer *)this);
  }
  FUN_004830f0(this->mbr_0xd8);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  ExceptionList = local_c;
  return (cls_0x5b5560 *)pcVar1;
}



// Function at 00520eb0

undefined4 __thiscall OOAnalyzer::cls_0x5b5560::virt_meth_0x520eb0(cls_0x5b5560 *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int in_stack_00000008;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  iVar1 = cls_0x5b8e94_TObjectInstance::virt_meth_0x4705f0((cls_0x5b8e94_TObjectInstance *)this,param_1);
  if (iVar1 != 0) {
    return 1;
  }
  if (in_stack_00000008 < 0) {
    if (param_1 == DAT_00667fcc) {
      puVar5 = &DAT_0066dcb0;
      iVar1 = FUN_0058ad30(*(undefined4 *)this->mbr_0x4c,&DAT_005e2c54);
      if ((iVar1 != 0) ||
         (iVar1 = FUN_0058ad30(*(undefined4 *)this->mbr_0x4c,&DAT_005e2c5c), iVar1 != 0)) {
        puVar5 = &DAT_0066db18;
      }
      puVar5[0x5f] = this;
      if ((this == (cls_0x5b5560 *)0x0) || (this->mbr_0xd8 == 0)) {
        puVar5[99] = 0;
      }
      else {
        uVar3 = puVar5[100];
        uVar8 = 10000;
        uVar7 = 0;
        uVar6 = 1;
        uVar2 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
        uVar3 = cls_0x4acb80::meth_0x4acb80
                          ((cls_0x4acb80 *)&UNK_0065abf8.field_0x418,DAT_0065c5c0,uVar2,uVar3,uVar6,
                           uVar7,uVar8);
        puVar5[99] = uVar3;
      }
      (**(code **)*puVar5)();
      cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)&DAT_0065caf0);
      uVar3 = cls_0x5a5320_TPlayScreen::meth_0x48ed60((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
      cls_0x5a5320_TPlayScreen::meth_0x48eea0((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0,uVar3);
      iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
      if ((-1 < iVar1) &&
         (iVar4 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar4 != 0)) {
        cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
      }
      return 0;
    }
    return 0;
  }
  return 0;
}



// Function at 00521cd0

cls_0x5b5560 * __thiscall OOAnalyzer::cls_0x5b5560::virt_meth_0x521cd0(cls_0x5b5560 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b5560(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00521cf0

cls_0x5a50e8 * __thiscall
OOAnalyzer::cls_0x5b5560::virt_meth_0x521cf0(cls_0x5b5560 *this,undefined4 param_1)

{
  cls_0x5a50e8 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a12db;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a50e8 *)FUN_00482fb0(0xdc);
  local_4 = 0;
  if (this_00 != (cls_0x5a50e8 *)0x0) {
    cls_0x5a50e8::cls_0x5a50e8(this_00);
    this_00->vftptr_0x0 = (cls_0x5a50e8__vftable_5a50e8 *)&cls_0x5b5560__vftable_5b5560_005b5560;
    this_00->mbr_0xd8 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a50e8 *)0x0;
}



