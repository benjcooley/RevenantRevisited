// REVSYNC: TObjectInstance::~TObjectInstance() (real dtor, called by ~ shim 0x528b00)
// FUN_0046e420_TObjectInstance_RealDtor @ 0046e420 size=516
//
// Sets vptr to 0x5a50e8 (TObjectInstance vptr descriptor), then teardown:
//   - Removes self from various MapPane / script registries
//     (DAT_0065d674, DAT_0065b088, DAT_0065b2d8 globals = currently-tracked
//     instance pointers).
//   - If animator (this[0x16] = +0x58): calls vt[+0x1c] (Term) then vt[0]
//     (~animator with arg 1 = scalar deleting).
//   - FUN_00446ba0(this[0x15]) = +0x54 cleanup.
//   - Frees per-instance allocations: this[0x27], this[0x28] (script handles).
//   - Iterates inventory list (this[0x1e] count this[0x1a]) calling
//     ~child(arg=1) for each owned inventory item — recursive ownership free.
//   - Frees stats array (this[0xe]) and the inventory array (this[0x1e]).
//   - this[0x21] = chardata-or-similar attached buffer; freed via FUN_004830f0.
//
// Caller chain: vector-deleting-dtor at 0x528b00 (slot 0 of TObjectInstance
// vtable) calls into 0x46e420 then heap-frees if low bit of arg set.
