// REVSYNC: disputed=TTextBar reason="class contains 'Could not create character event' / 'Could not create kill event for c...' strings and CreateEventA calls — character-event/lifecycle concerns, not UI text rendering as TTextBar name implies" rejected=MEDIUM date=2026-04-17 note="likely TCharacter-related (event/kill signals) but not confirmed; prior TTextBar suffix stripped."
// Decompiled methods and structure for class: cls_0x5a4358_TConsolePane

/*
/OOAnalyzer/cls_0x5a4358_TConsolePane
pack(disabled)
Structure cls_0x5a4358_TConsolePane {
   0   cls_0x5a4358_TConsolePane::vftable_5a4358 *   4   vftptr_0x0   "pointer to cls_0x5a4358_TConsolePane::vftable_5a4358"
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
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
   136   dword   4   mbr_0x88   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   176   dword   4   mbr_0xb0   "Unsigned Double-Word (ddw, 4-bytes)"
   180   dword   4   mbr_0xb4   "Unsigned Double-Word (ddw, 4-bytes)"
   184   dword   4   mbr_0xb8   "Unsigned Double-Word (ddw, 4-bytes)"
   188   dword   4   mbr_0xbc   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 204 Alignment: 1

*/

// Function at 0043dbc0

undefined4 __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::meth_0x43dbc0(cls_0x5a4358_TConsolePane *this)

{
  char cVar1;
  uint uVar2;
  dword dVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  char *in_stack_00000004;
  
  uVar2 = 0xffffffff;
  pcVar7 = (char *)this->mbr_0x60;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  dVar3 = ~uVar2 - 1;
  uVar4 = 0xffffffff;
  pcVar7 = in_stack_00000004;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  iVar5 = ~uVar4 - 1;
  this->mbr_0x6c = dVar3;
  if (iVar5 != 0) {
    if (9999 < (int)(~uVar2 + iVar5)) {
      return 0;
    }
    iVar6 = this->mbr_0x60 + this->mbr_0x70;
    FUN_0058b790(iVar6 + 1 + iVar5,iVar6 + 1,dVar3 - this->mbr_0x70);
    FUN_0058b790(this->mbr_0x70 + this->mbr_0x60);
    dVar3 = this->mbr_0x70 + iVar5;
    this->mbr_0x70 = dVar3;
    *(undefined *)(dVar3 + this->mbr_0x60) = 0x7b;
    iVar6 = FUN_0058ade0();
    while (iVar6 != 0) {
      in_stack_00000004 = (char *)(iVar6 + 1);
      this->mbr_0x74 = 0;
      this->mbr_0x78 = this->mbr_0x78 + 1;
      iVar6 = FUN_0058ade0(in_stack_00000004,10);
    }
    uVar2 = 0xffffffff;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      cVar1 = *in_stack_00000004;
      in_stack_00000004 = in_stack_00000004 + 1;
    } while (cVar1 != '\0');
    this->mbr_0x74 = this->mbr_0x74 + (~uVar2 - 1);
    this->mbr_0x6c = this->mbr_0x6c + iVar5;
    meth_0x43dda0(this);
    (*this->vftptr_0x0->virt_meth_0x444fb0_40)(this);
  }
  return 1;
}



// Function at 0043dca0

undefined4 __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::meth_0x43dca0(cls_0x5a4358_TConsolePane *this,int param_1)

{
  char cVar1;
  char *pcVar2;
  dword dVar3;
  int iVar4;
  uint uVar5;
  dword dVar6;
  char *pcVar7;
  int in_stack_00000008;
  
  uVar5 = 0xffffffff;
  pcVar2 = (char *)this->mbr_0x60;
  pcVar7 = pcVar2;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  dVar6 = ~uVar5 - 1;
  this->mbr_0x6c = dVar6;
  if ((in_stack_00000008 == 0) && (iVar4 = this->mbr_0x74 - this->mbr_0x7c, iVar4 < param_1)) {
    param_1 = iVar4;
  }
  if (-1 < param_1) {
    if ((int)(dVar6 - param_1) < 1) {
      param_1 = ~uVar5 - 2;
    }
    if (in_stack_00000008 == 0) {
      FUN_0058b790((int)(pcVar2 + this->mbr_0x70) - param_1,pcVar2 + this->mbr_0x70,
                   (dVar6 - this->mbr_0x70) + 1);
      this->mbr_0x70 = this->mbr_0x70 - param_1;
      this->mbr_0x74 = this->mbr_0x74 - param_1;
      return 1;
    }
    dVar3 = this->mbr_0x70;
    FUN_0058b790(pcVar2 + dVar3 + 1,pcVar2 + param_1 + dVar3 + 1,((dVar6 - dVar3) - param_1) + 1);
    return 1;
  }
  return 0;
}



