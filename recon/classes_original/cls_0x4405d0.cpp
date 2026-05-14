// Decompiled methods and structure for class: cls_0x4405d0

/*
/OOAnalyzer/cls_0x4405d0
pack(disabled)
Structure cls_0x4405d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   1404   dword   4   mbr_0x57c   "Unsigned Double-Word (ddw, 4-bytes)"
   1408   dword   4   mbr_0x580   "Unsigned Double-Word (ddw, 4-bytes)"
   1416   dword   4   mbr_0x588   "Unsigned Double-Word (ddw, 4-bytes)"
   1420   dword   4   mbr_0x58c   "Unsigned Double-Word (ddw, 4-bytes)"
   1424   dword   4   mbr_0x590   "Unsigned Double-Word (ddw, 4-bytes)"
   1428   dword   4   mbr_0x594   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1432 Alignment: 1

*/

// Function at 004405d0

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x4405d0(cls_0x4405d0 *this,dword param_1)

{
  dword dVar1;
  dword dVar2;
  dword *pdVar3;
  int iVar4;
  int in_stack_00000008;
  
  if ((-1 < (int)param_1) && ((in_stack_00000008 == 0 || ((int)this->mbr_0x57c < 0x100)))) {
    dVar1 = this->mbr_0x57c;
    dVar2 = 0;
    this->mbr_0x588 = 0;
    if (0 < (int)dVar1) {
      pdVar3 = &this->mbr_0x17c;
      do {
        if (*pdVar3 == param_1) {
          if ((int)dVar2 < (int)dVar1) {
            this->mbr_0x580 = dVar2;
            (**(code **)(this->mbr_0x0 + 0x2c))(1);
          }
          cls_0x443330::meth_0x443330((cls_0x443330 *)&DAT_00658018);
          return 1;
        }
        dVar2 = dVar2 + 1;
        pdVar3 = pdVar3 + 1;
      } while ((int)dVar2 < (int)dVar1);
    }
    if (in_stack_00000008 == 0) {
      if (0 < (int)dVar1) {
        iVar4 = 0;
        do {
          cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
          iVar4 = iVar4 + 1;
        } while (iVar4 < (int)this->mbr_0x57c);
      }
      this->mbr_0x57c = 0;
    }
    this->mbr_0x580 = this->mbr_0x57c;
    this->mbr_0x57c = this->mbr_0x57c + 1;
    (&this->mbr_0x17c)[this->mbr_0x580] = param_1;
    (**(code **)(this->mbr_0x0 + 0x2c))(1);
    cls_0x443330::meth_0x443330((cls_0x443330 *)&DAT_00658018);
    cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
    return 1;
  }
  return 0;
}



// Function at 004406e0

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x4406e0(cls_0x4405d0 *this)

{
  int *piVar1;
  int iVar2;
  dword *pdVar3;
  dword dVar4;
  
  if ((int)this->mbr_0x580 < 0) {
    return 0;
  }
  iVar2 = 0;
  if (0 < (int)this->mbr_0x57c) {
    pdVar3 = &this->mbr_0x17c;
    do {
      piVar1 = (int *)FUN_00452690_TMapPane_GetInstance(*pdVar3,0);
      if (piVar1 != (int *)0x0) {
        if ((piVar1[2] & 0x400U) != 0) {
          (**(code **)(*piVar1 + 0x40))(piVar1[2] & 0xfffffbff);
          if ((*(byte *)(piVar1 + 2) & 8) == 0) {
            cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
          }
        }
        if (-1 < piVar1[0x14]) {
          piVar1 = (int *)FUN_00452690_TMapPane_GetInstance(piVar1[0x14],0);
          if ((piVar1 != (int *)0x0) && ((piVar1[2] & 0x400U) != 0)) {
            (**(code **)(*piVar1 + 0x40))(piVar1[2] & 0xfffffbff);
            if ((*(byte *)(piVar1 + 2) & 8) == 0) {
              cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
            }
          }
        }
      }
      iVar2 = iVar2 + 1;
      pdVar3 = pdVar3 + 1;
    } while (iVar2 < (int)this->mbr_0x57c);
  }
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  iVar2 = FUN_00452690_TMapPane_GetInstance((&this->mbr_0x17c)[this->mbr_0x580],0);
  if (iVar2 != 0) {
    cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
  }
  dVar4 = this->mbr_0x580;
  FUN_0058b790(&this->mbr_0x17c + dVar4,&this->field_0x180 + dVar4 * 4,
               ((dVar4 + 1) * 0x3fffffff + this->mbr_0x57c) * 4);
  dVar4 = this->mbr_0x57c - 1;
  this->mbr_0x57c = dVar4;
  if ((int)dVar4 <= (int)this->mbr_0x580) {
    this->mbr_0x580 = this->mbr_0x580 - 1;
  }
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  return 1;
}



