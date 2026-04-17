// Decompiled methods and structure for class: cls_0x44ceb0

/*
/OOAnalyzer/cls_0x44ceb0
pack(disabled)
Structure cls_0x44ceb0 {
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
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 72 Alignment: 1

*/

// Function at 0044ceb0

cls_0x44ceb0 * __thiscall
OOAnalyzer::cls_0x44ceb0::cls_0x44ceb0
          (cls_0x44ceb0 *this,int *param_1,int param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5)

{
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  local_10 = *param_1 - param_2;
  local_8 = *param_1 + param_2;
  local_c = param_1[1] - param_2;
  local_4 = param_1[1] + param_2;
  meth_0x44cf80(this,&local_10,param_3,param_4,param_5);
  return this;
}



// Function at 0044cf10

cls_0x44ceb0 * __thiscall
OOAnalyzer::cls_0x44ceb0::cls_0x44ceb0
          (cls_0x44ceb0 *this,int param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  local_10 = *(int *)(param_1 + 0x10) + -0x400;
  local_8 = *(int *)(param_1 + 0x10) + 0x400;
  local_c = *(int *)(param_1 + 0x14) + -0x400;
  local_4 = *(int *)(param_1 + 0x14) + 0x400;
  meth_0x44cf80(this,&local_10,param_2 | 0x40,param_3,param_4);
  return this;
}



// Function at 0044cf80

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
OOAnalyzer::cls_0x44ceb0::meth_0x44cf80
          (cls_0x44ceb0 *this,dword *param_1,uint param_2,dword param_3,int param_4)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  dword in_stack_00000014;
  
  if (param_1 == (dword *)0x0) {
    this->mbr_0x28 = 0;
    this->mbr_0x2c = 0;
    this->mbr_0x30 = 0;
    this->mbr_0x34 = 0;
  }
  else {
    if ((param_2 & 0x43) == 0) {
      param_2 = param_2 | 3;
    }
    this->mbr_0x28 = *param_1;
    this->mbr_0x2c = param_1[1];
    dVar1 = param_1[3];
    this->mbr_0x30 = param_1[2];
    this->mbr_0x34 = dVar1;
  }
  this->mbr_0x0 = param_2;
  this->mbr_0x4 = param_3;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x10 = 0xffffffff;
  this->mbr_0x20 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x1c = 0;
  iVar2 = DAT_00666988;
  if (((param_2 & 0x100) == 0) && ((param_2 & 0x80) != 0)) {
    this->mbr_0x24 = in_stack_00000014;
    if (-1 < (int)in_stack_00000014) {
      this->mbr_0x0 = param_2 | 0x400;
      meth_0x44d080(this);
      return;
    }
  }
  else {
    if (param_4 == 0) {
      param_4 = DAT_00667fcc;
    }
    if (((param_2 & 0x100) == 0) || (param_4 == 0)) {
      this->mbr_0x24 = _DAT_00666970;
      iVar3 = DAT_0066698c;
    }
    else {
      this->mbr_0x24 = (uint)*(ushort *)(param_4 + 0xe);
      iVar2 = *(int *)(param_4 + 0x10);
      iVar3 = *(int *)(param_4 + 0x14);
    }
    this->mbr_0x40 = (iVar2 >> 10) + 1;
    this->mbr_0x38 = (iVar2 >> 10) - 1;
    this->mbr_0x3c = (iVar3 >> 10) - 1;
    this->mbr_0x44 = (iVar3 >> 10) + 1;
  }
  meth_0x44d080(this);
  return;
}



// Function at 0044d080

dword __thiscall OOAnalyzer::cls_0x44ceb0::meth_0x44d080(cls_0x44ceb0 *this)

