// Decompiled methods and structure for class: cls_0x5a5e1c

/*
/OOAnalyzer/cls_0x5a5e1c
pack(disabled)
Structure cls_0x5a5e1c {
   0   cls_0x5a5e1c::vftable_5a5e1c *   4   vftptr_0x0   "pointer to cls_0x5a5e1c::vftable_5a5e1c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   441   byte   1   mbr_0x1b9   "Unsigned Byte (db)"
   442   dword   4   mbr_0x1ba   "Unsigned Double-Word (ddw, 4-bytes)"
   457   byte   1   mbr_0x1c9   "Unsigned Byte (db)"
   460   dword   4   mbr_0x1cc   "Unsigned Double-Word (ddw, 4-bytes)"
   464   dword   4   mbr_0x1d0   "Unsigned Double-Word (ddw, 4-bytes)"
   484   dword   4   mbr_0x1e4   "Unsigned Double-Word (ddw, 4-bytes)"
   500   dword   4   mbr_0x1f4   "Unsigned Double-Word (ddw, 4-bytes)"
   532   dword   4   mbr_0x214   "Unsigned Double-Word (ddw, 4-bytes)"
   596   dword   4   mbr_0x254   "Unsigned Double-Word (ddw, 4-bytes)"
   636   byte   1   mbr_0x27c   "Unsigned Byte (db)"
   680   dword   4   mbr_0x2a8   "Unsigned Double-Word (ddw, 4-bytes)"
   684   dword   4   mbr_0x2ac   "Unsigned Double-Word (ddw, 4-bytes)"
   688   dword   4   mbr_0x2b0   "Unsigned Double-Word (ddw, 4-bytes)"
   692   dword   4   mbr_0x2b4   "Unsigned Double-Word (ddw, 4-bytes)"
   696   dword   4   mbr_0x2b8   "Unsigned Double-Word (ddw, 4-bytes)"
   700   dword   4   mbr_0x2bc   "Unsigned Double-Word (ddw, 4-bytes)"
   704   dword   4   mbr_0x2c0   "Unsigned Double-Word (ddw, 4-bytes)"
   708   dword   4   mbr_0x2c4   "Unsigned Double-Word (ddw, 4-bytes)"
   712   dword   4   mbr_0x2c8   "Unsigned Double-Word (ddw, 4-bytes)"
   716   dword   4   mbr_0x2cc   "Unsigned Double-Word (ddw, 4-bytes)"
   720   dword   4   mbr_0x2d0   "Unsigned Double-Word (ddw, 4-bytes)"
   724   dword   4   mbr_0x2d4   "Unsigned Double-Word (ddw, 4-bytes)"
   728   dword   4   mbr_0x2d8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 732 Alignment: 1

*/

// Function at 00463e00

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x5a5e1c::virt_meth_0x463e00(cls_0x5a5e1c *this)

{
  undefined *puVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  
  piVar3 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  iVar4 = _DAT_006668d0;
  puVar1 = PTR_DAT_005d79e0;
  this->mbr_0x17c = (dword)piVar3;
  piVar3[6] = iVar4;
  piVar3 = FUN_004a1ec0(0x41,0xe,*(uint *)(puVar1 + 0x38) & 0x3001f,0);
  uVar2 = DAT_0066733c;
  this->mbr_0x180 = (dword)piVar3;
  this->mbr_0x2a8 = 0;
  iVar4 = cls_0x5b93c4::virt_meth_0x435230
                    ((cls_0x5b93c4 *)this,s_mpingame_005d2ebc,s_playlistpanel_005d2eac,0x11,0,0,
                     0x280,0x1e0,0x1c2,0xa0,uVar2);
  if (iVar4 == 0) {
    return 0;
  }
  this->mbr_0x2b0 = 0;
  this->mbr_0x2b4 = 0;
  (*this->vftptr_0x0->virt_meth_0x435de0_80)((cls_0x5b98b8 *)this);
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return 1;
}



// Function at 00463ed0

void __thiscall OOAnalyzer::cls_0x5a5e1c::virt_meth_0x463ed0(cls_0x5a5e1c *this)

