// REVSYNC: candidate=? confidence=LOW evidence=none(no source-path, no distinguishing strings; 66 methods, 16 fields) src-file=unknown date=2026-04-17 note="prior suffix was 'likely_TDirectX_or_TNetwork' — pure speculation with no supporting evidence. Suffix stripped; leave unclassified until we find fingerprint evidence."
// Decompiled methods and structure for class: cls_0x57d9d0

/*
/OOAnalyzer/cls_0x57d9d0
pack(disabled)
Structure cls_0x57d9d0 {
   84   byte   1   mbr_0x54   "Unsigned Byte (db)"
   85   byte   1   mbr_0x55   "Unsigned Byte (db)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   byte   1   mbr_0x7c   "Unsigned Byte (db)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 160 Alignment: 1

*/

// Function at 0057d9d0

dword __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x57d9d0
          (cls_0x57d9d0 *this,undefined param_1,int param_2,dword param_3)

{
  cls_0x588410 *pcVar1;
  int *piVar2;
  cls_0x570900 *pcVar3;
  cls_0x570900 *pcVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  uint uVar9;
  cls_0x570900 *this_00;
  bool bVar10;
  undefined3 in_stack_00000005;
  uint in_stack_00000010;
  int in_stack_00000014;
  int iStack_c;
  int *local_8;
  
  piVar2 = (int *)_param_1;
  if (DAT_00676828 == 0) {
    return 0;
  }
  meth_0x57dc70(this);
  if (_param_1 == (cls_0x570900 *)0x0) {
    param_3 = 3;
  }
  uVar9 = (this->mbr_0x9c | in_stack_00000010 & 0xffff) & ~in_stack_00000010 >> 0x10;
  if (((uVar9 & 8) != 0) && (DAT_0067682c == 0)) {
    return 0;
  }
  if (((uVar9 & 0x10) != 0) && ((DAT_00676828 == 0 || (DAT_0067682c != 0)))) {
    return 0;
  }
  if (((uVar9 & 0x20) != 0) && (this->mbr_0x54 != 0)) {
    return 0;
  }
  if (((uVar9 & 0x40) != 0) && (this->mbr_0x55 != 0)) {
    return 0;
  }
  local_8 = (int *)0x0;
  if ((_param_1 == (cls_0x570900 *)0x0) ||
     ((*(short *)((int)_param_1 + 4) == 0xb &&
      (local_8 = (int *)_param_1, (*(byte *)((int)_param_1 + 0x36c) & 4) != 0)))) {
    uVar5 = -(uint)(DAT_0067682c != 0) & 3;
  }
  else {
    uVar5 = (**(code **)(*(int *)_param_1 + 0x178))();
  }
  if (((uVar9 & 1) == 0) && (((int)uVar5 < 1 || (0 < (int)this->mbr_0x74)))) {
    return 0;
  }
  iStack_c = 0;
  if (((_param_1 != (cls_0x570900 *)0x0) && (uVar5 == 2)) && ((uVar9 & 1) == 0)) {
    iStack_c = (**(code **)(*(int *)_param_1 + 0x184))();
  }
  this_00 = DAT_006766d4;
  bVar10 = DAT_006766d4 != (cls_0x570900 *)0x0;
  this->mbr_0x60 = 0;
  _param_1 = (cls_0x570900 *)0x0;
  DAT_006766ec = (cls_0x570900 *)0x0;
  if (bVar10) {
    do {
      iVar6 = cls_0x570900::meth_0x570c00(this_00);
      pcVar3 = DAT_006766ec;
      pcVar4 = _param_1;
      if (((((this_00->mbr_0x30 == 0) && (-1 < (int)this_00->mbr_0x34)) &&
           ((this_00->field_0x33 != '\0' && (iStack_c != *(int *)&this_00->field_0x1c)))) &&
          ((in_stack_00000014 == 0 || (*(int *)&this_00->field_0x1c == in_stack_00000014)))) &&
         (((DAT_00676828 != 0 && (DAT_0067682c == 0)) ||
          ((((uVar9 & 2) != 0 || (iVar7 = FUN_0045d590(this_00->mbr_0x34,piVar2), iVar7 != 0)) ||
           (((pcVar3 = DAT_006766ec, (uVar9 & 4) != 0 &&
             (((iVar6 != 0 && (local_8 != (int *)0x0)) && (*(char *)(local_8 + 0x125) != '\0')))) &&
            (iVar6 = FUN_0059a530_stricmp(local_8 + 0x125,iVar6 + 0x494), pcVar3 = DAT_006766ec, iVar6 == 0)
            ))))))) {
        if (this->mbr_0x60 == 0) {
          pcVar1 = (cls_0x588410 *)this_00->mbr_0x14;
          this->mbr_0x60 = (dword)pcVar1;
          this->mbr_0x5c = (dword)piVar2;
          this->mbr_0x64 = param_3;
          if (param_3 != 3) {
            cls_0x588410::meth_0x57c9c0(pcVar1,piVar2);
          }
          cls_0x588410::meth_0x588570((cls_0x588410 *)this->mbr_0x60);
          if (param_2 != 0) {
            pcVar1 = (cls_0x588410 *)this->mbr_0x60;
            puVar8 = (undefined *)pcVar1->mbr_0xc;
            pcVar1->mbr_0xc = (dword)(puVar8 + 1);
            if ((undefined *)pcVar1->mbr_0x4 < puVar8 + 1) {
              puVar8 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
            }
            *puVar8 = (undefined)param_2;
          }
          this->mbr_0x58 = (dword)this_00;
          pcVar3 = DAT_006766ec;
        }
        else {
          pcVar3 = this_00;
          pcVar4 = this_00;
          if (_param_1 != (cls_0x570900 *)0x0) {
            _param_1->mbr_0x10 = (dword)this_00;
            pcVar3 = DAT_006766ec;
          }
        }
      }
      _param_1 = pcVar4;
      DAT_006766ec = pcVar3;
      this_00 = (cls_0x570900 *)this_00->mbr_0xc;
    } while (this_00 != (cls_0x570900 *)0x0);
    if (_param_1 != (cls_0x570900 *)0x0) {
      _param_1->mbr_0x10 = 0;
    }
  }
  return this->mbr_0x60;
}



// Function at 0057dc70

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x57dc70(cls_0x57d9d0 *this)

{
  int *piVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  cls_0x588410 *this_00;
  cls_0x588410 *this_01;
  int iVar5;
  int iVar6;
  
  piVar1 = (int *)this->mbr_0x60;
  if (piVar1 == (int *)0x0) {
    DAT_006766ec = 0;
    return;
  }
  iVar2 = *piVar1;
  dVar3 = this->mbr_0x68;
  iVar4 = piVar1[3];
  iVar6 = DAT_006766ec;
  if (DAT_006766ec != 0) {
    do {
      this_00 = *(cls_0x588410 **)(iVar6 + 0x14);
      if (this->mbr_0x64 != 3) {
        cls_0x588410::meth_0x57c9c0(this_00,this->mbr_0x5c);
      }
      if (0 < (int)((iVar4 - iVar2) - dVar3)) {
        this_01 = (cls_0x588410 *)this->mbr_0x60;
        iVar5 = this_01->mbr_0x0 + this->mbr_0x68;
        if (this_01->mbr_0x4 < this_01->mbr_0xc) {
          iVar5 = cls_0x588410::meth_0x5884a0(this_01);
        }
        cls_0x588410::meth_0x5886d0(this_00,iVar5);
      }
      piVar1 = (int *)(iVar6 + 0x10);
      iVar6 = *piVar1;
    } while (*piVar1 != 0);
  }
  this->mbr_0x60 = 0;
  DAT_006766ec = 0;
  return;
}



// Function at 00583b60

uint __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583b60(cls_0x57d9d0 *this,undefined param_1)

{
  int **ppiVar1;
  dword dVar2;
  dword *pdVar3;
  dword **ppdVar4;
  dword **ppdVar5;
  undefined3 in_stack_00000005;
  char in_stack_00000008;
  
  if (*(int *)this->mbr_0x8c != 0) {
    for (ppiVar1 = (int **)((int *)this->mbr_0x8c)[2]; ppiVar1 != (int **)0x0;
        ppiVar1 = (int **)(*ppiVar1)[2]) {
      if (((dword *)ppiVar1[3] == _param_1) && (*(char *)(ppiVar1 + 4) == in_stack_00000008)) {
        return CONCAT31((int3)((uint)ppiVar1 >> 8),1);
      }
      if (**ppiVar1 == 0) break;
    }
  }
  if ((*(int *)this->mbr_0x80 == 0) ||
     (ppdVar5 = (dword **)((int *)this->mbr_0x80)[2], ppdVar5 == (dword **)0x0)) {
    ppdVar5 = (dword **)FUN_00482fb0(0x18);
    if (ppdVar5 == (dword **)0x0) {
      ppdVar5 = (dword **)0x0;
    }
    else {
      ppdVar5[1] = (dword *)0x0;
      *ppdVar5 = (dword *)0x0;
      ppdVar5[2] = (dword *)ppdVar5;
    }
  }
  dVar2 = this->mbr_0x98;
  ppdVar5[3] = _param_1;
  *(undefined *)(ppdVar5 + 4) = 1;
  ppdVar5[5] = (dword *)(dVar2 + 0x18);
  pdVar3 = *ppdVar5;
  if (pdVar3 != (dword *)0x0) {
    ppdVar4 = (dword **)ppdVar5[1];
    pdVar3[1] = (dword)ppdVar4;
    *ppdVar4 = pdVar3;
    *ppdVar5 = (dword *)0x0;
  }
  ppiVar1 = (int **)this->mbr_0x94;
  *ppdVar5 = &this->mbr_0x90;
  ppdVar5[1] = (dword *)ppiVar1;
  this->mbr_0x94 = (dword)ppdVar5;
  *ppiVar1 = (int *)ppdVar5;
  return (uint)ppdVar5 & 0xffffff00;
}