{
  TPlayScreen *this_00;
  cls_0x498eb0 *this_01;
  uint uVar1;
  dword dVar2;
  cls_0x5b4f30 *this_02;
  bool bVar3;
  int iVar4;
  int *piVar5;
  cls_0x45f660 *pcVar6;
  dword dVar7;
  dword dVar8;
  cls_0x45f660 local_38;
  int local_30;
  int iStack_2c;
  int iStack_28;
  int iStack_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((*(byte *)&this->mbr_0x0 & 0x20) == 0) {
    if (((TPlayScreen *)this->mbr_0xc != (TPlayScreen *)0x0) &&
       (iVar4 = TPlayScreen::meth_0x470040((TPlayScreen *)this->mbr_0xc), 0 < iVar4)) {
      this->mbr_0x20 = 0;
      this->mbr_0x1c = this->mbr_0xc;
    }
    this_00 = (TPlayScreen *)this->mbr_0x1c;
    this->mbr_0xc = 0;
    while (this_00 != (TPlayScreen *)0x0) {
      if (this->mbr_0xc != 0) goto LAB_0044d3c6;
      dVar7 = this->mbr_0x20;
      iVar4 = TPlayScreen::meth_0x470040(this_00);
      if ((int)dVar7 < iVar4) {
        this->mbr_0x20 = dVar7 + 1;
        dVar7 = TPlayScreen::meth_0x470110((TPlayScreen *)this->mbr_0x1c);
        this->mbr_0xc = dVar7;
      }
      else {
        dVar7 = *(dword *)(this->mbr_0x1c + 100);
        this->mbr_0x20 = (int)*(short *)(this->mbr_0x1c + 0x7e) + 1;
        this->mbr_0x1c = dVar7;
      }
      this_00 = (TPlayScreen *)this->mbr_0x1c;
    }
  }
  else {
    this->mbr_0xc = 0;
  }
  dVar7 = this->mbr_0xc;
  dVar8 = local_38.mbr_0x0;
  do {
    if (dVar7 != 0) {
LAB_0044d3c6:
      return this->mbr_0xc;
    }
    dVar7 = this->mbr_0x8 + 1;
    this->mbr_0x8 = dVar7;
    if ((int)this->mbr_0x18 <= (int)dVar7) {
      do {
        iVar4 = DAT_00668578;
        dVar7 = this->mbr_0x10 + 1;
        this->mbr_0x10 = dVar7;
        if (iVar4 <= (int)dVar7) {
          return 0;
        }
        this_01 = *(cls_0x498eb0 **)(DAT_00668588 + dVar7 * 4);
        if ((this_01 != (cls_0x498eb0 *)0x0) &&
           ((uVar1 = this->mbr_0x0, (uVar1 & 0x200) != 0 || (this_01[9].mbr_0x8 != 0)))) {
          if (((uVar1 & 0x100) == 0) && ((uVar1 & 0x80) != 0)) {
            if (((uVar1 & 0x400) == 0) || (this->mbr_0x24 == *(dword *)&this_01->field_0x4)) {
LAB_0044d1b1:
              if ((uVar1 & 1) == 0) {
                if (((uVar1 & 0x40) == 0) ||
                   (((cls_0x498eb0::meth_0x498f60(this_01), (int)this->mbr_0x28 <= local_8 &&
                     (local_10 <= (int)this->mbr_0x30)) &&
                    (((int)this->mbr_0x2c <= local_4 && (local_c <= (int)this->mbr_0x34))))))
                goto LAB_0044d231;
              }
              else {
                cls_0x498eb0::meth_0x498eb0(this_01);
                if (((((int)this->mbr_0x28 <= local_18) && (local_20 <= (int)this->mbr_0x30)) &&
                    ((int)this->mbr_0x2c <= local_14)) && (local_1c <= (int)this->mbr_0x34)) {
LAB_0044d231:
                  this->mbr_0x14 = (dword)this_01;
                  if (this->mbr_0x4 == 0) {
                    dVar7 = this_01[0xb].mbr_0x8;
                  }
                  else {
                    dVar7 = *(dword *)((int)this_01 + this->mbr_0x4 * 0x18 + 0xb4);
                  }
                  this->mbr_0x18 = dVar7;
                  this->mbr_0x8 = 0;
                }
              }
            }
          }
          else if (((((int)this->mbr_0x38 <= (int)this_01->mbr_0x8) &&
                    ((int)this_01->mbr_0x8 <= (int)this->mbr_0x40)) &&
                   ((int)this->mbr_0x3c <= (int)this_01->mbr_0xc)) &&
                  (((int)this_01->mbr_0xc <= (int)this->mbr_0x44 &&
                   (this->mbr_0x24 == *(dword *)&this_01->field_0x4)))) goto LAB_0044d1b1;
        }
      } while ((int)this->mbr_0x18 <= (int)this->mbr_0x8);
    }
    dVar7 = this->mbr_0x4;
    dVar2 = this->mbr_0x14;
    if (dVar7 == 0) {
      piVar5 = *(int **)(*(int *)(dVar2 + 200) + this->mbr_0x8 * 4);
    }
    else {
      piVar5 = *(int **)(*(int *)(dVar2 + 0xc4 + dVar7 * 0x18) + this->mbr_0x8 * 4);
      if (piVar5 == (int *)0x0) {
        piVar5 = *(int **)(dVar2 + dVar7 * 0x18 + 200);
      }
      piVar5 = *(int **)(*(int *)(dVar2 + 200) + *piVar5 * 4);
    }
    this->mbr_0xc = (dword)piVar5;
    if ((piVar5 == (int *)0x0) || (uVar1 = this->mbr_0x0, (uVar1 & 0x5e) == 0)) goto LAB_0044d3bd;
    if ((((uVar1 & 0x10) == 0) || ((*(byte *)(piVar5 + 2) & 8) == 0)) &&
       (((uVar1 & 8) == 0 || ((*(byte *)(piVar5 + 2) & 0x80) == 0)))) {
      bVar3 = false;
      if ((uVar1 & 2) == 0) {
        if ((uVar1 & 0x40) != 0) {
          bVar3 = true;
          pcVar6 = cls_0x45f660::cls_0x45f660(&local_38,piVar5[4]);
          dVar8 = cls_0x41c720::meth_0x41c720((cls_0x41c720 *)&this->mbr_0x28,pcVar6->mbr_0x0);
        }
      }
      else {
        bVar3 = true;
        (**(code **)(*piVar5 + 0xf4))(&local_30);
        if ((((iStack_28 < (int)this->mbr_0x28) || ((int)this->mbr_0x30 < local_30)) ||
            (iStack_24 < (int)this->mbr_0x2c)) || ((int)this->mbr_0x34 < iStack_2c)) {
          dVar8 = 0;
        }
        else {
          dVar8 = 1;
        }
      }
      if ((((*(byte *)&this->mbr_0x0 & 4) == 0) ||
          (this_02 = (cls_0x5b4f30 *)this->mbr_0xc,
          (*(byte *)&(this_02->TPlayScreen).TScreen.mbr_0x8 & 4) == 0)) ||
         (((this_02->TPlayScreen).TScreen.field_0x89 == '\0' || ((bVar3 && (dVar8 != 0)))))) {
        if ((bVar3) && (dVar8 == 0)) goto LAB_0044d3ba;
      }
      else {
        cls_0x5b4f30::meth_0x471430(this_02);
        if ((((iStack_28 < (int)this->mbr_0x28) || ((int)this->mbr_0x30 < local_30)) ||
            (iStack_24 < (int)this->mbr_0x2c)) || ((int)this->mbr_0x34 < iStack_2c)) {
          dVar8 = 0;
          goto LAB_0044d3ba;
        }
        dVar8 = 1;
      }
    }
    else {
LAB_0044d3ba:
      this->mbr_0xc = 0;
    }
LAB_0044d3bd:
    dVar7 = this->mbr_0xc;
  } while( true );
}



// Function at 0045f800

cls_0x44ceb0 * __thiscall
OOAnalyzer::cls_0x44ceb0::cls_0x44ceb0
          (cls_0x44ceb0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4)

{
  meth_0x44cf80(this,param_1,param_2,param_3,param_4);
  return this;
}