// Function at 00440840

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440840(cls_0x4405d0 *this)

{
  dword *pdVar1;
  int iVar2;
  dword in_stack_00000004;
  
  iVar2 = 0;
  this->mbr_0x588 = 0;
  if (0 < (int)this->mbr_0x57c) {
    pdVar1 = &this->mbr_0x17c;
    do {
      if (*pdVar1 == in_stack_00000004) {
        cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
        FUN_0058b790(&this->mbr_0x17c + iVar2,&this->field_0x180 + iVar2 * 4,
                     ((iVar2 + 1) * 0x3fffffff + this->mbr_0x57c) * 4);
        this->mbr_0x57c = this->mbr_0x57c - 1;
        if (iVar2 <= (int)this->mbr_0x580) {
          this->mbr_0x580 = this->mbr_0x580 - 1;
        }
        (**(code **)(this->mbr_0x0 + 0x2c))(1);
        return 1;
      }
      iVar2 = iVar2 + 1;
      pdVar1 = pdVar1 + 1;
    } while (iVar2 < (int)this->mbr_0x57c);
  }
  return 0;
}



// Function at 00440940

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440940(cls_0x4405d0 *this)

{
  dword in_stack_00000004;
  
  if ((int)this->mbr_0x57c <= (int)in_stack_00000004) {
    return 0;
  }
  this->mbr_0x580 = in_stack_00000004;
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  return 1;
}



// Function at 00440970

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440970(cls_0x4405d0 *this)

{
  if ((int)(this->mbr_0x57c - 1) <= (int)this->mbr_0x580) {
    return 0;
  }
  this->mbr_0x580 = this->mbr_0x580 + 1;
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  return 1;
}



// Function at 004409a0

undefined4 __thiscall OOAnalyzer::cls_0x4405d0::meth_0x4409a0(cls_0x4405d0 *this)

{
  if ((int)this->mbr_0x580 < 1) {
    return 0;
  }
  this->mbr_0x580 = this->mbr_0x580 - 1;
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  return 1;
}



// Function at 00440b20

void __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440b20(cls_0x4405d0 *this)

{
  byte bVar1;
  dword dVar2;
  int *piVar3;
  dword *pdVar4;
  dword *pdVar5;
  int local_10;
  cls_0x41c750 local_c;
  
  local_10 = 0;
  if (0 < (int)this->mbr_0x57c) {
    pdVar4 = &this->mbr_0x17c;
    pdVar5 = &this->mbr_0x594;
    do {
      this->mbr_0x588 = 1;
      piVar3 = (int *)FUN_00452690_TMapPane_GetInstance(*pdVar4,0);
      if (piVar3 != (int *)0x0) {
        pdVar5[-2] = piVar3[4];
        dVar2 = piVar3[5];
        *pdVar5 = piVar3[6];
        bVar1 = *(byte *)(piVar3 + 2);
        pdVar5[-1] = dVar2;
        if (((bVar1 & 2) == 0) && (cls_0x41c750::meth_0x41c750(&local_c), (piVar3[2] & 0x400U) == 0)
           ) {
          cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
          (**(code **)(*piVar3 + 0x40))(piVar3[2] | 0x400);
        }
        if ((((-1 < piVar3[0x14]) &&
             (piVar3 = (int *)FUN_00452690_TMapPane_GetInstance(piVar3[0x14],0), piVar3 != (int *)0x0)) &&
            ((piVar3[2] & 2U) == 0)) && ((piVar3[2] & 0x400U) == 0)) {
          cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
          (**(code **)(*piVar3 + 0x40))(piVar3[2] | 0x400);
        }
      }
      local_10 = local_10 + 1;
      pdVar4 = pdVar4 + 1;
      pdVar5 = pdVar5 + 3;
    } while (local_10 < (int)this->mbr_0x57c);
  }
  return;
}