// Function at 00583c20

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583c20(cls_0x57d9d0 *this)

{
  cls_0x588410 *this_00;
  undefined *puVar1;
  char cVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_00000004;
  
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    iVar3 = cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738);
    if (iVar3 != 0) {
      cVar2 = meth_0x583b60(this,(char)in_stack_00000004);
      if (cVar2 == '\0') {
        this_00 = *(cls_0x588410 **)(iVar3 + 0x14);
        puVar4 = (undefined *)this_00->mbr_0xc;
        this_00->mbr_0xc = (dword)(puVar4 + 1);
        if ((undefined *)this_00->mbr_0x4 < puVar4 + 1) {
          puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
        }
        puVar1 = (undefined *)this_00->mbr_0x4;
        *puVar4 = 1;
        puVar4 = (undefined *)this_00->mbr_0xc;
        this_00->mbr_0xc = (dword)(puVar4 + 4);
        if (puVar1 < puVar4 + 4) {
          puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
        }
        *puVar4 = (char)in_stack_00000004;
        puVar4[1] = (char)((uint)in_stack_00000004 >> 8);
        puVar4[2] = in_stack_00000004._2_1_;
        puVar4[3] = in_stack_00000004._3_1_;
      }
    }
  }
  return;
}



// Function at 00583cb0

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583cb0(cls_0x57d9d0 *this)

{
  cls_0x588410 *this_00;
  undefined *puVar1;
  char cVar2;
  int iVar3;
  undefined *puVar4;
  undefined in_stack_00000004;
  
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    iVar3 = cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738);
    if (iVar3 != 0) {
      cVar2 = meth_0x583b60(this,in_stack_00000004);
      if (cVar2 == '\0') {
        this_00 = *(cls_0x588410 **)(iVar3 + 0x14);
        puVar4 = (undefined *)this_00->mbr_0xc;
        this_00->mbr_0xc = (dword)(puVar4 + 1);
        if ((undefined *)this_00->mbr_0x4 < puVar4 + 1) {
          puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
        }
        puVar1 = (undefined *)this_00->mbr_0x4;
        *puVar4 = 2;
        puVar4 = (undefined *)this_00->mbr_0xc;
        this_00->mbr_0xc = (dword)(puVar4 + 1);
        if (puVar1 < puVar4 + 1) {
          puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
        }
        *puVar4 = in_stack_00000004;
      }
    }
  }
  return;
}



// Function at 00583da0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583da0(cls_0x57d9d0 *this)

