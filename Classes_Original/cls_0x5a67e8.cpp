// Decompiled methods and structure for class: cls_0x5a67e8

/*
/OOAnalyzer/cls_0x5a67e8
pack(disabled)
Structure cls_0x5a67e8 {
   0   cls_0x5a3980   144   cls_0x5a3980   "Component (member) class."
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 152 Alignment: 1

*/

// Function at 004a9160

cls_0x5a67e8 * __thiscall OOAnalyzer::cls_0x5a67e8::cls_0x5a67e8(cls_0x5a67e8 *this)

{
  cls_0x5a3980::cls_0x5a3980(&this->cls_0x5a3980);
  (this->cls_0x5a3980).cls_0x5a68d8.vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a67e8__vftable_5a67e8_005a67e8;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80 = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x24 = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x20 = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x78 = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x1c = 0;
  *(undefined4 *)&(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x84 = 0;
  (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x88 = 0;
  return this;
}



// Function at 004a9990

undefined4 __thiscall OOAnalyzer::cls_0x5a67e8::virt_meth_0x4a9990(cls_0x5a67e8 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  int *piVar4;
  
  piVar4 = (int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80;
  if (piVar4 != (int *)0x0) {
    iVar3 = (**(code **)(*piVar4 + 0xc))();
    if (iVar3 == DAT_0066a2ec) {
      dVar1 = (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80;
      (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80 = (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c;
      (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c = dVar1;
    }
    if (DAT_0066a900 != (int *)0x0) {
      (**(code **)(*DAT_0066a900 + 0x10))(DAT_0066a900,1,0x66a2f8);
      (**(code **)(*DAT_0066a900 + 8))(DAT_0066a900);
      DAT_0066a900 = (int *)0x0;
    }
    if (this->mbr_0x94 != 0) {
      if ((undefined4 *)this->mbr_0x90 != (undefined4 *)0x0) {
        (***(code ***)(undefined4 *)this->mbr_0x90)(1);
      }
      dVar1 = this->mbr_0x94;
      this->mbr_0x94 = 0;
      this->mbr_0x90 = dVar1;
    }
    FUN_004a79e0();
    piVar4 = (int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c;
    if (piVar4 != (int *)0x0) {
      iVar3 = (**(code **)(*piVar4 + 0xc))();
      if (iVar3 != 0) {
        piVar4 = (int *)(**(code **)(*(int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c + 0xc))();
        (**(code **)(*piVar4 + 8))(piVar4);
        puVar2 = (undefined4 *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c;
        if (puVar2 != (undefined4 *)0x0) {
          (**(code **)*puVar2)(1);
        }
        (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c = 0;
        DAT_0066a2ec = 0;
        (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x68 = 0;
      }
    }
    if ((int *)this->mbr_0x90 != (int *)0x0) {
      iVar3 = (**(code **)(*(int *)this->mbr_0x90 + 0xc))();
      if (iVar3 != 0) {
        piVar4 = (int *)(**(code **)(*(int *)this->mbr_0x90 + 0xc))();
        (**(code **)(*piVar4 + 8))(piVar4);
        if ((undefined4 *)this->mbr_0x90 != (undefined4 *)0x0) {
          (***(code ***)(undefined4 *)this->mbr_0x90)(1);
        }
        this->mbr_0x90 = 0;
        DAT_0066a2f0 = 0;
      }
    }
    piVar4 = (int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80;
    if (piVar4 != (int *)0x0) {
      iVar3 = (**(code **)(*piVar4 + 0xc))();
      if (iVar3 != 0) {
        piVar4 = (int *)(**(code **)(*(int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80 + 0xc))();
        (**(code **)(*piVar4 + 8))(piVar4);
        puVar2 = (undefined4 *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80;
        if (puVar2 != (undefined4 *)0x0) {
          (**(code **)*puVar2)(1);
        }
        (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80 = 0;
        DAT_0066a8f8 = 0;
      }
    }
    FUN_004a7460();
    (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8 = 0;
    (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x4 = 0;
    (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x88 = 0;
  }
  return 1;
}



// Function at 004a9b00

cls_0x5a67e8 * __thiscall OOAnalyzer::cls_0x5a67e8::~cls_0x5a67e8(cls_0x5a67e8 *this)

{
  cls_0x5a67e8 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dee0;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a3980).cls_0x5a68d8.vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a67e8__vftable_5a67e8_005a67e8;
  local_4 = 0;
  virt_meth_0x4a9990(this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a67e8 *)cls_0x5a3980::~cls_0x5a3980(&this->cls_0x5a3980);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004a9b70

undefined4 __thiscall OOAnalyzer::cls_0x5a67e8::virt_meth_0x4a9b70(cls_0x5a67e8 *this)

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  
  if ((this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80 == 0) {
    return 0;
  }
  if ((this->cls_0x5a3980).cls_0x5a68d8.mbr_0x88 == 0) {
    if (((this->cls_0x5a3980).cls_0x5a68d8.mbr_0x84 & 1) == 0) {
      FUN_004a7380();
      dVar2 = (this->cls_0x5a3980).cls_0x5a68d8.mbr_0xc;
      if (dVar2 == 0xf) {
        dVar2 = 0x10;
      }
      FUN_004a74f0((this->cls_0x5a3980).cls_0x5a68d8.mbr_0x4,
                   (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8,dVar2);
    }
    iVar3 = (**(code **)(*DAT_0066a8f8 + 0x60))(DAT_0066a8f8);
    if (iVar3 == -0x7789fe3e) {
      iVar3 = (**(code **)(*DAT_0066a8f8 + 0x6c))(DAT_0066a8f8);
      if (iVar3 != 0) {
        FUN_004a90d0(iVar3,s_d__revenant_Display_cpp_005dec90,0x168);
      }
      if (((this->cls_0x5a3980).cls_0x5a68d8.mbr_0x84 & 1) == 0) {
        piVar1 = (int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x80;
        (**(code **)(*piVar1 + 100))(0,0,piVar1[1],piVar1[2],0,0xffff,0x7f7f,0x80000000);
      }
    }
    iVar3 = (**(code **)(*DAT_0066a2ec + 0x60))(DAT_0066a2ec);
    if (iVar3 == -0x7789fe3e) {
      iVar3 = (**(code **)(*DAT_0066a8f8 + 0x6c))(DAT_0066a8f8);
      if (iVar3 != 0) {
        FUN_004a90d0(iVar3,s_d__revenant_Display_cpp_005deca8,0x16f);
      }
      piVar1 = (int *)(this->cls_0x5a3980).cls_0x5a68d8.mbr_0x8c;
      (**(code **)(*piVar1 + 100))(0,0,piVar1[1],piVar1[2],0,0xffff,0x7f7f,0x80000000);
    }
    (this->cls_0x5a3980).cls_0x5a68d8.mbr_0x88 = 1;
  }
  return 1;
}



// Function at 004aba00

cls_0x5a67e8 * __thiscall OOAnalyzer::cls_0x5a67e8::virt_meth_0x4aba00(cls_0x5a67e8 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a67e8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



