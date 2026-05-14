#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x57a960



// Function at 0057a960

int cls_0x57a960::meth_0x57a960(undefined param_1)

{
  undefined *puVar1;
  int iVar2;
  char *pcVar3;
  undefined *puVar4;
  char *pcVar5;
  undefined in_stack_00000008;
  cls_0x588410 local_44;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a27b0;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x588410::cls_0x588410((cls_0x588410 *)&local_44.mbr_0x1c);
  local_4 = 0;
  cls_0x588410::cls_0x588410(&local_44);
  local_44.mbr_0x18 = 0;
  puVar4 = (undefined *)(local_44.mbr_0x28 + 1);
  local_4 = CONCAT31(local_4._1_3_,1);
  puVar1 = (undefined *)local_44.mbr_0x28;
  local_44.mbr_0x28 = (dword)puVar4;
  if (local_44.mbr_0x20 < puVar4) {
    puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_44.mbr_0x1c);
  }
  *puVar1 = param_1;
  puVar4 = (undefined *)(local_44.mbr_0x28 + 1);
  puVar1 = (undefined *)local_44.mbr_0x28;
  local_44.mbr_0x28 = (dword)puVar4;
  if (local_44.mbr_0x20 < puVar4) {
    puVar1 = (undefined *)cls_0x588410::meth_0x5884a0((cls_0x588410 *)&local_44.mbr_0x1c);
  }
  *puVar1 = in_stack_00000008;
  iVar2 = cls_0x5b9f28::meth_0x579ce0
                    ((cls_0x5b9f28 *)&this->mbr_0x248,6,local_44.mbr_0x1c,
                     local_44.mbr_0x28 - local_44.mbr_0x1c,&local_44,20000);
  if ((((iVar2 == 0) && (DAT_006669b0 = DAT_006669b0 | 8, DAT_00667fcc != (cls_0x5b4f30_TPlayer *)0x0)) &&
      (this->mbr_0xf0 != 0)) && (this->mbr_0xf4 == 0)) {
    pcVar5 = (char *)(local_44.mbr_0xc + 1);
    pcVar3 = (char *)local_44.mbr_0xc;
    local_44.mbr_0xc = (dword)pcVar5;
    if (local_44.mbr_0x4 < pcVar5) {
      pcVar3 = (char *)cls_0x588410::meth_0x5884a0(&local_44);
    }
    if (*pcVar3 == '\x19') {
      cls_0x588410::meth_0x589810(&local_44);
      local_44.mbr_0xc = local_44.mbr_0xc + 1;
      if (local_44.mbr_0x4 < local_44.mbr_0xc) {
        cls_0x588410::meth_0x5884a0(&local_44);
      }
      local_44.mbr_0xc = local_44.mbr_0xc + 1;
      if (local_44.mbr_0x4 < local_44.mbr_0xc) {
        cls_0x588410::meth_0x5884a0(&local_44);
      }
      cls_0x588410::meth_0x5892e0(&local_44);
      cls_0x5b4f30_TPlayer::meth_0x4d4610(DAT_00667fcc);
    }
  }
  local_4 = local_4 & 0xffffff00;
  cls_0x588410::meth_0x588480(&local_44);
  local_4 = 0xffffffff;
  cls_0x588410::meth_0x588480((cls_0x588410 *)&local_44.mbr_0x1c);
  ExceptionList = local_c;
  return iVar2;
}



