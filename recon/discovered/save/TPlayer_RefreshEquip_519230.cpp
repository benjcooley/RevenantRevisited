// FUN_00519230 @ 00519230 size=193

/* WARNING: Removing unreachable block (ram,0x00519267) */
/* WARNING: Removing unreachable block (ram,0x00519268) */
/* WARNING: Removing unreachable block (ram,0x0051926e) */
/* WARNING: Removing unreachable block (ram,0x0051927d) */
/* WARNING: Removing unreachable block (ram,0x00519295) */
/* WARNING: Removing unreachable block (ram,0x00519292) */
/* WARNING: Removing unreachable block (ram,0x00519296) */
/* WARNING: Removing unreachable block (ram,0x0051929d) */
/* WARNING: Removing unreachable block (ram,0x005192ae) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00519230(int param_1)

{
  FUN_0046dfb0();
  if ((param_1 == DAT_00667fcc) && (DAT_00667fd0 == &DAT_0065caf0)) {
    _DAT_0065b830 = 1;
    _DAT_0065d548 = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
    DAT_0065b688 = 1;
  }
  return;
}


