import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;

public class RefsTo extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(a);
    while (ri.hasNext()) {
      Reference r = ri.next();
      Address from = r.getFromAddress();
      Function fn = getFunctionContaining(from);
      println(String.format("  ref from %s (%s) in %s", from, r.getReferenceType(),
        fn == null ? "<none>" : fn.getName()+"@"+fn.getEntryPoint()));
    }
  }
}
