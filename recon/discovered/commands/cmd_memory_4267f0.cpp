// FUN_004267f0 @ 004267f0 size=803

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004267f0(void)

{
  uint uVar1;
  _MEMORYSTATUS local_28;
  
  local_28.dwLength = 0x20;
  GlobalMemoryStatus(&local_28);
  FUN_0041ee50();
  FUN_0058b100(&DAT_00654a88,s_Percent_used___d___005cc48c,DAT_006672f4);
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Physical___4_1fM_Free___4_1fM_005cc4a4,
               (double)((float)DAT_006672f8 * _DAT_005a3aa8),
               (double)((float)DAT_006672fc * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Paged___4_1fM_Free___4_1fM_005cc4c8,
               (double)((float)DAT_00667300 * _DAT_005a3aa8),
               (double)((float)DAT_00667304 * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Virtual___4_1fM_Free___4_1fM_005cc4ec,
               (double)((float)DAT_00667308 * _DAT_005a3aa8),
               (double)((float)DAT_0066730c * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0041ee50(s_Current_memory_usage__005cc510);
  FUN_0058b100(&DAT_00654a88,s_Percent_used___d___005cc528,local_28.dwMemoryLoad);
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Physical___4_1fM_Free___4_1fM_005cc540,
               (double)((float)local_28.dwTotalPhys * _DAT_005a3aa8),
               (double)((float)local_28.dwAvailPhys * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Paged___4_1fM_Free___4_1fM_005cc564,
               (double)((float)local_28.dwTotalPageFile * _DAT_005a3aa8),
               (double)((float)local_28.dwAvailPageFile * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Virtual___4_1fM_Free___4_1fM_005cc588,
               (double)((float)local_28.dwTotalVirtual * _DAT_005a3aa8),
               (double)((float)local_28.dwAvailVirtual * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Memory_usaged_by_object_imagery__005cc5ac,
               (double)((float)DAT_006682cc * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  uVar1 = FUN_0041cdc0();
  FUN_0058b100(&DAT_00654a88,s_Memory_usaged_by_chunk_cache___4_005cc5d8,
               (double)((float)uVar1 * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  FUN_0058b100(&DAT_00654a88,s_Memory_allocated___4_1fM_Max___4_005cc600,
               (double)((float)DAT_0065ba08 * _DAT_005a3aa8),
               (double)((float)DAT_00667fbc * _DAT_005a3aa8));
  FUN_0041ee50(&DAT_00654a88);
  return 0;
}


