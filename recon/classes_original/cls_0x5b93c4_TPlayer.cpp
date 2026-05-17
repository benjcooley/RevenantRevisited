// Decompiled methods and structure for class: cls_0x5b93c4

/*
/OOAnalyzer/cls_0x5b93c4
pack(disabled)
Structure cls_0x5b93c4 {
   0   cls_0x5b93c4::vftable_5b93c4 *   4   vftptr_0x0   "pointer to cls_0x5b93c4::vftable_5b93c4"
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
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   132   dword   4   mbr_0x84   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   336   dword   4   mbr_0x150   "Unsigned Double-Word (ddw, 4-bytes)"
   340   dword   4   mbr_0x154   "Unsigned Double-Word (ddw, 4-bytes)"
   344   dword   4   mbr_0x158   "Unsigned Double-Word (ddw, 4-bytes)"
   375   byte   1   mbr_0x177   "Unsigned Byte (db)"
   376   dword   4   mbr_0x178   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   384   dword   4   mbr_0x180   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
   420   dword   4   mbr_0x1a4   "Unsigned Double-Word (ddw, 4-bytes)"
   424   dword   4   mbr_0x1a8   "Unsigned Double-Word (ddw, 4-bytes)"
   428   dword   4   mbr_0x1ac   "Unsigned Double-Word (ddw, 4-bytes)"
   432   dword   4   mbr_0x1b0   "Unsigned Double-Word (ddw, 4-bytes)"
   436   dword   4   mbr_0x1b4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 440 Alignment: 1

*/

// Function at 00435010

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x435010(cls_0x5b93c4 *this)

{
  if (((*(byte *)&this->mbr_0x60 & 0x10) != 0) && (this->mbr_0xb8 == 0)) {
    this->mbr_0xb8 = 1;
    this->mbr_0xc0 = 0;
    return;
  }
  cls_0x5a5320_TPlayScreen::virt_meth_0x491bd0((cls_0x5a5320_TPlayScreen *)this);
  return;
}



// Function at 00435150

void __thiscall
OOAnalyzer::cls_0x5b93c4::virt_meth_0x435150
          (cls_0x5b93c4 *this,undefined4 param_1,undefined4 param_2,uint param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
          undefined4 param_9,int param_10)

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  dword dVar4;
  int in_stack_0000002c;
  
  cls_0x5a4494::virt_meth_0x434e40((cls_0x5a4494 *)this);
  this->mbr_0x60 = param_3;
  if (param_10 == 0) goto LAB_004351de;
  iVar3 = FUN_0059a530_stricmp(param_10,s_widgets_005cd8e8);
  if (iVar3 != 0) {
    cls_0x5b98b8::meth_0x435990((cls_0x5b98b8 *)this);
    goto LAB_004351de;
  }
  if ((param_3 & 1) == 0) {
    dVar1 = this->mbr_0x74;
    if (DAT_0065bb10 == dVar1) {
      this->mbr_0x78 = 0;
      goto LAB_004351de;
    }
    dVar4 = DAT_0065bb10;
    if (dVar1 != 0) {
      dVar2 = this->mbr_0x78;
      goto joined_r0x004351d2;
    }
  }
  else {
    dVar1 = this->mbr_0x74;
    if (DAT_0066733c == dVar1) {
      this->mbr_0x78 = 0;
      goto LAB_004351de;
    }
    dVar4 = DAT_0066733c;
    if (dVar1 != 0) {
      dVar2 = this->mbr_0x78;
joined_r0x004351d2:
      if (dVar2 != 0) {
        FUN_00482f80(dVar1);
      }
    }
  }
  this->mbr_0x74 = 0;
  this->mbr_0x78 = 0;
  if (dVar4 != 0) {
    this->mbr_0x74 = dVar4;
    this->mbr_0x78 = 0;
  }
LAB_004351de:
  if (in_stack_0000002c != 0) {
    cls_0x5b98b8::meth_0x435b20((cls_0x5b98b8 *)this);
  }
  cls_0x5b98b8::meth_0x435040
            ((cls_0x5b98b8 *)this,(undefined)param_1,(undefined)param_2,param_3,param_4,param_5,
             param_6,param_7,param_8);
  return;
}



// Function at 00435230

void __thiscall
OOAnalyzer::cls_0x5b93c4::virt_meth_0x435230
          (cls_0x5b93c4 *this,undefined4 param_1,undefined4 param_2,uint param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
          undefined4 param_9,dword param_10)

{
  dword dVar1;
  dword dVar2;
  dword in_stack_0000002c;
  
  cls_0x5a4494::virt_meth_0x434e40((cls_0x5a4494 *)this);
  this->mbr_0x60 = param_3;
  dVar2 = DAT_0066733c;
  dVar1 = DAT_0065bb10;
  if (param_10 == 0) {
    if ((param_3 & 1) == 0) {
      dVar2 = this->mbr_0x74;
      if (DAT_0065bb10 != dVar2) {
        if ((dVar2 != 0) && (this->mbr_0x78 != 0)) {
          FUN_00482f80(dVar2);
        }
        this->mbr_0x74 = 0;
        this->mbr_0x78 = 0;
        param_10 = dVar1;
        goto joined_r0x004352be;
      }
    }
    else {
      dVar1 = this->mbr_0x74;
      if (DAT_0066733c != dVar1) {
        if ((dVar1 != 0) && (this->mbr_0x78 != 0)) {
          FUN_00482f80(dVar1);
        }
        this->mbr_0x74 = 0;
        this->mbr_0x78 = 0;
        param_10 = dVar2;
joined_r0x004352be:
        if (param_10 == 0) goto LAB_004352c6;
        goto LAB_004352c0;
      }
    }
  }
  else {
    dVar1 = this->mbr_0x74;
    if (param_10 == dVar1) goto LAB_004352c3;
    if ((dVar1 != 0) && (this->mbr_0x78 != 0)) {
      FUN_00482f80(dVar1);
    }
LAB_004352c0:
    this->mbr_0x74 = param_10;
  }
LAB_004352c3:
  this->mbr_0x78 = 0;
LAB_004352c6:
  if (in_stack_0000002c != 0) {
    dVar1 = this->mbr_0x6c;
    if (in_stack_0000002c != dVar1) {
      if ((dVar1 != 0) && (this->mbr_0x70 != 0)) {
        FUN_00482f80(dVar1);
      }
      this->mbr_0x6c = in_stack_0000002c;
    }
    this->mbr_0x70 = 0;
  }
  cls_0x5b98b8::meth_0x435040
            ((cls_0x5b98b8 *)this,(undefined)param_1,(undefined)param_2,param_3,param_4,param_5,
             param_6,param_7,param_8);
  return;
}



// Function at 00435cb0

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x435cb0(cls_0x5b93c4 *this)

{
  int *piVar1;
  int iVar2;
  dword in_stack_00000004;
  undefined local_54 [84];
  
  if (in_stack_00000004 == 0) {
    in_stack_00000004 = this->mbr_0x64;
  }
  piVar1 = (int *)this->mbr_0x84;
  if (piVar1 != (int *)0x0) {
    FUN_00438d80(local_54,0,0,0,0,piVar1[1],piVar1[2],0x80000000);
    (**(code **)(*piVar1 + 0x5c))(local_54,piVar1,0,0);
  }
  iVar2 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar2 * 4);
      if ((((*(byte *)(piVar1 + 5) & 2) == 0) && (piVar1[2] != 0)) &&
         (piVar1 != (int *)this->mbr_0xa8)) {
        (**(code **)(*piVar1 + 0x40))(in_stack_00000004);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)&this->field_0x88);
  }
  piVar1 = (int *)this->mbr_0xa8;
  if (((piVar1 != (int *)0x0) && ((*(byte *)(piVar1 + 5) & 2) == 0)) && (piVar1[2] != 0)) {
    (**(code **)(*piVar1 + 0x40))(in_stack_00000004);
  }
  return;
}



// Function at 00435d70

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x435d70(cls_0x5b93c4 *this)

{
  int *piVar1;
  dword dVar2;
  int iVar3;
  
  if ((*(byte *)&this->mbr_0x60 & 0x10) != 0) {
    if ((this->mbr_0xb8 != 0) && (this->mbr_0xbc == 0)) {
      (*this->vftptr_0x0->virt_meth_0x435010_8)(this);
    }
    dVar2 = this->mbr_0xbc;
    if ((int)dVar2 < (int)this->mbr_0xc0) {
      dVar2 = dVar2 + 1;
    }
    else {
      if ((int)dVar2 <= (int)this->mbr_0xc0) goto LAB_00435daf;
      dVar2 = dVar2 - 1;
    }
    this->mbr_0xbc = dVar2;
  }
LAB_00435daf:
  iVar3 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar3 * 4);
      if ((*(byte *)(piVar1 + 5) & 0x80) != 0) {
        (**(code **)(*piVar1 + 0x48))();
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < *(int *)&this->field_0x88);
  }
  return;
}



// Function at 00436010

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436010(cls_0x5b93c4 *this)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar3 * 4);
      uVar2 = piVar1[5];
      if (((((uVar2 & 2) == 0) && (piVar1[2] != 0)) && ((uVar2 & 0x800) == 0)) &&
         ((piVar1 != (int *)this->mbr_0xa8 && ((uVar2 & 0x200) != 0)))) {
        (**(code **)(*piVar1 + 0x50))();
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < *(int *)&this->field_0x88);
  }
  piVar1 = (int *)this->mbr_0xa8;
  if (((piVar1 != (int *)0x0) && ((*(byte *)(piVar1 + 5) & 2) == 0)) &&
     ((piVar1[2] != 0 && ((piVar1[5] & 0x200U) != 0)))) {
    (**(code **)(*piVar1 + 0x40))(this->mbr_0x64);
  }
  return;
}



// Function at 00436090

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436090(cls_0x5b93c4 *this)

{
  int *piVar1;
  dword dVar2;
  undefined *puVar3;
  int iVar4;
  
  piVar1 = (int *)this->mbr_0x64;
  if ((piVar1 != (int *)0x0) && ((this->mbr_0x60 & 1) != 0)) {
    if (DAT_006680c8 == 0) {
      piVar1 = (int *)piVar1[0x15];
      if ((piVar1 != (int *)0x0) && (iVar4 = (**(code **)(*piVar1 + 0x10))(), iVar4 != 0)) {
        cls_0x411eb0::meth_0x414d70
                  ((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),(char)this->mbr_0x4,(char)this->mbr_0x8,
                   1,piVar1,0);
      }
    }
    else {
      (**(code **)(*piVar1 + 0x1c))(1);
      puVar3 = PTR_DAT_005d79e0;
      dVar2 = this->mbr_0x64;
      FUN_00438d80(&stack0xffffffa8,0,0,0,0,*(undefined4 *)(dVar2 + 4),*(undefined4 *)(dVar2 + 8),
                   0x100);
      (**(code **)(*(int *)puVar3 + 0x5c))(&stack0xffffffa8,dVar2,0,0);
    }
  }
  FUN_004aacb0(this->mbr_0x4,this->mbr_0x8,this->mbr_0xc,this->mbr_0x10,1);
  return;
}



// Function at 004361b0

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x4361b0(cls_0x5b93c4 *this)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar2 * 4);
      (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 0x20);
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)&this->field_0x88);
  }
  return;
}



