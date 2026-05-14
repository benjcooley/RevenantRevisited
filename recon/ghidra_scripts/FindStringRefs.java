// Find functions referencing the byte sequence of a string (memory search,
// works even if Ghidra hasn't promoted the bytes to a Data string).
// arg0 = needle (literal ASCII), arg1 (optional) = output file
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.io.PrintWriter;

public class FindStringRefs extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    if (args.length < 1) { println("usage: FindStringRefs <needle> [out]"); return; }
    String needle = args[0];
    PrintWriter w = args.length > 1 ? new PrintWriter(args[1]) : null;
    byte[] pat = (needle + "\0").getBytes("ASCII");
    Memory mem = currentProgram.getMemory();
    Address found = mem.findBytes(mem.getMinAddress(), pat, null, true, monitor);
    int hits = 0;
    while (found != null) {
      println("string @ " + found + " : '" + needle + "'");
      if (w != null) w.println("string @ " + found + " : '" + needle + "'");
      ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(found);
      while (ri.hasNext()) {
        Reference r = ri.next();
        Function fn = getFunctionContaining(r.getFromAddress());
        String line = "  ref from " + r.getFromAddress() + "  fn=" +
            (fn==null ? "<none>" : fn.getName()+"@"+fn.getEntryPoint());
        println(line);
        if (w != null) w.println(line);
      }
      hits++;
      Address next = found.add(1);
      if (next.compareTo(mem.getMaxAddress()) >= 0) break;
      found = mem.findBytes(next, pat, null, true, monitor);
    }
    println("total hits: " + hits);
    if (w != null) w.close();
  }
}