{
  FUN_004830f0((LPCVOID)this->mbr_0x17c);
  FUN_004830f0((LPCVOID)this->mbr_0x180);
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 00465760

dword __thiscall OOAnalyzer::cls_0x5a5e1c::virt_meth_0x465760(cls_0x5a5e1c *this,byte *param_1)

{
  uint uVar1;
  int iVar2;
  TPlayScreen *this_00;
  int *piVar3;
  dword dVar4;
  uint extraout_ECX;
  dword extraout_ECX_00;
  int in_stack_00000008;
  cls_0x419dd0 cVar5;
  undefined2 uVar6;
  
  uVar1 = FUN_0059a530_stricmp((uint)this,param_1,(byte *)s_smallicon_005d3278);
  if (uVar1 != 0) {
    uVar1 = FUN_0059a530_stricmp(extraout_ECX,param_1,(byte *)s_partycolor_005d3284);
    if (uVar1 == 0) {
      uVar6 = 0;
      cVar5.mbr_0x0 = extraout_ECX_00;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffec);
      cls_0x4a22f0::meth_0x4a22f0((cls_0x4a22f0 *)this->mbr_0x180,cVar5.mbr_0x0,uVar6);
      return this->mbr_0x180;
    }
    dVar4 = cls_0x5b93c4::virt_meth_0x436de0((cls_0x5b93c4 *)this);
    return dVar4;
  }
  iVar2 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
  if (in_stack_00000008 < iVar2) {
    this_00 = (TPlayScreen *)
              cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,in_stack_00000008);
    if (this_00 != (TPlayScreen *)0x0) {
      iVar2 = TPlayScreen::meth_0x46e8a0(this_00);
      if (iVar2 != 0) {
        piVar3 = (int *)TPlayScreen::meth_0x46e8a0(this_00);
        piVar3 = (int *)(**(code **)(*piVar3 + 0xd4))();
        if (piVar3 != (int *)0x0) {
          FUN_004a31a0(piVar3,(int *)this->mbr_0x17c,DAT_006668d0);
          return this->mbr_0x17c;
        }
      }
    }
  }
  return 0;
}



// Function at 00465850

void __thiscall OOAnalyzer::cls_0x5a5e1c::virt_meth_0x465850(cls_0x5a5e1c *this)

{
  cls_0x5a3d44 *this_00;
  dword dVar1;
  int iVar2;
  int iVar3;
  
  cls_0x5b93c4::virt_meth_0x435d70((cls_0x5b93c4 *)this);
  if ((this->mbr_0x2b0 != 0) &&
     (iVar2 = cls_0x57b200::meth_0x57b200((cls_0x57b200 *)&DAT_00676738), iVar2 != 0)) {
    cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
    cls_0x5a3d44::meth_0x430c50((cls_0x5a3d44 *)this->mbr_0x2b0);
    (**(code **)(*(int *)this->mbr_0x2b0 + 0x1c))(((int *)this->mbr_0x2b0)[5] | 0x20);
  }
  this_00 = (cls_0x5a3d44 *)this->mbr_0x2b4;
  if ((this_00 != (cls_0x5a3d44 *)0x0) && (this->mbr_0x2ac != DAT_006766b0)) {
    iVar2 = (this_00->mbr_0xa4 - 1) + this_00->mbr_0x9c;
    iVar3 = this_00->mbr_0x98 - 1;
    if (iVar2 < iVar3) {
      iVar3 = iVar2;
    }
    dVar1 = this_00->mbr_0x98;
    cls_0x5a3d44::meth_0x430c50(this_00);
    if ((int)(dVar1 - 1) <= iVar3) {
      cls_0x5a3d44::meth_0x430ab0((cls_0x5a3d44 *)this->mbr_0x2b4);
    }
    (**(code **)(*(int *)this->mbr_0x2b4 + 0x1c))(((int *)this->mbr_0x2b4)[5] | 0x20);
    this->mbr_0x2ac = DAT_006766b0;
  }
  return;
}



// Function at 004890b0

cls_0x5a5e1c * __thiscall OOAnalyzer::cls_0x5a5e1c::cls_0x5a5e1c(cls_0x5a5e1c *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a5e1c__vftable_5a5e1c *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5e1c__vftable_5a5e1c_005a5e1c;
  return this;
}



// Function at 00489110

cls_0x5a5e1c * __thiscall OOAnalyzer::cls_0x5a5e1c::virt_meth_0x489110(cls_0x5a5e1c *this)

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5e1c(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5e1c *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00489150

cls_0x5a5e1c * __thiscall OOAnalyzer::cls_0x5a5e1c::~cls_0x5a5e1c(cls_0x5a5e1c *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5e1c *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d7ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a5e1c__vftable_5a5e1c *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5e1c *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



