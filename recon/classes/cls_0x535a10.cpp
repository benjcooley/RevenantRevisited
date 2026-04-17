// Decompiled methods and structure for class: cls_0x535a10

/*
/OOAnalyzer/cls_0x535a10
pack(disabled)
Structure cls_0x535a10 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   472   dword   4   mbr_0x1d8   "Unsigned Double-Word (ddw, 4-bytes)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   484   dword   4   mbr_0x1e4   "Unsigned Double-Word (ddw, 4-bytes)"
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 496 Alignment: 1

*/

// Function at 00535a10

void __thiscall OOAnalyzer::cls_0x535a10::meth_0x535a10(cls_0x535a10 *this)

{
  cls_0x4367d0 **ppcVar1;
  int iVar2;
  dword *pdVar3;
  cls_0x4367d0 **ppcVar4;
  
  ppcVar1 = (cls_0x4367d0 **)this->mbr_0x190;
  if (ppcVar1 != (cls_0x4367d0 **)0x0) {
    iVar2 = 0;
    ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
    ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
    if (0 < (int)ppcVar1[0x17]) {
      ppcVar4 = ppcVar1 + 0x40;
      do {
        if (*ppcVar4 != (cls_0x4367d0 *)0x0) {
          cls_0x4367d0::meth_0x4367d0(*ppcVar1);
          *ppcVar4 = (cls_0x4367d0 *)0x0;
        }
        iVar2 = iVar2 + 1;
        ppcVar4 = ppcVar4 + 1;
      } while (iVar2 < (int)ppcVar1[0x17]);
    }
    this->mbr_0x190 = 0;
  }
  iVar2 = 0;
  if (0 < (int)this->mbr_0x1d8) {
    pdVar3 = &this->mbr_0x1b8;
    do {
      if (pdVar3[-8] != 0) {
        FUN_00482f80(pdVar3[-8]);
      }
      if (*pdVar3 != 0) {
        FUN_00482f80(*pdVar3);
      }
      iVar2 = iVar2 + 1;
      pdVar3 = pdVar3 + 1;
    } while (iVar2 < (int)this->mbr_0x1d8);
  }
  this->mbr_0x1d8 = 0;
  this->mbr_0x1dc = 0xffffffff;
  this->mbr_0x1e4 = 0;
  return;
}



// Function at 00535d80

void __thiscall OOAnalyzer::cls_0x535a10::meth_0x535d80(cls_0x535a10 *this)

