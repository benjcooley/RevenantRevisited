// Decompiled methods and structure for class: cls_0x49ab30

/*
/OOAnalyzer/cls_0x49ab30
pack(disabled)
Structure cls_0x49ab30 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 128 Alignment: 1

*/

// Function at 0049ab30

void __thiscall OOAnalyzer::cls_0x49ab30::meth_0x49ab30(cls_0x49ab30 *this)

{
  LPCVOID *ppvVar1;
  LPCVOID pvVar2;
  int iVar3;
  dword *pdVar4;
  int iVar5;
  undefined4 local_4;
  
  if (DAT_00668114 == 0) {
    local_4 = 0;
    if (this->mbr_0x34 != 0) {
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_effect_volume_005da7a0,&local_4);
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_decay_time_005da7b4,&local_4);
      _AIL_set_3D_provider_preference_12
                (*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4),
                 s_EAX_damping_005da7c4,&local_4);
    }
    pdVar4 = &this->mbr_0x3c;
    iVar5 = 0x10;
    do {
      if (*pdVar4 != 0) {
        iVar3 = _AIL_sample_status_4(*pdVar4);
        if (iVar3 != 2) {
          _AIL_end_sample_4(*pdVar4);
        }
        _AIL_release_sample_handle_4(*pdVar4);
      }
      pdVar4 = pdVar4 + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    if (-1 < (int)this->mbr_0x4) {
      pdVar4 = &this->mbr_0x7c;
      iVar5 = 0x10;
      do {
        if (*pdVar4 != 0) {
          iVar3 = _AIL_3D_sample_status_4(*pdVar4);
          if (iVar3 != 2) {
            _AIL_end_3D_sample_4(*pdVar4);
          }
          _AIL_release_3D_sample_handle_4(*pdVar4);
        }
        pdVar4 = pdVar4 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      if (this->mbr_0x30 != 0) {
        _AIL_close_3D_listener_4(this->mbr_0x30);
      }
      _AIL_close_3D_provider_4(*(undefined4 *)(*(int *)(this->mbr_0x18 + this->mbr_0x4 * 4) + 4));
      this->mbr_0x4 = 0xffffffff;
    }
    if (this->mbr_0x0 != 0) {
      _AIL_waveOutClose_4(this->mbr_0x0);
      pdVar4 = &this->mbr_0x1c;
      iVar5 = 0;
      if (0 < (int)this->mbr_0x1c) {
        do {
          ppvVar1 = *(LPCVOID **)(this->mbr_0x2c + iVar5 * 4);
          if (ppvVar1 != (LPCVOID *)0x0) {
            if (ppvVar1[4] != (LPCVOID)0x0) {
              FUN_004830f0(ppvVar1[4]);
            }
            FUN_00482f80(*ppvVar1);
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)*pdVar4);
      }
      iVar5 = 0;
      if (0 < (int)*pdVar4) {
        do {
          if ((-1 < iVar5) &&
             (pvVar2 = *(LPCVOID *)(this->mbr_0x2c + iVar5 * 4), pvVar2 != (LPCVOID)0x0)) {
            FUN_004830f0(pvVar2);
          }
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar4);
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)*pdVar4);
      }
      *pdVar4 = 0;
      this->mbr_0x20 = 0;
    }
    pdVar4 = &this->mbr_0x8;
    iVar5 = 0;
    if (0 < (int)this->mbr_0x8) {
      do {
        if ((-1 < iVar5) &&
           (ppvVar1 = *(LPCVOID **)(this->mbr_0x18 + iVar5 * 4), ppvVar1 != (LPCVOID *)0x0)) {
          if (*ppvVar1 != (LPCVOID)0x0) {
            FUN_004830f0(*ppvVar1);
          }
          FUN_004830f0(ppvVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar4);
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)*pdVar4);
    }
    *pdVar4 = 0;
    this->mbr_0xc = 0;
    _AIL_shutdown_0();
  }
  return;
}