{
  int iVar1;
  undefined in_stack_00000004;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,in_stack_00000004,0,2);
    if (iVar1 != 0) {
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00583de0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583de0(cls_0x57d9d0 *this,undefined param_1)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,param_1,0,1);
    if (iVar1 != 0) {
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00583e30

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x583e30(cls_0x57d9d0 *this)

{
  int iVar1;
  undefined in_stack_00000004;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,in_stack_00000004,0x3f,0);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x57ca40((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00583e80

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x583e80
          (cls_0x57d9d0 *this,undefined4 param_1,undefined param_2,undefined4 param_3,
          undefined4 param_4)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined3 in_stack_00000009;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,_param_2,param_4);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 1);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 1) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = (undefined)param_3;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00583f60

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x583f60
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,param_2,param_4);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = (char)param_3;
      puVar2[1] = (char)((uint)param_3 >> 8);
      puVar2[2] = param_3._2_1_;
      puVar2[3] = param_3._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00583fe0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x583fe0
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,param_2,param_4);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = (char)param_3;
      puVar2[1] = (char)((uint)param_3 >> 8);
      puVar2[2] = param_3._2_1_;
      puVar2[3] = param_3._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584060

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584060
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,param_2,param_4);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005840b0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5840b0(cls_0x57d9d0 *this,undefined4 param_1,undefined param_2)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  undefined3 in_stack_00000009;
  undefined in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x1f,1);
    if (iVar2 != 0) {
      if (_param_2 == 0) {
        uVar4 = 0;
      }
      else {
        uVar4 = *(undefined4 *)(_param_2 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3[3] = param_1._3_1_;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = in_stack_0000000c;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584150

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584150
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,
          undefined4 param_4,undefined4 param_5,undefined4 param_6)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined4 uVar3;
  int in_stack_0000001c;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x22,1);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588d70((cls_0x588410 *)this->mbr_0x60);
      cls_0x588410::meth_0x588d70((cls_0x588410 *)this->mbr_0x60);
      cls_0x588410::meth_0x588db0((cls_0x588410 *)this->mbr_0x60);
      cls_0x588410::meth_0x588db0((cls_0x588410 *)this->mbr_0x60);
      cls_0x588410::meth_0x588db0((cls_0x588410 *)this->mbr_0x60);
      if (in_stack_0000001c == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = *(undefined4 *)(in_stack_0000001c + 0x40);
      }
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      param_1._2_1_ = (undefined)((uint)uVar3 >> 0x10);
      *puVar2 = (char)uVar3;
      param_1._3_1_ = (undefined)((uint)uVar3 >> 0x18);
      puVar2[1] = (char)((uint)uVar3 >> 8);
      puVar2[2] = param_1._2_1_;
      puVar2[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584220

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x584220(cls_0x57d9d0 *this,int param_1)

{
  int iVar1;
  
  if ((DAT_00676828 != 0) && (*(short *)(param_1 + 4) != 0x19)) {
    iVar1 = meth_0x57d9d0(this,(char)param_1,0,2);
    if (iVar1 != 0) {
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584270

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x584270(cls_0x57d9d0 *this,int param_1)

{
  cls_0x588410 *this_00;
  undefined4 uVar1;
  int iVar2;
  undefined *puVar3;
  undefined uStack0000000a;
  undefined uStack0000000b;
  
  if ((DAT_00676828 != 0) && (param_1 != 0)) {
    iVar2 = meth_0x57d9d0(this,(char)param_1,0xf,3);
    if (iVar2 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      uVar1 = *(undefined4 *)(param_1 + 0x40);
      puVar3 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      uStack0000000a = (undefined)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      uStack0000000b = (undefined)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      puVar3[2] = uStack0000000a;
      puVar3[3] = uStack0000000b;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005843f0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x5843f0(cls_0x57d9d0 *this)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  undefined *puVar7;
  int *in_stack_00000004;
  undefined uStack0000000a;
  undefined uStack0000000b;
  
  if (DAT_00676828 != 0) {
    iVar6 = (**(code **)(*in_stack_00000004 + 0xb4))();
    if (iVar6 == 0) {
      iVar6 = meth_0x57d9d0(this,(char)in_stack_00000004,0,1);
      if (iVar6 != 0) {
        meth_0x57dc70(this);
        return 1;
      }
    }
    else {
      iVar6 = meth_0x57d9d0(this,(char)in_stack_00000004,0x39,0);
      if (iVar6 != 0) {
        piVar5 = (int *)in_stack_00000004[0x19];
        piVar4 = in_stack_00000004;
        while (piVar3 = piVar5, piVar3 != (int *)0x0) {
          piVar4 = piVar3;
          piVar5 = (int *)piVar3[0x19];
        }
        pcVar1 = (cls_0x588410 *)this->mbr_0x60;
        puVar7 = (undefined *)pcVar1->mbr_0xc;
        uVar2 = *(undefined4 *)((-(uint)(piVar4 != in_stack_00000004) & (uint)piVar4) + 0x40);
        pcVar1->mbr_0xc = (dword)(puVar7 + 4);
        if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 4) {
          puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
        }
        uStack0000000a = (undefined)((uint)uVar2 >> 0x10);
        uStack0000000b = (undefined)((uint)uVar2 >> 0x18);
        *puVar7 = (char)uVar2;
        puVar7[1] = (char)((uint)uVar2 >> 8);
        puVar7[2] = uStack0000000a;
        pcVar1 = (cls_0x588410 *)this->mbr_0x60;
        puVar7[3] = uStack0000000b;
        uVar2 = *(undefined4 *)(in_stack_00000004[0x19] + 0x40);
        puVar7 = (undefined *)pcVar1->mbr_0xc;
        pcVar1->mbr_0xc = (dword)(puVar7 + 4);
        if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 4) {
          puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
        }
        uStack0000000a = (undefined)((uint)uVar2 >> 0x10);
        *puVar7 = (char)uVar2;
        uStack0000000b = (undefined)((uint)uVar2 >> 0x18);
        puVar7[1] = (char)((uint)uVar2 >> 8);
        puVar7[2] = uStack0000000a;
        puVar7[3] = uStack0000000b;
        meth_0x57dc70(this);
        return 1;
      }
    }
  }
  return 0;
}



// Function at 00584500

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584500(cls_0x57d9d0 *this,int param_1,int param_2)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  int in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(char)param_1,0x10,0);
    if (iVar2 != 0) {
      if (in_stack_0000000c == param_1) {
        uVar4 = 0xffffffff;
      }
      else {
        uVar4 = *(undefined4 *)(in_stack_0000000c + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      *puVar3 = (char)uVar4;
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3[3] = param_1._3_1_;
      uVar4 = *(undefined4 *)(param_2 + 0x40);
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005845c0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5845c0(cls_0x57d9d0 *this,int param_1,int param_2)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  int in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(char)param_1,0x11,0);
    if (iVar2 != 0) {
      if (in_stack_0000000c == param_1) {
        uVar4 = 0xffffffff;
      }
      else {
        uVar4 = *(undefined4 *)(in_stack_0000000c + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      *puVar3 = (char)uVar4;
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3[3] = param_1._3_1_;
      uVar4 = *(undefined4 *)(param_2 + 0x40);
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584680

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x584680(cls_0x57d9d0 *this)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined4 uVar3;
  undefined uStack_2;
  undefined uStack_1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(char)DAT_00667fcc,0x35,0);
    if (iVar1 != 0) {
      if ((DAT_0065d674 == 0) || (DAT_0065d674 == DAT_00667fcc)) {
        uVar3 = 0xffffffff;
      }
      else {
        uVar3 = *(undefined4 *)(DAT_0065d674 + 0x40);
      }
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      uStack_2 = (undefined)((uint)uVar3 >> 0x10);
      *puVar2 = (char)uVar3;
      uStack_1 = (undefined)((uint)uVar3 >> 0x18);
      puVar2[1] = (char)((uint)uVar3 >> 8);
      puVar2[2] = uStack_2;
      puVar2[3] = uStack_1;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584710

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584710(cls_0x57d9d0 *this,int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  cls_0x588410 *pcVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar3 = meth_0x57d9d0(this,(undefined)param_2,0x12,0);
    if (iVar3 != 0) {
      uVar1 = *(undefined4 *)(param_1 + 0x40);
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      param_2._2_1_ = (undefined)((uint)uVar1 >> 0x10);
      param_2._3_1_ = (undefined)((uint)uVar1 >> 0x18);
      *puVar4 = (char)uVar1;
      puVar4[1] = (char)((uint)uVar1 >> 8);
      puVar4[2] = param_2._2_1_;
      puVar4[3] = param_2._3_1_;
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar4 = (char)in_stack_0000000c;
      puVar4[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar4[2] = in_stack_0000000c._2_1_;
      puVar4[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005847c0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5847c0(cls_0x57d9d0 *this,int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  cls_0x588410 *pcVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_0000000c;
  
  if ((this->mbr_0x54 == 0) && (DAT_00676828 != 0)) {
    iVar3 = meth_0x57d9d0(this,(undefined)param_2,0x13,0);
    if (iVar3 != 0) {
      uVar1 = *(undefined4 *)(param_1 + 0x40);
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      param_2._2_1_ = (undefined)((uint)uVar1 >> 0x10);
      param_2._3_1_ = (undefined)((uint)uVar1 >> 0x18);
      *puVar4 = (char)uVar1;
      puVar4[1] = (char)((uint)uVar1 >> 8);
      puVar4[2] = param_2._2_1_;
      puVar4[3] = param_2._3_1_;
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar4 = (char)in_stack_0000000c;
      puVar4[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar4[2] = in_stack_0000000c._2_1_;
      puVar4[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584870

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584870
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,int *param_3,int param_4)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  byte *pbVar3;
  undefined *puVar4;
  undefined4 uVar5;
  int in_stack_00000014;
  
  if ((DAT_00676828 != 0) && (iVar2 = meth_0x57d9d0(this,(undefined)param_2,0x31,1), iVar2 != 0)) {
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    if (param_3 == (int *)0x0) {
      param_4 = 0;
    }
    else if (0x7e < param_4) {
      param_4 = 0x7f;
    }
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    pbVar3 = (byte *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(pbVar3 + 1);
    if ((byte *)pcVar1->mbr_0x4 < pbVar3 + 1) {
      pbVar3 = (byte *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    *pbVar3 = in_stack_00000014 != 0 | (char)param_4 << 1;
    if (0 < param_4) {
      do {
        pcVar1 = (cls_0x588410 *)this->mbr_0x60;
        if (*param_3 == 0) {
          uVar5 = 0xffffffff;
        }
        else {
          uVar5 = *(undefined4 *)(*param_3 + 0x40);
        }
        puVar4 = (undefined *)pcVar1->mbr_0xc;
        pcVar1->mbr_0xc = (dword)(puVar4 + 4);
        if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
          puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
        }
        param_2._2_1_ = (undefined)((uint)uVar5 >> 0x10);
        param_2._3_1_ = (undefined)((uint)uVar5 >> 0x18);
        *puVar4 = (char)uVar5;
        param_3 = param_3 + 1;
        puVar4[1] = (char)((uint)uVar5 >> 8);
        param_4 = param_4 + -1;
        puVar4[2] = param_2._2_1_;
        puVar4[3] = param_2._3_1_;
      } while (param_4 != 0);
    }
    if (in_stack_00000014 != 0) {
      cls_0x588410::meth_0x588e80((cls_0x588410 *)this->mbr_0x60);
    }
    meth_0x57dc70(this);
    return 1;
  }
  return 0;
}



// Function at 00584960

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x584960(cls_0x57d9d0 *this,int param_1)

{
  undefined uVar1;
  cls_0x588410 *pcVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined *puVar6;
  int iVar7;
  uint uStack00000008;
  
  iVar4 = param_1;
  if (DAT_00676828 != 0) {
    iVar5 = meth_0x57d9d0(this,(char)param_1,0x32,0);
    if (iVar5 != 0) {
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      uVar3 = *(uint *)(param_1 + 0x490);
      puVar6 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar6 + 4);
      uStack00000008 = uVar3;
      if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 4) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar6 = (char)uVar3;
      puVar6[1] = (char)(uVar3 >> 8);
      puVar6[2] = uStack00000008._2_1_;
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar6[3] = uStack00000008._3_1_;
      cls_0x588410::meth_0x588660(pcVar2);
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      uVar1 = *(undefined *)(param_1 + 0x4d8);
      puVar6 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar6 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 1) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      *puVar6 = uVar1;
      uVar1 = *(undefined *)(param_1 + 0x4dc);
      puVar6 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar6 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 1) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      *puVar6 = uVar1;
      cls_0x588410::meth_0x588570(pcVar2);
      uVar3 = pcVar2->mbr_0xc + 1;
      pcVar2->mbr_0xc = uVar3;
      if (pcVar2->mbr_0x4 < uVar3) {
        cls_0x588410::meth_0x5884a0(pcVar2);
      }
      param_1 = 0;
      iVar5 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
      if (0 < iVar5) {
        do {
          iVar5 = cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,param_1);
          if (((iVar5 != 0) && (iVar5 != iVar4)) && (*(char *)(iVar5 + 0x494) != '\0')) {
            iVar7 = FUN_0059a530_stricmp(iVar5 + 0x494,iVar4 + 0x494);
            uStack00000008 = (uint)(iVar7 == 0);
            if (uStack00000008 != 0) {
              pcVar2 = (cls_0x588410 *)this->mbr_0x60;
              uVar3 = *(uint *)(iVar5 + 0x40);
              puVar6 = (undefined *)pcVar2->mbr_0xc;
              pcVar2->mbr_0xc = (dword)(puVar6 + 4);
              uStack00000008 = uVar3;
              if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 4) {
                puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
              }
              *puVar6 = (char)uVar3;
              puVar6[1] = (char)(uVar3 >> 8);
              puVar6[2] = uStack00000008._2_1_;
              puVar6[3] = uStack00000008._3_1_;
            }
          }
          param_1 = param_1 + 1;
          iVar5 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
        } while (param_1 < iVar5);
      }
      cls_0x588410::meth_0x588a50((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584b30

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584b30
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined param_3,
          undefined4 param_4,undefined4 param_5,undefined *param_6)

{
  undefined uVar1;
  cls_0x588410 *pcVar2;
  undefined4 uVar3;
  int iVar4;
  undefined *puVar5;
  undefined *puVar6;
  char *pcVar7;
  char cVar8;
  undefined in_stack_0000000d;
  int in_stack_0000001c;
  
  puVar6 = param_6;
  if (DAT_00676828 != 0) {
    iVar4 = meth_0x57d9d0(this,(undefined)param_1,0x34,(uint)(param_6 != (undefined *)0x0));
    if (iVar4 != 0) {
      puVar5 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)this->mbr_0x60);
      *puVar5 = (undefined)param_4;
      puVar5 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)this->mbr_0x60);
      *puVar5 = (char)param_2;
      puVar5[1] = (char)((uint)param_2 >> 8);
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar5 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar5 + 2);
      if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 2) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar5 = param_3;
      puVar5[1] = in_stack_0000000d;
      puVar5 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)this->mbr_0x60);
      *puVar5 = (char)param_5;
      puVar5[1] = (char)((uint)param_5 >> 8);
      if (in_stack_0000001c == 0) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)this->mbr_0x60);
        *puVar5 = 0xff;
        puVar5[1] = 0xff;
        puVar5[2] = 0xff;
        puVar5[3] = 0xff;
      }
      else {
        pcVar2 = (cls_0x588410 *)this->mbr_0x60;
        uVar3 = *(undefined4 *)(in_stack_0000001c + 0x40);
        puVar5 = (undefined *)pcVar2->mbr_0xc;
        pcVar2->mbr_0xc = (dword)(puVar5 + 4);
        if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 4) {
          puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
        }
        param_6._2_1_ = (undefined)((uint)uVar3 >> 0x10);
        param_6._3_1_ = (undefined)((uint)uVar3 >> 0x18);
        *puVar5 = (char)uVar3;
        puVar5[1] = (char)((uint)uVar3 >> 8);
        puVar5[2] = param_6._2_1_;
        puVar5[3] = param_6._3_1_;
      }
      if (puVar6 == (undefined *)0x0) {
        pcVar2 = (cls_0x588410 *)this->mbr_0x60;
        puVar6 = (undefined *)pcVar2->mbr_0xc;
        pcVar2->mbr_0xc = (dword)(puVar6 + 1);
        if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 1) {
          puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
        }
        *puVar6 = 0xff;
        meth_0x57dc70(this);
        return 1;
      }
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      uVar1 = *puVar6;
      puVar5 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar5 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 1) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      *puVar5 = uVar1;
      uVar3 = *(undefined4 *)(puVar6 + 0x60);
      puVar5 = (undefined *)cls_0x588410::meth_0x5725f0(pcVar2);
      param_6._2_1_ = (undefined)((uint)uVar3 >> 0x10);
      *puVar5 = (char)uVar3;
      param_6._3_1_ = (undefined)((uint)uVar3 >> 0x18);
      puVar5[1] = (char)((uint)uVar3 >> 8);
      puVar5[2] = param_6._2_1_;
      puVar5[3] = param_6._3_1_;
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      if (*(int *)(puVar6 + 0x48) == 0) {
        pcVar2 = (cls_0x588410 *)this->mbr_0x60;
        puVar5 = (undefined *)pcVar2->mbr_0xc;
        pcVar2->mbr_0xc = (dword)(puVar5 + 1);
        if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 1) {
          puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
        }
        *puVar5 = 0xff;
      }
      else {
        pcVar2 = (cls_0x588410 *)this->mbr_0x60;
        uVar1 = *(undefined *)(*(int *)(puVar6 + 0x48) + 0x20);
        puVar5 = (undefined *)pcVar2->mbr_0xc;
        pcVar2->mbr_0xc = (dword)(puVar5 + 1);
        if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 1) {
          puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
        }
        *puVar5 = uVar1;
      }
      iVar4 = *(int *)(puVar6 + 0x4c);
      if (iVar4 != 0) {
        if ((*(uint *)(iVar4 + 0x24) & 0x1000) == 0) {
          if (iVar4 == 0) goto LAB_00584d20;
          cVar8 = -1 - *(char *)(iVar4 + 0x20);
        }
        else {
          cVar8 = *(char *)(iVar4 + 0x20);
        }
        pcVar2 = (cls_0x588410 *)this->mbr_0x60;
        pcVar7 = (char *)pcVar2->mbr_0xc;
        pcVar2->mbr_0xc = (dword)(pcVar7 + 1);
        if ((char *)pcVar2->mbr_0x4 < pcVar7 + 1) {
          pcVar7 = (char *)cls_0x588410::meth_0x5884a0(pcVar2);
        }
        *pcVar7 = cVar8;
        meth_0x57dc70(this);
        return 1;
      }
LAB_00584d20:
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar6 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar6 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar6 + 1) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar6 = 0x7f;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00584e00

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x584e00
          (cls_0x57d9d0 *this,undefined4 param_1,undefined param_2,undefined4 param_3)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  undefined3 in_stack_00000009;
  undefined uStack00000012;
  undefined uStack00000013;
  
  iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x2a,0);
  if (iVar2 != 0) {
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    puVar3 = (undefined *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(puVar3 + 1);
    if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    *puVar3 = (undefined)param_3;
    if (_param_2 == 0) {
      uVar4 = 0xffffffff;
    }
    else {
      uVar4 = *(undefined4 *)(_param_2 + 0x40);
    }
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    puVar3 = (undefined *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(puVar3 + 4);
    if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
      puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    uStack00000012 = (undefined)((uint)uVar4 >> 0x10);
    uStack00000013 = (undefined)((uint)uVar4 >> 0x18);
    *puVar3 = (char)uVar4;
    puVar3[1] = (char)((uint)uVar4 >> 8);
    puVar3[2] = uStack00000012;
    puVar3[3] = uStack00000013;
    meth_0x57dc70(this);
    return 1;
  }
  return 0;
}



// Function at 00585680

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x585680(cls_0x57d9d0 *this,int param_1)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined2 extraout_var;
  int *piVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  undefined4 *puVar8;
  undefined uVar9;
  int local_1c;
  undefined4 local_18;
  undefined4 local_14;
  cls_0x57d9d0 *local_10;
  undefined local_3;
  
  iVar5 = param_1;
  if ((DAT_00676828 != 0) && (param_1 != 0)) {
    local_10 = this;
    if (DAT_0067682c == 0) {
      iVar2 = meth_0x57d9d0(this,(char)param_1,0x16,0);
      if (iVar2 != 0) {
        piVar7 = (int *)(iVar5 + 0x2a0);
        iVar5 = 0xb;
        do {
          if (*piVar7 == 0) {
            uVar6 = 0xffffffff;
          }
          else {
            uVar6 = *(undefined4 *)(*piVar7 + 0x40);
          }
          pcVar1 = (cls_0x588410 *)this->mbr_0x60;
          puVar3 = (undefined *)pcVar1->mbr_0xc;
          pcVar1->mbr_0xc = (dword)(puVar3 + 4);
          param_1 = uVar6;
          if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
            puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
          }
          *puVar3 = (char)uVar6;
          piVar7 = piVar7 + 1;
          puVar3[1] = (char)((uint)uVar6 >> 8);
          iVar5 = iVar5 + -1;
          puVar3[2] = param_1._2_1_;
          puVar3[3] = param_1._3_1_;
        } while (iVar5 != 0);
        meth_0x57dc70(this);
        return 1;
      }
    }
    else {
      iVar2 = meth_0x57d9d0(this,(char)param_1,0,0);
      if (iVar2 != 0) {
        pcVar1 = (cls_0x588410 *)this->mbr_0x60;
        if (*(short *)(iVar5 + 4) == 0xb) {
          puVar3 = (undefined *)cls_0x588410::meth_0x5725f0(pcVar1);
          *puVar3 = 0x2b;
          cls_0x588410::meth_0x588570(pcVar1);
          cls_0x588410::meth_0x5725f0(pcVar1);
          local_3 = 1;
          cls_0x56fe30::cls_0x56fe30((cls_0x56fe30 *)&stack0x00000008);
          piVar7 = (int *)(iVar5 + 0x2a0);
          local_1c = 0xb;
          piVar4 = piVar7;
          do {
            iVar5 = *piVar4;
            if (iVar5 == 0) {
              puVar8 = &param_1;
              uVar9 = 1;
              param_1 = CONCAT31(param_1._1_3_,0xfe);
            }
            else {
              local_18 = *(undefined4 *)(*(int *)(iVar5 + 0x4c) + 0x1c);
              cls_0x56fe30::meth_0x56fe90((cls_0x56fe30 *)&stack0x00000008,&local_18,4);
              local_14 = CONCAT22(extraout_var,*(undefined2 *)(iVar5 + 0xc));
              puVar8 = &local_14;
              uVar9 = 2;
            }
            cls_0x56fe30::meth_0x56fe90((cls_0x56fe30 *)&stack0x00000008,puVar8,uVar9);
            piVar4 = piVar4 + 1;
            local_1c = local_1c + -1;
          } while (local_1c != 0);
          cls_0x588410::meth_0x57bc30(pcVar1);
          iVar5 = 0xb;
          do {
            FUN_00584fd0(*piVar7,pcVar1);
            piVar7 = piVar7 + 1;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          cls_0x588410::meth_0x588a50(pcVar1);
          this = local_10;
        }
        meth_0x57dc70(this);
        return 1;
      }
    }
  }
  return 0;
}



// Function at 00585880

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x585880(cls_0x57d9d0 *this)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  cls_0x570900 *this_00;
  undefined *puVar6;
  int *piVar7;
  int *in_stack_00000004;
  int in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    if (this->mbr_0x55 != 0) {
      return 1;
    }
    if (DAT_0067682c == 0) {
      return 1;
    }
    piVar4 = (int *)in_stack_00000004[0x19];
    piVar7 = in_stack_00000004;
    while (piVar3 = piVar4, piVar3 != (int *)0x0) {
      piVar7 = piVar3;
      piVar4 = (int *)piVar3[0x19];
    }
    piVar7 = (int *)(-(uint)(piVar7 != in_stack_00000004) & (uint)piVar7);
    if (piVar7 == (int *)0x0) {
      piVar7 = in_stack_00000004;
    }
    if ((*(short *)(piVar7 + 1) == 0xb) &&
       (iVar5 = meth_0x57d9d0(this,(char)piVar7,0x2c,0), iVar5 != 0)) {
      (**(code **)(*piVar7 + 0x184))();
      this_00 = (cls_0x570900 *)cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738);
      if (this_00 != (cls_0x570900 *)0x0) {
        cls_0x570900::meth_0x5722e0(this_00);
      }
      if (piVar7 == in_stack_00000004) {
        iVar5 = -1;
      }
      else {
        iVar5 = in_stack_00000004[0x10];
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar6 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar6 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar6 + 4) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      in_stack_00000004._2_1_ = (undefined)((uint)iVar5 >> 0x10);
      *puVar6 = (char)iVar5;
      in_stack_00000004._3_1_ = (undefined)((uint)iVar5 >> 0x18);
      puVar6[1] = (char)((uint)iVar5 >> 8);
      uVar2 = *(undefined4 *)(in_stack_00000008 + 0x40);
      puVar6[2] = in_stack_00000004._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar6[3] = in_stack_00000004._3_1_;
      puVar6 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar6 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar6 + 4) {
        puVar6 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      in_stack_00000004._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      in_stack_00000004._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      *puVar6 = (char)uVar2;
      puVar6[1] = (char)((uint)uVar2 >> 8);
      puVar6[2] = in_stack_00000004._2_1_;
      puVar6[3] = in_stack_00000004._3_1_;
      FUN_00584fd0(in_stack_00000008,this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005859c0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x5859c0(cls_0x57d9d0 *this)

{
  cls_0x588410 *this_00;
  int *piVar1;
  int *piVar2;
  int iVar3;
  cls_0x570900 *this_01;
  undefined *puVar4;
  int *piVar5;
  int *in_stack_00000004;
  
  if (DAT_00676828 != 0) {
    if (this->mbr_0x55 != 0) {
      return 1;
    }
    if (DAT_0067682c == 0) {
      return 1;
    }
    piVar2 = (int *)in_stack_00000004[0x19];
    piVar5 = in_stack_00000004;
    while (piVar1 = piVar2, piVar1 != (int *)0x0) {
      piVar5 = piVar1;
      piVar2 = (int *)piVar1[0x19];
    }
    piVar5 = (int *)(-(uint)(piVar5 != in_stack_00000004) & (uint)piVar5);
    if (piVar5 == (int *)0x0) {
      piVar5 = in_stack_00000004;
    }
    if ((*(short *)(piVar5 + 1) == 0xb) &&
       (iVar3 = meth_0x57d9d0(this,(char)piVar5,0x2d,0), iVar3 != 0)) {
      (**(code **)(*piVar5 + 0x184))();
      this_01 = (cls_0x570900 *)cls_0x5756d0::meth_0x5789a0((cls_0x5756d0 *)&DAT_00676738);
      if (this_01 != (cls_0x570900 *)0x0) {
        cls_0x570900::meth_0x5722e0(this_01);
      }
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      iVar3 = in_stack_00000004[0x10];
      puVar4 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      in_stack_00000004._2_1_ = (undefined)((uint)iVar3 >> 0x10);
      *puVar4 = (char)iVar3;
      in_stack_00000004._3_1_ = (undefined)((uint)iVar3 >> 0x18);
      puVar4[1] = (char)((uint)iVar3 >> 8);
      puVar4[2] = in_stack_00000004._2_1_;
      puVar4[3] = in_stack_00000004._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00585ab0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x585ab0(cls_0x57d9d0 *this,uint param_1,int param_2)

{
  cls_0x588410 *pcVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined *puVar5;
  undefined4 uVar6;
  uint uVar7;
  undefined4 in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    if (this->mbr_0x55 != 0) {
      return 1;
    }
    uVar3 = *(uint *)(param_1 + 100);
    uVar7 = param_1;
    while (uVar2 = uVar3, uVar2 != 0) {
      uVar7 = uVar2;
      uVar3 = *(uint *)(uVar2 + 100);
    }
    uVar7 = -(uint)(uVar7 != param_1) & uVar7;
    if (uVar7 == 0) {
      uVar7 = param_1;
    }
    iVar4 = meth_0x57d9d0(this,(char)uVar7,0x2e,0);
    if (iVar4 != 0) {
      if (uVar7 == param_1) {
        uVar6 = 0xffffffff;
      }
      else {
        uVar6 = *(undefined4 *)(param_1 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar5 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar5 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar5 + 4) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar6 >> 0x10);
      param_1._3_1_ = (undefined)((uint)uVar6 >> 0x18);
      *puVar5 = (char)uVar6;
      puVar5[1] = (char)((uint)uVar6 >> 8);
      puVar5[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar5[3] = param_1._3_1_;
      uVar6 = *(undefined4 *)(param_2 + 0x40);
      puVar5 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar5 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar5 + 4) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar6 >> 0x10);
      *puVar5 = (char)uVar6;
      param_1._3_1_ = (undefined)((uint)uVar6 >> 0x18);
      puVar5[1] = (char)((uint)uVar6 >> 8);
      puVar5[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar5[3] = param_1._3_1_;
      puVar5 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar5 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar5 + 2) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar5 = (char)in_stack_0000000c;
      puVar5[1] = (char)((uint)in_stack_0000000c >> 8);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00585bc0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x585bc0(cls_0x57d9d0 *this,undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  cls_0x588410 *this_00;
  int iVar2;
  undefined *puVar3;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x14,0);
    if (iVar2 != 0) {
      uVar1 = *(undefined4 *)(param_2 + 0x40);
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      param_1._2_1_ = (undefined)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      param_1._3_1_ = (undefined)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      cls_0x588410::meth_0x588e80((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00585e60

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x585e60(cls_0x57d9d0 *this,int param_1)

{
  int iVar1;
  short sVar2;
  char cVar3;
  short sVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 in_stack_00000008;
  
  uVar5 = 0;
  iVar6 = 0;
  sVar2 = 0;
  if (0 < *(int *)(param_1 + 0xb8)) {
    do {
      iVar1 = *(int *)(*(int *)(param_1 + 200) + iVar6 * 4);
      if ((iVar1 != 0) && (cVar3 = FUN_00585dd0(iVar1,in_stack_00000008), cVar3 != '\0')) {
        sVar4 = FUN_00585ec0(iVar1);
        sVar2 = sVar2 + sVar4;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < *(int *)(param_1 + 0xb8));
    uVar5 = CONCAT22((short)((uint)*(int *)(param_1 + 0xb8) >> 0x10),sVar2);
  }
  return uVar5;
}



// Function at 00585fb0

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x585fb0(cls_0x57d9d0 *this)

{
  uint uVar1;
  cls_0x588410 *pcVar2;
  undefined2 uVar3;
  int iVar4;
  undefined *puVar5;
  int *piVar6;
  int iVar7;
  int *in_stack_00000004;
  
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    iVar4 = meth_0x57d9d0(this,(char)in_stack_00000004,0x15,0);
    if (iVar4 != 0) {
      uVar3 = (**(code **)(*in_stack_00000004 + 0x1c0))();
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar5 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar5 + 2);
      if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 2) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar5 = (char)uVar3;
      puVar5[1] = (char)((ushort)uVar3 >> 8);
      uVar3 = FUN_00585cc0(in_stack_00000004);
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar5 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar5 + 2);
      if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 2) {
        puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      *puVar5 = (char)uVar3;
      puVar5[1] = (char)((ushort)uVar3 >> 8);
      cls_0x588410::meth_0x588570(pcVar2);
      uVar1 = pcVar2->mbr_0xc + 1;
      pcVar2->mbr_0xc = uVar1;
      if (pcVar2->mbr_0x4 < uVar1) {
        cls_0x588410::meth_0x5884a0(pcVar2);
      }
      if (((in_stack_00000004 == DAT_00667fcc) && (*(char *)(in_stack_00000004 + 0x125) != '\0')) &&
         ((DAT_00676dfc & 3) == 0)) {
        iVar7 = 0;
        iVar4 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
        if (0 < iVar4) {
          do {
            piVar6 = (int *)cls_0x45f7c0::meth_0x51eea0_TPlayerManager_GetPlayer((cls_0x45f7c0 *)&DAT_0065a890,iVar7);
            if (((piVar6 != (int *)0x0) && (piVar6 != in_stack_00000004)) &&
               (*(char *)(piVar6 + 0x125) != '\0')) {
              iVar4 = FUN_0059a530_stricmp(piVar6 + 0x125,in_stack_00000004 + 0x125);
              if (iVar4 == 0) {
                pcVar2 = (cls_0x588410 *)this->mbr_0x60;
                puVar5 = (undefined *)pcVar2->mbr_0xc;
                pcVar2->mbr_0xc = (dword)(puVar5 + 1);
                if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 1) {
                  puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
                }
                *puVar5 = (char)iVar7;
                uVar3 = FUN_00585cc0(piVar6);
                pcVar2 = (cls_0x588410 *)this->mbr_0x60;
                puVar5 = (undefined *)pcVar2->mbr_0xc;
                pcVar2->mbr_0xc = (dword)(puVar5 + 2);
                if ((undefined *)pcVar2->mbr_0x4 < puVar5 + 2) {
                  puVar5 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
                }
                *puVar5 = (char)uVar3;
                puVar5[1] = (char)((ushort)uVar3 >> 8);
              }
            }
            iVar7 = iVar7 + 1;
            iVar4 = cls_0x45f7c0::meth_0x51ee70_TPlayerManager_NumPlayers((cls_0x45f7c0 *)&DAT_0065a890);
          } while (iVar7 < iVar4);
        }
      }
      cls_0x588410::meth_0x588a50((cls_0x588410 *)this->mbr_0x60);
      DAT_00676dfc = DAT_00676dfc + 1;
      meth_0x57dc70(this);
    }
  }
  return;
}



// Function at 00586170

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586170(cls_0x57d9d0 *this,int param_1)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  char cVar4;
  short sVar5;
  int iVar6;
  undefined *puVar7;
  short sVar8;
  int in_stack_00000008;
  
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    iVar6 = meth_0x57d9d0(this,(undefined)param_1,0x3a,0);
    if (iVar6 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar7 = (undefined *)pcVar1->mbr_0xc;
      uVar2 = *(undefined4 *)(in_stack_00000008 + 8);
      pcVar1->mbr_0xc = (dword)(puVar7 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 1) {
        puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      *puVar7 = (char)uVar2;
      uVar2 = *(undefined4 *)(in_stack_00000008 + 0xc);
      puVar7 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar7 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 1) {
        puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      *puVar7 = (char)uVar2;
      uVar2 = *(undefined4 *)(in_stack_00000008 + 4);
      puVar7 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar7 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 1) {
        puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar7 = (char)uVar2;
      iVar6 = 0;
      param_1._0_2_ = 0;
      sVar5 = 0;
      if (0 < *(int *)(in_stack_00000008 + 0xb8)) {
        do {
          param_1._0_2_ = sVar5;
          iVar3 = *(int *)(*(int *)(in_stack_00000008 + 200) + iVar6 * 4);
          if (iVar3 != 0) {
            cVar4 = FUN_00585dd0(iVar3,0);
            if (cVar4 != '\0') {
              sVar5 = FUN_00585ec0(iVar3);
              param_1._0_2_ = (short)param_1 + sVar5;
            }
          }
          iVar6 = iVar6 + 1;
          sVar5 = (short)param_1;
        } while (iVar6 < *(int *)(in_stack_00000008 + 0xb8));
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar7 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar7 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 2) {
        puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      iVar6 = 0;
      *puVar7 = (char)(short)param_1;
      puVar7[1] = (char)((ushort)(short)param_1 >> 8);
      param_1._0_2_ = 0;
      sVar5 = 0;
      if (0 < *(int *)(in_stack_00000008 + 0xb8)) {
        do {
          param_1._0_2_ = sVar5;
          iVar3 = *(int *)(*(int *)(in_stack_00000008 + 200) + iVar6 * 4);
          if (iVar3 != 0) {
            cVar4 = FUN_00585dd0(iVar3,1);
            if (cVar4 != '\0') {
              sVar5 = FUN_00585ec0(iVar3);
              param_1._0_2_ = (short)param_1 + sVar5;
            }
          }
          iVar6 = iVar6 + 1;
          sVar5 = (short)param_1;
        } while (iVar6 < *(int *)(in_stack_00000008 + 0xb8));
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar7 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar7 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar7 + 2) {
        puVar7 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      sVar8 = 0;
      *puVar7 = (char)(short)param_1;
      puVar7[1] = (char)((ushort)(short)param_1 >> 8);
      param_1 = 0;
      sVar5 = 0;
      if (0 < *(int *)(in_stack_00000008 + 0xb8)) {
        do {
          sVar8 = sVar5;
          iVar6 = *(int *)(*(int *)(in_stack_00000008 + 200) + param_1 * 4);
          if (iVar6 != 0) {
            cVar4 = FUN_00585dd0(iVar6,2);
            if (cVar4 != '\0') {
              sVar5 = FUN_00585ec0(iVar6);
              sVar8 = sVar8 + sVar5;
            }
          }
          param_1 = param_1 + 1;
          sVar5 = sVar8;
        } while (param_1 < *(int *)(in_stack_00000008 + 0xb8));
      }
      puVar7 = (undefined *)cls_0x588410::meth_0x5725f0((cls_0x588410 *)this->mbr_0x60);
      *puVar7 = (char)sVar8;
      puVar7[1] = (char)((ushort)sVar8 >> 8);
      iVar6 = 0;
      if (0 < *(int *)(in_stack_00000008 + 0xb8)) {
        do {
          iVar3 = *(int *)(*(int *)(in_stack_00000008 + 200) + iVar6 * 4);
          if (iVar3 != 0) {
            cVar4 = FUN_00585dd0(iVar3,3);
            if (cVar4 != '\0') {
              FUN_00585ec0(iVar3);
            }
          }
          iVar6 = iVar6 + 1;
        } while (iVar6 < *(int *)(in_stack_00000008 + 0xb8));
      }
      cls_0x588410::meth_0x57bc30((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
    }
  }
  return;
}



// Function at 00586390

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586390(cls_0x57d9d0 *this,undefined4 param_1)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0,0);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x57d200((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005863f0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x5863f0(cls_0x57d9d0 *this)

{
  uint uVar1;
  int *piVar2;
  cls_0x588410 *pcVar3;
  undefined4 uVar4;
  int iVar5;
  char cVar6;
  undefined2 uVar7;
  int iVar8;
  undefined *puVar9;
  int *in_stack_00000004;
  int in_stack_00000008;
  undefined uStack0000000c;
  
  iVar5 = in_stack_00000008;
  if (DAT_00676828 != 0) {
    (**(code **)(*in_stack_00000004 + 0x184))();
    iVar8 = meth_0x57d9d0(this,(char)in_stack_00000004,0x3b,0);
    if (iVar8 != 0) {
      pcVar3 = (cls_0x588410 *)this->mbr_0x60;
      puVar9 = (undefined *)pcVar3->mbr_0xc;
      uVar4 = *(undefined4 *)(in_stack_00000008 + 8);
      pcVar3->mbr_0xc = (dword)(puVar9 + 1);
      if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 1) {
        puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
      }
      pcVar3 = (cls_0x588410 *)this->mbr_0x60;
      *puVar9 = (char)uVar4;
      uVar4 = *(undefined4 *)(in_stack_00000008 + 0xc);
      puVar9 = (undefined *)pcVar3->mbr_0xc;
      pcVar3->mbr_0xc = (dword)(puVar9 + 1);
      if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 1) {
        puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
      }
      pcVar3 = (cls_0x588410 *)this->mbr_0x60;
      *puVar9 = (char)uVar4;
      uVar4 = *(undefined4 *)(in_stack_00000008 + 4);
      puVar9 = (undefined *)pcVar3->mbr_0xc;
      pcVar3->mbr_0xc = (dword)(puVar9 + 1);
      if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 1) {
        puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
      }
      pcVar3 = (cls_0x588410 *)this->mbr_0x60;
      *puVar9 = (char)uVar4;
      puVar9 = (undefined *)pcVar3->mbr_0xc;
      pcVar3->mbr_0xc = (dword)(puVar9 + 1);
      if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 1) {
        puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
      }
      pcVar3 = (cls_0x588410 *)this->mbr_0x60;
      *puVar9 = uStack0000000c;
      cls_0x588410::meth_0x588570(pcVar3);
      uVar1 = pcVar3->mbr_0xc + 2;
      pcVar3->mbr_0xc = uVar1;
      if (pcVar3->mbr_0x4 < uVar1) {
        cls_0x588410::meth_0x5884a0(pcVar3);
      }
      piVar2 = (int *)(in_stack_00000008 + 0xb8);
      in_stack_00000008 = 0;
      if (0 < *piVar2) {
        do {
          iVar8 = *(int *)(*(int *)(iVar5 + 200) + in_stack_00000008 * 4);
          if (iVar8 != 0) {
            cVar6 = FUN_00585dd0(iVar8,_uStack0000000c);
            if (cVar6 != '\0') {
              pcVar3 = (cls_0x588410 *)this->mbr_0x60;
              uVar4 = *(undefined4 *)(iVar8 + 0x40);
              puVar9 = (undefined *)pcVar3->mbr_0xc;
              pcVar3->mbr_0xc = (dword)(puVar9 + 4);
              if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 4) {
                puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
              }
              in_stack_00000004._2_1_ = (undefined)((uint)uVar4 >> 0x10);
              in_stack_00000004._3_1_ = (undefined)((uint)uVar4 >> 0x18);
              *puVar9 = (char)uVar4;
              puVar9[1] = (char)((uint)uVar4 >> 8);
              puVar9[2] = in_stack_00000004._2_1_;
              puVar9[3] = in_stack_00000004._3_1_;
              uVar7 = FUN_00585ec0(iVar8);
              pcVar3 = (cls_0x588410 *)this->mbr_0x60;
              puVar9 = (undefined *)pcVar3->mbr_0xc;
              pcVar3->mbr_0xc = (dword)(puVar9 + 2);
              if ((undefined *)pcVar3->mbr_0x4 < puVar9 + 2) {
                puVar9 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar3);
              }
              *puVar9 = (char)uVar7;
              puVar9[1] = (char)((ushort)uVar7 >> 8);
            }
          }
          in_stack_00000008 = in_stack_00000008 + 1;
        } while (in_stack_00000008 < *(int *)(iVar5 + 0xb8));
      }
      cls_0x588410::meth_0x588a50((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005865a0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5865a0
          (cls_0x57d9d0 *this,undefined4 param_1,undefined1 param_2,undefined param_3,
          undefined4 param_4)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x19,1);
    if (iVar2 != 0) {
      cls_0x588410::meth_0x588e80((cls_0x588410 *)this->mbr_0x60);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      *puVar3 = param_3;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (undefined)param_4;
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586630

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586630(cls_0x57d9d0 *this,undefined4 param_1)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x1a,1);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586680

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586680
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  int in_stack_00000010;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x52,0);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_2;
      puVar3[1] = (char)((uint)param_2 >> 8);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_3;
      puVar3[1] = (char)((uint)param_3 >> 8);
      if (in_stack_00000010 == 0) {
        uVar4 = 0;
      }
      else {
        uVar4 = *(undefined4 *)(in_stack_00000010 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586750

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586750(cls_0x57d9d0 *this,undefined4 param_1)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined4 uVar3;
  int in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x54,0);
    if (iVar1 != 0) {
      if (in_stack_00000008 == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = *(undefined4 *)(in_stack_00000008 + 0x40);
      }
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      param_1._2_1_ = (undefined)((uint)uVar3 >> 0x10);
      *puVar2 = (char)uVar3;
      param_1._3_1_ = (undefined)((uint)uVar3 >> 0x18);
      puVar2[1] = (char)((uint)uVar3 >> 8);
      puVar2[2] = param_1._2_1_;
      puVar2[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005867d0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5867d0(cls_0x57d9d0 *this,undefined4 param_1,undefined param_2)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  undefined3 in_stack_00000009;
  undefined in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x55,0);
    if (iVar2 != 0) {
      if (_param_2 == 0) {
        uVar4 = 0;
      }
      else {
        uVar4 = *(undefined4 *)(_param_2 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3[3] = param_1._3_1_;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = in_stack_0000000c;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586870

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586870
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 *param_2,undefined param_3)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined3 in_stack_0000000d;
  undefined in_stack_00000010;
  
  if (DAT_00676828 != 0) {
    iVar3 = meth_0x57d9d0(this,(undefined)param_1,0x56,0);
    if (iVar3 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      uVar2 = *param_2;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      uVar2 = param_2[1];
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      uVar2 = param_2[2];
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = param_3;
      puVar4[1] = (char)in_stack_0000000d;
      puVar4[2] = in_stack_0000000d._1_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = in_stack_0000000d._2_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 1) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = in_stack_00000010;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005869a0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5869a0(cls_0x57d9d0 *this,undefined4 param_1,undefined1 param_2)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x5d,0);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 1);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 1) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = in_stack_0000000c;
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586a10

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586a10(cls_0x57d9d0 *this,undefined4 param_1)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x5c,0);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586a60

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586a60(cls_0x57d9d0 *this)

{
  cls_0x588410 *pcVar1;
  short sVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 uVar5;
  undefined in_stack_00000004;
  char cStack00000006;
  undefined uStack00000007;
  
  if ((DAT_00676828 != 0) &&
     (iVar3 = meth_0x57d9d0(this,in_stack_00000004,0x5e,0), uVar5 = DAT_0065a534, iVar3 != 0)) {
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    puVar4 = (undefined *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(puVar4 + 2);
    if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 2) {
      puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    *puVar4 = (char)uVar5;
    puVar4[1] = (char)((uint)uVar5 >> 8);
    if (DAT_0065a558 == 0) {
      uVar5 = 0;
    }
    else {
      uVar5 = *(undefined4 *)(DAT_0065a558 + 0x40);
    }
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    puVar4 = (undefined *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(puVar4 + 4);
    if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
      puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    cStack00000006 = (char)((uint)uVar5 >> 0x10);
    uStack00000007 = (undefined)((uint)uVar5 >> 0x18);
    *puVar4 = (char)uVar5;
    puVar4[1] = (char)((uint)uVar5 >> 8);
    puVar4[2] = cStack00000006;
    puVar4[3] = uStack00000007;
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    sVar2 = DAT_0065a54c;
    pcVar1 = (cls_0x588410 *)this->mbr_0x60;
    puVar4 = (undefined *)pcVar1->mbr_0xc;
    pcVar1->mbr_0xc = (dword)(puVar4 + 4);
    if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
      puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
    }
    cStack00000006 = (char)(sVar2 >> 0xf);
    *puVar4 = (char)sVar2;
    iVar3 = 0;
    puVar4[1] = (char)((ushort)sVar2 >> 8);
    puVar4[2] = cStack00000006;
    puVar4[3] = cStack00000006;
    if (0 < DAT_0065a54c) {
      do {
        cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
        iVar3 = iVar3 + 1;
      } while (iVar3 < DAT_0065a54c);
    }
    meth_0x57dc70(this);
    cls_0x532f40::meth_0x532f40((cls_0x532f40 *)&DAT_0065a3b8);
    return 1;
  }
  return 0;
}



// Function at 00586bb0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586bb0(cls_0x57d9d0 *this,undefined4 param_1,undefined4 *param_2)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar3 = meth_0x57d9d0(this,(undefined)param_1,0x53,0);
    if (iVar3 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      uVar2 = *param_2;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      uVar2 = param_2[1];
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      uVar2 = param_2[2];
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = (char)in_stack_0000000c;
      puVar4[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar4[2] = in_stack_0000000c._2_1_;
      puVar4[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586cc0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586cc0(cls_0x57d9d0 *this,int param_1,int param_2,int param_3)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined *puVar4;
  int in_stack_00000010;
  
  if ((DAT_00676828 != 0) && ((*(uint *)(param_1 + 0x36c) & 0x10000) != 0)) {
    if ((((param_3 == 3) || (((param_3 == 8 || (param_3 == 9)) || (param_3 == 6)))) ||
        (((param_3 == 7 || (param_3 == 2)) || (param_3 == 5)))) || (param_3 == 10)) {
      in_stack_00000010 = *(int *)(in_stack_00000010 + 0x40);
    }
    iVar3 = meth_0x57d9d0(this,(char)param_1,0x57,0);
    if (iVar3 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 1) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      *puVar4 = (char)param_3;
      uVar2 = *(undefined4 *)(param_2 + 0x40);
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = (char)in_stack_00000010;
      puVar4[1] = (char)((uint)in_stack_00000010 >> 8);
      puVar4[2] = in_stack_00000010._2_1_;
      puVar4[3] = in_stack_00000010._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586dd0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586dd0(cls_0x57d9d0 *this,undefined param_1)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined3 in_stack_00000005;
  undefined in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,0,0x58,3);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = param_1;
      puVar3[1] = (char)in_stack_00000005;
      puVar3[2] = in_stack_00000005._1_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3[3] = in_stack_00000005._2_1_;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = in_stack_00000008;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586e60

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586e60
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  
  if ((DAT_00676828 != 0) && (iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x59,0), iVar1 != 0)) {
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    this_00 = (cls_0x588410 *)this->mbr_0x60;
    puVar2 = (undefined *)this_00->mbr_0xc;
    this_00->mbr_0xc = (dword)(puVar2 + 2);
    if ((undefined *)this_00->mbr_0x4 < puVar2 + 2) {
      puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
    }
    *puVar2 = (char)param_3;
    puVar2[1] = (char)((uint)param_3 >> 8);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    meth_0x57dc70(this);
    return 1;
  }
  return 0;
}



// Function at 00586f00

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586f00
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  
  if ((DAT_00676828 != 0) && (iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x5a,0), iVar1 != 0)) {
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    this_00 = (cls_0x588410 *)this->mbr_0x60;
    puVar2 = (undefined *)this_00->mbr_0xc;
    this_00->mbr_0xc = (dword)(puVar2 + 2);
    if ((undefined *)this_00->mbr_0x4 < puVar2 + 2) {
      puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
    }
    *puVar2 = (char)param_3;
    puVar2[1] = (char)((uint)param_3 >> 8);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    meth_0x57dc70(this);
    return 1;
  }
  return 0;
}



// Function at 00586f80

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x586f80(cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x5b,0);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00586fd0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x586fd0(cls_0x57d9d0 *this,undefined4 param_1)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined4 in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x1b,0);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = (char)in_stack_00000008;
      puVar2[1] = (char)((uint)in_stack_00000008 >> 8);
      puVar2[2] = in_stack_00000008._2_1_;
      puVar2[3] = in_stack_00000008._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587040

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x587040
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 in_stack_00000010;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x5f,0);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_2;
      puVar3[1] = (char)((uint)param_2 >> 8);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_3;
      puVar3[1] = (char)((uint)param_3 >> 8);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)in_stack_00000010;
      puVar3[1] = (char)((uint)in_stack_00000010 >> 8);
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587310

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x587310(cls_0x57d9d0 *this)

{
  int iVar1;
  undefined in_stack_00000004;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,in_stack_00000004,0x3c,0);
    if (iVar1 != 0) {
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587350

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x587350
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  int in_stack_00000010;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x40,0);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_2;
      puVar3[1] = (char)((uint)param_2 >> 8);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_3;
      puVar3[1] = (char)((uint)param_3 >> 8);
      if (in_stack_00000010 == 0) {
        uVar4 = 0xffffffff;
      }
      else {
        uVar4 = *(undefined4 *)(in_stack_00000010 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587420

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x587420
          (cls_0x57d9d0 *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  int in_stack_00000010;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,(undefined)param_1,0x41,0);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_2;
      puVar3[1] = (char)((uint)param_2 >> 8);
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 2);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 2) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)param_3;
      puVar3[1] = (char)((uint)param_3 >> 8);
      if (in_stack_00000010 == 0) {
        uVar4 = 0xffffffff;
      }
      else {
        uVar4 = *(undefined4 *)(in_stack_00000010 + 0x40);
      }
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar4 >> 0x10);
      *puVar3 = (char)uVar4;
      param_1._3_1_ = (undefined)((uint)uVar4 >> 0x18);
      puVar3[1] = (char)((uint)uVar4 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005874f0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x5874f0(cls_0x57d9d0 *this,undefined param_1)

{
  undefined uVar1;
  cls_0x588410 *pcVar2;
  int iVar3;
  undefined *puVar4;
  undefined3 in_stack_00000005;
  undefined in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    iVar3 = meth_0x57d9d0(this,param_1,0x3d,1);
    if (iVar3 != 0) {
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      uVar1 = *(undefined *)(_param_1 + 0x36);
      pcVar2->mbr_0xc = (dword)(puVar4 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 1) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar4 = in_stack_00000008;
      pcVar2 = (cls_0x588410 *)this->mbr_0x60;
      puVar4 = (undefined *)pcVar2->mbr_0xc;
      pcVar2->mbr_0xc = (dword)(puVar4 + 1);
      if ((undefined *)pcVar2->mbr_0x4 < puVar4 + 1) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar2);
      }
      *puVar4 = uVar1;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587570

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x587570(cls_0x57d9d0 *this,undefined4 param_1,undefined1 param_2)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x44,1);
    if (iVar1 != 0) {
      cls_0x588410::meth_0x588e80((cls_0x588410 *)this->mbr_0x60);
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 1);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 1) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = in_stack_0000000c;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005875e0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5875e0(cls_0x57d9d0 *this,int param_1,int param_2)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_0000000c;
  
  if (((DAT_00676828 != 0) && (param_1 != 0)) && (param_2 != 0)) {
    iVar3 = meth_0x57d9d0(this,(char)param_1,0x47,0);
    if (iVar3 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      uVar2 = *(undefined4 *)(param_2 + 0x40);
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      *puVar4 = (char)uVar2;
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      uVar2 = *(undefined4 *)(*(int *)(param_2 + 0x4c) + 0x1c);
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = (char)in_stack_0000000c;
      puVar4[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar4[2] = in_stack_0000000c._2_1_;
      puVar4[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005876e0

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x5876e0(cls_0x57d9d0 *this,int param_1,int param_2)

{
  cls_0x588410 *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 in_stack_0000000c;
  
  if (((DAT_00676828 != 0) && (param_1 != 0)) && (param_2 != 0)) {
    iVar3 = meth_0x57d9d0(this,(char)param_1,0x48,0);
    if (iVar3 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      uVar2 = *(undefined4 *)(param_2 + 0x40);
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      param_1._2_1_ = (undefined)((uint)uVar2 >> 0x10);
      *puVar4 = (char)uVar2;
      param_1._3_1_ = (undefined)((uint)uVar2 >> 0x18);
      puVar4[1] = (char)((uint)uVar2 >> 8);
      puVar4[2] = param_1._2_1_;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar4[3] = param_1._3_1_;
      puVar4 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar4 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar4 + 4) {
        puVar4 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar4 = (char)in_stack_0000000c;
      puVar4[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar4[2] = in_stack_0000000c._2_1_;
      puVar4[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 005878a0

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x5878a0(cls_0x57d9d0 *this,undefined4 param_1)

{
  cls_0x588410 *this_00;
  int iVar1;
  undefined *puVar2;
  undefined4 in_stack_00000008;
  
  if (DAT_00676828 != 0) {
    iVar1 = meth_0x57d9d0(this,(undefined)param_1,0x33,0);
    if (iVar1 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      puVar2 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar2 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar2 + 4) {
        puVar2 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      *puVar2 = (char)in_stack_00000008;
      puVar2[1] = (char)((uint)in_stack_00000008 >> 8);
      puVar2[2] = in_stack_00000008._2_1_;
      puVar2[3] = in_stack_00000008._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587910

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x587910(cls_0x57d9d0 *this,int param_1)

{
  cls_0x588410 *this_00;
  undefined4 uVar1;
  int iVar2;
  undefined *puVar3;
  int in_stack_00000008;
  
  if (((DAT_00676828 != 0) && (param_1 != 0)) && (in_stack_00000008 != 0)) {
    iVar2 = meth_0x57d9d0(this,(char)param_1,0x4a,0);
    if (iVar2 != 0) {
      this_00 = (cls_0x588410 *)this->mbr_0x60;
      uVar1 = *(undefined4 *)(in_stack_00000008 + 0x40);
      puVar3 = (undefined *)this_00->mbr_0xc;
      this_00->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)this_00->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(this_00);
      }
      param_1._2_1_ = (undefined)((uint)uVar1 >> 0x10);
      *puVar3 = (char)uVar1;
      param_1._3_1_ = (undefined)((uint)uVar1 >> 0x18);
      puVar3[1] = (char)((uint)uVar1 >> 8);
      puVar3[2] = param_1._2_1_;
      puVar3[3] = param_1._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587990

undefined4 __thiscall
OOAnalyzer::cls_0x57d9d0::meth_0x587990(cls_0x57d9d0 *this,undefined param_1,undefined4 param_2)

{
  cls_0x588410 *pcVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 in_stack_0000000c;
  
  if (DAT_00676828 != 0) {
    iVar2 = meth_0x57d9d0(this,param_1,0x4b,0);
    if (iVar2 != 0) {
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 1);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 1) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (undefined)param_2;
      pcVar1 = (cls_0x588410 *)this->mbr_0x60;
      puVar3 = (undefined *)pcVar1->mbr_0xc;
      pcVar1->mbr_0xc = (dword)(puVar3 + 4);
      if ((undefined *)pcVar1->mbr_0x4 < puVar3 + 4) {
        puVar3 = (undefined *)cls_0x588410::meth_0x5884a0(pcVar1);
      }
      *puVar3 = (char)in_stack_0000000c;
      puVar3[1] = (char)((uint)in_stack_0000000c >> 8);
      puVar3[2] = in_stack_0000000c._2_1_;
      puVar3[3] = in_stack_0000000c._3_1_;
      meth_0x57dc70(this);
      return 1;
    }
  }
  return 0;
}



// Function at 00587a20

undefined4 __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x587a20(cls_0x57d9d0 *this)

{
  int iVar1;
  int in_stack_00000004;
  
  if ((((DAT_00676828 != 0) && (in_stack_00000004 != 0)) &&
      ((DAT_00676828 == 0 || ((DAT_0067682c != 0 || (in_stack_00000004 == DAT_00667fcc)))))) &&
     (iVar1 = meth_0x57d9d0(this,(char)in_stack_00000004,0x4d,0), iVar1 != 0)) {
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    cls_0x588410::meth_0x588660((cls_0x588410 *)this->mbr_0x60);
    meth_0x57dc70(this);
    return 1;
  }
  return 0;
}



// Function at 00588330

dword __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x588330(cls_0x57d9d0 *this)

{
  return this->mbr_0x78;
}



// Function at 00588340

void __thiscall OOAnalyzer::cls_0x57d9d0::meth_0x588340(cls_0x57d9d0 *this)

{
  byte in_stack_00000004;
  
  this->mbr_0x7c = in_stack_00000004;
  return;
}



