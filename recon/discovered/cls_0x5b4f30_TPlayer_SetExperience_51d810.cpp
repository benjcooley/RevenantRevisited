// FUN_0051d810_SetExperience @ 0051d810 size=46

void __thiscall FUN_0051d810_SetExperience(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
  *(int *)(param_1 + 0x370) = param_2;
  uVar2 = __allmul(param_2,param_2 >> 0x1f,0x18,0);
  uVar1 = __alldiv(uVar2,100,0);
  *(undefined4 *)(param_1 + 0x374) = uVar1;
  return;
}


