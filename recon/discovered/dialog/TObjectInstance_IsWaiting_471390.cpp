// FUN_00471390 @ 00471390 size=29

bool __fastcall FUN_00471390(int param_1)

{
  if (*(int *)(param_1 + 0x84) != 0) {
    return *(char *)(*(int *)(param_1 + 0x84) + 0xb4) != '\0';
  }
  return false;
}