// Function at 00440c60

void __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440c60(cls_0x4405d0 *this)

{
  int *piVar1;
  int iVar2;
  dword *pdVar3;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x57c) {
    pdVar3 = &this->mbr_0x17c;
    do {
      piVar1 = (int *)FUN_00452690_TMapPane_GetInstance(*pdVar3,0);
      if (piVar1 != (int *)0x0) {
        if (((piVar1[2] & 0x400U) != 0) &&
           ((**(code **)(*piVar1 + 0x40))(piVar1[2] & 0xfffffbff), (*(byte *)(piVar1 + 2) & 8) == 0)
           ) {
          cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
        }
        if ((((-1 < piVar1[0x14]) &&
             (piVar1 = (int *)FUN_00452690_TMapPane_GetInstance(piVar1[0x14],0), piVar1 != (int *)0x0)) &&
            ((piVar1[2] & 0x400U) != 0)) &&
           ((**(code **)(*piVar1 + 0x40))(piVar1[2] & 0xfffffbff), (*(byte *)(piVar1 + 2) & 8) == 0)
           ) {
          cls_0x5a5320_TPlayScreen::meth_0x454920((cls_0x5a5320_TPlayScreen *)&DAT_006668d8);
        }
      }
      iVar2 = iVar2 + 1;
      pdVar3 = pdVar3 + 1;
    } while (iVar2 < (int)this->mbr_0x57c);
  }
  (**(code **)(this->mbr_0x0 + 0x2c))(1);
  return;
}



// Function at 00440d20

void __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440d20(cls_0x4405d0 *this)

{
  int *piVar1;
  int iVar2;
  dword *pdVar3;
  dword *pdVar4;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x57c) {
    pdVar4 = &this->mbr_0x58c;
    pdVar3 = &this->mbr_0x17c;
    do {
      piVar1 = (int *)FUN_00452690_TMapPane_GetInstance(*pdVar3,0);
      (**(code **)(*piVar1 + 8))(pdVar4,0xffffffff,0);
      iVar2 = iVar2 + 1;
      pdVar3 = pdVar3 + 1;
      pdVar4 = pdVar4 + 3;
    } while (iVar2 < (int)this->mbr_0x57c);
  }
  return;
}



// Function at 00440d70

void __thiscall OOAnalyzer::cls_0x4405d0::meth_0x440d70(cls_0x4405d0 *this)

{
  int iVar1;
  bool bVar2;
  int *piVar3;
  int unaff_ESI;
  int unaff_retaddr;
  int in_stack_00000004;
  undefined local_10 [16];
  
  piVar3 = (int *)FUN_00452690_TMapPane_GetInstance((&this->mbr_0x17c)[this->mbr_0x580],0);
  if ((piVar3 != (int *)0x0) && (*(short *)(piVar3 + 1) == 9)) {
    iVar1 = piVar3[4];
    (**(code **)(*piVar3 + 0xf4))(local_10);
    bVar2 = false;
    if ((unaff_ESI + 0x80 < unaff_retaddr) || (unaff_retaddr < unaff_ESI + -0x80)) {
      bVar2 = true;
    }
    if (((iVar1 + 0x80 < in_stack_00000004) || (in_stack_00000004 < iVar1 + -0x80)) || (bVar2)) {
      cls_0x5b93c4::meth_0x4430f0((cls_0x5b93c4 *)&DAT_00658018);
    }
  }
  return;
}



