// List all callers of the function at the given address.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.RefType;

public class CallersOf extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    Function target = getFunctionAt(a);
    if (target == null) target = getFunctionContaining(a);
    println("target: " + (target==null?"<none>":target.getName()+"@"+target.getEntryPoint()));
    if (target == null) return;
    ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(target.getEntryPoint());
    while (ri.hasNext()) {
      Reference r = ri.next();
      if (r.getReferenceType().isCall()) {
        Address from = r.getFromAddress();
        Function caller = getFunctionContaining(from);
        println(String.format("  call from %s in %s", from, caller==null?"<none>":caller.getName()+"@"+caller.getEntryPoint()));
      }
    }
  }
}