// Function at 004361f0

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x4361f0(cls_0x5b93c4 *this,int param_1)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int in_stack_00000008;
  
  if ((((DAT_0065c9e0 != 0) && (in_stack_00000008 != 0)) && (param_1 == 0x52)) &&
     (*(char *)&this->mbr_0xd0 != '\0')) {
    cls_0x5b98b8::meth_0x4377c0((cls_0x5b98b8 *)this);
  }
  if ((int *)this->mbr_0xa0 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0xa0 + 0x30))(param_1);
    return;
  }
  if (((*(byte *)&this->mbr_0x60 & 8) == 0) ||
     ((piVar3 = *(int **)&this->field_0x9c, piVar3 == (int *)0x0 &&
      (uVar1 = *(int *)&this->field_0x88 - 1,
      piVar3 = *(int **)(*(int *)&this->field_0x98 + (uVar1 & ((int)uVar1 < 0) - 1) * 4),
      piVar3 == (int *)0x0)))) goto LAB_004362e4;
  if (in_stack_00000008 != 0) {
    if ((param_1 == 0x26) || (param_1 == 0x25)) {
      iVar4 = piVar3[3];
      do {
        iVar4 = iVar4 + -1;
        if (iVar4 < 0) {
          iVar4 = *(int *)&this->field_0x88 + -1;
        }
        iVar2 = cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
      } while ((iVar2 == 0) && (iVar4 != piVar3[3]));
      goto LAB_004362e4;
    }
    if ((param_1 == 0x28) || (param_1 == 0x27)) {
      iVar4 = piVar3[3];
      do {
        iVar4 = iVar4 + 1;
        if (*(int *)&this->field_0x88 <= iVar4) {
          iVar4 = 0;
        }
        iVar2 = cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
      } while ((iVar2 == 0) && (iVar4 != piVar3[3]));
      goto LAB_004362e4;
    }
  }
  if ((param_1 == 0xd) && (piVar3 != (int *)0x0)) {
    (**(code **)(*piVar3 + 0x2c))();
  }
LAB_004362e4:
  iVar4 = 0;
  if (0 < *(int *)&this->field_0x88) {
    while( true ) {
      piVar3 = *(int **)(*(int *)&this->field_0x98 + iVar4 * 4);
      if (((((piVar3[5] & 2U) == 0) && (piVar3[2] != 0)) && ((piVar3[5] & 4U) == 0)) &&
         (iVar2 = (**(code **)(*piVar3 + 0x58))(param_1), iVar2 != 0)) break;
      iVar4 = iVar4 + 1;
      if (*(int *)&this->field_0x88 <= iVar4) {
        return;
      }
    }
    (**(code **)(*piVar3 + 0x2c))();
  }
  return;
}



// Function at 00436340

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436340(cls_0x5b93c4 *this,int param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int in_stack_00000008;
  
  if (((*(byte *)&this->mbr_0x60 & 8) == 0) ||
     ((piVar1 = *(int **)&this->field_0x9c, piVar4 = piVar1, piVar1 == (int *)0x0 &&
      (uVar2 = *(int *)&this->field_0x88 - 1,
      piVar4 = *(int **)(*(int *)&this->field_0x98 + (uVar2 & ((int)uVar2 < 0) - 1) * 4),
      piVar4 == (int *)0x0)))) goto LAB_004363f9;
  if (in_stack_00000008 != 0) {
    if ((param_1 == 0x401) || (param_1 == 0x403)) {
      iVar5 = piVar4[3];
      do {
        iVar5 = iVar5 + -1;
        if (iVar5 < 0) {
          iVar5 = *(int *)&this->field_0x88 + -1;
        }
        iVar3 = cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
      } while ((iVar3 == 0) && (iVar5 != piVar4[3]));
      goto LAB_004363f9;
    }
    if ((param_1 == 0x406) || (param_1 == 0x404)) {
      iVar5 = piVar4[3];
      do {
        iVar5 = iVar5 + 1;
        if (*(int *)&this->field_0x88 <= iVar5) {
          iVar5 = 0;
        }
        iVar3 = cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
      } while ((iVar3 == 0) && (iVar5 != piVar4[3]));
      goto LAB_004363f9;
    }
  }
  if ((param_1 == 0x408) && (piVar1 != (int *)0x0)) {
    (**(code **)(*piVar1 + 0x2c))();
  }
LAB_004363f9:
  iVar5 = 0;
  if (0 < *(int *)&this->field_0x88) {
    while( true ) {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar5 * 4);
      if (((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) && ((piVar1[5] & 4U) == 0)) &&
         (iVar3 = (**(code **)(*piVar1 + 0x58))(param_1), iVar3 != 0)) break;
      iVar5 = iVar5 + 1;
      if (*(int *)&this->field_0x88 <= iVar5) {
        return;
      }
    }
    (**(code **)(*piVar1 + 0x2c))();
  }
  return;
}



// Function at 00436460

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436460(cls_0x5b93c4 *this,undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  
  if ((int *)this->mbr_0xa0 == (int *)0x0) {
    iVar2 = 0;
    if (0 < *(int *)&this->field_0x88) {
      do {
        piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar2 * 4);
        if ((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) && ((piVar1[5] & 4U) == 0)) {
          (**(code **)(*piVar1 + 0x34))(param_1);
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < *(int *)&this->field_0x88);
    }
    return;
  }
  (**(code **)(*(int *)this->mbr_0xa0 + 0x34))(param_1);
  return;
}



// Function at 004364d0

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x4364d0(cls_0x5b93c4 *this,int param_1)

{
  int iVar1;
  int iVar2;
  int in_stack_00000008;
  
  if ((((-1 < param_1) && (-1 < in_stack_00000008)) && (param_1 < (int)this->mbr_0xc)) &&
     ((in_stack_00000008 < (int)this->mbr_0x10 && (iVar2 = 0, 0 < *(int *)&this->field_0x88)))) {
    do {
      iVar1 = (**(code **)(**(int **)(*(int *)&this->field_0x98 + iVar2 * 4) + 0x54))(param_1);
      if (iVar1 != 0) {
        return 1;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)&this->field_0x88);
  }
  return 0;
}



// Function at 00436530

void __thiscall
OOAnalyzer::cls_0x5b93c4::virt_meth_0x436530(cls_0x5b93c4 *this,int param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  if ((int *)this->mbr_0xa8 != (int *)0x0) {
    iVar2 = (**(code **)(*(int *)this->mbr_0xa8 + 0x54))(param_2);
    if (iVar2 == 0) {
      if ((this->mbr_0xac == 0) || (((param_1 != 1 && (param_1 != 3)) && (param_1 != 2)))) {
        if (this->mbr_0xa4 != this->mbr_0xa8) {
          return;
        }
      }
      else {
        (**(code **)(*(int *)this->mbr_0xa8 + 0x5c))((int *)this->mbr_0xa8,0x65);
        this->mbr_0xa8 = 0;
      }
    }
    if ((int *)this->mbr_0xa8 != (int *)0x0) {
      (**(code **)(*(int *)this->mbr_0xa8 + 0x38))(param_1,param_2);
      return;
    }
  }
  if ((int *)this->mbr_0xa4 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0xa4 + 0x38))(param_1,param_2);
    return;
  }
  iVar2 = 0;
  if (0 < *(int *)&this->field_0x88) {
    while( true ) {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar2 * 4);
      if ((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) &&
         (((piVar1[5] & 4U) == 0 && (iVar3 = (**(code **)(*piVar1 + 0x54))(param_2), iVar3 != 0))))
      break;
      iVar2 = iVar2 + 1;
      if (*(int *)&this->field_0x88 <= iVar2) {
        return;
      }
    }
    if (param_1 == 1) {
      cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
    }
    (**(code **)(*piVar1 + 0x38))(param_1,param_2);
  }
  return;
}



// Function at 00436660

void __thiscall
OOAnalyzer::cls_0x5b93c4::virt_meth_0x436660
          (cls_0x5b93c4 *this,undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if ((int *)this->mbr_0xa4 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0xa4 + 0x3c))(param_1,param_2);
    return;
  }
  bVar2 = false;
  if (0 < *(int *)&this->field_0x88) {
    do {
      piVar1 = *(int **)(*(int *)&this->field_0x98 + iVar4 * 4);
      if (((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) && ((piVar1[5] & 4U) == 0)) &&
         (iVar3 = (**(code **)(*piVar1 + 0x54))(param_2), iVar3 != 0)) {
        if ((*(byte *)&this->mbr_0x60 & 2) != 0) {
          cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
          bVar2 = true;
        }
        (**(code **)(*piVar1 + 0x3c))(param_1,param_2);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)&this->field_0x88);
    if (bVar2) {
      return;
    }
  }
  if (((this->mbr_0x60 & 2) != 0) && ((this->mbr_0x60 & 4) == 0)) {
    cls_0x5b98b8::meth_0x4369f0((cls_0x5b98b8 *)this);
  }
  return;
}



// Function at 00436730

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x436730(cls_0x5b93c4 *this,undefined4 param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)&this->field_0x88) {
    do {
      iVar1 = (**(code **)(**(int **)(*(int *)&this->field_0x98 + iVar2 * 4) + 0x54))(param_1);
      if (iVar1 != 0) {
        return *(undefined4 *)(*(int *)&this->field_0x98 + iVar2 * 4);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)&this->field_0x88);
  }
  return 0;
}



// Function at 00436de0

int __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436de0(cls_0x5b93c4 *this)

{
  int iVar1;
  
  if (((cls_0x46d6b0 *)this->mbr_0x6c != (cls_0x46d6b0 *)0x0) &&
     (iVar1 = cls_0x46d6b0::meth_0x46d6b0((cls_0x46d6b0 *)this->mbr_0x6c), iVar1 != 0)) {
    return iVar1;
  }
  if (((cls_0x46d6b0 *)this->mbr_0x74 != (cls_0x46d6b0 *)0x0) &&
     (iVar1 = cls_0x46d6b0::meth_0x46d6b0((cls_0x46d6b0 *)this->mbr_0x74), iVar1 != 0)) {
    return iVar1;
  }
  return 0;
}



// Function at 00436ea0

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x436ea0(cls_0x5b93c4 *this)

{
  return 0;
}



// Function at 00437000

undefined __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x437000(cls_0x5b93c4 *this)

{
  undefined uVar1;
  int iVar2;
  undefined3 extraout_var;
  
  iVar2 = FUN_0047a410();
  if (iVar2 == 0) {
    uVar1 = (*this->vftptr_0x0->virt_meth_0x436b20_152)(this);
    return uVar1;
  }
  iVar2 = FUN_00479700(&DAT_005cda9c,0);
  while( true ) {
    if (iVar2 != 0) {
      FUN_004795a0();
      return 1;
    }
    uVar1 = (*this->vftptr_0x0->virt_meth_0x436ec0_172)(this);
    if (CONCAT31(extraout_var,uVar1) == 0) break;
    iVar2 = FUN_00479700(&DAT_005cda9c,0);
  }
  return 0;
}



// Function at 00437a80

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x437a80(cls_0x5b93c4 *this,int param_1)

{
  int iVar1;
  int in_stack_00000008;
  
  if ((in_stack_00000008 == 3000) && (param_1 != 0)) {
    iVar1 = FUN_0059a600(param_1 + 0x18,s_Panel_005cdc20,5);
    if (iVar1 == 0) {
      _strncpy((char *)&this->mbr_0x158,(char *)(param_1 + 0x1d),0x1f);
      this->mbr_0x177 = 0;
      this->mbr_0x150 = 1;
      return 1;
    }
  }
  return 1;
}



