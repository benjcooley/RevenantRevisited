// FUN_0047e500 @ 0047e500 size=326

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0047e500(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = DAT_0066f788;
joined_r0x0047e50c:
  if (iVar2 != 0) {
switchD_0047e556_default:
    return;
  }
  do {
    FUN_00537110();
    uVar1 = FUN_0048f040(&DAT_0066f748,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
    FUN_00537170();
    switch(uVar1) {
    case 1:
      goto switchD_0047e556_caseD_1;
    case 2:
      FUN_005399f0();
      iVar2 = FUN_0048f040(&DAT_0066fb08,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
      FUN_00539ab0();
      goto LAB_0047e599;
    case 3:
      _DAT_0066fe3c = 1;
      FUN_0053a8b0();
      FUN_0048f040(&DAT_0066fcc0,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
      FUN_0053aa60();
      _DAT_0065cb40 = 1;
      break;
    case 4:
      *(undefined **)(param_1 + 0x18) = &DAT_0065d358;
      FUN_0048ea40();
      return;
    case 5:
      PostQuitMessage(0);
    default:
      goto switchD_0047e556_default;
    }
  } while( true );
switchD_0047e556_caseD_1:
  _DAT_0066fa68 = 1;
  FUN_00539380();
  iVar2 = FUN_0048f040(&DAT_0066f8d0,(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
  FUN_00539440();
LAB_0047e599:
  _DAT_0065cb40 = 1;
  goto joined_r0x0047e50c;
}