// Function at 0043dda0

void __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::meth_0x43dda0(cls_0x5a4358_TConsolePane *this)

{
  char *pcVar1;
  char cVar2;
  dword dVar3;
  char *pcVar4;
  int iVar5;
  dword dVar6;
  
  dVar6 = this->mbr_0x90;
  dVar3 = this->mbr_0x78;
  do {
    if ((int)dVar6 <= (int)dVar3) {
      if ((int)(this->mbr_0x94 + this->mbr_0x90) <= (int)dVar3) {
        do {
          iVar5 = 0;
          pcVar4 = (char *)(this->mbr_0x68 + this->mbr_0x60);
          cVar2 = *pcVar4;
          while ((cVar2 != '\0' && (cVar2 != '\n'))) {
            if (*pcVar4 != '{') {
              iVar5 = iVar5 + 1;
            }
            pcVar1 = pcVar4 + 1;
            pcVar4 = pcVar4 + 1;
            cVar2 = *pcVar1;
          }
          dVar6 = this->mbr_0x68 + iVar5;
          this->mbr_0x68 = dVar6;
          if (*(char *)(dVar6 + this->mbr_0x60) != '\0') {
            this->mbr_0x68 = dVar6 + 1;
          }
          dVar6 = this->mbr_0x90 + 1;
          this->mbr_0x90 = dVar6;
        } while ((int)(dVar6 + this->mbr_0x94) <= (int)dVar3);
      }
      return;
    }
    if (0 < (int)this->mbr_0x68) {
      do {
        dVar6 = this->mbr_0x68 - 1;
        this->mbr_0x68 = dVar6;
        if (*(char *)(this->mbr_0x60 + dVar6) == '\n') break;
      } while (0 < (int)dVar6);
      if (0 < (int)this->mbr_0x68) {
        do {
          if (*(char *)((this->mbr_0x60 - 1) + this->mbr_0x68) == '\n') break;
          dVar6 = this->mbr_0x68 - 1;
          this->mbr_0x68 = dVar6;
        } while (0 < (int)dVar6);
      }
    }
    dVar6 = this->mbr_0x90 - 1;
    this->mbr_0x90 = dVar6;
  } while( true );
}



// Function at 0043de60

undefined4 __thiscall
OOAnalyzer::cls_0x5a4358_TConsolePane::meth_0x43de60(cls_0x5a4358_TConsolePane *this,dword param_1,dword param_2)

{
  char cVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  char *pcVar5;
  dword dVar6;
  dword dVar7;
  dword dVar8;
  char *pcVar9;
  dword dVar10;
  int in_stack_0000000c;
  
  dVar8 = param_2;
  if (in_stack_0000000c == 0) {
    if ((int)param_1 < (int)this->mbr_0x7c) {
      param_1 = this->mbr_0x7c;
    }
    dVar8 = this->mbr_0x80;
    if (((int)dVar8 <= (int)param_2) &&
       (iVar2 = dVar8 + this->mbr_0x88, dVar8 = param_2, iVar2 <= (int)param_2)) {
      dVar8 = iVar2 - 1;
    }
  }
  if ((param_1 == this->mbr_0x74) && (dVar8 == this->mbr_0x78)) {
    return 0;
  }
  pcVar5 = (char *)(this->mbr_0x70 + this->mbr_0x60);
  uVar4 = 0xffffffff;
  pcVar9 = pcVar5;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  FUN_0058b790(pcVar5,pcVar5 + 1,~uVar4 - 1);
  dVar10 = 0;
  this->mbr_0x70 = 0;
  if (0 < (int)dVar8) {
    dVar7 = this->mbr_0x60;
    dVar6 = param_1;
    do {
      cVar1 = *(char *)(this->mbr_0x70 + dVar7);
      while (cVar1 != '\n') {
        if (*(char *)(dVar7 + this->mbr_0x70) == '\0') goto LAB_0043df42;
        dVar3 = this->mbr_0x70 + 1;
        dVar6 = dVar6 + 1;
        this->mbr_0x70 = dVar3;
        cVar1 = *(char *)(dVar3 + dVar7);
      }
      dVar6 = 0;
      dVar10 = dVar10 + 1;
      this->mbr_0x70 = this->mbr_0x70 + 1;
    } while ((int)dVar10 < (int)dVar8);
  }
  dVar7 = 0;
  dVar10 = dVar8;
  dVar6 = param_1;
  if (0 < (int)param_1) {
    do {
      cVar1 = *(char *)(this->mbr_0x60 + this->mbr_0x70);
      dVar6 = dVar7;
      if ((cVar1 == '\0') || (cVar1 == '\n')) break;
      dVar7 = dVar7 + 1;
      this->mbr_0x70 = this->mbr_0x70 + 1;
      dVar6 = param_1;
    } while ((int)dVar7 < (int)param_1);
  }
LAB_0043df42:
  param_1 = dVar6;
  uVar4 = 0xffffffff;
  pcVar9 = (char *)this->mbr_0x60 + this->mbr_0x70;
  pcVar5 = (char *)this->mbr_0x60;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  FUN_0058b790(pcVar9 + 1,pcVar9,~uVar4 - this->mbr_0x70);
  dVar8 = this->mbr_0x70;
  *(undefined *)(dVar8 + this->mbr_0x60) = 0x7b;
  if ((int)dVar8 < (int)this->mbr_0x68) {
    this->mbr_0x68 = this->mbr_0x68 + 1;
  }
  this->mbr_0x74 = param_1;
  this->mbr_0x78 = dVar10;
  meth_0x43dda0(this);
  return 1;
}



