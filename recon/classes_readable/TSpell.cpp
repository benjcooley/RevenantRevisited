#include "TSpell.h"

// Decompiled methods and structure for class: TSpell



// Function at 0049e490

undefined4 TSpell::meth_0x49e490(int param_1)

{
  dword dVar1;
  undefined *puVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  cls_0x5a68d8 *this_00;
  int iVar5;
  int iVar6;
  dword in_stack_00000008;
  void *local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059dca3;
  local_c = ExceptionList;
  if (this->timer == 0) {
    dVar1 = (param_1 << 0x14) / 0x96048;
    ExceptionList = &local_c;
    this->effect = in_stack_00000008;
    this->source = 0x96048;
    this->master = dVar1;
    this->wait = dVar1 * 0x96048;
    this->targetnum = (int)dVar1 / (int)in_stack_00000008;
    puVar3 = FUN_00482ef0(dVar1 * 0x96048);
    this->timer = (dword)puVar3;
    VirtualLock(puVar3,this->wait);
    puVar3 = (undefined4 *)this->timer;
    puVar4 = FUN_00482fb0(this->master << 2);
    this->frame = (dword)puVar4;
    iVar6 = 0;
    if (0 < (int)this->master) {
      do {
        puVar4 = puVar3;
        for (iVar5 = 0x12; puVar2 = PTR_DAT_005d79e0, iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar4 = 0;
          puVar4 = puVar4 + 1;
        }
        *puVar3 = 0x280;
        puVar3[1] = 0x1e0;
        iVar5 = (**(code **)(*(int *)puVar2 + 0x18))();
        puVar3[0x11] = 0x96000;
        puVar3[4] = (-(uint)(iVar5 != 0xf) & 2) + 2;
        this_00 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
        uStack_4 = 0;
        if (this_00 == (cls_0x5a68d8 *)0x0) {
          this_00 = (cls_0x5a68d8 *)0x0;
        }
        else {
          cls_0x5a68d8::cls_0x5a68d8(this_00);
          uStack_4 = CONCAT31(uStack_4._1_3_,1);
          this_00->vftptr_0x0 =
               (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
          cls_0x5a68d8::meth_0x4a3a40(this_00);
        }
        puVar3 = (undefined4 *)((int)puVar3 + this->source);
        uStack_4 = 0xffffffff;
        *(cls_0x5a68d8 **)(this->frame + iVar6 * 4) = this_00;
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)this->master);
    }
    this->spell = 0;
    this->variant = 0;
    this->magic_defense = 0;
    this->magic_offense = (int)(this->effect << 0x10) / 0x18;
    this->targets = 0;
  }
  ExceptionList = local_c;
  return 1;
}



