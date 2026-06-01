// FUN_00546f40 @ 00546f40 size=405

void __thiscall
FUN_00546f40(int param_1,int param_2,int param_3,int param_4,int param_5,LPCSTR param_6,
            undefined1 *param_7,undefined4 param_8,int *param_9)

{
  int x;
  char cVar1;
  uint uVar2;
  int iVar3;
  int y;
  LPCSTR pCVar4;
  tagRECT local_10;
  
  uVar2 = 0xffffffff;
  pCVar4 = param_6;
  do {
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    cVar1 = *pCVar4;
    pCVar4 = pCVar4 + 1;
  } while (cVar1 != '\0');
  iVar3 = ~uVar2 - 1;
  SetTextAlign(*(HDC *)(param_1 + 0x17c),0);
  SetTextColor(*(HDC *)(param_1 + 0x17c),0);
  SetBkMode(*(HDC *)(param_1 + 0x17c),1);
  x = param_2 + 1;
  y = param_3 + 1;
  local_10.right = x + param_4;
  local_10.bottom = y + param_5;
  local_10.left = x;
  local_10.top = y;
  DrawTextA(*(HDC *)(param_1 + 0x17c),param_6,iVar3,&local_10,0x2810);
  local_10.left = param_2 + 2;
  local_10.right = local_10.left + param_4;
  local_10.bottom = y + param_5;
  local_10.top = y;
  DrawTextA(*(HDC *)(param_1 + 0x17c),param_6,iVar3,&local_10,0x2810);
  local_10.right = x + param_4;
  local_10.top = param_3 + 2;
  local_10.bottom = local_10.top + param_5;
  local_10.left = x;
  DrawTextA(*(HDC *)(param_1 + 0x17c),param_6,iVar3,&local_10,0x2810);
  SetTextColor(*(HDC *)(param_1 + 0x17c),(uint)CONCAT21(CONCAT11(*param_7,param_7[1]),param_7[2]));
  local_10.top = param_3;
  local_10.bottom = param_3 + param_5;
  local_10.left = param_2;
  local_10.right = param_2 + param_4;
  iVar3 = DrawTextA(*(HDC *)(param_1 + 0x17c),param_6,iVar3,&local_10,0x2810);
  if (param_9 != (int *)0x0) {
    *param_9 = *param_9 + iVar3;
  }
  MoveToEx(*(HDC *)(param_1 + 0x17c),x,y,(LPPOINT)0x0);
  return;
}


