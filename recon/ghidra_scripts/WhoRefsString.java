// Find the function containing each reference to a given string.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Data;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;

public class WhoRefsString extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    String needle = args[0];
    // Scan defined data for matching string
    for (Data d : currentProgram.getListing().getDefinedData(true)) {
      Object v = d.getValue();
      if (v instanceof String && ((String)v).contains(needle)) {
        println("string '" + v + "' @ " + d.getAddress());
        ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(d.getAddress());
        while (ri.hasNext()) {
          Reference r = ri.next();
          Address fromAddr = r.getFromAddress();
          Function fn = getFunctionContaining(fromAddr);
          println("  ref from " + fromAddr + "  fn=" + (fn==null?"<none>":fn.getName()+"@"+fn.getEntryPoint()));
        }
      }
    }
  }
}