// Function at 00442e10

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x442e10(cls_0x5b93c4 *this)

{
  int iVar1;
  int *piVar2;
  dword dVar3;
  dword dVar4;
  char cVar5;
  char *pcVar6;
  int iVar7;
  undefined *puVar8;
  undefined4 *puVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  undefined4 *puVar13;
  char *pcVar14;
  char *pcVar15;
  undefined4 *puVar16;
  char *in_stack_00000004;
  dword local_18c;
  char local_184 [128];
  undefined local_104 [260];
  
  iVar12 = 0;
  pcVar6 = (char *)FUN_00446830();
  uVar10 = 0xffffffff;
  do {
    pcVar15 = pcVar6;
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    pcVar15 = pcVar6 + 1;
    cVar5 = *pcVar6;
    pcVar6 = pcVar15;
  } while (cVar5 != '\0');
  uVar10 = ~uVar10;
  puVar9 = (undefined4 *)(pcVar15 + -uVar10);
  puVar13 = (undefined4 *)&DAT_00656720;
  for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar13 = *puVar9;
    puVar9 = puVar9 + 1;
    puVar13 = puVar13 + 1;
  }
  for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined *)puVar13 = *(undefined *)puVar9;
    puVar9 = (undefined4 *)((int)puVar9 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + 1);
  }
  FUN_0059be72(&DAT_00656720);
  iVar7 = _strncmp(in_stack_00000004,s_IMAGERY_005cf22c,7);
  cVar5 = DAT_00656720;
  if (iVar7 == 0) {
    FUN_0058b100(&DAT_00656720,s__sthumbnail__s_005cf234,&DAT_00656720,in_stack_00000004 + 8);
  }
  else {
    while (cVar5 != '\0') {
      cVar5 = FUN_0058b266((int)cVar5);
      local_184[iVar12] = cVar5;
      pcVar6 = &DAT_00656721 + iVar12;
      iVar12 = iVar12 + 1;
      cVar5 = *pcVar6;
    }
    local_184[iVar12] = '\0';
    puVar8 = (undefined *)FUN_0058ad30(local_184,s_IMAGERY_005cf244);
    *puVar8 = 0;
    iVar12 = -1;
    pcVar6 = local_184;
    do {
      pcVar15 = pcVar6;
      if (iVar12 == 0) break;
      iVar12 = iVar12 + -1;
      pcVar15 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar15;
    } while (cVar5 != '\0');
    *(undefined4 *)(pcVar15 + -1) = s_THUMBNAILS_005cf24c._0_4_;
    *(undefined4 *)(pcVar15 + 3) = s_THUMBNAILS_005cf24c._4_4_;
    *(undefined2 *)(pcVar15 + 7) = s_THUMBNAILS_005cf24c._8_2_;
    pcVar15[9] = s_THUMBNAILS_005cf24c[10];
    pcVar6 = (char *)FUN_0058ad30(&DAT_00656720,s_IMAGERY_005cf258);
    cVar5 = *pcVar6;
    while (cVar5 != '\\') {
      pcVar15 = pcVar6 + 1;
      pcVar6 = pcVar6 + 1;
      cVar5 = *pcVar15;
    }
    uVar10 = 0xffffffff;
    do {
      pcVar15 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar15 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar15;
    } while (cVar5 != '\0');
    uVar10 = ~uVar10;
    iVar12 = -1;
    pcVar6 = local_184;
    do {
      pcVar14 = pcVar6;
      if (iVar12 == 0) break;
      iVar12 = iVar12 + -1;
      pcVar14 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar14;
    } while (cVar5 != '\0');
    puVar9 = (undefined4 *)(pcVar15 + -uVar10);
    puVar13 = (undefined4 *)(pcVar14 + -1);
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar13 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar13 = puVar13 + 1;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *(undefined *)puVar13 = *(undefined *)puVar9;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
      puVar13 = (undefined4 *)((int)puVar13 + 1);
    }
    uVar10 = 0xffffffff;
    pcVar6 = in_stack_00000004;
    do {
      pcVar15 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar15 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar15;
    } while (cVar5 != '\0');
    uVar10 = ~uVar10;
    iVar12 = -1;
    pcVar6 = local_184;
    do {
      pcVar14 = pcVar6;
      if (iVar12 == 0) break;
      iVar12 = iVar12 + -1;
      pcVar14 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar14;
    } while (cVar5 != '\0');
    puVar9 = (undefined4 *)(pcVar15 + -uVar10);
    puVar13 = (undefined4 *)(pcVar14 + -1);
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar13 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar13 = puVar13 + 1;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *(undefined *)puVar13 = *(undefined *)puVar9;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
      puVar13 = (undefined4 *)((int)puVar13 + 1);
    }
    iVar12 = FUN_0058ade0(local_184,0x2e);
    uVar10 = 0xffffffff;
    pcVar6 = &DAT_005cf260;
    do {
      pcVar15 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar15 = pcVar6 + 1;
      cVar5 = *pcVar6;
      pcVar6 = pcVar15;
    } while (cVar5 != '\0');
    uVar10 = ~uVar10;
    puVar9 = (undefined4 *)(pcVar15 + -uVar10);
    puVar13 = (undefined4 *)(iVar12 + 1);
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar13 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar13 = puVar13 + 1;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *(undefined *)puVar13 = *(undefined *)puVar9;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
      puVar13 = (undefined4 *)((int)puVar13 + 1);
    }
  }
  local_18c = 0;
  dVar4 = this->mbr_0x194;
  while (dVar3 = dVar4, dVar3 != 0) {
    iVar12 = FUN_0059a530_stricmp(in_stack_00000004,dVar3);
    if (iVar12 == 0) {
      return *(undefined4 *)(dVar3 + 0x50);
    }
    local_18c = dVar3;
    dVar4 = *(dword *)(dVar3 + 0x54);
  }
  puVar9 = (undefined4 *)FUN_00482fb0(0x58);
  uVar10 = 0xffffffff;
  do {
    pcVar6 = in_stack_00000004;
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    pcVar6 = in_stack_00000004 + 1;
    cVar5 = *in_stack_00000004;
    in_stack_00000004 = pcVar6;
  } while (cVar5 != '\0');
  uVar10 = ~uVar10;
  puVar13 = (undefined4 *)(pcVar6 + -uVar10);
  puVar16 = puVar9;
  for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar16 = *puVar13;
    puVar13 = puVar13 + 1;
    puVar16 = puVar16 + 1;
  }
  for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined *)puVar16 = *(undefined *)puVar13;
    puVar13 = (undefined4 *)((int)puVar13 + 1);
    puVar16 = (undefined4 *)((int)puVar16 + 1);
  }
  FUN_0058b100(local_104,&DAT_005cf264,&DAT_0065bc44,local_184);
  iVar12 = FUN_004a13f0(local_104,&DAT_005cf26c,0);
  if (iVar12 == 0) {
    puVar9[0x14] = 0;
  }
  else {
    iVar7 = FUN_004a1ec0(0x10,0x10,0x201,0);
    iVar1 = *(int *)(iVar7 + 0x40);
    puVar9[0x14] = iVar7;
    if (iVar1 == 0) {
      iVar7 = 0;
    }
    else {
      iVar7 = iVar1 + 0x40 + iVar7;
    }
    FUN_004a15a0(iVar7,0x100,2,iVar12);
    piVar2 = (int *)puVar9[0x14];
    FUN_004a15a0(piVar2 + 0x12,piVar2[1] * *piVar2,1,iVar12);
    FUN_004b8b30(puVar9[0x14]);
    FUN_004a1540(iVar12);
  }
  puVar9[0x15] = 0;
  if (local_18c != 0) {
    *(undefined4 **)(local_18c + 0x54) = puVar9;
    return puVar9[0x14];
  }
  this->mbr_0x194 = (dword)puVar9;
  return puVar9[0x14];
}



// Function at 004430f0

int __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x4430f0(cls_0x5b93c4 *this)

