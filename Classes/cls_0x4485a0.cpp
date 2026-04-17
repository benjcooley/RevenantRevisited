// Decompiled methods and structure for class: cls_0x4485a0

/*
/OOAnalyzer/cls_0x4485a0
pack(disabled)
Structure cls_0x4485a0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 004485a0

void __thiscall OOAnalyzer::cls_0x4485a0::meth_0x4485a0(cls_0x4485a0 *this,undefined4 param_1)

{
  dword dVar1;
  int iVar2;
  int in_stack_00000008;
  
  this->mbr_0xc = 0;
  if (this->mbr_0x0 == 0) {
    dVar1 = FUN_0047f670(param_1,0xffffffff,0);
    this->mbr_0x0 = dVar1;
    if (dVar1 == 0) {
      return;
    }
  }
  if ((in_stack_00000008 != 0) && (iVar2 = FUN_004a1c00(in_stack_00000008,0), iVar2 != 0)) {
    dVar1 = FUN_004a1ec0(*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8)
                         ,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
    this->mbr_0x4 = dVar1;
    iVar2 = FUN_004a2ce0(in_stack_00000008);
    if (iVar2 != 0) goto LAB_00448626;
    FUN_004830f0(this->mbr_0x4);
  }
  this->mbr_0x4 = 0;
LAB_00448626:
  if (this->mbr_0x4 == 0) {
    dVar1 = cls_0x46d6b0::meth_0x46d710((cls_0x46d6b0 *)this->mbr_0x0);
    this->mbr_0x4 = dVar1;
  }
  dVar1 = cls_0x46d6b0::meth_0x46d710((cls_0x46d6b0 *)this->mbr_0x0);
  this->mbr_0x8 = dVar1;
  return;
}



// Function at 00448650

void __thiscall OOAnalyzer::cls_0x4485a0::meth_0x448650(cls_0x4485a0 *this)

{
  this->mbr_0xc = 0;
  if (this->mbr_0x0 != 0) {
    FUN_00482f80(this->mbr_0x0);
    this->mbr_0x0 = 0;
    this->mbr_0x8 = 0;
    this->mbr_0x4 = 0;
  }
  return;
}



// Function at 00448680

void __thiscall OOAnalyzer::cls_0x4485a0::meth_0x448680(cls_0x4485a0 *this,int param_1)

{
  undefined4 *puVar1;
  dword dVar2;
  int in_stack_00000008;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if ((this->mbr_0x8 != 0) && (puVar1 = (undefined4 *)this->mbr_0x4, puVar1 != (undefined4 *)0x0)) {
    dVar2 = this->mbr_0xc + param_1;
    this->mbr_0xc = dVar2;
    if (999 < (int)dVar2) {
      dVar2 = 1000;
    }
    this->mbr_0xc = dVar2;
    if (in_stack_00000008 != 0) {
      local_20 = puVar1[1];
      local_24 = *puVar1;
      local_54 = puVar1[5];
      local_50 = 0;
      local_4c = 0;
      local_48 = 0;
      local_2c = 0;
      local_28 = 0;
      local_1c = 0;
      local_18 = 0;
      local_2 = 0;
      local_4 = 0;
      local_40 = 0;
      local_44 = 0;
      local_30 = 0;
      local_34 = 0;
      local_38 = 0;
      local_3c = 0;
      local_8 = 0x1f;
      local_c = 0;
      local_14 = local_24;
      local_10 = local_20;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x58))(&local_54,puVar1);
    }
    dVar2 = this->mbr_0x8;
    local_24 = __ftol();
    local_28 = *(int *)(dVar2 + 0xc);
    if (local_28 < 0) {
      local_28 = -local_28;
    }
    local_2c = *(int *)(dVar2 + 8);
    if (local_2c < 0) {
      local_2c = -local_2c;
    }
    local_20 = *(undefined4 *)(dVar2 + 4);
    local_54 = *(undefined4 *)(this->mbr_0x4 + 0x14);
    local_50 = 0;
    local_4c = 0;
    local_48 = 0;
    local_1c = 0;
    local_18 = 0;
    local_2 = 0;
    local_4 = 0;
    local_40 = 0;
    local_44 = 0;
    local_30 = 0;
    local_34 = 0;
    local_38 = 0;
    local_3c = 0;
    local_8 = 0x1f;
    local_c = 0;
    local_14 = local_24;
    local_10 = local_20;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x58))(&local_54,dVar2);
    cls_0x4aa0c0::meth_0x4aa0c0((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
  }
  return;
}



// Function at 00448800

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x4485a0::meth_0x448800(cls_0x4485a0 *this,float param_1)

{
  undefined4 *puVar1;
  dword dVar2;
  int in_stack_00000008;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined2 local_4;
  undefined2 local_2;
  
  if ((this->mbr_0x8 != 0) && (puVar1 = (undefined4 *)this->mbr_0x4, puVar1 != (undefined4 *)0x0)) {
    dVar2 = __ftol();
    this->mbr_0xc = dVar2;
    if (in_stack_00000008 != 0) {
      local_24 = *puVar1;
      local_20 = puVar1[1];
      local_54 = puVar1[5];
      local_50 = 0;
      local_4c = 0;
      local_48 = 0;
      local_2c = 0;
      local_28 = 0;
      local_1c = 0;
      local_18 = 0;
      local_2 = 0;
      local_4 = 0;
      local_40 = 0;
      local_44 = 0;
      local_30 = 0;
      local_34 = 0;
      local_38 = 0;
      local_3c = 0;
      local_8 = 0x1f;
      local_c = 0;
      local_14 = local_24;
      local_10 = local_20;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x58))(&local_54,puVar1);
    }
    dVar2 = this->mbr_0x8;
    local_24 = __ftol();
    local_28 = *(int *)(dVar2 + 0xc);
    if (local_28 < 0) {
      local_28 = -local_28;
    }
    local_2c = *(int *)(dVar2 + 8);
    if (local_2c < 0) {
      local_2c = -local_2c;
    }
    local_20 = *(undefined4 *)(dVar2 + 4);
    local_54 = *(undefined4 *)(this->mbr_0x4 + 0x14);
    local_50 = 0;
    local_4c = 0;
    local_48 = 0;
    local_1c = 0;
    local_18 = 0;
    local_2 = 0;
    local_4 = 0;
    local_40 = 0;
    local_44 = 0;
    local_30 = 0;
    local_34 = 0;
    local_38 = 0;
    local_3c = 0;
    local_8 = 0x1f;
    local_c = 0;
    local_14 = local_24;
    local_10 = local_20;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x58))(&local_54,dVar2);
    cls_0x4aa0c0::meth_0x4aa0c0((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
  }
  return;
}



