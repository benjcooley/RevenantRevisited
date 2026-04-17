// Decompiled methods and structure for class: cls_0x5a3980

/*
/OOAnalyzer/cls_0x5a3980
pack(disabled)
Structure cls_0x5a3980 {
   0   cls_0x5a68d8   144   cls_0x5a68d8   "Component (member) class."
}
Length: 144 Alignment: 1

*/

// Function at 0041a170

dword __thiscall OOAnalyzer::cls_0x5a3980::virt_meth_0x41a170(cls_0x5a3980 *this)

{
  return (this->cls_0x5a68d8).mbr_0x68;
}



// Function at 0041a180

dword __thiscall OOAnalyzer::cls_0x5a3980::virt_meth_0x41a180(cls_0x5a3980 *this)

{
  return (this->cls_0x5a68d8).mbr_0x6c;
}



// Function at 0041a1c0

cls_0x5a3980 * __thiscall OOAnalyzer::cls_0x5a3980::virt_meth_0x41a1c0(cls_0x5a3980 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a3980(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004a5720

cls_0x5a3980 * __thiscall OOAnalyzer::cls_0x5a3980::cls_0x5a3980(cls_0x5a3980 *this)

{
  cls_0x5a68d8::cls_0x5a68d8(&this->cls_0x5a68d8);
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
  (this->cls_0x5a68d8).mbr_0x68 = 0;
  (this->cls_0x5a68d8).mbr_0x6c = 0;
  return this;
}



// Function at 004a6070

cls_0x5a3980 * __thiscall OOAnalyzer::cls_0x5a3980::~cls_0x5a3980(cls_0x5a3980 *this)

{
  cls_0x5a3980 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059de08;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
  local_4 = 0;
  cls_0x5a6858::meth_0x4a5fe0((cls_0x5a6858 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a3980 *)cls_0x5a6858::meth_0x4bcb50((cls_0x5a6858 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 004a60c0

undefined4 __thiscall OOAnalyzer::cls_0x5a3980::virt_meth_0x4a60c0(cls_0x5a3980 *this)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
  if (piVar1 != (int *)0x0) {
    iVar2 = (**(code **)(*piVar1 + 0x60))(piVar1);
    if (iVar2 == -0x7789fe3e) {
      piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
      iVar2 = (**(code **)(*piVar1 + 0x6c))(piVar1);
      if (iVar2 != 0) {
        return 0;
      }
    }
  }
  return 1;
}



// Function at 004a6270

undefined4 __thiscall OOAnalyzer::cls_0x5a3980::virt_meth_0x4a6270(cls_0x5a3980 *this)

{
  int *piVar1;
  int iVar2;
  
  if ((this->cls_0x5a68d8).mbr_0x34 != 0) {
    FUN_00482130();
    if ((this->cls_0x5a68d8).mbr_0x68 == 0) {
      return 0;
    }
    while( true ) {
      piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
      iVar2 = (**(code **)(*piVar1 + 0x80))(piVar1,0);
      if ((iVar2 == 0) || (iVar2 == -0x7789fdb8)) break;
      if (iVar2 == -0x7789fe3e) {
        piVar1 = (int *)(this->cls_0x5a68d8).mbr_0x68;
        iVar2 = (**(code **)(*piVar1 + 0x6c))(piVar1);
        if (iVar2 == 0) {
          FUN_00481c10((byte *)s_Couldn_t_restore_surface_005db014);
        }
      }
      else if (iVar2 != -0x7789fde4) {
        FUN_004a90d0(iVar2);
      }
    }
    (this->cls_0x5a68d8).mbr_0x34 = 0;
    FUN_00482140();
  }
  return 1;
}



// Function at 004a6930

undefined4 __thiscall
OOAnalyzer::cls_0x5a3980::virt_meth_0x4a6930
          (cls_0x5a3980 *this,undefined param_1,undefined param_2,undefined param_3,
          undefined param_4,undefined param_5,undefined param_6,undefined4 param_7)

{
  undefined uVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  undefined3 in_stack_00000005;
  undefined3 in_stack_00000009;
  undefined3 in_stack_0000000d;
  undefined3 in_stack_00000011;
  undefined3 in_stack_00000015;
  uint in_stack_00000020;
  uint uStack_bc;
  undefined4 uStack_b8;
  undefined4 uStack_b4;
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  undefined4 uStack_a8;
  undefined4 uStack_a4;
  undefined4 uStack_a0;
  undefined4 uStack_9c;
  undefined4 uStack_98;
  undefined4 uStack_94;
  undefined4 uStack_90;
  uint uStack_8c;
  undefined4 uStack_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined4 uStack_7c;
  undefined4 uStack_78;
  uint uStack_74;
  undefined4 uStack_70;
  undefined2 uStack_6c;
  undefined2 uStack_6a;
  uint uStack_68;
  undefined4 auStack_64 [17];
  uint uStack_20;
  undefined4 uStack_14;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (in_stack_00000020 == 0x80000000) {
    in_stack_00000020 = 0;
    uVar1 = (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x419e40_56)(this);
    if (CONCAT31(extraout_var,uVar1) != 0) {
      in_stack_00000020 = 0x400;
    }
  }
  if ((((DAT_006695a4 != 0) &&
       ((this->cls_0x5a68d8).mbr_0x38 == *(dword *)(PTR_DAT_005d79e0 + 0x38))) &&
      ((DAT_0066960c & 0x4000000) != 0)) &&
     ((uStack_68 = in_stack_00000020 & 0x400, uStack_68 == 0 || ((DAT_0066960c & 0x10000000) != 0)))
     ) {
    puVar5 = auStack_64;
    for (iVar3 = 0x19; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    auStack_64[0] = 100;
    uVar4 = uStack_68;
    if ((in_stack_00000020 & 0x400000) == 0) {
      uStack_14 = _param_5;
      uStack_94 = _param_1;
      uStack_8c = _param_3;
      uStack_90 = _param_2;
      uStack_b8 = 0;
      uStack_b4 = 0;
      uStack_b0 = 0;
      uStack_84 = 0;
      uStack_80 = 0;
      uStack_7c = 0;
      uStack_78 = 0;
      uStack_6a = 0;
      uStack_6c = 0;
      uStack_a8 = 0;
      uStack_ac = 0;
      uStack_98 = 0;
      uStack_9c = 0;
      uStack_a0 = 0;
      uStack_a4 = 0;
      uStack_70 = 0x1f;
      uStack_74 = 0;
      uStack_88 = _param_4;
      uStack_bc = in_stack_00000020 & 0xfffff3ff | 0x20000000;
      (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x4bd490_92)(&this->cls_0x5a68d8,&uStack_bc);
      uVar4 = uStack_74;
    }
    if ((uVar4 != 0) &&
       (uVar1 = (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x419e40_56)(this),
       CONCAT31(extraout_var_00,uVar1) != 0)) {
      uStack_20 = _param_3 & 0xffff;
      uStack_a0 = uStack_8;
      uStack_9c = uStack_4;
      uStack_bc = 0;
      uStack_90 = 0;
      uStack_8c = 0;
      uStack_88 = 0;
      uStack_84 = 0;
      uStack_78 = 0;
      uStack_b4 = 0;
      uStack_b8 = 0;
      uStack_a4 = 0;
      uStack_a8 = 0;
      uStack_ac = 0;
      uStack_b0 = 0;
      uStack_7c = 0x1f;
      uStack_80 = 0;
      uStack_94 = _param_4;
      (*((this->cls_0x5a68d8).vftptr_0x0)->virt_meth_0x4bd490_92)
                (&this->cls_0x5a68d8,&stack0xffffff38);
    }
    return 1;
  }
  uVar2 = cls_0x5a68d8::virt_meth_0x4bde60
                    (&this->cls_0x5a68d8,param_1,param_2,param_3,param_4,param_5,param_6,param_7);
  return uVar2;
}



