// Decompiled methods and structure for class: cls_0x41cbf0

/*
/OOAnalyzer/cls_0x41cbf0
pack(disabled)
Structure cls_0x41cbf0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 44 Alignment: 1

*/

// Function at 0041cbf0

void __thiscall OOAnalyzer::cls_0x41cbf0::meth_0x41cbf0(cls_0x41cbf0 *this)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  dword in_stack_00000004;
  
  dVar1 = (int)(in_stack_00000004 << 0x14) / 0x3000;
  this->mbr_0x0 = dVar1;
  puVar2 = FUN_00482ef0(dVar1 * 4);
  this->mbr_0x4 = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 2);
  this->mbr_0x8 = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 2);
  this->mbr_0xc = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 2);
  this->mbr_0x18 = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 2);
  this->mbr_0x1c = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 2);
  dVar1 = this->mbr_0x0;
  this->mbr_0x20 = (dword)puVar2;
  if (0 < (int)dVar1) {
    puVar4 = (undefined4 *)this->mbr_0x8;
    iVar5 = this->mbr_0x4 - (int)puVar4;
    iVar8 = this->mbr_0xc - (int)puVar4;
    iVar6 = this->mbr_0x18 - (int)puVar4;
    iVar7 = this->mbr_0x1c - (int)puVar4;
    iVar3 = (int)puVar2 - (int)puVar4;
    in_stack_00000004 = dVar1;
    do {
      *(undefined4 *)(iVar5 + (int)puVar4) = 0;
      *puVar4 = 0;
      *(undefined4 *)(iVar8 + (int)puVar4) = 0;
      *(undefined4 *)(iVar6 + (int)puVar4) = 0;
      *(undefined4 *)((int)puVar4 + iVar7) = 0;
      *(undefined4 *)(iVar3 + (int)puVar4) = 0;
      puVar4 = puVar4 + 1;
      in_stack_00000004 = in_stack_00000004 - 1;
    } while (in_stack_00000004 != 0);
  }
  this->mbr_0x14 = 0;
  this->mbr_0x28 = 0;
  puVar2 = FUN_00482ef0(dVar1 << 0xc);
  this->mbr_0x10 = (dword)puVar2;
  puVar2 = FUN_00482ef0(this->mbr_0x0 << 0xd);
  this->mbr_0x24 = (dword)puVar2;
  return;
}