// Function at 0043e370

undefined4 __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::virt_meth_0x43e370_TConsolePane_Initialize(cls_0x5a4358_TConsolePane *this)

{
  char cVar1;
  HANDLE hThread;
  uint uVar2;
  char *pcVar3;
  cls_0x5a4358_TConsolePane *pcStack_4;
  
  pcStack_4 = this;
  cls_0x5a42d4_TTextPane::virt_meth_0x43da00_TTextPane_Initialize((cls_0x5a42d4_TTextPane *)this);
  uVar2 = 0xffffffff;
  pcVar3 = &DAT_005ceef8;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (cVar1 != '\0');
  this->mbr_0x7c = ~uVar2 - 1;
  this->mbr_0x88 = 1;
  this->mbr_0xa4 = 0;
  this->mbr_0xa8 = 0;
  this->mbr_0xa0 = 0;
  this->mbr_0xc4 = 0;
  this->mbr_0xc0 = 0;
  this->mbr_0x9c = this->mbr_0xc;
  DAT_00656b20 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
  if (DAT_00656b20 == (HANDLE)0x0) {
    FUN_00481c10(s_Could_not_create_kill_event_for_c_005ceefc,0);
  }
  DAT_00656b24 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,(LPCSTR)0x0);
  if (DAT_00656b24 == (HANDLE)0x0) {
    FUN_00481c10(s_Could_not_create_character_event_005cef30,0);
  }
  hThread = (HANDLE)FUN_0058bd47(0,0,&LAB_0043f5a0,this,1,&pcStack_4);
  this->mbr_0xc8 = (dword)hThread;
  SetThreadPriority(hThread,1);
  return 1;
}



// Function at 0043e510

void __thiscall
OOAnalyzer::cls_0x5a4358_TConsolePane::meth_0x43e510
          (cls_0x5a4358_TConsolePane *this,undefined4 param_1,int param_2,int param_3,undefined4 param_4,
          int param_5,int param_6)

{
  dword dVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 in_stack_0000001c;
  
  iVar5 = param_6;
  iVar3 = param_2;
  if (param_3 < param_2) {
    param_2 = param_3;
    param_3 = iVar3;
  }
  if (param_6 < param_5) {
    param_6 = param_5;
    param_5 = iVar5;
  }
  puVar2 = (undefined4 *)FUN_00482fb0(0x24);
  puVar2[1] = param_2;
  puVar2[5] = param_4;
  *puVar2 = param_1;
  puVar2[6] = in_stack_0000001c;
  dVar1 = this->mbr_0xa8;
  iVar5 = param_3 - param_2;
  iVar3 = param_6 - param_5;
  puVar2[2] = iVar5;
  puVar2[3] = param_5;
  puVar2[4] = iVar3;
  puVar2[8] = dVar1;
  puVar2[7] = 0;
  if (dVar1 != 0) {
    this->mbr_0xa8 = (dword)puVar2;
    *(undefined4 **)(dVar1 + 0x1c) = puVar2;
    return;
  }
  this->mbr_0xa4 = (dword)puVar2;
  this->mbr_0xa0 = (dword)puVar2;
  this->mbr_0xbc = 0;
  this->mbr_0xb4 = param_2 + 1;
  this->mbr_0xb8 = param_5 + 1;
  if (iVar5 == 0) {
LAB_0043e5fe:
    if (iVar3 == 0) goto LAB_0043e61f;
  }
  else {
    if (iVar3 == 0) {
LAB_0043e5e1:
      uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
      FUN_0043a020(uVar4);
      goto LAB_0043e5fe;
    }
    FUN_0043a020(DAT_0065a28c);
    if (iVar3 == 0) goto LAB_0043e5e1;
  }
  if (iVar5 == 0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_00656ce4);
    FUN_0043a020(uVar4);
  }
