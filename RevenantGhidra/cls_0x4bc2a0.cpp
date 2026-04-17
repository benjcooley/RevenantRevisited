// Decompiled methods and structure for class: cls_0x4bc2a0

/*
/OOAnalyzer/cls_0x4bc2a0
pack(disabled)
Structure cls_0x4bc2a0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 144 Alignment: 1

*/

// Function at 004bc2a0

void __thiscall OOAnalyzer::cls_0x4bc2a0::meth_0x4bc2a0(cls_0x4bc2a0 *this)

{
  dword dVar1;
  HWND pHVar2;
  DWORD DVar3;
  DWORD DVar4;
  int *piVar5;
  DWORD DVar6;
  int iVar7;
  HWND in_stack_00000004;
  
  pHVar2 = GetFocus();
  if (pHVar2 == in_stack_00000004) {
    DVar3 = GetTickCount();
    DVar4 = GetTickCount();
    DVar6 = DVar3;
    if (DVar4 - DVar3 < 100) {
      do {
        piVar5 = (int *)(**(code **)(*(int *)this->mbr_0x4 + 0xc))();
        DVar6 = (**(code **)(*piVar5 + 100))(piVar5,0,&this->mbr_0x8,0,0);
        if (DVar6 == 0x887601c2) {
          piVar5 = (int *)(**(code **)(*(int *)this->mbr_0x4 + 0xc))();
          iVar7 = (**(code **)(*piVar5 + 0x6c))(piVar5);
          if (iVar7 != 0) {
            _SmackNextFrame_4(this->mbr_0x0);
            return;
          }
        }
        else if ((DVar6 != 0x887601ae) && (DVar6 != 0x8876021c)) break;
        DVar4 = GetTickCount();
      } while (DVar4 - DVar3 < 100);
    }
    if (DVar6 == 0) {
      _SmackToBuffer_28(this->mbr_0x0,0,0,this->mbr_0x18,this->mbr_0x10,this->mbr_0x2c,
                        this->mbr_0x84);
      _SmackDoFrame_4(this->mbr_0x0);
      piVar5 = (int *)(**(code **)(*(int *)this->mbr_0x4 + 0xc))();
      (**(code **)(*piVar5 + 0x80))(piVar5,0);
      (**(code **)(**(int **)(PTR_DAT_005d79e0 + 0x80) + 0x5c))(&stack0xffffffa4,this->mbr_0x4,0,0);
    }
    dVar1 = this->mbr_0x0;
    if (*(int *)(dVar1 + 0x374) == *(int *)(dVar1 + 0xc) + -1) {
      PostQuitMessage(0);
      return;
    }
    _SmackNextFrame_4(dVar1);
  }
  return;
}



// Function at 004bc470

void __thiscall OOAnalyzer::cls_0x4bc2a0::meth_0x4bc470(cls_0x4bc2a0 *this)

{
  dword *pdVar1;
  undefined4 uVar2;
  int iVar3;
  dword dVar4;
  cls_0x5a68d8 *this_00;
  int *piVar5;
  undefined4 uVar6;
  BOOL BVar7;
  int iVar8;
  dword *pdVar9;
  undefined4 in_stack_00000004;
  tagMSG tStack_28;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e103;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  _SmackSoundUseMSS_4(DAT_00667548);
  if (DAT_00668114 != 0) {
    _SmackSoundOnOff_8(this->mbr_0x0,0);
  }
  dVar4 = _SmackOpen_12(in_stack_00000004,0xfe000,0xffffffff);
  this->mbr_0x0 = dVar4;
  if (dVar4 != 0) {
    _SmackVolumePan_16(dVar4,0xfe000,DAT_005d7aa0 * 0xff,0x8000);
    this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
    uStack_4 = 0;
    if (this_00 == (cls_0x5a68d8 *)0x0) {
      this_00 = (cls_0x5a68d8 *)0x0;
    }
    else {
      uVar6 = *(undefined4 *)(this->mbr_0x0 + 8);
      uVar2 = *(undefined4 *)(this->mbr_0x0 + 4);
      cls_0x5a68d8::cls_0x5a68d8(this_00);
      uStack_4 = CONCAT31(uStack_4._1_3_,1);
      this_00->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
      this_00->mbr_0x68 = 0;
      cls_0x5a68d8::meth_0x4a5740(this_00,uVar2,uVar6);
      this_00->mbr_0x70 = 1;
    }
    pdVar1 = &this->mbr_0x8;
    this->mbr_0x4 = (dword)this_00;
    uStack_4 = 0xffffffff;
    pdVar9 = pdVar1;
    for (iVar8 = 0x1f; iVar8 != 0; iVar8 = iVar8 + -1) {
      *pdVar9 = 0;
      pdVar9 = pdVar9 + 1;
    }
    *pdVar1 = 0x7c;
    piVar5 = (int *)(**(code **)(*(int *)this->mbr_0x4 + 0xc))();
    iVar8 = (**(code **)(*piVar5 + 0x58))(piVar5,pdVar1);
    if (iVar8 != 0) {
      FUN_004a90d0(iVar8);
    }
    uVar6 = (**(code **)(*(int *)this->mbr_0x4 + 0xc))();
    dVar4 = _SmackDDSurfaceType_4(uVar6);
    piVar5 = (int *)this->mbr_0x4;
    iVar8 = piVar5[2];
    iVar3 = piVar5[1];
    this->mbr_0x84 = dVar4;
    (**(code **)(*piVar5 + 100))(0,0,iVar3,iVar8,0,0xffff,0x7f7f,0x80000000);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 100))
              (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),0,
               0xffff,0x7f7f,0x80000000);
    cls_0x5a68d8::meth_0x4a9ee0((cls_0x5a68d8 *)PTR_DAT_005d79e0);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 100))
              (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),0,
               0xffff,0x7f7f,0x80000000);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 100))
              (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),0,
               0xffff,0x7f7f,0x80000000);
    cls_0x5a68d8::meth_0x4a9ee0((cls_0x5a68d8 *)PTR_DAT_005d79e0);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 100))
              (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8),0,
               0xffff,0x7f7f,0x80000000);
    this->mbr_0x8c = 1;
    while( true ) {
      while( true ) {
        BVar7 = PeekMessageA(&tStack_28,(HWND)0x0,0,0,1);
        iVar8 = DAT_00667fd0;
        if (BVar7 != 0) break;
        iVar8 = _SmackWait_4(this->mbr_0x0);
        if (iVar8 == 0) {
          meth_0x4bc2a0(this);
        }
      }
      if (tStack_28.message == 0x12) break;
      TranslateMessage(&tStack_28);
      DispatchMessageA(&tStack_28);
    }
    this->mbr_0x8c = 0;
    if (iVar8 != 0) {
      *(undefined4 *)(iVar8 + 0x50) = 1;
    }
    _SmackClose_4(this->mbr_0x0);
    this->mbr_0x0 = 0;
  }
  ExceptionList = pvStack_c;
  return;
}