{
  dword *this_00;
  int iVar1;
  cls_0x4367d0 **ppcVar2;
  int iVar3;
  cls_0x4367d0 **ppcVar4;
  uint in_stack_00000004;
  
  if (this->mbr_0x40 != 0) {
    if (this->mbr_0x190 != 0) {
      meth_0x536010(this);
    }
    meth_0x535a10(this);
  }
  if (in_stack_00000004 != 0) {
    this_00 = &this->mbr_0x17c;
    iVar3 = 0;
    if (0 < (int)this->mbr_0x17c) {
      do {
        if ((-1 < iVar3) && (iVar1 = *(int *)(this->mbr_0x18c + iVar3 * 4), iVar1 != 0)) {
          FUN_005343e0();
          FUN_004830f0(iVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar3 = iVar3 + 1;
      } while (iVar3 < (int)*this_00);
    }
    *this_00 = 0;
    this->mbr_0x180 = 0;
    return;
  }
  in_stack_00000004 = 0;
  if (0 < (int)this->mbr_0x17c) {
    do {
      iVar3 = 0;
      if (((this->mbr_0x18c != 0) && (in_stack_00000004 < this->mbr_0x17c)) &&
         (*(int *)(this->mbr_0x18c + in_stack_00000004 * 4) != 0)) {
        ppcVar2 = *(cls_0x4367d0 ***)(this->mbr_0x18c + in_stack_00000004 * 4);
        ppcVar2[0x14] = (cls_0x4367d0 *)0x1;
        ppcVar2[0x16] = (cls_0x4367d0 *)0x0;
        if (0 < (int)ppcVar2[0x17]) {
          ppcVar4 = ppcVar2 + 0x40;
          do {
            if (*ppcVar4 != (cls_0x4367d0 *)0x0) {
              cls_0x4367d0::meth_0x4367d0(*ppcVar2);
              *ppcVar4 = (cls_0x4367d0 *)0x0;
            }
            iVar3 = iVar3 + 1;
            ppcVar4 = ppcVar4 + 1;
          } while (iVar3 < (int)ppcVar2[0x17]);
        }
      }
      in_stack_00000004 = in_stack_00000004 + 1;
    } while ((int)in_stack_00000004 < (int)this->mbr_0x17c);
  }
  return;
}



// Function at 00536010

void __thiscall OOAnalyzer::cls_0x535a10::meth_0x536010(cls_0x535a10 *this)

{
  cls_0x4367d0 **ppcVar1;
  cls_0x4d6000 *this_00;
  int iVar2;
  uint uVar3;
  cls_0x4367d0 **ppcVar4;
  
  if (((this->mbr_0x40 != 0) && (this->mbr_0x190 == 0)) && (uVar3 = 0, 0 < (int)this->mbr_0x17c)) {
    do {
      iVar2 = 0;
      if (((this->mbr_0x18c != 0) && (uVar3 < this->mbr_0x17c)) &&
         (*(int *)(this->mbr_0x18c + uVar3 * 4) != 0)) {
        this_00 = (cls_0x4d6000 *)
                  FUN_00452690(*(undefined4 *)(*(int *)(this->mbr_0x18c + uVar3 * 4) + 4),2);
        if (this_00 != (cls_0x4d6000 *)0x0) {
          cls_0x4d6000::meth_0x4d6000(this_00);
        }
        ppcVar1 = *(cls_0x4367d0 ***)(this->mbr_0x18c + uVar3 * 4);
        ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
        ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
        if (0 < (int)ppcVar1[0x17]) {
          ppcVar4 = ppcVar1 + 0x40;
          do {
            if (*ppcVar4 != (cls_0x4367d0 *)0x0) {
              cls_0x4367d0::meth_0x4367d0(*ppcVar1);
              *ppcVar4 = (cls_0x4367d0 *)0x0;
            }
            iVar2 = iVar2 + 1;
            ppcVar4 = ppcVar4 + 1;
          } while (iVar2 < (int)ppcVar1[0x17]);
        }
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)this->mbr_0x17c);
  }
  return;
}



// Function at 005360f0

void __thiscall OOAnalyzer::cls_0x535a10::meth_0x5360f0(cls_0x535a10 *this)

{
  cls_0x4367d0 **ppcVar1;
  int iVar2;
  dword *pdVar3;
  cls_0x4367d0 **ppcVar4;
  
  if (this->mbr_0x40 != 0) {
    ppcVar1 = (cls_0x4367d0 **)this->mbr_0x190;
    if (ppcVar1 != (cls_0x4367d0 **)0x0) {
      iVar2 = 0;
      ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
      ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
      if (0 < (int)ppcVar1[0x17]) {
        ppcVar4 = ppcVar1 + 0x40;
        do {
          if (*ppcVar4 != (cls_0x4367d0 *)0x0) {
            cls_0x4367d0::meth_0x4367d0(*ppcVar1);
            *ppcVar4 = (cls_0x4367d0 *)0x0;
          }
          iVar2 = iVar2 + 1;
          ppcVar4 = ppcVar4 + 1;
        } while (iVar2 < (int)ppcVar1[0x17]);
      }
      this->mbr_0x190 = 0;
    }
    iVar2 = 0;
    if (0 < (int)this->mbr_0x1d8) {
      pdVar3 = &this->mbr_0x1b8;
      do {
        if (pdVar3[-8] != 0) {
          FUN_00482f80(pdVar3[-8]);
        }
        if (*pdVar3 != 0) {
          FUN_00482f80(*pdVar3);
        }
        iVar2 = iVar2 + 1;
        pdVar3 = pdVar3 + 1;
      } while (iVar2 < (int)this->mbr_0x1d8);
    }
    this->mbr_0x1d8 = 0;
    this->mbr_0x1dc = 0xffffffff;
    this->mbr_0x1e4 = 0;
    if (this->mbr_0x40 != 0) {
      if (this->mbr_0x190 != 0) {
        meth_0x536010(this);
      }
      meth_0x535a10(this);
    }
    pdVar3 = &this->mbr_0x17c;
    iVar2 = 0;
    if (0 < (int)this->mbr_0x17c) {
      do {
        cls_0x45f7c0::meth_0x536320((cls_0x45f7c0 *)pdVar3);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)*pdVar3);
    }
    iVar2 = 0;
    *pdVar3 = 0;
    this->mbr_0x180 = 0;
    ppcVar1 = (cls_0x4367d0 **)this->mbr_0x190;
    if (ppcVar1 != (cls_0x4367d0 **)0x0) {
      ppcVar1[0x14] = (cls_0x4367d0 *)0x1;
      ppcVar1[0x16] = (cls_0x4367d0 *)0x0;
      if (0 < (int)ppcVar1[0x17]) {
        ppcVar4 = ppcVar1 + 0x40;
        do {
          if (*ppcVar4 != (cls_0x4367d0 *)0x0) {
            cls_0x4367d0::meth_0x4367d0(*ppcVar1);
            *ppcVar4 = (cls_0x4367d0 *)0x0;
          }
          iVar2 = iVar2 + 1;
          ppcVar4 = ppcVar4 + 1;
        } while (iVar2 < (int)ppcVar1[0x17]);
      }
      this->mbr_0x190 = 0;
      cls_0x5a5320::meth_0x47c580((cls_0x5a5320 *)&DAT_0065caf0);
      if (this->mbr_0x194 != 0) {
        FUN_00471290(s_Finish_005e3f54);
      }
      if ((this->mbr_0x194 != 0) && (*(int *)(this->mbr_0x194 + 0xd8) != 0)) {
        iVar2 = FUN_004dab80(&DAT_005e3f5c);
        if (iVar2 != 0) {
          *(undefined4 *)(this->mbr_0x194 + 0x108) = 1;
        }
      }
      (**(code **)(this->mbr_0x0 + 0x34))();
    }
  }
  return;
}



