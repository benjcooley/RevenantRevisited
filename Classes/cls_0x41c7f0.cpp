// Decompiled methods and structure for class: cls_0x41c7f0

/*
/OOAnalyzer/cls_0x41c7f0
pack(disabled)
Structure cls_0x41c7f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 00411300

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x411300(cls_0x41c7f0 *this)

{
  int *piVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (piVar1 = *(int **)(this->mbr_0x10 + in_stack_00000004 * 4), piVar1 != (int *)0x0)) {
    (**(code **)(*piVar1 + 8))(1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 00411330

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x411330(cls_0x41c7f0 *this)

{
  int iVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (iVar1 = *(int *)(this->mbr_0x10 + in_stack_00000004 * 4), iVar1 != 0)) {
    if (*(int *)(iVar1 + 8) != 0) {
      FUN_00482f80(*(int *)(iVar1 + 8));
    }
    if (*(int *)(iVar1 + 0xc) != 0) {
      FUN_00482f80(*(int *)(iVar1 + 0xc));
    }
    FUN_004830f0(iVar1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 0041c7f0

cls_0x41c7f0 * __thiscall OOAnalyzer::cls_0x41c7f0::cls_0x41c7f0(cls_0x41c7f0 *this,dword param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  dword in_stack_00000008;
  
  this->mbr_0x8 = param_1;
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  this->mbr_0xc = in_stack_00000008;
  this->mbr_0x10 = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(param_1 << 2);
  this->mbr_0x10 = (dword)puVar1;
  for (uVar2 = this->mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  return this;
}



// Function at 00438f20

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x438f20(cls_0x41c7f0 *this)

{
  undefined4 *puVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (puVar1 = *(undefined4 **)(this->mbr_0x10 + in_stack_00000004 * 4), puVar1 != (undefined4 *)0x0
     )) {
    (**(code **)*puVar1)(1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 00461700

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x461700(cls_0x41c7f0 *this)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      if ((-1 < iVar2) && (iVar1 = *(int *)(this->mbr_0x10 + iVar2 * 4), iVar1 != 0)) {
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  return;
}



// Function at 004a1e90

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x4a1e90(cls_0x41c7f0 *this)

{
  int iVar1;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) &&
     (iVar1 = *(int *)(this->mbr_0x10 + in_stack_00000004 * 4), iVar1 != 0)) {
    FUN_004830f0(iVar1);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
  return;
}



// Function at 004acfe0

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x4acfe0(cls_0x41c7f0 *this)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      if ((-1 < iVar2) && (iVar1 = *(int *)(this->mbr_0x10 + iVar2 * 4), iVar1 != 0)) {
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  this->mbr_0x0 = 0;
  this->mbr_0x4 = 0;
  return;
}



// Function at 0053fe80

undefined4 __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x53fe80(cls_0x41c7f0 *this,undefined4 param_1)

{
  cls_0x5b8e94 *this_00;
  cls_0x53f680 *this_01;
  undefined uVar1;
  int iVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined4 uVar4;
  int unaff_EBX;
  int unaff_EBP;
  dword dVar5;
  cls_0x5b4f30 *this_02;
  int iVar6;
  undefined4 *puVar7;
  int iVar8;
  int *piVar9;
  cls_0x5b4f30 *unaff_retaddr;
  cls_0x5b4f30 *in_stack_00000008;
  undefined4 in_stack_0000000c;
  cls_0x45f7c0 *in_stack_00000010;
  char *pcVar10;
  cls_0x44ceb0 local_50;
  
  iVar6 = 0;
  iVar3 = DAT_00667c4c;
  local_50.mbr_0x4 = (dword)this;
  if (0 < DAT_00667c3c) {
    do {
      iVar8 = 0;
      if (0 < **(int **)(iVar3 + iVar6 * 4)) {
        do {
          iVar3 = *(int *)(iVar3 + iVar6 * 4);
          iVar2 = *(int *)(*(int *)(iVar3 + 0x10) + iVar8 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(iVar3 + 0x14);
          }
          iVar2 = FUN_0059a530(iVar2 + 0x24,param_1);
          iVar3 = DAT_00667c4c;
          if (iVar2 == 0) {
            dVar5 = *(dword *)(DAT_00667c4c + iVar6 * 4);
            goto LAB_0053fee8;
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < **(int **)(DAT_00667c4c + iVar6 * 4));
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < DAT_00667c3c);
  }
  dVar5 = 0;
LAB_0053fee8:
  iVar6 = 0;
  local_50.mbr_0x0 = dVar5;
  if (0 < DAT_00667c3c) {
    do {
      iVar8 = 0;
      if (0 < **(int **)(iVar3 + iVar6 * 4)) {
        do {
          iVar3 = *(int *)(iVar3 + iVar6 * 4);
          iVar2 = *(int *)(*(int *)(iVar3 + 0x10) + iVar8 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(iVar3 + 0x14);
          }
          iVar3 = FUN_0059a530(iVar2 + 0x24,param_1);
          if (iVar3 == 0) {
            iVar3 = cls_0x540ad0::meth_0x540ad0(*(cls_0x540ad0 **)(DAT_00667c4c + iVar6 * 4));
            goto LAB_0053ff4f;
          }
          iVar8 = iVar8 + 1;
          iVar3 = DAT_00667c4c;
        } while (iVar8 < **(int **)(DAT_00667c4c + iVar6 * 4));
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < DAT_00667c3c);
  }
  iVar3 = 0;
LAB_0053ff4f:
  if (dVar5 == 0) {
    return 0;
  }
  if (iVar3 == 0) {
    return 0;
  }
  if ((*(dword *)(local_50.mbr_0x4 + 0x14) != 0) && (DAT_00668154 == 0)) {
    return 0;
  }
  iVar6 = *(int *)(iVar3 + 0x4c);
  if (in_stack_00000008 != (cls_0x5b4f30 *)0x0) {
    if (*(short *)&(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.mbr_0x4 == 0xb) {
      uVar1 = (*(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0[1].virt_meth_0x477ea0_468)
                        (in_stack_00000008);
      iVar8 = (int)((ulonglong)((longlong)(CONCAT31(extraout_var,uVar1) * iVar6) * -0x51eb851f) >>
                   0x20);
      iVar6 = iVar6 + ((iVar8 >> 5) - (iVar8 >> 0x1f));
    }
    uVar1 = (*((in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->virt_meth_0x477e90_464)
                      (in_stack_00000008);
    if ((CONCAT31(extraout_var_00,uVar1) < iVar6) && (DAT_0066810c == 0)) {
      if (in_stack_00000008 != DAT_00667fcc) {
        return 0;
      }
      if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) goto LAB_005400fe;
      pcVar10 = s_SPLMANA_005e4f10;
      goto LAB_005400be;
    }
    if ((-1 < *(int *)(iVar3 + 0x5c)) && (DAT_0066810c == 0)) {
      iVar6 = FUN_00483300(1,100);
      if (*(short *)&(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.mbr_0x4 != 0xb)
      goto LAB_005401a0;
      uVar1 = (*((in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->FUN_00477aa0_220)(0x2a)
      ;
      iVar8 = CONCAT31(extraout_var_01,uVar1) - *(int *)(iVar3 + 0x5c);
      if (iVar8 < 0) {
        if (iVar8 == -3) {
          iVar8 = 0x14;
        }
        else if (iVar8 == -2) {
          iVar8 = 0x28;
        }
        else {
          iVar8 = (-(uint)(iVar8 != -1) & 0xffffffb0) + 0x50;
        }
      }
      else {
        iVar8 = 100;
      }
      if ((iVar8 < iVar6) && (in_stack_00000008 == DAT_00667fcc)) {
        if (iVar8 == 0) {
          if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) {
LAB_005400fe:
            uVar4 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
            FUN_0054d170(&DAT_0065c5d0,uVar4);
            return 0;
          }
          pcVar10 = s_SPLLVLNEG_005e4f20;
        }
        else {
          if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0))
          goto LAB_005400fe;
          pcVar10 = s_SPLLVLLOW_005e4f38;
        }
LAB_005400be:
        FUN_00587280(in_stack_00000008,pcVar10);
        return 0;
      }
    }
    if (*(short *)&(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.mbr_0x4 == 0xb) {
      (*((in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->FUN_00477aa0_220)(0x35);
      (*((in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0)->FUN_00477aa0_220)(0x2a);
      if (0 < *(int *)(iVar3 + 0x5c)) {
        (*(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.vftptr_0x0[2].virt_meth_0x477920_56)
                  (in_stack_00000008);
      }
    }
  }
LAB_005401a0:
  iVar3 = *(int *)(unaff_EBP + 0x70);
  if (iVar3 == 0) {
    iVar6 = 0;
    if (0 < DAT_0067021c) {
      piVar9 = &DAT_00670220;
      do {
        iVar3 = FUN_0059a530(s_spell_005e4f50,*(undefined4 *)(*piVar9 + 4));
        if (iVar3 == 0) goto LAB_0054033b;
        iVar6 = iVar6 + 1;
        piVar9 = piVar9 + 1;
      } while (iVar6 < DAT_0067021c);
    }
  }
  else {
    iVar6 = 0;
    if (0 < DAT_0067021c) {
      piVar9 = &DAT_00670220;
      do {
        iVar8 = FUN_0059a530(iVar3,*(undefined4 *)(*piVar9 + 4));
        if (iVar8 == 0) goto LAB_0054033b;
        iVar6 = iVar6 + 1;
        piVar9 = piVar9 + 1;
      } while (iVar6 < DAT_0067021c);
    }
  }
  puVar7 = (undefined4 *)0x0;
LAB_005401e5:
  this_02 = in_stack_00000008;
  if ((*(int *)(unaff_EBP + 0x74) != 0) &&
     ((*(byte *)&(in_stack_00000008->cls_0x5a7b98).cls_0x5a50e8.mbr_0x110 & 0x20) != 0)) {
    cls_0x44ceb0::cls_0x44ceb0(&local_50,in_stack_00000008,0,5,0);
    this_00 = (cls_0x5b8e94 *)local_50.mbr_0xc;
    while (this_00 != (cls_0x5b8e94 *)0x0) {
      local_50.mbr_0xc = (dword)this_00;
      if ((((this_00->mbr_0x4 == 0x19) && (this_00 != (cls_0x5b8e94 *)0x0)) &&
          (dVar5 = this_00->mbr_0xd8, dVar5 != 0)) &&
         ((iVar3 = *(int *)(dVar5 + 0x120), iVar3 != 0 && (*(int *)(iVar3 + 0x74) != 0)))) {
        if ((*(int *)(iVar3 + 0x70) != 0) &&
           ((*(int *)(iVar3 + 0x70) != -0x74 &&
            (iVar3 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548), iVar3 != 0)))) {
          cls_0x41c7d0::meth_0x49bd90((cls_0x41c7d0 *)&DAT_00667548);
        }
        if (*(cls_0x5b4f30 **)(dVar5 + 4) == unaff_retaddr) {
          cls_0x5b8e94::meth_0x4defe0(this_00);
        }
      }
      cls_0x44ceb0::meth_0x44d080(&local_50);
      this_00 = (cls_0x5b8e94 *)local_50.mbr_0xc;
      this_02 = unaff_retaddr;
    }
  }
  local_50.mbr_0x34 =
       (**(code **)*puVar7)(this_02,param_1,in_stack_00000008,in_stack_0000000c,unaff_EBX,unaff_EBP)
  ;
  iVar6 = 0;
  iVar3 = DAT_00667c4c;
  if (0 < DAT_00667c3c) {
    do {
      iVar8 = 0;
      if (0 < **(int **)(iVar3 + iVar6 * 4)) {
        do {
          iVar3 = *(int *)(iVar3 + iVar6 * 4);
          iVar2 = *(int *)(*(int *)(iVar3 + 0x10) + iVar8 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(iVar3 + 0x14);
          }
          iVar2 = FUN_0059a530(iVar2 + 0x24,local_50.mbr_0x30);
          iVar3 = DAT_00667c4c;
          if (iVar2 == 0) {
            uVar4 = *(undefined4 *)(DAT_00667c4c + iVar6 * 4);
            goto LAB_00540379;
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < **(int **)(DAT_00667c4c + iVar6 * 4));
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < DAT_00667c3c);
  }
  uVar4 = 0;
LAB_00540379:
  *(undefined4 *)(local_50.mbr_0x34 + 0x11c) = uVar4;
  iVar6 = 0;
  if (0 < DAT_00667c3c) {
    do {
      iVar8 = 0;
      if (0 < **(int **)(iVar3 + iVar6 * 4)) {
        do {
          iVar3 = *(int *)(iVar3 + iVar6 * 4);
          iVar2 = *(int *)(*(int *)(iVar3 + 0x10) + iVar8 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(iVar3 + 0x14);
          }
          iVar3 = FUN_0059a530(iVar2 + 0x24,local_50.mbr_0x30);
          if (iVar3 == 0) {
            iVar3 = *(int *)(DAT_00667c4c + iVar6 * 4);
            dVar5 = *(dword *)(*(int *)(iVar3 + 0x10) + iVar8 * 4);
            if (dVar5 == 0) {
              dVar5 = *(dword *)(iVar3 + 0x14);
            }
            goto LAB_005403e2;
          }
          iVar8 = iVar8 + 1;
          iVar3 = DAT_00667c4c;
        } while (iVar8 < **(int **)(DAT_00667c4c + iVar6 * 4));
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < DAT_00667c3c);
  }
  dVar5 = 0;
LAB_005403e2:
  this_01 = (cls_0x53f680 *)local_50.mbr_0x34;
  *(dword *)(local_50.mbr_0x34 + 0x120) = dVar5;
  if (this_02 != (cls_0x5b4f30 *)0x0) {
    cls_0x5b4f30::meth_0x4d5900(this_02);
  }
  (in_stack_00000010->cls_0x41c7f0).mbr_0x14 = *(dword *)(unaff_EBX + 0x50);
  cls_0x53f680::meth_0x53f680(this_01);
  cls_0x45f7c0::meth_0x41c840(in_stack_00000010);
  return 1;
LAB_0054033b:
  puVar7 = (undefined4 *)(&DAT_00670220)[iVar6];
  goto LAB_005401e5;
}



// Function at 00540750

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x540750(cls_0x41c7f0 *this)

{
  dword dVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  
  if (0 < (int)this->mbr_0x14) {
    this->mbr_0x14 = this->mbr_0x14 - 1;
  }
  dVar1 = this->mbr_0x0;
  uVar5 = 0;
  if (0 < (int)dVar1) {
    do {
      if (((this->mbr_0x10 != 0) && (uVar5 < this->mbr_0x0)) &&
         (piVar2 = *(int **)(this->mbr_0x10 + uVar5 * 4), piVar2 != (int *)0x0)) {
        (**(code **)(*piVar2 + 8))();
        iVar4 = (**(code **)(**(int **)(this->mbr_0x10 + uVar5 * 4) + 4))();
        if (iVar4 != 0) {
          (**(code **)(**(int **)(this->mbr_0x10 + uVar5 * 4) + 0xc))();
          if ((-1 < (int)uVar5) &&
             (puVar3 = *(undefined4 **)(this->mbr_0x10 + uVar5 * 4), puVar3 != (undefined4 *)0x0)) {
            (**(code **)*puVar3)(1);
          }
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
        }
      }
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < (int)dVar1);
  }
  return;
}



// Function at 005407d0

int __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x5407d0(cls_0x41c7f0 *this)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar1 = this->mbr_0x0;
  iVar2 = 0;
  uVar3 = 0;
  if (0 < (int)uVar1) {
    piVar4 = (int *)this->mbr_0x10;
    do {
      if (((int *)this->mbr_0x10 != (int *)0x0) && (uVar3 < uVar1)) {
        if (*piVar4 != 0) {
          iVar2 = iVar2 + *(int *)(*piVar4 + 0x134);
        }
      }
      uVar3 = uVar3 + 1;
      piVar4 = piVar4 + 1;
    } while ((int)uVar3 < (int)uVar1);
    return iVar2;
  }
  return 0;
}



// Function at 00540820

int __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x540820(cls_0x41c7f0 *this)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar1 = this->mbr_0x0;
  iVar2 = 0;
  uVar3 = 0;
  if (0 < (int)uVar1) {
    piVar4 = (int *)this->mbr_0x10;
    do {
      if (((int *)this->mbr_0x10 != (int *)0x0) && (uVar3 < uVar1)) {
        if (*piVar4 != 0) {
          iVar2 = iVar2 + *(int *)(*piVar4 + 0x138);
        }
      }
      uVar3 = uVar3 + 1;
      piVar4 = piVar4 + 1;
    } while ((int)uVar3 < (int)uVar1);
    return iVar2;
  }
  return 0;
}



// Function at 005408d0

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x5408d0(cls_0x41c7f0 *this,undefined4 param_1)

{
  dword dVar1;
  cls_0x53f730 *this_00;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  
  dVar1 = this->mbr_0x0;
  uVar4 = 0;
  if (0 < (int)dVar1) {
    do {
      if ((((this->mbr_0x10 != 0) && (uVar4 < this->mbr_0x0)) &&
          (this_00 = *(cls_0x53f730 **)(this->mbr_0x10 + uVar4 * 4), this_00 != (cls_0x53f730 *)0x0)
          ) && (this_00 != (cls_0x53f730 *)0x0)) {
        cls_0x53f730::meth_0x53f730(this_00,param_1);
        iVar3 = (**(code **)(this_00->mbr_0x0 + 4))();
        if (iVar3 != 0) {
          if ((-1 < (int)uVar4) &&
             (puVar2 = *(undefined4 **)(this->mbr_0x10 + uVar4 * 4), puVar2 != (undefined4 *)0x0)) {
            (**(code **)*puVar2)(1);
          }
          cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
        }
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)dVar1);
  }
  return;
}



// Function at 00540940

void __thiscall OOAnalyzer::cls_0x41c7f0::meth_0x540940(cls_0x41c7f0 *this)

{
  dword dVar1;
  int *piVar2;
  undefined4 *puVar3;
  uint uVar4;
  
  uVar4 = 0;
  dVar1 = this->mbr_0x0;
  if (0 < (int)dVar1) {
    do {
      if (((this->mbr_0x10 != 0) && (uVar4 < this->mbr_0x0)) &&
         (piVar2 = *(int **)(this->mbr_0x10 + uVar4 * 4), piVar2 != (int *)0x0)) {
        (**(code **)(*piVar2 + 0xc))();
        if ((-1 < (int)uVar4) &&
           (puVar3 = *(undefined4 **)(this->mbr_0x10 + uVar4 * 4), puVar3 != (undefined4 *)0x0)) {
          (**(code **)*puVar3)(1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this);
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)dVar1);
  }
  return;
}