{
  short sVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *in_stack_00000004;
  undefined2 local_34;
  undefined2 local_32;
  undefined2 local_2a;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  
  piVar3 = (int *)cls_0x5a5320_TPlayScreen::meth_0x452520
                            ((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,DAT_00668510 - DAT_006668dc);
  if (((piVar3 != (int *)0x0) &&
      (((sVar1 = *(short *)(piVar3 + 1), sVar1 == 0xb || (sVar1 == 0xc)) || (sVar1 == 5)))) &&
     ((((uVar2 = this->mbr_0x17c, uVar2 == 4 || (uVar2 == 1)) || (uVar2 == 2)) ||
      (((uVar2 == 0x12 || (uVar2 == 7)) ||
       ((((uVar2 == 6 ||
          ((((uVar2 == 0x11 || (uVar2 == 0x16)) || (uVar2 == 0x10)) ||
           ((uVar2 == 0x15 || (uVar2 == 0x17)))))) || (uVar2 == 3)) || (uVar2 == 8)))))))) {
    if ((uVar2 < DAT_0065a258_TObjectClass_numclasses) && (iVar4 = (&DAT_0065a148_TObjectClass_classes)[uVar2], iVar4 != 0)) {
      uVar2 = this->mbr_0x180;
      if ((*(int *)(iVar4 + 0x34) == 0) ||
         ((*(uint *)(iVar4 + 0x24) <= uVar2 || (*(int *)(*(int *)(iVar4 + 0x34) + uVar2 * 4) == 0)))
         ) {
        iVar5 = 0;
      }
      else {
        iVar5 = *(int *)(*(int *)(iVar4 + 0x34) + uVar2 * 4);
        if (iVar5 == 0) {
          iVar5 = *(int *)(iVar4 + 0x38);
        }
      }
      (**(code **)(*piVar3 + 0x50))(*(undefined4 *)(iVar5 + 0x1c),1,0xffffffff);
      return -1;
    }
    return -1;
  }
  puVar6 = (undefined4 *)&local_34;
  for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  local_34 = *(undefined2 *)&this->mbr_0x17c;
  local_32 = *(undefined2 *)&this->mbr_0x180;
  local_2a = DAT_00666970;
  local_28 = *in_stack_00000004;
  local_24 = in_stack_00000004[1];
  local_20 = in_stack_00000004[2];
  iVar4 = cls_0x5a5320_TPlayScreen::meth_0x450e40((cls_0x5a5320_TPlayScreen *)&DAT_006668d8,&local_34);
  if (iVar4 < 0) {
    for (iVar4 = FUN_0058ade0(s_ERROR__Creating_object_005cf270,10); iVar4 != 0;
        iVar4 = FUN_0058ade0(iVar4 + 1,10)) {
      DAT_00656d68 = DAT_00656d68 + 1;
    }
    cls_0x5a4358_TConsolePane::meth_0x43dbc0((cls_0x5a4358_TConsolePane *)&DAT_00656ce8);
    DAT_00656db4 = DAT_00656d48 + DAT_00656d58;
    return -1;
  }
  cls_0x4405d0::meth_0x4405d0((cls_0x4405d0 *)&DAT_00656e78,iVar4);
  FUN_00452690_TMapPane_GetInstance(iVar4,0);
  return iVar4;
}



// Function at 0052ff40

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x52ff40(cls_0x5b93c4 *this,undefined4 param_1)

{
  short sVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  dword dVar6;
  int iVar7;
  undefined4 uVar8;
  cls_0x4746d0 *this_00;
  undefined4 *puVar9;
  dword dVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  int in_stack_00000008;
  cls_0x477870 local_1c;
  
  dVar6 = this->mbr_0x188;
  if (((dVar6 == 0xffffffff) || ((int)*(short *)&this->mbr_0x194 < (int)dVar6)) ||
     ((int *)this->mbr_0x1b4 == (int *)0x0)) {
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return 1;
  }
  if (in_stack_00000008 != 3000) {
    this->mbr_0x188 = 0xffffffff;
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
    return 0;
  }
  if ((this->mbr_0x17c & 1) != 0) {
    dVar10 = this->mbr_0x198;
    iVar3 = (**(code **)(*(int *)this->mbr_0x1b4 + 0x84))(&DAT_005e3dcc);
    if (iVar3 < *(int *)(dVar10 + dVar6 * 0x48 + 0x38)) {
      dVar6 = this->mbr_0x1a0;
      if ((dVar6 == 0) || (dVar10 = this->mbr_0x1a8, dVar10 == 0)) goto LAB_0053051d;
      if (dVar6 == 0) goto LAB_0052fff0;
      piVar2 = *(int **)(dVar6 + 0xd8);
    }
    else {
      iVar3 = (**(code **)(*(int *)this->mbr_0x1b4 + 0x88))();
      if (iVar3 == 0) goto LAB_0053051d;
      iVar7 = *(int *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48);
      iVar3 = this->mbr_0x198 + this->mbr_0x188 * 0x48;
      uVar8 = 1;
      if (((*(byte *)&this->mbr_0x17c & 0x10) != 0) &&
         (uVar8 = *(undefined4 *)(iVar3 + 0x3c), iVar7 == 0)) {
        iVar7 = *(int *)(iVar3 + 4);
      }
      if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) {
        (**(code **)(*(int *)this->mbr_0x1b4 + 0x54))(iVar7,uVar8,0xffffffff);
        (**(code **)(*(int *)this->mbr_0x1b4 + 0x78))
                  (&DAT_005e3dd4,*(undefined4 *)(this->mbr_0x198 + 0x38 + this->mbr_0x188 * 0x48));
      }
      else {
        cls_0x57d9d0::meth_0x5869a0
                  ((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,this->mbr_0x1b4,(char)iVar7);
      }
      dVar6 = this->mbr_0x1a0;
      if ((dVar6 == 0) || (dVar10 = this->mbr_0x1a4, dVar10 == 0)) goto LAB_0053051d;
      if (dVar6 == 0) goto LAB_0052fff0;
      piVar2 = *(int **)(dVar6 + 0xd8);
    }
    if ((piVar2 == (int *)0x0) || (*piVar2 != 0x10)) {
LAB_0052fff0:
      FUN_004d0a20(dVar10,0xffffffff,0);
      (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
      return 1;
    }
    goto LAB_0053051d;
  }
  if ((this->mbr_0x17c & 2) == 0) goto LAB_0053051d;
  this_00 = (cls_0x4746d0 *)(-(uint)(2 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a150);
  uVar4 = cls_0x4746d0::meth_0x475210(this_00,*(undefined4 *)(this->mbr_0x198 + 8 + dVar6 * 0x48));
  if (uVar4 == 0xffffffff) {
    this_00 = (cls_0x4746d0 *)(-(uint)(1 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a14c);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
    this_00 = (cls_0x4746d0 *)(-(uint)(4 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a158);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
    this_00 = (cls_0x4746d0 *)(-(uint)(0x12 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a190);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
    this_00 = (cls_0x4746d0 *)(-(uint)(5 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a15c);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
    this_00 = (cls_0x4746d0 *)(-(uint)(0x11 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a18c);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
    this_00 = (cls_0x4746d0 *)(-(uint)(0x15 < DAT_0065a258_TObjectClass_numclasses) & DAT_0065a19c);
    uVar4 = cls_0x4746d0::meth_0x475210
                      (this_00,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
    if (uVar4 != 0xffffffff) goto LAB_00530291;
  }
  else {
LAB_00530291:
    uVar5 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)&this_00->mbr_0x14);
    if (uVar4 < this_00->mbr_0x24) {
      dVar6 = *(dword *)(this_00->mbr_0x34 + uVar4 * 4);
      if (dVar6 == 0) {
        dVar6 = this_00->mbr_0x38;
      }
      if (uVar5 < (uint)(int)*(short *)(dVar6 + 0xc)) {
        dVar6 = *(dword *)(this_00->mbr_0x34 + uVar4 * 4);
        if (dVar6 == 0) {
          dVar6 = this_00->mbr_0x38;
        }
        if ((*(int *)(*(int *)(dVar6 + 0x10) + uVar5 * 4) == 1) &&
           (iVar3 = cls_0x48e630::meth_0x48e630((cls_0x48e630 *)&DAT_0065a618,this_00->mbr_0x8),
           iVar3 == 0)) {
          cls_0x48e630::meth_0x48e670((cls_0x48e630 *)&DAT_0065a618,this_00->mbr_0x8);
        }
      }
    }
  }
  dVar6 = this->mbr_0x1b4;
  if (dVar6 == 0) {
    FUN_0052f310();
    sVar1 = *(short *)&this->mbr_0x194;
    uVar4 = this->mbr_0x188;
    if (uVar4 < (uint)(int)sVar1) {
      iVar3 = *(short *)((int)&this->mbr_0x194 + 2) + -1;
      if ((int)uVar4 < iVar3) {
        iVar3 = iVar3 - uVar4;
        puVar11 = (undefined4 *)(this->mbr_0x198 + uVar4 * 0x48);
        do {
          iVar3 = iVar3 + -1;
          puVar9 = puVar11 + 0x12;
          puVar12 = puVar11;
          for (iVar7 = 0x12; iVar7 != 0; iVar7 = iVar7 + -1) {
            *puVar12 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar12 = puVar12 + 1;
          }
          puVar11 = puVar11 + 0x12;
        } while (iVar3 != 0);
      }
      sVar1 = sVar1 + -1;
      *(short *)&this->mbr_0x194 = sVar1;
      if (sVar1 < 1) {
        dVar6 = this->mbr_0x198;
        goto joined_r0x005304f4;
      }
    }
  }
  else {
    if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) {
      iVar3 = FUN_005330a0(dVar6,*(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
      if (iVar3 != 0) {
        (**(code **)(*(int *)this->mbr_0x1b4 + 0x54))
                  (&DAT_005e3de8,*(undefined4 *)(this->mbr_0x198 + 0x38 + this->mbr_0x188 * 0x48),
                   0xffffffff);
        cls_0x5a7b98_TCharacter::meth_0x519230((cls_0x5a7b98_TCharacter *)this->mbr_0x1b4);
      }
    }
    else {
      cls_0x57d9d0::meth_0x586a10((cls_0x57d9d0 *)&UNK_00676e04.field_0x4,dVar6);
    }
    local_1c.mbr_0x4 = this->mbr_0x1b4;
    local_1c.mbr_0x0 = 1;
    local_1c.mbr_0x10 = 0;
    local_1c.mbr_0xc = 0;
    local_1c.mbr_0x14 = 0;
    local_1c.mbr_0x18 = 0;
    local_1c.mbr_0x8 = local_1c.mbr_0x4;
    cls_0x477870::meth_0x46dfb0(&local_1c);
    while (local_1c.mbr_0x18 != 0) {
      iVar3 = FUN_0059a530_stricmp(*(undefined4 *)(local_1c.mbr_0x18 + 0x38),
                           *(undefined4 *)(this->mbr_0x198 + 8 + this->mbr_0x188 * 0x48));
      if ((iVar3 == 0) && (iVar3 = FUN_00470070(), iVar3 == 0)) goto LAB_0053051d;
      cls_0x477870::meth_0x46dfb0(&local_1c);
    }
    FUN_0052f310();
    sVar1 = *(short *)&this->mbr_0x194;
    uVar4 = this->mbr_0x188;
    if (uVar4 < (uint)(int)sVar1) {
      iVar3 = *(short *)((int)&this->mbr_0x194 + 2) + -1;
      if ((int)uVar4 < iVar3) {
        iVar3 = iVar3 - uVar4;
        puVar11 = (undefined4 *)(this->mbr_0x198 + uVar4 * 0x48);
        do {
          iVar3 = iVar3 + -1;
          puVar9 = puVar11 + 0x12;
          puVar12 = puVar11;
          for (iVar7 = 0x12; iVar7 != 0; iVar7 = iVar7 + -1) {
            *puVar12 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar12 = puVar12 + 1;
          }
          puVar11 = puVar11 + 0x12;
        } while (iVar3 != 0);
      }
      sVar1 = sVar1 + -1;
      *(short *)&this->mbr_0x194 = sVar1;
      if (sVar1 < 1) {
        dVar6 = this->mbr_0x198;
joined_r0x005304f4:
        if (dVar6 != 0) {
          FUN_004830f0(dVar6);
        }
        this->mbr_0x198 = 0;
        *(undefined2 *)&this->mbr_0x194 = 0;
        *(undefined2 *)((int)&this->mbr_0x194 + 2) = 0;
      }
    }
  }
  this->mbr_0x188 = 0xffffffff;
LAB_0053051d:
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  return 1;
}



// Function at 005339b0

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x5339b0(cls_0x5b93c4 *this)

{
  int iVar1;
  int iVar2;
  cls_0x5a3c68 *pcVar3;
  bool bVar4;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a19c1;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a4494::virt_meth_0x434e40((cls_0x5a4494 *)this);
  FUN_00483300_RandomRange(0,4);
  iVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  if ((-1 < iVar1) &&
     (iVar2 = cls_0x41c7d0::meth_0x49b650((cls_0x41c7d0 *)&DAT_00667548), iVar2 != 0)) {
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,(char)iVar1,0x7f,1);
  }
  bVar4 = DAT_0066f6f4 == 0;
  this->mbr_0x60 = this->mbr_0x60 | 0xe;
  if (bVar4) {
    ExceptionList = local_c;
    return 0;
  }
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 0;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,DAT_0066f6f4,s_Restart_005e3f18,0,0,0,0,0xffffffff,0x10,0xffffffff);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 1;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68(pcVar3,DAT_0066f6f4,&DAT_005e3f20,0,0,0,0,0xffffffff,0x10,0xffffffff)
    ;
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 2;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68(pcVar3,DAT_0066f6f4,&DAT_005e3f28,0,0,0,0,0xffffffff,0x10,0xffffffff)
    ;
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
  iVar1 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
  *(undefined **)(iVar1 + 0x80) = &LAB_00533950;
  iVar1 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  *(undefined **)(iVar1 + 0x80) = &LAB_00533970;
  *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
  iVar1 = cls_0x5b98b8::meth_0x436900((cls_0x5b98b8 *)this);
  *(undefined **)(iVar1 + 0x80) = &LAB_00533990;
  *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x40;
  ExceptionList = local_c;
  return 1;
}



// Function at 00533c00

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x533c00(cls_0x5b93c4 *this)

{
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  return;
}



// Function at 00533c10

void __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x533c10(cls_0x5b93c4 *this)

{
  undefined4 uVar1;
  
  if (this->mbr_0x50 != 0) {
    uVar1 = cls_0x46d6b0::meth_0x46d710(DAT_0066f6f4);
    cls_0x5a68d8::meth_0x4bd680((cls_0x5a68d8 *)PTR_DAT_005d79e0,0,0,uVar1,0x10);
    (*this->vftptr_0x0->virt_meth_0x4361b0_144)(this);
  }
  cls_0x5b98b8::virt_meth_0x435de0((cls_0x5b98b8 *)this);
  return;
}



// Function at 00533cc0

cls_0x5b93c4 * __thiscall OOAnalyzer::cls_0x5b93c4::cls_0x5b93c4(cls_0x5b93c4 *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5b93c4__vftable_5b93c4 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5b93c4__vftable_5b93c4_005b93c4;
  return this;
}



// Function at 00533d20

cls_0x5b93c4 * __thiscall OOAnalyzer::cls_0x5b93c4::~cls_0x5b93c4(cls_0x5b93c4 *this)

{
  cls_0x5b93c4 *pcVar1;
  undefined4 uVar2;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a19e6;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->vftptr_0x0 = &cls_0x5b93c4__vftable_5b93c4_005b93c4;
  uVar2 = 0;
  local_4 = 0;
  (*cls_0x5b93c4__vftable_5b93c4_005b93c4.virt_meth_0x533c00_184)(this);
  this->vftptr_0x0 = (cls_0x5b93c4__vftable_5b93c4 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  puStack_8 = (undefined *)0x1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  puStack_8 = (undefined *)0xffffffff;
  pcVar1 = (cls_0x5b93c4 *)FUN_004830f0(*(undefined4 *)&this->field_0x98,uVar2);
  ExceptionList = this;
  return pcVar1;
}



// Function at 00533da0

cls_0x5b93c4 * __thiscall OOAnalyzer::cls_0x5b93c4::virt_meth_0x533da0(cls_0x5b93c4 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b93c4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00538e40

void __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x538e40(cls_0x5b93c4 *this)

{
  ushort uVar1;
  undefined uVar2;
  ushort uVar3;
  ushort uVar4;
  undefined3 extraout_var;
  int iVar6;
  ushort *puVar7;
  ushort uVar8;
  ushort *puVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  ushort *puStack_24;
  cls_0x5a68d8 *local_1c;
  int iStack_18;
  int local_14;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  ushort *puVar5;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1b51;
  local_c = ExceptionList;
  if ((this->mbr_0x17c != 0) && (this->mbr_0x17c != DAT_00667fcc)) {
    ExceptionList = &local_c;
    local_1c = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
    local_4 = 0;
    if (local_1c == (cls_0x5a68d8 *)0x0) {
      local_1c = (cls_0x5a68d8 *)0x0;
    }
    else {
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
      cls_0x5a68d8::cls_0x5a68d8(local_1c);
      local_4 = CONCAT31(local_4._1_3_,1);
      local_1c->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
      cls_0x5a68d8::meth_0x4a39a0(local_1c,0x28,0x28);
    }
    uVar11 = 1000;
    local_4 = 0xffffffff;
    (*local_1c->vftptr_0x0->virt_meth_0x419e80_68)(local_1c,0,0,1000);
    uVar10 = 0x80000000;
    uVar11 = (**(code **)(*(int *)this->mbr_0x17c + 0x130))(0x80000000,0,uVar11);
    cls_0x5a68d8::meth_0x4bd680(local_1c,0,0,uVar11,uVar10);
    uVar2 = (*local_1c->vftptr_0x0->virt_meth_0x58bd3e_44)();
    puVar5 = (ushort *)CONCAT31(extraout_var,uVar2);
    if (puVar5 != (ushort *)0x0) {
      puStack_24 = (ushort *)(**(code **)(*(int *)this->mbr_0x1a8 + 0x2c))();
      if (puStack_24 == (ushort *)0x0) {
        (*local_1c->vftptr_0x0->virt_meth_0x58bd3e_48)();
        ExceptionList = local_c;
        return;
      }
      iVar6 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
      if (iVar6 == 0xf) {
        local_14 = 0x14;
        do {
          iStack_18 = 0x14;
          do {
            puVar9 = puVar5;
            uVar1 = *puVar9;
            uVar3 = puVar9[0x29] >> 2;
            uVar4 = puVar9[0x28] >> 2;
            uVar8 = puVar9[1] >> 2;
            puVar7 = puStack_24 + 1;
            *puStack_24 = (ushort)((puVar9[1] & 0x1f) + (puVar9[0x28] & 0x1f) +
                                   (puVar9[0x29] & 0x1f) + (uVar1 & 0x1f) >> 2) |
                          (short)((uVar1 & 0x3e0) >> 2) + (uVar8 & 0xf8) + (uVar4 & 0xf8) +
                          (uVar3 & 0xf8) & 0x3e0 |
                          (uVar8 & 0x3f00) + (uVar4 & 0x3f00) + (uVar3 & 0x3f00) +
                          (uVar1 >> 10) * 0x100 & 0x7c00;
            iStack_18 = iStack_18 + -1;
            puVar5 = puVar9 + 2;
            puStack_24 = puVar7;
          } while (iStack_18 != 0);
          puVar5 = puVar9 + 0x2a;
          local_14 = local_14 + -1;
        } while (local_14 != 0);
      }
      else {
        iStack_18 = 0x14;
        do {
          local_14 = 0x14;
          do {
            puVar9 = puVar5;
            uVar1 = *puVar9;
            uVar3 = puVar9[0x29] >> 2;
            uVar4 = puVar9[0x28] >> 2;
            uVar8 = puVar9[1] >> 2;
            puVar7 = puStack_24 + 1;
            *puStack_24 = (ushort)((puVar9[1] & 0x1f) + (puVar9[0x28] & 0x1f) +
                                   (puVar9[0x29] & 0x1f) + (uVar1 & 0x1f) >> 2) |
                          (short)((uVar1 & 0x7c0) >> 2) + (uVar8 & 0x1f0) + (uVar4 & 0x1f0) +
                          (uVar3 & 0x1f0) & 0x7e0 |
                          (uVar8 & 0x3e00) + (uVar4 & 0x3e00) + (uVar3 & 0x3e00) +
                          (uVar1 >> 0xb) * 0x200 & 0xf800;
            local_14 = local_14 + -1;
            puVar5 = puVar9 + 2;
            puStack_24 = puVar7;
          } while (local_14 != 0);
          puVar5 = puVar9 + 0x2a;
          iStack_18 = iStack_18 + -1;
        } while (iStack_18 != 0);
      }
      if (local_1c->mbr_0x34 != 0) {
        (*local_1c->vftptr_0x0->virt_meth_0x58bd3e_48)();
      }
      if (((int *)this->mbr_0x1a8)[0xd] != 0) {
        (**(code **)(*(int *)this->mbr_0x1a8 + 0x30))();
      }
      if (local_1c != (cls_0x5a68d8 *)0x0) {
        (*local_1c->vftptr_0x0->~cls_0x5a68d8_0)(local_1c);
      }
    }
  }
  ExceptionList = local_c;
  return;
}



// Function at 005391a0

void __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x5391a0(cls_0x5b93c4 *this)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  dword in_stack_00000004;
  
  if (this->mbr_0x17c != in_stack_00000004) {
    this->mbr_0x17c = in_stack_00000004;
    meth_0x538e40(this);
    (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
    if (*(uint *)&this->field_0x88 == 0) {
      piVar4 = (int *)0x0;
    }
    else {
      piVar4 = **(int ***)&this->field_0x98;
    }
    if (((1 < *(uint *)&this->field_0x88) &&
        (piVar1 = *(int **)(*(int *)&this->field_0x98 + 4), piVar1 != (int *)0x0)) &&
       (piVar4 != (int *)0x0)) {
      uVar2 = piVar4[5];
      iVar3 = *piVar4;
      this->mbr_0x188 = 0;
      (**(code **)(iVar3 + 0x1c))(uVar2 | 4);
      (**(code **)(*piVar1 + 0x1c))(piVar1[5] & 0xfffffffb);
    }
    if (DAT_0066829c != 0) {
      cls_0x57d9d0::meth_0x584680((cls_0x57d9d0 *)&UNK_00676e04.field_0x4);
    }
  }
  return;
}



// Function at 00539260

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x539260(cls_0x5b93c4 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < *(uint *)&this->field_0x88) {
    return *(undefined4 *)(*(int *)&this->field_0x98 + in_stack_00000004 * 4);
  }
  return 0;
}



// Function at 00543ca0

int __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x543ca0(cls_0x5b93c4 *this,int param_1)

{
  undefined **ppuVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined **ppuVar8;
  int unaff_retaddr;
  int in_stack_00000008;
  int iVar9;
  int iStack_8;
  
  if ((((0x20 < param_1) && (param_1 < 0xa3)) && (9 < in_stack_00000008)) &&
     ((in_stack_00000008 < 0x88 && (iVar6 = 0, DAT_00667fcc != (int *)0x0)))) {
    piVar2 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(s_spell_pouch_005e51d0);
    if (piVar2 == (int *)0x0) {
      (**(code **)(*DAT_00667fcc + 0xa8))(s_spellpouch_005e51dc);
    }
    else {
      iVar7 = 0x21;
      iVar5 = 10;
      if (PTR_DAT_005e4f80 != (undefined *)0x0) {
        ppuVar8 = &PTR_DAT_005e4f80;
        iVar9 = 0;
        do {
          iVar3 = (**(code **)(*piVar2 + 0xa8))(*ppuVar8);
          iVar4 = iVar9;
          if (((iVar3 != 0) &&
              ((iVar4 = iVar9 + 1, *(int *)(iStack_8 + 0x180) <= iVar9 ||
               (*(int *)(iStack_8 + 0x17c) == 0)))) && (iVar5 < 0x7e)) {
            if ((((iVar7 <= unaff_retaddr) && (unaff_retaddr < iVar7 + 0x1e)) && (iVar5 <= param_1))
               && (param_1 < iVar5 + 0x1e)) {
              return iVar6;
            }
            iVar9 = iVar7 + 0x1e;
            if (*(int *)(iStack_8 + 0x17c) != 0) {
              iVar9 = iVar7 + 0x3c;
            }
            iVar7 = iVar9;
            if (0x82 < iVar7) {
              iVar7 = 0x21;
              iVar5 = iVar5 + 0x1e;
            }
          }
          ppuVar1 = ppuVar8 + 1;
          ppuVar8 = ppuVar8 + 1;
          iVar6 = iVar6 + 1;
          iVar9 = iVar4;
          if (*ppuVar1 == (undefined *)0x0) {
            return -1;
          }
        } while( true );
      }
    }
  }
  return -1;
}



// Function at 005475e0

/* WARNING: Type propagation algorithm not settling */

undefined4 __thiscall OOAnalyzer::cls_0x5b93c4::meth_0x5475e0(cls_0x5b93c4 *this)

{
  dword *pdVar1;
  char cVar2;
  dword dVar3;
  LPSTR lpchText;
  undefined uVar4;
  undefined3 extraout_var;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  LPSTR pCVar9;
  uint uVar10;
  int unaff_EBX;
  undefined4 *puVar11;
  int unaff_EBP;
  char *pcVar12;
  LPSTR pCVar13;
  LPCSTR pCVar14;
  bool bVar15;
  HDC hdc;
  UINT align;
  int iStack_228;
  int local_224;
  int local_220;
  undefined4 local_21c;
  int local_218;
  int local_214;
  cls_0x5a36f8 *local_210;
  int local_20c;
  undefined4 local_208;
  undefined4 local_204;
  undefined4 uStack_1fc;
  undefined4 local_1f8;
  char *local_1f4;
  undefined local_1f0;
  undefined4 local_1ec;
  cls_0x478720 cStack_1e4;
  undefined4 uStack_1dc;
  undefined4 local_1d8;
  int iStack_1d4;
  int local_1d0;
  undefined4 *puStack_1cc;
  undefined uStack_1c8;
  undefined uStack_1c7;
  undefined uStack_1c6;
  undefined uStack_1c5;
  int iStack_1c4;
  undefined4 local_1c0;
  undefined uStack_1bc;
  undefined uStack_1bb;
  undefined uStack_1ba;
  undefined uStack_1b9;
  undefined uStack_1b8;
  undefined uStack_1b7;
  undefined uStack_1b6;
  undefined uStack_1b5;
  int iStack_1b4;
  int iStack_1b0;
  int iStack_1ac;
  int iStack_1a8;
  int local_1a4;
  int local_1a0;
  cls_0x5a3c68 *pcStack_19c;
  int iStack_198;
  int iStack_194;
  int iStack_190;
  int iStack_18c;
  undefined4 uStack_188;
  int iStack_184;
  int iStack_180;
  undefined4 uStack_17c;
  undefined4 uStack_178;
  undefined4 uStack_174;
  int iStack_170;
  tagDRAWTEXTPARAMS tStack_16c;
  tagDRAWTEXTPARAMS tStack_158;
  tagRECT tStack_144;
  tagRECT tStack_134;
  cls_0x5a36f8__vftable_5a36f8 *pcStack_124;
  char *local_120;
  dword local_11c;
  dword local_118;
  dword local_114;
  tagSIZE local_110;
  undefined4 auStack_108 [8];
  undefined auStack_e8 [20];
  undefined auStack_d4 [32];
  char acStack_b4 [9];
  undefined auStack_ab [63];
  undefined auStack_6c [92];
  void *pvStack_10;
  void *local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a2085;
  local_c = ExceptionList;
  local_220 = 0x14;
  local_224 = 0x14;
  local_1a4 = 0xc;
  local_1c0 = 0xffffffff;
  local_21c = 0x14;
  local_1d8 = CONCAT13(local_1d8._3_1_,0xffffff);
  local_1d0 = 0;
  local_1a0 = 1;
  ExceptionList = &local_c;
  if (this->mbr_0x198 == 0) {
    if (DAT_00667fcc == 0) {
      ExceptionList = pvStack_10;
      return 0;
    }
    ExceptionList = &local_c;
    (*this->vftptr_0x0->virt_meth_0x533c00_184)(this);
  }
  local_11c = this->mbr_0x1a0;
  if (local_11c == 0) {
    local_220 = 0xffffff;
    cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
    ExceptionList = pvStack_10;
    return 0;
  }
  pcStack_124 = &cls_0x5a36f8__vftable_5a36f8_005a36f8;
  local_118 = this->mbr_0x1a4 + local_11c;
  local_120 = s_String_005e54b8;
  local_114 = local_11c;
  cls_0x478720::cls_0x478720(&cStack_1e4);
  local_210 = (cls_0x5a36f8 *)&pcStack_124;
  uStack_8 = (undefined *)0x0;
  local_21c = 0;
  local_218 = 0;
  local_214 = 0;
  local_20c = 0;
  local_208 = 0;
  local_204 = 0;
  uStack_1fc = 0;
  local_1f8 = 0;
  local_1f0 = 0;
  local_1ec = 1;
  local_1f4 = (char *)FUN_00482fb0(0x2000);
  *local_1f4 = '\0';
  uStack_8 = (undefined *)0x1;
  FUN_00479450();
  while (local_20c != 10) {
    uVar4 = (**(code **)(*(int *)local_210 + 0xc))(local_210);
    iVar5 = FUN_00479700(s_CLASS_005e54c0,0);
    if (iVar5 == 0) {
      local_220 = 0xffffff;
      cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
      uStack_8 = (undefined *)0x2;
LAB_005478c9:
      FUN_004830f0(local_1f4);
      goto joined_r0x005478dc;
    }
    FUN_00479580();
    if (local_20c != 2) {
      local_220 = 0xffffff;
      cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
      uStack_8 = (undefined *)0x3;
      goto LAB_005478c9;
    }
    iVar5 = FUN_00479700(s_default_005e54c8,0);
    if ((iVar5 == 0) &&
       (iVar5 = FUN_00483460(local_1f4,*(undefined4 *)(*(int *)(this->mbr_0x198 + 0x48) + 4)),
       iVar5 == 0)) {
      FUN_004795c0();
    }
    else {
      FUN_00479580();
      bVar15 = true;
      iVar5 = FUN_00479700(s_FORMAT_005e54d0,0);
      if (iVar5 != 0) {
        FUN_00479580();
        iVar5 = FUN_00479700(this->mbr_0x19c,0);
        bVar15 = iVar5 != 0;
        FUN_00479580();
      }
      FUN_004795a0();
      if (bVar15) {
        if (CONCAT31(extraout_var,uVar4) == this->mbr_0x1a8) {
          local_1a4 = 0;
        }
        this->mbr_0x1a8 = CONCAT31(extraout_var,uVar4);
        if (local_1a4 == 0) goto LAB_0054791e;
        break;
      }
    }
    FUN_004795f0();
    FUN_004795a0();
  }
  cls_0x5b98b8::meth_0x436890((cls_0x5b98b8 *)this);
LAB_0054791e:
  iVar5 = FUN_00479700(s_BEGIN_005e54d8,0);
  if (iVar5 == 0) {
    local_220 = 0xffffff;
    cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
    uStack_8 = (undefined *)0x4;
    FUN_004830f0(local_1f4);
joined_r0x005478dc:
    if (local_218 == 0) {
      if (local_214 != 0) {
        FUN_004830f0(local_210);
LAB_00548dc3:
        FUN_004a1540(local_214);
      }
    }
    else {
      FUN_004830f0(local_210);
      FUN_004830f0(local_218);
    }
LAB_00548dcb:
    uStack_8 = (undefined *)0xffffffff;
    cls_0x478720::meth_0x478730(&cStack_1e4);
    ExceptionList = pvStack_10;
    return 0;
  }
  FUN_004795a0();
  iVar5 = FUN_00479700(&DAT_005e54e0,0);
  do {
    if (iVar5 != 0) {
      FUN_004795a0();
      uStack_8 = (undefined *)0x11;
      FUN_004830f0(local_1f4);
      if (local_218 == 0) {
        if (local_214 != 0) {
          FUN_004830f0(local_210);
          FUN_004a1540(local_214);
        }
      }
      else {
        FUN_004830f0(local_210);
        FUN_004830f0(local_218);
      }
      uStack_8 = (undefined *)0xffffffff;
      cls_0x478720::meth_0x478730(&cStack_1e4);
      ExceptionList = pvStack_10;
      return 1;
    }
    iVar5 = FUN_00479700(s_BUTTON_005e54e4,0);
    if (iVar5 == 0) {
      iVar5 = FUN_00479700(&DAT_005e5500,0);
      if (iVar5 == 0) {
        iVar5 = FUN_00479700(s_LINEHEIGHT_005e5508,0);
        if (iVar5 == 0) {
          iVar5 = FUN_00479700(s_LINEWRAP_005e5518,0);
          if (iVar5 == 0) {
            iVar5 = FUN_00479700(s_COLOR_005e5530,0);
            if (iVar5 == 0) {
              iVar5 = FUN_00479700(&DAT_005e5544,0);
              if (iVar5 == 0) {
                iVar5 = FUN_00479700(&DAT_005e5550,0);
                if (iVar5 == 0) {
                  iVar5 = FUN_00479700(&PTR_LAB_005e5560,0);
                  if (iVar5 == 0) {
                    iVar5 = FUN_00479700(s_NEXTLINE_005e5568,0);
                    if (iVar5 == 0) {
                      iVar5 = FUN_00479700(&DAT_005e5574,0);
                      if ((iVar5 == 0) && (iVar5 = FUN_00479700(s_FIELD_005e557c,0), iVar5 == 0)) {
                        cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                        uStack_8 = (undefined *)0x10;
                        FUN_004830f0(local_1f4);
                        if (local_218 != 0) {
                          FUN_004830f0(local_210);
                          FUN_004830f0(local_218);
                          goto LAB_00548dcb;
                        }
                        if (local_214 == 0) goto LAB_00548dcb;
                        FUN_004830f0(local_210);
                        goto LAB_00548dc3;
                      }
                      iStack_194 = FUN_00479700(s_FIELD_005e5584,0);
                      iStack_1b0 = 1;
                      local_1a0 = 0;
                      iStack_18c = 0;
                      iStack_1b4 = 0;
                      pcStack_19c = (cls_0x5a3c68 *)0x0;
                      iStack_190 = 0;
                      FUN_00479580();
                      iVar5 = FUN_00479700(s_block_005e558c,0);
                      if (iVar5 == 0) {
                        iVar5 = FUN_00479700(s_multipage_005e559c,0);
                        if (iVar5 == 0) {
                          iVar5 = FUN_00479700(s_grayable_005e55b0,0);
                          if (iVar5 == 0) {
                            iVar5 = FUN_00479700(s_prevbutton_005e55bc,0);
                            if (iVar5 == 0) {
                              iVar5 = FUN_00479700(s_nextbutton_005e55c8,0);
                              if (iVar5 == 0) {
                                iVar5 = FUN_00479700(s_center_005e55d4,0);
                                if (iVar5 == 0) {
                                  iVar5 = FUN_00479700(s_right_005e55dc,0);
                                  if (iVar5 == 0) {
                                    iVar5 = FUN_00479700(&DAT_005e55e4,0);
                                    if (iVar5 == 0) {
                                      SetTextAlign((HDC)this->mbr_0x17c,1);
                                    }
                                    else {
                                      SetTextAlign((HDC)this->mbr_0x17c,1);
                                      FUN_00479580();
                                    }
                                    goto LAB_005481fa;
                                  }
                                  hdc = (HDC)this->mbr_0x17c;
                                  align = 3;
                                }
                                else {
                                  hdc = (HDC)this->mbr_0x17c;
                                  align = 7;
                                }
                                SetTextAlign(hdc,align);
                                FUN_00479580();
                                iStack_1b0 = 0;
                                goto LAB_005481fa;
                              }
                              iStack_190 = 1;
                            }
                            else {
                              pcStack_19c = (cls_0x5a3c68 *)0x1;
                            }
                          }
                          iStack_1b4 = 1;
                          FUN_00479580();
                        }
                        else {
                          FUN_00479580();
                          iVar5 = FUN_0047a410(&local_21c,s__d__d_005e55a8,&local_1d0,&local_1d8);
                          if (iVar5 == 0) {
                            cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                            uStack_8 = (undefined *)0xe;
                            FUN_004830f0(local_1f4);
                            if (local_218 == 0) {
                              if (local_214 != 0) {
                                FUN_004830f0(local_210);
                                FUN_004a1540(local_214);
                              }
                            }
                            else {
                              FUN_004830f0(local_210);
                              FUN_004830f0(local_218);
                            }
                            uStack_8 = (undefined *)0xffffffff;
                            cls_0x478720::meth_0x478730(&cStack_1e4);
                            ExceptionList = pvStack_10;
                            return 0;
                          }
                          iStack_18c = 1;
                          local_1a0 = 1;
                        }
                      }
                      else {
                        FUN_00479580();
                        iVar5 = FUN_0047a410(&local_21c,s__d__d_005e5594,&local_1d0,&local_1d8);
                        if (iVar5 == 0) {
                          cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                          uStack_8 = (undefined *)0xd;
                          FUN_004830f0(local_1f4);
                          if (local_218 == 0) {
                            if (local_214 != 0) {
                              FUN_004830f0(local_210);
                              FUN_004a1540(local_214);
                            }
                          }
                          else {
                            FUN_004830f0(local_210);
                            FUN_004830f0(local_218);
                          }
                          uStack_8 = (undefined *)0xffffffff;
                          cls_0x478720::meth_0x478730(&cStack_1e4);
                          ExceptionList = pvStack_10;
                          return 0;
                        }
                        local_1a0 = 1;
                      }
LAB_005481fa:
                      iVar5 = iStack_1d4;
                      puStack_1cc = &uStack_1dc;
                      if ((local_20c == 4) && (iVar7 = 0, 0 < iStack_1d4)) {
                        pcVar12 = acStack_b4;
                        do {
                          iVar8 = FUN_0059a530_stricmp(pcVar12,local_1f4);
                          if (iVar8 == 0) {
                            puStack_1cc = auStack_108 + iVar7;
                            FUN_00479580();
                            break;
                          }
                          iVar7 = iVar7 + 1;
                          pcVar12 = pcVar12 + 10;
                        } while (iVar7 < iVar5);
                      }
                      iStack_198 = 1;
                      if (iStack_194 == 0) {
                        dVar3 = this->mbr_0x194;
                        pcVar12 = (char *)this->mbr_0x190;
                        _strncpy(pcVar12,local_1f4,dVar3 - 1);
                        pcVar12[dVar3 - 1] = '\0';
                      }
                      else {
                        pcVar12 = (char *)this->mbr_0x190;
                        iVar7 = 0;
                        *pcVar12 = '\0';
                        if (local_20c == 2) {
                          dVar3 = this->mbr_0x194;
                          _strncpy(pcVar12,local_1f4,dVar3 - 1);
                          pcVar12[dVar3 - 1] = '\0';
                          FUN_00479580();
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)this->mbr_0x190;
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar2 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar2 != '\0');
                          iVar7 = ~uVar10 - 1;
                        }
                        iVar8 = FUN_0047a410(&local_21c,&DAT_005e55ec,auStack_d4);
                        if (iVar8 == 0) {
                          cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                          uStack_8 = (undefined *)0xf;
                          FUN_004830f0(local_1f4);
                          if (local_218 == 0) {
                            if (local_214 != 0) {
                              FUN_004830f0(local_210);
                              FUN_004a1540(local_214);
                            }
                          }
                          else {
                            FUN_004830f0(local_210);
                            FUN_004830f0(local_218);
                          }
                          uStack_8 = (undefined *)0xffffffff;
                          cls_0x478720::meth_0x478730(&cStack_1e4);
                          ExceptionList = pvStack_10;
                          return 0;
                        }
                        iStack_1ac = 0;
                        iStack_198 = cls_0x5b98b8::meth_0x547240
                                               ((cls_0x5b98b8 *)this,auStack_d4,
                                                this->mbr_0x190 + iVar7);
                        iVar7 = iStack_1d4;
                        if (((iStack_198 != 0) && (iStack_1ac != 0)) && (iVar8 = 0, 0 < iVar5)) {
                          pcVar12 = acStack_b4;
                          do {
                            iVar5 = FUN_0059a530_stricmp(pcVar12,iStack_1ac);
                            if (iVar5 == 0) {
                              puStack_1cc = auStack_108 + iVar8;
                              break;
                            }
                            iVar8 = iVar8 + 1;
                            pcVar12 = pcVar12 + 10;
                          } while (iVar8 < iVar7);
                        }
                        if (local_20c == 2) {
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)this->mbr_0x190;
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar2 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar2 != '\0');
                          iVar5 = this->mbr_0x194 - (~uVar10 - 1);
                          pcVar12 = (char *)this->mbr_0x190 + (~uVar10 - 1);
                          _strncpy(pcVar12,local_1f4,iVar5 - 1);
                          pcVar12[iVar5 + -1] = '\0';
                          FUN_00479580();
                        }
                      }
                      FUN_004795a0();
                      if ((0 < iStack_1c4) && (iStack_1b0 != 0)) {
                        uVar10 = 0xffffffff;
                        pCVar14 = (LPCSTR)this->mbr_0x190;
                        do {
                          if (uVar10 == 0) break;
                          uVar10 = uVar10 - 1;
                          cVar2 = *pCVar14;
                          pCVar14 = pCVar14 + 1;
                        } while (cVar2 != '\0');
                        GetTextExtentExPointA
                                  ((HDC)this->mbr_0x17c,(LPCSTR)this->mbr_0x190,~uVar10 - 1,10000,
                                   (LPINT)0x0,(LPINT)0x0,&local_110);
                        if (iStack_1c4 < local_110.cx + local_224) {
                          iStack_228 = iStack_228 + iStack_1a8;
                          local_224 = local_220;
                          MoveToEx((HDC)this->mbr_0x17c,local_220,iStack_228,(LPPOINT)0x0);
                        }
                      }
                      if ((iStack_194 == 0) || (iStack_198 != 0)) {
                        if (iStack_18c == 0) {
                          if (local_1a0 == 0) {
                            if (iStack_1b4 == 0) {
                              cls_0x5b98b8::meth_0x546de0
                                        ((cls_0x5b98b8 *)this,(char)local_224,iStack_228);
                            }
                            else {
                              iVar5 = FUN_0059a530_stricmp(this->mbr_0x190,&DAT_005e55f4);
                              if ((iVar5 == 0) || (iStack_190 != 0)) {
                                if ((int)this->mbr_0x1ac < (int)this->mbr_0x1b0) {
                                  uStack_1b6 = 6;
                                  uStack_1b7 = 0xdd;
                                  uStack_1b8 = 0;
                                  uStack_1b5 = 0;
                                  cls_0x5b98b8::meth_0x546de0
                                            ((cls_0x5b98b8 *)this,(char)local_224,iStack_228);
                                }
                                else {
                                  unaff_EBX = 0x3c3c3c;
                                  cls_0x5b98b8::meth_0x546de0
                                            ((cls_0x5b98b8 *)this,(char)local_224,iStack_228);
                                }
                              }
                              else {
                                iVar5 = FUN_0059a530_stricmp(this->mbr_0x190,&DAT_005e55fc);
                                if ((iVar5 == 0) || (pcStack_19c != (cls_0x5a3c68 *)0x0)) {
                                  if ((int)this->mbr_0x1ac < 1) {
                                    uStack_1ba = 0x3c;
                                    uStack_1bb = 0x3c;
                                    uStack_1bc = 0x3c;
                                    uStack_1b9 = 0;
                                    cls_0x5b98b8::meth_0x546de0
                                              ((cls_0x5b98b8 *)this,(char)local_224,iStack_228);
                                  }
                                  else {
                                    uStack_1c6 = 6;
                                    uStack_1c7 = 0xdd;
                                    uStack_1c8 = 0;
                                    uStack_1c5 = 0;
                                    cls_0x5b98b8::meth_0x546de0
                                              ((cls_0x5b98b8 *)this,(char)local_224,iStack_228);
                                  }
                                }
                              }
                            }
                            goto LAB_005488ba;
                          }
                        }
                        else {
                          lpchText = (LPSTR)this->mbr_0x190;
                          uVar10 = 0xffffffff;
                          pCVar13 = lpchText;
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar2 = *pCVar13;
                            pCVar13 = pCVar13 + 1;
                          } while (cVar2 != '\0');
                          tStack_16c.iTabLength = 0;
                          tStack_144.right = local_1d0 + local_224;
                          tStack_16c.iLeftMargin = 0;
                          tStack_16c.iRightMargin = 0;
                          tStack_16c.uiLengthDrawn = 0;
                          tStack_144.top = iStack_228;
                          tStack_144.bottom = local_1d8 + iStack_228;
                          tStack_16c.cbSize = 0x14;
                          tStack_144.left = local_224;
                          DrawTextExA((HDC)this->mbr_0x180,lpchText,~uVar10 - 1,&tStack_144,0x2810,
                                      &tStack_16c);
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)this->mbr_0x190;
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar2 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar2 != '\0');
                          if (tStack_16c.uiLengthDrawn < ~uVar10 - 1) {
                            pCVar13 = (char *)this->mbr_0x190 + tStack_16c.uiLengthDrawn;
                            this->mbr_0x1b0 = 1;
                            if (pCVar13 == (LPSTR)0x0) goto LAB_00548808;
                            uVar10 = 0xffffffff;
                            tStack_158.iTabLength = 0;
                            tStack_158.cbSize = 0x14;
                            pCVar9 = pCVar13;
                            do {
                              if (uVar10 == 0) break;
                              uVar10 = uVar10 - 1;
                              cVar2 = *pCVar9;
                              pCVar9 = pCVar9 + 1;
                            } while (cVar2 != '\0');
                            tStack_158.iLeftMargin = 0;
                            tStack_134.left = local_224;
                            tStack_158.iRightMargin = 0;
                            tStack_134.right = local_1d0 + local_224;
                            tStack_158.uiLengthDrawn = 0;
                            tStack_134.top = iStack_228;
                            tStack_134.bottom = local_1d8 + iStack_228;
                            DrawTextExA((HDC)this->mbr_0x180,pCVar13,~uVar10 - 1,&tStack_134,0x2810,
                                        &tStack_158);
                            uVar10 = 0xffffffff;
                            pCVar9 = pCVar13;
                            do {
                              if (uVar10 == 0) break;
                              uVar10 = uVar10 - 1;
                              cVar2 = *pCVar9;
                              pCVar9 = pCVar9 + 1;
                            } while (cVar2 != '\0');
                            if (~uVar10 - 1 <= tStack_158.uiLengthDrawn) goto LAB_00548808;
                            this->mbr_0x1b0 = 2;
                            pCVar9 = pCVar13 + tStack_158.uiLengthDrawn;
                          }
                          else {
                            pCVar13 = (LPSTR)0x0;
                            this->mbr_0x1b0 = 0;
LAB_00548808:
                            pCVar9 = (LPSTR)0x0;
                          }
                          if ((int)this->mbr_0x1b0 < (int)this->mbr_0x1ac) {
                            this->mbr_0x1ac = this->mbr_0x1b0;
                            pCVar9 = (LPSTR)0x0;
                          }
                          dVar3 = this->mbr_0x1ac;
                          if ((((dVar3 != 0) && (lpchText = pCVar13, dVar3 != 1)) &&
                              (lpchText = pCVar9, dVar3 != 2)) || (lpchText == (LPSTR)0x0))
                          goto LAB_005488ba;
                        }
                        cls_0x5b98b8::meth_0x546f40
                                  ((cls_0x5b98b8 *)this,(char)local_224,(char)iStack_228,
                                   (char)local_1d0,local_1d8);
                      }
                    }
                    else {
                      FUN_00479580();
                      FUN_004795a0();
                      iStack_228 = iStack_228 + iStack_1a8;
                      local_224 = local_220;
                      MoveToEx((HDC)this->mbr_0x17c,local_220,iStack_228,(LPPOINT)0x0);
                    }
                  }
                  else {
                    FUN_00479580();
                    iVar5 = FUN_0047a410(&local_21c,&DAT_005e5564,&iStack_180);
                    if (iVar5 == 0) {
                      cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                      uStack_8 = (undefined *)0xc;
                      FUN_004830f0(local_1f4);
                      if (local_218 == 0) {
                        if (local_214 != 0) {
                          FUN_004830f0(local_210);
                          FUN_004a1540(local_214);
                        }
                      }
                      else {
                        FUN_004830f0(local_210);
                        FUN_004830f0(local_218);
                      }
                      uStack_8 = (undefined *)0xffffffff;
                      cls_0x478720::meth_0x478730(&cStack_1e4);
                      ExceptionList = pvStack_10;
                      return 0;
                    }
                    local_224 = iStack_180 + local_220;
                    MoveToEx((HDC)this->mbr_0x17c,local_224,iStack_228,(LPPOINT)0x0);
                  }
                }
                else {
                  FUN_00479580();
                  iVar5 = FUN_0047a410(&local_21c,s__d__d_005e5558,&iStack_170,&iStack_184);
                  if (iVar5 == 0) {
                    cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                    uStack_8 = (undefined *)0xb;
                    FUN_004830f0(local_1f4);
                    if (local_218 == 0) {
                      if (local_214 != 0) {
                        FUN_004830f0(local_210);
                        FUN_004a1540(local_214);
                      }
                    }
                    else {
                      FUN_004830f0(local_210);
                      FUN_004830f0(local_218);
                    }
                    uStack_8 = (undefined *)0xffffffff;
                    cls_0x478720::meth_0x478730(&cStack_1e4);
                    ExceptionList = pvStack_10;
                    return 0;
                  }
                  iStack_228 = iStack_228 + iStack_184;
                  local_224 = local_224 + iStack_170;
                  local_220 = local_224;
                  MoveToEx((HDC)this->mbr_0x17c,local_224,iStack_228,(LPPOINT)0x0);
                }
              }
              else {
                FUN_00479580();
                iVar5 = FUN_0047a410(&local_21c,s__d__d_005e5548,&local_224,&iStack_228);
                if (iVar5 == 0) {
                  cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                  uStack_8 = (undefined *)0xa;
                  FUN_004830f0(local_1f4);
                  if (local_218 == 0) {
                    if (local_214 != 0) {
                      FUN_004830f0(local_210);
                      FUN_004a1540(local_214);
                    }
                  }
                  else {
                    FUN_004830f0(local_210);
                    FUN_004830f0(local_218);
                  }
                  uStack_8 = (undefined *)0xffffffff;
                  cls_0x478720::meth_0x478730(&cStack_1e4);
                  ExceptionList = pvStack_10;
                  return 0;
                }
                local_220 = local_224;
                MoveToEx((HDC)this->mbr_0x17c,local_224,iStack_228,(LPPOINT)0x0);
              }
            }
            else {
              FUN_00479580();
              iVar5 = -1;
              puVar11 = &uStack_1dc;
              if (local_20c == 4) {
                iVar5 = 0;
                if (iStack_1d4 < 1) {
LAB_00547dcd:
                  _strncpy(acStack_b4 + iVar5 * 10,local_1f4,9);
                  iStack_1d4 = iVar5 + 1;
                  auStack_ab[iVar5 * 10] = 0;
                  puVar11 = auStack_108 + iVar5;
                }
                else {
                  pcVar12 = acStack_b4;
                  do {
                    iVar7 = FUN_0059a530_stricmp(pcVar12,local_1f4);
                    if (iVar7 == 0) {
                      puVar11 = auStack_108 + iVar5;
                      if (iVar5 < iStack_1d4) goto LAB_00547df9;
                      break;
                    }
                    iVar5 = iVar5 + 1;
                    pcVar12 = pcVar12 + 10;
                  } while (iVar5 < iStack_1d4);
                  if (iVar5 < 8) goto LAB_00547dcd;
                }
LAB_00547df9:
                FUN_00479580();
              }
              if (local_20c == 8) {
                iVar5 = FUN_0047a410(&local_21c,s__b__b__b_005e5538,(int)puVar11 + 2,
                                     (int)puVar11 + 1,puVar11);
                if (iVar5 == 0) {
                  cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                  uStack_8 = (undefined *)0x8;
                  FUN_004830f0(local_1f4);
                  if (local_218 == 0) {
                    if (local_214 != 0) {
                      FUN_004830f0(local_210);
                      FUN_004a1540(local_214);
                    }
                  }
                  else {
                    FUN_004830f0(local_210);
                    FUN_004830f0(local_218);
                  }
                  uStack_8 = (undefined *)0xffffffff;
                  cls_0x478720::meth_0x478730(&cStack_1e4);
                  ExceptionList = pvStack_10;
                  return 0;
                }
              }
              else {
                if ((iVar5 < 0) || (7 < iVar5)) {
                  cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                  uStack_8 = (undefined *)0x9;
                  FUN_004830f0(local_1f4);
                  if (local_218 == 0) {
                    if (local_214 != 0) {
                      FUN_004830f0(local_210);
                      FUN_004a1540(local_214);
                    }
                  }
                  else {
                    FUN_004830f0(local_210);
                    FUN_004830f0(local_218);
                  }
                  uStack_8 = (undefined *)0xffffffff;
                  cls_0x478720::meth_0x478730(&cStack_1e4);
                  ExceptionList = pvStack_10;
                  return 0;
                }
                uStack_1dc = *puVar11;
                FUN_004795a0();
              }
            }
          }
          else {
            FUN_00479580();
            iVar5 = FUN_00479700(&DAT_005e5524,0);
            if (iVar5 == 0) {
              iVar5 = FUN_0047a410(&local_21c,&DAT_005e552c,&iStack_1c4);
              if (iVar5 == 0) {
                cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
                uStack_8 = (undefined *)0x7;
                FUN_004830f0(local_1f4);
                if (local_218 == 0) {
                  if (local_214 != 0) {
                    FUN_004830f0(local_210);
                    FUN_004a1540(local_214);
                  }
                }
                else {
                  FUN_004830f0(local_210);
                  FUN_004830f0(local_218);
                }
                uStack_8 = (undefined *)0xffffffff;
                cls_0x478720::meth_0x478730(&cStack_1e4);
                ExceptionList = pvStack_10;
                return 0;
              }
            }
            else {
              FUN_00479580();
              FUN_004795a0();
              iStack_1c4 = -1;
            }
          }
        }
        else {
          FUN_00479580();
          iVar5 = FUN_0047a410(&local_21c,&DAT_005e5514,&iStack_1a8);
          if (iVar5 == 0) {
            cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
            uStack_8 = (undefined *)0x6;
            FUN_004830f0(local_1f4);
            if (local_218 == 0) {
              if (local_214 != 0) {
                FUN_004830f0(local_210);
                FUN_004a1540(local_214);
              }
            }
            else {
              FUN_004830f0(local_210);
              FUN_004830f0(local_218);
            }
            uStack_8 = (undefined *)0xffffffff;
            cls_0x478720::meth_0x478730(&cStack_1e4);
            ExceptionList = pvStack_10;
            return 0;
          }
        }
      }
      else {
        FUN_00479580();
        FUN_004795a0();
        piVar6 = (int *)(**(code **)(*(int *)this->mbr_0x184 + 0xc))();
        iVar5 = (**(code **)(*piVar6 + 0x68))(piVar6,this->mbr_0x17c);
        if (iVar5 != 0) {
          FUN_004a90d0(iVar5,s_d__revenant_StatPane_cpp_005e5620,0x360);
        }
        iVar5 = (**(code **)(*(int *)this->mbr_0x198 + 0x130))();
        iStack_1d4 = 0;
        if (iVar5 != 0) {
          if ((*(int *)(iVar5 + 0x18) == 0) && (*(short *)(this->mbr_0x198 + 4) != 8)) {
            iVar7 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
            iStack_1d4 = 1;
            *(uint *)(iVar5 + 0x18) = (-(uint)(iVar7 != 0x10) & 0xffff8400) + 0xf81f;
          }
          if (*(short *)(this->mbr_0x198 + 4) != 0xc) {
            cls_0x5a68d8::meth_0x4bda20
                      ((cls_0x5a68d8 *)this->mbr_0x184,unaff_EBX + 3,unaff_EBP + 3,iVar5,0x10100);
          }
          cls_0x5a68d8::meth_0x4bd680
                    ((cls_0x5a68d8 *)this->mbr_0x184,unaff_EBX,unaff_EBP,iVar5,0x100);
          if (*(short *)(this->mbr_0x198 + 4) == 0xc) {
            dVar3 = this->mbr_0x188;
            piVar6 = (int *)this->mbr_0x184;
            FUN_00438d80(auStack_6c,unaff_EBX,unaff_EBP,0,0,*(undefined4 *)(dVar3 + 4),
                         *(undefined4 *)(dVar3 + 8),0x100);
            (**(code **)(*piVar6 + 0x5c))(auStack_6c,dVar3,0,0);
          }
          if (iStack_1d4 != 0) {
            *(undefined4 *)(iVar5 + 0x18) = 0;
          }
        }
        piVar6 = (int *)(**(code **)(*(int *)this->mbr_0x184 + 0xc))();
        pdVar1 = &this->mbr_0x17c;
        iVar5 = (**(code **)(*piVar6 + 0x44))(piVar6,pdVar1);
        if (iVar5 != 0) {
          FUN_004a90d0(iVar5,s_d__revenant_StatPane_cpp_005e5604,0x354);
        }
        iVar5 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
        if (iVar5 == 0) {
          iVar5 = DAT_0065b024;
        }
        SelectObject((HDC)*pdVar1,*(HGDIOBJ *)(iVar5 + 0x38));
        SetViewportOrgEx((HDC)*pdVar1,*(int *)(this->mbr_0x184 + 0x14),
                         *(int *)(this->mbr_0x184 + 0x18),(LPPOINT)0x0);
      }
    }
    else {
      FUN_00479580();
      iVar5 = FUN_0047a410(&local_21c,s__20s__d__d__d__d_005e54ec,auStack_e8,&uStack_188,&uStack_174
                           ,&uStack_178,&uStack_17c);
      if (iVar5 == 0) {
        local_1c0 = 0xffffff;
        cls_0x5b98b8::meth_0x546de0((cls_0x5b98b8 *)this,0x14,0x14);
      }
      if (local_1a4 != 0) {
        pcStack_19c = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
        uStack_8._0_1_ = 5;
        if (pcStack_19c != (cls_0x5a3c68 *)0x0) {
          cls_0x5a3c68::cls_0x5a3c68
                    (pcStack_19c,auStack_e8,uStack_188,uStack_174,uStack_178,uStack_17c,0,0,0,0,
                     0xffffffff,0x100000);
        }
        uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,1);
        cls_0x5b98b8::meth_0x436790((cls_0x5b98b8 *)this);
      }
    }
LAB_005488ba:
    iVar5 = FUN_00479700(&DAT_005e54e0,0);
  } while( true );
}



