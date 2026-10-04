// FUN_00426d60 @ 00426d60 size=186

/* WARNING: Removing unreachable block (ram,0x00426ddc) */
/* WARNING: Removing unreachable block (ram,0x00426de0) */
/* WARNING: Removing unreachable block (ram,0x00426df2) */
/* WARNING: Removing unreachable block (ram,0x00426dfc) */

undefined4 FUN_00426d60(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_50 [40];
  undefined1 auStack_28 [40];
  
  iVar1 = FUN_0047a410(param_2,s__t__t_005cc6ac,auStack_50,auStack_28);
  if (iVar1 != 0) {
    FUN_00497800(auStack_50,param_1);
    FUN_0046dfb0();
    return 0;
  }
  return 4;
}


