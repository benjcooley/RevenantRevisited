// FUN_00546de0 @ 00546de0 size=349

void __thiscall
FUN_00546de0(int param_1,int param_2,int param_3,LPCSTR param_4,undefined1 *param_5,LONG *param_6,
            LONG *param_7)

{
  int y;
  char cVar1;
  uint uVar2;
  int c;
  LPCSTR pCVar3;
  tagPOINT local_8;
  
  uVar2 = 0xffffffff;
  pCVar3 = param_4;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pCVar3;
    pCVar3 = pCVar3 + 1;
  } while (cVar1 != '\0');
  c = ~uVar2 - 1;
  SetTextColor(*(HDC *)(param_1 + 0x17c),0);
  SetBkMode(*(HDC *)(param_1 + 0x17c),1);
  y = param_3 + 1;
  MoveToEx(*(HDC *)(param_1 + 0x17c),param_2 + 1,y,(LPPOINT)0x0);
  TextOutA(*(HDC *)(param_1 + 0x17c),param_2 + 1,y,param_4,c);
  MoveToEx(*(HDC *)(param_1 + 0x17c),param_2 + 2,y,(LPPOINT)0x0);
  TextOutA(*(HDC *)(param_1 + 0x17c),param_2 + 2,y,param_4,c);
  MoveToEx(*(HDC *)(param_1 + 0x17c),param_2 + 1,param_3 + 2,(LPPOINT)0x0);
  TextOutA(*(HDC *)(param_1 + 0x17c),param_2 + 1,param_3 + 2,param_4,c);
  SetTextColor(*(HDC *)(param_1 + 0x17c),(uint)CONCAT21(CONCAT11(*param_5,param_5[1]),param_5[2]));
  MoveToEx(*(HDC *)(param_1 + 0x17c),param_2,param_3,(LPPOINT)0x0);
  TextOutA(*(HDC *)(param_1 + 0x17c),param_2,param_3,param_4,c);
  GetCurrentPositionEx(*(HDC *)(param_1 + 0x17c),&local_8);
  if (param_6 != (LONG *)0x0) {
    *param_6 = local_8.x;
  }
  if (param_7 != (LONG *)0x0) {
    *param_7 = local_8.y;
  }
  return;
}