LAB_0043e61f:
  uVar4 = cls_0x5a5320_TPlayScreen::meth_0x48ed60((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
  cls_0x5a5320_TPlayScreen::meth_0x48eea0((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0,uVar4);
  this->mbr_0xa8 = (dword)puVar2;
  return;
}



// Function at 0043f350

undefined4 __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::virt_meth_0x43f350(cls_0x5a4358_TConsolePane *this)

{
  char *pcVar1;
  int iVar2;
  char *pcVar3;
  char cVar4;
  int iVar5;
  char *pcVar6;
  char *in_stack_00000004;
  char local_100 [256];
  
  pcVar3 = (char *)FUN_0058ade0();
  do {
    pcVar6 = pcVar3;
    if (pcVar6 == (char *)0x0) {
      meth_0x43dbc0(this);
      return 1;
    }
    cVar4 = *in_stack_00000004;
    iVar5 = 0;
    if (cVar4 != '\0') {
      iVar2 = -(int)in_stack_00000004;
      do {
        if ((cVar4 == '\n') || (0xfe < iVar5)) break;
        iVar5 = iVar5 + 1;
        in_stack_00000004[(int)(local_100 + iVar2)] = *in_stack_00000004;
        cVar4 = in_stack_00000004[1];
        in_stack_00000004 = in_stack_00000004 + 1;
      } while (cVar4 != '\0');
    }
    pcVar3 = (char *)(this->mbr_0x70 + this->mbr_0x60);
    local_100[iVar5] = '\0';
    iVar5 = 0;
    cVar4 = *pcVar3;
    while ((cVar4 != '\0' && (cVar4 != '\n'))) {
      if (*pcVar3 != '{') {
        iVar5 = iVar5 + 1;
      }
      pcVar1 = pcVar3 + 1;
      pcVar3 = pcVar3 + 1;
      cVar4 = *pcVar1;
    }
    meth_0x43de60(this,this->mbr_0x74 + iVar5,this->mbr_0x78);
    meth_0x43dbc0(this);
    meth_0x43dbc0(this);
    meth_0x43de60(this,this->mbr_0x7c,this->mbr_0x78 - 1);
    meth_0x43de60(this,0,this->mbr_0x78 + 1);
    cVar4 = *pcVar6;
    this->mbr_0x80 = this->mbr_0x80 + 1;
    if (cVar4 != '\0') {
      pcVar6 = pcVar6 + 1;
    }
    DAT_006581b8 = 0xd;
    SetEvent(DAT_00656b24);
    DAT_00656db4 = this->mbr_0x70 + this->mbr_0x60;
    pcVar3 = (char *)FUN_0058ade0(pcVar6,10);
    in_stack_00000004 = pcVar6;
  } while( true );
}



// Function at 0043f490

void __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::virt_meth_0x43f490(cls_0x5a4358_TConsolePane *this)

{
  int iVar1;
  
  for (iVar1 = FUN_0058ade0(); iVar1 != 0; iVar1 = FUN_0058ade0(iVar1 + 1,10)) {
    this->mbr_0x80 = this->mbr_0x80 + 1;
  }
  meth_0x43dbc0(this);
  DAT_00656db4 = this->mbr_0x70 + this->mbr_0x60;
  return;
}



// Function at 00445100

cls_0x5a4358_TConsolePane * __thiscall OOAnalyzer::cls_0x5a4358_TConsolePane::cls_0x5a4358_TConsolePane(cls_0x5a4358_TConsolePane *this)

{
  this->mbr_0x8 = 0x164;
  this->mbr_0x18 = 0x164;
  this->mbr_0x4 = 0;
  this->mbr_0xc = 0x168;
  this->mbr_0x1c = 0x168;
  this->mbr_0x14 = 0;
  this->mbr_0x10 = 0x7d;
  this->mbr_0x20 = 0x7d;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  this->mbr_0x60 = 0;
  this->vftptr_0x0 = &cls_0x5a4358__vftable_5a4358_005a4358;
  return this;
}



